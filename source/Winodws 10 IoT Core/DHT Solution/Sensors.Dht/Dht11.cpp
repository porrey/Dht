#include "pch.h"
#include "Dht11.h"
#include <ppltasks.h>

using namespace concurrency;
using namespace std;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::System::Threading;
using namespace Windows::Devices::Gpio;
using namespace Sensors::Dht;

_Use_decl_annotations_

Dht11::Dht11(Windows::Devices::Gpio::GpioPin^ pin, Windows::Devices::Gpio::GpioPinDriveMode inputReadMode)
{
	// ***
	// *** Set Drive Mode to Input
	// ***
	this->_inputReadMode = inputReadMode;
	pin->SetDriveMode(this->_inputReadMode);

	// ***
	// *** Save the Pin
	// ***
	this->_pin = pin;
}

Dht11::~Dht11()
{
	this->_pin = nullptr;
}

Windows::Foundation::IAsyncOperation<DhtReading>^ Dht11::GetReadingAsync()
{
	return this->GetReadingAsync(DEFAULT_MAX_RETRIES);
}

Windows::Foundation::IAsyncOperation<DhtReading>^ Dht11::GetReadingAsync(int maxRetries)
{
	return create_async([this, maxRetries]
	{
		DhtReading returnValue;
		int i = 0;

		for (i; i < maxRetries; i++)
		{
			returnValue = this->InternalGetReading();

			if (returnValue.IsValid)
			{
				break;
			}
		}

		returnValue.RetryCount = i;

		return returnValue;
	});
}

DhtReading Dht11::InternalGetReading()
{
	DhtReading returnValue;

	// ***
	// *** Create a buffer for the 40-bit reading
	// ***
	std::bitset<40> bits;

	// ***
	// *** Query the performance counter frequency
	// *** to calculate the correct timing.
	// ***
	LARGE_INTEGER qpf;
	QueryPerformanceFrequency(&qpf);

	// ***
	// *** This is the threshold used to determine whether a bit is a '0' or a '1'.
	// *** A '0' has a pulse time of 76 microseconds, while a '1' has a
	// *** pulse time of 120 microseconds. 110 is chosen as a reasonable threshold.
	// *** We convert the value to QPF units for later use.
	// ***
	const unsigned int oneThreshold = static_cast<unsigned int>(110LL * qpf.QuadPart / 1000000LL);

	// ***
	// *** Latch low value onto pin
	// ***
	this->_pin->Write(GpioPinValue::Low);

	// ***
	// *** Set pin as output
	// ***
	this->_pin->SetDriveMode(GpioPinDriveMode::Output);

	// ***
	// *** Wait for at least 18 ms
	// ***
	Sleep(SAMPLE_HOLD_LOW_MILLIS);

	// ***
	// *** Set pin back to input
	// ***
	this->_pin->SetDriveMode(this->_inputReadMode);

	// ***
	// *** Read the current value
	// ***
	GpioPinValue previousValue = this->_pin->Read();

	// ***
	// *** catch the first rising edge
	// ***
	const ULONG initialRisingEdgeTimeoutMillis = 1;
	ULONGLONG endTickCount = GetTickCount64() + initialRisingEdgeTimeoutMillis;
	for (;;)
	{
		if (GetTickCount64() > endTickCount)
		{
			returnValue.TimedOut = true;
			return returnValue;
		}

		GpioPinValue value = this->_pin->Read();
		if (value != previousValue)
		{
			// ***
			// *** Rising edge?
			// ***
			if (value == GpioPinValue::High)
			{
				break;
			}

			previousValue = value;
		}
	}

	LARGE_INTEGER prevTime = { 0 };

	const ULONG sampleTimeoutMillis = 10;
	endTickCount = GetTickCount64() + sampleTimeoutMillis;

	// ***
	// *** Capture every falling edge until all bits are received or
	// *** timeout occurs
	// ***
	for (unsigned int i = 0; i < (bits.size() + 1);)
	{
		if (GetTickCount64() > endTickCount)
		{
			returnValue.TimedOut = true;
			return returnValue;
		}

		GpioPinValue value = this->_pin->Read();
		if ((previousValue == GpioPinValue::High) && (value == GpioPinValue::Low))
		{
			// ***
			// *** A falling edge was detected
			// ***
			LARGE_INTEGER now;
			QueryPerformanceCounter(&now);

			if (i != 0)
			{
				unsigned int difference = static_cast<unsigned int>(now.QuadPart - prevTime.QuadPart);
				bits[bits.size() - i] = difference > oneThreshold;
			}

			prevTime = now;
			++i;
		}

		previousValue = value;
	}

	returnValue = this->CalculateValues(bits);

	return returnValue;
}

DhtReading Dht11::CalculateValues(std::bitset<40> bits)
{
	DhtReading returnValue;

	unsigned long long value = bits.to_ullong();

	unsigned int checksum =
		((value >> 32) & 0xff) +
		((value >> 24) & 0xff) +
		((value >> 16) & 0xff) +
		((value >> 8) & 0xff);

	returnValue.IsValid = (checksum & 0xff) == (value & 0xff);

	if (returnValue.IsValid)
	{
		unsigned long long value1 = bits.to_ullong();
		returnValue.Humidity = ((value1 >> 32) & 0xff) + ((value1 >> 24) & 0xff) / 10.0;

		unsigned long long value2 = bits.to_ullong();
		returnValue.Temperature = ((value2 >> 16) & 0xff) + ((value2 >> 8) & 0xff) / 10.0;
	}
	else
	{
		returnValue.Humidity = 0;
		returnValue.Temperature = 0;
	}

	return returnValue;
}

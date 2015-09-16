#pragma once

#define SAMPLE_HOLD_LOW_MILLIS 18
#define DEFAULT_MAX_RETRIES 20

namespace Sensors
{
	namespace Dht
	{
		public value struct DhtReading
		{
			bool TimedOut;
			bool IsValid;
			double Temperature;
			double Humidity;
			int RetryCount;
		};

		public interface class IDht
		{
			Windows::Foundation::IAsyncOperation<DhtReading>^ GetReading();
			Windows::Foundation::IAsyncOperation<DhtReading>^ GetReading(int maxRetries);
		};

		public ref class Dht11 sealed : IDht
		{
			public:
				Dht11(Windows::Devices::Gpio::GpioPin^ pin, Windows::Devices::Gpio::GpioPinDriveMode inputReadMode);
				virtual ~Dht11();

				virtual Windows::Foundation::IAsyncOperation<DhtReading>^ GetReading();
				virtual Windows::Foundation::IAsyncOperation<DhtReading>^ GetReading(int maxRetries);

			private:
				Windows::Devices::Gpio::GpioPinDriveMode _inputReadMode;
				Windows::Devices::Gpio::GpioPin^ _pin;

				DhtReading InternalGetReading();
				DhtReading Dht11::CalculateValues(std::bitset<40> bits);
		};
	}
}
#pragma once

namespace Sensors
{
	namespace Dht
	{
		public ref class Dht11 sealed
		{
			enum
			{
				SAMPLE_HOLD_LOW_MILLIS = 18
			};

			public:

			Dht11() :
				_pin(nullptr)
			{ }

			void Init(Windows::Devices::Gpio::GpioPin^ Pin);

			void GetReading();

			void Reset();

			property bool IsValid
			{
				bool get()
				{
					return _isValid;
				}
			}

			property double Humidity
			{
				double get()
				{
					return _humidity;
				}
			}

			property double Temperature
			{
				double get()
				{
					return _temperature;
				}
			}

			property bool TimedOut
			{
				bool get()
				{
					return _timedOut;
				}
			}

			private:
			bool _timedOut = false;
			bool _isValid = false;
			double _temperature = 0;
			double _humidity = 0;

			Windows::Devices::Gpio::GpioPin^ _pin;

			void CalculateValues(std::bitset<40> bits);
		};
	}
}
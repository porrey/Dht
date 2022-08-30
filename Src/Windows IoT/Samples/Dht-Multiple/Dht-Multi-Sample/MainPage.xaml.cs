// Copyright © 2015-2022 Daniel Porrey
//
// This file is part of the Dht-Multi-Sample Solution.
// 
// Dht-Multi-Sample Solution is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Dht-Multi-Sample Solution is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Dht-Multi-Sample Solution. If not, see http://www.gnu.org/licenses/.
//
using System;
using System.Threading.Tasks;
using Dht.Multi.Sample.Common;
using Sensors.Dht;
using Windows.Devices.Gpio;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Navigation;

namespace Dht.Multi.Sample
{
	public partial class MainPage : BindablePage
	{
		private DispatcherTimer _timer = new DispatcherTimer();

		// ***
		// *** DHT Sensor 1
		// ***
		private GpioPin _pin1 = null;
		private IDht _dht1 = null;

		// ***
		// *** DHT Sensor 2
		// ***
		private GpioPin _pin2 = null;
		private IDht _dht2 = null;

		// ***
		// *** DHT Sensor 3
		// ***
		private GpioPin _pin3 = null;
		private IDht _dht3 = null;

		public MainPage()
		{
			this.InitializeComponent();

			// ***
			// *** Initialize the timer
			// ***
			_timer.Interval = TimeSpan.FromSeconds(1);
			_timer.Tick += this.Timer_Tick;
		}

		protected override void OnNavigatedTo(NavigationEventArgs e)
		{
			base.OnNavigatedTo(e);

			// ***
			// *** Get a reference to the GPIO Controller.
			// ***
			GpioController controller = GpioController.GetDefault();

			// ***
			// *** Make sure the reference is valid (that e are connected to a device with
			// *** a GPIO Controller.
			// ***
			if (controller != null)
			{
				// ***
				// *** Initialize the pin and sensor for DHT 1.
				// ***
				_pin1 = GpioController.GetDefault().OpenPin(5, GpioSharingMode.Exclusive);
				_dht1 = new Dht22(_pin1, GpioPinDriveMode.Input);

				// ***
				// *** Initialize the pin and sensor for DHT 2.
				// ***
				_pin2 = GpioController.GetDefault().OpenPin(6, GpioSharingMode.Exclusive);
				_dht2 = new Dht22(_pin2, GpioPinDriveMode.Input);

				// ***
				// *** Initialize the pin and sensor for DHT 3.
				// ***
				_pin3 = GpioController.GetDefault().OpenPin(17, GpioSharingMode.Exclusive);
				_dht3 = new Dht11(_pin3, GpioPinDriveMode.Input);
			}

			// ***
			// *** Start the timer
			// ***
			_timer.Start();
		}

		private async void Timer_Tick(object sender, object e)
		{
			// ***
			// *** Read each sensor.
			// ***
			(this.Temperature1, this.Humidity1) = await this.ReadTemperature(_dht1);
			(this.Temperature2, this.Humidity2) = await this.ReadTemperature(_dht2);
			(this.Temperature3, this.Humidity3) = await this.ReadTemperature(_dht3);
		}

		private double _temperature1 = 0;
		public double Temperature1
		{
			get
			{
				return _temperature1;
			}
			set
			{
				// ***
				// *** A value of -1 is invalid. Leave the previous value
				// *** on the screen when this happens.
				// ***
				if (value != -1)
				{
					this.SetProperty(ref _temperature1, value);
				}
			}
		}

		private double _temperature2 = 0;
		public double Temperature2
		{
			get
			{
				return _temperature2;
			}
			set
			{
				// ***
				// *** A value of -1 is invalid. Leave the previous value
				// *** on the screen when this happens.
				// ***
				if (value != -1)
				{
					this.SetProperty(ref _temperature2, value);
				}
			}
		}

		private double _temperature3 = 0;
		public double Temperature3
		{
			get
			{
				return _temperature3;
			}
			set
			{
				// ***
				// *** A value of -1 is invalid. Leave the previous value
				// *** on the screen when this happens.
				// ***
				if (value != -1)
				{
					this.SetProperty(ref _temperature3, value);
				}
			}
		}

		private double _humidity1 = 0;
		public double Humidity1
		{
			get
			{
				return _humidity1;
			}
			set
			{
				// ***
				// *** A value of -1 is invalid. Leave the previous value
				// *** on the screen when this happens.
				// ***
				if (value != -1)
				{
					this.SetProperty(ref _humidity1, value);
				}
			}
		}

		private double _humidity2 = 0;
		public double Humidity2
		{
			get
			{
				return _humidity2;
			}
			set
			{
				// ***
				// *** A value of -1 is invalid. Leave the previous value
				// *** on the screen when this happens.
				// ***
				if (value != -1)
				{
					this.SetProperty(ref _humidity2, value);
				}
			}
		}

		private double _humidity3 = 0;
		public double Humidity3
		{
			get
			{
				return _humidity3;
			}
			set
			{
				// ***
				// *** A value of -1 is invalid. Leave the previous value
				// *** on the screen when this happens.
				// ***
				if (value != -1)
				{
					this.SetProperty(ref _humidity3, value);
				}
			}
		}

		protected async Task<(double Temperature, double Humidity)> ReadTemperature(IDht device)
		{
			(double Temperature, double Humidity) returnValue = (0, 0);

			DhtReading reading = new DhtReading();
			reading = await device.GetReadingAsync().AsTask();

			if (reading.IsValid)
			{
				// ***
				// *** Get the values from the reading.
				// ***
				returnValue.Temperature = reading.Temperature;
				returnValue.Humidity = reading.Humidity;
			}
			else
			{
				// ***
				// *** Show -1 so that it is evident there
				// *** is an error reading the device.
				// ***
				returnValue.Temperature = -1;
				returnValue.Humidity = -1;
			}

			return returnValue;
		}
	}
}

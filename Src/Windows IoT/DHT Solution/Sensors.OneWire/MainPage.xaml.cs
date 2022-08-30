// Copyright © 2015-2022 Daniel Porrey
//
// This file is part of the Sensors.OneWire Solution.
// 
// Sensors.OneWire Solution is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Sensors.OneWire Solution is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Sensors.OneWire Solution. If not, see http://www.gnu.org/licenses/.
//
using System;
using System.Collections.Generic;
using System.Linq;
using Sensors.Dht;
using Sensors.OneWire.Common;
using Windows.Devices.Gpio;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Navigation;

namespace Sensors.OneWire
{
	public sealed partial class MainPage : BindablePage
    {
        private DispatcherTimer _timer = new DispatcherTimer();

        GpioPin _pin = null;
        private IDht _dht = null;
        private List<int> _retryCount = new List<int>();
        private DateTimeOffset _startedAt = DateTimeOffset.MinValue;

        public MainPage()
        {
            this.InitializeComponent();

            _timer.Interval = TimeSpan.FromSeconds(1);
            _timer.Tick += this.Timer_Tick;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);

			GpioController controller = GpioController.GetDefault();

			if (controller != null)
			{
				_pin = GpioController.GetDefault().OpenPin(17, GpioSharingMode.Exclusive);
				_dht = new Dht11(_pin, GpioPinDriveMode.Input);
				_timer.Start();
				_startedAt = DateTimeOffset.Now;

				// ***
				// *** Uncomment to simulate heavy CPU usage
				// ***
				//CpuKiller.StartEmulation();
			}
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            _timer.Stop();

			// ***
			// *** Dispose the pin.
			// ***
			if (_pin != null)
			{
				_pin.Dispose();
				_pin = null;
			}

			// ***
			// *** Set the Dht object reference to null.
			// ***
			_dht = null;

			// ***
			// *** Stop the high CPU usage simulation.
			// ***
			CpuKiller.StopEmulation();

			base.OnNavigatedFrom(e);
        }

        private async void Timer_Tick(object sender, object e)
        {
            DhtReading reading = new DhtReading();
            int val = this.TotalAttempts;
            this.TotalAttempts++;

            reading = await _dht.GetReadingAsync().AsTask();

            _retryCount.Add(reading.RetryCount);
            this.OnPropertyChanged(nameof(this.AverageRetriesDisplay));
            this.OnPropertyChanged(nameof(this.TotalAttempts));
            this.OnPropertyChanged(nameof(this.PercentSuccess));

			if (reading.IsValid)
			{
				this.TotalSuccess++;
				this.Temperature = Convert.ToSingle(reading.Temperature);
				this.Humidity = Convert.ToSingle(reading.Humidity);
				this.LastUpdated = DateTimeOffset.Now;
				this.OnPropertyChanged(nameof(this.SuccessRate));
			}

            this.OnPropertyChanged(nameof(this.LastUpdatedDisplay));
        }

        public string PercentSuccess
        {
            get
            {
                string returnValue = string.Empty;

                int attempts = this.TotalAttempts;

                if (attempts > 0)
                {
                    returnValue = string.Format("{0:0.0}%", 100f * (float)this.TotalSuccess / (float)attempts);
                }
                else
                {
                    returnValue = "0.0%";
                }

                return returnValue;
            }
        }

        private int _totalAttempts = 0;
        public int TotalAttempts
        {
            get
            {
                return _totalAttempts;
            }
            set
            {
                this.SetProperty(ref _totalAttempts, value);
                this.OnPropertyChanged(nameof(this.PercentSuccess));
            }
        }

        private int _totalSuccess = 0;
        public int TotalSuccess
        {
            get
            {
                return _totalSuccess;
            }
            set
            {
                this.SetProperty(ref _totalSuccess, value);
                this.OnPropertyChanged(nameof(this.PercentSuccess));
            }
        }

        private float _humidity = 0f;
        public float Humidity
        {
            get
            {
                return _humidity;
            }

            set
            {
                this.SetProperty(ref _humidity, value);
                this.OnPropertyChanged(nameof(this.HumidityDisplay));
            }
        }

        public string HumidityDisplay
        {
            get
            {
                return string.Format("{0:0.0}% RH", this.Humidity);
            }
        }

        private float _temperature = 0f;
        public float Temperature
        {
            get
            {
                return _temperature;
            }
            set
            {
                this.SetProperty(ref _temperature, value);
                this.OnPropertyChanged(nameof(this.TemperatureDisplay));
            }
        }

        public string TemperatureDisplay
        {
            get
            {
                return string.Format("{0:0.0} °C", this.Temperature);
            }
        }

        private DateTimeOffset _lastUpdated = DateTimeOffset.MinValue;
        public DateTimeOffset LastUpdated
        {
            get
            {
                return _lastUpdated;
            }
            set
            {
                this.SetProperty(ref _lastUpdated, value);
                this.OnPropertyChanged(nameof(this.LastUpdatedDisplay));
            }
        }

        public string LastUpdatedDisplay
        {
            get
            {
                string returnValue = string.Empty;

                TimeSpan elapsed = DateTimeOffset.Now.Subtract(this.LastUpdated);

                if (this.LastUpdated == DateTimeOffset.MinValue)
                {
                    returnValue = "never";
                }
                else if (elapsed.TotalSeconds < 60d)
                {
                    int seconds = (int)elapsed.TotalSeconds;

                    if (seconds < 2)
                    {
                        returnValue = "just now";
                    }
                    else
                    {
                        returnValue = string.Format("{0:0} {1} ago", seconds, seconds == 1 ? "second" : "seconds");
                    }
                }
                else if (elapsed.TotalMinutes < 60d)
                {
                    int minutes = (int)elapsed.TotalMinutes == 0 ? 1 : (int)elapsed.TotalMinutes;
                    returnValue = string.Format("{0:0} {1} ago", minutes, minutes == 1 ? "minute" : "minutes");
                }
                else if (elapsed.TotalHours < 24d)
                {
                    int hours = (int)elapsed.TotalHours == 0 ? 1 : (int)elapsed.TotalHours;
                    returnValue = string.Format("{0:0} {1} ago", hours, hours == 1 ? "hour" : "hours");
                }
                else
                {
                    returnValue = "a long time ago";
                }

                return returnValue;
            }
        }

        public int AverageRetries
        {
            get
            {
                int returnValue = 0;

                if (_retryCount.Count() > 0)
                {
                    returnValue = (int)_retryCount.Average();
                }

                return returnValue;
            }
        }

        public string AverageRetriesDisplay
        {
            get
            {
                return string.Format("{0:0}", this.AverageRetries);
            }
        }

        public string SuccessRate
        {
            get
            {
                string returnValue = string.Empty;

                double totalSeconds = DateTimeOffset.Now.Subtract(_startedAt).TotalSeconds;
                double rate = this.TotalSuccess / totalSeconds;

                if (rate < 1)
                {
                    returnValue = string.Format("{0:0.00} seconds/reading", 1d / rate);
                }
                else
                {
                    returnValue = string.Format("{0:0.00} readings/sec", rate);
                }

                return returnValue;
            }
        }
    }
}

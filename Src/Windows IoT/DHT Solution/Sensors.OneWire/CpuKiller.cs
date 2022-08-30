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
using System.Threading;
using System.Threading.Tasks;

namespace Sensors.OneWire
{
	public class CpuKiller
	{
		private static Random Random = new Random();
		private static CancellationTokenSource _cancellationTokenSource = new CancellationTokenSource();

		public CpuKiller(TimeSpan length)
		{
			this.Length = length;
			this.StartDateTime = DateTimeOffset.Now;
		}

		public TimeSpan Length { get; set; }
		public DateTimeOffset StartDateTime { get; set; }

		public Task Run()
		{
			return Task.Factory.StartNew(() =>
			{
				while (DateTimeOffset.Now.Subtract(this.StartDateTime) < this.Length)
				{
					//decimal x = 1m + 2.1m - 10.1m + 1.262636474746367547474m + 10.1818181m;
					//x += 1.2m;
					//x /= 1.28282837374m;
					//x *= 6.103847456375785m;
					//Task.Delay(1).Wait();
				}
			});
		}

		public static void StartEmulation()
		{
			Task.Factory.StartNew(() =>
			{
				while (!_cancellationTokenSource.IsCancellationRequested)
				{
					int taskCount = CpuKiller.Random.Next(1000);
					Task[] tasks = new Task[taskCount];

					for (int i = 0; i < taskCount; i++)
					{
						CpuKiller taskSimulator = new CpuKiller(TimeSpan.FromSeconds(CpuKiller.Random.Next(60, 3600)));
						tasks[i] = taskSimulator.Run();
					}

					Task.WaitAll(tasks);
				}
			});
		}

		public static void StopEmulation()
		{
			_cancellationTokenSource.Cancel();
		}
	}
}

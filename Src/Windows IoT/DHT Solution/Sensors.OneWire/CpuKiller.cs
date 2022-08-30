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

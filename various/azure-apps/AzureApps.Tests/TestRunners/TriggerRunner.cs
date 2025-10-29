using System;
using System.IO;
using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Extensions.Timers;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;

namespace dk.schau.AzureApps.Tests.TestRunners
{
	public class TriggerRunner
	{
		readonly string _name;

		public TriggerRunner(string name)
		{
			_name = name;
		}

		public void ExecuteTimerTrigger(Action<TimerInfo, ILogger, ExecutionContext> action)
		{
			var context = GetContext();
			var schedule = new DailySchedule();
			var status = new ScheduleStatus();
			var timerInfo = new TimerInfo(schedule, status, isPastDue: false);

			var serviceCollection = new ServiceCollection();
			serviceCollection.AddLogging(builder => builder.AddConsole());

			using (var serviceProvider = serviceCollection.BuildServiceProvider())
			{
				using (var loggerFactory = serviceProvider.GetService<ILoggerFactory>())
				{
					action(timerInfo, loggerFactory.CreateLogger("Lichess"), context);
				}
			}
		}

		ExecutionContext GetContext()
		{
			var fullPath = Path.GetFullPath(@"../../../../AzureApps");
			return new ExecutionContext
			{
				FunctionAppDirectory = fullPath,
				FunctionDirectory = fullPath,
				FunctionName = "LichessTrigger",
				InvocationId = Guid.NewGuid(),
			};
		}
	}
}
using System;
using dk.schau.AzureApps.Modules.Eventor;
using dk.schau.AzureApps.Queries;
using Microsoft.Azure.WebJobs;
using Microsoft.Extensions.Logging;

namespace dk.schau.AzureApps
{
	public static class EventorTrigger
	{
		[FunctionName("Eventor")]
		public static void Run([TimerTrigger("0 0 1 * * *")]TimerInfo myTimer, ILogger log, ExecutionContext context)
		{
			var appsRc = new GetConfiguration(context).Execute();
			new Eventor(appsRc).Execute();
		}
	}
}

using System;
using dk.schau.AzureApps.Modules.ChessMag;
using dk.schau.AzureApps.Queries;
using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Host;
using Microsoft.Extensions.Logging;

namespace dk.schau.AzureApps
{
	public static class ChessMagTrigger
	{
		[FunctionName("ChessMag")]
		public static void Run([TimerTrigger("0 0 12 * * *")]TimerInfo myTimer, ILogger log, ExecutionContext context)
		{
			var appsRc = new GetConfiguration(context).Execute();
			new ChessMag(appsRc).Execute();
		}
	}
}

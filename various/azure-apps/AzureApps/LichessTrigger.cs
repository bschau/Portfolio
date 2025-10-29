using dk.schau.AzureApps.Modules.Lichess;
using dk.schau.AzureApps.Queries;
using Microsoft.Azure.WebJobs;
using Microsoft.Extensions.Logging;

namespace dk.schau.AzureApps
{
	public static class LichessTrigger
	{
		[FunctionName("Lichess")]
		public static void Run([TimerTrigger("0 0 7 * * *")]TimerInfo myTimer, ILogger log, ExecutionContext context)
		{
			var appsRc = new GetConfiguration(context).Execute();
			new Lichess(appsRc).Execute();
		}
	}
}

using dk.schau.AzureApps.Modules.Qotd;
using dk.schau.AzureApps.Queries;
using Microsoft.Azure.WebJobs;
using Microsoft.Extensions.Logging;

namespace dk.schau.AzureApps
{
	public static class QotdTrigger
	{

		[FunctionName("Qotd")]
		public static void Run([TimerTrigger("0 0 5 * * *")]TimerInfo myTimer, ILogger log, ExecutionContext context)
		{
			var appsRc = new GetConfiguration(context).Execute();
			new Qotd(appsRc).Execute();
		}
	}
}

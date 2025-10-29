using dk.schau.AzureApps.Modules.Comics;
using dk.schau.AzureApps.Queries;
using Microsoft.Azure.WebJobs;
using Microsoft.Extensions.Logging;

namespace dk.schau.AzureApps
{
	public static class ComicsTrigger
	{
		[FunctionName("Comics")]
		public static void Run([TimerTrigger("0 0 6 * * *")]TimerInfo myTimer, ILogger log, ExecutionContext context)
		{
			var appsRc = new GetConfiguration(context).Execute();
			new Comics(appsRc).Execute();
		}
	}
}

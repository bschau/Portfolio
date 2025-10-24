using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace dk.schau.AzureApps.Tests.TestRunners
{
	[TestClass]
	public class ComicsTriggerTest
	{
		[TestMethod]
		public void RunComics()
		{
			new TriggerRunner("ComicsTrigger").ExecuteTimerTrigger(ComicsTrigger.Run);
		}
	}
}
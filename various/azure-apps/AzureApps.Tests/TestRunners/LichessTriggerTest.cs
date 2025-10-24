using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace dk.schau.AzureApps.Tests.TestRunners
{
	[TestClass]
	public class LichessTriggerTest
	{
		[TestMethod]
		public void RunLichess()
		{
			new TriggerRunner("LichessTrigger").ExecuteTimerTrigger(LichessTrigger.Run);
		}
	}
}
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace dk.schau.AzureApps.Tests.TestRunners
{
	[TestClass]
	public class QotdTriggerTest
	{
		[TestMethod]
		public void RunQotd()
		{
			new TriggerRunner("QotdTrigger").ExecuteTimerTrigger(QotdTrigger.Run);
		}
	}
}
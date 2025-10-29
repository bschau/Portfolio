using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace dk.schau.AzureApps.Tests.TestRunners
{
	[TestClass]
	public class EventorTriggerTest
	{
		[TestMethod]
		public void RunEventor()
		{
			new TriggerRunner("EventorTrigger").ExecuteTimerTrigger(EventorTrigger.Run);
		}
	}
}
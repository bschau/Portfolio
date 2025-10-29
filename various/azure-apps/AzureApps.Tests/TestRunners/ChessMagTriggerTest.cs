using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace dk.schau.AzureApps.Tests.TestRunners
{
	[TestClass]
	public class ChessMagTriggerTest
	{
		[TestMethod]
		public void RunChessMag()
		{
			new TriggerRunner("ChessMagTrigger").ExecuteTimerTrigger(ChessMagTrigger.Run);
		}
	}
}
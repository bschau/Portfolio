using System;
using dk.schau.AzureApps.Translations;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace dk.schau.AzureApps.Tests.Translations
{
	[TestClass]
	public class PickupMailboxAddressTest
	{
		[TestMethod]
		[DataRow("Mr Unknown <mister@localhost>", "Mr Unknown", "mister@localhost")]
		[DataRow("Mr Unknown <mister@localhost", "Mr Unknown", "mister@localhost")]
		[DataRow("<mister@localhost>", "mister@localhost", "mister@localhost")]
		[DataRow("mister@localhost", "mister@localhost", "mister@localhost")]
		public void VerifyNameEmail(string src, string name, string email)
		{
			var pickup = new PickupMailboxAddress();

			var mailbox = pickup.Execute(src);

			Assert.AreEqual(name, mailbox.Name);
			Assert.AreEqual(email, mailbox.Address);
		}

		[TestMethod]
		[ExpectedException(typeof(ArgumentException))]
		public void NoEmailThrowsException()
		{
			var pickup = new PickupMailboxAddress();

			var mailbox = pickup.Execute("Mr Unknown");
		}

		[TestMethod]
		[ExpectedException(typeof(ArgumentException))]
		public void NullAddressThrowsException()
		{
			var pickup = new PickupMailboxAddress();

			var mailbox = pickup.Execute("Mr Unknown <>");
		}
	}
}
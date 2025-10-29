using System;
using MimeKit;

namespace dk.schau.AzureApps.Translations
{
	public class PickupMailboxAddress
	{
		public MailboxAddress Execute(string src)
		{
			var parts = src.Split('<', StringSplitOptions.RemoveEmptyEntries);
			var name = parts[0].Trim();
			var email = parts.Length == 1
						? name
						: parts[1];

			var endPos = email.IndexOf('>');
			email = (endPos < 0
					? email
					: email.Substring(0, endPos)).Trim();

			if (email.IndexOf('@') < 0)
			{
				throw new ArgumentException($"No @-sign in email address: {src}");
			}

			if (parts.Length == 1)
			{
				name = email;
			}

			return new MailboxAddress(name, email);
		}
	}
}
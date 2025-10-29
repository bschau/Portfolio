using dk.schau.AzureApps.DomainModel;
using dk.schau.AzureApps.Translations;
using MailKit.Net.Smtp;
using MimeKit;

namespace dk.schau.AzureApps.Commands
{
	internal class SendEmail
	{
		readonly MailSettings _mailSettings;

		internal SendEmail(MailSettings mailSettings)
		{
			_mailSettings = mailSettings;
		}

		internal void Execute(string subject, string body)
		{
			var message = new MimeMessage
			{
				Subject = subject,
			};

			var pickupMailboxAddress = new PickupMailboxAddress();
			message.From.Add(pickupMailboxAddress.Execute(_mailSettings.From));
			message.To.Add(pickupMailboxAddress.Execute(_mailSettings.To));

			var bodyBuilder = new BodyBuilder
			{
				HtmlBody = body
			};

			message.Body = bodyBuilder.ToMessageBody();

			using (var client = new SmtpClient())
			{
				client.Connect(_mailSettings.MailServer, GetSmtpPort(_mailSettings.MailServerPort));
				client.AuthenticationMechanisms.Remove("XOAUTH2");
				client.Authenticate(_mailSettings.MailServerAccount, _mailSettings.MailServerPassword);
				client.Send(message);
				client.Disconnect(true);
			}
		}

		int GetSmtpPort(int defaultPort)
		{
			return defaultPort == 0 ? 25 : defaultPort;
		}
	}
}
using MailKit.Net.Smtp;
using MimeKit;
using VPSApps.DomainModel;

namespace VPSApps.Commands
{
	internal class SendEmail
	{
		readonly EmailAddress _from;
		readonly EmailAddress _to;

		internal SendEmail(EmailAddress from, EmailAddress to)
		{
			_from = from;
			_to = to;
		}

		internal void Execute(string subject, string body)
		{
			ExecuteWithAttachment(subject,  body, attachment: null);
		}

		internal void ExecuteWithAttachment(string subject, string body, EmailAttachment attachment)
		{
			var message = new MimeMessage
			{
				Subject = subject,
			};

			var from = new MailboxAddress(_from.Name, _from.Email);
			message.From.Add(from);

			var to = new MailboxAddress(_to.Name, _to.Email);
			message.To.Add(to);

			var bodyBuilder = new BodyBuilder
			{
				HtmlBody = body
			};

			if (attachment != null)
			{
				bodyBuilder.Attachments.Add(attachment.Filename, attachment.Data, attachment.ContentType);
			}

			message.Body = bodyBuilder.ToMessageBody();
			using (var client = new SmtpClient())
			{
				client.Connect("localhost", 25, MailKit.Security.SecureSocketOptions.None);
				client.Send(message);
				client.Disconnect(true);
			}
		}
	}
}
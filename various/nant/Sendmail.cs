using System;
using Microsoft.Exchange.WebServices.Data;
using Nant.Queries;
using NAnt.Core;
using NAnt.Core.Attributes;
using Task = NAnt.Core.Task;

namespace Nant
{
	[TaskName("sendmail")]
	internal class Sendmail : Task
	{
		private const string _mailbox = "sender@sender.domain";
		private const string _password = "SenderPassword";
		private const string _mailFrom = "reply-to@sender.domain";
		private const string _smtpClient = "https://xxxxxxxxxx.outlook.com/EWS/exchange.asmx";
		private const int _timeoutInSeconds = 10;
		private readonly GetMailTemplateByName _getMailTemplateByName = new GetMailTemplateByName();

		[TaskAttribute("solution", Required = true)]
		[StringValidator(AllowEmpty = false)]
		public string Solution { get; set; }

		[TaskAttribute("template", Required = true)]
		[StringValidator(AllowEmpty = false)]
		public string Template { get; set; }

		[TaskAttribute("to", Required = true)]
		[StringValidator(AllowEmpty = false)]
		public string To { get; set; }

		protected override void ExecuteTask()
		{
			try
			{
				var template = _getMailTemplateByName.Execute(Template);
				var mailMessage = new EmailMessage(CreateBinding())
				{
					From = new EmailAddress(_mailFrom),
				};

				var addresses = AddRecipients(mailMessage);
				mailMessage.Subject = Substitute(template.Subject);
				mailMessage.Body = Substitute(template.Body);
				mailMessage.Body.BodyType = BodyType.HTML;
				mailMessage.Send();

				Log(Level.Info, "Email sent to " + addresses);
			}
			catch (Exception exception)
			{
				throw new BuildException(exception.Message, Location);
			}
		}

		private ExchangeService CreateBinding()
		{
			var service = new ExchangeService(ExchangeVersion.Exchange2010_SP1)
			{
				Credentials = new WebCredentials(_mailbox, _password),
				Url = new Uri(_smtpClient),
				Timeout = _timeoutInSeconds * 1000,
			};
			return service;
		}

		private string AddRecipients(EmailMessage mailMessage)
		{
			var addresses = To.Split(new [] { ',' }, StringSplitOptions.RemoveEmptyEntries);

			foreach (var address in addresses)
			{
				mailMessage.ToRecipients.Add(new EmailAddress(address));
			}

			return string.Join(", ", addresses);
		}

		private string Substitute(string source)
		{
			return source.Replace("@SOLUTION@", Solution);
		}
	}
}

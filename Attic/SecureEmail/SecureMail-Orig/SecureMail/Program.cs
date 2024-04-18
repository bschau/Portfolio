using SecureMail.DomainModel;
using System;
using System.Configuration;
using System.Linq;
using System.Net;
using System.Net.Mail;
using System.Security.Cryptography.X509Certificates;

namespace SecureMail
{
	class Program
	{
		const string _senderAddress = "rovsingapp@gmail.com";
		const string _senderPassword = "R0v$ing@pp";

		static void Main(string[] args)
		{
			/*
			 * If a "Keyset does not exist" is thrown then adjust permissions for the signing and/or seal certificate(s) in MMC.
			 */
			var encryptionCert = new X509Certificate2(@"c:\Users\Brian Schau\Desktop\certificate.crt"); //.pfx", "abcd1234");
			var signingCert = GetCertificate("SecureMailSignCertificate");
			var sealCert = GetCertificate("SecureMailSealCertificate");
			var attachmentData = new byte[] { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

			var message = new SecureMailMessage
			{
				From = new MailAddress("noreply@mail.esignatur.dk", "esignatur"),
				To = new MailAddress("bsc@esignatur.dk", "Brian"),
				ReplyTo = new MailAddress("brian.schau@gmail.com", "Brian"),
				Subject = "Udlån",
				Body = "<h2>Udlån</h2><p>Alle har nu underskrevet og lånet kan udbetales.</p>",
				EncryptionCertificate = encryptionCert,
				SealCertificate = sealCert,
				SignCertificate = signingCert,
			};
			message.Attachments.Add(new SecureAttachment
			{
				ContentType = "application/pdf",
				Data = attachmentData,
				Filename = "udlån.pdf"
			});

			var client = new SmtpClient("smtp.gmail.com", 587)
			{
				EnableSsl = true,
				DeliveryMethod = SmtpDeliveryMethod.Network,
				UseDefaultCredentials = false,
				Credentials = new NetworkCredential(_senderAddress, _senderPassword)
			};
			client.Send(message.ToMailMessage());
		}

		static X509Certificate2 GetCertificate(string certificateType)
		{
			var thumbprint = ConfigurationManager.AppSettings[certificateType];
			if (string.IsNullOrWhiteSpace(thumbprint))
			{
				throw new Exception(string.Format("{0} not defined in AppSettings", certificateType));
			}

			var findValue = new string(thumbprint.Trim().Where(x => x != ' ' && x != '\t').ToArray());
			var store = new X509Store(StoreName.My, StoreLocation.LocalMachine);
			try
			{
				store.Open(OpenFlags.ReadOnly);
				var collection = store.Certificates.Find(X509FindType.FindByThumbprint, findValue, validOnly: false);
				if (collection.Count == 0)
				{
					throw new ArgumentException(string.Format("Cannot find certificate with thumbprint: {0}", thumbprint));
				}

				return collection[0];
			}
			finally
			{
				store.Close();
			}
		}
	}
}

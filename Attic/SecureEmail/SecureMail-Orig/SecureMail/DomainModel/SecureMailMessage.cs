using System;
using System.Collections.Generic;
using System.IO;
using System.Net.Mail;
using System.Net.Mime;
using System.Security.Cryptography.Pkcs;
using System.Security.Cryptography.X509Certificates;
using System.Text;

namespace SecureMail.DomainModel
{
	public class SecureMailMessage
	{
		struct Chunk
		{
			internal string Boundary;
			internal byte[] Body;
		}

		internal MailAddress From { get; set; }
		internal MailAddress To { get; set; }
		internal MailAddress ReplyTo { get; set; }
		internal string Subject { get; set; }
		internal string Body { get; set; }
		internal List<SecureAttachment> Attachments { get; private set; }
		internal X509Certificate2 EncryptionCertificate { get; set; }
		internal X509Certificate2 SealCertificate { get; set; }
		internal X509Certificate2 SignCertificate { get; set; }

		public SecureMailMessage()
		{
			Attachments = new List<SecureAttachment>();
		}

		Chunk GetUnsignedContent()
		{
			var body = Encoding.UTF8.GetBytes(Body);
			body = Encoding.ASCII.GetBytes(Convert.ToBase64String(body, Base64FormattingOptions.InsertLineBreaks));

			var message = new StringBuilder();
			var boundary = NewBoundary();
			message.AppendFormat("\r\n--{0}\r\n", boundary);
			message.AppendFormat("Content-Type: text/html; boundary={0}; charset=UTF-8\r\n", boundary);
			message.Append("Content-Transfer-Encoding: base64\r\n\r\n");
			message.AppendFormat("{0}\r\n", Encoding.UTF8.GetString(body));

			foreach (var attachment in Attachments)
			{
				message.AppendFormat("--{0}\r\n", boundary);
				var filename = string.Format("\"=?UTF-8?B?{0}?=", Convert.ToBase64String(Encoding.UTF8.GetBytes(attachment.Filename)));
				message.AppendFormat("Content-Type: {0}; boundary={1}; filename={2}; charset=UTF-8\r\n", attachment.ContentType, boundary, attachment.Filename);
				message.AppendFormat("Content-Disposition: attachment; filename={0}\r\n", filename);
				message.Append("Content-Transfer-Encoding: base64\r\n\r\n");
				message.AppendFormat("{0}\r\n\r\n", Convert.ToBase64String(attachment.Data, Base64FormattingOptions.InsertLineBreaks));
			}

			message.AppendFormat("--{0}--\r\n", boundary);
			return new Chunk
			{
				Body = Encoding.ASCII.GetBytes(message.ToString()),
				Boundary = boundary
			};
		}

		Chunk Sign(byte[] body, string outerBoundary)
		{
			var header = BuildSignedMessageHeader(body, outerBoundary);
			var signature = GetSignature(header);
			var boundary = NewBoundary();
			var message = BuildSignedMessage(boundary, header, signature);

			return new Chunk
			{
				Body = Encoding.ASCII.GetBytes(message),
				Boundary = boundary
			};
		}

		string BuildSignedMessageHeader(byte[] body, string boundary)
		{
			var message = new StringBuilder();
			message.AppendFormat("Content-Type: multipart/mixed; boundary={0}\r\n", boundary);
			message.Append("Content-Transfer-Encoding: 7bit\r\n\r\n");
			message.Append(Encoding.ASCII.GetString(body));

			return message.ToString();
		}

		byte[] GetSignature(string message)
		{
			var bytes = Encoding.ASCII.GetBytes(message);
			var signedCms = new SignedCms(new ContentInfo(bytes), true);
			var cmsSigner = new CmsSigner(SubjectIdentifierType.IssuerAndSerialNumber, SignCertificate)
			{
				IncludeOption = X509IncludeOption.WholeChain,
			};

			cmsSigner.Certificates.Add(EncryptionCertificate);
			cmsSigner.SignedAttributes.Add(new Pkcs9SigningTime());
			signedCms.ComputeSignature(cmsSigner, false);

			return signedCms.Encode();
		}

		string BuildSignedMessage(string boundary, string header, byte[] signature)
		{
			var message = new StringBuilder();

			message.AppendFormat("--{0}\r\n", boundary);
			message.AppendFormat("{0}\r\n", header);
			message.AppendFormat("--{0}\r\n", boundary);
			message.Append("Content-Type: application/x-pkcs7-signature; name=\"smime.p7s\"\r\n");
			message.Append("Content-Transfer-Encoding: base64\r\n");
			message.Append("Content-Disposition: attachment; filename=\"smime.p7s\"\r\n\r\n");
			message.AppendFormat("{0}\r\n\r\n", Convert.ToBase64String(signature, Base64FormattingOptions.InsertLineBreaks));
			message.AppendFormat("--{0}--\r\n", boundary);
			return message.ToString();
		}

		byte[] Encrypt(byte[] body, string boundary)
		{
			var encryptionCertificates = new X509Certificate2Collection();
			encryptionCertificates.Add(EncryptionCertificate);
			encryptionCertificates.Add(SealCertificate);

			var message = new StringBuilder();
			message.AppendFormat("Content-Type: multipart/signed; boundary={0}; protocol=\"application/x-pkcs7-signature\"; micalg=SHA1\r\n", boundary);
			message.Append("Content-Transfer-Encoding: 7bit\r\n\r\n");
			message.Append(Encoding.ASCII.GetString(body));

			var bytes = Encoding.ASCII.GetBytes(message.ToString());
			var envelopedCms = new EnvelopedCms(new ContentInfo(bytes));
			var recipients = new CmsRecipientCollection(SubjectIdentifierType.IssuerAndSerialNumber, encryptionCertificates);
			envelopedCms.Encrypt(recipients);
			return envelopedCms.Encode();
		}

		public MailMessage ToMailMessage()
		{
			var mailMessage = new MailMessage
			{
				From = From,
				Sender = From,
				Subject = Subject,
				SubjectEncoding = Encoding.UTF8,
			};

			if (ReplyTo != null)
			{
				mailMessage.ReplyToList.Add(ReplyTo);
			}

			mailMessage.To.Add(To);

			var content = GetUnsignedContent();
			content = Sign(content.Body, content.Boundary);
			var bytes = Encrypt(content.Body, content.Boundary);

			var contentStream = new MemoryStream();
			contentStream.Write(bytes, 0, bytes.Length);
			contentStream.Position = 0;

			var contentType = string.Format("application/x-pkcs7-mime; smime-type=enveloped-data; name=smime.p7m; boundary={0}", NewBoundary());
			var contentView = new AlternateView(contentStream, contentType)
			{
				TransferEncoding = TransferEncoding.Base64,
			};

			mailMessage.AlternateViews.Add(contentView);
			return mailMessage;
		}

		string NewBoundary()
		{
			return string.Format("esignatur-{0}", Guid.NewGuid().ToString());
		}
	}
}

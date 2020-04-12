using System;
using System.IO;
using System.Security.Cryptography.X509Certificates;
using Microsoft.Extensions.Configuration;

namespace AzureNemID
{
	public static class Globals
	{
		public static void Init()
		{
			NemIDAppletPrefix = "Hello, there";
			LoadCertificate();
		}

		static void LoadCertificate()
		{
			var certificate = Convert.FromBase64String(Certificate.CertificateData);
			NemIDCertificate = new X509Certificate2(certificate, Certificate.CertificatePassword, X509KeyStorageFlags.Exportable | X509KeyStorageFlags.UserKeySet);

			var encodedCertificate = NemIDCertificate.Export(X509ContentType.Cert);
			Base64EncodedNemIDCertificate = Convert.ToBase64String(encodedCertificate);
		}

		public static string NemIDAppletPrefix { get; private set; }
		public static X509Certificate2 NemIDCertificate { get; private set; }
		public static string Base64EncodedNemIDCertificate { get; private set;}
	}
}

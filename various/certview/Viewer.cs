using certview.Queries;
using System;
using System.Globalization;
using System.Linq;
using System.Security.Cryptography.X509Certificates;

namespace certview
{
	public class Viewer
	{
		readonly bool _enableVerify;

		public Viewer(bool enableVerify)
		{
			_enableVerify = enableVerify;
		}

		public void Execute(string search)
		{
			var certificates = new GetCertificates().Execute(search);
			var previousLocation = string.Empty;
			var previousName = string.Empty;
			var header = false;

			if (!_enableVerify)
			{
				Console.WriteLine("Certification validation disabled");
			}

			foreach (var certificate in certificates.OrderBy(x => x.StoreLocation).ThenBy(x => x.StoreName).ThenBy(x => x.X509Certificate2.SubjectName.Name))
			{
				var x509Certificate2 = certificate.X509Certificate2;
				if (certificate.StoreLocation != previousLocation || certificate.StoreName != previousName)
				{
					previousLocation = certificate.StoreLocation;
					previousName = certificate.StoreName;
					if (header)
					{
						Console.WriteLine();
					}

					Console.WriteLine("{0}.{1}:", certificate.StoreLocation, certificate.StoreName);
				}
				Console.WriteLine(@"	Thumbprint:	 	{0}", x509Certificate2.Thumbprint);
				Console.WriteLine(@"	Subject:		{0}", x509Certificate2.Subject);
				Console.WriteLine(@"	Issuer:	 		{0}", x509Certificate2.Issuer);
				Console.WriteLine(@"	SerialNumber:	   	{0}", x509Certificate2.GetSerialNumberString());
				Console.WriteLine(@"	NotBefore:		{0}", x509Certificate2.NotBefore.ToString(CultureInfo.CurrentCulture));
				Console.WriteLine(@"	NotAfter:	   	{0}", x509Certificate2.NotAfter.ToString(CultureInfo.CurrentCulture));
				Console.WriteLine(@"	Version:		{0}", x509Certificate2.Version.ToString(CultureInfo.CurrentCulture));

				if (x509Certificate2.HasPrivateKey)
				{
					Console.WriteLine("	Private Key:");
					ShowKeyPath("From CSP", new GetPrivateKeyFilenameForCertificateByCsp(), x509Certificate2);
					ShowKeyPath("From Win32 API", new GetPrivateKeyFilenameForCertificateByWinApi(), x509Certificate2);
				}

				if (_enableVerify)
				{
					Console.WriteLine("	Status:			{0}", x509Certificate2.Verify() ? "OK" : "did not verify");
				}
				Console.WriteLine();
				header = true;
			}
		}

		void ShowKeyPath(string header, IGetPrivateKeyFilename getPrivateKeyFilename, X509Certificate2 certificate)
		{
			Console.Write("		{0}:	", header);
			try
			{
				var filepath = getPrivateKeyFilename.Execute(certificate);
				Console.WriteLine(@"{0}", filepath);
			}
			catch (Exception exception)
			{
				Console.WriteLine(@"{0}", exception.Message.Trim());
			}
		}
	}
}

using System;
using System.IO;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;

namespace certview.Queries
{
	public class GetPrivateKeyFilenameForCertificateByCsp : IGetPrivateKeyFilename
	{
		public string Execute(X509Certificate2 certificate)
		{
			var privateKey = certificate.PrivateKey as RSACryptoServiceProvider;
			if (privateKey == null)
			{
				throw new Exception("Certificate has no private key");
			}
			var keyFileName = privateKey.CspKeyContainerInfo.UniqueKeyContainerName;
			var machineKeyDirectory = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + @"\Microsoft\Crypto\RSA\MachineKeys";

			if (Directory.GetFiles(machineKeyDirectory, keyFileName).Length > 0)
			{
				return string.Format(@"{0}\{1}", machineKeyDirectory, keyFileName);
			}

			var currentUserProfile = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + @"\Microsoft\Crypto\RSA\";
			var subDirectories = Directory.GetDirectories(currentUserProfile);
			foreach (var directory in subDirectories)
			{
				if (Directory.GetFiles(directory, keyFileName).Length > 0)
				{
					return string.Format(@"{0}{1}\{2}", currentUserProfile, directory, keyFileName);
				}
			}

			return "Private key not found or is inaccessible";
		}
	}
}

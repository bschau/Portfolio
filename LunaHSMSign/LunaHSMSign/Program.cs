using System;
using System.IO;
using System.Linq;
using System.Security.Cryptography.X509Certificates;

namespace LunaHSMSign
{
	class Program
	{
		const string _certificateThumbprint = "680aa63403aa89f2f9a58358fdc0401f286921eb";

		static void Main(string[] args)
		{
			if (args.Length != 1)
			{
				Console.Error.WriteLine("Usage: LunaHSMSign.exe pdf-to-sign.pdf");
				Environment.Exit(1);
			}

			if (!File.Exists(args[0]))
			{
				Console.Error.WriteLine("Failed to find: {0}", args[0]);
				Environment.Exit(1);
			}

			var certificate = GetCertificateForThumbprint(_certificateThumbprint);
			var pdf = File.ReadAllBytes(args[0]);
			var signed = new Sign().Execute(pdf, certificate);
			var signedFilename = string.Format("{0}-signed.pdf", Path.GetFileNameWithoutExtension(args[0]));
			File.WriteAllBytes(signedFilename, signed);
		}

		static X509Certificate2 GetCertificateForThumbprint(string thumbprint)
		{
			var findValue = new string(thumbprint.Trim().Where(x => x != ' ' && x != '\t').ToArray());
			var store = new X509Store(StoreName.My, StoreLocation.LocalMachine);
			try
			{
				store.Open(OpenFlags.ReadOnly);
				var collection = store.Certificates.Find(X509FindType.FindByThumbprint, findValue, validOnly: false);
				if (collection.Count == 0)
				{
					Console.Error.WriteLine("Cannot find certificate with thumbprint: '{0}'", thumbprint);
					Environment.Exit(1);
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

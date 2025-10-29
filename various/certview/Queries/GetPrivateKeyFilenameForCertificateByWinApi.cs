using System;
using System.Runtime.InteropServices;
using System.Security.Cryptography.X509Certificates;
using System.Text;

namespace certview.Queries
{
	public class GetPrivateKeyFilenameForCertificateByWinApi : IGetPrivateKeyFilename
	{
		internal enum CryptGetProvParamType
		{
			PP_ENUMALGS = 1,
			PP_ENUMCONTAINERS = 2,
			PP_IMPTYPE = 3,
			PP_NAME = 4,
			PP_VERSION = 5,
			PP_CONTAINER = 6,
			PP_CHANGE_PASSWORD = 7,
			PP_KEYSET_SEC_DESCR = 8,
			PP_CERTCHAIN = 9,
			PP_KEY_TYPE_SUBTYPE = 10,
			PP_PROVTYPE = 16,
			PP_KEYSTORAGE = 17,
			PP_APPLI_CERT = 18,
			PP_SYM_KEYSIZE = 19,
			PP_SESSION_KEYSIZE = 20,
			PP_UI_PROMPT = 21,
			PP_ENUMALGS_EX = 22,
			PP_ENUMMANDROOTS = 25,
			PP_ENUMELECTROOTS = 26,
			PP_KEYSET_TYPE = 27,
			PP_ADMIN_PIN = 31,
			PP_KEYEXCHANGE_PIN = 32,
			PP_SIGNATURE_PIN = 33,
			PP_SIG_KEYSIZE_INC = 34,
			PP_KEYX_KEYSIZE_INC = 35,
			PP_UNIQUE_CONTAINER = 36,
			PP_SGC_INFO = 37,
			PP_USE_HARDWARE_RNG = 38,
			PP_KEYSPEC = 39,
			PP_ENUMEX_SIGNING_PROT = 40,
			PP_CRYPT_COUNT_KEY_USE = 41,
		}

		[DllImport("crypt32", CharSet = CharSet.Unicode, SetLastError = true)]
		extern static bool CryptAcquireCertificatePrivateKey(IntPtr pCert, uint dwFlags, IntPtr pvReserved, ref IntPtr phCryptProv, ref int pdwKeySpec, ref bool pfCallerFreeProv);

		[DllImport("advapi32", CharSet = CharSet.Unicode, SetLastError = true)]
		extern static bool CryptGetProvParam(IntPtr hCryptProv, CryptGetProvParamType dwParam, IntPtr pvData, ref int pcbData, uint dwFlags);

		[DllImport("advapi32", SetLastError = true)]
		extern static bool CryptReleaseContext(IntPtr hProv, uint dwFlags);

		public string Execute(X509Certificate2 certificate)
		{
			var keyFilename = GetKeyFileName(certificate);
			var keyFileDir = GetKeyFileDirectory(keyFilename);
			return string.Format(@"{0}\{1}", keyFileDir, keyFilename);
		}

		string GetKeyFileName(X509Certificate2 certificate)
		{
			const uint acquireFlags = 0;
			var hProvider = IntPtr.Zero;
			var freeProvider = false;
			var keyNumber = 0;
			var keyFileName = string.Empty;

			if (!CryptAcquireCertificatePrivateKey(certificate.Handle, acquireFlags, IntPtr.Zero, ref hProvider, ref keyNumber, ref freeProvider))
			{
				throw new Exception("Cannot acquire private key");
			}

			var pBytes = IntPtr.Zero;

			try
			{
				var cbBytes = 0;
				if (!CryptGetProvParam(hProvider, CryptGetProvParamType.PP_UNIQUE_CONTAINER, IntPtr.Zero, ref cbBytes, 0))
				{
					throw new Exception("Cannot get PP_UNIQUE_CONTAINER size");
				}

				pBytes = Marshal.AllocHGlobal(cbBytes);
				if (!CryptGetProvParam(hProvider, CryptGetProvParamType.PP_UNIQUE_CONTAINER, pBytes, ref cbBytes, 0))
				{
					throw new Exception("Cannot get PP_UNIQUE_CONTAINER");
				}

				var keyFileBytes = new byte[cbBytes];
				Marshal.Copy(pBytes, keyFileBytes, 0, cbBytes);

				keyFileName = Encoding.ASCII.GetString(keyFileBytes, 0, keyFileBytes.Length - 1);
			}
			finally
			{
				if (freeProvider)
				{
					CryptReleaseContext(hProvider, 0);
				}

				if (pBytes != IntPtr.Zero)
				{
					Marshal.FreeHGlobal(pBytes);
				}
			}

			if (keyFileName == null)
			{
				throw new InvalidOperationException("Unable to obtain private key file name");
			}

			return keyFileName;
		}

		string GetKeyFileDirectory(string keyFileName)
		{
			var allUserProfile = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData);
			var machineKeyDir = allUserProfile + @"\Microsoft\Crypto\RSA\MachineKeys";
			var fs = System.IO.Directory.GetFiles(machineKeyDir, keyFileName);

			if (fs.Length > 0)
			{
				return machineKeyDir;
			}

			var currentUserProfile = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
			var userKeyDir = currentUserProfile + @"\Microsoft\Crypto\RSA\";

			fs = System.IO.Directory.GetDirectories(userKeyDir);
			if (fs.Length < 1)
			{
				throw new InvalidOperationException("Unable to locate private key file directory");
			}

			foreach (var keyDir in fs)
			{
				fs = System.IO.Directory.GetFiles(keyDir, keyFileName);
				if (fs.Length > 0)
				{
					return keyDir;
				}
			}

			throw new InvalidOperationException("Unable to locate private key file directory");
		}
	}
}

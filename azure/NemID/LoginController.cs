using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;

namespace AzureNemID
{
	[Route("api/[controller]")]
	public class LoginController : Controller
	{
		readonly StringBuilder _iframeBuilder = new StringBuilder();
		readonly Dictionary<string, string> _parameters = new Dictionary<string, string>();

		struct GetResult {
			public string Script { get; set; }
		}

		[HttpGet]
		public JsonResult Get()
		{
			AddStandardParameters();
			AddParameters(_parameters);
			var scriptBlock = _iframeBuilder.ToString();
			var base64 = Convert.ToBase64String(Encoding.UTF8.GetBytes(scriptBlock));
			var result = new GetResult
			{
				Script = base64,
			};
			return new JsonResult(result);
		}

		void AddStandardParameters()
		{
			_parameters.Add("CLIENTFLOW", "Oceslogin2");
			_parameters.Add("CLIENTMODE", "LIMITED");
			_parameters.Add("SP_CERT", Globals.Base64EncodedNemIDCertificate);

			var now = DateTime.UtcNow;
			var timeZoneInfo = TimeZoneInfo.FindSystemTimeZoneById("Europe/Copenhagen");
			now = TimeZoneInfo.ConvertTimeFromUtc(now, timeZoneInfo);
			var timestamp = now.ToString(now.IsDaylightSavingTime() ? "yyyy-MM-dd HH:mm:ss+0200" : "yyyy-MM-dd HH:mm:ss+0100");
			_parameters.Add("TIMESTAMP", timestamp);

			var normalizedParameters = GetNormalizedParameters();
			var parameterSignature = CalculateSignature(Globals.NemIDCertificate, normalizedParameters);
			_parameters.Add("DIGEST_SIGNATURE", Convert.ToBase64String(parameterSignature));
			var parameterDigest = SHA256.Create().ComputeHash(normalizedParameters);
			_parameters.Add("PARAMS_DIGEST", Convert.ToBase64String(parameterDigest));
		}

		void AddParameters(Dictionary<string, string> parameters)
		{
			_iframeBuilder.Append("\r\n{\r\n");

			var strings = new List<string>();
			foreach (var parameter in parameters)
			{
				strings.Add($"\"{parameter.Key}\":\"{parameter.Value}\"");
			}
			_iframeBuilder.Append(string.Join(",", strings));
			_iframeBuilder.Append("\r\n}\r\n");
		}

		byte[] GetNormalizedParameters()
		{
			var builder = new StringBuilder();
			var sortedParameters = new SortedDictionary<string, string>(_parameters);

			foreach (var entry in sortedParameters)
			{
				builder.Append(entry.Key);
				builder.Append(entry.Value);
			}

			return Encoding.UTF8.GetBytes(builder.ToString());
		}

		byte[] CalculateSignature(X509Certificate2 certificate, byte[] data)
		{
			using (var rsa = certificate.GetRSAPrivateKey())
			{
				rsa.KeySize = 2048;
				var padding = RSASignaturePadding.Pkcs1;
				return rsa.SignData(data, HashAlgorithmName.SHA256, padding);
			}
		}
	}
}

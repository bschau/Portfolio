using System;
using System.IO;
using System.Linq;
using MimeKit;
using VPSApps.Commands;
using VPSApps.DomainModel;

namespace VPSApps.Modules
{
	class ChessMag : IModule
	{
		const string _moduleName = "Skakblad";
		const string _magazineUrl = "http://www.skak.dk/images/skakbladet/seneste.pdf";
		const string _cachedName = "skakbladet.pdf";
		readonly VpsAppsRc _vpsAppsRc;

		public ChessMag(VpsAppsRc vpsAppsRc)
		{
			_vpsAppsRc = vpsAppsRc;
		}

		public void Execute()
		{
			var data = new Fetcher().ExecuteAsync(_magazineUrl).Result;
			var cached = GetCachedMagazine();

			if (cached.SequenceEqual(data))
			{
				return;
			}

			var htmlBuilder = new HtmlBuilder(_moduleName);
			htmlBuilder.AppendFormat("Seneste <a href=\"{0}\">Skakbladet</a>.", _magazineUrl);
			var attachment = new EmailAttachment
			{
				Data = data,
				Filename = _cachedName,
				ContentType = new ContentType("application", "pdf"),
			};
			var sendEmail = new SendEmail(_vpsAppsRc.From, _vpsAppsRc.To);
			sendEmail.ExecuteWithAttachment(htmlBuilder.Title, htmlBuilder.ToString(), attachment);

			File.WriteAllBytes(_cachedName, data);
		}

		byte[] GetCachedMagazine()
		{
			if (!File.Exists(_cachedName))
			{
				return new byte[0];
			}

			return File.ReadAllBytes(_cachedName);
		}
	}
}
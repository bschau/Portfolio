using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml.Linq;
using VPSApps.Commands;
using VPSApps.DomainModel;

namespace VPSApps.Modules
{
	class Kwed : IModule
	{
		const string _moduleName = "KWED";
		const string _counterFile = "kwed-cnt";
		readonly VpsAppsRc _vpsAppsRc;
		int _counter;

		public Kwed(VpsAppsRc vpsAppsRc)
		{
			_vpsAppsRc = vpsAppsRc;
		}

		public void Execute()
		{
			const string baseDomain = "remix.kwed.org";

			LoadCounter();
			var data = new Fetcher().ExecuteAsync($"http://{baseDomain}/rss.xml").Result;
			var items = Parse(data);
			var kwedItems = items.OrderBy(x => x.KwedId);
			var htmlBuilder = new HtmlBuilder(_moduleName);
			var sendEmail = new SendEmail(_vpsAppsRc.From, _vpsAppsRc.To);
			foreach (var kwedItem in kwedItems)
			{
				var kwedId = kwedItem.KwedId;
				if (Seen(kwedId))
				{
					continue;
				}

				htmlBuilder.Reset();
				var url = $"http://{baseDomain}/download.php/{kwedId}";
				htmlBuilder.AppendFormat("<h1>{0}</h1><p><a href=\"{1}\">{1}</a></p>", kwedItem.Title, url);
				sendEmail.Execute(htmlBuilder.Title, htmlBuilder.ToString());
				_counter = kwedId;
				SaveCounter();
			}
		}

		void LoadCounter()
		{
			if (!File.Exists(_counterFile))
			{
				_counter= 0;
				return;
			}

			var line = File.ReadAllText(_counterFile);
			_counter = int.Parse(line);
		}

		void SaveCounter()
		{
			File.WriteAllText(_counterFile, _counter.ToString());
		}

		List<KwedItem> Parse(byte[] data)
		{
			using (var ms = new MemoryStream(data))
			{
				var xDocument = XDocument.Load(ms);
				return (from item
						in xDocument.Root.Descendants().First(x => "channel".Equals(x.Name.LocalName, StringComparison.CurrentCultureIgnoreCase))
							.Elements().Where(x => "item".Equals(x.Name.LocalName, StringComparison.CurrentCultureIgnoreCase))
						select new KwedItem(
							item.Elements().First(x => "link".Equals(x.Name.LocalName, StringComparison.CurrentCultureIgnoreCase)).Value,
							item.Elements().First(x => "title".Equals(x.Name.LocalName, StringComparison.CurrentCultureIgnoreCase)).Value
						)
				).ToList();
			}
		}

		bool Seen(int kwedId)
		{
			return kwedId <= _counter;
		}
	}
}
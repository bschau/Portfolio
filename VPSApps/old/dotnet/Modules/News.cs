using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml.Linq;
using VPSApps.Commands;
using VPSApps.DomainModel;

namespace VPSApps.Modules
{
	class News : IModule
	{
		const string _moduleName = "News";
		readonly Fetcher _fetcher = new Fetcher();
		readonly VpsAppsRc _vpsAppsRc;

		public News(VpsAppsRc vpsAppsRc)
		{
			_vpsAppsRc = vpsAppsRc;
		}

		public void Execute()
		{
			VerifySetup();

			var htmlBuilder = new HtmlBuilder(_moduleName);
			htmlBuilder.Append("<table style=\"width: 100%\">");
			var lineNumber = 0;
			foreach (var newsfeed in _vpsAppsRc.Newsfeeds)
			{
				try
				{
					var data = _fetcher.ExecuteAsync(newsfeed.Url).Result;
					var feedItems = Parse(newsfeed, data);
					var cache = new NewsCache(newsfeed.Name);

					foreach (var feedItem in feedItems)
					{
						if (cache.Seen(feedItem))
						{
							continue;
						}

						cache.Add(feedItem);
						var color = lineNumber % 2 == 1 ? "#efe" : "#fff";
						htmlBuilder.AppendFormat("<tr><td style=\"background-color: {0}; padding: 0.5em; font-size: 120%\">", color);
						htmlBuilder.AppendFormat("<a href=\"{0}\">({1}) {2}</a></td></tr>", feedItem.Link, feedItem.Feed.Name, feedItem.Title);
						htmlBuilder.AppendLine();
						lineNumber++;
					}

					cache.Expire();
					cache.Save();
				}
				catch (Exception exception)
				{
					Console.Error.WriteLine($"Faild to fetch '{newsfeed.Name}': {exception.Message}");
				}
			}

			if (lineNumber == 0)
			{
				return;
			}
			htmlBuilder.Append("</table>");
			var sendEmail = new SendEmail(_vpsAppsRc.From, _vpsAppsRc.To);
			sendEmail.Execute(htmlBuilder.Title, htmlBuilder.ToString());
		}

		void VerifySetup()
		{
			if (_vpsAppsRc.Newsfeeds == null || !_vpsAppsRc.Newsfeeds.Any())
			{
				throw new Exception($"No Newsfeeds defined in {Configuration.Filename}");
			}
		}

		List<NewsItem> Parse(Newsfeed newsfeed, byte[] data)
		{
			using (var ms = new MemoryStream(data))
			{
				var xDocument = XDocument.Load(ms);
				return (from item
						in xDocument.Root.Descendants().First(x => "channel".Equals(x.Name.LocalName, StringComparison.CurrentCultureIgnoreCase))
							.Elements().Where(x => "item".Equals(x.Name.LocalName, StringComparison.CurrentCultureIgnoreCase))
						select new NewsItem(
							newsfeed,
							item.Elements().First(x => "link".Equals(x.Name.LocalName, StringComparison.CurrentCultureIgnoreCase)).Value,
							item.Elements().First(x => "title".Equals(x.Name.LocalName, StringComparison.CurrentCultureIgnoreCase)).Value,
							item.Elements().First(x => "guid".Equals(x.Name.LocalName, StringComparison.CurrentCultureIgnoreCase)).Value
						)
				).ToList();
			}
		}
	}
}
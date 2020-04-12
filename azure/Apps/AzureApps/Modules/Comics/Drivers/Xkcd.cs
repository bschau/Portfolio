using System.Text;
using dk.schau.AzureApps.Commands;
using dk.schau.AzureApps.Modules.Comics.DomainModel;

namespace dk.schau.AzureApps.Modules.Comics.Drivers
{
	public class Xkcd : IComicsDriver
	{
		readonly Fetcher _fetcher;

		public Xkcd (Fetcher fetcher)
		{
			_fetcher = fetcher;
		}

		public string Execute(string url, string name)
		{
			var response = _fetcher.ExecuteAsync(url).Result;
			var html = Encoding.UTF8.GetString(response);
			var pos = html.IndexOf("<div id=\"comic\">");
			pos = html.IndexOf("<img src=\"", pos);
			var startPos = pos + 10;
			var endPos = html.IndexOf('"', startPos);
			var link = string.Format("http:{0}", html.Substring(startPos, endPos - startPos));

			pos = html.IndexOf("title=\"", endPos);
			var titleStart = pos + 7;
			var titleEnd = html.IndexOf('"', titleStart);
			var alt = html.Substring(titleStart, titleEnd - titleStart);

			return $"<p><img src=\"{link}\" alt=\"{name}\" /></p><p>{alt}</p><p><br /></p>";
		}
	}
}
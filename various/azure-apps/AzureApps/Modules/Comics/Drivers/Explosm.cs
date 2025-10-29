using System.Text;
using dk.schau.AzureApps.Commands;
using dk.schau.AzureApps.Modules.Comics.DomainModel;

namespace dk.schau.AzureApps.Modules.Comics.Drivers
{
	public class Explosm : IComicsDriver
	{
		readonly Fetcher _fetcher;

		public Explosm(Fetcher fetcher)
		{
			_fetcher = fetcher;
		}

		public string Execute(string url, string name)
		{
			var response = _fetcher.ExecuteAsync(url).Result;
			var html = Encoding.UTF8.GetString(response);
			var pos = html.IndexOf("div id=\"comic-wrap\"");
			if (pos >= 0)
			{
				pos = html.IndexOf("flex-video", pos);
				if (pos >= 0)
				{
					return $"<p>Today is a video. Click <a href=\"{url}\">here</a> to see it :-)</p>";
				}
			}

			pos = html.IndexOf("img id=\"main-comic\" src");
			var startPos = pos + 25;
			var endPos = html.IndexOf('"', startPos);
			var link = html.Substring(startPos, endPos - startPos);
			if (link.StartsWith("//"))
			{
				link = $"http:{link}";
			}

			return $"<p><img src=\"{link}\" alt=\"{name}\" width=\"900\" /></p><p><br /></p>";
		}
	}
}
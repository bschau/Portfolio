using System.Text;
using dk.schau.AzureApps.Commands;
using dk.schau.AzureApps.Modules.Comics.DomainModel;

namespace dk.schau.AzureApps.Modules.Comics.Drivers
{
	public class MonkeyUser : IComicsDriver
	{
		readonly Fetcher _fetcher;

		public MonkeyUser(Fetcher fetcher)
		{
			_fetcher = fetcher;
		}

		public string Execute(string url, string name)
		{
			var response = _fetcher.ExecuteAsync(url).Result;
			var html = Encoding.UTF8.GetString(response);
			var pos = html.IndexOf("div class=\"content\">");
			pos = html.IndexOf("src", pos);
			var startPos = pos + 5;
			var endPos = html.IndexOf('"', startPos);
			var link = html.Substring(startPos, endPos - startPos);
			return $"<p><img src=\"{link}\" alt=\"{name}\" width=\"900\" /></p><p><br /></p>";
		}
	}
}
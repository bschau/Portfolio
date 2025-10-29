using System.Text;
using dk.schau.AzureApps.Commands;
using dk.schau.AzureApps.Modules.Comics.DomainModel;

namespace dk.schau.AzureApps.Modules.Comics.Drivers
{
	public class ComicsKingdom : IComicsDriver
	{
		readonly Fetcher _fetcher;

		public ComicsKingdom(Fetcher fetcher)
		{
			_fetcher = fetcher;
		}

		public string Execute(string url, string name)
		{
			var response = _fetcher.ExecuteAsync(url).Result;
			var html = Encoding.UTF8.GetString(response);
			var pos = html.IndexOf("data-comic-image-url");
			var startPos = pos + 22;
			var endPos = html.IndexOf('"', startPos);
			var link = html.Substring(startPos, endPos - startPos);
			return $"<p><img src=\"{link}\" alt=\"{name}\" /></p><p><br /></p>";
		}
	}
}
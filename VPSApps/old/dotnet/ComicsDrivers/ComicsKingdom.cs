using System.Text;
using VPSApps.Commands;
using VPSApps.DomainModel;

namespace VPSApps.ComicsDrivers
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
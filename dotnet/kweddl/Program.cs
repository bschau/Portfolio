using System;
using System.IO;
using System.Linq;

namespace kweddl
{
	class Program
	{
		static string _hiddenPrefix;

		static void Main(string[] args)
		{
			try
			{
				const string baseDomain = "remix.kwed.org";
				var documentsFolder = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
				var desktopFolder = Environment.GetFolderPath(Environment.SpecialFolder.Desktop);
				Initialize();
				var saveMusic = new SaveMusic(desktopFolder);

				var counterFile = Path.Combine(documentsFolder, $"{_hiddenPrefix}kwedrc");
				var counter = new Counter(counterFile);
				counter.Load();

				var fetch = new Fetch();
				var rssData = fetch.ExecuteAsync($"http://{baseDomain}/rss.xml").Result;
				var items = new Rss().Parse(rssData);
				var kwedItems = items.OrderBy(x => x.KwedId);

				foreach (var kwedItem in kwedItems)
				{
					var kwedId = kwedItem.KwedId;
					if (counter.Seen(kwedId))
					{
						Console.WriteLine("Song already downladed: {0}", kwedId);
						continue;
					}

					var url = $"http://{baseDomain}/download.php/{kwedId}";
					Console.WriteLine("Fetching: {0}", url);
					var music = fetch.ExecuteAsync(url).Result;
					saveMusic.Execute(kwedItem, music);
					counter.Set(kwedId);
					counter.Save();
				}
			}
			catch (Exception exception)
			{
				Console.Error.WriteLine("{0}", exception.Message);
			}
		}

		static void Initialize()
		{
			var isWindows = System.Runtime.InteropServices.RuntimeInformation.IsOSPlatform(System.Runtime.InteropServices.OSPlatform.Windows);
			if (isWindows)
			{
				_hiddenPrefix = "_";
			}
			else
			{
				_hiddenPrefix = ".";
			}
		}
	}
}

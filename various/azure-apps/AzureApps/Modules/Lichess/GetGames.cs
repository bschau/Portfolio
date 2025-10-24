using System.Text;
using dk.schau.AzureApps.Commands;
using dk.schau.AzureApps.Modules.Lichess.DomainModel;
using Newtonsoft.Json;

namespace dk.schau.AzureApps.Modules.Lichess
{
	internal class GetGames
	{
		internal LichessResponse Execute(string token)
		{
			var fetcher = new Fetcher
			{
				Authorization = token,
			};
			var data = fetcher.ExecuteAsync("https://lichess.org/api/account/playing").Result;
			var json = Encoding.UTF8.GetString(data);
			return JsonConvert.DeserializeObject<LichessResponse>(json);
		}
	}
}
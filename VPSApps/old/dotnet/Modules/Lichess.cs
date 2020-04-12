using System;
using System.Text;
using Newtonsoft.Json;
using VPSApps.Commands;
using VPSApps.DomainModel;

namespace VPSApps.Modules
{
	internal class Lichess : IModule
	{
		const string _moduleName = "Lichess";
		const string _formatter = "<li>My turn in game vs. <i>{0}</i>: {1}{2}</li>";
		const string _linkformatter = " (<a href=\"https://lichess.org/{0}\">Link</a>)";
		readonly VpsAppsRc _vpsAppsRc;

		public Lichess(VpsAppsRc vpsAppsRc)
		{
			_vpsAppsRc = vpsAppsRc;
		}

		public void Execute()
		{
			VerifySetup();

			var fetcher = new Fetcher
			{
				Authorization = _vpsAppsRc.LichessToken,
			};
			var data = fetcher.ExecuteAsync("https://lichess.org/api/account/playing").Result;
			var json = Encoding.UTF8.GetString(data);
			var games = JsonConvert.DeserializeObject<LichessResponse>(json);

			if (games.NowPlaying.Count < 1)
			{
				return;
			}

			var htmlBuilder = new HtmlBuilder(_moduleName);
			foreach (var game in games.NowPlaying)
			{
				var myTurn = game.IsMyTurn ? "<b>YES</b>" : "no";
				var link = game.IsMyTurn ? string.Format(_linkformatter, game.FullId) : "";
				htmlBuilder.AppendFormat(_formatter, game.Opponent.Username, myTurn, link);
			}

			var sendEmail = new SendEmail(_vpsAppsRc.From, _vpsAppsRc.To);
			sendEmail.Execute(htmlBuilder.Title, htmlBuilder.ToString());
		}

		void VerifySetup()
		{
			if (string.IsNullOrWhiteSpace(_vpsAppsRc.LichessToken))
			{
				throw new Exception($"LichessToken not found in {Configuration.Filename}");
			}
		}
	}
}
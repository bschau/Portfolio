using System.Collections.Generic;
using System.Linq;
using dk.schau.AzureApps.Commands;
using dk.schau.AzureApps.DomainModel;
using dk.schau.AzureApps.Modules.Lichess.DomainModel;

namespace dk.schau.AzureApps.Modules.Lichess
{
	public class Lichess
	{
		readonly AppsRc _appsRc;

		public Lichess(AppsRc appsRc)
		{
			_appsRc = appsRc;
		}

		public void Execute()
		{
			new VerifySetup(_appsRc).Execute();

			var games = new GetGames().Execute(_appsRc.LichessToken);
			if (!games.NowPlaying.Any())
			{
				return;
			}

			var html = new HtmlBuilder("Lichess");
			CreateMailContent(html, games.NowPlaying);

			new SendEmail(_appsRc.MailSettings).Execute(html.Title, html.ToString());
		}

		void CreateMailContent(HtmlBuilder htmlBuilder, List<LichessGame> games)
		{
			const string formatter = "<li>My turn in game vs. <i>{0}</i>: {1}{2}</li>";
			const string linkformatter = " (<a href=\"https://lichess.org/{0}\">Link</a>)";

			foreach (var game in games)
			{
				var myTurn = game.IsMyTurn ? "<b>YES</b>" : "no";
				var link = game.IsMyTurn ? string.Format(linkformatter, game.FullId) : "";
				htmlBuilder.AppendFormat(formatter, game.Opponent.Username, myTurn, link);
			}
		}
	}
}
using System;
using dk.schau.AzureApps.Commands;
using dk.schau.AzureApps.DomainModel;

namespace dk.schau.AzureApps.Modules.ChessMag
{
	public class ChessMag
	{
		readonly AppsRc _appsRc;

		public ChessMag(AppsRc appsRc)
		{
			_appsRc = appsRc;
		}

		public void Execute()
		{
			const string magazineUrl = "http://www.skak.dk/images/skakbladet/seneste.pdf";
			var responseHeaders = new Head
			{
				IfModifiedSince = DateTime.Now.AddDays(-1)
			}.Execute(magazineUrl).Result;

			if ("notmodified".Equals(responseHeaders["_StatusCode"], StringComparison.CurrentCultureIgnoreCase))
			{
				return;
			}

			var htmlBuilder = new HtmlBuilder("ChessMag");
			htmlBuilder.Append("<p>Skakbladet er udkommet!</p>");
			htmlBuilder.AppendFormat("<p>Download: <a href=\"{0}\">{0}</a></p>", magazineUrl);

			new SendEmail(_appsRc.MailSettings).Execute(htmlBuilder.Title, htmlBuilder.ToString());
		}
	}
}
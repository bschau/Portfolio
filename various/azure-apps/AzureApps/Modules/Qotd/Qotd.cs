using dk.schau.AzureApps.Commands;
using dk.schau.AzureApps.DomainModel;

namespace dk.schau.AzureApps.Modules.Qotd
{
	public class Qotd
	{
		readonly AppsRc _appsRc;

		public Qotd(AppsRc appsRc)
		{
			_appsRc = appsRc;
		}

		public void Execute()
		{
			new VerifySetup(_appsRc).Execute();

			var rawQotd = new GetRawQotd().Execute(_appsRc.QotdFileUrl);
			var html = new HtmlBuilder("Qotd");
			new ParseQotd().Execute(html, rawQotd);
			new SendEmail(_appsRc.MailSettings).Execute(html.Title, html.ToString());
		}
	}
}
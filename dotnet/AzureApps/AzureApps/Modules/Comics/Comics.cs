using dk.schau.AzureApps.Commands;
using dk.schau.AzureApps.DomainModel;

namespace dk.schau.AzureApps.Modules.Comics
{
	public class Comics
	{
		readonly AppsRc _appsRc;

		public Comics(AppsRc appsRc)
		{
			_appsRc = appsRc;
		}

		public void Execute()
		{
			var htmlBuilder = new HtmlBuilder("Comics");

			var getComicsDriver = new GetComicsDriver();
			foreach (var comicsItem in _appsRc.Comics)
			{
				var comicsDriver = getComicsDriver.Execute(comicsItem.Driver);

				htmlBuilder.AppendFormat("<h1><a href=\"{0}\">{1}</a></h1>", comicsItem.Url, comicsItem.Name);
				htmlBuilder.Append(comicsDriver.Execute(comicsItem.Url, comicsItem.Name));
				htmlBuilder.AppendLine();
			}

			new SendEmail(_appsRc.MailSettings).Execute(htmlBuilder.Title, htmlBuilder.ToString());
		}
	}
}
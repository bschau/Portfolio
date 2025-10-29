using System.Collections.Generic;
using System.Linq;
using dk.schau.AzureApps.Commands;
using dk.schau.AzureApps.DomainModel;
using dk.schau.AzureApps.Modules.Eventor.DomainModel;

namespace dk.schau.AzureApps.Modules.Eventor
{
	public class Eventor
	{
		readonly AppsRc _appsRc;

		public Eventor(AppsRc appsRc)
		{
			_appsRc = appsRc;
		}

		public void Execute()
		{
			var times = new Times();
			var allEvents = new GetAllEvents().Execute(_appsRc.EventorFileUrl);
			var lookAhead = _appsRc.EventorLookAhead < 1 ? 7 : _appsRc.EventorLookAhead;
			var events = new Filter(times).Execute(allEvents, lookAhead);
			if (!events.Any())
			{
				return;
			}
			var htmlBuilder = new HtmlBuilder("Eventor");
			CreateMailContent(htmlBuilder, events);

			new SendEmail(_appsRc.MailSettings).Execute(htmlBuilder.Title, htmlBuilder.ToString());
		}

		void CreateMailContent(HtmlBuilder html, List<UpcomingEvent> events)
		{
			const string formatter = "<p>{0} {1}</p>";
			foreach (var upcomingEvent in events)
			{
				html.AppendFormat(formatter,
									upcomingEvent.Glyph,
									upcomingEvent.Text);
			}
		}
	}
}
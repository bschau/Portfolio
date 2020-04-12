using System.Collections.Generic;

namespace VPSApps.DomainModel
{
	public class VpsAppsRc
	{
		public string WorkFolder { get; set; }
		public EmailAddress From { get; set; }
		public EmailAddress To { get; set; }
		public string QotdFilePath { get; set; }
		public string EventsFilePath { get; set; }
		public int EventsLookAhead { get; set; }
		public string LichessToken { get; set; }
		public List<ComicsItem> Comics { get; set; }
		public List<Newsfeed> Newsfeeds { get; set; }
	}
}
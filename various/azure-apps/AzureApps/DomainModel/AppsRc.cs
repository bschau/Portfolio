using System.Collections.Generic;

namespace dk.schau.AzureApps.DomainModel
{
	public class AppsRc
	{
		public MailSettings MailSettings { get; set; }
		public string QotdFileUrl { get; set; }
		public string EventorFileUrl { get; set; }
		public int EventorLookAhead { get; set; }
		public string LichessToken { get; set; }
		public List<ComicsItem> Comics { get; set; }
		public List<Newsfeed> Newsfeeds { get; set; }
	}
}
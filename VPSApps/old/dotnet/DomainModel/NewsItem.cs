using System;

namespace VPSApps.DomainModel
{
	public class NewsItem
	{
		public NewsItem(Newsfeed feed, string link, string title, string guid)
		{
			Feed = feed;
			Link = link;
			Title = title;
			Guid = guid;
		}

		public Newsfeed Feed { get; private set; }
		public string Link { get; private set; }
		public string Title { get; private set; }
		public string Guid { get; private set; }
	}
}
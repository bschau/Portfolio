using System;
using System.Linq;

namespace kweddl
{
	public class KwedItem
	{
		public KwedItem(string link, string title)
		{
			const string prefix = "New C64 remix released: ";
			Link = link;
			Title = title.StartsWith(prefix, StringComparison.CurrentCultureIgnoreCase)
					? title.Substring(prefix.Length)
					: title;

			KwedId = int.Parse(link.Split('/').Last());
		}

		public string Link { get; private set; }
		public string Title { get; private set; }
		public int KwedId { get; private set; }
	}
}

using System;

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

			var parts = link.Split(new [] { '=' });
			var kwedId = 0;
			if (!int.TryParse(parts[1], out kwedId))
			{
				throw new Exception($"Failed to get kwedId out of {Link}");
			}

			KwedId = kwedId;
		}

		public string Link { get; private set; }
		public string Title { get; private set; }
		public int KwedId { get; private set; }
	}
}

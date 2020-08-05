using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml.Linq;

namespace kweddl
{
	class Rss
	{
		public List<KwedItem> Parse(byte[] data)
		{
			using (var ms = new MemoryStream(data))
			{
				var xDocument = XDocument.Load(ms);
				return (from item
						in xDocument.Root.Descendants()
							.First(x => "channel".Equals(x.Name.LocalName, StringComparison.CurrentCultureIgnoreCase))
							.Elements()
								.Where(x => "item".Equals(x.Name.LocalName, StringComparison.CurrentCultureIgnoreCase))
						select new KwedItem(
							item.Elements()
								.First(x => "link".Equals(x.Name.LocalName, StringComparison.CurrentCultureIgnoreCase)).Value,
							item.Elements()
								.First(x => "title".Equals(x.Name.LocalName, StringComparison.CurrentCultureIgnoreCase)).Value
						)
				).ToList();
			}
		}
	}
}

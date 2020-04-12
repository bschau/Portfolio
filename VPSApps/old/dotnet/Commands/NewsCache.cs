using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using VPSApps.DomainModel;

namespace VPSApps.Commands
{
	public class NewsCache
	{
		readonly List<CachedNewsItem> _items = new List<CachedNewsItem>();
		readonly string _filename;

		public NewsCache(string name)
		{
			_filename = string.Format("{0}.cache", Convert.ToBase64String(Encoding.UTF8.GetBytes(name)));
			Load();
		}

		public void Load()
		{
			_items.Clear();
			if (!File.Exists(_filename))
			{
				return;
			}

			var json = File.ReadAllText(_filename);
			_items.AddRange(JsonConvert.DeserializeObject<List<CachedNewsItem>>(json));
		}

		public void Save()
		{
			File.WriteAllText(_filename, JsonConvert.SerializeObject(_items));
		}

		public bool Seen(NewsItem newsItem)
		{
			return _items.Any(x => x.Guid.Equals(newsItem.Guid, StringComparison.CurrentCultureIgnoreCase));
		}

		public void Add(NewsItem newsItem)
		{
			var expires = DateTime.Now.AddDays(newsItem.Feed.Dtl).Ticks;
			_items.Add(new CachedNewsItem
			{
				Guid = newsItem.Guid,
				Expires = expires,
			});
		}

		public void Expire()
		{
			var now = DateTime.Now.Ticks;
			var items = _items.Where(x => x.Expires <= now);
			foreach (var item in items)
			{
				_items.Remove(item);
			}
		}
	}
}
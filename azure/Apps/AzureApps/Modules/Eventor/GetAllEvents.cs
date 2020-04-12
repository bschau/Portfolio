using System;
using System.Collections.Generic;
using System.Text;
using dk.schau.AzureApps.Commands;
using dk.schau.AzureApps.Modules.Eventor.DomainModel;

namespace dk.schau.AzureApps.Modules.Eventor
{
	internal class GetAllEvents
	{
		internal List<EventorEvent> Execute(string url)
		{
			var fetcher = new Fetcher();
			var data = fetcher.ExecuteAsync(url).Result;
			var events = Encoding.UTF8.GetString(data);

			var result = new List<EventorEvent>();
			foreach (var line in events.Split(
							    new[] { "\r\n", "\r", "\n" },
								StringSplitOptions.None)) {
				if (string.IsNullOrWhiteSpace(line))
				{
					continue;
				}
				result.Add(new EventorEvent(line.Trim()));
			}

			return result;
		}
	}
}
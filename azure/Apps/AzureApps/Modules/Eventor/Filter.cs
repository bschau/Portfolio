using System;
using System.Collections.Generic;
using System.Linq;
using dk.schau.AzureApps.Modules.Eventor.DomainModel;

namespace dk.schau.AzureApps.Modules.Eventor
{
	internal class Filter
	{
		internal Times _times;

		internal Filter(Times times)
		{
			_times = times;
		}

		internal List<UpcomingEvent> Execute(List<EventorEvent> allEvents, int lookAhead)
		{
			var result = new List<UpcomingEvent>();
			var getDistance = new GetDistance(_times, lookAhead);
			var eventorEventToUpcomingEvent = new EventorEventToUpcomingEvent(_times);

			foreach (var eventorEvent in allEvents)
			{
				var eventDate = new DateTime(_times.Year, eventorEvent.Month, eventorEvent.Day);
				var distance = getDistance.Execute(eventDate);
				if (distance < 0)
				{
					continue;
				}

				result.Add(eventorEventToUpcomingEvent.Execute(eventorEvent, distance));
			}
			return result.OrderBy(x => x.Distance).ToList();
		}
	}
}
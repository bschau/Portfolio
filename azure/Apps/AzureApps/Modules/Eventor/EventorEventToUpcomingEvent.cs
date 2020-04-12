using dk.schau.AzureApps.Modules.Eventor.DomainModel;

namespace dk.schau.AzureApps.Modules.Eventor
{
	internal class EventorEventToUpcomingEvent
	{
		readonly Times _times;

		internal EventorEventToUpcomingEvent(Times times)
		{
			_times = times;
		}

		internal UpcomingEvent Execute(EventorEvent eventorEvent, int distance)
		{
			var age = _times.Year - eventorEvent.Year;
			var text = eventorEvent.Occasion == EventorOccasionType.Birthday
						? $"F&oslash;dselsdag: {eventorEvent.Text} ({age} &aring;r)"
						: $"D&oslash;dsdag: {eventorEvent.Text} ({age} &aring;r siden)";

			if (distance == 0)
			{
				text = $"<b>{text} i dag!</b>";
			}
			else
			{
				var plural = distance == 1 ? "" : "e";
				text = $"{text} om {distance} dag{plural}";
			}

			return new UpcomingEvent
			{
				Glyph = eventorEvent.Occasion == EventorOccasionType.Birthday
						? "&#127874;"
						: "&#9760;",
				Text = text,
				Distance = distance,
			};
		}
	}
}
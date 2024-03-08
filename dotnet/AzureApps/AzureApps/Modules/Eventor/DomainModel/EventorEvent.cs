using System;

namespace dk.schau.AzureApps.Modules.Eventor.DomainModel
{
	public class EventorEvent
	{
		public EventorEvent(string src)
		{
			var parts = src.Split(',', StringSplitOptions.RemoveEmptyEntries);
			if (parts.Length != 5)
			{
				throw new Exception($"Malformed event: {src}");
			}

			Year = int.Parse(parts[0]);
			Month = int.Parse(parts[1]);
			Day = int.Parse(parts[2]);
			Occasion = "f".Equals(parts[3], StringComparison.CurrentCultureIgnoreCase)
						? EventorOccasionType.Birthday
						: EventorOccasionType.Death;
			Text = parts[4];
		}

		public int Year { get; private set; }
		public int Month { get; private set; }
		public int Day { get; private set; }
		public EventorOccasionType Occasion { get; private set; }
		public string Text { get; private set; }
	}
}
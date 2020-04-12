using System;

namespace dk.schau.AzureApps.Modules.Eventor.DomainModel
{
	internal class Times
	{
		internal Times()
		{
			var now = DateTime.Now;
			Year = now.Year;
			DayOfYear = now.DayOfYear;
			DaysInYear = new DateTime(Year, 12, 31).DayOfYear;
		}

		internal int Year { get; private set; }
		internal int DayOfYear { get; private set; }
		internal int DaysInYear { get; private set; }
	}
}
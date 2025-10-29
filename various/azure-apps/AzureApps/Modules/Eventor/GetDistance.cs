using System;
using dk.schau.AzureApps.Modules.Eventor.DomainModel;

namespace dk.schau.AzureApps.Modules.Eventor
{
	internal class GetDistance
	{
		readonly Times _times;
		readonly int _lookAhead;

		internal GetDistance(Times times, int lookAhead)
		{
			_times = times;
			_lookAhead = lookAhead;
		}

		internal int Execute(DateTime eventDate)
		{
			var dayOfYear = eventDate.DayOfYear;

			if (dayOfYear < _times.DayOfYear)
			{
				if ((_times.DayOfYear + _lookAhead) < dayOfYear + _times.DaysInYear)
				{
					return -1;
				}

				return dayOfYear + _times.DaysInYear - _times.DayOfYear;
			}

			if (_times.DayOfYear + _lookAhead < dayOfYear)
			{
				return -1;
			}

			return dayOfYear - _times.DayOfYear;
		}
	}
}
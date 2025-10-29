using System;

namespace dk.schau.AzureApps.Translations
{
	public class UtcNowToNow
	{
		readonly TimeZoneInfo _zoneInfo;

		public UtcNowToNow()
		{
			_zoneInfo = TimeZoneInfo.FindSystemTimeZoneById(GetZoneName());
		}

		string GetZoneName()
		{
			return Environment.OSVersion.Platform == PlatformID.Unix
					? "Europe/Copenhagen"
					: "Romance Standard Time";
		}

		public DateTime Execute()
		{
			var utc = DateTime.Now.ToUniversalTime();
			// See: https://docs.microsoft.com/en-us/previous-versions/windows/embedded/gg154758(v=winembedded.80)
			return TimeZoneInfo.ConvertTimeFromUtc(utc, _zoneInfo);
		}

		public string ExecuteAsString()
		{
			return Execute().ToString("yyyy-MM-dd HH:mm");
		}
	}
}
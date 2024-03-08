using System;
using Core;

namespace Nant.Builders
{
	public class SettingsBuilder
	{
		public Settings Build()
		{
			var settings = new Settings();
			settings.Read();
			if (!settings.Verify())
			{
				throw new Exception("DeveloperTools not configured. Please run the Configuration utility.");
			}
			return settings;
		}
	}
}

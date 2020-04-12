using System;
using System.IO;
using Newtonsoft.Json;
using VPSApps.DomainModel;

namespace VPSApps
{
	class Configuration
	{
		public static VpsAppsRc VpsAppsRc { get; private set; }
		public static string Filename { get; private set; }
		public static bool IsUnix { get; private set; }
		public static string Home { get; private set; }

		static internal VpsAppsRc Load()
		{
			Filename = GetFilePath();
			var content = LoadConfigFile();
			VpsAppsRc = JsonConvert.DeserializeObject<VpsAppsRc>(content);
			return VpsAppsRc;
		}

		static string GetFilePath()
		{
			var path = Environment.GetEnvironmentVariable("VPSAPPSRC");
			if (!string.IsNullOrWhiteSpace(path))
			{
				return path;
			}

			IsUnix = Environment.OSVersion.Platform == PlatformID.Unix;
			Home = IsUnix ?
				Environment.GetEnvironmentVariable("HOME") :
				Environment.ExpandEnvironmentVariables( "%HOMEDRIVE%%HOMEPATH%");
 
			return Path.Combine(Home, IsUnix ? ".vpsappsrc"  : "_vpsappsrc");
		}

		static string LoadConfigFile()
		{
			if (!File.Exists(Filename))
			{
				throw new Exception($"File not found: {Filename}");
			}

			return File.ReadAllText(Filename);
		}
	}
}
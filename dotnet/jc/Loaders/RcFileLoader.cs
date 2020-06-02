using System;
using System.IO;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace jc.Loaders
{
	class RcFileLoader
	{
		readonly Log _log;

		internal RcFileLoader(Log log)
		{
			_log = log;
		}

		internal string Execute(string configFilePath)
		{
			var path = ResolvePath(configFilePath);
			if (!File.Exists(path))
			{
				throw new ArgumentException($"Cannot read configuration file: {path}");
			}

			return File.ReadAllText(path);
		}

		string ResolvePath(string configFilePath)
		{
			if (!string.IsNullOrWhiteSpace(configFilePath))
			{
				_log.Verbose("Using ConfigFilePath: {0}", configFilePath);
				return configFilePath;
			}

			var path = Environment.GetEnvironmentVariable("JCRC");
			if (!string.IsNullOrWhiteSpace(path))
			{
				_log.Verbose("Using JCRC: {0}", path);
				return path;
			}

			var isUnix = Environment.OSVersion.Platform == PlatformID.Unix;
			var home = isUnix ?
					Environment.GetEnvironmentVariable("HOME") :
					Environment.ExpandEnvironmentVariables("%HOMEDRIVE%%HOMEPATH%");

			path = string.Format("{0}/{1}", home, isUnix ? ".jcrc" : "_jcrc");
			_log.Verbose("Using rc-file from HOME: {0}", path);
			return path;
		}
	}
}
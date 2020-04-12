using System;
using System.IO;
using System.Linq;
using System.Reflection;
using VPSApps.DomainModel;

namespace VPSApps
{
	class Program
	{
		static void Main(string[] args)
		{
			if (args.Length < 1)
			{
				Usage(1);
			}

			if ("help".Equals(args[0], StringComparison.CurrentCultureIgnoreCase))
			{
				Usage(0);
			}

			try
			{
				var vpsAppsRc = Configuration.Load();
				var module = GetModule(args[0], vpsAppsRc);

				GoToWorkFolder(vpsAppsRc);
				module.Execute();
			}
			catch (Exception exception)
			{
				Console.Error.WriteLine($"{exception.Message}");
			}
		}

		static void Usage(int errorCode)
		{
			var f = errorCode != 0 ? Console.Error : Console.Out;

			f.WriteLine("VPSApps v{0}", Assembly.GetExecutingAssembly().GetName().Version);
			f.WriteLine("Usage: dotnet.exe VPSApps.dll MODULE");
			f.WriteLine("       VPSApps[.exe] MODULE");
			f.WriteLine();
			f.WriteLine("MODULE:");
			f.WriteLine("  chessmag       Get latest skakbladet.pdf");
			f.WriteLine("  comics         Comics");
			f.WriteLine("  eventor        List of future recurring events");
			f.WriteLine("  kwed           Get latest tracks from remix.kwed.org");
			f.WriteLine("  lichess        Status of LiChess.org games");
			f.WriteLine("  news           Get latest news");
			f.WriteLine("  qotd           Quote of the day");
			Environment.Exit(errorCode);
		}

		static IModule GetModule(string moduleName, VpsAppsRc vpsAppsRc)
		{
			var type = typeof(IModule);
			var types = Assembly.GetExecutingAssembly()
						.GetTypes()
						.Where(x => type.IsAssignableFrom(x) &&
								x.Name != "IModule");

			var modules = types.Where(x =>
										moduleName.Equals(x.Name, StringComparison.CurrentCultureIgnoreCase))
								.ToList();

			if (modules.Count > 1)
			{
				throw new Exception($"Module '{moduleName}' defined twice - flog the programmer!");
			}

			if (modules.Count < 1)
			{
				Console.Error.WriteLine($"Module '{moduleName}' not found");
				Usage(1);
			}

			var iModule = Activator.CreateInstance(modules.First(), new object[] {
				vpsAppsRc,
			}) as IModule;
			if (iModule == null)
			{
				Console.Error.WriteLine($"Cannot activate '{moduleName}'");
				Environment.Exit(1);
			}

			return iModule;
		}

		static void GoToWorkFolder(VpsAppsRc vpsAppsRc)
		{
			var workFolder = GetWorkFolder(vpsAppsRc);
			if (!Directory.Exists(workFolder))
			{
				throw new Exception($"No such directory: {workFolder}");
			}

			Directory.SetCurrentDirectory(workFolder);
		}

		static string GetWorkFolder(VpsAppsRc vpsAppsRc)
		{
			if (!string.IsNullOrWhiteSpace(vpsAppsRc.WorkFolder))
			{
				return vpsAppsRc.WorkFolder;
			}

			return Path.Combine(Configuration.Home, Configuration.IsUnix
					? ".vpsapps"
					: "_vpsapps");
		}
	}
}

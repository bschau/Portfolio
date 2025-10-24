using System;
using System.Reflection;

namespace certview
{
	class Program
	{
		static string Version
		{
			get
			{
				return string.Format("V{0}", Assembly.GetExecutingAssembly().GetName().Version);
			}
		}

		static void Main(string[] args)
		{
			if (args.Length == 0)
			{
				Usage(0);
			}

			if (args.Length == 1 && args[0] == "-v")
			{
				Console.WriteLine("certview.exe {0}", Version);
				Environment.Exit(0);
			}

			var enableVerify = true;
			var index = 0;
			if (args[index] == "-n")
			{
				enableVerify = false;
				index++;
			}

			if (index >= args.Length)
			{
				Usage(1);
			}

			new Viewer(enableVerify).Execute(args[index]);
			Environment.Exit(0);
		}

		static void Usage(int exitCode)
		{
			var writer = exitCode == 0 ? Console.Out : Console.Error;
			writer.WriteLine("certview.exe {0}", Version);
			writer.WriteLine("Usage:");
			writer.WriteLine("		certview.exe -v");
			writer.WriteLine("			or");
			writer.WriteLine("certview.exe [-n] search-term");
			Environment.Exit(exitCode);
		}
	}
}

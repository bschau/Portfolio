using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;

namespace Cli
{
	public class CommandRunner
	{
		public int ExitCode { get; private set; }
		public List<string> Output { get; private set; }

		public void Execute(string program, string arguments)
		{
			Output = new List<string>();

			var process = new Process
			{
				StartInfo = new ProcessStartInfo
				{
					LoadUserProfile = true,
					FileName = program,
					Arguments = arguments,
					UseShellExecute = false,
					RedirectStandardOutput = true,
					RedirectStandardError = true,
					CreateNoWindow = true,
					WorkingDirectory = Directory.GetCurrentDirectory(),
				},
			};
 
			Console.ForegroundColor = ConsoleColor.Yellow;
			Console.WriteLine("Running {0} {1} in {2}", program, arguments, Directory.GetCurrentDirectory());

			Console.ResetColor();
			process.Start();

			AddToOutput(process.StandardOutput, Console.Out);

			Console.ForegroundColor = ConsoleColor.Red;
			AddToOutput(process.StandardError, Console.Error);
			process.WaitForExit();

			Console.ResetColor();

			ExitCode = process.ExitCode;
		}

		private void AddToOutput(StreamReader stream, TextWriter console)
		{
			while (!stream.EndOfStream)
			{
				var line = stream.ReadLine();
				Output.Add(line);
				console.WriteLine("{0}", line);
			}
		}
	}
}

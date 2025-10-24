using System;
using System.Collections.Generic;
using System.IO;
using Core.Cli;
using Nant.Builders;
using NAnt.Core;
using NAnt.Core.Attributes;

namespace Nant
{
	[TaskName("git")]
	public class Git : Task
	{
		[TaskAttribute("arguments", Required = true)]
		[StringValidator(AllowEmpty = false)]
		public string Arguments { get; set; }

		[TaskAttribute("mayfail", Required = false)]
		[BooleanValidator()]
		public bool MayFail { get; set; }

		[TaskAttribute("output", Required = false)]
		[StringValidator(AllowEmpty = true)]
		public string Output { get; set; }

		[TaskAttribute("workingdir", Required = true)]
		[StringValidator(AllowEmpty = false)]
		public string WorkingDir { get; set; }

		protected override void ExecuteTask()
		{
			var currentDirectory = Directory.GetCurrentDirectory();

			try
			{
				Directory.SetCurrentDirectory(WorkingDir);

				var settings = new SettingsBuilder().Build();
				var commandRunner = new CommandRunner();
				RunCommand(commandRunner, settings.GitCommand);
			}
			catch (Exception exception)
			{
				throw new BuildException(exception.Message, Location);
			}
			finally
			{
				Directory.SetCurrentDirectory(currentDirectory);
			}
		}

		private void RunCommand(CommandRunner commandRunner, string gitCommand)
		{
			var outputToFile = string.IsNullOrEmpty(Output) ? null : Output;
			commandRunner.Execute(gitCommand, Arguments, outputToFile);

			ShowOutput(commandRunner.StandardOutput, "    ");
			ShowOutput(commandRunner.StandardError, "(EE)");

			if (commandRunner.ExitCode != 0)
			{
				if (!MayFail)
				{
					throw new BuildException("Git command failed - see the output above for details.", Location);
				}
			}
		}

		private void ShowOutput(List<string> lines, string prefix)
		{
			foreach (var line in lines)
			{
				Log(Level.Info, prefix + "  " + line);
			}
		}
	}
}

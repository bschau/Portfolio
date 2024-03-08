using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Core.Cli;
using Nant.Builders;
using NAnt.Core;
using NAnt.Core.Attributes;

namespace Nant
{
	[TaskName("msbuild")]
	public class MsBuild : Task
	{
		private string _defaultConfiguration = "Release";

		[TaskAttribute("arguments", Required = false)]
		[StringValidator(AllowEmpty = true)]
		public string Arguments { get; set; }

		[TaskAttribute("configuration", Required = false)]
		[StringValidator(AllowEmpty = true)]
		public string Configuration { get { return _defaultConfiguration; } set { _defaultConfiguration = value; } }

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
				RunCommand(commandRunner, settings.MsBuildCommand);
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

		private void RunCommand(CommandRunner commandRunner, string msBuildCommand)
		{
			var arguments = BuildArguments();
			commandRunner.Execute(msBuildCommand, arguments);

			ShowOutput(commandRunner.StandardOutput, "    ");
			ShowOutput(commandRunner.StandardError, "(EE)");
		}

		private string BuildArguments()
		{
			var arguments = new StringBuilder();
			arguments.Append("/p:Configuration=");
			arguments.Append(Configuration);
			if (string.IsNullOrWhiteSpace(Arguments))
			{
				return arguments.ToString();
			}

			arguments.Append(" ");
			arguments.Append(Arguments);
			return arguments.ToString();
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

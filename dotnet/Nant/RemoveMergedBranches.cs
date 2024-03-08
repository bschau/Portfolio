using System;
using System.IO;
using Core.Cli;
using Nant.Builders;
using Nant.Commands;
using Nant.Queries;
using NAnt.Core;
using NAnt.Core.Attributes;

namespace Nant
{
	[TaskName("removemergedbranches")]
	public class RemoveMergedBranches: Task
	{
		private string _destination = "local";
		[TaskAttribute("destination", Required = false)]
		[StringValidator(AllowEmpty = true)]
		public string Destination { get { return _destination.ToLower(); } set { _destination = value; } }

		private string _prefix = "card-";
		[TaskAttribute("prefix", Required = false)]
		[StringValidator(AllowEmpty = true)]
		public string Prefix { get { return _prefix; } set { _prefix = value; } }

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
				var gitRunner = new GitRunner(new CommandRunner(), settings.GitCommand);
				RemoveMerged(gitRunner);
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

		private void RemoveMerged(GitRunner gitRunner)
		{
			var mergedCards = new GetMergedCards(gitRunner, Prefix).Execute(Destination);
			var removeBranch = GetRemoveBranchCommand(gitRunner);

			foreach (var mergedCard in mergedCards)
			{
				try
				{
					Log(Level.Info, "Removing " + Destination + " branch: " + mergedCard);
					removeBranch.Execute(mergedCard);
				}
				catch (Exception exception)
				{
					Log(Level.Error, exception.Message, Location);
				}
			}
		}

		private IRemoveBranch GetRemoveBranchCommand(GitRunner gitRunner)
		{
			if (Destination == "local")
			{
				return new RemoveLocalBranch(gitRunner);
			}
	
			return new RemoveRemoteBranch(gitRunner);
		}
	}
}

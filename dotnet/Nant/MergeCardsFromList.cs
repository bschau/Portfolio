using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Core.Cli;
using Core.DomainModel;
using Nant.Builders;
using Nant.Queries;
using NAnt.Core;
using NAnt.Core.Attributes;

namespace Nant
{
	[TaskName("mergecardsfromlist")]
	public class MergeCardsFromList : Task
	{
		[TaskAttribute("board", Required = true)]
		[StringValidator(AllowEmpty = false)]
		public string Board { get; set; }

		[TaskAttribute("list", Required = true)]
		[StringValidator(AllowEmpty = false)]
		public string List { get; set; }

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
				var service = new TrelloServiceBuilder(settings).Build();
				var board = new GetBoardByName(service).Execute(settings.Username, Board);
				var cards = new GetCards(service).Execute(board, List);
				var gitRunner = new GitRunner(new CommandRunner(), settings.GitCommand);

				RunMerge(gitRunner, cards);
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

		private void RunMerge(GitRunner gitRunner, List<Card> cards)
		{
			var tempFile = string.Empty;

			try
			{
				tempFile = Path.GetTempFileName();
				if (!gitRunner.GetRemoteCards(tempFile))
				{
					throw new Exception("Failed to retrieve list of remote branches");
				}
				var fileInfo = new FileInfo(tempFile);
				if (fileInfo.Length == 0)
				{
					Log(Level.Info, "No commits.");
					return;
				}

				var lines = File.ReadAllLines(tempFile);
				var allBranches = new List<string>();
				var branchMap = new Dictionary<string, string>();

				foreach (var line in lines)
				{
					var tokens = line.Split(new [] { '/' }, StringSplitOptions.RemoveEmptyEntries);
					if (!tokens.Any())
					{
						continue;
					}

					var cardName = tokens.Last();
					var card = cards.SingleOrDefault(x => x.CardName == cardName);
					if (card == null)
					{
						continue;
					}

					Log(Level.Info, card.CardName + " / MERGE: " + card.Name);
					MergeCard(gitRunner, line.Trim());
				}
			}
			finally
			{
				if (!string.IsNullOrWhiteSpace(tempFile))
				{
					File.Delete(tempFile);
				}
			}
		}

		private void MergeCard(GitRunner gitRunner, string card)
		{
			if (gitRunner.MergeBranch(card))
			{
				return;
			}

			Log(Level.Warning, "Merge failed. Please resolve manually and then press enter to continue");
			Console.ReadLine();
		}
	}
}

using System;
using System.Collections.Generic;
using Core.Cli;

namespace Nant.Queries
{
	public class GetMergedCards
	{
		private readonly GitRunner _gitRunner;
		private readonly string _prefix;

		public GetMergedCards(GitRunner gitRunner, string prefix)
		{
			_gitRunner = gitRunner;
			_prefix = prefix;
		}

		public List<string> Execute(string destination)
		{
			if (!GetMergedBranches(destination))
			{
				throw new Exception("Cannot find merged cards on " + destination);
			}

			return FilterBranches();
		}

		private bool GetMergedBranches(string destination)
		{
			if (destination == "local")
			{
				return _gitRunner.GetMergedBranchesLocal();
			}

			return _gitRunner.GetMergedBranchesRemote();
		}

		private List<string> FilterBranches()
		{
			var merged = new List<string>();
			foreach (var line in _gitRunner.StandardOutput)
			{
				if (line.StartsWith("*"))
				{
					continue;
				}

				if (line.Contains(_prefix))
				{
					merged.Add(GetBranchName(line));
				}
			}

			return merged;
		}

		private string GetBranchName(string line)
		{
			int position;

			if ((position = line.LastIndexOf('/')) > -1)
			{
				return line.Substring(position + 1).Trim();
			}

			position = line.IndexOf(_prefix);
			return line.Substring(position).Trim();
		}
	}
}

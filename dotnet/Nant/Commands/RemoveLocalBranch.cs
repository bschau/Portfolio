using System;
using Core.Cli;

namespace Nant.Commands
{
	public class RemoveLocalBranch : IRemoveBranch
	{
		private readonly GitRunner _gitRunner;

		public RemoveLocalBranch(GitRunner gitRunner)
		{
			_gitRunner = gitRunner;
		}

		public void Execute(string branchName)
		{
			try
			{
				_gitRunner.RemoveLocalBranch(branchName);
			}
			catch (Exception)
			{
				throw new Exception("Failed to remove local branch " + branchName);
			}
		}
	}
}

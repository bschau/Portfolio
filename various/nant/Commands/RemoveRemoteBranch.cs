using System;
using Core.Cli;

namespace Nant.Commands
{
	public class RemoveRemoteBranch : IRemoveBranch
	{
		private readonly GitRunner _gitRunner;

		public RemoveRemoteBranch(GitRunner gitRunner)
		{
			_gitRunner = gitRunner;
		}

		public void Execute(string branchName)
		{
			try
			{
				_gitRunner.RemoveRemoteBranch(branchName);
			}
			catch (Exception)
			{
				throw new Exception("Failed to remove remote branch " + branchName);
			}
		}
	}
}

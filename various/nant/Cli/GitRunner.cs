using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace Cli 
{
	public class GitRunner
	{
		private readonly CommandRunner _commandRunner;
		private readonly string _gitCommand;
		public IEnumerable<string> Output { get { return _commandRunner.Output; } }

		public GitRunner(CommandRunner commandRunner)
		{
			_commandRunner = commandRunner;
			_gitCommand = FindGit();
		}

		private string FindGit()
		{
			foreach (var path in new[]
			{
				@"c:\Program Files (x86)\Git\cmd\git.exe",
				@"c:\Program Files\Git\cmd\git.exe"
			}.Where(File.Exists))
			{
				return path;
			}

			throw new Exception("Git not found");
		}

		private bool Run(string args)
		{
			_commandRunner.Execute(_gitCommand, args);
			return _commandRunner.ExitCode == 0;
		}

		public bool CheckoutMaster()
		{
			return Run("checkout master");
		}

		public bool Checkout(string branchName)
		{
			return Run("checkout " + branchName);
		}

		public bool BranchExists(string branchName)
		{
			return Run("show-ref --verify --quiet refs/heads/" + branchName);
		}

		public bool CreateBranch(string branchName)
		{
			return Run("checkout -b " + branchName);
		}

		public void Gui()
		{
			Run("gui");
		}

		public bool Update()
		{
			if (!Run("pull rovsing"))
			{
				return false;
			}

			return Run("push -f origin");
		}
	}
}

using System;

namespace jc
{
	public class Log
	{
		private bool _quiet;
		private bool _verbose;

		public Log(bool quiet, bool verbose)
		{
			_quiet = quiet;
			_verbose = verbose;
		}

		public void WriteLine(string formatter, params object[] values)
		{
			if (_quiet)
			{
				return;
			}

			Console.WriteLine(formatter, values);
		}

		public void Verbose(string formatter, params object[] values)
		{
			if (!_verbose)
			{
				return;
			}

			Console.WriteLine(formatter, values);
		}
	}
}
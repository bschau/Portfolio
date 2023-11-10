using jc.Translations;
using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;

namespace jc
{
	public class Arguments
	{
		readonly Dictionary<string, string> _variables;
		int _index;
		int _argumentsLength;

		public Arguments(Dictionary<string, string> variables)
		{
			_variables = variables;
		}

		public void Parse(string[] arguments)
		{
			if (arguments.Length == 0)
			{
				Usage(1);
			}

			_argumentsLength = arguments.Length;
			for (_index = 0; _index < _argumentsLength; _index++)
			{
				var argument = arguments[_index];
				if (argument[0] != '-' && argument[0] != '/')
				{
					break;
				}

				switch (argument[1])
				{
					case 'c':
						BumpIndex("-c");
						ConfigFile = arguments[_index];
						break;

					case 'h':
						Usage(0);
						break;

					case 'm':
						BumpIndex("-m");
						IfModifiedSince = new DateExpander(_variables).ExecuteAsDateTime(arguments[_index]);
						break;

					case 'o':
						BumpIndex("-o");
						OutputFile = arguments[_index];
						break;

					case 'q':
						Quiet = true;
						break;

					case 'P':
						ParseOnly = true;
						break;

					case 't':
						BumpIndex("-t");
						TemplatesFile = arguments[_index];
						break;

					case 'V':
						Version(Console.Out);
						Environment.Exit(0);
						break;

					case 'v':
						Verbose = true;
						break;

					case 'X':
						Xml = true;
						break;

					default:
						throw new Exception(string.Format("Illegal argument: {0}", argument));
				}
			}

			if (_argumentsLength - _index < 2)
			{
				throw new Exception("Missing profile and/or request");
			}

			Profile = arguments[_index++];
			Request = arguments[_index++];

			if (_argumentsLength - _index > 0)
			{
				PostData = arguments[_index];
			}
		}

		void BumpIndex(string argument)
		{
			_index++;
			if (_index >= _argumentsLength)
			{
				throw new Exception(string.Format("No parameter given to {0}", argument));
			}
		}

		void Version(TextWriter writer)
		{
			writer.WriteLine("jc.exe v{0}", Assembly.GetExecutingAssembly().GetName().Version);
		}

		void Usage(int exitCode)
		{
			var writer = exitCode == 0 ? Console.Out : Console.Error;
			Version(writer);
			writer.WriteLine("Usage: jc [OPTIONS] profile request [post-data file]");
			writer.WriteLine();
			writer.WriteLine("  -c config       Configuration (.jcrc) file");
			writer.WriteLine("  -h              Help (this page)");
			writer.WriteLine("  -m time         If-Modified-Since");
			writer.WriteLine("  -o outputfile   Store output in outputfile");
			writer.WriteLine("  -q              Quiet");
			writer.WriteLine("  -P              Parse only, ie. do not send any request");
			writer.WriteLine("  -t file         Save generated template file");
			writer.WriteLine("  -v              Verbose");
			writer.WriteLine("  -V              Show version");
			writer.WriteLine("  -X              Cast xml file");
			Environment.Exit(exitCode);
		}

		public string ConfigFile { get; private set;}
		public DateTime? IfModifiedSince { get; private set; }
		public string OutputFile { get; private set; }
		public bool Quiet { get; private set; }
		public bool ParseOnly { get; private set; }
		public string TemplatesFile { get; private set; }
		public bool Verbose { get; private set; }
		public bool Xml { get; private set; }
		public string PostData { get; private set; }
		public string Profile { get; private set; }
		public string Request { get; private set; }
	}
}

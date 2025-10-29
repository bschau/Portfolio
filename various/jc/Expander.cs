using jc.DomainModel;
using jc.Queries;
using jc.Translations;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace jc
{
	internal class Expander
	{
		readonly GetVariable _getVariable;
		readonly DateExpander _dateExpander;

		internal Expander(Configuration configuration)
		{
			_getVariable = new GetVariable(configuration.Variables);
			_dateExpander = new DateExpander(configuration.Variables);
		}

		internal string Execute(string source)
		{
			while (true)
			{
				var tokens = new GetTokenDictionary(source).Execute();
				if (!tokens.Any())
				{
					break;
				}

				var result = ExpandTokens(tokens);
				source = new Substitute(result).Execute(source);
			}

			return source;
		}

		Dictionary<string, string> ExpandTokens(Dictionary<string, string> tokens)
		{
			var result = new Dictionary<string, string>();
			foreach (var token in tokens)
			{
				var key = token.Key;
				key = key.Substring(2, key.Length - 4);

				if (key[0] == '$')
				{
					result[token.Key] = _getVariable.Execute(key.Substring(1).ToLower());
				}
				else if ("now".Equals(key, StringComparison.CurrentCultureIgnoreCase))
				{
					result[token.Key] = _getVariable.Execute("now");
				}
				else if (key.StartsWith("now:", StringComparison.CurrentCultureIgnoreCase))
				{
					result[token.Key] = _dateExpander.Execute(key.Substring(4));
				}
				else if (key.StartsWith("file:", StringComparison.CurrentCultureIgnoreCase))
				{
					result[token.Key] = ExpandFile(key.Substring(5));
				}
				else if (key.StartsWith("base64:", StringComparison.CurrentCultureIgnoreCase))
				{
					result[token.Key] = ExpandBase64(key.Substring(7));
				}
				else
				{
					throw new ArgumentException(string.Format("Unknown token: {0}", token.Key));
				}
			}

			return result;
		}

		string ExpandFile(string filename)
		{
			filename = Execute(filename);
			EnsureValidFile(filename, "file:");
			return File.ReadAllText(filename);
		}

		void EnsureValidFile(string filename, string protocol)
		{
			if (string.IsNullOrEmpty(filename))
			{
				throw new ArgumentException(string.Format("Missing filename to {0}", protocol));
			}

			if (!File.Exists(filename))
			{
				throw new ArgumentException(string.Format("{0}{1} - file does not exists", protocol, filename));
			}
		}

		string ExpandBase64(string filename)
		{
			filename = Execute(filename);
			EnsureValidFile(filename, "base64:");
			var content = File.ReadAllBytes(filename);
			return Convert.ToBase64String(content);
		}
	}
}

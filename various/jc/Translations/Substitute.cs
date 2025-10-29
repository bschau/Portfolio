using System.Collections.Generic;
using System.Linq;

namespace jc.Translations
{
	public class Substitute
	{
		readonly Dictionary<string, string> _tokens;

		public Substitute(Dictionary<string, string> tokens)
		{
			_tokens = tokens;
		}

		public string Execute(string json)
		{
			return _tokens.Aggregate(json, (current, token) => current.Replace(token.Key, token.Value));
		}
	}
}

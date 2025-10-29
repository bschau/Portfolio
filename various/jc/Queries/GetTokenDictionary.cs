using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;

namespace jc.Queries
{
	public class GetTokenDictionary
	{
		readonly string _json;

		public GetTokenDictionary(string json)
		{
			_json = json;
		}

		public Dictionary<string, string> Execute()
		{
			var tokens = new Dictionary<string, string>();
			var matches = Regex.Matches(_json, "\\[\\[([^\"]*)\\]\\]");
			foreach (var token in matches.Cast<object>()
						.Select(match => match.ToString())
						.Where(token => !tokens.ContainsKey(token)))
			{
				tokens.Add(token, string.Empty);
			}

			return tokens;
		}
	}
}

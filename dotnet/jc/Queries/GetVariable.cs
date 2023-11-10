using System;
using System.Collections.Generic;
using System.Linq;

namespace jc.Queries
{
	public class GetVariable
	{
		readonly Dictionary<string, string> _variables;

		public GetVariable(Dictionary<string, string> variables)
		{
			_variables = variables;
		}

		public string Execute(string key)
		{
			var testKey = _variables.Keys.SingleOrDefault(x => key.Equals(x, StringComparison.CurrentCultureIgnoreCase));
			if (testKey == null)
			{
				throw new ArgumentException(string.Format("Variables [[${0}]] is not defined", key));
			}

			return _variables[testKey];
		}
	}
}
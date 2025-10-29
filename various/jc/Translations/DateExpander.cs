using System;
using System.Collections.Generic;
using System.Globalization;
using jc.Extension;

namespace jc.Translations
{
	class DateExpander
	{
		readonly Dictionary<string, string> _variables;

		public DateExpander(Dictionary<string, string> variables)
		{
			_variables = variables;
		}

		public string Execute(string modifier)
		{
			return ExecuteAsDateTime(modifier).JsonDate();
		}

		public DateTime ExecuteAsDateTime(string modifier)
		{
			if (string.IsNullOrEmpty(modifier))
			{
				return DateTime.Parse(_variables["now"]);
			}

			var now = DateTime.Parse(_variables["now"]);
			var adjust = ParseModifier(modifier);
			return now.AddSeconds(adjust);
		}

		int ParseModifier(string modifier)
		{
			var originalModifier = modifier;
			var suffix = modifier.Substring(modifier.Length - 1);
			var multiplier = 0;

			if (suffix == "S")
			{
				multiplier = 1;
			}
			else if (suffix == "M")
			{
				multiplier = 60;
			}
			else if (suffix == "H")
			{
				multiplier = 60 * 60;
			}
			else if (suffix == "d")
			{
				multiplier = 60 * 60 * 24;
			}
			else if (suffix == "m")
			{
				multiplier = 60 * 60 * 24 * 30;
			}
			else if (suffix == "y")
			{
				multiplier = 60 * 60 * 24 * 365;
			}
			else if (suffix == "w")
			{
				multiplier = 60 * 60 * 24 * 7;
			}

			if (multiplier == 0)
			{
				multiplier = 60 * 60 * 24;
			}
			else
			{
				modifier = modifier.Substring(0, modifier.Length - 1);
			}

			int adjust;
			if (!int.TryParse(modifier, out adjust))
			{
				throw new ArgumentException(string.Format("Invalid modifier in [[now:{0}]]", originalModifier));
			}

			return adjust * multiplier;
		}
	}
}

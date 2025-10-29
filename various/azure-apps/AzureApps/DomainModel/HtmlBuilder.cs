using System;
using System.Text;
using dk.schau.AzureApps.Translations;

namespace dk.schau.AzureApps.DomainModel
{
	public class HtmlBuilder
	{
		const string _header = @"
<!DOCTYPE html>
<html dir=""ltr"" lang=""en"">
<head>
	<meta http-equiv=""Content-Type"" content=""text/html; charset=UTF-8"">
	<meta name=""viewport"" content=""width=device-width"">
	<title>{0}</title>
</head>
<body>
";
		readonly StringBuilder _body = new StringBuilder();
		public string Now { get; private set; }
		public string Title { get; private set; }

		internal HtmlBuilder(string title)
		{
			Now = new UtcNowToNow().ExecuteAsString();
			Title = string.Format("[{0}] {1}", title, Now);
			Reset();
		}

		public override string ToString()
		{
			_body.Append("</body></html>");
			return _body.ToString();
		}

		public void Reset()
		{
			_body.Clear();
			_body.AppendFormat(_header.Trim(), Title);
		}

		public void AppendLine()
		{
			_body.AppendLine();
		}

		public void Append(char character)
		{
			_body.Append(character);
		}

		public void Append(string chain)
		{
			_body.Append(chain);
		}

		public void AppendFormat(string formatter, params object[] values)
		{
			_body.AppendFormat(formatter, values);
		}
	}
}
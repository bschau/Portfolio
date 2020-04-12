using System;
using System.IO;
using VPSApps.Commands;
using VPSApps.DomainModel;

namespace VPSApps.Modules
{
	internal class Qotd : IModule
	{
		const string _moduleName = "Qotd";
		readonly VpsAppsRc _vpsAppsRc;

		public Qotd(VpsAppsRc vpsAppsRc)
		{
			_vpsAppsRc = vpsAppsRc;
		}

		public void Execute()
		{
			var qotdFilePath = GetQotdFilePath();
			var qotds = File.ReadAllText(qotdFilePath) + "\n\n\n";
			var qotd = GetRawQotd(qotds);
			var htmlBuilder = new HtmlBuilder(_moduleName);
			qotd = ParseQotd(htmlBuilder, qotd);

			var sendEmail = new SendEmail(_vpsAppsRc.From, _vpsAppsRc.To);
			sendEmail.Execute(htmlBuilder.Title, htmlBuilder.ToString());
		}

		string GetQotdFilePath()
		{
			if (!string.IsNullOrWhiteSpace(_vpsAppsRc.QotdFilePath))
			{
				return _vpsAppsRc.QotdFilePath;
			}

			return Path.Combine(Configuration.Home, Configuration.IsUnix
					? ".qotd"
					: "_qotd");
		}

		string GetRawQotd(string qotds)
		{
			var random = new Random();
			var length = qotds.Length;

			var middle = random.Next() % length;
			var start = FindPosition(qotds, middle, -1);
			start = start == -1 ? 0 : start + 2;
			var end = FindPosition(qotds, start, 1);
			return qotds.Substring(start, end - start).Trim();
		}

		int FindPosition(string qotds, int current, int direction)
		{
			var length = qotds.Length - 1;
			while (true)
			{
				if (qotds[current] == '%' &&
					(qotds[current + 1] == '\n' ||
					(qotds[current + 1] == '\r' && qotds[current + 2] == '\n')))
				{
					return current;
				}

				current += direction;
				if (current < 0)
				{
					return -1;
				}
				else if (current > length)
				{
					return length;
				}
			}
		}

		string ParseQotd(HtmlBuilder htmlBuilder, string qotd)
		{
			var index = 0;
			var inBold = false;
			var inItalic = false;
			while (index < qotd.Length)
			{
				var character = qotd[index];
				if (character == '\\')
				{
					index++;
					htmlBuilder.Append(qotd[index]);
				}
				else if (character == '\n')
				{
					htmlBuilder.Append("<br />");
				}
				else if (character == '*')
				{
					htmlBuilder.Append(inBold ? "</b>" : "<b>");
					inBold = !inBold;
				}
				else if (character == '_')
				{
					htmlBuilder.Append(inItalic ? "</i>" : "<i>");
					inItalic = !inItalic;
				}
				else if (character == '#')
				{
					htmlBuilder.Append("&nbsp;");
				}
				else
				{
					htmlBuilder.Append(character);
				}
				index++;
			}

			return htmlBuilder.ToString();
		}
	}
}
using System.Text;
using dk.schau.AzureApps.DomainModel;

namespace dk.schau.AzureApps.Modules.Qotd
{
	class ParseQotd
	{
		internal void Execute(HtmlBuilder dst, string qotd)
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
					dst.Append(qotd[index]);
				}
				else if (character == '\n')
				{
					dst.Append("<br />");
				}
				else if (character == '*')
				{
					dst.Append(inBold ? "</b>" : "<b>");
					inBold = !inBold;
				}
				else if (character == '_')
				{
					dst.Append(inItalic ? "</i>" : "<i>");
					inItalic = !inItalic;
				}
				else if (character == '#')
				{
					dst.Append("&nbsp;");
				}
				else
				{
					dst.Append(character);
				}
				index++;
			}
		}
	}
}
using System;
using System.Text;
using dk.schau.AzureApps.Commands;

namespace dk.schau.AzureApps.Modules.Qotd
{
	class GetRawQotd
	{
		internal string Execute(string url)
		{
			var fetcher = new Fetcher();
			var data = fetcher.ExecuteAsync(url).Result;
			var qotds = Encoding.UTF8.GetString(data);

			var random = new Random();
			var length = qotds.Length;

			var middle = random.Next() % length;
			var start = FindPosition(qotds, middle, -1);
			start = start == -1 ? 0 : start + 2;
			var end = FindPosition(qotds, start, 1);
			return qotds.Substring(start, end - start).Trim();
		}

		static int FindPosition(string qotds, int current, int direction)
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
	}
}
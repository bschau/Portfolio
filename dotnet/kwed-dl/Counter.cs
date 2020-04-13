using System.IO;
using System.Linq;

namespace kweddl
{
	public class Counter
	{
		readonly string _counterFile;
		int _count;

		public Counter(string counterFile)
		{
			_counterFile = counterFile;
		}

		public void Load()
		{
			if (!File.Exists(_counterFile))
			{
				return;
			}

			var content = File.ReadAllLines(_counterFile).FirstOrDefault();
			if (string.IsNullOrWhiteSpace(content))
			{
				return;
			}
			
			int.TryParse(content, out _count);
		}

		public bool Seen(int kwedId)
		{
			return kwedId <= _count;
		}

		public void Set(int kwedId)
		{
			_count = kwedId;
		}

		public void Save()
		{
			File.WriteAllText(_counterFile, _count.ToString());
		}
	}
}
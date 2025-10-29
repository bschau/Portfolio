using System.Collections.Generic;

namespace jc.DomainModel
{
	public class Configuration
	{
		public Configuration()
		{
			Headers = new Dictionary<string, string>();
			Variables = new Dictionary<string, string>();
		}

		public Dictionary<string, string> Headers { get; set; }
		public Dictionary<string, string> Variables { get; set; }
		public string Profile { get; set; }
	}
}
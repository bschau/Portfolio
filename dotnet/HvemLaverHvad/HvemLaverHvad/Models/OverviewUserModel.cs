using System;
using System.Collections.Generic;

namespace HvemLaverHvad.Models
{
	public class OverviewUserModel : IComparable
	{
		public OverviewUserModel()
		{
			Tasks = new List<OverviewUserTaskModel>();
		}

		public string Name { get; set; }
		public List<OverviewUserTaskModel> Tasks { get; set; }
		public string Status { get; set; }
		public string Emoji { get; set; }
		public string BackgroundColor { get; set; }

		public int CompareTo(object obj)
		{
			return string.Compare(Name, ((OverviewUserModel)obj).Name, StringComparison.CurrentCulture);
		}
	}
}

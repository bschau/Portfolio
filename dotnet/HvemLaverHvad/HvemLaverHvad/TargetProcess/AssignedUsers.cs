using System.Collections.Generic;

namespace HvemLaverHvad.TargetProcess
{
	public class AssignedUsers
	{
		public AssignedUsers()
		{
			Items = new List<AssignedUser>();
		}

		public List<AssignedUser> Items { get; set; }
	}
}

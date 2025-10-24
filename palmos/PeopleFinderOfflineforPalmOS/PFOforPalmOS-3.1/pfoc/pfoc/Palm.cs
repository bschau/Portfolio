using System;
using System.Runtime.InteropServices;
using System.Text;

namespace pfoc
{
	/// <summary>
	/// Summary description for Palm.
	/// </summary>
	public class Palm
	{
		[DllImport("Instaide.dll", EntryPoint="PltGetUserCount", SetLastError=true,
			 CharSet=CharSet.Ansi, ExactSpelling=false,
			 CallingConvention=CallingConvention.StdCall)]
		public static extern int PltGetUserCount();

		[DllImport("Instaide.dll", EntryPoint="PltGetUser", SetLastError=true,
			 CharSet=CharSet.Ansi, ExactSpelling=false,
			 CallingConvention=CallingConvention.StdCall)]
		public static extern int PltGetUser(int iIndex, StringBuilder userName, ref int size);

		[DllImport("Instaide.dll", EntryPoint="PltInstallFile", SetLastError=true,
			 CharSet=CharSet.Ansi, ExactSpelling=false,
			 CallingConvention=CallingConvention.StdCall)]
		public static extern int PltInstallFile(string userName, string fileName);

		[DllImport("Instaide.dll", EntryPoint="PltRemoveInstallFile", SetLastError=true,
			 CharSet=CharSet.Ansi, ExactSpelling=false,
			 CallingConvention=CallingConvention.StdCall)]
		public static extern int PltRemoveInstallFile(string pUser,	string pFileName);

		public string[] users;

		public Palm()
		{
		}

		public int GetUserCount()
		{
			return Palm.PltGetUserCount();
		}

		public void GetUsers()
		{
			try 
			{
				int cnt=GetUserCount(), idx;
				StringBuilder userName=new StringBuilder(128);

				users=new string[cnt];

				for (idx=0; idx<cnt; idx++) 
				{
					int size=userName.Capacity;

					if (Palm.PltGetUser(idx, userName, ref size)>0) 
						users[idx]=userName.ToString();

					if (userName.Length>0)
						userName.Remove(0, userName.Length);
				}
			} 
			catch {}
		}

		public int InstallToMemory(string user, string file)
		{
			PltRemoveInstallFile(user, file);
			return PltInstallFile(user, file);
		}
	}
}

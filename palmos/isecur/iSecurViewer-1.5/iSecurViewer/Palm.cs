using System;
using System.Runtime.InteropServices;
using System.Text;

namespace iSecurViewer
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

        [DllImport("Instaide.dll", EntryPoint = "PltGetUserDirectory", SetLastError = true,
            	CharSet = CharSet.Ansi, ExactSpelling = false,
            	CallingConvention = CallingConvention.StdCall)]
        public static extern int PltGetUserDirectory(string user, StringBuilder path, ref int size);

		public string[] users;

		public Palm()
		{
		}

		/// <summary>
		/// Return number # of HotSync users registered on this PC.
		/// </summary>
		/// <returns>Count.</returns>
		public int GetUserCount()
		{
			return Palm.PltGetUserCount();
		}

		/// <summary>
		/// Build Users array.
		/// </summary>
		public void GetUsers()
		{
			try 
			{
				int cnt=GetUserCount(), idx;
				
				if (cnt>0)
				{
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
			} 
			catch {}
		}

		/// <summary>
		/// Return backup directory for this profile.
		/// </summary>
		/// <param name="profile">Profile</param>
		/// <returns>Path.</returns>
        public string GetBackupDir(string profile)
        {
        	try {
  				StringBuilder path=new StringBuilder(512);
                int size=path.Capacity;

                if (Palm.PltGetUserDirectory(profile, path, ref size)>0) {
                	path.Append("\\backup");
                    return path.ToString();
                }
            }
            catch {}

            return null;
        }
	}
}

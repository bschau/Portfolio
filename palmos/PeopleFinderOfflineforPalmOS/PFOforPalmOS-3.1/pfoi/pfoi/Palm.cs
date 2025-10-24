using System;
using System.Collections;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

namespace pfoi
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

        internal class InvariantComparer : IComparer
        {
            private CompareInfo compareInfo;
            internal static readonly InvariantComparer Default = new InvariantComparer();

            internal InvariantComparer()
            {
                compareInfo = CultureInfo.InvariantCulture.CompareInfo;
            }

            public int Compare(Object a, Object b)
            {
                String sa = a as String;
                String sb = b as String;
                if (sa != null && sb != null)
                    return compareInfo.Compare(sa, sb);
                else
                    return Comparer.Default.Compare(a, b);
            }
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

                Array.Sort(users, InvariantComparer.Default);
			} 
			catch {}
		}

		public int InstallToMemory(string user, string file)
		{
            int ret;

			PltRemoveInstallFile(user, file);
            if ((ret = PltInstallFile(user, file)) != 0)
                MessageBox.Show("Failed to install " + file + " - error code: " + ret, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);

            return ret;
		}
	}
}

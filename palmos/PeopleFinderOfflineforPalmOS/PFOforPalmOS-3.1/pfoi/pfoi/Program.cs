using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace pfoi
{
    static class Program
    {
        static string prcFile = "PFO.prc";

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            Palm palm = new Palm();
            bool oneShot;

            if ((args.Length > 0) && (args[0].ToLower().CompareTo("/oneshot")==0))
                oneShot = true;
            else
                oneShot = false;

            palm.GetUsers();
            if (palm.users.Length > 1)
            {
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Application.Run(new fMain(palm, prcFile, oneShot));
            }
            else
            {
                palm.InstallToMemory(palm.users[0], prcFile);
            }
        }
    }
}
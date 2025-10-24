using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace TramsInst
{
    public partial class fMain : Form
    {
        private Palm palm;
        private string prcFile;
        private bool oneShot;

        public fMain(Palm p, string file, bool os)
        {
            palm = p;
            prcFile = file;
            oneShot = os;
            InitializeComponent();

            for (int idx = 0; idx < palm.users.Length; idx++)
                cProfile.Items.Add(palm.users[idx]);

            cProfile.SelectedIndex = 0;
        }

        private void cCancel_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void cInstall_Click(object sender, EventArgs e)
        {
            int idx = cProfile.SelectedIndex;

            if ((idx >= 0) && (idx < palm.users.Length))
            {
                if (palm.InstallToMemory(palm.users[idx], prcFile) == 0)
                {
                    MessageBox.Show("Trams for Palm OS successfully queued for " + palm.users[idx], "Success!", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    if (oneShot)
                        Close();
                }
            }
        }
    }
}
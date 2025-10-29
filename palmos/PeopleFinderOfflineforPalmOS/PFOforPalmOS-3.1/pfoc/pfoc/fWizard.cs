using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;
using Microsoft.Win32;

namespace pfoc
{
    public partial class fWizard : Form
    {
        public static bool appStopped = false;
        private bool withErrors=true;
        private string errText;
        public bool win32;
        public Palm palm;
        public Config config;

        public fWizard()
        {
            string s = Environment.OSVersion.ToString();

            win32 = s.Contains("Windows");

            InitializeComponent();

            config = new Config();
            config.Load();

            if ((config.dbPath != null) && (config.dbPath.Length > 0))
            {
                if (File.Exists(config.dbPath) == false)
                    config.dbPath = null;
            }
            else
                config.dbPath = null;

            if ((config.outputPath != null) && (config.outputPath.Length > 0))
            {
                if (Directory.Exists(config.outputPath) == false)
                    config.outputPath = null;
            }
            else
                config.outputPath = null;

            if (win32)
            {
                if (config.dbPath == null)
                {
                    RegistryKey key;

                    config.dbPath = "";
                    if ((key = Registry.CurrentUser.OpenSubKey(@"SOFTWARE\HP\instant contact")) != null)
                    {
                        string p;

                        if ((p = (string)key.GetValue("workingDirectory")) != null)
                        {
                            if (p.EndsWith(@"\") == true)
                                p = p.Substring(0, p.Length - 1);

                            if (File.Exists(p + @"\database\PDA\database.ddb") == true)
                                config.dbPath = p + @"\database\PDA\database.ddb";
                            else if (File.Exists(p) == true)
                                config.dbPath = p;
                        }

                        key.Close();
                    }
                }

                if (config.outputPath == null)
                    config.outputPath = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + @"\HP\PeopleFinder Offline for Palm OS";

                palm = new Palm();
                palm.GetUsers();
            }
            else
            {
                config.dbPath = "";
                palm = null;
            }
        }

        /// <summary>
        /// Ask for confirmation to exit.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void pfocWizard_CloseFromCancel(object sender, CancelEventArgs e)
        {
            if (MessageBox.Show("Is it ok to close the PeopleFinder Offline Database to Palm Database Wizard?", "Close?", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.No)
                e.Cancel = true;
            else
                appStopped = true;
        }

        /// <summary>
        /// Setup the data screen.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void wpData_ShowFromNext(object sender, EventArgs e)
        {
            if ((cDBPath.Text == null) || (cDBPath.Text.Length < 1))
                cDBPath.Text = config.dbPath;

            if ((cPDBPath.Text == null) || (cPDBPath.Text.Length < 1))
                cPDBPath.Text = config.outputPath;

            if (win32)
            {
                for (int idx = 0; idx < palm.users.Length; idx++)
                    cProfile.Items.Add(palm.users[idx]);

                if ((config.palmProfile != null) && (config.palmProfile.Length > 0))
                {
                    int idx = cProfile.FindStringExact(config.palmProfile);

                    if (idx < 0)
                        cProfile.SelectedIndex = 0;
                    else
                        cProfile.SelectedIndex = idx;
                }
                else
                    cProfile.SelectedIndex = 0;

                cInstall.Checked = config.queue;
                cProfile.Enabled = cInstall.Checked;
            }
            else
            {
                cProfile.Enabled = false;
                cInstall.Enabled = false;
            }
        }

        /// <summary>
        /// Handle clicks on the "Install to Palm Profile" checkbox.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void cInstall_CheckedChanged(object sender, EventArgs e)
        {
            cProfile.Enabled = cInstall.Checked;
        }

        /// <summary>
        /// Handle the "Browse for database" dialog.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void cBrowseDB_Click(object sender, EventArgs e)
        {
            string s;

            if ((cDBPath.Text == null) || (cDBPath.Text.Length < 1))
                s = config.dbPath;
            else
                s = cDBPath.Text;

            ofDialog.FileName = Path.GetFileName(s);
            ofDialog.InitialDirectory = Path.GetDirectoryName(s);

            if (ofDialog.ShowDialog() == DialogResult.OK)
                cDBPath.Text = ofDialog.FileName;
        }

        /// <summary>
        /// Handle the "Browse for directory" dialog.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void cBrowsePDB_Click(object sender, EventArgs e)
        {
            string s;

            if ((cPDBPath == null) || (cPDBPath.Text.Length < 1))
                s = config.outputPath;
            else
                s = cPDBPath.Text;

            fbDialog.SelectedPath = s;

            if (fbDialog.ShowDialog() == DialogResult.OK)
                cPDBPath.Text = fbDialog.SelectedPath;
        }

        /// <summary>
        /// Handle data verification _before_ going to the process form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void wpData_CloseFromNext(object sender, Gui.Wizard.PageEventArgs e)
        {
            if (File.Exists(cDBPath.Text) == false)
            {
                MessageBox.Show("The database path is invalid.", "Error!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                e.Page = wpData;
                return;
            }
            else
                config.dbPath = cDBPath.Text;

            if (Directory.Exists(cPDBPath.Text) == false)
            {
                MessageBox.Show("The output path is invalid.", "Error!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                e.Page = wpData;
                return;
            }

            if (win32)
            {
                config.palmProfile = cProfile.Text;
                config.queue = cInstall.Checked;
            }
            else
            {
                config.palmProfile = "";
                config.queue = false;
            }

            config.Save();
        }

        /// <summary>
        /// Handle the process dialog.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void wpProcess_ShowFromNext(object sender, EventArgs e)
        {
            Convert conv = null;

            pfocWizard.BackEnabled = false;
            pfocWizard.NextEnabled = false;

            try
            {
                int fileCnt;

                conv = new Convert(this, config.dbPath);

                conv.Open();
                fileCnt = conv.FileCount();

                cProgressBar.Minimum = 0;
                cProgressBar.Maximum = fileCnt+150;
                cProgressBar.Step = 1;

                conv.BuildPersonArray();
                if (!appStopped)
                {
                    cStatus.Text = "Cross referencing managers/employees";
                    conv.MgrXRef();
                }

                if (!appStopped)
                    conv.CollapseAll();

                if (!appStopped)
                    conv.WriteFiles(config.outputPath);

                if (!appStopped)
                    conv.BuildPhoneDBLists();

                if (!appStopped)
                    conv.WritePhoneDBs(config.outputPath);

                cProgressBar.Value = cProgressBar.Maximum;
                withErrors = appStopped;
            }
            catch (Exception ex)
            {
                errText = ex.Message;
            }

            if (conv != null)
                conv.Close();

            if (!appStopped)
                pfocWizard.Next();
        }

        private void wpFinal_ShowFromNext(object sender, EventArgs e)
        {
            pfocWizard.CancelEnabled = false;
            if (withErrors)
            {
                if ((errText != null) && (errText.Length > 0))
                    cReport.Text = "ERROR: "+errText;
                else
                    cReport.Text = "ERROR: An unknown error occurred.";
            }
            else
                cReport.Text = "Conversion finished successfully!";
        }

        /// <summary>
        /// If "Back" is selected, jump to the Enter Data page.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void wpFinal_CloseFromBack(object sender, Gui.Wizard.PageEventArgs e)
        {
            pfocWizard.CancelEnabled = true;
            e.Page = wpData;
        }
    }
}
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;

namespace TramsCvt
{
    public partial class fWizard : Form
    {
        public bool win32 = Environment.OSVersion.ToString().Contains("Windows");
        public static bool appStopped = false;
        public Config config = new Config();
        public Palm palm;
        //        private bool withErrors = true;
        private string errText;

        public fWizard()
        {
            InitializeComponent();

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
                    config.dbPath = @"c:\Trams\mobile\traprg\ExpCat.dat";
                }

                if (config.outputPath == null)
                    config.outputPath = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + @"\HP\Trams for Palm OS";

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
        private void wizard_CloseFromCancel(object sender, CancelEventArgs e)
        {
            if (MessageBox.Show("Is it ok to close the Trams Mobile Database to Palm Database Converter Wizard?", "Close?", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.No)
                e.Cancel = true;
            else
                appStopped = true;
        }

        /// <summary>
        /// Setup the data screen.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void wizDataColl_ShowFromNext(object sender, EventArgs e)
        {
            if ((cDBPath.Text == null) || (cDBPath.Text.Length < 1))
                cDBPath.Text = config.dbPath;

            if ((cPDBPath.Text == null) || (cPDBPath.Text.Length < 1))
                cPDBPath.Text = config.outputPath;

            if (win32)
            {
                if (palm.users.Length>0)
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
        private void bBrowse1_Click(object sender, EventArgs e)
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
        private void bBrowse2_Click(object sender, EventArgs e)
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
        private void wizDataColl_CloseFromNext(object sender, Gui.Wizard.PageEventArgs e)
        {
            if (File.Exists(cDBPath.Text) == false)
            {
                MessageBox.Show("The database path is invalid.", "Error!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                e.Page = wizDataColl;
                return;
            }
            config.dbPath = cDBPath.Text;

            if (Directory.Exists(cPDBPath.Text) == false)
            {
                try
                {
                    Directory.CreateDirectory(cPDBPath.Text);
                }
                catch
                {
                    MessageBox.Show("Failed to create output directory: " + cPDBPath.Text, "Error!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    e.Page = wizDataColl;
                    return;
                }
            }
            config.outputPath = cPDBPath.Text;

            if (win32)
            {
                if (palm.users.Length > 0)
                {
                    config.palmProfile = cProfile.Text;
                    config.queue = cInstall.Checked;
                }
                else
                {
                    config.palmProfile = "";
                    config.queue = false;
                }
            }
            else
            {
                config.palmProfile = "";
                config.queue = false;
            }

            config.Save();
        }

        private void wizWorking_ShowFromNext(object sender, EventArgs e)
        {
            Converter conv = null;

            wizard.BackEnabled = false;
            wizard.NextEnabled = false;

            try
            {
                conv = new Converter(this);

                conv.LoadFile(config.dbPath);

                if (!appStopped)
                    conv.CreatePDB(config.outputPath);

                if (!appStopped)
                {
                    if ((win32 == true) && (cInstall.Checked == true))
                    {
                        if (palm.InstallToMemory(config.palmProfile, conv.pdbFile) == 0)
                            cStatus.Text = "Database queued for HotSync for " + config.palmProfile;
                        else
                            throw new Exception("Cannot queue database for HotSync for " + config.palmProfile);
                    }
                }
            }
            catch (Exception ex)
            {
                errText = ex.Message;
            }

            if (!appStopped)
                wizard.Next();            
        }

        private void wizFinish_ShowFromNext(object sender, EventArgs e)
        {
            wizard.CancelEnabled = false;

            if (errText != null)
                cReport.Text = "ERROR: "+errText;
            else
                cReport.Text = "Conversion finished successfully!";
        }

        private void wizFinish_CloseFromBack(object sender, Gui.Wizard.PageEventArgs e)
        {
            wizard.CancelEnabled = true;
            e.Page = wizDataColl;
        }
    }
}
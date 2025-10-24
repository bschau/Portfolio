namespace pfoc
{
    partial class fWizard
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(fWizard));
            this.ofDialog = new System.Windows.Forms.OpenFileDialog();
            this.fbDialog = new System.Windows.Forms.FolderBrowserDialog();
            this.pfocWizard = new Gui.Wizard.Wizard();
            this.wpWelcome = new Gui.Wizard.WizardPage();
            this.wpInfo = new Gui.Wizard.InfoPage();
            this.wpFinal = new Gui.Wizard.WizardPage();
            this.cReport = new System.Windows.Forms.TextBox();
            this.header3 = new Gui.Wizard.Header();
            this.wpProcess = new Gui.Wizard.WizardPage();
            this.cStatus = new System.Windows.Forms.TextBox();
            this.cProgressBar = new System.Windows.Forms.ProgressBar();
            this.header2 = new Gui.Wizard.Header();
            this.wpData = new Gui.Wizard.WizardPage();
            this.cInstall = new System.Windows.Forms.CheckBox();
            this.cBrowsePDB = new System.Windows.Forms.Button();
            this.cBrowseDB = new System.Windows.Forms.Button();
            this.cProfile = new System.Windows.Forms.ComboBox();
            this.panel4 = new System.Windows.Forms.Panel();
            this.cPDBPath = new System.Windows.Forms.TextBox();
            this.panel3 = new System.Windows.Forms.Panel();
            this.label2 = new System.Windows.Forms.Label();
            this.cDBPath = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.panel2 = new System.Windows.Forms.Panel();
            this.panel1 = new System.Windows.Forms.Panel();
            this.header1 = new Gui.Wizard.Header();
            this.pfocWizard.SuspendLayout();
            this.wpWelcome.SuspendLayout();
            this.wpFinal.SuspendLayout();
            this.wpProcess.SuspendLayout();
            this.wpData.SuspendLayout();
            this.SuspendLayout();
            // 
            // ofDialog
            // 
            this.ofDialog.DefaultExt = "ddb";
            this.ofDialog.Filter = "PeopleFinder Offline Database (*.ddb)|*.ddb|All Files (*.*)|*.*";
            this.ofDialog.ReadOnlyChecked = true;
            this.ofDialog.Title = "Select PeopleFinder Offline Database file";
            // 
            // fbDialog
            // 
            this.fbDialog.Description = "Select Output Folder";
            // 
            // pfocWizard
            // 
            this.pfocWizard.Controls.Add(this.wpData);
            this.pfocWizard.Controls.Add(this.wpWelcome);
            this.pfocWizard.Controls.Add(this.wpFinal);
            this.pfocWizard.Controls.Add(this.wpProcess);
            this.pfocWizard.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pfocWizard.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.pfocWizard.Location = new System.Drawing.Point(0, 0);
            this.pfocWizard.Name = "pfocWizard";
            this.pfocWizard.Pages.AddRange(new Gui.Wizard.WizardPage[] {
            this.wpWelcome,
            this.wpData,
            this.wpProcess,
            this.wpFinal});
            this.pfocWizard.Size = new System.Drawing.Size(538, 394);
            this.pfocWizard.TabIndex = 0;
            this.pfocWizard.CloseFromCancel += new System.ComponentModel.CancelEventHandler(this.pfocWizard_CloseFromCancel);
            // 
            // wpWelcome
            // 
            this.wpWelcome.Controls.Add(this.wpInfo);
            this.wpWelcome.Dock = System.Windows.Forms.DockStyle.Fill;
            this.wpWelcome.IsFinishPage = false;
            this.wpWelcome.Location = new System.Drawing.Point(0, 0);
            this.wpWelcome.Name = "wpWelcome";
            this.wpWelcome.Size = new System.Drawing.Size(538, 346);
            this.wpWelcome.TabIndex = 1;
            // 
            // wpInfo
            // 
            this.wpInfo.BackColor = System.Drawing.Color.White;
            this.wpInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.wpInfo.Image = ((System.Drawing.Image)(resources.GetObject("wpInfo.Image")));
            this.wpInfo.Location = new System.Drawing.Point(0, 0);
            this.wpInfo.Name = "wpInfo";
            this.wpInfo.PageText = "This wizard enables you to convert the PeopleFinder Offline Database to a set of " +
                "Palm Databases suitable for PFO. Please, before running this wizard, update your" +
                " PeopleFinder Offline Database.";
            this.wpInfo.PageTitle = "Welcome to the PeopleFinder Offline Database to Palm Database Converter";
            this.wpInfo.Size = new System.Drawing.Size(538, 346);
            this.wpInfo.TabIndex = 0;
            // 
            // wpFinal
            // 
            this.wpFinal.Controls.Add(this.cReport);
            this.wpFinal.Controls.Add(this.header3);
            this.wpFinal.Dock = System.Windows.Forms.DockStyle.Fill;
            this.wpFinal.IsFinishPage = true;
            this.wpFinal.Location = new System.Drawing.Point(0, 0);
            this.wpFinal.Name = "wpFinal";
            this.wpFinal.Size = new System.Drawing.Size(538, 346);
            this.wpFinal.TabIndex = 4;
            this.wpFinal.CloseFromBack += new Gui.Wizard.PageEventHandler(this.wpFinal_CloseFromBack);
            this.wpFinal.ShowFromNext += new System.EventHandler(this.wpFinal_ShowFromNext);
            // 
            // cReport
            // 
            this.cReport.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.cReport.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cReport.Location = new System.Drawing.Point(12, 172);
            this.cReport.Name = "cReport";
            this.cReport.ReadOnly = true;
            this.cReport.Size = new System.Drawing.Size(514, 16);
            this.cReport.TabIndex = 1;
            this.cReport.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // header3
            // 
            this.header3.BackColor = System.Drawing.SystemColors.Control;
            this.header3.CausesValidation = false;
            this.header3.Description = "";
            this.header3.Dock = System.Windows.Forms.DockStyle.Top;
            this.header3.Image = ((System.Drawing.Image)(resources.GetObject("header3.Image")));
            this.header3.Location = new System.Drawing.Point(0, 0);
            this.header3.Name = "header3";
            this.header3.Size = new System.Drawing.Size(538, 64);
            this.header3.TabIndex = 0;
            this.header3.Title = "Conversion Status";
            // 
            // wpProcess
            // 
            this.wpProcess.Controls.Add(this.cStatus);
            this.wpProcess.Controls.Add(this.cProgressBar);
            this.wpProcess.Controls.Add(this.header2);
            this.wpProcess.Dock = System.Windows.Forms.DockStyle.Fill;
            this.wpProcess.IsFinishPage = false;
            this.wpProcess.Location = new System.Drawing.Point(0, 0);
            this.wpProcess.Name = "wpProcess";
            this.wpProcess.Size = new System.Drawing.Size(538, 346);
            this.wpProcess.TabIndex = 3;
            this.wpProcess.ShowFromNext += new System.EventHandler(this.wpProcess_ShowFromNext);
            // 
            // cStatus
            // 
            this.cStatus.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.cStatus.Location = new System.Drawing.Point(25, 142);
            this.cStatus.Multiline = true;
            this.cStatus.Name = "cStatus";
            this.cStatus.ReadOnly = true;
            this.cStatus.Size = new System.Drawing.Size(488, 32);
            this.cStatus.TabIndex = 2;
            this.cStatus.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // cProgressBar
            // 
            this.cProgressBar.Location = new System.Drawing.Point(25, 180);
            this.cProgressBar.Name = "cProgressBar";
            this.cProgressBar.Size = new System.Drawing.Size(488, 23);
            this.cProgressBar.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            this.cProgressBar.TabIndex = 1;
            // 
            // header2
            // 
            this.header2.BackColor = System.Drawing.SystemColors.Control;
            this.header2.CausesValidation = false;
            this.header2.Description = "Conversion in progress - please wait!";
            this.header2.Dock = System.Windows.Forms.DockStyle.Top;
            this.header2.Image = ((System.Drawing.Image)(resources.GetObject("header2.Image")));
            this.header2.Location = new System.Drawing.Point(0, 0);
            this.header2.Name = "header2";
            this.header2.Size = new System.Drawing.Size(538, 64);
            this.header2.TabIndex = 0;
            this.header2.Title = "Working";
            // 
            // wpData
            // 
            this.wpData.Controls.Add(this.cInstall);
            this.wpData.Controls.Add(this.cBrowsePDB);
            this.wpData.Controls.Add(this.cBrowseDB);
            this.wpData.Controls.Add(this.cProfile);
            this.wpData.Controls.Add(this.panel4);
            this.wpData.Controls.Add(this.cPDBPath);
            this.wpData.Controls.Add(this.panel3);
            this.wpData.Controls.Add(this.label2);
            this.wpData.Controls.Add(this.cDBPath);
            this.wpData.Controls.Add(this.label1);
            this.wpData.Controls.Add(this.panel2);
            this.wpData.Controls.Add(this.panel1);
            this.wpData.Controls.Add(this.header1);
            this.wpData.Dock = System.Windows.Forms.DockStyle.Fill;
            this.wpData.IsFinishPage = false;
            this.wpData.Location = new System.Drawing.Point(0, 0);
            this.wpData.Name = "wpData";
            this.wpData.Size = new System.Drawing.Size(538, 346);
            this.wpData.TabIndex = 2;
            this.wpData.CloseFromNext += new Gui.Wizard.PageEventHandler(this.wpData_CloseFromNext);
            this.wpData.ShowFromNext += new System.EventHandler(this.wpData_ShowFromNext);
            // 
            // cInstall
            // 
            this.cInstall.AutoSize = true;
            this.cInstall.Location = new System.Drawing.Point(16, 201);
            this.cInstall.Name = "cInstall";
            this.cInstall.Size = new System.Drawing.Size(130, 17);
            this.cInstall.TabIndex = 5;
            this.cInstall.Text = "Install to Palm Profile:";
            this.cInstall.UseVisualStyleBackColor = true;
            this.cInstall.CheckedChanged += new System.EventHandler(this.cInstall_CheckedChanged);
            // 
            // cBrowsePDB
            // 
            this.cBrowsePDB.Location = new System.Drawing.Point(445, 146);
            this.cBrowsePDB.Name = "cBrowsePDB";
            this.cBrowsePDB.Size = new System.Drawing.Size(75, 23);
            this.cBrowsePDB.TabIndex = 4;
            this.cBrowsePDB.Text = "Browse...";
            this.cBrowsePDB.UseVisualStyleBackColor = true;
            this.cBrowsePDB.Click += new System.EventHandler(this.cBrowsePDB_Click);
            // 
            // cBrowseDB
            // 
            this.cBrowseDB.Location = new System.Drawing.Point(445, 88);
            this.cBrowseDB.Name = "cBrowseDB";
            this.cBrowseDB.Size = new System.Drawing.Size(75, 23);
            this.cBrowseDB.TabIndex = 2;
            this.cBrowseDB.Text = "Browse...";
            this.cBrowseDB.UseVisualStyleBackColor = true;
            this.cBrowseDB.Click += new System.EventHandler(this.cBrowseDB_Click);
            // 
            // cProfile
            // 
            this.cProfile.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cProfile.FormattingEnabled = true;
            this.cProfile.Location = new System.Drawing.Point(152, 199);
            this.cProfile.Name = "cProfile";
            this.cProfile.Size = new System.Drawing.Size(287, 21);
            this.cProfile.TabIndex = 6;
            // 
            // panel4
            // 
            this.panel4.Location = new System.Drawing.Point(10, 173);
            this.panel4.Name = "panel4";
            this.panel4.Size = new System.Drawing.Size(200, 10);
            this.panel4.TabIndex = 9;
            // 
            // cPDBPath
            // 
            this.cPDBPath.Location = new System.Drawing.Point(10, 146);
            this.cPDBPath.Name = "cPDBPath";
            this.cPDBPath.Size = new System.Drawing.Size(429, 21);
            this.cPDBPath.TabIndex = 3;
            // 
            // panel3
            // 
            this.panel3.Location = new System.Drawing.Point(10, 117);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(200, 10);
            this.panel3.TabIndex = 7;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(10, 130);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(144, 13);
            this.label2.TabIndex = 6;
            this.label2.Text = "Palm Database Output Path:";
            // 
            // cDBPath
            // 
            this.cDBPath.Location = new System.Drawing.Point(10, 90);
            this.cDBPath.Name = "cDBPath";
            this.cDBPath.Size = new System.Drawing.Size(429, 21);
            this.cDBPath.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Dock = System.Windows.Forms.DockStyle.Top;
            this.label1.Location = new System.Drawing.Point(10, 74);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(157, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "PeopleFinder Offline Database:";
            // 
            // panel2
            // 
            this.panel2.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel2.Location = new System.Drawing.Point(10, 64);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(528, 10);
            this.panel2.TabIndex = 2;
            // 
            // panel1
            // 
            this.panel1.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel1.Location = new System.Drawing.Point(0, 64);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(10, 282);
            this.panel1.TabIndex = 1;
            // 
            // header1
            // 
            this.header1.BackColor = System.Drawing.SystemColors.Control;
            this.header1.CausesValidation = false;
            this.header1.Description = "Please enter the Input and Output paths for the Wizard.";
            this.header1.Dock = System.Windows.Forms.DockStyle.Top;
            this.header1.Image = ((System.Drawing.Image)(resources.GetObject("header1.Image")));
            this.header1.Location = new System.Drawing.Point(0, 0);
            this.header1.Name = "header1";
            this.header1.Size = new System.Drawing.Size(538, 64);
            this.header1.TabIndex = 0;
            this.header1.Title = "Data Collection";
            // 
            // fWizard
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(538, 394);
            this.ControlBox = false;
            this.Controls.Add(this.pfocWizard);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimumSize = new System.Drawing.Size(540, 396);
            this.Name = "fWizard";
            this.Text = "PeopleFinder Offline Database to Palm Database Converter";
            this.pfocWizard.ResumeLayout(false);
            this.wpWelcome.ResumeLayout(false);
            this.wpFinal.ResumeLayout(false);
            this.wpFinal.PerformLayout();
            this.wpProcess.ResumeLayout(false);
            this.wpProcess.PerformLayout();
            this.wpData.ResumeLayout(false);
            this.wpData.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private Gui.Wizard.Wizard pfocWizard;
        private Gui.Wizard.WizardPage wpFinal;
        private Gui.Wizard.WizardPage wpProcess;
        private Gui.Wizard.WizardPage wpData;
        private Gui.Wizard.WizardPage wpWelcome;
        private Gui.Wizard.InfoPage wpInfo;
        private Gui.Wizard.Header header1;
        private System.Windows.Forms.TextBox cPDBPath;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox cDBPath;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button cBrowsePDB;
        private System.Windows.Forms.Button cBrowseDB;
        private System.Windows.Forms.Panel panel4;
        private System.Windows.Forms.OpenFileDialog ofDialog;
        private System.Windows.Forms.FolderBrowserDialog fbDialog;
        private Gui.Wizard.Header header2;
        private System.Windows.Forms.TextBox cReport;
        private Gui.Wizard.Header header3;
        public System.Windows.Forms.TextBox cStatus;
        public System.Windows.Forms.ProgressBar cProgressBar;
        public System.Windows.Forms.ComboBox cProfile;
        private System.Windows.Forms.CheckBox cInstall;
    }
}


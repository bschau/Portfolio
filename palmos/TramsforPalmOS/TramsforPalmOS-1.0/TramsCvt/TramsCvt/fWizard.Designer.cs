namespace TramsCvt
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
            this.wizard = new Gui.Wizard.Wizard();
            this.wizWelcome = new Gui.Wizard.WizardPage();
            this.infoPage = new Gui.Wizard.InfoPage();
            this.wizFinish = new Gui.Wizard.WizardPage();
            this.cReport = new System.Windows.Forms.Label();
            this.header3 = new Gui.Wizard.Header();
            this.wizWorking = new Gui.Wizard.WizardPage();
            this.cStatus = new System.Windows.Forms.Label();
            this.cProgress = new System.Windows.Forms.ProgressBar();
            this.header2 = new Gui.Wizard.Header();
            this.wizDataColl = new Gui.Wizard.WizardPage();
            this.cProfile = new System.Windows.Forms.ComboBox();
            this.cInstall = new System.Windows.Forms.CheckBox();
            this.bBrowse2 = new System.Windows.Forms.Button();
            this.bBrowse1 = new System.Windows.Forms.Button();
            this.panel3 = new System.Windows.Forms.Panel();
            this.cPDBPath = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.panel2 = new System.Windows.Forms.Panel();
            this.cDBPath = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.header1 = new Gui.Wizard.Header();
            this.wizard.SuspendLayout();
            this.wizWelcome.SuspendLayout();
            this.wizFinish.SuspendLayout();
            this.wizWorking.SuspendLayout();
            this.wizDataColl.SuspendLayout();
            this.SuspendLayout();
            // 
            // ofDialog
            // 
            this.ofDialog.DefaultExt = "dat";
            this.ofDialog.FileName = "ExpCat.dat";
            this.ofDialog.Filter = "Trams Mobile files|*.dat|All files|*.*";
            this.ofDialog.InitialDirectory = "c:\\Trams\\mobile\\traprg";
            this.ofDialog.ReadOnlyChecked = true;
            this.ofDialog.Title = "Select Trams Mobile Database";
            // 
            // fbDialog
            // 
            this.fbDialog.Description = "Select Output Folder";
            // 
            // wizard
            // 
            this.wizard.Controls.Add(this.wizWelcome);
            this.wizard.Controls.Add(this.wizFinish);
            this.wizard.Controls.Add(this.wizWorking);
            this.wizard.Controls.Add(this.wizDataColl);
            this.wizard.Dock = System.Windows.Forms.DockStyle.Fill;
            this.wizard.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.wizard.Location = new System.Drawing.Point(0, 0);
            this.wizard.Name = "wizard";
            this.wizard.Pages.AddRange(new Gui.Wizard.WizardPage[] {
            this.wizWelcome,
            this.wizDataColl,
            this.wizWorking,
            this.wizFinish});
            this.wizard.Size = new System.Drawing.Size(523, 436);
            this.wizard.TabIndex = 0;
            this.wizard.CloseFromCancel += new System.ComponentModel.CancelEventHandler(this.wizard_CloseFromCancel);
            // 
            // wizWelcome
            // 
            this.wizWelcome.Controls.Add(this.infoPage);
            this.wizWelcome.Dock = System.Windows.Forms.DockStyle.Fill;
            this.wizWelcome.IsFinishPage = false;
            this.wizWelcome.Location = new System.Drawing.Point(0, 0);
            this.wizWelcome.Name = "wizWelcome";
            this.wizWelcome.Size = new System.Drawing.Size(523, 388);
            this.wizWelcome.TabIndex = 1;
            // 
            // infoPage
            // 
            this.infoPage.BackColor = System.Drawing.Color.White;
            this.infoPage.Dock = System.Windows.Forms.DockStyle.Fill;
            this.infoPage.Image = ((System.Drawing.Image)(resources.GetObject("infoPage.Image")));
            this.infoPage.Location = new System.Drawing.Point(0, 0);
            this.infoPage.Name = "infoPage";
            this.infoPage.PageText = "This wizard enables you to convert the Expenses Category (ExpCat.dat) file from T" +
                "rams Mobile to a format suitable for Trams for Palm OS.";
            this.infoPage.PageTitle = "Welcome to the Trams Mobile database to Palm OS database converter";
            this.infoPage.Size = new System.Drawing.Size(523, 388);
            this.infoPage.TabIndex = 0;
            // 
            // wizFinish
            // 
            this.wizFinish.Controls.Add(this.cReport);
            this.wizFinish.Controls.Add(this.header3);
            this.wizFinish.Dock = System.Windows.Forms.DockStyle.Fill;
            this.wizFinish.IsFinishPage = true;
            this.wizFinish.Location = new System.Drawing.Point(0, 0);
            this.wizFinish.Name = "wizFinish";
            this.wizFinish.Size = new System.Drawing.Size(523, 388);
            this.wizFinish.TabIndex = 4;
            this.wizFinish.CloseFromBack += new Gui.Wizard.PageEventHandler(this.wizFinish_CloseFromBack);
            this.wizFinish.ShowFromNext += new System.EventHandler(this.wizFinish_ShowFromNext);
            // 
            // cReport
            // 
            this.cReport.Font = new System.Drawing.Font("Tahoma", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cReport.Location = new System.Drawing.Point(12, 170);
            this.cReport.Name = "cReport";
            this.cReport.Size = new System.Drawing.Size(499, 94);
            this.cReport.TabIndex = 2;
            this.cReport.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // header3
            // 
            this.header3.BackColor = System.Drawing.SystemColors.Control;
            this.header3.CausesValidation = false;
            this.header3.Description = "Conversion Finished!";
            this.header3.Dock = System.Windows.Forms.DockStyle.Top;
            this.header3.Image = ((System.Drawing.Image)(resources.GetObject("header3.Image")));
            this.header3.Location = new System.Drawing.Point(0, 0);
            this.header3.Name = "header3";
            this.header3.Size = new System.Drawing.Size(523, 64);
            this.header3.TabIndex = 1;
            this.header3.Title = "Finished";
            // 
            // wizWorking
            // 
            this.wizWorking.Controls.Add(this.cStatus);
            this.wizWorking.Controls.Add(this.cProgress);
            this.wizWorking.Controls.Add(this.header2);
            this.wizWorking.Dock = System.Windows.Forms.DockStyle.Fill;
            this.wizWorking.IsFinishPage = false;
            this.wizWorking.Location = new System.Drawing.Point(0, 0);
            this.wizWorking.Name = "wizWorking";
            this.wizWorking.Size = new System.Drawing.Size(523, 388);
            this.wizWorking.TabIndex = 3;
            this.wizWorking.ShowFromNext += new System.EventHandler(this.wizWorking_ShowFromNext);
            // 
            // cStatus
            // 
            this.cStatus.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cStatus.Location = new System.Drawing.Point(24, 174);
            this.cStatus.Name = "cStatus";
            this.cStatus.Size = new System.Drawing.Size(465, 23);
            this.cStatus.TabIndex = 3;
            this.cStatus.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // cProgress
            // 
            this.cProgress.Location = new System.Drawing.Point(27, 216);
            this.cProgress.Name = "cProgress";
            this.cProgress.Size = new System.Drawing.Size(462, 23);
            this.cProgress.Style = System.Windows.Forms.ProgressBarStyle.Marquee;
            this.cProgress.TabIndex = 2;
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
            this.header2.Size = new System.Drawing.Size(523, 64);
            this.header2.TabIndex = 0;
            this.header2.Title = "Working";
            // 
            // wizDataColl
            // 
            this.wizDataColl.Controls.Add(this.cProfile);
            this.wizDataColl.Controls.Add(this.cInstall);
            this.wizDataColl.Controls.Add(this.bBrowse2);
            this.wizDataColl.Controls.Add(this.bBrowse1);
            this.wizDataColl.Controls.Add(this.panel3);
            this.wizDataColl.Controls.Add(this.cPDBPath);
            this.wizDataColl.Controls.Add(this.label2);
            this.wizDataColl.Controls.Add(this.panel2);
            this.wizDataColl.Controls.Add(this.cDBPath);
            this.wizDataColl.Controls.Add(this.label1);
            this.wizDataColl.Controls.Add(this.panel1);
            this.wizDataColl.Controls.Add(this.header1);
            this.wizDataColl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.wizDataColl.IsFinishPage = false;
            this.wizDataColl.Location = new System.Drawing.Point(0, 0);
            this.wizDataColl.Name = "wizDataColl";
            this.wizDataColl.Size = new System.Drawing.Size(523, 388);
            this.wizDataColl.TabIndex = 2;
            this.wizDataColl.CloseFromNext += new Gui.Wizard.PageEventHandler(this.wizDataColl_CloseFromNext);
            this.wizDataColl.ShowFromNext += new System.EventHandler(this.wizDataColl_ShowFromNext);
            // 
            // cProfile
            // 
            this.cProfile.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cProfile.FormattingEnabled = true;
            this.cProfile.Location = new System.Drawing.Point(151, 211);
            this.cProfile.Name = "cProfile";
            this.cProfile.Size = new System.Drawing.Size(279, 21);
            this.cProfile.TabIndex = 11;
            // 
            // cInstall
            // 
            this.cInstall.AutoSize = true;
            this.cInstall.Location = new System.Drawing.Point(15, 211);
            this.cInstall.Name = "cInstall";
            this.cInstall.Size = new System.Drawing.Size(130, 17);
            this.cInstall.TabIndex = 10;
            this.cInstall.Text = "Install to Palm Profile:";
            this.cInstall.UseVisualStyleBackColor = true;
            this.cInstall.CheckedChanged += new System.EventHandler(this.cInstall_CheckedChanged);
            // 
            // bBrowse2
            // 
            this.bBrowse2.Location = new System.Drawing.Point(436, 147);
            this.bBrowse2.Name = "bBrowse2";
            this.bBrowse2.Size = new System.Drawing.Size(75, 23);
            this.bBrowse2.TabIndex = 9;
            this.bBrowse2.Text = "Browse...";
            this.bBrowse2.UseVisualStyleBackColor = true;
            this.bBrowse2.Click += new System.EventHandler(this.bBrowse2_Click);
            // 
            // bBrowse1
            // 
            this.bBrowse1.Location = new System.Drawing.Point(436, 93);
            this.bBrowse1.Name = "bBrowse1";
            this.bBrowse1.Size = new System.Drawing.Size(75, 23);
            this.bBrowse1.TabIndex = 8;
            this.bBrowse1.Text = "Browse...";
            this.bBrowse1.UseVisualStyleBackColor = true;
            this.bBrowse1.Click += new System.EventHandler(this.bBrowse1_Click);
            // 
            // panel3
            // 
            this.panel3.Location = new System.Drawing.Point(3, 176);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(200, 29);
            this.panel3.TabIndex = 7;
            // 
            // cPDBPath
            // 
            this.cPDBPath.Location = new System.Drawing.Point(15, 149);
            this.cPDBPath.Name = "cPDBPath";
            this.cPDBPath.Size = new System.Drawing.Size(415, 21);
            this.cPDBPath.TabIndex = 6;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 133);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(144, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "Palm Database Output Path:";
            // 
            // panel2
            // 
            this.panel2.Location = new System.Drawing.Point(3, 120);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(200, 10);
            this.panel2.TabIndex = 4;
            // 
            // cDBPath
            // 
            this.cDBPath.Location = new System.Drawing.Point(15, 93);
            this.cDBPath.Name = "cDBPath";
            this.cDBPath.Size = new System.Drawing.Size(415, 21);
            this.cDBPath.TabIndex = 3;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 77);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(219, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Trams Mobile Expenses Category Database:";
            // 
            // panel1
            // 
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(0, 64);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(523, 10);
            this.panel1.TabIndex = 1;
            // 
            // header1
            // 
            this.header1.BackColor = System.Drawing.SystemColors.Control;
            this.header1.CausesValidation = false;
            this.header1.Description = "Please enter the Input and Output paths for the Wizard";
            this.header1.Dock = System.Windows.Forms.DockStyle.Top;
            this.header1.Image = ((System.Drawing.Image)(resources.GetObject("header1.Image")));
            this.header1.Location = new System.Drawing.Point(0, 0);
            this.header1.Name = "header1";
            this.header1.Size = new System.Drawing.Size(523, 64);
            this.header1.TabIndex = 0;
            this.header1.Title = "Data Collection";
            // 
            // fWizard
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(523, 436);
            this.ControlBox = false;
            this.Controls.Add(this.wizard);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximumSize = new System.Drawing.Size(529, 461);
            this.MinimumSize = new System.Drawing.Size(529, 461);
            this.Name = "fWizard";
            this.ShowIcon = false;
            this.Text = "Trams Mobile Database to Palm OS Database Converter";
            this.wizard.ResumeLayout(false);
            this.wizWelcome.ResumeLayout(false);
            this.wizFinish.ResumeLayout(false);
            this.wizWorking.ResumeLayout(false);
            this.wizDataColl.ResumeLayout(false);
            this.wizDataColl.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private Gui.Wizard.Wizard wizard;
        private Gui.Wizard.WizardPage wizWelcome;
        private Gui.Wizard.WizardPage wizDataColl;
        private Gui.Wizard.WizardPage wizFinish;
        private Gui.Wizard.InfoPage infoPage;
        private Gui.Wizard.Header header1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.TextBox cDBPath;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button bBrowse2;
        private System.Windows.Forms.Button bBrowse1;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.TextBox cPDBPath;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.OpenFileDialog ofDialog;
        private System.Windows.Forms.FolderBrowserDialog fbDialog;
        private Gui.Wizard.Header header2;
        public Gui.Wizard.WizardPage wizWorking;
        public System.Windows.Forms.ProgressBar cProgress;
        private System.Windows.Forms.ComboBox cProfile;
        private System.Windows.Forms.CheckBox cInstall;
        private Gui.Wizard.Header header3;
        public System.Windows.Forms.Label cReport;
        public System.Windows.Forms.Label cStatus;
    }
}


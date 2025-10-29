using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.IO;

namespace iSecurViewer
{
	/// <summary>
	/// Main form.
	/// </summary>
	public class fMain : System.Windows.Forms.Form
	{
       	private Palm palm = null;
		private Config config=null;
		private PDB pdb=null;
		private ArrayList recList=null;
		private int categoryIdx=0, recListEntries=0;
		private System.Windows.Forms.StatusBar cMainStatBar;
		private System.Windows.Forms.ComboBox cMainCategory;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.MainMenu mMain;
		private System.Windows.Forms.MenuItem menuItem1;
		private System.Windows.Forms.MenuItem mMainFileOpen;
		private System.Windows.Forms.MenuItem mMenuFileExit;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Panel panel2;
		private System.Windows.Forms.OpenFileDialog ofdDatabase;
		private System.Windows.Forms.MenuItem menuItem2;
		private System.Windows.Forms.MenuItem mMainHelpAbout;
		private System.Windows.Forms.Panel panel4;
		private System.Windows.Forms.ListBox cMainListBox;
		private System.Windows.Forms.RadioButton rbSecrets;
		private System.Windows.Forms.RadioButton rbTemplates;
		private System.Windows.Forms.RadioButton rbAll;
		private System.Windows.Forms.Panel panel3;
	    private MenuItem mMainProfileOpen;
		private MenuItem menuItem5;
		private System.ComponentModel.IContainer components=null;

		public fMain()
		{
			InitializeComponent();

			config=new Config();
			config.Load();

 	        if (Environment.OSVersion.ToString().IndexOf("Windows")>-1)
 	        {
            	palm = new Palm();
                palm.GetUsers();
                
                if (palm.users!=null)
	                mMainProfileOpen.Enabled = true;

                if ((config.currentDB == null) || (config.currentDB.Length == 0))
                {
                	if ((config.profile==null) || (config.profile.Length==0))
                		config.profile = GetProfile(null);
                	
                    if ((config.profile!=null) && (config.profile.Length>0)) {
                    	config.currentDB = palm.GetBackupDir(config.profile) + "\\iSec_iSecur_Data.pdb";

                    	if (File.Exists(config.currentDB) == false)
                        {
                        	MessageBox.Show("The selected user has not yet HotSync'ed the iSecur database - please locate the database file manually.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Stop);
                            config.currentDB = "";
                        }
                    }
            	}
 	        }

 	        if ((config.currentDB == null) || (config.currentDB.Length == 0))
        	{
            	if (ofdDatabase.ShowDialog() == DialogResult.OK)
            		config.currentDB = ofdDatabase.FileName;
            }

            if ((config.currentDB != null) && (config.currentDB.Length > 0))
            {
            	if (File.Exists(config.currentDB) == false)
            	{
                	if (ofdDatabase.ShowDialog() == DialogResult.OK)
                    {
                    	LoadDatabase(ofdDatabase.FileName);
                    }
                } else
                	LoadDatabase(config.currentDB);
            }

			switch (config.showIdx)
			{
				case 1:
					rbSecrets.Checked=true;
					break;
				case 2:
					rbTemplates.Checked=true;
					break;
				default:
					rbAll.Checked=true;
					break;
			}
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(fMain));
			this.cMainStatBar = new System.Windows.Forms.StatusBar();
			this.label1 = new System.Windows.Forms.Label();
			this.cMainCategory = new System.Windows.Forms.ComboBox();
			this.mMain = new System.Windows.Forms.MainMenu(this.components);
			this.menuItem1 = new System.Windows.Forms.MenuItem();
			this.mMainFileOpen = new System.Windows.Forms.MenuItem();
			this.mMainProfileOpen = new System.Windows.Forms.MenuItem();
			this.menuItem5 = new System.Windows.Forms.MenuItem();
			this.mMenuFileExit = new System.Windows.Forms.MenuItem();
			this.menuItem2 = new System.Windows.Forms.MenuItem();
			this.mMainHelpAbout = new System.Windows.Forms.MenuItem();
			this.panel1 = new System.Windows.Forms.Panel();
			this.panel3 = new System.Windows.Forms.Panel();
			this.rbAll = new System.Windows.Forms.RadioButton();
			this.rbSecrets = new System.Windows.Forms.RadioButton();
			this.rbTemplates = new System.Windows.Forms.RadioButton();
			this.panel2 = new System.Windows.Forms.Panel();
			this.panel4 = new System.Windows.Forms.Panel();
			this.cMainListBox = new System.Windows.Forms.ListBox();
			this.ofdDatabase = new System.Windows.Forms.OpenFileDialog();
			this.panel1.SuspendLayout();
			this.panel3.SuspendLayout();
			this.panel2.SuspendLayout();
			this.panel4.SuspendLayout();
			this.SuspendLayout();
			// 
			// cMainStatBar
			// 
			this.cMainStatBar.Location = new System.Drawing.Point(0, 516);
			this.cMainStatBar.Name = "cMainStatBar";
			this.cMainStatBar.Size = new System.Drawing.Size(536, 16);
			this.cMainStatBar.TabIndex = 0;
			this.cMainStatBar.Text = "iSecurViewer-1.5 by Brian Schau (http://www.schau.com/)";
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(8, 6);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(56, 14);
			this.label1.TabIndex = 1;
			this.label1.Text = "Category:";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// cMainCategory
			// 
			this.cMainCategory.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cMainCategory.Location = new System.Drawing.Point(64, 6);
			this.cMainCategory.Name = "cMainCategory";
			this.cMainCategory.Size = new System.Drawing.Size(168, 21);
			this.cMainCategory.TabIndex = 0;
			this.cMainCategory.SelectedIndexChanged += new System.EventHandler(this.cMainCategory_SelectedIndexChanged);
			// 
			// mMain
			// 
			this.mMain.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
									this.menuItem1,
									this.menuItem2});
			// 
			// menuItem1
			// 
			this.menuItem1.Index = 0;
			this.menuItem1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
									this.mMainFileOpen,
									this.mMainProfileOpen,
									this.menuItem5,
									this.mMenuFileExit});
			this.menuItem1.Text = "&File";
			// 
			// mMainFileOpen
			// 
			this.mMainFileOpen.Index = 0;
			this.mMainFileOpen.Text = "&Open Database";
			this.mMainFileOpen.Click += new System.EventHandler(this.mMainFileOpen_Click);
			// 
			// mMainProfileOpen
			// 
			this.mMainProfileOpen.Enabled = false;
			this.mMainProfileOpen.Index = 1;
			this.mMainProfileOpen.Text = "Open &Profile";
			this.mMainProfileOpen.Click += new System.EventHandler(this.mMainProfileOpen_Click);
			// 
			// menuItem5
			// 
			this.menuItem5.Index = 2;
			this.menuItem5.Text = "-";
			// 
			// mMenuFileExit
			// 
			this.mMenuFileExit.Index = 3;
			this.mMenuFileExit.Text = "E&xit";
			this.mMenuFileExit.Click += new System.EventHandler(this.mMenuFileExit_Click);
			// 
			// menuItem2
			// 
			this.menuItem2.Index = 1;
			this.menuItem2.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
									this.mMainHelpAbout});
			this.menuItem2.Text = "Help";
			// 
			// mMainHelpAbout
			// 
			this.mMainHelpAbout.Index = 0;
			this.mMainHelpAbout.Text = "About";
			this.mMainHelpAbout.Click += new System.EventHandler(this.mMainHelpAbout_Click);
			// 
			// panel1
			// 
			this.panel1.Controls.Add(this.panel3);
			this.panel1.Controls.Add(this.label1);
			this.panel1.Controls.Add(this.cMainCategory);
			this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
			this.panel1.Location = new System.Drawing.Point(0, 0);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(536, 30);
			this.panel1.TabIndex = 4;
			// 
			// panel3
			// 
			this.panel3.Controls.Add(this.rbAll);
			this.panel3.Controls.Add(this.rbSecrets);
			this.panel3.Controls.Add(this.rbTemplates);
			this.panel3.Dock = System.Windows.Forms.DockStyle.Right;
			this.panel3.Location = new System.Drawing.Point(336, 0);
			this.panel3.Name = "panel3";
			this.panel3.Size = new System.Drawing.Size(200, 30);
			this.panel3.TabIndex = 5;
			// 
			// rbAll
			// 
			this.rbAll.Checked = true;
			this.rbAll.Location = new System.Drawing.Point(16, 6);
			this.rbAll.Name = "rbAll";
			this.rbAll.Size = new System.Drawing.Size(40, 18);
			this.rbAll.TabIndex = 4;
			this.rbAll.TabStop = true;
			this.rbAll.Text = "All";
			this.rbAll.CheckedChanged += new System.EventHandler(this.rbAll_CheckedChanged);
			// 
			// rbSecrets
			// 
			this.rbSecrets.Location = new System.Drawing.Point(56, 6);
			this.rbSecrets.Name = "rbSecrets";
			this.rbSecrets.Size = new System.Drawing.Size(64, 18);
			this.rbSecrets.TabIndex = 2;
			this.rbSecrets.Text = "Secrets";
			this.rbSecrets.CheckedChanged += new System.EventHandler(this.rbSecrets_CheckedChanged);
			// 
			// rbTemplates
			// 
			this.rbTemplates.Location = new System.Drawing.Point(120, 6);
			this.rbTemplates.Name = "rbTemplates";
			this.rbTemplates.Size = new System.Drawing.Size(80, 18);
			this.rbTemplates.TabIndex = 3;
			this.rbTemplates.Text = "Templates";
			this.rbTemplates.CheckedChanged += new System.EventHandler(this.rbTemplates_CheckedChanged);
			// 
			// panel2
			// 
			this.panel2.Controls.Add(this.panel4);
			this.panel2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel2.Location = new System.Drawing.Point(0, 30);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(536, 486);
			this.panel2.TabIndex = 5;
			// 
			// panel4
			// 
			this.panel4.Controls.Add(this.cMainListBox);
			this.panel4.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel4.Location = new System.Drawing.Point(0, 0);
			this.panel4.Name = "panel4";
			this.panel4.Size = new System.Drawing.Size(536, 486);
			this.panel4.TabIndex = 2;
			// 
			// cMainListBox
			// 
			this.cMainListBox.Dock = System.Windows.Forms.DockStyle.Fill;
			this.cMainListBox.Location = new System.Drawing.Point(0, 0);
			this.cMainListBox.Name = "cMainListBox";
			this.cMainListBox.Size = new System.Drawing.Size(536, 485);
			this.cMainListBox.TabIndex = 0;
			this.cMainListBox.DoubleClick += new System.EventHandler(this.cMainListBox_DoubleClick);
			// 
			// ofdDatabase
			// 
			this.ofdDatabase.DefaultExt = "pdb";
			this.ofdDatabase.FileName = "iSec_iSecur_Data.pdb";
			this.ofdDatabase.Filter = "Palm Database Files (*.pdb)|*.pdb";
			this.ofdDatabase.Title = "Open iSecur Secrets Database";
			// 
			// fMain
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.AutoScroll = true;
			this.ClientSize = new System.Drawing.Size(536, 532);
			this.Controls.Add(this.panel2);
			this.Controls.Add(this.panel1);
			this.Controls.Add(this.cMainStatBar);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Menu = this.mMain;
			this.MinimumSize = new System.Drawing.Size(440, 149);
			this.Name = "fMain";
			this.Text = "iSecurViewer";
			this.Closing += new System.ComponentModel.CancelEventHandler(this.fMain_Closing);
			this.panel1.ResumeLayout(false);
			this.panel3.ResumeLayout(false);
			this.panel2.ResumeLayout(false);
			this.panel4.ResumeLayout(false);
			this.ResumeLayout(false);
		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new fMain());
		}

		/// <summary>
		/// Stuff to do when closing iSecurViewer.
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void fMain_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			string cName=null;
		
			if (pdb!=null) 
			{
				if (categoryIdx==0xff)
					cName="All";
				else
					cName=pdb.catMgr.GetCategoryName(categoryIdx);
			}

			if (config!=null) 
			{
				config.categoryName=cName;
				config.Save();
			}
		}

		private void mMenuFileExit_Click(object sender, System.EventArgs e)
		{
			Close();
		}

		private void mMainFileOpen_Click(object sender, System.EventArgs e)
		{
			if (ofdDatabase.ShowDialog()==DialogResult.OK) 
			{
				LoadDatabase(ofdDatabase.FileName);
			}
		}

		/// <summary>
		/// Load the database to work on.
		/// </summary>
		/// <param name="dbName">Database name.</param>
		private void LoadDatabase(string dbName)
		{
			fPassword p=null;
			
			cMainCategory.Items.Clear();
			cMainListBox.Items.Clear();

			try
			{
				pdb=new PDB();
			
				pdb.PreLoad(dbName, "DATA", "iSec");
				
				p=new fPassword(dbName, pdb);

				p.ShowDialog();
				if ((p.password!=null) && (p.password.Length>0)) 
				{
					pdb.Load(p.password);
					config.currentDB=dbName;
					cMainStatBar.Text=pdb.Length+" Records in database";

					categoryIdx=pdb.catMgr.GetCategoryIndex(config.categoryName);
					SetupCategories();

					cMainCategory.SelectedIndex=categoryIdx;
					ChangeCategory();
				} 
			} catch (Exception ex) {
				MessageBox.Show(ex.Message, "Load Error", MessageBoxButtons.OK);
				System.Environment.Exit(1);
			} finally {
				if (p!=null) {
					if ((p.password!=null) && (p.password.Length>0)) 
						p.password.Remove(0, p.password.Length);
					p.Dispose();
				}
			} 
		}

		/// <summary>
		/// Setup the categories from the database.
		/// </summary>
		private void SetupCategories()
		{
			int i;

			cMainCategory.BeginUpdate();
			cMainCategory.Items.Clear();
			cMainCategory.Items.Add("All");

			for (i=0; i<pdb.catMgr.sortedCategories.Count; i++) 
			{
				cMainCategory.Items.Add((string)pdb.catMgr.sortedCategories.GetKey(i));
			}

			cMainCategory.Items.Add(pdb.catMgr.GetCategoryName(0));

			if (categoryIdx<0 || categoryIdx>cMainCategory.Items.Count)
				categoryIdx=0;
			
			cMainCategory.EndUpdate();
		}

		/// <summary>
		/// Rethink view when a new category is selected.
		/// </summary>
		private void ChangeCategory()
		{
			int rListCnt=0;

			if (pdb!=null) 
			{
				int i;
				Record r;

				cMainListBox.BeginUpdate();
				cMainListBox.Items.Clear();
				recList=new ArrayList(pdb.Length);
				for (i=0; i<pdb.Length; i++) 
				{
					if ((r=pdb.GetRecord(i))!=null) 
					{
						if ((config.showIdx==0) ||
							((config.showIdx==1) && (r.rTemplate==false)) ||
							((config.showIdx==2) && (r.rTemplate==true))) 
						{
							if ((categoryIdx==0xff) || (r.rCategory==categoryIdx)) 
							{
								if (r.rTemplate)
									cMainListBox.Items.Add("["+r.rTitle+"]");
								else
									cMainListBox.Items.Add(r.rTitle);

								recList.Add(i);
								rListCnt++;
							}
						}
					}
				}
				
				cMainListBox.EndUpdate();
			}
		
			recListEntries=rListCnt;
		}

		private void cMainCategory_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			if (cMainCategory.SelectedIndex==0)
				categoryIdx=0xff;
			else if (cMainCategory.SelectedIndex==(cMainCategory.Items.Count-1))
				categoryIdx=0;
			else
				categoryIdx=cMainCategory.SelectedIndex;

			ChangeCategory();
		}

		private void mMainHelpAbout_Click(object sender, System.EventArgs e)
		{
			fAbout f=new fAbout();

			f.ShowDialog();
			f.Dispose();
		}

		private void cMainListBox_DoubleClick(object sender, System.EventArgs e)
		{
			if (cMainListBox.SelectedIndex>-1 && recListEntries>0 && pdb!=null)
			{
				Record r=pdb.GetRecord((int)recList[cMainListBox.SelectedIndex]);
				fShow f=new fShow(pdb, r, r.rCategory);
				f.ShowDialog();
				f.Dispose();
			}
		}

		private void rbAll_CheckedChanged(object sender, System.EventArgs e)
		{
			config.showIdx=0;
			ChangeCategory();
		}

		private void rbSecrets_CheckedChanged(object sender, System.EventArgs e)
		{
			config.showIdx=1;
			ChangeCategory();
		}

		private void rbTemplates_CheckedChanged(object sender, System.EventArgs e)
		{
			config.showIdx=2;
			ChangeCategory();
		}

		/// <summary>
		/// Get new profile name.
		/// </summary>
		/// <param name="profile">Current profile (or null).</param>
		/// <returns>Selected profile or null.</returns>
		private string GetProfile(string profile)
		{
			string p=null;
			
			if (palm.users!=null) {
        		fProfile f = new fProfile(palm, profile);

	        	f.ShowDialog();
       			if (f.okClicked)
       				p = f.GetSelection();

	        	f.Dispose();
			}

        	return p;
		}

		private void mMainProfileOpen_Click(object sender, EventArgs e)
		{
			string p = GetProfile(config.profile);

			if ((p != null) && (p.Length > 0))
			{
				string db = palm.GetBackupDir(p) + "\\iSec_iSecur_Data.pdb";

				if (File.Exists(db) == true)
				{
					config.profile = p;
					config.currentDB = db;
					LoadDatabase(db);
				}
				else
					MessageBox.Show("The selected user has not yet HotSync'ed the iSecur database.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Stop);
			}
		}
	}
}

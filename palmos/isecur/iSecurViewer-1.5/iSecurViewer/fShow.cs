using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace iSecurViewer
{
	/// <summary>
	/// Edit form.
	/// </summary>
	public class fShow : System.Windows.Forms.Form
	{
		private PDB pdb;
		private System.Windows.Forms.Panel panel4;
		private System.Windows.Forms.Panel panel5;
		private System.Windows.Forms.Panel panel7;
		private System.Windows.Forms.Panel panel8;
		private System.Windows.Forms.Panel panel9;
		private System.Windows.Forms.Panel panel12;
		private System.Windows.Forms.Panel panel13;
		private System.Windows.Forms.Panel panel14;
		private System.Windows.Forms.Panel panel15;
		private System.Windows.Forms.Panel panel16;
		private System.Windows.Forms.Panel panel17;
		private System.Windows.Forms.Panel panel18;
		private System.Windows.Forms.Panel panel19;
		private System.Windows.Forms.Button cShowOK;
		private System.Windows.Forms.Label cShowTitle;
		private System.Windows.Forms.TextBox cShowSecret;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public fShow(PDB p, Record r, int cIdx)
		{
			pdb=p;

			InitializeComponent();

			if (r.rTemplate)
				this.Text="Template in "+pdb.catMgr.GetCategoryName(r.rCategory);
			else
				this.Text="Secret in "+pdb.catMgr.GetCategoryName(r.rCategory);

			if (r.rTitle!=null)
				this.cShowTitle.Text=r.rTitle;

			if (r.rSecret!=null) 
			{
				byte[] sec=new byte[r.rSecret.Length];
				uint idx=0;
				int cnt;

				Array.Copy(r.rSecret, sec, sec.Length);

				for (cnt=0; cnt<(sec.Length/32); cnt++) 
				{
					pdb.aes.Decrypt(ref sec, idx);
					pdb.aes.Decrypt(ref sec, idx+16);
					idx+=32;
				}

				for (cnt=0; (cnt<sec.Length) && (sec[cnt]!=0); cnt++);
				cShowSecret.Text=pdb.encoder.GetString(sec, 0, cnt).Replace("\n", "\r\n");
				cShowSecret.SelectionStart=cShowSecret.Text.Length+1;
				cShowSecret.SelectionLength=0;
			}
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(fShow));
			this.panel4 = new System.Windows.Forms.Panel();
			this.panel5 = new System.Windows.Forms.Panel();
			this.cShowTitle = new System.Windows.Forms.Label();
			this.panel8 = new System.Windows.Forms.Panel();
			this.panel7 = new System.Windows.Forms.Panel();
			this.panel9 = new System.Windows.Forms.Panel();
			this.panel12 = new System.Windows.Forms.Panel();
			this.panel13 = new System.Windows.Forms.Panel();
			this.cShowOK = new System.Windows.Forms.Button();
			this.panel15 = new System.Windows.Forms.Panel();
			this.panel14 = new System.Windows.Forms.Panel();
			this.panel16 = new System.Windows.Forms.Panel();
			this.panel17 = new System.Windows.Forms.Panel();
			this.cShowSecret = new System.Windows.Forms.TextBox();
			this.panel19 = new System.Windows.Forms.Panel();
			this.panel18 = new System.Windows.Forms.Panel();
			this.panel5.SuspendLayout();
			this.panel13.SuspendLayout();
			this.panel17.SuspendLayout();
			this.SuspendLayout();
			// 
			// panel4
			// 
			this.panel4.Dock = System.Windows.Forms.DockStyle.Top;
			this.panel4.Location = new System.Drawing.Point(0, 0);
			this.panel4.Name = "panel4";
			this.panel4.Size = new System.Drawing.Size(560, 7);
			this.panel4.TabIndex = 1000;
			// 
			// panel5
			// 
			this.panel5.Controls.Add(this.cShowTitle);
			this.panel5.Controls.Add(this.panel8);
			this.panel5.Controls.Add(this.panel7);
			this.panel5.Dock = System.Windows.Forms.DockStyle.Top;
			this.panel5.Location = new System.Drawing.Point(0, 7);
			this.panel5.Name = "panel5";
			this.panel5.Size = new System.Drawing.Size(560, 23);
			this.panel5.TabIndex = 1000;
			// 
			// cShowTitle
			// 
			this.cShowTitle.Dock = System.Windows.Forms.DockStyle.Fill;
			this.cShowTitle.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cShowTitle.Location = new System.Drawing.Point(8, 0);
			this.cShowTitle.Name = "cShowTitle";
			this.cShowTitle.Size = new System.Drawing.Size(544, 23);
			this.cShowTitle.TabIndex = 1001;
			this.cShowTitle.Text = "label1";
			this.cShowTitle.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// panel8
			// 
			this.panel8.Dock = System.Windows.Forms.DockStyle.Right;
			this.panel8.Location = new System.Drawing.Point(552, 0);
			this.panel8.Name = "panel8";
			this.panel8.Size = new System.Drawing.Size(8, 23);
			this.panel8.TabIndex = 1000;
			// 
			// panel7
			// 
			this.panel7.Dock = System.Windows.Forms.DockStyle.Left;
			this.panel7.Location = new System.Drawing.Point(0, 0);
			this.panel7.Name = "panel7";
			this.panel7.Size = new System.Drawing.Size(8, 23);
			this.panel7.TabIndex = 1000;
			// 
			// panel9
			// 
			this.panel9.Dock = System.Windows.Forms.DockStyle.Top;
			this.panel9.Location = new System.Drawing.Point(0, 30);
			this.panel9.Name = "panel9";
			this.panel9.Size = new System.Drawing.Size(560, 7);
			this.panel9.TabIndex = 1000;
			// 
			// panel12
			// 
			this.panel12.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.panel12.Location = new System.Drawing.Point(0, 551);
			this.panel12.Name = "panel12";
			this.panel12.Size = new System.Drawing.Size(560, 7);
			this.panel12.TabIndex = 1000;
			// 
			// panel13
			// 
			this.panel13.Controls.Add(this.cShowOK);
			this.panel13.Controls.Add(this.panel15);
			this.panel13.Controls.Add(this.panel14);
			this.panel13.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.panel13.Location = new System.Drawing.Point(0, 521);
			this.panel13.Name = "panel13";
			this.panel13.Size = new System.Drawing.Size(560, 30);
			this.panel13.TabIndex = 1000;
			// 
			// cShowOK
			// 
			this.cShowOK.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.cShowOK.Location = new System.Drawing.Point(8, 7);
			this.cShowOK.Name = "cShowOK";
			this.cShowOK.Size = new System.Drawing.Size(75, 22);
			this.cShowOK.TabIndex = 3;
			this.cShowOK.Text = "OK";
			this.cShowOK.Click += new System.EventHandler(this.cShowOK_Click);
			// 
			// panel15
			// 
			this.panel15.Dock = System.Windows.Forms.DockStyle.Right;
			this.panel15.Location = new System.Drawing.Point(552, 0);
			this.panel15.Name = "panel15";
			this.panel15.Size = new System.Drawing.Size(8, 30);
			this.panel15.TabIndex = 1000;
			// 
			// panel14
			// 
			this.panel14.Dock = System.Windows.Forms.DockStyle.Left;
			this.panel14.Location = new System.Drawing.Point(0, 0);
			this.panel14.Name = "panel14";
			this.panel14.Size = new System.Drawing.Size(8, 30);
			this.panel14.TabIndex = 1000;
			// 
			// panel16
			// 
			this.panel16.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.panel16.Location = new System.Drawing.Point(0, 514);
			this.panel16.Name = "panel16";
			this.panel16.Size = new System.Drawing.Size(560, 7);
			this.panel16.TabIndex = 1000;
			// 
			// panel17
			// 
			this.panel17.Controls.Add(this.cShowSecret);
			this.panel17.Controls.Add(this.panel19);
			this.panel17.Controls.Add(this.panel18);
			this.panel17.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel17.Location = new System.Drawing.Point(0, 37);
			this.panel17.Name = "panel17";
			this.panel17.Size = new System.Drawing.Size(560, 477);
			this.panel17.TabIndex = 1000;
			// 
			// cShowSecret
			// 
			this.cShowSecret.BackColor = System.Drawing.SystemColors.Control;
			this.cShowSecret.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.cShowSecret.Cursor = System.Windows.Forms.Cursors.Default;
			this.cShowSecret.Dock = System.Windows.Forms.DockStyle.Fill;
			this.cShowSecret.Location = new System.Drawing.Point(8, 0);
			this.cShowSecret.Multiline = true;
			this.cShowSecret.Name = "cShowSecret";
			this.cShowSecret.ReadOnly = true;
			this.cShowSecret.ScrollBars = System.Windows.Forms.ScrollBars.Both;
			this.cShowSecret.Size = new System.Drawing.Size(544, 477);
			this.cShowSecret.TabIndex = 1001;
			// 
			// panel19
			// 
			this.panel19.Dock = System.Windows.Forms.DockStyle.Right;
			this.panel19.Location = new System.Drawing.Point(552, 0);
			this.panel19.Name = "panel19";
			this.panel19.Size = new System.Drawing.Size(8, 477);
			this.panel19.TabIndex = 1000;
			// 
			// panel18
			// 
			this.panel18.Dock = System.Windows.Forms.DockStyle.Left;
			this.panel18.Location = new System.Drawing.Point(0, 0);
			this.panel18.Name = "panel18";
			this.panel18.Size = new System.Drawing.Size(8, 477);
			this.panel18.TabIndex = 1000;
			// 
			// fShow
			// 
			this.AcceptButton = this.cShowOK;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 14);
			this.CancelButton = this.cShowOK;
			this.ClientSize = new System.Drawing.Size(560, 558);
			this.Controls.Add(this.panel17);
			this.Controls.Add(this.panel16);
			this.Controls.Add(this.panel13);
			this.Controls.Add(this.panel12);
			this.Controls.Add(this.panel9);
			this.Controls.Add(this.panel5);
			this.Controls.Add(this.panel4);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MinimumSize = new System.Drawing.Size(300, 232);
			this.Name = "fShow";
			this.Text = "Secret";
			this.panel5.ResumeLayout(false);
			this.panel13.ResumeLayout(false);
			this.panel17.ResumeLayout(false);
			this.panel17.PerformLayout();
			this.ResumeLayout(false);
		}
		#endregion

		private void cShowOK_Click(object sender, System.EventArgs e)
		{
			cShowSecret.Text.Remove(0, cShowSecret.Text.Length);

			Close();
		}
	}
}

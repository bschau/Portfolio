using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace iSecurViewer
{
	/// <summary>
	/// Show the Password form.
	/// </summary>
	public class fPassword : System.Windows.Forms.Form
	{
		public string password;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Button cPasswordOK;
		private System.Windows.Forms.Button cPasswordCancel;
		private System.Windows.Forms.TextBox cPasswordFld;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label cPasswordLabel;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private string hint;

		public fPassword(string lbl, PDB pdb)
		{
			int pos;

			password=null;
			InitializeComponent();
			if ((pos=lbl.LastIndexOf(System.IO.Path.DirectorySeparatorChar))==-1)
				cPasswordLabel.Text=lbl;
			else
				cPasswordLabel.Text=lbl.Substring(pos+1);

			cPasswordFld.Focus();
			
			if ((hint=pdb.GetExtension(72))!=null)	// 'H'
				cPasswordHint.Enabled=true;
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(fPassword));
			this.label1 = new System.Windows.Forms.Label();
			this.cPasswordFld = new System.Windows.Forms.TextBox();
			this.cPasswordOK = new System.Windows.Forms.Button();
			this.cPasswordCancel = new System.Windows.Forms.Button();
			this.label2 = new System.Windows.Forms.Label();
			this.cPasswordLabel = new System.Windows.Forms.Label();
			this.cPasswordHint = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(8, 74);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(272, 15);
			this.label1.TabIndex = 0;
			this.label1.Text = "Please enter password to unlock the database file:";
			// 
			// cPasswordFld
			// 
			this.cPasswordFld.AcceptsReturn = true;
			this.cPasswordFld.Location = new System.Drawing.Point(8, 97);
			this.cPasswordFld.MaxLength = 32;
			this.cPasswordFld.Name = "cPasswordFld";
			this.cPasswordFld.PasswordChar = '*';
			this.cPasswordFld.Size = new System.Drawing.Size(272, 21);
			this.cPasswordFld.TabIndex = 1;
			// 
			// cPasswordOK
			// 
			this.cPasswordOK.Location = new System.Drawing.Point(8, 134);
			this.cPasswordOK.Name = "cPasswordOK";
			this.cPasswordOK.Size = new System.Drawing.Size(75, 21);
			this.cPasswordOK.TabIndex = 2;
			this.cPasswordOK.Text = "OK";
			this.cPasswordOK.Click += new System.EventHandler(this.cPasswordOK_Click);
			// 
			// cPasswordCancel
			// 
			this.cPasswordCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.cPasswordCancel.Location = new System.Drawing.Point(89, 134);
			this.cPasswordCancel.Name = "cPasswordCancel";
			this.cPasswordCancel.Size = new System.Drawing.Size(75, 21);
			this.cPasswordCancel.TabIndex = 3;
			this.cPasswordCancel.Text = "Cancel";
			this.cPasswordCancel.Click += new System.EventHandler(this.cPasswordCancel_Click);
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(8, 15);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(272, 21);
			this.label2.TabIndex = 4;
			this.label2.Text = "You are about to open the password protected file:";
			// 
			// cPasswordLabel
			// 
			this.cPasswordLabel.Location = new System.Drawing.Point(8, 37);
			this.cPasswordLabel.Name = "cPasswordLabel";
			this.cPasswordLabel.Size = new System.Drawing.Size(272, 15);
			this.cPasswordLabel.TabIndex = 5;
			this.cPasswordLabel.Text = "?";
			this.cPasswordLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// cPasswordHint
			// 
			this.cPasswordHint.Enabled = false;
			this.cPasswordHint.Location = new System.Drawing.Point(205, 134);
			this.cPasswordHint.Name = "cPasswordHint";
			this.cPasswordHint.Size = new System.Drawing.Size(75, 23);
			this.cPasswordHint.TabIndex = 6;
			this.cPasswordHint.Text = "Hint ...";
			this.cPasswordHint.UseVisualStyleBackColor = true;
			this.cPasswordHint.Click += new System.EventHandler(this.CPasswordHintClick);
			// 
			// fPassword
			// 
			this.AcceptButton = this.cPasswordOK;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 14);
			this.CancelButton = this.cPasswordCancel;
			this.ClientSize = new System.Drawing.Size(292, 176);
			this.ControlBox = false;
			this.Controls.Add(this.cPasswordHint);
			this.Controls.Add(this.cPasswordLabel);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.cPasswordCancel);
			this.Controls.Add(this.cPasswordOK);
			this.Controls.Add(this.cPasswordFld);
			this.Controls.Add(this.label1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "fPassword";
			this.Text = "Password Required!";
			this.ResumeLayout(false);
			this.PerformLayout();
		}
		private System.Windows.Forms.Button cPasswordHint;
		#endregion

		private void cPasswordOK_Click(object sender, System.EventArgs e)
		{
			if (cPasswordFld.Text.Length > 0)
			{
				password = cPasswordFld.Text;
				cPasswordFld.Text.Remove(0, cPasswordFld.Text.Length);
				Close();
			}
			else
				MessageBox.Show("You must enter a non-blank password.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Stop);
		}

		private void cPasswordCancel_Click(object sender, System.EventArgs e)
		{
			password=null;
			cPasswordFld.Text.Remove(0, cPasswordFld.Text.Length);
			Close();
		}
		
		void CPasswordHintClick(object sender, System.EventArgs e)
		{
			if (hint==null)
				MessageBox.Show("There is no Password Hint in this file.", "Password Hint", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
			else
				MessageBox.Show(hint, "Password Hint", MessageBoxButtons.OK, MessageBoxIcon.Information);
		}
	}
}

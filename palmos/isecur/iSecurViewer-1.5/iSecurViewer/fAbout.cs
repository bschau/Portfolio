using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace iSecurViewer
{
	/// <summary>
	/// Summary description for fAbout.
	/// </summary>
	public class fAbout : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Button cOK;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public fAbout()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(fAbout));
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.cOK = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.Font = new System.Drawing.Font("Arial Black", 20.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label1.Location = new System.Drawing.Point(4, 15);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(280, 42);
			this.label1.TabIndex = 0;
			this.label1.Text = "iSecurViewer-1.5";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// label2
			// 
			this.label2.Font = new System.Drawing.Font("Arial Black", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label2.Location = new System.Drawing.Point(16, 64);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(264, 24);
			this.label2.TabIndex = 1;
			this.label2.Text = "By Brian Schau";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// label3
			// 
			this.label3.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label3.Location = new System.Drawing.Point(24, 96);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(248, 21);
			this.label3.TabIndex = 2;
			this.label3.Text = "Please visit my website at:";
			// 
			// label4
			// 
			this.label4.Font = new System.Drawing.Font("Arial Black", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label4.Location = new System.Drawing.Point(8, 128);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(264, 19);
			this.label4.TabIndex = 3;
			this.label4.Text = "http://www.schau.com/";
			this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// label5
			// 
			this.label5.Location = new System.Drawing.Point(24, 168);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(248, 46);
			this.label5.TabIndex = 4;
			this.label5.Text = "On my website you will find the latest version of iSecurViewer, iSecur, Manuals, " +
			"FAQs and lots of other information.";
			// 
			// cOK
			// 
			this.cOK.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.cOK.Location = new System.Drawing.Point(112, 240);
			this.cOK.Name = "cOK";
			this.cOK.Size = new System.Drawing.Size(75, 21);
			this.cOK.TabIndex = 5;
			this.cOK.Text = "OK";
			this.cOK.Click += new System.EventHandler(this.cOK_Click);
			// 
			// fAbout
			// 
			this.AcceptButton = this.cOK;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 14);
			this.CancelButton = this.cOK;
			this.ClientSize = new System.Drawing.Size(285, 276);
			this.ControlBox = false;
			this.Controls.Add(this.cOK);
			this.Controls.Add(this.label5);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "fAbout";
			this.Text = "About iSecurViewer";
			this.ResumeLayout(false);
		}
		#endregion
		
		private void cOK_Click(object sender, System.EventArgs e)
		{
			Close();
		}
	}
}

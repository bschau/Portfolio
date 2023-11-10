using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace iSecurViewer
{
	public class fProfile : Form
    {
        public bool okClicked=false;
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
        	this.label1 = new System.Windows.Forms.Label();
        	this.cProfile = new System.Windows.Forms.ComboBox();
        	this.cOK = new System.Windows.Forms.Button();
        	this.cCancel = new System.Windows.Forms.Button();
        	this.SuspendLayout();
        	// 
        	// label1
        	// 
        	this.label1.AutoSize = true;
        	this.label1.Location = new System.Drawing.Point(2, 9);
        	this.label1.Name = "label1";
        	this.label1.Size = new System.Drawing.Size(162, 13);
        	this.label1.TabIndex = 0;
        	this.label1.Text = "Please select HotSync ID profile:";
        	// 
        	// cProfile
        	// 
        	this.cProfile.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
        	this.cProfile.FormattingEnabled = true;
        	this.cProfile.Location = new System.Drawing.Point(5, 35);
            this.cProfile.Name = "cProfile";
            this.cProfile.Size = new System.Drawing.Size(240, 21);
            this.cProfile.TabIndex = 1;
            // 
            // cOK
            // 
            this.cOK.Location = new System.Drawing.Point(5, 87);
            this.cOK.Name = "cOK";
            this.cOK.Size = new System.Drawing.Size(75, 23);
            this.cOK.TabIndex = 2;
            this.cOK.Text = "OK";
            this.cOK.UseVisualStyleBackColor = true;
            this.cOK.Click += new System.EventHandler(this.cOK_Click);
            // 
            // cCancel
            // 
            this.cCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cCancel.Location = new System.Drawing.Point(170, 87);
            this.cCancel.Name = "cCancel";
            this.cCancel.Size = new System.Drawing.Size(75, 23);
            this.cCancel.TabIndex = 3;
            this.cCancel.Text = "Cancel";
            this.cCancel.UseVisualStyleBackColor = true;
            this.cCancel.Click += new System.EventHandler(this.cCancel_Click);
            // 
            // fProfile
            // 
            this.AcceptButton = this.cOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.cCancel;
            this.ClientSize = new System.Drawing.Size(249, 124);
            this.ControlBox = false;
            this.Controls.Add(this.cCancel);
            this.Controls.Add(this.cOK);
            this.Controls.Add(this.cProfile);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximumSize = new System.Drawing.Size(255, 149);
            this.MinimumSize = new System.Drawing.Size(255, 149);
            this.Name = "fProfile";
            this.Text = "Select Profile";
            this.ResumeLayout(false);
            this.PerformLayout();
		}
		#endregion

	    private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button cOK;
        private System.Windows.Forms.Button cCancel;
        private System.Windows.Forms.ComboBox cProfile;

        public fProfile(Palm palm, string profile)
        {
           	int selIdx = -1;
            		
           	InitializeComponent();

          	okClicked = false;
	        for (int idx = 0; idx < palm.users.Length; idx++)
    	    {
				if (profile==palm.users[idx])
					selIdx = idx;

				cProfile.Items.Add(palm.users[idx]);
            }	

	        cProfile.SelectedIndex = selIdx;
        }

        private void cOK_Click(object sender, EventArgs e)
        {
        	if (cProfile.SelectedIndex > -1)
            {
             	okClicked = true;
                Close();
            }
            else
             	MessageBox.Show("Please select a profile.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Stop);
        }

        private void cCancel_Click(object sender, EventArgs e)
        {
           	Close();
        }

        public string GetSelection()
        {
            if (cProfile.SelectedIndex == -1)
            	return null;

            return cProfile.Items[cProfile.SelectedIndex].ToString();
        }
	}
}

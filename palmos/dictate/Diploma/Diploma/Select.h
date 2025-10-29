#pragma once

namespace Diploma
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary> 
	/// Summary for Select
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class Select : public System::Windows::Forms::Form
	{
	public:
		Select(void)
		{
			InitializeComponent();
			idxSelected=-1;
		}

	protected: 
		void Dispose(Boolean disposing)
		{
			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}
	private: System::Windows::Forms::Label *  label1;
	public: System::Windows::Forms::ListBox *  lbScores;
		int idxSelected;
			
		void ClearList(void)
		{
			lbScores->Items->Clear();
		}
        
		void AddItem(String *s)
		{
			lbScores->Items->Add(s);
		}			

	private: System::Windows::Forms::Button *  bPrint;
	private: System::Windows::Forms::Button *  bCancel;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container* components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->label1 = new System::Windows::Forms::Label();
			this->lbScores = new System::Windows::Forms::ListBox();
			this->bPrint = new System::Windows::Forms::Button();
			this->bCancel = new System::Windows::Forms::Button();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 8);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(192, 32);
			this->label1->TabIndex = 0;
			this->label1->Text = S"There are multiple scores in this file. Please select the score to print:";
			// 
			// lbScores
			// 
			this->lbScores->Location = System::Drawing::Point(8, 40);
			this->lbScores->Name = S"lbScores";
			this->lbScores->Size = System::Drawing::Size(184, 95);
			this->lbScores->TabIndex = 1;
			// 
			// bPrint
			// 
			this->bPrint->Location = System::Drawing::Point(8, 144);
			this->bPrint->Name = S"bPrint";
			this->bPrint->TabIndex = 2;
			this->bPrint->Text = S"Print";
			this->bPrint->Click += new System::EventHandler(this, bPrint_Click);
			// 
			// bCancel
			// 
			this->bCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->bCancel->Location = System::Drawing::Point(118, 144);
			this->bCancel->Name = S"bCancel";
			this->bCancel->TabIndex = 3;
			this->bCancel->Text = S"Cancel";
			this->bCancel->Click += new System::EventHandler(this, bCancel_Click);
			// 
			// Select
			// 
			this->AcceptButton = this->bPrint;
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->CancelButton = this->bCancel;
			this->ClientSize = System::Drawing::Size(200, 174);
			this->ControlBox = false;
			this->Controls->Add(this->bCancel);
			this->Controls->Add(this->bPrint);
			this->Controls->Add(this->lbScores);
			this->Controls->Add(this->label1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->Name = S"Select";
			this->Text = S"Select Score To Print";
			this->ResumeLayout(false);

		}		
	private: System::Void bCancel_Click(System::Object *  sender, System::EventArgs *  e)
			 {
				 Close();
			 }

	private: System::Void bPrint_Click(System::Object *  sender, System::EventArgs *  e)
			 {
				 idxSelected=lbScores->SelectedIndex;
				 Close();
			 }

	};
}
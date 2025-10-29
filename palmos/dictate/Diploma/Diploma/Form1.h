#pragma once
#using <mscorlib.dll>
#using <System.dll>
#include <stdio.h>
#include "pdb.h"
#include "Select.h"
#include "Page.h"

namespace Diploma
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO;
	using namespace System::Runtime::InteropServices;

	/// <summary> 
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class Form1 : public System::Windows::Forms::Form
	{	
	private: System::Windows::Forms::ComboBox *  cbDiplomas;

	public:
		Form1(void)
		{
			int idx=0;
			String *filesArray[];
			IEnumerator *files;
			FileInfo *fI;
			String *name, *n;
			char *p;

			InitializeComponent();

			if ((p=GetRegistryKey("InstPath"))==NULL) {
				ErrorBox("Path Not Found", "Installation path invalid.");
				exit(1);
			}

			if (Directory::Exists(p)==false) {
				ErrorBox("No Such Path", "The path pointed to by InstPath does not exist.");
				exit(1);
			}

			filesArray=Directory::GetFiles(p);
			files=filesArray->GetEnumerator();
			while (files->MoveNext()) {
				name=__try_cast<String *>(files->Current);
				if (name->ToLower()->EndsWith(".htm")) {
					fI=new FileInfo(name);
					n=fI->Name;
					cbDiplomas->Items->Add(n->Substring(0, n->LastIndexOf(".htm")));
					idx++;
				}
			}

			if (idx<1) {
				ErrorBox("No Diplomas", "Cannot find any diplomas.");
				exit(0);
			}

			if ((p=GetRegistryKey("Diploma"))!=NULL) {
				if ((idx=cbDiplomas->FindString(p))==-1)
					idx=0;
			} else
				idx=0;

			cbDiplomas->SelectedIndex=idx;

			if ((p=GetRegistryKey("ScoreFile"))!=NULL) {
				if (File::Exists(p)==true) {
					tfScoreFile->Text=p;
					free(p);
				}
			}
		}

		char *GetRegistryKey(char *subKey)
		{
			DWORD bufLen=REGISTRYVALUELEN;
			HKEY key;
			char buffer[REGISTRYVALUELEN+1];
			long err;

			buffer[0]='\x00';
			if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGISTRYKEY, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, NULL)!=ERROR_SUCCESS)
				return NULL;

    		err=RegQueryValueEx(key, subKey, NULL, NULL, (BYTE *)&buffer, &bufLen);
			RegCloseKey(key);
			if (err==ERROR_SUCCESS) {
				buffer[bufLen]='\x00';
				return strdup(buffer);
			}

			return NULL;
		}

		void SetRegistryKey(char *subKey, String *value)
		{
			HKEY key;
			DWORD bufLen;
			char buffer[REGISTRYVALUELEN+1];
			char *v;

			if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGISTRYKEY, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, NULL)!=ERROR_SUCCESS)
				return;

			v=(char *)Marshal::StringToHGlobalAnsi(value).ToPointer();
			bufLen=strlen(v);
			if (bufLen>REGISTRYVALUELEN)
				 bufLen=REGISTRYVALUELEN;

			strncpy(buffer, v, bufLen);
			buffer[bufLen]='\x00';

    		RegSetValueEx(key, subKey, NULL, REG_SZ, (BYTE *)&buffer, bufLen);
			Marshal::FreeHGlobal(v);
			RegCloseKey(key);
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

	private: System::Windows::Forms::Button *  bBrowse;
	private: System::Windows::Forms::Button *  bPrint;
	private: System::Windows::Forms::Button *  bClose;
	private: System::Windows::Forms::OpenFileDialog *  ofdBrowse;
	private: System::Windows::Forms::TextBox *  tfScoreFile;
	private: System::Windows::Forms::StatusBar *  statusBar1;

	private: System::Windows::Forms::Label *  label2;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container * components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::Resources::ResourceManager *  resources = new System::Resources::ResourceManager(__typeof(Diploma::Form1));
			this->label1 = new System::Windows::Forms::Label();
			this->tfScoreFile = new System::Windows::Forms::TextBox();
			this->bBrowse = new System::Windows::Forms::Button();
			this->bPrint = new System::Windows::Forms::Button();
			this->bClose = new System::Windows::Forms::Button();
			this->ofdBrowse = new System::Windows::Forms::OpenFileDialog();
			this->statusBar1 = new System::Windows::Forms::StatusBar();
			this->cbDiplomas = new System::Windows::Forms::ComboBox();
			this->label2 = new System::Windows::Forms::Label();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(16, 24);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(56, 16);
			this->label1->TabIndex = 0;
			this->label1->Text = S"Score file:";
			// 
			// tfScoreFile
			// 
			this->tfScoreFile->Location = System::Drawing::Point(80, 20);
			this->tfScoreFile->Name = S"tfScoreFile";
			this->tfScoreFile->Size = System::Drawing::Size(368, 20);
			this->tfScoreFile->TabIndex = 1;
			this->tfScoreFile->Text = S"";
			// 
			// bBrowse
			// 
			this->bBrowse->Location = System::Drawing::Point(464, 16);
			this->bBrowse->Name = S"bBrowse";
			this->bBrowse->Size = System::Drawing::Size(72, 24);
			this->bBrowse->TabIndex = 2;
			this->bBrowse->Text = S"Browse...";
			this->bBrowse->Click += new System::EventHandler(this, bBrowse_Click);
			// 
			// bPrint
			// 
			this->bPrint->Location = System::Drawing::Point(16, 88);
			this->bPrint->Name = S"bPrint";
			this->bPrint->TabIndex = 4;
			this->bPrint->Text = S"Print";
			this->bPrint->Click += new System::EventHandler(this, bPrint_Click);
			// 
			// bClose
			// 
			this->bClose->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->bClose->Location = System::Drawing::Point(464, 88);
			this->bClose->Name = S"bClose";
			this->bClose->Size = System::Drawing::Size(72, 23);
			this->bClose->TabIndex = 5;
			this->bClose->Text = S"Close";
			this->bClose->Click += new System::EventHandler(this, bClose_Click);
			// 
			// ofdBrowse
			// 
			this->ofdBrowse->DefaultExt = S"pdb";
			this->ofdBrowse->FileName = S"dIcT_Dictate Score.pdb";
			this->ofdBrowse->Filter = S"PDB Files|*.pdb|All files|*.*";
			this->ofdBrowse->InitialDirectory = S"c:\\Program Files\\palmOne";
			this->ofdBrowse->Title = S"Browse for Score file ...";
			// 
			// statusBar1
			// 
			this->statusBar1->Location = System::Drawing::Point(0, 126);
			this->statusBar1->Name = S"statusBar1";
			this->statusBar1->Size = System::Drawing::Size(550, 22);
			this->statusBar1->SizingGrip = false;
			this->statusBar1->TabIndex = 5;
			this->statusBar1->Text = S"Diploma by Brian Schau <http://www.schau.com/s/dictate/index.html>";
			// 
			// cbDiplomas
			// 
			this->cbDiplomas->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cbDiplomas->Location = System::Drawing::Point(80, 48);
			this->cbDiplomas->Name = S"cbDiplomas";
			this->cbDiplomas->Size = System::Drawing::Size(368, 21);
			this->cbDiplomas->Sorted = true;
			this->cbDiplomas->TabIndex = 3;
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(16, 52);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(48, 16);
			this->label2->TabIndex = 7;
			this->label2->Text = S"Diploma:";
			// 
			// Form1
			// 
			this->AcceptButton = this->bPrint;
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->CancelButton = this->bClose;
			this->ClientSize = System::Drawing::Size(550, 148);
			this->ControlBox = false;
			this->Controls->Add(this->label2);
			this->Controls->Add(this->cbDiplomas);
			this->Controls->Add(this->statusBar1);
			this->Controls->Add(this->bClose);
			this->Controls->Add(this->bPrint);
			this->Controls->Add(this->bBrowse);
			this->Controls->Add(this->tfScoreFile);
			this->Controls->Add(this->label1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->Name = S"Form1";
			this->Text = S"Diploma";
			this->ResumeLayout(false);

		}	
	private: System::Void bClose_Click(System::Object *  sender, System::EventArgs *  e)
			 {
				 Close();
			 }

	private: System::Void bBrowse_Click(System::Object *  sender, System::EventArgs *  e)
			 {
				if (ofdBrowse->ShowDialog()==DialogResult::OK)
				{
					tfScoreFile->Text=ofdBrowse->FileName;
				}
			 }

	private: System::Void ErrorBox(String *h, String *b)
			{
				#pragma push_macro("MessageBox")
				#undef MessageBox
				MessageBox::Show(b, h, MessageBoxButtons::OK, MessageBoxIcon::Error);
				#pragma pop_macro("MessageBox")
			}

	private: System::Void bPrint_Click(System::Object *  sender, System::EventArgs *  e)
			{
				String *fileName=tfScoreFile->Text->Trim();
				int idx=0, sCnt;
				PDB *p;
				Page *pp;

				if (fileName==NULL || fileName->Length<1) {
					ErrorBox("Cannot load Score file.", "Invalid Score file name given.");
					return;
				}

				p=new PDB(fileName);
				if (p->Load()==false) {
					p->Done();
					return;
				}

				if ((sCnt=p->GetCount())==0) {
					p->Done();
					ErrorBox("Huh?", "No scores in this file.");
					return;
				}

				if (sCnt>1) {
					Diploma::Select *s=new Diploma::Select();
					int i;

					s->ClearList();
					for (i=0; i<sCnt; i++) {
						Score *sc;
						char buffer[HotSyncNameLength+40];

						sc=p->GetScore(i);
						sprintf(buffer, "%s (%u of %u)", sc->name, sc->score, sc->round);
						s->AddItem(buffer);
					}

					s->ShowDialog();
					idx=s->idxSelected;
					s->Close();
					if (idx<0) {
						p->Done();
						return;
					}
				}

				if (cbDiplomas->Text==NULL || cbDiplomas->Text->Length<1)
					return;

				pp=new Page(GetRegistryKey("InstPath"), p, idx);
				if (pp->CreatePage(cbDiplomas->Text)==true) {
					SetRegistryKey("ScoreFile", fileName);
					SetRegistryKey("Diploma", cbDiplomas->Text);

					if (pp->ShowPage()==false)
						ErrorBox("Error", pp->errStr);
				} else {
					if (pp->errStr->Length>0)
						ErrorBox("Error", pp->errStr);
				}
			}
	};
}
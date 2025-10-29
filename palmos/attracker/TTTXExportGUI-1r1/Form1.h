#pragma once

#include <stdio.h>

#using <mscorlib.dll>
#using <System.dll>


namespace TTTXExportGUI
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Runtime::InteropServices;
	using namespace System::Diagnostics;
	using namespace System::IO;

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
	public:
		Form1(void)
		{
			char *p;
			int idx;

			InitializeComponent();

			if ((p=GetRegistryKey("TTTXExportPath"))==NULL) {
				if ((p=GetRegistryKey("Current"))==NULL)
					tbTttxexport->Text="";
				else
					tbTttxexport->Text=p;
			} else {
					tbTttxexport->Text=p;
					free(p);
			}

			if ((p=GetRegistryKey("Options"))==NULL)
					tbOptions->Text="";
			else {
					tbOptions->Text=p;
					free(p);
			}

			if ((p=GetRegistryKey("Archive"))==NULL)
				tbArchive->Text="C:\\Program Files\\Palm";
			else {
					tbArchive->Text=p;
					free(p);
			}

			if ((p=GetRegistryKey("Output"))==NULL)
					tbOutput->Text="";
			else {
					tbOutput->Text=p;
					free(p);
			}

			if ((p=GetRegistryKey("Format"))==NULL)
					idx=0;
			else {
					idx=atoi(p);
					if (idx<0 || idx>3)
							idx=0;
			}

			cbFormat->SelectedIndex=idx;
		}

		char *GetRegistryKey(char *subKey)
		{
			DWORD bufLen=REGISTRYVALUELEN;
			HKEY key;
			char buffer[REGISTRYVALUELEN+1];
			long err;

			buffer[0]='\x00';
			if (RegCreateKeyEx(HKEY_CURRENT_USER, REGISTRYKEY, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, NULL)!=ERROR_SUCCESS)
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

			if (RegCreateKeyEx(HKEY_CURRENT_USER, REGISTRYKEY, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, NULL)!=ERROR_SUCCESS)
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
	private: System::Windows::Forms::Label *  label2;
	private: System::Windows::Forms::Label *  label3;
	private: System::Windows::Forms::Label *  label4;
	private: System::Windows::Forms::TextBox *  tbTttxexport;
	private: System::Windows::Forms::TextBox *  tbOptions;
	private: System::Windows::Forms::TextBox *  tbArchive;
	private: System::Windows::Forms::TextBox *  tbOutput;
	private: System::Windows::Forms::Label *  label5;
	private: System::Windows::Forms::ComboBox *  cbFormat;
	private: System::Windows::Forms::Button *  bTttxexport;
	private: System::Windows::Forms::Button *  bArchive;
	private: System::Windows::Forms::Button *  bOutput;
	private: System::Windows::Forms::Button *  bExit;
	private: System::Windows::Forms::Button *  bExport;
private: System::Windows::Forms::OpenFileDialog *  ofdTttxexport;

private: System::Windows::Forms::OpenFileDialog *  ofdArchive;
private: System::Windows::Forms::FolderBrowserDialog *  fbdOutput;

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
			this->label1 = new System::Windows::Forms::Label();
			this->label2 = new System::Windows::Forms::Label();
			this->label3 = new System::Windows::Forms::Label();
			this->label4 = new System::Windows::Forms::Label();
			this->tbTttxexport = new System::Windows::Forms::TextBox();
			this->tbOptions = new System::Windows::Forms::TextBox();
			this->tbArchive = new System::Windows::Forms::TextBox();
			this->tbOutput = new System::Windows::Forms::TextBox();
			this->label5 = new System::Windows::Forms::Label();
			this->cbFormat = new System::Windows::Forms::ComboBox();
			this->bTttxexport = new System::Windows::Forms::Button();
			this->bArchive = new System::Windows::Forms::Button();
			this->bOutput = new System::Windows::Forms::Button();
			this->bExit = new System::Windows::Forms::Button();
			this->bExport = new System::Windows::Forms::Button();
			this->ofdTttxexport = new System::Windows::Forms::OpenFileDialog();
			this->fbdOutput = new System::Windows::Forms::FolderBrowserDialog();
			this->ofdArchive = new System::Windows::Forms::OpenFileDialog();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(16, 20);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(72, 16);
			this->label1->TabIndex = 0;
			this->label1->Text = S"TTTXExport:";
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(16, 52);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(48, 16);
			this->label2->TabIndex = 1;
			this->label2->Text = S"Options:";
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(16, 116);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(48, 16);
			this->label3->TabIndex = 2;
			this->label3->Text = S"Archive:";
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(16, 84);
			this->label4->Name = S"label4";
			this->label4->Size = System::Drawing::Size(48, 16);
			this->label4->TabIndex = 3;
			this->label4->Text = S"Output:";
			// 
			// tbTttxexport
			// 
			this->tbTttxexport->Location = System::Drawing::Point(96, 16);
			this->tbTttxexport->Name = S"tbTttxexport";
			this->tbTttxexport->Size = System::Drawing::Size(296, 20);
			this->tbTttxexport->TabIndex = 4;
			this->tbTttxexport->Text = S"";
			// 
			// tbOptions
			// 
			this->tbOptions->Location = System::Drawing::Point(96, 48);
			this->tbOptions->Name = S"tbOptions";
			this->tbOptions->Size = System::Drawing::Size(296, 20);
			this->tbOptions->TabIndex = 5;
			this->tbOptions->Text = S"";
			// 
			// tbArchive
			// 
			this->tbArchive->Location = System::Drawing::Point(96, 112);
			this->tbArchive->Name = S"tbArchive";
			this->tbArchive->Size = System::Drawing::Size(296, 20);
			this->tbArchive->TabIndex = 6;
			this->tbArchive->Text = S"";
			// 
			// tbOutput
			// 
			this->tbOutput->Location = System::Drawing::Point(96, 80);
			this->tbOutput->Name = S"tbOutput";
			this->tbOutput->Size = System::Drawing::Size(296, 20);
			this->tbOutput->TabIndex = 7;
			this->tbOutput->Text = S"";
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(16, 148);
			this->label5->Name = S"label5";
			this->label5->Size = System::Drawing::Size(40, 16);
			this->label5->TabIndex = 8;
			this->label5->Text = S"Format:";
			// 
			// cbFormat
			// 
			this->cbFormat->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			System::Object* __mcTemp__1[] = new System::Object*[4];
			__mcTemp__1[0] = S"Excel CSV";
			__mcTemp__1[1] = S"Standard CSV";
			__mcTemp__1[2] = S"PHP";
			__mcTemp__1[3] = S"Debug";
			this->cbFormat->Items->AddRange(__mcTemp__1);
			this->cbFormat->Location = System::Drawing::Point(96, 144);
			this->cbFormat->MaxDropDownItems = 4;
			this->cbFormat->Name = S"cbFormat";
			this->cbFormat->Size = System::Drawing::Size(121, 21);
			this->cbFormat->TabIndex = 9;
			// 
			// bTttxexport
			// 
			this->bTttxexport->Location = System::Drawing::Point(408, 16);
			this->bTttxexport->Name = S"bTttxexport";
			this->bTttxexport->TabIndex = 10;
			this->bTttxexport->Text = S"Browse...";
			this->bTttxexport->Click += new System::EventHandler(this, bTttxexport_Click);
			// 
			// bArchive
			// 
			this->bArchive->Location = System::Drawing::Point(408, 112);
			this->bArchive->Name = S"bArchive";
			this->bArchive->TabIndex = 11;
			this->bArchive->Text = S"Browse...";
			this->bArchive->Click += new System::EventHandler(this, bArchive_Click);
			// 
			// bOutput
			// 
			this->bOutput->Location = System::Drawing::Point(408, 80);
			this->bOutput->Name = S"bOutput";
			this->bOutput->TabIndex = 12;
			this->bOutput->Text = S"Browse...";
			this->bOutput->Click += new System::EventHandler(this, bOutput_Click);
			// 
			// bExit
			// 
			this->bExit->Location = System::Drawing::Point(16, 192);
			this->bExit->Name = S"bExit";
			this->bExit->TabIndex = 13;
			this->bExit->Text = S"Exit";
			this->bExit->Click += new System::EventHandler(this, bExit_Click);
			// 
			// bExport
			// 
			this->bExport->Location = System::Drawing::Point(408, 192);
			this->bExport->Name = S"bExport";
			this->bExport->TabIndex = 14;
			this->bExport->Text = S"Export";
			this->bExport->Click += new System::EventHandler(this, bExport_Click);
			// 
			// ofdTttxexport
			// 
			this->ofdTttxexport->FileName = S"tttxexport.exe";
			this->ofdTttxexport->Filter = S"TTTXExport Binary|tttxexport.exe";
			this->ofdTttxexport->Title = S"Browse for TTTXExport Program";
			// 
			// fbdOutput
			// 
			this->fbdOutput->RootFolder = System::Environment::SpecialFolder::MyComputer;
			// 
			// ofdArchive
			// 
			this->ofdArchive->DefaultExt = S"PDB";
			this->ofdArchive->Filter = S"@Tracker Archive|*.PDB|All Files|*.*";
			// 
			// Form1
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(504, 234);
			this->Controls->Add(this->bExport);
			this->Controls->Add(this->bExit);
			this->Controls->Add(this->bOutput);
			this->Controls->Add(this->bArchive);
			this->Controls->Add(this->bTttxexport);
			this->Controls->Add(this->cbFormat);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->tbOutput);
			this->Controls->Add(this->tbArchive);
			this->Controls->Add(this->tbOptions);
			this->Controls->Add(this->tbTttxexport);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
			this->Name = S"Form1";
			this->Text = S"TTTXExport GUI v1r1";
			this->ResumeLayout(false);

		}	



private: System::Void bExit_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 Close();
		 }

private: System::Void bTttxexport_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 ofdTttxexport->InitialDirectory=tbTttxexport->Text;
			 if (ofdTttxexport->ShowDialog()==DialogResult::OK)
				 tbTttxexport->Text=ofdTttxexport->FileName;
		 }

private: System::Void bOutput_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 if (fbdOutput->ShowDialog()==DialogResult::OK)
				 tbOutput->Text=fbdOutput->SelectedPath;
		 }

private: System::Void bArchive_Click(System::Object *  sender, System::EventArgs *  e)
		 {
		 	 ofdArchive->InitialDirectory=tbArchive->Text;
			 if (ofdArchive->ShowDialog()==DialogResult::OK)
				 tbArchive->Text=ofdArchive->FileName;
		 }

private: System::Void bExport_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 String *sStartDir=NULL, *sOpts=NULL, *sFileName=NULL, *sSuffix=NULL, *sFormat=NULL;
			 String *sOut=NULL, *sErr=NULL, *sMessage=NULL;
			 char buffer[32];
			 int i;
			 Process *tProc;
			 char *v, *t;

 			 if (tbTttxexport->Text->Trim()->Length<1 || tbArchive->Text->Trim()->Length<1) {
				 MessageBox(NULL, "TTTXExport path or Archive path empty.", "Error", MB_OK);
			  	 return;
			 }

			 SetRegistryKey("TTTXExportPath", tbTttxexport->Text->Trim());
			 SetRegistryKey("Options", tbOptions->Text->Trim());
			 SetRegistryKey("Archive", tbArchive->Text->Trim());
			 SetRegistryKey("Output", tbOutput->Text->Trim());

			 sprintf(buffer, "%i", cbFormat->SelectedIndex);
			 SetRegistryKey("Format", buffer);

			 if ((tbOutput->Text->Trim()->Length)>0)
					sStartDir=tbOutput->Text->Trim();
			 else
					sStartDir=NULL;
			 
			 switch (cbFormat->SelectedIndex) {
				 case 0:
					 sSuffix=".csv";
					 sFormat="excel";
					 break;
				 case 2:
					 sSuffix=".php";
					 sFormat="php";
					 break;
				 case 3:
					 sSuffix=".bin";
					 sFormat="debug";
					 break;
				 default:
					 sSuffix=".csv";
					 sFormat="stdcsv";
					 break;
			 }

			 sOpts=tbOptions->Text->Trim();
			 if (sOpts->IndexOf("-o")==-1) {
				 sFileName=tbArchive->Text->Trim();
				 if ((i=sFileName->LastIndexOf("\\"))>-1)
					 sFileName=sFileName->Substring(i+1);

				 if ((i=sFileName->LastIndexOf("."))>-1)
					sFileName=sFileName->Substring(0, i);

				 sOpts=String::Concat(sOpts, " -o \"", sFileName);
				 sOpts=String::Concat(sOpts, sSuffix, "\"");
			 }

			 sOpts=String::Concat(sOpts, " -O ", sFormat);
			 sOpts=String::Concat(sOpts, " \"", tbArchive->Text->Trim(), "\"");

			 try {
				tProc=new Process();
				tProc->StartInfo->Arguments=sOpts;
				tProc->StartInfo->CreateNoWindow=true;
				tProc->StartInfo->ErrorDialog=true;
				tProc->StartInfo->FileName=tbTttxexport->Text;
				tProc->StartInfo->RedirectStandardError=true;
				tProc->StartInfo->RedirectStandardInput=true;
				tProc->StartInfo->RedirectStandardOutput=true;
				tProc->StartInfo->UseShellExecute=false;
				if (sStartDir)
					tProc->StartInfo->WorkingDirectory=sStartDir;
				tProc->Start();
				sErr=tProc->StandardError->ReadToEnd();
				sOut=tProc->StandardOutput->ReadToEnd();
				tProc->WaitForExit();
			 } catch (...) {
				 MessageBox(NULL, "Failed to start TTTXExport", "Error", MB_OK);
				 return;
			 }

			 if (sErr && sErr->Trim()->Length>0) {
				 sMessage=sErr;
				 t="Error";
			 } else if (sOut && sOut->Trim()->Length>0) {
				 sMessage=sOut;
				 t="Output";
			 } else {
				 sMessage="Archive Exported";
				 t="OK";
			 }

			 v=(char *)Marshal::StringToHGlobalAnsi(sMessage).ToPointer();
			 MessageBox(NULL, v, t, MB_OK);
			 Marshal::FreeHGlobal(v);
		 }
};
}



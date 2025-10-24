#pragma once

namespace Diploma
{
#undef GetTempPath
	using namespace System;
	using namespace System::Diagnostics;
	using namespace System::IO;
	using namespace System::Text::RegularExpressions;

	public __gc class Page
	{
	private:
		String *instPath;
		PDB *pdb;
		int index;

	String *LoadPage(void)
	{
		StreamReader *sReader=NULL;
		String *s=NULL;

		try {
			sReader=new StreamReader(instPath);

			s=sReader->ReadToEnd();
		} catch (Exception *ex) {
			errStr=String::Concat(S"Cannot load HTML template file: ", instPath, S"\r\n", ex->Message);
		} __finally {
			if (sReader)
				sReader->Close();
		}

		return s;
	}

	bool WritePage(String *mem)
	{
		bool useFile=false;

		if (mem==NULL)
			return false;

		try 
		{
			Score *sc=pdb->GetScore(index);
			char buffer[32];
			StreamWriter *writer;
			int pct;
			Uri *u;

			pageFile=String::Concat(Path::GetTempPath(), S"\\", Environment::UserName, S".html");

			sprintf(buffer, "%u", sc->score);
			mem=Regex::Replace(mem, S"@@SCORE@@", buffer);
			sprintf(buffer, "%u", sc->round);
			mem=Regex::Replace(mem, S"@@ROUND@@", buffer);
			mem=Regex::Replace(mem, S"@@NAME@@", sc->name);
			mem=Regex::Replace(mem, S"@@GROUP@@", sc->group);

			if (sc->round)
				pct=(sc->score*100)/sc->round;
			else
				pct=0;

			sprintf(buffer, "%u", pct);
			mem=Regex::Replace(mem, S"@@PCT@@", buffer);

			sprintf(buffer, "%u", sc->year);
			mem=Regex::Replace(mem, S"@@YEAR@@", buffer);
			sprintf(buffer, "%u", sc->month);
			mem=Regex::Replace(mem, S"@@MONTH@@", buffer);
			sprintf(buffer, "%u", sc->day);
			mem=Regex::Replace(mem, S"@@DAY@@", buffer);

			u=new Uri(instPath);
			mem=Regex::Replace(mem, S"@@BASE@@", u->AbsoluteUri);

			writer=File::CreateText(pageFile);
			writer->Write(mem);
			writer->Close();
			useFile=true;
		} catch (Exception *ex) {
			errStr=String::Concat(S"Cannot create temporary page:\r\n", ex->Message);
		}

		if (useFile==false && pageFile!=NULL)
			File::Delete(pageFile);

		return useFile;
	}

	public:
		String *errStr;
		String *pageFile;

		Page(String *i, PDB *p, int idx) {
			instPath=i;
			pdb=p;
			index=idx;
			errStr="";
			pageFile=NULL;
		}

		bool CreatePage(String *diploma)
		{
			if (instPath==NULL || instPath->Length==0) {
				errStr=S"Invalid template path.";
				return false;
			}

			instPath=String::Concat(instPath, S"\\", diploma, S".htm");
			if (File::Exists(instPath)==false) {
				errStr=String::Concat(S"No such file: ", instPath);
				return false;
			}

			return WritePage(LoadPage());
		}

		bool ShowPage(void)
		{
			bool ret=false;

			errStr="";
			try {
				Process::Start(pageFile);
				ret=true;
			} catch (Exception *ex) {
				errStr=String::Concat("Cannot start Web-browser:\r\n", ex->Message);
			}

			return ret;
		}
	};
}
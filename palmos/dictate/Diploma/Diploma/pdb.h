#pragma once
#using <mscorlib.dll>

namespace Diploma
{
	using namespace System;
	using namespace System::IO;
	using namespace System::Windows::Forms;

	public __gc class PDB
	{
	private:
		String *fileName;
		char *pdbData;
		Score **scores;
		int scoresCnt;

        System::Void ErrorBox(String *h, String *b)
		{
				#pragma push_macro("MessageBox")
				#undef MessageBox
				MessageBox::Show(b, h, MessageBoxButtons::OK, MessageBoxIcon::Error);
				#pragma pop_macro("MessageBox")
		}

		unsigned long ntohl(unsigned long n)
		{
			return (((n&0xFF)<<24)+((n&0xFF00)<<8)+((n&0xFF0000)>>8)+((n&0xFF000000)>>24));
		}

		unsigned short ntohs(unsigned short n)
		{
			return ((n>>8)|(n<<8));
		}

	public:
		PDB(String *f)
		{
			fileName=f;
			pdbData=NULL;
			scores=NULL;
			scoresCnt=0;
		}

		// Deallocate everything
		void Done()
		{
			int i;

			if (scores) {
				for (i=0; scores[i]; i++)
					free(scores[i]);

				free(scores);
			}

			if (pdbData) {
				delete pdbData;
				pdbData=NULL;
			}
		}

		// Load the pdb file and create scores array
		bool Load()
		{
			FileStream *iStream=NULL;
			BinaryReader *bReader=NULL;
			bool ret=false;

			try {
				int hIdx=78, i;
				unsigned long u32;
				unsigned char buffer __gc[];
				
				iStream=new FileStream(fileName, FileMode::Open, FileAccess::Read);
				bReader=new BinaryReader(iStream);

				iStream->Seek(60, SeekOrigin::Begin);
				u32=bReader->ReadUInt32();

				if (u32!='ATAD')		// DATA reversed
					throw new Exception("DATA block not found.");

				u32=bReader->ReadUInt32();
				if (u32!='TcId')		// dIcT reversed
					throw new Exception("dIcT block not found.");

				iStream->Seek(76, SeekOrigin::Begin);
				scoresCnt=(int)(ntohs(bReader->ReadUInt16()));

				if ((scores=(Score **)calloc(sizeof(Score *)*(scoresCnt+1), 1))==NULL)
					throw new Exception("Out of memory.");

				for (i=0; i<scoresCnt; i++) {
					Score *s;

					if ((scores[i]=(Score *)calloc(sizeof(Score), 1))==NULL)
						throw new Exception("Out of memory.");

					iStream->Seek(hIdx, SeekOrigin::Begin);
					hIdx+=8;

					u32=ntohl(bReader->ReadUInt32());
					iStream->Seek(u32, SeekOrigin::Begin);

					buffer=bReader->ReadBytes(sizeof(Score));
					System::Runtime::InteropServices::Marshal::Copy(buffer, 0, scores[i], sizeof(Score));
					s=scores[i];
					s->round=ntohs(s->round);
					s->score=ntohs(s->score);
					s->year=ntohs(s->year);
					s->month=ntohs(s->month);
					s->day=ntohs(s->day);
				}

				scores[scoresCnt]=NULL;
				ret=true;
			} catch (Exception *ex) {
				ErrorBox("Cannot load Score file.", ex->Message);
			} __finally {
				if (bReader)
					bReader->Close();
			}

			return ret;
		}

		// Get number of scores
		int GetCount(void)
		{
			return scoresCnt;
		}

		// Get score
		Score *GetScore(int idx)
		{
			return scores[idx];
		}
	};
}
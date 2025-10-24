//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "mainform.h"
#include "bitmap.h"
#include "prc.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
AnsiString largeIcon, smallIcon;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm1::openLargeImage(TObject *Sender)
{
        if (openLargeImageDialog->Execute()) {
                largeIcon=openLargeImageDialog->FileName;
                largeIconImage->Picture->LoadFromFile(largeIcon);
        }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::smallIconButtonClick(TObject *Sender)
{
        if (openSmallImageDialog->Execute()) {
                smallIcon=openSmallImageDialog->FileName;
                smallIconImage->Picture->LoadFromFile(smallIcon);
        }
}

//---------------------------------------------------------------------------
void __fastcall TForm1::resetIconsButtonClick(TObject *Sender)
{
        AnsiString path=ParamStr(0);
        int idx;

        idx=path.LastDelimiter("\\");

        largeIcon=path.SubString(1, idx)+"large.bmp";
        smallIcon=path.SubString(1, idx)+"small.bmp";
        largeIconImage->Picture->LoadFromFile(largeIcon);
        smallIconImage->Picture->LoadFromFile(smallIcon);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::buildButtonClick(TObject *Sender)
{
        AnsiString url, protocol;
        char *bmpName, *userUrl, *launcherName, *prcName;
        unsigned char *largeBitmap, *smallBitmap, *prc;
        unsigned int largeBitmapLen, smallBitmapLen, prcLen;
        bool makeExe, status;

        // first some checks
        if (urlEdit->Text.Length()==0) {
                statusBar->SimpleText="You must enter a URL";
                Beep();
                return;
        }

        if (launcherNameEdit->Text.Length()==0) {
                statusBar->SimpleText="You must enter a Launcher Name";
                Beep();
                return;
        }

        launcherName=new char[launcherNameEdit->Text.Length()+1];
        strcpy(launcherName, launcherNameEdit->Text.c_str());

        // qualify url
        protocol=urlEdit->Text.SubString(1, 7);
        if (protocol.AnsiCompareIC("http://")==0)
                url=urlEdit->Text;
        else
                url="http://"+urlEdit->Text;

        userUrl=new char[url.Length()+1];
        strcpy(userUrl, url.c_str());

        bmpName=new char[largeIcon.Length()+1];
        strcpy(bmpName, largeIcon.c_str());
        largeBitmap=bmpCreate(bmpName, &largeBitmapLen);
        delete(bmpName);

        bmpName=new char[smallIcon.Length()+1];
        strcpy(bmpName, smallIcon.c_str());
        smallBitmap=bmpCreate(bmpName, &smallBitmapLen);
        delete(bmpName);

        if (largeBitmap) {
                if (smallBitmap) {
                        if (standaloneExecutableCheck->Checked==true)
                                makeExe=true;
                        else
                                makeExe=false;

                        prc=prcBuild(launcherName, userUrl, largeBitmap, smallBitmap, largeBitmapLen, smallBitmapLen, makeExe, &prcLen);
                        if (prc==NULL)
                                statusBar->SimpleText="Cannot create PRC file";
                        else {
                                if (saveOCCDialog->Execute()) {
                                        prcName=new char[saveOCCDialog->FileName.Length()+1];
                                        strcpy(prcName, saveOCCDialog->FileName.c_str());
                                        status=prcSave(prcName, prc, prcLen);
                                        delete(prcName);
                                        if (status==true)
                                                statusBar->SimpleText="Done";
                                        else {
                                                statusBar->SimpleText="Cannot save "+saveOCCDialog->FileName;
                                                Beep();
                                        }
                                } else
                                        statusBar->SimpleText="Aborted";

                                delete(prc);
                        }
                } else
                        statusBar->SimpleText="Load Error: "+smallIcon;
        } else
                statusBar->SimpleText="Load Error: "+largeIcon;

        delete(launcherName);
        delete(userUrl);
        if (largeBitmap)
                delete(largeBitmap);

        if (smallBitmap)
                delete(smallBitmap);
}


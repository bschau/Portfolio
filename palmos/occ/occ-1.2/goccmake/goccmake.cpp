//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("goccmake.res");
USEFORM("mainform.cpp", Form1);
USEUNIT("bitmap.cpp");
USEUNIT("prc.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->Title = "gOCCMake";
                 Application->CreateForm(__classid(TForm1), &Form1);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------

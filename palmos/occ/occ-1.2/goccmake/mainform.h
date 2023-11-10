//---------------------------------------------------------------------------

#ifndef mainformH
#define mainformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TLabel *urlLabel;
        TLabel *launcherNameLabel;
        TEdit *urlEdit;
        TEdit *launcherNameEdit;
        TCheckBox *standaloneExecutableCheck;
        TGroupBox *GroupBox1;
        TImage *largeIconImage;
        TImage *smallIconImage;
        TButton *largeIconButton;
        TButton *smallIconButton;
        TButton *resetIconsButton;
        TButton *buildButton;
        TStatusBar *statusBar;
        TOpenDialog *openLargeImageDialog;
        TOpenDialog *openSmallImageDialog;
        TSaveDialog *saveOCCDialog;
        void __fastcall openLargeImage(TObject *Sender);
        void __fastcall smallIconButtonClick(TObject *Sender);
        void __fastcall resetIconsButtonClick(TObject *Sender);
        void __fastcall buildButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
 
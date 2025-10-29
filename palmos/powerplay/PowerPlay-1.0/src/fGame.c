/*
 * fGame.c
 */
#include "Include.h"

/* protos */
static void UpdateDisplay(void);
static void SetupString(Char *, Boolean);
static void fHelpRun(void);
static Boolean EH(EventType *);

/* globals */
static Char *txtCorrect="Correct!", *txtILow="Incorrect - guess too low!", *txtIHi="Incorrect - guess too high!";

/*
 * fGameInit
 */
Boolean
fGameInit(void)
{
	inGame=true;

	if (prefs.flags&PFlgType)
		UIObjectShow(cGameLblE);
	else
		UIObjectShow(cGameLblB);

	return true;
}

/*
 * fGameEH
 */
Boolean
fGameEH(EventType *ev)
{
	Char *p;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		UpdateDisplay();
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cGameGuess:
			p=UIFieldGetText(cGameField);

			if (p && StrLen(p)>0) {
				UInt16 v=StrAToI(p), q;
				UInt8 quiz=session->quiz, quizIdx=session->quizIdx;

				if (prefs.flags&PFlgType)
					q=session->quiz&0x0f;
				else
					q=(session->quiz>>4)&0x0f;

				if (q==v) {
					SssUpdate(0);
					SetupString(txtCorrect, false);
				} else {
					Char *a;

					if (q>v) {
						a=txtILow;
						SssUpdate(q-v);
					} else {
						a=txtIHi;
						SssUpdate(v-q);
					}

					SetupString(a, true);
				}

				if (session->round>MaxRounds) {
					session->quiz=quiz;
					session->quizIdx=quizIdx;
				}

				UIFieldClear(cGameField);
				UpdateDisplay();
				if (session->round>MaxRounds) {
					UIObjectHide(cGameGuess);
					inGame=false;
					SysTaskDelay(2*SysTicksPerSecond());
					FrmGotoForm(fEnd);
				}
			} else {
				SetupString("You must enter a value!", true);
				UIFieldFocus(cGameField);
			}

			return true;

		case cGameStop:
			if (FrmAlert(aEndGame)==0) {
				SssDestroy();
				FrmGotoForm(fMain);
			}

			return true;

		case cGameHelp:
			fHelpRun();
			UIFormRedraw();
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * UpdateDisplay
 *
 * Update the display.
 */
static void
UpdateDisplay(void)
{
	MemHandle mh=DmGetResource(iconType, 1000);
	FieldType *fld=UIObjectGet(cGameField);
	RectangleType rct;
	Char buffer[20];
	UInt16 equalWidth, objWidth, lhsWidth;
	Int16 len, w, x, h;
	Char *p;
	FontID fID;

	UIObjectHide(cGameField);

	ErrFatalDisplayIf(mh==NULL, "(UpdateDisplay) Logo bitmap not found");
	WinDrawBitmap(MemHandleLock(mh), 64, 44);
	MemHandleUnlock(mh);
	DmReleaseResource(mh);

	mh=DmGetResource(bitmapRsc, (prefs.flags&PFlgLevel ? bmpHard : bmpEasy));
	ErrFatalDisplayIf(mh==NULL, "(UpdateDisplay) Level bitmap not found");
	WinDrawBitmap(MemHandleLock(mh), 0, 155);
	MemHandleUnlock(mh);
	DmReleaseResource(mh);

	RctSetRectangle(&rct, 0, 27, 160, 12);
	WinEraseRectangle(&rct, 0);

	fID=FntSetFont(boldFont);
	if (session->round>MaxRounds)
		WinDrawChars("--", 2, 0, 27);
	else {
		StrPrintF(buffer, "%u", session->round);
		WinDrawChars(buffer, StrLen(buffer), 0, 27);
	}

	StrPrintF(buffer, "%u", session->score);
	len=StrLen(buffer);
	w=FntCharsWidth(buffer, len);
	WinDrawChars(buffer, len, 160-w, 27);

	RctSetRectangle(&rct, 0, 100, 160, 35);
	WinEraseRectangle(&rct, 0);

	FntSetFont(largeBoldFont);
	h=FntCharHeight();

	objWidth=FntCharsWidth("99", 2);
	lhsWidth=(objWidth*2)+6;
	equalWidth=FntCharsWidth(" = ", 3);
	w=lhsWidth+equalWidth;

	p=SssQuizToAscii();
	len=StrLen(p);

	w+=FntCharsWidth(p, len);

	x=(160-w)/2;

	WinDrawChars(" = ", 3, x+lhsWidth, 108);
	WinDrawChars(p, len, x+lhsWidth+equalWidth, 108);

	if (prefs.flags&PFlgType) {
		StrPrintF(buffer, "%u", ((session->quiz)>>4)&0x0f);
		WinDrawChars(buffer, StrLen(buffer), x+objWidth-2-FntCharsWidth(buffer, StrLen(buffer)), 108);

		RctSetRectangle(&rct, x+objWidth+2, 101, objWidth, h);
		FldSetBounds(fld, &rct);
		rct.topLeft.x--;
		rct.extent.x+=2;
		WinDrawGrayRectangleFrame(simpleFrame, &rct);
	} else {
		RctSetRectangle(&rct, x, 108, objWidth, h);
		FldSetBounds(fld, &rct);
		rct.topLeft.x--;
		rct.extent.x+=2;
		WinDrawGrayRectangleFrame(simpleFrame, &rct);

		StrPrintF(buffer, "%u", (session->quiz)&0x0f);
		WinDrawChars(buffer, StrLen(buffer), x+4+objWidth, 101);
	}

	FntSetFont(fID);

	UIObjectShow(cGameField);
	UIFieldFocus(cGameField);
}

/*
 * SetupString
 *
 * Display string and fire off timer.
 *
 *  -> str		String to display.
 *  -> errCol		Error color?
 */
static void
SetupString(Char *str, Boolean errCol)
{
	FontID fID=FntSetFont(largeBoldFont);
	RGBColorType red={ 0, 255, 0, 0 };
	IndexedColorType ict=0;
	RectangleType rct;
	UInt16 len, w;

	if (errCol & ui->colorEnabled)
		ict=WinSetTextColor(WinRGBToIndex(&red));

	RctSetRectangle(&rct, 0, 80, 160, FntCharHeight());
	WinEraseRectangle(&rct, 0);
	len=StrLen(str);
	w=FntCharsWidth(str, len);
	deleteStringDelay=2*SysTicksPerSecond();
	WinDrawChars(str, len, (160-w)/2, rct.topLeft.y);
	FntSetFont(fID);

	if (errCol & ui->colorEnabled)
		WinSetTextColor(ict);
}

/*
 * fHelpRun
 *
 * Show the Help form.
 */
static void
fHelpRun(void)
{
	MemHandle mh=DmGetResource(strRsc, strHelpText);
	FormSave frm;

	ErrFatalDisplayIf(mh==NULL, "(fHelpRun) Cannot lock string resource.");

	UIFormPrologue(&frm, fHelp, EH);
	UIFieldSetText(cHelpTxt, MemHandleLock(mh));
	MemHandleUnlock(mh);
	DmReleaseResource(mh);
	UIFieldFocus(cHelpTxt);
	UIFieldUpdateScrollBar(cHelpTxt, cHelpScrBar);
	UIFormEpilogue(&frm, NULL, 0);
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	if (ev->eType==keyDownEvent)
		UIFieldScrollBarKeyHandler(ev, cHelpTxt, cHelpScrBar, true);

	UIFieldScrollBarHandler(ev, cHelpTxt, cHelpScrBar);
	return false;
}

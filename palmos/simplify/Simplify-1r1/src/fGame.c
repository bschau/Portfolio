/*
 * fGame.c
 */
#include "Include.h"

/* protos */
static void UpdateForm(void);
static void UpdateInfo(void);
static void DrawQuiz(void);
static Boolean Guess(void);
static Boolean GetValue(UInt16 *, UInt16);
static void UpdateDenominatorLevel(void);

/* globals */
static UInt16 focusBand[]={ cGameNumerator, cGameDenominator, 0 };

/*
 * fGameInit
 */
Boolean
fGameInit(void)
{
	if (session==NULL)
		SssNew();

	return true;
}

/*
 * fGameEH
 */
Boolean
fGameEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		UpdateForm();
		return true;

	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==cGameCheck)
			return Guess();

		break;

	case popSelectEvent:
		if (ev->data.popSelect.controlID==cGamePopup) {
			if (ev->data.popSelect.selection!=noListSelection) {
				if (ev->data.popSelect.selection!=ev->data.popSelect.priorSelection) {
					if (FrmAlert(aChange)==0) {
						level=ev->data.popSelect.selection;
						SssUpdate(0);
						session->score=0;
						session->round=1;
						UpdateForm();
					}
				}
			}
			return true;
		}
		break;

	case menuEvent:
		if (ev->data.menu.itemID==mGameAbout) {
			About();
			return true;
		} else if (ev->data.menu.itemID==mGameReset) {
			if (FrmAlert(aReset)==0) {
				SssNew();
				UpdateForm();
			}
			return true;
		}
		break;

	case keyDownEvent:
		UIFieldRefocus(focusBand, ev->data.keyDown.chr);
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * UpdateForm
 *
 * Redraw game form.
 */
static void
UpdateForm(void)
{
	RectangleType r;

	UIFieldClear(cGameNumerator);
	UIFieldClear(cGameDenominator);

	r.topLeft.x=0;
	r.topLeft.y=25;
	r.extent.x=160;
	r.extent.y=110;
	WinEraseRectangle(&r, 0);

	UpdateInfo();
	DrawQuiz();
	UpdateDenominatorLevel();
	UIFieldFocus(cGameNumerator);
}

/*
 * UpdateInfo
 *
 * Updates round and score display.
 */
static void
UpdateInfo(void)
{
	if (session->round-1) {
		FontID fId=FntSetFont(largeBoldFont);
		UInt16 pW=160, len;
		Char buffer[40];
		Boolean trunc;

		StrPrintF(buffer, "Score %ld out of %ld", session->score, session->round-1);
		len=StrLen(buffer);
		FntCharsInWidth(buffer, &pW, &len, &trunc);

		WinDrawChars(buffer, len, (160-pW)/2, 90);
		FntSetFont(fId);
	}
}

/*
 * DrawQuiz
 *
 * Draw the fraction to reduce.
 */
static void
DrawQuiz(void)
{
	UInt16 lineSize=0;
	Char buffer[16];
	UInt16 len, pW;
	FontID fId;
	Boolean trunc;
	RectangleType r;

	fId=FntSetFont(largeBoldFont);
	pW=50;
	StrPrintF(buffer, "%d", session->numerator);
	len=StrLen(buffer);
	FntCharsInWidth(buffer, &pW, &len, &trunc);

	if (pW>lineSize)
		lineSize=pW;

	WinDrawChars(buffer, len, 74-pW-1, 40);

	pW=50;
	StrPrintF(buffer, "%d", session->denominator);
	len=StrLen(buffer);
	FntCharsInWidth(buffer, &pW, &len, &trunc);

	if (pW>lineSize)
		lineSize=pW;

	WinDrawChars(buffer, len, 74-pW-1, 40+FntCharHeight()+2);

	lineSize+=2;

	pW=40+FntCharHeight();
	WinDrawLine(74-lineSize, pW, 74, pW);

	WinDrawLine(78, pW-1, 82, pW-1);
	WinDrawLine(78, pW+1, 82, pW+1);

	WinDrawLine(86, pW, 115, pW);
	FntSetFont(fId);

	r.topLeft.x=88;
	r.topLeft.y=39;
	r.extent.x=26;
	r.extent.y=14;
	WinDrawGrayRectangleFrame(simpleFrame, &r);

	r.topLeft.y=58;
	WinDrawGrayRectangleFrame(simpleFrame, &r);
}

/*
 * Guess
 *
 * Handle guess.
 *
 * Return true if guess is valid, false otherwise.
 */
static Boolean
Guess(void)
{
	UInt16 a=session->numerator, b=session->denominator, q, r, gN, gD, pts;
	Char numerator[16], denominator[16];

	if (GetValue(&gN, cGameNumerator)==false)
		return false;

	if (GetValue(&gD, cGameDenominator)==false)
		return false;

	for (;;) {
		q=b/a;
		r=b-(q*a);
		if (!r)
			break;

		b=a;
		a=r;
	}

	q=(session->numerator)/a;
	r=(session->denominator)/a;
	StrPrintF(numerator, "%d", q);
	StrPrintF(denominator, "%d", r);

	if ((q==gN) && (r==gD)) {
		pts=1;
		FrmCustomAlert(aRightOn, numerator, denominator, NULL);
	} else {
		pts=0;
		FrmCustomAlert(aAnswer, numerator, denominator, NULL);
	}

	SssUpdate(pts);

	UpdateForm();
	return true;
}

/*
 * GetValue
 *
 * Pickup and validate a integer field value.
 *
 *  -> d	Destination.
 *  -> id	Field ID.
 *
 * Returns true if value is ok, false otherwise.
 */
static Boolean
GetValue(UInt16 *d, UInt16 id)
{
	UInt16 v, i;
	Char *g;

	if ((g=UIFieldGetText(id))==NULL) {
		FrmAlert(aGuessInvalid);
		return false;
	}

	for (i=0; i<StrLen(g); i++) {
		if (g[i]<'0' || g[i]>'9') {
			FrmAlert(aGuessInvalid);
			return false;
		}
	}

	v=StrAToI(g);
	if (v<1 || v>1000) {
		FrmAlert(aGuessInvalid);
		return false;
	}

	*d=v;
	return true;
}

/*
 * UpdateDenominatorLevel
 *
 * Update denominator popup.
 */
static void
UpdateDenominatorLevel(void)
{
	ListType *lst=UIFormGetObject(cGameList);

	LstSetSelection(lst, level);
	CtlSetLabel(UIFormGetObject(cGamePopup), LstGetSelectionText(lst, level));
}

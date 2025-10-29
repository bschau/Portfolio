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
static void PrintD(UInt16, UInt16, UInt16, UInt16, Boolean);
static void PrintGrayBox(UInt16, UInt16, UInt16);
static void UpdateTrigger(void);

/* globals */
static UInt16 boxX[]={ 32, 70, 100 };
static UInt16 boxY[]={ 39, 58, 49 };
static UInt16 focusBand[]={ cGameLHSN, cGameLHSD, cGameRHSW, cGameRHSN, cGameRHSD, 0 };
static Char levelTrigger[32];
static Boolean erase;

/*
 * fGameInit
 */
Boolean
fGameInit(void)
{
	if (session==NULL)
		SssNew();

	erase=true;
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
		else if (ev->data.ctlSelect.controlID==cGameTrigger) {
			UInt16 l=level;

			Preferences();
			if (l!=level) {
				SssNew();
				UpdateForm();
				UpdateTrigger();
			}
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

	if (erase) {
		UIFieldClear(cGameLHSN);
		UIFieldClear(cGameLHSD);
		UIFieldClear(cGameRHSW);
		UIFieldClear(cGameRHSN);
		UIFieldClear(cGameRHSD);
	} else
		erase=true;

	r.topLeft.x=0;
	r.topLeft.y=25;
	r.extent.x=160;
	r.extent.y=110;
	WinEraseRectangle(&r, 0);

	UpdateTrigger();
	UpdateInfo();
	DrawQuiz();
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
	FontID fId=FntSetFont(largeBoldFont);
	UInt16 fH;

	fH=FntCharHeight()+1;
	if (session->mask&1)
		PrintD(cGameLHSN, boxX[0], boxY[0], session->numerator, true);
	else
		PrintGrayBox(cGameLHSN, boxX[0], boxY[0]);

	WinDrawLine(boxX[0]-2, boxY[0]+fH, boxX[0]+25, boxY[0]+fH);

	if (session->mask&2)
		PrintD(cGameLHSD, boxX[0], boxY[1], session->denominator, true);
	else
		PrintGrayBox(cGameLHSD, boxX[0], boxY[1]);

	WinDrawLine(60, boxY[0]+fH-1, 65, boxY[0]+fH-1);
	WinDrawLine(60, boxY[0]+fH+1, 65, boxY[0]+fH+1);

	if (session->mask&4)
		PrintD(cGameRHSW, boxX[1], boxY[2], session->whole, false);
	else
		PrintGrayBox(cGameRHSW, boxX[1], boxY[2]);	

	if (session->mask&8)
		PrintD(cGameRHSN, boxX[2], boxY[0], session->wNum, true);
	else
		PrintGrayBox(cGameRHSN, boxX[2], boxY[0]);

	WinDrawLine(boxX[2]-2, boxY[0]+fH, boxX[2]+25, boxY[0]+fH);

	if (session->mask&16)
		PrintD(cGameRHSD, boxX[2], boxY[1], session->wDenom, true);
	else
		PrintGrayBox(cGameRHSD, boxX[2], boxY[1]);

	FntSetFont(fId);

	if (level&LvlMixed) {
		if (!(session->mask&4))
			UIFieldFocus(cGameRHSW);
		else if (!(session->mask&8))
			UIFieldFocus(cGameRHSN);
		else
			UIFieldFocus(cGameRHSD);
	} else
		UIFieldFocus(cGameLHSN);
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
	Char wBuf[8], nBuf[8], dBuf[8];
	UInt16 w, n, d, num, denom;
	Int16 pts=0;

	erase=true;
	if (level&LvlMixed) {
		w=session->whole;
		n=session->wNum;
		d=session->wDenom;

		if (!(session->mask&4)) {
			if (GetValue(&w, cGameRHSW)==false)
				return false;
		}

		if (!(session->mask&8)) {
			if (GetValue(&n, cGameRHSN)==false)
				return false;
		}

		if (!(session->mask&16)) {
			if (GetValue(&d, cGameRHSD)==false)
				return false;
		}

		StrPrintF(wBuf, "%d", session->whole);
		StrPrintF(nBuf, "%d", session->wNum);
		StrPrintF(dBuf, "%d", session->wDenom);

		num=session->numerator;
		denom=session->denominator;
		UtilsReduce(&num, &denom);

		if (num==session->numerator && denom==session->denominator) {
			// Reduced
			if (w==session->whole && n==session->wNum && d==session->wDenom) {
				pts=1;
				FrmCustomAlert(aRightOnMixed, wBuf, nBuf, dBuf);
			} else {
				pts=0;
				FrmCustomAlert(aAnswerMixed, wBuf, nBuf, dBuf);
			}
		} else {
			// Non-reduced
			if (w==session->whole && n==session->wNum && d==session->wDenom) {
				pts=1;
				FrmCustomAlert(aRightOnMixed, wBuf, nBuf, dBuf);
			} else {

				// try to reduce
				UtilsReduce(&n, &d);
				if (w==session->whole && n==session->wNum && d==session->wDenom) {
		       
//			else if ((session->numerator==((w*d)+n)) && (session->denominator==d)) {
					if (session->guesses) {
						pts=0;
						FrmCustomAlert(aNotSimpleMixed, wBuf, nBuf, dBuf);
					} else {
						erase=false;
						pts=-1;
						FrmAlert(aAlmostRightMixed);
					}
				} else {
					pts=0;
					FrmCustomAlert(aAnswerMixed, wBuf, nBuf, dBuf);
				}
			}
		}
	} else {
		n=session->numerator;
		d=session->denominator;

		if (!(session->mask&1)) {
			if (GetValue(&n, cGameLHSN)==false)
				return false;
		}

		if (!(session->mask&2)) {
			if (GetValue(&d, cGameLHSD)==false)
				return false;
		}

		StrPrintF(nBuf, "%d", session->numerator);
		StrPrintF(dBuf, "%d", session->denominator);

		if (n==session->numerator && d==session->denominator) {
			pts=1;
			FrmCustomAlert(aRightOnFraction, nBuf, dBuf, NULL);
		} else {
			pts=0;
			FrmCustomAlert(aAnswerFraction, nBuf, dBuf, NULL);
		}
	}

	if (pts>=0)
		SssUpdate(pts);
	else 
		session->guesses++;

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
 * PrintD
 *
 * Print a centered or rightaligned number.
 *
 *  -> id	ID of object to hide.
 *  -> x	Box X.
 *  -> y	Box y.
 *  -> n	Number.
 *  -> c	Center (=true), rightalign (=false).
 */
static void
PrintD(UInt16 id, UInt16 x, UInt16 y, UInt16 n, Boolean c)
{
	Char buffer[16];
	UInt16 pW, len;
	Boolean trunc;

	UIHideObject(id);
	StrPrintF(buffer, "%d", n);
	pW=24;
	len=StrLen(buffer);
	FntCharsInWidth(buffer, &pW, &len, &trunc);
	if (c)
		WinDrawChars(buffer, len, x+((24-pW)/2), y);
	else
		WinDrawChars(buffer, len, x+24-pW, y);
}

/*
 * PrintGrayBox
 *
 * Print a grayed box around the field.
 *
 *  -> id	ID of object to show.
 *  -> x	Top X.
 *  -> y	Top Y.
 */
static void
PrintGrayBox(UInt16 id, UInt16 x, UInt16 y)
{
	RectangleType r;

	UIShowObject(id);

	r.topLeft.x=x-1;
	r.topLeft.y=y;
	r.extent.x=26;
	r.extent.y=14;

	WinDrawGrayRectangleFrame(simpleFrame, &r);
}

/*
 * UpdateTrigger
 *
 * Update the selector trigger.
 */
static void
UpdateTrigger(void)
{
	if (level&LvlMixed)
		StrCopy(levelTrigger, "Mixed, ");
	else
		StrCopy(levelTrigger, "Fractions, ");

	switch (level&LvlDenomMask) {
	case 0:
		StrCat(levelTrigger, "10");
		break;
	case 1:
		StrCat(levelTrigger, "20");
		break;
	case 2:
		StrCat(levelTrigger, "100");
		break;
	default:
		StrCat(levelTrigger, "200");
		break;
	}

	if (level&LvlAdvanced)
		StrCat(levelTrigger, ", Advanced");
	else
		StrCat(levelTrigger, ", Beginner");

	CtlSetLabel(UIFormGetObject(cGameTrigger), levelTrigger);
}

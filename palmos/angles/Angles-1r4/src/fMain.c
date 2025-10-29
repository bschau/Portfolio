/*
 * fMain.c
 */
#include "Include.h"

/* protos */
static void DrawScores(UInt16);

/*
 * fMainEH
 */
Boolean
fMainEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
	case frmUpdateEvent:
		FrmDrawForm(currentForm);
		WinDrawLine(54, 47, 104, 47);
		DrawScores(52);
		return true;

	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==cMainStart) {
			FrmGotoForm(fGame);
			return true;
		}
		break;

	case menuEvent:
		if (ev->data.menu.itemID==mMainAbout) {
			About();
			return true;
		} else if (ev->data.menu.itemID==mMainReset) {
			if (FrmAlert(aReset)==0) {
				RectangleType r;

				LowReset();

				r.topLeft.x=0;
				r.topLeft.y=52;
				r.extent.x=160;
				r.extent.y=80;
				WinEraseRectangle(&r, 0);
				FrmUpdateForm(fGame, frmRedrawUpdateCode);
				return true;
			}
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * DrawScores
 *
 * Draw High Score table.
 *
 *  -> y	Top pixel.
 */
static void
DrawScores(UInt16 y)
{
	FontID fId=FntSetFont(largeFont);
	UInt16 maxWidth=0, pos, pW, w, xPlus;
	Char *l;
	UInt16 score, lLen;
	Boolean trunc;
	Char buffer[16];

	/*
	 * Layout:
	 *
	 * 10		Position
	 * 4		Spacer
	 * 30		Score
	 * 6		Spacer
	 * rest
	 */
	for (pos=1; pos<6; pos++) {
		LowGetPair(pos, &l, &lLen, &score);

		pW=160;
		FntCharsInWidth(l, &pW, &lLen, &trunc);

		if (pW>maxWidth)
			maxWidth=pW;

		FntSetFont(stdFont);
	}

	if (maxWidth>110)
		maxWidth=110;

	xPlus=(160-50-maxWidth)/2;

	FntSetFont(largeFont);
	for (pos=1; pos<6; pos++) {
		LowGetPair(pos, &l, &lLen, &score);

		StrPrintF(buffer, "%d.", pos);
		w=2;
		pW=10;
		FntCharsInWidth(buffer, &pW, &w, &trunc);
		WinDrawChars(buffer, w, xPlus+(10-pW), y);

		StrPrintF(buffer, "%d", score);
		w=StrLen(buffer);
		pW=30;
		FntCharsInWidth(buffer, &pW, &w, &trunc);
		WinDrawChars(buffer, w, xPlus+(44-pW), y);

		pW=110;
		FntCharsInWidth(l, &pW, &lLen, &trunc);
		WinDrawChars(l, lLen, xPlus+50, y);

		y+=FntCharHeight();
		FntSetFont(stdFont);
	}
	FntSetFont(fId);
}

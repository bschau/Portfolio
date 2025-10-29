/*
 * fDictationDone.c
 */
#include "Include.h"

/* protos */

/* globals */

/*
 * fDictationDoneEH
 */
Boolean
fDictationDoneEH(EventType *ev)
{
	UInt16 pW=160, len;
	FontID fId;
	Char buffer[GroupNameLength+1];		// Must be >40
	Boolean trunc;
	RectangleType r;

	switch (ev->eType) {
	case frmOpenEvent:
		if (game.gameEnded.year==0)
			DateSecondsToDate(TimGetSeconds(), &game.gameEnded);

		FrmDrawForm(currentForm);
		if (game.saved==false)
			UIObjectShow(cDictationDoneSave);

	case frmUpdateEvent:
		fId=FntSetFont(largeBoldFont);

		PMGetGroupName(&dbSGroup, game.guid, buffer);

		r.topLeft.x=0;
		r.topLeft.y=30;
		r.extent.x=160;
		r.extent.y=FntCharHeight();

		len=StrLen(buffer);
		FntCharsInWidth(buffer, &pW, &len, &trunc);

		WinEraseRectangle(&r, 0);
		WinDrawChars(buffer, len, (160-pW)/2, r.topLeft.y);

		r.topLeft.y=84;

		if (game.round)
			StrPrintF(buffer, "%d%%", (game.score*100/game.round));
		else
			StrPrintF(buffer, "0%%");

		len=StrLen(buffer);
		pW=160;
		FntCharsInWidth(buffer, &pW, &len, &trunc);

		WinEraseRectangle(&r, 0);
		WinDrawChars(buffer, len, (160-pW)/2, r.topLeft.y);

		FntSetFont(fId);

		r.topLeft.y+=r.extent.y;
		r.extent.y=FntCharHeight();
		r.topLeft.y+=r.extent.y;

		StrPrintF(buffer, "(%d out of %d)", game.score, game.round);
		len=StrLen(buffer);
		pW=160;
		FntCharsInWidth(buffer, &pW, &len, &trunc);

		WinEraseRectangle(&r, 0);
		WinDrawChars(buffer, len, (160-pW)/2, r.topLeft.y);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cDictationDoneView:
			if (XRefCount(&dbSXref, game.guid)>0)
				FrmGotoForm(fViewAnswers);
			else
				FrmAlert(aViewNoDatabase);

			return true;

		case cDictationDoneGroups:
			FrmGotoForm(fStudent);
			return true;

		case cDictationDoneSave:
			if (fSaveScoreRun()==true) {
				game.saved=true;
				PMSetPref(&game, sizeof(Game), PrfGameInfo);
				UIObjectHide(cDictationDoneSave);
			}
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

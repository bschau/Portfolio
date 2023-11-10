/*
 * fEnd.c
 */
#include "Include.h"

/* protos */
static void UpdateDisplay(void);
static Int16 MakeRoomInBestScorersTable();

/* globals */
static Int16 lbPos;

/*
 * fEndInit
 */
Boolean
fEndInit(void)
{
	if ((lbPos=MakeRoomInBestScorersTable())>-1) {
		Char buffer[4];

		UIObjectShow(cEndLbl1);
		UIObjectShow(cEndLbl2);

		StrPrintF(buffer, "%u. ", lbPos+1);
		UIFieldSetText(cEndPos, buffer);
		UIObjectShow(cEndPos);

		UIObjectShow(cEndName);
		if (*prefs.lastName)
			UIFieldSelectAll(cEndName, prefs.lastName, true);
		else
			UIFieldFocus(cEndName);
	}

	return true;
}

/*
 * fEndEH
 */
Boolean
fEndEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		UpdateDisplay();
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cEndDone:
			if (lbPos>-1) {
				Char *p=UIFieldGetText(cEndName);
				UInt16 offset=(prefs.flags*5)+lbPos, len;

				bestScorers[offset].time=TimGetSeconds();
				bestScorers[offset].score=session->score;
				MemSet(bestScorers[offset].name, BestScorerNameLen+1, 0);
				
				if (p && (len=StrLen(p))>0)
					MemMove(bestScorers[offset].name, p, len);
				else
					StrCopy(bestScorers[offset].name, "Noname");

				MemMove(prefs.lastName, bestScorers[offset].name, BestScorerNameLen+1);
			}
			SssDestroy();
			FrmGotoForm(fMain);
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * UpdateDisplay
 */
static void
UpdateDisplay(void)
{
	FontID fID=FntSetFont(boldFont);
	Char buffer[32];
	Int16 w;
	UInt16 len;

	StrPrintF(buffer, "You scored: %u", session->score);
	len=StrLen(buffer);
	w=FntCharsWidth(buffer, len);
	if (lbPos==-1)
		WinDrawChars(buffer, len, (160-w)/2, 70);
	else
		WinDrawChars(buffer, len, (160-w)/2, 60);

	FntSetFont(fID);
}

/*
 * MakeRoomInBestScorersTable
 *
 * Shift scores down if score is qualifed for the Best Scorers table.
 *
 * Return position (0 based) or -1 if score didn't qualify.
 */
static Int16
MakeRoomInBestScorersTable(void)
{
	UInt16 offset=prefs.flags*5, idx;
	Int16 cnt;

	for (cnt=0; cnt<5; cnt++) {
		if (bestScorers[offset].time) {
			if (bestScorers[offset].score>=session->score) {
				if (cnt<4) {
					offset=(prefs.flags*5)+4;
					for (idx=4; idx>cnt; idx--) {
						MemMove(&bestScorers[offset].time, &bestScorers[offset-1].time, sizeof(BestScorerEntry));
						offset--;
					}
				}

				return cnt;
			}
		} else
			return cnt;

		offset++;
	}

	return -1;
}

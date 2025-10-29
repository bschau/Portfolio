/*
 * fQuizOver.c
 */
#include <DLServer.h>
#include "Include.h"

/* protos */
static void UpdateHiScores(void);

/* globals */
static Char name[NameLength+1];
static Char score[40];
static UInt16 scoreX;

/*
 * fQuizOverInit
 */
Boolean
fQuizOverInit(void)
{
	FontID fID=FntSetFont(largeBoldFont);
	UInt16 w;

	inGame=false;
	StrPrintF(score, "%lu out of %lu", session->score, session->round-1);
	w=FntCharsWidth(score, StrLen(score));
	scoreX=(160-w)/2;
	FntSetFont(fID);

	MemSet(name, sizeof(name), 0);

	if (PMGetPref(name, NameLength, PrfLastName)==false) {
		Char bufName[dlkUserNameBufSize+1];

		if (DlkGetSyncInfo(NULL, NULL, NULL, bufName, NULL, NULL)==errNone) {
			if ((w=StrLen(bufName))>NameLength)
				w=NameLength;

			MemMove(name, bufName, w+1);
		}

		if (StrLen(bufName)==0)
			StrCopy(name, "Eddie the Psyched PDA");
	}

	if ((w=StrLen(name))>0) {
		FieldType *fld=UIFormGetObject(cQuizOverName);

		UIFieldSetText(cQuizOverName, name);
		FldSetSelection(fld, 0, FldGetTextLength(fld));
	}

	UIFieldFocus(cQuizOverName);
	return true;
}

/*
 * fQuizOverEH
 */
Boolean
fQuizOverEH(EventType *ev)
{
	RectangleType r;
	FontID fID;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		RctSetRectangle(&r, 0, 74, 160, 16);
		WinEraseRectangle(&r, 0);
		fID=FntSetFont(largeBoldFont);
		WinDrawChars(score, StrLen(score), r.topLeft.x+scoreX, r.topLeft.y);
		FntSetFont(fID);
		return true;

	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==cQuizOverOK) {
			Char *p=UIFieldGetText(cQuizOverName);

			if (p && StrLen(p)>0) {
				MemSet(name, NameLength+1, 0);
				StrCopy(name, p);
			}

			UpdateHiScores();
			SssDestroy();
			FrmGotoForm(fScores);
			return true;
		} else if (ev->data.ctlSelect.controlID==cQuizOverDont) {
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
 * UpdateHiScores
 *
 * Update the Hi Scores block.
 */
static void
UpdateHiScores(void)
{
	MemMove(record+1, record, (MaxRecords-2)*sizeof(Record));

	MemSet(record, sizeof(Record), 0);

	record->started=session->started;
	record->stopped=session->stopped;
	record->round=session->round-1;
	record->score=session->score;
	record->operation=session->defOperations;
	record->timedMins=session->timedSecs/60;
	record->max=session->max;
	record->min=session->min;
	StrCopy(record->name, name);

	PMSetPref(name, NameLength, PrfLastName);
}

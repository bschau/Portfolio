/*
 * fExport.c
 */
#include "Include.h"

/* protos */
_FRM(static Boolean EH(EventType *));

/* globals */
static Char catToName[dmCategoryLength+2];
static UInt16 catTo;
static DB dbMemo;

/*
 * Export
 */
void
fExportRun(void)
{
	UInt32 pDateFormat=PrefGetPreference(prefDateFormat), pTimeFormat=PrefGetPreference(prefTimeFormat), len;
	FormSave frm;
	Char *memo, *p;
	UInt16 idx, attrs;
	DateTimeType dt;
	MemHandle mh, rh;
	Char *ds;

	DBInit(&dbMemo, "MemoDB");
	if (DBOpen(&dbMemo, dmModeReadWrite, true)==false)
		return;

	UIFormPrologue(&frm, fExport, EH);

	catTo=prefs.catMemoIdx;
	UIPopupSet(dbMemo.db, catTo, catToName, cExportPopup);
	if (StrLen(prefs.lastMemoTitle)>0) {
		UIFieldSetText(cExportField, prefs.lastMemoTitle);
		FldSetSelection(UIObjectGet(cExportField), 0, FldGetTextLength(UIObjectGet(cExportField)));
	}

	UIFieldFocus(cExportField);

	if (UIFormEpilogue(&frm, NULL, cExportCancel)==false) {
		DBClose(&dbMemo);
		return;
	}

	prefs.catMemoIdx=catTo;

	mh=MemHandleNew(600);
	ErrFatalDisplayIf(mh==NULL, "(fExport) Out of memory.");
	memo=MemHandleLock(mh);
	ds=memo;
	if (StrLen(prefs.lastMemoTitle)>0) {
		StrCopy(memo, prefs.lastMemoTitle);
		StrCat(memo, "\n\n");
		memo+=StrLen(memo);
	}

	for (idx=0; idx<game.noOfPlayers; idx++) {
		StrPrintF(memo, "%d. %s (%d) %d\n", idx+1, &game.names[idx][0], game.phase[idx], game.scores[idx]);
		memo+=StrLen(memo);
	}

	StrPrintF(memo, "\n%s: ", StringGet(Str15));
	memo+=StrLen(memo);
	TimSecondsToDateTime(game.gameStarted, &dt);
	DateToAscii(dt.month, dt.day, dt.year, pDateFormat, memo);
	memo+=StrLen(memo);
	*memo++=' ';
	TimeToAscii(dt.hour, dt.minute, pTimeFormat, memo);
	memo+=StrLen(memo);

	StrPrintF(memo, "\n%s: ", StringGet(Str16));
	memo+=StrLen(memo);
	TimSecondsToDateTime(game.gameEnded, &dt);
	DateToAscii(dt.month, dt.day, dt.year, pDateFormat, memo);
	memo+=StrLen(memo);
	*memo++=' ';
	TimeToAscii(dt.hour, dt.minute, pTimeFormat, memo);
	memo+=StrLen(memo);
	*memo++='\n';
	*memo='\x00';

	MemHandleUnlock(mh);

	memo=MemHandleLock(mh);
	len=StrLen(memo)+1;

	idx=dmMaxRecordIndex;
	rh=DmNewRecord(dbMemo.db, &idx, len);
	ErrFatalDisplayIf(rh==NULL, "(fExport) Device is full.");

	p=MemHandleLock(rh);
	DmWrite(p, 0, memo, len);
	MemHandleUnlock(rh);
	MemHandleFree(mh);

	DmRecordInfo(dbMemo.db, idx, &attrs, NULL, NULL);
	attrs&=~dmRecAttrCategoryMask;
	attrs|=prefs.catMemoIdx;
	DmSetRecordInfo(dbMemo.db, idx, &attrs, NULL);
	DmReleaseRecord(dbMemo.db, idx, true);
	DBClose(&dbMemo);
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	Char *p;

	if (ev->eType==ctlSelectEvent) {
		switch (ev->data.ctlSelect.controlID) {
		case cExportPopup:
			CategorySelect(dbMemo.db, currentForm, cExportPopup, cExportList, false, &catTo, catToName, 0, categoryHideEditCategory);
			return true;
		case cExportExport:
			if ((p=UIFieldGetText(cExportField)))
				StrNCopy(prefs.lastMemoTitle, p, MemoLength);
		default:	/* FALL-THRU */
			break;
		}
	}

	return false;
}

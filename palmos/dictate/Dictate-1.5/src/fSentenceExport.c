/*
 * fSentenceExport.c
 */
#include "Include.h"

/* macros */
#define MemoSize 4096
#define RealMemoSize (MemoSize-1)

/* protos */
static Boolean EH(EventType *);
static void ResetMemo(Char *);
static void SetMemo(DmOpenRef, Char *, UInt16);

/* globals */
static UInt16 memoSize;

/*
 * fSentenceExportRun
 */
void
fSentenceExportRun(void)
{
	UInt16 exported=0, sIdx=0;
	FormSave frm;
	Char buffer[16];
	MemHandle memoH;
	Char *memoPtr, *memo;

	DBClear(&dbMemo);
	if (DBOpen(&dbMemo, dmModeReadWrite, true)==false)
		return;

	UIFormPrologue(&frm, fSentenceExport, EH);

	catTo=prefs.catExchangeIdx;
	catFrom=prefs.catSentenceIdx;
	UIPopupSet(&dbMemo, catTo, catToName, cSentenceExportPopupTo);
	UIPopupSet(&dbTSentence, catFrom, catFromName, cSentenceExportPopupFrom);

	if (prefs.exchangeFlags&ExchgEDelete)
		CtlSetValue(UIObjectGet(cSentenceExportDelete), 1);

	if (UIFormEpilogue(&frm, NULL, cSentenceExportCancel)==false) {
		DBClose(&dbMemo);
		return;
	}

	prefs.catExchangeIdx=catTo;

	memoH=MemHandleNew(MemoSize);
	ErrFatalDisplayIf(memoH==NULL, "(fSentenceExportRun) Device is full.");
	memoPtr=MemHandleLock(memoH);
	ResetMemo(memoPtr);

	memo=memoPtr;
	for (EVER) {
		MemHandle mh;
		Char *m;
		UInt16 len;

		if ((mh=DmQueryNextInCategory(dbTSentence.db, &sIdx, catFrom))) {
			m=MemHandleLock(mh);
			len=StrLen(m);
			if (len>(memoSize-1)) {
				SetMemo(dbMemo.db, memoPtr, catTo);
				ResetMemo(memoPtr);
				memo=memoPtr;
			}

			MemMove(memo, m, len);
			memo+=len;
			*memo++='\n';
			memoSize-=(len+1);

			exported++;
			if (prefs.exchangeFlags&ExchgEDelete)
				DmDeleteRecord(dbTSentence.db, sIdx);

			sIdx++;
		} else
			break;
	}

	if (memoSize!=RealMemoSize)
		SetMemo(dbMemo.db, memoPtr, catTo);

	MemHandleFree(memoH);
	DBClose(&dbMemo);
	StrPrintF(buffer, "%d", exported);
	FrmCustomAlert(aSentenceExport, buffer, (exported==1) ? " " : "s ", NULL);
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cSentenceExportPopupFrom:
			CategorySelect(dbTSentence.db, currentForm, cSentenceExportPopupFrom, cSentenceExportListFrom, true, &catFrom, catFromName, 1, categoryHideEditCategory);
			return true;

		case cSentenceExportPopupTo:
			CategorySelect(dbMemo.db, currentForm, cSentenceExportPopupTo, cSentenceExportListTo, false, &catTo, catToName, 0, categoryHideEditCategory);
			return true;

		case cSentenceExportExport:
			if ((CtlGetValue(UIObjectGet(cSentenceExportDelete))))
				prefs.exchangeFlags|=ExchgEDelete;
			else
				prefs.exchangeFlags&=~ExchgEDelete;

			break;
		}

	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * ResetMemo
 *
 * Reset the memo information.
 *
 *  -> m		Memo.
 */
static void
ResetMemo(Char *m)
{
	memoSize=RealMemoSize;
	MemSet(m, RealMemoSize, 0);
}

/*
 * SetMemo
 *
 * Commit a memo to the memo pad.
 *
 *  -> db		DmOpenRef.
 *  -> m		Memo.
 *  -> catTo		Category.
 */
static void
SetMemo(DmOpenRef db, Char *m, UInt16 catTo)
{
	UInt16 len=RealMemoSize-memoSize, rIdx=dmMaxRecordIndex, attrs;
	MemHandle rh;
	void *p;

	rh=DmNewRecord(db, &rIdx, len+1);
	ErrFatalDisplayIf(rh==NULL, "(SetMemo) Device is full.");

	p=MemHandleLock(rh);
	DmWrite(p, 0, m, len+1);
	MemHandleUnlock(rh);

	DmRecordInfo(db, rIdx, &attrs, NULL, NULL);
	attrs&=~dmRecAttrCategoryMask;
	attrs|=catTo;
	DmSetRecordInfo(db, rIdx, &attrs, NULL);
	DmReleaseRecord(db, rIdx, true);
}

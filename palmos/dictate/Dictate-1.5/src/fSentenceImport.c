/*
 * fSentenceImport.c
 */
#include "Include.h"

/* protos */
static Boolean EH(EventType *);
static UInt16 GetMemos(Char *, UInt16);

/*
 * fSentenceImportRun
 */
void
fSentenceImportRun(void)
{
	UInt16 imported=0, memoIdx=0;
	FormSave frm;
	Char buffer[16];

	DBClear(&dbMemo);
	if (DBOpen(&dbMemo, dmModeReadWrite, true)==false)
		return;

	UIFormPrologue(&frm, fSentenceImport, EH);

	catTo=prefs.catSentenceIdx;
	catFrom=prefs.catExchangeIdx;
	UIPopupSet(&dbTSentence, catTo, catToName, cSentenceImportPopupTo);
	UIPopupSet(&dbMemo, catFrom, catFromName, cSentenceImportPopupFrom);

	if (prefs.exchangeFlags&ExchgIDelete)
		CtlSetValue(UIObjectGet(cSentenceImportDelete), 1);

	if (UIFormEpilogue(&frm, NULL, cSentenceImportCancel)==false) {
		DBClose(&dbMemo);
		return;
	}

	prefs.catExchangeIdx=catFrom;

	for (EVER) {
		MemHandle mh;

		if ((mh=DmQueryNextInCategory(dbMemo.db, &memoIdx, catFrom))) {
			imported+=GetMemos(MemHandleLock(mh), catTo);
			MemHandleUnlock(mh);

			if (prefs.exchangeFlags&ExchgIDelete)
				DmDeleteRecord(dbMemo.db, memoIdx);

			memoIdx++;
		} else
			break;
	}

	DBClose(&dbMemo);
	StrPrintF(buffer, "%d", imported);
	FrmCustomAlert(aSentenceImport, buffer, (imported==1) ? " " : "s ", NULL);
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
		case cSentenceImportPopupFrom:
			CategorySelect(dbMemo.db, currentForm, cSentenceImportPopupFrom, cSentenceImportListFrom, true, &catFrom, catFromName, 0, categoryHideEditCategory);
			return true;

		case cSentenceImportPopupTo:
			CategorySelect(dbTSentence.db, currentForm, cSentenceImportPopupTo, cSentenceImportListTo, false, &catTo, catToName, 1, categoryHideEditCategory);
			return true;

		case cSentenceImportImport:
			if ((CtlGetValue(UIObjectGet(cSentenceImportDelete))))
				prefs.exchangeFlags|=ExchgIDelete;
			else
				prefs.exchangeFlags&=~ExchgIDelete;

			break;
		}

	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * GetMemos
 *
 * Extract and insert sentences from memo. One sentence per line.
 *
 *  -> memo	Memo.
 *  -> catTo	Category.
 *
 * Return number of sentences imported.
 */
static UInt16
GetMemos(Char *memo, UInt16 catTo)
{
	UInt16 imported=0, len;
	Int32 memoLen=StrLen(memo);
	Char sentence[SentenceLength];
	Char *m;

	while (memoLen>0) {
		for (m=memo; ((memoLen>0) && (*memo!='\n')); memo++)
			memoLen--;

		len=(memo-m)>(SentenceLength-1) ? (SentenceLength-1) : memo-m;
		MemMove(sentence, m, len);
		sentence[len]='\x00';
		m=StringPurify(sentence);
		if (m) {
			if ((len=StrLen(m))>0) {
				DBSetRecord(&dbTSentence, dmMaxRecordIndex, catTo, m, len+1, SFString);
				imported++;
			}

			MemPtrFree(m);
		}

		if (memoLen>0) {
			memo++;
			memoLen--;
		}
	}

	return imported;
}

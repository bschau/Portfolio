/*
 * fTemplateImport.c
 */
#include "Include.h"

/* protos */
static Boolean EH(EventType *);

/*
 * fTemplateImportRun
 */
void
fTemplateImportRun(void)
{
	UInt16 imported=0, memoIdx=0;
	FormSave frm;
	Char buffer[16];
	MemHandle memoH;
	Char *memo;

	DBClear(&dbMemo);
	if (DBOpen(&dbMemo, dmModeReadWrite, true)==false)
		return;

	UIFormPrologue(&frm, fTemplateImport, EH);

	if (prefs.category==dmAllCategories)
		catTo=dmUnfiledCategory;
	else
		catTo=prefs.category;

	catFrom=prefs.catExchangeIdx;
	UIPopupSet(&dbTemplate, catTo, catName2, cTemplateImportPopupTo);
	UIPopupSet(&dbMemo, catFrom, catName3, cTemplateImportPopupFrom);

	if (prefs.exchangeFlags&ExchgIDelete)
		CtlSetValue(UIObjectGet(cTemplateImportDelete), 1);

	if (UIFormEpilogue(&frm, NULL, cTemplateImportCancel)==false) {
		DBClose(&dbMemo);
		return;
	}

	prefs.catExchangeIdx=catFrom;
	memoH=MemHandleNew(MemoSize);
	ErrFatalDisplayIf(memoH==NULL, "(fExportTemplateRun) Out of memory.");
	memo=MemHandleLock(memoH);

	for (EVER) {
		MemHandle mh;

		if ((mh=DmQueryNextInCategory(dbMemo.db, &memoIdx, catFrom))) {
			UInt16 len;

			len=MemHandleSize(mh);
			len=(len>MemoSize) ? MemoSize : len;
			MemMove(memo, MemHandleLock(mh), len);
			MemHandleUnlock(mh);
			memo[len]='\x00';
			DBSetRecord(&dbTemplate, dmMaxRecordIndex, catTo, memo, len+1, SFString);
			imported++;

			if (prefs.exchangeFlags&ExchgIDelete)
				DmDeleteRecord(dbMemo.db, memoIdx);
			else
				memoIdx++;
		} else
			break;
	}

	MemHandleFree(memoH);
	DBClose(&dbMemo);
	StrPrintF(buffer, "%d", imported);
	FrmCustomAlert(aTemplateImport, buffer, (imported==1) ? " " : "s ", NULL);
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
		case cTemplateImportPopupFrom:
			CategorySelect(dbMemo.db, currentForm, cTemplateImportPopupFrom, cTemplateImportListFrom, true, &catFrom, catName3, 0, categoryHideEditCategory);
			return true;

		case cTemplateImportPopupTo:
			CategorySelect(dbTemplate.db, currentForm, cTemplateImportPopupTo, cTemplateImportListTo, false, &catTo, catName2, 1, categoryHideEditCategory);
			return true;

		case cTemplateImportImport:
			if ((CtlGetValue(UIObjectGet(cTemplateImportDelete))))
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

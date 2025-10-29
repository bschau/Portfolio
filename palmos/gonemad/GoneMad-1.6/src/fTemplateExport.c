/*
 * fTemplateExport.c
 */
#include "Include.h"

/* protos */
static Boolean EH(EventType *);

/*
 * fTemplateExportRun
 */
void
fTemplateExportRun(void)
{
	UInt16 exported=0, sIdx=0;
	FormSave frm;
	Char buffer[16];

	DBClear(&dbMemo);
	if (DBOpen(&dbMemo, dmModeReadWrite, true)==false)
		return;

	UIFormPrologue(&frm, fTemplateExport, EH);

	if (prefs.catExchangeIdx==dmAllCategories)
		catTo=dmUnfiledCategory;
	else 
		catTo=prefs.catExchangeIdx;

	catFrom=prefs.category;
	UIPopupSet(&dbMemo, catTo, catName2, cTemplateExportPopupTo);
	UIPopupSet(&dbTemplate, catFrom, catName3, cTemplateExportPopupFrom);

	if (prefs.exchangeFlags&ExchgEDelete)
		CtlSetValue(UIObjectGet(cTemplateExportDelete), 1);

	if (UIFormEpilogue(&frm, NULL, cTemplateExportCancel)==false) {
		DBClose(&dbMemo);
		return;
	}

	prefs.catExchangeIdx=catTo;

	for (EVER) {
		MemHandle mh;
		Char *m;

		if ((mh=DmQueryNextInCategory(dbTemplate.db, &sIdx, catFrom))) {
			m=MemHandleLock(mh);
			DBSetRecord(&dbMemo, dmMaxRecordIndex, catTo, m, StrLen(m)+1, NULL);
			MemHandleUnlock(mh);

			exported++;
			if (prefs.exchangeFlags&ExchgEDelete)
				DmDeleteRecord(dbTemplate.db, sIdx);
			else
				sIdx++;
		} else
			break;
	}

	DBClose(&dbMemo);

	StrPrintF(buffer, "%d", exported);
	FrmCustomAlert(aTemplateExport, buffer, (exported==1) ? " " : "s ", NULL);
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
		case cTemplateExportPopupFrom:
			CategorySelect(dbTemplate.db, currentForm, cTemplateExportPopupFrom, cTemplateExportListFrom, true, &catFrom, catName3, 1, categoryHideEditCategory);
			return true;

		case cTemplateExportPopupTo:
			CategorySelect(dbMemo.db, currentForm, cTemplateExportPopupTo, cTemplateExportListTo, false, &catTo, catName2, 0, categoryHideEditCategory);
			return true;

		case cTemplateExportExport:
			if ((CtlGetValue(UIObjectGet(cTemplateExportDelete))))
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

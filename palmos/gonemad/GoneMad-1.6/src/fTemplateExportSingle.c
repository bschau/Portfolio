/*
 * fTemplateExportSingle.c
 */
#include "Include.h"

/* protos */
static Boolean EH(EventType *);

/*
 * fTemplateExportSingleRun
 *
 * Export a single template to Memo Pad.
 *
 *  -> template		The template text.
 */
void
fTemplateExportSingleRun(Char *template)
{
	FormSave frm;

	DBClear(&dbMemo);
	if (DBOpen(&dbMemo, dmModeReadWrite, true)==false)
		return;

	UIFormPrologue(&frm, fTemplateExportSingle, EH);

	if (prefs.catExchangeIdx==dmAllCategories)
		catTo=dmUnfiledCategory;
	else
		catTo=prefs.catExchangeIdx;

	UIPopupSet(&dbMemo, catTo, catName2, cTemplateExportSinglePopupTo);

	if (UIFormEpilogue(&frm, NULL, cTemplateExportSingleCancel)==true) {
		prefs.catExchangeIdx=catTo;

		DBSetRecord(&dbMemo, dmMaxRecordIndex, catTo, template, StrLen(template)+1, NULL);
		FrmAlert(aTemplateExportSingle);
	}

	DBClose(&dbMemo);
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
		case cTemplateExportSinglePopupTo:
			CategorySelect(dbMemo.db, currentForm, cTemplateExportSinglePopupTo, cTemplateExportSingleListTo, false, &catTo, catName2, 0, categoryHideEditCategory);
			return true;
		}

	default:	/* FALL-THRU */
		break;
	}

	return false;
}

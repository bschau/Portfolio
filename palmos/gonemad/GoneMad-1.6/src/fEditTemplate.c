/*
 * fMain.c
 */
#include "Include.h"

/* protos */
static void SaveData(void);

/* globals */
static UInt16 catSave;

/*
 * fEditTemplateRun
 *
 * Setup template for edit.
 *
 *  -> rIdx		Record Index.
 */
void
fEditTemplateRun(UInt16 rIdx)
{
	recordIdx=rIdx;
	FrmGotoForm(fEditTemplate);
}

/*
 * fEditTemplateInit
 */
Boolean
fEditTemplateInit(void)
{
	catSave=prefs.category;
	if (prefs.category==dmAllCategories)
		prefs.category=dmUnfiledCategory;

	UIPopupSet(&dbTemplate, prefs.category, catName2, cEditTemplatePopup);

	if (recordIdx!=dmMaxRecordIndex) {
		MemHandle mh;

		mh=DBGetRecord(&dbTemplate, recordIdx);
		UIFieldSetText(cEditTemplateField, MemHandleLock(mh));
		MemHandleUnlock(mh);
		UIObjectShow(cEditTemplateDelete);
	}

	UIFieldFocus(cEditTemplateField);
	UIFieldUpdateScrollBar(cEditTemplateField, cEditTemplateScrollBar);
	return true;
}

/*
 * fEditTemplateEH
 */
Boolean
fEditTemplateEH(EventType *ev)
{
	Char *p;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case frmSaveEvent:
		SaveData();
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cEditTemplateOK:
			if ((p=UIFieldGetText(cEditTemplateField)) && StrLen(p)>0) {
				DBSetRecord(&dbTemplate, recordIdx, prefs.category, p, StrLen(p)+1, SFString);
				FrmGotoForm(fMain);
			}
			return true;

		case cEditTemplateCancel:
			FrmGotoForm(fMain);
			return true;

		case cEditTemplateDelete:
			if (FrmAlert(aTemplateDelete)==0) {
				DmDeleteRecord(dbTemplate.db, recordIdx);
				FrmGotoForm(fMain);
			}
			return true;

		case cEditTemplatePopup:
			CategorySelect(dbTemplate.db, currentForm, cEditTemplatePopup, cEditTemplateList, false, &prefs.category, catName2, 1, 0);
			return true;
		}
		break;

	case menuEvent:
		if (ev->data.menu.itemID==mEditTemplateExport) {
			if (((p=UIFieldGetText(cEditTemplateField))!=NULL) && (StrLen(p)>0))
				fTemplateExportSingleRun(p);
			else
				FrmAlert(aNoTemplateToExport);

			return true;
		} else if (ev->data.menu.itemID==mEditTemplateBeam) {
			if (((p=UIFieldGetText(cEditTemplateField))!=NULL) && (StrLen(p)>0))
				IRBeam(p);
			else
				FrmAlert(aNothingToBeam);

			return true;
		}
		break;

	case keyDownEvent:
		UIFieldScrollBarKeyHandler(ev, cEditTemplateField, cEditTemplateScrollBar);
	default:	/* FALL-THRU */
		UIFieldScrollBarHandler(ev, cEditTemplateField, cEditTemplateScrollBar);
		break;
	}

	return false;
}

/*
 * SaveData
 *
 * Save any field data.
 */
static void
SaveData(void)
{
	Char *p=UIFieldGetText(cEditTemplateField);
	UInt16 len;
	UInt32 u;

	if (appStopped) {
		if (p==NULL || ((len=StrLen(p))==0))
			return;

		u=DBSetRecord(&dbTemplate, recordIdx, prefs.category, p, len+1, SFString);
		UIFieldSetText(cEditTemplateField, "");
		PMSetPref(&u, sizeof(UInt32), PrfTemplate);
	}
}

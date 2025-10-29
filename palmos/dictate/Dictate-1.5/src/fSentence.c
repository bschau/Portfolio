/*
 * fSentence.c
 */
#include "Include.h"

/* protos */
static Boolean SaveData(UInt32 *);
static void DeleteSentence(UInt16);

/* globals */
static UInt16 recordIdx, cancelIdx;

/*
 * fSentenceDrawTable
 *
 * Draw a sentence table.
 *
 *  -> tbl		Table.
 *  -> row		Current row.
 *  -> column		Current column.
 *  -> r		Rectangle.
 */
void
fSentenceDrawTable(void *tbl, Int16 row, Int16 column, RectangleType *r)
{
	UInt16 recIdx=0, pW, len;
	MemHandle mh;
	Char *p;
	Boolean trunc;

	if (DmSeekRecordInCategory(dbTSentence.db, &recIdx, tblSentence.top+row, dmSeekForward, prefs.catSentenceIdx)!=errNone)
		return;

	TblSetRowData(tbl, row, (UInt32)recIdx);
	WinEraseRectangle(r, 0);

	WinDrawBitmap(bmpBullet, r->topLeft.x, r->topLeft.y+3);

	mh=DBGetRecord(&dbTSentence, recIdx);
	p=MemHandleLock(mh);
	pW=r->extent.x-10;
	len=StrLen(p);
	FntCharsInWidth(p, &pW, &len, &trunc);
	WinDrawChars(p, len, r->topLeft.x+10, r->topLeft.y);
	MemHandleUnlock(mh);
}

/*
 * fSentenceInit
 */
Boolean
fSentenceInit(void)
{
	UIPopupSet(&dbTSentence, prefs.catSentenceIdx, catSentenceName, cSentencePopup);

	UITableInit(&dbTSentence, &tblSentence, cSentenceTable, cSentenceScrollBar, fSentenceDrawTable, tsCustom1);
	return true;
}

/*
 * fSentenceEH
 */
Boolean
fSentenceEH(EventType *ev)
{
	Boolean edited;
	UInt16 category;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cSentenceNew:
			recordIdx=dmMaxRecordIndex;
			FrmGotoForm(fEditSentence);
			return true;
		case cSentenceGroups:
			FrmGotoForm(fTeacher);
			return true;
		case cSentencePopup:
			category=prefs.catSentenceIdx;
			edited=CategorySelect(dbTSentence.db, currentForm, cSentencePopup, cSentenceList, true, &prefs.catSentenceIdx, catSentenceName, 1, 0);
			if (edited || (category!=prefs.catSentenceIdx)) {
				tblSentence.records=DmNumRecordsInCategory(dbTSentence.db, prefs.catSentenceIdx);
				UITableUpdateValues(&tblSentence, true);
			}

			return true;
		}
		break;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mSentenceImport:
			fSentenceImportRun();
			tblSentence.records=DmNumRecordsInCategory(dbTSentence.db, prefs.catSentenceIdx);
			UITableUpdateValues(&tblSentence, true);
			return true;

		case mSentenceExport:
			fSentenceExportRun();
			tblSentence.records=DmNumRecordsInCategory(dbTSentence.db, prefs.catSentenceIdx);
			UITableUpdateValues(&tblSentence, true);
			return true;
		}
		break;

	case tblSelectEvent:
		recordIdx=(UInt16)TblGetRowData(tblSentence.tbl, ev->data.tblSelect.row);
		FrmGotoForm(fEditSentence);
		return true;

	default:
		UITableEvents(&tblSentence, ev);
		break;
	}

	return false;
}

/*
 * fSentenceEditFromPrefs
 *
 * Possible open up the fEditSentence form from preferences.
 */
void
fSentenceEditFromPrefs(void)
{
	UInt32 uid;

	if (PMGetPref(&uid, sizeof(UInt32), PrfEdit)==true) {
		if (DmFindRecordByID(dbTSentence.db, uid, &recordIdx)==errNone) {
			FrmGotoForm(fEditSentence);
			return;
		}
	}

	FrmGotoForm(fTeacher);
}

/*
 * fEditSentenceInit
 */
Boolean
fEditSentenceInit(void)
{
	MemHandle mh;

	cancelIdx=prefs.catSentenceIdx;
	if (prefs.catSentenceIdx==dmAllCategories)
		prefs.catSentenceIdx=dmUnfiledCategory;

	if (recordIdx!=dmMaxRecordIndex) {
		UInt16 attrs;

		mh=DBGetRecord(&dbTSentence, recordIdx);
		UIFieldSetText(cEditSentenceField, MemHandleLock(mh));
		MemHandleUnlock(mh);
		UIObjectShow(cEditSentenceDelete);
		DmRecordInfo(dbTSentence.db, recordIdx, &attrs, NULL, NULL);
		prefs.catSentenceIdx=attrs&dmRecAttrCategoryMask;
	}

	UIPopupSet(&dbTSentence, prefs.catSentenceIdx, catSentenceName, cEditSentencePopup);
	UIFieldFocus(cEditSentenceField);
	UIFieldUpdateScrollBar(cEditSentenceField, cEditSentenceScrollBar);
	return true;
}

/*
 * EditSentenceEH
 */
Boolean
fEditSentenceEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		break;

	case frmSaveEvent:
		if (appStopped) {
			UInt32 uid;

			if (SaveData(&uid)==true)
				PMSetPref(&uid, sizeof(UInt32), PrfEdit);
		}
		break;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cEditSentencePopup:
			CategorySelect(dbTSentence.db, currentForm, cEditSentencePopup, cEditSentenceList, false, &prefs.catSentenceIdx, catSentenceName, 1, 0);
			return true;
		case cEditSentenceOK:
			SaveData(NULL);
			FrmGotoForm(fSentence);
			return true;
		case cEditSentenceCancel:
			UIFieldSetText(cEditSentenceField, "");
			PMSetPref(NULL, 0, PrfEdit);
			prefs.catSentenceIdx=cancelIdx;
			FrmGotoForm(fSentence);
			return true;
		case cEditSentenceDelete:
			if (FrmAlert(aDeleteSentence)==0) {
				DeleteSentence(recordIdx);
				FrmGotoForm(fSentence);
			}

			return true;
		}
		break;

	case keyDownEvent:
		UIFieldScrollBarKeyHandler(ev, cEditSentenceField, cEditSentenceScrollBar);
	default:	/* FALL-THRU */
		UIFieldScrollBarHandler(ev, cEditSentenceField, cEditSentenceScrollBar);
		break;
	}

	return false;
}

/*
 * SaveData
 *
 * Save data from text field.
 *
 * <-  uid	Where to store record UID.
 *
 * Returns true if data was saved, false otherwise.
 */
static Boolean
SaveData(UInt32 *uid)
{
	Char *p=UIFieldGetText(cEditSentenceField);
	UInt16 len;
	Char buffer[SentenceLength+1];
	UInt32 u;

	if (!p)
		return false;

	if ((len=StrLen(p))<1)
		return false;

	MemMove(buffer, p, len);
	buffer[len]='\x00';

	if ((p=StringPurify(buffer))==NULL)
		return false;

	UIFieldSetText(cEditSentenceField, "");
	u=DBSetRecord(&dbTSentence, recordIdx, prefs.catSentenceIdx, p, StrLen(p)+1, SFString);
	if (uid)
		*uid=u;

	MemPtrFree(p);
	return true;
}

/*
 * DeleteSentence
 *
 * Delete a sentence from all databases.
 *
 *  -> rIdx		Record index.
 */
static void
DeleteSentence(UInt16 rIdx)
{
	UInt32 suid;

	DmRecordInfo(dbTSentence.db, rIdx, NULL, &suid, NULL);
	XRefDeleteAllSuid(&dbTXref, suid);

	DmDeleteRecord(dbTSentence.db, rIdx);
}

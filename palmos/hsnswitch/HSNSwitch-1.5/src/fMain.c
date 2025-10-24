/*
 * fMain.c
 */
#include "Include.h"

/* macros */
#define WXApplication 90
#define WXHotSyncID 60

/* protos */
static void DrawTable(void *, Int16, Int16, RectangleType *);
static void UpdateSelector(void);
static void AddApplication(void);
static Boolean AddApplicationEH(EventType *);
static void AddApplicationDrawTable(void *, Int16, Int16, RectangleType *);
static void DefaultHotSyncName(void);
static Boolean DefaultHotSyncNameEH(EventType *);

/* globals */
static Table tblMain, tblApplication;
static Int16 returnTop;
static UInt16 addApplicationTop;

/*
 * DrawTable
 *
 * Draw the main table.
 *
 *  -> tbl		Table.
 *  -> row		Current row.
 *  -> column		Current column.
 *  -> r		Rectangle.
 */
static void
DrawTable(void *tbl, Int16 row, Int16 column, RectangleType *r)
{
	UInt16 pW=WXApplication, len;
	MemHandle mh;
	HotSyncRecord *hsr;
	Boolean trunc;

	mh=DBGetRecord(&dbHSN, row+tblMain.top);
	TblSetRowData(tbl, row, row+tblMain.top);
	hsr=MemHandleLock(mh);

	len=StrLen(hsr->appName);
	FntCharsInWidth(hsr->appName, &pW, &len, &trunc);
	WinDrawChars(hsr->appName, len, r->topLeft.x, r->topLeft.y);

	len=StrLen(hsr->hotSyncName);
	pW=WXHotSyncID;
	FntCharsInWidth(hsr->hotSyncName, &pW, &len, &trunc);
	WinDrawChars(hsr->hotSyncName, len, r->topLeft.x+WXApplication+WXHotSyncID-pW, r->topLeft.y);

	MemHandleUnlock(mh);
}

/*
 * UpdateSelector
 *
 * Update the hotsync name selector trigger.
 */
static void
UpdateSelector(void)
{
	CtlSetLabel(UIObjectGet(cMainSelector), prefs.hotSyncName);
}

/*
 * fMainInit
 */
Boolean
fMainInit(void)
{
	UITableInit(&dbHSN, &tblMain, cMainTable, cMainScrollBar, DrawTable, tsCustom1);
	UITableUpdateValues(&tblMain, false);
	return true;
}

/*
 * fMainEH
 */
Boolean
fMainEH(EventType *ev)
{
	HotSyncRecord hsr;
	HotSyncRecord *h;
	MemHandle mh;
	UInt16 rIdx, ret;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		UpdateSelector();
		if (tapMenuEvent.eType) {
			EvtAddEventToQueue(&tapMenuEvent);
			tapMenuEvent.eType=0;
		}

		return true;

	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==cMainAdd) {
			AddApplication();
			tblMain.records=DmNumRecordsInCategory(dbHSN.db, dmAllCategories);
			UITableUpdateValues(&tblMain, true);
			return true;
		} else if (ev->data.ctlSelect.controlID==cMainSelector) {
			DefaultHotSyncName();
			UpdateSelector();
			return true;
		}
		break;

	case tblSelectEvent:
		returnTop=tblMain.top;
		rIdx=(UInt16)TblGetRowData(tblMain.tbl, ev->data.tblSelect.row);
		mh=DBGetRecord(&dbHSN, rIdx);
		h=(HotSyncRecord *)MemHandleLock(mh);
		StrNCopy(hsr.appName, h->appName, dmDBNameLength);
		StrNCopy(hsr.hotSyncName, h->hotSyncName, HotSyncNameLength);
		MemHandleUnlock(mh);

		fDetailsGet(&ret, &hsr);
		if (ret!=HsngCancel) {
			if (ret==HsngDelete)
				DmRemoveRecord(dbHSN.db, rIdx);
			else
				DBSetRecord(&dbHSN, rIdx, &hsr);

			tblMain.records=DmNumRecordsInCategory(dbHSN.db, dmAllCategories);
			UITableUpdateValues(&tblMain, true);
		}

		return true;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mMainAbout:
			About();
			return true;
		case mMainPurge:
			if (FrmAlert(aPurge)==0) {
				DBPurge(&dbHSN);

				tblMain.records=DmNumRecordsInCategory(dbHSN.db, dmAllCategories);
				UITableUpdateValues(&tblMain, true);
			}
			return true;
		case mMainDetect:
			fDetectRun();
			return true;
		}
		break;
	default:
		UITableEvents(&tblMain, ev);
		break;
	}

	return false;
}

/*
 * AddApplication
 *
 * Add an application/hotsync to the database.
 */
static void
AddApplication(void)
{
	FormSave frm;

	UIFormPrologue(&frm, fAddApplication, AddApplicationEH);
	UITableInit(NULL, &tblApplication, cAddApplicationTable, cAddApplicationScrollBar, AddApplicationDrawTable, tsCustom1);
	tblApplication.top=addApplicationTop;
	tblApplication.records=lstPrograms.count;
	UITableUpdateValues(&tblApplication, true);
	UIFormEpilogue(&frm, NULL, cAddApplicationCancel);
}

/*
 * AddApplicationEH
 */
static Boolean
AddApplicationEH(EventType *ev)
{
	if (ev->eType==tblSelectEvent) {
		Node *n=(Node *)TblGetRowData(tblApplication.tbl, ev->data.tblSelect.row);
		HotSyncRecord hsr;
		UInt16 ret;

		TblUnhighlightSelection(tblApplication.tbl);

		StrNCopy(hsr.appName, n->name, dmDBNameLength);
		if (DBFindRecord(&dbHSN, &hsr)==dmMaxRecordIndex) {
			hsr.hotSyncName[0]='\x00';

			fDetailsGet(&ret, &hsr);
			if (ret!=HsngCancel) {
				DBSetRecord(&dbHSN, dmMaxRecordIndex, &hsr);
				addApplicationTop=tblApplication.top;
				CtlHitControl(UIObjectGet(cAddApplicationCancel));
			}
		} else 
			FrmAlert(aAlreadyPresent);

		return true;
	} else if (ev->eType==keyDownEvent) {
		WChar w=0, c;
		Int16 row=0;
		Node *n;

		switch (ev->data.keyDown.chr) {
		case pageUpChr:
		case prevFieldChr:
		case pageDownChr:
		case nextFieldChr:
			UITableEvents(&tblApplication, ev);
			break;
		default:
			c=ev->data.keyDown.chr;
			if (c>33 && c<127) {
				c&=223;
				for (n=lstPrograms.head; n; n=n->next) {
					w=(n->name[0]&223);
					if (w<c)
						row++;
					else
						break;
				}

				if (row!=tblApplication.top)
					UITableScroll(&tblApplication, row-tblApplication.top);
			}
			break;
		}
	} else
		UITableEvents(&tblApplication, ev);

	return false;
}

/*
 * AddApplicationDrawTable
 *
 * Draw the application table.
 *
 *  -> tbl		Table.
 *  -> row		Current row.
 *  -> column		Current column.
 *  -> r		Rectangle.
 */
static void
AddApplicationDrawTable(void *tbl, Int16 row, Int16 column, RectangleType *r)
{
	Int16 tableRow=row+tblApplication.top;
	UInt16 pW, len;
	Boolean trunc;
	Node *n;

	for (n=lstPrograms.head; n->next && tableRow>0; ) {
		n=n->next;
		tableRow--;
	}

	TblSetRowData(tblApplication.tbl, row, (UInt32)n);
	pW=140;
	len=StrLen(n->name);
	FntCharsInWidth(n->name, &pW, &len, &trunc);
	WinDrawChars(n->name, len, r->topLeft.x, r->topLeft.y);
}

/*
 * DefaultHotSyncName
 *
 * Retrieve the default HotSync Name from user.
 */
static void
DefaultHotSyncName(void)
{
	FormSave frm;

	UIFormPrologue(&frm, fDefaultHotSyncName, DefaultHotSyncNameEH);
	if (StrLen(prefs.hotSyncName)==0)
		DlkGetSyncInfo(NULL, NULL, NULL, prefs.hotSyncName, NULL, NULL);

	UIFieldSetText(cDefaultHotSyncNameField, prefs.hotSyncName);
	UIFieldFocus(cDefaultHotSyncNameField);
	UIFormEpilogue(&frm, NULL, cDefaultHotSyncNameCancel);
}

/*
 * DefaultHotSyncNameEH
 */
static Boolean
DefaultHotSyncNameEH(EventType *ev)
{
	if (ev->eType==ctlSelectEvent) {
		Char buffer[HotSyncNameLength+2];
		Char *p;

		switch (ev->data.ctlSelect.controlID) {
		case cDefaultHotSyncNameOK:
			if ((p=UIFieldGetText(cDefaultHotSyncNameField)))
				StrNCopy(prefs.hotSyncName, p, HotSyncNameLength);
			else
				DlkGetSyncInfo(NULL, NULL, NULL, prefs.hotSyncName, NULL, NULL);
			break;
		case cDefaultHotSyncNameCurrent:
			DlkGetSyncInfo(NULL, NULL, NULL, buffer, NULL, NULL);
			UIFieldSetText(cDefaultHotSyncNameField, buffer);
			UIFieldFocus(cDefaultHotSyncNameField);
			return true;
		}
	}

	return false;
}

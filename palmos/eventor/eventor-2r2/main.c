/*
**	main.c
**
**	Code to drive the main form.
*/
#include "eventor.h"

#define DateLength 10
#define Spacing 1

/* protos */
static void DrawTable(void *, Int16, Int16, RectangleType *);
static Boolean MainFormSelectHandler(EventPtr);
static void FindNextEvent(void);
static void Print(void);

/* globals */
static Table tblMain;

/*
**	DrawTable
**
**	Draw the table contents. See Reference guide for arguments.
*/
static void
DrawTable(void *tbl, Int16 row, Int16 column, RectangleType *r)
{
	UInt16 item=0, pw, sLen;
	MemHandle mh;
	Boolean trunc;
	Char buffer[16];
	TEvent *e;

	if (DmSeekRecordInCategory(dbData.db, &item, tblMain.top+row, dmSeekForward, categoryIdx)==errNone) {
		mh=DBGetRecord(&dbData, item);
		TblSetRowData(tbl, row, (UInt32)item);
		WinEraseRectangle(r, 0);

		e=MemHandleLock(mh);
		UtilsGetShortDate(buffer, e->date, dateFormat, dateSeparator);
		pw=DATEFIELDWIDTH;
		sLen=StrLen(buffer);
		FntCharsInWidth(buffer, &pw, &sLen, &trunc);
		pw=DATEFIELDWIDTH-pw;
		WinDrawChars(buffer, sLen, r->topLeft.x+pw, r->topLeft.y);
		pw=r->extent.x-SPACING-DATEFIELDWIDTH;
		sLen=StrLen(e->event);
		FntCharsInWidth(e->event, &pw, &sLen, &trunc);
		WinDrawChars(e->event, sLen, r->topLeft.x+DATEFIELDWIDTH+SPACING, r->topLeft.y);
		MemHandleUnlock(mh);
	}
}

/*
**	MainFormInit
**
**	Initialize the main form.
**
**	Returns true if successful, false otherwise.
*/
Boolean
MainFormInit(void)
{
	TableStyle styles[]={ {0, customTableItem }, { -1 } };
	ControlType *ctl=UIFormGetObject(fMainCategoryPopup);

	CategoryGetName(dbData.db, categoryIdx, categoryName);
	CategorySetTriggerLabel(ctl, categoryName);

	/* initialize table */
	UITableInit(NULL, &tblMain, fMainTable, fMainScrollBar, DrawTable, styles);
	tblMain.records=DmNumRecordsInCategory(dbData.db, categoryIdx);
	UITableUpdateValues(&tblMain);

	if (PalmPrintOpen()==true)
		UIShowObject(fMainPrint);

	return true;
}

/*
**	MainFormEventHandler
**
**	Event handler for main form.
**
**	 -> ev		Event.
**
**	Returns true if event is handled, false otherwise.
*/
Boolean
MainFormEventHandler(EventPtr ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		WinDrawLine(1, 29, 159, 29);
		return true;

	case tblSelectEvent:
		viewIdx=(UInt16)TblGetRowData(UIFormGetObject(fMainTable), ev->data.tblSelect.row);
		FrmGotoForm(fView);
		return true;

	case ctlSelectEvent:
		return MainFormSelectHandler(ev);

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mMainNotifications:
			if (DmNumRecordsInCategory(dbData.db, dmAllCategories)==0)
				FrmAlert(aNoEvents);
			else
				AlarmShowEvents(false);

			return true;
		case mMainPrefs:
			PreferencesDialog();
			return true;
		case mMainAbout:
			About();
			return true;
		default:
			break;
		}
		break;

	default:
		UITableEvents(&tblMain, ev);
		break;
	}

	return false;
}

/*
**	MainFormSelectHandler
**
**	Handle select events for controls on main form.
**
**	 -> ev		Event.
**
**	Returns true if event is handled, false otherwise.
*/
static Boolean
MainFormSelectHandler(EventPtr ev)
{
	Boolean edited;
	UInt16 category;

	switch (ev->data.ctlSelect.controlID) {
	case fMainCategoryPopup:
		category=categoryIdx;
		edited=CategorySelect(dbData.db, currentForm, fMainCategoryPopup, fMainCategoryList, true, &categoryIdx, categoryName, CATNONEDITABLES, 0);
		if (edited || (category!=categoryIdx))
			UITableChanged(&tblMain, DmNumRecordsInCategory(dbData.db, categoryIdx));

		return true;

	case fMainNew:
		recordIdx=dmMaxRecordIndex;
		FrmGotoForm(fEdit);
		return true;

	case fMainNext:
		FindNextEvent();
		return true;

	case fMainPrint:
		Print();
		return true;
	}

	return false;
}

/*
**	FindNextEvent
**
**	Find (and view) the next event closest to this date.
*/
static void
FindNextEvent(void)
{
	UInt16 recIdx=0, nowDate, date;
	Int16 allRecs;
	MemHandle mh;
	DateTimeType dtt;

	if ((allRecs=DmNumRecordsInCategory(dbData.db, dmAllCategories))==0) {
		FrmAlert(aNoEvents);
		return;
	}

	TimSecondsToDateTime(TimGetSeconds(), &dtt);
	nowDate=UtilsPackDate(dtt.year, dtt.month, dtt.day)&MaskOnlyDate;
	viewIdx=0;
	for (; allRecs>0; allRecs--) {
		if ((mh=DmQueryNextInCategory(dbData.db, &recIdx, dmAllCategories))==NULL)
			break;

		date=((TEvent *)MemHandleLock(mh))->date;
		MemHandleUnlock(mh);

		date&=MaskOnlyDate;
		if (date>=nowDate) {
			viewIdx=recIdx;
			break;
		}

		recIdx++;
	}

	FrmGotoForm(fView);
}

/*
**	Print
**
**	Send Event list to Palm Print.
*/
static void
Print(void)
{
	Char line[DateLength+Spacing+EventLength+Spacing+dmCategoryLength+1];
	Char date[DateLength+1], catName[dmCategoryLength];
	UInt16 item=0, len, attrs;
	Int16 y, m, d;
	MemHandle mh;
	Char *p;
	TEvent *e;

	if (DmNumRecordsInCategory(dbData.db, categoryIdx)==0) {
		FrmAlert(aNothingToPrint);
		return;
	}

	PalmPrintLineModeStart();

	// Do header
	MemSet(line, sizeof(line)-1, ' ');
	line[sizeof(line)-1]='\x00';

	mh=UtilsLockString(strDate);
	p=MemHandleLock(mh);
	len=StrLen(p);
	MemMove(line+DateLength-len, p, len);
	UtilsUnlockString(mh);

	mh=UtilsLockString(strEvent);
	p=MemHandleLock(mh);
	len=StrLen(p);
	MemMove(line+DateLength+Spacing, p, len);
	UtilsUnlockString(mh);

	mh=UtilsLockString(strCategory);
	p=MemHandleLock(mh);
	len=StrLen(p);
	MemMove(line+DateLength+Spacing+EventLength+Spacing, p, len);
	UtilsUnlockString(mh);

	PalmPrintLineModePrint(line);

	MemSet(line, sizeof(line)-1, '-');
	line[sizeof(line)-1]='\x00';
	PalmPrintLineModePrint(line);

	while (true) {
		if (DmSeekRecordInCategory(dbData.db, &item, 0, dmSeekForward, categoryIdx)!=errNone)
			break;

		mh=DBGetRecord(&dbData, item);
		e=MemHandleLock(mh);

		MemSet(line, sizeof(line)-1, ' ');
		line[sizeof(line)-1]='\x00';
		UtilsUnpackDate(e->date, &y, &m, &d);
		UtilsFormatDate(date, y+MAGICYEAR, m, d);
		len=StrLen(date);
		MemMove(line+DateLength-len, date, len);
		len=StrLen(e->event);
		MemMove(line+DateLength+Spacing, e->event, len);
		MemHandleUnlock(mh);
		
		DmRecordInfo(dbData.db, item, &attrs, NULL, NULL);
		CategoryGetName(dbData.db, attrs&dmRecAttrCategoryMask, catName);
		len=StrLen(catName);
		MemMove(line+DateLength+Spacing+EventLength+Spacing, catName, len);
		PalmPrintLineModePrint(line);
		item++;
	}

	PalmPrintLineModeStop();
}

/*
 * fMain.c
 */
#include "Include.h"

/* macros */
#define DefaultWidth 42

/* protos */
static void DrawTable(void *, Int16, Int16, RectangleType *);
static void EntryOut(Record *, UInt16, UInt32, Boolean, UInt32, UInt16, UInt16, UInt16);
static void DrawHeader(void);
static void StringOut(UInt16, UInt16);

/* globals */
static Int16 filter;
static Table tblMain;

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
	UInt16 recIdx=0, pW, len, month, day;
	UInt32 oldValue=0;
	Boolean useOld=false, trunc;
	Char buffer[32];
	Record *record;
	MemHandle mh;

	if (DmSeekRecordInCategory(dbData.db, &recIdx, tblMain.top+row, dmSeekForward, dmUnfiledCategory)!=errNone)
		return;

	TblSetRowData(tbl, row, (UInt32)recIdx);
	WinEraseRectangle(r, 0);

	mh=DBGetRecord(&dbData, recIdx);
	record=MemHandleLock(mh);

	month=((record->date)>>5)&0x0f;
	day=(record->date)&0x1f;

	switch (PrefGetPreference(prefDateFormat)) {
	case dfMDYWithSlashes:
		StrPrintF(buffer, "%d-%d", month, day);
		break;
	case dfDMYWithSlashes:
		StrPrintF(buffer, "%d/%d", day, month);
		break;
	case dfDMYWithDots:
		StrPrintF(buffer, "%d.%d", day, month);
		break;
	case dfDMYWithDashes:
		StrPrintF(buffer, "%d-%d", day, month);
		break;
	case dfYMDWithDots:
		StrPrintF(buffer, "%d.%d", month, day);
		break;
	case dfYMDWithDashes:
		StrPrintF(buffer, "%d-%d", month, day);
		break;
	case dfMDYWithDashes:
		StrPrintF(buffer, "%d-%d", month, day);
		break;
	case dfYMDWithSlashes:
	default:	/* Fall-Thru */
		StrPrintF(buffer, "%d/%d", month, day);
		break;
	}

	pW=24;
	r->extent.x=pW;
	r->topLeft.x+=pW;
	len=StrLen(buffer);
	FntCharsInWidth(buffer, &pW, &len, &trunc);
	WinDrawChars(buffer, len, r->topLeft.x-pW, r->topLeft.y);

	if (filter) {
		UInt16 rCnt=DmNumRecordsInCategory(dbData.db, dmAllCategories);

		switch (filter) {
		case 1:
			if (!(record->flags&RFlgNewEMeter))
				useOld=true;
			break;
		case 2:
			if (!(record->flags&RFlgNewGMeter))
				useOld=true;
			break;
		case 3:
			if (!(record->flags&RFlgNewWMeter))
				useOld=true;
			break;
		}

		if ((rCnt<=1) || (tblMain.top+row>=(rCnt-1)))
			useOld=false;

		if (useOld) {
			UInt16 oIdx=0;

			if (DmSeekRecordInCategory(dbData.db, &oIdx, tblMain.top+row+1, dmSeekForward, dmUnfiledCategory)==errNone) {
				MemHandle mh;
				Record *rec;

				mh=DBGetRecord(&dbData, oIdx);
				rec=MemHandleLock(mh);
				switch (filter) {
				case 1:
					oldValue=rec->el;
					break;
				case 2:
					oldValue=rec->gas;
					break;
				case 3:
					oldValue=rec->water;
					break;
				}
	
				MemHandleUnlock(mh);
			}
		}
	}

	switch (filter) {
	case 1:
		EntryOut(record, RFlgNewEMeter, record->el, useOld, oldValue, 90, r->topLeft.y, 76);
		break;
	case 2:
		EntryOut(record, RFlgNewGMeter, record->gas, useOld, oldValue, 90, r->topLeft.y, 76);
		break;
	case 3:
		EntryOut(record, RFlgNewWMeter, record->water, useOld, oldValue, 90, r->topLeft.y, 76);
		break;
	default:
		r->extent.x+=DefaultWidth+2;
		EntryOut(record, RFlgNewEMeter, record->el, false, 0, r->extent.x, r->topLeft.y, DefaultWidth);
		r->extent.x+=DefaultWidth;
		EntryOut(record, RFlgNewGMeter, record->gas, false, 0, r->extent.x, r->topLeft.y, DefaultWidth);
		r->extent.x+=DefaultWidth;
		EntryOut(record, RFlgNewWMeter, record->water, false, 0, r->extent.x, r->topLeft.y, DefaultWidth);
		break;
	}

	MemHandleUnlock(mh);
}

/*
 * EntryOut
 *
 * Writeout an entry of the table.
 *
 *  -> record		Record.
 *  -> flag		Flag to test.
 *  -> value		Value to write.
 *  -> useOld		Use "old" (=previous) value.
 *  -> oldValue		Old value.
 *  -> xPos		X Position.
 *  -> yPos		Y Position.
 *  -> width		Width of field.
 */
static void
EntryOut(Record *record, UInt16 flag, UInt32 value, Boolean useOld, UInt32 oldValue, UInt16 xPos, UInt16 yPos, UInt16 width)
{
	UInt16 len;
	Char buffer[16];
	Boolean fitAll;

	if ((record->flags&flag)==flag)
		FntSetFont(boldFont);

	StrPrintF(buffer, "%lu", value);
	len=StrLen(buffer);
	FntCharsInWidth(buffer, &width, &len, &fitAll);
	WinDrawChars(buffer, len, xPos-width, yPos);

	if ((record->flags&flag)==flag)
		FntSetFont(stdFont);

	if (useOld) {
		StrPrintF(buffer, "%ld", value-oldValue);
		len=StrLen(buffer);
		width=60;
		FntCharsInWidth(buffer, &width, &len, &fitAll);
		WinDrawChars(buffer, len, 150-width, yPos);
	}
}

/*
 * fMainInit
 */
Boolean
fMainInit(void)
{
	TableStyle style[]={ { 0, customTableItem }, { -1 } };
	ListType *l=UIObjectGet(cMainList);

	LstSetSelection(l, filter);
	CtlSetLabel(UIObjectGet(cMainPopup), LstGetSelectionText(l, filter));

	UITableInit(&dbData, &tblMain, cMainTable, cMainScrollBar, DrawTable, style, 0);
	tblMain.records=DmNumRecordsInCategory(dbData.db, dmAllCategories);
	UITableUpdateValues(&tblMain, false);
	return true;
}

/*
 * fMainEH
 */
Boolean
fMainEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		DrawHeader();
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cMainNew:
			if (fEditRun(dmMaxRecordIndex)==true) {
				tblMain.records=DmNumRecordsInCategory(dbData.db, dmAllCategories);
				UITableUpdateValues(&tblMain, true);
			}
			return true;
		case cMainStatistics:
			if (tblMain.records<2)
				FrmAlert(aNoStatistics);
			else
				fStatisticsRun(filter);
			return true;
		}
		break;

	case popSelectEvent:
		if (ev->data.popSelect.listID==cMainList) {
			if ((ev->data.popSelect.selection!=ev->data.popSelect.priorSelection) && (ev->data.popSelect.selection!=noListSelection)) {
				filter=ev->data.popSelect.selection;
				DrawHeader();
				TblDrawTable(UIObjectGet(cMainTable));
			}
		}
		break;

	case tblSelectEvent:
		if (fEditRun((UInt16)TblGetRowData(tblMain.tbl, ev->data.tblSelect.row))==true) {
			tblMain.records=DmNumRecordsInCategory(dbData.db, dmAllCategories);
			UITableUpdateValues(&tblMain, true);
		}
		TblUnhighlightSelection(tblMain.tbl);
		return true;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mMainAbout:
			fAboutRun();
			FrmUpdateForm(fMain, frmRedrawUpdateCode);
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
 * DrawHeader
 *
 * Print the header based on the selection of the filter.
 */
static void
DrawHeader(void)
{
	FontID fID=FntSetFont(boldFont);
	RectangleType r;

	RctSetRectangle(&r, 26, 18, 134, FntCharHeight());
	WinEraseRectangle(&r, 0);
	
	switch (filter) {
	case 1:
		StringOut(strEl, 90);
		StringOut(strDiff, 150);
		break;
	case 2:
		StringOut(strGas, 90);
		StringOut(strDiff, 150);
		break;
	case 3:
		StringOut(strWater, 90);
		StringOut(strDiff, 150);
		break;
	default:
		StringOut(strEl, 68);
		StringOut(strGas, 110);
		StringOut(strWater, 152);
		break;
	}

	FntSetFont(fID);
}

/*
 * StringOut
 *
 * Write a header string right aligned to rightX.
 *
 *  -> id		String id.
 *  -> rightX		Right pixel.
 */
static void
StringOut(UInt16 id, UInt16 rightX)
{
	MemHandle mh=DmGetResource(strRsc, id);
	UInt16 pW=DefaultWidth, len;
	Char *str;
	Boolean fitAll;

	str=MemHandleLock(mh);
	len=StrLen(str);

	FntCharsInWidth(str, &pW, &len, &fitAll);
	WinDrawChars(str, len, rightX-pW, 18);

	MemHandleUnlock(mh);
	DmReleaseResource(mh);
}

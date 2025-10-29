/*
 * fHistory.c
 */
#include "Include.h"

/* protos */
static void DrawTable(void *, Int16, Int16, RectangleType *);
static Boolean Details(UInt16);
static Boolean DEH(EventType *);

/* globals */
static UInt16 category=dmAllCategories, catDetails;
static DateTimeType today;
static Table tblHistory;
static Boolean dChanged;

/*
 * DrawTable
 */
static void
DrawTable(void *tbl, Int16 row, Int16 column, RectangleType *r)
{
	UInt16 rIdx=0;

	if (DmSeekRecordInCategory(dbHistory.db, &rIdx, tblHistory.top+row, dmSeekForward, category)==errNone) {
		UInt16 pW=100, bold=boldFont, len;
		Char bufDate[dateStringLength+1];
		Boolean trunc;
		MemHandle mh;
		History *h;
		DateTimeType dt;
		FontID fID;

		TblSetRowData(tbl, row, (UInt32)rIdx);
		WinEraseRectangle(r, 0);

		if (category!=dmAllCategories)
			bold=stdFont;

		mh=DmQueryRecord(dbHistory.db, rIdx);
		ErrFatalDisplayIf(mh==NULL, "(DrawTable) No such record");

		DmRecordInfo(dbHistory.db, rIdx, &len, NULL, NULL);
		if (len&HCatRead)
			fID=FntSetFont(bold);
		else
			fID=FntSetFont(stdFont);

		h=MemHandleLock(mh);

		len=StrLen(h->docName);
		FntCharsInWidth(h->docName, &pW, &len, &trunc);
		WinDrawChars(h->docName, len, r->topLeft.x, r->topLeft.y);

		TimSecondsToDateTime(h->last, &dt);
		if ((dt.day==today.day) && (dt.month==today.month) && (dt.year==today.year))
			TimeToAscii(dt.hour, dt.minute, PrefGetPreference(prefTimeFormat), bufDate);
		else
			DateToAscii(dt.month, dt.day, dt.year, PrefGetPreference(prefDateFormat), bufDate);

		len=StrLen(bufDate);
		pW=FntCharsWidth(bufDate, len);
		WinDrawChars(bufDate, len, r->topLeft.x+149-pW, r->topLeft.y);
		MemHandleUnlock(mh);

		FntSetFont(fID);
	}
}

/*
 * fHistoryInit
 */
Boolean
fHistoryInit(void)
{
	UITableInit(&tblHistory, cHistoryTable, cHistoryScrollBar, DrawTable, style1, DmNumRecordsInCategory(dbHistory.db, category));
	UITableUpdateValues(&tblHistory, false);

	UIPopupSet(dbHistory.db, category, catName1, cHistoryPopup);
	TimSecondsToDateTime(TimGetSeconds(), &today);
	return true;
}

/*
 * fHistoryEH
 */
Boolean
fHistoryEH(EventType *ev)
{
	UInt16 cat;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		WinDrawLine(0, 27, 160, 27);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cHistoryDone:
			FrmGotoForm(fMain);
			return true;

		case cHistoryClear:
			if (FrmAlert(aHistoryClear)==0) {
				HistoryClear(category);
				tblHistory.records=0;
				UITableUpdateValues(&tblHistory, true);
			}
			return true;

		case cHistoryPopup:
			cat=category;
			CategorySelect(dbHistory.db, currentForm, cHistoryPopup, cHistoryList, true, &category, catName1, 2, categoryHideEditCategory);
			if (cat!=category) {
				tblHistory.records=DmNumRecordsInCategory(dbHistory.db, category);
				UITableUpdateValues(&tblHistory, true);
			}
			return true;
		}
		break;

	case tblSelectEvent:
		if (Details((UInt16)TblGetRowData(tblHistory.tbl, ev->data.tblSelect.row))==true) {
			tblHistory.records=DmNumRecordsInCategory(dbHistory.db, category);
			UITableUpdateValues(&tblHistory, true);
		}
		return true;
		
	default:
		UITableEvents(&tblHistory, ev);
		break;
	}

	return false;
}

/*
 * Details
 *
 * View details and perhaps delete entry.
 *
 *  -> rIdx		Record Index.
 *
 * Returns true if entry was deleted, false otherwise.
 */
static Boolean
Details(UInt16 rIdx)
{
	Char buf[longDateStrLength+timeStringLength+4];
	MemHandle mh;
	FormSave frm;
	History *h;
	DateTimeType dt;
	UInt16 attr;

	dChanged=false;
	recordIdx=rIdx;

	DmRecordInfo(dbHistory.db, rIdx, &attr, NULL, NULL);
	catDetails=attr&HCatRead;

	UIFormPrologue(&frm, fHistoryDetails, DEH);

	UIPopupSet(dbHistory.db, catDetails, catName2, cHistoryDetailsPopup);

	mh=DBGetRecord(&dbHistory, rIdx);
	h=MemHandleLock(mh);

	UIFieldSetText(cHistoryDetailsName, h->docName);

	TimSecondsToDateTime(h->opened, &dt);
	DateToAscii(dt.month, dt.day, dt.year, PrefGetPreference(prefLongDateFormat), buf);
	StrCat(buf, "   ");
	TimeToAscii(dt.hour, dt.minute, PrefGetPreference(prefTimeFormat), buf+StrLen(buf));
	UIFieldSetText(cHistoryDetailsOpened, buf);

	TimSecondsToDateTime(h->last, &dt);
	DateToAscii(dt.month, dt.day, dt.year, PrefGetPreference(prefLongDateFormat), buf);
	StrCat(buf, "   ");
	TimeToAscii(dt.hour, dt.minute, PrefGetPreference(prefTimeFormat), buf+StrLen(buf));
	UIFieldSetText(cHistoryDetailsLast, buf);

	MemHandleFree(mh);

	if (UIFormEpilogue(&frm, NULL, cHistoryDetailsCancel)==false)
		return false;

	return dChanged;
}

/*
 * DEH
 */
static Boolean
DEH(EventType *ev)
{
	if (ev->eType==ctlSelectEvent) {
		UInt16 attr;

		switch (ev->data.ctlSelect.controlID) {
		case cHistoryDetailsOK:
			DmRecordInfo(dbHistory.db, recordIdx, &attr, NULL, NULL);
			if ((attr&HCatRead)!=catDetails) {
				attr&=~dmRecAttrCategoryMask;
				attr|=catDetails;
				DmSetRecordInfo(dbHistory.db, recordIdx, &attr, NULL);
			}
			break;
			
		case cHistoryDetailsDelete:
			if (FrmAlert(aDeleteDetail)==0) {
				DmDeleteRecord(dbHistory.db, recordIdx);
				dChanged=true;
			}
			break;

		case cHistoryDetailsPopup:
			CategorySelect(dbHistory.db, currentForm, cHistoryDetailsPopup, cHistoryDetailsList, false, &catDetails, catName2, 2, categoryHideEditCategory);
			return true;
		}
	}

	return false;
}

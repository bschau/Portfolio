/*
**	mainform.c
**
**	Code to drive the main form.
*/
#include "bmilog.h"

/* protos */
static void DrawTable(void *, Int16, Int16, RectangleType *);
static void Edit(UInt16);
static UInt16 PersonCount(DB *);

/* globals */
static Table tblMain;

/*
**	DrawTable
**
**	Draw main table.
*/
static void
DrawTable(void *tbl, Int16 row, Int16 column, RectangleType *r)
{
	DmOpenRef db=dbData.db;
	UInt16 item=0;
	UInt8 type;
	FontID fid;
	MemHandle pmh, bmh;
	Person *p;
	BMI *b;

	// Fast forward to my type
	for (EVER) {
		if ((pmh=DmQueryNextInCategory(db, &item, dmAllCategories))==NULL)
			return;

		p=MemHandleLock(pmh);
		type=p->r.type;
		MemHandleUnlock(pmh);

		if (type>RPerson)
			return;

		if (type==RPerson)
			break;

		item++;
	}

	if (DmSeekRecordInCategory(dbData.db, &item, tblMain.top+row, dmSeekForward, dmAllCategories)!=errNone)
		return;

	if ((pmh=DmQueryRecord(dbData.db, item))==NULL)
		return;

	p=MemHandleLock(pmh);

	switch (column) {
	case 0:	// note
		WinEraseRectangle(r, 0);
		fid=FntSetFont(symbolFont);
		WinDrawChars("\x0b", 1, r->topLeft.x+2, r->topLeft.y);
		FntSetFont(fid);
		break;
	case 1: // smiley
		item=0;
		if (BmiFirst(&dbData, p->r.id, &item)==true) {
			if ((bmh=DmQueryRecord(dbData.db, item))) {
				b=MemHandleLock(bmh);
				BmiDrawSmiley(BmiCalculate(p->format, b->height, b->weight), r);
				MemHandleUnlock(bmh);
			}
		}
		break;
	case 2:	// name
		TblSetRowData(tblMain.tbl, row, (UInt32)item);
		UITextDraw(p->name, r);
		break;
	}
	MemHandleUnlock(pmh);
}

/*
**	MainInit
**
**	Initialize the main form.
**
**	Returns true if successful, false otherwise.
*/
Boolean
MainInit(void)
{
	TableStyle styles[]={ { 0, customTableItem, true }, { 1, customTableItem, true }, { 2, customTableItem, true }, { -1 } };

	UITableInit(NULL, &tblMain, fMainTable, fMainScrollBar, DrawTable, styles);
	TblSetColumnSpacing(tblMain.tbl, 1, TblGetColumnSpacing(tblMain.tbl, 1)+4);
	tblMain.records=PersonCount(&dbData);
	UITableUpdateValues(&tblMain);
	return true;
}

/*
**	MainEventHandler
**
**	Event handler for main form.
**
**	 -> ev		Event.
**
**	Returns true if event is handled, false otherwise.
*/
Boolean
MainEventHandler(EventPtr ev)
{
	Char buffer[32];
	MemHandle mh;
	Person *p;
	BMI *b;
	UInt32 id;
	UInt16 item, format;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==fMainNew) {
			Edit(dmMaxRecordIndex);
			return true;
		}
		break;

	case tblSelectEvent:
		TblUnhighlightSelection(tblMain.tbl);
		switch (ev->data.tblSelect.column) {
		case 0:
			Edit((UInt16)TblGetRowData(tblMain.tbl, ev->data.tblSelect.row));
			return true;

		case 1:
			mh=DmQueryRecord(dbData.db, (UInt16)TblGetRowData(tblMain.tbl, ev->data.tblSelect.row));
			ErrFatalDisplayIf(mh==NULL, "(MainEventHandler) Cannot query record.");
			p=MemHandleLock(mh);
			id=p->r.id;
			format=p->format;
			MemHandleUnlock(mh);
			item=0;
			if (BmiFirst(&dbData, id, &item)==true) {
				mh=DmQueryRecord(dbData.db, item);
				ErrFatalDisplayIf(mh==NULL, "(MainEventHandler) Cannot query record.");
				b=MemHandleLock(mh);
				StrPrintF(buffer, "%u", BmiCalculate(format, b->height, b->weight));
				MemHandleUnlock(mh);
				FrmCustomAlert(aBMIInfo, buffer, NULL, NULL);
			}
			return true;

		case 2:
			recordIdx=(UInt16)TblGetRowData(tblMain.tbl, ev->data.tblSelect.row);
			FrmGotoForm(UtilsGetHistoryForm(recordIdx));
			return true;
		}
		break;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mQueryFindHeight:
			FrmGotoForm(fQueryFindHeight);
			return true;
		case mQueryFindWeight:
			FrmGotoForm(fQueryFindWeight);
			return true;
		case mQueryConversion:
			FrmGotoForm(fConversion);
			return true;
		case mOptionsPreferences:
			PreferencesNew();
			return true;
		case mAboutBMITable:
			if (prefs.bmiTable==BmiTableAmerican) {
				AboutHelp(MemHandleLock(resStrBMITableTitle), MemHandleLock(resStrBMITableTextAmerican));
				MemHandleUnlock(resStrBMITableTextAmerican);
			} else {
				AboutHelp(MemHandleLock(resStrBMITableTitle), MemHandleLock(resStrBMITableTextEuropean));
				MemHandleUnlock(resStrBMITableTextEuropean);
			}
			MemHandleUnlock(resStrBMITableTitle);
			return true;
		case mAboutWhatIs:
			AboutHelp(MemHandleLock(resStrWhatIsTitle), MemHandleLock(resStrWhatIsText));
			MemHandleUnlock(resStrWhatIsTitle);
			MemHandleUnlock(resStrWhatIsText);
			return true;
		case mAboutAbout:
			AboutAbout();
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
**	Edit
**
**	Edit/Add a person.
**
**	 -> recIdx	Record index of person to edit (or dmMaxRecordIndex).
*/
static void
Edit(UInt16 recIdx)
{
	EditPerson(recIdx);
	UITableChanged(&tblMain, PersonCount(&dbData));
}

/*
**	PersonCount
**
**	Count the number of persons.
**
**	 -> dbo		Database object.
*/
static UInt16
PersonCount(DB *dbo)
{
	DmOpenRef db=dbo->db;
	UInt16 recIdx=0, count=0;
	MemHandle mh;
	Record *p;

	for (EVER) {
		if ((mh=DmQueryNextInCategory(db, &recIdx, dmAllCategories))==NULL)
			break;

		p=MemHandleLock(mh);
		if (p->type==RPerson)
			count++;
		MemHandleUnlock(mh);

		recIdx++;
	}

	return count;
}

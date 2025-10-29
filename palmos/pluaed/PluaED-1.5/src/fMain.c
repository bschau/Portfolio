/*
 * fMain.c
 */
#include "Include.h"

/* protos */
static void DrawTable(void *, Int16, Int16, RectangleType *);

/* globals */
static Table tblMain;
static Int16 returnTop;

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
	UInt16 recIdx=0, pW=150, len=0;
	Boolean trunc;
	MemHandle mh;
	Char *p, *s, *t, *q;
	UInt32 mhSize;
	FontID fId;

	if (DmSeekRecordInCategory(dbMemo.db, &recIdx, tblMain.top+row, dmSeekForward, prefs.category)!=errNone)
		return;

	TblSetRowData(tbl, row, (UInt32)recIdx);
	WinEraseRectangle(r, 0);

	mh=DBGetRecord(&dbMemo, recIdx);
	mhSize=MemHandleSize(mh);
	p=MemHandleLock(mh);
	if (mhSize>7) {
		s=p;
		if (*s=='-' && *(s+1)=='-') {
			for (s+=2; *s && *s==' '; s++);
			if (*s) {
				for (t=s; *t && *t!='.'; t++);
				if (*t=='.') {
					q=t+1;
					if (*q && *q=='l' && *(q+1) && *(q+1)=='u' && *(q+2) && *(q+2)=='a') {
						len=t-s;
						FntCharsInWidth(s, &pW, &len, &trunc);
						WinDrawChars(s, len, r->topLeft.x, r->topLeft.y);
						MemHandleUnlock(mh);
						return;
					}
				}
			}
		}
	}
	
	for (s=p; *s && *s!='\n'; s++)
		len++;

	fId=FntSetFont(symbolFont);
	WinDrawChars("\x09", 1, r->topLeft.x, r->topLeft.y);
	FntSetFont(fId);
	pW-=12;
	FntCharsInWidth(p, &pW, &len, &trunc);
	WinDrawChars(p, len, r->topLeft.x+12, r->topLeft.y);

	MemHandleUnlock(mh);
}

/*
 * fMainInit
 */
Boolean
fMainInit(void)
{
	TableStyle style[]={ { 0, customTableItem }, { -1 } };

	UITableInit(&tblMain, cMainTable, cMainScrollBar, DrawTable, style, DmNumRecordsInCategory(dbMemo.db, dmAllCategories));
	tblMain.top=returnTop;
	fMainRethink();
	return true;
}

/*
 * fMainEH
 */
Boolean
fMainEH(EventType *ev)
{
	Boolean edited;
	UInt16 category, rIdx;
	UInt32 uid;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case frmSaveEvent:
		prefs.uid=0;
		prefs.insPt=0;
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cMainNew:
			uid=fNewRun();
			fMainRethink();
			if (uid) {
				if (DmFindRecordByID(dbMemo.db, uid, &rIdx)==errNone)
					fEditRun(rIdx);
			}
			return true;

		case cMainPopup:
			category=prefs.category;
			edited=CategorySelect(dbMemo.db, currentForm, cMainPopup, cMainList, true, &prefs.category, catName, 1, 0);
			if (edited || (category!=prefs.category))
				fMainRethink();
			return true;
		}
		break;

	case tblSelectEvent:
		returnTop=tblMain.top;
		rIdx=(UInt16)TblGetRowData(tblMain.tbl, ev->data.tblSelect.row);
		fEditRun(rIdx);
		return true;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mMainPreferences:
			fPreferencesRun();
			return true;

		case mMainAbout:
			fAboutRun();
			UIFormRedraw();
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
 * fMainRethink
 *
 * Update table values.
 */
void
fMainRethink(void)
{
	tblMain.records=DmNumRecordsInCategory(dbMemo.db, prefs.category);
	UITableUpdateValues(&tblMain, true);
	UIPopupSet(dbMemo.db, prefs.category, catName, cMainPopup);
}

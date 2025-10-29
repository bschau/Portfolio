/*
 * fMemoBrowser.c
 */
#include "Include.h"

/* protos */
static void DrawTable(void *, Int16, Int16, RectangleType *);
static Boolean EH(EventType *);
static UInt16 GetExact(Char **);

/* globals */
static UInt16 category=dmAllCategories, selected;
static Char catName[dmCategoryLength+2];
static DB db;
static Table tblMemo;

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
	UInt16 recIdx=0, pW=148, len;
	MemHandle mh;
	Boolean trunc;
	Char *p;

	if (DmSeekRecordInCategory(db.db, &recIdx, tblMemo.top+row, dmSeekForward, category)!=errNone)
		return;

	TblSetRowData(tbl, row, (UInt32)recIdx);
	WinEraseRectangle(r, 0);

	mh=DBGetRecord(&db, recIdx);
	p=MemHandleLock(mh);
	if ((len=GetExact(&p))>0) {
		FntCharsInWidth(p, &pW, &len, &trunc);
		WinDrawChars(p, len, r->topLeft.x, r->topLeft.y);
	}

	MemHandleUnlock(mh);
}

/*
 * fMemoBrowserRun
 *
 * Returns index of selected memo (or dmMaxRecordIndex).
 */
UInt16
fMemoBrowserRun(void)
{
	TableStyle style[]={ { 0, customTableItem }, { -1 } };
	FormSave frm;

	DBInit(&db, "MemoDB");
	if (DBOpen(&db, dmModeReadOnly, true)==false)
		return dmMaxRecordIndex;

	UIFormPrologue(&frm, fMemoBrowser, EH);

	UITableInit(NULL, &tblMemo, cMemoBrowserTable, cMemoBrowserScrollBar, DrawTable, style);
	tblMemo.records=DmNumRecordsInCategory(db.db, category);
	UITableUpdateValues(&tblMemo);

	CategoryGetName(db.db, category, catName);
	CategorySetTriggerLabel(UIObjectGet(cMemoBrowserPopup), catName);

	selected=dmMaxRecordIndex;
	UIFormEpilogue(&frm, NULL, cMemoBrowserCancel);

	DBClose(&db);
	return selected;
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	Boolean edited;
	UInt16 cat;

	switch (ev->eType) {
	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==cMemoBrowserPopup) {
			cat=category;
			edited=CategorySelect(db.db, currentForm, cMemoBrowserPopup, cMemoBrowserList, true, &category, catName, 1, 0);
			if (edited || (cat!=category)) {
				tblMemo.records=DmNumRecordsInCategory(db.db, category);
				UITableUpdateValues(&tblMemo);
			}
			return true;
		}
		break;

	case tblSelectEvent:
		selected=(UInt16)TblGetRowData(tblMemo.tbl, ev->data.tblSelect.row);
		CtlHitControl(UIObjectGet(cMemoBrowserCancel));
		return true;

	default:
		UITableEvents(&tblMemo, ev);
		break;
	}

	return false;
}

/*
 * GetExact
 *
 * Find exact start and length of line. Leading and trailing whitespaces are
 * omitted.
 *
 * <-> src		Source string.
 *
 * Returns length of string.
 */
static UInt16
GetExact(Char **src)
{
	UInt16 len=0;
	Char *s;

	if (src==NULL)
		return 0;

	if ((s=*src)==NULL) {
		*src=NULL;
		return 0;
	}

	for (; *s && *s<=' '; s++);

	if (!*s) {
		*src=NULL;
		return 0;
	}
	*src=s;

	for (; *s && *s!='\n'; s++)
		len++;

	for (s--; s>=*src && *s<=' '; s--)
		len--;

	return len;
}

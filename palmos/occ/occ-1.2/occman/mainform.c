#include "occman.h"

/* protos */
static void ScrollTable(Int16);
static void UpdateTableValues(void);
static void DrawTable(void *, Int16, Int16, RectangleType *);

/* globals */
static Int16 tblTop;

/*
**	ScrollTable
**
**	Scroll table a couple of lines up and down.
**
**	 -> amount	Amount to scroll table (may be negative).
*/
static void
ScrollTable(Int16 amount)
{
	tblTop+=amount;
	if (tblTop<0)
		tblTop=0;
	else if (tblTop>(occList.count-MaxLinesInTable))
		tblTop=occList.count-MaxLinesInTable;

	UpdateTableValues();
	TblDrawTable(UIGetFormObject(fMainTable));
}

/*
**	UpdateTableValues
**
**	Set values for the table (scrollbar, selectables).
*/
static void
UpdateTableValues(void)
{
	TableType *tbl=UIGetFormObject(fMainTable);
	UInt16 pos, maxValue, idx, numRows;

	if (tblTop>(occList.count-MaxLinesInTable))
		tblTop=occList.count-MaxLinesInTable;

	if (tblTop<0)
		tblTop=0;

	pos=tblTop;
	if (occList.count>MaxLinesInTable)
		maxValue=occList.count-MaxLinesInTable;
	else
		maxValue=0;

	SclSetScrollBar(UIGetFormObject(fMainScrollbar), pos, 0, maxValue, MaxLinesInTable);

	numRows=TblGetNumberOfRows(tbl);
	for (idx=0; idx<numRows; idx++) {
		if (idx<occList.count) {
			TblSetRowUsable(tbl, idx, true);
			TblSetRowSelectable(tbl, idx, true);
		} else {
			TblSetRowUsable(tbl, idx, false);
			TblSetRowSelectable(tbl, idx, false);
		}
	}
}

/*
**	DrawTable
**
**	Draw the table contents. As we only have one column this works much
**	like a list.
**	See Reference guide for arguments.
*/
static void
DrawTable(void *tbl, Int16 row, Int16 column, RectangleType *r)
{
	UInt16 pw=r->extent.x, len;
	Int16 tT=tblTop;
	Boolean trunc;
	OCCNode *n;

	for (n=occList.head; n->next && tT>0; n=n->next)
		tT--;
		
	for (; n->next && row>0; n=n->next)
		row--;

	if (n->next) {
		len=StrLen(n->name);
		FntCharsInWidth(n->name, &pw, &len, &trunc);
		WinDrawChars(n->name, len, r->topLeft.x, r->topLeft.y);
	}
}

/*
**	MainFormInit
**
**	Initializes the main form.
**
**	Returns true if successful, false otherwise.
*/
Boolean
MainFormInit(void)
{
	TableType *tbl=UIGetFormObject(fMainTable);
	UInt16 idx, numRows;

	/* initialize table */
	numRows=TblGetNumberOfRows(tbl);
	for (idx=0; idx<numRows; idx++)
		TblSetItemStyle(tbl, idx, 0, customTableItem);

	TblSetColumnUsable(tbl, 0, true);
	TblSetCustomDrawProcedure(tbl, 0, DrawTable);
	TblHasScrollBar(tbl, true);
	UpdateTableValues();
		
	return true;
}

/*
**	MainFormEventHandler
**
**	Event handler for the main form.
**
**	 -> ev		The event to handle.
**
**	Returns true if event was handled, false otherwise.
*/
Boolean
MainFormEventHandler(EventPtr ev)
{
	FormPtr tFrm;
	RectangleType box;
	Boolean didChange;
	TableType *tbl;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		box.topLeft.x=1;
		box.topLeft.y=23;
		box.extent.x=146;
		box.extent.y=112;
		WinDrawRectangleFrame(simpleFrame, &box);
		return true;

	case tblSelectEvent:
		if ((didChange=ViewOCC(ev->data.tblSelect.row+tblTop))==true) {
			tbl=UIGetFormObject(fMainTable);
			TblUnhighlightSelection(tbl);
			UpdateTableValues();
			TblDrawTable(tbl);
		}
		return true;

	case sclRepeatEvent:
		if (ev->data.sclRepeat.scrollBarID==fMainScrollbar) {
			tblTop=ev->data.sclRepeat.newValue;
			TblDrawTable(UIGetFormObject(fMainTable));
		}
		break;

	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==fMainNew) {
			if ((didChange=EditOCC(NEWOCC))==true) {
				tbl=UIGetFormObject(fMainTable);
				TblUnhighlightSelection(tbl);
				UpdateTableValues();
				TblDrawTable(tbl);
			}
		}
		return true;

	case keyDownEvent:
		switch (ev->data.keyDown.chr) {
		case pageUpChr:
		case prevFieldChr:
			ScrollTable(-(MaxLinesInTable-1));
			break;
		case pageDownChr:
		case nextFieldChr:
			ScrollTable(MaxLinesInTable-1);
			break;
		}
		break;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mMainOptionsAbout:
			tFrm=FrmInitForm(fAbout);
			FrmSetActiveForm(tFrm);

			FrmDoDialog(tFrm);
			FrmSetActiveForm(currentForm);
			FrmEraseForm(tFrm);
			FrmDeleteForm(tFrm);
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

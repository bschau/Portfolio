/*
**	ui.c
**
**	An assorted collection of User Interface subroutines.
*/
#include "eventor.h"

/*
**	UIFormGetObject
**
**	Return pointer to form object given by id.
**
**	 -> id		Object ID.
**
**	Returns pointer or NULL (not found).
*/
void *
UIFormGetObject(UInt16 id)
{
	return FrmGetObjectPtr(currentForm, FrmGetObjectIndex(currentForm, id));
}

/*
**	UIFormPrologue
**
**	Standard form start for modal forms.
**
**	 -> frm		Form object.
**	 -> id		ID of new form.
**	 -> eh		Event handler (NULL means no event handler).
*/
void
UIFormPrologue(FormSave *frm, UInt16 id, FormEventHandlerType *eh)
{
	frm->oldForm=currentForm;
	frm->oldId=currentFormID;
	FrmSaveActiveState(&frm->state);

	currentFormID=id;
	currentForm=FrmInitForm(id);
	ErrFatalDisplayIf(currentForm==NULL, "(UIFormPrologue) Cannot initialize form.");

	if (eh)
		FrmSetEventHandler(currentForm, eh);

	FrmSetActiveForm(currentForm);
	FrmDrawForm(currentForm);
}

/*
**	UIFormEpilogue
**
**	Standard form showing and ending modal forms.
**
**	 -> frm		Form object.
**	 -> hook	Hook - stuff to do after returning to form.
**	 -> falseButton	ID of button generating 'false' event (cancel).
**
**	Returns true or false (depending on which button was pressed).
*/
Boolean
UIFormEpilogue(FormSave *frm, void (*hook)(), UInt16 falseButton)
{
	UInt16 button;

	button=FrmDoDialog(currentForm);
	currentForm=frm->oldForm;
	FrmReturnToForm(frm->oldId);
	FrmRestoreActiveState(&frm->state);

	if (hook)
		hook();

	FrmDrawForm(currentForm);

	if (button==falseButton)
		return false;

	return true;
}

/*
**	UIFieldFocus
**
**	Set insertion point as active in this field.
**
**	 -> id		Field id in current form.
*/
void
UIFieldFocus(UInt16 id)
{
	FrmSetFocus(currentForm, FrmGetObjectIndex(currentForm, id));
}

/*
**	UIFieldGetText
**
**	Get text field pointer.
**
**	 -> id		Field id in current form.
**
**	Returns whatever pointer is in the field.  Beware! Can be NULL!
*/
Char *
UIFieldGetText(UInt16 id)
{
	FieldType *fld=UIFormGetObject(id);

	return FldGetTextPtr(fld);
}

/*
**	UIFieldSetText
**
**	Update field with copy of buffer.  Any previous text is discarded.
**
**	 -> id		Field id in current form.
**	 -> buffer	Buffer to get text from.
*/
void
UIFieldSetText(UInt16 id, Char *buffer)
{
	FieldType *fld=UIFormGetObject(id);
	UInt16 tLen=StrLen(buffer);
	MemHandle mh, oldH;
	Char *tp;

	mh=MemHandleNew(tLen+1);
	ErrFatalDisplayIf(mh==NULL, "(UIFieldSetText) Out of memory");

	tp=MemHandleLock(mh);
	if (tLen) 
		MemMove(tp, buffer, tLen);

	tp[tLen]='\x00';
	MemHandleUnlock(mh);

	oldH=FldGetTextHandle(fld);
	FldSetTextHandle(fld, mh);

	if (oldH)
		MemHandleFree(oldH);

	FldDrawField(fld);
}

#if 0
/*
**	UIFieldUneditable
**
**	Make a field uneditable.
**
**	 -> id		Field ID.
*/
void
UIFieldUneditable(UInt16 id)
{
	FieldType *fld=UIFormGetObject(id);
	FieldAttrType attr;

	FldGetAttributes(fld, &attr);
	attr.editable=0;
	attr.underlined=noUnderline;
	FldSetAttributes(fld, &attr);
	FldDrawField(fld);
}

/*
**	UIFieldClear
**
**	Clears text from the field.
**
**	 -> id		Field ID.
*/
void
UIFieldClear(UInt16 id)
{
	FieldType *fld=UIFormGetObject(id);

	FldReleaseFocus(fld);
	FldDelete(fld, 0, FldGetTextLength(fld));
	FldRecalculateField(fld, true);
	UIFieldFocus(id);
}
#endif

/*
**	UIFieldScrollBarHandler
**
**	Handler for scrollbar events.
**
**	 -> ev		Event.
**	 -> fId		Field ID.
**	 -> sId		ScrollBar ID.
**
**	Returns true if handled, false otherwise.
*/
Boolean
UIFieldScrollBarHandler(EventType *ev, UInt16 fId, UInt16 sId)
{
	switch (ev->eType) {
	case fldChangedEvent:
		if (ev->data.fldChanged.fieldID==fId) {
			UIFieldUpdateScrollBar(fId, sId);
			return true;
		}
		break;
	case sclRepeatEvent:
		if (ev->data.sclRepeat.scrollBarID==sId) {
			UIFieldScrollLines(fId, ev->data.sclRepeat.newValue-ev->data.sclRepeat.value);
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
**	UIFieldScrollBarKeyHandler
**
**	Handler for scrollbar key events.
**
**	 -> ev		Event.
**	 -> fId		Field ID.
**	 -> sId		ScrollBar ID.
**
**	Returns true if handled, false otherwise.
*/
Boolean
UIFieldScrollBarKeyHandler(EventType *ev, UInt16 fId, UInt16 sId)
{
	Int16 focusIndex;
	UInt16 focusId;

	if ((focusIndex=FrmGetFocus(currentForm))==noFocus)
		return false;

	if ((focusId=FrmGetObjectId(currentForm, focusIndex))==fId) {
		switch (ev->data.keyDown.chr) {
		case pageUpChr:
			UIFieldPageScroll(fId, sId, winUp);
			return true;
		case pageDownChr:
			UIFieldPageScroll(fId, sId, winDown);
			return true;
			/* FALL-THROUGH */
		default:
			break;
		}
	}

	return false;
}

/*
**	UIFieldUpdateScrollBar
**
**	Update scrollbar based on field attributes.
**
**	 -> fId		Field ID.
**	 -> sId		ScrollBar ID.
*/
void
UIFieldUpdateScrollBar(UInt16 fId, UInt16 sId)
{
	FieldType *fld=UIFormGetObject(fId);
	ScrollBarType *scr=UIFormGetObject(sId);
	UInt16 mv=0, curPos, txtH, fldH;

	FldGetScrollValues(fld, &curPos, &txtH, &fldH);

	if (txtH>fldH)
		mv=txtH-fldH;
	else {
		if (curPos)
			mv=curPos;
	}

	SclSetScrollBar(scr, curPos, 0, mv, fldH-1);
}

/*
**	UIFieldScrollLines
**
**	Scroll field number of lines.
**
**	 -> fId		Field ID.
**	 -> num		Number of lines to scroll.
*/
void
UIFieldScrollLines(UInt16 fId, Int16 num)
{
	FieldType *fld=UIFormGetObject(fId);

	if (num<0)
		FldScrollField(fld, -num, winUp);
	else
		FldScrollField(fld, num, winDown);
}

/*
**	UIFieldPageScroll
**
**	Scroll field a page.
**
**	 -> fId		Field ID.
**	 -> sId		ScrollBar ID.
**	 -> dir		Direction to scroll.
*/
void
UIFieldPageScroll(UInt16 fId, UInt16 sId, WinDirectionType dir)
{
	FieldType *fld=UIFormGetObject(fId);
	Int16 lts;

	if (FldScrollable(fld, dir)) {
		lts=FldGetVisibleLines(fld)-1;

		if (dir==winUp)
			lts=-lts;

		UIFieldScrollLines(fId, lts);
		UIFieldUpdateScrollBar(fId, sId);
	}
}

#if 0
/*
**	UITextDraw
**
**	Draws a possible bounded text line.
**
**	 -> txt		Text to draw.
**	 -> r		Rectangle.
*/
void
UITextDraw(Char *txt, RectangleType *r)
{
	UInt16 len=StrLen(txt), width=r->extent.x;
	Boolean trunc;

	WinEraseRectangle(r, 0);
	FntCharsInWidth(txt, &width, &len, &trunc);
	WinDrawChars(txt, len, r->topLeft.x, r->topLeft.y);
}

/*
**	UITextDrawR
**
**	Draws a possible bounded text line. The line is right aligned.
**
**	 -> txt		Text to draw.
**	 -> r		Rectangle.
*/
void
UITextDrawR(Char *txt, RectangleType *r)
{
	UInt16 len=StrLen(txt), width=r->extent.x;
	Boolean trunc;

	WinEraseRectangle(r, 0);
	FntCharsInWidth(txt, &width, &len, &trunc);
	width=r->extent.x-width;
	WinDrawChars(txt, len, r->topLeft.x+width, r->topLeft.y);
}
#endif

/*
**	UITableInit
**
**	Initialize a table structure.
**
**	 -> db		Database object (NULL to skip init. of records).
**	 -> table	Table object.
**	 -> id		Table id.
**	 -> scrBarId	Scrollbar id.
**	 -> dc		DrawTable Callback.
**	 -> style	Table styles.
*/
void
UITableInit(DB *db, Table *table, UInt16 id, UInt16 scrBarId, TableDrawItemFuncPtr dc, TableStyle *style)
{
	TableType *tbl=UIFormGetObject(id);
	Int16 rowIdx, sIdx;

	table->id=id;
	table->scrollBarId=scrBarId;
	table->tbl=tbl;
	table->top=0;
	if (db)
		table->records=DmNumRecordsInCategory(db->db, dmAllCategories);
	else
		table->records=0;

	table->rows=TblGetNumberOfRows(tbl);

	if (scrBarId)
		TblHasScrollBar(tbl, true);

	for (rowIdx=0; rowIdx<(table->rows); rowIdx++) {
		for (sIdx=0; style[sIdx].column!=-1; sIdx++) {
			TblSetItemStyle(tbl, rowIdx, style[sIdx].column, style[sIdx].style);
			TblSetCustomDrawProcedure(tbl, sIdx, dc);
		}
	}

	for (sIdx=0; style[sIdx].column!=-1; sIdx++)
		TblSetColumnUsable(tbl, style[sIdx].column, true);
}

/*
**	UITableScroll
**
**	Scroll table a couple of lines up and down.
**
**	 -> table	Table object.
**	 -> amount	Amount to scroll table (may be negative - scroll up).
*/
void
UITableScroll(Table *table, Int16 amount)
{
	table->top+=amount;
	if ((table->top)<0)
		table->top=0;
	else if ((table->top)>((table->records)-(table->rows)))
		table->top=(table->records)-(table->rows);

	UITableUpdateValues(table);
}

/*
**	UITableUpdateValues
**
**	Set values for the table (scrollbar, selectables).
**
**	 -> table	Table object.
*/
void
UITableUpdateValues(Table *table)
{
	TableType *tbl=table->tbl;
	UInt16 maxValue, idx;

	if ((table->top)>((table->records)-(table->rows)))
		table->top=(table->records)-(table->rows);

	if ((table->top)<0)
		table->top=0;

	if ((table->records)>(table->rows))
		maxValue=(table->records)-(table->rows);
	else
		maxValue=0;

	if (table->scrollBarId)
		SclSetScrollBar(UIFormGetObject(table->scrollBarId), table->top, 0, maxValue, table->rows);

	for (idx=0; idx<(table->rows); idx++) {
		if (idx<(table->records)) {
			TblSetRowUsable(tbl, idx, true);
			TblSetRowSelectable(tbl, idx, true);
		} else {
			TblSetRowUsable(tbl, idx, false);
			TblSetRowSelectable(tbl, idx, false);
		}
	}
	TblEraseTable(tbl);
	TblDrawTable(tbl);
}

/*
**	UITableEvents
**
**	Handle basic events for a table.
**
**	 -> table	Table object.
**	 -> ev		Event.
**
**	Returns true or false.
*/
Boolean
UITableEvents(Table *table, EventType *ev)
{
	switch (ev->eType) {
	case sclRepeatEvent:
		if (ev->data.sclRepeat.scrollBarID==table->scrollBarId) {
			if (table->top!=ev->data.sclRepeat.newValue) {
				table->top=ev->data.sclRepeat.newValue;
				TblEraseTable(table->tbl);
				TblDrawTable(table->tbl);
			}
		}

		return true;

	case keyDownEvent:
		switch (ev->data.keyDown.chr) {
		case pageUpChr:
		case prevFieldChr:
			UITableScroll(table, -((table->rows)-1));
			break;
		case pageDownChr:
		case nextFieldChr:
			UITableScroll(table, (table->rows)-1);
			break;
		}

		return true;
	default:
		break;
	}

	return false;
}

/*
**	UITableChanged
**
**	Updates table structure and redraw.
**
**	 -> tbl		Table object.
**	 -> count	Count of items.
*/
void
UITableChanged(Table *tbl, UInt16 count)
{
	tbl->records=count;
	UITableUpdateValues(tbl);
	TblUnhighlightSelection(tbl->tbl);
	TblEraseTable(tbl->tbl);
	TblDrawTable(tbl->tbl);
}

#if 0
/*
**	UIRefocus
**
**	Refocus insertion point.
**
**	 -> focusBand	Array of object IDs to switch between.
**	 -> evChr	Event character.
**
**	Returns true if refocussed, false otherwise.
*/
Boolean
UIRefocus(UInt16 *focusBand, WChar evChr)
{
	Int16 idx, direction, focusIdx;
	UInt16 id;

	switch (evChr) {
	case prevFieldChr:
	case pageUpChr:
		direction=-1;
		break;
	case nextFieldChr:
	case pageDownChr:
		direction=1;
		break;
	default:
		return false;
	}

	if ((focusIdx=FrmGetFocus(currentForm))==noFocus)
		return false;

	id=FrmGetObjectId(currentForm, focusIdx);
	for (idx=1; focusBand[idx]; idx++) {
		if (focusBand[idx]==id) {
			if ((id=focusBand[idx+direction])==0)
				id=focusBand[1];

			FrmSetFocus(currentForm, FrmGetObjectIndex(currentForm, id));
			return true;
		}
	}

	return false;
}
#endif

/*
**	UIShowObject
**
**	Show an object in the current form.
**
**	 -> id		Object ID.
*/
void
UIShowObject(UInt16 id)
{
	FrmShowObject(currentForm, FrmGetObjectIndex(currentForm, id));
}

#if 0
/*
**	UIListSetPopup
**
**	Set values of list and popup to the given value.
**
**	 -> lId		List ID.
**	 -> pId		Popup ID.
**	 -> sel		Selection.
*/
void
UIListSetPopup(UInt16 lId, UInt16 pId, Int16 sel)
{
	ListType *l=UIFormGetObject(lId);

	LstSetSelection(l, sel);
	CtlSetLabel(UIFormGetObject(pId), LstGetSelectionText(l, sel));
}
#endif

/*
 * UI.c
 */
#include "Include.h"

/*
 * UIObjectGet
 *
 * Return pointer to form object given by id.
 *
 *  -> id		Object ID.
 *
 * Returns pointer or NULL (not found).
 */
void *
UIObjectGet(UInt16 id)
{
	return FrmGetObjectPtr(currentForm, FrmGetObjectIndex(currentForm, id));
}

/*
 * UIObjectShow
 *
 * Show an object in the current form.
 *
 *  -> id	Object ID.
 */
void
UIObjectShow(UInt16 id)
{
	FrmShowObject(currentForm, FrmGetObjectIndex(currentForm, id));
}

/*
 * UIObjectHid
 *
 * Hide an object in the current form.
 *
 *  -> id	Object ID.
 */
void
UIObjectHide(UInt16 id)
{
	FrmHideObject(currentForm, FrmGetObjectIndex(currentForm, id));
}

/*
 * UIFormPrologue
 *
 * Standard form start for Modal forms.
 *
 *  -> frm		Form object.
 *  -> id		ID of new form.
 *  -> eh		Event handler (NULL means no event handler).
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
 * UIFormEpilogue
 *
 * Standard form for showing and ending Modal forms.
 *
 *  -> frm		Form object.
 *  -> hook		Hook - stuff to do after returning to previous form.
 *  -> falseButton	ID of button generating 'false' event (cancel).
 *
 *  Returns true or false (depending on which button was pressed).
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
	FrmUpdateForm(frm->oldId, frmRedrawUpdateCode);

	if (button==falseButton)
		return false;

	return true;
}

/*
 * UIFieldFocus
 *
 * Bring this field to Focus.
 *
 *   -> id		Field id in current form.
 */
void
UIFieldFocus(UInt16 id)
{
	FrmSetFocus(currentForm, FrmGetObjectIndex(currentForm, id));
}

/*
 * UIFieldRefocus
 *
 * Refocus insertion point.
 *
 *  -> focusBand	Array of object IDs to switch between.
 *  -> evChr	Event character.
 *
 * Returns true if refocussed, false otherwise.
 */
Boolean
UIFieldRefocus(UInt16 *focusBand, WChar evChr)
{
	Int16 idx, focusIdx, zeroPos, startPos;
	UInt16 id;
	FieldAttrType attr;

	if ((focusIdx=FrmGetFocus(currentForm))==noFocus)
		return false;

	id=FrmGetObjectId(currentForm, focusIdx);

	startPos=-1;
	for (zeroPos=0; focusBand[zeroPos]; zeroPos++) {
		if (focusBand[zeroPos]==id)
			startPos=zeroPos;
	}

	if (startPos==-1)
		return false;

	if (evChr==prevFieldChr || evChr==pageUpChr) {
		for (idx=startPos-1; ; ) {
			if (idx<0)
				idx=zeroPos-1;

			id=focusBand[idx];
			FldGetAttributes(UIObjectGet(id), &attr);
			if (attr.editable && attr.usable) {
				FrmSetFocus(currentForm, FrmGetObjectIndex(currentForm, id));
				return true;
			}
			idx--;
			if (idx==startPos)
				break;
		}
	} else if (evChr==nextFieldChr || evChr==pageDownChr) {
		for (idx=startPos+1; ; ) {
			if (focusBand[idx]==0)
				idx=0;

			id=focusBand[idx];
			FldGetAttributes(UIObjectGet(id), &attr);
			if (attr.editable && attr.usable) {
				FrmSetFocus(currentForm, FrmGetObjectIndex(currentForm, id));
				return true;
			}
			idx++;
			if (idx==startPos)
				break;
		}
	}

	return false;
}

/*
 * UIFieldGetText
 *
 * Get text field pointer.
 *
 *  -> id		Field id in current form.
 *
 * Returns whatever pointer is in the field.  Beware! Can be NULL!
 */
Char *
UIFieldGetText(UInt16 id)
{
	return FldGetTextPtr(UIObjectGet(id));
}

/*
 * UIFieldSetText
 *
 * Update field with copy of buffer.  Any previous text is discarded.
 *
 *  -> id		Field id in current form.
 *  -> buffer		Buffer to get text from.
 */
void
UIFieldSetText(UInt16 id, Char *buffer)
{
	FieldType *fld=UIObjectGet(id);
	UInt16 tLen=StrLen(buffer), max;
	MemHandle mh, oldH;
	Char *tp;

	if ((max=FldGetMaxChars(fld))>0) {
		if (max<tLen)
			tLen=max;
	} else
		max=tLen;

	mh=MemHandleNew(max+1);
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

/*
 * UIFieldClear
 *
 * Clear the text field.
 *
 *  -> id		Field id in current form.
 */
void
UIFieldClear(UInt16 id)
{
	FieldType *fld=UIObjectGet(id);

	FldDelete(fld, 0, FldGetTextLength(fld));
}

/*
 * UIFieldSelectAll
 *
 * Set text and select all text in the field.
 *
 *  -> id		Field id.
 *  -> text		Text to set (may be NULL).
 *  -> focus		Focus after selection?
 */
void
UIFieldSelectAll(UInt16 id, Char *text, Boolean focus)
{
	FieldType *fld=UIObjectGet(id);

	if (text)
		UIFieldSetText(id, text);

	FldSetSelection(fld, 0, FldGetTextLength(fld));
	if (focus)
		UIFieldFocus(id);
}

/*
 * UIFieldScrollBarHandler
 *
 * Handler for scrollbar events.
 *
 *  -> ev		Event.
 *  -> fId		Field ID.
 *  -> sId		ScrollBar ID.
 *
 * Returns true if handled, false otherwise.
 */
Boolean
UIFieldScrollBarHandler(EventType *ev, UInt16 fId, UInt16 sId)
{
	if (ev->eType==fldChangedEvent) {
		if (ev->data.fldChanged.fieldID==fId) {
			UIFieldUpdateScrollBar(fId, sId);
			return true;
		}
	} else if (ev->eType==sclRepeatEvent) {
		if (ev->data.sclRepeat.scrollBarID==sId) {
			UIFieldScrollLines(fId, ev->data.sclRepeat.newValue-ev->data.sclRepeat.value);
			return true;
		}
	}

	return false;
}

/*
 * UIFieldScrollBarKeyHandler
 *
 * Handler for scrollbar key events.
 *
 *  -> ev		Event.
 *  -> fId		Field ID.
 *  -> sId		ScrollBar ID.
 *
 * Returns true if handled, false otherwise.
 */
Boolean
UIFieldScrollBarKeyHandler(EventType *ev, UInt16 fId, UInt16 sId)
{
	Int16 focusIndex;
	UInt16 focusId;

	if ((focusIndex=FrmGetFocus(currentForm))==noFocus)
		return false;

	if ((focusId=FrmGetObjectId(currentForm, focusIndex))==fId) {
		if (ev->data.keyDown.chr==pageUpChr) {
			UIFieldPageScroll(fId, sId, winUp);
			return true;
		} else if (ev->data.keyDown.chr==pageDownChr) {
			UIFieldPageScroll(fId, sId, winDown);
			return true;
		}
	}

	return false;
}

/*
 * UIFieldUpdateScrollBar
 *
 * Update scrollbar based on field attributes.
 *
 *  -> fId		Field ID.
 *  -> sId		ScrollBar ID.
 */
void
UIFieldUpdateScrollBar(UInt16 fId, UInt16 sId)
{
	FieldType *fld=UIObjectGet(fId);
	ScrollBarType *scr=UIObjectGet(sId);
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
 * UIFieldScrollLines
 *
 * Scroll field number of lines.
 *
 *  -> fId		Field ID.
 *  -> num		Number of lines to scroll.
 */
void
UIFieldScrollLines(UInt16 fId, Int16 num)
{
	FieldType *fld=UIObjectGet(fId);

	if (num<0)
		FldScrollField(fld, -num, winUp);
	else
		FldScrollField(fld, num, winDown);
}

/*
 * UIFieldPageScroll
 *
 * Scroll field a page.
 *
 *  -> fId		Field ID.
 *  -> sId		ScrollBar ID.
 *  -> dir		Direction to scroll.
 */
void
UIFieldPageScroll(UInt16 fId, UInt16 sId, WinDirectionType dir)
{
	FieldType *fld=UIObjectGet(fId);

	if (FldScrollable(fld, dir)) {
		Int16 lts;

		lts=FldGetVisibleLines(fld)-1;

		if (dir==winUp)
			lts=-lts;

		UIFieldScrollLines(fId, lts);
		UIFieldUpdateScrollBar(fId, sId);
	}
}

/*
 * UITableInit
 *
 * Initialize a table structure.
 *
 *  -> table		Table object.
 *  -> id		Table id.
 *  -> scrBarId		Scrollbar id.
 *  -> dc		DrawTable Callback.
 *  -> style		Table styles.
 *  -> numRecs		Number of records in table.
 */
void
UITableInit(Table *table, UInt16 id, UInt16 scrBarId, TableDrawItemFuncPtr dc, TableStyle *style, UInt16 numRecs)
{
	TableType *tbl=UIObjectGet(id);
	Int16 rowIdx, sIdx;

	table->id=id;
	table->scrollBarId=scrBarId;
	table->tbl=tbl;
	table->top=0;
	table->records=numRecs;

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
 * UITableScroll
 *
 * Scroll table a couple of lines up and down.
 *
 *  -> table		Table object.
 *  -> amount		Amount to scroll table (may be negative - scroll up).
 */
void
UITableScroll(Table *table, Int16 amount)
{
	table->top+=amount;
	if ((table->top)<0)
		table->top=0;
	else if ((table->top)>((table->records)-(table->rows)))
		table->top=(table->records)-(table->rows);

	UITableUpdateValues(table, true);
}

/*
 * UITableUpdateValues
 *
 * Set values for the table (scrollbar, selectables).
 *
 *  -> table	Table object.
 *  -> redraw	Redraw table?
 */
void
UITableUpdateValues(Table *table, Boolean redraw)
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
		SclSetScrollBar(UIObjectGet(table->scrollBarId), table->top, 0, maxValue, table->rows);

	for (idx=0; idx<(table->rows); idx++) {
		if (idx<(table->records)) {
			TblSetRowUsable(tbl, idx, true);
			TblSetRowSelectable(tbl, idx, true);
		} else {
			TblSetRowUsable(tbl, idx, false);
			TblSetRowSelectable(tbl, idx, false);
		}
	}

	if (redraw) {
		TblEraseTable(tbl);
		TblDrawTable(tbl);
	}
}

/*
 * UITableEvents
 *
 * Handle basic events for a table.
 *
 *  -> table		Table object.
 *  -> ev		Event.
 *
 * Returns true or false.
 */
Boolean
UITableEvents(Table *table, EventType *ev)
{
	if (ev->eType==sclRepeatEvent) {
		if (ev->data.sclRepeat.scrollBarID==table->scrollBarId) {
			if (table->top!=ev->data.sclRepeat.newValue) {
				table->top=ev->data.sclRepeat.newValue;
				TblEraseTable(table->tbl);
				TblDrawTable(table->tbl);
			}
		}

		return true;
	} else if (ev->eType==keyDownEvent) {
		if (ev->data.keyDown.chr==pageUpChr || ev->data.keyDown.chr==prevFieldChr)
			UITableScroll(table, -((table->rows)-1));
		else if (ev->data.keyDown.chr==pageDownChr || ev->data.keyDown.chr==nextFieldChr)
			UITableScroll(table, (table->rows)-1);

		return true;
	}

	return false;
}

/*
 * UITableChanged
 *
 * Updates table structure and redraw.
 *
 *  -> tbl		Table object.
 *  -> count		Count of items.
 */
void
UITableChanged(Table *tbl, UInt16 count)
{
	tbl->records=count;
	UITableUpdateValues(tbl, false);
	TblUnhighlightSelection(tbl->tbl);
	TblEraseTable(tbl->tbl);
	TblDrawTable(tbl->tbl);
}

/*
 * UIPopupSet
 *
 * Set category popup from database.
 *
 *  -> db		Database.
 *  -> idx		Category index.
 *  -> dst		Destination string.
 *  -> id		Popup ID.
 */
void
UIPopupSet(DmOpenRef db, UInt16 idx, Char *dst, UInt16 id)
{
	CategoryGetName(db, idx, dst);
	CategorySetTriggerLabel(UIObjectGet(id), dst);
}

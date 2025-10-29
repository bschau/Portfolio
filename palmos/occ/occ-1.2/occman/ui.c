#include "occman.h"

/*
**	UIGetFormObject
**
**	Find a given object in the current form.
**
**	 -> id		Object ID to look for.
**
**	Returns pointer to object or NULL if object was not found.
*/
void *
UIGetFormObject(UInt16 id)
{
	return FrmGetObjectPtr(currentForm, FrmGetObjectIndex(currentForm, id));
}

/*
**	UIGetFieldText
**
**	Get text field pointer.
**
**	 -> id		Field id in current form.
**
**	Returns whatever pointer is in the field.  Beware! Can be NULL!
*/
Char *
UIGetFieldText(UInt16 id)
{
	FieldType *fld=UIGetFormObject(id);

	return FldGetTextPtr(fld);
}

/*
**	UISetFieldText
**
**	Update field with copy of buffer.  Any previous text is discarded.
**
**	 -> id		Field id in current form.
**	 -> buffer	Buffer to get text from.
*/
void
UISetFieldText(UInt16 id, Char *buffer)
{
	FieldType *fld=UIGetFormObject(id);
	UInt16 tLen=StrLen(buffer);
	MemHandle mh, oldH;
	Char *tp;

	mh=MemHandleNew(tLen+1);
	ErrFatalDisplayIf(mh==NULL, "(UISetFieldText) Out of memory.");

	tp=MemHandleLock(mh);
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
**	UIReFocus
**
**	Re-focus insertion point.
**
**	 -> focusBand	String of focus objects.
**	 -> evChr	Event character.
**
**	Returns true (if did re-focus), false otherwise.
*/
Boolean
UIReFocus(UInt16 *focusBand, WChar evChr)
{
	Int16 idx, direction, focusIdx;
	UInt16 objId;

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

	objId=FrmGetObjectId(currentForm, focusIdx);
	for (idx=1; focusBand[idx]; idx++) {
		if (focusBand[idx]==objId) {
			if ((objId=focusBand[idx+direction])==0)
				objId=focusBand[1];

			FrmSetFocus(currentForm, FrmGetObjectIndex(currentForm, objId));
			return true;
		}
	}

	return false;
}

/*
**	UIScrollbarHandler
**
**	Handles movement of scrollbar and update of field.
**
**	 -> ev		Event.
**	 -> scrId	Scrollbar ID.
**	 -> fldId	Field ID.
*/
Boolean
UIScrollbarHandler(EventPtr ev, UInt16 scrId, UInt16 fldId)
{
	switch (ev->eType) {
	case fldChangedEvent:
		if (ev->data.fldChanged.fieldID==fldId) {
			UIUpdateScrollbar(scrId, fldId);
			return true;
		}
		break;
	case sclRepeatEvent:
		if (ev->data.sclRepeat.scrollBarID==scrId) {
			UIScrollLines(fldId, ev->data.sclRepeat.newValue-ev->data.sclRepeat.value);
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
**	UIScrollbarKeyHandler
**
**	Handler for scrollbar keypress.
**
**	 -> ev		Event.
**	 -> scrId	Scollbar ID.
**	 -> fldId	Field ID.
*/
Boolean
UIScrollbarKeyHandler(EventPtr ev, UInt16 scrId, UInt16 fldId)
{
	Int16 fIdx=FrmGetFocus(currentForm);

	if (fIdx==noFocus)
		return false;

	if ((fIdx=FrmGetObjectId(currentForm, fIdx))==fldId) {
		switch (ev->data.keyDown.chr) {
		case pageUpChr:
		case prevFieldChr:
			UIScrollPage(scrId, fldId, winUp);
			return true;
		case pageDownChr:
		case nextFieldChr:
			UIScrollPage(scrId, fldId, winDown);
			return true;
		default:
			break;
		}
	}

	return false;
}

/*
**	UIUpdateScrollbar
**
**	Update scrollbar position and sizes in lieu of what is in field.
**
**	 -> scrId	Scrollbar ID.
**	 -> fldId	Field ID.
*/
void
UIUpdateScrollbar(UInt16 scrId, UInt16 fldId)
{
	ScrollBarType *scr=UIGetFormObject(scrId);
	FieldType *fld=UIGetFormObject(fldId);
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
**	UIScrollLines
**
**	Scroll field number of lines.
**
**	 -> fldId	Field ID.
**	 -> amount	Amount of lines to scroll.
*/
void
UIScrollLines(UInt16 fldId, Int16 amount)
{
	FieldType *fld=UIGetFormObject(fldId);

	if (amount==0)
		return;

	if (amount<0)
		FldScrollField(fld, -amount, winUp);
	else
		FldScrollField(fld, amount, winDown);
}

/*
**	UIScrollPage
**
**	Scroll a page when the scroll buttons are hit.
**
**	 -> scrId	Scrollbar ID.
**	 -> fldId	Field ID.
**	 -> dir		Direction.
*/
void
UIScrollPage(UInt16 scrId, UInt16 fldId, WinDirectionType dir)
{
	FieldType *fld=UIGetFormObject(fldId);
	Int16 toScroll;

	if (FldScrollable(fld, dir)) {
		toScroll=FldGetVisibleLines(fld)-1;

		if (dir==winUp)
			toScroll=-toScroll;

		UIScrollLines(fldId, toScroll);
		UIUpdateScrollbar(scrId, fldId);
	}
}

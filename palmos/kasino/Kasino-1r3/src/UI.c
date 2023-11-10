/*
 * UI.c
 */
#include "Include.h"

/*
 * UIFormGetObject
 *
 * Return pointer to form object given by id.
 *
 *  -> id		Object ID.
 *
 * Returns pointer or NULL (not found).
 */
void *
UIFormGetObject(UInt16 id)
{
	return FrmGetObjectPtr(currentForm, FrmGetObjectIndex(currentForm, id));
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
	FieldType *fld=UIFormGetObject(id);

	return FldGetTextPtr(fld);
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
	FieldType *fld=UIFormGetObject(id);
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
 * UIFieldReFocus
 *
 * Re-focus insertion point.
 *
 *  -> focusBand	String of focus objects.
 *  -> evChr		Event character.
 *
 * Returns true (if did re-focus), false otherwise.
 */
Boolean
UIFieldReFocus(UInt16 *focusBand, WChar evChr)
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
 * UIFieldScrollbarHandler
 *
 * Handles movement of scrollbar and update of field.
 *
 *  -> ev		Event.
 *  -> scrId		Scrollbar ID.
 *  -> fldId		Field ID.
 */
Boolean
UIFieldScrollbarHandler(EventPtr ev, UInt16 scrId, UInt16 fldId)
{
	switch (ev->eType) {
	case fldChangedEvent:
		if (ev->data.fldChanged.fieldID==fldId) {
			UIFieldUpdateScrollbar(scrId, fldId);
			return true;
		}
		break;
	case sclRepeatEvent:
		if (ev->data.sclRepeat.scrollBarID==scrId) {
			UIFieldScrollLines(fldId, ev->data.sclRepeat.newValue-ev->data.sclRepeat.value);
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * UIFieldScrollbarKeyHandler
 *
 * Handler for scrollbar keypress.
 *
 *  -> ev		Event.
 *  -> scrId		Scollbar ID.
 *  -> fldId		Field ID.
 */
Boolean
UIFieldScrollbarKeyHandler(EventPtr ev, UInt16 scrId, UInt16 fldId)
{
	Int16 fIdx=FrmGetFocus(currentForm);

	if (fIdx==noFocus)
		return false;

	if ((fIdx=FrmGetObjectId(currentForm, fIdx))==fldId) {
		switch (ev->data.keyDown.chr) {
		case pageUpChr:
		case prevFieldChr:
			UIFieldScrollPage(scrId, fldId, winUp);
			return true;
		case pageDownChr:
		case nextFieldChr:
			UIFieldScrollPage(scrId, fldId, winDown);
			return true;
		default:
			break;
		}
	}

	return false;
}

/*
 * UIFieldUpdateScrollbar
 *
 * Update scrollbar position and sizes in lieu of what is in field.
 *
 *  -> scrId		Scrollbar ID.
 *  -> fldId		Field ID.
 */
void
UIFieldUpdateScrollbar(UInt16 scrId, UInt16 fldId)
{
	ScrollBarType *scr=UIFormGetObject(scrId);
	FieldType *fld=UIFormGetObject(fldId);
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
 *  -> fldId		Field ID.
 *  -> amount		Amount of lines to scroll.
 */
void
UIFieldScrollLines(UInt16 fldId, Int16 amount)
{
	FieldType *fld=UIFormGetObject(fldId);

	if (amount==0)
		return;

	if (amount<0)
		FldScrollField(fld, -amount, winUp);
	else
		FldScrollField(fld, amount, winDown);
}

/*
 * UIFieldScrollPage
 *
 * Scroll a page when the scroll buttons are hit.
 *
 *  -> scrId		Scrollbar ID.
 *  -> fldId		Field ID.
 *  -> dir		Direction.
 */
void
UIFieldScrollPage(UInt16 scrId, UInt16 fldId, WinDirectionType dir)
{
	FieldType *fld=UIFormGetObject(fldId);
	Int16 toScroll;

	if (FldScrollable(fld, dir)) {
		toScroll=FldGetVisibleLines(fld)-1;

		if (dir==winUp)
			toScroll=-toScroll;

		UIFieldScrollLines(fldId, toScroll);
		UIFieldUpdateScrollbar(scrId, fldId);
	}
}

/*
 * UIIntegerDialog
 *
 * Bring up an Integer Dialog.
 *
 *  -> dialog		Dialog.
 *  -> fldID		Integer field.
 *  -> okID		Id of OK button.
 * <-> dst		Where to store integer.
 *
 * Returns true if the OK button was pressed, false otherwise.
 */
Boolean
UIIntegerDialog(UInt16 dialog, UInt16 fldID, UInt16 okID, UInt32 *dst)
{
	FormPtr iFrm=FrmInitForm(dialog), oldForm=currentForm;
	Boolean returnCode=false;
	Char buffer[maxStrIToALen+2];
	Char *fldText;

	ErrFatalDisplayIf(iFrm==NULL, "(UIIntegerDialog) Cannot initialize Integer form.");
	currentForm=iFrm;
	FrmSetActiveForm(currentForm);
	FrmDrawForm(currentForm);
	FrmSetFocus(currentForm, FrmGetObjectIndex(currentForm, fldID));

	if (*dst) {
		StrIToA(buffer, (Int32)*dst);
		UIFieldSetText(fldID, buffer);
	}

	if ((FrmDoDialog(currentForm))==okID) {
		returnCode=true;
		fldText=UIFieldGetText(fldID);
		if (fldText==NULL)
			*dst=0;
		else
			*dst=StrAToI(fldText);
	}

	FrmEraseForm(iFrm);
	FrmDeleteForm(iFrm);
	currentForm=oldForm;
	FrmSetActiveForm(currentForm);

	return returnCode;
}

/*
 * UIObjectHide
 *
 * Hide an object in the current form.
 *
 *  -> id		Object id.
 */
void
UIObjectHide(UInt16 id)
{
	FrmHideObject(currentForm, FrmGetObjectIndex(currentForm, id));
}

/*
 * UIObjectHideX
 *
 * Hide several objects in the current form.
 *
 *  -> ids		Object id list (zero terminated).
 */
void
UIObjectHideX(UInt16 *ids)
{
	while (*ids) {
		FrmHideObject(currentForm, FrmGetObjectIndex(currentForm, *ids));
		ids++;
	}
}

/*
 * UIObjectShow
 *
 * Show an object in the current form.
 *
 *  -> id		Object id.
 */
void
UIObjectShow(UInt16 id)
{
	FrmShowObject(currentForm, FrmGetObjectIndex(currentForm, id));
}

/*
 * UIObjectShowX
 *
 * Show several objects in the current form.
 *
 *  -> ids		Object id list (zero terminated).
 */
void
UIObjectShowX(UInt16 *ids)
{
	while (*ids) {
		FrmShowObject(currentForm, FrmGetObjectIndex(currentForm, *ids));
		ids++;
	}
}

/*******************************************************************************
*
*	scrollbar.c
*
*	Routines to drive scrollbar objects.
*/
#include "sit.h"

/* protos */
static void scrolllines(UInt16, UInt16, Int16);
static void pagescroll(UInt16, UInt16, WinDirectionType);

/*******************************************************************************
*
*	scrollbarhandler
*
*	Handler for scrollbar events.
*/
Boolean
scrollbarhandler(EventPtr ev,
                 UInt16 scrbid,
                 UInt16 fldid)
{
	FormPtr frm=FrmGetActiveForm();

	switch (ev->eType) {
	case fldChangedEvent:
		if (ev->data.fldChanged.fieldID==fldid) {
			updatescrollbar(frm, scrbid, fldid);
			return true;
		}
		break;
	case sclRepeatEvent:
		if (ev->data.sclRepeat.scrollBarID==scrbid) {
			scrolllines(scrbid, fldid, ev->data.sclRepeat.newValue-ev->data.sclRepeat.value);
			return true;
		}
		break;
	default:
		break;
	}

	return false;
}

/*******************************************************************************
*
*	scrollbarkeyhandler
*
*	Handler for scrollbar key events.
*/
Boolean
scrollbarkeyhandler(EventPtr ev,
                 UInt16 scrbid,
                 UInt16 fldid)
{
	FormPtr frm=FrmGetActiveForm();
	Int16 focusidx;

	focusidx=FrmGetFocus(frm);
	if (focusidx==noFocus)
		return false;

	focusidx=FrmGetObjectId(frm, focusidx);
	if (focusidx==fldid) {
		switch (ev->data.keyDown.chr) {
		case pageUpChr:
			pagescroll(scrbid, fldid, winUp);
			return true;
		case pageDownChr:
			pagescroll(scrbid, fldid, winDown);
			return true;
			/* FALL-THROUGH */
		default:
			break;
		}
	}

	return false;
}

/*******************************************************************************
*
*	updatescrollbar
*
*	Update scrollbar based on field.
*/
void
updatescrollbar(FormPtr form,
                UInt16 scrbar,
                UInt16 field)
{
	UInt16 mv=0, curpos, txth, fldh;
	ScrollBarPtr scrp;
	FieldPtr fldp;

	scrp=objectinform(form, scrbar);
	fldp=objectinform(form, field);

	FldGetScrollValues(fldp, &curpos, &txth, &fldh);

	if (txth>fldh)
		mv=txth-fldh;
	else {
		if (curpos)
			mv=curpos;
	}

	SclSetScrollBar(scrp, curpos, 0, mv, fldh-1);
}

/*******************************************************************************
*
*	scrolllines
*
*	Scroll scrollbar number of lines.
*/
static void
scrolllines(UInt16 scrbid,
            UInt16 fldid,
            Int16 num)
{
	FormPtr frm=FrmGetActiveForm();
	FieldPtr fld;

	fld=objectinform(frm, fldid);
	if (num<0)
		FldScrollField(fld, -num, winUp);
	else
		FldScrollField(fld, num, winDown);
}

/*******************************************************************************
*
*	pagescroll
*
*	Scroll a page when scroll buttons are hit.
*/
static void
pagescroll(UInt16 scrbid,
           UInt16 fldid,
           WinDirectionType dir)
{
	FormPtr frm=FrmGetActiveForm();
	FieldPtr fld;
	Int16 lts;

	fld=objectinform(frm, fldid);
	if (FldScrollable(fld, dir)) {
		lts=FldGetVisibleLines(fld)-1;

		if (dir==winUp)
			lts=-lts;

		scrolllines(scrbid, fldid, lts);
		updatescrollbar(frm, scrbid, fldid);
	}
}

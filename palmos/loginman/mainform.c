#include "loginman.h"

/* globals */
static Int16 selecteditem=noListSelection;

/* protos */
static void drawlist(Int16, RectangleType *, Char **);
static Boolean evh(EventPtr);
static Boolean selectev(EventPtr, FormPtr);
static Boolean menuev(EventPtr, FormPtr);

/*******************************************************************************
*
*	formmaininit
*
*	Initializes the main form.
*/
void
formmaininit(FormPtr form)
{
	ListType *lp;
	UInt16 items;

	FrmSetEventHandler(form, (FormEventHandlerPtr)evh);

	items=DmNumRecords(qdb);
	lp=getformobject(form, mfrealms);
	LstSetListChoices(lp, NULL, items);
	LstSetSelection(lp, -1);
	LstSetDrawFunction(lp, drawlist);
}

/*******************************************************************************
*
*	drawlist
*
*	Draw realm list.
*/
static void
drawlist(Int16 item,
         RectangleType *rect,
         Char **data)
{
	UInt16 pw=rect->extent.x, len;
	MemHandle mh;
	UInt8 *recptr;
	Boolean trunc;

	mh=DmQueryRecord(qdb, item);
	if (mh) {
		recptr=MemHandleLock(mh);
		len=GETOFFSET(recptr);
		recptr+=6;

		FntCharsInWidth(recptr, &pw, &len, &trunc);

		WinDrawChars(recptr, len, rect->topLeft.x, rect->topLeft.y);
		MemHandleUnlock(mh);
	}
}

/*******************************************************************************
*
*	evh
*
*	Event handler.
*/
static Boolean
evh(EventPtr ev)
{
	FormPtr form;
	ListType *lp;

	form=FrmGetActiveForm();
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(form);
		lp=getformobject(form, mfrealms);
		if (selecteditem!=noListSelection)
			LstSetSelection(lp, selecteditem);
		return true;

	case keyDownEvent:
		switch (ev->data.keyDown.chr) {
		case vchrPageUp:
			lp=getformobject(form, mfrealms);
			LstScrollList(lp, winUp, 10);
			return true;
		case vchrPageDown:
			lp=getformobject(form, mfrealms);
			LstScrollList(lp, winDown, 10);
			return true;
		}
		break;

	case lstSelectEvent:
		lp=getformobject(form, mfrealms);
		selecteditem=LstGetSelection(lp);
		if (selecteditem!=noListSelection)
			preparetoview(selecteditem);

		return true;

	case ctlSelectEvent:
		return selectev(ev, form);

	case menuEvent:
		return menuev(ev, form);

	default:
		break;
	}

	return false;
}

/*******************************************************************************
*
*	selectev
*
*	Handle select events.
*/
static Boolean
selectev(EventPtr ev,
         FormPtr form)
{
	switch (ev->data.ctlSelect.controlID) {
	case mfnew:
		preparetoedit(ENEW, 0);
		return true;
	}

	return false;
}

/*******************************************************************************
*
*	menuev
*
*	Handle menu events.
*/
static Boolean
menuev(EventPtr ev,
       FormPtr form)
{
	FormPtr tfrm;

	switch (ev->data.menu.itemID) {
	case mffabout:
		tfrm=FrmInitForm(formabout);
		FrmSetActiveForm(tfrm);

		FrmDoDialog(tfrm);
		FrmSetActiveForm(form);
		FrmEraseForm(tfrm);
		FrmDeleteForm(tfrm);
		return true;
	default:
		break;
	}

	return false;
}

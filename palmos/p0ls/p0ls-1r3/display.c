#include "p0ls.h"

/* protos */
static Boolean evh(EventPtr);
static Boolean selev(EventPtr, FormPtr);

/*******************************************************************************
*
*	displayinit
*
*	Initializes the display form.
*
*	Input:
*		form - form.
*		id - form id.
*/
void
displayinit(FormPtr form,
            UInt16 formid)
{
	FrmSetEventHandler(form, (FormEventHandlerPtr)evh);
}

/*******************************************************************************
*
*	evh
*
*	Start form event handler.
*
*	Input:
*		ev - event.
*	Output:
*		handled: true.
*		not handled: false.
*/
static Boolean
evh(EventPtr ev)
{
	Boolean handled=false;
	FormPtr form;
	ControlPtr button;

	form=FrmGetActiveForm();
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(form);
		turtlerender();
		turtledone();
		button=objectinform(form, ddone);
		CtlShowControl(button);
		button=objectinform(form, dsave);
		CtlShowControl(button);
		return true;

	case ctlSelectEvent:
		handled=selev(ev, form);
		/* FALL-THROUGH */
	default:
		break;
	}

	return handled;
}

/*******************************************************************************
*
*	selev
*
*	Handle select events.
*
*	Input:
*		ev - event.
*		form - form.
*	Output:
*		handled: true.
*		not handled: false.
*/
static Boolean
selev(EventPtr ev,
      FormPtr form)
{
	switch (ev->data.ctlSelect.controlID) {
	case ddone:
		FrmGotoForm(formstart);
		return true;
	case dsave:
		ioform=SAVEFORM;
		FrmGotoForm(formio);
		return true;
	}

	return false;
}

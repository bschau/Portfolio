#include "sit.h"

/* globals */
static UInt16 passfunc;
static Char inpwd[PASSWORDLEN+2];

/* protos */
static Boolean evh(EventPtr);
static void selev(EventPtr, FormPtr);

/*******************************************************************************
*
*	init*
*
*	Initializers for the various password dialogs.
*
*	Input:
*		func - function (Single or Double).
*	Output:
*		form id.
*/
UInt16
inittextual(UInt16 func)
{
	passfunc=func;
	return formtextual;
}

/*******************************************************************************
*
*	textualinit
*
*	Initializes the textual form.
*
*	Input:
*		form - form.
*/
void
textualinit(FormPtr form)
{
	FrmSetEventHandler(form, (FormEventHandlerPtr)evh);
	FrmSetTitle(form, formtitle);
	FrmSetFocus(form, FrmGetObjectIndex(form, txtpwd1));
}

/*******************************************************************************
*
*	evh
*
*	Event handler.
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
	FormPtr form;

	form=FrmGetActiveForm();
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(form);
		return true;

	case ctlSelectEvent:
		selev(ev, form);
		return true;

	case menuEvent:
		return handleeditmenu(ev, form);

	default:
		break;
	}

	return false;
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
*/
static void
selev(EventPtr ev,
      FormPtr form)
{
	UInt16 ctl=ev->data.ctlSelect.controlID;
	FieldType *fld=findobject(txtpwd1);
	Char *key;
	Int16 ret;

	switch (ctl) {
	case txtok:
		key=FldGetTextPtr(fld);
		if (key) {
			MemMove(inpwd, key, FldGetTextLength(fld));

			if (passfunc==PASSSINGLE) {
				ret=singlepassword(inpwd);
				MemSet(inpwd, PASSWORDLEN, 0);
				if (ret) {	/* password error */
					FldSetSelection(fld, 0, FldGetTextLength(fld));
					FldCut(fld);
					FrmSetFocus(form, FrmGetObjectIndex(form, txtpwd1));
				} else
					gotostart();
			} else {
				changedbpwd(inpwd);
				MemSet(inpwd, PASSWORDLEN, 0);
				gotostart();
			}
		} else
			FrmCustomAlert(alertonechar, NULL, NULL, NULL);
		/* FALL-THROUGH */
	default:
		break;
	}
}

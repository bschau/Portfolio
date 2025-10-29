#include "sit.h"

/* globals */
static UInt16 passfunc, starflag;
static UInt8 inpwd[PASSWORDLEN+2];

/* protos */
static Boolean evh(EventPtr);
static void selev(EventPtr, FormPtr);
static void toggle(FormPtr);

/*******************************************************************************
*
*	init*
*
*	Initializers for the various password dialogs.
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
*/
void
textualinit(FormPtr form)
{
	FrmSetEventHandler(form, (FormEventHandlerPtr)evh);
	FrmSetTitle(form, formtitle);
	FrmSetFocus(form, FrmGetObjectIndex(form, txtpwd1));

	starflag=0;
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

	form=FrmGetActiveForm();
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(form);
		toggle(form);
		return true;

	case ctlSelectEvent:
		selev(ev, form);
		return true;

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
*/
static void
selev(EventPtr ev,
      FormPtr form)
{
	UInt16 ctl=ev->data.ctlSelect.controlID;
	FieldType *fld=objectinform(form, txtpwd1);
	UInt8 *key;
	Int16 ret;

	switch (ctl) {
	case txttoggle:
		toggle(form);
		break;
		
	case txtok:
		key=FldGetTextPtr(fld);
		ret=fieldblank(key);
		if (!ret) {
			MemMove(inpwd, key, FldGetTextLength(fld));

			if (passfunc==PASSSINGLE) {
				ret=singlepassword(inpwd);
				MemSet(inpwd, PASSWORDLEN, 0);
				if (ret) {	/* password error */
					FldSetSelection(fld, 0, FldGetTextLength(fld));
					FldCut(fld);
					FrmSetFocus(form, FrmGetObjectIndex(form, txtpwd1));
				} else
					loadentry();
			} else {
				changepassword(inpwd);
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

/*******************************************************************************
*
*	toggle
*
*	toggle field font.
*/
static void
toggle(FormPtr form)
{
	FieldType *fld=objectinform(form, txtpwd1);

	starflag=!starflag;
	if (!starflag)
		FldSetFont(fld, stdFont);
	else
		FldSetFont(fld, symbolFont);

	updatetoggle(starflag, TEXTUALTOGGLEX, TEXTUALTOGGLEY);
}

#include "p0ls.h"

/* protos */
static Boolean evh(EventPtr);
static Boolean selev(EventPtr, FormPtr);
static void validatefields(FormPtr);
static Int16 valfield(FormPtr, UInt16, UInt16, UInt16, UInt16 *, Char *);

/*******************************************************************************
*
*	startinit
*
*	Initializes the start form.
*
*	Input:
*		form - form.
*		id - form id.
*/
void
startinit(FormPtr form,
          UInt16 formid)
{
	char buffer[32];

	FrmSetEventHandler(form, (FormEventHandlerPtr)evh);

	StrPrintF(buffer, "%d", prefs.depth);
	setfieldtext(form, fsdepth, buffer);
	StrPrintF(buffer, "%d", prefs.angle);
	setfieldtext(form, fsangle, buffer);

	setfieldtext(form, fsaxiom, axiom);

	FrmSetFocus(form, FrmGetObjectIndex(form, fsaxiom));
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

	form=FrmGetActiveForm();
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(form);
		return true;

	case ctlSelectEvent:
		handled=selev(ev, form);
		break;

	case menuEvent:
		handled=globalmenu(ev, form);
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
	case fsrules:
		validatefields(form);
		return true;
	case fsload:
		ioform=LOADFORM;
		FrmGotoForm(formio);
		return true;
	}

	return false;
}

/*******************************************************************************
*
*	validatefields
*
*	Validate input fields.
*
*	Input:
*		form - form.
*/
static void
validatefields(FormPtr form)
{
	Int16 err=0;
	Char *fp;

	/* individual fields */
	err|=valfield(form, fsdepth, 1, 99, &prefs.depth, "Depth");
	err|=valfield(form, fsangle, 1, 360, &prefs.angle, "Angle");

	/* axiom */
	fp=FldGetTextPtr(objectinform(form, fsaxiom));
	if (!fp)
		err=1;
	else {
		StrNCopy(axiom, fp, AXIOMLEN);
		err=compileaxiom();
	}

	if (err) {
		FrmCustomAlert(alertaxiomerr, NULL, NULL, NULL);
		return;
	}

	FrmGotoForm(formprodrules);
}

/*******************************************************************************
*
*	valfield
*
*	Validate field.
*
*	Input:
*		form - form.
*		obj - object in form.
*		lower - lower bound.
*		upper - upper bound.
*		pref - prefitem.
*		txt - textual name.
*	Output:
*		s: 0.
*		f: !0.
*/
static Int16
valfield(FormPtr form,
         UInt16 obj,
         UInt16 lower,
         UInt16 upper,
         UInt16 *pref,
         Char *txt)
{
	Char *fp;
	UInt16 err;
	Int32 val;
	char lwr[12], hgr[12];

	fp=FldGetTextPtr(objectinform(form, obj));
	if (!fp)
		err=1;
	else {
		val=StrAToI(fp);
		if (val<lower || val>upper)
			err=1;
		else
			err=0;
	}

	if (err) {
		StrPrintF(lwr, "%d", lower);
		StrPrintF(hgr, "%d", upper);
		FrmCustomAlert(alertvalues, lwr, txt, hgr);
		return 1;
	}

	*pref=val;
	return 0;
}

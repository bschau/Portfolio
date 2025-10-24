#include "p0ls.h"

/* protos */
static Boolean evh(EventPtr);
static Boolean selev(EventPtr, FormPtr);
static void reload(FormPtr);

/* globals */
static UInt16 fields[]={ pruf, prlf, prr0, prr1, prr2, prr3, prr4, prr5, prr6, prr7, prr8, prr9, 0 };

/*******************************************************************************
*
*	prodrulesinit
*
*	Initializes the production rules form.
*
*	Input:
*		form - form.
*		id - form id.
*/
void
prodrulesinit(FormPtr form,
              UInt16 formid)
{
	FrmSetEventHandler(form, (FormEventHandlerPtr)evh);
	reload(form);
}

/*******************************************************************************
*
*	evh
*
*	Form event handler.
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
	UInt16 idx=0;
	Int16 err;
	FieldType *f;

	getfieldtext(form, pruf, ruleuf, PRODRULELEN);
	getfieldtext(form, prlf, rulelf, PRODRULELEN);
	getfieldtext(form, prr0, rule0, PRODRULELEN);
	getfieldtext(form, prr1, rule1, PRODRULELEN);
	getfieldtext(form, prr2, rule2, PRODRULELEN);
	getfieldtext(form, prr3, rule3, PRODRULELEN);
	getfieldtext(form, prr4, rule4, PRODRULELEN);
	getfieldtext(form, prr5, rule5, PRODRULELEN);
	getfieldtext(form, prr6, rule6, PRODRULELEN);
	getfieldtext(form, prr7, rule7, PRODRULELEN);
	getfieldtext(form, prr8, rule8, PRODRULELEN);
	getfieldtext(form, prr9, rule9, PRODRULELEN);

	switch (ev->data.ctlSelect.controlID) {
	case praxiom:
		FrmGotoForm(formstart);
		return true;
	case prgenerate:
		err=validaterules();
		if (!err)
			FrmGotoForm(formworking);

		return true;
	case prclear:
		*ruleuf='\x00';
		*rulelf='\x00';
		*rule0='\x00';
		*rule1='\x00';
		*rule2='\x00';
		*rule3='\x00';
		*rule4='\x00';
		*rule5='\x00';
		*rule6='\x00';
		*rule7='\x00';
		*rule8='\x00';
		*rule9='\x00';

		reload(form);
		for (; fields[idx]; idx++) {
			f=objectinform(form, fields[idx]);
			FldDrawField(f);
		}
		
		return true;
	}

	return false;
}

/*******************************************************************************
*
*	reload
*
*	Reload fields and sets focus.
*
*	Input:
*		form - form.
*/
static void
reload(FormPtr form)
{
	UInt16 obj=pruf;

	resetdefaultrules();

	setfieldtext(form, pruf, ruleuf);
	setfieldtext(form, prlf, rulelf);
	setfieldtext(form, prr0, rule0);
	setfieldtext(form, prr1, rule1);
	setfieldtext(form, prr2, rule2);
	setfieldtext(form, prr3, rule3);
	setfieldtext(form, prr4, rule4);
	setfieldtext(form, prr5, rule5);
	setfieldtext(form, prr6, rule6);
	setfieldtext(form, prr7, rule7);
	setfieldtext(form, prr8, rule8);
	setfieldtext(form, prr9, rule9);
	
	if ((*ax<=TFORWARD) && (*ax>=TRULE0))
		obj=(*ax)-TRULE0+prr0;

	FrmSetFocus(form, FrmGetObjectIndex(form, obj));
}

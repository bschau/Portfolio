#include "loginman.h"

/* globals */
static UInt16 vidx;
static UInt8 realm[QALMFIELDLEN+2], login[QALMFIELDLEN+2], password[QALMFIELDLEN+2];

/* protos */
static Boolean evh(EventPtr);
static Boolean selectev(EventPtr, FormPtr);

/*******************************************************************************
*
*	preparetoview
*
*	Set up edit/new dialog.
*/
void
preparetoview(UInt16 idx)
{
	vidx=idx;

	FrmGotoForm(formview);
}

/*******************************************************************************
*
*	formviewinit
*
*	Initializes the viewentry form.
*/
void
formviewinit(FormPtr form)
{
	UInt8 *errstr="Cannot get entry";
	realmrecord rr;
	MemHandle mh;

	MemSet(realm, QALMFIELDLEN+2, 0);
	MemSet(login, QALMFIELDLEN+2, 0);
	MemSet(password, QALMFIELDLEN+2, 0);

	mh=clonerecord(qdb, vidx);
	if (mh) {
		qunpack(&rr, MemHandleLock(mh));

		MemMove(realm, rr.realm, StrLen(rr.realm));
		if (rr.login)
			MemMove(login, rr.login, StrLen(rr.login));
		else
			*login=' ';

		if (rr.password)
			MemMove(password, rr.password, StrLen(rr.password));
		else
			*password=' ';

		MemHandleFree(mh);
	} else {
		StrCat(realm, errstr);
		StrCat(login, errstr);
		StrCat(password, errstr);
		FrmHideObject(form, FrmGetObjectIndex(form, vfedit));
		FrmHideObject(form, FrmGetObjectIndex(form, vfdelete));
	}

	setfieldtext(form, vfrealm, realm);
	setfieldtext(form, vflogin, login);
	setfieldtext(form, vfpassword, password);

	FrmSetEventHandler(form, (FormEventHandlerPtr)evh);
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
	FormPtr form=FrmGetActiveForm();

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(form);
		return true;
	case ctlSelectEvent:
		return selectev(ev, form);
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
	Int16 button;

	switch (ev->data.ctlSelect.controlID) {
	case vfok:
		FrmGotoForm(formmain);
		return true;
	case vfedit:
		preparetoedit(EEDIT, vidx);
		return true;
	case vfdelete:
		button=FrmCustomAlert(alertdelete, NULL, NULL, NULL);
		if (button==1)	/* no, don't delete */
			return true;

		DmRemoveRecord(qdb, vidx);
		FrmGotoForm(formmain);
		return true;
	default:
		break;
	}

	return false;
}

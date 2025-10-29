#include "loginman.h"

/* globals */
static UInt8 *edittitle="Edit entry", *newtitle="New entry";
static UInt16 eemode, eeidx;
static refocustab rfprev[]={
	{ efrealm, efpassword },
	{ eflogin, efrealm },
	{ efpassword, eflogin },
	{ 0, 0 }
};
static refocustab rfnext[]={
	{ efpassword, efrealm },
	{ eflogin, efpassword },
	{ efrealm, eflogin },
	{ 0, 0 }
};

/* protos */
static Boolean evh(EventPtr);
static Boolean selectev(EventPtr, FormPtr);
static void update(FormPtr, Boolean);

/*******************************************************************************
*
*	preparetoedit
*
*	Set up edit/new dialog.
*/
void
preparetoedit(UInt16 mode,
              UInt16 idx)
{
	eemode=mode;
	if (mode==ENEW)
		eeidx=dmMaxRecordIndex;
	else
		eeidx=idx;

	FrmGotoForm(formedit);
}

/*******************************************************************************
*
*	formeditinit
*
*	Initializes the editentry form.
*
*	Input:
*		form - form.
*/
void
formeditinit(FormPtr form)
{
	MemHandle mh;
	realmrecord rr;

	FrmSetEventHandler(form, (FormEventHandlerPtr)evh);
	if (eemode==EEDIT) {
		FrmSetTitle(form, edittitle);
		mh=clonerecord(qdb, eeidx);
		if (mh) {
			qunpack(&rr, MemHandleLock(mh));
			setfieldhandle(form, efrealm, rr.realm);
			setfieldhandle(form, eflogin, rr.login);
			setfieldhandle(form, efpassword, rr.password);
			MemHandleFree(mh);
		}
	} else
		FrmSetTitle(form, newtitle);

	FrmSetFocus(form, FrmGetObjectIndex(form, efrealm));
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
	case keyDownEvent:
		if (ev->data.keyDown.chr==prevFieldChr)
			return refocus(form, &rfprev[0]);

		if (ev->data.keyDown.chr==nextFieldChr)
			return refocus(form, &rfnext[0]);
		break;
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
	UInt8 *realm;

	switch (ev->data.ctlSelect.controlID) {
	case efok:
		realm=FldGetTextPtr(getformobject(form, efrealm));
		if (isfieldblank(realm)==true)
			FrmCustomAlert(alertrealmempty, NULL, NULL, NULL);
		else
			update(form, true);

		return true;
	case efcancel:
		update(form, false);
		return true;
	default:
		break;
	}

	return false;
}

/*******************************************************************************
*
*	update
*
*	Updates record (if necessary).  Cleanup.  Go to main form.
*/
static void
update(FormPtr form,
       Boolean recflag)
{
	realmpacked rp;
	UInt8 *realm, *login, *password;

	if (recflag==true) {
		realm=FldGetTextPtr(getformobject(form, efrealm));
		login=FldGetTextPtr(getformobject(form, eflogin));
		password=FldGetTextPtr(getformobject(form, efpassword));

		qpack(&rp, realm, login, password);
		if (eeidx==dmMaxRecordIndex) {
			/* it might be new, it might be there already */
			eeidx=findrecord(qdb, rp.mh);
		}
		storerecord(qdb, &rp, eeidx);
		MemHandleFree(rp.mh);
	}

	FrmGotoForm(formmain);
}

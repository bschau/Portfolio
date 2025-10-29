#include "sit.h"

/* globals */
static MemHandle mh;
static recdesc rd;
static UInt16 recidx;

/* protos */
static Boolean srevh(EventPtr);
static Boolean srselev(EventPtr, FormPtr);
static Int16 deleterecord(FormPtr);
static void showtitle(void);
static Boolean stevh(EventPtr);

/*******************************************************************************
*
*	doneshow
*
*	Deallocate.
*/
void
doneshow(void)
{
	if (mh) {
		MemHandleFree(mh);
		mh=NULL;
	}
}

/*******************************************************************************
*
*	showrecord
*
*	Prepare to show record.
*/
void
showrecord(UInt16 item)
{
	MemHandle rech;
	UInt32 rechsize;
	UInt8 *mp;

	recidx=item;

	rech=DmQueryRecord(sitdb, recidx);
	if (!rech) {
		FrmCustomAlert(alertqueryfailed, NULL, NULL, NULL);
		return;
	}

	rechsize=MemHandleSize(rech);
	mh=MemHandleNew(rechsize);
	if (!mh) {
		FrmCustomAlert(alertoom, NULL, NULL, NULL);
		return;
	}

	mp=MemHandleLock(mh);
	MemMove(mp, MemHandleLock(rech), rechsize);
	MemHandleUnlock(rech);

	recorddesc(&rd, mp);
	decrypt(rd.secret, rd.secretlen);

	if (prefs.flags&PREFQUICKACCESS)
		FrmGotoForm(formshowentry);
	else
		FrmGotoForm(formshowentrynqa);
}

/*******************************************************************************
*
*	showentryinit
*
*	Initializes the showentry form.
*/
void
showentryinit(FormPtr form)
{
	FieldType *fld;

	StrNCopy(formtitle, rd.title, FORMTITLELEN);
	FrmSetTitle(form, formtitle);

	fld=objectinform(form, seffield);
	FldSetTextPtr(fld, rd.secret);
	FldRecalculateField(fld, true);
	FrmSetFocus(form, FrmGetObjectIndex(form, seffield));
	FrmSetEventHandler(form, (FormEventHandlerPtr)srevh);

	updatescrollbar(form, seffieldscrb, seffield);
}

/*******************************************************************************
* 
*	srevh
*
*	Event handler for Show Record form.
*/
static Boolean
srevh(EventPtr ev)
{
	FormPtr form=FrmGetActiveForm();
	Boolean f;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(form);
		return true;
	case ctlSelectEvent:
		return srselev(ev, form);
	default:
		f=scrollbarkeyhandler(ev, seffieldscrb, seffield);
		if (f==true)
			return true;

		scrollbarhandler(ev, seffieldscrb, seffield);
		break;
	}

	return false;
}

/*******************************************************************************
* 
*	srselev
*
*	Handle select events.
*/
static Boolean
srselev(EventPtr ev,
        FormPtr form)
{
	Int16 t;

	switch (ev->data.ctlSelect.controlID) {
	case sefok:
		doneshow();
		gotostart();
		return true;
	case seftitle:
		showtitle();
		FrmSetActiveForm(form);
		FrmDrawForm(form);
		return true;
	case sefedit:
		doneshow();
		preparetoedit(recidx, NULL);
		return true;
	case sefdelete:
		t=deleterecord(form);
		if (t) {
			doneshow();
			gotostart();
		} else {
			FrmSetActiveForm(form);
			FrmDrawForm(form);
		}
		return true;
	default:
		break;
	}

	return false;
}

/*******************************************************************************
* 
*	deleterecord
*
*	Delete the selected record.	Asks for confirmation.
*/
static Int16
deleterecord(FormPtr pfrm)
{
	Int16 t=deletedialog(pfrm);

	if (t)
		DmRemoveRecord(sitdb, recidx);

	return t;
}

/*******************************************************************************
*
*	showtitle
*
*	Show title in seperate form.
*/
static void
showtitle(void)
{
	FormPtr tfrm;
	FieldType *fld;

	if (prefs.flags&PREFQUICKACCESS)
		tfrm=FrmInitForm(formshowtitle);
	else
		tfrm=FrmInitForm(formshowtitlenqa);

	fld=objectinform(tfrm, shtfield);
	FldSetTextPtr(fld, rd.title);
	FldRecalculateField(fld, true);
	FrmSetFocus(tfrm, FrmGetObjectIndex(tfrm, shtfield));

	FrmSetEventHandler(tfrm, (FormEventHandlerPtr)stevh);

	updatescrollbar(tfrm, shtfieldscrb, shtfield);
	FrmSetActiveForm(tfrm);
	FrmDoDialog(tfrm);
	FrmEraseForm(tfrm);
	FrmDeleteForm(tfrm);
}

/*******************************************************************************
* 
*	stevh
*
*	Event handler for Show Title form.
*/
static Boolean
stevh(EventPtr ev)
{
	scrollbarhandler(ev, shtfieldscrb, shtfield);
	return false;
}

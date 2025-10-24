#include "sit.h"

/* protos */
static UInt16 getlistpopup(FormPtr, UInt16);
static void setlistpopup(FormPtr, UInt16, UInt16, UInt16);
static void setflag(FormPtr, UInt16, UInt32);
static void setcheckbox(FormPtr, UInt16, UInt32);
static void protectdb(DmOpenRef, UInt8 *);

/*******************************************************************************
*
*	getlistpopup
*
*	Get values from list.
*/
static UInt16
getlistpopup(FormPtr form,
             UInt16 obj)
{
	ListType *list=objectinform(form, obj);

	return LstGetSelection(list);
}

/*******************************************************************************
*
*	setlistpopup
*
*	Set values of list and popup from prefs.
*/
static void
setlistpopup(FormPtr form,
             UInt16 listobj,
             UInt16 popupobj,
             UInt16 value)
{
	ListType *list=objectinform(form, listobj);
	ControlPtr popup=objectinform(form, popupobj);
	UInt8 *p;

	LstSetSelection(list, value);
	p=LstGetSelectionText(list, value);
	CtlSetLabel(popup, p);
}

/*******************************************************************************
*
*	setflag
*
*	Set value of prefs.flags depending on checkbox.
*/
static void
setflag(FormPtr form,
        UInt16 obj,
        UInt32 mask)
{
	ControlType *chk=objectinform(form, obj);

	if (CtlGetValue(chk)==true)
		prefs.flags|=mask;
	else
		prefs.flags&=~mask;
}

/*******************************************************************************
*
*	setcheckbox
*
*	Sets value of checkbox depending on prefs.flags.
*/
static void
setcheckbox(FormPtr form,
            UInt16 obj,
            UInt32 mask)
{
	ControlType *chk=objectinform(form, obj);

	if (prefs.flags&mask)
		CtlSetValue(chk, true);
	else
		CtlSetValue(chk, false);
}

/*******************************************************************************
*
*	visual
*
*	Show visual dialog.
*/
void
visual(FormPtr parform)
{
	UInt32 oldquickaccess=prefs.flags&PREFQUICKACCESS;
	FormPtr tfrm;
	UInt16 button;

	tfrm=FrmInitForm(formvisual);
	FrmSetActiveForm(tfrm);

	setcheckbox(tfrm, vquickchk, PREFQUICKACCESS);
	setcheckbox(tfrm, vconfirmchk, PREFCONFIRMDELETE);
	setcheckbox(tfrm, vpagescroll, PREFPAGESCROLL);

	setlistpopup(tfrm, vpinlist, vpinpopup, prefs.pinidx);

	button=FrmDoDialog(tfrm);
	switch (button) {
	case vok:
		setflag(tfrm, vquickchk, PREFQUICKACCESS);
		setflag(tfrm, vconfirmchk, PREFCONFIRMDELETE);
		setflag(tfrm, vpagescroll, PREFPAGESCROLL);
		prefs.pinidx=getlistpopup(tfrm, vpinlist);
		break;
	case vcancel:
		/* FALL-THRU */
	default:
		break;
	}

	FrmEraseForm(tfrm);
	FrmDeleteForm(tfrm);
	if ((prefs.flags&PREFQUICKACCESS)==oldquickaccess)
		FrmSetActiveForm(parform);
	else {
		FrmCloseAllForms();
		gotostart();
	}
}

/*******************************************************************************
*
*	security
*
*	Show visuals dialog.
*/
void
security(FormPtr parform)
{
	FormPtr tfrm;
	UInt16 button;

	tfrm=FrmInitForm(formsecurity);
	FrmSetActiveForm(tfrm);

	setcheckbox(tfrm, secprotect, PREFPROTECTDB);
	setcheckbox(tfrm, seclockdev, PREFLOCKDEVICE);
	setcheckbox(tfrm, seclogoutpd, PREFLOGOUTPD);

	setlistpopup(tfrm, secalalist, secalapopup, prefs.alaidx);
	setlistpopup(tfrm, seckaboomlist, seckaboompopup, prefs.kaboomidx);

	button=FrmDoDialog(tfrm);
	switch (button) {
	case secok:
		setflag(tfrm, secprotect, PREFPROTECTDB);
		setflag(tfrm, seclockdev, PREFLOCKDEVICE);
		setflag(tfrm, seclogoutpd, PREFLOGOUTPD);

		prefs.alaidx=getlistpopup(tfrm, secalalist);
		prefs.kaboomidx=getlistpopup(tfrm, seckaboomlist);
		rethinkpreferences();
		setdbprotect();
		break;
	case seccancel:
		/* FALL-THRU */
	default:
		break;
	}

	FrmSetActiveForm(parform);
}

/*******************************************************************************
*
*	rethinkpreferences
*
*	Recalculate kaboom and timeout factors.
*/
void
rethinkpreferences(void)
{
	UInt32 pref;

	switch (prefs.kaboomidx) {
	case KABOOMNEVER:
		kaboom=0;
		break;
	case KABOOM1:
		kaboom=1;
		break;
	case KABOOM3:
		kaboom=3;
		break;
	case KABOOM5:
		kaboom=5;
		break;
	}

	switch (prefs.alaidx) {
	case LOGOUT30:
		autooff=30;
		break;
	case LOGOUT1:
		autooff=60;
		break;
	case LOGOUT2:
		autooff=120;
		break;
	case LOGOUT3:
		autooff=180;
		break;
	case LOGOUTPREFS:
		pref=PrefGetPreference(prefVersion);
		if (pref) {		/* shouldn't fail ... */
			if (pref<preferenceDataVer8) {
				pref=PrefGetPreference(prefAutoOffDuration);
				if (!pref)
					pref=PrefGetPreference(defaultAutoOffDuration);
				if (!pref)
					pref=3;

				autooff=(UInt16)(pref*minutesInSeconds);
			} else {
				autooff=(UInt16)PrefGetPreference(prefAutoOffDurationSecs);
				if (!autooff)
					autooff=(UInt16)PrefGetPreference(defaultAutoOffDurationSecs);
				if (!autooff)
					autooff=(UInt16)(PrefGetPreference(prefAutoOffDuration)*minutesInSeconds);
				if (!autooff)
					autooff=(UInt16)(PrefGetPreference(defaultAutoOffDuration)*minutesInSeconds);
				if (!autooff)
					autooff=180;
			}
		} else
			autooff=180;

		break;
	}
}

/*******************************************************************************
*
*	setdbprotect
*
*	Set or clear database protection bit.
*
*	This function gets it's information from prefs.protectdb.
*/
void
setdbprotect(void)
{
	protectdb(sitdb, SECRETSDATABASENAME);
	protectdb(tmpldb, TEMPLATESDATABASENAME);
}

/*******************************************************************************
*
*	protectdb
*
*	Piggyback for setdbprotect.
*/
static void
protectdb(DmOpenRef db,
          UInt8 *dbname)
{

	UInt16 dbattr, card;
	LocalID dbid;
	Err err;

	err=DmOpenDatabaseInfo(db, &dbid, NULL, NULL, &card, NULL);
	if (err==errNone) {
		err=DmDatabaseInfo(card, dbid, NULL, &dbattr, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		if (err==errNone) {
			if (prefs.flags&PREFPROTECTDB)
				dbattr&=~dmHdrAttrBackup;
			else
				dbattr|=dmHdrAttrBackup;

			err=DmSetDatabaseInfo(card, dbid, NULL, &dbattr, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
			if (err==errNone)
				return;
		}
	}

	FrmCustomAlert(alertsetdbinfo, dbname, NULL, NULL);
}

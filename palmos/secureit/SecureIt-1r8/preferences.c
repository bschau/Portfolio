#include "sit.h"

/*******************************************************************************
 *
 *	visual
 *
 *	Show visual dialog.
 *
 * 	Input:
 * 		parform - parent form.
 */
void
visual(FormPtr parform)
{
	Char qaf=prefs.quickaccess, *p;
	FormPtr tfrm;
	UInt16 button;
	ControlPtr qachk, cfchk, popup;
	ListType *pinlst;

	tfrm=FrmInitForm(formvisual);
	FrmSetActiveForm(tfrm);

	qachk=objectinform(tfrm, vquickchk);
	if (prefs.quickaccess)
		CtlSetValue(qachk, true);
	else
		CtlSetValue(qachk, false);

	cfchk=objectinform(tfrm, vconfirmchk);
	if (prefs.confirmdelete)
		CtlSetValue(cfchk, true);
	else
		CtlSetValue(cfchk, false);

	pinlst=objectinform(tfrm, vpinlist);
	LstSetSelection(pinlst, prefs.pinidx);
	p=LstGetSelectionText(pinlst, prefs.pinidx);
	popup=objectinform(tfrm, vpinpopup);
	CtlSetLabel(popup, p);

	button=FrmDoDialog(tfrm);
	switch (button) {
		case vdone:
			prefs.quickaccess=CtlGetValue(qachk);
			prefs.confirmdelete=CtlGetValue(cfchk);
			prefs.pinidx=LstGetSelection(pinlst);
			break;
		case vcancel:
			/* FALL-THRU */
		default:
			break;
	}

	if (qaf==prefs.quickaccess)
		FrmSetActiveForm(parform);
	else {
		FrmDeleteForm(tfrm);
		FrmCloseAllForms();
		gotostart();
	}
}

/*******************************************************************************
 *
 *	security
 *
 *	Show visuals dialog.
 *
 * 	Input:
 * 		parform - parent form.
 */
void
security(FormPtr parform)
{
	ControlPtr protchk;
	Char *p;
	FormPtr tfrm;
	UInt16 button;
	ControlPtr popup;
	ListType *alalst, *kaboomlst;

	tfrm=FrmInitForm(formsecurity);
	FrmSetActiveForm(tfrm);

	protchk=objectinform(tfrm, secprotect);
	if (prefs.protectdb)
		CtlSetValue(protchk, true);
	else
		CtlSetValue(protchk, false);

	alalst=objectinform(tfrm, secalalist);
	LstSetSelection(alalst, prefs.alaidx);
	p=LstGetSelectionText(alalst, prefs.alaidx);
	popup=objectinform(tfrm, secalapopup);
	CtlSetLabel(popup, p);

	kaboomlst=objectinform(tfrm, seckaboomlist);
	LstSetSelection(kaboomlst, prefs.kaboomidx);
	p=LstGetSelectionText(kaboomlst, prefs.kaboomidx);
	popup=objectinform(tfrm, seckaboompopup);
	CtlSetLabel(popup, p);

	button=FrmDoDialog(tfrm);
	switch (button) {
		case secdone:
			prefs.protectdb=CtlGetValue(protchk);
			prefs.alaidx=LstGetSelection(alalst);
			prefs.kaboomidx=LstGetSelection(kaboomlst);
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
		default:
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
		default:
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
	UInt16 dbver=DATABASEVERSION, dbattr, card;
	LocalID dbid;
	Err err;


	err=DmOpenDatabaseInfo(sitdb, &dbid, NULL, NULL, &card, NULL);
	if (err==errNone) {
		err=DmDatabaseInfo(card, dbid, NULL, &dbattr, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		if (err==errNone) {
			if (prefs.protectdb==true)
				dbattr&=~dmHdrAttrBackup;
			else
				dbattr|=dmHdrAttrBackup;

			err=DmSetDatabaseInfo(card, dbid, NULL, &dbattr, &dbver, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
			if (err==errNone)
				return;
		}
	}

	FrmCustomAlert(alertsetdbinfo, NULL, NULL, NULL);
}

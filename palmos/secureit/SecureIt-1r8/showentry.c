#include "sit.h"

/* protos */
static void deleterecord(FormPtr, UInt16);

/*******************************************************************************
 *
 *	showrecord
 *
 *	Show the selected record.
 *
 *	Input:
 *		rn - record number.
 *		rc - record category.
 *		parform - parent form.
 *	Output:
 *		0 - normal return (user pressed "done" or error).
 *		1 - edit return.
 *		2 - delete return.
 *	Note:
 *		Returncode 1 + 2 means the list must be redrawn!
 */
int
showrecord(Int16 rn,
           UInt16 rc,
           FormPtr parform)
{
	UInt16 item=0;
	FormPtr tfrm;
	MemHandle mh;
	char bs[BSSIZE];
	Err err;
	char *k;
	UInt16 button;
	int retval;
	
	if (rn==noListSelection)
		return 0;

	err=DmSeekRecordInCategory(sitdb, &item, rn, dmSeekForward, rc);
	if (err!=errNone)
		return 0;

	editindex=item;
	mh=DmQueryRecord(sitdb, item);
	if (mh) {
		k=MemHandleLock(mh);
		MemMove(bs, (char *)k, BSSIZE);
		MemHandleUnlock(mh);
	} else {
		FrmCustomAlert(alertoom, NULL, NULL, NULL);
		return 0;
	}

	recunpack(&trec, bs);

	if (prefs.quickaccess)
		tfrm=FrmInitForm(formshowentry);
	else
		tfrm=FrmInitForm(formshowentrynqa);

	FrmSetTitle(tfrm, trec.key);
	FrmSetActiveForm(tfrm);

	retval=setfieldtext(tfrm, seffield, trec.txt);
	if (retval)
		return 0;

	button=FrmDoDialog(tfrm);
	switch (button) {
		case sefedit:
			retval=1;
			break;
		case sefdelete:
			deleterecord(tfrm, item);
			retval=2;
			break;
		default:
			retval=0;
			break;
	}

	FrmSetActiveForm(parform);
	FrmDeleteForm(tfrm);
	return retval;
}

/*******************************************************************************
 * 
 *	deleterecord
 *
 *	Delete the selected record.	Asks for confirmation.
 *
 *	Input:
 *		pfrm - parent form.
 *		ri - record index.
 */
static void
deleterecord(FormPtr pfrm,
             UInt16 ri)
{
	FormPtr tfrm;
	UInt16 button;
	ControlPtr chkbox;

	if (prefs.confirmdelete) {
		tfrm=FrmInitForm(formdeleteentry);
		FrmSetActiveForm(tfrm);

		chkbox=objectinform(tfrm, defcheckbox);
		CtlSetValue(chkbox, true);

		button=FrmDoDialog(tfrm);
		prefs.confirmdelete=CtlGetValue(chkbox);

		FrmSetActiveForm(pfrm);
		FrmDeleteForm(tfrm);

		if (button==defno)
			return;
	}

	DmRemoveRecord(sitdb, ri);
}

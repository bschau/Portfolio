#include "occman.h"

/* protos */
static Boolean SetValuesFromOCC(void);
static Boolean EditFormEventHandler(EventPtr ev);
static Boolean UpdateOCC(UInt16, Char *, Char *);
static void TmpName(Char *);

/* globals */
static Int16 occIndex;
static UInt16 occStandalone, occCard;
static LocalID occId;
static Boolean didChange;
static Char *occName, *occUrl;
static MemHandle occUrlH;

/*
**	SetValuesFromOCC
**
**	Update variables with values from OCC.
**
**	Returns true if succesful, false otherwise.
*/
static Boolean
SetValuesFromOCC(void)
{
	Int16 item=occIndex;
	OCCNode *n;
	Boolean err;

	for (n=occList.head; n->next && item>0; n=n->next)
		item--;

	if (!n->next)
		return false;

	if ((err=OCCGetByName(n->name, &occCard, &occId))==false)
		return false;

	occName=n->name;

	if ((occUrlH=OCCGetUrl(occCard, occId))==NULL)
		return false;

	if ((err=OCCIsStandalone(occCard, occId, &occStandalone))==false)
		return false;

	return true;
}

/*
**	EditOCC
**
**	Edit or create a new OCC.
**
**	 -> occIdx	Index of OCC to edit. If -1, create a new.
**
**	Returns true if anything changed, false otherwise.
*/
Boolean
EditOCC(Int16 occIdx)
{
	FormPtr oFrm=currentForm, eFrm;
	Boolean err;

	didChange=false;
	occIndex=occIdx;
	occName=NULL;
	occUrl=NULL;
	occUrlH=NULL;

	if (occIndex!=NEWOCC) {
		if ((err=SetValuesFromOCC())==false) {
			FrmCustomAlert(aCannotGet, NULL, NULL, NULL);
			return didChange;
		}
	}

	eFrm=FrmInitForm(fEdit);
	ErrFatalDisplayIf(eFrm==NULL, "(EditOCC) Cannot initialize Edit form.");
	currentForm=eFrm;

	FrmSetActiveForm(currentForm);
	FrmSetEventHandler(currentForm, (FormEventHandlerPtr)EditFormEventHandler);
	FrmDrawForm(currentForm);

	if (occIndex!=NEWOCC) {
		UISetFieldText(fEditName, occName);
		occUrl=MemHandleLock(occUrlH);
		UISetFieldText(fEditUrl, occUrl);
		CtlSetValue(UIGetFormObject(fEditStandalone), (occStandalone==OCCTypePiggyBack) ? 0 : 1);
	}

	UIUpdateScrollbar(fEditScrollbar, fEditUrl);
	FrmSetFocus(currentForm, FrmGetObjectIndex(currentForm, fEditName));
	FrmDoDialog(currentForm);

	FrmEraseForm(eFrm);
	FrmDeleteForm(eFrm);
	currentForm=oFrm;
	FrmSetActiveForm(currentForm);

	if (occUrlH)
		MemHandleFree(occUrlH);

	return didChange;
}

/*
**	EditFormEventHandler
**
**	Event handler for Edit form.
**
**	 -> ev		Event to handle.
**
**	Returns true if event is handled, false otherwise.
*/
static Boolean
EditFormEventHandler(EventPtr ev)
{
	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case fEditOK:
			if ((didChange=UpdateOCC(CtlGetValue(UIGetFormObject(fEditStandalone)), UIGetFieldText(fEditName), UIGetFieldText(fEditUrl)))==false)
				break;

//			if (pref.flags.wantLaunch==1)
//				LaunchIt(UIGetFieldText(fEditUrl));
			
			// FIXME: insert OCC etc. etc.
			// if (pref.flags.wantLaunch==1) LaunchIt();
		case fEditCancel: // FALL-THRU
			return false;
		}
		break;

	default:
		if (UIScrollbarKeyHandler(ev, fEditScrollbar, fEditUrl)==true)
			return true;

		UIScrollbarHandler(ev, fEditScrollbar, fEditUrl);
		/* do not return true here as it disables sclRepeat */
		break;
	}
	return false;
}

/*
**	UpdateOCC
**
**	Update or create a new OCC.
**
**	 -> sa		Standalone flag.
**	 -> name	Name of OCC.
**	 -> url		Url to set in OCC.
**
**	Returns true if anything changed, false otherwise.
*/
static Boolean
UpdateOCC(UInt16 sa, Char *name, Char *url)
{
	Char oldName[dmDBNameLength], newName[dmDBNameLength];
	UInt16 card;
	LocalID id;
	Boolean err;

	if (name==NULL || url==NULL) {
		FrmCustomAlert(aMissingField, NULL, NULL, NULL);
		return false;
	}

	if (occIndex!=NEWOCC) {
		if (sa==occStandalone && StrCompare(occName, name)==0 && StrCompare(occUrl, url)==0)
			return false;	/* nothing changed */

		TmpName(newName);
		err=DmDatabaseInfo(occCard, occId, oldName, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		if ((err|=DmSetDatabaseInfo(occCard, occId, newName, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL))!=errNone) {
			FrmCustomAlert(aCannotBackup, NULL, NULL, NULL);
			return false;
		}

		if (PrcBuild(name, url, sa)==false) {
			FrmCustomAlert(aCannotCreateOCC, NULL, NULL, NULL);
			if ((err=DmSetDatabaseInfo(occCard, occId, oldName, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL))!=errNone)
				FrmCustomAlert(aCannotRollback, NULL, NULL, NULL);
			return false;
		}

		if ((err=DmDeleteDatabase(occCard, occId))!=errNone)
			FrmCustomAlert(aCannotDeleteOCC, NULL, NULL, NULL);
	} else {
		if (OCCGetByName(name, &card, &id)==true) {
			FrmCustomAlert(aOCCAlreadyExist, NULL, NULL, NULL);
			return false;
		}
		
		if ((err=PrcBuild(name, url, sa))==false) {
			FrmCustomAlert(aCannotCreateOCC, NULL, NULL, NULL);
			return false;
		}
	}

	OCCListDestroy(&occList);
	OCCDiscoverAll(&occList);
	return true;
}

/*
**	TmpName
**
**	Get a new "random" database name.
**
**	 -> dst		Where to store new name.
*/
static void
TmpName(Char *dst)
{
	UInt32 now=TimGetSeconds();
	UInt16 card;
	LocalID id;

	for (EVER) {
		StrPrintF(dst, "tmp%lu", now);
		if (OCCGetByName(dst, &card, &id)==false)
			break;

		now+=SysRandom(now);
	}
}

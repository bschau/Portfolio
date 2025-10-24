/**
 * A simple pin board layout (0-9)
 */
#include <PalmOS.h>
#include "PublicApi.h"
#include "Resources.h"

/* macros */
#define LibraryVersion 1
#define exitKeyboardEvent firstUserEvent

/* structs */
typedef struct {
	Int16 refCount;
} KeyboardGlobals;

/* structs */
typedef struct {
	UInt16 flags;
} KeyboardPrefs;
#define KPFCalculator 1
#define KPFMask 2

/* protos */
static KeyboardGlobals *LockGlobals(UInt16);
static void *ObjectGet(UInt16);
static Boolean EH(EventType *);
static void GetPrefs(KeyboardPrefs *);
static void SetPrefs(KeyboardPrefs *);
static void UpdateKeys(KeyboardPrefs *);
static void UpdateMask(KeyboardPrefs *);

/**
 * start
 */
Err
LibMain(UInt16 refNum, SysLibTblEntryPtr e)
{
	extern void *jmptable();

	e->dispatchTblP=(void *)jmptable;
	e->globalsP=NULL;
	return errNone;
}

/**
 * Lock the Library globals.
 *
 * @param refNum Library ref num.
 * @return Pointer to locked globals or NULL.
 */
static KeyboardGlobals *
LockGlobals(UInt16 refNum)
{
	KeyboardGlobals *g=NULL;
	SysLibTblEntryPtr sysLib=NULL;
	MemHandle mh=NULL;

	ErrFatalDisplayIf(refNum==sysInvalidRefNum, "(LockGlobals) Invalid refNum");
	sysLib=SysLibTblEntry(refNum);
	ErrFatalDisplayIf(sysLib==NULL, "(LockGlobals) Invalid refNum");

	if ((mh=sysLib->globalsP))
		g=MemHandleLock(mh);

	return g;
}

/**
 * Open
 */
Err
KeyboardLibOpen(UInt16 refNum, UInt16 version)
{
	KeyboardGlobals *g=NULL;

	ErrFatalDisplayIf(refNum==sysInvalidRefNum, "(KeyboardLibOpen) Invalid refNum");

	if ((g=LockGlobals(refNum))==NULL) {
		SysLibTblEntryPtr sysLib;
		MemHandle mh;

		sysLib=SysLibTblEntry(refNum);
		ErrFatalDisplayIf(sysLib==NULL, "(KeyboardLibOpen) Invalid refNum");
		ErrFatalDisplayIf(sysLib->globalsP, "(KeyboardLibOpen) Library globals already exist?!?!");
		mh=MemHandleNew(sizeof(KeyboardGlobals));
		ErrFatalDisplayIf(mh==NULL, "(KeyboardLibOpen) Out of memory");

		sysLib->globalsP=mh;
		g=LockGlobals(refNum);
		ErrFatalDisplayIf(g==NULL, "(KeyboardLibOpen) Cannot lock library globals");
		MemPtrSetOwner(g, 0);
		MemSet(g, sizeof(KeyboardGlobals), 0);

		g->refCount=1;
	} else
		g->refCount++;

	MemPtrUnlock(g);
	return errNone;
}

/**
 * Close library.
 *
 * @return 0 if library was closed succesfully and useCount is to believed.
 */
Err
KeyboardLibClose(UInt16 refNum, UInt16 *useCount)
{
	KeyboardGlobals *g=NULL;

	ErrFatalDisplayIf(refNum==sysInvalidRefNum, "(KeyboardLibClose) Invalid refNum");

	if (useCount==NULL || ((g=LockGlobals(refNum))==NULL))
		return -1;

	g->refCount--;
	ErrFatalDisplayIf(g->refCount<0, "(KeyboardLibClose) Library under lock");

	*useCount=g->refCount;
	MemPtrUnlock(g);

	if (*useCount<=0) {
		SysLibTblEntryPtr sysLib=SysLibTblEntry(refNum);
		MemHandle mh;

		ErrFatalDisplayIf(sysLib==NULL, "(KeyboardLibClose) Invalid refNum");

		mh=sysLib->globalsP;
		sysLib->globalsP=NULL;

		if (mh)
			MemHandleFree(mh);
	}

	return 0;
}

/**
 * Sleep
 */
Err
KeyboardLibSleep(UInt16 refNum)
{
	return errNone;
}

/**
 * Wake
 */
Err
KeyboardLibWake(UInt16 refNum)
{
	return errNone;
}

/**
 * Return the version of the Keyboard library.
 *
 * @param refNum Refnum.
 * @param version Where to store version.
 */
void
KeyboardLibVersion(UInt16 refNum, UInt32 *version)
{
	ErrFatalDisplayIf(version==NULL, "(KeyboardLibVersion) null argument");

	*version=LibraryVersion;
}

/**
 * Return API version compiled for.
 *
 * @param refNum Refnum.
 * @param api Where to API version.
 */
void
KeyboardLibAPIVersion(UInt16 refNum, UInt32 *api)
{
	ErrFatalDisplayIf(api==NULL, "(KeyboardLibAPIVersion) null argument");

	*api=KeyboardAPIVersion;
}

/**
 * Run the dialog.
 *
 * @param refNum Refnum.
 * @param dstPwdH Where to store password (-handle).
 * @param okClose Was dialog OK'ed or Cancelled?
 * @return errNone if successful, other in case of error.
 */
Err
KeyboardLibRun(UInt16 refNum, MemHandle *dstPwdH, UInt16 *okClose)
{
	LocalID id=0;
	FormType *form;
	MemHandle dotFontH;
	EventType ev;
	Err err;
	UInt16 idx;
	DmOpenRef db;

	ErrFatalDisplayIf(dstPwdH==NULL, "(KeyboardLibRun) null argument");

	*okClose=kbfCloseCancel;
	if (FtrSet(KeyboardLibCRID, KeyboardFtrClose, (UInt32)okClose)!=errNone)
		return erriSecurKeyboardOther;

	*dstPwdH=NULL;
	if (FtrSet(KeyboardLibCRID, KeyboardFtrStart, (UInt32)dstPwdH)!=errNone)
		return erriSecurKeyboardOther;

	for (idx=0; idx<MemNumCards(); idx++) {
		if ((id=DmFindDatabase(idx, KeyboardLibName))!=0)
			break;
	}

	ErrFatalDisplayIf(id==0, "(KeyboardLibRun) Cannot find myself");
	
	db=DmOpenDatabase(idx, id, dmModeReadOnly);
	ErrFatalDisplayIf(db==NULL, "(KeyboardLibRun) Cannot open myself");

	dotFontH=DmGetResource('NFNT', ObjectIDStart);
	ErrFatalDisplayIf(dotFontH==NULL, "(KeyboardLibRun) dotFont not found");
	FntDefineFont(FontStart, MemHandleLock(dotFontH));

	if ((form=FrmInitForm(fKeyboard))==NULL) {
		FtrUnregister(KeyboardLibCRID, KeyboardFtrStart);
		FtrUnregister(KeyboardLibCRID, KeyboardFtrClose);
		return erriSecurKeyboardOther;
	}

	FrmSetActiveForm(form);
	FrmSetEventHandler(form, (FormEventHandlerType *)EH);

	MemSet(&ev, sizeof(EventType), 0);
	ev.eType=frmOpenEvent;
	ev.data.frmLoad.formID=fKeyboard;
	EvtAddEventToQueue(&ev);

	for (;;) {
		EvtGetEvent(&ev, evtWaitForever);
		if (ev.eType==appStopEvent) {
			EvtAddEventToQueue(&ev);
			return erriSecurKeyboardAbort; 
		} else if ((ev.eType==frmCloseEvent) || (ev.eType==exitKeyboardEvent))
			break;

		if (SysHandleEvent(&ev)==true ||
		    MenuHandleEvent(0, &ev, &err)==true)
			continue;
		
		FrmDispatchEvent(&ev);
	}

	FrmEraseForm(form);
	FrmDeleteForm(form);

	MemHandleUnlock(dotFontH);
	DmReleaseResource(dotFontH);

	DmCloseDatabase(db);
	FtrUnregister(KeyboardLibCRID, KeyboardFtrStart);
	FtrUnregister(KeyboardLibCRID, KeyboardFtrClose);
	return erriSecurKeyboardNone;
}

/**
 * Return pointer to object.
 *
 * @param id Object id.
 */
static void *
ObjectGet(UInt16 id)
{
	FormType *frm=FrmGetActiveForm();

	return FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, id));
}

/**
 * EH
 */
static Boolean
EH(EventType *ev)
{
	FormType *frm=FrmGetActiveForm();

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(frm);
		FrmSetFocus(frm, FrmGetObjectIndex(frm, cKeyboardPwd));
		UpdateKeys(NULL);
		UpdateMask(NULL);
	case frmUpdateEvent:
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cKeyboardLayout:
			{
				KeyboardPrefs p;

				GetPrefs(&p);
				if (p.flags&KPFCalculator)
					p.flags&=~KPFCalculator;
				else
					p.flags|=KPFCalculator;

				SetPrefs(&p);
				UpdateKeys(&p);
			}
			break;
		case cKeyboardOK:
			{
				MemHandle *dstH=NULL;
				UInt32 v;
				UInt16 *okClose=NULL;

				if (FtrGet(KeyboardLibCRID, KeyboardFtrStart, (UInt32 *)&dstH)==errNone) {
					FieldType *fld=ObjectGet(cKeyboardPwd);

					*dstH=FldGetTextHandle(fld);
					FldSetTextHandle(fld, NULL);
				}

				if (FtrGet(KeyboardLibCRID, KeyboardFtrClose, &v)==errNone) {
					okClose=(UInt16 *)v;
					*okClose=kbfCloseOK;
				}
			}
		case cKeyboardCancel:
			{
				EventType e;

				MemSet(&e, sizeof(EventType), 0);
				e.eType=exitKeyboardEvent;
				EvtAddEventToQueue(&e);
			}
			return true;
		case cKeyboardMaskDot:
			{
				KeyboardPrefs p;

				GetPrefs(&p);
				p.flags&=~KPFMask;
				p.flags|=KPFMask;

				SetPrefs(&p);
				UpdateMask(&p);
			}
			break;
		case cKeyboardMaskAlp:
			{
				KeyboardPrefs p;

				GetPrefs(&p);
				p.flags&=~KPFMask;

				SetPrefs(&p);
				UpdateMask(&p);
			}
			break;
		case cKeyboardBS:
			{
				FieldType *fld=ObjectGet(cKeyboardPwd);
				UInt16 len;

				if ((len=FldGetTextLength(fld)))
					FldDelete(fld, len-1, len);
			}
			return true;
		case cKeyboardClear:
			{
				FieldType *fld=ObjectGet(cKeyboardPwd);
				UInt16 len;

				len=FldGetTextLength(fld);
				FldDelete(fld, 0, len);
			}
			return true;
		default:
			{
				UInt16 idx=ev->data.ctlSelect.controlID;

				if ((idx>=cKeyboard0) && (idx<=cKeyboard9)) {
					ControlType *ctl=ObjectGet(idx);
					FieldType *fld=ObjectGet(cKeyboardPwd);
					Char *c;

					c=(Char *)CtlGetLabel(ctl);
					FldInsert(fld, c, 1);
					return true;
				}
			}
			break;
		}
	default:
		break;
	}

	return false;
}

/**
 * Get keyboard preferences block.
 *
 * @param dst Where to store preferences.
 */
static void
GetPrefs(KeyboardPrefs *dst)
{
	UInt16 l=sizeof(KeyboardPrefs);
	Int16 retPrf;

	retPrf=PrefGetAppPreferences(KeyboardLibCRID, KeyboardPrfStart, dst, &l, false);
	if ((retPrf==noPreferenceFound) || (l!=sizeof(KeyboardPrefs)))
		dst->flags=KPFCalculator|KPFMask;
}

/**
 * Set keyboard preferences block.
 *
 * @param src Preferences.
 */
static void
SetPrefs(KeyboardPrefs *src)
{
	PrefSetAppPreferences(KeyboardLibCRID, KeyboardPrfStart, 1, src, sizeof(KeyboardPrefs), false);
}

/**
 * Update the label on the keys.
 */
static void
UpdateKeys(KeyboardPrefs *p)
{
	char *cal="7894561230\x05";
	char *phn="1234567890\x06";
	ControlType *ctl;
	KeyboardPrefs local;
	Char *c, *t;
	UInt16 idx;

	if (!p) {
		GetPrefs(&local);
		p=&local;
	}

	if (p->flags&KPFCalculator)
		t=cal;
	else
		t=phn;

	for (idx=0; idx<11; idx++) {
		ctl=ObjectGet(cKeyboard0+idx);
		c=(Char *)CtlGetLabel(ctl);
		*c=*t++;
		CtlSetLabel(ctl, c);
	}
}

/**
 * Update the mask key and field font.
 */
static void
UpdateMask(KeyboardPrefs *p)
{
	FieldType *fld=ObjectGet(cKeyboardPwd);
	FormType *frm=FrmGetActiveForm();
	KeyboardPrefs local;

	if (!p) {
		GetPrefs(&local);
		p=&local;
	}

	if (p->flags&KPFMask) {
		FrmHideObject(frm, FrmGetObjectIndex(frm, cKeyboardMaskDot));
		FrmShowObject(frm, FrmGetObjectIndex(frm, cKeyboardMaskAlp));
		FldSetFont(fld, FontStart);
	} else {
		FrmHideObject(frm, FrmGetObjectIndex(frm, cKeyboardMaskAlp));
		FrmShowObject(frm, FrmGetObjectIndex(frm, cKeyboardMaskDot));
		FldSetFont(fld, stdFont);
	}
}

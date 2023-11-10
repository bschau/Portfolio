#define MAIN
#include "sit.h"

/* globals */
static UInt16 timeoutctl;
static UInt32 lt, t;
static Boolean killedbytimeout;

/* protos */
static Boolean startapp(void);
static void stopapp(void);
static void eventloop(void);
static void timeout(UInt32);
static UInt16 getromversion(void);

/*******************************************************************************
*
*	startapp
*
*	Start application - opens database etc. etc.
*/
static Boolean
startapp(void)
{
	UInt16 prflen=sizeof(sitpref);
	Int16 retprf;

	killedbytimeout=false;
	reoto2r5();
	retprf=PrefGetAppPreferences(CREATORID, PrfApplication, &prefs, &prflen, true);
	if (retprf==noPreferenceFound || prflen!=sizeof(sitpref)) {
		prefs.flags=PREFCONFIRMDELETE|PREFPAGESCROLL|PREFLOGOUTPD;
		prefs.catidx=0;
		prefs.pinidx=PINPHONEPAD;
		prefs.alaidx=LOGOUT3;
		prefs.kaboomidx=KABOOM5;
		prefs.pinstars=1;
	}

	catlist.idx=prefs.catidx;
	rethinkpreferences();
	enabletimeout();

	KeySetMask(keyBitsAll);
	return opendbs();
}

/*******************************************************************************
*
*	stopapp
*
*	Stop application - closes databases etc. etc.
*/
static void
stopapp(void)
{
	Char *longString="                                        ";
	Char *emptyString="";

	timeout(0);

	prefs.catidx=catlist.idx;
	closedbs();
	FrmSaveAllForms();
	FrmCloseAllForms();

	if (mainkeyh)
		MemHandleFree(mainkeyh);

	doneshow();
	if ((killedbytimeout==true) && (prefs.flags&PREFLOCKDEVICE))
		EvtEnqueueKey((WChar)lockChr, 0, commandKeyMask);

	PrefSetAppPreferences(CREATORID, PrfApplication, APPVER, &prefs, sizeof(sitpref), true);
	ClipboardAddItem(clipboardText, longString, StrLen(longString));
	ClipboardAddItem(clipboardText, emptyString, 0);
}

/*******************************************************************************
*
*	PilotMain
*
*	Main for pilot applications.	This is the only place with UpperLower
*	case mix in function names.    It's ugly :)
*/
UInt32
PilotMain(UInt16 cmd,
          void *cmdPBP,
          UInt16 launchFlags)
{
	Boolean err;

	switch (cmd) {
	case sysAppLaunchCmdAlarmTriggered:
		EvtWakeup();
		break;

  	case sysAppLaunchCmdNormalLaunch:
		romv=getromversion();
		if (!romv)
			FrmCustomAlert(alertrom20, NULL, NULL, NULL);
		else {
			err=startapp();
			if (!err)
				eventloop();

			stopapp();
		}
		break;

	default:
		break;
	}

	return 0;
}

/*******************************************************************************
*
*	eventloop
*
*	Main dispatch routine.
*/
static void
eventloop(void)
{
	Boolean handled;
	UInt16 formid, err;
	FormPtr form;
	EventType ev, stopev;

	switch (nopwd) {
	case 1:	/* new database */
		infobox(NULL, formnewdb);
		getpassword(PASSDOUBLE);
		break;
	case 2: /* db present, no password */
		infobox(NULL, formnopassword);
		getpassword(PASSDOUBLE);
		break;
	default: /* log user in */
		getpassword(PASSSINGLE);
		break;
	}

	lt=TimGetSeconds();
	t=lt;
	for (EVER) {
		EvtGetEvent(&ev, evtWaitForever);

		if (timeoutctl) {
			t=TimGetSeconds();
			if ((t-lt)>=autooff) {
				killedbytimeout=true;
				break;
			}
		}
	
		if (ev.eType==appStopEvent)
			break;

		if (ev.eType==nilEvent)
			continue;

		if (timeoutctl) {
			lt=t;
			timeout(TimGetSeconds()+autooff+1);
		}

		//if (ev.eType==keyDownEvent && ev.data.keyDown.chr==vchrPowerOff) {
		if (ev.eType==keyDownEvent && ev.data.keyDown.chr==hardPowerChr) {
			if (prefs.flags&PREFLOGOUTPD) {
				MemSet(&stopev, sizeof(EventType), 0);
				stopev.eType=appStopEvent;
				EvtAddEventToQueue(&stopev);
				EvtEnqueueKey((WChar)hardPowerChr, 0, commandKeyMask);
				continue;
			}
		}

		handled=SysHandleEvent(&ev);
		if (handled) 
			continue;

		handled=MenuHandleEvent(NULL, &ev, &err);
		if (handled)
			continue;

		switch (ev.eType) {
		case frmLoadEvent:
			formid=ev.data.frmLoad.formID;
			form=FrmInitForm(formid);
			FrmSetActiveForm(form);
			switch (formid) {
			case formstart:
			case formstartnqa:
				if (preloadtmpl) {
					storerecord(tmpldb, "Password", "Hostname:\nUsername:\nPassword:", 0, dmMaxRecordIndex);
					storerecord(tmpldb, "Creditcard", "Card no#:\nExpiry date:\nName as printed:", 0, dmMaxRecordIndex);
					storerecord(tmpldb, "Pincode", "Pincode:", 0, dmMaxRecordIndex);
					preloadtmpl=0;
				}
				startinit(form);
				break;

			case formeditentry:
				editentryinit(form);
				break;

			case formpincode:
				pincodeinit(form);
				break;

			case formtextual:
				textualinit(form);
				break;

			case formabout:
				aboutinit(form);
				break;

			case formshowentry:
			case formshowentrynqa:
				showentryinit(form);
				break;

			case formtemplateeditor:
				templateeditorinit(form);
				break;

			case formedittemplate:
				edittemplateinit(form);
				break;
			}
			break;

		default:
			FrmDispatchEvent(&ev);
			break;
		}
	}
}

/*******************************************************************************
*
*	timeout
*
*	Set next timeout alarm.
*/
static void
timeout(UInt32 t)
{
	Err err;
	UInt16 card;
	LocalID dbid;

	err=SysCurAppDatabase(&card, &dbid);
	if (!err) {
		err=AlmSetAlarm(card, dbid, 0, t, true);
		if (!err)
			return;
	}
}

/*******************************************************************************
*
* 	enabletimeout
*
*	Enable timeout engine.
*/
void
enabletimeout(void)
{
	t=TimGetSeconds();
	lt=t;
	timeoutctl=1;
}

/*******************************************************************************
*
*	disabletimeout
*
*	Disable timeout engine.
*/
void
disabletimeout(void)
{
	timeout(0);
	lt=t;
	timeoutctl=0;
}

/*******************************************************************************
*
*	getromversion
*
*	Get ROM version.  Implements minimum logic.
*/
static UInt16
getromversion(void)
{
	UInt32 rv;

	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &rv);
	if (rv>=0x03003000)
		return ROMV3;

	if (rv>=0x02003000)
		return ROMV2;

	return ROMNONE;
}

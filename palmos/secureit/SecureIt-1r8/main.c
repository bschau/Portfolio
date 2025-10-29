#define MAIN
#include "sit.h"

/* globals */
static UInt16 nopwd, timeoutctl;
static UInt32 lt, t;

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
*
*	Output:
*		s: 0.
*		f: !0.
*/
static Boolean
startapp(void)
{
	UInt16 prflen=sizeof(sitpref), card;
	Int16 retprf;
	LocalID dbid, appid;
	Err err;
	MemHandle h;
	aiblock *ai;

	retprf=PrefGetAppPreferences((UInt32)CREATORID, 0, &prefs, &prflen, true);
	if (retprf==noPreferenceFound || prflen!=sizeof(sitpref)) {
		prefs.quickaccess=false;
		prefs.confirmdelete=true;
		prefs.protectdb=false;
		prefs.catidx=0;
		prefs.pinidx=PINPHONEPAD;
		prefs.alaidx=LOGOUT3;
		prefs.kaboomidx=KABOOM5;
		prefs.pinstars=1;
	}

	editflag=0;
	rethinkpreferences();
	enabletimeout();

	dbid=DmFindDatabase(0, DATABASENAME);
	if (!dbid) {	/* create database */
		nopwd=1;
		err=DmCreateDatabase(0, DATABASENAME, (UInt32)CREATORID, (UInt32)DATABASETYPE, false);
		if (err)
			return err;

		dbid=DmFindDatabase(0, DATABASENAME);
		sitdb=DmOpenDatabase(0, dbid, dmModeReadWrite);
		if (!sitdb)
			return DmGetLastErr();

		/* create aiblock */
		err=DmOpenDatabaseInfo(sitdb, &dbid, NULL, NULL, &card, NULL);
		if (err)
			return -1;

		h=DmNewHandle(sitdb, sizeof(aiblock));
		if (!h)
			return -1;

		appid=MemHandleToLocalID(h);
		DmSetDatabaseInfo(card, dbid, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &appid, NULL, NULL, NULL);
		ai=(aiblock *)MemHandleLock(h);
		DmSet(ai, 0, sizeof(aiblock), 0);

		CategoryInitialize((AppInfoPtr)ai, sfmaincat);

		MemPtrUnlock(ai);
	} else {	/* existing database */
		nopwd=0;
		sitdb=DmOpenDatabase(0, dbid, dmModeReadWrite);
		if (!sitdb)
			return DmGetLastErr();

		err=DmOpenDatabaseInfo(sitdb, &dbid, NULL, NULL, &card, NULL);
		if (err)
			return -1;

		err=DmDatabaseInfo(card, dbid, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &appid, NULL, NULL, NULL);
		if (err)
			return -1;

		ai=(aiblock *)MemLocalIDToLockedPtr(appid, card);
		if (ai->pwdgiven)
			MemMove(md5pwd, ai->md5, sizeof(unsigned long)*4);
		else
			nopwd=2;

		pwdgiven=ai->pwdgiven;

		MemPtrUnlock((MemPtr)ai);
	}

	setdbprotect();

	newlist(&catlist, prefs.catidx);
	keyh=MemHandleNew(32);
	if (!keyh)
		return DmGetLastErr();

	keyp=(Char *)MemHandleLock(keyh);

	mainkeyh=MemHandleNew(sizeof(blowfishkey));
	if (!mainkeyh)
		return DmGetLastErr();

	return 0;
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
	timeout(0);

	if (mainkeyh) {
		MemHandleFree(mainkeyh);
		mainkeyh=NULL;
	}

	if (keyh) {
		MemHandleUnlock(keyh);
		MemHandleFree(keyh);
		keyh=NULL;
	}

	prefs.catidx=catlist.idx;
	freelist(&catlist);
	freelist(&entrylist);
	if (sitdb) {
		DmCloseDatabase(sitdb);
		sitdb=NULL;
	}
	PrefSetAppPreferences((UInt32)CREATORID, 0, APPVER, &prefs, sizeof(sitpref), true);
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
	UInt16 formid, err;
	FormPtr form;
	Boolean handled;
	EventType ev;

	switch (nopwd) {
	case 1:	/* new database */
		about(NULL, formnewdb);
		getpassword(PASSDOUBLE);
		break;
	case 2: /* db present, no password */
		about(NULL, formnopassword);
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
			if ((t-lt)>=autooff)
				break;
		}
	
		if (ev.eType==appStopEvent)
			break;

		if (ev.eType==nilEvent)
			continue;

		if (timeoutctl) {
			lt=t;
			timeout(TimGetSeconds()+autooff+1);
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
				/* FALL-THROUGH */
			case formstartnqa:
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
 *
 *	Input:
 *		t - time.
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
 *
 *	Output:
 *		ROMNONE, ROMV2, ROMV3.
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

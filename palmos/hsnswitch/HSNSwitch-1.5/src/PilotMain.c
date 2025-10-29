/*
 * PilotMain.c
 */
#define MAIN
#include "Include.h"

/* protos */
static Boolean RomCheck(SHSInfo *);
static Boolean Start(void);
static void Stop(void);
static void EventLoop(void);
static void RegisterForNotifications(void);
static void HandleNotification(SysNotifyParamType *);
static Boolean SetHotSyncName(Char *);

/*
 * RomCheck
 *
 *  -> shs		System HotSync record info.
 *
 * Returns true if the Rom Version is equal to or better than the required
 * Rom Version. False is returned otherwise.
 *
 * Note. This routine may be called when Globals are not present (after a 
 * reset f.ex.).
 */
static Boolean
RomCheck(SHSInfo *shs)
{
	UInt32 version=0;

	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version);

	if ((version<sysMakeROMVersion(5, 0, 0, sysROMStageDevelopment, 0)) || 
	    (version>sysMakeROMVersion(5, 5, 0, sysROMStageDevelopment, 0))) {
		return false;
	}

	switch (version) {
	default:
		shs->nameIdx=SHSNameIdxOS50;
		shs->logIdx=SHSLogIdxOS50;
		shs->size=SHSSizeOS50;
		break;
	}
	return true;
}

/*
 * Start
 *
 * Start application (open database etc. etc.).
 *
 * Return false in case of errors.
 */
static Boolean
Start(void)
{
	if (PMGetPref(&prefs, sizeof(prefs), PrfApplication)==false) {
		prefs.hotSyncName[0]='\x00';
		prefs.lastHotSyncName[0]='\x00';
		prefs.prefIdx=4;
		prefs.slots=30;
		prefs.trustPrefIdx=false;
	} else if (StrLen(prefs.hotSyncName)<1) {
		prefs.hotSyncName[0]='\x00';
		prefs.lastHotSyncName[0]='\x00';
		prefs.trustPrefIdx=false;
	}

	if (!prefs.hotSyncName[0])
		DlkGetSyncInfo(NULL, NULL, NULL, prefs.hotSyncName, NULL, NULL);

	if (prefs.trustPrefIdx==false) {
		tapMenuEvent.eType=menuEvent;
		tapMenuEvent.penDown=false;
		tapMenuEvent.data.menu.itemID=mMainDetect;
	}

	ListInit(&lstPrograms);
	ListAppDiscover(&lstPrograms);

	DBCreateDatabase();
	RegisterForNotifications();
	return DBOpen(&dbHSN, true);
}

/*
 * Stop
 *
 * Application shutdown.
 */
static void
Stop(void)
{
	FrmSaveAllForms();
	FrmCloseAllForms();

	DBClose(&dbHSN);

	ListDestroy(&lstPrograms);

	PMSetPref(&prefs, sizeof(prefs), PrfApplication);
}

/*
 * PilotMain
 */
UInt32
PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
	switch (cmd) {
	case sysAppLaunchCmdSystemReset:
	case sysAppLaunchCmdSyncNotify:
		RegisterForNotifications();
		break;

	case sysAppLaunchCmdNormalLaunch:
		if (RomCheck(&gShs)==true) {
			if (Start()==true) {
				FrmGotoForm(fMain);
				EventLoop();
			}
			Stop();
		} else
			FrmAlert(aBadRom);

		break;

	case sysAppLaunchCmdNotify:
		HandleNotification((SysNotifyParamType *)cmdPBP);

	default:	/* FALL-THRU */
		break;
	}

	return 0;
}

/*
 * EventLoop
 *
 * Main event loop dispatcher.
 */
static void
EventLoop(void)
{
	UInt16 err;
	EventType ev;

	appStopped=false;
	for (EVER) {
		EvtGetEvent(&ev, evtWaitForever);

		if (ev.eType==appStopEvent) {
			appStopped=true;
			break;
		}

		if (ev.eType==nilEvent)
			continue;

		if (SysHandleEvent(&ev)==true ||
		    MenuHandleEvent(NULL, &ev, &err)==true)
			continue;

		switch (ev.eType) {
		case frmLoadEvent:
			currentFormID=ev.data.frmLoad.formID;
			currentForm=FrmInitForm(currentFormID);
			FrmSetActiveForm(currentForm);

			if (currentFormID==fMain) {
				if (fMainInit()==true) {
					FrmSetEventHandler(currentForm, (FormEventHandlerType *)fMainEH);
					break;
				} else
					ErrFatalDisplay("(EventLoop) MainForm failed to initialize.");
			}

			ErrFatalDisplay("(EventLoop) The program tried to initialize a non-existing form.");
			break;
		default:
			FrmDispatchEvent(&ev);
			break;
		}
	}
}

/*
 * PMGetPref
 *
 * Get preference #.
 *
 *  -> p	Destination.
 *  -> len	Length of destination.
 *  -> id	Preference id.
 *
 * Returns true if preferences was gotten, false otherwise.
 */
Boolean
PMGetPref(void *p, UInt16 len, UInt16 id)
{	
	UInt16 l=len;
	Int16 retPrf;

	retPrf=PrefGetAppPreferences((UInt32)CRID, id, p, &l, false);
	if (retPrf==noPreferenceFound || len!=l)
		return false;

	return true;
}

/*
 * PMSetPref
 *
 * Set preference #.
 *
 *  -> p	Source.
 *  -> len	Length of source.
 *  -> id	Preferences id.
 */
void
PMSetPref(void *p, UInt16 len, UInt16 id)
{
	PrefSetAppPreferences((UInt32)CRID, id, PREFSVER, p, len, false);
}

/*
 * RegisterForNotifications
 *
 * Setup to receive notifications.
 */
static void
RegisterForNotifications(void)
{
	SHSInfo shs;

	if (RomCheck(&shs)==true) {
		UInt16 cardNo;
		LocalID dbID;

		SysCurAppDatabase(&cardNo, &dbID);
		SysNotifyRegister(cardNo, dbID, sysNotifyAppLaunchingEvent, NULL, sysNotifyNormalPriority, NULL);
		SysNotifyRegister(cardNo, dbID, sysNotifyAppQuittingEvent, NULL, sysNotifyNormalPriority, NULL);
	}
}

/*
 * HandleNotification
 *
 * Handles a notification.
 *
 *  -> snp	SysNotifyParamType.
 */
static void
HandleNotification(SysNotifyParamType *snp)
{
	Char appName[dmDBNameLength+2];
	HotSyncRecord hsr;
	DB dbo;
	UInt16 rIdx;
	MemHandle mh;

	snp->handled=false;
	if (snp->notifyType!=sysNotifyAppLaunchingEvent && snp->notifyType!=sysNotifyAppQuittingEvent)
		return;

	if (DmDatabaseInfo(((SysNotifyAppLaunchOrQuitType *)snp->notifyDetailsP)->cardNo, ((SysNotifyAppLaunchOrQuitType *)snp->notifyDetailsP)->dbID, appName, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)!=errNone)
		return;

	if (DBOpen(&dbo, false)==false)
		return;

	StrNCopy(hsr.appName, appName, dmDBNameLength);
	if ((rIdx=DBFindRecord(&dbo, &hsr))!=dmMaxRecordIndex) {
		if (snp->notifyType==sysNotifyAppLaunchingEvent) {
			mh=DBGetRecord(&dbo, rIdx);
			snp->handled=SetHotSyncName(((HotSyncRecord *)MemHandleLock(mh))->hotSyncName);
			MemHandleUnlock(mh);
		} else
			snp->handled=SetHotSyncName(NULL);

	}
	DBClose(&dbo);
}

/*
 * SetHotSyncName
 *
 * Set HotSync name.
 *
 *  -> name		HotSync name (or NULL to reset).
 *
 * Return true if name was set, false otherwise.
 */
static Boolean
SetHotSyncName(Char *name)
{
	UInt16 len=0, rLen, nameLen;
	SHSInfo shs;
	UInt8 *rSrc, *rDst;
	MemHandle mh, rh;
	Prefs p;
	Char *n, *t;

	if (PMGetPref(&p, sizeof(prefs), PrfApplication)==false)
		return false;

	if (p.trustPrefIdx==false)
		return false;

	if (name)
		n=name;
	else
		n=p.hotSyncName;

	if ((nameLen=StrLen(n))<1)
		return false;

	if (PrefGetAppPreferences((UInt32)'psys', p.prefIdx, NULL, &len, false)==noPreferenceFound)
		return false;

	if (len<SHSSizeOS50 || len>9999)
		return false;

	RomCheck(&shs);

	mh=MemHandleNew(len);
	ErrFatalDisplayIf(mh==NULL, "(SetHotSyncName) Device is full.");

	rSrc=MemHandleLock(mh);
	PrefGetAppPreferences((UInt32)'psys', p.prefIdx, rSrc, &len, false);

	rLen=shs.size+nameLen+1+1;
	rh=MemHandleNew(rLen);
	ErrFatalDisplayIf(rh==NULL, "(SetHotSyncName) Device is full.");

	rDst=MemHandleLock(rh);

	MemMove(rDst, rSrc, shs.size);
	t=rDst+shs.size;
	MemMove(t, n, nameLen);
	t+=nameLen;
	*t++='\x00';
	*t++='\x00';
	rDst[shs.nameIdx]=nameLen+1;
	rDst[shs.logIdx]=1;

	PrefSetAppPreferences((UInt32)'psys', p.prefIdx, 0x0201, rDst, rLen, false);
	MemHandleFree(rh);
	MemHandleFree(mh);
	return true;
}

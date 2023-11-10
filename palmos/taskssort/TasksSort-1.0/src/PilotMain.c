/*
 * PilotMain.c
 */
#define MAIN
#include "Include.h"

/* protos */
static Boolean RomCheck(void);
static Boolean Start(Boolean);
static void Stop(void);
static void EventLoop(void);
static Err SwitcherHook(SysNotifyParamType *);

/* globals */
static struct {
	UInt16 id;
	Boolean (*init)();
	FormEventHandlerType *evh;
} formInits[]= {
	{ fMain, fMainInit, fMainEH },
	{ 0 },
};
static UInt32 romVersion;

/*
 * RomCheck
 *
 * Returns true if the Rom Version is equal to or better than the required
 * Rom Version. False is returned otherwise.
 */
static Boolean
RomCheck(void)
{
	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);

	if (romVersion<sysMakeROMVersion(5, 0, 0, sysROMStageRelease, 0)) {
		FrmAlert(aBadRom);
		return false;
	}

	return true;
}

/*
 * Start
 *
 * Start application (open database etc. etc.).
 *
 *  -> normal		Normal start (=true).
 *
 * Return false in case of errors.
 */
static Boolean
Start(Boolean normal)
{
#ifdef DEBUG
#if SYS==0
	gHostFile=HostFOpen("/tmp/TasksSort.log", "a+");
#else
	gHostFile=HostFOpen("c:\\temp\\TasksSort.log", "a+");
#endif
	ErrFatalDisplayIf(gHostFile==NULL, "(Start) HostFOpen failed");
	DPrint("======== TasksSort Started ====\n");
#endif

	if (PMGetPref(&prefs, sizeof(Prefs), PrfApplication, true)==false) {
		prefs.flags=PFlgLaunchTasks|PFlgHotKey;
		prefs.hotKey=(WChar)'t';
	}

	if (PMGetPref(&state, sizeof(State), PrfState, false)==false)
		state.sortOrder=SSOAscending;

	ui=UIInit();

	return true;
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

	PMSetPref(&prefs, sizeof(Prefs), PrfApplication, true);
	PMSetPref(&state, sizeof(State), PrfState, false);

#ifdef DEBUG
	if (gHostFile)
		HostFClose(gHostFile);
#endif
}

/*
 * PilotMain
 */
UInt32
PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
#if DEBUG==1
	{
		UInt32 v=0;

		if (FtrGet(kPalmOSEmulatorFeatureCreator, 0, &v)==ftrErrNoSuchFeature) {
			if ((FtrGet(sysFtrCreator, sysFtrNumProcessorID, &v)==ftrErrNoSuchFeature) || ((v&sysFtrNumProcessorMask)!=sysFtrNumProcessorx86)) {
				FrmAlert(aNoPose);
				return 0;
			}
		}
	}
#endif

	switch (cmd) {
	case sysAppLaunchCmdNormalLaunch:
		PMUnregisterAppEvents();
		if (RomCheck()==true) {
			if (Start(true)==true) {
				FrmGotoForm(fMain);
				EventLoop();
			}

			Stop();
			PMRegisterAppEvents();
		}
		break;

	case sysAppLaunchCmdNotify:
		{
			Prefs prf;

			if (PMGetPref(&prf, sizeof(Prefs), PrfApplication, true)==false) 
				break;

			if (((prf.flags&PFlgHotKey)==0) || (!prf.hotKey))
				break;

			if (PMHasNotifications()) {
				SysNotifyAppLaunchOrQuitType *tag=((SysNotifyParamType *)cmdPBP)->notifyDetailsP;
				UInt32 crid, type;

				switch (((SysNotifyParamType *)cmdPBP)->notifyType) {
				case sysNotifyAppLaunchingEvent:
					if ((DmDatabaseInfo(tag->cardNo, tag->dbID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &type, &crid)==errNone) && (crid==(UInt32)'PTod') && (type==(UInt32)'appl')) {
						MemHandle mh=DmGetResource(sysResTAppCode, 1);
						Boolean release=true;

						ErrFatalDisplayIf(mh==NULL, "Failed to get own code segment");
						(void)MemHandleLock(mh);
						MemHandleSetOwner(mh, 0);

						if (FtrSet(CRID, FtrSwitcher, (UInt32)mh)==errNone) {
							UInt32 hotKey=(UInt32)prf.hotKey;
							LocalID id;
							UInt16 card;

							SysCurAppDatabase(&card, &id);
							if (SysNotifyRegister(card, id, sysNotifyEventDequeuedEvent, SwitcherHook, sysNotifyNormalPriority, (void *)hotKey)==errNone)
								release=false;
						}

						if (release) {
							FtrUnregister(CRID, FtrSwitcher);
							MemHandleUnlock(mh);
							DmReleaseResource(mh);
						}
					}
					break;

				case sysNotifyAppQuittingEvent:
					if ((DmDatabaseInfo(tag->cardNo, tag->dbID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &type, &crid)==errNone) && (crid==(UInt32)'PTod') && (type==(UInt32)'appl'))
						PMUnregisterAppEvents();
					break;

				default:
					break;
				}
			}
		}
		break;

	case sysAppLaunchCmdSyncNotify:
	case sysAppLaunchCmdSystemReset:
		PMUnregisterAppEvents();
		PMRegisterAppEvents();
		break;

	default:
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
	UInt16 err, idx;
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

			for (idx=0; formInits[idx].id>0; idx++) {
				if (formInits[idx].id==currentFormID) {
					if ((formInits[idx].init!=NULL) && ((formInits[idx].init())==false))
						return;

					FrmSetEventHandler(currentForm, (FormEventHandlerPtr)formInits[idx].evh);
					break;
				}
			}

			ErrFatalDisplayIf(formInits[idx].id==0, "(EventLoop) The program tried to initialize a non-existing form.");
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
 *  -> type	Get from saved preferences (= true).
 *
 * Returns true if preferences was gotten, false otherwise.
 */
Boolean
PMGetPref(void *p, UInt16 len, UInt16 id, Boolean type)
{	
	UInt16 l=len;
	Int16 retPrf;

	retPrf=PrefGetAppPreferences((UInt32)CRID, id, p, &l, type);
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
 *  -> type	Set in saved prefs (= true).
 */
void
PMSetPref(void *p, UInt16 len, UInt16 id, Boolean type)
{
	PrefSetAppPreferences((UInt32)CRID, id, PREFSVER, p, len, type);
}

/**
 * Is notifications supported?
 *
 * @return true (yes) or false (no).
 */
Boolean
PMHasNotifications(void)
{
	UInt32 v=0;

	if (FtrGet(sysFtrCreator, sysFtrNumNotifyMgrVersion, &v)==errNone) {
		if (v)
			return true;
	}

	return false;
}

/**
 * Register the AppLaunching and AppQuitting events.
 */
void
PMRegisterAppEvents(void)
{
	if (PMHasNotifications()) {
		Prefs prf;

		if (PMGetPref(&prf, sizeof(Prefs), PrfApplication, true)==true) {
			UInt16 card;
			LocalID id;

			SysCurAppDatabase(&card, &id);

			if (prf.flags&PFlgHotKey) {
				SysNotifyRegister(card, id, sysNotifyAppLaunchingEvent, NULL, sysNotifyNormalPriority, NULL);
				SysNotifyRegister(card, id, sysNotifyAppQuittingEvent, NULL, sysNotifyNormalPriority, NULL);
			} else {
				SysNotifyUnregister(card, id, sysNotifyAppLaunchingEvent, sysNotifyNormalPriority);
				SysNotifyUnregister(card, id, sysNotifyAppQuittingEvent, sysNotifyNormalPriority);
			}
		}
	}
}

/**
 * Unregister the AppLaunching events.
 */
void
PMUnregisterAppEvents(void)
{
	UInt32 mh=NULL;
	LocalID id;
	UInt16 card;

	SysCurAppDatabase(&card, &id);
	SysNotifyUnregister(card, id, sysNotifyEventDequeuedEvent, sysNotifyNormalPriority);

	if (FtrGet(CRID, FtrSwitcher, &mh)==errNone) {
		FtrUnregister(CRID, FtrSwitcher);
		if (mh) {
			MemHandleUnlock((MemHandle)mh);
			DmReleaseResource((MemHandle)mh);
		}
	}
}

/**
 * Switcher ...
 */
static Err
SwitcherHook(SysNotifyParamType *p)
{
	static Boolean seenGlyph=false;

	if (p->notifyType==sysNotifyEventDequeuedEvent) {
		EventType *evt=(EventType *)p->notifyDetailsP;

		if (evt->eType==EndianSwap16(keyDownEvent)) {
			if (evt->data.keyDown.modifiers&EndianSwap16(commandKeyMask)) {
				seenGlyph=true;
			} else if (seenGlyph) {
				UInt32 c=(UInt32)p->userDataP;

				if (evt->data.keyDown.chr==EndianSwap16(((UInt16)c))) {
					MemHandle appls=NULL;
					UInt16 cnt=0;

					if (SysCreateDataBaseList((UInt32)'appl', CRID, &cnt, &appls, false)==true) {
						if (cnt>0) {
							SysDBListItemType *sys=MemHandleLock(appls);

							SysUIAppSwitch(sys->cardNo, sys->dbID, sysAppLaunchCmdNormalLaunch, NULL);
						}	
					}

					if (appls)
						MemHandleFree(appls);

					evt->eType=EndianSwap16(nilEvent);
				}
				seenGlyph=false;
			}
		}
	}

	return errNone;
}

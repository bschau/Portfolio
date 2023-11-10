/*
 * PilotMain.c
 */
#define MAIN
#include "Include.h"

/* protos */
static Boolean RomCheck(void);
static void GetAppPrefs(Prefs *);
static Boolean Start(void);
static void Stop(void);
static void EventLoop(void);
static void Alarm(UInt32, UInt32);
static UInt32 ConvertAutoDigi(UInt16);
static void AutoDigiAlarmHandler(UInt16, SysAlarmTriggeredParamType *);

/*
 * RomCheck
 *
 * Returns true if the Rom Version is equal to or better than the required
 * Rom Version. False is returned otherwise.
 */
static Boolean
RomCheck(void)
{
	UInt32 version=0;

	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version);

	if (version<sysMakeROMVersion(3, 0, 0, sysROMStageRelease, 0)) {
		FrmAlert(aBadRom);
		return false;
	}

	return true;
}

/*
 * GetAppPrefs
 *
 * Get application preferences (or default values).
 *
 * <-  dst		Destination.
 */
static void
GetAppPrefs(Prefs *dst)
{
	if (PMGetPref(dst, sizeof(Prefs), PrfApplication)==false) {
		dst->flags=0;
		dst->delay=2;
		dst->autoDigi=0;
		dst->timeout=30;
		dst->precision=1;
	}
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
	GetAppPrefs(&prefs);

	oldKeyMask=KeySetMask(~HardKeys);
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

	KeySetMask(oldKeyMask);
	PMSetPref(&prefs, sizeof(Prefs), PrfApplication);
}

/*
 * PilotMain
 */
UInt32
PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
	DigitizerPrefs d;
	Prefs prf;

	switch (cmd) {
	case sysAppLaunchCmdNormalLaunch:
		if (RomCheck()==true) {
			NotificationsSetup();
			if (!cmdPBP) {
				Alarm(0, 0);
				if (Start()==true) {
					if (PMGetPref(&d, sizeof(DigitizerPrefs), PrfDigitizer)==false)
						fDigitizerRun(false, prefs.timeout, PMPrecision(prefs.precision));

					FrmGotoForm(fMain);
					EventLoop();
				}
				Stop();
			} else {
				GetAppPrefs(&prf);
				fDigitizerRun(false, prf.timeout, PMPrecision(prf.precision));
			}

			PMSetupAutoDigi();
		}
		break;

	case sysAppLaunchCmdSystemReset:
		GetAppPrefs(&prf);
		Alarm(prf.delay+TimGetSeconds(), 1);
		break;
		
	case sysAppLaunchCmdSyncNotify:
		NotificationsSetup();
		if (prf.flags&PFlgHotSync)
			Alarm(TimGetSeconds()+1, 1);
		break;

	case sysAppLaunchCmdNotify:
		switch (((SysNotifyParamType *)cmdPBP)->notifyType) {
		case sysNotifySyncFinishEvent:
			GetAppPrefs(&prf);
			if (prf.flags&PFlgHotSync)
				Alarm(TimGetSeconds()+1, 1);
			break;

		case sysNotifyAppLaunchingEvent:
		case sysNotifyAppQuittingEvent:
		case sysNotifyLateWakeupEvent:
			if (PMGetPref(&d, sizeof(DigitizerPrefs), PrfDigitizer)) {
				PenResetCalibration();
				PenCalibrate(&d.aTL, &d.aBR, &d.eTL, &d.eBR);
				PMSetupAutoDigi();
			}
			break;
		}
		break;

	case sysAppLaunchCmdAlarmTriggered:
		GetAppPrefs(&prf);
		if (!((SysAlarmTriggeredParamType *)cmdPBP)->ref) {
			if (prf.autoDigi) {
				((SysAlarmTriggeredParamType *)cmdPBP)->purgeAlarm=true;
				if (PMGetPref(&d, sizeof(DigitizerPrefs), PrfDigitizer)) {
					PenResetCalibration();
					PenCalibrate(&d.aTL, &d.aBR, &d.eTL, &d.eBR);
				}
				PMSetupAutoDigi();
			}
		} else {
			UInt16 cardNo;
			LocalID dbID;
			MemHandle mh;

			if ((mh=MemHandleNew(4))) {
				Char *p=MemHandleLock(mh);

				MemMove(p, "Rcal", 4);
				MemPtrSetOwner(p, 0);
				SysCurAppDatabase(&cardNo, &dbID);
				if (SysUIAppSwitch(cardNo, dbID, sysAppLaunchCmdNormalLaunch, p))
					MemHandleFree(mh);
			}
		}
		break;

		/*
	case sysAppLaunchCmdDisplayAlarm:
		GetAppPrefs(&prf);
		if (FtrGet((UInt32)CRID, 0, &ftrValue)==errNone) {
			UInt16 cardNo;
			LocalID dbID;
			MemHandle mh;

			FtrUnregister((UInt32)CRID, 0);
			if ((mh=MemHandleNew(4))) {
				Char *p=MemHandleLock(mh);

				MemMove(p, "Rcal", 4);
				MemPtrSetOwner(p, 0);
				SysCurAppDatabase(&cardNo, &dbID);
				if (SysUIAppSwitch(cardNo, dbID, sysAppLaunchCmdNormalLaunch, p))
					MemHandleFree(mh);
			}
		} else
			Alarm(0);

			*/
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
	Boolean hasNav=PMNavSupported();
	UInt16 err;
	EventType ev;

	appStopped=false;
	for (EVER) {
		EvtGetEvent(&ev, evtWaitForever);
		if (ev.eType==appStopEvent) {
			appStopped=true;
			break;
		}

		if (hasNav==false) {
			if ((currentFormID==fMain) && (KeyCurrentState()&HardKeys)) {
				fMainSavePrefs();
				fDigitizerRun(true, prefs.timeout, PMPrecision(prefs.precision));
				FrmGotoForm(fMain);
				continue;	
			}
		}

		if (ev.eType==nilEvent)
			continue;

		if (SysHandleEvent(&ev)==true ||
		    MenuHandleEvent(NULL, &ev, &err)==true)
			continue;

		switch (ev.eType) {
		case frmLoadEvent:
			if ((currentFormID=ev.data.frmLoad.formID)==fDigitize) {
				fDigitizerRun(true, prefs.timeout, PMPrecision(prefs.precision));
				FrmGotoForm(fMain);
				break;
			}

			currentForm=FrmInitForm(currentFormID);
			FrmSetActiveForm(currentForm);

			if (currentFormID==fMain) {
				if (fMainInit()==false)
					return;

				FrmSetEventHandler(currentForm, (FormEventHandlerPtr)fMainEH);
			} else
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
 * PMOS5Device
 *
 * Returns true if this is a Palm OS 5.x device, false otherwise.
 */
Boolean
PMOS5Device(void)
{
	UInt32 version=0;

	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version);

	if ((version<sysMakeROMVersion(5, 0, 0, sysROMStageDevelopment, 0)) ||
	    (version>sysMakeROMVersion(5, 5, 0, sysROMStageRelease, 0))) 
		return false;

	return true;
}

/*
 * Alarm
 *
 * Set alarm to fire off at X.
 *
 *  -> X		Time to fire off alarm.
 *  -> refValue		Ref value sent to handler.
 */
static void
Alarm(UInt32 X, UInt32 refValue)
{
	UInt16 cardNo;
	LocalID dbID;

	if (SysCurAppDatabase(&cardNo, &dbID)==errNone)
		AlmSetAlarm(cardNo, dbID, refValue, X, true);
}

/*
 * ConvertAutoDigi
 *
 * Convert an autoDigi index to seconds.
 *
 *  -> idx		Auto Digi index.
 *
 * Returns 0 (disable alarm) or number of seconds to next alarm.
 */
static UInt32
ConvertAutoDigi(UInt16 idx)
{
	UInt32 secsArr[]={ 10, 30, 60, 120, 300, 600, 1800, 3600, 7200, 21600, 43200, 86400, 604800, 1209600, 25920000 };

	if (idx<1 && idx>15)
		return 0;

	return (secsArr[idx-1]+TimGetSeconds());
}

/*
 * AutoDigiAlarmHandler
 */
static void
AutoDigiAlarmHandler(UInt16 almProcCmd, SysAlarmTriggeredParamType *paramP)
{
	Prefs prf;

	GetAppPrefs(&prf);
	if (prf.autoDigi) {
		DigitizerPrefs d;

		paramP->purgeAlarm=true;
		if (PMGetPref(&d, sizeof(DigitizerPrefs), PrfDigitizer)) {
			PenResetCalibration();
			PenCalibrate(&d.aTL, &d.aBR, &d.eTL, &d.eBR);
		}
		PMSetupAutoDigi();
	}
}

/*
 * PMSetupAutoDigi
 *
 * Receive AutoDigi alarms.
 */
void
PMSetupAutoDigi(void)
{
	Prefs p;

	GetAppPrefs(&p);
	if (p.autoDigi) {
		UInt32 version;

		FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version);

		if (version<sysMakeROMVersion(3, 2, 0, sysROMStageRelease, 0))
			Alarm(ConvertAutoDigi(p.autoDigi), 0);
		else
			AlmSetProcAlarm(AutoDigiAlarmHandler, 0, ConvertAutoDigi(p.autoDigi));
	}
}

/*
 * PMNavSupported
 *
 * Return true if 5 way navigation is supported, false otherwise.
 */
Boolean
PMNavSupported(void)
{
	UInt32 v;

	if (FtrGet(sysFtrCreator, sysFtrNumFiveWayNavVersion, &v)==0)
		return true;

	return false;
}

/*
 * PMPrecision
 *
 * Convert a prefs.precision index to points.
 * 
 *  -> pp	Prefs.precision.
 */
UInt16
PMPrecision(UInt16 pp)
{
	switch (pp) {
	case 3:
		return 5;
	case 4:
		return 10;
	}

	return (pp+1);
}

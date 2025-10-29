/*
**	eventor.c
**
**	Main entry points and form handler for Eventor.
*/
#define MAIN
#include "eventor.h"

/* protos */
static Boolean Start(void);
static void Stop(void);
static void EventLoop(void);

/* globals */
static struct {
	UInt16 id;
	Boolean (*init)();
	FormEventHandlerPtr evh;
} formInits[]= {
	{ fMain, MainFormInit, &MainFormEventHandler },
	{ fEdit, EditFormInit, &EditFormEventHandler },
	{ fView, ViewFormInit, &ViewFormEventHandler },
	{ 0, NULL },
};

/*
**	Start
**
**	Start application (open database etc. etc.).
**
**	Returns false in case of errors.
*/
static Boolean
Start(void)
{
	if (PreferencesGet(&prefs)==false) {
		prefs.categoryIdx=dmAllCategories;
		prefs.alarmHour=0;
		prefs.alarmMinute=0;
		prefs.flags.showNotifications=0;
		prefs.flags.soundAlarm=0;
	} else
		categoryIdx=prefs.categoryIdx;

	EditOpen();
	UtilsDateFormat(&dateFormat, &dateSeparator);
	DBInit(&dbData, DBNAME);
	return DBOpen(&dbData, dmModeReadWrite, "Eventor Data");
}

/*
**	Stop
**
**	Stop application (close database etc. etc).
*/
static void
Stop(void)
{
	FrmSaveAllForms();
	FrmCloseAllForms();

	DBClose(&dbData);
	EditClose();
	prefs.categoryIdx=categoryIdx;
	PrefSetAppPreferences((UInt32)CRID, 0, APPVER, &prefs, sizeof(Preferences), true);
}

/*
**	PilotMain
*/
UInt32
PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
	UInt16 card;
	Boolean err;
	SysNotifyParamType notifyParam;
	SysNotifyParamType *snp;
	Preferences localPrefs;
	LocalID lid;
	UInt32 ftrValue;
	DB db;

	switch (cmd) {
	case sysAppLaunchCmdSystemReset:
		DBCreateDatabases();
	case sysAppLaunchCmdTimeChange:
		AlarmSetNextAlarm();
	case sysAppLaunchCmdAlarmTriggered:	/* FALL-THRU */
		break;

	case sysAppLaunchCmdSyncNotify:
		DBInit(&db, DBNAME);
		if ((DBOpen(&db, dmModeReadWrite, NULL))==true) {
			DmQuickSort(db.db, EventorCompareFunction, 0);
			DBClose(&db);
		} else
			DBCreateDatabases();

		AlarmSetNextAlarm();
		if (PreferencesGet(&localPrefs)==false)
			break;

		err=localPrefs.flags.showNotifications==1 ? true : false;
		UtilsSetNotification(err, sysNotifyLateWakeupEvent);
		UtilsSetNotification(err, eventorLateWakeup);
		break;

	case sysAppLaunchCmdDisplayAlarm:
		SysCurAppDatabase(&card, &lid);
		AlmSetAlarm(card, lid, 0, 0, true);

		/* If a feature exists, we already have a dialog open */
		if (FtrGet(CRID, FtrRecords, &ftrValue)==0)
			break;

		AlarmShowEvents(true);
		AlarmSetNextAlarm();
		break;

	case sysAppLaunchCmdFind:
		FSearch((FindParamsPtr)cmdPBP);
		break;

	case sysAppLaunchCmdGoTo:
		if (UtilsGetRomVersion(NULL, sysMakeROMVersion(3, 0, 0, sysROMStageRelease, 0))==false)
			FrmAlert(aBadRom);
		else {
			viewIdx=((GoToParamsType *)cmdPBP)->recordNum;
			if ((launchFlags&sysAppLaunchFlagNewGlobals)) {
				/* Called if selecting an entry outside eventor */
				if ((err=Start())==true) {
					viewReturn=true;
					FrmGotoForm(fView);
					EventLoop();
				}
				Stop();
			} else	/* Called, f.ex., if we have Eventor open */
				FrmGotoForm(fView);
		}
		break;
			
  	case sysAppLaunchCmdNormalLaunch:
		if (UtilsGetRomVersion(NULL, sysMakeROMVersion(3, 0, 0, sysROMStageRelease, 0))==false)
			FrmAlert(aBadRom);
		else {
			DBCreateDatabases();
			if ((err=Start())==true) {
				D(DPreLoad());
				FrmGotoForm(fMain);
				EventLoop();
			}

			Stop();
		}
		AlarmSetNextAlarm();
		//D(DSendAlarmSignal());
		break;

	case sysAppLaunchCmdNotify:
		/* If a feature exists, we already have a dialog open */
		if (FtrGet(CRID, FtrRecords, &ftrValue)==0)
			break;

		SysCurAppDatabase(&card, &lid);
		snp=(SysNotifyParamType *)cmdPBP;
		switch (snp->notifyType) {
		case sysNotifyLateWakeupEvent:
			if (PreferencesGet(&localPrefs)==false)
				break;

			if (localPrefs.flags.showNotifications==0)
				break;

			if ((PrefGetPreference(prefDeviceLocked)))
				UtilsSetNotification(true, sysNotifyDeviceUnlocked);
			else {
				notifyParam.notifyType=eventorLateWakeup;
				notifyParam.broadcaster=CRID;
				notifyParam.notifyDetailsP=NULL;
				notifyParam.handled=false;
				SysNotifyBroadcastDeferred(&notifyParam, 0);
			}
			break;
		case sysNotifyDeviceUnlocked:
			UtilsSetNotification(false, sysNotifyDeviceUnlocked);
		case eventorLateWakeup: 	/* FALL-THRU */
			AlarmShowEvents(false);
		default:	/* FALL-THRU */
			break;
		}
		break;

	default:	/* FALL-THRU */
		break;
	}

	return 0;
}

/*
**	EventLoop
**
**	Main event loop (event dispatcher).
*/
static void
EventLoop(void)
{
	UInt16 err, idx, fid;
	EventType ev;

	for (EVER) {
		EvtGetEvent(&ev, evtWaitForever);

		if (ev.eType==appStopEvent)
			break;

		if (ev.eType==nilEvent ||
		    SysHandleEvent(&ev)==true ||
		    MenuHandleEvent(NULL, &ev, &err)==true)
			continue;

		switch (ev.eType) {
		case frmLoadEvent:
			fid=ev.data.frmLoad.formID;
			currentForm=FrmInitForm(fid);
			FrmSetActiveForm(currentForm);

			for (idx=0; formInits[idx].id>0; idx++) {
				if (formInits[idx].id==fid) {
					if ((formInits[idx].init())==false)
						return;
					FrmSetEventHandler(currentForm, (FormEventHandlerPtr)formInits[idx].evh);
					break;
				}
			}

			ErrFatalDisplayIf(formInits[idx].id==0, "(EventLoop) The program tried to initialize a nonexisting form.");
			break;
		default:
			FrmDispatchEvent(&ev);
			break;
		}
	}
}

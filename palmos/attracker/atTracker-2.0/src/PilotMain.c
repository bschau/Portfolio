/*
 *	PilotMain.c
 */
#define MAIN
#include "Include.h"

/* protos */
static Boolean Start(void);
static void Stop(void);
static void EventLoop(void);
static Boolean GetPreferences(Preferences *);

/* globals */
static struct {
	UInt16 id;
	Boolean (*init)();
	FormEventHandlerType *evh;
} formInits[]= {
	{ fMain, fMainInit, fMainEH },
	{ fEdit, fEditInit, fEditEH },
	{ fCleanUp, fCleanUpInit, fCleanUpEH },
	{ fSummary, fSummaryInit, fSummaryEH },
	{ fArchive, fArchiveInit, fArchiveEH },
	{ fRestore, fRestoreInit, fRestoreEH },
	{ 0 },
};
static MemHandle bulletH=NULL;

/*
 * Start
 */
static Boolean
Start(void)
{
	if (GetPreferences(&prefs)==false) {
		prefs.categoryIdx=dmUnfiledCategory;
		prefs.defaultTime=0;
		prefs.flags=0;
		prefs.barColor=UIFieldTextHighlightForeground;
	}

	bulletH=DmGetResource(bitmapRsc, bBullet);
	ErrFatalDisplayIf(bulletH==NULL, "(Start) Cannot lock bitmap.");
	bmpBullet=MemHandleLock(bulletH);

	QuickTextGet();
	TimeHistoryGet(PrfTimeHistory);

	fMainFirstTime();
	DBInit(&dbData, DBNAME);
	return DBOpen(&dbData, dmModeReadWrite, true);
}

/*
 * Stop
 */
static void
Stop(void)
{
	FrmSaveAllForms();
	FrmCloseAllForms();

	DBClose(&dbData);

	fMainLastTime();

	TimeHistoryPut(PrfTimeHistory);
	QuickTextPut();

	if (bulletH) {
		MemHandleUnlock(bulletH);
		DmReleaseResource(bulletH);
	}

	PrefSetAppPreferences((UInt32)CRID, PrfApplication, APPVER, &prefs, sizeof(Preferences), true);
}

/*
 * PilotMain
 */
UInt32
PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
	DB db;

	switch (cmd) {
	case sysAppLaunchCmdSyncNotify:
		DBInit(&db, DBNAME);
		if (DBOpen(&db, dmModeReadWrite, false)==true) {
			DmQuickSort(db.db, TaskCompare, 0);
			DBClose(&db);
		}
		break;

	case sysAppLaunchCmdFind:
		FSearch((FindParamsType *)cmdPBP);
		break;

	case sysAppLaunchCmdGoTo:
		if (PMRomCheck()==true) {
			editCmd=((GoToParamsType *)cmdPBP)->recordNum|EditMaskIdx;
			if ((launchFlags&sysAppLaunchFlagNewGlobals)) {
				// Called if selecting an entry outside @Tracker
				if (Start()==true) {
					editCmd|=EditMaskAppStop;
					FrmGotoForm(fEdit);
					EventLoop();
				}
				Stop();
			} else
				FrmGotoForm(fEdit);
		}
		break;

  	case sysAppLaunchCmdNormalLaunch:
		if (PMRomCheck()==true) {
			DBCreateDatabase();
			if (Start()==true) {
				D(DPreload());
				if (fEditSaveRecord(false, NULL, NULL, NULL, NULL)==false)
					FrmGotoForm(fMain);
				else {
					editCmd=dmMaxRecordIndex|EditMaskPrf;
					FrmGotoForm(fEdit);
				}

				EventLoop();
			}

			Stop();
		}
	default:	/* FALL-THRU */
		break;
	}

	return 0;
}

/*
 * EventLoop
 *
 * Main event loop (event dispatcher).
 */
static void
EventLoop(void)
{
	UInt16 err, idx;
	EventType ev;

	for (EVER) {
		EvtGetEvent(&ev, evtWaitForever);

		if (ev.eType==appStopEvent)
			break;

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
					if ((formInits[idx].init) && (formInits[idx].init()==false))
						return;

					FrmSetEventHandler(currentForm, (FormEventHandlerType *)formInits[idx].evh);
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
 * GetPreferences
 *
 * Get preferences block for application.
 *
 *  -> p		Where to store preferences.
 *
 * Returns true if preferences was found, false otherwise.
 */
static Boolean
GetPreferences(Preferences *p)
{
	UInt16 len=sizeof(Preferences);
	Int16 retPrf;

	retPrf=PrefGetAppPreferences((UInt32)CRID, PrfApplication, p, &len, true);
	if (retPrf==noPreferenceFound || len!=sizeof(Preferences))
		return false;

	return true;
}

/*
 * PMRomCheck
 *
 * Returns true if current Rom Version is equal or better to required
 * Rom Version. False is returned otherwise.
 */
Boolean
PMRomCheck(void)
{
	UInt32 version;

	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version);

	if (version>=sysMakeROMVersion(3, 1, 0, sysROMStageRelease, 0)) {
		canDoubleTable=PMCanDoubleTable();
		WinScreenMode(winScreenModeGet, NULL, NULL, &version, NULL);

		if (version>3)
			canColor=true;
		else
			canColor=false;

		return true;
	}

	FrmAlert(aBadRom);
	return false;
}

/*
 * PMCanDoubleTable
 *
 * Can this device do double line tables?
 */
Boolean
PMCanDoubleTable(void)
{
	UInt32 version;

	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version);

	if (version>=sysMakeROMVersion(4, 0, 0, sysROMStageRelease, 0)) 
		return true;

	return false;
}

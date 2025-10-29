/*
 * PilotMain.c
 */
#define MAIN
#include "Include.h"

/* protos */
static Boolean RomCheck(void);
static Boolean Start(void);
static void Stop(void);
static void EventLoop(void);

/* globals */
static struct {
	UInt16 id;
	Boolean (*init)();
	FormEventHandlerPtr evh;
} formInits[]= {
	{ fMain, fMainInit, fMainEH },
	{ fStatistics, NULL, fStatisticsEH },
	{ 0 },
};

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
 * Start
 *
 * Start application (open database etc. etc.).
 *
 * Return false in case of errors.
 */
static Boolean
Start(void)
{
	DBInit(&dbData, DBNameData);

	return DBOpen(&dbData, dmModeReadWrite, true);
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

	DBClose(&dbData);
}

/*
 * PilotMain
 */
UInt32
PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
	DB db;

	switch (cmd) {
	case sysAppLaunchCmdNormalLaunch:
		if (RomCheck()==true) {
			DBCreateDatabase(DBNameData, 0);
			if (Start()==true) {
				FrmGotoForm(fMain);
				EventLoop();
			}
			Stop();
		}
		break;

	case sysAppLaunchCmdSyncNotify:
		DBInit(&db, DBNameData);

		if (DBOpen(&db, dmModeReadWrite, false)==true) {
			DmQuickSort(db.db, DateSort, 0);
			DBClose(&db);
		}
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
 * PMFormatDate
 *
 * Format a date to long format.
 *
 * <-  dst		Where to store formatted date.
 *  -> y		Year.
 *  -> m		Month.
 *  -> d		Day.
 */
void
PMFormatDate(Char *dst, Int16 y, Int16 m, Int16 d)
{
	switch (PrefGetPreference(prefDateFormat)) {
	case dfMDYWithSlashes:
		StrPrintF(dst, "%d-%d-%d", m, d, y);
		break;
	case dfDMYWithSlashes:
		StrPrintF(dst, "%d/%d/%d", d, m, y);
		break;
	case dfDMYWithDots:
		StrPrintF(dst, "%d.%d.%d", d, m, y);
		break;
	case dfDMYWithDashes:
		StrPrintF(dst, "%d-%d-%d", d, m, y);
		break;
	case dfYMDWithDots:
		StrPrintF(dst, "%d.%d.%d", y, m, d);
		break;
	case dfYMDWithDashes:
		StrPrintF(dst, "%d-%d-%d", y, m, d);
		break;
	case dfMDYWithDashes:
		StrPrintF(dst, "%d-%d-%d", m, d, y);
		break;
	case dfYMDWithSlashes:
	default:	/* Fall-Thru */
		StrPrintF(dst, "%d/%d/%d", y, m, d);
		break;
	}
}

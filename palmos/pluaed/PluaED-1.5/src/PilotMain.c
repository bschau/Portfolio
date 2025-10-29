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
static void PMRun(void);

/* globals */
static struct {
	UInt16 id;
	Boolean (*init)();
	FormEventHandlerPtr evh;
} formInits[]= {
	{ fMain, fMainInit, fMainEH },
	{ fEdit, fEditInit, fEditEH },
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
	if (PMGetPref(&prefs, sizeof(Prefs), PrfApplication)==false) {
		MemSet(&prefs, sizeof(Prefs), 0);
		prefs.flags=PFlgUnderlined|PFlgSearchFromTop;
		prefs.category=dmUnfiledCategory;
		prefs.sourceSize=DefaultSourceSize;
	}

	ui=UIInit();

	DBInit(&dbMemo, DBNameMemo);
	if (DBOpen(&dbMemo, dmModeReadWrite, true)==false)
		return false;

	if (prefs.category==dmUnfiledCategory) {
		if ((prefs.category=CategoryFind(dbMemo.db, "Plua"))==dmAllCategories)
			prefs.category=dmUnfiledCategory;
	}

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

	DBClose(&dbMemo);

	PMSetPref(&prefs, sizeof(Prefs), PrfApplication);
}

/*
 * PilotMain
 */
UInt32
PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
	switch (cmd) {
	case sysAppLaunchCmdNormalLaunch:
		if (RomCheck()==true) {
			if (Start()==true) {
				PMRun();
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
 * PMRun
 *
 * Run program.
 */
static void
PMRun(void)
{
	UInt16 len, rIdx, attrs;
	UInt32 uid;

	if (!prefs.uid) {
		FrmGotoForm(fMain);
		return;
	}

	len=DmNumRecordsInCategory(dbMemo.db, dmAllCategories);

	for (rIdx=0; rIdx<len; rIdx++) {
		if (DmRecordInfo(dbMemo.db, rIdx, &attrs, &uid, NULL)==errNone) {
			if (uid==prefs.uid) {
				if ((attrs&(dmRecAttrDelete|dmRecAttrSecret))==0) {
					fEditRun(rIdx);
					return;
				}

				break;
			} 
		}
	}

	FrmGotoForm(fMain);
}

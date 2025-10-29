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
	FormEventHandlerType *evh;
} formInits[]= {
	{ fMain, fMainInit, fMainEH },
	{ fExpenses, fExpensesInit, fExpensesEH },
	{ fPreferences, fPreferencesInit, fPreferencesEH },
	{ 0 },
};

#ifdef DEBUG
/*
 * HexDump
 *
 * Dump src data to log file.
 *
 *  -> src		Source data.
 *  -> srcLen		Source length.
 */
void
HexDump(UInt8 *src, UInt16 srcLen)
{
	Char hexChars[]="0123456789abcdef";
	UInt16 idx;

	for (idx=0; idx<srcLen; idx++) {
		DPrint("%c", hexChars[(*src>>4)&0x0f]);
		DPrint("%c", hexChars[*src&0x0f]);
		src++;
	}

	DPrint("\n");
}
#endif

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

	if (version<sysMakeROMVersion(4, 0, 0, sysROMStageRelease, 0)) {
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
#ifdef DEBUG
#if SYS==0
	gHostFile=HostFOpen("/tmp/Trams.log", "a+");
#else
	gHostFile=HostFOpen("c:\\temp\\Trams.log", "a+");
#endif
	ErrFatalDisplayIf(gHostFile==NULL, "(Start) HostFOpen failed");
	DPrint("======== Trams Started ====\n");
#endif

	LocOpen();

	if (PMGetPref(&prefs, sizeof(Prefs), PrfApplication, true)==false) {
		MemSet(&prefs, sizeof(Prefs), 0);
	}

	if (PMGetPref(&state, sizeof(State), PrfState, false)==false) {
		MemSet(&state, sizeof(State), 0);
		state.countryIndex=LocCountryToIndex("Denmark");
		state.currencyIndex=LocCurrencyToIndex("DKK");
	}

	LocValidate(&state);

	ui=UIInit();

	DBInit(&dbExpCat, "TrmsExpCat");
	return DBOpen(&dbExpCat, dmModeReadOnly, true);
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

	DBClose(&dbExpCat);

	LocClose();

	TLDestroyList();
	PMSetPref(&state, sizeof(State), PrfState, false);
	PMSetPref(&prefs, sizeof(Prefs), PrfApplication, true);
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
	switch (cmd) {
	case sysAppLaunchCmdNormalLaunch:
		if (RomCheck()==true) {
//			DBCreateDatabase(DBNameData, defCatTemplate);
			if (Start()==true) {
				if (*state.currentTER) {
					UInt16 idx=TLGetIndex(state.currentTER);

					if (idx==dmMaxRecordIndex)
						FrmGotoForm(fMain);
					else
						fExpensesRun(idx);
				} else
					FrmGotoForm(fMain);

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
 *  -> type	Get from saved preferences (=true).
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
 *  -> type	Set in saved preferences (=true).
 */
void
PMSetPref(void *p, UInt16 len, UInt16 id, Boolean type)
{
	PrefSetAppPreferences((UInt32)CRID, id, PREFSVER, p, len, type);
}

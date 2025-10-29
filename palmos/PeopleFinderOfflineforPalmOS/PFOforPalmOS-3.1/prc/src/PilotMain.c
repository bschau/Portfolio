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

/* macros */
typedef enum
{
	BtToggleLaunchCmdBtOn=sysAppLaunchCmdCustomBase, // Turn Bluetooth on
	BtToggleLaunchCmdBtOff, // Turn Bluetooth off
	BtToggleLaunchCmdBtToggle, // BT on->off or off->on
	BtToggleLaunchCmdGetBt // Return the current BT status (on=1 or off=0)
} BtToggleLaunchCodes;

/* globals */
static struct {
	UInt16 id;
	Boolean (*init)();
	FormEventHandlerPtr evh;
} formInits[]= {
	{ fMain, NULL, fMainEH },
	{ fNameSearch, fNameSearchInit, fNameSearchEH },
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

	if (version<sysMakeROMVersion(3, 5, 0, sysROMStageRelease, 0)) {
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
	gHostFile=HostFOpen("/tmp/PFO.log", "a+");
#else
	gHostFile=HostFOpen("c:\\tmp\\PFO.log", "a+");
#endif
	ErrFatalDisplayIf(gHostFile==NULL, "(Start) HostFOpen failed");
	DPrint("======== PFO Started ====\n");
#endif

	ui=UIInit();

	DBInit(&dbLocations, "PFOC_pfoc Locations", rootPath);
	DBOpen(&dbLocations, dmModeReadOnly, false);

	DBInit(&dbManagers, "PFOC_pfoc Managers", rootPath);
	DBOpen(&dbManagers, dmModeReadOnly, false);

	NSOpen();

	if (PMGetPref(&prefs, sizeof(Prefs), PrfApplication, true)==false)
		MemSet(&prefs, sizeof(Prefs), 0);

	if (PMGetPref(&state, sizeof(State), PrfState, false)==false)
		MemSet(&state, sizeof(State), 0);

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

	DBClose(&dbLocations);
	DBClose(&dbManagers);

	NSClose();
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
			if (Start()==true) {
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
 *  -> saved	Get from saved preferences (yes = true, no = false).
 *
 * Returns true if preferences was gotten, false otherwise.
 */
Boolean
PMGetPref(void *p, UInt16 len, UInt16 id, Boolean saved)
{	
	UInt16 l=len;
	Int16 retPrf;

	retPrf=PrefGetAppPreferences((UInt32)CRID, id, p, &l, saved);
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
 *  -> saved	Store in saved preferences (yes = true, no = false).
 */
void
PMSetPref(void *p, UInt16 len, UInt16 id, Boolean saved)
{
	PrefSetAppPreferences((UInt32)CRID, id, PREFSVER, p, len, saved);
}

/*
 * PMBtToggleEnabled
 *
 * Returns true if this device has BtToggle-2.x or greater installed.
 *
 * <-  dstCard		Where to store card (or NULL).
 * <-  dstId		Where to store local id (or NULL).
 */
Boolean
PMBtToggleEnabled(UInt16 *dstCard, LocalID *dstId)
{
	DmSearchStateType sI;
	LocalID id;
	UInt16 card, v;

	if (DmGetNextDatabaseByTypeCreator(true, &sI, 0, (UInt32)'BTTG', true, &card, &id)!=errNone)
		return false;

	if (DmDatabaseInfo(card, id, NULL, NULL, &v, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)!=errNone)
		return false;

	if (v<2)
		return false;

	if (dstCard)
		*dstCard=card;

	if (dstId)
		*dstId=id;


	return true;
}

/*
 * PMBtToggleOn
 *
 * Turn bluetooth on.
 */
Err
PMBtToggleOn(void)
{
	UInt16 card;
	LocalID id;

	if (PMBtToggleEnabled(&card, &id)==true) {
		Boolean btStatus=false;
		UInt32 result;

		if ((SysAppLaunch(card, id, 0, BtToggleLaunchCmdGetBt, &btStatus, &result)==errNone) && (btStatus==false))
			return SysAppLaunch(card, id, 0, sysAppLaunchCmdCustomBase, NULL, &result);
	}

	return errNone;
}

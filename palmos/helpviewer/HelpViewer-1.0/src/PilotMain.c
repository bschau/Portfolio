/**
 * PilotMain
 */
#define MAIN
#include "Include.h"

/* protos */
static Boolean Start(void);
static void Stop(void);
static void EventLoop(void);
static Boolean RomCheck(void);
static void FindHelpFiles(void);
static LocalID FindDatabase(UInt32, UInt32, Char *, UInt16 *);

/* globals */
static struct {
	UInt16 id;
	Boolean (*init)();
	FormEventHandlerPtr evh;
} formInits[]= {
	{ fMain, fMainInit, fMainEH },
	{ fTopic, fTopicInit, fTopicEH },
	{ 0 },
};

/**
 * Start application (open database etc. etc.).
 *
 * @return false in case of errors.
 */
static Boolean
Start(void)
{
	UInt16 len=sizeof(State);
	Int16 retPrf;

#ifdef DEBUG
#if SYS==0
	gHostFile=HostFOpen("/tmp/HelpViewer.log", "a+");
#else
	gHostFile=HostFOpen("c:\\temp\\HelpViewer.log", "a+");
#endif
	ErrFatalDisplayIf(gHostFile==NULL, "(Start) HostFOpen failed");
	DPrint("======== HelpViewer Started ====\n");
#endif

	ui=UIInit();

	retPrf=PrefGetAppPreferences((UInt32)CRID, PrfState, &state, &len, false);
	if (retPrf==noPreferenceFound || len!=sizeof(State)) {
		state.helpFile=0;
	}

	FindHelpFiles();
	if (ph1ID && ph2ID)
		fMainOpen(state.helpFile);
	else if (ph1ID)
		fMainOpen(0);
	else if (ph2ID)
		fMainOpen(1);
	else {
		FrmAlert(aNoHelpFiles);
		return false;
	}

	return true;
}

/**
 * Stop application (close database etc. etc).
 */
static void
Stop(void)
{
	FrmSaveAllForms();
	FrmCloseAllForms();

	fMainClose();

	PrefSetAppPreferences((UInt32)CRID, PrfState, APPVER, &state, sizeof(State), false);

#ifdef DEBUG
	if (gHostFile)
		HostFClose(gHostFile);
#endif
}

/**
 * PilotMain
 */
UInt32
PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
	switch (cmd) {
  	case sysAppLaunchCmdNormalLaunch:
		if (RomCheck()==true) {
			if (Start()==true)
				EventLoop();

			Stop();
		}
	default:	/* FALL-THRU */
		break;
	}

	return 0;
}

/**
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
					if ((formInits[idx].init())==false)
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

/**
 * Check ROM version in device.
 *
 * @return true if current Rom Version is equal or better to required
 * Rom Version. False is returned otherwise.
 */
static Boolean
RomCheck(void)
{
	UInt32 romVersion;

	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);

	if (romVersion>=sysMakeROMVersion(3, 0, 0, sysROMStageRelease, 0))
		return true;

	FrmAlert(aBadRom);
	return false;
}

/**
 * Find the Plua Help Files.
 */
static void
FindHelpFiles(void)
{
	ph1ID=FindDatabase((UInt32)'Help', (UInt32)'Plua', ph1Name, &ph1Card);
	ph2ID=FindDatabase((UInt32)'Help', (UInt32)'LuaP', ph2Name, &ph2Card);
}

/**
 * Find a database by type, creator id and name. Searches all cards.
 *
 * @param type Type of database.
 * @param crid Creator ID.
 * @param name Database name.
 * @param card Where to store card #.
 * @return LocalID or 0 if not found.
 */
static LocalID
FindDatabase(UInt32 type, UInt32 crid, Char *name, UInt16 *card)
{
	UInt16 cards=MemNumCards(), idx;
	LocalID id;

	for (idx=0; idx<cards; idx++) {
		if ((id=DmFindDatabase(idx, name))) {
			*card=idx;
			return id;
		}
	}

	return 0;
}

/**
 * PilotMain
 */
#define MAIN
#include "Include.h"

/* protos */
static Boolean Start(void);
static void Stop(void);
static void EventLoop(void);
static void GetGame(void);
static Boolean RomCheck(void);

/* globals */
static struct {
	UInt16 id;
	Boolean (*init)();
	FormEventHandlerPtr evh;
} formInits[]= {
	{ fMain, fMainInit, fMainEH },
	{ fInGame, fInGameInit, fInGameEH },
	{ fWinner, fWinnerInit, fWinnerEH },
	{ 0 },
};
static MemHandle miscFontH;

/**
 * Start application (open database etc. etc.).
 *
 * @return false in case of errors.
 */
static Boolean
Start(void)
{
	UInt16 len=sizeof(Prefs);
	Int16 retPrf;

#ifdef DEBUG
#if SYS==0
	gHostFile=HostFOpen("/tmp/PXSc.log", "a+");
#else
	gHostFile=HostFOpen("c:\\temp\\PXSc.log", "a+");
#endif
	ErrFatalDisplayIf(gHostFile==NULL, "(Start) HostFOpen failed");
	DPrint("======== PXSc Started ====\n");
#endif

	DBInit(&dbData, DBNAME);
	if (DBOpen(&dbData, dmModeReadWrite, true)==false)
		return false;

	ui=UIInit();

	retPrf=PrefGetAppPreferences((UInt32)CRID, PrfApplication, &prefs, &len, false);
	if (retPrf==noPreferenceFound || len!=sizeof(Prefs)) {
		prefs.lastMemoTitle[0]='\x00';
		prefs.catMemoIdx=dmUnfiledCategory;
	}

	miscFontH=DmGetResource('NFNT', 1000);
	ErrFatalDisplayIf(miscFontH==NULL, "(Start) miscFont not found");
	FntDefineFont(miscFont, MemHandleLock(miscFontH));

//#if DEBUG==0
	if (DmNumRecords(dbData.db)>0)
		GetGame();
	else
		FrmGotoForm(fMain);
	/*
#else
	MemSet(&game, sizeof(PXScGame), 0);
	game.gameStarted=TimGetSeconds()-10;
	game.noOfPlayers=2;
	game.phase[0]=10;
	game.phase[1]=10;
	game.oldPhase[0]=9;
	game.oldPhase[1]=9;
	game.scoreDone[0]=0;
	game.scoreDone[1]=0;
	StrNCopy(&game.names[0][0], "Brian", PlayerNameLength);
	StrNCopy(&game.names[1][0], "Charlotte", PlayerNameLength);
	game.scores[0]=10;
	game.scores[1]=20;
	valid=1;
	fInGameRun();
#endif
	*/

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

	DBClose(&dbData);

	StringFree();

	if (miscFontH) {
		MemHandleUnlock(miscFontH);
		DmReleaseResource(miscFontH);
	}

	if (valid)
		PrefSetAppPreferences((UInt32)CRID, PrfGame, APPVER, &game, sizeof(PXScGame), false);

	PrefSetAppPreferences((UInt32)CRID, PrfApplication, APPVER, &prefs, sizeof(Prefs), false);

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
			DBCreateDatabase();
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
 * Retrieve a possibly saved game.
 */
static void
GetGame(void)
{
	UInt16 len=sizeof(PXScGame);
	Int16 retPrf;

	retPrf=PrefGetAppPreferences((UInt32)CRID, PrfGame, &game, &len, false);
	if (retPrf==noPreferenceFound || len!=sizeof(PXScGame)) {
		valid=0;
		FrmGotoForm(fMain);
	} else {
		valid=1;
		fInGameRun();
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

	if (romVersion>=sysMakeROMVersion(3, 1, 0, sysROMStageRelease, 0))
		return true;

	FrmAlert(aBadRom);
	return false;
}

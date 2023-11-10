/*
 * PilotMain.c
 */
#define MAIN
#include "Include.h"

/* protos */
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
	{ fNewGame, fNewGameInit, fNewGameEH },
	{ fScores, fScoresInit, fScoresEH },
	{ 0 },
};

/*
 * Start
 */
static Boolean
Start(void)
{
	UInt32 romVersion;
	UInt16 len;
	Int16 retPrf;
	
	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
	if (romVersion<0x02003000) {
		FrmCustomAlert(aRom20, NULL, NULL, NULL);
		return false;
	}

	RulesAllocate();

	if (DBOpen()==false)
		return false;

	len=sizeof(KasinoPref);
	retPrf=PrefGetAppPreferences((UInt32)CRID, 0, &pref, &len, true);
	if (retPrf==noPreferenceFound || len!=sizeof(KasinoPref)) {
		pref.noOfRounds=0;
		pref.noOfPoints=0;
		pref.rulesChapter=0;
	}

	return true;
}

/*
 * Stop
 */
static void
Stop(void)
{
	FrmSaveAllForms();
	FrmCloseAllForms();

	PrefSetAppPreferences((UInt32)CRID, 0, APPVER, &pref, sizeof(KasinoPref), true);

	if (game.noOfPlayers==0)
		MemSet(&game, sizeof(KasinoGame), 0);

	DBSetGame();
	DBClose();
	RulesDeallocate();
}

/*
 * PilotMain
 */
UInt32
PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
	Boolean err;

	switch (cmd) {
  	case sysAppLaunchCmdNormalLaunch:
		if ((err=Start())==true) {
			if (game.noOfPlayers==0)
				FrmGotoForm(fNewGame);
			else
				FrmGotoForm(fMain);

			EventLoop();
		}

		Stop();
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

		if (ev.eType==nilEvent ||
		    SysHandleEvent(&ev)==true ||
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

			if (formInits[idx].id==0) {
				FrmCustomAlert(aNoSuchForm, NULL, NULL, NULL);
				return;
			}
			break;
		default:
			FrmDispatchEvent(&ev);
			break;
		}
	}
}

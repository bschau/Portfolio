/*
 * PilotMain.c
 */
#define MAIN
#include "Include.h"

/* protos */
static Boolean Start(void);
static void Stop(void);
static void EventLoop(void);

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
	(void)SysRandom(TimGetTicks());

	if (UtilsGetPref(&level, sizeof(level), PrfApplication)==false)
		level=0;

	SssGet();

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

	SssSet();

	UtilsSetPref(&level, sizeof(level), PrfApplication);
}

/*
 * PilotMain
 */
UInt32
PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
	if (cmd==sysAppLaunchCmdNormalLaunch) {
		if (UtilsRomCheck()==true) {
			if (Start()==true) {
				FrmGotoForm(fGame);
				EventLoop();
			}
			Stop();
		}
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
	EventType ev;
	UInt16 err;

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

			if (currentFormID==fGame) {
				if ((fGameInit()))
					FrmSetEventHandler(currentForm, fGameEH);
				else
					return;
			} else
				ErrFatalDisplay("(EventLoop) The program tried to initialize a non-existing form.");
			break;
		default:
			FrmDispatchEvent(&ev);
			break;
		}
	}
}

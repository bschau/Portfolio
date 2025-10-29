/*
 * PilotMain.c
 */
#define MAIN
#include "Include.h"

/* protos */
static void EventLoop(void);

/*
 * PilotMain
 */
UInt32
PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
	if (cmd==sysAppLaunchCmdNormalLaunch) {
		FrmGotoForm(fMain);
		EventLoop();
		FrmSaveAllForms();
		FrmCloseAllForms();
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
	UInt16 err;
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
			if (ev.data.frmLoad.formID==fMain) {
				currentForm=FrmInitForm(ev.data.frmLoad.formID);
				FrmSetActiveForm(currentForm);

				FrmSetEventHandler(currentForm, (FormEventHandlerType *)fMainEH);
			} else
				ErrFatalDisplay("(EventLoop) The program tried to initialize a non-existing form.");

			break;
		default:
			FrmDispatchEvent(&ev);
			break;
		}
	}
}

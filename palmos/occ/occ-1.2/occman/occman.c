#define MAIN
#include "occman.h"

/* protos */
static Boolean Start(void);
static void Stop(void);
static void EventLoop(void);

/* globals */
static struct {
	UInt16 id;
	Boolean (*init)();
	FormEventHandlerPtr evh;
} formInits[]= {
	{ fMain, MainFormInit, MainFormEventHandler },
	{ 0, NULL, NULL },
};

/*
**	Start
**
**	Initialize occman (Get all executables with same Crid as OccMan).
**	Return true if successful, false otherwise.
*/
static Boolean
Start(void)
{
	OCCDiscoverAll(&occList);
	return true;
}

/*
**	Stop
**
**	Deallocate everything .. clean up.
*/
static void
Stop(void)
{
	OCCListDestroy(&occList);
	FrmSaveAllForms();
	FrmCloseAllForms();
}

/*******************************************************************************
*
*	PilotMain
*
*	Main for pilot applications.
*/
UInt32
PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
	SysAppLaunchCmdOpenDBType *odb=(SysAppLaunchCmdOpenDBType *)cmdPBP;
	UInt32 rv;

	/* we need at least a 3.1 device */
	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &rv);
	if (rv<sysMakeROMVersion(3, 1, 0, sysROMStageRelease, 0)) {
		FrmCustomAlert(aRom31, NULL, NULL, NULL);
		return 0;
	}

	switch (cmd) {
	case sysAppLaunchCmdNormalLaunch:
		if (Start()==true) {
			FrmGotoForm(fMain);
			EventLoop();
			Stop();
		}
		break;

	case sysAppLaunchCmdOpenDB:
		OCCLaunch(odb->cardNo, odb->dbID);
		break;
	}

	return 0;
}

/*
**	EventLoop
**
**	Main dispatch routine.
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

			ErrFatalDisplayIf(formInits[idx].id==0, "(EventLoop) No such form. Shoot the programmer!");
			break;

		default:
			FrmDispatchEvent(&ev);
			break;
		}
	}
}

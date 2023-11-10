#define MAIN
#include "loginman.h"

/* protos */
static Boolean romcheck(void);
static void eventloop(void);

/*******************************************************************************
*
*	romcheck
*
*	Assure that the program is running on a v3.1 device or better.
*/
static Boolean
romcheck(void)
{
	UInt32 rv;

	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &rv);
	if (rv>=sysMakeROMVersion(3, 1, 0, sysROMStageRelease, 0))
		return true;

	FrmCustomAlert(alertrom31, NULL, NULL, NULL);
	return false;
}

/*******************************************************************************
*
*	PilotMain
*
*	Main for pilot applications.	This is the only place with UpperLower
*	case mix in function names.    It's ugly :)
*/
UInt32
PilotMain(UInt16 cmd,
          void *cmdPBP,
          UInt16 launchFlags)
{
	DmOpenRef db;
	Boolean err;

	err=romcheck();
	if (err==true) {
		switch (cmd) {
		case sysAppLaunchCmdURLParams:
			if (cmdPBP) {
				db=opendatabase();
				if (db) {
					clipping(db, (UInt8 *)cmdPBP, true),
					closedatabase(db);
				}
				FrmSaveAllForms();
				FrmCloseAllForms();
			}
			break;
  		case sysAppLaunchCmdNormalLaunch:
			db=opendatabase();
			if (db) {
				if (cmdPBP)
					clipping(db, (UInt8 *)cmdPBP, false);
				else {
					qdb=db;
					FrmGotoForm(formmain);
					eventloop();
				}
				closedatabase(db);
				FrmSaveAllForms();
				FrmCloseAllForms();
			}
			break;

		default:
			break;
		}
	}

	return 0;
}

/*******************************************************************************
*
*	eventloop
*
*	Main dispatch routine.
*/
static void
eventloop(void)
{
	UInt16 formid, err;
	FormPtr form;
	EventType ev;

	for (EVER) {
		EvtGetEvent(&ev, evtWaitForever);

		if (ev.eType==appStopEvent)
			break;

		if (ev.eType==nilEvent)
			continue;

		if (SysHandleEvent(&ev))
			continue;

		if (MenuHandleEvent(NULL, &ev, &err))
			continue;

		switch (ev.eType) {
		case frmLoadEvent:
			formid=ev.data.frmLoad.formID;
			form=FrmInitForm(formid);
			FrmSetActiveForm(form);
			switch (formid) {
			case formmain:
				formmaininit(form);
				break;
			case formedit:
				formeditinit(form);
				break;
			case formview:
				formviewinit(form);
				break;
			}
			break;

		default:
			FrmDispatchEvent(&ev);
			break;
		}
	}
}

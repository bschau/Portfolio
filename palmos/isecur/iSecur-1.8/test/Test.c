/**
 * Test.c
 */
#include <PalmOS.h>
#include "Resources.h"
#define MAIN
#include "PublicApi.h"

/* macros */
#define EVER ;;
#define PREFSVER 1

/* protos */
static Boolean RomCheck(void);
static Boolean Start(void);
static void Stop(void);
static void EventLoop(void);
static Boolean EH(EventType *);

/* globals */
static FormType *currentForm;
static UInt16 currentFormID;
static UInt16 libRefNum;

/**
 * Returns true if the Rom Version is equal to or better than the required
 * Rom Version. False is returned otherwise.
 */
static Boolean
RomCheck(void)
{
	UInt32 romVersion;

	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);

	if (romVersion<sysMakeROMVersion(2, 0, 0, sysROMStageRelease, 0)) {
		FrmAlert(aBadRom);
		return false;
	}

	return true;
}

/**
 * Start application (open database etc. etc.).
 *
 * @return false in case of errors.
 */
static Boolean
Start(void)
{
	UInt32 v;

#ifdef DEBUG
#if SYS==0
	gHostFile=HostFOpen("/tmp/Test.log", "a+");
#else
	gHostFile=HostFOpen("c:\\tmp\\Test.log", "a+");
#endif
	ErrFatalDisplayIf(gHostFile==NULL, "(Start) HostFOpen failed");
	DPrint("======== Test Started ====\n");
#endif

	libRefNum=sysInvalidRefNum;
	if (SysLibFind(KeyboardLibName, &libRefNum)!=errNone) {
		if (SysLibLoad('kbde', 'iSec', &libRefNum)!=errNone)
			ErrFatalDisplay("No Keyboard library");
	}

	KeyboardLibAPIVersion(libRefNum, &v);
	ErrFatalDisplayIf(v!=1, "Invalid library");

	if (KeyboardLibOpen(libRefNum, 0)!=errNone) {
		DPrint("KeyboardLibOpen failed\n");
		return false;
	}

	return true;
}

/**
 * Stop
 *
 * Application shutdown.
 */
static void
Stop(void)
{
	FrmSaveAllForms();
	FrmCloseAllForms();

	if (libRefNum!=sysInvalidRefNum) {
		UInt16 useCount;

		if ((KeyboardLibClose(libRefNum, &useCount)==0) && (!useCount))
			SysLibRemove(libRefNum);
	}
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
			if (Start()==true) {
				FrmGotoForm(fMain);
				EventLoop();
			}
			Stop();
		}
	default:
		break;
	}

	return 0;
}

/**
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
			return;

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

			if (currentFormID==fMain)
				FrmSetEventHandler(currentForm, (FormEventHandlerPtr)EH);
			else
				ErrFatalDisplay("(EventLoop) The program tried to initialize a non-existing form.");
			break;
		default:
			FrmDispatchEvent(&ev);
			break;
		}
	}
}

/**
 * EventHandler.
 */
static Boolean
EH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cMainKeyboard:
			{
				FormType *frm=FrmGetActiveForm();
				FormActiveStateType frmSave;
				MemHandle mh, sh;
				FieldType *fld;

				FrmSaveActiveState(&frmSave);
				KeyboardLibRun(libRefNum, &mh);

				FrmRestoreActiveState(&frmSave);

				if (mh) {
					fld=FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, cMainField));
					sh=FldGetTextHandle(fld);
					FldSetTextHandle(fld, mh);
					FldDrawField(fld);
					if (sh)
						MemHandleFree(sh);
				}
			}
			return true;
		}
		break;

	case keyDownEvent:
//		switch (ev->data.keyDown.chr) {
//		}

	default:	/* FALL-THRU */
		break;
	}

	return false;
}

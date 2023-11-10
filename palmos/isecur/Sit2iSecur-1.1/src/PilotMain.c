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
	{ fPasswords, NULL, fPasswordsEH },
	{ fDone, NULL, fDoneEH },
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
	gHostFile=HostFOpen("/tmp/Sit2iSecur.log", "a+");
#else
	gHostFile=HostFOpen("c:\\tmp\\Sit2iSecur.log", "a+");
#endif
	ErrFatalDisplayIf(gHostFile==NULL, "(Start) HostFOpen failed");
	DPrint("======== Sit2iSecur Started ====\n");
#endif

	AESOpen();

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

	fReoClose();
	AESClose();

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

/*
 * PilotMain.c
 */
#define MAIN
#include "Include.h"

/* protos */
static Boolean Start(void);
static void Stop(void);
static void EventLoop(void);
static Boolean PMRomCheck(void);

/* globals */
static struct {
	UInt16 id;
	Boolean (*init)();
	FormEventHandlerType *evh;
} formInits[]= {
	{ fGame, fGameInit, fGameEH },
	{ fMain, fMainInit, fMainEH },
	{ fQuizOver, fQuizOverInit, fQuizOverEH },
	{ fScores, fScoresInit, fScoresEH },
	{ 0 },
};
static Char *txtTimeIsUp="Time is up!";
static MemHandle recordH;

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
	gHostFile=HostFOpen("/tmp/MathAce.log", "a+");
#else
	gHostFile=HostFOpen("c:\\tmp\\MathAce.log", "a+");
#endif
	ErrFatalDisplayIf(gHostFile==NULL, "(Start) DOpen failed");
	DPrint("======== MathAce Started ====\n");
#endif

	(void)SysRandom(TimGetTicks());

	if (PMGetPref(&prefs, sizeof(Prefs), PrfApplication)==false) {
		prefs.operation=PrfQuizType|PrfOperationAdd|PrfOperationSub|PrfOperationMul|PrfOperationDiv|PrfMaxType;
		prefs.maxOperand=10;
		prefs.maxAnswer=10;
		prefs.timedMins=0;
		prefs.memoCategory=dmUnfiledCategory;
	}

	SssGet();
	deleteStringDelay=-1;

	recordH=MemHandleNew(sizeof(Record)*MaxRecords);
	ErrFatalDisplayIf(recordH==NULL, "(Start) Out of memory.");
	record=MemHandleLock(recordH);
	MemSet(record, sizeof(Record)*MaxRecords, 0);

	PMGetPref(record, sizeof(Record)*MaxRecords, PrfRecords);
	prefsRunning=false;

	if (session) {
		if (session->stopped)
			FrmGotoForm(fQuizOver);
		else
			FrmGotoForm(fGame);
	} else
		FrmGotoForm(fMain);

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

	PMSetPref(record, sizeof(Record)*MaxRecords, PrfRecords);
	PMSetPref(&prefs, sizeof(Prefs), PrfApplication);

	MemHandleFree(recordH);

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
	Prefs p;

	switch (cmd) {
	case sysAppLaunchCmdNormalLaunch:
		if (PMRomCheck()==true) {
			if (Start()==true)
				EventLoop();
			Stop();
		}
		break;

	case sysAppLaunchCmdExgAskUser:
		((ExgAskParamType *)cmdPBP)->result=exgAskOk;
		if ((launchFlags&sysAppLaunchFlagSubCall)) {
			if (prefsRunning==true) {
				FrmAlert(aCannotReceive);
				((ExgAskParamType *)cmdPBP)->result=exgAskCancel;
			}
		}
		break;

	case sysAppLaunchCmdExgReceiveData:
		if (IRReceive((ExgSocketType *)cmdPBP, &p)==true) {
			PMSetPref(&p, sizeof(Prefs), PrfApplication);
			if ((launchFlags&sysAppLaunchFlagSubCall))
				MemMove(&prefs, &p, sizeof(Prefs));
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
	UInt32 prevTime=0, nTime, secs;
	EventType ev;
	UInt16 err, w;
	RectangleType r;
	Char buffer[16];

	for (EVER) {
		EvtGetEvent(&ev, 1);
		if (ev.eType==appStopEvent)
			break;

		if (session && session->timedSecs>0 && inGame) {
			Boolean endQuiz=false;

			nTime=TimGetSeconds();
			if (nTime-prevTime>=1) {
				prevTime=nTime;
				secs=nTime-session->started;

				if (secs>=session->timedSecs)
					endQuiz=true;

				secs=(session->started+session->timedSecs)-nTime;

				RctSetRectangle(&r, 60, 146, 40, 12);
				StrPrintF(buffer, "%u:%02u", (UInt16)(secs/60), (UInt16)(secs%60));
				err=StrLen(buffer);
				w=FntCharsWidth(buffer, err);
				WinEraseRectangle(&r, 0);

				WinDrawChars(buffer, err, r.topLeft.x+(r.extent.x-w)/2, r.topLeft.y);

			}

			if (endQuiz) {
				UInt32 cutTime=TimGetSeconds()+3;
				FontID fID=FntSetFont(boldFont);
				UInt16 idx;

				RectangleType r;

				CtlSetEnabled(UIFormGetObject(cGameStop), 0);
				CtlSetEnabled(UIFormGetObject(cGameAnswer), 0);

				for (idx=cGameBut0; idx<=cGameButOK; idx++)
					CtlSetEnabled(UIFormGetObject(idx), 0);

				InsPtEnable(false);

				RctSetRectangle(&r, 30, 70, 100, 20);
				WinEraseRectangle(&r, 0);
				WinDrawRectangleFrame(boldRoundFrame, &r);

				WinDrawChars(txtTimeIsUp, 11, (160-FntCharsWidth(txtTimeIsUp, 11))/2, (160-FntCharHeight())/2);
				FntSetFont(fID);

				while (TimGetSeconds()<cutTime) {
					EvtGetEvent(&ev, SysTicksPerSecond());
					if (ev.eType==appStopEvent)
						return;

					if (ev.eType==nilEvent ||
					    SysHandleEvent(&ev)==true ||
					    MenuHandleEvent(NULL, &ev, &err)==true)
						continue;
				}

				fGameEndQuiz();
				continue;
			}
		}

		if (deleteStringDelay==0 && inGame) {
			RctSetRectangle(&r, 0, 20, 160, 16);
			WinEraseRectangle(&r, 0);
			deleteStringDelay=-1;
		} else if (deleteStringDelay>0)
			deleteStringDelay--;

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

			for (w=0; formInits[w].id>0; w++) {
				if (formInits[w].id==currentFormID) {
					if ((formInits[w].init!=NULL) && ((formInits[w].init())==false))
						return;

					FrmSetEventHandler(currentForm, (FormEventHandlerPtr)formInits[w].evh);
					break;
				}
			}

			ErrFatalDisplayIf(formInits[w].id==0, "(EventLoop) The program tried to initialize a non-existing form.");
			break;
		default:
			FrmDispatchEvent(&ev);
			break;
		}
	}
}

/*
 * PMRomCheck
 *
 * Returns true if the Rom Version is equal to or better than the required
 * Rom Version. False is returned otherwise.
 */
static Boolean
PMRomCheck(void)
{
	UInt32 version;

	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version);

	if (version<sysMakeROMVersion(3, 0, 0, sysROMStageRelease, 0)) {
		FrmAlert(aBadRom);
		return false;
	}

	return true;
}

/*
 * PMGetPref
 *
 * Get preference #.
 *
 *  -> p	Destination.
 *  -> len	Length of destination.
 *  -> id	Preference id.
 *
 * Returns true if preferences was gotten, false otherwise.
 */
Boolean
PMGetPref(void *p, UInt16 len, UInt16 id)
{	
	UInt16 l=len;
	Int16 retPrf;

	retPrf=PrefGetAppPreferences((UInt32)CRID, id, p, &l, false);
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
 */
void
PMSetPref(void *p, UInt16 len, UInt16 id)
{
	PrefSetAppPreferences((UInt32)CRID, id, APPVER, p, len, false);
}

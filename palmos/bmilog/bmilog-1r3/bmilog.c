/*
**	bmilog.c
**
**	Main entry points and form handler for BMILog.
*/
#define MAIN
#include "bmilog.h"

/* macros */
typedef struct {
} Resources;

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
	{ fMain, MainInit, &MainEventHandler },
	{ fHistoryKgCm, HistoryInit, &HistoryEventHandler },
	{ fHistoryLbsInch, HistoryInit, &HistoryEventHandler },
	{ fConversion, ConversionInit, &ConversionEventHandler },
	{ fQueryFindHeight, QueryFindHeightInit, &QueryFindHeightEventHandler },
	{ fQueryFindWeight, QueryFindWeightInit, &QueryFindWeightEventHandler },
	{ 0, NULL },
};

static struct {
	DmResType type;
	DmResID id;
	MemHandle *dst;
	UInt16 offSet;
} resources[]= {
	{ strRsc, strSelectDate, &resStrSelectDate, 0 },
	{ strRsc, strWhatIsTitle, &resStrWhatIsTitle, 0 },
	{ strRsc, strWhatIsText, &resStrWhatIsText, 0 },
	{ strRsc, strBMITableTitle, &resStrBMITableTitle, 0 },
	{ strRsc, strBMITableTextAmerican, &resStrBMITableTextAmerican, 0 },
	{ strRsc, strBMITableTextEuropean, &resStrBMITableTextEuropean, 0 },
	{ strRsc, strCm, &resStrCm, 0 },
	{ strRsc, strFeet, &resStrFeet, 0 },
	{ strRsc, strInch, &resStrInch, 0 },
	{ strRsc, strKg, &resStrKg, 0 },
	{ strRsc, strLbs, &resStrLbs, 0 },
	{ strRsc, strAbout, &resStrAbout, 0 },
	{ strRsc, strName, &resStrName, 0 },
	{ strRsc, strDate, &resStrDate, 0 },
	{ strRsc, strBMI, &resStrBMI, 0 },
	{ strRsc, strFeetInch, &resStrFeetInch, 0 },
	{ bitmapRsc, bmpNegativeBW, &bmpNegativeH, OffSetToColor },
	{ bitmapRsc, bmpNullBW, &bmpNullH, OffSetToColor },
	{ bitmapRsc, bmpPositiveBW, &bmpPositiveH, OffSetToColor },
	{ 0, 0, NULL, 0 },
}; 

/*
**	Start
**
**	Start application (open database etc. etc.).
**
**	Returns false in case of errors.
*/
static Boolean
Start(void)
{
	Boolean origColor;
	UInt16 idx;
	DmResID resID;
	MemHandle *dst;

	WinScreenMode(winScreenModeGet, NULL, NULL, NULL, &origColor);
	for (idx=0; resources[idx].dst; idx++) {
		resID=resources[idx].id;
		if (origColor==true)
			resID+=resources[idx].offSet;

		dst=resources[idx].dst;

		*dst=DmGetResource(resources[idx].type, resID);
		ErrFatalDisplayIf(*dst==NULL, "(Start) Cannot lock resources.");
	}

	DBInit(&dbData, DBNAME);
	
	return DBOpenDatabase(&dbData, dmModeReadWrite, "Person");
}

/*
**	Stop
**
**	Stop application (close database etc. etc).
*/
static void
Stop(void)
{
	UInt16 idx;
	MemHandle *src;

	FrmSaveAllForms();
	FrmCloseAllForms();

	DBClose(&dbData);

	PrefSetAppPreferences((UInt32)CRID, 0, APPVER, &prefs, sizeof(Preferences), true);
	for (idx=0; resources[idx].dst; idx++) {
		src=resources[idx].dst;
		if (*src)
			DmReleaseResource(*src);
	}
}

/*
**	PilotMain
*/
UInt32
PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
	DB db;

	switch (cmd) {
	case sysAppLaunchCmdSyncNotify:
		DBInit(&db, DBNAME);
		if ((DBOpen(&db, dmModeReadWrite))==true) {
			DmQuickSort(db.db, DBCompare , 0);
			DBClose(&db);
		} else
			DBCreateDatabase();
		break;
		
  	case sysAppLaunchCmdNormalLaunch:
		if (UtilsRomOK()==false)
			break;

		if (PreferencesGet(&prefs)==false) {
			prefs.bmiTable=BmiTableEuropean;
			prefs.format=FormatKgCm;
		}

		DBCreateDatabase();
		if (Start()==true) {
			FrmGotoForm(fMain);
			EventLoop();
		}

		Stop();
		break;

	case sysAppLaunchCmdFind:
		FSearch((FindParamsPtr)cmdPBP);
		break;

	case sysAppLaunchCmdGoTo:
		if (UtilsRomOK()==false)
			break;

		recordIdx=((GoToParamsType *)cmdPBP)->recordNum;
		if ((launchFlags&sysAppLaunchFlagNewGlobals)) {
			if (Start()==true) {
				findReturn=true;
				FrmGotoForm(UtilsGetHistoryForm(recordIdx));
				EventLoop();
			}
			Stop();
		} else
			FrmGotoForm(UtilsGetHistoryForm(recordIdx));

		break;

	default:	/* FALL-THRU */
		break;
	}

	return 0;
}

/*
**	EventLoop
**
**	Main event loop (event dispatcher).
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
				FrmAlert(aNoSuchForm);
				return;
			}
			break;
		default:
			FrmDispatchEvent(&ev);
			break;
		}
	}
}

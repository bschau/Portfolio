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
static void LoadDatabase(void);

/* globals */
static struct {
	UInt16 id;
	Boolean (*init)();
	FormEventHandlerPtr evh;
} formInits[]= {
	{ fMain, fMainInit, fMainEH },
	{ fKeywords, fKeywordsInit, fKeywordsEH },
	{ fStory, fStoryInit, fStoryEH },
	{ fEditTemplate, fEditTemplateInit, fEditTemplateEH },
	{ 0 },
};

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

	if (version<sysMakeROMVersion(3, 0, 0, sysROMStageRelease, 0)) {
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
	if (PMGetPref(&prefs, sizeof(prefs), PrfApplication)==false) {
		prefs.category=1;
		prefs.catExchangeIdx=dmUnfiledCategory;
		prefs.exchangeFlags=0;
	}

	DBInit(&dbTemplate, DBNameTemplate);
	DBInit(&dbKeyword, DBNameKeyword);
	DBInit(&dbMemo, "MemoDB");

	if (DBOpen(&dbTemplate, dmModeReadWrite, true)==false)
		return false;

	if (DBOpen(&dbKeyword, dmModeReadWrite, true)==false)
		return false;

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
	StoryDone();

	DBClose(&dbKeyword);
	DBClose(&dbTemplate);

	PMSetPref(&prefs, sizeof(prefs), PrfApplication);
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
			DBCreateDatabase(DBNameTemplate, defCatTemplate);
			DBCreateDatabase(DBNameKeyword, 0);
			if (Start()==true) {
				LoadDatabase();
				if (PMRun()==true)
					EventLoop();
			}
			Stop();
		}
		break;

	case sysAppLaunchCmdSyncNotify:
		DBCreateDatabase(DBNameTemplate, defCatTemplate);
		DBCreateDatabase(DBNameKeyword, 0);
		break;

	case sysAppLaunchCmdExgAskUser:
		((ExgAskParamType *)cmdPBP)->result=exgAskOk;
		break;

	case sysAppLaunchCmdExgReceiveData:
		if (!(launchFlags&sysAppLaunchFlagSubCall)) {
			DB dbo;

			DBInit(&dbo, DBNameTemplate);
			if (DBOpen(&dbo, dmModeReadWrite, true)==true) {
				IRReceive((ExgSocketType *)cmdPBP, &dbo);
				DBClose(&dbo);
			}
		} else {
			IRReceive((ExgSocketType *)cmdPBP, &dbTemplate);
			fMainRethink();
		}

		break;

	case sysAppLaunchCmdGoTo:
		if (launchFlags&sysAppLaunchFlagNewGlobals) {
			if (RomCheck()==false)
				break;

			if (Start()==false)
				break;
		}

		fMainRethink();
		prefs.category=dmUnfiledCategory;
		fKeywordsRun(((GoToParamsType *)cmdPBP)->recordNum);
		if (launchFlags&sysAppLaunchFlagNewGlobals) {
			EventLoop();
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

/*
 * PMRun
 *
 * Run program.
 *
 * Returns true if program was successfully started, false otherwise.
 */
Boolean
PMRun(void)
{
	UInt32 uid;
	UInt16 rIdx;

	if (PMGetPref(&uid, sizeof(UInt32), PrfTemplate)==true) {
		if (DmFindRecordByID(dbTemplate.db, uid, &rIdx)==errNone) {
			fEditTemplateRun(rIdx);
			PMSetPref(NULL, 0, PrfTemplate);
			return true;
		} else
			PMSetPref(NULL, 0, PrfTemplate);
	}

	if (PMGetPref(&uid, sizeof(UInt32), PrfKeywords)==true) {
		if (DmFindRecordByID(dbTemplate.db, uid, &rIdx)==errNone) {
			fKeywordsRun(rIdx);
			PMSetPref(NULL, 0, PrfKeywords);
			return true;
		} else
			PMSetPref(NULL, 0, PrfKeywords);
	}

			
	FrmGotoForm(fMain);
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
	PrefSetAppPreferences((UInt32)CRID, id, PREFSVER, p, len, false);
}

/*
 * LoadDatabase
 *
 * Load the default stories into category 1 ("Samples") if no entries exist.
 */
static void
LoadDatabase(void)
{
#if DEBUG
	D(DPreload());
#else
	UInt16 str=strStory1, category, len;
	MemHandle memoH, mh;
	Char *memo;
	
	if (DmNumRecordsInCategory(dbTemplate.db, dmAllCategories)>0)
		return;

	if (FrmAlert(aLoadDatabase)==1)
		return;

	if ((category=CategoryFind(dbTemplate.db, "Samples"))==dmAllCategories)
		category=dmUnfiledCategory;

	memoH=MemHandleNew(MemoSize);
	ErrFatalDisplayIf(memoH==NULL, "(LoadDatabase) Out of memory.");
	memo=MemHandleLock(memoH);

	for (EVER) {
		if ((mh=DmGetResource(strRsc, str))==NULL)
			break;

		len=MemHandleSize(mh);
		MemMove(memo, MemHandleLock(mh), len);
		MemHandleUnlock(mh);
		DmReleaseResource(mh);
		memo[len]='\x00';

		DBSetRecord(&dbTemplate, dmMaxRecordIndex, category, memo, len+1, SFString);
		str++;
	}

	MemHandleFree(memoH);
#endif
}

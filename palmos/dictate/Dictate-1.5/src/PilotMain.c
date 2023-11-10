/*
 * PilotMain.c
 */
#define MAIN
#include <DLServer.h>
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
	FormEventHandlerPtr evh;
} formInits[]= {
	{ fTeacher, fTeacherInit, fTeacherEH },
	{ fSentence, fSentenceInit, fSentenceEH },
	{ fEditSentence, fEditSentenceInit, fEditSentenceEH },
	{ fGroup, NULL, fGroupEH },
	{ fTDictate, fTDictateInit, fTDictateEH },
	{ fSDictate, fSDictateInit, fSDictateEH },
	{ fStudent, fStudentInit, fStudentEH },
	{ fDictationDone, NULL, fDictationDoneEH },
	{ fViewAnswers, fViewAnswersInit, fViewAnswersEH },
	{ fWait, NULL, fWaitEH },
	{ 0 },
};
static MemHandle bulletH=NULL;

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
		prefs.function=0;
		prefs.exchangeFlags=0;
		prefs.catSentenceIdx=dmUnfiledCategory;
		prefs.catExchangeIdx=dmUnfiledCategory;
		prefs.userName[0]='\x00';
	}

	if (!prefs.userName[0])
		DlkGetSyncInfo(NULL, NULL, NULL, prefs.userName, NULL, NULL);

	bulletH=DmGetResource(bitmapRsc, bBullet);
	ErrFatalDisplayIf(bulletH==NULL, "(Start) Cannot lock bitmap.");
	bmpBullet=MemHandleLock(bulletH);

	DBInit(&dbTGroup, DBNameTGroup);
	DBInit(&dbTSentence, DBNameTSentence);
	DBInit(&dbTXref, DBNameTGXref);
	DBInit(&dbSGroup, DBNameSGroup);
	DBInit(&dbSSentence, DBNameSSentence);
	DBInit(&dbSXref, DBNameSGXref);
	DBInit(&dbScore, DBNameScore);
	DBInit(&dbAnswer, DBNameAnswer);
	DBInit(&dbBeam, DBNameBeam);
	DBInit(&dbMemo, "MemoDB");

	if (DBOpen(&dbTGroup, dmModeReadWrite, true)==false)
		return false;

	if (DBOpen(&dbTSentence, dmModeReadWrite, true)==false)
		return false;

	if (DBOpen(&dbTXref, dmModeReadWrite, true)==false)
		return false;

	if (DBOpen(&dbSGroup, dmModeReadWrite, true)==false)
		return false;

	if (DBOpen(&dbSSentence, dmModeReadWrite, true)==false)
		return false;

	if (DBOpen(&dbSXref, dmModeReadWrite, true)==false)
		return false;

	if (DBOpen(&dbScore, dmModeReadWrite, true)==false)
		return false;

	if (DBOpen(&dbAnswer, dmModeReadWrite, true)==false)
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

	DBClose(&dbSGroup);
	DBClose(&dbSSentence);
	DBClose(&dbSXref);
	DBClose(&dbTGroup);
	DBClose(&dbTSentence);
	DBClose(&dbTXref);
	DBClose(&dbScore);
	DBClose(&dbAnswer);

	if (bulletH) {
		MemHandleUnlock(bulletH);
		DmReleaseResource(bulletH);
	}

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
			IRRegister();
			DBCreateDatabase(DBNameTGroup, 0);
			DBCreateDatabase(DBNameTSentence, defCatSentences);
			DBCreateDatabase(DBNameTGXref, 0);
			DBCreateDatabase(DBNameSGroup, 0);
			DBCreateDatabase(DBNameSSentence, 0);
			DBCreateDatabase(DBNameSGXref, 0);
			DBCreateDatabase(DBNameScore, 0);
			DBCreateDatabase(DBNameAnswer, 0);

			if (Start()==true) {
				D(DPreLoad());
				if (PMRun()==true)
					EventLoop();
			}
			Stop();
		}
		break;

	case sysAppLaunchCmdSyncNotify:
	case sysAppLaunchCmdSystemReset:
		IRRegister();
		break;
	case sysAppLaunchCmdExgReceiveData:
		IRReceive((ExgSocketType *)cmdPBP);
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

		if (ev.eType==nilEvent) {
			IRImport();
			continue;
		}

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
	if (!prefs.function)
		prefs.function=FrmAlert(aPrimaryFunction)+1;

	if (prefs.function==PFuncStudent) {
		if (DBFindDatabase(&dbBeam)==FDFound)
			FtrSet((UInt32)CRID, 0, 1);
		else
			FtrUnregister((UInt32)CRID, 0);

		if (PMGetPref(&game, sizeof(Game), PrfGameInfo)==true) {
			if (game.round<XRefCount(&dbSXref, game.guid)) {
				groupUid=game.guid;
				FrmGotoForm(fSDictate);
				return true;
			}
		}
		FrmGotoForm(fStudent);
	} else
		fSentenceEditFromPrefs();

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
 * PMGetGroupName
 *
 * Get group name from database.
 *
 *  -> dbo		Database object.
 *  -> guid		Group ID.
 * <-  dst		Destination (can be NULL);
 *
 * Returns true if group was found, false otherwise.
 */
Boolean
PMGetGroupName(DB *dbo, UInt32 guid, Char *dst)
{
	UInt16 rIdx;

	if (DmFindRecordByID(dbo->db, guid, &rIdx)==errNone) {
		if (dst) {
			MemHandle mh=DBGetRecord(dbo, rIdx);
			Char *p;

			p=MemHandleLock(mh);
			StrNCopy(dst, p, GroupNameLength);
			dst[StrLen(p)]='\x00';
			MemHandleUnlock(mh);
		}

		return true;
	}

	if (dst) {
		*dst++='-';
		*dst='\x00';
	}

	return false;
}

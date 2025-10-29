/*
 * PilotMain.c
 */
#define MAIN
#include "Include.h"

/* protos */
static Boolean RomCheck(void);
static Boolean Start(Boolean);
static void Stop(void);
static void HandleOpenDB(UInt32, UInt16, LocalID);
static void EventLoop(void);
static Boolean Login(void);
static void PMRun(void);
static void RegisterNotification(Boolean);

/* globals */
static struct {
	UInt16 id;
	Boolean (*init)();
	FormEventHandlerType *evh;
} formInits[]= {
	{ fMain, fMainInit, fMainEH },
	{ fShow, fShowInit, fShowEH },
	{ fEdit, fEditInit, fEditEH },
	{ fNewDatabase, NULL, fNewDatabaseEH },
	{ fCreateDatabase, fCreateDatabaseInit, fCreateDatabaseEH },
	{ fIE, fIEInit, fIEEH },
	{ 0 },
};
static MemHandle dotFontH, scissorFontH, miscFontH;
static UInt32 romVersion;
extern UInt16 libRefNum, kbdRefNum;
extern MemHandle aesData, sha256Data;
extern Prefs prefs;
extern State state;
extern UI *ui;
extern Boolean notificationsEnabled, appStopped;
extern DB dbData, dbMemo;
extern UInt8 loginDigest[32];
extern UInt8 gKey[AESKeyLength];
extern Int16 mainLstIndex;
extern Char gPwd[];

/*
 * RomCheck
 *
 * Returns true if the Rom Version is equal to or better than the required
 * Rom Version. False is returned otherwise.
 */
static Boolean
RomCheck(void)
{
	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);

	if (romVersion<sysMakeROMVersion(3, 5, 0, sysROMStageRelease, 0)) {
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
 *  -> normal		Normal start (=true).
 *
 * Return false in case of errors.
 */
static Boolean
Start(Boolean normal)
{
	UInt32 v;

#ifdef DEBUG
#if SYS==0
	gHostFile=HostFOpen("/tmp/iSecur.log", "a+");
#else
	gHostFile=HostFOpen("c:\\temp\\iSecur.log", "a+");
#endif
	ErrFatalDisplayIf(gHostFile==NULL, "(Start) HostFOpen failed");
	DPrint("======== iSecur Started ====\n");
#endif

	libRefNum=sysInvalidRefNum;
	if (SysLibFind("iSecur Library", &libRefNum)!=errNone) {
		if (SysLibLoad('libr', 'iSec', &libRefNum)!=errNone) {
			FrmAlert(aNoLibrary);
			return false;
		}
	}

	iSecurLibVersion(libRefNum, &v);
	if (v!=1) {
		FrmAlert(aNoLibrary);
		return false;
	}

	iSecurLibOpen(libRefNum, 0);

	kbdRefNum=sysInvalidRefNum;
	if (SysLibFind("iSecur Keyboard", &kbdRefNum)!=errNone) {
		if (SysLibLoad('kbde', 'iSec', &kbdRefNum)!=errNone)
			kbdRefNum=sysInvalidRefNum;
	}

	if (kbdRefNum!=sysInvalidRefNum) {
		Boolean remLib=true;

		if (KeyboardLibOpen(kbdRefNum, 0)==errNone) {
			KeyboardLibAPIVersion(kbdRefNum, &v);
			if (v==1)
				remLib=false;
		}

		if (remLib) {
			UInt16 useCount;

			if ((KeyboardLibClose(kbdRefNum, &useCount)==errNone) && (!useCount))
				SysLibRemove(kbdRefNum);
		}
	}

	aesData=AESOpen();
	ErrFatalDisplayIf(aesData==NULL, "(Start) Out of memory");

	sha256Data=SHA256Open();
	ErrFatalDisplayIf(sha256Data==NULL, "(Start) Out of memory");

	if (PMGetPref(&prefs, sizeof(Prefs), PrfApplication, true)==false) {
		MemSet(&prefs, sizeof(Prefs), 0);
		prefs.flags=(PFlgEnableFind|PFlgLogoutPower|PFlgLogoutTimeout|PFlgEmptyClipboard|PFlgSecretStay);
	}

	if (PMGetPref(&state, sizeof(State), PrfState, false)==false) {
		state.category=dmUnfiledCategory;
		state.filter=0;
		state.pwdGenLength=1;
		state.pwdGenContains=1|2|4;
		state.prfPage=0;
		state.memoCat=dmUnfiledCategory;
		state.impCat=dmUnfiledCategory;
		state.expCat=dmUnfiledCategory;
		state.impExpFlags=0;
	}

	ui=UIInit();
	mainLstIndex=noListSelection;

	notificationsEnabled=false;
	v=0;
	if (FtrGet(sysFtrCreator, sysFtrNumNotifyMgrVersion, &v)==errNone) {
		if (v)
			notificationsEnabled=true;
	}

	fShowOpen();
	fKeywordsInit();

	dotFontH=DmGetResource('NFNT', 1000);
	ErrFatalDisplayIf(dotFontH==NULL, "(Start) dotFont not found");
	FntDefineFont(dotFont, MemHandleLock(dotFontH));

	scissorFontH=DmGetResource('NFNT', 1001);
	ErrFatalDisplayIf(scissorFontH==NULL, "(Start) scissorFont not found");
	FntDefineFont(scissorFont, MemHandleLock(scissorFontH));

	miscFontH=DmGetResource('NFNT', 1002);
	ErrFatalDisplayIf(miscFontH==NULL, "(Start) miscFont not found");
	FntDefineFont(miscFont, MemHandleLock(miscFontH));

	fIconOpen();
	pool=EntropyOpen();

	PMPwdChgAlarm();
	RegisterNotification(true);
	FtrSet((UInt32)CRID, iSecurFtrHasGlobals, 1);
	DBInit(&dbData, DBNameData);
	DBClear(&dbMemo);

	if (!normal)
		return true;

	switch (DBFindDatabase(&dbData)) {
	case FDFound:
		if (DBOpen(&dbData, dmModeReadWrite, true)==true) {
			if (Login()==true) {
				PMRun();
				return true;
			}
		}
		break;
	case FDNotFound:
		FrmGotoForm(fNewDatabase);
		return true;
	default:
		ErrFatalDisplay("Error while looking for the iSecur database");
		SysReset();
		break;
	}

	return false;
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

	FtrUnregister((UInt32)CRID, iSecurFtrHasGlobals);
	RegisterNotification(false);

	if (prefs.flags&PFlgEmptyClipboard) {
		Char *str=" ";

		ClipboardAddItem(clipboardText, str, 1);
	}

	if (miscFontH) {
		MemHandleUnlock(miscFontH);
		DmReleaseResource(miscFontH);
	}

	if (scissorFontH) {
		MemHandleUnlock(scissorFontH);
		DmReleaseResource(scissorFontH);
	}

	if (dotFontH) {
		MemHandleUnlock(dotFontH);
		DmReleaseResource(dotFontH);
	}

	EntropyClose(pool);
	fKeywordsClose();
	fShowClose();
	fIconClose();
	fIEClose();
	DBClose(&dbData);

	MemSet(gKey, AESKeyLength, 0);
	MemSet(loginDigest, 32, 0);

	PMSetPref(&state, sizeof(State), PrfState, false);
	PMSetPref(&prefs, sizeof(Prefs), PrfApplication, true);

	if (kbdRefNum!=sysInvalidRefNum) {
		UInt16 useCount;

		if ((KeyboardLibClose(kbdRefNum, &useCount)==errNone) && (!useCount))
			SysLibRemove(kbdRefNum);
	}

	SHA256Close();
	AESClose();

	if (libRefNum!=sysInvalidRefNum) {
		UInt16 useCount;

		if (iSecurLibClose(libRefNum, &useCount)==0 && useCount==0)
			SysLibRemove(libRefNum);
	}

	MemSet(gPwd, PasswordLength+2, 0);
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
	UInt32 id=0;
	Prefs prf;
	Boolean openErr;
	DmOpenRef dbRef;

#if DEBUG==1
	{
		UInt32 v=0;

		if (FtrGet(kPalmOSEmulatorFeatureCreator, 0, &v)==ftrErrNoSuchFeature) {
			if ((FtrGet(sysFtrCreator, sysFtrNumProcessorID, &v)==ftrErrNoSuchFeature) || ((v&sysFtrNumProcessorMask)!=sysFtrNumProcessorx86)) {
				FrmAlert(aNoPose);
				return 0;
			}
		}
	}
#endif

	switch (cmd) {
	case sysAppLaunchCmdNormalLaunch:
		if (RomCheck()==true) {
			if (Start(true)==true)
				EventLoop();

			Stop();
		}
		break;

	case sysAppLaunchCmdFind:
		if (PMGetPref(&prf, sizeof(Prefs), PrfApplication, true)==false)
			break;

		if (prf.flags&PFlgEnableFind)
			FindSearch((FindParamsType *)cmdPBP);
		break;

	case sysAppLaunchCmdGoTo:
		if (RomCheck()==true) {
			UInt16 rIdx=((GoToParamsType *)cmdPBP)->recordNum;
			if ((launchFlags&sysAppLaunchFlagNewGlobals)) {
				if (Start(false)==true) {
					if (DBFindDatabase(&dbData)==FDFound) {
						if (DBOpen(&dbData, dmModeReadWrite, true)==true) {
							if (Login()==true) {
								PMOpenSecret(rIdx);
								EventLoop();

							}
						}
					}
					Stop();
				}
			} else
				PMOpenSecret(rIdx);
		}
		break;

	case sysAppLaunchCmdNotify:
		if ((((SysNotifyParamType *)cmdPBP)->notifyType)==sysNotifySleepNotifyEvent) {
			Prefs p;

			if ((PMGetPref(&p, sizeof(Prefs), PrfApplication, true)==true) && (p.flags&PFlgLogoutTimeout)) {
				PMQueueAppStopEvent();
				((SysNotifyParamType *)cmdPBP)->handled=true;
			}
		}
		break;

#if 0
	case sysAppLaunchCmdAlarmTriggered:
		{
			UInt32 almTime;

			if (PMGetPref(&almTime, sizeof(UInt32), PrfPwdChangeAlarm, false)==true) {
				if (TimGetSeconds()<almTime)
					((SysAlarmTriggeredParamType *)cmdPBP)->purgeAlarm=true;
			} else
				((SysAlarmTriggeredParamType *)cmdPBP)->purgeAlarm=true;
		}

		PMFPCSetAlarm();
		break;
#endif

	case sysAppLaunchCmdDisplayAlarm:
		{
			UInt32 v=0;

			if (FtrGet((UInt32)CRID, iSecurFtrHasGlobals, &v)==errNone) {
				if (FrmAlert(aPwdChange)==0)
					fMainChangePassword();
			} else {
				if (FrmAlert(aPwdChangeReminder)==0) {
					UInt16 card;
					LocalID id;

					SysCurAppDatabase(&card, &id);
					SysUIAppSwitch(card, id, iSecurAppLaunchCmdChangePassword, NULL);
			
				}
			}
		}

		PMPwdChgAlarm();
		break;

	case sysAppLaunchCmdSyncNotify:
	case sysAppLaunchCmdSystemReset:
		{
			DB db;

			DBInit(&db, DBNameData);
			if (DBOpen(&db, dmModeReadWrite, false)==true) {
				DmQuickSort(db.db, DBSort, 0);
				DBClose(&db);
			}
		}
		PMPwdChgAlarm();
		break;

	case sysAppLaunchCmdExgAskUser:
		((ExgAskParamType *)cmdPBP)->result=exgAskOk;
		break;

	case sysAppLaunchCmdExgReceiveData:
		IRReceive((ExgSocketType *)cmdPBP);
		break;

	case iSecurAppLaunchCmdChangePassword:
		if (RomCheck()==true) {
			if ((launchFlags&sysAppLaunchFlagNewGlobals)) {
				if (Start(false)==true) {
					if (DBFindDatabase(&dbData)==FDFound) {
						if (DBOpen(&dbData, dmModeReadWrite, true)==true) {
							if (Login()==true)
								fMainChangePassword();
						}
					}
					Stop();
				}
			}
		}
		break;

	case sysAppLaunchCmdOpenDB:
		openErr=true;
		if ((launchFlags&sysAppLaunchFlagNewGlobals)) {
			if ((dbRef=DmOpenDatabase(((SysAppLaunchCmdOpenDBType *)cmdPBP)->cardNo, ((SysAppLaunchCmdOpenDBType *)cmdPBP)->dbID, dmModeReadOnly))!=NULL) {
				UInt16 sIdx;

				if ((sIdx=DmFindResource(dbRef, constantRscType, iSecurLetResID, NULL))!=dmMaxRecordIndex) {
					MemHandle mh=DmGetResourceIndex(dbRef, sIdx);
	
					if (mh) {
						id=*(UInt32 *)(MemHandleLock(mh));
						MemHandleUnlock(mh);
						DmReleaseResource(mh);
						openErr=false;
					}
				}

				DmCloseDatabase(dbRef);
			}
		}

		if (openErr)
			FrmAlert(aCannotOpenSlet);
		else
			HandleOpenDB(id, ((SysAppLaunchCmdOpenDBType *)cmdPBP)->cardNo, ((SysAppLaunchCmdOpenDBType *)cmdPBP)->dbID);

	default:	/* FALL-THRU */
		break;
	}

	return 0;
}

/*
 * HandleOpenDB
 *
 * Handle the sys..OpenDB launch code.
 *
 *  -> id		Record ID.
 *  -> cardNo		Card no of Let db.
 *  -> dbID		Database ID of Let db.
 */
static void
HandleOpenDB(UInt32 id, UInt16 cardNo, LocalID dbID)
{
	UInt16 rIdx;

	if (Start(false)==true) {
		if (DBFindDatabase(&dbData)==FDFound) {
			if (DBOpen(&dbData, dmModeReadWrite, true)==true) {
				if (DBFindRecordByID(&dbData, id, &rIdx)==errNone) {
					if (Login()==true) {
						fShowRun(rIdx, false);
						EventLoop();
					}
				} else {
					if (FrmAlert(aNoSuchLet)==0) {
						if (DmDeleteDatabase(cardNo, dbID)!=errNone)
							FrmAlert(aLetDeleteError);
					}
				}
			}
		}
		Stop();
	}
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

		if (PMHardPower(&ev)==true)
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
 * PMGetPref
 *
 * Get preference #.
 *
 *  -> p	Destination.
 *  -> len	Length of destination.
 *  -> id	Preference id.
 *  -> type	Get from saved preferences (= true).
 *
 * Returns true if preferences was gotten, false otherwise.
 */
Boolean
PMGetPref(void *p, UInt16 len, UInt16 id, Boolean type)
{	
	UInt16 l=len;
	Int16 retPrf;

	retPrf=PrefGetAppPreferences((UInt32)CRID, id, p, &l, type);
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
 *  -> type	Set in saved prefs (= true).
 */
void
PMSetPref(void *p, UInt16 len, UInt16 id, Boolean type)
{
	PrefSetAppPreferences((UInt32)CRID, id, PREFSVER, p, len, type);
}

/*
 * Login
 *
 * Try login - try up to prefs.maxFailedLogins times.
 */
static Boolean
Login(void)
{
//	Char pwd[PasswordLength+1];
	LocalID appID;
	AppInfoBlock *a;
	Boolean v;
	UInt16 version;

	if (DmDatabaseInfo(dbData.card, dbData.id, NULL, NULL, &version, NULL, NULL, NULL, NULL, &appID, NULL, NULL, NULL)!=errNone)
		return false;

	a=(AppInfoBlock *)MemLocalIDToLockedPtr(appID, dbData.card);
//	v=fPasswordRun(PwdDialogNormal, pwd, a, prefs.maxFailedLogins, 0, false, version);
	v=fPasswordRun(PwdDialogNormal, a, prefs.maxFailedLogins, 0, false, version);
	if (appStopped) {
//		MemSet(pwd, sizeof(pwd), 0);
		MemPtrUnlock(a);
		return false;
	}

	if (v) {
		AESCreateKeys(gPwd);
//		MemSet(pwd, sizeof(pwd), 0);
		MemMove(gKey, a->key, AESKeyLength);
		AESDecrypt(gKey, gKey);
		AESDecrypt(gKey+16, gKey+16);

		AESCreateKeys(gKey);
		MemPtrUnlock(a);
		return true;
	}

	MemPtrUnlock(a);
//	MemSet(pwd, sizeof(pwd), 0);

	if (prefs.flags&PFlgDestroyDB) {
		UInt16 rIdx=0, card=dbData.card;
		MemHandle mh;
		LocalID id=dbData.id;

		if (DmDatabaseInfo(card, id, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &appID, NULL, NULL, NULL)!=errNone)
			return false;

		a=(AppInfoBlock *)MemLocalIDToLockedPtr(appID, card);

		DmSet(a, 0, sizeof(AppInfoBlock), 0);
		MemPtrUnlock(a);

		while ((mh=DmQueryNextInCategory(dbData.db, &rIdx, dmAllCategories))!=NULL) {
			DmSet(MemHandleLock(mh), 0, MemHandleSize(mh), 0);
			MemHandleUnlock(mh);
			DmRemoveRecord(dbData.db, rIdx);
		}

		DBClose(&dbData);
		DmDeleteDatabase(card, id);
	}
	return false;
}

/*
 * PMRun
 *
 * Go to first form.
 */
static void
PMRun(void)
{
	UInt32 uid;

	if (PMGetPref(&uid, sizeof(UInt32), PrfSaveData, false)==true) {
		UInt16 rIdx;

		PMSetPref(NULL, 0, PrfSaveData, false);

		if (DBFindRecordByID(&dbData, uid, &rIdx)==errNone) {
			fEditRun(rIdx, 0, false);
			return;
		}
	}

	FrmGotoForm(fMain);
}

/*
 * PMOpenSecret
 *
 * Open the secret.
 *
 *  -> rIdx		Record index.
 */
void
PMOpenSecret(UInt16 rIdx)
{
	if (prefs.flags&PFlgSecretOpenEdit)
		fEditRun(rIdx, 0, false);
	else
		fShowRun(rIdx, true);
}

/*
 * PMHardPower
 *
 * "Logout" if Hard Power key is pressed.
 *
 *  -> ev		Event.
 *
 * Returns true if Hard Power was pressed, false otherwise.
 */
Boolean
PMHardPower(EventType *ev)
{
	if ((ev->eType==keyDownEvent) && (ev->data.keyDown.chr==hardPowerChr)) {
		if (prefs.flags&PFlgLogoutPower) {
			PMQueueAppStopEvent();
			EvtEnqueueKey(hardPowerChr, 0, commandKeyMask);
			return true;
		}
	}
	
	return false;
}

/*
 * RegisterNotification
 *
 * Register to receive notifications.
 *
 *  -> reg		Register (= true), Unregister (= false).
 */
static void
RegisterNotification(Boolean reg)
{
	if (notificationsEnabled) {
		UInt16 card;
		LocalID id;

		SysCurAppDatabase(&card, &id);
		if (reg) {
			if (SysNotifyRegister(card, id, sysNotifySleepNotifyEvent, NULL, sysNotifyNormalPriority, NULL)==errNone)
				return;
		} else {
			if (SysNotifyUnregister(card, id, sysNotifySleepNotifyEvent, sysNotifyNormalPriority)==errNone)
				return;
		}
		FrmAlert(aNotificationError);
	}
}

#if 0
/*
 * PMFPCSetup
 *
 * Setup time to force password change.
 */
void
PMFPCSetup(void)
{
	DateTimeType dt;
	Prefs p;
	UInt32 almTime;

	if ((PMGetPref(&p, sizeof(Prefs), PrfApplication, true)==false) || (!p.pwdChangeDays))
		return;

	TimSecondsToDateTime(TimGetSeconds(), &dt);
#if DEBUG
	almTime=(TimDateTimeToSeconds(&dt))+(3);
#else
	dt.hour=0;
	dt.minute=0;
	dt.second=0;
	almTime=(TimDateTimeToSeconds(&dt))+(p.pwdChangeDays*86400);
#endif

	PMSetPref(&almTime, sizeof(almTime), PrfPwdChangeAlarm, false);
}

/*
 * PMFPCSetAlarm
 *
 * Set the alarm time for the trigger.
 */
void
PMFPCSetAlarm(void)
{
	DateTimeType dt;
	UInt16 card;
	LocalID id;

	SysCurAppDatabase(&card, &id);
	TimSecondsToDateTime(TimGetSeconds(), &dt);
#if DEBUG
	AlmSetAlarm(card, id, 0, TimDateTimeToSeconds(&dt)+10, true);
#else
	dt.hour=0;
	dt.minute=0;
	dt.second=0;
	AlmSetAlarm(card, id, 0, TimDateTimeToSeconds(&dt)+90000, true);
#endif
}
#endif

/*
 * PMQueueAppStopEvent
 *
 * Insert an appStopEvent into the event queue.
 */
void
PMQueueAppStopEvent(void)
{
	EventType ev;

	MemSet(&ev, sizeof(EventType), 0);
	ev.eType=appStopEvent;
	EvtAddEventToQueue(&ev);
}

/**
 * Queue alarm for password change if not previously set.
 */
void
PMPwdChgAlarm(void)
{
	UInt32 when, curAlm, ref;
	DateTimeType dt;
	UInt16 card;
	LocalID id;
	Prefs p;

	SysCurAppDatabase(&card, &id);
	if ((PMGetPref(&p, sizeof(Prefs), PrfApplication, true)==false) || (!p.pwdChangeDays)) {
		AlmSetAlarm(card, id, 0, 0, true);
		return;
	}

	TimSecondsToDateTime(TimGetSeconds(), &dt);
#if DEBUG
	when=TimDateTimeToSeconds(&dt)+10;

#else
	dt.hour=0;
	dt.minute=0;
	dt.second=0;
	when=TimDateTimeToSeconds(&dt)+90000;	/* 1 o'clock at night */
#endif

	if ((curAlm=AlmGetAlarm(card, id, &ref))) {
		if (curAlm<=when)
			return;
	}

	AlmSetAlarm(card, id, 0, when, true);
}

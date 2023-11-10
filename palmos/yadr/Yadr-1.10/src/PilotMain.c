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
static void PMRun(void);
static void SubLaunch(UInt16, UInt16, LocalID);

/* globals */
static struct {
	UInt16 id;
	Boolean (*init)();
	FormEventHandlerType *evh;
} formInits[]= {
	{ fReader, fReaderInit, fReaderEH },
	{ fReaderFullScreen, fReaderInit, fReaderEH },
	{ fMain, fMainInit, fMainEH },
	{ fFormatting, fFormattingInit, fFormattingEH },
	{ fBookmarks, fBookmarksInit, fBookmarksEH },
	{ fHistory, fHistoryInit, fHistoryEH },
	{ 0 },
};
static MemHandle folderBitmapH;

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
	UInt32 w, h;

	DOpen("Yadr");
	if (PMGetPref(&prefs, sizeof(Prefs), PrfApplication)==false) {
		prefs.flags=0;
		prefs.font=stdFont;
		prefs.drawMode=PDMLeft;
		prefs.searchDocs=PSDMemory;
		prefs.foreColor=UIColorGetTableEntryIndex(UIObjectForeground);
		prefs.backColor=UIColorGetTableEntryIndex(UIFormFill);
		prefs.controlMode=PCMUpDown;
		prefs.lineSpacing=0;
	}

	if (WinScreenMode(winScreenModeGet, &w, &h, NULL, NULL)!=errNone) {
		dispW=160;
		dispH=160;
	} else {
		dispW=(UInt16)w;
		dispH=(UInt16)h;
	}

	folderBitmapH=DmGetResource(bitmapRsc, bmpFolder);
	ErrFatalDisplayIf(folderBitmapH==NULL, "(Start) Cannot find folder bitmap");
	folderBitmap=MemHandleLock(folderBitmapH);

	fMainAlloc();
	fReaderAlloc();
	fFindAlloc();

	sbReader=SBNew(64);
	sbBookmarks=SBNew(512);
	sbGoTo=SBNew(512);

	if (BookmarksOpen()==false)
		return false;

	doc=NULL;
	return HistoryOpen();
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

	DocClose(&doc);
	SBFree(sbGoTo);
	SBFree(sbBookmarks);
	SBFree(sbReader);
	fReaderFree();
	fBookmarksClose();
	fMainClose();
	fFindClose();

	MemHandleUnlock(folderBitmapH);
	DmReleaseResource(folderBitmapH);

	HistoryClose();
	BookmarksClose();

	PMSetPref(&prefs, sizeof(Prefs), PrfApplication);
	DClose();
}

/*
 * PilotMain
 */
UInt32
PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
	Boolean disconnect;
	Err err;

	switch (cmd) {
	case sysAppLaunchCmdNormalLaunch:
		if (RomCheck()==true) {
			DBCreateDatabase(DBNameBookmarks, 0);
			if (Start()==true) {
				PMRun();
				EventLoop();
			}
			Stop();
		}
		break;

	case sysAppLaunchCmdExgAskUser:
		((ExgAskParamType *)cmdPBP)->result=exgAskOk;
		break;

	case sysAppLaunchCmdExgReceiveData:
		IRReceive((ExgSocketType *)cmdPBP, &disconnect, &err);
		if (disconnect)
			ExgDisconnect((ExgSocketType *)cmdPBP, err);

		if (launchFlags&sysAppLaunchFlagSubCall) {
			fMainRethink();
		}
	
		break;

	case sysAppLaunchCmdOpenDB:
		SubLaunch(launchFlags, ((SysAppLaunchCmdOpenDBType *)cmdPBP)->cardNo, ((SysAppLaunchCmdOpenDBType *)cmdPBP)->dbID);
		break;

	case sysAppLaunchCmdGoTo:
		SubLaunch(launchFlags, ((GoToParamsType *)cmdPBP)->dbCardNo, ((GoToParamsType *)cmdPBP)->dbID);
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

		if (ev.eType==keyDownEvent && ev.data.keyDown.chr==vchrFind) {
			MemSet(&ev, sizeof(EventType), 0);
			ev.eType=yadrFindEvent;
			EvtAddEventToQueue(&ev);
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
 * PMRun
 *
 * Decide which form to show.
 */
static void
PMRun(void)
{
	UInt16 goToForm=fMain, pSize=0, l;
	Int16 retPrf;
	MemHandle mh;
	PrfSaveDoc *s;

	if (PrefGetAppPreferences((UInt32)CRID, PrfDocument, NULL, &pSize, false)==noPreferenceFound) {
		FrmGotoForm(fMain);
		return;
	}

	mh=MemHandleNew(pSize);
	ErrFatalDisplayIf(mh==NULL, "(PMRun) Out of memory");
	s=MemHandleLock(mh);

	l=pSize;
	retPrf=PrefGetAppPreferences((UInt32)CRID, PrfDocument, s, &l, false);
	if (retPrf!=noPreferenceFound && l==pSize) {
		Char *p=((Char *)s)+sizeof(PrfSaveDoc);
		Char *vfs=NULL;
		UInt32 len;

		if (s->vfs) {
			Char *s;

			for (s=p; *s; s++);
			s++;

			len=StrLen(s);
			if (len) {
				MemHandle vfsH=MemHandleNew(len+1);

				ErrFatalDisplayIf(vfsH==NULL, "(PMRun) Out of memory");
				vfs=MemHandleLock(vfsH);
				MemMove(vfs, s, len);
				vfs[len]=0;
			}
		}

		fMainStatus(txtPleaseWait);
		if ((doc=DocOpen(p, vfs, s->position))!=NULL) {
			if (prefs.flags&PFFullScreen)
				goToForm=fReaderFullScreen;
			else
				goToForm=fReader;
		}
	}

	MemHandleFree(mh);
	PMSetPref(NULL, 0, PrfDocument);
	FrmGotoForm(goToForm);
}

/*
 * SubLaunch
 *
 * Launch Yadr (from System) and open ebook.
 *
 *  -> launchFlags	Launch Flags.
 *  -> card		Card No of ebook.
 *  -> id		Local ID of ebook.
 */
static void
SubLaunch(UInt16 launchFlags, UInt16 card, LocalID id)
{
	Err err;
	Char dbName[DocNameLength];

	if ((err=DmDatabaseInfo(card, id, dbName, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL))==errNone) {
		if (launchFlags&sysAppLaunchFlagNewGlobals) {
			if (RomCheck()==false)
				return;

			DBCreateDatabase(DBNameBookmarks, 0);
			if (Start()==false)
				return;
		} else
			DocClose(&doc);

		if ((doc=DocOpen(dbName, NULL, 0))!=NULL) {
			PMSetPref(NULL, 0, PrfDocument);
			fReaderRun();
		}

		if (launchFlags&sysAppLaunchFlagNewGlobals) {
			EventLoop();
			Stop();
		}
	}
}

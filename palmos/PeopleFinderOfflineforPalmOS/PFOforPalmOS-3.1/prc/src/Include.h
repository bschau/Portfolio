#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include <VFSMgr.h>
#include <FrmGlue.h>
#include <TblGlue.h>
#include "Resources.h"

/* macros */
#define EVER ;;
#define PREFSVER 3
#define DBVERSION 3

enum { FDFound=0, FDNotFound, FDError };
enum { PrfApplication=0, PrfState };

typedef struct {
	Char name[dmDBNameLength];
	Char *rootPath;
	DmOpenRef db;
	LocalID id;
       	UInt16 card;
	Boolean isVFS;
	FileRef fileRef;
	Boolean isOpen;
} DB;

typedef struct {
	UInt32 dispW;
	UInt32 dispH;
	UInt32 depth;
	Boolean colorEnabled;
	Boolean oneHandedEnabled;
	IndexedColorType backColor;
	IndexedColorType foreColor;
} UI;

typedef struct {
	FormType *oldForm;
	UInt16 oldId;
	FormActiveStateType state;
} FormSave;

typedef struct {
	UInt16 flags;
} Prefs;
#define PFlgBluetooth 1

typedef struct {
	Char name[SearchNameLength+2];
	Char phone[MaxPhoneLength+2];
} State;

/* DB.c */
UInt16 DBFindDatabase(DB *);
void DBInit(DB *, Char *, Char *);
Boolean DBOpen(DB *, UInt16, Boolean);
void DBClose(DB *);
void DBClear(DB *);
MemHandle DBGetRecord(DB *, UInt16);
MemHandle DBGetRecordSilent(DB *, UInt16);
UInt16 DBNumRecords(DB *);

/* NameSearch.c */
void NSOpen(void);
void NSClose(void);
void NSSetup(Char *, Char *);
UInt32 NSFindPerson(Char *, UInt32, UInt32);
UInt32 NSNextPerson(Char *, UInt32);
Boolean NSContainsPerson(Int32);
UInt16 NSCountNames(void);
Char *NSUncrunchPhone(Char *, Char *);
Char *NSUppercaseWords(Char *);

/* PhoneSearch.c */
Boolean PhoneSearch(Char *, MemHandle *, UInt16 *);

/* PilotMain.c */
#ifdef DEBUG
void HexDump(UInt8 *, UInt16);
#else
#define HexDump(x, y) ;
#endif
Boolean PMGetPref(void *, UInt16, UInt16, Boolean);
void PMSetPref(void *, UInt16, UInt16, Boolean);
Boolean PMBtToggleEnabled(UInt16 *, LocalID *);
Err PMBtToggleOn(void);

/* UI.c */
UI *UIInit(void);
void *UIObjectGet(UInt16);
void UIObjectShow(UInt16);
void UIObjectHide(UInt16);
void UIObjectFocus(UInt16);
void UIFormPrologue(FormSave *, UInt16, FormEventHandlerType *);
Boolean UIFormEpilogue(FormSave *, void (*)(), UInt16);
Boolean UIFieldRefocus(UInt16 *, WChar);
Char *UIFieldGetText(UInt16);
void UIFieldSetText(UInt16, Char *);
void UIFieldClear(UInt16);
Boolean UIFieldScrollBarHandler(EventType *, UInt16, UInt16);
Boolean UIFieldScrollBarKeyHandler(EventType *, UInt16, UInt16);
void UIFieldUpdateScrollBar(UInt16, UInt16);
void UIFieldScrollLines(UInt16, Int16);
void UIFieldPageScroll(UInt16, UInt16, WinDirectionType);
void UIListInit(UInt16, void (*)(Int16, RectangleType *, Char **), Int16, Int16);
Boolean UIListKeyHandler(EventType *, UInt16);

/* fDetails.c */
void fDetailsRun(MemHandle, UInt32);
MemHandle fDetailsGetManager(DB *, UInt8, UInt16);

/* fMain.c */
Boolean fMainEH(EventType *);
void fMainRethink(void);

/* fNameSearch.c */
Boolean fNameSearchInit(void);
Boolean fNameSearchEH(EventType *);

/* Debug.c */
#if DEBUG==1
#include <HostControl.h>
void DAlert(UInt8 *, ...);
#define D(x) x
#define DPrint(x...)	HostFPrintF(gHostFile, x); \
			HostFFlush(gHostFile);
#else
#define DAlert(x, ...) ;
#define D(x) ;
#define DPrint(x...)	;
#endif

#ifdef MAIN
	Char hexChars[]="0123456789ABCDEF";
	Char *rootPath="/PALM/Launcher";
	UInt16 currentFormID;
	FormPtr currentForm;
	UI *ui;
	State state;
	Prefs prefs;
	Boolean appStopped;
	DB dbLocations, dbManagers, dbNameSearch;
	Int32 recordIdx;
#ifdef DEBUG
	HostFILE *gHostFile;
#endif
#else
	extern Char hexChars[];
	extern Char *rootPath;
	extern UInt16 currentFormID;
	extern FormPtr currentForm;
	extern UI *ui;
	extern State state;
	extern Prefs prefs;
	extern Boolean appStopped;
	extern DB dbLocations, dbManagers, dbNameSearch;
	extern Int32 recordIdx;
#ifdef DEBUG
	extern HostFILE *gHostFile;
#endif
#endif

#endif

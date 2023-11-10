#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include <DLServer.h>
#include <Helper.h>
#include <HelperServiceClass.h>
#include <VFSMgr.h>
#include "Resources.h"

/* macros */
#define EVER ;;
#define CRID 'HSNS'
#define PREFSVER 1
#define DBVERSION 1
#define DBHotSyncIDs "HSNS_HSNSwitch HSN"

/* Base architecture:  Tungsten T - OS 5.0 */
#define SHSNameIdxOS50 38
#define SHSLogIdxOS50 40
#define SHSSizeOS50 42

enum { PrfApplication=0 };
enum { HsngNull=0, HsngOK, HsngCancel, HsngDelete };

typedef struct {
	Char name[dmDBNameLength];
	DmOpenRef db;
	LocalID id;
       	UInt16 card;
} DB;

typedef struct {
	FormType *oldForm;
	UInt16 oldId;
	FormActiveStateType state;
} FormSave;

typedef struct {
	UInt16 id;
	UInt16 scrollBarId;
	TableType *tbl;
	Int16 top;
	Int16 records;
	Int16 rows;
} Table;

typedef struct {
	Int16 column;
	TableItemStyleType style;
} TableStyle;

struct Node {
	struct Node *next;
	struct Node *prev;
	Char name[NameLength];
};
typedef struct Node Node;

typedef struct {
	Node *head;
	Node *tail;
	Node *tailPred;
	UInt16 count;
} List;

typedef struct {
	Char appName[NameLength+1];
	Char hotSyncName[HotSyncNameLength+1];
} HotSyncRecord;

typedef struct {
	Char hotSyncName[dlkUserNameBufSize+1];
	Char lastHotSyncName[HotSyncNameLength+1];
	Boolean trustPrefIdx;
	UInt16 prefIdx;
	UInt16 slots;
} Prefs;

typedef struct {
	UInt16 nameIdx;
	UInt16 logIdx;
	UInt16 size;
} SHSInfo;

/* AboutForm.c */
void About(void);

/* DB.c */
void DBCreateDatabase(void);
Boolean DBOpen(DB *, Boolean);
void DBClose(DB *);
UInt32 DBSetRecord(DB *, UInt16, HotSyncRecord *);
MemHandle DBGetRecord(DB *, UInt16);
UInt16 DBFindRecord(DB *, HotSyncRecord *);
void DBPurge(DB *);

/* List.c */
void ListInit(List *);
void ListDestroy(List *);
void ListAppDiscover(List *);

/* PilotMain.c */
Boolean PMGetPref(void *, UInt16, UInt16);
void PMSetPref(void *, UInt16, UInt16);

/* UI.c */
void *UIObjectGet(UInt16);
void UIObjectShow(UInt16);
void UIObjectHide(UInt16);
void UIFormPrologue(FormSave *, UInt16, FormEventHandlerType *);
Boolean UIFormEpilogue(FormSave *, void (*)(), UInt16);
void UIFieldFocus(UInt16);
Boolean UIFieldRefocus(UInt16 *, WChar);
Char *UIFieldGetText(UInt16);
void UIFieldSetText(UInt16, Char *);
void UIFieldClear(UInt16);
Boolean UIFieldScrollBarHandler(EventType *, UInt16, UInt16);
Boolean UIFieldScrollBarKeyHandler(EventType *, UInt16, UInt16);
void UIFieldUpdateScrollBar(UInt16, UInt16);
void UIFieldScrollLines(UInt16, Int16);
void UIFieldPageScroll(UInt16, UInt16, WinDirectionType);
void UITableInit(DB *, Table *, UInt16, UInt16, TableDrawItemFuncPtr, TableStyle *);
void UITableScroll(Table *, Int16);
void UITableUpdateValues(Table *, Boolean);
Boolean UITableEvents(Table *, EventType *);
void UITableChanged(Table *, UInt16);
void UIPopupSet(DB *, UInt16, Char *, UInt16);

/* fMain.c */
Boolean fMainInit(void);
Boolean fMainEH(EventType *);

/* fDetails.c */
void fDetailsGet(UInt16 *, HotSyncRecord *);

/* fDetect.c */
void fDetectRun(void);

/* Debug.c */
#if DEBUG==1
#define D(x) x
void DAlert(UInt8 *, ...);
#else
#define D(x) ;
#endif

#ifdef MAIN
	TableStyle tsCustom1[]={ { 0, customTableItem }, { -1 } };
	EventType tapMenuEvent;
	DB dbHSN;
	UInt16 currentFormID;
	FormPtr currentForm;
	Prefs prefs;
	Boolean appStopped;
	List lstPrograms;
	SHSInfo gShs;
#else
	extern TableStyle tsCustom1[];
	extern EventType tapMenuEvent;
	extern DB dbHSN;
	extern UInt16 currentFormID;
	extern FormPtr currentForm;
	extern Prefs prefs;
	extern Boolean appStopped;
	extern List lstPrograms;
	extern SHSInfo gShs;
#endif

#endif

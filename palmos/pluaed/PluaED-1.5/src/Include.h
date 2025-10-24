#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include "Resources.h"

/* macros */
#define EVER ;;
#define PREFSVER 1
#define DBVERSION 1
#define DBNameMemo "MemoDB"
#define DefaultSourceSize 65534

enum { PrfApplication=0 };
enum { FDFound, FDNotFound, FDError };
enum { PositionAtEnd=0, PositionGuess };

typedef struct {
	Char name[dmDBNameLength];
	DmOpenRef db;
	LocalID id;
       	UInt16 card;
} DB;

typedef struct {
	UInt32 dispW;
	UInt32 dispH;
	UInt32 depth;
	Boolean colorEnabled;
	IndexedColorType backColor;
	IndexedColorType foreColor;
} UI;

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

typedef struct {
	UInt16 flags;
	UInt16 category;
	UInt16 sourceSize;
	UInt16 insPt;
	UInt32 uid;
	Int16 pluaVersion;
} Prefs;
#define PFlgUnderlined 1
#define PFlgSearchFromTop 2

/* DB.c */
UInt16 DBFindDatabase(DB *);
void DBInit(DB *, Char *);
Boolean DBOpen(DB *, UInt16, Boolean);
void DBClose(DB *);
void DBClear(DB *);
UInt32 DBSetRecord(DB *, UInt16, UInt16, void *, UInt16, DmComparF);
MemHandle DBGetRecord(DB *, UInt16);

/* PilotMain.c */
Boolean PMGetPref(void *, UInt16, UInt16);
void PMSetPref(void *, UInt16, UInt16);

/* UI.c */
UI *UIInit(void);
void *UIObjectGet(UInt16);
void UIObjectShow(UInt16);
void UIObjectHide(UInt16);
void UIFormPrologue(FormSave *, UInt16, FormEventHandlerType *);
Boolean UIFormEpilogue(FormSave *, void (*)(), UInt16);
void UIFormRedraw(void);
void UIFieldFocus(UInt16);
Boolean UIFieldRefocus(UInt16 *, WChar);
Char *UIFieldGetText(UInt16);
void UIFieldSetText(UInt16, Char *);
void UIFieldClear(UInt16);
void UIFieldSelectAll(UInt16, Char *, Boolean);
Boolean UIFieldScrollBarHandler(EventType *, UInt16, UInt16);
Boolean UIFieldScrollBarKeyHandler(EventType *, UInt16, UInt16);
void UIFieldUpdateScrollBar(UInt16, UInt16);
void UIFieldScrollLines(UInt16, Int16);
void UIFieldPageScroll(UInt16, UInt16, WinDirectionType);
void UITableInit(Table *, UInt16, UInt16, TableDrawItemFuncPtr, TableStyle *, UInt16);
void UITableScroll(Table *, Int16);
void UITableUpdateValues(Table *, Boolean);
Boolean UITableEvents(Table *, EventType *);
void UITableChanged(Table *, UInt16);
void UIPopupSet(DmOpenRef, UInt16, Char *, UInt16);

/* fAbout.c */
void fAboutRun(void);

/* fEdit.c */
void fEditRun(UInt16);
Boolean fEditInit(void);
Boolean fEditEH(EventType *);

/* fMain.c */
Boolean fMainInit(void);
Boolean fMainEH(EventType *);
void fMainRethink(void);

/* fNew.c */
UInt32 fNewRun(void);

/* fPreferences.c */
void fPreferencesRun(void);

/* Debug.c */
#if DEBUG==1
#define D(x) x
void DAlert(UInt8 *, ...);
#else
#define D(x) ;
#endif

#ifdef MAIN
	DB dbMemo;
	UI *ui;
	UInt16 currentFormID, recordIdx;
	FormPtr currentForm;
	Prefs prefs;
	Boolean appStopped;
	Char catName[dmCategoryLength+2];
#else
	extern DB dbMemo;
	extern UI *ui;
	extern UInt16 currentFormID, recordIdx;
	extern FormPtr currentForm;
	extern Prefs prefs;
	extern Boolean appStopped;
	extern Char catName[];
#endif

#endif

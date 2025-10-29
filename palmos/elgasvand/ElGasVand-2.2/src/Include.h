#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include "Resources.h"

/* macros */
#define EVER ;;
#define PREFSVER 1
#define DBVERSION 1
#define DBNameData "EGVx_El, Gas, Vand Data"

enum { FDFound, FDNotFound, FDError };

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

typedef struct {
	UInt16 date;
	UInt16 flags;
	UInt32 el;
	UInt32 gas;
	UInt32 water;
} Record;
#define RFlgNewEMeter 1
#define RFlgNewGMeter 2
#define RFlgNewWMeter 4

/* DB.c */
UInt16 DBFindDatabase(DB *);
void DBCreateDatabase(Char *, UInt16);
void DBInit(DB *, Char *);
Boolean DBOpen(DB *, UInt16, Boolean);
void DBClose(DB *);
void DBClear(DB *);
UInt32 DBSetRecord(DB *, UInt16, UInt16, void *, UInt16, DmComparF);
MemHandle DBGetRecord(DB *, UInt16);

/* PilotMain.c */
void PMFormatDate(Char *, Int16, Int16, Int16);

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
void UITableInit(DB *, Table *, UInt16, UInt16, TableDrawItemFuncPtr, TableStyle *, UInt16);
void UITableScroll(Table *, Int16);
void UITableUpdateValues(Table *, Boolean);
Boolean UITableEvents(Table *, EventType *);
void UITableChanged(Table *, UInt16);
void UIPopupSet(DB *, UInt16, Char *, UInt16);

/* fAbout.c */
void fAboutRun(void);

/* fEdit.c */
Int16 DateSort(void *, void *, Int16, SortRecordInfoType *, SortRecordInfoType *, MemHandle);
Boolean fEditRun(UInt16);

/* fKeyboard.c */
Boolean fKeyboardRun(UInt32 *, UInt16);

/* fMain.c */
Boolean fMainInit(void);
Boolean fMainEH(EventType *);

/* fStatistics.c */
void fStatisticsRun(Int16);
Boolean fStatisticsEH(EventType *);

/* Debug.c */
#if DEBUG==1
#define D(x) x
void DAlert(UInt8 *, ...);
#else
#define D(x) ;
#endif

#ifdef MAIN
	DB dbData;
	UInt16 currentFormID, recordIdx;
	FormType *currentForm;
	Boolean appStopped;
	Record record;
#else
	extern DB dbData;
	extern UInt16 currentFormID, recordIdx;
	extern FormType *currentForm;
	extern Boolean appStopped;
	extern Record record;
#endif

#endif

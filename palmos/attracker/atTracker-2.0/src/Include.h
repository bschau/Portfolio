#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include <Libraries/PalmOSGlue/TxtGlue.h>
#include "Resources.h"

/* macros */
#define EVER ;;
#define CRID 'TTTX'
#define DBTYPE 'DATA'
#define DBARCHIVE 'ARCH' 
#define DBNAME "TTTX_atTracker Data"
#define DBVERSION 1
#define APPVER 1
#define CATNONEDITABLES 1
#define MAGICYEAR 1904

#define EditMaskNew 0x80000000
#define EditMaskIdx 0x40000000
#define EditMaskPrf 0x20000000
#define EditMaskEvt 0x10000000
#define EditMaskAppStop 0x00010000

/* PrfUnlockKey (idx = 3) was removed in 2.0. The hole is to not mess up user
 * preferences */
enum { PrfApplication=0, PrfSaveRecord, PrfSaveRecordTaskText, PrfTimeHistory=4, PrfQuickText };

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
	AppInfoType appInfo;
} AppInfoBlock;

typedef struct {
	UInt16 categoryIdx;
	UInt16 defaultTime;
	UInt16 flags;
	IndexedColorType barColor;
} Preferences;
#define PFlagDoubleTable 1
#define PFlagAlternate 2

typedef struct {
	UInt16 categoryIdx;
	UInt16 date;
	UInt16 time;
} SaveRecordPref;

typedef struct {
	UInt16 date;
	UInt16 time;
} TTask;

/* Sections */
#ifdef _FRM
#undef _FRM
#endif
#define _FRM(x) x __attribute__ ((section("frm")))

#ifdef _UI
#undef _UI
#endif
#define _UI(x) x __attribute__ ((section("ui")))

/* BMH.c */
#if DEBUG
void BMHTest(void);
#endif
void *BMHOpen(Char *);
Int32 BMHSearch(void *, Char *);
void BMHClose(void *);

/* DateTime.c */
UInt16 DTToday(void);
UInt16 DTDatePack(Int16, Int16, Int16);
void DTDateUnpack(UInt16, Int16 *, Int16 *, Int16 *);
void DTDateFormatLong(Char *, UInt16);
void DTDateFormatShort(Char *, UInt16);
UInt16 DTTimePack(UInt8, UInt8);
void DTTimeUnpack(UInt16, UInt8 *, UInt8 *);
Char DTTimeDelimiter(void);
void DTTimeFormat(Char *, UInt16);
void DTSelectorSetDate(UInt16, Char *, UInt32);
void DTSelectorSetTime(UInt16, Char *, UInt32);

/* DB.c */
Int16 TaskCompare(void *, void *, Int16, SortRecordInfoType *, SortRecordInfoType *, MemHandle);
void DBCreateDatabase(void);
void DBInit(DB *, Char *);
Boolean DBOpen(DB *, UInt16, Boolean);
void DBClose(DB *);
void DBSetRecord(UInt16, UInt16, Char *, UInt16, UInt16);
MemHandle DBGetRecord(UInt16);
UInt16 DBIdToIndex(UInt32);

/* Find.c */
void FSearch(FindParamsType *);

/* PalmPrint.c */
Boolean PalmPrintOpen(void);
void PalmPrintGetInfo(UInt32 *, UInt32 *);
void PalmPrintLineModeStart(void);
void PalmPrintLineModeStop(void);
void PalmPrintLineModePrint(Char *);
void PalmPrintSetBold(void);
void PalmPrintSetPlain(void);

/* PilotMain.c */
Boolean PMRomCheck(void);
Boolean PMCanDoubleTable(void);

/* QuickText.c */
void QuickTextGet(void);
void QuickTextPut(void);
void QuickTextListInit(UInt16);
UInt16 QuickTextGetCount(void);
void QuickTextManager(void);
Char *QuickTextIdxToText(UInt16);

/* String.c */
Char *StringSkipLeading(Char *);
void StringWhiteSpaceConvert(Char *, Char *, UInt16);

/* TimeSelector.c */
Boolean TimeSelector(UInt8 *, UInt8 *, const Char *);
void TimeHistoryGet(UInt16);
void TimeHistoryPut(UInt16);
void TimeHistoryAdd(UInt8, UInt8);

/* UI.c */
_UI(void *UIObjectGet(UInt16));
_UI(void UIObjectShow(UInt16));
_UI(void UIObjectHide(UInt16));
_UI(void UIFormPrologue(FormSave *, UInt16, FormEventHandlerType *));
_UI(Boolean UIFormEpilogue(FormSave *, void (*)(), UInt16));
_UI(void UIFieldFocus(UInt16));
_UI(Boolean UIFieldRefocus(UInt16 *, WChar));
_UI(Char *UIFieldGetText(UInt16));
_UI(void UIFieldSetText(UInt16, Char *));
_UI(void UIFieldClear(UInt16));
_UI(Boolean UIFieldScrollBarHandler(EventType *, UInt16, UInt16));
_UI(Boolean UIFieldScrollBarKeyHandler(EventType *, UInt16, UInt16));
_UI(void UIFieldUpdateScrollBar(UInt16, UInt16));
_UI(void UIFieldScrollLines(UInt16, Int16));
_UI(void UIFieldPageScroll(UInt16, UInt16, WinDirectionType));
_UI(void UITableInit(DB *, Table *, UInt16, UInt16, TableDrawItemFuncType *, TableStyle *));
_UI(void UITableScroll(Table *, Int16));
_UI(void UITableUpdateValues(Table *, Boolean));
_UI(Boolean UITableEvents(Table *, EventType *));
_UI(void UITableChanged(Table *, UInt16));
_UI(void UIPopupSet(DB *, UInt16, Char *, UInt16));
_UI(Boolean UIGadgetHandler(EventType *, UInt16));

/* fAbout.c */
void fAboutRun(void);

/* fArchive.c */
Boolean fArchiveInit(void);
Boolean fArchiveEH(EventType *);

/* fCleanUp.c */
Boolean fCleanUpInit(void);
Boolean fCleanUpEH(EventType *);

/* fEdit.c */
Boolean fEditInit(void);
Boolean fEditEH(EventType *);
Boolean fEditSaveRecord(Boolean, MemHandle *, UInt16 *, UInt16 *, UInt16 *);

/* fMain.c */
void fMainFirstTime(void);
void fMainLastTime(void);
Boolean fMainInit(void);
Boolean fMainEH(EventType *);

/* fManager.c */
void fManagerRun(Char *, UInt16 *, LocalID *);

/* fPreferences.c */
void fPreferencesRun(void);

/* fRange.c */
Boolean fRangeRun(UInt16 *, UInt16 *);

/* fRestore.c */
Boolean fRestoreInit(void);
Boolean fRestoreEH(EventType *);

/* fSummary.c */
Boolean fSummaryInit(void);
Boolean fSummaryEH(EventType *);

/* Debug.c */
#if DEBUG==1
#define D(x) x
void DAlert(UInt8 *, ...);
void DPreload(void);
#else
#define D(x) ;
#endif

#ifdef MAIN
	UInt16 currentFormID;
	DB dbData;
	FormType *currentForm;
	Preferences prefs;
	UInt8 categoryName[dmCategoryLength+2], selector1[SelectorLabelLength+2], selector2[SelectorLabelLength+2];
	EventType editEvent;
	UInt32 editCmd;
	Boolean canDoubleTable, canColor;
	BitmapType *bmpBullet;
#else
	extern UInt16 currentFormID, recordIdx;
	extern DB dbData;
	extern FormType *currentForm;
	extern Preferences prefs;
	extern UInt8 categoryName[], selector1[], selector2[];
	extern EventType editEvent;
	extern UInt32 editCmd;
	extern UInt16 unlockKey;
	extern Boolean canDoubleTable, canColor;
	extern BitmapType *bmpBullet;
#endif

#endif

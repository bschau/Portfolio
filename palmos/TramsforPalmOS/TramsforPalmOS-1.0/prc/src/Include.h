#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include <FrmGlue.h>
#include "Resources.h"

/* macros */
#define EVER ;;
#define PREFSVER 1
#define DBVERSION 1

enum { PrfApplication=0, PrfState };
enum { FDFound, FDNotFound, FDError };

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
	Boolean oneHandedEnabled;
	IndexedColorType backColor;
	IndexedColorType foreColor;
} UI;

//typedef struct {
//	UInt16 id;
//	Char *button;
//} Gadget;

typedef struct {
	FormType *oldForm;
	UInt16 oldId;
	FormActiveStateType state;
} FormSave;

typedef struct {
	UInt16 flags;
	Char autoComment[ExpensesCommentLength];
} Prefs;
#define PFlgAutoComment 1

typedef struct {
	Int16 countryIndex;
	Int16 currencyIndex;
	Char currentTER[dmDBNameLength];
} State;

typedef struct {
	DateType date;
	Int16 country;
	Int16 currency;
	Char strCurrency[4];
	UInt16 flags;
	Char expCat[4];
	Char amount[ExpensesAmountLength];
	Char comment[ExpensesCommentLength];
} Expense;
#define EFlgOriginalReceipt 1

/* DB.c */
UInt16 DBFindDatabase(DB *);
void DBCreateDatabase(Char *, UInt16);
void DBInit(DB *, Char *);
Boolean DBOpen(DB *, UInt16, Boolean);
void DBClose(DB *);
void DBClear(DB *);
UInt32 DBSetRecord(DB *, UInt16, UInt16, void *, UInt16, DmComparF);
MemHandle DBGetRecord(DB *, UInt16);

/* ExpCat.c */
UInt16 ExpCatGetIndex(Char *);
Char *ExpCatGetCategory(Char *);

/* Expenses.c */
Int16 ExpensesSort(void *, void *, Int16, SortRecordInfoType *, SortRecordInfoType *, MemHandle);
void ExpensesInit(Expense *);
MemHandle ExpensesRecordToMH(Expense *);
void ExpensesMHToRecord(Expense *, MemHandle);

/* Locale.c */
void LocOpen(void);
void LocClose(void);
void LocValidate(State *);
Int16 LocCountryToIndex(Char *);
Char *LocIndexToCountry(Int16);
const UInt8 *LocIndexToCountryCode(Int16);
Int16 LocCurrencyToIndex(Char *);
Char *LocIndexToCurrency(Int16);
Int16 LocCountryToCurrency(Int16);
void LocSetupList(UInt16, Int16, Boolean);

/* PilotMain.c */
#ifdef DEBUG
void HexDump(UInt8 *, UInt16);
#else
#define HexDump(x, y)	;
#endif
Boolean PMGetPref(void *, UInt16, UInt16, Boolean);
void PMSetPref(void *, UInt16, UInt16, Boolean);

/* TERList.c */
void TLBuildList(void);
void TLSort(void);
void TLDestroyList(void);
Boolean TLHasTer(Char *);
UInt16 TLGetIndex(Char *);

/* UI.c */
UI *UIInit(void);
void *UIObjectGet(UInt16);
void UIObjectShow(UInt16);
void UIObjectHide(UInt16);
void UIObjectFocus(UInt16);
void UIFormPrologue(FormSave *, UInt16, FormEventHandlerType *);
Boolean UIFormEpilogue(FormSave *, void (*)(), UInt16);
//void UIFormRedraw(void);
Boolean UIFieldRefocus(UInt16 *, WChar);
Char *UIFieldGetText(UInt16);
void UIFieldSetText(UInt16, Char *);
void UIFieldClear(UInt16);
//void UIFieldSelectAll(UInt16, Char *, Boolean);
Boolean UIFieldScrollBarHandler(EventType *, UInt16, UInt16);
Boolean UIFieldScrollBarKeyHandler(EventType *, UInt16, UInt16, Boolean);
void UIFieldUpdateScrollBar(UInt16, UInt16);
void UIFieldScrollLines(UInt16, Int16);
void UIFieldPageScroll(UInt16, UInt16, WinDirectionType);
void UIListInit(UInt16, void (*)(Int16, RectangleType *, Char **), Int16, Int16);
Boolean UIListKeyHandler(EventType *, UInt16);
//void UIPopupSet(DmOpenRef, UInt16, Char *, UInt16);
//UInt16 UIGadgetFontHandler(Gadget *, FontID, UInt16, UInt16);

/* fAbout.c */
void fAboutRun(void);

/* fCountry.c */
Boolean fCountryRun(Int16 *, Int16 *);

/* fExpenses.c */
void fExpensesRun(UInt16);
Boolean fExpensesInit(void);
Boolean fExpensesEH(EventType *);

/* fMain.c */
Boolean fMainInit(void);
Boolean fMainEH(EventType *);

/* fPreferences.c */
Boolean fPreferencesInit(void);
Boolean fPreferencesEH(EventType *);

/* Debug.c */
#if DEBUG==1
#include <HostControl.h>
void DAlert(UInt8 *, ...);
#define D(x)		x
#define DPrint(x...)	HostFPrintF(gHostFile, x); \
			HostFFlush(gHostFile);
#else
#define DAlert(x, ...)	;
#define D(x)		;
#define DPrint(x...)	;
#endif

#ifdef MAIN
	DB dbExpCat;
	UInt16 currentFormID, recordIdx;
	FormType *currentForm;
	Boolean appStopped;
	UI *ui;
	Prefs prefs;
	State state;
	SysDBListItemType *terList;
	UInt16 terCnt;
#ifdef DEBUG
	HostFILE *gHostFile;
#endif
#else
	extern DB dbExpCat;
	extern UInt16 currentFormID, recordIdx;
	extern FormType *currentForm;
	extern Boolean appStopped;
	extern UI *ui;
	extern Prefs prefs;
	extern State state;
	extern SysDBListItemType *terList;
	extern UInt16 terCnt;
#ifdef DEBUG
	extern HostFILE *gHostFile;
#endif
#endif

#endif

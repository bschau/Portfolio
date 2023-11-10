#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include "Resources.h"

/* macros */
#define EVER ;;
#define CRID 'GMAD'
#define PREFSVER 1
#define DBVERSION 1
#define DBNameTemplate "GMAD_Gone Mad! Template"
#define DBNameKeyword "GMAD_Gone Mad! Keyword"

enum { PrfApplication=0, PrfTemplate, PrfKeywords };
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
	UInt16 category;
	UInt16 catExchangeIdx;
	UInt16 exchangeFlags;
} Prefs;
#define ExchgEDelete 1
#define ExchgIDelete 2

/* DB.c */
UInt16 DBFindDatabase(DB *);
void DBCreateDatabase(Char *, UInt16);
void DBInit(DB *, Char *);
Boolean DBOpen(DB *, UInt16, Boolean);
void DBClose(DB *);
void DBClear(DB *);
UInt32 DBSetRecord(DB *, UInt16, UInt16, void *, UInt16, DmComparF);
MemHandle DBGetRecord(DB *, UInt16);

/* IR.c */
void IRBeam(Char *);
void IRReceive(ExgSocketType *, DB *);

/* PilotMain.c */
Boolean PMRun(void);
Boolean PMGetPref(void *, UInt16, UInt16);
void PMSetPref(void *, UInt16, UInt16);

/* SortFunctions.c */
Int16 SFString(void *, void *, Int16, SortRecordInfoPtr, SortRecordInfoPtr, MemHandle);

/* Story.c */
void StoryDone(void);
void StoryGet(UInt16);
UInt16 StoryGetCount(void);
Char *StoryGetKeyword(UInt16);
UInt16 StoryGetKeywordLength(UInt16);
Char *StoryGetValue(UInt16);
void StorySetValue(UInt16, Char *);
Boolean StoryContainsBlanks(void);
MemHandle StoryGetFinal(void);
Boolean StorySaveKeywords(void);
void StoryClearKeywords(void);

/* String.c */
UInt16 StringGetExact(Char **);

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
void UITableUpdateValues(Table *);
Boolean UITableEvents(Table *, EventType *);
void UITableChanged(Table *, UInt16);
void UIPopupSet(DB *, UInt16, Char *, UInt16);

/* fAbout.c */
void fAboutRun(void);

/* fEditTemplate.c */
void fEditTemplateRun(UInt16);
Boolean fEditTemplateInit(void);
Boolean fEditTemplateEH(EventType *);

/* fHelp.c */
void fHelpRun(Char *, UInt16);

/* fKeywords.c */
void fKeywordsRun(UInt16);
Boolean fKeywordsInit(void);
Boolean fKeywordsEH(EventType *);

/* fMain.c */
Boolean fMainInit(void);
Boolean fMainEH(EventType *);
void fMainRethink(void);

/* fMemoBrowser.c */
UInt16 fMemoBrowserRun(void);

/* fStory.c */
void fStoryRun(UInt16);
Boolean fStoryInit(void);
Boolean fStoryEH(EventType *);

/* fTemplateExport.c */
void fTemplateExportRun(void);

/* fTemplateExportSingle.c */
void fTemplateExportSingleRun(Char *);

/* fTemplateImport.c */
void fTemplateImportRun(void);

/* Debug.c */
#if DEBUG==1
#define D(x) x
void DAlert(UInt8 *, ...);
void DPreload(void);
#else
#define D(x) ;
#endif

#ifdef MAIN
	DB dbTemplate, dbKeyword, dbMemo;
	UInt16 currentFormID, catTo, catFrom, recordIdx;
	FormPtr currentForm;
	Prefs prefs;
	Boolean appStopped;
	Char catName1[dmCategoryLength+2], catName2[dmCategoryLength+2];
	Char catName3[dmCategoryLength+2];
#else
	extern DB dbTemplate, dbKeyword, dbMemo;
	extern UInt16 currentFormID, catTo, catFrom, recordIdx;
	extern FormPtr currentForm;
	extern Prefs prefs;
	extern Boolean appStopped;
	extern Char catName1[], catName2[], catName3[];
#endif

#endif

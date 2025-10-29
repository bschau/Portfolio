#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include <Libraries/PalmOSGlue/TxtGlue.h>
#include "Resources.h"

/* macros */
#define EVER ;;
#define CRID 'dIcT'
#define PREFSVER 1
#define DBVERSION 1
#define DBNameTGroup "dIcT_Dictate TGroup"
#define DBNameTSentence "dIcT_Dictate TSentence"
#define DBNameTGXref "dIcT_Dictate T-G XREF"
#define DBNameSGroup "dIcT_Dictate SGroup"
#define DBNameSSentence "dIcT_Dictate SSentence"
#define DBNameSGXref "dIcT_Dictate S-G XREF"
#define DBNameScore "dIcT_Dictate Score"
#define DBNameAnswer "dIcT_Dictate Answer"
#define DBNameBeam "dIcT_Dictate Beam.dIcT"

enum { PrfApplication=0, PrfEdit, PrfGameInfo, PrfGameField };
enum { PFuncNotSet=0, PFuncStudent, PFuncTeacher };
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
	UInt32 guid;
	UInt32 suid;
} XRefRecord;

typedef struct {
	UInt8 function;
	UInt8 exchangeFlags;
	UInt16 catSentenceIdx;
	UInt16 catExchangeIdx;
	Char userName[HotSyncNameLength+2];
} Prefs;
#define ExchgIDelete 1
#define ExchgEDelete 2
#define ExchgSaveScore 4

typedef struct {
	UInt32 guid;
	UInt16 score;
	UInt16 round;
	UInt16 sentenceLength;
	DateType gameEnded;
	Boolean saved;
} Game;

typedef struct {
	UInt16 score;
	UInt16 round;
	UInt16 year;
	UInt16 month;
	UInt16 day;
	Char name[HotSyncNameLength+1];
	Char group[GroupNameLength+1];
} Score;

/* Beam.c */
void Beam(Char *, DB *, DB *);

/* DB.c */
UInt16 DBFindDatabase(DB *);
void DBCreateDatabase(Char *, UInt16);
void DBInit(DB *, Char *);
Boolean DBOpen(DB *, UInt16, Boolean);
void DBClose(DB *);
void DBClear(DB *);
UInt32 DBSetRecord(DB *, UInt16, UInt16, void *, UInt16, DmComparF);
MemHandle DBGetRecord(DB *, UInt16);

/* Exchange.c */
void ExchangeDelete(UInt32, DB *, DB *);
UInt16 ExchangeDBToStudent(UInt32);
UInt16 ExchangeIRToStudent(void);

/* IR.c */
void IRRegister(void);
void IRReceive(ExgSocketType *);
void IRImport(void);

/* PilotMain.c */
Boolean PMRun(void);
Boolean PMGetPref(void *, UInt16, UInt16);
void PMSetPref(void *, UInt16, UInt16);
Boolean PMGetGroupName(DB *, UInt32, Char *);

/* SortFunctions.c */
Int16 SFString(void *, void *, Int16, SortRecordInfoPtr, SortRecordInfoPtr, MemHandle);
Int16 SFScore(void *, void *, Int16, SortRecordInfoPtr, SortRecordInfoPtr, MemHandle);

/* String.c */
Char *StringPurify(Char *);
void StringStrip(Char *, Char *);

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

/* XRef.c */
UInt16 XRefIndex(DB *, UInt32, Int32);
void XRefInsert(DB *, XRefRecord *);
UInt16 XRefCount(DB *, UInt32);
void XRefDelete(DB *, UInt32, Int32);
void XRefDeleteAllGuid(DB *, UInt32);
void XRefDeleteAllSuid(DB *, UInt32);
void XRefSwap(DB *, UInt32, UInt16, UInt16);
Boolean XRefExists(DB *, XRefRecord *);

/* fAbout.c */
void fAboutRun(void);

/* fDictationDone.c */
Boolean fDictationDoneEH(EventType *);

/* fGroup.c */
void fGroupOpen(UInt16);
Boolean fGroupEH(EventType *);

/* fSaveScore.c */
Boolean fSaveScoreRun(void);

/* fSDictate.c */
Boolean fSDictateInit(void);
Boolean fSDictateEH(EventType *);

/* fSentence.c */
void fSentenceDrawTable(void *, Int16, Int16, RectangleType *);
Boolean fSentenceInit(void);
Boolean fSentenceEH(EventType *);
void fSentenceEditFromPrefs(void);
Boolean fEditSentenceInit(void);
Boolean fEditSentenceEH(EventType *);

/* fSentenceExport.c */
void fSentenceExportRun(void);

/* fSentenceImport.c */
void fSentenceImportRun(void);

/* fStudent.c */
Boolean fStudentInit(void);
Boolean fStudentEH(EventType *);
void fStudentIR(void);

/* fTDictate.c */
Boolean fTDictateInit(void);
Boolean fTDictateEH(EventType *);

/* fTeacher.c */
Boolean fTeacherInit(void);
Boolean fTeacherEH(EventType *);

/* fViewAnswers.c */
Boolean fViewAnswersInit(void);
Boolean fViewAnswersEH(EventType *);

/* fWait.c */
Boolean fWaitEH(EventType *);

/* Debug.c */
#if DEBUG==1
#define D(x) x
void DAlert(UInt8 *, ...);
void DPreLoad(void);
#else
#define D(x) ;
#endif

#ifdef MAIN
	TableStyle tsCustom1[]={ { 0, customTableItem }, { -1 } };
	Table tblSentence;
	DB dbTGroup, dbTSentence, dbTXref;
	DB dbSGroup, dbSSentence, dbSXref;
	DB dbScore, dbMemo, dbAnswer, dbBeam;
	UInt16 currentFormID, catTo, catFrom;
	UInt32 groupUid;
	FormPtr currentForm;
	Prefs prefs;
	Boolean appStopped;
	Char catSentenceName[dmCategoryLength+2];
	Char catFromName[dmCategoryLength+2], catToName[dmCategoryLength+2];
	Game game;
	BitmapType *bmpBullet;
#else
	extern TableStyle tsCustom1[];
	extern Table tblSentence;
	extern DB dbTGroup, dbTSentence, dbTXref;
	extern DB dbSGroup, dbSSentence, dbSXref;
	extern DB dbScore, dbMemo, dbAnswer, dbBeam;
	extern UInt16 currentFormID, catTo, catFrom;
	extern UInt32 groupUid;
	extern FormPtr currentForm;
	extern Prefs prefs;
	extern Boolean appStopped;
	extern Char catSentenceName[];
	extern Char catFromName[], catToName[];
	extern Game game;
	extern BitmapType *bmpBullet;
#endif

#endif

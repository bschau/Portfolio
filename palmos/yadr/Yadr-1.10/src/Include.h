#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include <Helper.h>
#include <HelperServiceClass.h>
#include <VFSMgr.h>
#include <HostControl.h>
#include "Resources.h"

/* macros */
#define EVER ;;
#define PREFSVER 1
#define DBVERSION 1
#define DBNameBookmarks "YADr_Yadr Bookmarks"
#define DBNameHistory "YADr_Yadr History"
#define MaxPathLen 2048
#define yadrFindEvent sysEventFirstUserEvent

enum { PrfApplication=0, PrfDocument, PrfVFSPath };
enum { FDFound, FDNotFound, FDError };

typedef struct {
	Char name[dmDBNameLength];
	Char *path;
	DmOpenRef db;
	LocalID id;
       	UInt16 card;
	FileRef fileRef;
	Boolean isOpen;
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
	UInt16 flags;
	UInt16 font;
	UInt8 drawMode;
	UInt8 searchDocs;
	IndexedColorType foreColor;
	IndexedColorType backColor;
	UInt8 controlMode;
	UInt16 lineSpacing;
} Prefs;
#define PFHistory 1
#define PFFromTop 2
#define PFFullScreen 4
#define PDMLeft 0
#define PDMCenter 1
#define PDMRight 2
#define PDMJustify 3
#define PSDMemory 0
#define PSDCard 1
#define PCMUpDown 0
#define PCMLeftRight 1

typedef struct {
	UInt16 version;
	UInt16 spare;
	UInt32 length;
	UInt16 records;
	UInt16 size;
	UInt32 position;
} DOCHeader;

typedef struct {
	DB db;
	Char title[DocNameLength];
	UInt16 records;
	UInt16 version;
	UInt32 length;
	UInt8 *page;
	UInt16 pageSize;
	UInt16 *pageLengths;
	UInt16 pageNo;
	UInt32 posInPage;
	UInt32 position;
} Document;

typedef struct {
	UInt32 position;
	UInt8 vfs;
} PrfSaveDoc;

struct DocNode {
	struct DocNode *next;
	struct DocNode *prev;
	Char *docName;
	Char *fileName;
	UInt8 type;
};
typedef struct DocNode DocNode;
#define NodeTypeFile 1
#define NodeTypeDirectory 2

typedef struct {
	DocNode *head;
	DocNode *tail;
	DocNode *tailPred;
	UInt16 count;
} DocList;

typedef struct {
	UInt16 capacity;
	UInt16 growth;
	UInt16 length;
	UInt8 *data;
} StringBuffer;

typedef struct {
	Char docName[DocNameLength];
	Char bookmark[BookmarkLength];
	UInt32 position;
} Bookmark;

typedef struct {
	Char docName[DocNameLength];
	UInt32 opened;
	UInt32 last;
} History;
#define HCatOpened 0
#define HCatRead 1

/* Bookmarks.c */
Int16 BookmarksSort(void *, void *, Int16, SortRecordInfoType *, SortRecordInfoType *, MemHandle);
Boolean BookmarksOpen(void);
void BookmarksClose(void);
MemHandle BookmarksBuildList(Char *, UInt16, Boolean, UInt16, StringBuffer *);
void BookmarksAdd(Char *, UInt32, Char *);
UInt32 BookmarksGetPosition(Char *, Int16);

/* DB.c */
UInt16 DBFindDatabase(DB *);
void DBCreateDatabase(Char *, UInt16);
void DBInit(DB *, Char *, Char *);
Boolean DBOpen(DB *, UInt16, Boolean);
void DBClose(DB *);
void DBClear(DB *);
UInt32 DBSetRecord(DB *, UInt16, UInt16, void *, UInt16, DmComparF);
MemHandle DBGetRecord(DB *, UInt16);
MemHandle DBGetRecordSilent(DB *, UInt16);
UInt16 DBNumRecords(DB *);

/* Doc.c */
Document *DocOpen(Char *, Char *, UInt32);
void DocClose(Document **);
void DocPagePos(Document *, UInt32, UInt16 *, UInt32 *);
Boolean DocGetLine(Document *, UInt16, UInt16 *, Boolean *);
void DocGetPage(Document *, UInt16);

/* History.c */
Int16 HistorySort(void *, void *, Int16, SortRecordInfoType *, SortRecordInfoType *, MemHandle);
Boolean HistoryOpen(void);
void HistoryClose(void);
Boolean HistoryAdd(Char *, UInt32 *);
void HistoryMarkAsRead(UInt32);
void HistoryClear(UInt16);

/* IR.c */
void IRBeam(DocNode *);
Boolean IRReceive(ExgSocketType *, Boolean *, Err *);

/* PilotMain.c */
Boolean PMGetPref(void *, UInt16, UInt16);
void PMSetPref(void *, UInt16, UInt16);

/* StringBuffer.c */
StringBuffer *SBNew(UInt16);
void SBFree(StringBuffer *);
void SBAppend(StringBuffer *, UInt8 *, UInt16);
UInt8 *SBGetData(StringBuffer *, UInt16 *);
void SBReset(StringBuffer *);

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

/* fBookmarks.c */
void fBookmarksRun(UInt16);
void fBookmarksClose(void);
Boolean fBookmarksInit(void);
Boolean fBookmarksEH(EventType *);

/* fFind.c */
void fFindAlloc(void);
void fFindClose(void);
void fFindReset(void);
Boolean fFindRun(UInt32 *, Document *);
Boolean fFindNext(UInt32 *, Document *);

/* fFormatting.c */
Boolean fFormattingInit(void);
Boolean fFormattingEH(EventType *);

/* fHistory.c */
Boolean fHistoryInit(void);
Boolean fHistoryEH(EventType *);

/* fMain.c */
void fMainAlloc(void);
void fMainFree(void);
void fMainClose(void);
Boolean fMainInit(void);
Boolean fMainEH(EventType *);
void fMainStatus(Char *);
void fMainRethink(void);

/* fPreferences.c */
void fPreferencesRun(void);

/* fReader.c */
void fReaderAlloc(void);
void fReaderFree(void);
void fReaderRun(void);
Boolean fReaderInit(void);
Boolean fReaderEH(EventType *);

/* Debug.c */
#if DEBUG==1
#define D(x) x
void DAlert(UInt8 *, ...);
#define DOpen(n)	gHostFile=HostFOpen("/tmp/" ## n ## ".log", "a+"); \
			ErrFatalDisplayIf(gHostFile==NULL, "DOpen failed"); \
			DPrint("====== " ## n ##" Started ===\n");
#define DClose()	if (gHostFile) HostFClose(gHostFile);
#define DPrint(x...)	HostFPrintF(gHostFile, x); \
			HostFFlush(gHostFile);
#else
#define D(x) ;
#define DOpen(n)	;
#define DClose()	;
#define DPrint(x...)	;
#endif

#ifdef MAIN
	Char *txtPleaseWait="Please Wait!";
	TableStyle style1[]={ { 0, customTableItem }, { -1 } };
	UInt16 currentFormID, recordIdx;
	FormType *currentForm;
	Boolean appStopped;
	Prefs prefs;
	Document *doc;
	UInt16 dispW, dispH;
	StringBuffer *sbReader, *sbBookmarks, *sbGoTo;
	BitmapType *folderBitmap;
	DB dbBookmarks, dbHistory;
	Char catName1[dmCategoryLength+2], catName2[dmCategoryLength+2];
	Char *findTerm;
#ifdef DEBUG
	HostFILE *gHostFile;
#endif
#else
	extern Char *txtPleaseWait;
	extern TableStyle style1[];
	extern UInt16 currentFormID, recordIdx;
	extern FormType *currentForm;
	extern Boolean appStopped;
	extern Prefs prefs;
	extern Document *doc;
	extern UInt16 dispW, dispH;
	extern StringBuffer *sbReader, *sbBookmarks, *sbGoTo;
	extern BitmapType *folderBitmap;
	extern DB dbBookmarks, dbHistory;
	extern Char catName1[], catName2[];
	extern Char *findTerm;
#ifdef DEBUG
	extern HostFILE *gHostFile;
#endif
#endif

#endif

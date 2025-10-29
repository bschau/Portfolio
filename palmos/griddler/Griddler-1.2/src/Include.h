#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include "Resources.h"

/* macros */
#define EVER ;;
#define CRID 'griD'
#define PREFSVER 1
#define DBVERSION 1

enum { PrfApplication=0, PrfState, PrfPuzzle, PrfGrid };
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
	IndexedColorType backColor;
	IndexedColorType foreColor;
} UI;

typedef struct {
	FormType *oldForm;
	UInt16 oldId;
	FormActiveStateType state;
} FormSave;

typedef struct {
	UInt16 cellSize;
} Prefs;

typedef struct {
	Int16 lsPCList;
	Int16 lsPuzzleList;
} State;
	
typedef struct {
	Char currentPuzzle[dmDBNameLength];
	UInt32 currentPuzzleId;
	UInt16 gridSize;
} Puzzle;

/* DB.c */
UInt16 DBFindDatabase(DB *);
void DBCreateDatabase(Char *, UInt16);
void DBInit(DB *, Char *);
Boolean DBOpen(DB *, UInt16, Boolean);
void DBClose(DB *);
void DBClear(DB *);
UInt32 DBSetRecord(DB *, UInt16, UInt16, void *, UInt16, DmComparF);
MemHandle DBGetRecord(DB *, UInt16);

/* PCList.c */
void PCBuildList(void);
void PCSort(void);
void PCDestroyList(void);
UInt16 PCGetIndex(Char *);

/* PilotMain.c */
#ifdef DEBUG
void HexDump(UInt8 *, UInt16);
#else
#define HexDump(x, y)	;
#endif
Boolean PMGetPref(void *, UInt16, UInt16, Boolean);
void PMSetPref(void *, UInt16, UInt16, Boolean);

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
Boolean UIFieldScrollBarKeyHandler(EventType *, UInt16, UInt16, Boolean);
void UIFieldUpdateScrollBar(UInt16, UInt16);
void UIFieldScrollLines(UInt16, Int16);
void UIFieldPageScroll(UInt16, UInt16, WinDirectionType);
void UIListInit(UInt16, void (*drawFunc)(Int16, RectangleType *, Char **), Int16, Int16);
Boolean UIListKeyHandler(EventType *, UInt16);

/* fAbout.c */
void fAboutRun(void);

/* fGame.c */
Boolean fGameInit(void);
void fGameCloseGridWindow(void);
void fGameCloseGrid(void);
void fGameSaveGrid(void);
Boolean fGameLoadGrid(void);
Boolean fGameEH(EventType *);

/* fMain.c */
Boolean fMainInit(void);
Boolean fMainEH(EventType *);

/* fPuzzleDone.c */
Boolean fPuzzleDoneInit(void);
Boolean fPuzzleDoneEH(EventType *);

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
	UI *ui;
	DB dbPuzzle;
	UInt16 currentFormID, recordIdx, osGood;
	FormType *currentForm;
	Boolean appStopped;
	Prefs prefs;
	State state;
	Puzzle puzzle;
	SysDBListItemType *pcList;
	UInt16 pcCnt, puzzleCnt, cellsX, cellsY;
	UInt8 *grid;
#ifdef DEBUG
	HostFILE *gHostFile;
#endif
#else
	extern UI *ui;
	extern DB dbPuzzle;
	extern UInt16 currentFormID, recordIdx, osGood;
	extern FormType *currentForm;
	extern Boolean appStopped;
	extern Prefs prefs;
	extern State state;
	extern Puzzle puzzle;
	extern SysDBListItemType *pcList;
	extern UInt16 pcCnt, puzzleCnt, cellsX, cellsY;
	extern UInt8 *grid;
#ifdef DEBUG
	extern HostFILE *gHostFile;
#endif
#endif

#endif

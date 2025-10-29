#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include <Libraries/PalmOSGlue/TxtGlue.h>
#include <Libraries/PalmOSGlue/FrmGlue.h>
#include "Resources.h"

/* macros */
#define EVER ;;
#define CRID 'PXSc'
#define DBTYPE 'DATA'
#define DBNAME "PXSc_PXSc Players"
#define DBVERSION 1
#define APPVER 1

enum { PrfApplication=0, PrfGame };

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
	Boolean navSupported;
	IndexedColorType backColor;
	IndexedColorType foreColor;
} UI;

typedef struct {
	FormType *oldForm;
	UInt16 oldId;
	FormActiveStateType state;
} FormSave;

typedef struct {
	Char lastMemoTitle[MemoLength+2];
	UInt16 catMemoIdx;
} Prefs;

typedef struct {
	UInt32 gameStarted;
	UInt32 gameEnded;
	UInt8 noOfPlayers;
	UInt8 phase[MaxPlayers];
	UInt8 oldPhase[MaxPlayers];
	UInt8 scoreDone[MaxPlayers];
	Char names[MaxPlayers][PlayerNameLength+1];
	UInt16 scores[MaxPlayers];
} PXScGame;

/* Sections */
#ifdef _FRM
#undef _FRM
#endif
#define _FRM(x) x __attribute__ ((section("frm")))

#ifdef _UI
#undef _UI
#endif
#define _UI(x) x __attribute__ ((section("ui")))

/* DB.c */
void DBCreateDatabase(void);
void DBInit(DB *, Char *);
Boolean DBOpen(DB *, UInt16, Boolean);
void DBClose(DB *);
void DBAddName(UInt16, Char *);
MemHandle DBGetName(UInt16);

/* String.c */
Char *StringGet(UInt16);
void StringFree(void);

/* UI.c */
_UI(UI *UIInit(void));
_UI(void *UIObjectGet(UInt16));
_UI(void UIObjectShow(UInt16));
_UI(void UIObjectHide(UInt16));
_UI(void UIFormPrologue(FormSave *, UInt16, FormEventHandlerType *));
_UI(Boolean UIFormEpilogue(FormSave *, void (*)(), UInt16));
_UI(void UIFormRedraw(void));
_UI(void UIFieldFocus(UInt16));
_UI(Boolean UIFieldRefocus(UInt16 *, WChar));
_UI(Char *UIFieldGetText(UInt16));
_UI(void UIFieldSetText(UInt16, Char *));
_UI(void UIFieldClear(UInt16));
_UI(void UIFieldSelectAll(UInt16, Char *, Boolean));
_UI(Boolean UIFieldScrollBarHandler(EventType *, UInt16, UInt16));
_UI(Boolean UIFieldScrollBarKeyHandler(EventType *, UInt16, UInt16));
_UI(void UIFieldUpdateScrollBar(UInt16, UInt16));
_UI(void UIFieldScrollLines(UInt16, Int16));
_UI(void UIFieldPageScroll(UInt16, UInt16, WinDirectionType));
_UI(void UIPopupSet(DmOpenRef , UInt16, Char *, UInt16));
_UI(void UIListInit(UInt16, void (*)(Int16, RectangleType *, Char **), Int16, Int16));
_UI(Boolean UIListKeyHandler(EventType *, UInt16, UInt16));
_UI(void UIListUpdateFocus(UInt16, Int16));

/* fAbout.c */
_FRM(void About(void));

/* fExport.c */
_FRM(void fExportRun(void));

/* fKeyboard.c */
_FRM(void fKeyboardRun(UInt16 *));

/* fMain.c */
_FRM(Boolean fMainInit(void));
_FRM(Boolean fMainEH(EventType *));

/* fInGame.c */
_FRM(void fInGameRun(void));
_FRM(Boolean fInGameInit(void));
_FRM(Boolean fInGameEH(EventType *));
_FRM(UInt16 *GetIndices(void));
_FRM(void GameSamePlayers(void));

/* fPlayers.c */
_FRM(UInt16 fPlayersRun(Char *, Boolean));

/* fWinner.c */
_FRM(Boolean fWinnerInit(void));
_FRM(Boolean fWinnerEH(EventType *));

/* Debug.c */
#if DEBUG==1
#define D(x)		x
#include <HostControl.h>
void DAlert(UInt8 *, ...);
#ifdef MAIN
	HostFILE *gHostFile;
#else
	extern HostFILE *gHostFile;
#endif
#define DPrint(x...)	HostFPrintF(gHostFile, x); \
			HostFFlush(gHostFile);
#else
#define DAlert(x, ...)	;
#define D(x)		;
#define DPrint(x...)	;
#endif

#ifdef MAIN
	UInt16 currentFormID, valid;
	DB dbData;
	FormPtr currentForm;
	PXScGame game;
	Prefs prefs;
	UI *ui;
#else
	extern UInt16 currentFormID, valid;
	extern DB dbData;
	extern FormPtr currentForm;
	extern PXScGame game;
	extern Prefs prefs;
	extern UI *ui;
#endif

#endif

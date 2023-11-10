#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include <Libraries/PalmOSGlue/TxtGlue.h>
#include "Resources.h"

/* macros */
#define EVER ;;
#define CRID 'kScc'
#define DBNAME "Kasino Data"
#define DBTYPE 'DATA'
#define DBVERSION 1
#define APPVER 1

/* structs */
typedef struct {
	UInt32 noOfRounds;
	UInt32 noOfPoints;
	UInt16 rulesChapter;
} KasinoPref;

typedef struct {
	UInt16 noOfPlayers;
	UInt32 noOfRounds;
	UInt32 noOfPoints;
	UInt32 currentRound;
	Char playerName[MaxPlayers][MaxNameLength+1];
	UInt32 playerScore[MaxPlayers];
} KasinoGame;

typedef struct {
	AppInfoType appInfo;
	KasinoGame game;
} KasinoAppInfo;

/* DB.c */
Boolean DBOpen(void);
void DBClose(void);
void DBSetGame(void);
void DBGetGame(void);
void DBAddName(Char *);

/* UI.c */
void *UIFormGetObject(UInt16);
Char *UIFieldGetText(UInt16);
void UIFieldSetText(UInt16, Char *);
Boolean UIFieldReFocus(UInt16 *, WChar);
Boolean UIFieldScrollbarHandler(EventPtr, UInt16, UInt16);
Boolean UIFieldScrollbarKeyHandler(EventPtr, UInt16, UInt16);
void UIFieldUpdateScrollbar(UInt16, UInt16);
void UIFieldScrollLines(UInt16, Int16);
void UIFieldScrollPage(UInt16, UInt16, WinDirectionType);
Boolean UIIntegerDialog(UInt16, UInt16, UInt16, UInt32 *);
void UIObjectHide(UInt16);
void UIObjectHideX(UInt16 *);
void UIObjectShow(UInt16);
void UIObjectShowX(UInt16 *);

/* fMain.c */
Boolean fMainInit(void);
Boolean fMainEH(EventPtr);

/* fNames.c */
Boolean fNamesRun(Char *);

/* fNewGame.c */
Boolean fNewGameInit(void);
Boolean fNewGameEH(EventPtr);

/* fRules.c */
void RulesAllocate(void);
void RulesDeallocate(void);
void fRulesRun(void);

/* fScores.c */
Boolean fScoresInit(void);
Boolean fScoresEH(EventPtr);

/* fWinner.c */
Boolean FindWinners(void);

/* Debug.c */
#ifdef DEBUG
void DAlert(UInt8 *, ...);
#endif

#ifdef MAIN
	UInt16 currentFormID, dbCard;
	LocalID dbLocalID;
	DmOpenRef dbH;
	FormPtr currentForm;
	KasinoPref pref;
	KasinoGame game;
#else
	extern UInt16 currentFormID, dbCard;
	extern LocalID dbLocalID;
	extern DmOpenRef dbH;
	extern FormPtr currentForm;
	extern KasinoPref pref;
	extern KasinoGame game;
#endif

#endif

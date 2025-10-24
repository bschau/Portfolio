#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include "Resources.h"

/* macros */
#define EVER ;;
#define PREFSVER 1
#define MaxRounds 10

enum { PrfApplication=0, PrfBestScorers, PrfSession, PrfEasyLevel, PrfHardLevel };
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
	UInt16 flags;
	UInt16 memoCategory;
	Char lastName[BestScorerNameLen+1];
} Prefs;
#define PFlgLevel 1	/* 0 = easy, 1 = hard */
#define PFlgType 2	/* 0 = base, 1 = exponent */

typedef struct {
	UInt16 round;
	UInt16 score;
	UInt8 quiz;
	UInt8 quizIdx;
} Session;

typedef struct {
	UInt32 time;
	UInt16 score;
	Char name[BestScorerNameLen+1];
} BestScorerEntry;

/* DB.c */
UInt16 DBFindDatabase(DB *);
void DBInit(DB *, Char *);
Boolean DBOpen(DB *, UInt16, Boolean);
void DBClose(DB *);
void DBClear(DB *);
UInt32 DBSetRecord(DB *, UInt16, UInt16, void *, UInt16, DmComparF);

/* PilotMain.c */
#ifdef DEBUG
void HexDump(UInt8 *, UInt16);
#else
#define HexDump(x, y)	;
#endif
Boolean PMGetPref(void *, UInt16, UInt16, Boolean);
void PMSetPref(void *, UInt16, UInt16, Boolean);

/* Session.c */
void SssInit(void);
void SssFree(void);
void SssGet(void);
void SssNew(void);
void SssUpdate(UInt16);
void SssSet(void);
void SssDestroy(void);
Char *SssQuizToAscii(void);

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

/* fAbout.c */
void fAboutRun(void);

/* fEnd.c */
Boolean fEndInit(void);
Boolean fEndEH(EventType *);

/* fMain.c */
Boolean fMainInit(void);
Boolean fMainEH(EventType *);

/* fGame.c */
Boolean fGameInit(void);
Boolean fGameEH(EventType *);

/* fPreferences.c */
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
	UInt16 currentFormID;
	FormType *currentForm;
	Boolean appStopped, inGame;
	Prefs prefs;
	Session *session;
	Int16 deleteStringDelay;
	UI *ui;
	BestScorerEntry *bestScorers;
#ifdef DEBUG
	HostFILE *gHostFile;
#endif
#else
	extern UInt16 currentFormID;
	extern FormType *currentForm;
	extern Boolean appStopped, inGame;
	extern Prefs prefs;
	extern Session *session;
	extern Int16 deleteStringDelay;
	extern UI *ui;
	extern BestScorerEntry *bestScorers;
#ifdef DEBUG
	extern HostFILE *gHostFile;
#endif
#endif

#endif

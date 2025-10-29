#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include "Resources.h"

/* macros */
#define EVER ;;
#define APPVER 1
#define DBVERSION 1
#define PrfQuizTypeLcm 0x01
#define PrfQuizTypeGcf 0x02
#define PrfQuizTypeRandom 0x03
#define PrfNumbers 0x04		/* 3 if set, 2 if unset */
#define PrfScoreType 0x08	/* First guess if unset, Correct guess if set */

enum { PrfApplication=0, PrfSession, PrfLastName, PrfRecords };
enum { FDFound, FDNotFound, FDError };

typedef struct {
	UInt16 operation;
	UInt16 maxOperand;
	UInt16 timedMins;
	UInt16 memoCategory;
} Prefs;

typedef struct {
	UInt32 started;
	UInt32 stopped;
	UInt32 round;
	UInt32 score;
	UInt16 defOperations;
	UInt16 operation;
	UInt32 timedSecs;
	UInt16 max;
	UInt32 clues;
	Boolean failed;
	UInt16 nums[3];
	UInt16 prevNums[3];
} Session;

typedef struct {
	UInt32 started;
	UInt32 stopped;
	UInt32 round;
	UInt32 score;
	UInt16 operation;
	UInt16 timedMins;
	UInt16 max;
	UInt32 clues;
	Char name[NameLength+2];
} Record;
#define MaxRecords 10

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
	UInt16 id;
	Char *button;
} Gadget;

/* AboutForm.c */
void About(void);

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
void IRBeam(void);
Boolean IRReceive(ExgSocketType *, Prefs *);

/* PilotMain.c */
Boolean PMGetPref(void *, UInt16, UInt16);
void PMSetPref(void *, UInt16, UInt16);

/* Session.c */
void SssGet(void);
void SssNew(void);
void SssUpdate(UInt16);
void SssSet(void);
void SssDestroy(void);

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
Boolean UIFieldScrollBarKeyHandler(EventType *, UInt16, UInt16);
void UIFieldUpdateScrollBar(UInt16, UInt16);
void UIFieldScrollLines(UInt16, Int16);
void UIFieldPageScroll(UInt16, UInt16, WinDirectionType);
UInt16 UIGadgetFontHandler(Gadget *, FontID, UInt16, UInt16);

/* fClue.c */
void fClueClose(void);
void fClueRun(void);

/* fGame.c */
Boolean fGameInit(void);
Boolean fGameEH(EventType *);
void fGameEndQuiz(void);

/* fMain.c */
Boolean fMainInit(void);
Boolean fMainEH(EventType *);

/* fPreferences.c */
void fPreferencesRun(void);

/* fQuizOver.c */
Boolean fQuizOverInit(void);
Boolean fQuizOverEH(EventType *);

/* fScores.c */
Boolean fScoresInit(void);
Boolean fScoresEH(EventType *);

/* Debug.c */
#if DEBUG==1
#define D(x)		x
#include <HostControl.h>
void DAlert(UInt8 *, ...);
#define DPrint(x...)	HostFPrintF(gHostFile, x); \
			HostFFlush(gHostFile);
#else
#define DAlert(x, ...)	;
#define D(x)		;
#define DPrint(x...)	;
#endif

#ifdef MAIN
	UInt16 currentFormID;
	FormPtr currentForm;
	Session *session;
	Prefs prefs;
	Boolean prefsChanged, inGame, prefsRunning, appStopped;
	Int16 deleteStringDelay;
	Record *record;
#ifdef DEBUG
	HostFILE *gHostFile;
#endif
#else
	extern UInt16 currentFormID;
	extern FormPtr currentForm;
	extern Session *session;
	extern Prefs prefs;
	extern Boolean prefsChanged, inGame, prefsRunning, appStopped;
	extern Int16 deleteStringDelay;
	extern Record *record;
#ifdef DEBUG
	extern HostFILE *gHostFile;
#endif
#endif

#endif

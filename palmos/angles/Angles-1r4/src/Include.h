#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include <DLServer.h>
#include "Resources.h"

/* macros */
#define EVER ;;
#define Radius 50
#define CRID 'GNWJ'
#define APPVER 1
#define RANDOM(x) ((((UInt32)SysRandom(0))*x)/(((UInt32)sysRandomMax)+1))

enum { PrfApplication=0, PrfLowScores, PrfSession, PrfLastName };

typedef struct {
	UInt16 flags;
} Preferences;
#define PrfIncludeReflex 1

typedef struct {
	UInt16 scores[5];
	Char names[5][dlkUserNameBufSize];
} LowScoreTable;

typedef struct {
	UInt16 round;
	UInt16 score;
	UInt16 angle;
	UInt16 start;
} Session;

typedef struct {
	FormType *oldForm;
	UInt16 oldId;
	FormActiveStateType state;
} FormSave;

/* AboutForm.c */
void About(void);

/* LoScores.c */
void LowLoadTable(void);
void LowSaveTable(void);
void LowGetPair(UInt16, Char **, UInt16 *, UInt16 *);
void LowAddPair(UInt16, Char *);
UInt16 LowQualify(UInt16);
void LowReset(void);

/* Session.c */
void SssGet(void);
void SssNew(void);
void SssUpdate(UInt16);
void SssSet(void);
void SssDestroy(void);

/* UI.c */
void *UIFormGetObject(UInt16);
void UIFormPrologue(FormSave *, UInt16, FormEventHandlerType *);
Boolean UIFormEpilogue(FormSave *, void (*)(), UInt16);
void UIFieldFocus(UInt16);
Char *UIFieldGetText(UInt16);
void UIFieldSetText(UInt16, Char *);
void UIFieldClear(UInt16);
Boolean UIFieldScrollBarHandler(EventType *, UInt16, UInt16);
Boolean UIFieldScrollBarKeyHandler(EventType *, UInt16, UInt16);
void UIFieldUpdateScrollBar(UInt16, UInt16);
void UIFieldScrollLines(UInt16, Int16);
void UIFieldPageScroll(UInt16, UInt16, WinDirectionType);
void UIShowObject(UInt16);

/* Utils.c */
Boolean UtilsRomCheck(void);
Boolean UtilsGetPref(void *, UInt16, UInt16);
void UtilsSetPref(void *, UInt16, UInt16);

/* fEnd.c */
Boolean fEndInit(void);
Boolean fEndEH(EventType *);

/* fGame.c */
Boolean fGameInit(void);
Boolean fGameEH(EventType *);

/* fMain.c */
Boolean fMainEH(EventType *);

/* Debug.c */
#if DEBUG==1
#define D(x) x
void DAlert(UInt8 *, ...);
#else
#define D(x) ;
#endif

#ifdef MAIN
	UInt16 currentFormID;
	FormPtr currentForm;
	Session *session;
	Preferences prefs;
#else
	extern UInt16 currentFormID;
	extern FormPtr currentForm;
	extern Session *session;
	extern Preferences prefs;
#endif

/* all */
extern float CosTab[360], SinTab[360];

#endif

#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include "Resources.h"

/* macros */
#define EVER ;;
#define PREFSVER 2
#define CRID 'Rcal'
#define HardKeys (keyBitPageUp|keyBitPageDown)

enum { PrfApplication=0, PrfDigitizer };

typedef struct {
	FormType *oldForm;
	UInt16 oldId;
	FormActiveStateType state;
} FormSave;

typedef struct {
	UInt16 flags;
	UInt16 delay;
	UInt16 autoDigi;
	UInt16 timeout;
	UInt16 precision;
} Prefs;
#define PFlgHotSync 1
#define PFlgLaunching 2
#define PFlgWakeup 4
#define PFlgQuitting 8

typedef struct {
	PointType aTL;
	PointType aBR;
	PointType eTL;
	PointType eBR;
} DigitizerPrefs;

/* Notifications.c */
Boolean NotificationsExists(void);
void NotificationsSetup(void);

/* PilotMain.c */
Boolean PMGetPref(void *, UInt16, UInt16);
void PMSetPref(void *, UInt16, UInt16);
Boolean PMOS5Device(void);
void PMSetupAutoDigi(void);
Boolean PMNavSupported(void);
UInt16 PMPrecision(UInt16);

/* UI.c */
void *UIObjectGet(UInt16);
void UIObjectShow(UInt16);
void UIFormPrologue(FormSave *, UInt16, FormEventHandlerType *);
Boolean UIFormEpilogue(FormSave *, void (*)(), UInt16);
void UIFieldFocus(UInt16);
Char *UIFieldGetText(UInt16);
void UIFieldSetText(UInt16, Char *);
Boolean UIFieldScrollBarHandler(EventType *, UInt16, UInt16);
Boolean UIFieldScrollBarKeyHandler(EventType *, UInt16, UInt16);
void UIFieldUpdateScrollBar(UInt16, UInt16);
void UIFieldScrollLines(UInt16, Int16);
void UIFieldPageScroll(UInt16, UInt16, WinDirectionType);

/* fAbout.c */
void fAboutRun(void);

/* fDigitizer.c */
void fDigitizerRun(Boolean, UInt16, UInt16);

/* fMain.c */
Boolean fMainInit(void);
Boolean fMainEH(EventType *);
void fMainSavePrefs(void);

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
	Boolean appStopped;
	Prefs prefs;
	UInt32 oldKeyMask;
#else
	extern UInt16 currentFormID;
	extern FormPtr currentForm;
	extern Boolean appStopped;
	extern Prefs prefs;
	extern UInt32 oldKeyMask;
#endif

#endif

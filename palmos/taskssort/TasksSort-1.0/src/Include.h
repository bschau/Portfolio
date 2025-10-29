#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include <FrmGlue.h>
#include "Resources.h"

/* macros */
#define EVER ;;
#define PREFSVER 1
#define EndianSwap16(n) (((((UInt16)n)<<8)&0xff00)|((((UInt16)n)>>8)&0x00ff))
#define CRID 'SoRt'

enum { PrfApplication=0, PrfState };
enum { FtrSwitcher=0 };

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
	UInt32 flags;
	WChar hotKey;
} Prefs;
#define PFlgLaunchTasks 1
#define PFlgHotKey 2

typedef struct {
	Int16 sortOrder;
} State;
#define SSOAscending 1
#define SSODescending 2

/* Sections */
#ifdef _FRM
#undef _FRM
#endif
#define _FRM(x) x __attribute__ ((section("frm")))

#ifdef _UI
#undef _UI
#endif
#define _UI(x) x __attribute__ ((section("ui")))

/* PilotMain.c */
/* Do not put these in their own segment! */	
Boolean PMGetPref(void *, UInt16, UInt16, Boolean);
void PMSetPref(void *, UInt16, UInt16, Boolean);
Boolean PMHasNotifications(void);
void PMRegisterAppEvents(void);
void PMUnregisterAppEvents(void);

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
/*
_UI(void UIPopupSet(DmOpenRef , UInt16, Char *, UInt16));
_UI(UInt16 UIGadgetFontHandler(Gadget *, FontID, UInt16, UInt16));
_UI(void UIListInit(UInt16, void (*)(Int16, RectangleType *, Char **), Int16, Int16));
_UI(Boolean UIListKeyHandler(EventType *, UInt16, UInt16));
_UI(void UIListUpdateFocus(UInt16, Int16));
*/

/* fAbout.c */
_FRM(void fAboutRun(void));

/* fMain.c */
_FRM(Boolean fMainInit(void));
_FRM(Boolean fMainEH(EventType *));

/* Debug.c */
#ifdef DEBUG
#define D(x)		x
#include <HostControl.h>
void DAlert(UInt8 *, ...);
void DHexDump(UInt8 *, UInt16 srcLen);
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
#define DHexDump(x, y)	;
#endif

#ifdef MAIN
	FormType *currentForm;
	UInt16 currentFormID;
	Boolean appStopped;
	Prefs prefs;
	State state;
	UI *ui;
#else
	extern FormType *currentForm;
	extern UInt16 currentFormID;
	extern Boolean appStopped;
	extern Prefs prefs;
	extern State state;
	extern UI *ui;
#endif

#endif

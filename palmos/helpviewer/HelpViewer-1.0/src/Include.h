#ifndef INCLUDE_H
#define INCLUDE_H

#include <PalmOS.h>
#include <Libraries/PalmOSGlue/TxtGlue.h>
#include <Libraries/PalmOSGlue/FrmGlue.h>
#include "Resources.h"

/* macros */
#define EVER ;;
#define CRID 'PLUV'
#define APPVER 1

enum { PrfState=0 };

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
	Int16 helpFile;
} State;

/* Sections */
#ifdef _FRM
#undef _FRM
#endif
#define _FRM(x) x __attribute__ ((section("frm")))

#ifdef _UI
#undef _UI
#endif
#define _UI(x) x __attribute__ ((section("ui")))

/* String.c */
/*
Char *StringGet(UInt16);
void StringFree(void);
*/

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
_FRM(void fAboutRun(void));

/* fMain.c */
_FRM(void fMainOpen(UInt16));
_FRM(void fMainClose(void));
_FRM(Boolean fMainInit(void));
_FRM(Boolean fMainEH(EventType *));

/* fTopic.c */
_FRM(void fTopicRun(Char *, Char *));
_FRM(Boolean fTopicInit(void));
_FRM(Boolean fTopicEH(EventType *));

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
	Char *ph1Name="PluaHelp", *ph2Name="Plua2Help";
	UInt16 currentFormID;
	FormType *currentForm;
	UInt16 ph1Card, ph2Card;
	LocalID ph1ID, ph2ID;
	UI *ui;
	State state;
#else
	extern Char *ph1Name, *ph2Name;
	extern UInt16 currentFormID;
	extern FormType *currentForm;
	extern UInt16 ph1Card, ph2Card;
	extern LocalID ph1ID, ph2ID;
	extern UI *ui;
	extern State state;
#endif

#endif

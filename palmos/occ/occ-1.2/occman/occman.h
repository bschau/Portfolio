#ifndef OCCMAN_H
#define OCCMAN_H

#include <PalmOS.h>
#include <unix_stdarg.h>
#include "resources.h"
#include "occinclude.h"

/* macros */
#define EVER ;;
#define CRID 'OccX'
#define NEWOCC -1
#define APPVER 1

/* structs */
struct OCCNode {
	struct OCCNode *next;
	struct OCCNode *prev;
	MemHandle mh;
	Char name[dmDBNameLength];
};
typedef struct OCCNode OCCNode;

typedef struct {
	OCCNode *head;
	OCCNode *tail;
	OCCNode *tailPred;
	UInt16 count;
} OCCList;

enum { OCCTypePiggyBack=0, OCCTypeStandalone };

/* editform.c */
Boolean EditOCC(Int16);

/* mainform.c */
Boolean MainFormInit(void);
Boolean MainFormEventHandler(EventPtr ev);

/* occ.c */
void OCCListInit(OCCList *);
void OCCListDestroy(OCCList *);
void OCCDiscover(UInt32, UInt16, LocalID, OCCList *);
void OCCDiscoverAll(OCCList *);
Boolean OCCGetByName(Char *, UInt16 *, LocalID *);
MemHandle OCCGetUrl(UInt16, LocalID);
Boolean OCCIsStandalone(UInt16, LocalID, UInt16 *);
Boolean OCCDelete(OCCList *, Int16);
void OCCLaunch(UInt16, LocalID);

/* prc.c */
Boolean PrcBuild(Char *, Char *, UInt16);

/* ui.c */
void *UIGetFormObject(UInt16);
Char *UIGetFieldText(UInt16);
void UISetFieldText(UInt16, Char *);
Boolean UIReFocus(UInt16 *, WChar);
Boolean UIScrollbarHandler(EventPtr, UInt16, UInt16);
Boolean UIScrollbarKeyHandler(EventPtr, UInt16, UInt16);
void UIUpdateScrollbar(UInt16, UInt16);
void UIScrollLines(UInt16, Int16);
void UIScrollPage(UInt16, UInt16, WinDirectionType);

/* viewform.c */
Boolean ViewOCC(Int16);

#if DEBUG==1
#define DEBUGCOMMAND(x) x
void DAlert(UInt8 *, ...);
#else
#define DEBUGCOMMAND(x) ;
#endif

#ifdef MAIN
	FormPtr currentForm;
	UInt16 currentFormID;
	OCCList occList;
#else
	extern FormPtr currentForm;
	extern UInt16 currentFormID;
	extern OCCList occList;
#endif

#endif

/*
 * fRules.c
 */
#include "Include.h"

/* protos */
static void SelectChapter(void);
static Boolean RulesEH(EventPtr);

/* globals */
static struct {
	UInt16 id;
	MemHandle mh;
	Char *text;
} rules[]={
	{ sRules1, NULL, NULL },
	{ sRules2, NULL, NULL },
	{ sRules3, NULL, NULL },
	{ sRules4, NULL, NULL },
	{ sRules5, NULL, NULL },
	{ sRules6, NULL, NULL },
	{ sRules7, NULL, NULL },
	{ sRules8, NULL, NULL },
	{ sRules9, NULL, NULL },
	{ sRules10, NULL, NULL },
	{ sRules11, NULL, NULL },
	{ sRules12, NULL, NULL },
	{ 0, NULL, NULL },
};

/*
 * RulesAllocate
 *
 * Initialize rules stuff.
 */
void
RulesAllocate(void)
{
	MemHandle mh;
	UInt16 idx;

	for (idx=0; rules[idx].id; idx++) {
		mh=DmGetResource('tSTR', rules[idx].id);
		ErrFatalDisplayIf(mh==NULL, "(RulesAllocate) Cannot find resource.");

		rules[idx].mh=mh;
		rules[idx].text=MemHandleLock(mh);
	}
}

/*
 * RulesDeallocate
 *
 * Deallocate rules stuff.
 */
void
RulesDeallocate(void)
{
	MemHandle mh;
	UInt16 idx;

	for (idx=0; rules[idx].id; idx++) {
		if ((mh=rules[idx].mh)!=NULL) {
			MemHandleUnlock(mh);
			DmReleaseResource(mh);
		}
	}
}

/*
 * SelectChapter
 *
 * Select a new chapter - update field/scrollbars.
 */
static void
SelectChapter(void)
{
	CtlSetLabel(UIFormGetObject(fRulesPopup), LstGetSelectionText(UIFormGetObject(fRulesList), pref.rulesChapter));
	UIFieldSetText(fRulesField, rules[pref.rulesChapter].text);
	UIFieldUpdateScrollbar(fRulesScrollbar, fRulesField);
}

/*
 * fRulesRun
 *
 * Show the Rules form.
 */
void
fRulesRun(void)
{
	FormPtr rFrm=FrmInitForm(fRules), oldForm=currentForm;
	RectangleType box;
	ListType *lst;

	ErrFatalDisplayIf(rFrm==NULL, "(RulesForm) Cannot initialize Rules form.");
	currentForm=rFrm;
	FrmSetActiveForm(currentForm);
	FrmSetEventHandler(currentForm, (FormEventHandlerPtr)RulesEH);
	FrmDrawForm(currentForm);
	box.topLeft.x=3;
	box.topLeft.y=32;
	box.extent.x=138;
	box.extent.y=102;
	WinDrawRectangleFrame(simpleFrame, &box);
	lst=UIFormGetObject(fRulesList);
	LstSetSelection(lst, pref.rulesChapter);
	LstMakeItemVisible(lst, pref.rulesChapter);
	SelectChapter();

	FrmDoDialog(currentForm);

	FrmEraseForm(rFrm);
	FrmDeleteForm(rFrm);
	currentForm=oldForm;
	FrmSetActiveForm(currentForm);
}

/*
 * RulesEH
 */
static Boolean
RulesEH(EventPtr ev)
{

	switch (ev->eType) {
	case popSelectEvent:
		if (ev->data.popSelect.controlID==fRulesPopup) {
			pref.rulesChapter=ev->data.popSelect.selection;
			SelectChapter();
			return true;
		}
		break;
	default:
		if (UIFieldScrollbarKeyHandler(ev, fRulesScrollbar, fRulesField)==true)
			return true;

		UIFieldScrollbarHandler(ev, fRulesScrollbar, fRulesField);
		/* do not return true here - it disables sclRepeat */
		break;
	}

	return false;
}

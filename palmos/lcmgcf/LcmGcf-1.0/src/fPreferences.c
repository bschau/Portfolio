/*
 * Preferences.c
 */
#include "Include.h"

/* protos */
static Boolean EventHandler(EventType *);

/*
 * Preferences
 *
 * Show and handle the preferences dialog.
 */
void
fPreferencesRun(void)
{
	FormSave frm;
	Char buffer[16];
	UInt16 v;
	ListType *l;

	prefsRunning=true;
	UIFormPrologue(&frm, fPreferences, EventHandler);

	l=UIObjectGet(cPreferencesTypeList);
	v=(prefs.operation&PrfQuizTypeRandom)-1;
	LstSetSelection(l, v);
	CtlSetLabel(UIObjectGet(cPreferencesTypePopup), LstGetSelectionText(l, v));

	l=UIObjectGet(cPreferencesNumbersList);
	v=(prefs.operation&PrfNumbers) ? 1 : 0;
	LstSetSelection(l, v);
	CtlSetLabel(UIObjectGet(cPreferencesNumbersPopup), LstGetSelectionText(l, v));

	l=UIObjectGet(cPreferencesOperandList);
	LstSetSelection(l, prefs.maxOperand);
	CtlSetLabel(UIObjectGet(cPreferencesOperandPopup), LstGetSelectionText(l, prefs.maxOperand));

	l=UIObjectGet(cPreferencesScoreList);
	v=(prefs.operation&PrfScoreType) ? 1 : 0;
	LstSetSelection(l, v);
	CtlSetLabel(UIObjectGet(cPreferencesScorePopup), LstGetSelectionText(l, v));

	if (prefs.timedMins>0) {
		StrPrintF(buffer, "%u", prefs.timedMins);
		UIFieldSetText(cPreferencesTimedMins, buffer);
	}

	UIFieldFocus(cPreferencesTimedMins);

	UIFormEpilogue(&frm, NULL, cPreferencesCancel);
	prefsRunning=false;
}

/*
 * EventHandler
 */
static Boolean
EventHandler(EventType *ev)
{
	Int16 max;
	Char *p;

	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cPreferencesOK:
			if ((max=LstGetSelection(UIObjectGet(cPreferencesTypeList)))!=noListSelection)
				prefs.operation=max+1;
			else
				prefs.operation=PrfQuizTypeRandom;

			if (LstGetSelection(UIObjectGet(cPreferencesNumbersList)))
				prefs.operation|=PrfNumbers;

			if ((max=LstGetSelection(UIObjectGet(cPreferencesOperandList)))!=noListSelection)
				prefs.maxOperand=max;
			else
				prefs.maxOperand=0;
				
			if (LstGetSelection(UIObjectGet(cPreferencesScoreList)))
				prefs.operation|=PrfScoreType;

			if ((p=UIFieldGetText(cPreferencesTimedMins))!=NULL)
				prefs.timedMins=StrAToI(p);

		default:	/* FALL-THRU */
			break;
		}

		break;

	default:	/* FALL-THRU */
		break;	
	}

	return false;
}

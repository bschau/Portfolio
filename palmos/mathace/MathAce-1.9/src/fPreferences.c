/*
 * Preferences.c
 */
#include "Include.h"

/* protos */
static Boolean EventHandler(EventType *);
static void SetupMax(void);

/* globals */
static UInt16 focusBand[]={ cPreferencesTimedMins, cPreferencesMax, 0 };

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

	l=UIFormGetObject(cPreferencesTypeList);
	v=(prefs.operation&PrfQuizType) ? 1 : 0;
	LstSetSelection(l, v);
	CtlSetLabel(UIFormGetObject(cPreferencesTypePopup), LstGetSelectionText(l, v));

	l=UIFormGetObject(cPreferencesSolveList);
	v=(prefs.operation&PrfSolveType) ? 1 : 0;
	LstSetSelection(l, v);
	CtlSetLabel(UIFormGetObject(cPreferencesSolvePopup), LstGetSelectionText(l, v));

	l=UIFormGetObject(cPreferencesMaxList);
	v=(prefs.operation&PrfMaxType) ? 1 : 0;
	LstSetSelection(l, v);
	CtlSetLabel(UIFormGetObject(cPreferencesMaxPopup), LstGetSelectionText(l, v));

	l=UIFormGetObject(cPreferencesScoreList);
	v=(prefs.operation&PrfScoreType) ? 1 : 0;
	LstSetSelection(l, v);
	CtlSetLabel(UIFormGetObject(cPreferencesScorePopup), LstGetSelectionText(l, v));

	SetupMax();

	StrPrintF(buffer, "%u", prefs.minOperand);
	UIFieldSetText(cPreferencesMin, buffer);

	CtlSetValue(UIFormGetObject(cPreferencesAdd), (prefs.operation&PrfOperationAdd) ? 1 : 0);
	CtlSetValue(UIFormGetObject(cPreferencesSub), (prefs.operation&PrfOperationSub) ? 1 : 0);
	UIFieldFocus(cPreferencesTimedMins);

	CtlSetValue(UIFormGetObject(cPreferencesSigned), (prefs.operation&PrfOperationSigned) ? 1 : 0);

	if (prefs.timedMins>0) {
		StrPrintF(buffer, "%u", prefs.timedMins);
		UIFieldSetText(cPreferencesTimedMins, buffer);
	}

	UIFormEpilogue(&frm, NULL, cPreferencesCancel);
	prefsRunning=false;
}

/*
 * EventHandler
 */
static Boolean
EventHandler(EventType *ev)
{
	UInt16 max;
	Char *p;

	switch (ev->eType) {
	case popSelectEvent:
		if (ev->data.popSelect.controlID==cPreferencesMaxPopup) {
			if (ev->data.popSelect.selection!=ev->data.popSelect.priorSelection)
				SetupMax();
		}
		break;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cPreferencesOK:
			if ((p=UIFieldGetText(cPreferencesMax))!=NULL) {
				if ((max=(UInt16)StrAToI(p))>200)
					max=200;
			} else
				max=0;

			if (!max) {
				FrmAlert(aNotNull);
				UIFieldFocus(cPreferencesMax);
				return true;
			}

			if ((p=UIFieldGetText(cPreferencesMin))!=NULL) {
				if ((prefs.minOperand=(UInt16)StrAToI(p))>200)
					prefs.minOperand=200;
			} else
				prefs.minOperand=0;

			prefs.operation=0;

			if (LstGetSelection(UIFormGetObject(cPreferencesMaxList))) {
				prefs.operation|=PrfMaxType;
				prefs.maxOperand=max;
			} else
				prefs.maxAnswer=max;

			if (LstGetSelection(UIFormGetObject(cPreferencesTypeList)))
				prefs.operation|=PrfQuizType;

			if (LstGetSelection(UIFormGetObject(cPreferencesSolveList)))
				prefs.operation|=PrfSolveType;

			if (LstGetSelection(UIFormGetObject(cPreferencesScoreList)))
				prefs.operation|=PrfScoreType;

			if (CtlGetValue(UIFormGetObject(cPreferencesAdd)))
				prefs.operation|=PrfOperationAdd;

			if (CtlGetValue(UIFormGetObject(cPreferencesSub)))
				prefs.operation|=PrfOperationSub;

			if (CtlGetValue(UIFormGetObject(cPreferencesMul)))
				prefs.operation|=PrfOperationMul;

			if (CtlGetValue(UIFormGetObject(cPreferencesDiv)))
				prefs.operation|=PrfOperationDiv;

			if (CtlGetValue(UIFormGetObject(cPreferencesSigned)))
				prefs.operation|=PrfOperationSigned;

			/* ensure at least one operation */
			if (!(prefs.operation&(PrfOperationAdd|PrfOperationSub|PrfOperationMul|PrfOperationDiv))) {
				prefs.operation|=PrfOperationAdd;
				FrmAlert(aNoOperationSelected);
			}

			if ((p=UIFieldGetText(cPreferencesTimedMins))!=NULL)
				prefs.timedMins=StrAToI(p);

		default:	/* FALL-THRU */
			break;
		}

		break;

	case keyDownEvent:
		return UIFieldRefocus(focusBand, ev->data.keyDown.chr);

	default:	/* FALL-THRU */
		break;	
	}

	return false;
}

/*
 * SetupMax
 *
 * Setup the Max popup and Max fields.  Also check/uncheck checkboxes.
 */
static void
SetupMax(void)
{
	Char buffer[16];

	if (LstGetSelection(UIFormGetObject(cPreferencesMaxList))) {
		StrPrintF(buffer, "%u", prefs.maxOperand);

		CtlSetValue(UIFormGetObject(cPreferencesMul), (prefs.operation&PrfOperationMul) ? 1 : 0);
		UIShowObject(cPreferencesMul);
		CtlSetValue(UIFormGetObject(cPreferencesDiv), (prefs.operation&PrfOperationDiv) ? 1 : 0);
		UIShowObject(cPreferencesDiv);
	} else {
		StrPrintF(buffer, "%u", prefs.maxAnswer);

		CtlSetValue(UIFormGetObject(cPreferencesMul), 0);
		UIHideObject(cPreferencesMul);
		CtlSetValue(UIFormGetObject(cPreferencesDiv), 0);
		UIHideObject(cPreferencesDiv);
	}

	UIFieldSetText(cPreferencesMax, buffer);
}

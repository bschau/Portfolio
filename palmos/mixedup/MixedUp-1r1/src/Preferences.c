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
Preferences(void)
{
	UInt16 defLevel=level, v;
	ListType *l;
	FormSave frm;

	UIFormPrologue(&frm, fPreferences, EventHandler);

	l=UIFormGetObject(cPreferencesTypeList);
	v=(level&LvlMixed) ? 1 : 0;
	LstSetSelection(l, v);
	CtlSetLabel(UIFormGetObject(cPreferencesTypePopup), LstGetSelectionText(l, v));

	l=UIFormGetObject(cPreferencesDenomList);
	v=level&LvlDenomMask;
	LstSetSelection(l, v);
	CtlSetLabel(UIFormGetObject(cPreferencesDenomPopup), LstGetSelectionText(l, v));

	l=UIFormGetObject(cPreferencesLevelList);
	v=(level&LvlAdvanced) ? 1 : 0;
	LstSetSelection(l, v);
	CtlSetLabel(UIFormGetObject(cPreferencesLevelPopup), LstGetSelectionText(l, v));

	if (UIFormEpilogue(&frm, NULL, cPreferencesCancel)==false)
		level=defLevel;
}

/*
 * EventHandler
 */
static Boolean
EventHandler(EventType *ev)
{
	switch (ev->eType) {
	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==cPreferencesOK) {
			level=((UInt16)LstGetSelection(UIFormGetObject(cPreferencesDenomList)));
			level|=((UInt16)LstGetSelection(UIFormGetObject(cPreferencesLevelList)))<<LvlAdvancedShift;
			level|=((UInt16)LstGetSelection(UIFormGetObject(cPreferencesTypeList)))<<LvlMixedShift;
		}

	default:	/* FALL-THRU */
		break;	
	}

	return false;
}

/*
**	preference.c
**
**	Code to drive the Preference form.
*/
#include "bmilog.h"

/* protos */
static Boolean PreferencesEventHandler(EventType *);

/*
**	PreferencesGet
**
**	Get preferences block for application.
**
**	 -> p		Where to store preferences.
**
**	Returns true if preferences was found, false otherwise.
*/
Boolean
PreferencesGet(Preferences *p)
{
	UInt16 len;
	Int16 retPrf;
	
	len=sizeof(Preferences);
	retPrf=PrefGetAppPreferences((UInt32)CRID, 0, p, &len, true);
	if (retPrf==noPreferenceFound || len!=sizeof(Preferences))
		return false;

	return true;
}

/*
**	PreferencesNew
**
**	Show the Preferences form.
*/
void
PreferencesNew(void)
{
	FormSave frm;

	UIFormPrologue(&frm, fPreferences, PreferencesEventHandler);
	UIListSetPopup(fPreferencesTableList, fPreferencesTablePopup, prefs.bmiTable);
	UIListSetPopup(fPreferencesFormatList, fPreferencesFormatPopup, prefs.format);
	UIFormEpilogue(&frm, NULL, fPreferencesCancel);
}

/*
**	PreferencesEventHandler
*/
static Boolean
PreferencesEventHandler(EventType *ev)
{
	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case fPreferencesOK:
			prefs.bmiTable=LstGetSelection(UIFormGetObject(fPreferencesTableList));
			prefs.format=LstGetSelection(UIFormGetObject(fPreferencesFormatList));
			break;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

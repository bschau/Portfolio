/*
 * fPreferences.c
 */
#include "Include.h"

/*
 * fPreferencesEH
 */
Boolean
fPreferencesEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		return true;

	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==cPreferencesBack) {
			FrmGotoForm(fMain);
			return true;
		}

		prefs.flags=ev->data.ctlSelect.controlID-cPreferencesBEasy;
		SssNew();
		FrmGotoForm(fGame);
		return true;

	default:
		break;
	}

	return false;
}

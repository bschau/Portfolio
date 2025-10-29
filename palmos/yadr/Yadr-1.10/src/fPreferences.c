/*
 * fPreferences.c
 */
#include "Include.h"

/* protos */
static Boolean EH(EventType *);

/*
 * fPreferencesRun
 */
void
fPreferencesRun(void)
{
	FormSave frm;

	UIFormPrologue(&frm, fPreferences, EH);
	if (prefs.flags&PFHistory)
		CtlSetValue(UIObjectGet(cPreferencesHistoryTracking), 1);

	if (UIFormEpilogue(&frm, NULL, cPreferencesCancel)==false)
		return;

	HistoryClose();
	if (prefs.flags&PFHistory)
		HistoryOpen();
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	if ((ev->eType==ctlSelectEvent) && (ev->data.ctlSelect.controlID==cPreferencesOK)) {
		if (CtlGetValue(UIObjectGet(cPreferencesHistoryTracking)))
			prefs.flags|=PFHistory;
		else
			prefs.flags&=~PFHistory;
	}

	return false;
}

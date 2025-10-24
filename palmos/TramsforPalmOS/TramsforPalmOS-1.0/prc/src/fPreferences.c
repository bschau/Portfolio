/*
 * fPreferences.c
 */
#include "Include.h"

/*
 * fPreferencesInit
 */
Boolean
fPreferencesInit(void)
{
	if (*prefs.autoComment) {
		UIFieldSetText(cPreferencesComment, prefs.autoComment);
		if (prefs.flags&PFlgAutoComment) {
			CtlSetValue(UIObjectGet(cPreferencesCommentChk), 1);
			UIObjectFocus(cPreferencesCommentChk);
		}
	} else
		CtlSetValue(UIObjectGet(cPreferencesCommentChk), 0);

	return true;
}

/*
 * EH
 */
Boolean
fPreferencesEH(EventType *ev)
{
	Char *p;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cPreferencesCommentChk:
			if (CtlGetValue(UIObjectGet(cPreferencesCommentChk)))
				UIObjectFocus(cPreferencesComment);
			return true;

		case cPreferencesOK:
			MemSet(&prefs, sizeof(Prefs), 0);
			if (CtlGetValue(UIObjectGet(cPreferencesCommentChk)))
				prefs.flags=PFlgAutoComment;

			if ((p=UIFieldGetText(cPreferencesComment))) {
				UInt16 len=StrLen(p);

				MemMove(prefs.autoComment, p, len);
			}

		case cPreferencesCancel:
			FrmGotoForm(fMain);
			return true;
		}
		break;

	default:	/* FALL-THRU */
		break;
	}
	
	return false;
}

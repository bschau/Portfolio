/*
 * fPasswords.c
 */
#include "Include.h"

/* protos */

/* globals */
static UInt16 focusBand[]={ cPasswordsSit, cPasswordsISecur, 0 };

/*
 * fPasswordsEH
 */
Boolean
fPasswordsEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		UIFieldFocus(cPasswordsSit);
	case frmUpdateEvent:
		return true;

	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==cPasswordsOK) {
			Char *p1=UIFieldGetText(cPasswordsSit), *p2=UIFieldGetText(cPasswordsISecur);
			MemHandle mh;

			if (!p1 || StrLen(p1)==0 || !p2 || StrLen(p2)==0) {
				FrmAlert(aPwdNotBlank);
				return true;
			}

			fReoRun(p1, p2);

			if ((mh=FldGetTextHandle(UIObjectGet(cPasswordsSit)))) {
				MemSet(MemHandleLock(mh), MemHandleSize(mh), 0);
				MemHandleUnlock(mh);
			}

			if ((mh=FldGetTextHandle(UIObjectGet(cPasswordsISecur)))) {
				MemSet(MemHandleLock(mh), MemHandleSize(mh), 0);
				MemHandleUnlock(mh);
			}

			return true;
		}
		break;

	case keyDownEvent:
		UIFieldRefocus(focusBand, ev->data.keyDown.chr);

	default: /* FALL-THRU */
		break;
	}

	return false;
}

/*
 * fMain.c
 */
#include "Include.h"

/* protos */

/* globals */

/*
 * fMainInit
 */
Boolean
fMainInit(void)
{
	MemHandle mh=DmGetResource(strRsc, strAboutText);

	UIFieldSetText(cMainFld, MemHandleLock(mh));
	MemHandleUnlock(mh);
	DmReleaseResource(mh);
	UIFieldFocus(cMainFld);
	UIFieldUpdateScrollBar(cMainFld, cMainScrBar);
	return true;
}

/*
 * fMainEH
 */
Boolean
fMainEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		return true;

	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==cMainNext) {
			FrmGotoForm(fPasswords);
			return true;
		}
		break;

	case keyDownEvent:
		UIFieldScrollBarKeyHandler(ev, cMainFld, cMainScrBar);
	default:
		UIFieldScrollBarHandler(ev, cMainFld, cMainScrBar);
		break;
	}

	return false;
}

/*
 * fWait.c
 */
#include "Include.h"

/*
 * fViewAnswersEH
 */
Boolean
fWaitEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==cWaitView) {
			FrmGotoForm(fDictationDone);
			return true;
		}
	default: /* FALL-THRU */
		break;
	}

	return false;
}

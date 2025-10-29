/*
 * fDone.c
 */
#include "Include.h"

/*
 * fDoneEH
 */
Boolean
fDoneEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	default: /* FALL-THRU */
		break;
	}

	return false;
}

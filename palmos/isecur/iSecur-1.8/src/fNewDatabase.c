/*
 * fNewDatabase.c
 */
#include "Include.h"

/*
 * fNewDatabaseEH
 */
Boolean
fNewDatabaseEH(EventType *ev)
{
	EntropyAdd(pool, ev);

	switch (ev->eType) {
	case frmOpenEvent:
	case frmUpdateEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==cNewDatabaseNext) {
			FrmEraseForm(currentForm);
			FrmGotoForm(fCreateDatabase);
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

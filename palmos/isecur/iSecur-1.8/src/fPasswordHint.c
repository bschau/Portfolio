/**
 * Password Hint management routines.
 */
#include "Include.h"

/* protos */
static Boolean EH(EventType *);

/**
 * Run the Password Hint entry dialog.
 */
void
fPasswordHintRun(void)
{
	FormSave frm;
	MemHandle mh;

	UIFormPrologue(&frm, fPasswordHint, EH);

	if ((mh=AIBExtensionGet(AIBPasswordHint))) {
		UIFieldSetText(cPasswordHintField, MemHandleLock(mh));
		MemHandleFree(mh);
	}

	UIFieldFocus(cPasswordHintField);

	UIFormEpilogue(&frm, NULL, 0);
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	if ((ev->eType==ctlSelectEvent) && (ev->data.ctlSelect.controlID==cPasswordHintOK)) {
		Char *p=UIFieldGetText(cPasswordHintField);

		if (p) {
			if (!*p)
				p=NULL;
		}

		AIBExtensionSet(AIBPasswordHint, p);
	}

	return false;
}

/*
 * fHelp.c
 */
#include "Include.h"

/* protos */
static Boolean EH(EventType *);

/*
 * fHelpRun
 *
 * Show the Help form.
 *
 *  -> title		Help form title.
 *  -> rsc		String resource to show.
 */
void
fHelpRun(Char *title, UInt16 rsc)
{
	MemHandle mh=DmGetResource(strRsc, rsc);
	FormSave frm;

	ErrFatalDisplayIf(mh==NULL, "(Help) Cannot lock resource.");

	UIFormPrologue(&frm, fHelp, EH);
	FrmSetTitle(currentForm, title);
	UIFieldSetText(cHelpText, MemHandleLock(mh));
	MemHandleUnlock(mh);
	DmReleaseResource(mh);
	UIFieldFocus(cHelpText);
	UIFieldUpdateScrollBar(cHelpText, cHelpScrollBar);
	UIFormEpilogue(&frm, NULL, 0);
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	if (ev->eType==keyDownEvent)
		UIFieldScrollBarKeyHandler(ev, cHelpText, cHelpScrollBar);

	UIFieldScrollBarHandler(ev, cHelpText, cHelpScrollBar);
	return false;
}

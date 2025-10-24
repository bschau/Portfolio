/*
 * AboutForm.c
 */
#include "Include.h"

/* protos */
static Boolean EH(EventType *);

/*
 * fAboutRun
 *
 * Show the About form.
 */
void
fAboutRun(void)
{
	MemHandle mh=DmGetResource(strRsc, strAboutText);
	FormSave frm;

	ErrFatalDisplayIf(mh==NULL, "(About) Cannot lock resource.");

	UIFormPrologue(&frm, fAbout, EH);
	UIFieldSetText(cAboutText, MemHandleLock(mh));
	MemHandleUnlock(mh);
	DmReleaseResource(mh);
	UIFieldFocus(cAboutText);
	UIFieldUpdateScrollBar(cAboutText, cAboutScrollBar);
	UIFormEpilogue(&frm, NULL, 0);
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	if (ev->eType==keyDownEvent)
		UIFieldScrollBarKeyHandler(ev, cAboutText, cAboutScrollBar);

	UIFieldScrollBarHandler(ev, cAboutText, cAboutScrollBar);
	return false;
}

/*
 * fAbout.c
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

	ErrFatalDisplayIf(mh==NULL, "(About) Cannot lock string resource.");

	UIFormPrologue(&frm, fAbout, EH);
	UIFieldSetText(cAboutTxt, MemHandleLock(mh));
	MemHandleUnlock(mh);
	DmReleaseResource(mh);
	UIObjectFocus(cAboutTxt);
	UIFieldUpdateScrollBar(cAboutTxt, cAboutScrBar);
	UIFormEpilogue(&frm, NULL, 0);
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	if (ev->eType==keyDownEvent)
		UIFieldScrollBarKeyHandler(ev, cAboutTxt, cAboutScrBar, true);

	UIFieldScrollBarHandler(ev, cAboutTxt, cAboutScrBar);
	return false;
}

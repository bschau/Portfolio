/*
 * AboutForm.c
 */
#include "Include.h"

/* protos */
static Boolean EventHandler(EventType *);

/*
 * About
 *
 * Show the About form.
 */
void
About(void)
{
	MemHandle mh=DmGetResource(strRsc, strAboutText);
	FormSave frm;

	ErrFatalDisplayIf(mh==NULL, "(About) Cannot lock resource.");

	UIFormPrologue(&frm, fAbout, EventHandler);
	UIFieldSetText(cAboutText, MemHandleLock(mh));
	MemHandleUnlock(mh);
	DmReleaseResource(mh);
	UIFieldFocus(cAboutText);
	UIFieldUpdateScrollBar(cAboutText, cAboutScrollBar);
	UIFormEpilogue(&frm, NULL, 0);
}

/*
 * EventHandler.
 */
static Boolean
EventHandler(EventType *ev)
{
	if (ev->eType==keyDownEvent)
		UIFieldScrollBarKeyHandler(ev, cAboutText, cAboutScrollBar);

	UIFieldScrollBarHandler(ev, cAboutText, cAboutScrollBar);
	return false;
}

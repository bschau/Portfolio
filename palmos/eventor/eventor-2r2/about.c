/*
**	about.c
**
**	Code to drive the About form.
*/
#include "eventor.h"

/* protos */
static Boolean AboutEventHandler(EventType *);

/*
**	About
**
**	Show About Eventor form.
*/
void
About(void)
{
	MemHandle mh=UtilsLockString(strAboutText);
	FormSave frm;

	UIFormPrologue(&frm, fAbout, AboutEventHandler);
	UIFieldSetText(fAboutText, MemHandleLock(mh));
	UtilsUnlockString(mh);
	UIFieldFocus(fAboutText);
	UIFieldUpdateScrollBar(fAboutText, fAboutScrollBar);
	UIFormEpilogue(&frm, NULL, 0);
}

/*
**	AboutAboutEventHandler
*/
static Boolean
AboutEventHandler(EventType *ev)
{
	switch (ev->eType) {
	case keyDownEvent:
		UIFieldScrollBarKeyHandler(ev, fAboutText, fAboutScrollBar);
	default:	/* FALL-THRU */
		UIFieldScrollBarHandler(ev, fAboutText, fAboutScrollBar);
		break;
	}

	return false;
}

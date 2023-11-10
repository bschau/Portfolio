/*
 * fAbout.c
 */
#include "Include.h"

/* protos */
static Boolean EH(EventType *);

/*
 * fAboutRun
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
	switch (ev->eType) {
	case keyDownEvent:
		UIFieldScrollBarKeyHandler(ev, cAboutText, cAboutScrollBar);
	default:	/* FALL-THRU */
		UIFieldScrollBarHandler(ev, cAboutText, cAboutScrollBar);
		break;
	}

	return false;
}

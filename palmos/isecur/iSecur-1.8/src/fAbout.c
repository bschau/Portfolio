/*
 * fAbout.c
 */
#include "Include.h"

/* protos */
_UTL(static Boolean EH(EventType *));

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
	UIFieldFocus(cAboutTxt);
	UIFieldUpdateScrollBar(cAboutTxt, cAboutScrBar);
	UIFormEpilogue(&frm, NULL, 0);
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	EntropyAdd(pool, ev);
	if (ev->eType==keyDownEvent) {
		if (ev->data.keyDown.chr==pageUpChr) {
			UIFieldPageScroll(cAboutTxt, cAboutScrBar, winUp);
		} else if (ev->data.keyDown.chr==pageDownChr) {
			UIFieldPageScroll(cAboutTxt, cAboutScrBar, winDown);
		}
	}

	UIFieldScrollBarHandler(ev, cAboutTxt, cAboutScrBar);
	return false;
}

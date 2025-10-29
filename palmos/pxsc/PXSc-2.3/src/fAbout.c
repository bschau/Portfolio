/*
 * fAbout.c
 */
#include "Include.h"

/* protos */
_FRM(static Boolean AboutEH(EventType *));

/*
 * About
 *
 * Show the About form.
 */
void
About(void)
{
	FormSave frm;

	UIFormPrologue(&frm, fAbout, AboutEH);
	UIFieldSetText(cAboutText, StringGet(Str01));
	UIFieldFocus(cAboutText);
	UIFieldUpdateScrollBar(cAboutText, cAboutScrollBar);
	UIFormEpilogue(&frm, NULL, 0);
}

/*
 * AboutEH
 */
static Boolean
AboutEH(EventType *ev)
{
	if (ev->eType==keyDownEvent)
		UIFieldScrollBarKeyHandler(ev, cAboutText, cAboutScrollBar);

	UIFieldScrollBarHandler(ev, cAboutText, cAboutScrollBar);
	return false;
}

/*
**	about.c
**
**	Code to drive the About forms.
*/
#include "bmilog.h"

/* protos */
static Boolean AboutAboutEventHandler(EventType *);
static Boolean AboutHelpEventHandler(EventType *);

/*
**	AboutAbout
**
**	Show About BMILog form.
*/
void
AboutAbout(void)
{
	FormSave frm;

	UIFormPrologue(&frm, fAboutAbout, AboutAboutEventHandler);
	UIFieldSetText(fAboutAboutText, MemHandleLock(resStrAbout));
	MemHandleUnlock(resStrAbout);
	UIFieldFocus(fAboutAboutText);
	UIFieldUpdateScrollBar(fAboutAboutText, fAboutAboutScrollBar);
	UIFormEpilogue(&frm, NULL, 0);
}

/*
**	AboutAboutEventHandler
*/
static Boolean
AboutAboutEventHandler(EventType *ev)
{
	switch (ev->eType) {
	case keyDownEvent:
		UIFieldScrollBarKeyHandler(ev, fAboutAboutText, fAboutAboutScrollBar);
	default:	/* FALL-THRU */
		UIFieldScrollBarHandler(ev, fAboutAboutText, fAboutAboutScrollBar);
		break;
	}

	return false;
}

/*
**	AboutHelp
**
**	Shows a "help" dialog with the given resource string.
**
**	 -> title	Title string.
**	 -> text	Text string.
**
**	Note! Neither text nor title may be stack based!
*/
void
AboutHelp(Char *title, Char *text)
{
	FormSave frm;

	UIFormPrologue(&frm, fAboutHelp, AboutHelpEventHandler);
	FrmSetTitle(currentForm, title);
	UIFieldSetText(fAboutHelpText, text);
	UIFieldFocus(fAboutHelpText);
	UIFieldUpdateScrollBar(fAboutHelpText, fAboutHelpScrollBar);
	UIFormEpilogue(&frm, NULL, 0);
}

/*
**	AboutHelpEventHandler
*/
static Boolean
AboutHelpEventHandler(EventType *ev)
{
	switch (ev->eType) {
	case keyDownEvent:
		UIFieldScrollBarKeyHandler(ev, fAboutHelpText, fAboutHelpScrollBar);
	default:	/* FALL-THRU */
		UIFieldScrollBarHandler(ev, fAboutHelpText, fAboutHelpScrollBar);
		break;
	}

	return false;
}

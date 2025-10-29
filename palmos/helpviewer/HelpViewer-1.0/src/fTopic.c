/*
 * fMain.c
 */
#include "Include.h"

/* protos */

/* globals */
static Char *title, *body;

/**
 * Open a topic.
 *
 * @param t Title.
 * @param b Body text.
 */
void
fTopicRun(Char *t, Char *b)
{
	title=t;
	body=b;
	FrmGotoForm(fTopic);
}

/**
 * fTopicInit
 */
Boolean
fTopicInit(void)
{
	FrmSetTitle(currentForm, title);
	UIFieldSetText(cTopicText, body);
	UIFieldFocus(cTopicText);
	UIFieldUpdateScrollBar(cTopicText, cTopicScrollBar);
	return true;
}

/**
 * fTopicEH
 */
Boolean
fTopicEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==cTopicDone)
			FrmGotoForm(fMain);

		return true;

	case keyDownEvent:
		UIFieldScrollBarKeyHandler(ev, cTopicText, cTopicScrollBar);
	default:	/* FALL-THRU */
		UIFieldScrollBarHandler(ev, cTopicText, cTopicScrollBar);
		break;
	}

	return false;
}

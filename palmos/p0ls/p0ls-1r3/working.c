#include "p0ls.h"

/* macros */
#define STATUSTLX 2
#define STATUSTLY 60
#define STATUSEX 156
#define STATUSEY 12
#define PBARW 120
#define PBARH 10
#define PBARX ((160-PBARW)/2)
#define PBARY (STATUSTLY+STATUSEY+8)

/* globals */
static RectangleType status, pbar;

/* protos */
static Boolean evh(EventPtr);

/*******************************************************************************
*
*	working
*
*	Drives the turtle.
*
*	Input:
*		form - form.
*/
void
working(FormPtr form)
{
	FrmSetEventHandler(form, (FormEventHandlerPtr)evh);
	turtleinit();
}

/*******************************************************************************
 *
 *	evh
 *
 *	Event handler.
 */
static Boolean
evh(EventPtr ev)
{
	FormPtr form;

	switch (ev->eType) {
	case frmOpenEvent:
		form=FrmGetActiveForm();
		FrmDrawForm(form);

		status.topLeft.x=STATUSTLX;
		status.topLeft.y=STATUSTLY;
		status.extent.x=STATUSEX;
		status.extent.y=STATUSEY;

		pbar.topLeft.x=PBARX;
		pbar.topLeft.y=PBARY;
		pbar.extent.x=PBARW;
		pbar.extent.y=PBARH;
		WinDrawRectangleFrame(rectangleFrame, &pbar);

		pbar.topLeft.x=PBARX;
		pbar.topLeft.y=PBARY;
		pbar.extent.x=0;
		pbar.extent.y=PBARH;

		userinput=0;
		return true;

	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==fwcancel) {
			turtledone();
			FrmGotoForm(formstart);
		}

		return true;
	default:
		break;
	}

	return false;
}

/*******************************************************************************
 *
 *	updatevisual
 *
 *	Update progress display.
 *
 *	Input:
 *		step - step number.
 */
void
updatevisual(UInt16 step)
{
	Char buffer[64];
	UInt16 len, pix;
	UInt32 sw;

	sw=(((UInt32)PBARW)<<16)/(prefs.depth+2);
	pbar.extent.x=((sw*step)>>16)&0xffff;
	if (step<=prefs.depth)
		StrPrintF(buffer, "Expanding ... %d of %d", step, prefs.depth);
	else {
		if (step==prefs.depth+1)
			StrCopy(buffer, "Measuring stack size");
		else {
			StrCopy(buffer, "Calculating curve size");
			pbar.extent.x=PBARW;
		}
	}

	len=StrLen(buffer);
	pix=(156-FntCharsWidth(buffer, len))/2;
		
	WinFillRectangle(&status, 0);
	WinDrawChars(buffer, len, pix, STATUSTLY);

	WinDrawRectangle(&pbar, 0);
}

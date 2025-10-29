#include "sit.h"

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
static FormPtr parentform, thisform;
static UInt32 stepw, stepno, stepmax;

/* protos */
static Boolean evh(EventPtr);

/*******************************************************************************
*
*	working
*
*	Open working screen.
*/
void
working(FormPtr form,
        UInt16 maxsteps)
{
	WinSetPattern((const CustomPatternType *)blankpattern);
	parentform=form;
	thisform=FrmInitForm(formworking);
	FrmSetEventHandler(thisform, (FormEventHandlerPtr)evh);
	FrmSetActiveForm(thisform);
	FrmDrawForm(thisform);

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

	stepmax=maxsteps;
	stepw=((UInt32)PBARW<<16)/maxsteps;
	stepno=0;
}

/*******************************************************************************
*
*	stopworking
*
*	Stop working - dispose forms etc.
*/
void
stopworking(void)
{
	FrmEraseForm(thisform);
	FrmDeleteForm(thisform);
	if (parentform) {
		FrmSetActiveForm(parentform);
		FrmDrawForm(parentform);
	}
}

/*******************************************************************************
*
*	evh
*
* 	Event handler.
*/
static Boolean
evh(EventPtr ev)
{
	return false;
}

/*******************************************************************************
*
*	updatevisual
*
*	Update progress display.
*/
void
updatevisual(UInt8 *txt)
{
	UInt32 len;
	UInt16 pix;

	stepno++;
	if (stepno>=stepmax) {
		stepno=stepmax;
		pbar.extent.x=PBARW;
	} else
		pbar.extent.x=(stepno*stepw)>>16;

	len=StrLen(txt);
	pix=(156-FntCharsWidth(txt, len))/2;
		
	WinFillRectangle(&status, 0);
	WinDrawChars(txt, len, pix, STATUSTLY);

	WinDrawRectangle(&pbar, 0);
}

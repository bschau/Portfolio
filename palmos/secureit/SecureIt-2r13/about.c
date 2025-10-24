#include "sit.h"

/* macros */
#define BOXTLX 4
#define BOXTLY 72
#define BOXEX 136
#define BOXEY 60

/* globals */
static UInt8 *abouttext="Algorithms\n\n"
"This program uses the Blowfish crypto-algorithm developed by Bruce Schneier.\n"
"A detailed description of Blowfish can be found at:\n\n"
"http://www.counterpane.com\n\n"
"This program also uses the MD5 Message Digest algorithm developed by Ronald L. Rivest.\n"
"A detailed description of MD5 can be found at:\n\n"
"http://www.rsasecurity.com\n\n\n"
"Support\n\n"
"This program is free software. Use at your own risk. For the latest version, FAQ and user manual go to:\n\n"
"http://www.schau.com/sit/\n\n\n"
"Thanks\n\n"
"The list is getting longer and longer :) Thank you for valuable feedback to:\n\n"
"  Allan Marillier\n  Henrik Schledermann\n  Jan Lund Thomsen\n  Michael Stone\n  Mike Hanson\n  Uwe von der Wege\n  Yehuda\n\n  ... and a lot of people who has given valuable help\n\n"
"Also a big thank you to all at PenReader.com for supplying me with a debug version of PiLoc.\n\n"
"\nTranslations:\n\nJapanese: Tominaga Satoshi\nFrench: Marc Collin\nGerman: Christian Bolz\n\n";

/* protos */
static Boolean evh(EventPtr);

/*******************************************************************************
*
*	aboutinit
*
*	Initializes the about form.
*/
void
aboutinit(FormPtr form)
{
	FieldType *fld;

	fld=objectinform(form, abtfield);
	FldSetTextPtr(fld, abouttext);
	FldRecalculateField(fld, true);
	FrmSetFocus(form, FrmGetObjectIndex(form, abtfield));
		
	updatescrollbar(form, abtfieldscrb, abtfield);
	FrmSetEventHandler(form, (FormEventHandlerPtr)evh);
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
	FormPtr form=FrmGetActiveForm();
	RectangleType box;
	Boolean f;

	switch (ev->eType) {
	case frmOpenEvent:
		box.topLeft.x=BOXTLX;
		box.topLeft.y=BOXTLY;
		box.extent.x=BOXEX;
		box.extent.y=BOXEY;

		FrmDrawForm(form);
		WinDrawRectangleFrame(simpleFrame, &box);
		return true;
	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==abtok) {
			gotostart();
			return true;
		}
		break;
	default:
		f=scrollbarkeyhandler(ev, abtfieldscrb, abtfield);
		if (f==true)
			return true;
		
		scrollbarhandler(ev, abtfieldscrb, abtfield);
		/* no 'true' here - disables sclRepeat */
		break;
	}

	return false;
}

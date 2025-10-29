#include "sit.h"

/* macros */
#define PINCODEFRAME simpleFrame /* roundFrame */
#define STARSTLX 12	/* 2 */
#define STARSTLY 15
#define STARSEX 146	/* 156 */
#define STARSEY 15

/* globals */
static UInt8 starglyph[PASSWORDLEN]="************************";
static UInt8 keylayout[32];
static RectangleType stars;
static UInt8 inpwd[PASSWORDLEN+2], chkpwd[PASSWORDLEN+2];
static UInt16 dstindex, starflag, pass, passfunc;

/* protos */
static void initkeys(UInt8 *, UInt16);
static void setkeys(FormPtr);
static Boolean evh(EventPtr);
static void selev(EventPtr, FormPtr);
static void addchartopassword(UInt8);
static void updatedisplay(void);

/*******************************************************************************
*
*	initpwdvars
*
*	Initializes password variables.
*/
static void
initpwdvars(void)
{
	MemSet(inpwd, PASSWORDLEN+2, 0);
	dstindex=0;
}

/*******************************************************************************
*
*	initkeys
*
*	Initializes the key and password areas.
*/
static void
initkeys(UInt8 *layout,
         UInt16 func)
{
	UInt8 *p=keylayout;
	UInt16 i;

	stars.topLeft.x=STARSTLX;
	stars.topLeft.y=STARSTLY;
	stars.extent.x=STARSEX;
	stars.extent.y=STARSEY;

	MemSet(p, 32, 0);
	for (i=0; i<16; i++) {
		*p=*layout++;
		p+=2;
	}

	initpwdvars();
	starflag=prefs.pinstars;
	pass=0;
	passfunc=func;
}

/*******************************************************************************
*
*	setkeys
*
*	Commit key area to buttons.
*/
static void
setkeys(FormPtr form)
{
	UInt8 *p=keylayout;
	UInt16 j=pinbut00, i;
	ControlPtr b;

	for (i=0; i<16; i++) {
		if (*p!=' ') {
			b=objectinform(form, j);
			CtlSetLabel(b, p);
			j++;
		}
		p+=2;
	}
}

/*******************************************************************************
*
*	init*
*
*	Initializers for the various password dialogs.
*/
UInt16
initshuffle(UInt16 func)
{
	UInt8 *p=keylayout;
	UInt16 k, i, j;
	UInt8 t;

	initphonepad(func);

	if (func) {
		for (k=0; k<100; ) {
			i=(SysRandom(0)&0xf)<<1;
			j=(SysRandom(0)&0xf)<<1;
			if (i!=j) {
				t=*(p+i);
				*(p+i)=*(p+j);
				*(p+j)=t;
				k++;
			}
		}
	}

	return formpincode;
}

UInt16
initphonepad(UInt16 func)
{
	initkeys("1234567890      ", func);
	return formpincode;
}

UInt16
initcalculator(UInt16 func)
{
	initkeys("7894561230      ", func);
	return formpincode;
}

/*******************************************************************************
*
*	pincodeinit
*
* 	Initializes the pincode form.
*/
void
pincodeinit(FormPtr form)
{
	
	WinSetPattern((const CustomPatternType *)blankpattern);
	FrmSetEventHandler(form, (FormEventHandlerPtr)evh);
	FrmSetTitle(form, formtitle);
	setkeys(form);
}

/*******************************************************************************
*
* 	evh
*
* 	Event handler.
*/
static Boolean
evh(EventPtr ev)
{
	FormPtr form;

	form=FrmGetActiveForm();
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(form);
		WinDrawRectangleFrame(PINCODEFRAME, &stars);
		updatetoggle(starflag, PINCODETOGGLEX, PINCODETOGGLEY);
		return true;

	case keyDownEvent:
		addchartopassword(ev->data.keyDown.chr);
		updatedisplay();
		return true;

	case ctlSelectEvent:
		selev(ev, form);
		return true;

	default:
		break;
	}

	return false;
}

/*******************************************************************************
*
*	selev
*
* 	Handle select events.
*/
static void
selev(EventPtr ev,
      FormPtr form)
{
	UInt16 button=ev->data.ctlSelect.controlID;
	Int16 ret;
	ControlPtr b;
	const UInt8 *p;

	switch (button) {
	case pinbut00:
	case pinbut01:
	case pinbut02:
	case pinbut03:
	case pinbut04:
	case pinbut05:
	case pinbut06:
	case pinbut07:
	case pinbut08:
	case pinbut09:
		b=objectinform(form, button);
		p=CtlGetLabel(b);
		addchartopassword(*p);
		updatedisplay();
		break;

	case pinbutok:
		if (!dstindex)
			FrmCustomAlert(alertonechar, NULL, NULL, NULL);
		else {
			prefs.pinstars=starflag;
			if (passfunc==PASSSINGLE) {
				ret=singlepassword(inpwd);
				initpwdvars();
				if (ret)
					updatedisplay();
				else
					loadentry();
			} else {
				pass++;
				if (pass==1) {
					StrNCopy(formtitle, "Re-enter Password", FORMTITLELEN);
					FrmSetTitle(form, formtitle);

					MemMove(chkpwd, inpwd, PASSWORDLEN+1);
					initpwdvars();
					updatedisplay();
				} else {
					ret=doublepassword(inpwd, chkpwd);
					if (ret) {
						pass=0;
						StrNCopy(formtitle, "Enter Password", FORMTITLELEN);
						FrmSetTitle(form, formtitle);
						MemSet(chkpwd, PASSWORDLEN, 0);
						initpwdvars();
						updatedisplay();
					} else {
						changepassword(inpwd);
						MemSet(chkpwd, PASSWORDLEN, 0);
						initpwdvars();
						gotostart();
					}
				}
			}
		}

		break;

	case pinbutclear:
		initpwdvars();
		updatedisplay();
		break;

	case pinbuttoggle:
		starflag=!starflag;
		updatetoggle(starflag, PINCODETOGGLEX, PINCODETOGGLEY);
		updatedisplay();
		break;
	}
}

/*******************************************************************************
*
*	addchartopassword
*
* 	Add character to password.
*/
static void
addchartopassword(UInt8 chr)
{
	if (chr==8) {	/* Backspace */
		if (dstindex>0)
			dstindex--;

		inpwd[dstindex]='\x00';
		return;
	}

	if (chr<32 || chr>126)
		return;

	if (dstindex==PASSWORDLEN)
		return;

	inpwd[dstindex]=chr;
	dstindex++;
}

/*******************************************************************************
*
*	updatedisplay
*
* 	Update asterisk line.
*/
static void
updatedisplay(void)
{
	Int16 lenp=STARSEX, len=dstindex;
	Boolean flag;

	WinFillRectangle(&stars, 4);
	if (dstindex) {
		if (starflag)
			WinDrawChars(starglyph, dstindex, STARSTLX+((STARSEX-(dstindex*6))/2), STARSTLY+4);
		else {
			FntCharsInWidth(inpwd, &lenp, &len, &flag);
			WinDrawChars(inpwd, len, STARSTLX+((STARSEX-lenp)/2), STARSTLY+2);
		}
	}
}

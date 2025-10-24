#include "sit.h"

/* macros */
#define PINCODEFRAME simpleFrame /* roundFrame */
#define STARSTLX 12	/* 2 */
#define STARSTLY 15
#define STARSEX 146	/* 156 */
#define STARSEY 15

/* globals */
static Char starglyph[PASSWORDLEN]="************************";
static RectangleType stars;
static Char inpwd[PASSWORDLEN+2], chkpwd[PASSWORDLEN+2];
static UInt16 dstindex, starflag, pass, passfunc;

/* protos */
static void initkeys(Char *, UInt16);
static void setkeys(FormPtr);
static Boolean evh(EventPtr);
static void selev(EventPtr, FormPtr);
static void addchartopassword(Char);
static void updatedisplay(void);
static void updatetoggle(void);

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
*
*	Input:
*		layout - button string (layout).
*		func - function (Single or Double).
*/
static void
initkeys(Char *layout,
         UInt16 func)
{
	Char *p=keyp;
	UInt16 i;

	stars.topLeft.x=STARSTLX;
	stars.topLeft.y=STARSTLY;
	stars.extent.x=STARSEX;
	stars.extent.y=STARSEY;

	MemSet(keyp, 32, 0);
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
*
*	Input:
*		form - form.
*/
static void
setkeys(FormPtr form)
{
	Char *p=keyp;
	UInt16 j=0, i;
	ControlPtr b;

	for (i=0; i<16; i++) {
		if (*p!=' ') {
			b=objectinform(form, pinbut00+j);
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
*
*	Input:
*		func - function (Single or Double).
*	Output:
*		form id.
*/
UInt16
initshuffle(UInt16 func)
{
	UInt16 k, i, j;
	Char t;

	initphonepad(func);

	if (func) {
		for (k=0; k<100; ) {
			i=(SysRandom(0)&0xf)<<1;
			j=(SysRandom(0)&0xf)<<1;
			if (i!=j) {
				t=*(keyp+i);
				*(keyp+i)=*(keyp+j);
				*(keyp+j)=t;
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
*
* 	Input:
* 		form - form.
*/
void
pincodeinit(FormPtr form)
{
	FrmSetEventHandler(form, (FormEventHandlerPtr)evh);
	FrmSetTitle(form, formtitle);
	setkeys(form);
}

/*******************************************************************************
*
* 	evh
*
* 	Event handler.
*
* 	Input:
* 		ev - event.
* 	Output:
* 		handled: true.
* 		not handled: false.
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
		updatetoggle();
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
*
* 	Input:
* 		ev - event.
* 		form - form.
*/
static void
selev(EventPtr ev,
      FormPtr form)
{
	UInt16 button=ev->data.ctlSelect.controlID;
	Int16 ret;
	ControlPtr b;
	const Char *p;

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
					gotostart();
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
						changedbpwd(inpwd);
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
		updatetoggle();
		updatedisplay();
		break;
	}
}

/*******************************************************************************
*
*	addchartopassword
*
* 	Add character to password.
*
* 	Input:
* 		chr - character.
*/
static void
addchartopassword(Char chr)
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

/*******************************************************************************
*
*	updatetoggle
*
*	Update starflag toggle graphics.
*/
static void
updatetoggle(void)
{
	MemHandle bh;
	BitmapPtr bp;

	bh=DmGetResource(bitmapRsc, (starflag) ? pincodeswitchon : pincodeswitchoff);
	if (bh) {
		bp=MemHandleLock(bh);
		WinDrawBitmap(bp, PINCODETOGGLEX, PINCODETOGGLEY);
		MemHandleUnlock(bh);
	}
}

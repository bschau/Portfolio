/*
 * fGame.c
 */
#include "Include.h"

/* macros */
#define DRAWPIXEL(x, y) WinDrawLine(x, y, x, y)

/* protos */
static void UpdateTrigger(void);
static void UpdateForm(void);
static void UpdateInfo(void);
static void DrawAngle(void);
static Boolean Guess(void);

/* globals */
static Char *txtIncludeReflex="Include Reflex Angles", *txtExcludeReflex="Exclude Reflex Angles";

/*
 * fGameInit
 */
Boolean
fGameInit(void)
{
	if (session==NULL)
		SssNew();

	return true;
}

/*
 * fGameEH
 */
Boolean
fGameEH(EventType *ev)
{
	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
	case frmUpdateEvent:
		UpdateForm();
		return true;

	case ctlSelectEvent:
		if (ev->data.ctlSelect.controlID==cGameGuess)
			return Guess();
		else if (ev->data.ctlSelect.controlID==cGameTrigger) {
			if (prefs.flags&PrfIncludeReflex)
				prefs.flags&=~PrfIncludeReflex;
			else
				prefs.flags|=PrfIncludeReflex;

			UpdateTrigger();
		}

		break;

	case menuEvent:
		if (ev->data.menu.itemID==mGameQuit && FrmAlert(aQuit)==0) {
			session=NULL;
			FrmGotoForm(fMain);
			return true;
		}	
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * UpdateTrigger
 *
 * Update the text in the trigger.
 */
static void
UpdateTrigger(void)
{
	ControlType *ctl=UIFormGetObject(cGameTrigger);

	if (prefs.flags&PrfIncludeReflex)
		CtlSetLabel(ctl, txtIncludeReflex);
	else
		CtlSetLabel(ctl, txtExcludeReflex);
}

/*
 * UpdateForm
 *
 * Redraw form.
 */
static void
UpdateForm(void)
{
	UpdateTrigger();
	UIFieldClear(cGameDegrees);
	UpdateInfo();
	DrawAngle();
	UIFieldFocus(cGameDegrees);
}

/*
 * UpdateInfo
 *
 * Updates round and score display.
 */
static void
UpdateInfo(void)
{
	Char buffer[16];

	StrPrintF(buffer, "%d", session->round);
	UIFieldSetText(cGameRound, buffer);
	StrPrintF(buffer, "%d", session->score);
	UIFieldSetText(cGameScore, buffer);
}

/*
 * DrawAngle
 *
 * Draw the angle to guess.
 */
static void
DrawAngle(void)
{
	float sX, sY;
	UInt16 i, a;
	UInt32 version;

	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version);
	
	a=session->start;
	if (version<sysMakeROMVersion(3, 5, 0, sysROMStageRelease, 0)) {
		for (i=0; i<360; i+=45)
			DRAWPIXEL((80+(Radius*CosTab[i])), (80+(Radius*SinTab[i])));

		for (i=0; i<session->angle; i++) {
			if (a>359)
				a=0;

			DRAWPIXEL((80+(20*CosTab[a])), (80+(20*SinTab[a])));
			a++;
		}
	} else {
		for (i=0; i<360; i+=45)
			WinDrawPixel(80+(Radius*CosTab[i]), 80+(Radius*SinTab[i]));

		for (i=0; i<session->angle; i++) {
			if (a>359)
				a=0;

			WinDrawPixel(80+(20*CosTab[a]), 80+(20*SinTab[a]));
			a++;
		}
	}

	sX=80+(Radius*CosTab[session->start]);
	sY=80+(Radius*SinTab[session->start]);
	WinDrawLine(sX, sY, 80, 80);

	a=session->start+session->angle;
	if (a>359)
		a-=360;

	sX=80+(Radius*CosTab[a]);
	sY=80+(Radius*SinTab[a]);
	WinDrawLine(sX, sY, 80, 80);
}

/*
 * Guess
 *
 * Handle guess.
 *
 * Return true if guess is valid, false otherwise.
 */
static Boolean
Guess(void)
{
	Char buffer[16];
	Char *g;
	Int16 v;
	UInt16 i;

	if ((g=UIFieldGetText(cGameDegrees))==NULL) {
		FrmAlert(aGuessInvalid);
		return false;
	}

	for (i=0; i<StrLen(g); i++) {
		if (g[i]<'0' || g[i]>'9') {
			FrmAlert(aGuessInvalid);
			return false;
		}
	}

	v=StrAToI(g);
	if (v<0 || v>360) {
		FrmAlert(aGuessInvalid);
		return false;
	}

	if (v>session->angle)
		i=v-session->angle;
	else
		i=session->angle-v;

	StrPrintF(buffer, "%d", session->angle);

	if (!i)
		FrmCustomAlert(aRightOn, buffer, NULL, NULL);
	else if (i<11)
		FrmCustomAlert(aClose, buffer, NULL, NULL);
	else
		FrmCustomAlert(aAngleIs, buffer, NULL, NULL);

	SssUpdate(i);
	if (session->round<11) {
		RectangleType r;

		r.topLeft.x=0;
		r.topLeft.y=28;
		r.extent.x=160;
		r.extent.y=118;
		WinEraseRectangle(&r, 0);
		UpdateForm();
	} else
		FrmGotoForm(fEnd);

	return true;
}

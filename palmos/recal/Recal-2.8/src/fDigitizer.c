/*
 * fDigitize.c
 */
#include "Include.h"

/* macros */
#define AllKeys (keyBitPower|keyBitPageUp|keyBitPageDown|keyBitHard1|keyBitHard2|keyBitHard3|keyBitHard4|keyBitCradle|keyBitAntenna|keyBitContrast)
#define DigiSpot 10
#define GfxSpot 10
#define recalGoOn firstUserEvent

/* structs */
typedef struct {
	Coord extentX;
	Coord extentY;
	UInt16 state;
	UInt16 toGo;
	PointType actTopLeft;
	PointType actBottomRight;
	PointType expTopLeft;
	PointType expBottomRight;
	MemHandle digiSpotH;
	Coord pX;
	Coord pY;
	UInt16 timeout;
	UInt16 precision;
} DigiVars;
enum { SDrawTopLeft, SDrawBottomRight, SDrawCenter, SVerify, SFinal, SError };

/* protos */
static DigiVars *InitDigitizer(void);
static void CloseDigitizer(DigiVars *);
static void UpdateDisplay(DigiVars *, Coord, Coord);
static void SwitchState(DigiVars *);
static Boolean RunTap(DigiVars *);

/*
 * InitDigitizer
 *
 * Initialize digitizer screen etc.
 */
static DigiVars *
InitDigitizer(void)
{
	MemHandle mh=MemHandleNew(sizeof(DigiVars));
	DigiVars *dv=NULL;

	if (!mh) 
		return NULL;

	dv=MemHandleLock(mh);

	WinGetDisplayExtent(&dv->extentX, &dv->extentY);

	dv->digiSpotH=DmGetResource(bitmapRsc, bmpPointer);

	dv->state=SDrawTopLeft;

	return dv;
}

/*
 * CloseDigitizer
 *
 * Dealloc digitizer stuff.
 *
 *  -> dv		DigiVars.
 */
static void
CloseDigitizer(DigiVars *dv)
{
	if (dv) {
		if (dv->digiSpotH)
			DmReleaseResource(dv->digiSpotH);

		MemPtrFree(dv);
	}
}

/*
 * UpdateDisplay
 *
 * Draw instructions and the Digi Spot at the specified location.
 *
 *  -> dv		DigiVars.
 *  -> sX		Digi Spot X.
 *  -> sY		Digi Spot Y.
 */
static void
UpdateDisplay(DigiVars *dv, Coord sX, Coord sY)
{
	Char strBuf[32];
	UInt16 pW=dv->extentX, sLen, x;
	FontID fId=FntSetFont(boldFont);
	RectangleType r;
	Boolean trunc;

	RctSetRectangle(&r, 0, 0, dv->extentX, dv->extentY);
	WinEraseRectangle(&r, 0);

	WinDrawBitmap(MemHandleLock(dv->digiSpotH), sX-GfxSpot, sY-GfxSpot);
	MemHandleUnlock(dv->digiSpotH);

	MemSet(strBuf, sizeof(strBuf), 0);
	SysStringByIndex(strTapThree, 0, strBuf, sizeof(strBuf));
	sLen=StrLen(strBuf);
	pW=dv->extentX;
	FntCharsInWidth(strBuf, &pW, &sLen, &trunc);
	x=(dv->extentX-pW)/2;
	WinDrawChars(strBuf, sLen, x, 40);

	MemSet(strBuf, sizeof(strBuf), 0);
	SysStringByIndex(strTapThree, 1, strBuf, sizeof(strBuf));
	WinDrawChars(strBuf, StrLen(strBuf), x, 52);

	MemSet(strBuf, sizeof(strBuf), 0);
	if (dv->toGo>1) {
		SysStringByIndex(strTapThree, 2, strBuf, sizeof(strBuf));
		WinDrawChars(strBuf, StrLen(strBuf), x, 110);

		x+=pW;
		StrPrintF(strBuf, "%u", dv->toGo);
		sLen=StrLen(strBuf);
		pW=40;
		FntCharsInWidth(strBuf, &pW, &sLen, &trunc);
		x-=pW;
		WinDrawChars(strBuf, sLen, x, 110);
	} else {
		SysStringByIndex(strTapThree, 3, strBuf, sizeof(strBuf));
		sLen=StrLen(strBuf);
		pW=dv->extentX;
		FntCharsInWidth(strBuf, &pW, &sLen, &trunc);
		WinDrawChars(strBuf, sLen, (dv->extentX-pW)/2, 110);
	}

	FntSetFont(fId);
}

/*
 * SwitchState
 *
 * Switch to selected state.
 *
 *  -> dv		DigiVars.
 */
static void
SwitchState(DigiVars *dv)
{
	Coord x, y;
	DigitizerPrefs d;

	for (EVER) {
		switch (dv->state) {
		case SDrawTopLeft:
			dv->expTopLeft.x=DigiSpot;
			dv->expTopLeft.y=DigiSpot;
			PenResetCalibration();
			UpdateDisplay(dv, dv->expTopLeft.x, dv->expTopLeft.y);
			dv->state=SDrawBottomRight;
			return;

		case SDrawBottomRight:
			dv->actTopLeft.x=dv->pX;
			dv->actTopLeft.y=dv->pY;
			dv->expBottomRight.x=dv->extentX-DigiSpot-1;
			dv->expBottomRight.y=dv->extentX-DigiSpot-1;
			SysTaskDelay(SysTicksPerSecond()/4);
			UpdateDisplay(dv, dv->expBottomRight.x, dv->expBottomRight.y);
			dv->state=SDrawCenter;
			return;

		case SDrawCenter:
			dv->actBottomRight.x=dv->pX;
			dv->actBottomRight.y=dv->pY;
			PenCalibrate(&dv->actTopLeft, &dv->actBottomRight, &dv->expTopLeft, &dv->expBottomRight);
			SysTaskDelay(SysTicksPerSecond()/4);
			UpdateDisplay(dv, dv->extentX/2, dv->extentY/2);
			dv->state=SVerify;
			return;

		case SVerify:
			x=dv->extentX/2;
			y=dv->extentY/2;

			if ((dv->pX<(x-dv->precision)) || (dv->pX>(x+dv->precision)) || (dv->pY<(y-dv->precision)) || (dv->pY>(y+dv->precision))) {
				dv->toGo--;
				if (dv->toGo>0)
					dv->state=SDrawTopLeft;
				else
					dv->state=SError;
			} else
				dv->state=SFinal;
			break;

		case SFinal:
			MemMove(&d.aTL, &dv->actTopLeft, 4*sizeof(PointType));
			PMSetPref(&d, sizeof(DigitizerPrefs), PrfDigitizer);
		case SError:
			return;

		}
	}
}

/*
 * fDigitizerRun
 *
 * Show and run the Digitizer.
 *
 *  -> c		Close/Save all forms.
 *  -> to		Timeout.
 *  -> prec		Precision.
 */
void
fDigitizerRun(Boolean c, UInt16 to, UInt16 prec)
{
	DigitizerPrefs dP;
	Boolean dPValid;
	UInt32 oldMask;
	DigiVars *dv;
	UInt16 cardNo;
	LocalID dbID;
	Coord pX, pY;
	Boolean tapValid;

	if (c) {
		EventType ev;
		Err err;

		FrmSaveAllForms();
		FrmCloseAllForms();

		MemSet(&ev, sizeof(EventType), 0);
		ev.eType=recalGoOn;
		EvtAddEventToQueue(&ev);

		for (EVER) {
			EvtGetEvent(&ev, evtWaitForever);
			if (ev.eType==appStopEvent)
				return;

			if (ev.eType==nilEvent)
				continue;

			if (ev.eType==recalGoOn)
				break;

			if (SysHandleEvent(&ev)==true ||
			    MenuHandleEvent(NULL, &ev, &err)==true)
				continue;

			FrmDispatchEvent(&ev);
		}
	}

	if (SysCurAppDatabase(&cardNo, &dbID)==errNone)
		AlmSetAlarm(cardNo, dbID, 0, 0, 0);

	if ((dv=InitDigitizer())==NULL) {
		ErrNonFatalDisplay("(fDigitizeRun) Out of memory.");
		return;
	}

	if (dv->digiSpotH==NULL) {
		CloseDigitizer(dv);
		ErrNonFatalDisplay("(fDigitizerRun) Cannot initialize.");
		return;
	}

	dv->timeout=to;
	dPValid=PMGetPref(&dP, sizeof(DigitizerPrefs), PrfDigitizer);

	do {
		EvtGetPen(&pX, &pY, &tapValid);
	} while (tapValid);

	dv->toGo=5;
	dv->precision=prec;
	SwitchState(dv);

	oldMask=KeySetMask(keyBitsAll);
	if (RunTap(dv)==false) {
		if (dPValid)
			PenCalibrate(&dP.aTL, &dP.aBR, &dP.eTL, &dP.eBR);
	}

	KeySetMask(oldMask);
	CloseDigitizer(dv);
}

/*
 * RunTap
 *
 * Run the "Tap Thrice" dialog.
 *
 *  -> dv		DigiVars.
 *
 * Return true if calibration succeeded, false otherwise.
 */
static Boolean
RunTap(DigiVars *dv)
{
	UInt32 ticks;
	Coord pX, pY;
	Boolean tapValid;
	EventType ev;

	while (dv->state<SFinal) {
		ticks=TimGetSeconds();
		do {
			EvtGetPen(&pX, &pY, &tapValid);
			if (TimGetSeconds()-ticks>dv->timeout)
				return false;
			do {
				EvtGetEvent(&ev, 0);
				if (ev.eType==appStopEvent)
					return false;

				if ((KeyCurrentState()&AllKeys))
					return false;
			} while (ev.eType!=nilEvent);
		} while (!tapValid);

		ticks=TimGetSeconds();
		do {
			EvtGetPen(&dv->pX, &dv->pY, &tapValid);
			if (TimGetSeconds()-ticks>dv->timeout)
				return false;
		} while (tapValid);

		SwitchState(dv);
	}

	return (dv->state==SFinal) ? true : false;
}

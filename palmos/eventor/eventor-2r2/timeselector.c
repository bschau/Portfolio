#include "eventor.h"

/* macros */
enum { TimeNoScroll=0, TimePrev, TimeNext };

/* globals */
static Int16 updateButton, wHour, wMinuteH, wMinuteL;
static FormPtr tsFrm;
static Boolean ampm, isPm;
static Char txtHour[6], txtMinuteH[6], txtMinuteL[6];

/* protos */
static void *GetFormObject(UInt16);
static void ShowTime(UInt16);
static Boolean EventHandler(EventType *);

/*
**	GetFormObject
**
**	Return pointer to the form object in the current form given by id.
**
**	 -> id		Object ID.
**
**	Returns pointer or NULL (not found).
*/
static void *
GetFormObject(UInt16 id)
{
	return FrmGetObjectPtr(tsFrm, FrmGetObjectIndex(tsFrm, id));
}

/*
**	ShowTime
**
**	Sets new labels on the time buttons. Adds/decs if necessary.
**
**	 -> action	Action for the buttons.
*/
static void
ShowTime(UInt16 action)
{
	switch (action) {
	case TimePrev:
		switch (updateButton) {
		case fTimeSelectorHour:
			wHour--;
			if (ampm==true) {
				if (wHour<1)
					wHour=12;
			} else {
				if (wHour<0)
					wHour=23;
			}
			break;
		case fTimeSelectorMinuteH:
			wMinuteH--;
			if (wMinuteH<0)
				wMinuteH=5;
			break;
		case fTimeSelectorMinuteL:
			wMinuteL--;
			if (wMinuteL<0)
				wMinuteL=9;
			break;
		}
		break;

	case TimeNext:
		switch (updateButton) {
		case fTimeSelectorHour:
			wHour++;
			if (ampm==true) {
				if (wHour>12)
					wHour=1;
			} else {
				if (wHour>23)
					wHour=0;
			}
			break;
		case fTimeSelectorMinuteH:
			wMinuteH++;
			if (wMinuteH>5)
				wMinuteH=0;
			break;
		case fTimeSelectorMinuteL:
			wMinuteL++;
			if (wMinuteL>9)
				wMinuteL=0;
			break;
		}
		break;

	default:	/* FALL-THRU */
		break;
	}

	StrPrintF(txtHour, "%u", wHour);
	StrPrintF(txtMinuteH, "%u", wMinuteH);
	StrPrintF(txtMinuteL, "%u", wMinuteL);

	CtlSetLabel(GetFormObject(fTimeSelectorHour), txtHour);
	CtlSetLabel(GetFormObject(fTimeSelectorMinuteL), txtMinuteL);
	CtlSetLabel(GetFormObject(fTimeSelectorMinuteH), txtMinuteH);
}

/*
**	TimeSelectorNeedsAMPM
**
**	Decide if this device uses AM/PM notation.
**
**	Returns true if device does, false otherwise.
*/
Boolean
TimeSelectorNeedsAMPM(void)
{
	switch (PrefGetPreference(prefTimeFormat)) {
	case tfColonAMPM:
	case tfDotAMPM:
	case tfHoursAMPM:
		return true;
	default:
		break;
	}

	return false;
}

/*
**	TimeSelector
**
**	Implements the SelectOneTime dialog - nag, nag, why is this
**	a v3.1 only routine. Palm really should have thought of that! :o)
**
**	<-> hour	The hour selected in the form.
**	<-> minute	The minute selected in the form.
**	 -> title	Title for the form.
**
**	Returns true if time was changed by user, false otherwise (or error).
*/
Boolean
TimeSelector(Int16 *hour, Int16 *minute, const Char *title)
{
	FormPtr oFrm=FrmGetActiveForm();
	Boolean retCode=false;

	tsFrm=FrmInitForm(fTimeSelector);
	ErrFatalDisplayIf(tsFrm==NULL, "(TimeSelector) Cannot initialize TimeSelector form.");

	updateButton=fTimeSelectorHour;
	wHour=*hour;
	wMinuteH=*minute/10;
	wMinuteL=*minute%10;

	ampm=TimeSelectorNeedsAMPM();
	isPm=false;
	if (ampm==true) {
		if (wHour>12) {
			wHour-=12;
			isPm=true;
		}

		if (wHour==0)
			wHour=12;
	}

	FrmSetEventHandler(tsFrm, (FormEventHandlerPtr)EventHandler);
	FrmSetActiveForm(tsFrm);
	FrmDrawForm(tsFrm);

	if (ampm==false) {
		FrmHideObject(tsFrm, FrmGetObjectIndex(tsFrm, fTimeSelectorAM));
		FrmHideObject(tsFrm, FrmGetObjectIndex(tsFrm, fTimeSelectorPM));
	}

	FrmSetTitle(tsFrm, (Char *)title);
	ShowTime(TimeNoScroll);
	FrmSetControlValue(tsFrm, FrmGetObjectIndex(tsFrm, updateButton), 1);
	FrmSetControlValue(tsFrm, FrmGetObjectIndex(tsFrm, (isPm==false) ? fTimeSelectorAM : fTimeSelectorPM), 1);

	if (FrmDoDialog(tsFrm)==fTimeSelectorOK) {
		retCode=true;
		if (ampm==true) {
			if (wHour==12)
				wHour=0;
			if (isPm==true)
				wHour+=12;
		}

		*hour=wHour;
		*minute=(wMinuteH*10)+wMinuteL;
	}
	FrmSetActiveForm(oFrm);
	FrmEraseForm(tsFrm);
	FrmDeleteForm(tsFrm);
	return retCode;
}

/*
**	EventHandler
**
**	Event handler for the TimeSelector form.
**
**	 -> ev		Event.
**
**	Returns true if event is handled, false otherwise.
*/
static Boolean
EventHandler(EventType *ev)
{
	switch (ev->eType) {
	case keyDownEvent:
		switch (ev->data.keyDown.chr) {
		case pageUpChr:
			ShowTime(TimeNext);
			return true;
		case pageDownChr:
			ShowTime(TimePrev);
			return true;
		}
		break;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case fTimeSelectorOK:
		case fTimeSelectorCancel:
			break;
		case fTimeSelectorUp:
			ShowTime(TimeNext);
			return true;
		case fTimeSelectorDown:
			ShowTime(TimePrev);
			return true;
		case fTimeSelectorHour:
		case fTimeSelectorMinuteH:
		case fTimeSelectorMinuteL:
			updateButton=ev->data.ctlSelect.controlID;
			return true;
		case fTimeSelectorAM:
			isPm=false;
			return true;
		case fTimeSelectorPM:
			isPm=true;
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

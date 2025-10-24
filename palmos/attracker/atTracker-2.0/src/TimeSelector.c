/*
 * TimeSelector.c
 *
 * A Select Time requester with history.
 */
#include "Include.h"

/* macros */
enum { TimeNoScroll=0, TimePrev, TimeNext };

#define HistoryLength 5
#define HistorySize (HistoryLength*sizeof(UInt16))

/* globals */
static Int16 updateButton;
static UInt8 wHour, wMinuteH, wMinuteL;
static FormType *tsFrm;
static Char txtHour[6], txtMinuteH[6], txtMinuteL[6];
static Char historyTxt[HistoryLength][6];
static Char *hisArr[HistoryLength+1];
static UInt16 history[HistoryLength];

/* protos */
static void *GetFormObject(UInt16);
static void HistoryToLabel(UInt16, Char *);
static void ShowTime(UInt16);
static Boolean EH(EventType *);

/*
 * GetFormObject
 *
 * Return pointer to the form object in the current form given by id.
 *
 *  -> id		Object ID.
 *
 * Returns pointer or NULL (not found).
 */
static void *
GetFormObject(UInt16 id)
{
	return FrmGetObjectPtr(tsFrm, FrmGetObjectIndex(tsFrm, id));
}

/*
 * HistoryToLabel
 *
 * Convert a time to label.
 *
 * -> t			Time.
 * -> dst		Destination label.
 */
static void
HistoryToLabel(UInt16 t, Char *dst)
{
	Char delim=':';

	switch (PrefGetPreference(prefTimeFormat)) {
	case tfDot:
	case tfDotAMPM:
	case tfDot24h:
		delim='.';
		break;
	case tfComma24h:
		delim=',';
	default:	/* FALL-THRU */
		break;
	}
	StrPrintF(dst, "%2u%c%02u", (t>>8)&0xff, delim, t&0xff);
}

/*
 * ShowTime
 *
 * Sets new labels on the time buttons. Adds/decs if necessary.
 *
 *  -> action	Action for the buttons.
 */
static void
ShowTime(UInt16 action)
{
	switch (action) {
	case TimePrev:
		switch (updateButton) {
		case cTimeSelectorHour:
			if (wHour)
				wHour--;
			else
				wHour=23;
			break;

		case cTimeSelectorMinuteH:
			if (wMinuteH)
				wMinuteH--;
			else
				wMinuteH=5;
			break;

		case cTimeSelectorMinuteL:
			if (wMinuteL)
				wMinuteL--;
			else
				wMinuteL=9;
			break;
		}
		break;

	case TimeNext:
		switch (updateButton) {
		case cTimeSelectorHour:
			if (wHour==23)
				wHour=0;
			else
				wHour++;
			break;

		case cTimeSelectorMinuteH:
			if (wMinuteH==5)
				wMinuteH=0;
			else
				wMinuteH++;
			break;

		case cTimeSelectorMinuteL:
			if (wMinuteL==9)
				wMinuteL=0;
			else
				wMinuteL++;
			break;
		}
		break;

	default:	/* FALL-THRU */
		break;
	}

	StrPrintF(txtHour, "%u", wHour);
	StrPrintF(txtMinuteH, "%u", wMinuteH);
	StrPrintF(txtMinuteL, "%u", wMinuteL);

	CtlSetLabel(GetFormObject(cTimeSelectorHour), txtHour);
	CtlSetLabel(GetFormObject(cTimeSelectorMinuteL), txtMinuteL);
	CtlSetLabel(GetFormObject(cTimeSelectorMinuteH), txtMinuteH);
}

/*
 * TimeSelector
 *
 * Implements the SelectOneTime dialog - nag, nag, why is this
 * a v3.1 only routine. Palm really should have thought of that! :o)
 * Anyway, this TimeSelector also keeps a small history.
 *
 * <-> hour	The hour selected in the form.
 * <-> minute	The minute selected in the form.
 *  -> title	Title for the form.
 *
 * Returns true if time was changed by user, false otherwise (or error).
 */
Boolean
TimeSelector(UInt8 *hour, UInt8 *minute, const Char *title)
{
	FormType *oFrm=FrmGetActiveForm();
	Boolean retCode=false;
	ListType *lst;
	UInt16 i;

	tsFrm=FrmInitForm(fTimeSelector);
	ErrFatalDisplayIf(tsFrm==NULL, "(TimeSelector) Cannot initialize TimeSelector form.");

	updateButton=cTimeSelectorHour;
	wHour=*hour;
	wMinuteH=*minute/10;
	wMinuteL=*minute%10;

	FrmSetEventHandler(tsFrm, (FormEventHandlerType *)EH);
	FrmSetActiveForm(tsFrm);

	if (history[0]) {
		for (i=0; i<HistoryLength; i++) {
			if (history[i]) {
				HistoryToLabel(history[i], &historyTxt[i][0]);
				hisArr[i]=&historyTxt[i][0];
			} else
				break;
		}
		hisArr[i]=NULL;

		lst=GetFormObject(cTimeSelectorList);
		LstSetListChoices(lst, &hisArr[0], i);
		LstSetHeight(lst, i);

		FrmShowObject(tsFrm, FrmGetObjectIndex(tsFrm, cTimeSelectorPopup));
	}

	FrmSetTitle(tsFrm, (Char *)title);
	ShowTime(TimeNoScroll);
	FrmSetControlValue(tsFrm, FrmGetObjectIndex(tsFrm, updateButton), 1);

	if (FrmDoDialog(tsFrm)==cTimeSelectorOK) {
		retCode=true;
		*hour=wHour;
		*minute=(wMinuteH*10)+wMinuteL;
	}
	FrmSetActiveForm(oFrm);
	FrmEraseForm(tsFrm);
	FrmDeleteForm(tsFrm);
	return retCode;
}

/*
 * EventHandler
 *
 * Event handler for the TimeSelector form.
 *
 *  -> ev		Event.
 *
 * Returns true if event is handled, false otherwise.
 */
static Boolean
EH(EventType *ev)
{
	UInt16 t;

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
		case cTimeSelectorOK:
		case cTimeSelectorCancel:
			break;
		case cTimeSelectorUp:
			ShowTime(TimeNext);
			return true;
		case cTimeSelectorDown:
			ShowTime(TimePrev);
			return true;
		case cTimeSelectorHour:
		case cTimeSelectorMinuteH:
		case cTimeSelectorMinuteL:
			updateButton=ev->data.ctlSelect.controlID;
			return true;
		}
		break;

	case popSelectEvent:
		if (ev->data.popSelect.controlID==cTimeSelectorPopup) {
			t=history[ev->data.popSelect.selection];
			wHour=(t>>8)&0xff;
			wMinuteH=(t&0xff)/10;
			wMinuteL=(t&0xff)%10;
			CtlHitControl(GetFormObject(cTimeSelectorOK));
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * TimeHistoryGet
 *
 * Get history block from time preferences.
 *
 *  -> id		Preferences block ID.
 */
void
TimeHistoryGet(UInt16 id)
{
	UInt16 len=HistorySize;
	Int16 retPrf;

	retPrf=PrefGetAppPreferences((UInt32)CRID, id, &history, &len, true);
	if (retPrf==noPreferenceFound || len!=HistorySize)
		MemSet(&history[0], HistorySize, 0);
}

/*
 * TimeHistoryPut
 *
 * Save history block to preferences.
 *
 *  -> id		Preferences block ID.
 */
void
TimeHistoryPut(UInt16 id)
{
	PrefSetAppPreferences((UInt32)CRID, id, APPVER, &history, HistorySize, true);
}

/*
 * TimeHistoryAdd
 *
 * Add time to history.
 *
 *  -> h		Hour.
 *  -> m		Minute.
 */
void
TimeHistoryAdd(UInt8 h, UInt8 m)
{
	UInt16 t=(h<<8)|m;
	Int16 j=HistoryLength, i;

	for (i=0; i<HistoryLength; i++) {
		if (t==history[i]) {
			/* shuffle this one to the top */
			j=i+1;
			break;
		}
	}

	for (i=j-1; i>0; i--)
		history[i]=history[i-1];

	history[0]=t;
}

/*
 * fPreferences.c
 */
#include "Include.h"

/* protos */
static Boolean EH(EventType *);
static void DrawGadget(IndexedColorType);

/* globals */
static UInt16 prfTime;
static ControlType *prfDouble, *prfAlternate;

/*
 * fPreferencesRun
 *
 * Handle preferences dialog.
 */
void
fPreferencesRun(void)
{
	FormSave frm;

	prfTime=prefs.defaultTime;
	UIFormPrologue(&frm, fPreferences, EH);
	prfDouble=UIObjectGet(cPreferencesDouble);
	prfAlternate=UIObjectGet(cPreferencesAlternate);

	DTSelectorSetTime(cPreferencesTime, selector1, prfTime);

	if (canDoubleTable==true) {
		if (prefs.flags&PFlagDoubleTable)
			CtlSetValue(prfDouble, 1);
		else
			CtlSetValue(prfDouble, 0);

		UIObjectShow(cPreferencesDouble);
	}

	if (canColor==true) {
		if (prefs.flags&PFlagAlternate)
			CtlSetValue(prfAlternate, 1);
		else
			CtlSetValue(prfAlternate, 0);

		UIObjectShow(cPreferencesAlternate);

		UIObjectShow(cPreferencesGadget);
		DrawGadget(prefs.barColor);
	}

	if (UIFormEpilogue(&frm, NULL, cPreferencesCancel)==true)
		prefs.defaultTime=prfTime;
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	UInt16 f=0;
	UInt8 h, m;

        switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cPreferencesTime:
			DTTimeUnpack(prfTime, &h, &m);
			if (TimeSelector(&h, &m, "Set Default Time")==true) {
				prfTime=DTTimePack(h, m);
				DTSelectorSetTime(cPreferencesTime, selector1, prfTime);
			}
			return true;
		case cPreferencesOK:
			if (CtlGetValue(prfDouble))
				f=PFlagDoubleTable;

			if (CtlGetValue(prfAlternate))
				f|=PFlagAlternate;

			prefs.flags=f;
			break;
		}
		break;

	case penDownEvent:
		if (UIGadgetHandler(ev, cPreferencesGadget)==true) {
			UIPickColor(&prefs.barColor, NULL, NULL, "Select New Color", NULL);
			FrmDrawForm(currentForm);
			DrawGadget(prefs.barColor);
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}
	return false;
}

/*
 * DrawGadget
 *
 * Draw the color gadget.
 */
static void
DrawGadget(IndexedColorType c)
{
	RectangleType r;
	IndexedColorType o;

	FrmGetObjectBounds(currentForm, FrmGetObjectIndex(currentForm, cPreferencesGadget), &r);
	WinDrawRectangleFrame(simpleFrame, &r);

	o=WinSetForeColor(c);
	WinDrawRectangle(&r, 0);
	WinSetForeColor(o);
}

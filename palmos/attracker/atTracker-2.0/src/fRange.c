/*
 * fRange.c
 */
#include "Include.h"

/* globals */
static UInt16 dS, dE;

/* protos */
static void Convert(UInt16 *);
static Boolean EH(EventType *ev);

/*
 * Convert
 *
 * Convert a pseudo date (0 or 0xffff) to min/max.
 *
 *  -> d		Date.
 */
static void
Convert(UInt16 *d)
{
	if (*d==0)
		*d=DTDatePack(MAGICYEAR, 1, 1);
	else if (*d==0xffff)
		*d=DTDatePack(2031, 12, 31);
}

/*
 * fRangeRun
 *
 * Handle range dialog.
 *
 *  -> fLo		Filter Lo.
 *  -> fHi		Filter Hi.
 *
 * Returns true if filter values should be used, false otherwise.
 */
Boolean
fRangeRun(UInt16 *fLo, UInt16 *fHi)
{
	FormSave frm;

	dS=*fLo;
	dE=*fHi;

	Convert(&dS);
	Convert(&dE);

	UIFormPrologue(&frm, fRange, EH);
	DTSelectorSetDate(cRangeStart, selector1, dS);
	DTSelectorSetDate(cRangeEnd, selector2, dE);

	if (UIFormEpilogue(&frm, NULL, cRangeCancel)==false)
		return false;

	if (dS<=dE) {
		*fLo=dS;
		*fHi=dE;
	} else {
		*fLo=dE;
		*fHi=dS;
	}

	return true;
}

/*
 * EH
 *
 * Event handler for the alarm form.
 *
 *  -> ev          Event.
 *
 * Returns true if event is handled, false otherwise.
 */
static Boolean
EH(EventType *ev)
{
	Int16 y, m, d;

        switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cRangeStart:
			DTDateUnpack(dS, &y, &m, &d);
			if (SelectDay(selectDayByDay, &m, &d, &y, "Select Start Date")==true) {
				dS=DTDatePack(y, m, d);
				DTSelectorSetDate(cRangeStart, selector1, dS);
			}
			return true;
		case cRangeEnd:
			DTDateUnpack(dE, &y, &m, &d);
			if (SelectDay(selectDayByDay, &m, &d, &y, "Select End Date")==true) {
				dE=DTDatePack(y, m, d);
				DTSelectorSetDate(cRangeEnd, selector2, dE);
			}
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}
	return false;
}

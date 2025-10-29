/*
 * fSummary.c
 */
#include "Include.h"

/* protos */
static void Summary(void);

/* globals */
static UInt16 dS, dE, category;

/*
 * fSummaryInit
 */
Boolean
fSummaryInit(void)
{
	dS=0;
	dE=0;

	DTSelectorSetDate(cSummaryStart, selector1, dS);
	DTSelectorSetDate(cSummaryEnd, selector2, dE);

	category=prefs.categoryIdx;

	UIPopupSet(&dbData, category, categoryName, cSummaryPopup);

	UIFieldFocus(cSummarySearch);
	return true;
}

/*
 * fSummaryEH
 */
Boolean
fSummaryEH(EventType *ev)
{
	UInt16 y, m, d;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cSummaryPopup:
			CategorySelect(dbData.db, currentForm, cSummaryPopup, cSummaryList, true, &category, categoryName, CATNONEDITABLES, categoryHideEditCategory);
			return true;

		case cSummaryStart:
			if (dS)
				DTDateUnpack(dS, &y, &m, &d);
			else
				DTDateUnpack(DTToday(), &y, &m, &d);

			if (SelectDay(selectDayByDay, &m, &d, &y, "Select Start Date")==true) {
				dS=DTDatePack(y, m, d);
				DTSelectorSetDate(cSummaryStart, selector1, dS);
			}
			return true;

		case cSummaryEnd:
			if (dE)
				DTDateUnpack(dE, &y, &m, &d);
			else
				DTDateUnpack(DTToday(), &y, &m, &d);

			if (SelectDay(selectDayByDay, &m, &d, &y, "Select End Date")==true) {
				dE=DTDatePack(y, m, d);
				DTSelectorSetDate(cSummaryEnd, selector1, dE);
			}
			return true;


		case cSummaryOK:
			if ((dS && dE) || (!dS && !dE))
				Summary();
			else {
				FrmAlert(aSummaryBothDates);	
				return true;
			}
		case cSummaryCancel: /* FALL-THRU */
			FrmGotoForm(fMain);
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * Summary
 *
 * Perform summary.
 */
static void
Summary(void)
{
	Boolean searchFlag=false, dateFlag=false, shouldAdd;
	DmOpenRef db=dbData.db;
	UInt16 recIdx=0, minutes=0, dT;
	MemHandle mh=NULL;
	void *bmh=NULL;
	UInt32 hours=0;
	Char *txt;
	TTask *t;
	UInt8 tH, tM;
	Char delim[2];

	if (dS && dE) {
		dateFlag=true;
		if (dS>dE) {
			dT=dS;
			dS=dE;
			dE=dT;
		}
	}

	if ((txt=UIFieldGetText(cSummarySearch))) {
		if (StrLen(txt)>0) {
			bmh=BMHOpen(txt);
			ErrFatalDisplayIf(bmh==NULL, "(Summary) Out of memory.");
			searchFlag=true;
		}
	}

	for (EVER) {
		if (DmSeekRecordInCategory(db, &recIdx, 0, dmSeekForward, category)!=errNone)
			break;

		mh=DBGetRecord(recIdx);
		t=MemHandleLock(mh);

		shouldAdd=false;
		if (dateFlag==true) {
			if (t->date>=dS && t->date<=dE) {
				if (searchFlag==true) {
					if (BMHSearch(bmh, (UInt8 *)t+sizeof(TTask))>-1)
						shouldAdd=true;
				} else
					shouldAdd=true;
			}
		} else if (searchFlag==true) {
			if (BMHSearch(bmh, (UInt8 *)t+sizeof(TTask))>-1)
				shouldAdd=true;
		}

		dT=t->time;
		MemHandleUnlock(mh);

		if (shouldAdd==false) {
			if (dateFlag==false && searchFlag==false)
				shouldAdd=true;
		}

		if (shouldAdd==true) {
			DTTimeUnpack(dT, &tH, &tM);
			minutes+=tM;
			if (minutes>=60) {
				hours++;
				minutes-=60;
			}
			hours+=tH;
		}
		recIdx++;
	}

	BMHClose(bmh);

	StrPrintF(selector1, "%lu", hours);
	StrPrintF(selector2, "%02u", minutes);
	delim[0]=DTTimeDelimiter();
	delim[1]='\x00';
	FrmCustomAlert(aSummaryResult, selector1, delim, selector2);
}

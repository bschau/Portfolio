/*
 * fCleanUp.c
 */
#include "Include.h"

/* protos */
static void CleanUp(void);

/* globals */
static UInt16 focusBand[]={ cCleanUpDays, cCleanUpString, 0 };
static UInt16 before, category;

/*
 * fCleanUpInit
 */
Boolean
fCleanUpInit(void)
{
	before=0;
	DTSelectorSetDate(cCleanUpSelector, selector1, before);

	category=prefs.categoryIdx;

	UIPopupSet(&dbData, category, categoryName, cCleanUpPopup);

	UIFieldFocus(cCleanUpDays);
	return true;
}

/*
 * fCleanUpEH
 */
Boolean
fCleanUpEH(EventType *ev)
{
	UInt16 y, m, d;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cCleanUpPopup:
			CategorySelect(dbData.db, currentForm, cCleanUpPopup, cCleanUpList, true, &category, categoryName, CATNONEDITABLES, categoryHideEditCategory);
			return true;

		case cCleanUpSelector:
			if (before)
				DTDateUnpack(before, &y, &m, &d);
			else
				DTDateUnpack(DTToday(), &y, &m, &d);

			if (SelectDay(selectDayByDay, &m, &d, &y, "Select Before Date")==true) {
				before=DTDatePack(y, m, d);
				DTSelectorSetDate(cCleanUpSelector, selector1, before);
			}
			return true;

		case cCleanUpOK:
			if (FrmAlert(aCleanUpOK)==0)
				CleanUp();
			else
				return true;

		case cCleanUpCancel:	/* FALL-THRU */
			FrmGotoForm(fMain);
			return true;
		}
		break;

	case keyDownEvent:
		UIFieldRefocus(focusBand, ev->data.keyDown.chr);
	default:
		break;
	}

	return false;
}

/*
 * CleanUp
 *
 * Perform clean up.
 */
static void
CleanUp(void)
{
	Boolean olderFlag=false, searchFlag=false, shouldDelete;
	DmOpenRef db=dbData.db;
	Int32 older=0;
	UInt16 deleteCnt=0, recIdx=0, olderDate=DTToday();
	MemHandle mh=NULL;
	void *bmh=NULL;
	Char *txt;
	TTask *t;

	if ((txt=UIFieldGetText(cCleanUpDays))) {
		if (StrLen(txt)>0) {
			older=StrAToI(txt);
			DateAdjust((DateType *)&olderDate, -older);
			olderFlag=true;
		}
	}

	if ((txt=UIFieldGetText(cCleanUpString))) {
		if (StrLen(txt)>0) {
			bmh=BMHOpen(txt);
			ErrFatalDisplayIf(bmh==NULL, "(CleanUp) Out of memory.");
			searchFlag=true;
		}
	}

	for (EVER) {
		if (DmSeekRecordInCategory(db, &recIdx, 0, dmSeekForward, category)!=errNone)
			break;

		mh=DBGetRecord(recIdx);
		t=MemHandleLock(mh);

		shouldDelete=false;
		if (before) {
			if (t->date<before)
				shouldDelete=true;
		}

		if (shouldDelete==false) {
			if (olderFlag==true) {
				if (t->date<olderDate)
					shouldDelete=true;
			}

			if (shouldDelete==false && searchFlag==true) {
				if (BMHSearch(bmh, (UInt8 *)t+sizeof(TTask))>-1)
					shouldDelete=true;
			}
		}

		MemHandleUnlock(mh);

		if (shouldDelete==false) {
			if (before==0 && olderFlag==false && searchFlag==false)
				shouldDelete=true;
		}

		if (shouldDelete==true) {
			DmDeleteRecord(db, recIdx);
			deleteCnt++;
		} else
			recIdx++;
	}

	BMHClose(bmh);

	StrPrintF(selector1, "%u", deleteCnt);
	FrmCustomAlert(aCleanUpResult, selector1, (deleteCnt==1) ? " " : "s ", NULL);
}

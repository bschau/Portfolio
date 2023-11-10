/*
**	viewform.c
**
**	Code to drive the view form.
*/
#include "eventor.h"

/*
**	ViewFormInit
**
**	Initialize the view form.
**
**	Returns true if successful, false otherwise.
*/
Boolean
ViewFormInit(void)
{
	MemHandle mh, sh, shp;
	Int16 year, month, day;
	Char catName[dmCategoryLength+2], fieldText[64];
	UInt16 date, attrs;
	TEvent *e;
	DateTimeType dtt;
	Char *p;

	mh=DBGetRecord(&dbData, viewIdx);
	e=MemHandleLock(mh);

	FrmDrawForm(currentForm);
	TimSecondsToDateTime(TimGetSeconds(), &dtt);

	UIFieldSetText(fViewEvent, e->event);
	UtilsUnpackDate(e->date, &year, &month, &day);
	UtilsFormatDate(fieldText, year+MAGICYEAR, month, day);
	for (p=fieldText; *p; p++);

	StrPrintF(p, "   (%u)", dtt.year-(year+MAGICYEAR));
	UIFieldSetText(fViewDate, fieldText);

	DmRecordInfo(dbData.db, viewIdx, &attrs, NULL, NULL);
	CategoryGetName(dbData.db, attrs&dmRecAttrCategoryMask, catName);
	UIFieldSetText(fViewCategory, catName);

	if ((e->flags&FlgRemindMe)==FlgRemindMe) {
		date=e->flags&MaskFLGDays;
		if (date==0) {
			sh=UtilsLockString(strOnTheDay);
			StrPrintF(fieldText, MemHandleLock(sh));
			UtilsUnlockString(sh);
		} else {
			attrs=e->flags&MaskFLGDays;
			sh=UtilsLockString(strDaysBefore);
			shp=UtilsLockString(strDaysBeforeP);
			p=MemHandleLock(shp);
			StrPrintF(fieldText, MemHandleLock(sh), attrs, (attrs==1) ? "" : p);
			UtilsUnlockString(shp);
			UtilsUnlockString(sh);
			if ((e->flags&FlgRepeat)==FlgRepeat) {
				StrCat(fieldText, " - ");
				sh=UtilsLockString(strRepeating);
				StrCat(fieldText, MemHandleLock(sh));
				UtilsUnlockString(sh);
			}

			StrCat(fieldText, ".");
		}

		UIFieldSetText(fViewRemind, fieldText);
	} else {
		sh=UtilsLockString(strNever);
		UIFieldSetText(fViewRemind, MemHandleLock(sh));
		UtilsUnlockString(sh);
	}

	MemHandleUnlock(mh);
	return true;
}

/*
**	ViewFormEventHandler
**
**	Event handler for view form.
**
**	 -> ev		Event.
**
**	Returns true if event is handled, false otherwise.
*/
Boolean
ViewFormEventHandler(EventPtr ev)
{
	EventType stopEvent;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case fViewDone:
			if (viewReturn==true) {
				MemSet(&stopEvent, sizeof(EventType), 0);
				stopEvent.eType=appStopEvent;
				EvtAddEventToQueue(&stopEvent);
			} else
				FrmGotoForm(fMain);

			return true;

		case fViewEdit:
			recordIdx=viewIdx;
			FrmGotoForm(fEdit);
			return true;

		case fViewDelete:
			if (FrmAlert(aDelete)==0) {
				DmDeleteRecord(dbData.db, viewIdx);
				FrmGotoForm(fMain);
			}
			return true;
		}
		break;

	default:
		break;
	}

	return false;
}

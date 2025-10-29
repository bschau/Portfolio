/*
**	editform.c
**
**	Code to drive the edit form.
*/
#include "eventor.h"

/* protos */
static void AdjustIfFakeDate(Int16 *, Int16 *, Int16 *);

/* globals */
static MemHandle newEventMH, editEventMH;
static Char *titleNewEvent, *titleEditEvent;
static Char selectorDate[32];
static UInt16 backupCategoryIdx;
static Int16 year, month, day;

/*
**	EditOpen
**
**	One time initialization.
*/
void
EditOpen(void)
{
	newEventMH=UtilsLockString(strNewEvent);
	titleNewEvent=MemHandleLock(newEventMH);
	editEventMH=UtilsLockString(strEditEvent);
	titleEditEvent=MemHandleLock(editEventMH);
}

/*
**	EditClose
**
**	Close.
*/
void
EditClose(void)
{
	UtilsUnlockString(newEventMH);
	UtilsUnlockString(editEventMH);
}

/*
**	AdjustIfFakeDate
**
**	Set part of todays date if any of year, month, day is zero.
**
**	<-> year	Year.
**	<-> month	Month.
**	<-> day		Day (surprise, surprise :o)
*/
static void
AdjustIfFakeDate(Int16 *year, Int16 *month, Int16 *day)
{
	UInt32 seconds=TimGetSeconds();
	DateTimeType dt;

	TimSecondsToDateTime(seconds, &dt);

	if (*year<MAGICYEAR || *year>2031)
		*year=dt.year;

	if (*month<1 || *month>12)
		*month=dt.month;

	if (*day<1 || *day>31)
		*day=dt.day;
}

/*
**	EditFormInit
**
**	Initialize the edit form.
**
**	Returns true if successful, false otherwise.
*/
Boolean
EditFormInit(void)
{
	UInt16 attrs;
	Char buffer[16];
	MemHandle mh;
	TEvent *e;

	backupCategoryIdx=categoryIdx;
	FrmDrawForm(currentForm);

	if (recordIdx==dmMaxRecordIndex) {
		if (categoryIdx==dmAllCategories)
			categoryIdx=dmUnfiledCategory;

		FrmSetTitle(currentForm, titleNewEvent);
		mh=UtilsLockString(strNotSet);
		StrCopy(selectorDate, MemHandleLock(mh));
		UtilsUnlockString(mh);
		year=0;
		month=0;
		day=0;
	} else {
		mh=DBGetRecord(&dbData, recordIdx);
		e=MemHandleLock(mh);

		UtilsUnpackDate(e->date, &year, &month, &day);
		year+=MAGICYEAR;
		UIFieldSetText(fEditEvent, e->event);

		UtilsFormatDate(selectorDate, year, month, day);

		if ((e->flags&FlgRemindMe)==FlgRemindMe) {
			CtlSetValue(UIFormGetObject(fEditRemindMe), 1);
			UIFieldSetText(fEditRemindDays, StrIToA(buffer, (Int32)(e->flags&MaskFLGDays)));
			if ((e->flags&FlgRepeat)==FlgRepeat)
				CtlSetValue(UIFormGetObject(fEditRepeat), 1);
		} else {
			CtlSetValue(UIFormGetObject(fEditRemindMe), 0);
			CtlSetValue(UIFormGetObject(fEditRepeat), 0);
			UIFieldSetText(fEditRemindDays, "");
		}

		MemHandleUnlock(mh);
		DmRecordInfo(dbData.db, recordIdx, &attrs, NULL, NULL);
		categoryIdx=attrs&dmRecAttrCategoryMask;

		FrmSetTitle(currentForm, titleEditEvent);
	}

	CategoryGetName(dbData.db, categoryIdx, categoryName);
	CategorySetTriggerLabel(UIFormGetObject(fEditCategoryPopup), categoryName);

	CtlSetLabel(UIFormGetObject(fEditDateSelector), selectorDate);
	UIFieldFocus(fEditEvent);
	return true;
}

/*
**	EditFormEventHandler
**
**	Event handler for edit form.
**
**	 -> ev		Event.
**
**	Returns true if event is handled, false otherwise.
*/
Boolean
EditFormEventHandler(EventPtr ev)
{
	MemHandle mh;
	Char *r;
	TEvent e;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		return true;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case fEditCategoryPopup:
			CategorySelect(dbData.db, currentForm, fEditCategoryPopup, fEditCategoryList, false, &categoryIdx, categoryName, CATNONEDITABLES, 0);
			return true;

		case fEditOk:
			MemSet(&e, sizeof(TEvent), 0);
			if ((mh=UtilsTidyString(UIFieldGetText(fEditEvent)))==NULL) {
				FrmAlert(aEmptyEvent);
				return true;
			}
			StrNCopy(e.event, MemHandleLock(mh), EventLength);
			MemHandleFree(mh);

			if (year==0 && month==0 && day==0) 
				FrmAlert(aInvalidDate);
			else {
				if ((r=UIFieldGetText(fEditRemindDays)))
					e.flags=(UInt16)StrAToI(r);

				if ((CtlGetValue(UIFormGetObject(fEditRemindMe))))
					e.flags|=FlgRemindMe;

				if ((CtlGetValue(UIFormGetObject(fEditRepeat))))
					e.flags|=FlgRepeat;
				
				e.date=UtilsPackDate(year, month, day);
				DBSetRecord(&dbData, categoryIdx, (UInt8 *)&e, sizeof(TEvent), recordIdx);
				FrmGotoForm(fMain);
			} 
			return true;

		case fEditCancel:
			categoryIdx=backupCategoryIdx;
			FrmGotoForm(fMain);
			return true;

		case fEditDateSelector:
			AdjustIfFakeDate(&year, &month, &day);
			mh=UtilsLockString(strSetDate);
			if (SelectDay(selectDayByDay, &month, &day, &year, MemHandleLock(mh))==true) {
				UtilsFormatDate(selectorDate, year, month, day);
				CtlSetLabel(UIFormGetObject(fEditDateSelector), selectorDate);
			}
			UtilsUnlockString(mh);
			return true;
		}
		break;

	default:
		break;
	}

	return false;
}

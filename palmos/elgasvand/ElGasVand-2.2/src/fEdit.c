/*
 * fEdit.c
 */
#include "Include.h"

/* protos */
static Boolean EH(EventType *ev);
static void SetSelector(void);
static void SetEGW(void);

/* globals */
static Char selDate[32], selE[16], selG[16], selW[16];
static UInt32 el, gas, water;
static Boolean update;
static DateTimeType dt;

/*
 * DateSort
 */
Int16
DateSort(void *r1, void *r2, Int16 u1, SortRecordInfoType *u2, SortRecordInfoType *u3, MemHandle u4)
{
	if ((((Record *)r1)->date)>(((Record *)r2)->date))
		return -1;
	else if ((((Record *)r1)->date)<(((Record *)r2)->date))
		return 1;

	return 0;
}

/*
 * fEditRun
 *
 * Setup for edit.
 *
 *  -> rIdx		Record Index.
 *
 * Returns true if OK was pressed, false otherwise.
 */
Boolean
fEditRun(UInt16 rIdx)
{
	FormSave frm;

	recordIdx=rIdx;
	update=false;

	UIFormPrologue(&frm, fEdit, EH);

	if (recordIdx==dmMaxRecordIndex) {
		UInt32 secs=TimGetSeconds();

		TimSecondsToDateTime(secs, &dt);
		el=0;
		gas=0;
		water=0;
	} else {
		MemHandle mh;
		Record *r;

		mh=DBGetRecord(&dbData, recordIdx);
		r=MemHandleLock(mh);

		dt.year=(((r->date)>>9)&0x7f)+1904;
		dt.month=((r->date)>>5)&0x0f;
		dt.day=(r->date)&0x1f;
		SetSelector();

		el=r->el;
		gas=r->gas;
		water=r->water;

		if (r->flags&RFlgNewEMeter)
			CtlSetValue(UIObjectGet(cEditChkE), 1);

		if (r->flags&RFlgNewGMeter)
			CtlSetValue(UIObjectGet(cEditChkG), 1);

		if (r->flags&RFlgNewWMeter)
			CtlSetValue(UIObjectGet(cEditChkW), 1);

		MemHandleUnlock(mh);
		UIObjectShow(cEditDelete);
	}

	SetSelector();
	SetEGW();
	UIFormEpilogue(&frm, NULL, 0);
	return update;
}

/*
 * EH
 */
static Boolean
EH(EventType *ev)
{
	MemHandle mh;

	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cEditOK:
			if (dt.year<1904)
				record.date=0;
			else
				record.date=(dt.year-1904)<<9;

			record.date|=(dt.month<<5);
			record.date|=dt.day;

			record.el=el;
			record.gas=gas;
			record.water=water;

			if ((CtlGetValue(UIObjectGet(cEditChkE))))
				record.flags=RFlgNewEMeter;
			else
				record.flags=0;

			if ((CtlGetValue(UIObjectGet(cEditChkG))))
				record.flags|=RFlgNewGMeter;

			if ((CtlGetValue(UIObjectGet(cEditChkW))))
				record.flags|=RFlgNewWMeter;

			DBSetRecord(&dbData, recordIdx, dmUnfiledCategory, &record, sizeof(Record), DateSort);
			update=true;
			break;

		case cEditSelector:
			mh=DmGetResource(strRsc, strSelectDay);
			ErrFatalDisplayIf(mh==NULL, "(EditEH) String not found.");

			if (SelectDay(selectDayByDay, &dt.month, &dt.day, &dt.year, MemHandleLock(mh))==true)
				SetSelector();

			MemHandleUnlock(mh);
			DmReleaseResource(mh);
			return true;

		case cEditSelE:
			if (fKeyboardRun(&el, strMeterE)==true)
				SetEGW();
			return true;

		case cEditSelG:
			if (fKeyboardRun(&gas, strMeterG)==true)
				SetEGW();
			return true;

		case cEditSelW:
			if (fKeyboardRun(&water, strMeterW)==true)
				SetEGW();
			return true;

		case cEditDelete:
			if (FrmAlert(aDelete)==0) {
				DmDeleteRecord(dbData.db, recordIdx);
				FrmGotoForm(fMain);
			}
			break;
		}
		break;

	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * SetSelector
 *
 * Set the text in the selector.
 */
static void
SetSelector(void)
{
	PMFormatDate(selDate, dt.year, dt.month, dt.day);
	CtlSetLabel(UIObjectGet(cEditSelector), selDate);
}

/*
 * SetEGW
 *
 * Set El, Gas, Water selectors.
 */
static void
SetEGW(void)
{
	if (el) {
		StrPrintF(selE, "%lu", el);
		CtlSetLabel(UIObjectGet(cEditSelE), selE);
	} else
		CtlSetLabel(UIObjectGet(cEditSelE), "-----");

	if (gas) {
		StrPrintF(selG, "%lu", gas);
		CtlSetLabel(UIObjectGet(cEditSelG), selG);
	} else
		CtlSetLabel(UIObjectGet(cEditSelG), "-----");

	if (water) {
		StrPrintF(selW, "%lu", water);
		CtlSetLabel(UIObjectGet(cEditSelW), selW);
	} else
		CtlSetLabel(UIObjectGet(cEditSelW), "-----");
}

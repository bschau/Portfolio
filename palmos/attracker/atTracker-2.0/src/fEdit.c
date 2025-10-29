/*
 *	editform.c
 */
#include "Include.h"

/* protos */
static Boolean DeleteRecord(void);
static void ExitForm(UInt16);
static void SetSaveRecord(Char *);

/* globals */
static UInt16 backupCategoryIdx, recordIdx, time, date;
static Boolean saveFlag;

/*
 * fEditInit
 */
Boolean
fEditInit(void)
{
	UInt16 len, attrs;
	Char evBuf[maxCharBytes];
	TTask *t;
	Char *p;
	MemHandle mh;

	backupCategoryIdx=prefs.categoryIdx;
	FrmDrawForm(currentForm);

	saveFlag=false;
	recordIdx=(UInt16)(editCmd&0xffff);
	if ((editCmd&EditMaskNew)==EditMaskNew) {
		saveFlag=true;
		if (prefs.categoryIdx==dmAllCategories)
			prefs.categoryIdx=dmUnfiledCategory;

		date=DTToday();
		time=prefs.defaultTime;
		if ((editCmd&EditMaskEvt)==EditMaskEvt) {
			len=TxtGlueSetNextChar(evBuf, 0, editEvent.data.keyDown.chr);
			if (TxtGlueTransliterate(evBuf, len, evBuf, &len, translitOpUpperCase)==0)
				TxtGlueGetNextChar(evBuf, 0, &editEvent.data.keyDown.chr);
		}
	} else if ((editCmd&EditMaskPrf)==EditMaskPrf) {
		fEditSaveRecord(true, &mh, &date, &time, &prefs.categoryIdx);

		UIFieldSetText(cEditTask, MemHandleLock(mh));
		MemHandleFree(mh);

		PrefSetAppPreferences((UInt32)CRID, PrfSaveRecord, APPVER, NULL, 0, false);
		PrefSetAppPreferences((UInt32)CRID, PrfSaveRecordTaskText, APPVER, NULL, 0, false);
	} else if ((editCmd&EditMaskIdx)==EditMaskIdx) {
		mh=DBGetRecord(recordIdx);
		t=MemHandleLock(mh);

		date=t->date;
		time=t->time;
		p=(UInt8 *)t+sizeof(TTask);
		if (*p)
			UIFieldSetText(cEditTask, p);

		MemHandleUnlock(mh);

		DmRecordInfo(dbData.db, recordIdx, &attrs, NULL, NULL);
		prefs.categoryIdx=attrs&dmRecAttrCategoryMask;

		saveFlag=true;
		UIObjectShow(cEditDelete);
	}

	UIPopupSet(&dbData, prefs.categoryIdx, categoryName, cEditCategoryPopup);
	DTSelectorSetDate(cEditDate, selector1, date);
	DTSelectorSetTime(cEditTime, selector2, time);
	UIFieldFocus(cEditTask);
	UIFieldUpdateScrollBar(cEditTask, cEditTaskScrollbar);
	QuickTextListInit(cEditQuickTextList);
	return true;
}

/*
 * fEditEH
 */
Boolean
fEditEH(EventType *ev)
{
	Int16 y, m, d;
	UInt8 hours, minutes;
	Char *taskTxt;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		if ((editCmd&EditMaskEvt)==EditMaskEvt) {
			EvtAddEventToQueue(&editEvent);
			editCmd&=~EditMaskEvt;
		}
		return true;

	case frmSaveEvent:
		if (saveFlag==true) {
			if ((taskTxt=UIFieldGetText(cEditTask))!=NULL) {
				if (StringSkipLeading(taskTxt)!=NULL) {
					if (recordIdx==dmMaxRecordIndex)
						SetSaveRecord(taskTxt);
					else
						DBSetRecord(recordIdx, date, taskTxt, time, prefs.categoryIdx);
				}
			}
		}
		break;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cEditCategoryPopup:
			CategorySelect(dbData.db, currentForm, cEditCategoryPopup, cEditCategoryList, false, &prefs.categoryIdx, categoryName, CATNONEDITABLES, 0);
			return true;

		case cEditOK:
			if ((taskTxt=UIFieldGetText(cEditTask))!=NULL) {
				if (StringSkipLeading(taskTxt)==NULL) {
					if (recordIdx!=dmMaxRecordIndex) {
						if (DeleteRecord()==false)
							break;
					}
				} else {
					DBSetRecord(recordIdx, date, taskTxt, time, prefs.categoryIdx);
					DTTimeUnpack(time, &hours, &minutes);
					TimeHistoryAdd(hours, minutes);
				}
			}

			saveFlag=true;
			ExitForm(fMain);
			return true;

		case cEditCancel:
			prefs.categoryIdx=backupCategoryIdx;
			ExitForm(fMain);
			return true;

		case cEditDelete:
			if (DeleteRecord()==false)
				break;

			ExitForm(fMain);
			return true;

		case cEditDate:
			DTDateUnpack(date, &y, &m, &d);
			if (SelectDay(selectDayByDay, &m, &d, &y, "Select Date")==true) {
				date=DTDatePack(y, m, d);
				DTSelectorSetDate(cEditDate, selector1, date);
			}
			return true;

		case cEditTime:
			DTTimeUnpack(time, &hours, &minutes);
			if (TimeSelector(&hours, &minutes, "Set Duration")==true) {
				time=DTTimePack(hours, minutes);
				DTSelectorSetTime(cEditTime, selector2, time);
			}
			return true;
		}
		break;

	case popSelectEvent:
		if (ev->data.popSelect.selection<(QuickTextGetCount()-1)) {
			if ((taskTxt=QuickTextIdxToText(ev->data.popSelect.selection))==NULL)
				return true;

			UIFieldSetText(cEditTask, taskTxt);
			UIFieldFocus(cEditTask);
		} else {
			QuickTextManager();
			QuickTextListInit(cEditQuickTextList);
		}

		return true;

	case keyDownEvent:
		UIFieldScrollBarKeyHandler(ev, cEditTask, cEditTaskScrollbar);

	default:
		UIFieldScrollBarHandler(ev, cEditTask, cEditTaskScrollbar);
		break;
	}

	return false;
}

/*
 * DeleteRecord
 *
 * Delete record.
 *
 * Returns true if deleted, false otherwise.
 */
static Boolean
DeleteRecord(void)
{
	if (FrmAlert(aDeleteRecord)==0) {
		DmDeleteRecord(dbData.db, recordIdx);
		return true;
	}

	return false;
}

/*
 * ExitForm
 *
 * Return to callee.
 *
 *  -> fId		Desired form.
 */
static void
ExitForm(UInt16 fId)
{
	EventType ev;

	if ((editCmd&EditMaskAppStop)==EditMaskAppStop) {
		MemSet(&ev, sizeof(EventType), 0);
		ev.eType=appStopEvent;
		EvtAddEventToQueue(&ev);
	} else
		FrmGotoForm(fMain);
}

/*
 * SetSaveRecord
 *
 * Save the current "record" to preferences.
 *
 *  -> txt		Task text.
 */
static void
SetSaveRecord(Char *txt)
{
	UInt32 tLen=StrLen(txt);
	SaveRecordPref s;

	PrefSetAppPreferences((UInt32)CRID, PrfSaveRecordTaskText, APPVER, txt, tLen, false);

	s.categoryIdx=prefs.categoryIdx;
	s.date=date;
	s.time=time;

	PrefSetAppPreferences((UInt32)CRID, PrfSaveRecord, APPVER, &s, sizeof(SaveRecordPref), false);
}

/*
 * fEditSaveRecord
 *
 * Locate the Save Record if any.
 *
 *  -> retrieve		True = yes, false = no.
 * <-  mh		MemHandle to task text.
 * <-  d		Date.
 * <-  t		Time.
 * <-  c		Category.
 *
 * Returns true if record gotten, false otherwise.
 */
Boolean
fEditSaveRecord(Boolean retrieve, MemHandle *mh, UInt16 *d, UInt16 *t, UInt16 *c)
{
	UInt16 len=sizeof(SaveRecordPref);
	Int16 retPrf;
	SaveRecordPref s;
	MemHandle th;
	Char *p;

	retPrf=PrefGetAppPreferences((UInt32)CRID, PrfSaveRecord, &s, &len, false);
	if (retPrf==noPreferenceFound || len!=sizeof(SaveRecordPref))
		return false;

	len=0;
	retPrf=PrefGetAppPreferences((UInt32)CRID, PrfSaveRecordTaskText, NULL, &len, false);
	if (retPrf==noPreferenceFound)
		return false;

	if (retrieve) {
		if ((th=MemHandleNew(len+1))==NULL)
			return false;

		p=MemHandleLock(th);
		PrefGetAppPreferences((UInt32)CRID, PrfSaveRecordTaskText, p, &len, false);
		p[len]='\x00';
		MemHandleUnlock(th);

		*mh=th;
		*d=s.date;
		*t=s.time;
		*c=s.categoryIdx;
	}

	return true;
}

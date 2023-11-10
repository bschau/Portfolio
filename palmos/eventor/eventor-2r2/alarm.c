/*
**	alarm.c
**
**	Code to drive the Alarm forms.
*/
#include "eventor.h"

/* macros */
#define ONEDAY (60*60*24L)
enum { EventNoScroll=0, EventPrev, EventNext };

/* structs */
typedef struct {
	UInt32 time;
	UInt16 recIdx;
} TimeIdx;

/* protos */
static void SetFieldText(FormPtr, UInt16, Char *);
static Int16 RecordCompareFunction(void *, void *, Int16, SortRecordInfoPtr, SortRecordInfoPtr, MemHandle);
static void AddEventDate(TimeIdx *, UInt16, UInt32);
static Boolean EventHandler(EventPtr);
static void RethinkControls(void);
static void BlindControls(UInt16, UInt32);
static void ShowEvent(UInt16);

/*
**	SetFieldText
**
**	Update field with copy of buffer.  Any previous text is discarded.
**
**	 -> frm		Form.
**	 -> id		Field id in current form.
**	 -> buffer	Buffer to get text from.
*/
void
SetFieldText(FormPtr frm, UInt16 id, Char *buffer)
{
	FieldType *fld=FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, id));
	UInt16 tLen=StrLen(buffer);
	MemHandle mh, oldH;
	Char *tp;

	mh=MemHandleNew(tLen+1);
	ErrFatalDisplayIf(mh==NULL, "(SetFieldText) Out of memory");

	tp=MemHandleLock(mh);
	MemMove(tp, buffer, tLen);
	tp[tLen]='\x00';
	MemHandleUnlock(mh);

	oldH=FldGetTextHandle(fld);
	FldSetTextHandle(fld, mh);

	if (oldH)
		MemHandleFree(oldH);

	FldDrawField(fld);
}

/*
**	RecordCompareFunction
**
**	Compare function for SysQSort.
*/
static Int16
RecordCompareFunction(void *r1, void *r2, Int16 unused1, SortRecordInfoPtr unused2, SortRecordInfoPtr unused3, MemHandle unused4)
{
	TimeIdx *t1=r1, *t2=r2;

	if (t1->time==t2->time)
		return 0;

	if ((t1->time)<(t2->time))
		return -1;

	return 1;
}

/*
**	AddDate
**
**	Add a date/event to the list of due date events.
**
**	 -> ti		Time Index pointer.
**	 -> recIdx	Record Index.
**	 -> secs	Seconds to event (0 = today).
*/
static void
AddEventDate(TimeIdx *ti, UInt16 recIdx, UInt32 secs)
{
	UInt16 idx=ti[0].recIdx;

	idx++;
	ti[idx].time=secs;
	ti[idx].recIdx=recIdx;
	ti[0].recIdx=idx;
}

/*
**	AlarmSetNextAlarm
**
**	Set alarm to fire off on the next day at 0:00.00 + what's in prefs :o)
*/
void
AlarmSetNextAlarm(void)
{
	UInt16 meCard, nr;
	UInt32 now=TimGetSeconds(), seconds;
	LocalID meId;
	DateTimeType dtt;
	Preferences lPrefs;
	DB dbo;

	SysCurAppDatabase(&meCard, &meId);

	DBInit(&dbo, DBNAME);
	if (DBOpen(&dbo, dmModeReadOnly, NULL)==false) {
		/* cancel alarms */
		AlmSetAlarm(meCard, meId, 0, 0, true);
		return;
	}

	nr=DmNumRecordsInCategory(dbo.db, dmAllCategories);
	DBClose(&dbo);
	if (nr==0) {
		/* cancel alarms */
		AlmSetAlarm(meCard, meId, 0, 0, true);
		return;
	}

	TimSecondsToDateTime(now, &dtt);

	dtt.second=0;
	if (PreferencesGet(&lPrefs)==false) {
		dtt.minute=0;
		dtt.hour=0;
	} else {
		dtt.minute=lPrefs.alarmMinute;
		dtt.hour=lPrefs.alarmHour;
	}

	seconds=TimDateTimeToSeconds(&dtt);
	if (seconds<now) {
		dtt.day++;	/* ... the next day ... */

		if (dtt.day>DaysInMonth(dtt.month, dtt.year)) {
			dtt.day=1;
			dtt.month++;
			if (dtt.month>12) {
				dtt.month=1;
				dtt.year++;
			}
		}
		dtt.weekDay=DayOfWeek(dtt.month, dtt.day, dtt.year);
		seconds=TimDateTimeToSeconds(&dtt);
	}

	AlmSetAlarm(meCard, meId, 0, seconds, true);
}

/*
**	AlarmShowEvents
**
**	 -> soundAlarm	Sound Alarm bell (if not overridden by prefs).
**
**	Show events (if any).
*/
void
AlarmShowEvents(Boolean soundAlarm)
{
	UInt32 now=TimGetSeconds(), eventSecs, eventLower;
	UInt16 recIdx=0, numRecs;
	MemHandle mh, recHandles;
	DateTimeType dtt;
	Int16 year, month, day, exactMonth, exactDay;
	TimeIdx *ti;
	FormPtr oFrm, frm;
	Preferences localPrefs;
	TEvent *e;
	DB dbo;

	/* Find no.# of records */
	DBInit(&dbo, DBNAME);
	if (DBOpen(&dbo, dmModeReadOnly, NULL)==false)
		return;

	if ((numRecs=DmNumRecordsInCategory(dbo.db, dmAllCategories))==0) {
		DBClose(&dbo);
		return;
	}

	if ((recHandles=MemHandleNew((numRecs+1)*sizeof(TimeIdx)))==NULL) {
		DBClose(&dbo);
		return;
	}

	ti=MemHandleLock(recHandles);
	ti[0].recIdx=0;
	ti[0].time=0;

	TimSecondsToDateTime(now, &dtt);
	dtt.second=0;
	dtt.minute=0;
	dtt.hour=0;

	exactMonth=dtt.month;
	exactDay=dtt.day;

	/* build list */
	while (numRecs>0) {
		if ((mh=DmQueryNextInCategory(dbo.db, &recIdx, dmAllCategories))!=NULL) {
			e=MemHandleLock(mh);

			if ((e->flags&FlgRemindMe)==FlgRemindMe) {
				UtilsUnpackDate(e->date, &year, &month, &day);

				if (month==exactMonth && day==exactDay)
					AddEventDate(ti, recIdx, 0);
				else {
					if ((e->flags&FlgRepeat)==FlgRepeat) {
						dtt.day=day;
						dtt.month=month;
						dtt.weekDay=DayOfWeek(dtt.month, dtt.day, dtt.year);
						eventSecs=TimDateTimeToSeconds(&dtt);
						eventLower=eventSecs-(((UInt32)e->flags&MaskFLGDays)*ONEDAY);
						if (now>=eventLower && now<=eventSecs)
							AddEventDate(ti, recIdx, eventSecs-now);
					}
				}
			}
			MemHandleUnlock(mh);
		}
		recIdx++;
		numRecs--;
	}
	DBClose(&dbo);

	if (ti[0].recIdx==0) {
		MemHandleFree(recHandles);
		return;
	}

	/* sort list */
	SysQSort(&ti[1].time, ti[0].recIdx, sizeof(TimeIdx), (CmpFuncPtr)RecordCompareFunction, 0);

	MemHandleUnlock(recHandles);

	if (FtrSet(CRID, FtrRecords, (UInt32)recHandles)!=0) {
		MemHandleFree(recHandles);
		return;
	}

	oFrm=FrmGetActiveForm();
	if ((frm=FrmInitForm(fAlarm))) {
		FrmSetEventHandler(frm, (FormEventHandlerPtr)EventHandler);
		FrmSetActiveForm(frm);
		FrmDrawForm(frm);

		RethinkControls();
		ShowEvent(EventNoScroll);

		if (soundAlarm==true && PreferencesGet(&localPrefs)==true) {
			if (localPrefs.flags.soundAlarm==1)
				SndPlaySystemSound(sndAlarm);
		}

		FrmDoDialog(frm);
		FrmEraseForm(frm);
		FrmDeleteForm(frm);
		FrmSetActiveForm(oFrm);
	}

	FtrUnregister(CRID, FtrRecords);
	MemHandleFree(recHandles);
}

/*
**	EventHandler
**
**	Event handler for the alarm form.
**
**	 -> ev		Event.
**
**	Returns true if event is handled, false otherwise.
*/
static Boolean
EventHandler(EventPtr ev)
{
	switch (ev->eType) {
	case keyDownEvent:
		switch (ev->data.keyDown.chr) {
		case pageUpChr:
			ShowEvent(EventPrev);
			return true;
		case pageDownChr:
			ShowEvent(EventNext);
			return true;
		}
		break;

	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case fAlarmDone:
			break;
		case fAlarmPrev:
			ShowEvent(EventPrev);
			return true;
		case fAlarmNext:
			ShowEvent(EventNext);
			return true;
		}
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
**	RethinkControls
**
**	Rethink the controls based on no. of records and current index.
*/
static void
RethinkControls(void)
{
	MemHandle recs;
	TimeIdx *ti;

	if (FtrGet(CRID, FtrRecords, (UInt32 *)&recs)==0) {
		ti=MemHandleLock(recs);
		BlindControls(ti[0].recIdx, ti[0].time);
		MemHandleUnlock(recs);
	}
}

/*
**	BlindControls
**
**	Hide or show controls dependent on position.
**
**	 -> recCnt	Number of records.
**	 -> pos		Position.
*/
static void
BlindControls(UInt16 recCnt, UInt32 pos)
{
	FormPtr frm=FrmGetActiveForm();

	FrmHideObject(frm, FrmGetObjectIndex(frm, fAlarmPrev));
	FrmHideObject(frm, FrmGetObjectIndex(frm, fAlarmNext));
	if (recCnt>1) {
		if (pos>0)
			FrmShowObject(frm, FrmGetObjectIndex(frm, fAlarmPrev));

		if (pos<(recCnt-1))
			FrmShowObject(frm, FrmGetObjectIndex(frm, fAlarmNext));
	}
}

/*
**	ShowEvent
**
**	Show an event.
**
**	 -> dir		Direction to scroll from current event.
*/
static void
ShowEvent(UInt16 dir)
{
	FormPtr frm=FrmGetActiveForm();
	Char catName[dmCategoryLength], buffer[32];
	MemHandle recs, rh;
	TimeIdx *ti;
	UInt16 recIdx, attrs, year, month, day;
	UInt32 time, days;
	DB dbo;
	TEvent *e;
	DateTimeType dtt;
	Char *p;

	if (FtrGet(CRID, FtrRecords, (UInt32 *)&recs)!=0)
		return;

	DBInit(&dbo, DBNAME);
	if (DBOpen(&dbo, dmModeReadOnly, NULL)==false)
		return;

	TimSecondsToDateTime(TimGetSeconds(), &dtt);

	ti=MemHandleLock(recs);
	switch (dir) {
	case EventPrev:
		if (ti[0].time>0) {
			ti[0].time--;
			BlindControls(ti[0].recIdx, ti[0].time);
		}
		break;
	case EventNext:
		if (ti[0].time<ti[0].recIdx) {
			ti[0].time++;
			BlindControls(ti[0].recIdx, ti[0].time);
		}
	default:	/* FALL-THRU */
		break;
	}

	recIdx=ti[1+ti[0].time].recIdx;
	time=(ti[1+ti[0].time].time);
	MemHandleUnlock(recs);

	if ((rh=DmQueryRecord(dbo.db, recIdx))!=NULL) {
		DmRecordInfo(dbo.db, recIdx, &attrs, NULL, NULL);
		CategoryGetName(dbo.db, attrs&dmRecAttrCategoryMask, catName);
		SetFieldText(frm, fAlarmCategory, catName);

		e=MemHandleLock(rh);
		SetFieldText(frm, fAlarmEvent, e->event);

		UtilsUnpackDate(e->date, &year, &month, &day);
		UtilsFormatDate(buffer, year+MAGICYEAR, month, day);

		for (p=buffer; *p; p++);

		StrPrintF(p, "   (%u)", dtt.year-(year+MAGICYEAR));
		SetFieldText(frm, fAlarmEventDate, buffer);

		MemHandleUnlock(rh);
		if (time) {
			days=time/ONEDAY;
			if ((time%ONEDAY)!=0)
				days++;

			if (days==1) {
				rh=UtilsLockString(strTomorrow);
				StrCopy(buffer, MemHandleLock(rh));
			} else {
				rh=UtilsLockString(strInDays);
				StrPrintF(buffer, MemHandleLock(rh), days);
			}
		} else {
			rh=UtilsLockString(strToday);
			StrCopy(buffer, MemHandleLock(rh));
		}

		UtilsUnlockString(rh);
		SetFieldText(frm, fAlarmDate, buffer);
	}
	DBClose(&dbo);
}

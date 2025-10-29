/*******************************************************************************
*
*	debug.c
*
*	Routines which shouldn't go into production.
*/
#include "eventor.h"
#include <unix_stdarg.h>

/* globals */
static struct {
	Char *event;
	Int16 year;
	Int16 month;
	Int16 day;
	UInt16 remindDays;
	Boolean repeat;
} events[]={
	{ "Today", 2003, 1, 4, 10, 1 },
	{ "Tomorrow", 2003, 1, 5, 10, 1 },
	{ "Day after Tomorrow", 2003, 1, 6, 10, 1 },
	{ "My Birthday", 1971, 6, 12, 0, 0 },
	{ "Event 1", 1972, 1, 1, 3, 1 },
	{ "Event 2", 1974, 2, 3, 1, 1 },
	{ "Event 3", 1976, 3, 5, 2, 0 },
	{ "Event 4", 1978, 4, 7, 2, 0 },
	{ "Event 5", 1980, 5, 9, 0, 1 },
	{ "Event 6", 1982, 6, 11, 3, 1 },
	{ "Event 7", 1984, 7, 13, 31, 1 },
	{ "Event 8", 1986, 8, 15, 999, 1 },
	{ "Event 9", 1988, 9, 17, 365, 1 },
	{ "Christmas", 1, 12, 24, 365, 1 },
	{ "Maikens F›dselsdag", 1968, 1, 19, 1, 1 },
	{ "Idas F›dselsdag", 2002, 1, 16, 7, 1 },
	{ "WMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW", 1, 1, 1, 1 },
	{ NULL, 0, 0, 0 },
};

/*
**	DAlert
**
**	Debug alert (alert 3456) - up to 255 chars.
**
**	Note:
**		Possible buffer overflow if output is >256 chars.
*/
void
DAlert(UInt8 *fmt,
       ...)
{
	va_list args;
	UInt8 txt[256];	/* I mean it .. */

	va_start(args, fmt);
	StrVPrintF(txt, fmt, args);
	va_end(args);
	FrmCustomAlert(3456, txt, NULL, NULL);
}

/*
**	DPreLoad
**
**	Preloads database with some entries.
*/
void
DPreLoad(void)
{
	UInt16 idx;
	TEvent e;

	if (DmNumRecordsInCategory(dbData.db, dmAllCategories)>0)
		return;

	for (idx=0; events[idx].event; idx++) {
		MemSet(&e, sizeof(TEvent), 0);
		StrNCopy(e.event, events[idx].event, EventLength);
		e.date=UtilsPackDate(events[idx].year, events[idx].month, events[idx].day);

		if ((e.flags=events[idx].remindDays))
			e.flags|=FlgRemindMe;

		if (events[idx].repeat)
			e.flags|=FlgRepeat;

		recordIdx=dmMaxRecordIndex;
		DBSetRecord(&dbData, dmUnfiledCategory, (UInt8 *)&e, sizeof(TEvent), recordIdx);
	}
}

/*
**	DSendAlarmSignal
**
**	Send myself an alarm signal.
*/
void
DSendAlarmSignal(void)
{
	UInt16 card;
	LocalID id;

	SysCurAppDatabase(&card, &id);
	AlmSetAlarm(card, id, 0, TimGetSeconds(), true);
}

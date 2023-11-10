/*
 * Debug.c
 */
#include "Include.h"
#include <unix_stdarg.h>

static struct {
	UInt16 year;
	UInt16 month;
	UInt16 day;
	UInt8 hour;
	UInt8 minute;
	Char *task;
} tasks[]= {
	{ 2003, 9, 20, 1, 0, "This is a funky task" },
	{ 2003, 8, 21, 8, 10, "Dinko" },
	{ 2003, 7, 22, 9, 20, "Long\nLong\nLong\nLong\nTask with many\n\nmany\n\nmany\n\nlines" },
	{ 2003, 6, 23, 8, 30, "Buller opgave" },
	{ 2003, 5, 24, 7, 40, "Review todays agenda" },
	{ 2003, 4, 25, 6, 50, "This is a bingo task" },
	{ 2003, 3, 26, 5, 0, "Da club rulez" },
	{ 2003, 2, 27, 4, 50, "Badminton" },
	{ 2003, 1, 28, 3, 40, "Boeffelhud" },
	{ 2002, 12, 21, 2, 30, "Do da do da do da" },
	{ 2002, 11, 22, 1, 20, "This is a fingo task" },
	{ 2002, 10, 23, 2, 10, "This is a fango task" },
	{ 2002, 9, 24, 3, 0, "This is a fungo task" },
	{ 2002, 8, 25, 4, 10, "Broetchenburgenbirg" },
	{ 2002, 7, 26, 5, 20, "Nothing" },
	{ 2002, 6, 27, 6, 30, "Zillion zings zum do" },
	{ 2002, 5, 28, 5, 40, "Zlatko Bratkovic - hans liv og levned" },
	{ 2002, 4, 29, 4, 50, "This is a flappy task" },
	{ 2002, 3, 19, 3, 0, "This is a floppy task" },
	{ 2002, 2, 20, 2, 50, "This is a fluppy task" },
	{ 2002, 1, 12, 1, 40, "Tiresome" },
	{ 0, 0, 0, 0, 0, NULL },
};

/*
 * DAlert
 *
 * Debug alert (alert 3456) - up to 255 chars.
 *
 * Note:
 * 	Possible buffer overflow if output is >256 chars.
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
 * DPreload
 */
void
DPreload(void)
{
	UInt16 i, date, time;

	if (DmNumRecordsInCategory(dbData.db, dmAllCategories))
		return;

	for (i=0; tasks[i].task; i++) {
		date=DTDatePack(tasks[i].year, tasks[i].month, tasks[i].day);
		time=DTTimePack(tasks[i].hour, tasks[i].minute);
		DBSetRecord(dmMaxRecordIndex, date, tasks[i].task, time, dmUnfiledCategory);
	}
}

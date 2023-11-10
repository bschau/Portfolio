/*
 * Debug.c
 *
 * Routines which shouldn't go into production.
 */
#include "Include.h"
#include <unix_stdarg.h>

/* globals */
static struct {
	Char *name;
} groups[]={
	{ "First grade, A" },
	{ "First grade, B" },
	{ "First grade, C" },
	{ "First grade, D" },
	{ "Second grade, A", },
	{ "Second grade, B", },
	{ "Second grade, C", },
	{ "Second grade, D", },
	{ "Third grade, A" },
	{ "Third grade, B" },
	{ "Third grade, C" },
	{ "Third grade, D" },
	{ NULL },
};

static struct {
	Char *s;
} sentences[]={
	{ "A white hat." },
	{ "Two black cats." },
	{ "The three stooges" },
	{ NULL },
};

/*
 * DAlert
 *
 * Debug alert (alert 3456) - up to 255 characters.
 *
 * NOTE: Do not overflow buffer ...!
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
 * DPreLoad
 *
 * Preload databases.
 */
void
DPreLoad(void)
{
	UInt16 i;
	Char *p;

	if (DmNumRecordsInCategory(dbTGroup.db, dmAllCategories)==0) {
		for (i=0; groups[i].name; i++) {
			p=StringPurify(groups[i].name);
			DBSetRecord(&dbTGroup, dmMaxRecordIndex, dmUnfiledCategory, p, StrLen(p)+1, SFString);
			MemPtrFree(p);
		}
	}

	if (DmNumRecordsInCategory(dbTSentence.db, dmAllCategories)==0) {
		for (i=0; sentences[i].s; i++) {
			p=StringPurify(sentences[i].s);
			DBSetRecord(&dbTSentence, dmMaxRecordIndex, dmUnfiledCategory, p, StrLen(p)+1, SFString);
			MemPtrFree(p);
		}
	}
}

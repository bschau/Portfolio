/*
 * Debug.c
 *
 * Routines which shouldn't go into production.
 */
#include "Include.h"
#include <unix_stdarg.h>

/* globals */
static Char *story="Civil War\nby Glenn Hervieux\n\nIn the 1860's, our [noun1]	went to war over [plural noun1].  The country was divided into two parts, the North (the Union) and the South (the Confederacy).  Being a [noun2] wasn't easy.  You had to carry a [noun3], a [adjective] rifle, a canteen full of [liquid], [number] rounds of ammunition in a [adjective-shape] cartridge box, and a [adjective] blanket.  Next, you had to [verb-present tense] for many [plural noun-unit of time] before you would get to [verb2-present tense].  The food was not the best.  If you were lucky, you might get a hunk of [houn4-food] and some hardtack.  Many men got sick from {verb-ing ending] [adjective2] water.  Unfortunately, many [pluralnoun3] died [adverb-ly ending] from dysentery (diarrhea) instead of from an enemy [noun].  Perhaps that was better than being wounded and having your [noun-body part] cut off.  Did you know more [pluralnoun2] died than soldiers?  It's a fact!  The Civil War cost the lives of many [pluralnoun3] for the freedom of all.  Never forget that freedom isn't free!";

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
 * DPreload
 *
 * Preload database.
 */
void
DPreload(void)
{
	if (DmNumRecordsInCategory(dbTemplate.db, dmAllCategories)>0)
		return;

	DBSetRecord(&dbTemplate, dmMaxRecordIndex, dmUnfiledCategory, story, StrLen(story)+1, SFString);
}

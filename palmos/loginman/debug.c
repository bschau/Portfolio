#include "loginman.h"

/* structs */
static UInt8 *data[]={ "abcdef", "123", "yyyh", "---djd", "www.brisse.com", "donedeal", "somebodys", "this is a realm", "wonka wonka", "3000 miles to Graceland", "Tv 2|Nyhederne", "Depeche Mode", "Bullerby", NULL };

/*******************************************************************************
*
*	dalert
*
*	Show debug alert (currently resource 3456) - up to 256 chars.
*
*	Input:
*		fmt - format string.
*		... - args (see pp 842-845 in reference manual).
*	Note:
*		BIG FAT WARNING: no check is done on the length of the
*		generated string - it is entirely possible to overflow
*		the buffer ...
*/
void
dalert(UInt8 *fmt,
       ...)
{
	va_list args;
	UInt8 txt[256];	/* don't beat it, buster! */

	va_start(args, fmt);
	StrVPrintF(txt, fmt, args);
	va_end(args);
	FrmCustomAlert(alertdebug, txt, NULL, NULL);
}

/*******************************************************************************
*
*	preload
*
*	Preload database.
*/
void
preload(DmOpenRef db)
{
	realmpacked rp;
	UInt16 i;

	for (i=0; data[i]; i++) {
		qpack(&rp, data[i], data[i], data[i]);
		storerecord(db, &rp, dmMaxRecordIndex);
		MemHandleFree(rp.mh);
	}
}

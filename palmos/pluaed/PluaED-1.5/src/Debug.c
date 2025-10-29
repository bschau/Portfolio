/*
 * Debug.c
 *
 * Routines which shouldn't go into production.
 */
#include "Include.h"
#include <unix_stdarg.h>

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

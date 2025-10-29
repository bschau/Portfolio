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

/*
 * DHexDump
 *
 * Dump src data to log file.
 *
 *  -> src		Source data.
 *  -> srcLen		Source length.
 */
void
DHexDump(UInt8 *src, UInt16 srcLen)
{
	Char hexChars[]="0123456789abcdef";
	UInt16 idx;

	for (idx=0; idx<srcLen; idx++) {
		DPrint("%c", hexChars[(*src>>4)&0x0f]);
		DPrint("%c", hexChars[*src&0x0f]);
		src++;
	}

	DPrint("\n");
}

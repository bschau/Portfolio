/*******************************************************************************
*
*	debug.c
*
*	Routines which shouldn't go into production.
*/
#include "sit.h"
#include <unix_stdarg.h>

/*******************************************************************************
*
*	dalert
*
*	Debug alert (alert 3456) - up to 256 chars.
*
*	Note:
*		Possible buffer overflow if output is >256 chars.
*/
void
dalert(UInt8 *fmt,
       ...)
{
	va_list args;
	UInt8 txt[256];	/* I mean it .. */

	va_start(args, fmt);
	StrVPrintF(txt, fmt, args);
	va_end(args);
	FrmCustomAlert(3456, txt, NULL, NULL);
}

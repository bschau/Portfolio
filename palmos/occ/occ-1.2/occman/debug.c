#include "occman.h"

/*
**	DAlert
**
**	Show debug alert (currently resource 3456) - up to 256 chars.
**
**	Input:
**		fmt - format string.
**		... - args (see pp 842-845 in reference manual).
**	Note:
**		BIG FAT WARNING: no check is done on the length of the
**		generated string - it is entirely possible to overflow
**		the buffer ...
*/
void
DAlert(UInt8 *fmt, ...)
{
	va_list args;
	UInt8 txt[256];	/* don't beat it, buster! */

	va_start(args, fmt);
	StrVPrintF(txt, fmt, args);
	va_end(args);
	FrmCustomAlert(aDebug, txt, NULL, NULL);
}

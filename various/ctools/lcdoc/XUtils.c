/**
 * Misc. utility functions.
 */
#include <stdarg.h>
#include <stdlib.h>
#include "Include.h"

/**
 * Write error to stderr and exit.
 *
 * @param ctx Ignored.
 * @param fmt Format string.
 * @param ... Varargs.
 */
void
XError(void *ctx, const char *fmt, ...)
{
	va_list ap;

	ctx=ctx;

	if (currentFile) {
		fprintf(stderr, "(%s:%i) ", currentFile, lineNo);
	}

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	fprintf(stderr, "\n");
	exit(1);
}

/**
 * Output string if verbose level is equal to strings verbose level.
 *
 * @param lvl String level.
 * @param fmt Format string.
 * @param ... Varargs.
 */
void
XOut(int lvl, const char *fmt, ...)
{
	if (verbose>=lvl) {
		va_list ap;

		va_start(ap, fmt);
		vprintf(fmt, ap);
		va_end(ap);

		printf("\n");
	}
}

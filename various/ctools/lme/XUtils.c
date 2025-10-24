/**
 * Misc. utility functions.
 */
#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
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

/**
 * Read file into memory.
 *
 * @param f File to read.
 * @return memory chunk with file content.
 */
char *
XReadFile(char *f)
{
	FILE *in;
	struct stat sBuf;
	char *mem;

	if (stat(f, &sBuf)!=0) {
		XError(NULL, "%s:%s", f, strerror(errno));
	}

	mem=UAlloc(NULL, sBuf.st_size+2);
	if ((in=fopen(f, "rb"))==0) {
		XError(NULL, "%s:%s", f, strerror(errno));
	}

	if (fread(mem, sBuf.st_size, 1, in)!=1) {
		XError(NULL, "%s:%s", f, strerror(errno));
	}

	fclose(in);

	mem[sBuf.st_size]=0;
	mem[sBuf.st_size+1]=0;

	return mem;
}

/**
 * Misc. utility functions.
 */
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "Include.h"

/**
 * Write error to stderr and exit.
 *
 * @param fmt Format string.
 * @param ... Varargs.
 */
void
XError(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	fprintf(stderr, "\n");
	exit(1);
}

#if 0
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
#endif

/**
 * Allocate and clear bytes.
 *
 * @param cnt Number of bytes to allocate.
 * @return Memory.
 * @note Caller must free memory after use.
 */
void *
XAlloc(int cnt)
{
	void *mem=calloc(1, cnt);

	if (!mem)
		XError("OOM while allocating %i bytes", cnt);

	return mem;
}

/**
 * Return pointer to string in string.
 *
 * @param haystack Where to find.
 * @param needle What to find.
 * @return Pointer to first occurrence of needle in haystack or NULL if not
 *         found.
 */
char *
XStrInStr(char *haystack, char *needle)
{
	int len=strlen(needle);
	char *s;

	for (EVER) {
		if ((s=strchr(haystack, *needle))==NULL)
			break;

		if (strncmp(s, needle, len)==0)
			return s;

		haystack++;
	}

	return NULL;
}

/**
 * Duplicate a string.
 *
 * @param src Source string.
 * @return String.
 */
char *
XStrDup(char *src)
{
	char *c=strdup(src);

	if (!c)
		XError("OOM while duplicating string '%s'", src);

	return c;
}

/**
 * Trim the input string.
 *
 * @param src Source string.
 * @return Trimmed string.
 * @note Trimming will be done in-place.
 */
char *
XTrim(char *src)
{
	int len=strlen(src), idx, end;

	while (len && isspace((int)src[len-1]))
		len--;

	for (idx=0; (idx<len) && (isspace((int)src[idx])); idx++);
	end=len-idx;

	if (end)
		memmove(src, src+idx, end);

	src[end]=0;

	return src;
}

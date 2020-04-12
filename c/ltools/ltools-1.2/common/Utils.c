/**
 * Misc. utility functions.
 */
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

static void (*fXError)(void *, const char *, ...);

/**
 * Initialise error callback
 *
 * @param cb Callback.
 */
void
UInit(void (*func)(void *, const char *, ...))
{
	fXError=func;
}

/**
 * Allocate and clear bytes.
 *
 * @param ctx Context (or NULL).
 * @param cnt Number of bytes to allocate.
 * @return Memory.
 * @note Caller must free memory after use.
 */
void *
UAlloc(void *ctx, int cnt)
{
	void *mem=calloc(1, cnt);

	if (!mem) {
		fXError(ctx, "OOM while allocating %i bytes", cnt);
	}

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
UStrInStr(char *haystack, char *needle)
{
	int len=strlen(needle);
	char *s;

	while (1) {
		if ((s=strchr(haystack, *needle))==NULL) {
			break;
		}

		if (strncmp(s, needle, len)==0) {
			return s;
		}

		haystack++;
	}

	return NULL;
}

/**
 * Duplicate a string.
 *
 * @param ctx Context (or NULL).
 * @param src Source string.
 * @return String.
 */
char *
UStrDup(void *ctx, char *src)
{
	char *c=strdup(src);

	if (!c) {
		fXError(ctx, "OOM while duplicating string '%s'", src);
	}

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
UTrim(char *src)
{
	int len=strlen(src);

	if (len) {
		int idx, end;

		while (len && isspace((int)src[len-1])) {
			len--;
		}

		for (idx=0; (idx<len) && (isspace((int)src[idx])); idx++);
		end=len-idx;

		if (end) {
			memmove(src, src+idx, end);
		}

		src[end]=0;
	}

	return src;
}

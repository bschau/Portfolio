/**
 * Misc. utility functions.
 */
#include <sys/stat.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "include.h"

/**
 * Allocate and clear bytes.
 *
 * @param cnt Number of bytes to allocate.
 * @return Memory.
 * @note Caller must free memory after use.
 */
void *u_alloc(int cnt) {
	void *mem = calloc(1, cnt);

	if (!mem) {
		u_error("OOM while allocating %i bytes", cnt);
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
char *u_strinstr(char *haystack, char *needle) {
	int len = strlen(needle);
	char *s;

	while (1) {
		if ((s = strchr(haystack, *needle)) == NULL) {
			break;
		}

		if (strncmp(s, needle, len) == 0) {
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
char *u_strdup(char *src) {
	char *c = strdup(src);

	if (!c) {
		u_error("OOM while duplicating string '%s'", src);
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
char *u_trim(char *src) {
	int len = strlen(src);

	if (len) {
		int idx, end;

		while ((len) && (isspace((int) src[len - 1]))) {
			len--;
		}

		for (idx = 0; (idx < len) && (isspace((int) src[idx])); idx++);
		end = len - idx;

		if (end) {
			memmove(src, src + idx, end);
		}

		src[end] = 0;
	}

	return src;
}

/**
 * Write error to stderr and exit.
 *
 * @param fmt Format string.
 * @param ... Varargs.
 */
void u_error(const char *fmt, ...) {
	va_list ap;

	if (current_file) {
		fprintf(stderr, "(%s:%i) ", current_file, line_no);
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
void u_out(int lvl, const char *fmt, ...) {
	if (verbose >= lvl) {
		va_list ap;

		va_start(ap, fmt);
		vprintf(fmt, ap);
		va_end(ap);

		printf("\n");
	}
}

#ifdef WIN32
/**
 * Return the last component of this path.
 *
 * @param path Path.
 * @return Pointer to last component.
 */
char *basename(char *path) {
	char *p = strrchr(path, '\\');

	if (p) {
		return ++p;
	}

	if ((p = strrchr(path, '/'))) {
		return ++p;
	}

	return path;
}
#endif

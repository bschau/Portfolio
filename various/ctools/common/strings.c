#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

/**
 * Duplicate a string.
 *
 * @param str String to duplicate
 */
char *xstrdup(const char *str) {
	char *s = strdup(str);

	if (s == NULL) {
		fprintf(stderr, "Out of memory dup'ing string\n");
		exit(1);
	}

	return s;
}

#include "include.h"

/* globals */
static char path[PATH_MAX + 1];
static int path_len;

/**
 * Init path system.
 */
void path_init(void) {
	path_len = 0;
}

/**
 * @return path.
 */
char *path_get(void) {
	return path;
}

/**
 * Add path component.
 *
 * @param p Component.
 */
void path_add(char *p) {
	int len = strlen(p);

	if ((path_len + len + 2) > PATH_MAX) {
		fprintf(stderr, "Exceeding path length with %s + %s\n", path, p);
		exit(1);
	}

	path[path_len++] = SEPARATOR_CHAR;
	strcpy(path + path_len, p);
	path_len += len;
	path[path_len] = 0;
}

/**
 * Remove last componet from path.
 */
void path_up(void) {
	char *p = strrchr(path, SEPARATOR_CHAR);

	if (p == NULL) {
		path_len = 0;
		path[0] = 0;
	} else {
		*p = 0;
		path_len = strlen(path);
	}
}

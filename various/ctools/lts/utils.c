#include <ctype.h>
#include <time.h>
#include "include.h"

/**
 * Collapse slashes into one.
 *
 * @param src Source/dst string.
 */
void collapse_slashes(char *src) {
	char *dst = src;
	int first = 0;

	while (*src) {
		if (*src == '/') {
			if (!first) {
				*dst++ = '/';
				first = 1;
			}
		} else {
			first = 0;
			*dst++ = *src;
		}

		src++;
	}

	*dst = 0;
}

/**
 * Get current working directory.
 *
 * @param dst Where to store cwd. Must hold PATH_MAX bytes.
 */
void get_cwd(char *dst) {
	if (getcwd(dst, PATH_MAX) == NULL) {
		perror("getcwd");
		exit(1);
	}
}

/**
 * Change directory to dir.
 *
 * @param dir Directory.
 */
void ch_dir(char *dir) {
	if (chdir(dir)) {
		perror(dir);
		exit(1);
	}
}

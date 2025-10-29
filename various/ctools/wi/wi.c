#include <sys/stat.h>
#include <ctype.h>
#include <dirent.h>
#include <libgen.h>
#include <limits.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef Linux
#include <unistd.h>
#endif
#define MAIN
#include "common.h"

/* forward decl */
#ifndef MACOSX
static char *strcasestr(char *haystack, char *needle);
#endif
static void grep(char *o);
static void get_working_directory(void);
static void locate(char *p);

/* globals */
static char *obj;
static int look_for_directory, case_sensitive;
static char cur_path[PATH_MAX + 1], full_path[PATH_MAX + 1];

#ifndef MACOSX
/**
 * Perform case insensitive search for needle in haystack.
 *
 * @param haystack Haystack to look in.
 * @param needle Needle to look for.
 * @return Position in haystack where needle was found or NULL if needle
 *         wasn't found.
 */
static char *strcasestr(char *haystack, char *needle) {
	char *startn = NULL, *np = NULL, *p;

	for (p = haystack; *p; p++) {
		if (np) {
			if (toupper(*p) == toupper(*np)) {
				if (!*++np) {
					return startn;
				}
			} else {
				np = NULL;
			}
		} else if (toupper(*p) == toupper(*needle)) {
			np = needle + 1;
			startn = p;
		}
	}

	return NULL;
}
#endif

/**
 * Grep (* ahem *) for search term in current filename.
 *
 * @param o Current file name.
 */
static void grep(char *o) {
	char *m;

	if (case_sensitive) {
		m = strstr(o, obj);
	} else {
		m = strcasestr(o, obj);
	}

	if (m) {
		printf("%s\n", full_path);
	}
}

/**
 * Get current working directory.
 */
static void get_working_directory(void) {
	if (getcwd(cur_path, PATH_MAX) == NULL) {
		fprintf(stderr, "where am I?\n");
		exit(1);
	}
}

/**
 * Search for filename/directory name in path.
 *
 * @param p Path to search.
 */
static void locate(char *p) {
	struct dirent *de;
	struct stat buf;
	DIR *dh;

	if (chdir(p) != 0) {
		perror(p);
		return;
	}

	get_working_directory();

	if ((dh = opendir(".")) == NULL) {
		perror(p);
		return;
	}

	while ((de = readdir(dh)) != NULL) {
		if ((strcmp(de->d_name, ".") == 0) || (strcmp(de->d_name, "..") == 0)) {
			continue;
		}

		snprintf(full_path, PATH_MAX, "%s%c%s", cur_path, SEPARATOR_CHAR, de->d_name);
#ifdef WIN32
		if (stat(full_path, &buf) == 0) {
#else
		if (lstat(full_path, &buf) == 0) {
#endif
			if (S_ISDIR(buf.st_mode)) {
				if (look_for_directory) {
					grep(de->d_name);
				}

				locate(full_path);
				if (chdir("..") != 0) {
					fprintf(stderr, "parent directory '%s' gone\n", dirname(full_path));
					exit(1);
				}

				get_working_directory();
			} else if (S_ISREG(buf.st_mode)) {
				if (!look_for_directory) {
					grep(de->d_name);
				}
			}
		} else {
			perror(full_path);
		}
	}

	closedir(dh);
}

/**
 * Write usage info and exit.
 *
 * @param ec Exit code.
 */
static void usage(int ec) {
	FILE *o = (ec) ? stderr : stdout;

	fprintf(o, "wi %s\n", bstools_version);
	fprintf(o, "Usage: wi [OPTIONS] object\n");
	fprintf(o, "\n[OPTIONS]\n");
	fprintf(o, "  -d    Look for directory\n");
	fprintf(o, "  -e    Exact case\n");
	fprintf(o, "  -h    Help (this page)\n");
	exit(ec);
}

/**
 * Main.
 */
int main(int argc, char *argv[]) {
	int opt;

	case_sensitive = 0;
	look_for_directory = 0;
	while ((opt = getopt(argc, argv, "deh")) != -1) {
		switch (opt) {
			case 'd':
				look_for_directory = 1;
				break;
			case 'e':
				case_sensitive = 1;
				break;
			case 'h':
				usage(0);
			default:
				fprintf(stderr, "Unknown option %c\n", opt);
				usage(1);
		}
	}

	if ((argc - optind) != 1) {
		usage(1);
	}

	obj = argv[optind];

	locate(".");
	exit(0);
}

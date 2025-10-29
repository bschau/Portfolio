#include <sys/param.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAIN
#include "common.h"

/* forward decls */
static void usage(int ec);
static void get_working_directory(void);
static void remtilde(char *p);

/* globals */
static int show_only = 0, quiet = 0, recurse = 0;
static long long int files_seen = 0, files_deleted = 0;
static long long int dirs_seen = 0, reclaimed = 0;
#ifdef WIN32
#define ARGS_STR "hqrs"
#else
#define ARGS_STR "hiqrs"
static int ignore_hidden = 0;
#endif
static char cur_path[PATH_MAX + 1], full_path[PATH_MAX + 1];

/**
 * Write usage screen and exit.
 *
 * @param ec Exit code.
 */
static void usage(int ec) {
	FILE *f = (ec) ? stderr : stdout;

	fprintf(f, "remtilde %s\n", bstools_version);
	fprintf(f, "Usage: remtilde [OPTIONS] [path1 ... pathX]\n");
	fprintf(f, "\n[OPTIONS]\n\n");
	fprintf(f, " -h       Help (this screen)\n");
#ifndef WIN32
	fprintf(f, " -i       Ignore hidden files and folders\n");
#endif
	fprintf(f, " -q       Quiet - do not show deleted files\n");
	fprintf(f, " -r       Recurse into directories\n");
	fprintf(f, " -s       Show files to delete (but do not delete them)\n");
	exit(ec);
}

/**
 * Main.
 */
int main(int argc, char *argv[]) {
	int opt;

	while ((opt = getopt(argc, argv, ARGS_STR)) != -1) {
		switch (opt) {
		case 'h':
			usage(0);

#ifndef WIN32
		case 'i':
			ignore_hidden = 1;
			break;
#endif

		case 'q':
			quiet = 1;
			break;

		case 'r':
			recurse = 1;
			break;

		case 's':
			show_only = 1;
			break;

		default:
			fprintf(stderr, "Unknown option '%c'\n", opt);
			exit(1);
		}
	}

	if ((argc - optind) == 0) {
		remtilde(".");
	} else {
		for (; optind < argc; optind++) {
			remtilde(argv[optind]);
		}
	}

	if (!quiet) {
#ifdef WIN32
		printf("(%I64d dir%s/%I64d file%s/%I64d deleted/", dirs_seen, (dirs_seen == 1 ? "" : "s"), files_seen, (files_seen == 1 ? "" : "s"), files_deleted);
#else
		printf("(%lld dir%s/%lld file%s/%lld deleted/", dirs_seen, (dirs_seen == 1 ? "" : "s"), files_seen, (files_seen == 1 ? "" : "s"), files_deleted);
#endif
		if (reclaimed < 1024) {
#ifdef WIN32
			printf("%I64d byte%s", reclaimed, (reclaimed == 1 ? "" : "s"));
#else
			printf("%lld byte%s", reclaimed, (reclaimed == 1 ? "" : "s"));
#endif
		} else if (reclaimed < (1024 * 1024)) {
			reclaimed /= 1024;
#ifdef WIN32
			printf("%I64d KB", reclaimed);
#else
			printf("%lld KB", reclaimed);
#endif
		} else {
			reclaimed /= (1024 * 1024);
#ifdef WIN32
			printf("%I64d MB", reclaimed);
#else
			printf("%lld MB", reclaimed);
#endif
		}

		printf(" reclaimed)\n");
	}	

	exit(0);
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
 * Remove ~ files.
 *
 * @param p path.
 */
static void remtilde(char *p) {
	struct dirent *de;
	struct stat buf;
	DIR *dh;

	if (chdir(p) != 0) {
		fprintf(stderr, "failed to enter '%s'\n", p);
		return;
	}

	get_working_directory();

	if ((dh = opendir(".")) == NULL) {
		perror(p);
		return;
	}

	dirs_seen++;
	while ((de = readdir(dh)) != NULL) {
		if ((strcmp(de->d_name, ".") == 0) || (strcmp(de->d_name, "..") == 0)) {
			continue;
		}

		files_seen++;
#ifndef WIN32
		if ((de->d_name[0] == '.') && (ignore_hidden)) {
			continue;
		}
#endif 

		snprintf(full_path, PATH_MAX, "%s%c%s", cur_path, SEPARATOR_CHAR, de->d_name);

#ifdef WIN32
		if (stat(full_path, &buf) == 0) {
#else
		if (lstat(full_path, &buf) == 0) {
#endif
			if ((S_ISDIR(buf.st_mode)) && (recurse)) {
				remtilde(full_path);
				if (chdir("..") != 0) {
					fprintf(stderr, "parent directory '%s' gone\n", dirname(full_path));
					exit(1);
				}
				get_working_directory();
			} else if (S_ISREG(buf.st_mode)) {
				int len = strlen(de->d_name);

				if (de->d_name[len - 1] == '~') {
					files_deleted++;
					reclaimed += buf.st_size;
					if ((show_only) ||
					    ((!show_only) && (!quiet))) {
						printf("%s\n", full_path);
					}

					if (!show_only) {
						if (unlink(full_path)) {
							fprintf(stderr, "failed to remove '%s'\n", full_path);
						}
					}
				}
			}
		} else {
			fprintf(stderr, "cannot access '%s'\n", full_path);
		}
	}

	closedir(dh);
}

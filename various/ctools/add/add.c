#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MAIN
#include "common.h"

/* forward decls */
static void usage(int ec);
static long long int get_long_long(StringBuilder s_bld, int colunn);

/* globals */
static int quiet = 0;

/**
 * Write usage screen and exit.
 *
 * @param ec Exit code.
 */
static void usage(int ec) {
	FILE *f = (ec) ? stderr : stdout;

	fprintf(f, "add %s\n", bstools_version);
	fprintf(f, "Usage: add [OPTIONS] [file]\n");
	fprintf(f, "\n[OPTIONS]\n\n");
	fprintf(f, " -c col   Column to add/subtract (defaults to 1)\n");
	fprintf(f, " -h       Help (this screen)\n");
	fprintf(f, " -q       Quiet\n");
	fprintf(f, " -s       Subtract instead of add\n");
	fprintf(f, " -z val   Set zero to val (defaults to 0)\n");
	exit(ec);
}

/**
 * Main.
 */
int main(int argc, char *argv[]) {
	int column = 1, subtract = 0, opt, c;
	long long int zero = 0, val;
	FILE *fh = stdin;
	StringBuilder s_bld = stringbuilder_new(1024);

	if (s_bld == NULL) {
		fprintf(stderr, "Out of memory\n");
		exit(1);
	}

	while ((opt = getopt(argc, argv, "c:hqsz:")) != -1) {
		switch (opt) {
		case 'c':
			column = atoi(optarg);
			break;

		case 'h':
			usage(0);

		case 'q':
			quiet = 1;
			break;

		case 's':
			subtract = 1;
			break;

		case 'z':
			zero = atoll(optarg);
			break;

		default:
			fprintf(stderr, "Unknown option '%c'\n", opt);
			exit(1);
		}
	}

	if ((argc - optind) > 0) {
		if ((fh = fopen(argv[optind], "rb")) == NULL) {
			perror("fopen");
			exit(1);
		}
	}

	while (!feof(fh)) {
		if ((c = fgetc(fh)) == EOF) {
			break;
		}

		if (c == '\n') {
			val = get_long_long(s_bld, column);

			if (subtract) {
				zero -= val;
			} else {
				zero += val;
			}

			stringbuilder_reset(s_bld);
		} else {
			stringbuilder_append_char(s_bld, (char) c);
		}
	}

#ifdef WIN32
	printf("%I64d\n", zero);
#else
	printf("%lld\n", zero);
#endif

	exit(0);
}

/**
 * Get the long long int from the desired column.
 *
 * @param s_bld StringBuilder.
 * @param column Column.
 * @return value.
 *
 * Note: may print error in which case value returned is 0.
 */
static long long int get_long_long(StringBuilder s_bld, int column) {
	char *s = stringbuilder_to_string(s_bld);
	long long int val = 0;
	int c = column;

	while (c > 0) {
		for (; (*s) && (isspace((int) *s)); s++);

		if (!*s) {
			if (!quiet) {
				fprintf(stderr, "no such column\n");
			}

			return 0;
		}

		c--;
		if (c > 0) {
			for (; (*s) && (!isspace((int) *s)); s++);

			if (!*s) {
				if (!quiet) {
					fprintf(stderr, "no such column\n");
				}

				return 0;
			}
		}
	}

#ifdef WIN32
	if ((c = sscanf(s, "%I64d", &val)) != 1) {
#else
	if ((c = sscanf(s, "%lld", &val)) != 1) {
#endif
		if (!quiet) {
			fprintf(stderr, "cannot convert column %i\n", column);
		}

		return 0;
	}

	return val;
}

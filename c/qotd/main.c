#define MAIN
#include "include.h"

/** forward decls */
static void usage(int ec);

/**
 * Write usage informati0on and exit with 'ec' status.
 *
 * @param ec Error code.
 */
static void usage(int ec)
{
	FILE *f = (ec) ? stderr : stdout;

	fprintf(f, "qotd v%s\n", QOTD_VERSION);
	fprintf(f, "Usage: qotd [COMMAND] [index]\n");
	fprintf(f, "\n[COMMAND]\n");
	fprintf(f, "  help         Show help (this page)\n");
	fprintf(f, "  file file    Read Quote of the day from 'file'\n");
	fprintf(f, "\n[index]\n");
	fprintf(f, "Specify a numeric value to select this particular quote of the day\n");
	exit(ec);
}

/**
 * Main. (Surprise!)
 */
int main(int argc, char *argv[])
{
	char *qotd_file = NULL;
	int optind = 1, preferred = -1;

	while (optind < argc) {
		if (strcmp(argv[optind], "file") == 0) {
			if (optind + 1 >= argc) {
				fprintf(stderr, "file needs argument (file to quotes of the day)\n");
				exit(1);
			}

			optind++;
			qotd_file = argv[optind];
		} else if (strcmp(argv[optind], "help") == 0) {
			usage(0);
		} else {
			char *rest;

			preferred = strtol(argv[optind], &rest, 10);
			if (*rest != 0) {
				fprintf(stderr, "Unknown argument: %s\n", argv[optind]);
				exit(1);
			}
		}

		optind++;
	}

	qotd_file_set(qotd_file);
	qotd_file_load();
	char *qotd = qotd_file_pick_one(preferred);
	qotd_file_output(stdout, qotd);

	exit(0);
}

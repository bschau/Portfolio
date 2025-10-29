/**
 * Mirrors a src directory to a dst directory.
 */
#define MAIN
#include "include.h"

/** forward decls */
static void usage(int ec);

/** globals */
extern char *bstools_version;

/**
 * Write usage information and exit with 'ec' status.
 *
 * @param ec Error code.
 */
static void usage(int ec) {
	FILE *f = (ec) ? stderr : stdout;

	fprintf(f, "lts %s\n", bstools_version);
	fprintf(f, "Usage: lts [OPTIONS] src dst\n\n");
	fprintf(f, "[OPTIONS]\n");
	fprintf(f, "  -d       Dry run - print what to do without doing it\n");
	fprintf(f, "  -h       This page\n");
	exit(ec);
}

/**
 * Main.
 */
int main(int argc, char *argv[]) {
	int opt;

	while ((opt = getopt(argc, argv, "dh")) != -1) {
		switch (opt) {
		case 'd':
			dry_run = 1;
			break;

		case 'h':
			usage(0);

		default:
			fprintf(stderr, "Unknown option: '%c'\n", (char) opt);
			usage(1);
		}
	}

	if ((argc - optind) < 2) {
		usage(1);
	}

	get_cwd(full_path);

	ch_dir(argv[optind]);
	get_cwd(s_path);

	ch_dir(full_path);
	ch_dir(argv[optind + 1]);
	get_cwd(d_path);

	walk_dst();

	path_init();
	ch_dir(s_path);
	synchronize_files(".");

	delete_files();

	path_init();
	ch_dir(d_path);
	delete_empty_dirs(".");

	exit(0);
}

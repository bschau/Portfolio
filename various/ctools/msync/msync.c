/**
 * Synchronizes a directory tree to a remote ftp server based on a stamp date.
 * msync will never delete files or directories!
 *
 * Up to bstools-2.3 this command was known as rsc.
 */
#define MAIN
#include "common.h"
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

	fprintf(f, "msync %s\n", bstools_version);
	fprintf(f, "Usage: msync [OPTIONS] command files\n\n");
	fprintf(f, "[OPTIONS]\n");
	fprintf(f, "  -f             Force operation\n");
	fprintf(f, "  -h             This page\n");
	fprintf(f, "\nCOMMANDS:\n");
	fprintf(f, "  get x .. y                      Get files from server\n");
	fprintf(f, "  ignore x .. y                   Add components to ignore list\n");
	fprintf(f, "  init ftp://user:pass@host/dir   Initialize root with path and remote root\n");
	fprintf(f, "   or  file:///path/to/dst        Initialize root with path to local filesystem\n");
	fprintf(f, "  lock                            Lock tree (disable sync)\n");
	fprintf(f, "  ls                              List content of current directory on remote\n");
	fprintf(f, "  put x .. y                      Put files to server\n");
	fprintf(f, "  rm x .. y                       Delete objects on server\n");
	fprintf(f, "  show                            Show files to be synchronized\n");
	fprintf(f, "  sync                            Synchronize\n");
	fprintf(f, "  unlock                          Remove lock\n");
	fprintf(f, "  uptodate                        Mark tree as being up to date\n");
	exit(ec);
}

/**
 * Main.
 */
int main(int argc, char *argv[]) {
	int opt;

	while ((opt = getopt(argc, argv, "fh")) != -1) {
		switch (opt) {
		case 'f':
			force = 1;
			break;

		case 'h':
			usage(0);

		default:
			fprintf(stderr, "Unknown option: '%c'\n", (char) opt);
			usage(1);
		}
	}

	if ((argc - optind) < 1) {
		usage(1);
	}

	if (strcmp(argv[optind], "get") == 0) {
		cmd_get(argc, argv, ++optind);
	} else if (strcmp(argv[optind], "ignore") == 0) {
		cmd_ignore(argc, argv, ++optind);
	} else if (strcmp(argv[optind], "info") == 0) {
		cmd_info();
	} else if (strcmp(argv[optind], "init") == 0) {
		cmd_init(argc, argv, ++optind);
	} else if (strcmp(argv[optind], "lock") == 0) {
		cmd_lock();
	} else if (strcmp(argv[optind], "ls") == 0) {
		cmd_ls();
	} else if (strcmp(argv[optind], "put") == 0) {
		cmd_put(argc, argv, ++optind);
	} else if (strcmp(argv[optind], "rm") == 0) {
		cmd_rm(argc, argv, ++optind);
	} else if (strcmp(argv[optind], "show") == 0) {
		cmd_sync(0);
	} else if (strcmp(argv[optind], "sync") == 0) {
		cmd_sync(1);
	} else if (strcmp(argv[optind], "unlock") == 0) {
		cmd_unlock();
	} else if (strcmp(argv[optind], "uptodate") == 0) {
		cmd_uptodate();
	} else {
		fprintf(stderr, "Unknown command: %s\n", argv[optind]);
		usage(1);
	}

	exit(0);
}

/**
 * Manipulate and view zen stories.
 */
#define MAIN
#include "include.h"

/** forward decls */
static void usage(int ec);

/**
 * Write usage information and exit with 'ec' status.
 *
 * @param ec Error code.
 */
static void usage(int ec)
{
	FILE *f = (ec) ? stderr : stdout;

	fprintf(f, "zen v%s\n", ZEN_VERSION);
	fprintf(f, "Usage: zen [OPTIONS] command\n\n");
	fprintf(f, "[OPTIONS]\n");
	fprintf(f, "  -h                              This page\n");
	fprintf(f, "\nCOMMANDS:\n");
	fprintf(f, "  a | add [\"story\" | @story]      Add story or story from file\n");
	fprintf(f, "                                  If omitted, launch editor\n");
	fprintf(f, "                                  Multiple stories can be given\n");
	fprintf(f, "  cl | close id1 id2 .. idN       Close story with ids\n");
	fprintf(f, "  c | count [all]                 Return count of open stories or all\n");
	fprintf(f, "                                  Use count0 (or c0) to omit trailing newline.\n");
	fprintf(f, "  d | delete id1 id2 .. idN       Delete story with ids\n");
	fprintf(f, "  e | edit id \"story\" | @story    Edit story with id with content of file\n");
	fprintf(f, "  init                            Initialize zen story repository here\n");
	fprintf(f, "  l | list [brief] [all]          List open stories or all\n");
	fprintf(f, "  e | edit id [\"story\" | @story]  Edit story with id with content of file\n");
	fprintf(f, "                                  If omitted, launch editor\n");
	fprintf(f, "  r | reopen id1 id2 .. idN       Re-open story with ids\n");
	fprintf(f, "  s | status [id1 id2 .. idN]     Show story status of all stories or ids\n");
	fprintf(f, "  v | view id                     View story with id\n");
	fprintf(f, "\n'brief' and 'all' can be abbreviatdd to 'b' and 'a'.\n");
	exit(ec);
}

/**
 * Main.
 */
int main(int argc, char *argv[])
{
	int opt;

	while ((opt = getopt(argc, argv, "h")) != -1) {
		switch (opt) {
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

	if (strcmp(argv[optind], "add") == 0 ||
		strcmp(argv[optind], "a") == 0) {
		cmd_add(argc, argv, ++optind);
	} else if (strcmp(argv[optind], "close") == 0 ||
		strcmp(argv[optind], "cl") == 0) {
		cmd_close(argc, argv, ++optind);
	} else if (strcmp(argv[optind], "count") == 0 ||
		strcmp(argv[optind], "c") == 0) {
		cmd_count(argc, argv, ++optind, 0);
	} else if (strcmp(argv[optind], "count0") == 0 ||
		strcmp(argv[optind], "c0") == 0) {
		cmd_count(argc, argv, ++optind, 1);
	} else if (strcmp(argv[optind], "delete") == 0 ||
		strcmp(argv[optind], "d") == 0) {
		cmd_delete(argc, argv, ++optind);
	} else if (strcmp(argv[optind], "edit") == 0 ||
		strcmp(argv[optind], "e") == 0) {
		cmd_edit(argc, argv, ++optind);
	} else if (strcmp(argv[optind], "init") == 0) {
		cmd_init();
	} else if (strcmp(argv[optind], "list") == 0 ||
		strcmp(argv[optind], "l") == 0) {
		cmd_list(argc, argv, ++optind);
	} else if (strcmp(argv[optind], "reopen") == 0 ||
		strcmp(argv[optind], "r") == 0) {
		cmd_reopen(argc, argv, ++optind);
	} else if (strcmp(argv[optind], "status") == 0 ||
		strcmp(argv[optind], "s") == 0) {
		cmd_status(argc, argv, ++optind);
	} else if (strcmp(argv[optind], "view") == 0 ||
		strcmp(argv[optind], "v") == 0) {
		cmd_view(argc, argv, ++optind);
	} else {
		fprintf(stderr, "Unknown command: %s\n", argv[optind]);
		usage(1);
	}

	exit(0);
}

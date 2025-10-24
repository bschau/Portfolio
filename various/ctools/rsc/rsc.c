#define MAIN
#include "include.h"

static void usage(int ec);

static void usage(int ec)
{
	FILE *f = (ec) ? stderr : stdout;

	fprintf(f, "rsc %s\n", bstools_version);
	fprintf(f, "Usage: rsc [OPTIONS] command files\n\n");
	fprintf(f, "[OPTIONS]\n");
	fprintf(f, "  -h             This page\n");
	fprintf(f, "\nCOMMANDS:\n");
	fprintf(f, "  ignore x .. y              Add components to ignore list\n");
	fprintf(f, "  info                       Show info about tree\n");
	fprintf(f, "  init user@server::module   Initialize root with path and remote root\n");
	fprintf(f, "  lock                       Lock tree (disable sync)\n");
	fprintf(f, "  set var value              Set variable to value\n");
	fprintf(f, "  show                       Show what will happen on next synchronize\n");
	fprintf(f, "  sync                       Synchronize\n");
	fprintf(f, "  unlock                     Remove lock\n");
	exit(ec);
}

int main(int argc, char *argv[])
{
	int opt;

	while ((opt = getopt(argc, argv, "h")) != -1)
	{
		switch (opt) {
		case 'h':
			usage(0);

		default:
			fprintf(stderr, "Unknown option: '%c'\n", (char) opt);
			usage(1);
		}
	}

	if (argc - optind < 1)
	{
		usage(1);
	}

	if (strcmp(argv[optind], "ignore") == 0)
	{
		cmd_ignore(argc, argv, ++optind);
	}
	else if (strcmp(argv[optind], "info") == 0)
	{
		cmd_info();
	}
	else if (strcmp(argv[optind], "init") == 0)
	{
		cmd_init(argc, argv, ++optind);
	}
	else if (strcmp(argv[optind], "lock") == 0)
	{
		cmd_lock();
	}
	else if (strcmp(argv[optind], "set") == 0)
	{
		cmd_set(argc, argv, ++optind);
	}
	else if (strcmp(argv[optind], "show") == 0)
	{
		cmd_sync(1);
	}
	else if (strcmp(argv[optind], "sync") == 0)
	{
		cmd_sync(0);
	}
	else if (strcmp(argv[optind], "unlock") == 0)
	{
		cmd_unlock();
	}
	else
	{
		fprintf(stderr, "Unknown command: %s\n", argv[optind]);
		usage(1);
	}

	exit(0);
}

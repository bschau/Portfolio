#include "include.h"

void cmd_set(int argc, char *argv[], int optind)
{
	if (argc - optind < 2)
	{
		fprintf(stderr, "Missing variable and value to set\n");
		exit(1);
	}

	config_find();
	config_read();

	if (strcmp(argv[optind], "command") == 0)
	{
		command = argv[++optind];
	}
	else if (strcmp(argv[optind], "module") == 0)
	{
		module = argv[++optind];
	}
	else if (strcmp(argv[optind], "remoteconfig") == 0)
	{
		remote_config = argv[++optind];
	}
	else if (strcmp(argv[optind], "remotersync") == 0)
	{
		remote_rsync = argv[++optind];
	}
	else
	{
		fprintf(stderr, "Unknown variable: %s\n", argv[optind]);
		exit(1);
	}

	config_write();
}

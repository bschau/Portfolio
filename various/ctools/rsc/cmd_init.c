#include <sys/stat.h>
#include "include.h"

static void usage(void);

static void usage(void)
{
	fprintf(stderr, "init server::module\n");
	exit(1);
}

void cmd_init(int argc, char *argv[], int optind)
{
	struct stat s_buf;
	FILE *fp;

	if (argc - optind < 1)
	{
		usage();
	}

	if (stat(RCFILE, &s_buf) == 0)
	{
		fprintf(stderr, "This root is already initialized\n");
		exit(1);
	}

#ifdef WIN32
	if (mkdir(".RSC"))
#else
	if (mkdir(".RSC", 0700))
#endif
	{
		perror(".RSC");
		exit(1);
	}

	if (strstr(argv[optind], "::") == NULL)
	{
		usage();
	}

	module = argv[optind];

	config_write();

	set_file_permissions(RCFILE, 0600);

	config_read();

	fp = fopen(STAMPFILE, "wb");
	if (fp == NULL)
	{
		perror(STAMPFILE);
		exit(1);
	}

	fprintf(fp, "0\n");
	fclose(fp);

	set_file_permissions(STAMPFILE, 0600);

	ignore_list_read();
	ignore_list_add(".RSC");
	ignore_list_add("*~");
	ignore_list_write();
}

#include "include.h"

void cmd_lock(void)
{
	char p[PATH_MAX + 1];
	FILE *fp;

	config_find();

	snprintf(p, PATH_MAX, "%s/%s", root_dir, LOCKFILE);

	exit_if_link(p);

	fp = fopen(p, "wb");
	if (fp == NULL)
	{
		fprintf(stderr, "Cannot write to lock file\n");
		exit(1);
	}

#ifdef WIN32
	fprintf(fp, "0");
#else
	fprintf(fp, "%i", (int) getuid());
#endif
	fclose(fp);

	set_file_permissions(p, 0600);
}

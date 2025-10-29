#include "common.h"
#include "include.h"

/**
 * Lock tree (= disable sync).
 */
void cmd_lock(void) {
	char p[PATH_MAX + 1];
	FILE *fp;

	config_find();

	snprintf(p, PATH_MAX, "%s/%s", root_dir, LOCKFILE);

	exit_if_link(p);

	if ((fp = fopen(p, "wb")) == NULL) {
		fprintf(stderr, "Cannot write to lock file\n");
		exit(1);
	}

	fprintf(fp, "%i", (int) getuid());
	fclose(fp);

	set_file_permissions(p, 0600);
}

#include <pwd.h>
#include "common.h"
#include "include.h"

/**
 * Unlock tree (= enable sync).
 */
void cmd_unlock(void) {
	char p[PATH_MAX + 1];
	int uid;

	config_find();

	snprintf(p, PATH_MAX, "%s/%s", root_dir, LOCKFILE);
	exit_if_link(p);

	if ((uid = is_locked()) >= 0) {
		if ((uid_t) uid == getuid()) {
			unlink(p);
		} else {
			struct passwd *pwd = getpwuid((uid_t) uid);

			fprintf(stderr, "Locked");
			if (pwd) {
				fprintf(stderr, " by %s", pwd->pw_name);
			}
			fprintf(stderr, "\n");
		}
	}
}

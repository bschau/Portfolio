#include <pwd.h>
#include <time.h>
#include "common.h"
#include "include.h"

/**
 * Show info about tree.
 */
void cmd_info(void) {
	char p[PATH_MAX + 1];
	int uid;

	config_find();
	config_read();
	ignore_list_read();

	if (path) {
		printf("Local path: %s\n", path);
	} else {
		printf("Host: %s\n", host);
		printf("Path: %s\n", remote_dir);
		printf("User: %s\n", user);

		if ((pass) && (strlen(pass) > 0)) {
			printf("Password: <set>\n");
		} else {
			printf("Password: <unset>\n");
		}
	}

	printf("Stamp: %i - %s", (int) stamp, ctime(&stamp));

	snprintf(p, PATH_MAX, "%s/%s", root_dir, LOCKFILE);
	exit_if_link(p);

	if ((uid = is_locked()) >= 0) {
		struct passwd *pwd = getpwuid((uid_t) uid);

		printf("Locked: %s\n", pwd ? pwd->pw_name : "<yes>");
	}

	printf("Ignore:");
	ignore_list_print(' ');
	printf("\n");
}

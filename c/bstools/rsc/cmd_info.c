#ifndef WIN32
#include <pwd.h>
#endif
#include <time.h>
#include "include.h"

void cmd_info(void)
{
	StringBuilder s_bld;
	char p[PATH_MAX + 1];
	int uid;

	config_find();
	config_read();
	ignore_list_read();

	s_bld = command_expand(0);
	printf("Command:       %s\n", stringbuilder_to_string(s_bld));
	printf("Module:        %s\n", module);
	printf("Remote config: %s\n", remote_config);
	printf("Remote rsync:  %s\n", remote_rsync);
	printf("Stamp:         %i - %s", (int) stamp, ctime(&stamp));

	snprintf(p, PATH_MAX, "%s/%s", root_dir, LOCKFILE);
	exit_if_link(p);

	uid = is_locked();
	if (uid >= 0)
	{
#ifdef WIN32
		printf("Locked:       yes\n");
#else
		struct passwd *pwd = getpwuid((uid_t) uid);

		printf("Locked:        %s\n", pwd ? pwd->pw_name : "<yes>");
#endif
	}

	printf("Ignore:       ");
	ignore_list_print(' ');
	printf("\n");
}

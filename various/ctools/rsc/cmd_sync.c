#ifndef WIN32
#include <pwd.h>
#endif
#include "include.h"

void cmd_sync(int show)
{
	StringBuilder s_bld;
	int uid;

	config_find();
	config_read();
	ignore_list_read();

	uid = is_locked();
	if (uid >= 0)
	{
#ifdef WIN32
		fprintf(stderr, "Tree locked\n");
#else
		struct passwd *pwd = getpwuid((uid_t) uid);

		fprintf(stderr, "Tree locked");
		if (pwd)
		{
			fprintf(stderr, " by %s", pwd->pw_name);
		}

		fprintf(stderr, "\n");
		exit(1);
#endif
	}

	s_bld = command_expand(show);

	printf("%s\n", stringbuilder_to_string(s_bld));
	uid = system(stringbuilder_to_string(s_bld));
	if (uid != 0)
	{
		perror("system");
		exit(1);
	}

	if (!show)
	{
		config_write_stamp();
	}

	exit(0);
}

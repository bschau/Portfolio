#ifndef WIN32
#include <pwd.h>
#endif
#include "include.h"

void cmd_unlock(void)
{
	char p[PATH_MAX + 1];
#ifndef WIN32
	int uid;
#endif

	config_find();

	snprintf(p, PATH_MAX, "%s/%s", root_dir, LOCKFILE);
	exit_if_link(p);

#ifdef WIN32
	unlink(p);
#else
	uid = is_locked();
	if (uid >= 0)
	{
		if ((uid_t) uid == getuid())
		{
			unlink(p);
		} else {
			struct passwd *pwd = getpwuid((uid_t) uid);

			fprintf(stderr, "Locked");
			if (pwd)
			{
				fprintf(stderr, " by %s", pwd->pw_name);
			}
			fprintf(stderr, "\n");
		}
	}
#endif
}

#include "private.h"

/*******************************************************************************
*
*	installsignals
*
*	Install signals from qsiglist.
*
*	Input:
*		qs - qsiglist.
*/
void
installsignals(qsiglist *qs)
{
	struct sigaction sigact[MAXSIGNALS];
	int cnt;

	memset(&sigact, 0, sizeof(sigaction)*MAXSIGNALS);
	for (cnt=0; qs->signum && cnt<MAXSIGNALS; cnt++) {
		sigemptyset(&sigact[cnt].sa_mask);
		sigact[cnt].sa_flags=0;
		sigact[cnt].sa_handler=qs->action;
		sigaction(qs->signum, &sigact[cnt], NULL);
		qs++;
	}
}

/*******************************************************************************
*
*	dropprivs
*
*	Drop privileges - 'su' to other user/group.
*
*	Input:
*		user - user to drop privileges to.
*	Output:
*		s: 0.
*		f: !0.
*/
int
dropprivs(char *user)
{
	struct passwd *u;
	uid_t uid;
	gid_t gid;

	u=getpwnam(user);
	if (!u)
		return 1;

	uid=u->pw_uid;
	gid=u->pw_gid;

	endpwent();

	setgid(gid);
	setuid(uid);

	return 0;
}

/*******************************************************************************
*
*	requireroot
*
*	You must be root!
*/
void
requireroot(void)
{
	if (!getuid())
		return;

	fprintf(stderr, "you must be root to run this application\n");
	exit(1);
}

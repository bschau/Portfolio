#include "private.h"

/* protos */
static void childexit(int);

/*******************************************************************************
*
*	childexit
*
*	Signal handler for SIGCHLD.   Whenever a child dies (exits) this
*	handler is called.
*/
static void
childexit(int unused)
{
	reap(&runningchilds);
}

/*******************************************************************************
*
*	endless
*
*	Master loop.
*/
void
endless(void)
{
	int sfd=getserverfd();
	struct sigaction sigact;
	struct sockaddr_in clnt;
	int clntlen, cfd;
	pid_t cpid;
	session fake;

	sigact.sa_handler=childexit;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags=0;
	sigaction(SIGCHLD, &sigact, 0);

	macroize(&fake);
	qlog(QINFO, "(endless) started");
	for (EVER) {
		clntlen=sizeof(clnt);
		cfd=accept(sfd, (struct sockaddr *)&clnt, &clntlen);
		if (cfd>-1) {
			cpid=fork();
			switch (cpid) {
				case -1:
					qlog(QWARN, "(endless) fork error - refusing connections for %i seconds", sleephard);
					netstatusw(cfd, &fake, smtp421);
					nsleep(sleephard, 0);
					break;
				case 0:	
					mainchild(cfd, &clnt);
					alarm(0);
					exit(0);
				default:
					runningchilds++;
					break;
			}
		}

		/* I'm the parent */
		close(cfd);
	}
}

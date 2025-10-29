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
	struct dirent *de=NULL;
	DIR *dir=NULL;
	struct sigaction sigact;
	char cntlfilename[CNTLFILENAMELEN+1];
	pid_t cpid;
	int err, cfd;

	if (chdir(spooldir)) {
		qlog(QFATAL, "(endless) cannot change to spooldir %s (%i)", spooldir, errno);
		exit(0);
	}

	dir=opendir(".");
	if (!dir) {
		qlog(QFATAL, "(endless) cannot open directory for reading", errno);
		exit(0);
	}

	sigact.sa_handler=childexit;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags=0;
	sigaction(SIGCHLD, &sigact, 0);

	cntlfilename[CNTLFILENAMELEN]='\x00';
	qlog(QINFO, "(endless) started");
	for (EVER) {
		de=readdir(dir);
		if (de) {
			if (*(de->d_name)!='c')
				continue;

			if (strlen(de->d_name)!=CNTLFILENAMELEN)
				continue;

			cfd=open(de->d_name, O_RDWR);
			if (cfd<0) {
				qlog(QINFO, "(brain) cannot open file %s", de->d_name);
				continue;
			}

			err=lockfile(cfd);
			close(cfd);
			if (err)	/* it is already locked */
				continue;

			while (isbusy()) {
				qlog(QWARN, "(endless) server is busy - sleeping for %i seconds", sleepsoft);
				nsleep(sleepsoft, 0);
			}

			memmove(cntlfilename, de->d_name, CNTLFILENAMELEN);

			/* spawn child */
			runningchilds++;
			cpid=fork();
			switch (cpid) {
				case -1:
					runningchilds--;
					qlog(QWARN, "(endless) fork error - waiting %i seconds for things to cool down", sleephard);
					nsleep(sleephard, 0);
					break;
				case 0:
					mainchild(cntlfilename);
					exit(0);
				default:
					sleep(interdelay);
					break;
			}
		} else {
			/* hit end, wait */
			rewinddir(dir);
			nsleep(sleepqueue, 0);
		}
	}
}

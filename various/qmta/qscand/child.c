#include "private.h"

/* globals */
static session gs;
static recipients rcpt;
static int keepmail;

/* protos */
static void cleanup(void);
static void childalarm(int signum);

/*******************************************************************************
*
*	cleanup.
*
*	Clean up this child.
*/
static void
cleanup(void)
{
	sweepdone();
	freerecipients(&rcpt);
	freesession(&gs, keepmail);

	if (cfd>-1) {
		unlockfile(cfd);
		close(cfd);
	}
}

/*******************************************************************************
*
*	childalarm
*
*	Special alarm handler to catch sweep/munpack errors so that the child
*	will not hang.
*
*	Input:
*		alarmsig - alarm signal.
*/
static void
childalarm(int alarmsig)
{
	qlog(QWARN, "(childalarm) alarm - time is up");
	signal(SIGALRM, SIG_DFL);
	_exit(0);
}

/*******************************************************************************
*
*	mainchild
*
*	Main entry point for a child.
*
*	Input:
*		cntl - control filename.
*/
void
mainchild(char *cntl)
{
	qsiglist sl[]={
		{ SIGALRM, childalarm },
		{ SIGHUP, SIG_IGN },
		{ SIGINT, SIG_IGN },
		{ SIGQUIT, SIG_IGN },
		{ SIGPIPE, SIG_IGN },
		{ SIGUSR1, SIG_IGN },
		{ SIGUSR2, SIG_IGN },
		{ 0, NULL },
	};
	int err, nosend;
	time_t now;

	keepmail=1;
	master=0;
	umask(077);
	cfd=open(cntl, O_RDWR);
	if (cfd<0) {
		qlog(QDEBUG, "(mainchild) cannot open file %s", cntl);
		return;
	}

	err=lockfile(cfd);
	if (err) {
		close(cfd);
		qlog(QDEBUG, "(mainchild) file %s already in use", cntl);
		return;
	}

	if (atexit(cleanup)) {
		qlog(QFATAL, "(mainchild) cannot register atexit handler - going down in flames!");
		return;
	}

	installsignals(&sl[0]);

	/* too old? */
	time(&now);
	err=checkdequeue(&gs, &rcpt, cfd, cntl, SMSILENT, now, &nosend);
	if (err)
		return;

	if (nosend) {
		err=dequeue(&gs, "cannot scan mail for vira (been in queue for too long)");
		if (!err)
			keepmail=0;

		return;
	}

	sweep(cntl);
}

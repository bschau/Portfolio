#include "private.h"

/* globals */
static int cfd=-1;
static database db;
static char *path;

/* protos */
static void cleanup(void);
static void childalarm(int);

/*******************************************************************************
*
*	cleanup.
*
*	Clean up this child.
*/
static void
cleanup(void)
{
	donemail();
	qdbclose(&db);
	if (path)
		free(path);

	if (cfd>-1) {
		unlockfile(cfd);
		close(cfd);
	}
}

/*******************************************************************************
*
*	childalarm
*
*	Alarm handler to drop connection if idle time is too large.
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
	int err;

	umask(077);
	master=0;
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

	path=relpath(qroot, QDOMAINDB);
	if (path) {
		err=qdbopen(&db, path);
		if (err) {
			qlog(QFATAL, "(mainchild) cannot open %s", path);
			return;
		}

		alarm(otime);
		sendmail(cntl, cfd, &db, SMQUEUE, SMSILENT);
		alarm(0);
	}
}

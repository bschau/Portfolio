#include "private.h"

/* globals */
static child ch;
static session sess;

/* protos */
static void cleanup(void);
static void childalarm(int);
static int childsetup(int, struct sockaddr_in *);

/*******************************************************************************
*
*	cleanup.
*
*	Clean up this child.
*/
static void
cleanup(void)
{
	if (ch.fd>-1)
		close(ch.fd);

	if (ch.helodomain)
		free(ch.helodomain);

	qdbclose(&ch.spammerdb);
	qdbclose(&ch.domaindb);
	qdbclose(&ch.accessdb);

	destroysession(&ch);
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
*	childsetup
*
*	Setup newly spawned child.   Allocates files etc. etc.
*
*	Input:
*		cfd - child file descriptor.
*		peer - peer descriptor.
*	Output:
*		s: 0.
*		f: !0.
*/
static int
childsetup(int cfd,
           struct sockaddr_in *peer)
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

	ch.sess=&sess;
	if (atexit(cleanup)) {
		qlog(QFATAL, "(childsetup) cannot register atexit handler - going down in flames!");
		return 1;
	}

	installsignals(&sl[0]);

	time(&ch.startconn);
	ch.fd=cfd;

	resolveip(ch.sess, peer->sin_addr.s_addr, NULL);

	err=chdir(qroot);
	if (err)
		qlog(QFATAL, "(childsetup) cannot go root");
	else {
		qdbopen(&ch.accessdb, QACCESSDB);
		qdbopen(&ch.domaindb, QDOMAINDB);
		qdbopen(&ch.spammerdb, QSPAMMERDB);
		err=0;
	}

	if (chdir(spooldir)) {
		qlog(QFATAL, "(childsetup) cannot change to spooldir %s", spooldir);
		return 1;
	}

	return err;
}

/*******************************************************************************
*
*	mainchild
*
*	Main entry point for a child.
*
*	Input:
*		cfd - child file descriptor.
*		peer - peer descriptor.
*/
void
mainchild(int cfd,
          struct sockaddr_in *peer)
{
	int err;

	master=0;
	macroize(&sess);

	alarm(itime);
	if (isbusy()) {
		netstatusw(cfd, &sess, smtp421);
		qlog(QWARN, "(mainchild) server is busy - refusing connections for %i seconds", sleepsoft);
		nsleep(sleepsoft, 0);
	} else {
		umask(077);

		err=childsetup(cfd, peer);
		if (!err) {
			/* no spam, please */
			err=spamcheck(sess.remoteaddr, SCSILENT);
			switch (err) {
				case SCSPAMMER:
					netstatusw(cfd, &sess, smtp550blacklisted);
					qlog(QWARN, "(mainchild) %s \"%s\" is listed as spammer - connection refused", sess.remotename, sess.remoteaddr);
					break;
				case SCPARK:
					netstatusw(cfd, &sess, smtp421);
					qlog(QWARN, "(mainchild) spamcheck returned SCPARK for %s \"%s\"", sess.remotename, sess.remoteaddr);
					break;
				default:
					smtp(&ch);
					break;
			}

			return;
		}

		netstatusw(cfd, &sess, smtp421);
	}
}

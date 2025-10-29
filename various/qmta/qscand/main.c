#define MAIN
#include "private.h"

/* protos */
static void cleanup(void);
static void graceful(int);

/*******************************************************************************
*
*	cleanup
*
*	Perform global cleanup.
*/
static void
cleanup(void)
{
	foreground();
	syscleanup();
}

/*******************************************************************************
*
*	graceful
*
*	Major shutdown.   Wait for childs to finish before shutting down.
*
*	Input:
*		s - signal number received.
*/
static void
graceful(int s)
{
	int cnt;

	/* kill childs */
	kill(0, SIGTERM);

	/* let them finish their businesses */
	nsleep(3, 0);

	/* reap */
	cnt=reap(NULL);
	if (cnt)
		qlog(QINFO, "(cleanup) brought down %i childs", cnt);

	qlog(QINFO, "(graceful) goodbye");
	signal(s, SIG_DFL);
	exit(0);
}

/*******************************************************************************
*
*	main
*
*	Main entry.    Arguments:
*
*		-h - request help.
*		-r root - root directory (qroot).
*/
int
main(int argc,
     char *argv[])
{
	int rest;

	rest=common(argc, argv, NULL, "hr:", NULL, &cleanup, &graceful, DAEMONIZE, ROOTONLY);
	if (rest)
		usage(1);

	maxchilds=smaxchilds;

	rest=dropprivs(user);
	if (rest) {
		qlog(QFATAL, "(main) cannot drop privileges to %s", user);
		exit(1);
	} else
		qlog(QINFO, "(main) privileges dropped, acting as %s", user);

	endless();
	/* shouldn't reach this ... */
	exit(1);
}

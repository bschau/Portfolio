#include "private.h"

/* globals */
static char *usstr;

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
	syscleanup();
}

/*******************************************************************************
*
*	graceful
*
*	Signal handler.
*
*	Input:
*		s - signal number.
*/
static void
graceful(int s)
{
	fprintf(stderr, "caught signal number %i - exiting\n", s);
	signal(s, SIG_DFL);
	exit(0);
}

/*******************************************************************************
*
*	usage
*
*	Writes usage to stderr.
*
*	Input:
*		ret - returncode.
*/
void
usage(int ret)
{
	fprintf(stderr, "%s %s\nUsage: %s [-h] [-r root] %s\n", whoami, VERSION, whoami, ((usstr) ? usstr : ""));
	exit(ret);
}

/*******************************************************************************
*
*	common
* 
*	Set up common stuff for tools.   Understand these switches:
*
*		-h - request help.
*		-r root - root directory (qroot).
*
*	Input:
*		argc - main argc.
*		argv - main argv.
*		optfunc - options function.
*		optstr - options string.
*		us - usage string (or NULL).
*		cfunc - cleanup function ptr.
*		gfunc - graceful function ptr.
*		daemonize - daemonize process?
*	Output:
*		arguments left.
*/
int
common(int argc,
       char *argv[],
       int (*optfunc)(char, char *),
       char *optstr,
       char *us,
       void (*cfunc)(void),
       void (*gfunc)(int),
       int daemonize,
       int rootonly)
{
	qsiglist sl[]={
		{ SIGTERM, NULL },
		{ SIGHUP, SIG_IGN },
		{ SIGINT, SIG_IGN },
		{ SIGQUIT, SIG_IGN },
		{ SIGPIPE, SIG_IGN },
		{ SIGUSR1, SIG_IGN },
		{ SIGUSR2, SIG_IGN },
		{ 0, NULL },
	};
	int err, opt, restopt;

	if (rootonly)
		requireroot();

	elinkopen();
	setroot();
	iofd=-1;

	whoami=strrchr(argv[0], '/');
	if (whoami)
		whoami++;
	else
		whoami=argv[0];

	if (gfunc)
		sl[0].action=gfunc;
	else
		sl[0].action=&graceful;

	/* parent */
	if (cfunc)
		opt=atexit(cfunc);
	else
		opt=atexit(cleanup);

	if (opt) {
		fprintf(stderr, "cannot register atexit handler\n");
		exit(1);
	}

	installsignals(&sl[0]);
	usstr=us;

	while ((opt=getopt(argc, argv, optstr))!=-1) {
		switch (opt) {
			case 'h':
				usage(0);
				break;
			case 'r':
				qroot=optarg;
				break;
			default:
				if (optfunc)
					err=(optfunc)(opt, optarg);
				else
					err=1;

				if (err) {
					fprintf(stderr, "unknown switch %c\n", opt);
					usage(1);
				}
				break;
		}
	}

	restopt=argc-optind;

	if (chdir(qroot)) {
		fprintf(stderr, "cannot find my roots (%s)\n", qroot);
		usage(1);
	}

	opt=configure();
	if (opt)
		exit(1);

	if (daemonize) {
		if (isrunning()) {
			fprintf(stderr, "%s already running\n", whoami);
			exit(1);
		}

		background();

		/* child of parent */
		if (cfunc)
			opt=atexit(cfunc);
		else
			opt=atexit(cleanup);
	
		if (opt) {
			qlog(QFATAL, "cannot register atexit handler\n");
			exit(1);
		}
	}

	return restopt;
}

/*******************************************************************************
*
*	syscleanup
*
*	System cleanup.
*/
void
syscleanup(void)
{
	elinkclose();
	deconfigure();
}

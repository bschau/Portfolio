#include "private.h"

/* protos */
static void daemonize(void);
static void makerunpid(void);

/*******************************************************************************
*
*	background
*
*	Put process into background, write pid file.
*/
void
background(void)
{
	openlog(whoami, LOG_PID, LOG_DAEMON);
	daemonize();
	makerunpid();
	master=1;
}

/*******************************************************************************
*
*	foreground
*
*	Cleanup background stuff.
*/
void
foreground(void)
{
	char *pd;

	if (!master)
		return;

	if (iofd>-1)
		close(iofd);

	if (pidfile) {	
		if (mypid) {
			pd=relpath(qroot, PIDDIR);
			if (pd) {
				if (!chdir(pd))
					unlink(pidfile);
				else
					qlog(QFATAL, "(foreground) cannot change to pid directory (%s) (%i)", pd, errno);
				free(pd);
			} else
				qlog(QFATAL, "(foreground) cannot make pid path");
		}
		free(pidfile);
	}

	closelog();
}

/*******************************************************************************
*
*	daemonize
*
*	Put program in the background as a 'real' daemon.
*/
static void
daemonize(void)
{
	struct rlimit reslim={0};
	int err;

	err=fork();
	switch (err) {
		case -1:
			fprintf(stderr, "%s: couldn't fork - first\n", whoami);
			exit(1);
		case 0:
			break;
		default:
			exit(0);
	}

	reslim.rlim_max=0;
	err=getrlimit(RLIMIT_NOFILE, &reslim);
	if (err==-1) {
		fprintf(stderr, "%s: couldn't set limits\n", whoami);
		exit(1);
	}

	for (err=0; err<reslim.rlim_max; err++)
		close(err);

	err=setsid();
	if (err==-1) {
		fprintf(stderr, "%s: couldn't set session\n", whoami);
		exit(1);
	}

	err=fork();
	switch (err) {
		case -1:
			fprintf(stderr, "%s: couldn't fork - second\n", whoami);
			exit(1);
		case 0:	
			break;
		default:
			exit(0);
	}

	if (chdir("/")) {
		fprintf(stderr, "%s: couldn't change directory to /\n", whoami);
		exit(1);
	}

	umask(0);
	iofd=open("/dev/null", O_RDWR);
	if (iofd==-1) {
		fprintf(stderr, "%s: no /dev/null? (%i)\n", whoami, errno);
		exit(1);
	}

	dup(iofd);
	dup(iofd);
}

/*******************************************************************************
*
*	makerunpid
*
*	Create PIDfile - exit on error.
*/
static void
makerunpid(void)
{
	char *pd=relpath(qroot, PIDDIR);
	int pidfd=-1;
	char pidbuf[12];
	struct passwd *u;

	if (!pd) {
		qlog(QFATAL, "(makerunpid) cannot make pid path");
		exit(1);
	}

	if (chdir(pd)) {
		qlog(QFATAL, "(makerunpid) couldn't change to pid directory (%s) (%i)", pd, errno);
		free(pd);
		exit(1);
	}
	free(pd);

	pidfile=xconcat(whoami, ".pid");
	if (!pidfile) {
		qlog(QFATAL, "(makerunpid) out of memory");
		exit(1);
	}

	pidfd=open(pidfile, O_RDWR|O_CREAT|O_EXCL, 0644);
	if (pidfd==-1) {
		qlog(QFATAL, "(makerunpid) couldn't create pidfile or existing pidfile (%i).", errno);
		exit(1);
	}

	sprintf(pidbuf, "%i", getpid());
	write(pidfd, pidbuf, strlen(pidbuf));
	close(pidfd);

	u=getpwnam(user);
	if (u)
		chown(pidfile, u->pw_uid, u->pw_gid);
	endpwent();

	mypid=1;
}

/*******************************************************************************
*
*	isrunning
*
*	Is program already started?
*
*	Input:
*		program not running: 0.
*		program running: !0.
*/
int
isrunning(void)
{
	char *pd=relpath(qroot, PIDDIR);
	char *pf;
	int fd;

	if (!pd)
		return 1;

	if (chdir(pd)) {
		free(pd);
		return 1;
	}
	free(pd);

	pf=xconcat(whoami, ".pid");
	if (!pf)
		return 1;

	fd=open(pf, O_RDONLY);
	free(pf);
	if (fd<0) {
		if (errno==ENOENT)
			return 0;
	} else
		close(fd);

	return 1;
}

/*******************************************************************************
*
*	reap
*
*	Reap childrens.
*
*	Input:
*		cnt - child cnt or NULL.
*	Output:
*		childs reapt.
*/
int
reap(int *cnt)
{
	int reapt=0;
	int status;
	pid_t pid;

	while ((pid=waitpid(-1, &status, WNOHANG))>0) {
		reapt++;
		if (cnt) {
			(*cnt)--;
			if (*cnt<0) {
				/* this is not critical - it means that we have
				   no idea of how many childs we have spawned.
				   But it will not prevent us from spawning
				   more ... */
				qlog(QINFO, "(reap) master %i unsynchonized", getpid());
				*cnt=0;
			}
		}
	}

	return reapt;
}

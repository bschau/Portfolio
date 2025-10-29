/*
**	main.c
**
**	Main loop of proxy.
*/
#define SNMPPROXYMAIN
#include "include.h"

/* protos */
static void Usage(int);
static int Daemonize(void);
static void ExitHandler(void);
static void SigHandler(int);
static void CreateRunPid(void);
static void DropPrivileges(void);
static void Brain(void);

/* globals */
static int myPid=0;

/*
**	Usage
**
**	Write usage for program.
**
**	 -> eC		Exit code.
*/
static void
Usage(int eC)
{
	FILE *f=stderr;

	if (!eC)
		f=stdout;

	fprintf(f, "Usage: snmpproxy [OPTIONS] configurationfile\n\nOPTIONS:\n\n  -D{acflot}  Dump configuration\n  -d          Turn debugging on\n  -F          Stay in foreground\n  -h          Write usage (this screen)\n  -v          Write version information\n");
	exit(eC);
}

/*
**	Daemonize
**
**	Go into background.
*/
static int
Daemonize(void)
{
	int fd, fdL;

	switch (fork())
	{
	case 0:
		break;
	case -1:
		fprintf(stderr, "(Daemonize) Cannot fork\n");
		return -1;
	default:
		_exit(0);
	}

	fdL=sysconf(_SC_OPEN_MAX);
	for (fd=0; fd<fdL; fd++)
		close(fd);

	if (setsid()<0)
		return -1;

	switch (fork())
	{
	case 0:
		break;
	case -1:
		return -1;
	default:
		_exit(0);
	}

	/* close all std i/o */
	if ((fd=open("/dev/null", O_RDWR))<0) {
		XLog(LOG_ERR, "(Daemonize) Cannot open /dev/null");
		return 1;
	}

	dup(fd);
	dup(fd);

	return 0;
}

/*
**	ExitHandler
**
**	Handle exit conditions.
*/
static void
ExitHandler(void)
{
	if (myPid) {
		if (unlink(pPidFile)<0)
			XLog(LOG_WARNING, "(ExitHandler) Cannot delete pid file, %s (%i)", pPidFile, errno);
	}
	XLog(LOG_INFO, "(ExitHandler) Goodbye");
	closelog();
}

/*
**	SigHandler
**
**	Handle signal.
**
**	 -> s		Signal number.
*/
static void
SigHandler(int s)
{
	XLog(LOG_DEBUG, "(SigHandler) Received signal %i", s);

	signal(s, SIG_DFL);
	exit(0);
}

/*
**	CreateRunPid
**
**	Write pid file to state directory.
*/
static void
CreateRunPid(void)
{
	int pidFd;
	char pidBuffer[16];

	if ((pidFd=open(pPidFile, O_RDWR|O_CREAT|O_EXCL, 0644))==-1) {
		XLog(LOG_ERR, "(CreateRunPid) Cannot create pid file, %s (%i)", pPidFile, errno);
		exit(1);
	}

	sprintf(pidBuffer, "%i", getpid());
	write(pidFd, pidBuffer, strlen(pidBuffer));
	close(pidFd);

	myPid=1;
}

/*
**	DropPrivileges
**
**	Drop privileges and become ordinary user if daemon was started
**	by root.
*/
static void
DropPrivileges(void)
{
	struct passwd *u;
	uid_t uid;
	gid_t gid;

	if ((uid=getuid())) {
		if ((u=getpwuid(uid))==NULL) {
			XLog(LOG_ERR, "(DropPrivileges) Cannot find user described by id %i in password database", uid);
			exit(1);
		}
	} else {
		if ((u=getpwnam(pUser))==NULL) {
			XLog(LOG_ERR, "(DropPrivileges) User '%s' not found in password database", pUser);
			exit(1);
		}

		uid=u->pw_uid;
		gid=u->pw_gid;

		setgid(gid);
		setuid(uid);

	}

	XLog(LOG_INFO, "(DropPrivileges) Running as '%s' (%i)", u->pw_name, uid);
	endpwent();
}

/*
**	Brain
**
**	Main loop.
*/
static void
Brain(void)
{
	char buffer[20];
	struct in_addr c;

	memset((char *)&client, 0, sizeof(client));
	client.sin_family=AF_INET;
	client.sin_addr.s_addr=htonl(INADDR_ANY);
	client.sin_port=pPort;

	clientLen=sizeof(client);
	if (bind(sFd, (struct sockaddr *)&client, clientLen)<0) {
		XLog(LOG_ERR, "(Brain) Cannot bind to socket");
		exit(1);
	}

	XLog(LOG_INFO, "(Brain) Proxy is running on port %i", ntohs(pPort));
	for (EVER) {
		clientLen=sizeof(client);

		memset((char *)&client, 0, sizeof(client));
		client.sin_family=AF_INET;
		client.sin_addr.s_addr=htonl(INADDR_ANY);
		client.sin_port=htons(pPort);

		pkgLength=recvfrom(sFd, pkgBuffer, PKGBUFFERLEN, 0, (struct sockaddr *)&client, &clientLen);
		if (pkgLength<0) {
			if (errno==EINTR || errno==EAGAIN)
				continue;

			XLog(LOG_ERR, "(Brain) Broken connection (%i)", errno);
			break;
		}

		if (pkgLength>=2) {
			c.s_addr=ntohl(client.sin_addr.s_addr);
			if (TrustedAccess(&c)==0)
				Proxy();
			else {
				XIPToDottedAscii(buffer, &c);
				XLog(LOG_WARNING, "(Brain) Client %s denied access", buffer);
			}
		} else
			XLog(LOG_DEBUG, "(Brain) Got package lesser than 2 bytes - skipping");
	}
}

/*
**	main
*/
int
main(int argc, char *argv[])
{
	int debugOverride=0, dumpConfig=0, daemonize=1, opt;
	char *dumpString=NULL;

	while ((opt=getopt(argc, argv, "D:dFhv"))!=-1) {
		switch (opt) {
		case 'D':
			dumpConfig=1;
			dumpString=optarg;
			break;
		case 'd':
			debugOverride=1;
			break;
		case 'F':
			daemonize=0;
			break;
		case 'h':
			Usage(0);
			break;
		case 'v':
			printf("snmpproxy v1.0 (c) Brian Schau <mailto:brian@schau.dk>\n");
			exit(1);
		default:
			Usage(1);
			break;
		}
	}

	if ((argc-optind)!=1)
		Usage(1);

	InitNibbleMem(512, 512);
	if (dumpConfig) {
		hasStdout=1;
		ConfigLoad(argv[optind]);

		opt=0;
		for (; *dumpString; dumpString++) {
			if (opt)
				printf("\n\n");
			else
				opt=1;

			switch (*dumpString) {
			case 'a':
				ConfigDump();
				printf("\n\n");
				TrustedDump();
				printf("\n\n");
				OidDump();
				printf("\n\n");
				OidDumpGetNextList();
				printf("\n\n");
				OidDumpFakeList();
				break;
			case 'c':
				ConfigDump();
				break;
			case 'f':
				OidDumpFakeList();
				break;
			case 'l':
				OidDumpGetNextList();
				break;	
			case 'o':
				OidDump();
				break;
			case 't':
				TrustedDump();
				break;
			default:
				fprintf(stderr, "(main) Unknown dump type '%c'\n", *dumpString);
				break;
			}
		}

		exit(0);
	}

	if (daemonize) {
		if (Daemonize()<0)
			exit(1);
	} else
		hasStdout=1;

	openlog("snmpproxy", LOG_PID|LOG_CONS, LOG_DAEMON);

	ConfigLoad(argv[optind]);

	if (atexit(ExitHandler)<0) {
		XLog(LOG_ERR, "(main) Cannot install atexit handler");
		exit(1);
	}
	
	if (debugOverride) {
		XLog(LOG_DEBUG, "(main) Entering debug mode");
		pDebug=1;
	}

	DropPrivileges();
	CreateRunPid();

	signal(SIGTERM, SigHandler);
	signal(SIGHUP, SigHandler);
	signal(SIGQUIT, SigHandler);
	signal(SIGINT, SigHandler);
	signal(SIGABRT, SigHandler);
	signal(SIGPIPE, SIG_IGN);

	/* fs unmountable */
	if (chdir("/")<0)
		XLog(LOG_WARNING, "(main) Cannot change directory to /");

	pkgBuffer=XMalloc(PKGBUFFERLEN);
	masqBuffer=XMalloc(MASQBUFFERLEN);
	pPeer.tick=0;		/* force name lookup on first packet */

	DynBufInit(512);
	for (EVER) {
		if ((sFd=socket(AF_INET, SOCK_DGRAM, 0))==-1) {
			XLog(LOG_ERR, "(main) Cannot allocate socket");
			exit(1);
		}
		Brain();
		close(sFd);
		XLog(LOG_WARNING, "(main) Sleeping for 10 seconds");
		sleep(10);
	}

	exit(0);
}

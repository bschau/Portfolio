#define MAIN
#include "libqmta.h"
#include <dirent.h>

/* macros */
#define BUFSIZE 256

/* globals */
static int cfd=-1;
static session gs;
static recipients rcpt;

/* protos */
static void peek(char *);
static int preparesession(char *);
static void prettyprint(char *);
static void retime(time_t *);

/*******************************************************************************
*
*	main
*
*	Entry point - uses standard switches.
*/
int
main(int argc,
     char *argv[])
{
	DIR *dir=NULL;
	struct dirent *de=NULL;
	int rest, err, cnt;

	rest=common(argc, argv, NULL, "hr:", "[control-file1 ... control-filen]", NULL, NULL, NORMAL, ROOTONLY);

	err=dropprivs(user);
	if (err) {
		fprintf(stderr, "cannot drop privileges to %s\n", user);
		exit(1);
	}

	if (chdir(spooldir)) {
		fprintf(stderr, "cannot change to spooldir %s (%i)\n", spooldir, errno);
		exit(1);
	}

	if (rest) {
		for (cnt=0; cnt<rest; cnt++) {
			peek(argv[optind]);
			optind++;
		}
	} else {
		dir=opendir(".");
		if (dir) {
			for (de=readdir(dir); de; de=readdir(dir)) {
				if (strlen(de->d_name)==CNTLFILENAMELEN)
					peek(de->d_name);
			}
			closedir(dir);
		} else {
			fprintf(stderr, "cannot open directory (%i)\n", errno);
			exit(1);
		}
	}

	exit(0);
}

/*******************************************************************************
*
*	peek
*
*	Peek this file.
*/
static void
peek(char *cntl)
{
	int err;

	err=preparesession(cntl);
	if (err)
		return;

	prettyprint(cntl);

	freerecipients(&rcpt);
	freesession(&gs, SESSNODELETE);
	if (cfd>-1)
		close(cfd);
}

/*******************************************************************************
*
*	preparesession
*
*	Open and load session.   This will _not_ lock the session files.
*
*	Input:
*		cntl - control file name.
*	Output:
*		s: 0.
*		f: !0.
*/
static int
preparesession(char *cntl)
{
	int err;

	if (*cntl!='c' && *cntl!='s')
		return -1;

	cfd=open(cntl, O_RDWR);
	if (cfd<0) {
		fprintf(stderr, "cannot open %s (%i)\n", cntl, errno);
		return -1;
	}

	err=initsession(&gs, cfd, cntl);
	if (err) {
		fprintf(stderr, "cannot init session (%s)\n", cntl);
		return -1;
	}

	err=getsession(&gs, cfd);
	if (err) {
		fprintf(stderr, "cannot get session (%s)\n", cntl);
		return -1;
	}

	loadrecipients(&rcpt, cntl);

	return 0;
}

/*******************************************************************************
*
*	prettyprint
*
*	Pretty print session.
*
*	Input:
*		cntl - control file name.
*/
static void
prettyprint(char *cntl)
{
	char *p;

	printf("%s%s:\n  c/d/a:", cntl, ((gs.messagetype) ? " (status from system)" : ""));
	retime(&gs.created);
	retime(&gs.dequeue);
	retime(&gs.access);
	printf("\n  from: %s\n", gs.from);
	if (rcpt.recip) {
		printf("  to:");
		rewindrecipients(&rcpt);
		for (p=nextrecipient(&rcpt, 1); p; p=nextrecipient(&rcpt, 1))
			printf(" %s", p);

		printf("\n");
	} else 
		printf("  error loading recipients\n");
}

/*******************************************************************************
*
*	retime
*
*	Output time in 're' format.
*
*	Input:
*		t - time to 're'.
*/
static void
retime(time_t *t)
{
	char buf[BUFSIZE+2];
	struct tm *tmp;

	memset(buf, 0, BUFSIZE+1);
	tmp=localtime(t);
	strftime(buf, BUFSIZE, "%d/%m/%Y %H:%M:%S", tmp);
	printf(" (%s)", buf);
}

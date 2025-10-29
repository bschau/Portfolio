#include "private.h"

/* protos */
static int agedmail(session *, time_t);
static void snip(char *, char *, char);

/*******************************************************************************
*
*	agedmail
*
*	Has mail been in the queue for too long?
*
*	Input:
*		gs - session.
*		now - time.
*	Output:
*		mail good: 0.
*		mail too old: !0.
*	Note:
*		this is really crude.  If the clock skews too much, this
*		goes bananas.
*/
static int
agedmail(session *gs,
         time_t now)
{
	if ((int)now<(int)gs->dequeue)
		return 0;	/* mail good */

	qlog(QSMTP, "(agedmail) mail too old - will dequeue (%s)", gs->cntlname);
	return -1;
}

/*******************************************************************************
*
*	checkdequeue
*
*	Opens and parses control file.   Checks for dequeue.
*
*	Input:
*		gs - session.
*		rcpt - recipients.
*		cfd - control file handle.
*		cfn - control file name.
*		verbose - verboisity flag.
*		now - timestamp.
*		qs - where to store status.
*	Output:
*		s: 0.
*		f: !0.
*/
int
checkdequeue(session *gs,
             recipients *rcpt,
             int cfd,
             char *cfn,
             int verbose,
             time_t now,
             int *qs)
{
	int err;

	/* open files */
	err=initsession(gs, cfd, cfn);
	if (err) {
		if (verbose)
			fprintf(stderr, "could not open files (%s)\n", cfn);
		else
			qlog(QWARN, "(checkdequeue) could not open files (%s)", cfn);
		
		return -1;
	}

	err=getsession(gs, cfd);
	if (err) {
		if (verbose)
			fprintf(stderr, "could not get session (%s)\n", cfn);
		else
			qlog(QWARN, "(checkdequeue) could not get session (%s)", cfn);

		return -1;
	}

	/* get recipients */
	err=loadrecipients(rcpt, cfn);
	if (err) {
		if (verbose)
			fprintf(stderr, "could not get recipients (%s)\n", cfn);
		else
			qlog(QWARN, "(checkdequeue) could not get recipients (%s)", cfn);

		return -1;
	}

	gs->recipients=rcpt;

	*qs=agedmail(gs, now);

	return 0;
}

/*******************************************************************************
*
*	dequeue
*
*	Dequeues a mail.
*
*	Input:
*		gs - session.
*		mess - message to add.
*	Output:
*		s: 0.
*		f: !0.
*/
int
dequeue(session *gs,
        char *mess)
{
	int err;

	err=elinkadd(mess);
	if (err) {
		qlog(QWARN, "(dequeue) cannot add elink message");
		return -1;
	}

	err=injectmail(gs, QTXTMAILAGED);
	if (err) {
		qlog(QWARN, "(dequeue) cannot inject mail (%s)", QTXTMAILAGED);
		return -1;
	}

	return 0;
}

/*******************************************************************************
*
*	findheader
*
*	Loop through header searching for info.
*
*	Input:
*		sess - session.
*		xh - xheader.
*		dst - where to store header line.
*		linelen - length of dst.
*	Output:
*		s: 0.
*		f: !0.
*/
int
findheader(session *sess,
           char *xh,
           char *dst,
           int linelen)
{
	int ret=-1, len=strlen(xh);
	char *s;

	rewind(sess->mailfile);
	for (EVER) {
		s=fgets(dst, linelen, sess->mailfile);
		if (!s)	/* done */
			break;

		for (; *s && isspace((int)*s); s++);
		if (!*s)
			break;

		if (!strncasecmp(dst, xh, len)) {
			ret=0;
			break;
		}
	}

	rewind(sess->mailfile);
	return ret;
}

/*******************************************************************************
*
*	getmailbox
*
*	Pick up mailbox from a string.   This will clean "<>" and
*	comments around the mailbox.
*
*	Input:
*		dst - where to store mailbox.
*		src - where to start pickup.
*	Output:
*		s: 0.
*		f: !0.
*/
int
getmailbox(char *dst,
           char *src)
{
	char *ar, *ap, a;

	SKIPBLANKS(src);
	/* must be able to evaluate:
	**
	**	"Brian Schau" <Brian@Schau.dk>
	**	<Brian@Schau.dk> "Brian Schau"
	**	"Brian Schau" (brian@schau.dk)
	**	(brian@schau.dk) "Brian Schau"
	**	brian (brian@schau.dk)
	**	(brian@schau.dk) brian
	**	Brian@Schau.dk
	**	"Brian Schau" <brian@[194.255.4.52]>
	**	<Brian@[194.255.4.52]> "Brian Schau"
	**	Brian@[194.255.4.52]
	**	Brian
	**	<>
	**
	** Treat anything else as an error.
	*/

	/* look for <> */
	for (ar=src; *ar && *ar!='<'; ar++);
	if (*ar=='<') {
		snip(dst, ar+1, '>');
		return 0;
	}

	/* look for () */
	for (ar=src; *ar && *ar!='('; ar++);
	if (*ar=='(') {
		snip(dst, ar+1, ')');
		return 0;
	}

	/* look for '@' */
	for (ar=src; *ar; ) {
		if (!*ar)
			break;

		if (*ar=='"') {		/* in comment? */
			ar++;
			for (; *ar && *ar!='"'; ar++);
			if (!*ar)
				break;

			ar++;
			continue;
		}

		if (*ar=='@')
			break;

		ar++;
	}

	if (*ar!='@')			/* cannot go on ... */
		return -1;

	for (ap=ar-1; ap>=src && !isspace((int)*ap); ap--);

	if (ap<src)
		ap++;
	
	if (*ap=='<' || *ap=='(')
		a=*ap++;
	else
		a='\x00';

	snip(dst, ap, a);
	return 0;
}

/*******************************************************************************
*
*	snip
*
*	Fixate mailbox in destination.
*
*	Input:
*		dst - destination.
*		src - source.
*		tt - terminating char.
*/
static void
snip(char *dst,
     char *src,
     char tt)
{
	char *ap, save;
	int i;

	for (ap=src; *ap; ap++) {
		if (tt) {
			if (*ap==tt)
				break;
		} else
			if (isspace((int)*ap))
				break;
	}

	save=*ap;
	*ap='\x00';

	if (strlen(src))
		strncpy(dst, src, MAILBOXLEN+1);
	else
		strncpy(dst, "<>", MAILBOXLEN+1);

	*ap=save;

	/* remove whitespaces */
	i=MAILBOXLEN;
	for (ap=dst; *ap && i>0; ap++) {
		i--;
		if (isspace((int)*ap))
			*ap='\x00';
	}
}

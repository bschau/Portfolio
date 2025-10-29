#define MAIN
#include "private.h"

/* globals */
static recipients rcpt;
static int keepmail=1, donemailcalled, handled;
static char *cfn;

/* protos */
static int timeup(int, time_t);
static int errhandler(char *);
static void endmail(void);
static void putstatusmsg(void);

/*******************************************************************************
*
*	donemail
*
*	Cleanup everything ..
*/
void
donemail(void)
{
	if (donemailcalled)
		return;

	endmail();

	donemailcalled=1;
	cfn=NULL;

	if (emf) {
		fclose(emf);
		emf=NULL;
	}

	mxdestroylist(&mxl);
	freerecipients(&rcpt);
	freesession(&gs, keepmail);
	doneinjectmail();
	if (sck>-1)
		close(sck);
}

/*******************************************************************************
*
*	timeup
*
*	Decide if mail should be sent.
*
*	Input:
*		force - force.
*		now - time.
*	Output:
*		send mail: 0.
*		do not send mail: !0.
*	Note:
*		this is really crude.  If the clock skews too much, this
*		goes bananas.
*/
int
timeup(int force,
       time_t now)
{
	if (force)	/* forced mail? */
		return 0;

	if ((int)gs.access<=(int)now)
		return 0;	/* ok, time is up */

	return -1;
}

/*******************************************************************************
*
*	sendmail
*
*	Send a mail.    In fact, a hughie.  :)     Caller must open and lock
*	the control file.
*
*	Input:
*		cntl - control file name.
*		cfd - control file descriptor.
*		db - domain database.
*		force - send file (0 - honour cfn, 1 - force send).
*		verb - echo to stderr.
*	Output:
*		s: 0.
*		f: !0.
*/
int
sendmail(char *cntl,
         int cfd,
         database *db,
         int force,
         int verb)
{
	int err, et, ea, nosend;
	time_t now;
	char *r;

	gs.cntlfd=cfd;
	verbose=verb;
	cfn=cntl;

	time(&now);
	err=checkdequeue(&gs, &rcpt, cfd, cfn, verbose, now, &nosend);
	if (err)
		return 0;

	if (nosend) {
		err=dequeue(&gs, "mail aged - been in queue for too long");
		if (!err) {
			handled=1;
			keepmail=0;
		}

		return 0;
	}

	nosend=timeup(force, now);
	if (nosend)
		return 0;

	elinkopen();
	handled=1;
	for (r=getrecipient(&rcpt); r; r=getrecipient(&rcpt)) {
		erroropen();

		gs.to=r;
		mail(db, r);
		
		et=errorget(EGTYPE);
		ea=errorget(EGACTION);

		switch (et) {
			case ERRNOERR:
				updaterecipient(&rcpt, RSHANDLED);
				break;

			case ERRMAIL:	/* mail format error */
				err=errhandler("%r (%i): %l");
				if (err) {
					/* this is not correct - but we really
					   do not want to loose the return
					   status.  Hopefully, we have more
					   luck next time around ... */
					updaterecipient(&rcpt, RSUNHANDLED);
				} else 
					updaterecipient(&rcpt, RSHANDLED);

				break;

			case ERRSMTP:	/* smtp error */
				/* do nothing at this stage (requeue) */
				if ((int)gs.access==(int)gs.created) {
					if (verbose)
						fprintf(stderr, "mail queued for delivery\n");
					else {
						err=errhandler("%R ... connection or DNS error - mail queued\n");
						if (err)
							qlog(QWARN, "(sendmail) cannot inject status mail (%s)", cfn);
					}
				}
				updaterecipient(&rcpt, RSUNHANDLED);
				break;
		}
	}

	return 0;
}

/*******************************************************************************
*
*	errhandler
*
*	Expands and links error message.
*
*	Input:
*		fmt - format string.
*	Output:
*		s: 0.
*		f: !0.
*/
static int
errhandler(char *fmt)
{
	char errbuf[HUGELINELEN+2];
	int err;

	memset(errbuf, 0, HUGELINELEN+1);
	s2sexpand(&gs, fmt, errbuf, HUGELINELEN);

	err=elinkadd(errbuf);
	if (err) {
		if (!verbose)
			qlog(QWARN, "(errhandler) cannot add elink message");
	}

	return err;
}

/*******************************************************************************
*
*	endmail
*
*	Ends mail session (called from donemail).
*/
static void
endmail(void)
{
	int err, cnt;
	time_t now;

	if (!handled)
		return;

	/* clean up */
	putstatusmsg();

	/* are we done? */
	cnt=countrecipients(&rcpt);
	if (cnt) {
		err=flushrecipients(&rcpt, gs.rcptfile);
		if (err) {
			if (verbose)
				fprintf(stderr, "cannot flush recipients (%s)\n", cfn);
			else
				qlog(QWARN, "(endmail) cannot flush recipients (%s)", cfn);
		}

		time(&now);
		gs.access=((int)now)+(mailretry*60);
		err=putsession(&gs);
		if (err) {
			if (verbose)
				fprintf(stderr, "cannot put session\n");
			else
				qlog(QWARN, "(sendmail) cannot put session (%s)\n", cfn);
		}
	} else
		keepmail=0;
}

/*******************************************************************************
*
*	putstatusmsg
*
*	Flush status message to disk.
*/
static void
putstatusmsg(void)
{
	int err;

	err=elinkstatus();
	if (err) {
		if (!verbose) {
			err=injectmail(&gs, QTXTTRANSACTION);
			if (err)
				qlog(QWARN, "(putstatusmsg) cannot inject status mail (%s)", QTXTTRANSACTION);
		}
	}

	elinkclose();
}

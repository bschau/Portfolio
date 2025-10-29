#include "private.h"

/* globals */
static session es;
static int keeperrsess;

/*******************************************************************************
*
*	doneinjectmail
*
*	Cleanup after injectmail.
*/
void
doneinjectmail(void)
{
	freesession(&es, keeperrsess);
}

/*******************************************************************************
*
*	injectmail
*
*	Injects a mail into the mail queue.
*
*	Input:
*		ccs - session to copycat.
*		mess - message to inject into queue.
*	Output:
*		mail injected: 0.
*		mail not injected: !0.
*/
int
injectmail(session *ccs,
           char *mess)
{
	char *header="Received: (from %P) by %f (" QNAME "); %T\r\nDate: %T\r\nMessage-Id: <%m@%f>\r\nFrom: %P\r\nTo: %R\r\nX-qmta: DSN\r\n";
	char renbuf[CNTLFILENAMELEN+1], mb[(MAILBOXLEN*2)+2];
	int err, cnt;

	if (ccs->messagetype) {
		if (verbose)
			fprintf(stderr, "will not return status message on status message\n");
		else
			qlog(QWARN, "(injectmail) will not return status message on status message");
		return 0;	/* this is ok */
	}

	keeperrsess=0;

	emf=fopenrel(qroot, mess, "r");
	if (!emf)
		return 1;
	
	err=newsession(&es);
	if (err)
		return 1;

	/* setup session defaults */
	es.messagetype=1;
	strncpy(es.from, es.postmaster, MAILBOXLEN+1);
	es.remoteip=ccs->remoteip;
	strncpy(es.remotename, ccs->remotename, DNSFQDNLEN);
	es.remoteaddr=ccs->remoteaddr;
	es.recipients=ccs->recipients;

	err=findheader(ccs, "Errors-To:", line, HUGELINELEN);
	if (err)
		es.to=ccs->from;
	else {
		err=getmailbox(mb, line+10);
		if (err)
			es.to=ccs->from;
		else
			es.to=mb;
	}

	/* write recipients file */
	cnt=snprintf(line, HUGELINELEN, "%c%s\n", RSUNHANDLED, es.to);
	err=writefile(es.rcptfile, line, cnt);
	if (err) {
		if (verbose)
			fprintf(stderr, "cannot write to rcpt file (%i)\n", errno);
		else
			qlog(QWARN, "(injectmail) cannot write to rcpt file (%i)", errno);
		return 1;
	}

	/* write mail file */
	cnt=s2fexpand(&es, header, es.mailfile, 0);
	if (!cnt) {
		if (verbose)
			fprintf(stderr, "cannot write header to mail file\n");
		else
			qlog(QWARN, "(injectmail) cannot write header to mail file");

		return 1;
	}

	es.to=ccs->to;	/* We fake it :) */
	cnt=f2fexpand(&es, emf, es.mailfile, 0);
	if (!cnt) {
		if (verbose)
			fprintf(stderr, "cannot write to mail file\n");
		else
			qlog(QWARN, "(injectmail) cannot write to mail file");

		return 1;
	}
	
	/* setup control stuff */
	err=putsession(&es);
	if (err) {
		if (verbose)
			fprintf(stderr, "cannot put session\n");
		else
			qlog(QWARN, "(injectmail) cannot put session");

		return 1;
	}

	/* make it available for mailing */
	memcpy(renbuf, es.cntlname, CNTLFILENAMELEN);
	renbuf[0]='c';
	renbuf[CNTLFILENAMELEN]='\x00';
	err=rename(es.cntlname, renbuf);
	if (err) {
		if (verbose)
			fprintf(stderr, "cannot rename from %s to %s (%i)\n", es.cntlname, renbuf, errno);
		else
			qlog(QINFO, "(injectmail) cannot rename from %s to %s (%i)", es.cntlname, renbuf, errno);
	}

	keeperrsess=1;

	return 0;
}

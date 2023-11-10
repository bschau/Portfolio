#include "private.h"

/* globals */
static session gs;
static recipients rcpt;
static int infkeep;

/*******************************************************************************
*
*	doneinfected
*
*	Cleanup after 'infected' call.
*/
void
doneinfected(void)
{
	freerecipients(&rcpt);
	freesession(&gs, infkeep);
}

/*******************************************************************************
*
*	infected
*
*	Notification - in case of an infected mail.
*
*	Input:
*		cntl - control file name.
*		msg - message.
*/
void
infected(char *cntl,
         char *msg)
{
	int err;

	infkeep=1;	/* keep original message */

	err=initsession(&gs, cfd, cntl);
	if (err) {
		qlog(QWARN, "(infected) failed to init session (%s)", cntl);
		return;	/* too bad */
	}

	/* setup session defaults */
	err=getsession(&gs, cfd);
	if (err) {
		qlog(QWARN, "(infected) failed to get session (%s)", cntl);
		return;	/* maybe next time? */
	}

	err=findheader(&gs, "X-tv2:", line, HUGELINELEN);
	if (!err) {
		if ((!strncasecmp(line+7, "sendt", 5)) || (!strncasecmp(line+7, "kladde", 6))) {
			infkeep=0;
			return;
		}
	}

	err=loadrecipients(&rcpt, cntl);
	if (err) {
		qlog(QWARN, "(infected) failed to load recipients (%s)", cntl);
		return;	/* darn! */
	}

	gs.recipients=&rcpt;

	/* original session gotten - place reject mail */

	/* BIG FAT WARNING.      Originally every intended recipient in the
	   original mail should be notified.   This is a bad idea (and thus
	   not implemented) as it opens up for a possible Denial-Of-Service
	   attack  (send an infected mail to X recipients = fill the spool-
	   queue :) */

	err=injectmail(&gs, msg);
	if (err) {
		qlog(QWARN, "(infected) failed to inject mail (%s)", QTXTVIRUS);
		return;	/* hmmm ... */
	}

	infkeep=0;	/* kill original message */
}

#include "private.h"

/* globals */
static char recipbuf[MAILBOXLEN+2];

/*******************************************************************************
*
*	loadrecipients
*
*	Loads recipients file.
*
*	Input:
*		rcp - recipients block.
*		cntl - control filename.
*	Output:
*		s: 0.
*		f: !0.
*/
int
loadrecipients(recipients *rcp,
               char *cntl)
{
	char recipfilename[CNTLFILENAMELEN+1];

	memmove(recipfilename, cntl, CNTLFILENAMELEN);
	recipfilename[CNTLFILENAMELEN]='\x00';
	*recipfilename='r';

	rcp->recip=getfile(recipfilename);
	rewindrecipients(rcp);

	if (!(rcp->recip))
		return 1;

	return 0;
}

/*******************************************************************************
*
*	rewindrecipients
*
*	Rewind recipients block.
*
*	Input:
*		rcp - recipients block.
*/
void
rewindrecipients(recipients *rcp)
{
	rcp->rptr=rcp->recip;
}

/*******************************************************************************
*
*	freerecipients
*
*	Free recipients from recipients block.
*
*	Input:
*		rcp - recipients block.
*/
void
freerecipients(recipients *rcp)
{
	if (rcp->recip) {
		free(rcp->recip);
		rcp->recip=NULL;
	}
}

/*******************************************************************************
*
*	getrecipient
*
*	Find next unhandled recipient.
*
*	Input:
*		rcp - recipients block.
*	Output:
*		s: recipient.
*		f: NULL (no more recipients).
*/
char *
getrecipient(recipients *rcp)
{
	char *rp=rcp->rptr, *r;
	int cnt;

	if (!*rp)	/* no more recipients */
		return NULL;

	for (EVER) {
		rcp->rptr=rp;
		r=rp+1;
		FINDEOL(r);
		if (*rp==RSUNHANDLED) {
			/* found recipient */
			cnt=r-rp-1;
			if (cnt>MAILBOXLEN)
				cnt=MAILBOXLEN;	/* this will fail
						   mailtransaction */
			memmove(recipbuf, rp+1, cnt);
			recipbuf[cnt]='\x00';

			return recipbuf;
		}

		rp=r+1;
		if (!*rp)
			break;
	}

	return NULL;
}

/*******************************************************************************
*
*	nextrecipient
*
*	Get next recipient (regardless of status).
*
*	Input:
*		rcp - recipients block.
*		stat - want status as well (0 - no, !0 - yes).
*	Output:
*		s: recipient.
*		f: NULL (no more recipients).
*/
char *
nextrecipient(recipients *rcp,
              int stat)
{
	char *rp=rcp->rptr, *r;
	int cnt;

	if (!*rp)	/* no more recipients */
		return NULL;

	r=rp;
	FINDEOL(r);
	cnt=r-rp;
	if (cnt>MAILBOXLEN)
		cnt=MAILBOXLEN;	/* this will fail mailtransaction */

	if (!stat) {
		rp++;
		cnt--;
	}
	memmove(recipbuf, rp, cnt);
	recipbuf[cnt]='\x00';
	rcp->rptr=++r;

	return recipbuf;
}

/*******************************************************************************
*
*	updaterecipient
*
*	Update status of recipient.
*
*	Input:
*		rcp - recipients block.
*		status - recipient status.
*/
void
updaterecipient(recipients *rcp,
                char status)
{
	char *r=rcp->rptr;

	*(rcp->rptr)=status;
	FINDEOL(r);
	rcp->rptr=++r;
}

/*******************************************************************************
*
*	countrecipients
*
*	Count number of unhandled recipients left.
*
*	Input:
*		rcp - recipients block.
*	Output:
*		recipients left.
*/
int
countrecipients(recipients *rcp)
{
	int cnt=0;
	char *r;

	for (r=rcp->recip; r && *r; ) {
		if (*r==RSUNHANDLED)
			cnt++;

		FINDEOL(r);
		r++;
	}

	return cnt;
}

/*******************************************************************************
*
*	flushrecipients
*
*	Writes (flushes) recipientstatus back to disk.
*
*	Input:
*		rcp - recipients.
*		rff - recipients file.
*	Output:
*		s: 0.
*		f: !0.
*/		
int
flushrecipients(recipients *rcp,
               FILE *rff)
{
	char *r=rcp->recip;
	long fpos=0, ret=0;
	int err;

	for (r=rcp->recip; r && *r; ) {
		err=fseek(rff, fpos, SEEK_SET);
		if (err) {
			ret=-1;
			break;
		}

		/* only touch status byte */
		err=fputc((int)*r, rff);
		if (err==EOF) {
			ret=-1;
			break;
		}

		FINDEOL(r) fpos++;
		r++;
		fpos++;
	}

	rewind(rff);
	return ret;
}

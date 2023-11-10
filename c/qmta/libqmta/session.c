#include "private.h"

/* macros */
#define SESSLINELEN 256
#define SESSIONLOCK 10

/*******************************************************************************
*
*	unlinkall
*
*	Unlink files - used if a file is missing (initsession).
*
*	Input:
*		sess - session.
*/
static void
unlinkall(session *sess)
{
	unlink(sess->cntlname);
	unlink(sess->rcptname);
	unlink(sess->mailname);
	qlog(QWARN, "(unlinkall) damaged session removed");
}

/*******************************************************************************
*
*	macroize
*
*	Fill in some basic information.
*
*	Input:
*		sess - session.
*/
void
macroize(session *sess)
{
	int err;
	char *p, *src;

	memset(sess->hostname, 0, DNSPARTLEN+1);
	memset(sess->domainname, 0, DNSPARTLEN+1);
	memset(sess->fqdn, 0, DNSFQDNLEN+1);
	memset(sess->postmaster, 0, MAILBOXLEN+1);

	if (relayname) {
		/* %h macro */
		p=strchr(relayname, '.');
		if (p)
			err=p-relayname;
		else 
			err=strlen(relayname);
		
		if (err>DNSPARTLEN)
			err=DNSPARTLEN;
		memmove(sess->hostname, relayname, err);

		/* %d macro */
		if (p)
			src=p+1;
		else
			src="localdomain";

		err=strlen(src);
		if (err>DNSPARTLEN)
			err=DNSPARTLEN;
		memmove(sess->domainname, src, err);
	} else {	/* guess */
		/* %h macro */
		gethostname(sess->hostname, DNSPARTLEN);

		/* %d macro */
		getdomainname(sess->domainname, DNSPARTLEN);
	}

	err=strlen(sess->hostname);
	if (!err)	/* blank */
		memcpy(sess->hostname, "localhost.", 9);

	err=strlen(sess->domainname);
	if (!err)	/* blank */
		memcpy(sess->domainname, "localdomain", 11);

	/* %f macro */
	strncpy(sess->fqdn, sess->hostname, DNSPARTLEN+1);
	strcat(sess->fqdn, ".");
	strncat(sess->fqdn, sess->domainname, DNSPARTLEN+1);

	/* %P macro */
	memmove(sess->postmaster, "Postmaster@", 11);
	strcat(sess->postmaster, sess->fqdn);
}

/*******************************************************************************
*
*	newsession
*
*	Create a new session.
*
*	Input:
*		sess - session block.
*	Output:
*		s: 0.
*		f: !0.
*/
int
newsession(session *sess)
{
	*sess->from='\x00';
	sess->messagetype=0;
	sess->recipients=NULL;
	macroize(sess);
	time(&sess->created);
	sess->access=sess->created;
	sess->dequeue=sess->created+(maxage*3600);

	sess->cntlname=strdup("SXXXXXX");
	if (sess->cntlname) {
		sess->cntlfd=mkstemp(sess->cntlname);
		if (sess->cntlfd>-1) {
			sess->rcptname=strdup(sess->cntlname);
			sess->mailname=strdup(sess->cntlname);
			if (sess->rcptname && sess->mailname) {
				*(sess->rcptname)='r';
				*(sess->mailname)='m';
				sess->rcptfile=fopen(sess->rcptname, "w");
				sess->mailfile=fopen(sess->mailname, "w");
				if (sess->rcptfile && sess->mailfile)
					return 0;	/* success */
			}
		}
	}

	return 1;
}

/*******************************************************************************
*
*	initsession
*
*	Inits (opens) a previously created session.
*
*	Input:
*		ss - session.
*		cfd - session file descriptor.
*		cfn - session file name.
*	Output:
*		s: 0.
*		f: !0.
*/
int
initsession(session *ss,
            int cfd,
            char *cfn)
{
	struct stat sbuf;

	ss->cntlfd=cfd;
	ss->recipients=NULL;
	ss->cntlname=strdup(cfn);
	ss->rcptname=strdup(cfn);
	ss->mailname=strdup(cfn);
	if (ss->cntlname && ss->rcptname && ss->mailname) {
		*(ss->rcptname)='r';
		*(ss->mailname)='m';

		sbuf.st_size=0;
		stat(ss->rcptname, &sbuf);
		if (!sbuf.st_size) {
			qlog(QWARN, "rcptname null");
			unlinkall(ss);
			return -1;
		}

		sbuf.st_size=0;
		stat(ss->mailname, &sbuf);
		if (!sbuf.st_size) {
			qlog(QWARN, "mailname null");
			unlinkall(ss);
			return -1;
		}

		ss->rcptfile=fopen(ss->rcptname, "r+");
		ss->mailfile=fopen(ss->mailname, "r+");
		if (ss->rcptfile && ss->mailfile) {
			macroize(ss);
			return 0;
		}
	}

	return 1;
}

/*******************************************************************************
*
*	freesession
*
*	Deallocates and zeros session.
*
*	Input:
*		sess - session block.
*		keep - keep files (0 - no, !0 - yes).
*/
void
freesession(session *sess,
            int keep)
{
	if ((sess->cntlfd)>-1) {
		close(sess->cntlfd);
		sess->cntlfd=-1;
	}

	if (sess->rcptfile) {
		fclose(sess->rcptfile);
		sess->rcptfile=NULL;
	}

	if (sess->mailfile) {
		fclose(sess->mailfile);
		sess->mailfile=NULL;
	}

	if (!keep) {
		if (sess->cntlname)
			unlink(sess->cntlname);
		if (sess->rcptname)
			unlink(sess->rcptname);
		if (sess->mailname)
			unlink(sess->mailname);
	}

	safefree(&sess->cntlname);
	safefree(&sess->rcptname);
	safefree(&sess->mailname);
}

/*******************************************************************************
*
*	putsession
*
*	Populate control file.
*
*	Input:
*		sess - session block.
*	Output:
*		s: 0.
*		f: !0.
*/
int
putsession(session *sess)
{
	int sl=60, loop, cnt, err;
	char sessline[SESSLINELEN+2];

	cnt=snprintf(sessline, SESSLINELEN, "A%i\nC%i\nD%i\nF%s\nM%i\n", (int)sess->access, (int)sess->created, (int)sess->dequeue, sess->from, sess->messagetype);

	for (loop=0; loop<SESSIONLOCK; loop++) {
		lseek(sess->cntlfd, 0, SEEK_SET);
		err=writefd(sess->cntlfd, sessline, cnt);
		if (!err)
			return 0;

		sl+=sl;
	}

	return 1;
}

/*******************************************************************************
*
*	getsession
*
*	Get and parse session file.
*
*	Input:
*		sess - session block.
*		fd - control file descriptor.
*	Output:
*		s: 0.
*		f: !0.
*/
int
getsession(session *sess,
           int fd)
{
	char cfile[HUGELINELEN+2];
	char *ptr=cfile, *tmp;
	int err;

	lseek(fd, 0, SEEK_SET);

	memset(cfile, 0, HUGELINELEN+1);
	err=read(fd, cfile, HUGELINELEN);
	if (err==-1)
		return -1;

	for (EVER) {
		switch (*ptr) {
			case 'A':
				tmp=++ptr;
				FINDEOL(ptr);
				*ptr++='\x00';
				sess->access=atoi(tmp);
				break;
			case 'C':
				tmp=++ptr;
				FINDEOL(ptr);
				*ptr++='\x00';
				sess->created=atoi(tmp);
				break;
			case 'D':
				tmp=++ptr;
				FINDEOL(ptr);
				*ptr++='\x00';
				sess->dequeue=atoi(tmp);
				break;
			case 'F':
				tmp=++ptr;
				FINDEOL(ptr);
				*ptr++='\x00';
				strncpy(sess->from, tmp, MAILBOXLEN+1);
				break;
			case 'M':
				tmp=++ptr;
				FINDEOL(ptr);
				*ptr++='\x00';
				sess->messagetype=atoi(tmp);
				break;
			default:
				if (!*ptr)
					return 0;
				else {
					tmp=ptr;
					FINDEOL(ptr);
				}
		}
	}
}

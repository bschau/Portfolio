#include "private.h"

/* macros */
#define SMTPSTATEHELOGIVEN 1
#define SMTPSTATEMAILGIVEN 2
#define SMTPSTATERCPTGIVEN 3
#define PEERNAMEIP sess->remotename, sess->remoteaddr

/* protos */
static char *fixhelodomain(char *);
static void initattack(void);
static int smtpattack(int);
static int checkargs(char *, char *, char *, char *);
static int smtpcmdhelo(char *);
static int smtpcmdmail(char *);
static int smtpcmdrcpt(char *);
static int smtpcmddata(char *);
static int smtpcmdrset(char *);
static int smtpcmdnoop(char *);
static int smtpcmdquit(char *);
static int smtpcmdhelp(char *);
static int smtpcmdehlo(char *);
static int mailfrom(void);
static int mailboxstatus(char *);
static int dblookup(char *);
static int rcptto(char *, char *);

/* globals */
static struct {
	char *cmd;
	int (*action)();
	char **helptext;
	int seen;
	int maxseen;
	int sleep;
} mailcmds[]= {
	{ "RCPT", smtpcmdrcpt, &smtphelprcpt, -1, 0, -1 },
	{ "HELO", smtpcmdhelo, &smtphelphelo, -1, 3, -1 },
	{ "MAIL", smtpcmdmail, &smtphelpmail, -1, 3, -1 },
	{ "DATA", smtpcmddata, &smtphelpdata, -1, 3, -1 },
	{ "QUIT", smtpcmdquit, &smtphelpquit, -1, 0, -1 },
	{ "RSET", smtpcmdrset, &smtphelprset, -1, 5, -1 },
	{ "NOOP", smtpcmdnoop, &smtphelpnoop, -1, 5, -1 },
	{ "HELP", smtpcmdhelp, &smtphelphelp, -1, 10, -1 },
	{ "EHLO", smtpcmdehlo, NULL, -1, 2, -1 },
	{ NULL, NULL, NULL, -1, 3, -1 },
};

static int ever=1, state, rcptsgiven, messsize;
static child *ch;
static session *sess;

/*******************************************************************************
*
*	smtp
*
*	Handles connection - speaks smtp.
*
*	Input:
*		c - child info block.
*	Output:
*		s: 0.
*		f: !0.
*/
int
smtp(child *c)
{
	int err, cnt;
	char *args;

	ch=c;
	sess=c->sess;

	/* update smtpattack for RCPT */
	initattack();
	for (cnt=0; mailcmds[cnt].cmd; cnt++) {
		if (!(strncasecmp(mailcmds[cnt].cmd, "RCPT", 4))) {
			mailcmds[cnt].maxseen=maxrecipients+3;
			break;
		}
	}
			
	err=NETSTATUSW(smtp220);
	if (err) 
		return 1;

	for (;ever;) {
		cnt=ngetline(ch->fd, line, HUGELINELEN-1);
		if (cnt<=0)	/* client went away */
			return 1;

		line[cnt]='\x00';

		for (cnt=0; mailcmds[cnt].cmd; cnt++) {
			if (!(strncasecmp(mailcmds[cnt].cmd, line, 4))) {
				err=smtpattack(cnt);
				if (err)	/* we're under attack - abort */
					return 1;

				args=line+4;
				SKIPBLANKS(args);
				if (!*args)
					args=NULL;

				ch->response=NULL;
				err=(*mailcmds[cnt].action)(args);
				if (err) 
					return 1;

				break;
			}
		}

		if (!(mailcmds[cnt].cmd)) {
			/* dirty talk */
			qlog(QSMTP, "(smtp) %s (%s) unknown command '%s'", PEERNAMEIP, line);

			err=smtpattack(cnt);	/* under attack? */
			if (err)
				return 1;

			/* oops, illegal command */
			err=NETSTATUSW(smtp500unknowncommand);
			if (err)  /* client went away */
				return 1;
		}
	}

	return 0;
}

/*******************************************************************************
*
*	fixhelodomain
*
*	Sets helo domain in child structure.
*
*	Input:
*		args - args pointer.
*	Output:
*		args pointer after domain.
*/
static char *
fixhelodomain(char *args)
{
	char *ar;
	char save;

	for (ar=args; *ar && !isspace((int)*ar); ar++);

	save=*ar;
	*ar='\x00';

	ch->helodomain=strdup(args);
	*ar=save;

	return ar;
}

/*******************************************************************************
*
*	initattack
*
*	Initialize attack table.
*/
static void
initattack(void)
{
	int cnt;

	for (cnt=0; mailcmds[cnt].cmd; cnt++) {
		mailcmds[cnt].seen=0;
		mailcmds[cnt].sleep=1;
	}

	mailcmds[cnt].seen=0;
	mailcmds[cnt].sleep=1;
}

/*******************************************************************************
*
*	smtpattack
*
*	Try to detect smtpattack.
*
*	Algorithm (unfair) is:
*
*		if we see a command more than 'maxseen' times, we
*		sleep for 'sleep' seconds.   'sleep' doubles each
*		additional time (1, 2, 4, 8, ...).
*		When 'sleep' reaches 256 we exit.
*
*	Input:
*		cnt - entry in mailcmd table.
*	Output:
*		stay in session: 0.
*		abort session: !0.
*/
static int
smtpattack(int cnt)
{
	if (!mailcmds[cnt].maxseen)
		return 0;

	if (mailcmds[cnt].seen<mailcmds[cnt].maxseen) {
		mailcmds[cnt].seen++;
		return 0;
	}

	nsleep(mailcmds[cnt].sleep, 0);
	if (mailcmds[cnt].sleep==1)
		qlog(QINFO, "(smtpattack) possible smtp attack from %s (%i)", PEERNAMEIP);

	mailcmds[cnt].sleep<<=1;
	if (mailcmds[cnt].sleep>256) {
		qlog(QINFO, "(smtpattack) aborting session from %s (%i) - under attack", PEERNAMEIP);
		NETSTATUSW(smtp221);
		return 1;
	}

	return 0;
}

/*******************************************************************************
*
*	checkargs
*
*	Validate args pointer (if necessary).
*
*	Input:
*		args - args.
*		needs - what args need to follow.
*		caller - calling routine.
*		func - function.
*	Output:
*		args ok: 0.
*		args not ok: !0.
*/
static int
checkargs(char *args,
          char *needs,
          char *caller,
          char *func)
{
	if (args) {
		if (!strncasecmp(args, needs, strlen(needs)))
			return 0;
	}

	qlog(QSMTP, "(checkargs) (%s) %s (%s) %s, but no %s", caller, PEERNAMEIP, func, needs);
	NETSTATUSW(smtp501argswrong);
	return 1;
}

/*******************************************************************************
*
*	smtpcmdrcpt (RCPT TO:)
*
*	Collect recipients.
*
*	Input:
*		args - pointer to 1st char in args or NULL if no args.
*	Output:
*		s: 0.
*		f: !0.
*	States:
*		S: 250, 251.
*		F: 450, 451, 452, 550, 551, 552, 553.
*		E: 421, 500, 501, 503.
*/
static int
smtpcmdrcpt(char *args)
{
	char mb[(MAILBOXLEN*2)+2];
	char *user, *domain;
	int err, cnt;

	err=checkargs(args, "TO:", "smtpcmdrcpt", "rcpt");
	if (err)
		return 0;	/* stay in session */

	args+=3;

	if (state<SMTPSTATEMAILGIVEN) {
		qlog(QSMTP, "(smtpcmdrcpt) %s (%s) needs mail from", PEERNAMEIP);
		return NETSTATUSW(smtp503needsmail);
	}

	if (rcptsgiven>=maxrecipients) {
		qlog(QSMTP, "(smtpcmdrcpt) %s (%s) too many recipients", PEERNAMEIP);
		return NETSTATUSW(smtp552toomanyrecipients);
	}

	/* get rcpt */
	memset(mb, 0, sizeof(mb));
	err=getmailbox(mb, args);
	if (err) {
		qlog(QSMTP, "(smtpcmdrcpt) %s (%s) bogus mailbox", PEERNAMEIP);
		ever=0;
		return NETSTATUSW(smtp452bogusmailbox);
	}

	/* break it up in user/domain part */
	user=mb;
	for (domain=mb; *domain && *domain!='@'; domain++);
	if (*domain!='@') {
		domain++;
		strncat(domain, sess->fqdn, MAILBOXLEN);
	} else
		*domain++='\x00';

	/* check to see if recipient is valid */
	err=rcptto(user, domain); 
	if (err==DENY) {
		qlog(QSMTP, "(smtpcmdrcpt) %s (%s) denying rcpt to %s (%s)", PEERNAMEIP, mb, ((ch->response) ? ch->response : smtp550norelay));
		if (ch->response)
			return NETSTATUSW(ch->response);
		else
			return NETSTATUSW(smtp550norelay);
	}

	*(domain-1)='@';

	cnt=snprintf(line, HUGELINELEN, "%c%s\n", RSUNHANDLED, mb);
	err=writefile(sess->rcptfile, line, cnt);
	err|=fflush(sess->rcptfile);
	if (err) {
		qlog(QINFO, "(smtpcmdrcpt) cannot write to rcpt file (%i)", errno);
		ever=0;
		return NETSTATUSW(smtp452syserr);
	}

	if (logrecipients)
		qlog(QSMTP, "[s%s] from=%s to=%s", (sess->cntlname)+1, sess->from, mb);

	state=SMTPSTATERCPTGIVEN;
	rcptsgiven++;
	return NETSTATUSW(smtp250);
}

/*******************************************************************************
*
*	smtpcmdhelo (HELO)
*
*	Introduce yourself.
*
*	Input:
*		args - pointer to 1st char in args or NULL if no args.
*	Output:
*		s: 0.
*		f: !0.
*	States:
*		S: 250.
*		E: 421, 500, 501, 504.
*/
static int
smtpcmdhelo(char *args)
{
	int err;

	if (state==SMTPSTATEHELOGIVEN) {
		qlog(QSMTP, "(smtpcmdhelo) %s (%s) helo already given", PEERNAMEIP);

		return NETSTATUSW(smtp501helogiven);
	}

	if (!args) {
		qlog(QSMTP, "(smtpcmdhelo) %s (%s) helo without domain", PEERNAMEIP);

		err=NETSTATUSW(smtp501nodomain);
	} else {
		fixhelodomain(args);
		err=validdomain(ch->helodomain);
		if (err) {
			free(ch->helodomain);
			qlog(QSMTP, "(smtpcmdhelo) %s (%s) invalid helo domain", PEERNAMEIP);
			ch->helodomain=NULL;
			err=NETSTATUSW(smtp501invalid);
		} else {
			state=SMTPSTATEHELOGIVEN;
			err=NETSTATUSW(smtp250);
		}
	}
	return err;
}

/*******************************************************************************
*
*	smtpcmdmail (MAIL FROM:)
*
*	Collect sender address.
*
*	Input:
*		args - pointer to 1st char in args or NULL if no args.
*	Output:
*		s: 0.
*		f: !0.
*	States:
*		S: 250.
*		F: 451, 452, 552.
*		E: 421, 500, 501.
*	Notes:
*		This function differs from rfc821 in that it will return 503
*		and 550 codes.
*/
static int
smtpcmdmail(char *args)
{
	int err;

	err=checkargs(args, "FROM:", "smtpcmdmail", "mail");
	if (err)
		return 0;	/* stay in session */

	args+=5;

	if (state>SMTPSTATEHELOGIVEN) {
		qlog(QSMTP, "(smtpcmdmail) %s (%s) trying to change sender address", PEERNAMEIP);
		return NETSTATUSW(smtp503mailgiven);
	}

	if (state!=SMTPSTATEHELOGIVEN)
		qlog(QSMTP, "(smtpcmdmail) %s (%s) did not say hello", PEERNAMEIP);

	/* new session */
	err=createsession(ch);
	if (err) {
		qlog(QINFO, "(smtpcmdmail) cannot create session");
		return NETSTATUSW(smtp452syserr);
	}

	/* get sender */
	err=getmailbox(sess->from, args);
	if (err) {
		qlog(QINFO, "(smtpcmdmail) cannot get mailbox");
		err=NETSTATUSW(smtp452syserr);
		ever=0;
	} else {
		err=mailfrom();
		if (err==DENY) {
			qlog(QSMTP, "(smtpcmdmail) %s (%s) denying mail from %s (%s)", PEERNAMEIP, sess->from, ((ch->response) ? ch->response : smtp550norelay));
			if (ch->response)
				err=NETSTATUSW(ch->response);
			else
				err=NETSTATUSW(smtp550norelay);
		} else {
			state=SMTPSTATEMAILGIVEN;
			err=NETSTATUSW(smtp250);
		}
	}

	return err;
}

/*******************************************************************************
*
*	smtpcmddata (DATA)
*
*	Collect mail body, headers and attachments.  The works.
*
*	Input:
*		args - pointer to 1st char in args or NULL if no args.
*	Output:
*		s: 0.
*		f: !0.
*	States:
*		I: 354 -> data -> S:250 or F: 451, 452, 552, 554.
*		F: 451, 554.
*		E: 421, 500, 501, 503.
*/
static int
smtpcmddata(char *args)
{
	int mailok=0, couldntupdatemail=1, mailtoobig=1, seenfrom=0, ms;
	char fakemailbox[MAILBOXLEN+2];
	char renamebuffer[CNTLFILENAMELEN+1];
	char ebuf[HUGELINELEN+2];
	int err, cnt;
	time_t tt;

	if (state==SMTPSTATERCPTGIVEN) {
		/* write time stamp */
		memset(ebuf, 0, HUGELINELEN+1);
		cnt=s2sexpand(sess, "Received: from %r ([%i]) by %f (" QNAME "); %T\r\n", ebuf, HUGELINELEN);
		err=writefile(sess->mailfile, ebuf, cnt);
		if (err) {
			qlog(QINFO, "(smtpcmddata) cannot update mail file");
			return NETSTATUSW(smtp452syserr);
		}

		/* ok, accept mail */
		err=NETSTATUSW(smtp354);
		if (err) {
			qlog(QINFO, "(smtpcmddata) sudden death");
			return err;
		}

		/* loop until:
			a single dot is encountered
			or until we run out of space */

		if (maxmailsize)
			ms=maxmailsize+1;
		else
			ms=0;

		for (EVER) {
			cnt=ngetline(ch->fd, line, HUGELINELEN);
			if (cnt<=0) {
				qlog(QINFO, "(smtpcmddata) sudden death");
				return -1;
			}

			line[cnt]='\x00';
			/* 3 chars as pr. spec, 2 chars - rest of the world */
			if ((line[0]=='.' && line[1]=='\r' && line[2]=='\n') ||
			    (line[0]=='.' && line[1]=='\n')) {
				mailok=1;
				break;
			}

			/* hunt for sender */
			if (!seenfrom) {
				if (strlen(line)==2 && line[0]=='\r' && line[1]=='\n')
					seenfrom=1;	/* header stop */
				else {
					if (!strncasecmp(line, "From:", 5)) {
						err=getmailbox(fakemailbox, line+5);
						if (!err)
							strcpy(sess->from, fakemailbox);
						seenfrom=1;
					}
				}
			}

			if (ms) {
				if (cnt>ms) {	/* no more storage */
					if (mailtoobig) {
						qlog(QSMTP, "(smtpcmddata) %s (%s) mail too big", PEERNAMEIP);
						mailtoobig=0;
					}
					cnt=ms;
				}
			}

			err=writefile(sess->mailfile, line, cnt);
			if (err) {
				if (couldntupdatemail) {
					qlog(QINFO, "(smtpcmddata) cannot update mail file");
					couldntupdatemail=0;
				}
			}
			messsize+=cnt;
		}

		err=fflush(sess->mailfile);
		if (err) {
			qlog(QINFO, "(smtpcmddata) cannot flush %s", sess->cntlname);
			mailok=0;
		}

		sess->messagetype=0;
		err=putsession(sess);
		if (err) {
			qlog(QINFO, "(smtpcmddata) cannot put session %s", sess->cntlname);
			mailok=0;
		}

		if (mailok) {
			ch->validmail=1;
			memcpy(renamebuffer, sess->cntlname, CNTLFILENAMELEN);
			renamebuffer[0]='s';
			renamebuffer[CNTLFILENAMELEN]='\x00';
			err=rename(sess->cntlname, renamebuffer);
			if (err) {
				qlog(QINFO, "(smtpcmddata) cannot rename from %s to %s", sess->cntlname, renamebuffer);
				err=NETSTATUSW(smtp452syserr);
			} else {
				*(sess->cntlname)='s';
				err=NETSTATUSW(smtp250);
				time(&tt);
				qlog(QSMTP, "[%s] from=%s recipients=%i messagesize=%i time=%i", renamebuffer, sess->from, rcptsgiven, messsize, tt-(ch->startconn));
				ch->startconn=tt;
				initattack();
			}
			destroysession(ch);
			/* in the future we might to TURN here :) */
		} else {
			err=NETSTATUSW(smtp554tfmtl);
			ever=0;
		}
	} else {
		qlog(QSMTP, "(smtpcmddata) %s (%s) rcpt to not given", PEERNAMEIP);
		err=NETSTATUSW(smtp503norecips);
	}

	return err;
}

/*******************************************************************************
*
*	smtpcmdrset (RSET)
*
*	Reset session.  Mail data and reciepients must be cleared.   State
*	must be set to start of session (like we've just met!)
*
*	Input:
*		args - pointer to 1st char in args or NULL if no args.
*	Output:
*		s: 0.
*		f: !0.
*	States:
*		S: 250.
*		E: 421, 500, 501, 504.
*/
static int
smtpcmdrset(char *args)
{
	int err;

	rcptsgiven=0;
	messsize=0;

	destroysession(ch);
	err=NETSTATUSW(smtp250);
	state=SMTPSTATEHELOGIVEN;

	return err;
}

/*******************************************************************************
*
*	smtpcmdnoop (NOOP)
*
*	No operation.
*
*	Input:
*		args - pointer to 1st char in args or NULL if no args.
*	Output:
*		s: 0.
*		f: !0.
*	States:
*		S: 250.
*		E: 421, 500.
*/
static int
smtpcmdnoop(char *args)
{
	return NETSTATUSW(smtp250);
}

/*******************************************************************************
*
*	smtpcmdquit (QUIT)
*
*	Quit.   Ends mailsession.
*
*	Input:
*		args - pointer to 1st char in args or NULL if no args.
*	Output:
*		s: 0.
*		f: !0.
*	States:
*		S: 221.
*		E: 500.
*/
static int
smtpcmdquit(char *args)
{
	ever=0;
	return NETSTATUSW(smtp221);
}

/*******************************************************************************
*
*	smtpcmdhelp (HELP)
*
*	Display helptext.   This doesn't change status.
*
*	Input:
*		args - pointer to 1st char in args or NULL if no args.
*	Output:
*		s: 0.
*		f: !0.
*	States:
*		S: 211, 214.
*		E: 421, 500, 501, 502, 504.
*/
static int
smtpcmdhelp(char *args)
{
	char *egoboost="214-" QNAME "\r\n214-This mail gateway was designed and created by Brian Schau,\r\n214 Anti Virus technology supplied by Sophos.\r\n";

	int cnt;

	if (args) {
		for (cnt=0; mailcmds[cnt].cmd; cnt++) {
			if (strncasecmp(mailcmds[cnt].cmd, args, 4))
				continue;

			if (!mailcmds[cnt].helptext)
				continue;

			return nwrite(ch->fd, *mailcmds[cnt].helptext, strlen(*mailcmds[cnt].helptext));
		}

		/* special case :) */
		if (!(strncasecmp(args, "egoboost", 8)))
			return nwrite(ch->fd, egoboost, strlen(egoboost));
		else {
			qlog(QSMTP, "(smtpcmdhelp) %s (%s) no help on that topic", PEERNAMEIP);

			return nwrite(ch->fd, smtphelpnohelp, strlen(smtphelpnohelp));
		}
	} else
		qlog(QSMTP, "(smtpcmdhelp) %s (%s) help?", PEERNAMEIP);

	return nwrite(ch->fd, smtphelp, strlen(smtphelp));
}

/*******************************************************************************
*
*	smtpcmdehlo
*
*	Introduce yourself (the advanced way).    This command is NOT
*	supported but it is issued to frequent to be ignored.
*
*	Input:
*		args - pointer to 1st char in args or NULL if no args.
*	Output:
*		s: 0.
*		f: !0.
*/
static int
smtpcmdehlo(char *args)
{
	return NETSTATUSW(smtp500ehlo);
}

/*******************************************************************************
*
*	mailfrom
*
*	Extra checks to be done on 'MAIL FROM:' command.
*
*	Output:
*		accept this sender: ACCEPT.
*		deny this sender: DENY.
*/
static int
mailfrom(void)
{
	mxnode *m=NULL;
	int err, domainok;
	char *dom, *d;

	/* this "kludge" as pr. rfc1123, 5.2.9 is _stupid_ */
	if (!strcmp(sess->from, "<>"))
		return ACCEPT;

	/* is mailbox format valid? */
	ch->response=smtp553mbformaterror;
	err=validmailbox(sess->from);
	if (err)	
		return DENY;

	/* is sender blacklisted in spammer db? */
	if (ch->spammerdb.f) {
		err=qdbfetch(&ch->spammerdb, sess->from);
		if (!err) {
			if (ch->spammerdb.dptr) {
				ch->response=ch->spammerdb.dptr;
				return DENY;
			}
		}
	}

	/* is domain blacklisted in access db? */
	dom=getdomain(sess->from);
	domainok=mailboxstatus(dom);
	switch (domainok) {
		case ACCEPT:
			return ACCEPT;
		case DENY:
			ch->response=smtp550blacklisted;
			return DENY;
	}

	/* does domain name resolve? */
	d=islocal(&ch->domaindb, dom);
	if (!d) {
		m=mxlookup(dom);
		if (m) {
			mxdestroylist(&m);
			return ACCEPT;
		}
	}

	ch->response=smtp501nosuchsenderdomain;
	return DENY;
}

/*******************************************************************************
*
*	mailboxstatus
*
*	Should we deny or accept mail from (based on databases).
*
*	Input:
*		domain - domainpart of mailbox.
*	Output:
*		accept this sender: ACCEPT.
*		deny this sender: DENY.
*/
static int
mailboxstatus(char *domain)
{
	char *m;
	int stat;

	if (!ch->accessdb.f)
		return ACCEPT;

	/* mailbox lookup */
	stat=dblookup(sess->from);
	if (stat!=NONE) {
		qlog(QDEBUG, "(mailboxstatus) %s: %s", sess->from, ((stat==ACCEPT) ? "accepted" : "denied"));
		return stat;
	}

	/* domain lookup */
	m=domain;
	for (EVER) {
		stat=dblookup(m);
		if (stat!=NONE) {
			qlog(QDEBUG, "(mailboxstatus) %s (%s): %s", sess->from, m, ((stat==ACCEPT) ? "accepted" : "denied"));
			return stat;
		}
		m=strchr(m, '.');
		if (m)
			m++;
		else
			break;
	}

	return ACCEPT;
}

/*******************************************************************************
*
*	dblookup
*
*	Look up entry in access database.
*
*	Input:
*		key - key.
*	Output:
*		accept this sender: ACCEPT.
*		deny this sender: DENY.
*		I have no idea: NONE.
*/
static int
dblookup(char *key)
{
	int err;

	err=qdbfetch(&ch->accessdb, key);
	if (err)
		return NONE;

	if (ch->accessdb.dptr) {
		switch (*ch->accessdb.dptr) {
			case '+':
				return ACCEPT;
			case '-':
				return DENY;
			default:
				qlog(QINFO, "(dblookup) illegal value for %s in access database", key);
				break;
		}
	}

	return NONE;
}

/*******************************************************************************
*
*	rcptto
*
*	Extra checks to be done on 'RCPT TO:' command.
*
*	Input:
*		user - intended user.
*		hd - host/domain.
*	Output:
*		accept this recipient: ACCEPT.
*		deny this recipient: DENY.
*/
static int
rcptto(char *user,
       char *hd)
{
	char *dom, *peer, *src, *dst;
	mxnode *m;
	struct hostent *he;
	int err;

	/* user part ok? */
	ch->response=smtp553mbformaterror;
	if (!*user)
		return DENY;

	/* domain part ok? */
	err=validdomain(hd);
	if (err)
		return DENY;

	/* destination domain known? */
	dst=islocal(&ch->domaindb, hd);
	if (!dst) {
		m=mxlookup(hd);
		if (m)		/* succeeded */
			mxdestroylist(&m);
		else {
			/* maybe on ip-address */
			he=gethostbyname(hd);
			if (!he) {	/* mailbox does not exist - accept
					   mail anyway so a message will be
					   returned to sender */
				return ACCEPT;
			}
		}
	}

/* 	ok to relay?
*
*	If peer is local (!NULL) then everthing is ok.
*	If peer is non-local (NULL) then
*
*		if (src==NULL && dst==!NULL) RELAY   # internet to me
*		else NO-RELAY
*/
	ch->response=NULL;
	peer=islocal(&ch->domaindb, sess->remotename);
	if (peer)
		return ACCEPT;

	dom=getdomain(sess->from);
	src=islocal(&ch->domaindb, dom);

	if (!src && dst)
		return ACCEPT;

	ch->response=smtp550relayingdenied;
	return DENY;
}

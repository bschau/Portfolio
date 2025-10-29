#include "private.h"

/* structs */
struct ipa {
	union {
		bit32 addr;
		char *dns;
	} t;
	int type;
};

/* macros */
#define IPAADDR 0
#define IPADNS 1
#define CONNECTIONATTEMPTS 5

/* globals */
static struct ipa ipaddrs[IPADDRSPRDOMAIN];
static int ipcnt;
static unsigned short dstport;

/* protos */
static int hostconnect(session *, int);
static int hconnect(int, struct sockaddr *, int);
static int local(database *, char *);
static int getipaddress(database *, char *);

/*******************************************************************************
*
*	hostconnect
*
*	Connect to destination host on smtpport (usually port 25).
*
*	Input:
*		ss - session.
*		ipc - ip count (index into ipaddrs).
*	Output:
*		s: 0.
*		f: -2, resolve error.
*		   -1, connection error.
*/
static int
hostconnect(session *ss,
            int ipc)
{
	int err=-1, len;
	struct hostent *he;
	struct sockaddr_in addr;
	char **addrs;

	addr.sin_family=AF_INET;
	addr.sin_port=dstport;
	len=sizeof(addr);
	if (ipaddrs[ipc].type==IPADNS) {
		/* dns resolve name */
		endhostent();
		he=gethostbyname(ipaddrs[ipc].t.dns);
		if (!he)
			return -2;

		addrs=he->h_addr_list;
		while (*addrs) {
			addr.sin_addr.s_addr=(uint32_t)inet_addr(inet_ntoa(*(struct in_addr *)*addrs));
			if (verbose)
				fprintf(stderr, "connecting to %s (%s) on port %i ...\n", ipaddrs[ipc].t.dns, inet_ntoa(addr.sin_addr), htons(dstport));

			resolveip(ss, addr.sin_addr.s_addr, ipaddrs[ipc].t.dns);
			err=hconnect(sck, (struct sockaddr *)&addr, len);
			if (!err)
				break;
	
			addrs++;
		}
	} else {
		addr.sin_addr.s_addr=(uint32_t)ipaddrs[ipc].t.addr;
		if (verbose)
			fprintf(stderr, "connecting to %s on port %i ...\n", inet_ntoa(addr.sin_addr), htons(dstport));

		resolveip(ss, addr.sin_addr.s_addr, NULL);
		err=hconnect(sck, (struct sockaddr *)&addr, len);
	}

	if (err==-1)
		return -1;

	return 0;
}

/*******************************************************************************
*
*	hconnect
*
*	Attempt (up to CONNECTIONATTEMPTS times) to connect to this host.
*	Double sleep period between attempts.
*
*	Input:
*		sck - socket.
*		addr - socket address.
*		len - length of addr.
*	Output:
*		s: 0.
*		f: !0.
*/
static int
hconnect(int sck,
         struct sockaddr *addr,
         int len)
{
	int sl=5, cnt, err;

	for (cnt=0; cnt<CONNECTIONATTEMPTS; cnt++) {
		err=connect(sck, addr, len);
		if (err==-1) {
			sl<<=1;		/* sleep */
			nsleep(sl, 0);
		} else
			return 0;		/* connected */
	}

	return -1;	/* connection failed */
}

/*******************************************************************************
*
*	local
*
*	Retrieve local ip address(es) if host/domain is local.
*
*	Input:
*		db - domain database.
*		hd - host/domain.
*	Output:
*		domain local: !0.
*		domain not local: 0.
*/
static int
local(database *db,
      char *hd)
{
	char *d;
	int cnt;
	bit32 *bp;

	d=islocal(db, hd);
	if (!d)
		return 0;

	/* fill in ipaddrs */
	bp=(bit32 *)d;
	cnt=(db->dlen)/sizeof(bit32);
	for (; cnt>0 && ipcnt<IPADDRSPRDOMAIN; cnt--) {
		ipaddrs[ipcnt].t.addr=*bp++;
		ipaddrs[ipcnt].type=IPAADDR;
		ipcnt++;
	}

	return 1;
}

/*******************************************************************************
*
*	getipaddress
*
*	Finds ip address(es) for a given host/domain.
*
*	Input:
*		db - domain database.
*		hd - host/domain.
*	Output:
*		s: 0.
*		f: !0.
*/
static int
getipaddress(database *db,
             char *hd)
{
	int l;
	mxnode *m;

	/* is host/domain local? */
	l=local(db, hd);
	if (l)
		return 0;

	/* do full dns search */
	mxl=mxlookup(hd);
	if (mxl) {
		/* fill in ipaddrs */
		for (m=mxl; m && ipcnt<IPADDRSPRDOMAIN; m=m->next) {
			ipaddrs[ipcnt].t.dns=m->exchange;
			ipaddrs[ipcnt].type=IPADNS;
			ipcnt++;
		}

		return 0;
	}

	/* treat it as a hostname (direct) */
	ipaddrs[ipcnt].t.dns=hd;
	ipaddrs[ipcnt].type=IPADNS;
	ipcnt++;

	return 0;
}

/*******************************************************************************
*
*	mail
*
*	Send the mail to the recipient.
*
*	Input:
*		db - domain database.
*		r - recipients.
*	Note:
*		use errorget to determine return status.
*/
void
mail(database *db,
     char *r)
{
	char *m=getdomain(r);
	char expbuf[HUGELINELEN+2];
	int err, ipc;

	if (!m)	{
		if (verbose)
			fprintf(stderr, "%s - domain missing\n", r);
		else {
			memset(expbuf, 0, HUGELINELEN+1);
			s2sexpand(&gs, "Cannot deliver to %R - not a valid domain.", expbuf, HUGELINELEN);
			err=elinkadd(expbuf);
			if (err)
				qlog(QWARN, "(mail) could not add elink message");
		}

		errorraise(ERRMAIL, ERRADDRESS);
		return;
	}

	/* to 'us'? */
	if (!(strcasecmp(gs.fqdn, m))) {
		/* yes */
		if (verbose)
			fprintf(stderr, "(%s local, aliasing to %s)\n", r, localmailto);
		r=localmailto;
		m=getdomain(localmailto);
	}

	ipcnt=0;
	err=getipaddress(db, m);
	if (err) {
		if (verbose)
			fprintf(stderr, "no ip address(es) for %s\n", m);
		else {
			memset(expbuf, 0, HUGELINELEN+1);
			s2sexpand(&gs, "Cannot deliver to %R - unknown domain.", expbuf, HUGELINELEN);
			err=elinkadd(expbuf);
			if (err)
				qlog(QWARN, "(mail) could not add elink message");
		}

		errorraise(ERRMAIL, ERRADDRESS);
		return;
	}

	/* get mail port */
	dstport=getsmtpport();

	/* start sending */
	for (ipc=0; ipc<ipcnt; ipc++) {
		/* open a socket to host */
		if (sck>-1) {
			err=sck;
			sck=-1;
			close(err);
		}

		sck=socket(AF_INET, SOCK_STREAM, 0);
		if (sck==-1) {
			if (verbose)
				fprintf(stderr, "could not create socket (%i)\n", errno);
			else
				qlog(QWARN, "(mail) could not create socket (%i)", errno);
			break;
		}

		err=hostconnect(&gs, ipc);
		if (!err) {
			/* got peers attention ... now do some talking */
			err=smtp(r);
			if (!err) /* transaction ok */
				errorraise(ERRNOERR, ERRNOTHING);

			return;
		}

		/* 1 of 2 things happend:
		   1) connection to this host failed
		   2) hostname resolution failed
		   Both are to be treated as temporary errors */
	}

	/* bummer, all known hosts failed */
	errorraise(ERRSMTP, ERRREQUEUE);
}

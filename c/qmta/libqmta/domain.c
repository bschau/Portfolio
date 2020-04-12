#include "private.h"

/* protos */
static int skipname(u_char *, u_char *, u_char *);
static int skipdata(u_char *, u_char *, u_char *, u_short *, u_short *);

/*******************************************************************************
*
*	mxlookup
*
*	Lookup MX-rr for given domain.
*
*	Input:
*		ch - child object.
*		domain - domain.
*		response - where to store response string.
*	Output:
*		s: mxnodes.
*		f: NULL.
*/
mxnode *
mxlookup(char *domain)
{
	mxnode *mxl=NULL, *m;
	int resplen;
	u_char *cp, *eom, *tcp;
	int cnt, err;
	u_short type, dlen, weight;
	u_char namebuf[MAXDNAME+1];
	union {
		HEADER hdr;
		u_char buf[PACKETSZ];
	} pkg;

	resplen=res_query(domain, C_IN, T_MX, (u_char *)&pkg, sizeof(pkg));
	if (resplen<0)
		return NULL;

	if (resplen>sizeof(pkg))
		resplen=sizeof(pkg);

	eom=pkg.buf+resplen;
	cp=pkg.buf+sizeof(HEADER);
	err=skipname(pkg.buf, eom, cp);
	if (err<0)
		return NULL;

	cp+=(err+QFIXEDSZ);

	cnt=ntohs(pkg.hdr.ancount);
	while (cnt) {
		if (cp>=eom)
			break;

		err=skipdata(pkg.buf, eom, cp, &type, &dlen);
		if (err<0) {	/* don't know how to continue from here */
			mxdestroylist(&mxl);
			return NULL;
		}

		cp+=err;
		if (type==T_MX) {
			tcp=cp;
			GETSHORT(weight, tcp);
			memset(namebuf, 0, MAXDNAME+1);
			err=dn_expand(pkg.buf, eom, tcp, namebuf, MAXDNAME);
			if (err>=0) {
				m=mxalloc(weight, namebuf);
				if (m) 
					mxl=mxinsert(mxl, m);
				else
					qlog(QWARN, "(mxlookup) mxalloc failed");
			} else
				qlog(QWARN, "(mxlookup) dn_expand failed");
		}

		cp+=dlen;
		cnt--;
	}

	return mxl;
}

/*******************************************************************************
*
*	skipname
*
*	Pseudo expands name - this is a fast forward function :)
*
*	Input:
*		som - start of message.
*		eom - end of message.
*		cp - char pointer.
*	Output:
*		s: >=0.
*		f: -1.
*/
static int
skipname(u_char *som,
         u_char *eom,
         u_char *cp)
{
	char buf[MAXDNAME+1];
	int cnt;

	cnt=dn_expand(som, eom, cp, buf, MAXDNAME);
	if (cnt<0) {
		qlog(QWARN, "(skipname) dn_expand failed");
		return -1;
	}

	return cnt;
}

/*******************************************************************************
*
*	skipdata
*
*	Parse a rdata block.
*
*	Input:
*		som - start of message.
*		eom - end of message.
*		cp - char pointer.
*		type - where to store rdata type.
*		dlen - where to store rdata length.
*	Output:
*		s: >0.
*		f: -1.
*/
static int
skipdata(u_char *som,
         u_char *eom,
         u_char *cp,
         u_short *type,
         u_short *dlen)
{
	u_char *tcp=cp;
	int err;
	u_short class;
	u_int32_t ttl;

	err=skipname(som, eom, tcp);
	if (err<0) {
		qlog(QWARN, "(skipdata) skipname failed");
		return -1;
	}

	tcp+=err;
	GETSHORT(*type, tcp);
	GETSHORT(class, tcp);
	GETLONG(ttl, tcp);
	GETSHORT(*dlen, tcp);

	return (tcp-cp);
}

/*******************************************************************************
*
*	getdomain
*
*	Return domain part of mailbox.
*
*	Input:
*		m - mailbox.
*	Output:
*		s: domain.
*		f: NULL.
*/
char *
getdomain(char *m)
{
	for (; *m && *m!='@'; m++);

	if (*m=='@') {
		m++;
		return m;
	}

	return NULL;
}

/*******************************************************************************
*
*	islocal
*
*	Is host/domain local?   A local host/domain is a host/domain we know of.
*
*	Input:
*		db - database.
*		hd - host/domain.
*	Output:
*		host/domain is local: *ptr.
*		host/domain is not local: NULL.
*/
char *
islocal(database *db,
        char *hd)
{
	int err;

	if (!db->f)	/* no database, not local */
		return NULL;

	if (!hd)	/* no host/domain, not local */
		return NULL;

	err=qdbfetch(db, hd);
	if (err) {	/* database error, not local */
		qlog(QWARN, "(islocal) database error");
		return NULL;
	}

	if (db->dptr)	/* found it */
		return db->dptr;

	/* not in db, not local */
	return NULL;
}

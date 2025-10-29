#include "private.h"

/* macros */
#define IDXSIZE (sizeof(bit32))
#define CNTSIZE (sizeof(unsigned short))
#define KEYDATASIZE 2048
#define PUTCNT(x, y) \
	*x++=((y)>>8)&0xff; \
	*x++=(y)&0xff;
#define GETCNT(x, y) \
	y=(*x<<8)|(*(x+1)); \
	x+=2;

/* protos */
static int saferead(FILE *, int, char *, int);
static int safewrite(FILE *, int, char *, int);
static int hashpjw(int, char *);
static void hashtablesize(dblist *);
static int initfile(dblist *, char *);

/*******************************************************************************
*
*	saferead
*
*	Reads the requested amount of bytes.
*
*	Input:
*		f - file.
*		pos - where to read from.
*		dst - destination.
*		len - number of bytes to read.
*	Output:
*		s: 0.
*		f: !0.
*/
static int
saferead(FILE *f,
         int pos,
         char *dst,
         int len)
{
	int l=len, err;

	err=fseek(f, pos, SEEK_SET);
	if (err)
		return 1;

	while (l>0) {
		err=fread(dst, 1, l, f);
		if (err==l)
			return 0;

		if (ferror(f) || feof(f))
			return 1;

		l-=err;
		dst+=l;
	}

	return 0;
}

/*******************************************************************************
*
*	safewrite
*
*	Writes a string with seek.
*
*	Input:
*		f - file.
*		pos - position.
*		src - source string.
*		len - number of bytes to write.
*	Output:
*		s: 0.
*		f: !0.
*/
static int
safewrite(FILE *f,
          int pos,
          char *src,
          int len)
{
	int l=len, err;

	err=fseek(f, pos, SEEK_SET);
	if (err)
		return 1;

	while (l>0) {
		err=fwrite(src, 1, l, f);
		if (err==l)
			return 0;

		if (ferror(f))
			return 1;

		l-=err;
		src+=err;
	}

	return 0;
}

/*******************************************************************************
*
*	hashpjw
*
*	P.J. Weinberger hashing algorithm.   Designed to perform well in
*	hashing strings.
*
*	Input:
*		ht - hash table size.
*		k - key.
*	Output:
*		bucket value.
*	Note:
*		build to last on 32bit platforms.   Might need rethinking
*		on other platforms. (Beware: ugly ANDs and SHIFTs ahead!)
*/
static int
hashpjw(int ht,
        char *k)
{
	int val=0, t;

	for (; *k; k++) {
		val=(val<<4)+(*k);
		t=val;
		if (t&0xf0000000) {
			val=val^(t>>24);
			val=val^t;
		}
	}

	return val%ht;
}

/*******************************************************************************
*
*	hashtablesize
*
*	Stores database hash table size in dblist.
*
*	Input:
*		d - dblist.
*/
static void
hashtablesize(dblist *d)
{
	if ((d->items)<=7) {
		d->htsize=7;
		return;
	}

	if ((d->items)<=997) {
		d->htsize=997;
		return;
	}

	/* we have a lot of domains if we reach this ... :) */
	d->htsize=9973;
}

/*******************************************************************************
*
*	initfile
*
*	Initializes the database file.
*
*	Input:
*		d - dblist.
*		f - filename.
*	Output:
*		s: 0.
*		f: !0.
*/
static int
initfile(dblist *d,
         char *f)
{
	bit32 v;
	int err, cnt;

	hashtablesize(d);

	d->file=fopen(f, "w+");
	if (!d->file)
		return 1;

	v=(bit32)(d->htsize);
	err=safewrite(d->file, 0, (char *)&v, 4);
	if (err)
		return 1;

	v=0;
	for (cnt=1; cnt<=(d->htsize); cnt++) {
		err=safewrite(d->file, cnt*IDXSIZE, (char *)&v, IDXSIZE);
		if (err)
			return 1;
	}

	d->offset=IDXSIZE*((d->htsize)+1);
	return 0;
}

/*******************************************************************************
*
*	initdblist
*
*	Initializes dblist.
*
*	Input:
*		d - dblist header.
*/
void
initdblist(dblist *d)
{
	d->head=(dbnode *)&d->tail;
	d->tail=NULL;
	d->tailpred=(dbnode *)d;
	d->items=0;
	d->file=NULL;
}

/*******************************************************************************
*
*	freedblist
*
*	Deallocates entries in dblist.
*
*	Input:
*		d - dblist header.
*/
void
freedblist(dblist *d)
{
	dbnode *n;

	if (d->file) {
		fflush(d->file);
		fclose(d->file);
	}

	if (d->items) {
		for (n=d->head; n->next; ) {
			if (n->key)
				free(n->key);

			d->head=n->next;
			free(n);
			n=d->head;
		}
	}

	initdblist(d);
}

/*******************************************************************************
*
*	adddblistentry
*
*	Adds a dblist entry.   Entry is 'sorted in' based on key.
*
*	Input:
*		d - dblist.
*		key - key.
*		data - data.
*		datalen - data len.
*	Output:
*		s: 0.
*		f: !0.
*/
int
adddblistentry(dblist *d,
               char *key,
               char *data,
               int datalen)
{
	int keylen=strlen(key), dlen;
	dbnode *n, *ns, *match;
	char *ks, *ds;

	/* sanity */
	if (keylen<1 || datalen<1)
		return 1;

	if ((keylen+datalen)>KEYDATASIZE)
		return 1;

	n=(dbnode *)calloc(sizeof(struct dbnode), 1);
	if (!n)
		return 1;

	ks=(char *)calloc(keylen+datalen+1, 1);
	if (!ks) {
		free(n);
		return 1;
	}

	strncpy(ks, key, keylen);
	ds=ks+keylen+1;
	memmove(ds, data, datalen);
	
	n->key=ks;
	n->data=ds;
	n->keylen=keylen;
	n->datalen=datalen;

	ds=ks;
	for (dlen=0; dlen<keylen; dlen++) {
		*ds=(char)tolower((int)*ds);
		ds++;
	}

	for (ns=d->head; ns->next; ns=ns->next) {
		if (strcmp(ns->key, ks)>0)
			break;

		if (!strncmp(ns->key, ks, keylen)) {
			/* ho, ho, we found an exact match - overwrite */
			match=ns;
			n->next=ns->next;
			n->prev=ns->prev;
			ns=n->next;
			ns->prev=n;
			ns=n->prev;
			ns->next=n;
			free(match->key);
			free(match);
			return 0;
		}
	}

	n->prev=ns->prev;
	n->next=ns;
	ns->prev=n;
	ns=n->prev;
	ns->next=n;

	d->items++;
	return 0;
}

/*******************************************************************************
*
*	populate
*
*	Populates database file.
*
*	Input:
*		d - dblist.
*		f - filename.
*	Output:
*		s: 0.
*		f: !0.
*/
int
populate(dblist *d,
         char *f)
{
	bit32 hv, dv;
	dbnode *n;
	int pos, err;
	char buf[KEYDATASIZE+CNTSIZE+CNTSIZE+IDXSIZE];
	char *p;

	err=initfile(d, f);
	if (err)
		return 1;

	for (n=d->head; n->next; n=n->next) {
		pos=hashpjw(d->htsize, n->key);
		hv=(pos+1)*IDXSIZE;
		dv=hv;
		while (hv) {
			dv=hv;
			err=saferead(d->file, hv, (char *)&hv, IDXSIZE);
			if (err)
				return 1;
		}

		hv=d->offset;
		err=safewrite(d->file, dv, (char *)&hv, IDXSIZE);
		if (err)
			return 1;

		p=buf;
		memset(p, 0, IDXSIZE);
		p+=IDXSIZE;
		PUTCNT(p, n->keylen);
		memmove(p, n->key, n->keylen);
		p+=n->keylen;
		PUTCNT(p, n->datalen);
		memmove(p, n->data, n->datalen);
		p+=n->datalen;

		pos=p-buf;
		err=safewrite(d->file, hv, buf, pos);
		if (err)
			return 1;

		d->offset+=pos;
	}

	return 0;
}

/*******************************************************************************
*
*	qdbopen
*
*	Open database for querying.
*
*	Input:
*		d - database structure.
*		f - filename.
*	Output:
*		s: 0.
*		f: !0.
*/
int
qdbopen(database *d,
        char *f)
{
	bit32 ht;
	int err;

	d->dptr=NULL;
	d->dlen=0;

	d->f=fopen(f, "r");
	if (!d->f)
		return 1;

	err=saferead(d->f, 0, (char *)&ht, IDXSIZE);
	if (err)
		return 1;

	d->htsize=ht;
	return 0;
}

/*******************************************************************************
*
*	qdbclose
*
*	Closes database.
*
*	Input:
*		d - database.
*/
void
qdbclose(database *d)
{
	if (d->f) {
		fclose(d->f);
		d->f=NULL;
	}

	if (d->dptr) {
		free(d->dptr);
		d->dptr=NULL;
	}
}

/*******************************************************************************
*
*	qdbfetch
*
*	Fetches (if possible) a key, data pair.
*
*	Input:
*		d - database.
*		k - key.
*	Output:
*		s: 0.
*		f: !0.
*	Note:
*		success here means: operation succeeded.  Check dp to see
*		if any data was found.
*/
int
qdbfetch(database *d,
         char *k)
{
	int keylen, pos, err, klen;
	bit32 hv, offset;
	char buf[KEYDATASIZE+CNTSIZE+CNTSIZE+IDXSIZE];
	char *p;

	/* catch all */
	if (d->dptr) {
		free(d->dptr);
		d->dptr=NULL;
	}

	/* sanity */
	if (!k)
		return 0;

	keylen=strlen(k);

	pos=hashpjw(d->htsize, k);
	hv=(pos+1)*IDXSIZE;
	err=saferead(d->f, hv, (char *)&hv, IDXSIZE);
	if (err)
		return 1;

	for (EVER) {
		if (!hv)
			/* key doesn't exist */
			return 0;

		err=saferead(d->f, hv, buf, IDXSIZE+CNTSIZE);
		if (err)
			break;

		offset=hv+IDXSIZE+CNTSIZE;
		hv=(*(buf+3)<<24)|(*(buf+2)<<16)|(*(buf+1)<<8)|(*buf);
		p=buf+IDXSIZE;
		GETCNT(p, klen);
		if (klen!=keylen) /* keylength mismatch */
			continue;

		/* they might match, keep searching */
		err=saferead(d->f, offset, buf, klen);
		if (err)
			break;

		if (strncmp(buf, k, keylen))	/* wrong key */
			continue;

		/* found it */
		offset+=klen;
		err=saferead(d->f, offset, buf, CNTSIZE);
		if (err)
			break;

		p=buf;
		GETCNT(p, klen);
		offset+=CNTSIZE;

		p=(char *)calloc(klen+1, 1);	/* trailing 0 is a good thing */
		if (!p)	/* so much trouble, and we're out of memory ... */
			break;

		err=saferead(d->f, offset, p, klen);
		if (err) {
			free(p);
			break;
		}

		d->dptr=p;
		d->dlen=klen;
		return 0;
	}

	return 1;
}

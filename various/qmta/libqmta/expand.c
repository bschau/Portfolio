#include "private.h"
 
/* macros */
#define RFC821BUFLEN 128

/* globals */
static char *gnfstr, *gntstr;
static FILE *gnffil, *gntfil;
static session *ss;
static int maxlen, bytespushed;

/* protos */
static char pullfromstring(void);
static int pushtostring(char);
static char pullfromfile(void);
static int pushtofile(char);
static int expand(char (*)(void), int (*)(char));
static int expandmacro(char, int (*)(char));
static int push(char *, int (*)(char));

/*******************************************************************************
*
*	pullfromstring
*
*	Gets next byte from string.
*
*	Output:
*		s: byte.
*		f: 0 (end of string).
*/
static char
pullfromstring(void)
{
	return (*gnfstr++);
}

/*******************************************************************************
*
*	pushtostring
*
*	Add byte to string.
*
*	Input:
*		b - byte to add.
*	Output:
*		keep pushing: 0.
*		stop pushing: !0.
*/
static int
pushtostring(char b)
{
	*gntstr++=b;
	bytespushed++;

	if (maxlen) {
		if (bytespushed<maxlen)
			return 0;

		return 1;
	}

	return 0;
}

/*******************************************************************************
*
*	pullfromfile
*
*	Gets next byte from file.
*
*	Output:
*		s: byte.
*		f: 0 (end of file).
*/
static char
pullfromfile(void)
{
	int err;

	err=fgetc(gnffil);
	if (err==EOF)
		return 0;

	return (char)err;
}

/*******************************************************************************
*
*	pushtofile
*
*	Add byte to file.
*
*	Input:
*		b - byte to add.
*	Output:
*		keep pushing: 0.
*		stop pushing: !0.
*/
static int
pushtofile(char b)
{
	int err;

	err=fputc((int)b, gntfil);
	if (err==EOF)
		return 1;

	bytespushed++;
	if (maxlen) {
		if (bytespushed<maxlen)
			return 0;

		return 1;
	}

	return 0;
}

/*******************************************************************************
*
*	expand
*
*	Expand source to destination.
*
*	Input:
*		src - source (pull).
*		dst - destination (push).
*	Output:
*		number of bytes in dst.
*/
static int
expand(char (*src)(void),
       int (*dst)(char))
{
	int err;
	char b;

	bytespushed=0;
	for (EVER) {
		b=(src)();
		if (!b)	/* we're done */
			break;

		if (b=='%') {
			b=(src)();
			if (!b) {
				(dst)('%');
				break;
			}

			err=expandmacro(b, dst);
			if (err)
				break;
		} else {
			err=(dst)(b);
			if (err)
				break;
		}
	}

	return bytespushed;
}

/*******************************************************************************
*
*	expandmacro
*
*	Expand a macro to dst.
*
*	Input:
*		m - macro to expand.
*		dst - to.
*	Output:
*		keep expanding: 0.
*		stop expanding: !0.
*/
static int
expandmacro(char m,
            int (*dst)(char))
{
	int err=0, cnt;
	char tsbuf[RFC821BUFLEN+2];
	time_t tt;
	struct tm *lt;
	char *p;
	recipients rcpt;

	switch (m) {
		case 'E':
			elinktravstart();
			for (p=elinknext(); p; p=elinknext()) {
				err=push(p, dst);
				if (err)
					break;
			}
			break;
		case 'P':	/* Postmaster */
			err=push(ss->postmaster, dst);
			break;
		case 'R':	/* to (rcpt to:) */
			err=push(ss->to, dst);
			break;
		case 'S':	/* sender (mail from:) */
			err=push(ss->from, dst);
			break;
		case 'T':	/* rfc821 timestamp */
			time(&tt);
			lt=localtime(&tt);
			cnt=strftime(tsbuf, RFC821BUFLEN, "%a, %d %b %Y %H:%M:%S %z", lt);
			if (cnt)
				err=push(tsbuf, dst);
			else
				err=push("now", dst);
			break;
		case 'Y':	/* all recipients */
			if (ss->recipients) {
				rcpt.recip=ss->recipients->recip;
				rewindrecipients(&rcpt);
				for (p=nextrecipient(&rcpt, 0); p; p=nextrecipient(&rcpt, 0)) {
					err=push(p, dst);
					if (err)
						break;
					err=push("\n", dst);
					if (err)
						break;
				}
			} else
				err=0;
			break;
		case 'Z':	/* recipients not reached */
			if (ss->recipients) {
				rcpt.recip=ss->recipients->recip;
				rewindrecipients(&rcpt);
				for (p=nextrecipient(&rcpt, 1); p; p=nextrecipient(&rcpt, 1)) {
					if (*p==RSUNHANDLED) {
						err=push(p+1, dst);
						if (err)
							break;
						err=push("\n", dst);
						if (err)
							break;
					}
				}
			} else
				err=0;
			break;
		case 'd':	/* our domainname */
			err=push(ss->domainname, dst);
			break;
		case 'f':	/* our fqdn */
			err=push(ss->fqdn, dst);
			break;
		case 'h':	/* our hostname */
			err=push(ss->hostname, dst);
			break;
		case 'i':	/* remote ip address */
			err=push(ss->remoteaddr, dst);
			break;
		case 'l':	/* dump of line */
			err=push(line, dst);
			break;
		case 'm':	/* message id */
			err=push((ss->cntlname)+1, dst);
			break;
		case 'r':	/* remote name */
			err=push(ss->remotename, dst);
			break;
		default:
			err=(dst)(m);
			break;
	}

	return err;
}

/*******************************************************************************
*
*	push
*
*	Push a string to dst.
*
*	Input:
*		src - source string to push.
*		dst - to.
*	Output:
*		keep pushing: 0.
*		stop pushing: !0.
*/
static int
push(char *src,
     int (*dst)(char))
{
	int err;

	if (!src)
		return 0;

	for (; *src; src++) {
		err=(dst)(*src);
		if (err)
			return 1;
	}

	return 0;
}

/*******************************************************************************
*
*	s2sexpand
*
*	Expand string with macros to string.
*
*	Input:
*		sess - session.
*		src - line to be expanded.
*		dst - destination.
*		len - how much to expand (max).
*	Output:
*		number of bytes in dst.
*/
int
s2sexpand(session *sess,
          char *src,
          char *dst,
          int len)
{
	gnfstr=src;
	gntstr=dst;
	ss=sess;
	maxlen=len;
	return expand(pullfromstring, pushtostring);
}

/*******************************************************************************
*
*	f2fexpand
*
*	Expand file with macros to file.
*
*	Input:
*		sess - session.
*		src - file to expand.
*		dst - file to expand to.
*		len - how much to expand (max).
*	Output:
*		number of bytes in dst.
*/
int
f2fexpand(session *sess,
          FILE *src,
          FILE *dst,
          int len)
{
	gnffil=src;
	gntfil=dst;
	ss=sess;
	maxlen=len;
	return expand(pullfromfile, pushtofile);
}

/*******************************************************************************
*
*	s2fexpand
*
*	Expand string with macros to file.
*
*	Input:
*		sess - session.
*		src - line to be expanded.
*		dst - destination file.
*		len - how much to expand (max).
*	Output:
*		number of bytes in dst.
*/
int
s2fexpand(session *sess,
          char *src,
          FILE *dst,
          int len)
{
	gnfstr=src;
	gntfil=dst;
	ss=sess;
	maxlen=len;
	return expand(pullfromstring, pushtofile);
}

#include "private.h"

/* protos */
static int senddata(void);
static int readandparse(void);
static int evaluate(int, int);
static int vnetw(char *);
static void dump(char *);

/*******************************************************************************
*
*	senddata
*
*	Send mail data to peer.
*
*	Output:
*		s: 0.
*		f: !0.
*/
static int
senddata(void)
{
	int ever=1, len;
	char *p;

	rewind(gs.mailfile);
	clearerr(gs.mailfile);
	for (;ever;) {
		p=fgets(line, HUGELINELEN-4, gs.mailfile);
		if (!p) {
			if (feof(gs.mailfile)) {
				p=line;
				*p++='.';
				*p++='\r';
				*p++='\n';
				*p='\x00';
				ever=0;
			} else
				return -1;
		} else {
			if (*p=='.') {
				len=nwrite(sck, ".", 1);
				if (len)
					return 1;
			}
				
			len=strlen(line);

			/* term line */
			p=&line[len-1];
			if (*p=='\n') {
				p--;
				if (*p!='\r') {
					*(p+1)='\r';
					*(p+2)='\n';
					*(p+3)='\x00';
				}
			} else {
				if (*p=='\r') {
					*(p+1)='\n';
					*(p+2)='\x00';
				} else {
					*(p+1)='\r';
					*(p+2)='\n';
					*(p+3)='\x00';
				}
			}

		}

		len=nwrite(sck, line, strlen(line));
		if (len)
			return -1;
	}

	return 0;
}

/*******************************************************************************
*
*	readandparse
*
*	Reads data from peer, parses status code.
*
*	Output:
*		s: status code.
*		f: 0.
*/
static int
readandparse(void)
{
	char *ptr;
	int cnt;

	for (EVER) {
		cnt=ngetline(sck, line, HUGELINELEN-1);
		if (cnt<=0)	/* peer went away */
			return 0;

		line[cnt]='\x00';
		if (verbose) {
			cnt=strlen(line);
			if (line[cnt-1]=='\n')
				cnt=0;

			fprintf(stderr, "%s%s", line, ((cnt) ? "\n" : ""));
		}

		for (ptr=line; *ptr && isdigit((int)*ptr); ptr++);

		if (*ptr==' ') {
			*ptr='\x00';
			cnt=atoi(line);
			*ptr=' ';
			return cnt;
		}

		/* multiline, read some more */
	}

	return 0;
}

/*******************************************************************************
*
*	evaluate
*
*	Evaluate response.
*
*	Input:
*		err - errorcode.
*		flex - flexible error action.
*	Output:
*		go on with mail transaction: 0.
*		abort mail transaction: !0.
*/
static int
evaluate(int err,
         int flex)
{
	if (!err) {	/* readandparse failure */
		errorraise(ERRSMTP, ERRREQUEUE);
		return -1;
	}

	if (*line=='2' || *line=='3')
		return 0;

	if (*line=='4') {
		errorraise(ERRSMTP, ERRREQUEUE);
		return -1;
	}

	errorraise(ERRSMTP, flex);
	return -1;
}

/*******************************************************************************
*
*	vnetw
*
*	Verbose netwrite.
*
*	Input:
*		src - message to write.
*	Output:
*		see 'netstatusw'.
*/
static int
vnetw(char *src)
{
	int err;

	if (verbose) {
		memset(line, 0, HUGELINELEN+1);
		err=s2sexpand(&gs, src, line, HUGELINELEN);
		if (err>0) {
			line[err]='\x00';
			fprintf(stderr, ">>> %s", line);
		} /* if not, it's not so important :) */
	}

	memset(line, 0, HUGELINELEN+1);
	err=s2sexpand(&gs, src, line, HUGELINELEN);
	if (err>0)
		line[err]='\x00';

	return netstatusw(sck, &gs, src);
}

/*******************************************************************************
*
*	dump
*
*	Dump arg if in verbose mode.
*
*	Input:
*		arg - arg to be dumped.
*/
static void
dump(char *arg)
{
	if (verbose) {
		fprintf(stderr, arg);
		fflush(stderr);
	}
}

/*******************************************************************************
*
*	smtp
*
*	Carry out smtp conversion with "other end".
*
*	Input:
*		r - intended recipient mailbox.
*	Output:
*		s: 0.
*		f: !0.
*/
int
smtp(char *r)
{
	int err;

	gs.to=r;

	/* read banner */
	err=readandparse();
	err=evaluate(err, ERRGENERIC500);
	if (err)
		return err;

	/* hello, I'm ... */
	err=vnetw("HELO %d\r\n");
	if (err) {
		errorraise(ERRSMTP, ERRREQUEUE);
		return -1;
	}

	err=readandparse();
	err=evaluate(err, ERRGENERIC500);
	if (err)
		return err;

	/* I've got mail from ... */
	err=vnetw("MAIL FROM: <%S>\r\n");
	if (err) {
		errorraise(ERRSMTP, ERRREQUEUE);
		return -1;
	}

	err=readandparse();
	err=evaluate(err, ERRINVALIDFROM);
	if (err)
		return err;

	/* the mail is to ... */
	err=vnetw("RCPT TO: <%R>\r\n");
	if (err) {
		errorraise(ERRSMTP, ERRREQUEUE);
		return -1;
	}

	err=readandparse();
	err=evaluate(err, ERRINVALIDTO);
	if (err)
		return err;

	/* ... and so followth the mail */
	err=nwrite(sck, "DATA\r\n", 6);
	dump(">>> DATA\r\n");
	if (err) {
		errorraise(ERRSMTP, ERRREQUEUE);
		return -1;
	}

	err=readandparse();
	if (err!=354) {
		if (verbose)
			fprintf(stderr, "%s", line);

		errorraise(ERRSMTP, ERRREQUEUE);
		return -1;
	}

	dump(">>> Sending data to peer. Data will not be shown.\r\n");
	err=senddata();
	dump(">>> .\r\n");
	if (err) {
		errorraise(ERRSMTP, ERRREQUEUE);
		return -1;
	}

	err=readandparse();
	err=evaluate(err, ERRGENERIC500);
	if (err)
		return err;

	/* bye! */
	err=nwrite(sck, "QUIT\r\n", 6);
	dump(">>> QUIT\r\n");
	if (err) {
		errorraise(ERRSMTP, ERRREQUEUE);
		return -1;
	}

	err=readandparse();
	err=evaluate(err, ERRGENERIC500);
	if (err)
		return err;

	return 0;
}

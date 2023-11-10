#include "private.h"

/* macros */
#define DIRTYHIGHMARK 10	/* how long will we ioctl(FIONREAD) on a pipe
				   which might have been closed before we
				   decide that it is closed */
#define BUFFERSIZE 2048

/* globals */
static char buffer[BUFFERSIZE+2];
static char *bptr;
static int bcnt;

/*******************************************************************************
*
*	nwrite
*
*	Write to network socket.
*
*	Input:
*		fd - fd.
*		src - what to write.
*		cnt - how much to write.
*	Output:
*		s: 0.
*		f: !0.
*/
int
nwrite(int fd,
       char *src,
       int cnt)
{
	int s;

	while (cnt>0) {
		s=send(fd, src, cnt, 0);
		if (s==-1)
			return -1;

		cnt-=s;
		src+=s;
	}

	return 0;
}

/*******************************************************************************
*
*	ngetline
*
*	Gets a line from the socket.
*
*	Input:
*		fd - socket.
*		dst - where to store data.
*		len - max size of line.
*	Output:
*		s: bytes received.
*		f: -1.
*/
int
ngetline(int fd,
         char *dst,
         int len)
{
	int cnt=0, err;
	fd_set rfs, tfs;
	
	FD_ZERO(&tfs);
	FD_SET(fd, &tfs);

	while (len>0) {
		if (bcnt) {
			*dst=*bptr++;
			cnt++;
			bcnt--;
			if (*dst=='\n') {
				*(dst+1)='\x00';
				return cnt;	/* exit */
			}

			dst++;
		} else {
			bptr=buffer;
			
			/* waiting for peer to speak ... */
			rfs=tfs;
			err=select(fd+1, &rfs, NULL, NULL, NULL);
			if (err<-1) {
				bcnt=0;
				*bptr='\00';
				return -1;
			}

			if (FD_ISSET(fd, &rfs)) {
				bcnt=recv(fd, bptr, BUFFERSIZE, 0);
				if (bcnt<=0) {
					bcnt=0;
					*bptr='\x00';
					return -1;
				}
			}
		}
	}

	return cnt;
}

/*******************************************************************************
*
*	netstatusw
*
*	Write status reply to network socket.   This will expand macros.
*
*	Input:
*		fd - network socket.
*		sess - session.
*		src - null terminated string.
*	Output:
*		see 'nwrite'.
*/
int
netstatusw(int fd,
           session *sess,
           char *src)
{
	char expbuf[HUGELINELEN+2];
	int len;

	memset(expbuf, 0, HUGELINELEN+1);
	len=s2sexpand(sess, src, expbuf, HUGELINELEN);
	if (len<2) {
		expbuf[0]='\r';
		expbuf[1]='\n';
		len=2;
	}

	expbuf[len]='\x00';

	return nwrite(fd, expbuf, len);
}

/*******************************************************************************
*
*	getsmtpport
*
*	Finds the port number for smtp/tcp.
*
*	Output:
*		port number.
*/
unsigned short
getsmtpport(void)
{
	struct servent *sv;
	unsigned short p;

	sv=getservbyname("smtp", "tcp");
	if (sv)
		p=sv->s_port;
	else
		p=25;

	return p;
}

/*******************************************************************************
*
*	resolveip
*
*	Resolve ip address to domain name.   Updates session vars.
*
*	Input:
*		ss - session.
*		ip - ip address.
*		dns - dns name (if NULL, resolve).
*/
void
resolveip(session *ss,
          uint32_t ip,
          char *dns)
{
	struct hostent *he;
	struct in_addr addr;

	addr.s_addr=ip;
	ss->remoteip=ntohl(ip);
	ss->remoteaddr=inet_ntoa(addr);
	inet_aton(ss->remoteaddr, &addr);
	if (dns)
		strncpy(ss->remotename, dns, DNSFQDNLEN);
	else {
		endhostent();
		he=gethostbyaddr((char *)&addr, sizeof(addr), AF_INET);
		if (he)
			strncpy(ss->remotename, he->h_name, DNSFQDNLEN);
		else
			strncpy(ss->remotename, "UNKNOWN", DNSFQDNLEN);
	}
}

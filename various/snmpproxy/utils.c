/*
**	utils.c
**
**	Various utilities.
*/
#include "include.h"

/* macros */
#define SYSLOGBUFFER 256
#define LINEBUFFER 80

/*
**	XLog
**
**	Logs the specified variable format list.
**
**	 -> pri		Priority.
**	 -> fmt		Format.
**	 -> ...		Arguments.
*/
void
#ifdef HPUX
XLog(pri, fmt, va_alist)
int pri;
const char *fmt;
va_dcl
#else
XLog(int pri, const char *fmt, ...)
#endif
{
	char msg[SYSLOGBUFFER];
	va_list vArgs;

	if (pDebug==0 && pri==LOG_DEBUG)
		return;

#ifdef HPUX
	va_start(vArgs);
#else
	va_start(vArgs, fmt);
#endif

	vsnprintf(msg, SYSLOGBUFFER-1, fmt, vArgs);
	va_end(vArgs);

	if (hasStdout) {
		fprintf(stdout, "%s\n", msg);
		fflush(NULL);
	} else
		syslog(pri, msg);
}

/*
**	XCaptureToFile
**
**	Dump content to file.
**
**	 -> title	Header title.
**	 -> dFile	Dump file.
**	 -> src		Source content.
**	 -> len		Length.
*/
void
XCaptureToFile(char *title, char *dFile, void *src, int len)
{
	int offset=0, bIndex=0, pLen=len, i;
	char *errStr=NULL;
	unsigned char *s=src;
	char line[LINEBUFFER];
	FILE *fH;
	Message m;
	SBuf sBuf;

	if (!pDebug)
		return;

	if ((fH=fopen(dFile, "a+"))==NULL) {
		XLog(LOG_ERR, "(XCaptureToFile) Cannot open file %s for writing (%i)", dFile, errno);
		return;
	}

	fwrite(title, strlen(title), 1, fH);
	for (; len>0; ) {
		memset(line, ' ', LINEBUFFER);

		sprintf(line, "%04X: ", offset);
		for (i=0; i<16; i++) {
			if (len<1)
				break;

			sprintf(line+6+(i*3), "%02X", s[bIndex]);
			line[(i*3)+6+2]=' ';
			if (isprint((int)s[bIndex]))
				sprintf(line+60+i, "%c", s[bIndex]);
			else
				sprintf(line+60+i, ".");

			len--;
			bIndex++;
		}

		i=strlen(line);
		line[i-1]='\n';

		fwrite(line, i, 1, fH);
	}
	fprintf(fH, "%i bytes\n", pLen);

	if (pDumpMessage) {
		if ((src=VerifyPackage(src, &pLen))) {
			if (DisectPackage(&m, &sBuf, src, pLen)==0) {
				PrintMessage(fH, &m, 0);
				FreeMessage(&m);
			} else
				errStr="Decoding of message failed";
		} else
			errStr="Package failed verification";

		if (errStr)
			fprintf(fH, "%s\n", errStr);
	}
	fclose(fH);
}

/*
**	XMalloc
**
**	Allocate bytes, clear area.
**
**	 -> size	No. of bytes to allocate.
**
**	Returns area. In case of error, logs error & exits!
*/
void *
XMalloc(int size)
{
	void *c=calloc(size, 1);

	if (c==NULL) {
		XLog(LOG_ERR, "(XMalloc) Cannot allocate %i bytes", size);
		exit(1);
	}

	return c;
}

/*
**	XStrDup
**
**	Duplicate string.
**
**	 -> src		Source string.
**
**	Returns duplicated string. In case of error, logs error & exits!
*/
char *
XStrDup(char *src)
{
	char *s=strdup(src);

	if (!s) {
		XLog(LOG_ERR, "(XStrDup) Out of memory");
		exit(1);
	}

	return s;
}

/*
**	XStrCat
**
**	Concatenate up to gazillion strings.
**
**	 -> sep		Separator (or NULL).
**	 -> ...		Strings to concatenate.
**
**	Note:
**		Last entry must be NULL!
**
**	Returns null terminated string.
*/
char *
#ifdef HPUX
XStrCat(sep, va_alist)
char *sep;
va_dcl
#else
XStrCat(char *sep, ...)
#endif
{
	int strLen=0, entries=0, sepLen, len;
	char *dst, *s, *w;
	va_list l;

	if (sep)
		sepLen=strlen(sep);
	else
		sepLen=0;

	/* find length of entries & no. of entries */
#ifdef HPUX
	va_start(l);
#else
	va_start(l, sep);
#endif
	while ((s=va_arg(l, char *))) {
		strLen+=strlen(s);
		entries++;
	}
	va_end(l);

	/* allocate string */
	if ((entries--)<0) {
		XLog(LOG_ERR, "(XStrCat) Called with 0 arguments");
		exit(1);
	}

	strLen+=(entries*sepLen)+1;
	if ((dst=calloc(strLen, 1))==NULL) {
		XLog(LOG_ERR, "(XStrCat) Out of memory");
		exit(1);
	}

	/* create string */
	w=dst;

#ifdef HPUX
	va_start(l);
#else
	va_start(l, sep);
#endif

	while ((s=va_arg(l, char *))) {
		len=strlen(s);
		strncpy(w, s, len);
		w+=len;
		entries--;
		if (entries<0)
			break;

		if (sepLen>0) {
			strncpy(w, sep, sepLen);
			w+=sepLen;
		}
	}
	va_end(l);

	/* terminate string */
	*w='\x00';
	return dst;
}

/*
**	XLookup
**
**	Lookup hostname in DNS.
**
**	 -> h		Host.
**	 -> keepOld	If !0, keep old ip-address.
**
**	If h->address is 0 hostname didn't resolve.
*/
void
XLookup(Host *h, int keepOld)
{
	struct hostent *res;

	h->tick=time(NULL);

	if ((res=gethostbyname(h->name))==NULL) {
		XLog(LOG_WARNING, "(XLookup) Cannot resolve %s", h->name);
		if (!keepOld)
			h->address.s_addr=INADDR_ANY;

		return;
	}

	h->address.s_addr=((struct in_addr *)*res->h_addr_list)->s_addr;
	XLog(LOG_DEBUG, "(XLookup) Address %X", h->address.s_addr);
}

/*
**	XIPToDottedAscii
**
**	Write IP address in ascii form.
**
**	 -> dst		Destination buffer.
**	 -> ip		IP Address.
*/
void
XIPToDottedAscii(char *dst, struct in_addr *ip)
{
	unsigned int i=ip->s_addr;
	int o1, o2, o3, o4;

	o1=(int)((i>>24)&0xff);
	o2=(int)((i>>16)&0xff);
	o3=(int)((i>>8)&0xff);
	o4=(int)(i&0xff);
	sprintf(dst, "%i.%i.%i.%i", o1, o2, o3, o4);
}

/*
**	XGetIPAddress
**
**	Parse ip-address.
**
**	 -> dst		Where to store IP Address.
**	 -> src		Source.
**
**	Returns 0 if successful, !0 otherwise.
*/
int
XGetIPAddress(struct in_addr *dst, char *src)
{
	int o1, o2, o3, o4;

	if (XPickupIPAddress(src, &o1, &o2, &o3, &o4)<0)
		return -1;

	dst->s_addr=(o1<<24)|(o2<<16)|(o3<<8)|o4;
	return 0;
}

/*
**	XGetHost
**
**	Parse a host:port entry.
**
**	 -> dst		Destination host.
**	 -> src		Source.
**
**	Returns 0 if successful, !0 otherwise.
**
**	WARNING: Caller must free dst->name!
*/
int
XGetHost(Host *dst, char *src, int defPort)
{
	char *port=src, *b;
	int p=htons(defPort);
	struct servent *s;

	for (; *port && *port!=':'; port++);
	if (*port==':') {
		*port++='\x00';
		for (b=port; *b && isdigit((int)*b); b++);
		if (!*b) {	/* pure digits */
			errno=0;
			p=atoi(port);
			if (errno || p<1 || p>65535) {
				XLog(LOG_ERR, "(XGetHost) Port number out of range");
				return -1;
			}
			p=htons((unsigned short)p);
		} else {	/* alnums - maybe a service */
			if ((s=getservbyname(port, "udp"))==NULL) {
				XLog(LOG_ERR, "(XGetHost) Service not found");
				return -1;
			}
			p=s->s_port;
		}
	}

	dst->port=p;
	dst->name=XStrDup(src);

	return 0;
}

/*
**	XGetString
**
**	Parse a string.
**
**	 -> src		Source.
**
**	Returns string (up to terminator or EOL).
*/
char *
XGetString(char *src)
{
	char term=0;
	char *arg;

	if (*src=='"' || *src=='\'')
		term=*src++;

	arg=src;

	for (; *src; src++) {
		if (term) {
			if (*src==term)
				break;
		} else {
			if (isspace((int)*src))
				break;
		}
	}

	*src='\x00';

	return arg;
}

/*
**	XPickupIPAdress
**
**	Parse an IP address from a string.
**
**	 -> src		Source string.
**	 -> o1-o4	Where to store octets.
**
**	Returns 0 if successful, !0 if not successful.
*/
int
XPickupIPAddress(char *src, int *o1, int *o2, int *o3, int *o4)
{
	if (sscanf(src, "%i.%i.%i.%i", o1, o2, o3, o4)!=4) 
		return -1;

	if (*o1<0 || *o1>255 || *o2<0 || *o2>255 || *o3<0 || *o3>255 || *o4<0 || *o4>255)
		return -1;


	return 0;
}

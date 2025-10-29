/*
**	config.c
**
**	Load and parse configuration.
*/
#include "include.h"

/* macros */
#define MAXLINELEN 1024
#define SYSLOGBUFFER 256
#define PARSEPROTO(x) static void x(int line, char *args)

enum { COParse=0, COInteger, COString, COBoolean };

/* protos */
static void Init(void);
static void Parse(int, char *);
static void SetInteger(int, unsigned int *, char *);
static void SetString(int, char **, char *);
static void SetBoolean(int, int *, char *);
static void GetNetwork(int, Network *, char *);
PARSEPROTO(ParseTrusted);
PARSEPROTO(ParsePeer);
PARSEPROTO(ParseGetNext);
PARSEPROTO(ParsePort);
PARSEPROTO(ParseFakeList);

/* globals */
static GetNextOid **lastOid=&gnl;
static FakeList **lastFake=&fakeList;
static struct {
	char *token;
	int coType;
	void *dst;
	void (*parse)(int, char *);
} tokenList[]= {
	{ "cachename", COInteger, &pCacheName, NULL },
	{ "capturefile", COString, &pCaptureFile, NULL },
	{ "community", COString, &pCommunity, NULL },
	{ "debug", COBoolean, &pDebug, NULL },
	{ "dumpmessage", COBoolean, &pDumpMessage, NULL },
	{ "fakelist", COParse, NULL, ParseFakeList },
	{ "getnext", COParse, NULL, ParseGetNext },
	{ "peer", COParse, NULL, ParsePeer },
	{ "pidfile", COString, &pPidFile, NULL },
	{ "port", COParse, NULL, ParsePort },
	{ "retry", COInteger, &pRetry, NULL },
	{ "timeout", COInteger, &pTimeout, NULL },
	{ "trusted", COParse, NULL, ParseTrusted },
	{ "user", COString, &pUser, NULL },
	{ NULL },
};

/*
**	Init
**
**	Initializes configuration.
*/
static void
Init(void)
{
	pPort=htons(16000);
	pPidFile=XStrDup("/var/run/snmpproxy.pid");
	pUser=XStrDup("nobody");
	pDebug=0;
	pCaptureFile=XStrDup("/tmp/snmpproxy.pkg");
	pCommunity=XStrDup("public");
	pDumpMessage=0;
	pPeer.name=XStrDup("localhost");
	pPeer.port=htons(161);
	pCacheName=(60*60*24);	/* One day */
	pRetry=5;
	pTimeout=1;
	XLookup(&pPeer, 0);
}

/*
**	ConfigDump
**
**	Write configuration to stdout.
*/
void
ConfigDump(void)
{
	char buffer[20];
	struct in_addr i;

	printf("Configuration:\n\n");
	printf("  Server port:                     %i\n", pPort);
	printf("  Process ID file:                 '%s'\n", pPidFile);
	printf("  Debugging:                       %s\n", (pDebug) ? "on" : "off");
	if (pDebug) {
		printf("    Capture file:                  '%s'\n", pCaptureFile);
		printf("    Dump messages:                 %s\n", (pDumpMessage) ? "yes" : "no");
	}

	i.s_addr=ntohl(pPeer.address.s_addr);
	XIPToDottedAscii(buffer, &i);
	printf("  Peer:                            (%s) %s:%i\n", buffer, pPeer.name, ntohs(pPeer.port));
	printf("  Cache name (seconds):            %i\n", pCacheName);
	printf("  Retry (times):                   %i\n", pRetry);
	printf("  Timeout (seconds):               %i\n", pTimeout);
}

/*
**	ConfigLoad
**
**	Loads and parses configuration.
**
**	 -> fN		Fill name.
*/
void
ConfigLoad(char *fN)
{
	int line=1;
	char buffer[MAXLINELEN];
	char *l;
	FILE *cF;

	if ((cF=fopen(fN, "r+"))==NULL) {
		XLog(LOG_ERR, "(ConfigLoad) Cannot open %s (%i)", fN, errno);
		exit(1);
	}

	Init();

	for (EVER) {
		if ((l=fgets(buffer, MAXLINELEN, cF))==NULL) {
			if (feof(cF)) {
				fclose(cF);
				return;
			}

			XLog(LOG_ERR, "(ConfigLoad) Sudden end of file");
			break;
		} else {
			for (; *l; l++);
			if (*(l-1)=='\n') {
				Parse(line, buffer);
				line++;
			} else {
				XLog(LOG_ERR, "(ConfigLoad) %i: Line too long", line);
				break;
			}
		}
	}

	exit(1);
}

/*
**	Parse
**
**	Parse a line from the configuration line.
**
**	 -> line	Line no.
**	 -> src		Source line.
*/
static void
Parse(int line, char *src)
{
	char *token, *argEat;
	int i;

	for (; *src && isspace((int)*src); src++);
	if (!*src)		/* blank line */
		return;

	if (*src=='#')		/* comment */
		return;

	token=src;
	for (; *src && isgraph((int)*src); src++)
		*src=(char)tolower((int)*src);

	if (!*src) {
		XLog(LOG_ERR, "(Parse) %i: Sudden end of token", line);
		exit(1);
	}

	*src++='\x00';
	for (; *src && isspace((int)*src); src++);
	if (!*src) {
		XLog(LOG_ERR, "(Parse) %i: Token needs argument", line);
		exit(1);
	}

	/* 'eat' trailing whitespaces */
	for (argEat=src; *argEat; argEat++);
	for (argEat--; argEat>=src; argEat--) {
		if (isspace((int)*argEat))
			*argEat='\x00';
		else
			break;
	}

	for (i=0; tokenList[i].token; i++) {
		if (strcmp(tokenList[i].token, token)==0) {
			switch (tokenList[i].coType) {
			case COParse:
				tokenList[i].parse(line, src);
				return;

			case COInteger:
				SetInteger(line, tokenList[i].dst, src);
				return;

			case COString:
				SetString(line, tokenList[i].dst, src);
				return;

			case COBoolean:
				SetBoolean(line, tokenList[i].dst, src);
				return;

			default:
				XLog(LOG_ERR, "(Parse) Unknown COType = %i", tokenList[i].coType);
				exit(1);
			}
		}
	}

	/* not a token - maybe an oid? */
	if ((OidIsOid(token)) || *token!='.') {
		XLog(LOG_ERR, "(Parse) %i: Not a known token, not an OID", line);
		exit(1);
	}

	OidAdd(token+1, src);
}

/*
**	SetX
**
**	The SetX's below parses and sets the type in a corresponding variable.
**
**	 -> line	Line no.
**	 -> dst		Destination (int *, char *, ...).
**	 -> src		Argument.
*/
/*
**	SetInteger
*/
static void
SetInteger(int line, unsigned int *dst, char *src)
{
	int i;
	char *p;

	i=strtoul(src, &p, 10);
	if (src==p || *p) {
		XLog(LOG_ERR, "(SetInteger) %i: integer conversion failed (%i)", line, errno);
		exit(1);
	}

	*dst=i;
}

/*
**	SetString
*/
static void
SetString(int line, char **dst, char *src)
{
	*dst=XStrDup(XGetString(src));
}

/*
**	SetBoolean
*/
static void
SetBoolean(int line, int *dst, char *src)
{
	char *a=src;

	for (; *a; a++)
		*a=(char)tolower((int)*a);

	if (strcmp(src, "1")==0 || strcmp(src, "on")==0 || strcmp(src, "yes")==0 || strcmp(src, "true")==0)
		*dst=1;
	else if (strcmp(src, "0")==0 || strcmp(src, "off")==0 || strcmp(src, "no")==0 || strcmp(src, "false")==0)
		*dst=0;
	else {
		XLog(LOG_ERR, "(SetBoolean) %i: use [1|on|yes|true] to turn on, [0|off|no|false] to turn off", line);
		exit(1);
	}
}

/*
**	GetNetwork
**
**	Parse network entity.
**
**	 -> line	Line no.
**	 -> dst		Where to store network.
**	 -> src		Source arguments.
*/
static void
GetNetwork(int line, Network *n, char *src)
{
	struct in_addr mask;
	int t;
	char *q, *b;
	

	/* format:

		ip
		ip/bits
		ip/mask
	*/

	mask.s_addr=0;

	/* get qualifer (bits or mask) */
	for (q=src; *q && *q!='/'; q++);
	if (*q=='/') {
		*q++='\x00';

		/* take a guess .. */
		for (b=q; *b && isdigit((int)*b); b++);
		if (!*b) {
			/* pure digits */
			errno=0;
			t=atoi(q);
			if (errno || t<0 || t>32) {
				XLog(LOG_ERR, "(GetNetwork) %i: Bit value invalid", line);
				exit(1);
			}

			for (; t>0; t--) {
				mask.s_addr>>=1;
				mask.s_addr|=0x80000000;
			}
		} else { /* polka dots */
			if ((XGetIPAddress(&mask, q))) {
				XLog(LOG_ERR, "(GetNetwork) %i: Cannot parse IP Mask", line);
				exit(1);
			}
		}
	}
	
	n->mask=mask;

	/* get ip address */
	if ((XGetIPAddress(&n->network, src))) {
		XLog(LOG_ERR, "(GetNetwork) %i: Cannot parse IP Address", line);
		exit(1);
	}
}

/*
**	ParseX
**
**	Parse esoteric variables.
**
**	 -> line	Line no.
**	 -> args	Arguments.
*/

/*
**	ParseTrusted
*/
PARSEPROTO(ParseTrusted)
{
	Network n;

	GetNetwork(line, &n, args);
	TrustedAdd(&n);
}

/*
**	ParsePeer
*/
PARSEPROTO(ParsePeer)
{
	Host h;

	if ((XGetHost(&h, args, 161))) {
		XLog(LOG_ERR, "(ParsePeer) %i: Cannot parse host entry", line);
		exit(1);
	}

	pPeer.name=h.name;
	pPeer.port=h.port;
}

/*
**	ParseGetNext
*/
PARSEPROTO(ParseGetNext)
{
	char *baseOid, *suffix;
	GetNextOid *oid;

	/* pickup base */
	baseOid=args;
	for (baseOid=args; *args && !isspace((int)*args); args++);
	if (!*args) {
		XLog(LOG_ERR, "(ParseGetNext) %i: Malformed list", line);
		exit(1);
	}

	*args++='\x00';
	if ((OidIsOid(baseOid))) {
		XLog(LOG_ERR, "(ParseGetNext) %i: Malformed base OID", line);
		exit(1);
	}

	/* got base an' all - parse the list */
	for (EVER) {
		for (; *args && isspace((int)*args); args++);
		if (!*args)
			break;

		suffix=args;
		for (; *args && !(isspace((int)*args)); args++);
		*args++='\x00';

		if (OidIsOid(suffix)!=0) {
			XLog(LOG_ERR, "(ParseGetNext) %i: Malformed suffix ('%s')", line, suffix);
			exit(1);
		}

		oid=XMalloc(sizeof(GetNextOid));

		oid->oid.octs=XStrCat(NULL, baseOid, suffix, NULL);
		if (*args)
			oid->type=GNONormal;
		else
			oid->type=GNOTerm;

		oid->oid.octetLen=strlen(oid->oid.octs);
		if (AsciiToOid(&oid->encodedOid, oid->oid.octs, 0)!=0)
			exit(1);

		/* link it in */
		*lastOid=oid;
		lastOid=&oid->next;
	}
}

/*
**	ParsePort
*/
PARSEPROTO(ParsePort)
{
	struct servent *s;
	int p;

	if ((s=getservbyname(args, "udp"))==NULL) {
		errno=0;
		p=atoi(args);
		if (errno || p<1 || p>65535) {
			XLog(LOG_ERR, "(ParsePort) Port number out of range");
			exit(1);
		}
		pPort=htons((unsigned short)p);
	} else
		pPort=s->s_port;
}

/*
**	ParseFakeList
*/
PARSEPROTO(ParseFakeList)
{
	char *baseOid, *nextOid;
	FakeList *f;

	/* pickup base */
	baseOid=args;
	for (baseOid=args; *args && !isspace((int)*args); args++);
	if (!*args) {
		XLog(LOG_ERR, "(ParseFakeList) %i: Malformed list", line);
		exit(1);
	}

	*args++='\x00';
	if ((OidIsOid(baseOid))) {
		XLog(LOG_ERR, "(ParseFakeList) %i: Malformed base OID", line);
		exit(1);
	}

	for (; *args && isspace((int)*args); args++);
	if (!*args) {
		XLog(LOG_ERR, "(ParseFakeList) %i: No next element specified", line);
		exit(1);
	}

	for (nextOid=args; *args && !isspace((int)*args); args++);
	if (*args) {
		XLog(LOG_ERR, "(ParseFakeList) %i: Malformed list", line);
		exit(1);
	}

	if ((OidIsOid(nextOid))) {
		XLog(LOG_ERR, "(ParseFakeList) %i: Malformed first element OID", line);
		exit(1);
	}

	f=XMalloc(sizeof(FakeList));

	f->oid.octs=XStrDup(baseOid);
	f->oid.octetLen=strlen(baseOid);
	if (AsciiToOid(&f->encodedOid, f->oid.octs, 0)!=0)
		exit(1);

	f->nextOid.octs=XStrDup(nextOid);
	f->nextOid.octetLen=strlen(nextOid);
	if (AsciiToOid(&f->nextEncodedOid, f->nextOid.octs, 0)!=0)
		exit(1);

	*lastFake=f;
	lastFake=&f->next;
}

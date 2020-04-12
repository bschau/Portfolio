/*
**	oid.c
**
**	OID subroutines.
*/
#include "include.h"

/* macros */
#define MAXBUFFERSIZE 128

/* protos */
static void PrettyPrintOid(OidNode *);
static void GetSInt(OidNode *, char *, int, char *, char *);
static void GetUInt(OidNode *, char *, int, char *, char *);
static OidNode *SetSpawn(OidNode *);

/* globals */
static OidNode *oidList;

/*
**	OidIsOid
**
**	Determine if source string could be an oid.
**
**	 -> src		Source string.
**
**	Returns 0 if string looks like an oid, !0 otherwise.
*/
int
OidIsOid(char *src)
{
	if (*src++!='.')
		return -1;

	for (; *src; src++) {
		if (!(isdigit((int)*src) || *src=='.'))
			return -1;
	}

	return 0;
}

/*
**	PrettyPrintOid
**
**	Pretty print content of OID.
**
**	 -> n		OID Node.
*/
static void
PrettyPrintOid(OidNode *n)
{
	char *o;
	char buffer[32];

	switch (n->oidType) {
	case OTMasquerade:
		if ((o=OidToDottedAscii(&n->o.masquerade)))
			printf(" masquerade for .%s\n", o);
		else
			printf(" -- error (no mem)\n");
		break;
	case OTIpAddress:
		o=&n->o.ip[0];
		sprintf(buffer, "%i.%i.%i.%i", *o, *(o+1), *(o+2), *(o+3));
		printf(" return IP address %s\n", buffer);
		break;
	case OTSpawn:
		printf(" return output from %s\n", n->o.spawn);
		break;
	case OTNumber:
		printf(" return %i as number\n", n->o.sNumber);
		break;
	case OTCounter:
		printf(" return %u as counter\n", n->o.uNumber);
		break;
	case OTGauge:
		printf(" return %u as gauge\n", n->o.uNumber);
		break;
	case OTTicks:
		printf(" return %u as ticks\n", n->o.uNumber);
		break;
	case OTString:
		printf(" return \"%s\"\n", n->o.string);
		break;
	case OTArbitrary:
		printf(" return \"%s\"\n", n->o.string);
		break;
	case OTObject:
		if ((o=OidToDottedAscii(&n->o.object))) 
			printf(" return .%s as object\n", o);
		else
			printf(" -- error (no mem)\n");
		break;
	case OTNull:
		printf(" return NULL\n");
		break;
	}
}

/*
**	OidDump
**
**	Dump OID list.
*/
void
OidDump(void)
{
	OidNode *n;

	printf("OIDs defined:\n\n");

	if (oidList) {
		for (n=oidList; n; n=n->next) {
			printf("  .%s: ", n->oid.octs);
			PrettyPrintOid(n);
		}
	} else
		printf("  - empty -\n");
}

/*
**	OidDumpGetNextList
**
**	Dump the GetNextList(s) and contents.
*/
void
OidDumpGetNextList(void)
{
	GetNextOid *o, *m;
	OidNode *n;

	printf("GetNext lists:\n\n");

	if (!gnl) {
		printf("  - empty -\n");
		return;
	}

	printf("  Segment:\n");
	for (o=gnl; o; o=o->next) {
		printf("    %s:  ", o->oid.octs);
		if (o->type==GNONormal) {
			m=o->next;
			if ((n=OidGetNode((m->oid.octs)+1))) {
				printf(".%s", n->oid.octs);
				PrettyPrintOid(n);
			} else
				printf("undefined oid (%s)\n", m->oid.octs);
		} else {
			printf("terminate segment\n");
			if (o->next)
				printf("\n  Segment:\n");
		}
	}
}

/*
**	OidDumpFakeList
**
**	Dump the fake list.
*/
void
OidDumpFakeList(void)
{
	FakeList *f;

	printf("Fake lists:\n\n");

	if (!fakeList) {
		printf("  - empty -\n");
		return;
	}

	for (f=fakeList; f; f=f->next)
		printf("  %s -> %s\n", f->oid.octs, f->nextOid.octs);
}

/*
**	OidAdd
**
**	Add an OID to the oid list.
**
**	 -> oid		OID.
**	 -> act		Action.
*/
void
OidAdd(char *oid, char *action)
{
	OidNode *n;
	int o1, o2, o3, o4;
	char *p;

	for (n=oidList; n; n=n->next) {
		if (strcmp(n->oid.octs, oid)==0) {
			XLog(LOG_WARNING, "(OidAdd) %s already present on list - ignoring", oid);
			return;
		}
	}

	n=XMalloc(sizeof(OidNode));
	n->next=oidList;
	n->oid.octs=XStrDup(oid);
	n->oid.octetLen=strlen(n->oid.octs);
	if (AsciiToOid(&n->encodedOid, oid, 0)!=0)
		exit(1);

	switch (action[0]) {
	case '.':
		n->oidType=OTMasquerade;
		if (AsciiToOid(&n->o.masquerade, action, 0)!=0)
			exit(1);
		break;
	case '|':
		n->oidType=OTSpawn;
		n->o.spawn=XStrDup(action+1);
		n->dataLen=strlen(n->o.spawn);
		break;
	case 'i':
		n->oidType=OTIpAddress;
		if (XPickupIPAddress(action+2, &o1, &o2, &o3, &o4)!=0) {
			XLog(LOG_ERR, "(OidAdd) IP Address invalid (%s %s)", oid, action);
			exit(1);
		}

		p=&n->o.ip[0];
		n->dataLen=4;

		*p++=(unsigned char)o1;
		*p++=(unsigned char)o2;
		*p++=(unsigned char)o3;
		*p=(unsigned char)o4;
		break;
	case 'c':
		GetUInt(n, action+2, OTCounter, oid, "counter");
		break;
	case 'g':
		GetUInt(n, action+2, OTGauge, oid, "gauge");
		break;
	case 'n':
		GetSInt(n, action+2, OTNumber, oid, "number");
		break;
	case 't':
		GetUInt(n, action+2, OTTicks, oid, "ticks");
		break;
	case 'a':
		n->oidType=OTArbitrary;
		n->o.string=XStrDup(XGetString(action+2));
		n->dataLen=strlen(n->o.string);
		break;
	case 'o':
		n->oidType=OTObject;
		if (AsciiToOid(&n->o.object, action+2, 0)!=0)
			exit(1);
		break;
	case '0':
		n->oidType=OTNull;
		break;
	case 's':
		n->oidType=OTString;
		n->o.string=XStrDup(XGetString(action+2));
		n->dataLen=strlen(n->o.string);
		break;
	default:
		XLog(LOG_ERR, "(OidAdd) Unknown returntype '%c' (%s %s)", action[0], oid, action);
		exit(1);
	}

	oidList=n;
}

/*
**	GetSInt
**
**	Pickup signed int from string. Store choiceId and number in OidNode.
**
**	 -> n		OID node.
**	 -> src		Source string.
**	 -> type	Type.
**	 -> oid		Oid (string) (NULL ok).
**	 -> name	Name (for error).
*/
static void
GetSInt(OidNode *n, char *src, int type, char *oid, char *name)
{
	int i;
	char *p;

	i=strtol(src, &p, 10);
	if (src==p || *p) {
		if (oid) {
			XLog(LOG_ERR, "(GetInt) Failed to convert %s (%s %s)", name, oid, src);
			exit(1);
		} else	// Ok to return from here - we come from set spawn
			XLog(LOG_ERR, "(GetInt) Failed to convert %s (%s)", name, src);
	}

	n->oidType=type;
	n->o.sNumber=i;
}

/*
**	GetUInt
**
**	Pickup unsigned int from string. Store choiceId and number in OidNode.
**
**	 -> n		OID node.
**	 -> src		Source string.
**	 -> type	Type.
**	 -> oid		Oid (string) (NULL ok).
**	 -> name	Name (for error).
*/
static void
GetUInt(OidNode *n, char *src, int type, char *oid, char *name)
{
	unsigned int i;
	char *p;

	i=strtoul(src, &p, 10);
	if (src==p || *p) {
		if (oid) {
			XLog(LOG_ERR, "(GetInt) Failed to convert %s (%s %s)", name, oid, src);
			exit(1);
		} else	// Ok to return from here - we come from set spawn
			XLog(LOG_ERR, "(GetInt) Failed to convert %s (%s)", name, src);
	}

	n->oidType=type;
	n->o.uNumber=i;
}

/*
**	OidToDottedAscii
**
**	Extract OID to dotted ascii form.
**
**	 -> oid		OID.
**
**	Returns string representation.
*/
char *
OidToDottedAscii(AsnOcts *oid)
{
	int size=2, arcNum, i, faNum;
	char *s, *t;

	/* calculate length */
	for (i=0; (i<oid->octetLen) && (oid->octs[i]&0x80); i++);

	i++;

	for (; i<oid->octetLen; i++) {
		for (arcNum=0; (i<oid->octetLen) && (oid->octs[i]&0x80); i++);
		size++;
	}

	/* adjust size and allocate */
	size<<=2;
	size++;

	if ((s=NibbleAlloc(size))==NULL)
		return NULL;

	/* create string */
	t=s;
	arcNum=0;
	for (i=0; (i<oid->octetLen) && (oid->octs[i]&0x80); i++)
		arcNum=(arcNum<<7)+(oid->octs[i]&0x7f);

	arcNum=(arcNum<<7)+(oid->octs[i]&0x7f);
	i++;

	faNum=arcNum/40;
	if (faNum>2)
		faNum=2;

	t+=sprintf(t, "%i", faNum);
	t+=sprintf(t, ".%i", arcNum-(faNum*40));

	for (; i<oid->octetLen; ) {
		for (arcNum=0; (i<oid->octetLen) && (oid->octs[i]&0x80); i++)
			arcNum=(arcNum<<7)+(oid->octs[i]&0x7f);

		arcNum=(arcNum<<7)+(oid->octs[i]&0x7f);
		i++;
		t+=sprintf(t, ".%i", arcNum);
	}

	return s;
}

/*
**	AsciiToOid
**
**	Convert ascii to oid.
**
**	 -> dst		Destination OID.
**	 -> src		Source oid string.
**	 -> nibble	Use nibble mem (0 = no, !0 = yes).
**
**	Returns 0 if successfull, !0 otherwise.
*/
int
AsciiToOid(AsnOcts *dst, char *src, int nibble)
{
	unsigned long head, i;
	char *end, *d;
	int hLen;

	if (*src=='.')
		src++;

	/* munge head */
	head=strtol(src, &end, 10);
	if (*end!='.') {
		XLog(LOG_ERR, "(AsciiToOid) Not enough elements");
		return -1;
	}

	i=strtol(end+1, &end, 10);

	head=(40*head)+i;
	
	for (dst->octetLen=1; (head>>=7)!=0; dst->octetLen++);

	for (; *end=='.'; ) {
		head=strtol(end+1, &end, 10);
		dst->octetLen++;
		for (; (head>>=7)!=0; dst->octetLen++);
	}

	if (*end) {
		XLog(LOG_ERR, "(AsciiToOid) Garbled OID");
		return -1;
	}

	/* Build oid */
	if (nibble)
		dst->octs=NibbleAlloc(dst->octetLen);
	else
		dst->octs=calloc(dst->octetLen, 1);

	if (!dst) {
		XLog(LOG_ERR, "(AsciiToOid) Out of memory");
		return -1;
	}

	head=strtol(src, &end, 10);
	i=strtol(end+1, &end, 10);

	head=(40*head)+i;

	i=head;
	for (hLen=0; (i>>=7)!=0; hLen++);

	d=dst->octs;
	for (i=0; i<hLen; i++)
		*d++=0x80|(head>>((hLen-i)*7));

	*d++=head&0x7f;
	
	for (; *end=='.'; ) {
		head=strtol(end+1, &end, 10);
		i=head;
		for (hLen=0; (i>>=7)!=0; hLen++);

		for (i=0; i<hLen; i++)
			*d++=0x80|(head>>((hLen-i)*7));

		*d++=head&0x7f;
	}

	return 0;
}

/*
**	OidGetNode
**
**	Get oid node.
**
**	 -> oid		OID.
**
**	Returns oid node or NULL (if not found).
*/
OidNode *
OidGetNode(char *oid)
{
	OidNode *n;

	for (n=oidList; n; n=n->next) {
		if (strcmp(n->oid.octs, oid)==0)
			return n;
	}

	return NULL;
}

/*
**	OidGetEncodeNode
**
**	Get oid node given it's encoded form.
**
**	 -> eOid	Encode OID.
**
**	Returns oid node or NULL (if not found).
*/
OidNode *
OidGetEncodedNode(AsnOcts *eOid)
{
	OidNode *n;

	for (n=oidList; n; n=n->next) {
		if (memcmp(n->encodedOid.octs, eOid->octs, eOid->octetLen)==0)
			return n;
	}

	return NULL;
}

/*
**	OidLocal
**
**	Retrieve local data for oid.
**
**	 -> os		ObjectSyntax.
**	 -> oid		OID.
**
**	Return error status (or 0).
*/
int
OidLocal(struct ObjectSyntax *os, OidNode *oid)
{
	if (oid->oidType==OTSpawn) {
		if ((oid=SetSpawn(oid))==NULL)
			return GENERR;
	}

	switch (oid->oidType) {
	case OTNumber:
		if ((os->a.simple=TypeSetSimpleNumber(os, oid->o.sNumber))!=NULL)
			return NOERROR;
		break;
	case OTString:
		if ((os->a.simple=TypeSetSimpleString(os, oid->o.string, oid->dataLen, SIMPLESYNTAX_STRING))!=NULL)
			return NOERROR;
		break;
	case OTObject:
		if ((os->a.simple=TypeSetSimpleString(os, oid->o.object.octs, oid->o.object.octetLen, SIMPLESYNTAX_OBJECT))!=NULL)
			return NOERROR;
		break;
	case OTArbitrary:
		if ((os->a.application_wide=TypeSetApplicationString(os, oid->o.arbitrary, oid->dataLen, APPLICATIONSYNTAX_ARBITRARY))!=NULL)
			return NOERROR;
		break;
	case OTCounter:
		if ((os->a.application_wide=TypeSetApplicationNumber(os, oid->o.uNumber, APPLICATIONSYNTAX_COUNTER))!=NULL)
			return NOERROR;
		break;
	case OTGauge:
		if ((os->a.application_wide=TypeSetApplicationNumber(os, oid->o.uNumber, APPLICATIONSYNTAX_GAUGE))!=NULL)
			return NOERROR;
		break;
	case OTTicks:
		if ((os->a.application_wide=TypeSetApplicationNumber(os, oid->o.uNumber, APPLICATIONSYNTAX_TICKS))!=NULL)
			return NOERROR;
		break;
	case OTNull:
		if ((os->a.simple=TypeSetSimpleNull(os))!=NULL)
			return NOERROR;
		break;
	case OTIpAddress:
		if ((os->a.application_wide=TypeSetApplicationIp(os, oid->o.ip))!=NULL)
			return NOERROR;
		break;
	default:
		break;
	}

	return GENERR;
}

/*
**	SetSpawn
**
**	Spawn program, read output. Copy output to new OidNode.
**
**	 -> n		OidNode.
**
**	Return new READ-ONLY OidNode or NULL.
*/
static OidNode *
SetSpawn(OidNode *n)
{
	OidNode *t=NibbleAlloc(sizeof(OidNode));
	int totBytesRead=0, bytesRead, fileReadError, o1, o2, o3, o4, len;
	char buffer[MAXBUFFERSIZE];
	char *d, *p;
	FILE *f;

	if ((f=popen(n->o.spawn, "r"))==NULL) {
		XLog(LOG_WARNING, "(SetSpawn) Cannot spawn command (%s)", n->o.spawn);
		return NULL;
	}

	DynBufReset();
	while (!feof(f) && !ferror(f)) {
		if ((bytesRead=fread(buffer, 1, MAXBUFFERSIZE, f))>0) {
			DynBufAdd(buffer, bytesRead);
			totBytesRead+=bytesRead;
		}
	}
	fileReadError=ferror(f);
	pclose(f);

	if (fileReadError) {
		XLog(LOG_ERR, "(SetSpawn) Error while reading from command (%s)", n->o.spawn);
		return NULL;
	}

	t->oid.octetLen=n->oid.octetLen;
	t->dataLen=totBytesRead-2;

	if ((d=DynBufPtr())==NULL) {
		XLog(LOG_ERR, "(SetSpawn) Command did not return any output (%s)", n->o.spawn);
		return NULL;
	}

	if (totBytesRead>2) {
		len=DynBufLen();
		do {
			len--;
			if (isspace((int)d[len]))
				d[len]='\x00';
			else
				break;

		} while (len>1);
		
		switch (*d) {
		case 'i':
			t->oidType=OTIpAddress;
			if (XPickupIPAddress(d+2, &o1, &o2, &o3, &o4)!=0) {
				XLog(LOG_ERR, "(SetSpawn) IP Address invalid (%s)", d+2);
				exit(1);
			}

			p=&t->o.ip[0];
			*p++=(unsigned char)o1;
			*p++=(unsigned char)o2;
			*p++=(unsigned char)o3;
			*p++=(unsigned char)o4;
			*p='\x00';

			t->dataLen=4;
			return t;
		case 'n':
			GetSInt(t, d+2, OTNumber, NULL, "number");
			return t;
		case 'c':
			GetUInt(t, d+2, OTCounter, NULL, "counter");
			return t;
		case 'g':
			GetUInt(t, d+2, OTGauge, NULL, "gauge");
			return t;
		case 't':
			GetUInt(t, d+2, OTTicks, NULL, "ticks");
			return t;
		case 's':
			t->oidType=OTString;
			t->o.string=d+2;
			t->dataLen=strlen(d+2);
			return t;
		case 'a':
			t->oidType=OTArbitrary;
			t->o.arbitrary=d+2;
			t->dataLen=strlen(d+2);
			return t;
		case 'o':
			t->oidType=OTObject;
			if (AsciiToOid(&t->o.object, d+2, 1)!=0)
				return NULL;

			return t;
		default:
			XLog(LOG_WARNING, "(SetSpawn) (%s): command returned false output ('%s')", n->o.spawn, d);
			break;
		}
	} else if (*d=='0') {
		t->oidType=OTNull;
		return t;
	} else
		XLog(LOG_WARNING, "(SetSpawn) (%s): command returned too little information ('%s')", n->o.spawn, d);

	return NULL;
}

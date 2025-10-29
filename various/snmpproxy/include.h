#ifndef INCLUDE_H
#define INCLUDE_H

#ifdef HPUX
#include <arpa/inet.h>
#else
#include <netinet/in.h>
#endif
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <pwd.h>
#include <signal.h>
#ifdef LINUX
#include <stdarg.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#ifdef HPUX
#include <varargs.h>
#endif
#include "asn-incl.h"
#include "rfclib/rfc1155.h"
#include "rfclib/rfc1157.h"

/* macros */
#define EVER ;;
#define PKGBUFFERLEN 1024
#define MASQBUFFERLEN 1024
enum { OTMasquerade=0, OTSpawn, OTIpAddress, OTCounter, OTGauge, OTNumber, OTTicks, OTArbitrary, OTObject, OTNull, OTString };
enum { GNONormal=0, GNOTerm };

/* structs */
typedef struct {
	struct in_addr network;
	struct in_addr mask;
} Network;

typedef struct {
	unsigned char *name;
	unsigned short port;
	struct in_addr address;
	time_t tick;
} Host;

struct OidNode {
	struct OidNode *next;
	int oidType;
	AsnOcts oid;
	AsnOcts encodedOid;
	union OidNodeTypeUnion {
		AsnOcts masquerade;
		AsnOcts object;
		char *spawn;
		char *string;
		char ip[5];
		char *arbitrary;
		unsigned int uNumber;
		int sNumber;
	} o;
	unsigned int dataLen;
};
typedef struct OidNode OidNode;

struct GetNextOid {
	struct GetNextOid *next;
	AsnOcts oid;
	AsnOcts encodedOid;
	int type;
};
typedef struct GetNextOid GetNextOid;

struct FakeList {
	struct FakeList *next;
	AsnOcts oid;
	AsnOcts encodedOid;
	AsnOcts nextOid;
	AsnOcts nextEncodedOid;
};
typedef struct FakeList FakeList;

/* config.c */
void ConfigDump(void);
void ConfigLoad(char *);

/* dynbuf.c */
void DynBufInit(int);
char *DynBufPtr(void);
int DynBufLen(void);
void DynBufAdd(char *, int);
void DynBufReset(void);

/* oid.c */
void OidDump(void);
int OidIsOid(char *);
void OidDumpGetNextList(void);
void OidDumpFakeList(void);
void OidAdd(char *, char *);
char *OidToDottedAscii(AsnOcts *);
int AsciiToOid(AsnOcts *, char *, int);
OidNode *OidGetNode(char *);
OidNode *OidGetEncodedNode(AsnOcts *);
int OidLocal(struct ObjectSyntax *, OidNode *);

/* proxy.c */
void Proxy(void);
unsigned char *VerifyPackage(unsigned char *, int *);
int DisectPackage(Message *, SBuf *, unsigned char *, int);
int ProxyPackage(Host *, unsigned char *, int);
Message *Masquerade(Message *, OidNode *, int);
Message *CopyMessage(Message *, OidNode *, int, int);
int XmitMessage(Message *m);
void CopyVarBind(VarBind *, Message *);

/* rget.c */
int GetRequest(Message *);

/* rnext.c */
int GetNextRequest(Message *);

/* trusted.c */
void TrustedDump(void);
void TrustedAdd(Network *);
int TrustedAccess(struct in_addr *);

/* types.c */
SimpleSyntax *TypeSetSimpleString(ObjectSyntax *, char *, unsigned int, int);
SimpleSyntax *TypeSetSimpleNumber(ObjectSyntax *, unsigned int);
SimpleSyntax *TypeSetSimpleNull(ObjectSyntax *);
ApplicationSyntax *TypeSetApplicationString(ObjectSyntax *, char *, unsigned int, int);
ApplicationSyntax *TypeSetApplicationNumber(ObjectSyntax *, unsigned int, int);
ApplicationSyntax *TypeSetApplicationIp(ObjectSyntax *, char *);

/* utils.c */
#ifdef HPUX
void XLog();
#else
void XLog(int, const char *, ...);
#endif
void XCaptureToFile(char *, char *, void *, int);
void *XMalloc(int);
char *XStrDup(char *);
#ifdef HPUX
char *XStrCat();
#else
char *XStrCat(char *, ...);
#endif
void XLookup(Host *, int);
void XIPToDottedAscii(char *, struct in_addr *);
int XGetIPAddress(struct in_addr *, char *);
int XGetHost(Host *, char *, int);
char *XGetString(char *);
int XPickupIPAddress(char *, int *, int *, int *, int *);

#ifdef SNMPPROXYMAIN
	int sFd=-1, pDebug;
	socklen_t clientLen;
	unsigned int pPort, pCacheName, pRetry, pTimeout;
	unsigned int pDumpMessage;
	int hasStdout;
	char *pPidFile, *pCaptureFile, *pCommunity, *pUser;
	Host pPeer;
	struct sockaddr_in client;
	unsigned char *pkgBuffer, *masqBuffer;
	int pkgLength;
	GetNextOid *gnl;
	FakeList *fakeList;
#else
	extern int sFd, pDebug;
	extern socklen_t clientLen;
	extern unsigned int pPort, pCacheName, pRetry, pTimeout;
	extern unsigned int pDumpMessage;
	extern int hasStdout;
	extern char *pPidFile, *pCaptureFile, *pCommunity, *pUser;
	extern Host pPeer;
	extern struct sockaddr_in client;
	extern unsigned char *pkgBuffer, *masqBuffer;
	extern int pkgLength;
	extern GetNextOid *gnl;
	extern FakeList *fakeList;
#endif

#endif

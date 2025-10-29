#ifndef MACROS_H
#define MACROS_H
#define DEBUG

/* macros */
#define VERSION "1.0"
#define QNAME "MTA-1"
#define QCNF "etc/qmta.cnf"
#define QACCESS "etc/access"
#define QACCESSDB "var/access.qdb"
#define QDOMAIN "etc/domain"
#define QDOMAINDB "var/domain.qdb"
#define QSPAMMER "etc/spammer"
#define QSPAMMERDB "var/spammer.qdb"

#define PIDDIR "var/"
#define CNTLFILENAMELEN 7
#define EVER ;;
#define IPADDRSPRDOMAIN 100	/* IP addresses per domain */
#define HUGELINELEN 1026	/* pr. rfc821 (it says 1000, 1026 is magic) */
#define READBUFFERSIZE 2048	/* global readbuffer */
#define STATUSLINELEN 512	/* pr. rfc821 */
#define DNSPARTLEN 64
#define PREATLEN 64
#define DNSFQDNLEN (DNSPARTLEN+DNSPARTLEN+4)
#define MAILBOXLEN (PREATLEN+DNSFQDNLEN+2)	/* '@' */

#ifdef DEBUG
#define debug(x) x;
#define debugout(fmt, args...) fprintf(stderr, fmt, ##args);fflush(NULL);
#else
#define debug(x) ;
#define debugout(x) ;
#endif

#define SKIPBLANKS(x) for (; (*x) && isspace((int)(*x)); (x)++)
#define SKIPTOWHITE(x) for (; (*x) && !isspace((int)(*x)); (x)++)
#define FINDEOL(x) for (; (*x) && ((*x)!='\n'); (x)++)

#define HUGELINELEN 1026	/* ref rfc821 */

#ifdef OSLinux
typedef unsigned long bit32;
#endif

#ifdef OSOSF1
typedef unsigned int bit32;
#endif

/* texts */
#define QTXTTRANSACTION "txt/transactionstatus"
#define QTXTVIRUS "txt/virusinfected"
#define QTXTMAILAGED "txt/agedmail"
#define QTXTSCANNERPROBLEMS "txt/scannerproblems"

/* recipients */
#define RSUNHANDLED ' '
#define RSHANDLED '+'

/* spamcheck + modules */
#define SCINNOCENT 0	/* host not listed */
#define SCSPAMMER 1	/* host is listed, ie. a spammer */
#define SCPARK 2	/* cannot decide, remote must requeue */

#endif

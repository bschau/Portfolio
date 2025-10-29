#ifndef LIBQMTA_H
#define LIBQMTA_H

#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <pwd.h>
#include <resolv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#ifdef OSOSF1
#include <sys/table.h>
#endif

#ifdef OSLinux
#include <arpa/nameser.h>
#include <signal.h>
#include <time.h>
#endif

#include "macros.h"

/* structs */
struct mxnode {
	struct mxnode *next;
	unsigned short weight;
	char *exchange;
};
typedef struct mxnode mxnode;

typedef struct qsiglist {
	int signum;
	void (*action)();
} qsiglist;

struct dbnode {
	struct dbnode *next;
	struct dbnode *prev;
	char *key;
	char *data;
	int keylen;
	int datalen;
};
typedef struct dbnode dbnode;

struct dblist {
	struct dbnode *head;
	struct dbnode *tail;
	struct dbnode *tailpred;
	int items;
	int htsize;
	FILE *file;
	unsigned long offset;
};
typedef struct dblist dblist;

struct database {
	FILE *f;
	int htsize;
	char *dptr;
	int dlen;
};
typedef struct database database;

struct recipients {
	char *recip;
	char *rptr;
};
typedef struct recipients recipients;

struct session {
	char *cntlname;
	char *rcptname;
	char *mailname;
	int cntlfd;
	FILE *rcptfile;
	FILE *mailfile;
	time_t access;			/* A */
	time_t created;			/* C */
	time_t dequeue;			/* D */
	char from[MAILBOXLEN+2];	/* F */
	int messagetype;		/* M */
	char hostname[DNSPARTLEN+2];
	char domainname[DNSPARTLEN+2];
	char fqdn[DNSFQDNLEN+2];
	char postmaster[MAILBOXLEN+2];
	unsigned long remoteip;
	char remotename[DNSFQDNLEN+2];
	char *remoteaddr;
	char *to;
	recipients *recipients;
};
typedef struct session session;

/* control.c */
int checkdequeue(session *, recipients *, int, char *, int, time_t, int *);
int dequeue(session *, char *);
int findheader(session *, char *, char *, int);
int getmailbox(char *, char *);

/* database.c */
void initdblist(dblist *);
void freedblist(dblist *);
int adddblistentry(dblist *, char *, char *, int);
int populate(dblist *, char *);
int qdbopen(database *, char *);
void qdbclose(database *);
int qdbfetch(database *, char *);

#ifdef DEBUG
/* debug */
void dumpsession(session *);
#endif

/* domain.c */
mxnode *mxlookup(char *);
char *getdomain(char *);
char *islocal(database *, char *);

/* elink.c */
void elinkopen(void);
void elinkclose(void);
int elinkadd(char *);
void elinktravstart(void);
char *elinknext(void);
int elinkstatus(void);

/* expand.c */
int s2sexpand(session *, char *, char *, int);
int f2fexpand(session *, FILE *, FILE *, int);
int s2fexpand(session *, char *, FILE *, int);

/* file.c */
int writefd(int, char *, int);
int writefile(FILE *, char *, int);
int getfilesize(char *);
int lockfile(int);
void unlockfile(int);
char *getfile(char *);
char *relpath(char *, char *);
FILE *fopenrel(char *, char *, char *);

/* injectmail.c */
void doneinjectmail(void);
int injectmail(session *, char *);

/* loadavg.c */
int isbusy(void);

/* mailbox.c */
int validdomain(char *);
int validmailbox(char *);
char *getdomain(char *);

/* main.c */
void usage(int);
int common(int, char **, int (*)(char, char *), char *, char *, void (*)(void), void (*)(int), int, int);
#define NORMAL 0
#define DAEMONIZE 1
#define ALLUSERS 0
#define ROOTONLY 1
void syscleanup(void);

/* misc.c */
void qlog(int, const char *, ...);
#define QFATAL LOG_ALERT	/* when the situation cannot be saved and the
				   master driver/child must shut down - this
				   log code is usually followed by 'exit(X)' */
#define QWARN LOG_ERR		/* when something unexpected happens */
#define QINFO LOG_WARNING	/* informational message */
#define QSMTP LOG_NOTICE	/* smtp stats */
#define QDEBUG LOG_DEBUG	/* debug messages */
int nsleep(time_t, long);
char *xconcat(char *, char *);
void safefree(char **);

/* mxlist.c */
void mxdestroylist(mxnode **);
mxnode *mxalloc(unsigned short, char *);
void mxdestroy(mxnode *);
mxnode *mxinsert(mxnode *, mxnode *);

/* net.c */
int nwrite(int, char *, int);
int ngetline(int, char *, int);
int netstatusw(int, session *, char *);
unsigned short getsmtpport(void);
void resolveip(session *, uint32_t, char *);

/* preferences.c */
void setroot(void);
int configure(void);
void deconfigure(void);
void dumpconfig(void);
int helpconfig(char *);
int varconfig(char *);

/* process.c */
void background(void);
void foreground(void);
int isrunning(void);
int reap(int *);

/* rbl.c */
int rbl(char *, char *, int);

/* recipients.c */
int loadrecipients(recipients *, char *);
void rewindrecipients(recipients *);
void freerecipients(recipients *);
char *getrecipient(recipients *);
char *nextrecipient(recipients *, int);
void updaterecipient(recipients *, char);
int countrecipients(recipients *);
int flushrecipients(recipients *, FILE *);
#define RSUNHANDLED ' '
#define RSHANDLED '+'

/* sendmail.c */
void donemail(void);
int sendmail(char *, int, database *, int, int);
#define SMSILENT 0
#define SMVERBOSE 1
#define SMQUEUE 0
#define SMFORCE 1

/* session.c */
void macroize(session *);
int newsession(session *);
int initsession(session *, int, char *);
void freesession(session *, int);
int putsession(session *);
int getsession(session *, int);
#define SESSDELETE 0
#define SESSNODELETE 1

/* sophos.c */
int sophosopen(int);
void sophosclose(void);
void sophoslist(void);
int sophos(char *);
#define SCLEAN 0
#define SINFECTED 1

/* spamcheck.c */
int spamcheck(char *, int);
#define SCSILENT 0
#define SCVERBOSE 1

/* system.c */
void installsignals(qsiglist *);
int dropprivs(char *);
void requireroot(void);

#ifdef MAIN
	char *qroot;		/* root of all evil */
	char *privlogf;		/* 'logfacility' */
	int comtimeout;		/* 'inputcomtimeout' */
	int imaxchilds;		/* 'inputmaxchilds' */
	char *localmailto;	/* 'localmailto' */
	int logrecipients;	/* 'logrecipients' */
	int mailretry;		/* 'mailretry' */
	int maxage;		/* 'maxage' */
	int maxload;		/* 'maxload' */
	int maxmailsize;	/* 'maxmailsize' */
	int maxrecipients;	/* 'maxrecipients' */
	char *munpackbin;	/* 'munpack' */
	int omaxchilds;		/* 'outputmaxchilds' */
	char *rblhost;		/* 'rbl' */
	char *relayname;	/* 'relayname' */
	int sleephard;		/* 'sleephard' */
	int sleepqueue;		/* 'sleepqueue' */
	int sleepsoft;		/* 'sleepsoft' */
	int spamcheckact;	/* 'spamcheck' */
	char *spooldir;		/* 'spooldirectory' */
	int smaxsweep;		/* 'sweepalarm' */
	int smaxchilds;		/* 'sweepchilds' */
	char *user;		/* 'user' */
	int itime;		/* 'inputtime' */
	int otime;		/* 'outputtime' */
	char *ideurl;		/* 'ideurl' */
	char *savihome;		/* 'savihome' */
	int logfacility;	/* piggyback (privlogf) */
	int runningchilds;	/* number of running childs */
	int maxchilds;		/* max childs */
	int iofd;		/* io descriptor */
	int mypid;		/* is pid file mine? */
	char *pidfile;		/* pidfile */
	char *whoami;		/* as it says */
	int master;		/* am I master? */
	int interdelay;		/* delay between forks */
	char line[HUGELINELEN+2];
#else
	extern char *qroot, *privlogf, *spooldir, *user;
	extern char *usagestring, *pidfile, *whoami, *relayname;
	extern char *localmailto, *munpackbin, *rblhost;
	extern char *ideurl, *savihome;
	extern int maxload, maxmailsize, mailretry, logrecipients;
	extern int maxrecipients, sleephard, sleepqueue, sleepsoft;
	extern int imaxchilds, omaxchilds, comtimeout, maxage;
	extern int logfacility, runningchilds, maxchilds, spamcheckact;
	extern int smaxsweep, smaxchilds, iofd, mypid, master;
	extern int itime, otime, interdelay;
	extern char line[];
#endif

/* all */
extern int errno;

#endif

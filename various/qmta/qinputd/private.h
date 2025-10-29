#ifndef PRIVATE_H
#define PRIVATE_H

#include "libqmta.h"
#include "smtptext.h"

/* macros */
#define ACCEPT 0
#define DENY 1
#define NONE 2
#define NETSTATUSW(y) netstatusw(ch->fd, sess, y)

/* structs */
typedef struct child {
	int fd;			/* socket to peer */
	unsigned long peerip;	/* peer remote ip */
	char *helodomain;	/* domain given at HELO/EHLO */
	char *response;		/* response string */
	time_t startconn;	/* connection time start */
	database accessdb;	/* database access */
	database domaindb;	/* database domain */
	database spammerdb;	/* database spammer */
	/* session */
	session *sess;		/* session block */
	int validmail;		/* is the mail valid */
} child;

/* brain.c */
void endless(void);

/* child.c */
void mainchild(int, struct sockaddr_in *);

/* mailbox.c */
int validdomain(char *);
int validmailbox(char *);

/* net.c */
void initnet();
void closenet(void);
int getserverfd(void);

/* session.c */
int createsession(child *);
void destroysession(child *);
int sessionize(child *);

/* smtp.c */
int smtp(child *);

#endif

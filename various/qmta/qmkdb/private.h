#ifndef PRIVATE_H
#define PRIVATE_H

#ifdef OSLinux
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#include "libqmta.h"

/* access.c */
void mkdbaccess(void);

/* brain.c */
void brain(char *, char *, int (*)());

/* domain.c */
void mkdbdomain(void);

/* spammer.c */
void mkdbspammer(void);

#ifdef MAIN
	dblist dbl;
#else
	extern dblist dbl;
#endif

#endif

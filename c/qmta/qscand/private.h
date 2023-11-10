#ifndef PRIVATE_H
#define PRIVATE_H

#include <sys/types.h>
#include <dirent.h>

#include "libqmta.h"

/* brain.c */
void endless(void);

/* child.c */
void mainchild(char *);

/* infected.c */
void doneinfected(void);
void infected(char *, char *);

/* munpack.c */
int munpack(char *);

/* sweep.c */
void sweepdone(void);
void sweep(char *);

#ifdef MAIN
	int cfd=-1;
#else
	extern int cfd;
#endif

#endif

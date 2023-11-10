#ifndef PRIVATE_H
#define PRIVATE_H

#include "libqmta.h"
#include "helptext.h"
#include "savi/csavi2c.h"

/* macros */
#define CFAINT 1
#define CFASTR 2
#define MAXSIGNALS 16

/* structs */
typedef struct cfa {
	char *name;
	char *description;
	char **help;
	int type;
	int *idst;
	int iinit;
	char **sdst;
	char *sinit;
	int private;
} cfa;

/* errors.c */
void erroropen(void);
void errorraise(int, int);
int errorget(int);
#define EGTYPE 1
#define EGACTION 2
/* error-types */
#define ERRNOERR 0	/* no error */
#define ERRMAIL 1	/* mail format error (missing from/to etc.) */
#define ERRSMTP 2	/* smtp error (usually implies requeueing) */
/* error-actions */
#define ERRNOTHING 0		/* do nothing */
#define ERRGENERIC500 100	/* catch all generic 5xx */
#define ERRADDRESS 101		/* return address error mail */
#define ERRINVALIDFROM 102	/* rejected sender mailbox - severe */
#define ERRINVALIDTO 103	/* rejected reciever mailbox */
#define ERRREQUEUE 201		/* requeue mail message */

/* mail.c */
void mail(database *, char *);

/* sanity.c */
void sanity(void);

/* smtp.c */
int smtp(char *);

#ifdef MAIN
	int sck=-1, verbose;
	session gs;
	mxnode *mxl;
	FILE *emf;
#else
	extern int sck, verbose;
	extern session gs;
	extern mxnode *mxl;
	extern FILE *emf;
#endif

#ifndef CFASWITCHES
	extern cfa cfaconfig[];
#endif

/* all */
extern int h_errno;

#endif

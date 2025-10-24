#ifndef INCLUDE_H
#define INCLUDE_H

#include <stdio.h>

#ifdef WIN32
#define strncasecmp strnicmp
#endif

/* macros */
#define EVER ;;

/* Utils.c */
void XError(const char *, ...);
void *XAlloc(int);
char *XStrInStr(char *, char *);
char *XStrDup(char *);
char *XTrim(char *);

#endif

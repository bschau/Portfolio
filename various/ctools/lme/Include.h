#ifndef INCLUDE_H
#define INCLUDE_H

#include <stdio.h>
#define USES_STRINGBUILDER
#define USES_OPTIONS
#define USES_HASHTABLE
#ifdef WIN32
#define USES_WINUTILS
#endif
#include "Common.h"
#include "Version.h"

/* macros */
#define EVER ;;
enum { XOSilent=0, XOInfo, XODebug };

/* OS.c */
char *OSUserId(void);

/* Parse.c */
void Parse(char *);

/* Properties.c */
void PropertiesRead(char *);

/* XUtils.c */
void XError(void *, const char *, ...);
void XOut(int, const char *, ...);
char *XReadFile(char *);

/* globals */
#ifdef MAIN
#ifdef WIN32
	char *OS="WIN32";
	char dirSepChar='\\';
#else
	char *OS="Linux";
	char dirSepChar='/';
#endif
	int verbose=XOSilent;
	char *startTag="$(", *endTag=")";
	int startTagLen, endTagLen;
	HashTable vars;
	FILE *outFile;
	char *inFile;
#else
	extern char *OS;
	extern char dirSepChar;
	extern int verbose;
	extern char *startTag, *endTag;
	extern int startTagLen, endTagLen;
	extern HashTable vars;
	extern FILE *outFile;
	extern char *inFile;
#endif

#endif

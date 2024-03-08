#ifndef INCLUDE_H
#define INCLUDE_H

#include <stdio.h>
#define USES_STRINGBUILDER
#define USES_OPTIONS
#ifdef WIN32
#define USES_WINUTILS
#endif
#include "Common.h"
#include "Version.h"

/* macros */
#define EVER ;;
enum { XOSilent=0, XOInfo, XODebug };

/* structs */
struct Node {
	struct Node *next;
	char *key;
	char *desc;
	char *signature;
	char *param;
	char *ret;
	char *note;
};

/* Output.c */
void Output(void);
void Index(void);

/* Parse.c */
void ParseOpen(void);
void ParseClose(void);
void ParseNew(char *);
void ParseFree(void);
void Parse(void);

/* XUtils.c */
void XError(void *, const char *, ...);
void XOut(int, const char *, ...);

/* globals */
#ifdef MAIN
	char *styleSheet="body { font-family: Verdana, Arial, serif } \
table { border-spacing: 10px; } \
h1 {} \
td.left { vertical-align: top; } \
a.left { font-color: red; } \
td.right {} \
p.global {} \
p.entry { font-size: 1.1em; font-weight: bold; } \
div.signature { margin-left: 2cm; } \
p.desc { margin-left: 2cm; } \
p.right { font-weight: bold; margin-left: 2cm; } \
.npr { margin-left: 4cm; margin-top: -1em; } \
h3 { border: solid 1px; width: 100%; background-color: #87cefa; color: black; } \
code {} \
a.index {} \
a.selected { font-size: 1.1em; border: 1px solid black; }";
	char *currentFile;
	int verbose=XOSilent, lineNo, addStatics;
	struct Node *gList, *eList, *sList, *vList, *fList, *tList;
#else
	extern char *styleSheet, *currentFile;
	extern int verbose, lineNo, addStatics;
	extern struct Node *gList, *eList, *sList, *vList, *fList, *tList;
#endif

#endif

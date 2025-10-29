#ifndef INCLUDE_H
#define INCLUDE_H

#include <stdio.h>
#define USES_HASHTABLE
#define USES_OPTIONS
#define USES_STRINGBUILDER
#ifdef WIN32
#define USES_WINUTILS
#endif
#include "Common.h"
#include "Version.h"

/* macros */
#define EVER ;;
typedef void * Var;
enum { XOSilent=0, XOInfo, XODebug };
/* Do not reorder this list. Add to the end. */
enum { TEOF=0, TEOL, TString, TProcedure, TAssignment, TVariable, TPositional, TIf, TElse, TEndIf, TInclude, TEqual, TAnd, TOr, TListStart, TListEnd, TIfDef, TIfNDef, TExit, TIn, TMsg, TTest };

/* structs */
typedef struct {
	Var *var;
	char *cwd;
	char *lmakeFile;
	StringBuilder tb;
	StringBuilder sb;
	StringBuilder exp;
	StringBuilder args;
	int lineNo;
	int ungetc;
	int inQuote;
	FILE *fp;
	char *src;
	char *procedure;
	int verbose;
	int retCode;
	int argc;
	char **argv;
	int forceOutput;
	int suppressCommand;
} CTX;

typedef struct {
	char *data;
	int dLen;
	char *file;
	int lineNo;
} VARDesc;

/* Command.c */
int (*CmdLookup(char *))(CTX *);
void CmdDryRun(CTX *);

/* Context.c */
CTX *CTXNew(Var, char *, char *);
void CTXOpen(CTX *, char *);
void CTXClose(CTX *);
void CTXFree(CTX *);

/* Expand.c */
char *Expand(CTX *, char *);

/* External.c */
int ExternalRun(CTX *, char *);

/* Lexer.c */
int GetChar(CTX *);
void SkipWhite(CTX *);
int GetEOL(CTX *);
void NextLine(CTX *);
int GetToken(CTX *);
int Peek(CTX *);

/* LMake.c */
int LMake(Var, char *, char *, int, int, char **);

/* LMParse.c */
void LMParse(CTX *);

/* Main.c */
void Run(Var, int, char **, int);

/* OS.c */
char *OSUserId(void);
void InitVars(Var);

/* TIf.c */
#define IFIsTest 0
#define IFIsIf 1
void TIfDo(CTX *, int);
void TIfDefDo(CTX *);
void TIfNDefDo(CTX *);
void TElseDo(CTX *);

/* Var.c */
Var VarNew(Var);
void VarFree(Var);
void VarInitialize(Var);
int VarGet(Var, char *, VARDesc *);
void VarSet(Var, char *, void *, int, int, char *);
void VarSetSystem(Var, char *, void *, int, int, char *);
int VarDelete(Var, char *);
void VarDump(Var);

/* XUtils.c */
void XError(void *, const char *, ...);
void XOut(CTX *, int, const char *, ...);
void XChDir(CTX *, char *);
char *XGetCwd(CTX *);
char *XRealPath(char *);
void XDumpArgs(int, char **);
char *XGetToken(CTX *, char *, int *, char **);
void XOptionPickUp(CTX *, char *, int *, char ***);
void XOptionFree(char **);

/* globals */
#ifdef MAIN
	Var globals;
	int dryRun;
#else
	extern Var globals;
	extern int dryRun;
#endif

#endif

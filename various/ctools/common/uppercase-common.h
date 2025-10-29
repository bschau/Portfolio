#ifndef COMMON_H
#define COMMON_H

#ifdef USES_HASHTABLE
typedef void * HashTable;
HashTable HashNew(int);
void HashFree(HashTable, int);
#define HTPreserveValue 0
#define HTFreeValue 1
int HashAdd(HashTable, const char *, const void *, int, int);
void *HashGet(HashTable, const char *, int *);
int HashDelete(HashTable, const char *);
int HashFindFirst(HashTable, const char **, const void **, int *);
int HashFindNext(HashTable, const char **, const void **, int *);
#endif

#ifdef USES_OPTIONS
void OptionInit(void);
int OptionNext(int, char * const *, const char *);
#endif

#ifdef USES_STRINGBUILDER
typedef void * StringBuilder;

/* StringBuilder.c */
StringBuilder SBNew(int);
void SBFree(StringBuilder);
void SBAppend(StringBuilder, char *, int);
void SBAppendChar(StringBuilder, char);
char *SBToString(StringBuilder);
int SBLength(StringBuilder);
void SBSetLength(StringBuilder, int);
void SBReset(StringBuilder);
void SBCopyTo(StringBuilder, StringBuilder);
int SBPosition(StringBuilder);
#endif

/* Utils.c */
void UInit(void (*)(void *, const char *, ...));
void *UAlloc(void *, int);
char *UStrInStr(char *, char *);
char *UStrDup(void *, char *);
char *UTrim(char *);

#ifdef USES_WINUTILS
char *basename(char *);
#define strcasecmp stricmp
#define strncasecmp strnicmp
#endif

#endif

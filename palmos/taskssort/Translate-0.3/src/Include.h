#ifndef INCLUDE_H
#define INCLUDE_H

/* macros */
#define EVER ;;
typedef void * HashTable;
typedef void * StringBuilder;

/* HashTable.c */
HashTable HashNew(int);
void HashFree(HashTable, int);
#define HTPreserveValue 0
#define HTFreeValue 1
int HashAdd(HashTable, const char *, const void *, int, int);
void *HashGet(HashTable, const char *, int *);
int HashDelete(HashTable, const char *);
int HashFindFirst(HashTable, const char **, const void **, int *);
int HashFindNext(HashTable, const char **, const void **, int *);

/* Options.c */
void OptionInit(void);
int OptionNext(int, char * const *, const char *);

/* StringBuilder.c */
StringBuilder SBNew(int);
void SBFree(StringBuilder);
void SBAppend(StringBuilder, unsigned char *, int);
void SBAppendChar(StringBuilder, unsigned char);
unsigned char *SBToString(StringBuilder);
int SBLength(StringBuilder);
void SBSetLength(StringBuilder, int);
void SBReset(StringBuilder);
void SBCopyTo(StringBuilder, StringBuilder);
int SBPosition(StringBuilder);

#ifdef MAIN
	char *tClosing="@@", *tOpening="@@";
	int tClosingLen, tOpeningLen;
	HashTable htBase, htLang, htCli;
#else
	extern char *tClosing, *tOpening;
	extern int tClosingLen, tOpeningLen;
	extern HashTable htBase, htLang, htCli;
#endif

#endif

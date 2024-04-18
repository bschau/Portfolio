#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BSIZE	(128)
#define NONE	(-1)
#define EOS	'\0'
#define NUM	256
#define DIV	257
#define MOD	258
#define ID	259
#define DONE	260
#define STRMAX	(999)
#define SYMMAX	(100)

struct entry {
	char *lexptr;
	int token;
};

/* emitter.c */
void emit(int t, int tval);

/* error.c */
void error(char *m);

/* init.c */
void init(void);

/* lexer.c */
int lexan(void);

/* parser.c */
void parse(void);

/* symbol.c */
int lookup(char s[]);
int insert(char s[], int tok);

#ifdef MAIN
	int lineno, lookahead, tokenval;
	struct entry symtable[SYMMAX];
#else
	extern int lineno, lookahead, tokenval;
	extern struct entry symtable[SYMMAX];
#endif

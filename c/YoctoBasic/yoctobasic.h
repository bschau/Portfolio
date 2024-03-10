#ifndef YOCTOBASIC_H
#define YOCTOBASIC_H

#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define EVER ;;
#define SI_GOSUB (1)
#define SI_FOR (SI_GOSUB + 1)

struct stack_item {
	int si_type;
	int variable;
	int to;
	int step;
};

/* error.c */
void error(char *format, ...);

/* expressions */
int expression(void);

/* keywords.c */
void kw_clear(void);
void kw_cls(void);
void kw_end(void);
void kw_for(void);
void kw_gosub(void);
void kw_goto(void);
void kw_if(void);
void kw_input(void);
void kw_let(void);
void kw_next(void);
void kw_print(void);
void kw_rem(void);
void kw_return(void);

/* main.c */
void reset(void);

/* relop.c */
int relop_pickup(void);
int relop_evaluate(int relop, int lhs, int rhs);

/* run.c */
void run(void);
void statement(void);

/* stack.c */
void stack_reset(void);
void stack_push(void *object);
void stack_pop(void *object);
void stack_peek(void *object);

/* utils.c */
void no_junk_eol(int skip);
void skip_whitespaces(void);
int pickup_variable(int skip);
void match(char c);
void eol(void);
char *locate_label(int label);

#ifdef MAIN
	int variables[26] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	int line_number;
	char *prom, *ip;
#else
	extern int variables[26];
	extern int line_number;
	extern char *prom, *ip;
#endif

#endif

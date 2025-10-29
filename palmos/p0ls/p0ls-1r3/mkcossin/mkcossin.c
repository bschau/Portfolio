/*******************************************************************************
*
*	mkcossin
*
*	Cosinus and Sinus curve creator.
*/
#include <stdio.h>
#include <math.h>

/* macros */
#define SIZE 360

/* globals */
static float pi=3.1415927;
static int indent;

/* protos */
static void tabstart(char *);
static void tabstop(void);
static void tabout(int, float);
static void costab(void);
static void sintab(void);

/*******************************************************************************
*
*	main
*/
int
main(int argc,
     char *argv[])
{
	printf("#include \"p0ls.h\"\n\n");
	costab();
	printf("\n");
	sintab();
	exit(0);
}

/*******************************************************************************
*
*	table functions.
*
*	tabstart:	start table.
*	tabstop:	stop table.
*	tabout:		output to table.
*/
static void
tabstart(char *header)
{
	printf("float %s[%i]={\n", header, SIZE);
	indent=0;
}

static void
tabstop(void)
{
	printf("};\n");
}

static void
tabout(int idx,
       float cv)
{
	if (!indent)
		printf("\t");
	else
		printf(" ");

/*	printf("0x%.8lX", (long)cv);	*/
	printf("%f", cv);
	if (idx<SIZE-1)
		printf(",");

	indent++;
	if (indent>5) {
		printf("\n");
		indent=0;
	}
}

/*******************************************************************************
*
*	costab
*
*	Calculate scaled cosinus table.
*/
static void
costab(void)
{
	int i;
	float cv;

	tabstart("costab");
	for (i=0; i<SIZE; i++) {
		cv=cosf(pi*2*i/(float)SIZE);
		tabout(i, cv);
	}
	tabstop();
}

/*******************************************************************************
*
*	sintab
*
*	Calculate scaled sinus table.
*/
static void
sintab(void)
{
	int i;
	float cv;

	tabstart("sintab");
	for (i=0; i<SIZE; i++) {
		cv=sinf(pi*2*i/(float)SIZE);
		tabout(i, cv);
	}
	tabstop();
}

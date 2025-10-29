/**
 * Main.
 */
#define MAIN
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "Include.h"

/* macros */
#define MAXARRAY 100

/* protos */
static void Usage(int);
static int GetInteger(char *, char *);
static void GetMeta(char *, char **, char **);
static char *GetSolution(char *);
static void GetCount(char *, int, int, int, int);
static char *Nicefy(char *);

/**
 * Write usage information.
 *
 * @param ec Error code.
 */
static void
Usage(int ec)
{
	FILE *f;

	if (ec)
		f=stderr;
	else
		f=stdout;

	fprintf(f, "ptconvert v0.1\n");
	fprintf(f, "Usage: ptconvert [-s] file1 ... fileX\n");
	exit(ec);
}

/**
 * Main.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return 0 if successful, !0 otherwise.
 */
int
main(int argc, char *argv[])
{
	int optIndex=1, dumpSolution=0, w, h, i;
	char *file=NULL, *mem, *puzzle, *author, *solution;
	FILE *fp;
	struct stat sBuf;

	if (argc<2)
		Usage(1);

	if (argv[1][0]=='-' && argv[1][1]=='s') {
		dumpSolution=1;
		optIndex++;
	}

	printf("#\n#\tThese puzzles are from Rachel Benns wonderful site:\n#\n#\t\thttp://www.playtsunami.com\n#\n\n");
	for (; optIndex<argc; optIndex++) {
		file=argv[optIndex];
		if (stat(file, &sBuf))
			XError("Failed to stat '%s'", file);

		mem=XAlloc(sBuf.st_size+2);
		if ((fp=fopen(file, "rb"))==NULL)
			XError("Failed to open '%s'", file);

		if (fread(mem, sBuf.st_size, 1, fp)!=1)
			XError("Failed to read '%s'", file);

		fclose(fp);
		mem=XTrim(mem);

		GetMeta(mem, &solution, &author);
		puzzle=Nicefy(solution);
		free(solution);
		author=Nicefy(author);

		printf("[%s]\nAuthor %s\nSite http://www.playtsunami.com/\nx", puzzle, author);
		w=GetInteger(mem, "gridWidth");
		h=GetInteger(mem, "gridHeight");
		solution=GetSolution(mem);
		GetCount(solution, w, h, w, 1);
		printf("\ny");
		GetCount(solution, h, w, 1, w);
		printf("\n\n");

		if (dumpSolution) {
			fprintf(stderr, "[%s]\nAuthor %s\n", puzzle, author);
			for (; h>0; h--) {
				for (i=0; i<w; i++) {
					if (*solution++!='0')
						fprintf(stderr, "*");
					else
						fprintf(stderr, " ");
				}
	
				fprintf(stderr, "\n");
			}
		}

		free(puzzle);
		free(author);
		free(mem);
	}

	exit(0);
}

/**
 * Find the integer pointed to by this variable.
 *
 * @param haystack Source string.
 * @param var Variable.
 * @return Value.
 */
static int
GetInteger(char *haystack, char *var)
{
	char *p=haystack, *d;
	char save;
	int i;
	
	while ((p=XStrInStr(p, var))) {
		p+=strlen(var);
		for (; *p && isspace((int)*p); p++);
		if (!*p)
			break;

		if (*p=='=') {
			for (p++; *p && isspace((int)*p); p++);
			for (d=p; *p && isdigit((int)*p); p++);
			save=*p;
			*p=0;
			i=atoi(d);
			*p=save;
			return i;
		}
	}

	XError("No such variable: '%s'", var);
	exit(1);
}

/**
 * Get meta info for this puzzle.
 *
 * @param haystack Source string.
 * @param name Where to store puzzlename.
 * @param author Where to store author.
 */
static void
GetMeta(char *mem, char **name, char **author)
{
	char *p=XStrInStr(mem, "<h3>"), *e;

	if (!p)
		XError("No meta data");

	p+=4;
	if ((e=XStrInStr(p, "</h3>"))==NULL)
		XError("No valid meta data");

	*e=0;
	mem=XStrDup(p);
	*e='<';

	*author="Unknown";
	if ((e=XStrInStr(mem, "<br>"))==NULL) {
		*name=XTrim(mem);
		return;
	}

	*e=0;
	*name=XTrim(mem);
	e+=4;
	if ((e=XStrInStr(e, "by "))==NULL)
		return;

	e+=3;
	if ((p=XStrInStr(e, "</")))
		*p=0;

	*author=XTrim(e);
}

/**
 * Find the solution string.
 *
 * @param haystack Source string.
 * @return Solution string.
 */
static char *
GetSolution(char *haystack)
{
	char *p=haystack, *d;
	
	while ((p=XStrInStr(p, "solution0"))) {
		p+=9;
		for (; *p && isspace((int)*p); p++);
		if (!*p)
			break;

		if (*p=='=') {
			for (p++; *p && isspace((int)*p); p++);
			if (*p=='"')
				p++;

			for (d=p; *p && isdigit((int)*p); p++);
			*p=0;
			return XTrim(d);
		}
	}

	XError("No solution0 variable");
	exit(1);
}

/**
 * Get count.
 *
 * @param sol Solution.
 * @param size1 Size of a row/column.
 * @param size2 Size of the other.
 * @param add Add to pointer.
 * @param start Start multiplier.
 */
static void
GetCount(char *sol, int size1, int size2, int add, int start)
{
	int stride, cIdx, idx, t;
	int count[MAXARRAY];
	char pre;

	for (stride=0; stride<size1; stride++) {
		memset(count, 0, sizeof(int)*MAXARRAY);
		cIdx=0;
		idx=stride*start;
		for (t=0; t<size2; t++) {
			if (sol[idx]!='0')
				count[cIdx]++;
			else
				cIdx++;

			idx+=add;
		}

		t=0;
		pre=' ';
		for (idx=0; idx<MAXARRAY; idx++) {
			if (count[idx]) {
				printf("%c%i", pre, count[idx]);
				pre=',';
			}
		}

		if (pre==' ')
			printf(" 0");
	}
}

/**
 * Turn a string into something nice by removing unnecessary white spaces,
 * turning each letter at the start of the word to uppercase and detecting
 * &amp; sequences.
 *
 * @param src Source name.
 * @return New string.
 * @note Caller must free.
 */
static char *
Nicefy(char *src)
{
	int sLen=strlen(src)+1;
	int canWhite=0;
	char prev=' ';
	char *dst, *p;
	char c;

	dst=XAlloc(sLen);
	p=dst;
	for (; *src; src++) {
		if (isspace((int)*src)) {
			if (canWhite) {
				*p++=' ';
				canWhite=0;
			}
		} else {
			if (strncasecmp(src, "&amp;", 5)==0) {
				*p++='&';
				src+=4;
				canWhite=1;
			} else {
				c=tolower((int)*src);
				*p++=c;
				canWhite=1;
			}
		}
	}

	*p=0;

	for (p=dst; *p; p++) {
		if (islower((int)*p) && (prev==' ' || prev=='&'))
			*p=toupper((int)*p);

		prev=*p;
	}

	return XTrim(dst);
}

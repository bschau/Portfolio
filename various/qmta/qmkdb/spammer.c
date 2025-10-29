#include "private.h"

/* globals */
static int lineno;

/* protos */
static int parse(FILE *file);
static void parseline(char *);

/*******************************************************************************
*
*	mkdbspammers
*
*	Recreates the spammers database.
*/
void
mkdbspammer(void)
{
	brain(QSPAMMERDB, QSPAMMER, &parse);
}

/*******************************************************************************
*
*	parse
*
*	Parses spammers file.
*
*	Input:
*		file - spammers file.
*	Output:
*		s: 0.
*		f: !0.
*/
static int
parse(FILE *file)
{
	char line[HUGELINELEN+2];
	char *ptr;
	int len;

	lineno=0;
	while (fgets(line, HUGELINELEN, file)) {
		lineno++;
		len=strlen(line);
		if (line[len-1]=='\n')
			line[len-1]='\x00';

		ptr=line;
		SKIPBLANKS(ptr);
		if (!*ptr || *ptr=='#')	/* eol or comment? */
			continue;

		parseline(ptr);
	}

	if (!feof(file))
		return 1;

	return 0;
}

/*******************************************************************************
*
*	parseline
*
*	Parse line, adding to spammers database if necessary.
*
*	Input:
*		src - where to start from.
*/
static void
parseline(char *src)
{
	char *ssrc=src, *r="550 mail rejected - you have been abusing the system!", *dd;
	int err, len;

	SKIPTOWHITE(ssrc);
	*ssrc++='\x00';
	SKIPBLANKS(ssrc);
	if (*ssrc)
		r=ssrc;

	len=strlen(r);
	dd=(char *)calloc(len+3, 1);
	if (dd) {
		memmove(dd, r, len);
		dd[len]='\r';
		dd[len+1]='\n';
		err=adddblistentry(&dbl, src, dd, len+3);
		if (err)
			fprintf(stderr, "cannot add entry to database\n");

		free(dd);
	} else
		fprintf(stderr, "out of memory\n");
}

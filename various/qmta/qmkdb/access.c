#include "private.h"

/* globals */
static int lineno;

/* protos */
static int parse(FILE *file);
static void parseline(char *);

/*******************************************************************************
*
*	mkdbaccess
*
*	Recreates the access database.
*/
void
mkdbaccess(void)
{
	brain(QACCESSDB, QACCESS, &parse);
}

/*******************************************************************************
*
*	parse
*
*	Parses access file.
*
*	Input:
*		file - access file.
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
*	Parse line, adding to access database if necessary.
*
*	Input:
*		src - where to start from.
*/
static void
parseline(char *src)
{
	char *ssrc=src;
	char atype;
	int err;

	SKIPTOWHITE(ssrc);
	*ssrc++='\x00';
	SKIPBLANKS(ssrc);
	if (*ssrc=='a' || *ssrc=='A')
		atype='+';
	else if (*ssrc=='d' || *ssrc=='D')
		atype='-';
	else {
		fprintf(stderr, "%i: missing accept or deny\n", lineno);
		return;
	}

	err=adddblistentry(&dbl, src, (char *)&atype, sizeof(char));
	if (err)
		fprintf(stderr, "cannot add entry to database\n");
}

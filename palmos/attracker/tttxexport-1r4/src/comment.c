/*
**	comment.c
**
**	Code to handle comments.
*/
#include "include.h"

/* globals */
static int comments, commentLen, commentPrefix;

/*
**	CommentParse
**
**	Parse and set new comments specifier.
**
**	 -> cS		Comment string.
*/
void
CommentParse(char *cS)
{
	char *s;
	int sLen;

	comments=1;

	for (s=cS; *s && *s!=','; s++);
	if (*s==',')
		*s++='\x00';

	commentLen=atoi(cS);

	if (!*s)
		return;

	sLen=(int)strlen(s);
	if (sLen==1) {
		commentPrefix=(int)*s;
		return;
	}

	if (*s=='0' && *(s+1)=='x') {
		if (sLen==4) {
			sscanf(s, "%X", &commentPrefix);
			return;
		}

		fprintf(stderr, "(SetComments) Hex delimiter not in form 0xXX\n");
		exit(1);	
	}

	fprintf(stderr, "(SetComments) Invalid comments prefix %s\n", s);
	exit(1);
}

/*
**	CommentHeader
**
**	Emit comment header.
**
**	 -> f		File.
**	 -> prc		PRC.
*/
void
CommentHeader(FILE *f, void *prc)
{
	if (comments)
		fprintf(f, "%c V1 %i records%s", (char)commentPrefix, PrcNumRecords(prc), eol);
}

/*
**	CommentComment
**
**	Emit a normal comment line.
**
**	 -> f		File.
**	 -> prc		PRC.
**	 -> delim	Delimiter.
*/
void
CommentComment(FILE *f, void *prc, char *delim)
{
	char *d="", *s;
	int i;

	if (comments) {
		if (!(PrcGetRecIdx(prc)%commentLen)) {
			fprintf(f, "%c ", (int)commentPrefix);
			for (i=0; (s=FieldGetName(i))!=NULL; i++) {
				fprintf(f, "%s%s", d, s);
				d=delim;
			}
			fprintf(f, "%s", eol);
		}
	}
}

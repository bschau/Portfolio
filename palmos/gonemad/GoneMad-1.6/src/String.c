/*
 * String.c
 */
#include "Include.h"

/*
 * StringGetExact
 *
 * Find exact start and length of line. Leading and trailing whitespaces are
 * omitted.
 *
 * <-> src		Source string.
 *
 * Returns length of string.
 */
UInt16
StringGetExact(Char **src)
{
	UInt16 len=0;
	Char *s;

	if (src==NULL)
		return 0;

	if ((s=*src)==NULL) {
		*src=NULL;
		return 0;
	}

	for (; *s && *s<=' '; s++);

	if (!*s) {
		*src=NULL;
		return 0;
	}
	*src=s;

	for (; *s && *s!='\n'; s++)
		len++;

	for (s--; s>=*src && *s<=' '; s--)
		len--;

	return len;
}

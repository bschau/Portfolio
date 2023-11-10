/*
 * String.c
 */
#include "Include.h"

/*
 * StringSkipLeading
 *
 * Skips leading non-printable characters.
 *
 *  -> s		String.
 *
 * Returns position of first printable character or NULL if no more.
 */
Char *
StringSkipLeading(Char *s)
{
	for (; *s; s++) {
		if (TxtCharIsGraph(*s)==true)
			return s;
	}

	return NULL;
}

/*
 * StringWhiteSpaceConvert
 *
 * Convert all whitespace to a single space.
 *
 *  -> dst		Where to store destination string.
 *  -> src		Source.
 *  -> len		Length of string.
 */
void
StringWhiteSpaceConvert(Char *dst, Char *src, UInt16 len)
{
	while (len>0) {
		if (TxtGlueCharIsSpace(*src))
			*dst++=' ';
		else
			*dst++=*src;

		src++;
		len--;
	}
}

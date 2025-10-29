/*
 * String.c
 */
#include "Include.h"

/*
 * StringPurify
 *
 * Remove redundant characters from a string. Redundant characters are runs
 * of whitespaces. Characters < ' ' are substituted with ' '.
 *
 *  -> src		Source string to purify.
 *
 * Returns purified string or NULL (empty string).
 *
 * Note. Remember to free the string after use.
 */
Char *
StringPurify(Char *src)
{
	MemHandle dstH;
	Char *dst, *p;
	UInt16 len;
	Boolean spaceOk;

	// Remove leading whites
	for (; *src && *src<=' '; src++);
	if (!*src)
		return NULL;

	len=StrLen(src);

	dstH=MemHandleNew(len+1);
	ErrFatalDisplayIf(dstH==NULL, "(StringPurify) Out of memory.");
	dst=MemHandleLock(dstH);
	MemMove(dst, src, len);
	dst[len]='\x00';

	// Remove trailing whites
	p=dst+len;
	while (p>dst) {
		p--;
		if (*p>' ')
			break;
		else
			*p='\x00';
	}

	p=dst;
	src=dst;
	spaceOk=false;
	while (*src) {
		if (*src>' ') {
			*p++=*src;
			spaceOk=true;
		} else {
			if (spaceOk==true) {
				*p++=' ';
				spaceOk=false;
			} 
		}
		src++;
	}

	*p='\x00';
	return dst;
}

/*
 * StringStrip
 *
 * Remove all whitespaces from string.
 *
 * <-  dst		Where to store string.
 *  -> src		Source string.
 *
 * Note: operation can be done in place.
 */
void
StringStrip(Char *dst, Char *src)
{
	for (;;) {
		for (; *src && *src<=' '; src++);
		if (!*src)
			break;

		for (; *src && *src>' '; src++)
			*dst++=*src;
	}

	*dst='\x00';
}

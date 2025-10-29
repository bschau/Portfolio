#include "loginman.h"

/*******************************************************************************
*
*	urldecode
*
*	Decode url (in place).
*/
UInt16
urldecode(UInt8 *url)
{
	UInt8 *src=url, *dst=url;
	UInt16 len;

	if (!url)
		return 0;

	len=StrLen(url);
	while (len--) {
		if (*src=='+')
			*dst=' ';
		else if (*src=='%' && len>=2 && isxdigit(*(src+1)) && isxdigit(*(src+2))) {
			*dst=htoi(src+1);
			src+=2;
			len-=2;
		} else
			*dst=*src;

		src++;
		dst++;
	}

	*dst='\x00';
	return dst-url;
}

/*******************************************************************************
*
*	urlencodehandle
*
*	Encodes an url in place - replaces old handle.
*/
UInt16
urlencodehandle(MemHandle *ohp)
{
	UInt8 hex[16]="0123456789ABCDEF";
	MemHandle oh=*ohp, nh;
	UInt16 nl=0, ol, i;
	UInt8 *op, *np;

	if (oh) {
		op=MemHandleLock(oh);
		ol=StrLen(op);

		nh=MemHandleNew((ol*3)+1);
		ErrFatalDisplayIf(nh==NULL, "(urlencodehandle) Out of memory");

		np=MemHandleLock(nh);

		for (i=0; i<ol; i++) {
			if (isalnum(*op))
				*np=*op;
			else {
				switch (*op) {
				case ' ':
					*np='+';
					break;
				case '_':	/* FALL-THRU */
				case '-':	/* FALL-THRU */
				case '.':
					*np=*op;
					break;
				default:
					*np='%';
					*(np+1)=hex[(*op>>4)&0xf];
					*(np+2)=hex[(*op)&0xf];
					np+=2;
					nl+=2;
				}
			}

			np++;
			op++;
			nl++;
		}

		MemHandleUnlock(nh);
		MemHandleUnlock(oh);
		MemHandleFree(*ohp);
		*ohp=nh;
	}

	return nl;
}

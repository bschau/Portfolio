#include <stdlib.h>
#include <string.h>
#include "occmake.h"

/*
**	XConCat
**
**	Concatenates two strings.
**
**	 -> str1	First string.
**	 -> str2	Second string.
**	<- string or NULL.
*/
char *
XConCat(char *str1, char *str2)
{
	int sl1, sl2;
	char *p;

	sl1=strlen(str1);
	sl2=strlen(str2);

	if ((p=(char *)malloc(sl1+sl2+1))) {
		memmove(p, str1, sl1);
		memmove(p+sl1, str2, sl2);
		*(p+sl1+sl2)='\x00';
	}

	return p;
}

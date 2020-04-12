/*
**	dynbuf.c
**
**	Dynmaic buffer.
*/
#include "include.h"

/* protos */
static void DynBufGrow(void);

/* globals */
static char *dynBuf;
static int dynBufGrow, dynBufSize, dynBufLen;

/*
**	DynBufInit
**
**	Initialize dynamic buffer.
**
**	 -> grow	Size to grow buffer pr. grow request.
*/
void
DynBufInit(int grow)
{
	dynBuf=NULL;
	dynBufSize=0;
	dynBufLen=0;
	dynBufGrow=grow;
}

/*
**	DynBufPtr
**
**	Return start of dyn buf (data).
*/
char *
DynBufPtr(void)
{
	return dynBuf;
}

/*
**	DynBufLen
**
**	Return size of data in dyn buf.
*/
int
DynBufLen(void)
{
	return dynBufLen;
}

/*
**	DynBufAdd
**
**	Add src data of len to dyn buf.
**
**	 -> src		Source data.
**	 -> len		Length of source data.
*/
void
DynBufAdd(char *src, int len)
{
	while ((dynBufLen+len)>dynBufSize)
		DynBufGrow();

	memmove(dynBuf+dynBufLen, src, len);
	dynBufLen+=len;
}

/*
**	DynBufReset
**
**	Reset dyn buf.
*/
void
DynBufReset(void)
{
	dynBufLen=0;
}

/*
**	DynBufGrow
**
**	Grow dynamic buffer.
*/
static void
DynBufGrow(void)
{
	int s=dynBufSize+dynBufGrow;
	char *d;
	
	if ((d=calloc(s, 1))==NULL) {
		XLog(LOG_ERR, "(DynBufGrow) Out of memory");
		exit(1);
	}

	if (dynBuf) {
		if (dynBufLen>0)
			memmove(d, dynBuf, dynBufLen);

		free(dynBuf);
	}

	dynBuf=d;
	dynBufSize=s;
}

/*
**	csvbuffer.c
**
**	Code to drive the minimal CSV buffer.
*/
#include "include.h"

/* globals */
static int csvBufferSize=1024;
static char *csvBuffer;
static FILE *csvFh;
static int csvPtr;

/*
**	CsvBufferSetSize
**
**	Parse and set new CSV buffer size.
**
**	 -> str		String to parse.
*/
void
CsvBufferSetSize(char *str)
{
	int s;

	if ((s=atoi(str))<1) {
		fprintf(stderr, "(CsvBufferSetSize) Invalid size (%i)\n", s);
		exit(1);
	}

	csvBufferSize=s;
}

/*
**	CsvBufferAlloc
**
**	Allocate CSV buffer.
**
**	 -> outH	Handle to flush bytes to.
*/
void
CsvBufferAlloc(FILE *outH)
{
	if ((csvBuffer=malloc(csvBufferSize))==NULL) {
		fprintf(stderr, "(CsvBufferAlloc) Failed to allocate %i bytes for CSV buffer\n", csvBufferSize);
		exit(1);
	}

	csvFh=outH;
}

/*
**	CsvReset
**
**	Reset CSV buffer pointer.
*/
void
CsvReset(void)
{
	csvPtr=0;
}

/*
**	CsvFlush
**
**	Flush csvBuffer.
*/
void
CsvFlush(void)
{
	if (fwrite(csvBuffer, csvPtr, 1, csvFh)==1) {
		CsvReset();
		return;
	}

	fprintf(stderr, "(CsvFlush) Cannot flush CSV buffer\n");
	exit(1);
}

/*
**	CsvAdd
**
**	Add bytes to buffer.
**
**	 -> src		Source bytes.
**	 -> srcLen	Source length.
*/
void
CsvAdd(char *src, int srcLen)
{
	if ((srcLen+csvPtr)>csvBufferSize)
		CsvFlush();

	if ((srcLen+csvPtr)>csvBufferSize) {
		if (fwrite(src, srcLen, 1, csvFh)==1)
			return;

		fprintf(stderr, "(CsvAdd) Cannot add/write to buffer/file\n");
		exit(1);
	}

	memmove(csvBuffer+csvPtr, src, srcLen);
	csvPtr+=srcLen;
}

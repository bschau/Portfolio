#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "occmake.h"

#define PALMBITMAPLEN 16
#define BMPBITSSTARTOFFSET 10
#define BMPWIDTHOFFSET 18
#define BMPHEIGHTOFFSET 22

/* protos */
static unsigned long GetLong(unsigned char *);
static void PutShort(unsigned char *, unsigned short);

/*
**	GetLong
**
**	Get a long word value.
**
**	 -> lwB		Long word base.
**	<-  long word.
*/
static unsigned long
GetLong(unsigned char *lwB)
{
	unsigned long l;

	l=(*(lwB+3)<<24);
	l|=(*(lwB+2)<<16);
	l|=(*(lwB+1)<<8);
	l|=*lwB;

	return l;
}

/*
**	PutShort
**
**	Put a short value to pointer.
**
**	 -> p		Destination pointer.
**	 -> v		Value.
*/
static void
PutShort(unsigned char *p, unsigned short v)
{
	*p++=(v>>8)&0xff;
	*p=v&0xff;
}

/*
**	BitmapLoad
**
**	Fetches a bitmap from file or memory.
**
**	 -> bmpName	Name of disk bitmap file.
**	<-  bmpData	Merged bitmap.
**	<-  bmpLen	Length.
**	 -> defBmp	Default (memory) bitmap file.
**	 -> defLen	Length.
*/
void
BitmapLoad(char *bmpName, unsigned char **bmpData, unsigned long *bmpLen, unsigned char *defBmp, unsigned long defLen)
{
	char *dst=NULL;
	FILE *fh=NULL;
	long fLen, oLen;
	BitMap bms;

	if (bmpName) {
		if ((fh=fopen(bmpName, "rb"))==NULL) {
			fprintf(stderr, "cannot load bitmap '%s'\n", bmpName);
			exit(1);
		}

		if (fseek(fh, 0, SEEK_END)) {
			fprintf(stderr, "cannot seek in bitmap file '%s'\n", bmpName);
			exit(1);
		}

		fLen=ftell(fh);
		rewind(fh);

		if ((dst=(char *)malloc(fLen))==NULL) {
			fclose(fh);
			fprintf(stderr, "out of memory\n");
			exit(1);
		}

		oLen=fLen;
		fLen=fread(dst, 1, fLen, fh);
		fclose(fh);
		if (fLen!=oLen) {
			fprintf(stderr, "cannot load %s\n", bmpName);
			exit(1);
		}
	} else {
		if ((dst=(char *)malloc(defLen))==NULL) {
			fprintf(stderr, "cannot load in-memory bitmap\n");
			exit(1);
		}

		memmove(dst, defBmp, defLen);
	}

	BitmapParse(bmpName, dst, &bms);
	BitmapMerge(&bms, bmpData, bmpLen);
}

/*
**	BitmapParse
**
**	Parses bitmap information into structure.
**
**	 -> name	Name of bitmap file.
**	 -> bmP		Bitmap pointer.
**	<-> bmS		Bitmap struct.
*/
void
BitmapParse(char *name, unsigned char *bmP, BitMap *bmS)
{
	if (!(*bmP=='B' && *(bmP+1)=='M')) {
		fprintf(stderr, "%s is not a usable bitmap file\n", (name ? name : "builtin"));
		exit(1);
	}

	bmS->width=(unsigned short)GetLong(bmP+BMPWIDTHOFFSET);
	bmS->height=(unsigned short)GetLong(bmP+BMPHEIGHTOFFSET);
	bmS->rawBytes=(((bmS->width)+31)/32)*4;

	bmS->bits=bmP+GetLong(bmP+BMPBITSSTARTOFFSET);
}

/*
**	BitmapMerge
**
**	Merge bitmap structures.
**
**	 -> bmS		Bitmap structure.
**	<-  newP	New bitmap pointer.
**	<-  newL	New bitmap length.
*/
void
BitmapMerge(BitMap *bmS, unsigned char **newP, unsigned long *newL)
{
	int len=bmS->height*bmS->rawBytes, h;
	unsigned char *p, *tryl, *src;

	if ((p=(unsigned char *)calloc(len+PALMBITMAPLEN, 1))==NULL) {
		fprintf(stderr, "out of memory");
		exit(1);
	}

	PutShort(p, bmS->width);
	PutShort(p+2, bmS->height);
	PutShort(p+4, bmS->rawBytes);
	*(p+8)=1;		/* pixelLine */
	*(p+9)=1;		/* version */

	tryl=p+PALMBITMAPLEN;
	src=bmS->bits+(bmS->height*bmS->rawBytes)-bmS->rawBytes;
	for (h=0; h<bmS->height; h++) {
		memmove(tryl, src, bmS->rawBytes);
		tryl+=bmS->rawBytes;
		src-=bmS->rawBytes;
	}

	tryl=p+PALMBITMAPLEN;
	for (h=0; h<len; h++) {
		*tryl=~*tryl;
		tryl++;
	}

	*newP=p;
	*newL=len+PALMBITMAPLEN;
}

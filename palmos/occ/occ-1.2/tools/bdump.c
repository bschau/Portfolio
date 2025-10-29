#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* macros */
enum { FormatPalm, FormatOther };
#define PALMBITMAPLEN 16
#define BMPBITSSTARTOFFSET 10
#define BMPWIDTHOFFSET 18
#define BMPHEIGHTOFFSET 22

/* structs */
typedef struct {
	unsigned short width;
	unsigned short height;
	unsigned short rawBytes;
	unsigned char *bits;
} BitMap;

/* protos */
static unsigned long GetLong(unsigned char *);
static void PutShort(unsigned char *, unsigned short);
static int BitmapLoad(FILE *, unsigned char **, unsigned long *);
static int BitmapParse(unsigned char *, BitMap *);
static int BitmapMerge(BitMap *, unsigned char **, unsigned long *);
static int Dump(char *, FILE *, FILE *, int);

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
**	 -> ifh		In file Handle.
**	<-  bmpData	Merged bitmap.
**	<-  bmpLen	Length.
**	 -> defBmp	Default (memory) bitmap file.
**	 -> defLen	Length.
**
**	Returns 0 if successful, !0 otherwise.
*/
static int
BitmapLoad(FILE *ifh, unsigned char **bmpData, unsigned long *bmpLen)
{
	char *dst=NULL;
	long fLen, oLen;
	BitMap bms;

	if (fseek(ifh, 0, SEEK_END)) {
		fprintf(stderr, "cannot seek in bitmap file\n");
		return -1;
	}

	fLen=ftell(ifh);
	rewind(ifh);

	if ((dst=(char *)malloc(fLen))==NULL) {
		fprintf(stderr, "out of memory\n");
		return -1;
	}

	oLen=fLen;
	fLen=fread(dst, 1, fLen, ifh);
	if (fLen!=oLen) {
		fprintf(stderr, "cannot load bitmap\n");
		return -1;
	}

	if (BitmapParse(dst, &bms)==0) {
		if (BitmapMerge(&bms, bmpData, bmpLen)==0)
			return 0;
	}

	return -1;
}

/*
**	BitmapParse
**
**	Parses bitmap information into structure.
**
**	 -> bmP		Bitmap pointer.
**	<-> bmS		Bitmap struct.
**
**	Returns 0 if successful, !0 otherwise.
*/
static int
BitmapParse(unsigned char *bmP, BitMap *bmS)
{
	if (!(*bmP=='B' && *(bmP+1)=='M')) {
		fprintf(stderr, "The supplied file is not a usable bitmap file\n");
		return -1;
	}

	bmS->width=(unsigned short)GetLong(bmP+BMPWIDTHOFFSET);
	bmS->height=(unsigned short)GetLong(bmP+BMPHEIGHTOFFSET);
	bmS->rawBytes=(((bmS->width)+31)/32)*4;

	bmS->bits=bmP+GetLong(bmP+BMPBITSSTARTOFFSET);

	return 0;
}

/*
**	BitmapMerge
**
**	Merge bitmap structures.
**
**	 -> bmS		Bitmap structure.
**	<-  newP	New bitmap pointer.
**	<-  newL	New bitmap length.
**
**	Returns 0 if successful, !0 otherwise.
*/
static int
BitmapMerge(BitMap *bmS, unsigned char **newP, unsigned long *newL)
{
	int len=bmS->height*bmS->rawBytes, h;
	unsigned char *p, *tryl, *src;

	if ((p=(unsigned char *)calloc(len+PALMBITMAPLEN, 1))==NULL) {
		fprintf(stderr, "out of memory");
		return -1;
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

	return 0;
}

/*
**	main
*/
int
main(int argc,
     char *argv[])
{
	FILE *ifh=NULL, *ofh=NULL;
	int format;

	if (argc!=5) {
		fprintf(stderr, "Usage: bdump format name in-file c-file\n");
		fprintf(stderr, "\nFormat can be either Palm or Other\n");
		exit(1);
	}

	if (strcasecmp(argv[1], "palm")==0)
		format=FormatPalm;
	else
		format=FormatOther;

	ifh=fopen(argv[3], "rb");
	ofh=fopen(argv[4], "wb+");
	if (ifh && ofh) {
		if (Dump(argv[2], ifh, ofh, format))
			fprintf(stderr, "failed ...\n");
	} else
		fprintf(stderr, "cannot open files\n");

	if (ofh)
		fclose(ofh);

	if (ifh)
		fclose(ifh);

	exit(0);
}

/*
**	Dump
**
**	Dumps from in to out converting picture as we go.
**
**	 -> name	Name of variable.
**	 -> ifh		Input file handle (prc file).
**	 -> ofh		Output file handle (c file).
**	 -> format	Format of output (FormatPalm or FormatOther).
**	<-  error	0 - succes, 1 - failure.
*/
static int
Dump(char *name, FILE *ifh, FILE *ofh, int format)
{
	unsigned char *data=NULL;
	unsigned long len=0, idx=0;

	if (BitmapLoad(ifh, &data, &len)!=0)
		return 1;

	if (format==FormatPalm) {
		fprintf(ofh, "UInt32 %sLen=%li;\n\n", name, len);
		fprintf(ofh, "UInt8 %sArr[%li]={\n\t", name, len);
	} else {
		fprintf(ofh, "unsigned long %sLen=%li;\n\n", name, len);
		fprintf(ofh, "unsigned char %sArr[%li]={\n\t", name, len);
	}

	for (; len>0; len--) {
		fprintf(ofh, "0x%02X", (unsigned char)*data);

		if (len>1)
			fprintf(ofh, ", ");

		if ((idx%11)==10)
			fprintf(ofh, "\n\t");

		data++;
		idx++;
	}
	fprintf(ofh, "\n};\n");

	return 0;
}

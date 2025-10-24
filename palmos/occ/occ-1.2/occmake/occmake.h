#ifndef OCCMAKE_H
#define OCCMAKE_H

#include "otresources.h"
#include "occinclude.h"

/* macros */
#define MAKE32(a,b,c,d) ((((unsigned long)a)<<24)|(((unsigned long)b)<<16)|(((unsigned long)c)<<8)|((unsigned long)d))
#define CREATORID MAKE32('O','c','c','X')

/* structs */
typedef struct {
	unsigned short width;
	unsigned short height;
	unsigned short rawBytes;
	unsigned char *bits;
} BitMap;

/* bitmap.c */
void BitmapLoad(char *, unsigned char **, unsigned long *, unsigned char *, unsigned long);
void BitmapParse(char *, unsigned char *, BitMap *);
void BitmapMerge(BitMap *, unsigned char **, unsigned long *);

/* utils.c */
char *XConCat(char *, char *);

/* prc.c */
void BuildPrc(char *, char *, int);

#ifdef WIN32
extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;
int getopt(int argc, char* const *argv, const char *optstr);
#endif

#ifdef MAIN
	int prcOk;
	unsigned char *url, *fileName, *largeBitmap, *smallBitmap;
	unsigned char *inMemPrc;
	unsigned char *mergedLargeBitmap, *mergedSmallBitmap;
	unsigned long mergedLargeBitmapLen, mergedSmallBitmapLen;
#else
	extern int prcOk;
	extern unsigned char *url, *fileName, *largeBitmap, *smallBitmap;
	extern unsigned char *inMemPrc;
	extern unsigned char *mergedLargeBitmap, *mergedSmallBitmap;
	extern unsigned long mergedLargeBitmapLen, mergedSmallBitmapLen;
#endif

/* all */
extern int errno;

#endif

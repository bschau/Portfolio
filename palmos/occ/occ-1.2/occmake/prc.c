#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>	/* Cygwin needs this for malloc */
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "occmake.h"

/* macros */
typedef unsigned char UInt8;	/* sigh, Palm defines these ... */
typedef unsigned short UInt16;
typedef unsigned long UInt32;
typedef unsigned long LocalID;
#define dmDBNameLength 32
#define PADBYTES 2

/* structs */
typedef struct {
	LocalID nextRecordListID;
	UInt16 numRecords;
} RecordListType;

typedef struct {
	UInt32 type;
	UInt16 id;
	LocalID localChunkID;
} RsrcEntryType;
#define RSRCENTRYLEN 10

typedef struct {
	UInt8 name[dmDBNameLength];
	UInt16 attributes;
	UInt16 version;
	UInt32 creationDate;
	UInt32 modificationDate;
	UInt32 lastBackupDate;
	UInt32 modificationNumber;
	LocalID appInfoID;
	LocalID sortInfoID;
	UInt32 type;
	UInt32 creator;
	UInt32 uniqueIDSeed;
	RecordListType recordList;
} DatabaseHdrType;
#define DATABASEHDRLEN 78

/* protos */
static void Push2Prc(unsigned int, unsigned short, unsigned char *, unsigned long);
static void StartPrc(char *, int);

/* globals */
static unsigned char *occVersion=OCCVERSION;
static unsigned int rsrcOffSet=DATABASEHDRLEN, dataOffSet, urlLen, occVersionLen, numRes;
extern unsigned long code0000Len, code0001Len, data0000Len, pref0000Len;
extern unsigned long rloc0000Len, largeLen, smallLen;
extern unsigned char code0000Arr[], code0001Arr[], data0000Arr[];
extern unsigned char pref0000Arr[], rloc0000Arr[], largeArr[], smallArr[];

/*
**	Push2Prc
**
**	Push a RecordList + data to inMemPrc.
**
**	 -> seg		Segment.
**	 -> id		Identifier.
**	 -> src		Source data.
**	 -> len		Source length.
*/
static void
Push2Prc(unsigned int seg, unsigned short id, unsigned char *src, unsigned long len)
{
	unsigned char *p=inMemPrc+rsrcOffSet;

	*p++=(seg>>24)&0xff;
	*p++=(seg>>16)&0xff;
	*p++=(seg>>8)&0xff;
	*p++=seg&0xff;

	*p++=(id>>8)&0xff;
	*p++=id&0xff;

	*p++=(dataOffSet>>24)&0xff;
	*p++=(dataOffSet>>16)&0xff;
	*p++=(dataOffSet>>8)&0xff;
	*p=dataOffSet&0xff;

	memmove(inMemPrc+dataOffSet, src, len);

	dataOffSet+=len;
	rsrcOffSet+=RSRCENTRYLEN;
}

/*
**	StartPrc
**
**	Create inMemPrc. Populate header.
**
**	 -> dbName	Database name.
**	 -> makeExe	OCC is executable.
*/
static void
StartPrc(char *dbName, int makeExe)
{
	int prcLen=mergedLargeBitmapLen+mergedSmallBitmapLen;
	int now=time(NULL)+2082844800L;
	DatabaseHdrType *dbHeader;

	if (makeExe)
		prcLen+=code0000Len+code0001Len+data0000Len+pref0000Len+rloc0000Len;

	prcLen+=(occVersionLen+1);
	prcLen+=(urlLen+1);
	prcLen+=((numRes*RSRCENTRYLEN)+DATABASEHDRLEN);
	prcLen+=(PADBYTES);

	if ((inMemPrc=(unsigned char *)calloc(prcLen, 1))==NULL) {
		fprintf(stderr, "out of memory");
		exit(1);
	}

	dbHeader=(DatabaseHdrType *)inMemPrc;
	strncpy(dbHeader->name, dbName, dmDBNameLength-1);
	if (makeExe) {
		/* AttrBackup | AttrResDB */
		dbHeader->attributes=htons(0x0009);
		dbHeader->type=htonl(MAKE32('a','p','p','l'));
	} else {
		/* LaunchableData | AttrBackup | AttrResDB */
		dbHeader->attributes=htons(0x0209);
		dbHeader->type=htonl(MAKE32('o','c','c',' '));
	}
	dbHeader->creator=htonl(CREATORID);
	dbHeader->creationDate=htonl(now);
	dbHeader->modificationDate=htonl(now);
	dbHeader->recordList.numRecords=htons(numRes);
}

/*
**	BuildPrc
**
**	Builds prc file
**
**	 -> prcName	Name of prc.
**	 -> nameInPrc	Name of prc in database.
**	 -> makeExe	Make executable?
*/
void
BuildPrc(char *prcName, char *nameInPrc, int makeExe)
{
	char *p;
	int len, dLen;
	FILE *fh;

	occVersionLen=strlen(occVersion);
	urlLen=strlen(url);

	if (makeExe)
		numRes=9;	/* code0+1 data pref rloc taib0+1 tver tstr */
	else
		numRes=4;	/* taib0+1 tver tstr */

	dataOffSet=DATABASEHDRLEN+(numRes*RSRCENTRYLEN)+PADBYTES;
	if (!nameInPrc)
		nameInPrc=prcName;

	StartPrc(nameInPrc, makeExe);

	if (makeExe) {
		Push2Prc(MAKE32('c','o','d','e'), 0, code0000Arr, code0000Len);
		Push2Prc(MAKE32('c','o','d','e'), 1, code0001Arr, code0001Len);
		Push2Prc(MAKE32('d','a','t','a'), 0, data0000Arr, data0000Len);
		Push2Prc(MAKE32('p','r','e','f'), 0, pref0000Arr, pref0000Len);
		Push2Prc(MAKE32('r','l','o','c'), 0, rloc0000Arr, rloc0000Len);
	}

	Push2Prc(MAKE32('t','A','I','B'), 1000, mergedLargeBitmap, mergedLargeBitmapLen);
	Push2Prc(MAKE32('t','A','I','B'), 1001, mergedSmallBitmap, mergedSmallBitmapLen);
	Push2Prc(MAKE32('t','S','T','R'), URLID, url, urlLen);
	dataOffSet++;
	Push2Prc(MAKE32('t','v','e','r'), 1, occVersion, occVersionLen);
	dataOffSet++;

	if ((fh=fopen(fileName, "wb+"))==NULL) {
		fprintf(stderr, "cannot create prc file\n");
		exit(1);
	}

	p=inMemPrc;
	dLen=dataOffSet;
	while (dLen>0) {
		if ((len=fwrite(p, 1, dLen, fh))==0) {
			fprintf(stderr, "cannot write prc\n");
			fclose(fh);
			exit(1);
		}

		dLen-=len;
		p+=len;
	}
	
	fclose(fh);
}

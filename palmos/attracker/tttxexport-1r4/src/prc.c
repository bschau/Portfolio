/*
**	prc.c
**
**	Code to read from a prc file.
*/
#include "include.h"

/* these are some of the types that is used in Palm OS */
#pragma pack(2)
typedef unsigned long UInt32;
typedef unsigned char UInt8;
typedef char Char;
typedef short Int16;
typedef unsigned short UInt16;
typedef unsigned long LocalID;

/* macros */
#define dmRecNumCategories 16
#define dmCategoryLength 16
#define dmDBNameLength 32
#define PADBYTES 2
#define CRID ((UInt32)(('T'<<24)|('T'<<16)|('T'<<8)|'X'))
#define TYPE ((UInt32)(('A'<<24)|('R'<<16)|('C'<<8)|'H'))

/* structs */
typedef struct {
	LocalID nextRecordListID;
        UInt16 numRecords;
} RecordListType;

typedef struct {
        LocalID localChunkID;
	UInt8 attributes;
	UInt8 uniqueID[3];
} RecordEntryType;

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
} DatabaseHdrType;

typedef struct {
	FILE *f;
	DatabaseHdrType hdr;
	int numRecs;
	int recordList;
	int recIdx;
	int recLen;
	int dbSize;
	char *catName;
} PRC;

/* protos */
static void GetBytes(FILE *, int, void *, int);
static void *GetRecord(PRC *);

/* globals */
static char categories[dmRecNumCategories][dmCategoryLength];

/*
**	GetBytes
**
**	Get X bytes from file into dst.
**
**	 -> f		File.
**	 -> pos		Position to get bytes from.
**	 -> dst		Where to store bytes.
**	 -> cnt		Number of bytes to get.
*/
static void
GetBytes(FILE *f, int pos, void *dst, int cnt)
{
	if (fseek(f, pos, SEEK_SET)<0) {
		fprintf(stderr, "(GetBytes) Cannot seek to position %i in file (%i)\n", pos, errno);
		exit(1);
	}

	if (fread(dst, cnt, 1, f)==1)
		return;

	fprintf(stderr, "(GetBytes) Cannot read %i bytes from file (%i)\n", cnt, errno);
	exit(1);
}

/*
**	GetRecord
**
**	Read record at index from PRC.
**
**	 -> p		PRC.
**
**	Returns record or NULL (no more records).
*/
static void *
GetRecord(PRC *p)
{
	RecordEntryType rte1, rte2;
	int offSet, len;
	void *v;

	if (p->recIdx==p->numRecs)
		return NULL;

	if (p->recIdx<(p->numRecs-1)) {
		offSet=p->recordList+(p->recIdx*sizeof(RecordEntryType));
		GetBytes(p->f, offSet, &rte1, sizeof(RecordEntryType));
		offSet+=sizeof(RecordEntryType);
		GetBytes(p->f, offSet, &rte2, sizeof(RecordEntryType));
		offSet=ntohl(rte1.localChunkID);
		len=ntohl(rte2.localChunkID)-offSet;
	} else {
		GetBytes(p->f, p->recordList+(p->recIdx*sizeof(RecordEntryType)), &rte1, sizeof(RecordEntryType));
		offSet=ntohl(rte1.localChunkID);
		len=p->dbSize-offSet;
	}

	if ((v=malloc(len))==NULL) {
		fprintf(stderr, "(GetRecord) Cannot allocate %i bytes\n", len);
		exit(1);
	}

	GetBytes(p->f, offSet, v, len);
	p->catName=&categories[rte1.attributes&(dmRecNumCategories-1)][0];
	p->recLen=len;
	return v;
}

/*
**	PrcOpen
**
**	Open a prc file.  Allocate Prc structure.
**
**	 -> n		PRC Name.
**
**	Returns handle or NULL.
*/
void *
PrcOpen(char *n)
{
	PRC *p=calloc(1, sizeof(PRC));
	RecordListType rlt;

	if (!p) {
		fprintf(stderr, "(PrcOpen) Cannot allocate PRC structure\n");
		exit(1);
	}

#ifdef WIN32
	if ((p->f=fopen(n, "rb"))==NULL) {
#else
	if ((p->f=fopen(n, "r"))==NULL) {
#endif
		fprintf(stderr, "(PrcOpen) Cannot open %s (%i)\n", n, errno);
		exit(1);
	}

	/* Read header, check if this is a valid archive */
	GetBytes(p->f, 0, &p->hdr, sizeof(DatabaseHdrType));

	if (((UInt32)(ntohl(p->hdr.creator))!=CRID) || (((UInt32)ntohl(p->hdr.type))!=TYPE)) {
		fprintf(stderr, "(PrcOpen) %s: not a atTracker archive\n", n);
		exit(1);
	}

	/* Extract categories */
	GetBytes(p->f, ntohl(p->hdr.appInfoID)+2, categories, (dmRecNumCategories*dmCategoryLength));

	/* Housekeeping */
	p->recordList=sizeof(DatabaseHdrType)+sizeof(RecordListType);
	GetBytes(p->f, sizeof(DatabaseHdrType), &rlt, sizeof(RecordListType));
	p->numRecs=(int)(ntohs(rlt.numRecords));

	if (fseek(p->f, 0, SEEK_END)<0) {
		fprintf(stderr, "(PrcOpen) Cannot seek in archive %s (%i)\n", n, errno);
		exit(1);
	}

	p->dbSize=(int)ftell(p->f);
	rewind(p->f);
	
	return p;
}

/*
**	PrcNumRecords
**
**	Return number of records.
**
**	 -> prc		PRC.
*/
int
PrcNumRecords(void *prc)
{
	return ((PRC *)prc)->numRecs;
}

/*
**	PrcGetFirstRecord
**
**	Get first record from file.
**
**	 -> prc		PRC.
**
**	Returns record.
*/
void *
PrcGetFirstRecord(void *prc)
{
	((PRC *)prc)->recIdx=0;

	return GetRecord((PRC *)prc);
}

/*
**	PrcGetNextRecord
**
**	Get next record from file.
**
**	 -> prc		PRC.
**
**	Returns record.
*/
void *
PrcGetNextRecord(void *prc)
{
	((PRC *)prc)->recIdx++;

	return GetRecord((PRC *)prc);
}

/*
**	PrcGetRecIdx
**
**	Return record index.
**
**	 -> prc		PRC.
*/
int
PrcGetRecIdx(void *prc)
{
	return ((PRC *)prc)->recIdx;
}

/*
**	PrcGetRecLen
**
**	Return record length.
**
**	 -> prc		PRC.
*/
int
PrcGetRecLen(void *prc)
{
	return ((PRC *)prc)->recLen;
}

/*
**	PrcGetCategoryName
**
**	Return category name of current record.
**
**	 -> prc		PRC.
*/
char *
PrcGetCategoryName(void *prc)
{
	return ((PRC *)prc)->catName;
}

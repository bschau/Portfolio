//---------------------------------------------------------------------------

#ifndef prcH
#define prcH

// macros - mine
#define MAKE32(a,b,c,d) ((((unsigned long)a)<<24)|(((unsigned long)b)<<16)|(((unsigned long)c)<<8)|((unsigned long)d))
#define CREATORID MAKE32('O','c','c','X')
#define URLID 100

// macros - these are PalmOS constants
typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned long UInt32;
typedef unsigned long LocalID;
#define dmDBNameLength 32
#define PADBYTES 2

// structs - PalmOS structs
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

//---------------------------------------------------------------------------
extern unsigned char *prcBuild(unsigned char *, unsigned char *, unsigned char *, unsigned char *, unsigned int, unsigned int, bool, unsigned int *);
extern bool prcSave(char *, unsigned char *, unsigned int);

//---------------------------------------------------------------------------
#endif

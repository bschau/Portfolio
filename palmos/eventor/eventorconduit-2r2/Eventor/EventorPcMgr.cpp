/*****************************************************************************
 *
 * Generic Conduit CEventorPCMgr Src File
 *
 ****************************************************************************/
#define ASSERT(f)          ((void)0)
#define TRACE0(sz)
#define TRACE(sz)
#include <windows.h>
#include <string.h>
#include <stdio.h>
#ifdef METROWERKS_WIN
#include <wmem.h>
#else
#include <memory.h>
#endif
#include <sys/stat.h>
#include <TCHAR.H>
#include <syncmgr.h>

#include "EventorPCMgr.h"
#include "resource.h"

#ifndef NO_HSLOG
extern HANDLE hLangInstance;
#endif

/* buffers ... */
#define MaxBufSize 4096
#define EventLength 32
#define MAGICYEAR 1904
#define FlgRemindMe 0x8000
#define FlgRepeat 0x4000
#define MaskOnlyDate 0x1ff      /* lower 9 bits */
#define MaskFLGDays 0x3ff       /* lower 10 bits */

/******************************************************************************
 *
 * Class:   CEventorPCMgr
 *
 * Method:  Constructor and Destructor
 *
 * Description: This method initializes the class object.
 *
 * Parameter(s): szDbName - a pointer to a character buffer specifying the 
 *                          name of the database being synced.
 *               pFileName - a pointer to a character buffer specifying the 
 *                          filename of the database.
 *               pDirName - a pointer to a character buffer specifying the 
 *                          directory where the database is stored.
 *               syncType - an enum value specifying the sync type to be performed.
 *
 * Return Value(s): None.
 *
 *****************************************************************************/
CEventorPCMgr::CEventorPCMgr(CPLogging *pLogging, DWORD dwGenericFlags, char *szDbName, TCHAR *pFileName, TCHAR *pDirName, eSyncTypes syncType)  
: CPcMgr(pLogging, dwGenericFlags, szDbName, pFileName, pDirName, syncType)
{
	m_pCatMgr=new CPCategoryMgr();
}

CEventorPCMgr::~CEventorPCMgr() 
{
    TRACE0("CEventorPCMgr Destructor\n");
}

/******************************************************************************
 *
 * Class:   CEventorPCMgr
 *
 * Method:  RetrieveDB()
 *
 * Description: This method loads the database from file.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 *****************************************************************************/
long CEventorPCMgr::RetrieveDB(void)
{
	// return CPcMgr::RetrieveDB();
	long version=0, retVal;
	char buffer[MaxBufSize];

	m_bNeedToSave=FALSE;

	if (!_tcslen(m_szDataFile))
		return GEN_ERR_INVALID_DB_NAME;

	if (m_hFile==INVALID_HANDLE_VALUE)
		return GEN_ERR_INVALID_DB;

	if ((retVal=ReadLine((unsigned char *)buffer, sizeof(buffer)))) {
		if (retVal==GEN_ERR_STORAGE_EOF)
			return 0;

		CloseDB();
		return GEN_ERR_READING_DBINFO;
	}

	if (strncmp(buffer, EVENTOR_OBJECT_ID1, strlen(EVENTOR_OBJECT_ID1))==0)
		version=1;
	else if (strncmp(buffer, EVENTOR_OBJECT_ID2, strlen(EVENTOR_OBJECT_ID2))==0)
		version=2;
	else if (strncmp(buffer, EVENTOR_OBJECT_ID3, strlen(EVENTOR_OBJECT_ID3))==0)
		version=3;

	if (!version) {
		CloseDB();
		return GEN_ERR_DB_VERSION_MISMATCH;
	}

	if ((retVal=CreateDBInfo())) {
		CloseDB();
		return retVal;
	}

	if (m_pCatMgr)
		delete m_pCatMgr;

	m_pCatMgr=new CPCategoryMgr();
	if (!m_pCatMgr)
		return GEN_ERR_LOW_MEMORY;

	if ((retVal=LoadCategories())==0) {
		switch (version) {
		case 1:
			retVal=LoadRecordsV1();
			break;
		case 2:
			retVal=LoadRecordsV2();
			break;
		case 3:
			retVal=LoadRecordsV3();
			break;
		}
	}

	CloseDB();
	m_bNeedToSave=FALSE;
	return retVal;
}

/******************************************************************************
 *
 * Class:   CEventorPCMgr
 *
 * Method:  ReadLine(unsigned char *, long)
 *
 * Description: Read a line from the input database. Trailing newline will
 *				be stripped.
 *
 * Parameter(s): dst - where to store line.
 *				 len - max size of receiving buffer.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 *****************************************************************************/
long CEventorPCMgr::ReadLine(unsigned char *dst, long len)
{
	unsigned char *start=dst;
	long retVal;

	while (len>0) {
		if ((retVal=ReadInData(dst, 1)))
			return retVal;

		if (*dst=='\x00') {
			if (start==dst)
				continue;
			
			return GEN_ERR_STORAGE_EOF;
		}

		if (*dst=='\n') {
			*dst='\x00';
			return 0;
		}

		dst++;
		len--;
	}

	return -1;	
}

/******************************************************************************
 *
 * Class:   CEventorPCMgr
 *
 * Method:  Tokenize(char **)
 *
 * Description: Returns start of token, advance pointer.
 *
 * Parameter(s): tP - token pointer.
 *
 * Return Value(s): NULL - no more tokens.  !NULL, token.
 *
 *****************************************************************************/
char *CEventorPCMgr::Tokenize(char **tP)
{
	char *t=*tP, *start=*tP;

	if (!*t)
		return NULL;

	for (; *t && *t!='\t'; t++);

	if (*t=='\t')
		*t++='\x00';

	*tP=t;
	return start;
}

/******************************************************************************
 *
 * Class:   CEventorPCMgr
 *
 * Method:  HexToVal(unsigned char, unsigned char)
 *
 * Description: Convert two unsigned hex chars to one octet.
 *
 * Parameter(s): hi - hi part of octet.
 *				 lo - lo part of octet.
 *
 * Return Value(s): octet.
 *
 *****************************************************************************/
unsigned char CEventorPCMgr::HexToVal(unsigned char hi, unsigned char lo)
{
	unsigned char hex[]="0123456789abcdef";
	unsigned char v=0, c, idx;

	c=tolower(hi);
	for (idx=0; hex[idx]; idx++) {
		if (hex[idx]==c) {
			v=idx;
			break;
		}
	}

	v<<=4;
	c=tolower(lo);
	for (idx=0; hex[idx]; idx++) {
		if (hex[idx]==c) {
			v|=idx;
			break;
		}
	}

	return v;
}

/******************************************************************************
 *
 * Class:   CEventorPCMgr
 *
 * Method:  LoadCategories()
 *
 * Description: Load categories to data file.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 *****************************************************************************/
long CEventorPCMgr::LoadCategories(void)
{
	char buffer[MaxBufSize];
	long retVal;
	DWORD categoryCount;
	char *p, *d, *name;
	CPCategory cat;

	if ((retVal=ReadLine((unsigned char *)buffer, sizeof(buffer))))
		return retVal;

	// Add 'Unfiled' category
	cat.SetIndex(0);
	cat.SetID(0);
	cat.SetName("Unfiled");
	if ((retVal=m_pCatMgr->Add(cat)))
		return retVal;

	categoryCount=atol(buffer);

	while (categoryCount>0) {
		if ((retVal=ReadLine((unsigned char *)buffer, sizeof(buffer))))
			return retVal;

		// index
		p=buffer;
		d=Tokenize(&p);
		if (d==NULL)
			return GEN_ERR_READING_DBINFO;

		cat.SetIndex(atol(d));

		// id
		d=Tokenize(&p);
		if (d==NULL)
			return GEN_ERR_READING_DBINFO;

		cat.SetID(atol(d));

		// dirty
		d=Tokenize(&p);
		if (d==NULL)
			return GEN_ERR_READING_DBINFO;

		cat.SetDirty((atol(d)==0) ? FALSE : TRUE);

		// name
		name=Tokenize(&p);
		if (name==NULL)
			return GEN_ERR_READING_DBINFO;

		cat.SetName(name);

		// add category
		if ((retVal=m_pCatMgr->Add(cat)))
			return retVal;

		categoryCount--;
	}

	m_pCatMgr->SetChanged(FALSE);
	return 0;
}

/******************************************************************************
 *
 * Class:   CEventorPCMgr
 *
 * Method:  SetRecord()
 *
 * Description: Add a record.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 *****************************************************************************/
void CEventorPCMgr::SetRecord(CPalmRecord *rec, char *rawData, long rawLen, DWORD id, DWORD category, char *attrs)
{
	long retVal=1;
	
	rec->SetRawData(rawLen, (unsigned char *)rawData);
	rec->SetID(id);
	rec->SetIndex(-1);
	rec->SetCategory(category);
	rec->ResetAttribs();

	retVal=1;
	while (retVal) {
		switch (*attrs) {
		case 'N':
			rec->SetNew();
			break;
		case 'M':
			rec->SetUpdate();
			break;
		case 'D':
			rec->SetDeleted();
			break;
		case 'A':
			rec->SetArchived();
			break;
		default:
			retVal=0;
			break;
		}

		attrs++;
	}
}

/******************************************************************************
 *
 * Class:   CEventorPCMgr
 *
 * Method:  LoadRecordsV1()
 *
 * Description: Load V1 records from data file.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 *****************************************************************************/
long CEventorPCMgr::LoadRecordsV1(void)
{
	DWORD id, category, dataLen;
	char buffer[MaxBufSize], rawData[MaxBufSize];
	long retVal, recCnt;
	CPalmRecord rec;
	char *p, *d, *data, *attrs, *rd;
	WORD *wBuf;

	if ((retVal=ReadLine((unsigned char *)buffer, sizeof(buffer))))
		return retVal;

	recCnt=atol(buffer);
	while (recCnt>0) {
		if ((retVal=ReadLine((unsigned char *)buffer, sizeof(buffer))))
			return retVal;

		// id, cat, attrs, data
		p=buffer;
		if ((d=Tokenize(&p))==NULL)
			return GEN_ERR_READING_DBINFO;

		id=atol(d);

		if ((d=Tokenize(&p))==NULL)
			return GEN_ERR_READING_DBINFO;

		category=atol(d);

		if ((attrs=Tokenize(&p))==NULL)
			return GEN_ERR_READING_DBINFO;

		if ((data=Tokenize(&p))==NULL)
			return GEN_ERR_READING_DBINFO;

		// Event
		memset(rawData, 0, EventLength);
		strncpy(rawData, data+8, EventLength);

		// Hex
		rd=rawData+EventLength;
		data[0]=HexToVal((unsigned char)data[0], (unsigned char)data[1]);
		data[1]=HexToVal((unsigned char)data[2], (unsigned char)data[3]);
		data[2]=HexToVal((unsigned char)data[4], (unsigned char)data[5]);
		data[3]=HexToVal((unsigned char)data[6], (unsigned char)data[7]);
		wBuf=(WORD *)data;
		*(WORD *)rd=SyncHostToHHWord(*wBuf);
		rd+=sizeof(WORD);
		wBuf=((WORD *)data)+1;
		*(WORD *)rd=SyncHostToHHWord(*wBuf);

		// create record
		SetRecord(&rec, rawData, rd+sizeof(WORD)-rawData, id, category, attrs);
		if ((AddRec(rec)))
			return GEN_ERR_READING_DBINFO;

		recCnt--;
	}

	return 0;
}

/******************************************************************************
 *
 * Class:   CEventorPCMgr
 *
 * Method:  LoadRecordsV2()
 *
 * Description: Load V2 records from data file.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 *****************************************************************************/
long CEventorPCMgr::LoadRecordsV2(void)
{
	DWORD id, category, dataLen;
	char buffer[MaxBufSize], rawData[MaxBufSize];
	long retVal, recCnt;
	CPalmRecord rec;
	char *p, *d, *date, *attrs, *event, *rd, *flags;
	WORD *wBuf;

	if ((retVal=ReadLine((unsigned char *)buffer, sizeof(buffer))))
		return retVal;

	recCnt=atol(buffer);
	while (recCnt>0) {
		if ((retVal=ReadLine((unsigned char *)buffer, sizeof(buffer))))
			return retVal;

		// id, cat, attrs, data
		p=buffer;
		if ((d=Tokenize(&p))==NULL)
				return GEN_ERR_READING_DBINFO;

		id=atol(d);

		if ((d=Tokenize(&p))==NULL)
			return GEN_ERR_READING_DBINFO;

		category=atol(d);

		if ((attrs=Tokenize(&p))==NULL)
			return GEN_ERR_READING_DBINFO;

		if ((date=Tokenize(&p))==NULL)
			return GEN_ERR_READING_DBINFO;

		if ((flags=Tokenize(&p))==NULL)
			return GEN_ERR_READING_DBINFO;

		if ((event=Tokenize(&p))==NULL)
			return GEN_ERR_READING_DBINFO;

		memset(rawData, 0, EventLength);
		strncpy(rawData, event, EventLength);
		rd=rawData+EventLength;

		*(WORD *)rd=SyncHostToHHWord(atol(date));
		rd+=sizeof(WORD);
		*(WORD *)rd=SyncHostToHHWord(atol(flags));

		// create record
		SetRecord(&rec, rawData, rd+sizeof(WORD)-rawData, id, category, attrs);
		if ((AddRec(rec)))
			return GEN_ERR_READING_DBINFO;

		recCnt--;
	}

	return 0;
}

/******************************************************************************
 *
 * Class:   CEventorPCMgr
 *
 * Method:  LoadRecordsV3()
 *
 * Description: Load V3 records from data file.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 *****************************************************************************/
long CEventorPCMgr::LoadRecordsV3(void)
{
	DWORD id, category, dataLen, pdaDate, pdaFlags;
	char buffer[MaxBufSize], rawData[MaxBufSize];
	long retVal, recCnt;
	CPalmRecord rec;
	char *p, *X, *date, *attrs, *event, *rd, *flags, *days;
	WORD *wBuf, y, m, d;

	if ((retVal=ReadLine((unsigned char *)buffer, sizeof(buffer))))
		return retVal;

	recCnt=atol(buffer);
	while (recCnt>0) {
		if ((retVal=ReadLine((unsigned char *)buffer, sizeof(buffer))))
			return retVal;

		// id, cat, attrs, data
		p=buffer;
		if ((X=Tokenize(&p))==NULL)
				return GEN_ERR_READING_DBINFO;

		id=atol(X);

		if ((X=Tokenize(&p))==NULL)
			return GEN_ERR_READING_DBINFO;

		category=atol(X);

		if ((attrs=Tokenize(&p))==NULL)
			return GEN_ERR_READING_DBINFO;

		if ((date=Tokenize(&p))==NULL)
			return GEN_ERR_READING_DBINFO;

		if ((flags=Tokenize(&p))==NULL)
			return GEN_ERR_READING_DBINFO;

		if ((days=Tokenize(&p))==NULL)
			return GEN_ERR_READING_DBINFO;

		if ((event=Tokenize(&p))==NULL)
			return GEN_ERR_READING_DBINFO;

		sscanf(date, "%d-%d-%d", &y, &m, &d);
		y-=MAGICYEAR;
		pdaDate=y<<9;
		pdaDate|=m<<5;
		pdaDate|=d;

		if (*flags=='Y')
			pdaFlags=FlgRemindMe;
		else
			pdaFlags=0;

		flags++;
		if (*flags=='Y')
			pdaFlags|=FlgRepeat;

		pdaFlags|=atol(days);

		memset(rawData, 0, EventLength);
		strncpy(rawData, event, EventLength);
		rd=rawData+EventLength;

		*(WORD *)rd=SyncHostToHHWord(pdaDate);
		rd+=sizeof(WORD);
		*(WORD *)rd=SyncHostToHHWord(pdaFlags);

		SetRecord(&rec, rawData, rd+sizeof(WORD)-rawData, id, category, attrs);
		if ((AddRec(rec)))
			return GEN_ERR_READING_DBINFO;

		recCnt--;
	}

	return 0;
}


/******************************************************************************
 *
 * Class:   CEventorPCMgr
 *
 * Method:  StoreDB()
 *
 * Description: This method is called to store changes back into the 
 *          storage meduim.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 *****************************************************************************/
long CEventorPCMgr::StoreDB(void)
{
	// return CPcMgr::StoreDB();

	long retVal;
	char buffer[16];

	if (!m_bNeedToSave) {
		if ((!m_pCatMgr) || (!m_pCatMgr->IsChanged()))
			return 0;
	}

	if (!_tcslen(m_szDataFile))
		return GEN_ERR_INVALID_DB_NAME;

	if ((OpenDB()))
		return GEN_ERR_UNABLE_TO_SAVE;

	sprintf(buffer, "%s\n", EVENTOR_OBJECT_ID3);
	if ((retVal=WriteOutData(buffer, strlen(buffer))))
		return retVal;

	if ((retVal=StoreCategories())==0) {
		if ((retVal=StoreRecords())==0)
			m_bNeedToSave=FALSE;
	}

	CloseDB();
	return retVal;
}

/******************************************************************************
 *
 * Class:   CEventorPCMgr
 *
 * Method:  StoreCategories()
 *
 * Description: Writes categories to data file.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 *****************************************************************************/
long CEventorPCMgr::StoreCategories(void)
{
	long retVal;
	DWORD categoryCount=1, tmp, idx;
	CPCategory *cat;
	int size;
	TCHAR buffer[MAX_PATH];

	if (!m_pCatMgr)
		return GEN_ERR_NO_CATEGORIES;

	categoryCount=m_pCatMgr->GetCount();

	sprintf(buffer, "%d\n", (categoryCount>1) ? categoryCount-1 : 0);
	if ((retVal=WriteOutData(buffer, strlen(buffer))))
		return retVal;

	if (categoryCount==0)
		return 0;		// only one category, 'Unfiled'

	for (idx=0; idx<MAX_CATEGORIES; idx++) {
		if (!(cat=m_pCatMgr->GetByIndex(idx)))
			continue;

		if (cat->GetID()==0)	// 'Unfiled'
			continue;

		sprintf(buffer, "%d\t%d\t0\t", idx, cat->GetID());
		if ((retVal=WriteOutData(buffer, strlen(buffer))))
			return retVal;

		size=sizeof(buffer);
		if ((retVal=cat->GetName(buffer, &size)))
			return retVal;

		if ((retVal=WriteOutData(buffer, strlen(buffer))))
			return retVal;

		if ((retVal=WriteOutData("\n", 1)))
			return retVal;
	}
	return 0;
}

/******************************************************************************
 *
 * Class:   CEventorPCMgr
 *
 * Method:  StoreRecords()
 *
 * Description: Writes records to data file.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 *****************************************************************************/
long CEventorPCMgr::StoreRecords(void)
{
	DWORD recCnt=m_dwRecordCount, yieldCount=0, bufSize;
	WORD *wBuf, date, flags, y, m, d;
	CPalmRecord rec;
	long retVal;
	char buffer[MaxBufSize], UInt16val[4], tmpBuf[16], remind, repeat;
	long value;
	
	sprintf(buffer, "%d\n", recCnt);
	if ((retVal=WriteOutData(buffer, strlen(buffer))))
		return retVal;

	if ((retVal=FindFirstRecByIndex(rec)))
		return retVal;

	wBuf=(WORD *)(buffer+EventLength);
	while (!retVal) {
		sprintf(buffer, "%d\t%d\t-\t", rec.GetID(), rec.GetCategory());
		if ((retVal=WriteOutData(buffer, strlen(buffer))))
			break;

		bufSize=sizeof(buffer);
		if ((retVal=rec.GetRawData((unsigned char *)buffer, &bufSize)))
			break;

		date=SyncHHToHostWord(*wBuf);
		y=(date>>9)&0x7f;
		m=(date>>5)&0x0f;
		d=date&0x1f;

		sprintf(tmpBuf, "%u-%u-%u\t", y+MAGICYEAR, m, d);
		if ((retVal=WriteOutData(tmpBuf, strlen(tmpBuf))))
			break;

		flags=SyncHHToHostWord(*(wBuf+1));
		if (flags&FlgRemindMe)
			remind='Y';
		else
			remind='N';

		if (flags&FlgRepeat)
			repeat='Y';
		else
			repeat='N';

		sprintf(tmpBuf, "%c%c\t%u\t", remind, repeat, (flags&MaskFLGDays));
		if ((retVal=WriteOutData(tmpBuf, strlen(tmpBuf))))
			break;

		*wBuf=0;
		if ((retVal=WriteOutData(buffer, strlen(buffer))))
			break;

		if ((retVal=WriteOutData("\n", 1)))
			break;

		recCnt--;
		if (recCnt==0)
			break;

		if ((retVal=FindNext(rec))==GEN_ERR_NO_MORE_RECORDS) {
			retVal=0;
			break;
		}

		yieldCount++;
		if (yieldCount>m_dwYieldTime) {
#ifndef NO_HSLOG
			SyncYieldCycles(100);
#endif
			yieldCount=0;
		}
	}

	return retVal;
}

/******************************************************************************
 *
 * Class:   CEventorPCMgr
 *
 * Method:  ExtractCategories()
 *
 * Description: Dummy function.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *
 *****************************************************************************/
long CEventorPCMgr::ExtractCategories(void)
{
	return 0;
}
/*****************************************************************************
 *
 * Generic Conduit CBMILogPCMgr Src File
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

#include "BMILogPCMgr.h"
#include "resource.h"

#ifndef NO_HSLOG
extern HANDLE hLangInstance;
#endif

#define MaxBufSize 1024
#define MaxNameLength 40

/******************************************************************************
 *
 * Class:   CBMILogPCMgr
 *
 * Method:  Constructor
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
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/

CBMILogPCMgr::CBMILogPCMgr(CPLogging *pLogging, DWORD dwGenericFlags, char *szDbName, TCHAR *pFileName, TCHAR *pDirName, eSyncTypes syncType)  
: CPcMgr(pLogging, dwGenericFlags, szDbName, pFileName, pDirName, syncType)
{

	// TODO - Put your own file format initalization here
	// TODO - Put your own class initalization here


}


CBMILogPCMgr::~CBMILogPCMgr() 
{
    TRACE0("CBMILogPCMgr Destructor\n");
}



/******************************************************************************
 *
 * Class:   CBMILogPCMgr
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
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CBMILogPCMgr::RetrieveDB(void)
{
	char buffer[MaxBufSize];
	long retVal;

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

	if (strncmp(buffer, BMILOG_OBJECT_ID, strlen(BMILOG_OBJECT_ID))) {
		CloseDB();
		return GEN_ERR_DB_VERSION_MISMATCH;
	}

	if ((retVal=CreateDBInfo())) {
		CloseDB();
		return retVal;
	}

	retVal=LoadRecords();

	CloseDB();
	m_bNeedToSave=FALSE;
	return retVal;
}

/******************************************************************************
 *
 * Class:   CBMILogPCMgr
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
long CBMILogPCMgr::ReadLine(unsigned char *dst, long len)
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
 * Class:   CBMILogPCMgr
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
char *CBMILogPCMgr::Tokenize(char **tP)
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
 * Class:   CBMILogPCMgr
 *
 * Method:  LoadRecords()
 *
 * Description: Load records from data file.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 *****************************************************************************/
long CBMILogPCMgr::LoadRecords(void)
{
	DWORD id;
	char buffer[MaxBufSize], rawData[MaxBufSize];
	long retVal, recCnt;
	CPalmRecord rec;
	char *p, *d, *attrs, *rd, *type;

	if ((retVal=ReadLine((unsigned char *)buffer, sizeof(buffer))))
		return retVal;

	recCnt=atol(buffer);
	while (recCnt>0) {
		if ((retVal=ReadLine((unsigned char *)buffer, sizeof(buffer))))
			return retVal;

		// id, attrs, data
		p=buffer;
		if ((d=Tokenize(&p))==NULL)
			return GEN_ERR_READING_DBINFO;

		id=atol(d);

		if ((attrs=Tokenize(&p))==NULL)
			return GEN_ERR_READING_DBINFO;

		if ((type=Tokenize(&p))==NULL)
			return GEN_ERR_READING_DBINFO;

		rd=rawData;
		*rd++=*type;				// type
		*rd++='\x00';				// filer

		if ((d=Tokenize(&p))==NULL)
			return GEN_ERR_READING_DBINFO;

		*(DWORD *)rd=SyncHostToHHDWord(atol(d));
		rd+=sizeof(DWORD);
		
		switch (*type) {
		case '1':	// Person
			if ((d=Tokenize(&p))==NULL)
				return GEN_ERR_READING_DBINFO;

			*(WORD *)rd=SyncHostToHHWord(atol(d));
			rd+=sizeof(WORD);
			memset(rd, 0, MaxNameLength+2);
			strncpy(rd, p, MaxNameLength);
			rd+=(MaxNameLength+2);
			break;

		case '2':	// BMI
			if ((d=Tokenize(&p))==NULL)		// date
				return GEN_ERR_READING_DBINFO;

			*(WORD *)rd=SyncHostToHHWord(atol(d));
			rd+=sizeof(WORD);

			if ((d=Tokenize(&p))==NULL)		// height
				return GEN_ERR_READING_DBINFO;
			
			*(WORD *)rd=SyncHostToHHWord(atol(d));
			rd+=sizeof(WORD);
			
			if ((d=Tokenize(&p))==NULL)		// weight
				return GEN_ERR_READING_DBINFO;
			
			*(WORD *)rd=SyncHostToHHWord(atol(d));
			rd+=sizeof(WORD);
			break;
		}

		// create record
		rec.SetRawData(rd-rawData, (unsigned char *)rawData);
		rec.SetID(id);
		rec.SetIndex(-1);
		rec.SetCategory(0);
		rec.ResetAttribs();

		retVal=1;
		while (retVal) {
			switch (*attrs) {
			case 'N':
				rec.SetNew();
				break;
			case 'M':
				rec.SetUpdate();
				break;
			case 'D':
				rec.SetDeleted();
				break;
			case 'A':
				rec.SetArchived();
				break;
			default:
				retVal=0;
				break;
			}

			attrs++;
		}

		if ((retVal=AddRec(rec)))
			return GEN_ERR_READING_DBINFO;

		recCnt--;
	}

	return 0;
}

/******************************************************************************
 *
 * Class:   CBMILogPCMgr
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
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CBMILogPCMgr::StoreDB(void)
{
	long retVal;
	char buffer[16];

	if (!m_bNeedToSave)
		return 0;

	if (!_tcslen(m_szDataFile))
		return GEN_ERR_INVALID_DB_NAME;

	if ((OpenDB()))
		return GEN_ERR_UNABLE_TO_SAVE;

	sprintf(buffer, "%s\n", BMILOG_OBJECT_ID);
	if ((retVal=WriteOutData(buffer, strlen(buffer))))
		return retVal;

	if ((retVal=StoreRecords())==0)
		m_bNeedToSave=FALSE;
	
	CloseDB();
	return retVal;
}

/******************************************************************************
 *
 * Class:   CBMILogPCMgr
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
long CBMILogPCMgr::StoreRecords(void)
{
	DWORD recCnt=m_dwRecordCount, yieldCount=0, bufSize;
	WORD *wBuf;
	WORD date, height, weight;
	CPalmRecord rec;
	long retVal;
	char recBuf[MaxBufSize], buffer[MaxBufSize], type;
	char *bp;
	
	sprintf(buffer, "%d\n", recCnt);
	if ((retVal=WriteOutData(buffer, strlen(buffer))))
		return retVal;

	if ((retVal=FindFirstRecByIndex(rec)))
		return retVal;

	wBuf=(WORD *)buffer;
	while (!retVal) {
		sprintf(buffer, "%d\t-\t", rec.GetID());		// and attrs  "-"
		if ((retVal=WriteOutData(buffer, strlen(buffer))))
			break;

		bufSize=sizeof(recBuf);
		if ((retVal=rec.GetRawData((unsigned char *)recBuf, &bufSize)))
			break;

		// parse here ...
		bp=recBuf;
		type=*bp;
		bp+=2;

		sprintf(buffer, "%c\t%lu\t", type, SyncHHToHostDWord(*(DWORD *)bp));
		if ((retVal=WriteOutData(buffer, strlen(buffer))))
			break;

		bp+=sizeof(DWORD);
		wBuf=(WORD *)bp;

		switch (type) {
		case '1':
			bp+=sizeof(WORD);
			sprintf(buffer, "%u\t%s\n", SyncHHToHostWord(*wBuf), bp);
			break;
		case '2':
			date=SyncHHToHostWord(*wBuf++);
			height=SyncHHToHostWord(*wBuf++);
			weight=SyncHHToHostWord(*wBuf);

			sprintf(buffer, "%u\t%u\t%u\n", date, height, weight);
			break;
		}

		if ((retVal=WriteOutData(buffer, strlen(buffer))))
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
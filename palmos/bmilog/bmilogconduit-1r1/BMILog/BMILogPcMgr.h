/*****************************************************************************
 *
 * CBMILogPCMgr Generic Conduit PC Database Mgr Header File
 *
 ****************************************************************************/
#ifndef _BMILog_PC_DB_MGR_H_
#define _BMILog_PC_DB_MGR_H_
#include <CPString.h>
#include <PcMgr.h>

#define BMILOG_OBJECT_ID "bmi1"

class CBMILogPCMgr : public CPcMgr
{
public: 
    CBMILogPCMgr(CPLogging *pLogging, DWORD dwGenericFlags, char *szDbName, TCHAR *pFileName = NULL, TCHAR *pDirName = NULL, eSyncTypes syncType = eDoNothing);
    virtual ~CBMILogPCMgr();
    virtual long RetrieveDB(void);
  	virtual long ReadLine(unsigned char *, long);
	virtual char *Tokenize(char **);
	virtual long LoadRecords(void);
	virtual long StoreDB(void);
	virtual long StoreRecords(void);
};



#endif




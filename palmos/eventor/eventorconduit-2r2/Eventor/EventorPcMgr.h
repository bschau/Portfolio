/*****************************************************************************
 *
 * CEventorPCMgr Generic Conduit PC Database Mgr Header File
 *
 ****************************************************************************/
#ifndef _Eventor_PC_DB_MGR_H_
#define _Eventor_PC_DB_MGR_H_
#include <CPString.h>
#include <PcMgr.h>

/* ev1: Eventor 1r1 - 1r7 */
#define EVENTOR_OBJECT_ID1 "ev1"

/* ev2: Eventor 2r1 */
#define EVENTOR_OBJECT_ID2 "ev2"

/* ev3: Eventor 2r1 - output changed */
#define EVENTOR_OBJECT_ID3 "ev3"

class CEventorPCMgr : public CPcMgr
{
public: 
    CEventorPCMgr(CPLogging *pLogging, DWORD dwGenericFlags, char *szDbName, TCHAR *pFileName = NULL, TCHAR *pDirName = NULL, eSyncTypes syncType = eDoNothing);
    virtual ~CEventorPCMgr();
    virtual long RetrieveDB(void);
	virtual long ReadLine(unsigned char *, long);
	virtual char *Tokenize(char **);
	virtual unsigned char HexToVal(unsigned char, unsigned char);
	virtual long LoadCategories(void);
	virtual void SetRecord(CPalmRecord *, char *, long, DWORD, DWORD, char *);
	virtual long LoadRecordsV1(void);
	virtual long LoadRecordsV2(void);
	virtual long LoadRecordsV3(void);
	virtual long StoreDB(void);
	virtual long StoreCategories(void);
	virtual long StoreRecords(void);
	virtual long ExtractCategories(void);
};



#endif




/*****************************************************************************
 *
 * CEventorBackupMgr Generic Conduit Backup Database Mgr Header File
 *
 ****************************************************************************/

#ifndef _Eventor_BACKUP_DB_MGR_H_
#define _Eventor_BACKUP_DB_MGR_H_

#include "EventorPCMgr.h"


class CEventorBackupMgr : public CEventorPCMgr
{
public:
    CEventorBackupMgr(CPLogging *pLogging, DWORD dwGenericFlags, char *szDbName, TCHAR *pFileName, TCHAR *pDirName = NULL, eSyncTypes syncType = eDoNothing);
    virtual ~CEventorBackupMgr();

protected:  // protected Variables
    TCHAR m_szRealDb[256];


public:     // Public Functions
    virtual long PreSync(void);
    virtual long PostSync(void);
    virtual long Close(BOOL bDontUpdate=FALSE);

protected:    // protected Functions
    virtual long OpenDB(void);

};

#endif


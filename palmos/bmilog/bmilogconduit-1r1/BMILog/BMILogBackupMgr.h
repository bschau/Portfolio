/*****************************************************************************
 *
 * CBMILogBackupMgr Generic Conduit Backup Database Mgr Header File
 *
 ****************************************************************************/

#ifndef _BMILog_BACKUP_DB_MGR_H_
#define _BMILog_BACKUP_DB_MGR_H_

#include "BMILogPCMgr.h"


class CBMILogBackupMgr : public CBMILogPCMgr
{
public:
    CBMILogBackupMgr(CPLogging *pLogging, DWORD dwGenericFlags, char *szDbName, TCHAR *pFileName, TCHAR *pDirName = NULL, eSyncTypes syncType = eDoNothing);
    virtual ~CBMILogBackupMgr();

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


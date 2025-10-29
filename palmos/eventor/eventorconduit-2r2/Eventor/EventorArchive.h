/*****************************************************************************
 *
 * EventorArchive Generic Conduit Archive Database Mgr Header File
 *
 ****************************************************************************/
#ifndef _Eventor_ARCHIVE_DB_MGR_H_
#define _Eventor_ARCHIVE_DB_MGR_H_

#include "EventorPCMgr.h"


class CEventorArchive : public CEventorPCMgr
{
public:
    CEventorArchive(CPLogging *pLogging, DWORD dwGenericFlags, char *szDbName, TCHAR *pFileName, TCHAR *pDirName = NULL);
    virtual ~CEventorArchive();
public:   // Public Variables

protected:  // protected Variables

public:     // Public Functions
    virtual long PostSync(void);
    virtual long PreSync(void);

protected:    // protected Functions
    virtual long OpenDB(void);
};

#endif


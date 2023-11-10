/*****************************************************************************
 *
 * Generic Conduit CEventorSync Header File
 *
 ****************************************************************************/
#ifndef __Eventor_SYNCHRONIZER_H_
#define __Eventor_SYNCHRONIZER_H_

#include <GenSync.H>
#include "EventorPCMgr.h"
#include "EventorBackupMgr.h"



class CEventorSync : public CSynchronizer
{
public:
    CEventorSync(CSyncProperties& rProps, DWORD dwDatabaseFlags);
    virtual ~CEventorSync();


protected:
    virtual CPDbBaseMgr *CreateArchiveManager(TCHAR *pFilename);
	virtual long CreatePCManager(void);
    virtual long CreateBackupManager(void);

    virtual long SynchronizeAppInfo(void);
    virtual long CopyAppInfoHHtoPC( void );
    virtual long CopyAppInfoPCtoHH( void );
};

#endif



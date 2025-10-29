/*****************************************************************************
 *
 * Generic Conduit CBMILogSync Header File
 *
 ****************************************************************************/
#ifndef __BMILog_SYNCHRONIZER_H_
#define __BMILog_SYNCHRONIZER_H_

#include <GenSync.H>
#include "BMILogPCMgr.h"
#include "BMILogBackupMgr.h"



class CBMILogSync : public CSynchronizer
{
public:
    CBMILogSync(CSyncProperties& rProps, DWORD dwDatabaseFlags);
    virtual ~CBMILogSync();


protected:
    virtual long ArchiveRecord(CPalmRecord &palmRec);
	virtual long CreatePCManager(void);
    virtual long CreateBackupManager(void);

};

#endif



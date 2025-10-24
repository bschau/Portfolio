/*****************************************************************************
 *
 * Generic Conduit - CBMILogSync Src File
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
#include "BMILogSync.h"
extern HANDLE hLangInstance;

CBMILogSync::CBMILogSync(CSyncProperties& rProps, DWORD dwDatabaseFlags)
: CSynchronizer(rProps, dwDatabaseFlags)
{
}

CBMILogSync::~CBMILogSync()
{
}


/******************************************************************************
 *
 * Class:   CBMILogSync
 *
 * Method:  CreatePCManager()
 *
 * Description: This method creates the PC database manager. If it successfully creates
 *          the manager, it then opens the database.
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
long CBMILogSync::CreatePCManager(void)
{
    DeletePCManager();


    
    m_dbPC = new CBMILogPCMgr(m_pLog, 
                        m_dwDatabaseFlags,
                        m_remoteDB->m_Name, 
                        m_rSyncProperties.m_LocalName, 
                        m_rSyncProperties.m_PathName,
                        m_rSyncProperties.m_SyncType);
    if (!m_dbPC)
        return GEN_ERR_LOW_MEMORY;
    return m_dbPC->Open();
}

/******************************************************************************
 *
 * Class:   CBMILogSync
 *
 * Method:  CreateBackupManager()
 *
 * Description: This method creates the backup database manager. If it successfully creates
 *          the manager, it then opens the database.
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
long CBMILogSync::CreateBackupManager(void)
{
    DeleteBackupManager();

    m_dbBackup = new CBMILogBackupMgr(m_pLog, 
                                m_dwDatabaseFlags,
                                m_remoteDB->m_Name, 
                                m_rSyncProperties.m_LocalName, 
                                m_rSyncProperties.m_PathName,
                                m_rSyncProperties.m_SyncType);
    if (!m_dbBackup)
        return GEN_ERR_LOW_MEMORY;
    return m_dbBackup->Open();
}

/******************************************************************************
 *
 * Class:   CBMILogSync
 *
 * Method:  ArchiveRecord()
 *
 * Description: This method opens the archive database associated with this record and
 *              adds the record.
 *
 * Parameter(s): palmRec - a CPalmRecord object specifying the record to be archived
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
long CBMILogSync::ArchiveRecord(CPalmRecord &palmRec)
{
    long retval = 0;
    // since this method has no actions, it essential turns off
    // archiving in this conduit. If you want to support archiving,
    // you can either remove this method, in which case the CSynchronizer:ArchiveRecord
    // will handle the archiving, or you can implement your own ArchiveRecord logic.

    return retval;
}





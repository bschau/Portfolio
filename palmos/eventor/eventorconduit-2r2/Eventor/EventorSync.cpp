/*****************************************************************************
 *
 * Generic Conduit - CEventorSync Src File
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
#include "EventorSync.h"
#include "EventorArchive.h"
extern HANDLE hLangInstance;

CEventorSync::CEventorSync(CSyncProperties& rProps, DWORD dwDatabaseFlags)
: CSynchronizer(rProps, dwDatabaseFlags)
{
}

CEventorSync::~CEventorSync()
{
}


/******************************************************************************
 *
 * Class:   CEventorSync
 *
 * Method:  managers
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
long CEventorSync::CreatePCManager(void)
{
    DeletePCManager();

	m_dbPC = new CEventorPCMgr(m_pLog, 
                        m_dwDatabaseFlags,
                        m_remoteDB->m_Name, 
                        m_rSyncProperties.m_LocalName, 
                        m_rSyncProperties.m_PathName,
                        m_rSyncProperties.m_SyncType);
    
	if (!m_dbPC)
        return GEN_ERR_LOW_MEMORY;
    
	return m_dbPC->Open();
}

long CEventorSync::CreateBackupManager(void)
{
    DeleteBackupManager();

    m_dbBackup = new CEventorBackupMgr(m_pLog, 
                                m_dwDatabaseFlags,
                                m_remoteDB->m_Name, 
                                m_rSyncProperties.m_LocalName, 
                                m_rSyncProperties.m_PathName,
                                m_rSyncProperties.m_SyncType);
    
	if (!m_dbBackup)
        return GEN_ERR_LOW_MEMORY;
    
	return m_dbBackup->Open();
}

CPDbBaseMgr *CEventorSync::CreateArchiveManager(TCHAR *pFilename) 
{
    return (CPDbBaseMgr *)new CEventorArchive(m_pLog,
                                m_dwDatabaseFlags,
                                m_remoteDB->m_Name, 
                                pFilename, 
                                m_rSyncProperties.m_PathName);
}

/******************************************************************************
 *
 * Class:   CEventorSync
 *
 * Method:  SynchronizeAppInfo
 *
 * Description: Takes care of all the AppInfo block synchronization
 *
 * Parameter(s): None
 *
 * Return Value(s): 0 - success
 *					<0 - error
 *
 *****************************************************************************/
long CEventorSync::SynchronizeAppInfo()
{
	long retVal = 0;

	//do the presync stuff

	if ((retVal=m_dbHH->PreAppInfoSync())) {
		if (retVal==GEN_ERR_CATEGORIES_NOT_SUPPORTED)
			return 0;
		
		return retVal;
	}

	if ((retVal=m_dbPC->PreAppInfoSync())) {
		if (retVal==GEN_ERR_CATEGORIES_NOT_SUPPORTED)
			return 0;
		
		return retVal;
	}

	if ((retVal=SynchronizeCategories()))
		return retVal;

	if ((retVal=m_dbHH->PostAppInfoSync()))
		return retVal;

	return m_dbPC->PostAppInfoSync();
}

/******************************************************************************
 *
 * Class:   CEventorSync
 *
 * Method:  CopyAppInfoHHtoPC
 *
 * Description: Copies the appinfo block from the device to the PC in 
 *				Device overwrites PC synchronizations.
 *
 * Parameter(s): None
 *
 * Return Value(s): 0 - success
 *					<0 - error
 *
 *****************************************************************************/
long CEventorSync::CopyAppInfoHHtoPC()
{
	long retVal;

	CPCategoryMgr *pHHCatMgr, *pPCCatMgr;
	CPCategory* pCat;

	if ((retVal=m_dbHH->PreAppInfoSync()))
		return retVal;

	pHHCatMgr=m_dbHH->GetCatMgr();
	pPCCatMgr=m_dbPC->GetCatMgr();
	pCat=pHHCatMgr->FindFirst();

	while (pCat && !retVal) {
		retVal=pPCCatMgr->Add(*pCat);
		pCat=pHHCatMgr->FindNext();
	}

	if (!retVal)
		retVal=m_dbPC->PostAppInfoSync();

	return retVal;
}

/******************************************************************************
 *
 * Class:   CEventorSync
 *
 * Method:  CopyAppInfoPCtoHH
 *
 * Description: Copies the PC appinfo block over to the device in PC overwrites Handheld
 *				synchronizations.
 *
 * Parameter(s): None
 *
 *
 * Return Value(s): 0 - success
 *					<0 - error
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/24/00    ELP         initial revision
 *
 *****************************************************************************/
long CEventorSync::CopyAppInfoPCtoHH()
{
	CPCategoryMgr *pPCCatMgr, *pHHCatMgr;
	CPCategory *pCat;
	long retVal;

	if ((retVal=m_dbPC->PreAppInfoSync()))
		return retVal;

	if ((retVal=m_dbHH->PreAppInfoSync()))
		return retVal;
	
	pPCCatMgr=m_dbPC->GetCatMgr();
	pHHCatMgr=m_dbHH->GetCatMgr();
    pHHCatMgr->Empty();

	pCat=pPCCatMgr->FindFirst();
	while (pCat && !retVal) {
		retVal=pHHCatMgr->Add(*pCat);
		pCat=pPCCatMgr->FindNext();
	}

	if (!retVal)
		retVal=m_dbHH->PostAppInfoSync();

	return retVal;
}
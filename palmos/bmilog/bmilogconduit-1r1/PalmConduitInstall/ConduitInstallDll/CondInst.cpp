////////////////////////////////////////////////////////////////////////////
// Sample Code Disclaimer
//
// Copyright © 2000 Palm, Inc. or its subsidiaries. All rights reserved.
//
// You may incorporate this sample code (the "Code") into your applications
// for Palm OS(R) platform products and may use the Code to develop
// such applications without restriction.  The Code is provided to you on
// an "AS IS" basis and the responsibility for its operation is 100% yours.
// PALM, INC. AND ITS SUBSIDIARIES (COLLECTIVELY, "PALM") DISCLAIM
// ALL WARRANTIES, TERMS AND CONDITIONS WITH RESPECT TO THE CODE, EXPRESS,
// IMPLIED, STATUTORY OR OTHERWISE, INCLUDING WARRANTIES, TERMS OR
// CONDITIONS OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
// NONINFRINGEMENT AND SATISFACTORY QUALITY.  You are not permitted to
// redistribute the Code on a stand-alone basis and you may only
// redistribute the Code in object code form as incorporated into your
// applications.  TO THE FULL EXTENT ALLOWED BY LAW, PALM ALSO EXCLUDES ANY
// LIABILITY, WHETHER BASED IN CONTRACT OR TORT (INCLUDING NEGLIGENCE), FOR
// INCIDENTAL, CONSEQUENTIAL, INDIRECT, SPECIAL OR PUNITIVE DAMAGES OF ANY
// KIND, OR FOR LOSS OF REVENUE OR PROFITS, LOSS OF BUSINESS, LOSS OF
// INFORMATION OR DATA, OR OTHER FINANCIAL LOSS ARISING OUT OF OR IN
// CONNECTION WITH THE USE OR PERFORMANCE OF THE CODE.  The Code is subject
// to Restricted Rights for U.S. government users and export regulations.
//
////////////////////////////////////////////////////////////////////////////
//
// SAMPLE NAME:		Palm Conduit Installer
//
// FILE:     		CondInst.cpp
//
// DESCRIPTION:		
//	This file provides sample functions to install HotSync(R) technology
//	conduits to a target system. This file contains the code necessary
//	to implement a conduit installer Dll. We provide, with this project, a
//	simple installer executable to demonstrate the use of these functions.
//	The following functions are exported and can be called directly from
//	your installer executable:
//
//	InstallConduit	- sample conduit install function.
//	UnInstallConduit- sample conduit un-install function.
//
////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <direct.h>
#include <crtdbg.h>
#include "CondMgr.h"
#include "HSAPI.h"

#include "CondInst.h"

//----- Defines ---------------------------------------------------------------

#define	_THIS_FILE					"CondInst.dll"



// Define any Conduit Manager function pointer types
typedef int (WINAPI *CmGetCorePathPtr)(TCHAR *pPath, int *piSize);
typedef int (WINAPI *CmGetHotSyncExecPathPtr)(char *szPath, int *iSize);

typedef int (WINAPI *CmInstallCreatorPtr)(const char *pCreator, int iType);
typedef int (WINAPI *CmRemoveConduitByCreatorIDPtr)(const char *pCreator);
typedef int (WINAPI *CmSetCreatorModulePtr)(const char *pCreator, const TCHAR *pConduitModule);
typedef int (WINAPI *CmSetCreatorDirectoryPtr)(const char *pCreator, const TCHAR *pDirectory);
typedef int (WINAPI *CmSetCreatorFilePtr)(const char *pCreator, const TCHAR *pFile);
typedef int (WINAPI *CmSetCreatorRemotePtr)(const char *pCreator, const TCHAR *pRemote);
typedef int (WINAPI *CmSetCreatorNamePtr)(const char *pCreator, const TCHAR *pConduitName);
typedef int (WINAPI *CmSetCreatorPriorityPtr)(const char *pCreator, DWORD dwPriority);
typedef int (WINAPI *CmSetCreatorInfoPtr)(const char *pCreator, const TCHAR *pInfo);
typedef int (WINAPI *CmSetCreatorIntegratePtr)(const char *pCreator, DWORD dwIntegrate);
typedef int (WINAPI *CmSetCreatorValueDwordPtr)(const char *pCreator, TCHAR *pValue, DWORD dwValue);

// Define any HSAPI function pointer types
typedef int (WINAPI *HsCheckApiStatusPtr)(void);
typedef int (WINAPI *HsGetSyncStatusPtr)(DWORD *dwStatus);
typedef int (WINAPI *HsSetAppStatusPtr)(HsStatusType statusType, DWORD dwStartFlags);

	

//----- Functions -------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////
//
//	Function:		GetPalmDesktopInstallDirectory()
//
//	Description:	Determines the Palm Desktop install directory
//
//	Parameters:
//		hCondMgrDll		- handle to instance of previously loaded Conduit
//							 Manager DLL.
//		pPDInstallDirectory	- buffer for storage of Palm Desktop directory.
//		pSize				- pointer to buffer size. On entry, size of buffer.
//								On exit, actual length of path.
//
//	Returns:
//		0			- no error.
//		non-zero	- error code.
//
/////////////////////////////////////////////////////////////////////////////
INT32 GetPalmDesktopInstallDirectory(HINSTANCE hCondMgrDll, TCHAR *pPDInstallDirectory,
										int	*pSize )
{
	INT32	dwReturnCode;

	// Get the Palm Desktop Installation directory
	CmGetHotSyncExecPathPtr	lpfnCmGetHotSyncExecPath;
	lpfnCmGetHotSyncExecPath = (CmGetHotSyncExecPathPtr) GetProcAddress(hCondMgrDll, "CmGetHotSyncExecPath");

	if( lpfnCmGetHotSyncExecPath == NULL )
		// Invalid Conduit Manager Library - missing functions
		return PCI_ERR_INVALID_CONDMGR;
		
	if( (dwReturnCode=(*lpfnCmGetHotSyncExecPath)(pPDInstallDirectory, pSize)) != 0)
		// Error determining Palm Desktop install directory,
		// return the Conduit Manager error code.
		return dwReturnCode;

	if( strstr( pPDInstallDirectory, PCI_EXECUTABLE_EXTENSION ) != NULL )
	{
		// Remove the "HotSync.exe" portion
		char*	cPtr = &(pPDInstallDirectory[*pSize]);
		while( cPtr >= pPDInstallDirectory )
		{
			if( *cPtr == PCI_DIRECTORY_SEPARATOR )
				break;
			cPtr--;
		}
		if( *cPtr == PCI_DIRECTORY_SEPARATOR )
			*cPtr = '\0';
	}
	
	// Set the size of the returned path
	*pSize = strlen(pPDInstallDirectory);
			
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Function:		LoadConduitManagerDll()
//
//	Description:	Loads the Conduit Manager DLL. Uses the following
//		algorithm:
//			1. Load the temporary Conduit Manager Dll shipped with the
//				installer.
//			2. Find the Palm Desktop installation directory.
//			3. Load the Palm Desktop Conduit Manager.
//			4. Return a handle to the Conduit Manager library.
//
//	Parameters:
//		hCondMgrDll		- handle to instance of loaded Conduit Manager DLL.
//							Returns NULL on error.
//		szInstallDir	- location of the Conduit Manager included with the
//							installer.
//
//	Returns:
//		0			- no error.
//		non-zero	- error code.
//
/////////////////////////////////////////////////////////////////////////////
INT32 LoadConduitManagerDll(HINSTANCE* hCondMgrDll, LPSTR szInstallDir)
{
	INT32	dwReturnCode;
	HINSTANCE	hInstallerCondMgrDll;

	// Initialize the return value
	*hCondMgrDll=NULL;
	
	// First, load the temporary Conduit Manager Dll shipped with the installer
	TCHAR	szInstallerCondMgrPath[_MAX_PATH];
	strncpy(szInstallerCondMgrPath, szInstallDir, _MAX_PATH);
	strncat(szInstallerCondMgrPath, PCI_DIRECTORY_SEPARATOR_STR, _MAX_PATH-strlen(szInstallerCondMgrPath));
	strncat(szInstallerCondMgrPath, PCI_CONDMGR_FILENAME, _MAX_PATH-strlen(szInstallerCondMgrPath));
	if( (hInstallerCondMgrDll=LoadLibrary(szInstallerCondMgrPath)) == NULL )
		// Error loading temporary Conduit Manager Library
		return PCI_ERR_LOADING_TEMP_CONDMGR;

	// Get the Palm Desktop Installation directory
	TCHAR	szPalmDesktopDir[_MAX_PATH];
	int	size=_MAX_PATH;
	if( (dwReturnCode=GetPalmDesktopInstallDirectory(hInstallerCondMgrDll, szPalmDesktopDir, &size)) != 0 )
		return dwReturnCode;
			
	// Construct the path of the Palm Desktop Conduit Manager
	TCHAR	szPDCondMgrPath[_MAX_PATH];
	strncpy(szPDCondMgrPath, szPalmDesktopDir, _MAX_PATH);
	strncat(szPDCondMgrPath, PCI_DIRECTORY_SEPARATOR_STR, _MAX_PATH-strlen(szPDCondMgrPath));
	strncat(szPDCondMgrPath, PCI_CONDMGR_FILENAME, _MAX_PATH-strlen(szPDCondMgrPath));
			
	// Load the Conduit Manager library from the Palm Desktop directory
	if( (*hCondMgrDll=LoadLibrary(szPDCondMgrPath)) == NULL )
	{
		// Error loading Palm Desktop Conduit Manager library
		// use the one shipped with the installer
		*hCondMgrDll = hInstallerCondMgrDll;
	}
	else
	{
		// Success loading the Palm Desktop Conduit Manager
		// free the temporary Conduit Manager
		FreeLibrary(hInstallerCondMgrDll);
	}
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
//	Function:		LoadHsapiDll()
//
//	Description:	Loads the HSAPI DLL from the Palm Desktop installation
//		directory. If it does not exist there, load the one included in the
//		installer. If neither can be loaded, return an error.
//
//	Parameters:
//		hHsapiDLL		- returned handle to instance of loaded HSAPI DLL.
//							Returns NULL on error.
//		szPalmDesktopDirectory	- Palm Desktop installation directory.
//		szSupportDllDirectory	- location of the HSAPI Dll included with
//									the installer.
//
//	Returns:
//		0			- no error.
//		non-zero	- error code.
//
/////////////////////////////////////////////////////////////////////////////
INT32 LoadHsapiDll(HINSTANCE* hHsapiDLL, const char* szPalmDesktopDirectory,
					const char* szSupportDllDirectory)
{
	// Initialize the return value
	*hHsapiDLL=NULL;
	
	// First, try loading the HSAPI Dll from the Palm Desktop directory
	TCHAR	szHsapiPath[_MAX_PATH];
	strncpy(szHsapiPath, szPalmDesktopDirectory, _MAX_PATH);
	strncat(szHsapiPath, PCI_DIRECTORY_SEPARATOR_STR, _MAX_PATH-strlen(szHsapiPath));
	strncat(szHsapiPath, PCI_HSAPI_FILENAME, _MAX_PATH-strlen(szHsapiPath));
	if( (*hHsapiDLL=LoadLibrary(szHsapiPath)) != NULL )
		// Successfully loaded HSAPI Library from Palm Desktop Directory
		return 0;

	// Second, try loading the HSAPI Dll included with the installer
	strncpy(szHsapiPath, szSupportDllDirectory, _MAX_PATH);
	strncat(szHsapiPath, PCI_DIRECTORY_SEPARATOR_STR, _MAX_PATH-strlen(szHsapiPath));
	strncat(szHsapiPath, PCI_HSAPI_FILENAME, _MAX_PATH-strlen(szHsapiPath));
	if( (*hHsapiDLL=LoadLibrary(szHsapiPath)) != NULL )
		// Successfully loaded HSAPI Library from support directory
		return 0;
	
	// If we get here, then there was an error loading the library
	return PCI_ERR_HSAPI_NOT_FOUND;
}

/////////////////////////////////////////////////////////////////////////////
//
//	Function:		IsHotSyncRunning()
//
//	Description:	Determines if HotSync is currently running.
//
//	Parameters:
//		hHsapiDLL		- handle to instance of loaded HSAPI DLL.
//
//	Returns:
//		TRUE	- HotSync running.
//		FALSE	- HotSync not running or HSAPI error.
//
/////////////////////////////////////////////////////////////////////////////
BOOL IsHotSyncRunning(HINSTANCE hHsapiDLL)
{
	BOOL	bRetVal = FALSE;
	
	_ASSERT(hHsapiDLL);
	
	// Prepare to use the HSAPI functions
	HsCheckApiStatusPtr	lpfnHsCheckApiStatus;
	lpfnHsCheckApiStatus = (HsCheckApiStatusPtr) GetProcAddress(hHsapiDLL, "HsCheckApiStatus");
	
	if( lpfnHsCheckApiStatus )
	{
		if( (*lpfnHsCheckApiStatus)() == 0 )
			bRetVal = TRUE;
	}
	return bRetVal;
}

/////////////////////////////////////////////////////////////////////////////
//
//	Function:		IsHotSyncInProgress()
//
//	Description:	Determines if there is currently a HotSync in progress.
//
//	Parameters:
//		hHsapiDLL		- handle to instance of loaded HSAPI DLL.
//
//	Returns:
//		TRUE	- HotSync in progress or HSAPI error.
//		FALSE	- no HotSync in progress.
//
/////////////////////////////////////////////////////////////////////////////
BOOL IsHotSyncInProgress(HINSTANCE hHsapiDLL)
{
	BOOL	bRetVal = TRUE;
	DWORD	dwStatus;
	
	_ASSERT(hHsapiDLL);
	
	if(IsHotSyncRunning(hHsapiDLL))
	{
		// Prepare to use the HSAPI functions
		HsGetSyncStatusPtr	lpfnHsGetSyncStatus;
		lpfnHsGetSyncStatus = (HsGetSyncStatusPtr) GetProcAddress(hHsapiDLL, "HsGetSyncStatus");
	
		if( lpfnHsGetSyncStatus )
		{
			if( (*lpfnHsGetSyncStatus)(&dwStatus) == 0 )
				if( dwStatus == HOTSYNC_STATUS_IDLE )
					bRetVal = FALSE;
		}
	}
	return bRetVal;
}

/////////////////////////////////////////////////////////////////////////////
//
//	Function:		ShutdownHotSync()
//
//	Description:	Shuts down HotSync. If HotSync isn't stopped after 
//					a few seconds, just return.
//
//	Parameters:
//		hHsapiDLL		- handle to instance of loaded HSAPI DLL.
//
//	Returns:
//		void.
//
/////////////////////////////////////////////////////////////////////////////
VOID ShutdownHotSync(HINSTANCE hHsapiDLL)
{
	_ASSERT(hHsapiDLL);
	BOOL	bHotSyncRunning=IsHotSyncRunning(hHsapiDLL);
	
	if(bHotSyncRunning)
	{
		// Prepare to use the HSAPI functions
		HsSetAppStatusPtr	lpfnHsSetAppStatus;
		lpfnHsSetAppStatus = (HsSetAppStatusPtr) GetProcAddress(hHsapiDLL, "HsSetAppStatus");
	
		if( lpfnHsSetAppStatus )
			(*lpfnHsSetAppStatus)(HsCloseApp, HSFLAG_NONE);
		
		// Wait for HotSync to stop
		for( int i=0; (i<PCI_HOTSYNC_MAX_WAIT*2) && bHotSyncRunning; i++ )
		{
			if( (bHotSyncRunning=IsHotSyncRunning(hHsapiDLL)) == TRUE )
				Sleep(500);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//	Function:		StartHotSync()
//
//	Description:	Starts HotSync. If HotSync doesn't start after a few
//					seconds, just return.
//
//	Parameters:
//		hHsapiDLL		- handle to instance of loaded HSAPI DLL.
//
//	Returns:
//		void.
//
/////////////////////////////////////////////////////////////////////////////
VOID StartHotSync(HINSTANCE hHsapiDLL)
{
	_ASSERT(hHsapiDLL);
	BOOL	bHotSyncRunning=IsHotSyncRunning(hHsapiDLL);
	
	if(!bHotSyncRunning)
	{
		// Prepare to use the HSAPI functions
		HsSetAppStatusPtr	lpfnHsSetAppStatus;
		lpfnHsSetAppStatus = (HsSetAppStatusPtr) GetProcAddress(hHsapiDLL, "HsSetAppStatus");
	
		if( lpfnHsSetAppStatus )
			(*lpfnHsSetAppStatus)(HsStartApp, HSFLAG_NONE);
		
		// Wait for HotSync to start
		for( int i=0; (i<PCI_HOTSYNC_MAX_WAIT*2) && !bHotSyncRunning; i++ )
		{
			if( (bHotSyncRunning=IsHotSyncRunning(hHsapiDLL)) == FALSE )
				Sleep(500);
		}
	}
}



/////////////////////////////////////////////////////////////////////////////
// EXTERN
/////////////////////////////////////////////////////////////////////////////
//
//	Function:		UninstallConduit()
//
//	Description:
//		Uninstalls a conduit on local system using Conduit Manager API.
//
//	Parameters:
//		szInstallDirectory	- location (folder) of conduit dll.
//		szCondMgrDirectory	- location where the uninstaller places any
//								Palm Support dll's (ie - Conduit Manager
//								HSAPI, UserData, InstallAide).
//		pCreator			- creator id in string form.
//
//	Returns:
//		0				- success.
//		non-zero		- error.
//
/////////////////////////////////////////////////////////////////////////////
INT32 WINAPI UninstallConduit(LPSTR szSupportDllDirectory, const char *pCreator )
{ 
	INT32	dwReturnCode;
	BOOL	bHotSyncRunning = FALSE;

	_ASSERT(szSupportDllDirectory);
	_ASSERT(*szSupportDllDirectory);

	// Load the Conduit Manager DLL.
	HINSTANCE hConduitManagerDLL;
	if( (dwReturnCode = LoadConduitManagerDll(&hConduitManagerDLL, szSupportDllDirectory)) != 0 )
		return(dwReturnCode);
	
	// Prepare to uninstall the conduit using Conduit Manager functions
	CmRemoveConduitByCreatorIDPtr	lpfnCmRemoveConduitByCreatorID;
	lpfnCmRemoveConduitByCreatorID = (CmRemoveConduitByCreatorIDPtr) GetProcAddress(hConduitManagerDLL, "CmRemoveConduitByCreatorID");
	
	if( (lpfnCmRemoveConduitByCreatorID == NULL) )
	{
		// Return error code.
		return(PCI_ERR_INVALID_CONDMGR);
	}
	
	// Get the Palm Desktop Installation directory
	TCHAR	szPalmDesktopDir[_MAX_PATH];
	int	size=_MAX_PATH;
	if( (dwReturnCode=GetPalmDesktopInstallDirectory(hConduitManagerDLL, szPalmDesktopDir, &size)) != 0 )
		return dwReturnCode;
	
	// Load the HSAPI DLL.
	HINSTANCE hHsapiDLL;
	if( (dwReturnCode = LoadHsapiDll(&hHsapiDLL, szPalmDesktopDir, szSupportDllDirectory)) != 0 )
		return(dwReturnCode);
		
	// Shutdown the HotSync Process if it is running
	if( (bHotSyncRunning=IsHotSyncRunning(hHsapiDLL)) )
	{
		// Check for any synchronizations in progress
		if( IsHotSyncInProgress(hHsapiDLL) )
			return PCI_ERR_HOTSYNC_IN_PROGRESS;
			
		ShutdownHotSync(hHsapiDLL);
	}
	
	// Actually uninstall the conduit
	dwReturnCode = (*lpfnCmRemoveConduitByCreatorID)(pCreator);
	
	// Re-start HotSync if it was running before
	if( bHotSyncRunning )
		StartHotSync(hHsapiDLL);
		
	if( dwReturnCode < 0 ) return dwReturnCode;
	else return(0);
}



/////////////////////////////////////////////////////////////////////////////
// EXTERN
/////////////////////////////////////////////////////////////////////////////
//
//	Function:		InstallConduit()
//
//	Description:
//		Installs a conduit on local system using Conduit Manager API.
//
//	Parameters (10):
//
//		szSupportDllDirectory	- directory containing any Palm Support dll's		
//								  (ie - CondMgr.dll, HSAPI.dll, UserData...)	
//	    pCreator				- creator id in string form.
//		szInstallDirectory		- location where the installer places the
//								  conduit dll.
//		pConduit				- Conduit dll filename. Just the basename,
//								  not the full path.
//		pDirectory				- conduit data directory.
//		pFile					- conduit data file name.
//		pRemoteDB				- handheld database name.
//		pName					- conduit display name.
//		dwPriority				- conduit priority.
//		pInfo					- conduit information.
//
//	Returns:
//		0				- success.
//		non-zero		- error.
//
/////////////////////////////////////////////////////////////////////////////
INT32 WINAPI InstallConduit(LPSTR szSupportDllDirectory, const char *pCreator,
	LPSTR szInstallDirectory, const TCHAR *pConduit, const TCHAR *pDirectory,
	const TCHAR *pFile, const TCHAR *pRemoteDB, const TCHAR *pName,
	DWORD dwPriority, const TCHAR *pInfo )
{ 
	INT32	dwReturnCode;
	BOOL	bHotSyncRunning = FALSE;

	_ASSERT(szInstallDirectory);
	_ASSERT(*szInstallDirectory);
	_ASSERT(szSupportDllDirectory);
	_ASSERT(*szSupportDllDirectory);

		
	// Prepare the full path of the conduit.
	// Applications should not place conduits in the Palm Desktop directory.
	// The Palm Desktop installer only manages the Palm Desktop conduits.
	TCHAR	szConduitPath[_MAX_PATH];
	strncpy(szConduitPath, szInstallDirectory, _MAX_PATH);
	strncat(szConduitPath, PCI_DIRECTORY_SEPARATOR_STR, _MAX_PATH-strlen(szConduitPath));
	strncat(szConduitPath, pConduit, _MAX_PATH-strlen(szConduitPath));
	
	// Make sure the conduit dll exists
	struct _finddata_t dll_file;
	long hFile;
	if( (hFile = _findfirst( szConduitPath, &dll_file )) == -1L )
		return PCI_ERR_CONDUIT_NOT_FOUND;
	
	// Load the Conduit Manager DLL.
	HINSTANCE hConduitManagerDLL;
	if( (dwReturnCode = LoadConduitManagerDll(&hConduitManagerDLL, szSupportDllDirectory)) != 0 )
		return(dwReturnCode);
	
	// Prepare to install the conduit using Conduit Manager functions
	CmInstallCreatorPtr	lpfnCmInstallCreator;
	lpfnCmInstallCreator = (CmInstallCreatorPtr) GetProcAddress(hConduitManagerDLL, "CmInstallCreator");
	CmSetCreatorModulePtr	lpfnCmSetCreatorModule;
	lpfnCmSetCreatorModule = (CmSetCreatorModulePtr) GetProcAddress(hConduitManagerDLL, "CmSetCreatorModule");
	CmSetCreatorDirectoryPtr	lpfnCmSetCreatorDirectory;
	lpfnCmSetCreatorDirectory = (CmSetCreatorDirectoryPtr) GetProcAddress(hConduitManagerDLL, "CmSetCreatorDirectory");
	CmSetCreatorFilePtr	lpfnCmSetCreatorFile;
	lpfnCmSetCreatorFile = (CmSetCreatorFilePtr) GetProcAddress(hConduitManagerDLL, "CmSetCreatorFile");
	CmSetCreatorRemotePtr	lpfnCmSetCreatorRemote;
	lpfnCmSetCreatorRemote = (CmSetCreatorRemotePtr) GetProcAddress(hConduitManagerDLL, "CmSetCreatorRemote");
	CmSetCreatorNamePtr	lpfnCmSetCreatorName;
	lpfnCmSetCreatorName = (CmSetCreatorNamePtr) GetProcAddress(hConduitManagerDLL, "CmSetCreatorName");
	CmSetCreatorPriorityPtr	lpfnCmSetCreatorPriority;
	lpfnCmSetCreatorPriority = (CmSetCreatorPriorityPtr) GetProcAddress(hConduitManagerDLL, "CmSetCreatorPriority");
	CmSetCreatorInfoPtr	lpfnCmSetCreatorInfo;
	lpfnCmSetCreatorInfo = (CmSetCreatorInfoPtr) GetProcAddress(hConduitManagerDLL, "CmSetCreatorInfo");
	CmSetCreatorIntegratePtr	lpfnCmSetCreatorIntegrate;
	lpfnCmSetCreatorIntegrate = (CmSetCreatorIntegratePtr) GetProcAddress(hConduitManagerDLL, "CmSetCreatorIntegrate");
	
	if( (lpfnCmInstallCreator == NULL) 
		|| (lpfnCmSetCreatorModule == NULL)
		|| (lpfnCmSetCreatorDirectory == NULL)
		|| (lpfnCmSetCreatorFile == NULL)
		|| (lpfnCmSetCreatorRemote == NULL)
		|| (lpfnCmSetCreatorName == NULL)
		|| (lpfnCmSetCreatorPriority == NULL)
		|| (lpfnCmSetCreatorInfo == NULL)
		|| (lpfnCmSetCreatorIntegrate == NULL)
		)
	{
		// Return error code.
		return(PCI_ERR_INVALID_CONDMGR);
	}
	
	// Get the Palm Desktop Installation directory
	TCHAR	szPalmDesktopDir[_MAX_PATH];
	int	size=_MAX_PATH;
	if( (dwReturnCode=GetPalmDesktopInstallDirectory(hConduitManagerDLL, szPalmDesktopDir, &size)) != 0 )
		return dwReturnCode;
	
	// Load the HSAPI DLL.
	HINSTANCE hHsapiDLL;
	if( (dwReturnCode = LoadHsapiDll(&hHsapiDLL, szPalmDesktopDir, szSupportDllDirectory)) != 0 )
		return(dwReturnCode);
		
	// Shutdown the HotSync Process if it is running
	if( (bHotSyncRunning=IsHotSyncRunning(hHsapiDLL)) )
	{
		// Check for any synchronizations in progress
		if( IsHotSyncInProgress(hHsapiDLL) )
			return PCI_ERR_HOTSYNC_IN_PROGRESS;
			
		ShutdownHotSync(hHsapiDLL);
	}
	
	// Actually install the conduit as an Application Conduit
	dwReturnCode = (*lpfnCmInstallCreator)(pCreator, CONDUIT_APPLICATION);
	if( dwReturnCode == 0 )
	{
		dwReturnCode = (*lpfnCmSetCreatorName)(pCreator, szConduitPath);
		if( dwReturnCode != 0 ) return dwReturnCode;
		dwReturnCode = (*lpfnCmSetCreatorDirectory)(pCreator, pDirectory);
		if( dwReturnCode != 0 ) return dwReturnCode;
		dwReturnCode = (*lpfnCmSetCreatorFile)(pCreator, pFile);
		if( dwReturnCode != 0 ) return dwReturnCode;
		dwReturnCode = (*lpfnCmSetCreatorRemote)(pCreator, pRemoteDB);
		if( dwReturnCode != 0 ) return dwReturnCode;
		dwReturnCode = (*lpfnCmSetCreatorModule)(pCreator, pName);
		if( dwReturnCode != 0 ) return dwReturnCode;
		dwReturnCode = (*lpfnCmSetCreatorPriority)(pCreator, dwPriority);
		if( dwReturnCode != 0 ) return dwReturnCode;
		dwReturnCode = (*lpfnCmSetCreatorInfo)(pCreator, pInfo);
		if( dwReturnCode != 0 ) return dwReturnCode;
		// Applications should always set Integrate to 0
		dwReturnCode = (*lpfnCmSetCreatorIntegrate)(pCreator, (DWORD)0);
	}
	
	// Re-start HotSync if it was running before
	if( bHotSyncRunning )
		StartHotSync(hHsapiDLL);
		
	return(dwReturnCode);
}


/////////////////////////////////////////////////////////////////////////////
// EXTERN
/////////////////////////////////////////////////////////////////////////////
//
//	Function:		DecodePCIErrorCode()
//
//	Description:
//		Displays a Message Box detailing  the InstallConduit return code.
//
//	Parameters (1):
//
//		dwResult		- the result of the InstallConduit routine.
//
//	Returns:
//		void.
//
/////////////////////////////////////////////////////////////////////////////
VOID WINAPI DecodePCIErrorCode(DWORD dwResult)
{
	switch( dwResult )
	{
	case	0:
		// Success !
		break;

	case	PCI_ERR_LOADING_TEMP_CONDMGR:
		::MessageBox(GetFocus(), "Error: CondMgr.dll not included with installer!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	PCI_ERR_INVALID_CONDMGR:
		::MessageBox(GetFocus(), "Error: invalid CondMgr.dll!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	PCI_ERR_HSAPI_NOT_FOUND:
		::MessageBox(GetFocus(), "Error: HSAPI.dll not found!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	PCI_ERR_CONDUIT_NOT_FOUND:
		::MessageBox(GetFocus(), "Error: conduit dll not found!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	PCI_ERR_HOTSYNC_IN_PROGRESS:
		::MessageBox(GetFocus(), "Error: HotSync in progress!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	ERR_CONDUIT_MGR:
		::MessageBox(GetFocus(), "Error: Unknown Conduit Manager error!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	ERR_INDEX_OUT_OF_RANGE:
		::MessageBox(GetFocus(), "Error: Conduit Manager, index out of range!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	ERR_UNABLE_TO_DELETE:
		::MessageBox(GetFocus(), "Error: Conduit Manager, unable to delete conduit!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	ERR_NO_CONDUIT:
		::MessageBox(GetFocus(), "Error: Conduit Manager, conduit not previously installed!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	ERR_NO_MEMORY:
		::MessageBox(GetFocus(), "Error: Conduit Manager, out of memory!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	ERR_CREATORID_ALREADY_IN_USE:
		::MessageBox(GetFocus(), "Error: Conduit Manager, creator id already in use!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	ERR_REGISTRY_ACCESS:
		::MessageBox(GetFocus(), "Error: Conduit Manager, unable to access registry!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	ERR_UNABLE_TO_CREATE_CONDUIT:
		::MessageBox(GetFocus(), "Error: Conduit Manager, unable to create conduit!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	ERR_UNABLE_TO_SET_CONDUIT_VALUE:
		::MessageBox(GetFocus(), "Error: Conduit Manager, unable to set conduit value!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	ERR_INVALID_HANDLE:
		::MessageBox(GetFocus(), "Error: Conduit Manager, invalid handle passed!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	ERR_BUFFER_TOO_SMALL:
		::MessageBox(GetFocus(), "Error: Conduit Manager, buffer too small!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	ERR_VALUE_NOT_FOUND:
		::MessageBox(GetFocus(), "Error: Conduit Manager, value not found!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	ERR_INVALID_CREATOR_ID:
		::MessageBox(GetFocus(), "Error: Conduit Manager, invalid creator id!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	ERR_INVALID_POINTER:
		::MessageBox(GetFocus(), "Error: Conduit Manager, invalid pointer passed!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	ERR_UNABLE_TO_INSTALL_OLD:
		::MessageBox(GetFocus(), "Error: Conduit Manager, unable to install conduit!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	ERR_INVALID_CONDUIT_TYPE:
		::MessageBox(GetFocus(), "Error: Conduit Manager, invalid conduit type!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	ERR_INVALID_COM_PORT_TYPE:
		::MessageBox(GetFocus(), "Error: Conduit Manager, invalid COM port type!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	case	ERR_NO_LONGER_SUPPORTED:
		::MessageBox(GetFocus(), "Error: Conduit Manager, function no longer supported!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	
	default:
		::MessageBox(GetFocus(), "Unknown error occurred!\n" , _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
		break;
	}

}

/////////////////////////////////////////////////////////////////////////////
// EXTERN
/////////////////////////////////////////////////////////////////////////////
//
//	Function:		InstallShieldInstallConduit()
//
//	Description:
//		Entry Point for InstallShield, stub for InstallConduit 
//		
//
//	Parameters (5): (Prototype defined by InstallShield)
//
//		hISW				- Handle the InstallShield Window		
//		szSourceDirectory	- <SRCDIR> placeholder from InstallShield
//		szSupportDirectory  - <SUPPORTDIR> placeholder from InstallShield
//		szInstallDirectory  - <INSTALLDIR> placeholder from InstallShield
//		szDatabaseDirectory - <DATABASEDIR> placeholder from InstallShield
//
//	Returns:
//		char
//
/////////////////////////////////////////////////////////////////////////////
CHAR WINAPI InstallShieldInstallConduit(HWND hISW, LPSTR szSourceDirectory, LPSTR szSupportDirectory, LPSTR szInstallDirectory, LPSTR szDatabaseDirectory)
{
	INT32	dwResult=0;

	/////////////////////////////////////////////////////////////////////////
	// Change the values of the following (8) parameters to support your conduit.
	// Verify these parameters using the command line installer (PCI.exe)   
	/////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////// 
	// NOTE: The parameters are defaulted for a Memo Pad example
	/////////////////////////////////////////////////////////////////////////

	// creator ID in string form
	const char *pCreator = "BgSc";
	// conduit dll filename. Only the basename, not the full path. 
	const char *pConduit = "BMILog10.dll";
	// conduit data directory	
	const char *pDirectory = "BMILog";
	// conduit data file name
	const char *pFile = "BgSc_BMILogData.dat";
	// handheld database name
	const char *pRemoteDB = "BgSc_BMILogData";
	// conduit display name
	const char *pName = "BMILog";
	// conduit priority
	DWORD dwPriority = 2;
	// conduit information
	const char *pInfo = "Conduit for BMILog";

	// Assume that we should overwrite (uninstall) an existing conduit
	dwResult = UninstallConduit(szSupportDirectory,pCreator);
	
	// Ignore the case of no previous conduit installed
	if (dwResult == ERR_NO_CONDUIT) 
		dwResult = 0;

	if (dwResult==0)
	{
		// Install the conduit
		dwResult = InstallConduit(szSupportDirectory,pCreator,szInstallDirectory,pConduit,pDirectory,pFile,pRemoteDB,pName,dwPriority,pInfo);
	}

	DecodePCIErrorCode(dwResult);

	// If we return 0 InstallShield will exit, therefore return 1	
	return 1;
}


/////////////////////////////////////////////////////////////////////////////
// EXTERN
/////////////////////////////////////////////////////////////////////////////
//
//	Function:		InstallShieldUninstallConduit()
//
//	Description:
//		Entry Point for InstallShield, stub for UninstallConduit 
//		
//
//	Parameters (5): (Prototype defined by InstallShield)
//
//		hISW				- Handle the InstallShield Window		
//		szSourceDirectory	- <SRCDIR> placeholder from InstallShield
//		szSupportDirectory  - <SUPPORTDIR> placeholder from InstallShield
//		szInstallDirectory  - <INSTALLDIR> placeholder from InstallShield
//		szDatabaseDirectory - <DATABASEDIR> placeholder from InstallShield
//
//	Returns:
//		char
//
/////////////////////////////////////////////////////////////////////////////
CHAR WINAPI InstallShieldUninstallConduit(HWND hISW, LPSTR szSourceDirectory, LPSTR szSupportDirectory, LPSTR szInstallDirectory, LPSTR szDatabaseDirectory)
{
	INT32 dwResult=0;
	
	// To support InstallShield Uninstall, implement a call to UninstallConduit()

	// If we return 0 InstallShield will exit, therefore return 1
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
// EXTERN
/////////////////////////////////////////////////////////////////////////////
//
//	Function:		GetPalmDesktopPath()
//
//	Description:
//		Return the path to Palm Desktop.
//		
//
//	Parameters (2): 
//
//		szSupportDllDirectory	- directory containing any Palm Support dll's		
//								  (ie - CondMgr.dll, HSAPI.dll, UserData...)	
//		desktopPath - where to store path.
//		pathLen - max bytes to store.
//
//	Returns:
//		Error code.
//
/////////////////////////////////////////////////////////////////////////////
INT32 WINAPI GetPalmDesktopPath(LPSTR szSupportDllDirectory, TCHAR *desktopPath, int *pathLen)
{ 
	INT32 dwReturnCode;
	HINSTANCE hConduitManagerDLL;

	_ASSERT(szSupportDllDirectory);
	_ASSERT(*szSupportDllDirectory);
	
	// Load the Conduit Manager DLL.
	if ((dwReturnCode=LoadConduitManagerDll(&hConduitManagerDLL, szSupportDllDirectory))!= 0)
		return (dwReturnCode);
	
	// Prepare to install the conduit using Conduit Manager functions
	CmInstallCreatorPtr	lpfnCmInstallCreator;
	lpfnCmInstallCreator = (CmInstallCreatorPtr) GetProcAddress(hConduitManagerDLL, "CmInstallCreator");
	CmSetCreatorModulePtr	lpfnCmSetCreatorModule;
	lpfnCmSetCreatorModule = (CmSetCreatorModulePtr) GetProcAddress(hConduitManagerDLL, "CmSetCreatorModule");
	CmSetCreatorDirectoryPtr	lpfnCmSetCreatorDirectory;
	lpfnCmSetCreatorDirectory = (CmSetCreatorDirectoryPtr) GetProcAddress(hConduitManagerDLL, "CmSetCreatorDirectory");
	CmSetCreatorFilePtr	lpfnCmSetCreatorFile;
	lpfnCmSetCreatorFile = (CmSetCreatorFilePtr) GetProcAddress(hConduitManagerDLL, "CmSetCreatorFile");
	CmSetCreatorRemotePtr	lpfnCmSetCreatorRemote;
	lpfnCmSetCreatorRemote = (CmSetCreatorRemotePtr) GetProcAddress(hConduitManagerDLL, "CmSetCreatorRemote");
	CmSetCreatorNamePtr	lpfnCmSetCreatorName;
	lpfnCmSetCreatorName = (CmSetCreatorNamePtr) GetProcAddress(hConduitManagerDLL, "CmSetCreatorName");
	CmSetCreatorPriorityPtr	lpfnCmSetCreatorPriority;
	lpfnCmSetCreatorPriority = (CmSetCreatorPriorityPtr) GetProcAddress(hConduitManagerDLL, "CmSetCreatorPriority");
	CmSetCreatorInfoPtr	lpfnCmSetCreatorInfo;
	lpfnCmSetCreatorInfo = (CmSetCreatorInfoPtr) GetProcAddress(hConduitManagerDLL, "CmSetCreatorInfo");
	CmSetCreatorIntegratePtr	lpfnCmSetCreatorIntegrate;
	lpfnCmSetCreatorIntegrate = (CmSetCreatorIntegratePtr) GetProcAddress(hConduitManagerDLL, "CmSetCreatorIntegrate");
	
	if( (lpfnCmInstallCreator == NULL) 
		|| (lpfnCmSetCreatorModule == NULL)
		|| (lpfnCmSetCreatorDirectory == NULL)
		|| (lpfnCmSetCreatorFile == NULL)
		|| (lpfnCmSetCreatorRemote == NULL)
		|| (lpfnCmSetCreatorName == NULL)
		|| (lpfnCmSetCreatorPriority == NULL)
		|| (lpfnCmSetCreatorInfo == NULL)
		|| (lpfnCmSetCreatorIntegrate == NULL)
		)
	{
		// Return error code.
		return (PCI_ERR_INVALID_CONDMGR);
	}
	
	// Get the Palm Desktop Installation directory
	if( (dwReturnCode=GetPalmDesktopInstallDirectory(hConduitManagerDLL, desktopPath, pathLen)) != 0 )
		return dwReturnCode;

	return(dwReturnCode);
}

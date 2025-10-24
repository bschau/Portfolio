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
// FILE:     		CondInst.h
//
// DESCRIPTION:		
//		Header file for CondInst.dll.
//
////////////////////////////////////////////////////////////////////////////

//----- Defines ---------------------------------------------------------------

#define	PCI_ERR_BASE					-1200
#define	PCI_ERR_LOADING_TEMP_CONDMGR	(PCI_ERR_BASE-0x01)
#define	PCI_ERR_INVALID_CONDMGR			(PCI_ERR_BASE-0x02)
#define	PCI_ERR_HSAPI_NOT_FOUND			(PCI_ERR_BASE-0x03)
#define	PCI_ERR_CONDUIT_NOT_FOUND		(PCI_ERR_BASE-0x04)
#define	PCI_ERR_HOTSYNC_IN_PROGRESS		(PCI_ERR_BASE-0x05)

#define	PCI_CONDMGR_FILENAME		"CondMgr.dll"
#define	PCI_HSAPI_FILENAME			"HsApi.dll"
#define	PCI_DIRECTORY_SEPARATOR		'\\'
#define	PCI_DIRECTORY_SEPARATOR_STR	"\\"
#define	PCI_EXECUTABLE_EXTENSION	".exe"
#define	PCI_HOTSYNC_MAX_WAIT		30			// wait for HotSync to start/stop in seconds

#ifdef __cplusplus
extern "C" {
#endif 

//----- Functions -------------------------------------------------------------

extern INT32 WINAPI InstallConduit( LPSTR szSupportDllDirectory, const char *pCreator,
	LPSTR szInstallDirectory, const TCHAR *pConduit, const TCHAR *pDirectory,
	const TCHAR *pFile, const TCHAR *pRemoteDB, const TCHAR *pName,
	DWORD dwPriority, const TCHAR *pInfo );

extern INT32 WINAPI UninstallConduit( LPSTR szSupportDllDirectory, const char *pCreator );

extern void WINAPI DecodePCIErrorCode (DWORD dwResult);

extern char WINAPI InstallShieldInstallConduit(HWND hISW, LPSTR szSourceDirectory, LPSTR szSupportDirectory, LPSTR szInstallDirectory, LPSTR szDatabaseDirectory);

extern char WINAPI InstallShieldUninstallConduit(HWND hISW, LPSTR szSourceDirectory, LPSTR szSupportDirectory, LPSTR szInstallDirectory, LPSTR szDatabaseDirectory);

extern INT32 WINAPI GetPalmDesktopPath(LPSTR szSupportDllDirectory, TCHAR *desktopPath, int *pathLen);

#ifdef __cplusplus
}
#endif 
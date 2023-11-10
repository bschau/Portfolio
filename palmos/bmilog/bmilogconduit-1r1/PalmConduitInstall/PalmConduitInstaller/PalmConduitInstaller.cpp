// PalmConduitInstaller.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <stdio.h>
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
// FILE:     		PalmConduitInstaller.cpp
//
// DESCRIPTION:		
//	This file implements automatic install of HotSync(R) technology
//	conduits to a target system. This is the main program and implements the
//	following:
//
//		Usage:	PCI [/o] [/u] [/i] arguments
//				where <arguments> is a space-separated list of:
//					<support_dir>	- directory containing support dll's
//										(CondMgr, HSAPI)
//					<creator>		- creator id
//					<install_dir>	- installation directory, containing conduit
//										to be installed
//					<conduit_dll>	- filename of conduit dll
//					<data_dir>		- conduit data directory
//					<data_filename>	- conduit data filename
//					<remote_db>		- remote database name
//					<conduit_name>  - display name of conduit
//					<priority>		- conduit priority (0-4)
//					<info>			- conduit information
//
//		/i switch:
//					<support_dir>	- directory containing support dll's
//										(CondMgr, HSAPI)
//					<program>		- program to add to InstApp
//
//		When the /u option is specified, only <support_dir> and <creator arguments are required.
//
//		The files PCI.exe and CondInst.dll built by this project should be shipped with your installer
//		and located in the same directory to run properly. To test using the examples below, copy PCI.exe
//		and CondInst.dll to a temporary directory like C:\Temp, copy your version of CondMgr.dll to
//		C:\Windows\Temp and execute the examples.
//
//		Examples:
//			
//		Install the memopad conduit, overwriting any existing installation:
//			PCI /o "C:\Windows\Temp" "memo" "C:\Program Files\MyDirectory" "memcn30.dll" "memopad" "memopad.dat" "MemoDB" "Memo Pad (PCI)" 2 "PCI Installed"
//
//			The conduit should be placed in the following directory by your installer: "C:\Program Files\MyDirectory".
//			The CondMgr.dll and HSAPI.dll files should be placed in the following temporary directory by your
//			installer: "C:\Windows\Temp".
//
//			PLEASE NOTE: YOUR INSTALLER SHOULD REMOVE THE TEMPORARY CONDMGR.DLL AND HSAPI>DLL FILES INSTALLED
//			AFTER COMPLETION. WITH MOST INSTALLATION PACKAGES, THESE FILES ARE INCLUDED AS TEMPORARY INSTALLATION
//			FILES. THE INSTALLER USUALLY PLACES THESE IN A TEMPORARY DIRECTORY CHOSEN BY THE INSTALLER. YOU THEN
//			PASS THIS VALUE TO PCI USING An INSTALLER KEYWORD. FOR INSTALLSHIELD, THE KEYWORD IS <SUPPORT_DIR>.
//			THE INSTALLER WILL CLEAN UP THESE TEMPORARY FILES FOR YOU.
//
//		Uninstall the memopad conduit:
//			PCI /u "C:\Windows\Temp" "memo"
//
//		Install Palm OS application (= queue Palm OS application)
//			PCI /i "C:\Windows\Temp" "application"
//			
//
////////////////////////////////////////////////////////////////////////////
#include <io.h>
#include <direct.h>
#include <crtdbg.h>

#include "CondInst.h"
#include "CondMgr.h"

#define	_THIS_FILE					"PCI.exe"
#define	PCI_NUMBER_OF_PARAMS_INSTALL		(11)
#define	PCI_NUMBER_OF_PARAMS_UNINSTALL		(3)
#define PCI_NUMBER_OF_PARAMS_INSTAPP		(3)
#define PCI_INSTAPP_FILENAME				"Instapp.exe"

void usage(char *msg)
{
	char	szMessage[4096];
	
	// Display usage dialog.
	sprintf( szMessage, "%s\n\n%s", msg, "usage: PCI [/uo] <arguments>\n\nwhere <arguments> is a space-separated list of:\n\t1. <support_dir>\t- directory containing support dll's (CondMgr, HSAPI)\n\t2. <creator>\t- creator id\n\t3. <install_dir>\t- directory of conduit to be installed\n\t4. <conduit_dll>\t- filename of conduit dll\n\t5. <data_dir>\t- conduit data directory\n\t6. <data_filename>\t- conduit data filename\n\t7. <remote_db>\t- remote database name\n\t8. <conduit_name>\t- display name of conduit\n\t9. <priority>\t- conduit priority (0-4)\n\t10. <info>\t- conduit information\n\nOr, using the /i switch:\n\t1. <support_dir>\t- directory containing support dll's (CondMgr, HSAPI)\n\t2. <program>\t- program to install" );
	::MessageBox(GetFocus(), szMessage, _THIS_FILE, MB_OK | MB_ICONEXCLAMATION );
	exit(-1);
}





int main(int argc, char* argv[])
{
	INT32	dwResult=0;
	char	szMessage[512];
	BOOL	bOverwrite = FALSE;
	BOOL	bUninstall = FALSE;
	BOOL	bInstApp = FALSE;
	
	// Check for minimum number of arguments
	if( argc < PCI_NUMBER_OF_PARAMS_INSTAPP )
	{
		sprintf( szMessage, "wrong number of parameters...%d", argc-1 );
		usage( szMessage );
	}
	
	// Check for any command line switches
	while( *(argv[1]) == '/' )
	{
		if( strcmp(argv[1], "/o") == 0 )
			bOverwrite = TRUE;
		else if( strcmp(argv[1], "/u") == 0 )
			bUninstall = TRUE;
		else if( strcmp(argv[1], "/i") == 0 )
			bInstApp = TRUE;
		
		--argc;
		for( int i=1; i<argc; i++ )
			argv[i] = argv[i+1];
	}

	if (bOverwrite==FALSE && bUninstall==FALSE && bInstApp==FALSE)
		usage("don't know what to do ... specify /o /u or /i");

	if( bOverwrite )
	{
		// Check the input parameters
		if( argc != PCI_NUMBER_OF_PARAMS_INSTALL )
		{
			sprintf( szMessage, "wrong number of parameters...%d", argc-1 );
			usage( szMessage );
		}

		// Ensure install directory is not relative path.
		// (for example:  C:\My Directory, not .\My Directory)
		if( *(argv[1]+1) != ':' )
		{
			sprintf( szMessage, "parameter 1 (%s) must be full path (non-relative)", argv[1] );
			usage( szMessage );
		}
	
		// Ensure conduit dll filename is not full path. Should only be
		// the filename (for example: memcond.dll, not C:\Palm\memcond.dll)
		if( strchr(argv[4], '\\') )
		{
			sprintf( szMessage, "parameter 4 (%s) should not contain directory separators \"\\\"", argv[4] );
			usage( szMessage );
		}
	
		// Ensure priority is a digit from 0-4
		if( (strlen(argv[9]) != 1) || !(isdigit(*(argv[9]))) )
		{
			sprintf( szMessage, "parameter 9 (%s) must be 0-4, single digit", argv[10] );
			usage( szMessage );
		}
	}
	else if ( bUninstall )
	{
		// Check the input parameters
		if( argc < PCI_NUMBER_OF_PARAMS_UNINSTALL )
		{
			sprintf( szMessage, "wrong number of parameters...%d", argc-1 );
			usage( szMessage );
		}
	}
	else if ( bInstApp )
	{
		// Check the input parameters
		if( argc < PCI_NUMBER_OF_PARAMS_INSTAPP )
		{
			sprintf( szMessage, "wrong number of parameters...%d", argc-1 );
			usage( szMessage );
		}
	}
		
	if( bUninstall || bOverwrite )
	{
		// Uninstall the conduit
		dwResult = UninstallConduit(argv[1], argv[2]);

		// Ignore the case of /o flag and no previous conduit installed
		if( dwResult == ERR_NO_CONDUIT && bOverwrite )
			dwResult = 0;
	}

	if( bOverwrite && !bInstApp && (dwResult == 0) )
	{
		// Install the conduit
		dwResult = InstallConduit(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], atol(argv[9]), argv[10]);
	}

	if( bInstApp )
	{
		TCHAR	pDir[_MAX_PATH+_MAX_PATH];
		int	size=_MAX_PATH;

		if ((dwResult = GetPalmDesktopPath(argv[1], pDir, &size)) == 0)
		{
				// append data
			strncat(pDir, PCI_DIRECTORY_SEPARATOR_STR, (2*_MAX_PATH)-strlen(pDir));
			strncat(pDir, PCI_INSTAPP_FILENAME, (2*_MAX_PATH)-strlen(pDir));
			strncat(pDir, " ", (2*_MAX_PATH)-strlen(pDir));
			strncat(pDir, argv[2], (2*_MAX_PATH)-strlen(pDir));

			system(pDir);			
		}
	}

	DecodePCIErrorCode(dwResult);
	
	return 0;
}


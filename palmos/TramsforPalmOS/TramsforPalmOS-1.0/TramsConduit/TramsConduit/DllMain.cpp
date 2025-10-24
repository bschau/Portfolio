#include <windows.h>
#include <COMMCTRL.H>
#include <syncmgr.h>
#include <HSLog.h>
#include <UserData.h>
#include <shlobj.h>
#include "resource.h"
#include "TramsConduit.h"

HANDLE hLangInstance;
HANDLE hAppInstance;
extern HANDLE hLangInstance;
extern HANDLE hAppInstance;

long CALLBACK ConfigureDlgProc(HWND, UINT, WPARAM, LPARAM);
void LoadCfgDlgBitmaps(HWND);

static int iTerminationCount=0;

/*
 * DllMain
 *
 * Initialize DLL.
 */
extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason) {
		case DLL_PROCESS_ATTACH:
	        if (!iTerminationCount )
			{
				hAppInstance=hInstance;
				hLangInstance=hInstance;
			}
			iTerminationCount++;
			break;

		case DLL_PROCESS_DETACH:
			iTerminationCount--;
			break;
	}

	return 1;   // ok
}

/*
 * OpenConduit
 *
 * Open and run Conduit.
 */
ExportFunc long OpenConduit(PROGRESSFN pFn, CSyncProperties& rProps)
{
    long ret=-1;

	if (pFn)
	{
		TramsSync *ts=new TramsSync(rProps);

		if (ts) {
			ret=ts->Synchronize();

			delete ts;
		}
	}

	return ret;
}

/*
 * GetConduitName
 *
 * Return name of this conduit.
 */
ExportFunc long
GetConduitName(char *name, WORD nLen)
{
	long retval = -1;

    if (::LoadString((HINSTANCE)hLangInstance, IDS_CONDUIT_NAME, name, nLen))
        retval = 0;

    return retval;
}

/*
 * GetConduitVersion
 *
 * Return Version no. of conduit.
 */
ExportFunc DWORD
GetConduitVersion()
{
    return 0x0100;		// HiByte: Major, LoByte: Minor
}

/*
 * ConfigureConduit
 *
 * Run configuration dialog - only for Pre 3.0 HotSyncs.
 */
ExportFunc long
ConfigureConduit(CSyncPreference& pref)
{
	pref.m_SyncType=eDoNothing;		/* Please upgrade ... */
    pref.m_SyncPref=eTemporaryPreference;

	return 0;
}

/*
 * GetConduitInfo
 *
 * This function provides a way for a Conduit to provide info to the caller. 
 * In this version of the call, MFC Version, Conduit Name, and Default sync
 * action are the types of information this call will return.
 *
 *  -> infoType		enum specifying what info is being requested.
 *  -> pInArgs		this parameter may be null, except for the Conduit name enum,
 *					this value will be a ConduitRequestInfoType structure.
 * The following two parameters vary depending upon the info being requested. 
 * 
 * For enum eConduitName
 * <-  pOut			will be a pointer to a character buffer
 *  -> pdwOutSize	will be a pointer to a DWORD specifying the size of the character buffer.
 *
 * For enum eMfcVersion
 * <-  pOut			will be a pointer to a DWORD
 *  -> pdwOutSize	will be a pointer to a DWORD specifying the size of pOut.
 *
 * For enum eDefaultAction
 * <-  pOut			will be a pointer to a eSyncType variable
 *  -> pdwOutSize	will be a pointer to a DWORD specifying the size of pOut.
 *
 * Returns 0 if successful, !0 otherwise.
 */
ExportFunc long
GetConduitInfo(ConduitInfoEnum infoType, void *pInArgs, void *pOut, DWORD *pdwOutSize)
{
    if (!pOut)
        return CONDERR_INVALID_PTR;

	if (!pdwOutSize)
        return CONDERR_INVALID_OUTSIZE_PTR;

    switch (infoType) {
		case eConduitName:
            if (!pInArgs)
                return CONDERR_INVALID_INARGS_PTR;

            ConduitRequestInfoType *pInfo;
            pInfo = (ConduitRequestInfoType *)pInArgs;
            if ((pInfo->dwVersion != CONDUITREQUESTINFO_VERSION_1) ||
                (pInfo->dwSize != SZ_CONDUITREQUESTINFO))
                return CONDERR_INVALID_INARGS_STRUCT;
            
	        if (!::LoadString((HINSTANCE)hLangInstance, IDS_CONDUIT_NAME, (TCHAR*)pOut, *pdwOutSize))
                return CONDERR_CONDUIT_RESOURCE_FAILURE;
            break;
   
		case eDefaultAction:
            if (*pdwOutSize!=sizeof(eSyncTypes))
                return CONDERR_INVALID_BUFFER_SIZE;

            (*(eSyncTypes*)pOut)=eHHtoPC;
            break;

        case eMfcVersion:
            if (*pdwOutSize!=sizeof(DWORD))
                return CONDERR_INVALID_BUFFER_SIZE;

            (*(DWORD*)pOut)=MFC_NOT_USED;
            break;

        default:
            return CONDERR_UNSUPPORTED_CONDUITINFO_ENUM;
    }

    return 0;
}

/*
 * CfgConduit
 */
ExportFunc long
CfgConduit(ConduitCfgEnum cfgType, void *pArgs, DWORD *pdwArgsSize)
{
	long nRtn=-1;
	TCHAR szName[256];
    DWORD dwNamesize;
    ConduitRequestInfoType infoStruct;
    CfgConduitInfoType *pCfgInfo;

    dwNamesize=sizeof(szName);

    if (!pArgs)
        return CONDERR_INVALID_INARGS_PTR;

	if (!pdwArgsSize)
        return CONDERR_INVALID_ARGSSIZE_PTR;

	if (*pdwArgsSize!=SZ_CFGCONDUITINFO) 
        return CONDERR_INVALID_ARGSSIZE;

    if (cfgType!=eConfig1)
            return CONDERR_UNSUPPORTED_CFGCONDUIT_ENUM;

    pCfgInfo=(CfgConduitInfoType *)pArgs;
    if (pCfgInfo->dwVersion!=CFGCONDUITINFO_VERSION_1)
        return CONDERR_UNSUPPORTED_STRUCT_VERSION;

    infoStruct.dwVersion=CONDUITREQUESTINFO_VERSION_1;
    infoStruct.dwSize=SZ_CONDUITREQUESTINFO;
    infoStruct.dwCreatorId=pCfgInfo->dwCreatorId;
    infoStruct.dwUserId=pCfgInfo->dwUserId;
    strcpy_s(infoStruct.szUser, sizeof(infoStruct.szUser), pCfgInfo->szUser);
    if ((nRtn=GetConduitInfo(eConduitName, (void *)&infoStruct, (void *)szName, &dwNamesize))!=0)
        return nRtn;

    (void)DialogBoxParam((HINSTANCE)hLangInstance, MAKEINTRESOURCE(IDD_CONDUIT_ACTION), 
              GetForegroundWindow(), (DLGPROC)ConfigureDlgProc, (LPARAM)pCfgInfo);

	return 0;
}

/*
 * ConfigureDlgProc
 *
 * Configure dialog call back.
 */
long CALLBACK
ConfigureDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    static CfgConduitInfoType *pCfgInfo;
    TCHAR szPath[256];
	DWORD uid;

    switch (Message) {
        case WM_INITDIALOG:
            if (lParam != 0) {
				long dSize;

				LoadCfgDlgBitmaps(hWnd);

                pCfgInfo=(CfgConduitInfoType *)lParam;
                switch (pCfgInfo->syncTemporary){
                    case eHHtoPC:
                        CheckRadioButton(hWnd, IDC_RADIO_HHTOPC, IDC_RADIO_DONOTHING, IDC_RADIO_HHTOPC);
                        break;
                    case eDoNothing:
                    default:
                        CheckRadioButton(hWnd, IDC_RADIO_HHTOPC, IDC_RADIO_DONOTHING, IDC_RADIO_DONOTHING);
                        break;
                }

				if (UmGetIDFromName(pCfgInfo->szUser, &uid)==0) {
					dSize=sizeof(szPath);
					if (UmGetString(uid, "Trams", "DataPath", szPath, &dSize, "")==0)
						SetDlgItemText(hWnd, IDC_OUTPUT_PATH, szPath);
					else
						SetDlgItemText(hWnd, IDC_OUTPUT_PATH, "C:\\TRAMS\\mobile\\data");
				}
            }
            break;

        case WM_COMMAND:
            switch (wParam) {
                case IDC_RADIO_HHTOPC:
                case IDC_RADIO_DONOTHING:
                    CheckRadioButton(hWnd, IDC_RADIO_HHTOPC, IDC_RADIO_DONOTHING, wParam);
                    break;
                case IDCANCEL:
                    EndDialog(hWnd, 1);
                    return TRUE;
                case IDOK:
					if (IsDlgButtonChecked(hWnd, IDC_RADIO_HHTOPC))
                        pCfgInfo->syncNew=eHHtoPC;
                    else
                        pCfgInfo->syncNew=eDoNothing;

					if (IsDlgButtonChecked(hWnd, IDC_MAKEDEFAULT))
                        pCfgInfo->syncPref=ePermanentPreference;
                    else
                        pCfgInfo->syncPref=eTemporaryPreference;

					if (UmGetIDFromName(pCfgInfo->szUser, &uid)==0) {
						GetDlgItemText(hWnd, IDC_OUTPUT_PATH, szPath, sizeof(szPath));
						UmSetString(uid, "Trams", "DataPath", szPath);
					}

					EndDialog(hWnd, 0);
                    return TRUE;

				case IDC_BROWSE:
					{
						BROWSEINFO BrowseInfo;
						LPITEMIDLIST pList;
						char szBuffer[MAX_PATH];

						memset(&BrowseInfo, 0, sizeof(BrowseInfo));
		
						BrowseInfo.hwndOwner=hWnd;
						BrowseInfo.pszDisplayName=szBuffer;
						BrowseInfo.lpszTitle="Select Output Directory";
						BrowseInfo.ulFlags=BIF_RETURNONLYFSDIRS;
	
						if ((pList=SHBrowseForFolder(&BrowseInfo))!=NULL)
						{
							LPMALLOC pMalloc; 

							SHGetPathFromIDList(pList, szBuffer);
							SetDlgItemText(hWnd, IDC_OUTPUT_PATH, szBuffer);
							if (SUCCEEDED(SHGetMalloc(&pMalloc)))
								pMalloc->Free(pList);
						}
					}
					return true;
	
                default:
                    break;
            }
            break;

		case WM_SYSCOLORCHANGE:
			LoadCfgDlgBitmaps(hWnd);
			break;
        default:
            break;
    }
    return FALSE;
}

/*
 * LoadCfgDlgBitmaps
 *
 * Load the bitmaps.
 */
void
LoadCfgDlgBitmaps(HWND hDlg)
{
	COLORMAP		colorMap;
	HWND			hwndButton;
	HBITMAP			hBitmap, hOldBitmap;

	colorMap.to = GetSysColor(COLOR_BTNFACE);
	colorMap.from = RGB(192,192,192);

	// HH to PC 
	hBitmap=CreateMappedBitmap((HINSTANCE)hLangInstance, IDB_HHTOPC, 0, &colorMap, 1);
	if ((hwndButton=GetDlgItem(hDlg, IDC_HHTOPC))!=NULL)
	{
		if ((hOldBitmap=(HBITMAP)SendMessage(hwndButton, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE)hBitmap))!=NULL)
			DeleteObject((HGDIOBJ)hOldBitmap);
	}

	// Do Nothing 
	hBitmap=CreateMappedBitmap((HINSTANCE)hLangInstance, IDB_DONOTHING, 0, &colorMap, 1);
	if ((hwndButton=GetDlgItem(hDlg, IDC_DONOTHING))!= NULL)
	{
		if ((hOldBitmap=(HBITMAP)SendMessage(hwndButton, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE)hBitmap))!=NULL)
			DeleteObject((HGDIOBJ)hOldBitmap);
	}
}
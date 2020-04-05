/****************************************************************************
*
*  Logon.CPP
*  battle.net user interface functions
*
*  By Dan Liebgold
*
***/

#include "pch.h"
#pragma hdrstop

#include <tchar.h>

extern HWND ghWndUiMainParent;		

//===========================================================================
#define MAX_NAME_LEN		30
#define MAX_PASS_LEN		8

static LPBYTE           sgBkgBmp;
static LPBYTE           sgNewBkgBmp;
static LPBYTE           sgButtonBmp;
static SIZE             sgButtonSize;
static LPBYTE           sgMedBtnBmp;
static SNETUIDATAPTR    sgpInterfacedata;
static TCHAR            sgszNewName[256];
static TCHAR            sgszPassword[256];

#define PLAYSELECTSND                                           \
    if (sgpInterfacedata->soundcallback)                        \
    	sgpInterfacedata->soundcallback(PROVIDERID,             \
    	                                SNET_SND_SELECTITEM,    \
    	                                0)


//===========================================================================
static void NewLoadArtwork(HWND window, SNETGETARTPROC artcallback) {
    int btn_ids[] = {
        IDOK,
        IDCANCEL,
        0
    };

    SIZE bgSize;
    
	UiLoadArtwork(
			artcallback,
			window,
			NULL,
			SNET_ART_BATTLE_NEW_ACCOUNT_BKG,
			TEXT(""),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,	
			TRUE,
			FALSE,
			&sgNewBkgBmp,
			&bgSize);

    SDlgSetControlBitmaps(window,
                          btn_ids,
                          NULL,
                          sgButtonBmp,
                          &sgButtonSize,
                          SDLG_ADJUST_VERTICAL);
}

//===========================================================================
static BOOL CheckPassword(HWND window) {
    TCHAR szPass2[256];

    SendMessage(GetDlgItem(window,IDC_PASS1),
                WM_GETTEXT,
                (WPARAM)sizeof(sgszPassword),
                (LPARAM)sgszPassword);
    SendMessage(GetDlgItem(window,IDC_PASS2),
                WM_GETTEXT,
                (WPARAM)sizeof(szPass2),
                (LPARAM)szPass2);
                
    if (_tcscmp(sgszPassword,szPass2)) return FALSE;
    return TRUE;
}

//===========================================================================
static BOOL CALLBACK NewDialogProc(HWND   window,
                                   UINT   message,
                                   WPARAM wparam,
                                   LPARAM lparam)
{
    switch (message) {
        case WM_COMMAND:
			switch (LOWORD(wparam)) {
			    case IDOK: {
			        SendMessage(GetDlgItem(window,IDC_NAME),
			                    WM_GETTEXT,
			                    (WPARAM)sizeof(sgszNewName),
			                    (LPARAM)sgszNewName);                    

                    PLAYSELECTSND;

			        if (CheckPassword(window)) {
    			        SDlgEndDialog(window,TRUE);
    			        return 0;
			        }

                    char szPassErr[256];
                    LoadString(global_hinstance,
                               IDS_PASSWORD_MISMATCH,
                               szPassErr,
                               sizeof(szPassErr));

                    UiMessageBox(sgpInterfacedata->messageboxcallback,
                                 window,
                                 szPassErr,
                                 "",
                                 MB_OK|MB_ICONWARNING);
			        break;
			    }

			    case IDCANCEL:
    			    PLAYSELECTSND;
			        SDlgEndDialog(window,FALSE);
			        break;


                case IDC_PASS2:
                    if (HIWORD(wparam) == EN_CHANGE) {
                        int len = 0;
                        len += SendMessage(GetDlgItem(window, IDC_NAME),
                                           WM_GETTEXTLENGTH,
                                           0,0);
                        len += SendMessage(GetDlgItem(window, IDC_PASS1),
                                           WM_GETTEXTLENGTH,
                                           0,0);
                        len += SendMessage((HWND)lparam,
                                           WM_GETTEXTLENGTH,
                                           0,0);
			        	EnableWindow(GetDlgItem(window, IDOK), len);
                    }
                    break;

			}
			break;
			
        case WM_DESTROY:
            if (sgNewBkgBmp) {
                FREE(sgNewBkgBmp);
                sgNewBkgBmp = NULL;
            }
            break;

        case WM_INITDIALOG:
            NewLoadArtwork(window, sgpInterfacedata->artcallback);
			HWND name = GetDlgItem(window, IDC_NAME);
			SetFocus(name);
			SendMessage(name, EM_LIMITTEXT, MAX_NAME_LEN - 1, 0);

			EnableWindow(GetDlgItem(window, IDOK), FALSE);

            return 0;            
    }
	return SDlgDefDialogProc(window,message,wparam,lparam);
}


//===========================================================================
static void DestroyArtwork (HWND window) {
	if (sgBkgBmp) {
		FREE(sgBkgBmp); sgBkgBmp = NULL;
	}
	if (sgButtonBmp) {
		FREE(sgButtonBmp); sgButtonBmp = NULL;
	}
    if (sgMedBtnBmp) {
        FREE(sgMedBtnBmp); sgMedBtnBmp = NULL;
    }
}

//===========================================================================
static BOOL LoadArtwork (HWND window, SNETGETARTPROC artcallback) {
	int btn_ids[] = {
		IDOK,
		IDCANCEL,
		0
	};

	int static_txt[] = {
		IDC_TITLE,
		0
	};

	SIZE bgSize;
	SIZE medBtnSize;

	UiLoadArtwork(
			artcallback,
			window,
			NULL,
			SNET_ART_BATTLE_LOGIN_BKG,
			TEXT(""),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,	
			TRUE,
			FALSE,
			&sgBkgBmp,
			&bgSize);

	UiLoadArtwork(
			artcallback,
			NULL,
			NULL,
			SNET_ART_BUTTON_XSML,
			TEXT("Button"),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,	
			FALSE,
			FALSE,
			&sgButtonBmp,
			&sgButtonSize);

	UiLoadArtwork(
			artcallback,
			GetDlgItem(window,IDS_NEWACCOUNT),
			NULL,
			SNET_ART_BUTTON_MED,
			TEXT("Button"),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,	
			FALSE,
			FALSE,
			&sgMedBtnBmp,
			&medBtnSize);

    SDlgSetControlBitmaps (window,btn_ids,NULL,sgButtonBmp,&sgButtonSize,SDLG_ADJUST_VERTICAL);
	return 1;
}

//===========================================================================
static LPBYTE LogonGetNames(LPCTSTR key, LPCTSTR value, DWORD *pBytes) {
    LPBYTE buffer = NULL;
    DWORD bytesread = 0;
    DWORD datatype = REG_MULTI_SZ;
    DWORD dwNameLen = pBytes ? *pBytes : 0;
    LONG lError;
    for (int iter = 0; iter < 2; ++iter) {
        HKEY keyhandle;
        if (ERROR_SUCCESS == (lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                          key,
                                          0,
                                          KEY_READ,
                                          &keyhandle)))
        {
		    RegQueryValueEx(keyhandle,
                            value,
                            0,
                            &datatype,
                            buffer,
                            &bytesread);
            RegCloseKey(keyhandle);
            if (!buffer) buffer = (LPBYTE)ALLOCZERO(bytesread + dwNameLen + 2);
        }
        else {
            
        }
    }
    if (pBytes) *pBytes = bytesread;
    return buffer;
}

//===========================================================================
static void LogonRememberName(LPCSTR szName) {
	TCHAR key[MAX_PATH];
    DWORD dwNameLen = _tcslen(szName);
    BOOL success = 0;

	SRegGetBaseKey(SREG_FLAG_BATTLENET,key,MAX_PATH);
	_tcscat(key,TEXT("Characters"));
    TCHAR valuename[MAX_PATH] = TEXT("Names");

    DWORD bytesread = _tcslen(szName);
    LPBYTE buffer = LogonGetNames(key,valuename,&bytesread);

    if (!buffer) {
        buffer = (LPBYTE)ALLOCZERO(dwNameLen + 2);
       *buffer = 0;
    }

    TCHAR *pszTemp = (TCHAR*)buffer,*pszName;
    while (*pszTemp) {
        pszName = pszTemp;
        if (!_tcscmp(szName,pszName))
            goto REMEMBER_EXIT; // name is already there
        pszTemp += _tcslen(pszName) + 1;
    }

    // Add the name to front of list
    memmove(buffer + dwNameLen + 1, buffer, bytesread);
    _tcscpy((TCHAR*)buffer,szName);
    *(buffer + dwNameLen) = 0;

    // Write the buffer back to the registry
    HKEY  keyhandle;
    DWORD disposition;
    if (!RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                        key,
                        0,
                        NULL,
                        REG_OPTION_NON_VOLATILE,
                        KEY_WRITE,
                        (LPSECURITY_ATTRIBUTES)NULL,
                        &keyhandle,
                        &disposition)) {
      success |= !RegSetValueEx(keyhandle,
                                valuename,
                                0,
                                REG_MULTI_SZ,
                                (const BYTE *)buffer,
                                bytesread + dwNameLen + 2);
      RegCloseKey(keyhandle);
    }
REMEMBER_EXIT:
    FREE(buffer);
}


//===========================================================================
static void LogonGetNames(HWND combo) {
	TCHAR key[MAX_PATH];

	SRegGetBaseKey(SREG_FLAG_BATTLENET,key,MAX_PATH);
	_tcscat(key,TEXT("Characters"));
    TCHAR valuename[MAX_PATH] = TEXT("Names");

    DWORD bytesread = 0;
    LPBYTE buffer = LogonGetNames(key,valuename,&bytesread);

    if (!buffer) return;
    
    TCHAR *pszName = (TCHAR*)buffer;
    while (*pszName) {
        SendMessage(combo,CB_ADDSTRING,0,(LPARAM)pszName);
        pszName += _tcslen(pszName) + 1;
    }
    
    // select the first name
    SendMessage(combo,CB_SETCURSEL,0,0);
    FREE(buffer);
}


//===========================================================================
BOOL CALLBACK LogonDialogProc (HWND   window,
                               UINT   message,
                               WPARAM wparam,
                               LPARAM lparam) {

	switch (message) {
		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDOK:
                    PLAYSELECTSND;

					TCHAR szName[MAX_NAME_LEN];
					TCHAR szPass[MAX_PASS_LEN];
					SendDlgItemMessage(window, IDC_COMBO_NAME, WM_GETTEXT, MAX_NAME_LEN, (LPARAM)(LPCSTR)szName);
					SendDlgItemMessage(window, IDC_EDIT_PASSWORD, WM_GETTEXT, MAX_PASS_LEN, (LPARAM)(LPCSTR)szPass);
                    
					DWORD errorcode;
					if (SrvLogon (szName,szPass, &errorcode)) {
						LogonRememberName(szName);
						SDlgEndDialog(window, 1);
						return 0;
					}

					SDlgEndDialog(window, 0);
					return 0;

				case IDCANCEL:
				    PLAYSELECTSND;
					SDlgEndDialog(window, 0);
					return 0;

                case IDS_NEWACCOUNT:
                    PLAYSELECTSND;
                    BOOL bRes;
                    bRes = SDlgDialogBox(global_hinstance,
        	                             TEXT("DIALOG_NEW_ACCOUNT"),
        	                             window,
        	                             (DLGPROC)NewDialogProc);
                    if (bRes) {
                        HWND combo = GetDlgItem(window,IDC_COMBO_NAME);
                        int nPos = SendMessage(combo,
                                               CB_ADDSTRING,
                                               0,
                                               (LPARAM)sgszNewName);
                        SendMessage(combo,CB_SETCURSEL,(WPARAM)nPos,0);
                        SendMessage(window,
                                    WM_COMMAND,
                                    MAKEWPARAM(IDC_COMBO_NAME,CBN_SELCHANGE),
                                    (LPARAM)combo);
                    }
                    return 0;

				case IDC_COMBO_NAME: 
					if (HIWORD(wparam) == CBN_EDITCHANGE) {
						EnableWindow(
							GetDlgItem(window, IDOK),
							(BOOL)SendMessage((HWND)LOWORD(lparam), WM_GETTEXTLENGTH, 0, 0));
					}
					return 0;
   	   		}
   	   		break;

		case WM_CTLCOLORSTATIC:
			if (GetWindowLong((HWND)lparam, GWL_ID) == IDC_TITLE) {
				SetTextColor((HDC) wparam, RGB(0xff, 0xff, 0x00));
				return (BOOL) GetStockObject(NULL_BRUSH);
			}
			break;

		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			SendMessage(SDrawGetFrameWindow(), message, wparam, lparam);
			break;

		case WM_DESTROY:
			ghWndUiMainParent = NULL;

			UiVidFadeOut(DEFAULT_STEPS*2);		// Note that VidFadeOut also erase the screen

			ShowCursor(FALSE);					// Leave cursor off
			UiVidFade(1,1);						// restore palette for parent window

		  	DestroyArtwork(window);
			break;
		

   		case WM_INITDIALOG:
            ghWndUiMainParent = window;

			sgpInterfacedata = (SNETUIDATAPTR) lparam;
			if (sgpInterfacedata->artcallback)
			    LoadArtwork(window, sgpInterfacedata->artcallback);
			EnableWindow(GetDlgItem(window, IDOK), FALSE);

			HWND namecombo = GetDlgItem(window, IDC_COMBO_NAME);
			SetFocus(namecombo);
			SendMessage(namecombo, CB_LIMITTEXT, MAX_NAME_LEN - 1, 0);

            LogonGetNames(namecombo);

			UiLoadCursors(window, sgpInterfacedata);

			UiVidFade(1,1);
		  	return 0;
	}
	return SDlgDefDialogProc(window,message,wparam,lparam);
}



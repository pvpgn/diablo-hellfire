/****************************************************************************
*
*  cr8game.CPP
*  BattleNet Chat Room
*
*  By Michael Morhaime
*
***/

#include "pch.h"
#include "stdio.h"
#include "uisnd.h"

#define MAX_STRING_LEN 256
#define NUM_DIFF_SETTINGS 3
#define NUM_DIFF_BTN_POSITIONS 2


//****************************************************************************
// Private Data
//****************************************************************************
static LPBYTE sgBackgroundBmp = NULL;
static LPBYTE sgButtonBmp     = NULL;
static LPBYTE sgDiffBtnBmp    = NULL;

static SIZE sgDiffBtnSize;		 // Holds size of individual button in pixels

static TGAMEDATA sgGameData;	// Holds game initialization data
static TGAMEINFO sgGameInfo;	// Holds game name and password

static LONG sglPrivate;
static LONG sglDifficulty;
static LONG sglRestricted;

extern int UiOkDialog(HWND parent, LPCSTR oktext, BOOL bErrFlag);

extern BOOL IsNastyName(LPCTSTR name);
extern BOOL InvalidChars(LPCTSTR name, LPCTSTR additionalbad);
extern DWORD GetPlayerCategory(const char *szDesc);		//### MM Diablo Patch #2  3/21/97

typedef struct _create_params {
	SNETCREATEDATAPTR   createdata;  
	SNETPROGRAMDATAPTR  programdata;  
	SNETPLAYERDATAPTR	playerdata;
	SNETUIDATAPTR       interfacedata;
	SNETVERSIONDATAPTR  versiondata;  
	DWORD              *playerid;   
} TCREATEPARAMS, PTCREATEPARAMS;

static TCREATEPARAMS sgCreateParams;

//===========================================================================
static void DestroyArtwork (HWND window) {
	if (sgBackgroundBmp) {
		FREE(sgBackgroundBmp);
		sgBackgroundBmp = NULL;
	}

	if (sgButtonBmp) {
		FREE(sgButtonBmp);
		sgButtonBmp = NULL;
	}

	if (sgDiffBtnBmp) {
		FREE(sgDiffBtnBmp);
		sgDiffBtnBmp = NULL;
	}
}

//===========================================================================
static BOOL LoadArtwork (HWND window) {
	int btn_ids[] = {
		IDOK,
		IDCANCEL,
		0
	};

	int diff_btn_ids[] = {
		IDC_RADIO_NORMAL,
		IDC_RADIO_NIGHTMARE,
		IDC_RADIO_HELL,
		0
	};


	SIZE btnbmpsize;
	SIZE bgSize;

	// For the background bitmap, "Dialog" is specified as the controltype to prevent 
	// LoadArtFile() from loading a palette, which would be done if we simply specified "".
	LoadArtFile(
		window,
		NULL,
		TEXT("Dialog"),
		SDLG_STYLE_ANY,
		SDLG_USAGE_BACKGROUND,
		TEXT("ui_art\\creat_bg.pcx"),
		&sgBackgroundBmp, 
		&bgSize);


	UiLoadBmpFile(
		TEXT("ui_art\\but_xsm.pcx"),
		&sgButtonBmp,
		&btnbmpsize
	);
	SDlgSetControlBitmaps (window, btn_ids, NULL, sgButtonBmp, &btnbmpsize, SDLG_ADJUST_VERTICAL);

	UiLoadBmpFile(
		TEXT("ui_art\\diffbtns.pcx"),
		&sgDiffBtnBmp,
		&sgDiffBtnSize
	);
	UiSetStaticBmp(window, IDC_STATIC_DIFF_DESC, sgBackgroundBmp, &bgSize);
	return 1;
}

//===========================================================================
static void DrawDiffBtn(HWND hWndBtn, int nDiff, BOOL bPressed) {
	TPBMP tpbmp;
	int nBtnBytes;

	tpbmp = (TPBMP) GetWindowLong(hWndBtn, GWL_USERDATA);
	if (tpbmp == NULL || sgDiffBtnBmp == NULL)
		return;

	// Calculate byte offset of each button in source bitmap
	nBtnBytes = sgDiffBtnSize.cx * tpbmp->datasize.cy;

	// now draw the appropriate button into
   	SBltROP3 (
   		tpbmp->data,
		sgDiffBtnBmp + ((nDiff*NUM_DIFF_BTN_POSITIONS)+bPressed)*nBtnBytes,
		tpbmp->datasize.cx,
		tpbmp->datasize.cy,
		tpbmp->datasize.cx,
		sgDiffBtnSize.cx,
		NULL,
		SRCCOPY
   	);

	// Make sure storm redraws this button
	InvalidateRect(hWndBtn, NULL, FALSE);
}

//===========================================================================
static void DestroyDiffBtns(HWND window) {
	HWND hWndBtn;
	RECT r;
	TPBMP tpbmp;

	for (int i=0; i<NUM_DIFF_SETTINGS; i++) {
		hWndBtn = GetDlgItem(window, i+IDC_RADIO_NORMAL);
		GetClientRect(hWndBtn, &r);


		tpbmp = (TPBMP) GetWindowLong(hWndBtn, GWL_USERDATA);
		SetWindowLong(window, GWL_USERDATA, (LONG) 0);
		if (tpbmp) {
			if (tpbmp->data) {
				FREE(tpbmp->data);
				tpbmp->data = 0;
			}
		
			FREE(tpbmp);
		}
	}
}

//===========================================================================
static void InitDiffBtns(HWND window) {
	HWND hWndBtn;
	RECT r;
	TPBMP tpbmp;

	for (int i=0; i<NUM_DIFF_SETTINGS; i++) {
		hWndBtn = GetDlgItem(window, i+IDC_RADIO_NORMAL);
		GetClientRect(hWndBtn, &r);

		tpbmp = (TPBMP) ALLOC(sizeof(TBMP));
		if (!tpbmp)
			break;
			
		tpbmp->data = (LPBYTE) ALLOC(r.right*r.bottom);
		tpbmp->datasize.cx = r.right;
		tpbmp->datasize.cy = r.bottom;
		tpbmp->userdata = 0;

		SetWindowLong(hWndBtn, GWL_USERDATA, (LONG) tpbmp);

		// set it so the button uses the bmp just created
		SDlgSetBitmap(
			hWndBtn,
			NULL,
			TEXT(""),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND | SDLG_USAGE_SELECTED | SDLG_USAGE_NORMAL,
			tpbmp->data,
			NULL,
			tpbmp->datasize.cx,
			tpbmp->datasize.cy
		);

		// Init the button image
		DrawDiffBtn(hWndBtn, i, FALSE);
	}
}

//===========================================================================
static void InitDefaultOptions(HWND hWnd) {
	// Init unused game flags to 0
	sglPrivate = 0;
	sglRestricted = 0;

	// Set to impossible values, so that help text is updated.
	sglDifficulty = -1;

	// Simulate button clicks, so that description text gets updated.
	SendDlgItemMessage(hWnd, IDC_RADIO_NORMAL, BM_CLICK, 0, 0);
}

//===========================================================================
static void SetBtns(HWND window, int nSelected) {
	if (sglDifficulty != -1) 
		DrawDiffBtn(GetDlgItem(window, IDC_RADIO_NORMAL+sglDifficulty), sglDifficulty, FALSE);

	DrawDiffBtn(GetDlgItem(window, IDC_RADIO_NORMAL+nSelected), nSelected, TRUE);
}

//===========================================================================
static void SetDesc(HWND hWndDesc, LONG lFirstString, LONG lValue) {
	char szText[MAX_STRING_LEN];

	LoadString(global_hinstance, lFirstString + lValue, szText, sizeof(szText));
	SetWindowText(hWndDesc, szText);
}

//===========================================================================
static int ProcessName(LPSTR szName) {
	char szTemp[MAX_GAME_LEN];
	int nStart, nEnd;

	// Take the name and remove leading and trailing spaces
	strcpy(szTemp, szName);
	
	// Proceed until we find a non-space or null termination
	for (nStart=0; ;nStart++) {
		if (szTemp[nStart] != ' ')
			break;
		if (szTemp[nStart] == 0)			
			return 0;
	}


	// Now start at the end and look for a non-space character. Proceed until
	// we reach the beginning
	for (nEnd=strlen(szTemp)-1; ;nEnd--) {
		
		if (szTemp[nEnd] != ' ')
			break;

		if (nEnd <= nStart)
			return 0;
	}
	szTemp[++nEnd] = 0;		// NULL terminate string before trailing spaces

	strcpy(szName, &szTemp[nStart]);
	return(nEnd - nStart);	// Length of string not including NULL termination
}




//===========================================================================
BOOL TryCreateGame(HWND window) {
	char szString[256];
	char szGameDesc[SNETSPI_MAXSTRINGLENGTH];
	BOOL bReturn;


	// Create Game description for Storm (and for display of game later)
	GameData2Txt(
			&sgGameData, 
			sgCreateParams.playerdata->playername, 
			sgCreateParams.playerdata->playerdescription, 
			szGameDesc, 
			sizeof(szGameDesc));

	if (!UiAuthCallback(
			SNET_AUTHTYPE_GAME, 
			sgCreateParams.playerdata->playername, 
			sgCreateParams.playerdata->playerdescription,
			0,			
			szGameDesc,
			szString,
			sizeof(szString))) {
		UiMessageBoxCallback(window, szString, NULL, MB_ICONWARNING | MB_OK);
		return 0;
	}

	// Make sure the application has allocated enough data for the structure
	// (could be version differences with the .exe)
	if (sgCreateParams.programdata->initdatabytes >= sizeof(sgGameData)) {
		TGAMEDATA *pInitData = (TGAMEDATA *)sgCreateParams.programdata->initdata;
		pInitData->bDiff = sgGameData.bDiff;
	}

	bReturn = SNetCreateGame(sgGameInfo.szName, 
				sgGameInfo.szPassword,
				szGameDesc,
				GetPlayerCategory(sgCreateParams.playerdata->playerdescription),	//### MM Diablo Patch #2  3/21/97
				sgCreateParams.programdata->initdata,			// Diablo is responsible for making this point to an sgGameData structure
				sgCreateParams.programdata->initdatabytes,								
				sgCreateParams.createdata->maxplayers,
				sgCreateParams.playerdata->playername,
				NULL,
				sgCreateParams.playerid);

	if (bReturn == FALSE) {
		char szFmt[128];
		if (GetLastError() == SNET_ERROR_ALREADY_EXISTS) {
			LoadString(global_hinstance, IDS_GAMEALREADYEXISTS, szFmt, sizeof(szFmt));
			sprintf(szString, szFmt, sgGameInfo.szName);
		}	
		else
			LoadString(global_hinstance, IDS_CREATEGAMEERROR, szString, sizeof(szString));

		UiMessageBoxCallback(window, szString, NULL, MB_ICONWARNING | MB_OK);
	}

	return bReturn;
}


//===========================================================================
static HFONT SetFonts (HWND window) {
	HFONT hFont = NULL;
	LOGFONT lFont;

	if ((hFont = (HFONT) SendMessage(window, WM_GETFONT, 0, 0L))) {
		// Start with dialog font and modify size and weight
		if (GetObject(hFont, sizeof(LOGFONT), (LPSTR) &lFont)) {
			lFont.lfHeight = -MulDiv(12, 96, 72);	// height of 12 pixels
			lFont.lfWidth = 0;						// let Windows autosize width

			if (hFont = CreateFontIndirect((LPLOGFONT) &lFont)) {
				SendDlgItemMessage(window, IDC_RADIO_NORMAL, WM_SETFONT, (WPARAM)hFont, 0);
				SendDlgItemMessage(window, IDC_RADIO_NIGHTMARE, WM_SETFONT, (WPARAM)hFont, 0);
				SendDlgItemMessage(window, IDC_RADIO_HELL, WM_SETFONT, (WPARAM)hFont, 0);
			}
		}
	}
	return hFont;
}

//===========================================================================
static BOOL CALLBACK ChatCreateGameDialogProc (HWND   window,
                                       UINT   message,
                                       WPARAM wparam,
                                       LPARAM lparam) {

	static HWND hWndName, hWndPassword;
	static HFONT shFont;

	switch (message) {
		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDOK: {
					char szText[MAX_STRING_LEN];

					// First, verify user entered required information
					if (!SendMessage(hWndName, WM_GETTEXTLENGTH, 0, 0L)) {
						LoadString(global_hinstance,IDS_NAME_REQUIRED,szText,MAX_STRING_LEN);
						UiOkDialog(window, szText, TRUE);
						return 0;
					}

					// Fill create game structure
					SendMessage(hWndName, WM_GETTEXT, sizeof(sgGameInfo.szName), (LPARAM)((LPSTR)sgGameInfo.szName));
					if (IsNastyName(sgGameInfo.szName) || InvalidChars(sgGameInfo.szName, "") || !ProcessName(sgGameInfo.szName)) {
						LoadString(global_hinstance,IDS_GAME_NAME_INVALID,szText,MAX_STRING_LEN);
						UiOkDialog(window, szText, TRUE);
						sgGameInfo.szName[0] = 0;
						return 0;
					}
					

					// Check if password was entered
					sgGameInfo.szPassword[0] = 0;
					if (SendMessage(hWndPassword, WM_GETTEXTLENGTH, 0, 0)) {
						SendMessage(hWndPassword, WM_GETTEXT, sizeof(sgGameInfo.szPassword), (LPARAM)(LPSTR)(sgGameInfo.szPassword));
					}

					// Now that we have verified that the create game is successful, we can start 
					// modifying the global create game data.

					// Set difficulty
					sgGameData.bDiff = (BYTE) sglDifficulty;

					UiSndPlayEnter();

					if (TryCreateGame(window))
						SDlgEndDialog(window, TRUE);
					return 0;
	
			   }
				case IDCANCEL:
					UiSndPlayEnter();
					SDlgEndDialog(window, FALSE);
					return 0;	 


				case IDC_RADIO_NORMAL:
				case IDC_RADIO_NIGHTMARE:
				case IDC_RADIO_HELL: {
					int nDiff = LOWORD(wparam) - IDC_RADIO_NORMAL;
					int nNotify = HIWORD(wparam);

					// Don't allow these buttons to get the keyboard focus
					if (nNotify == BN_SETFOCUS)
						SetFocus(hWndName);

					if (sglDifficulty != nDiff) {
						if (sglDifficulty != -1) {
							// Unhilite previous button
							SendMessage((HWND)GetDlgItem(window, sglDifficulty + IDC_RADIO_NORMAL), BM_SETSTATE, FALSE, 0);
							UiSndPlayClick();
						}

						SetBtns(window, nDiff);
						SetDesc(GetDlgItem(window, IDC_STATIC_DIFF_DESC),
								IDS_NORMAL_DESC_XLONG,
								nDiff);

						sglDifficulty = nDiff;
					}

					// Make sure current selection is hilited (windows will want to unhilite it)
					SendMessage((HWND)lparam, BM_SETSTATE, TRUE, 0);
					break;			
				 }
   	   		}						   
   	   		break;


		case WM_CTLCOLORSTATIC:
			if (GetWindowLong((HWND)lparam, GWL_ID) == IDC_TITLE) {
				UiSetTextYellow((HDC)wparam);
				return (BOOL) GetStockObject(NULL_BRUSH);
			}

		break;

		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			SendMessage(SDrawGetFrameWindow(), message, wparam, lparam);
		break;

		case WM_DESTROY:
		  	DestroyArtwork(window);
			DestroyDiffBtns(window);
			if (shFont) 
				DeleteObject(shFont);

			hWndName = hWndPassword = NULL;
			break;
		
   		case WM_INITDIALOG: 
			hWndName = GetDlgItem(window, IDC_EDIT_NAME);
			hWndPassword = GetDlgItem(window, IDC_EDIT_PASSWORD);

			LoadArtwork(window);
			InitDiffBtns(window);
			InitDefaultOptions(window);
			shFont = SetFonts(window);

			SendMessage(hWndName, EM_LIMITTEXT, sizeof(sgGameInfo.szName)-1, 0);
			SendMessage(hWndPassword, EM_LIMITTEXT, sizeof(sgGameInfo.szPassword)-1, 0);

		  	return 1;
		
	}

	return SDlgDefDialogProc(window,message,wparam,lparam);

}


/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

							
//===========================================================================
BOOL CALLBACK UiCreateGameCallback (SNETCREATEDATAPTR   createdata,
									SNETPROGRAMDATAPTR  programdata,
									SNETPLAYERDATAPTR	playerdata,
									SNETUIDATAPTR       interfacedata,
									SNETVERSIONDATAPTR  versiondata,
									DWORD              *playerid) {

	BOOL bResult;


	// Init global params data
	sgCreateParams.createdata = createdata;
	sgCreateParams.programdata = programdata;
	sgCreateParams.playerdata = playerdata;
	sgCreateParams.interfacedata = interfacedata;
	sgCreateParams.versiondata = versiondata;
	sgCreateParams.playerid = playerid;



	// DISPLAY THE DIALOG BOX
	bResult = SDlgDialogBox(global_hinstance,TEXT("DIALOG_CREATE_GAME"),interfacedata->parentwindow,ChatCreateGameDialogProc);
	if (bResult == -1)
		return 0;
	return bResult;
}

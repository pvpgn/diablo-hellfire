/***************************************************************************
*
*  ad.cpp
*  Battle.net advertisement handling
*
*
*  By Michael Morhaime
*
*  NOTE: The advertisement window is implimented as a modeless dialog. Since the chatroom is a modal 
*  dialog, the bnetlogo dialog (the chatroom's parent) is disabled. If we were to impliment the 
*  ad window as a simple window and child of bnetlogo, the ad window also would be disabled and would
*  never receive mouse click messages.
* 
*  Therefore, the advertisement window is, in fact, a modeless dialog with no child windows. Whenever
*  it receives a mouse click, it processes the click without allowing the window to activate.
***/


//****************************************************************************
// Modification Log:
// 
// Diablo Patch #1
// 2/17/97  MM	-	Added call to SrvNotifyDisplayAd() upon successful display of ad.
//
// Starcraft Modifications
// 10/21/97 DML -   Changed ad window to industry standard 468x60 pixels
//                  Will blt old ads clipped to this window
//****************************************************************************

#include "pch.h"

//***************************************************************************
#define MAX_URL_SIZE 1024
#define BATTLE_HTML_FILE "battle.htm"
#define AD_TIMER_DELAY 50					// 20 fps is our max frame rate
#define AD_WIDTH 468
#define AD_HEIGHT 60

#define PREFSKEY				"Preferences"
#define PREFSVALUE_USEDLINK		"Clicked Link"

//***************************************************************************
typedef struct _TADINFO {
	DWORD dwAdId;
	DWORD dwAdType;
	char szAdURL[MAX_URL_SIZE];
	char szAdFilename[MAX_PATH];
} TADINFO, *PTADINFO;

//***************************************************************************

static HWND sghWndAd = NULL;

static LPBYTE sgpAdData = NULL;
static SIZE sgAdSize;

static HSVIDEO sghVideo = NULL;
static LPBYTE sgpAdAnimation = NULL;		// Holds Smack flic
static UINT sgAdTimer = 0;


// Current ad information
static TADINFO sgCurrAdInfo = { 0 };
static TADINFO sgNextAdInfo = { 0 };


BOOL UiSetCustomArt (HWND           hWnd,
					PALETTEENTRY   *pe,
					int				nFirstColor,
					int				nNumColorsUsed,
					BOOL			bSetPaletteNow,
					LPBYTE          data,
					int				nWidth,
					int				nHeight);




//***************************************************************************
static void AdStopAd(void) {
	// free any memory associated with the current Ad
	sgCurrAdInfo.szAdURL[0] = 0;
	sgCurrAdInfo.szAdFilename[0] = 0;

	if (sgAdTimer) {
		SDlgKillTimer(sghWndAd, sgAdTimer);
		sgAdTimer = 0;
	}		

	if (sghVideo) {
		SVidPlayEnd(sghVideo);
		sghVideo = NULL;
	}

	if (sgpAdAnimation) {
		FREE(sgpAdAnimation);
		sgpAdAnimation = NULL;
	}
}

//***************************************************************************
static BOOL AdInitWindow(HWND hWndAd, SNETGETARTPROC artcallback) {
	RECT r;
	LPBYTE pTemp = NULL;
	SIZE tempSize;
	int nSize;

	sghWndAd = hWndAd;
	if (!sghWndAd)
		return FALSE;


	SetWindowPos(
		sghWndAd, 
		NULL, 
		0, 0, AD_WIDTH, AD_HEIGHT,
		SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	GetClientRect(hWndAd, &r);
	sgAdSize.cx = r.right;
	sgAdSize.cy = r.bottom;

	// make sure window has double word width
	sgAdSize.cx += (4 - sgAdSize.cx%4);
	nSize = sgAdSize.cx * sgAdSize.cy;
	sgpAdData = (LPBYTE)ALLOC(nSize);
	if (sgpAdData == NULL)
		return FALSE;

	// Init to blackness
	ZeroMemory(sgpAdData, nSize);

	SDlgSetBitmap(
			hWndAd,
			NULL,	
			NULL,
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,	
			sgpAdData,
			NULL,
			sgAdSize.cx,
			sgAdSize.cy);


	UiLoadCustomArt(
			artcallback,
			NULL,
			SNET_ART_BATTLE_WELCOME_AD,
			32, 
			64,
			TRUE,
			&pTemp,
			&tempSize);
	
	// Set up welcome ad to point to battle.net
	strcpy(sgCurrAdInfo.szAdURL, "http://www.battle.net");

	if (pTemp) {
		// Copy bitmap to Ad buffer
		SBltROP3(sgpAdData, pTemp, min(tempSize.cx, sgAdSize.cx), min(tempSize.cy, sgAdSize.cy), sgAdSize.cx, tempSize.cx, NULL, SRCCOPY);

		// Free bitmap
		FREE(pTemp);
	}

	return (TRUE);
}

//***************************************************************************
static void AdDestroyWindow(void) {
	AdStopAd();

	sghWndAd = NULL;
	if (sgpAdData) {
		FREE(sgpAdData);
		sgpAdData = NULL;
	}
}


//***************************************************************************
void AdSetInfo(SNADINFOPTR pAdInfo) {

	// Set type of ad
	sgNextAdInfo.dwAdType = pAdInfo->adtype;
	sgNextAdInfo.dwAdId = pAdInfo->id;

	// Copy url string
	if (strlen(pAdInfo->url) < sizeof(sgNextAdInfo.szAdURL))
		strcpy(sgNextAdInfo.szAdURL, pAdInfo->url);
	else
		sgNextAdInfo.szAdURL[0] = 0;

	// Copy filename string
	if (strlen(pAdInfo->filename) < sizeof(sgNextAdInfo.szAdFilename))
		strcpy(sgNextAdInfo.szAdFilename, pAdInfo->filename);
	else
		sgNextAdInfo.szAdFilename[0] = 0;
}



//***************************************************************************
static BOOL AdDisplayPCX(LPBYTE pPCXData, DWORD dwSize) {
	PALETTEENTRY pe[256];
	LPBYTE pBuffer;
	DWORD dwBufferSize;
	int nWidth, nHeight, nBitDepth;


	if (!sghWndAd)
		return FALSE;

	// Query decode function for dimensions of pcx
 	if (!SBmpDecodeImage(SBMP_IMAGETYPE_PCX,
					pPCXData,
					dwSize,
					&pe[0],
					NULL,
					0,
					&nWidth,
					&nHeight,
					&nBitDepth))
		return 0;

	dwBufferSize = nWidth*nHeight*nBitDepth/8;
	pBuffer = (LPBYTE)ALLOC(dwBufferSize);
	if (pBuffer == NULL)
		return 0;

 	if (!SBmpDecodeImage(SBMP_IMAGETYPE_PCX,
					pPCXData,
					dwSize,
					&pe[0],
					pBuffer,
					dwBufferSize,
					&nWidth,
					&nHeight,
					&nBitDepth)) {
		FREE(pBuffer);
		return 0;
	}


	// Clear window before changing palette
	RECT r;
	GetClientRect(sghWndAd, &r);
	SDlgBltToWindow(
		sghWndAd,
		NULL, 
		0, 0,
		pBuffer,			// This isn't used, but can't be NULL anyways
		&r,
		(LPSIZE)&r.right,
		0xffffffff,
		0,
		BLACKNESS);



	// Set this bitmap in the add window
	if (!UiSetCustomArt(NULL,	// This means just set palette entries (leave window bitmap ptr unchanged)
						&pe[0],
						32,			// FIRST AD COLOR
						64,			// NUM AD COLORS
						TRUE,		// Set palette now
						pBuffer,	// Ad data
						nWidth,
						nHeight)) {
		FREE(pBuffer);
		return 0;
	}

	// Copy bitmap to AdData ptr  
	SBltROP3(sgpAdData,
	         pBuffer,
	         min(nWidth, sgAdSize.cx),
	         min(nHeight, sgAdSize.cy),
	         sgAdSize.cx,
	         nWidth,
	         NULL,
	         SRCCOPY);
	InvalidateRect(sghWndAd, NULL, FALSE);

	// Free bitmap
	FREE(pBuffer);
	return 1;
}

													
//***************************************************************************
static void CALLBACK AdSMKTimer(HWND hWnd, UINT uMsg, UINT uID, DWORD dwTime) {
	BOOL bUpdated = FALSE;

	if (sghVideo == NULL)
		return;
			  
	SVidPlayContinueSingle (sghVideo, FALSE, &bUpdated);

	if (bUpdated) 
		InvalidateRect(hWnd, NULL, FALSE);
}

//***************************************************************************
static BOOL AdDisplaySMK(LPBYTE pSMKData, DWORD dwSize) {
// @#@ resolve crash playing smacks to different sized buffer
    return 0;
    
	RECT r;
	SVIDPALETTEUSE paletteUse = { sizeof(SVIDPALETTEUSE), 32, 64 };


	if (!sghWndAd)
		return FALSE;

	sgpAdAnimation = (LPBYTE)ALLOC(dwSize);
	if (!sgpAdAnimation)
		return FALSE;

	// Make our own copy of the animation
	memcpy(sgpAdAnimation, pSMKData, dwSize);

	SetRect(&r, 0, 0, sgAdSize.cx-1, sgAdSize.cy-1);
	SVidPlayBeginFromMemory(sgpAdAnimation,
	                        dwSize,
	                        sgpAdData,
	                        &r,
	                        &sgAdSize,
	                        &paletteUse,
	                        SVID_FLAG_TOBUFFER | SVID_FLAG_LOOP,
	                        &sghVideo);
	sgAdTimer = SDlgSetTimer(sghWndAd, 1, AD_TIMER_DELAY, AdSMKTimer);

	GetClientRect(sghWndAd, &r);
	SDlgBltToWindow(
		sghWndAd,
		NULL, 
		0, 0,
		sgpAdAnimation,			// This isn't used, but can't be NULL anyways
		&r,
		(LPSIZE)&r.right,
		0xffffffff,
		0,
		BLACKNESS);

	return 1;
}

//***************************************************************************
void AdDisplay(LPVOID pData, DWORD dwSize) {
	BOOL bSuccess;


	if (!sghWndAd)
		return;

	AdStopAd();

	// Handle advertisement data based on ad type
	switch(sgNextAdInfo.dwAdType) {
		case ADTYPE_PCX:
			bSuccess = AdDisplayPCX((LPBYTE) pData, dwSize);
		break;

		case ADTYPE_SMK:
			bSuccess = AdDisplaySMK((LPBYTE) pData, dwSize);
		break;
	}

	if (bSuccess) {
		sgCurrAdInfo = sgNextAdInfo;
		SrvNotifyDisplayAd(sgCurrAdInfo.dwAdId, sgCurrAdInfo.szAdFilename, sgCurrAdInfo.szAdURL);
	}
}



//***************************************************************************
void AdNavigate(SNETUIDATAPTR interfacedata) {
	char szText[256];
	char szTitle[32];
	char szBrowserApp[MAX_PATH];
	char szURLSave[MAX_URL_SIZE];
	FILE *fp;
	BOOL bAdSuccess = TRUE;		// Assume user will successful get to the internet

	if (sgCurrAdInfo.szAdURL[0] == 0)
		return;

	if (interfacedata->soundcallback)
		interfacedata->soundcallback(PROVIDERID, SNET_SND_CHANGEFOCUS, 0);

	// Save a copy of the url, in case it changes while user is viewing the message box
	strcpy(szURLSave, sgCurrAdInfo.szAdURL);



	// Prompt user if they really want to launch their browser (only prompt the first time they click)
	DWORD dwUsedLink = 0;
    SRegLoadValue(PREFSKEY,PREFSVALUE_USEDLINK,SREG_FLAG_BATTLENET,&dwUsedLink);
	if (dwUsedLink == 0) {
		dwUsedLink = 1;
		SRegSaveValue(PREFSKEY,PREFSVALUE_USEDLINK,SREG_FLAG_BATTLENET,dwUsedLink);

		LoadString(global_hinstance, IDS_QUERYBROWSEWEB, szText, sizeof(szText));
		LoadString(global_hinstance, IDS_BATTLENET, szTitle, sizeof(szTitle));
		if (IDOK != UiMessageBox(interfacedata->messageboxcallback, interfacedata->parentwindow, szText, szTitle, MB_OKCANCEL)) {
			SrvNotifyClickAd(sgCurrAdInfo.dwAdId, FALSE);
			return;
		}
	}


 	// Switching to the Desktop will cause us to be minimized and switch out of Direct Draw Mode.
	// We'll want to minimize our app before calling ShellExecute().
	// SetForegroundWindow() works better than ShowWindow(SDrawGetFrameWindow(), SW_MINIMIZE), which causes the
	// task bar (if auto hidden) to be drawn whenever the frame window is displayed.
	SetForegroundWindow(GetDesktopWindow());

	// First try to launch browser by just passing ShellExecute() a url
	if (32 >= (unsigned int)ShellExecute(
					SDrawGetFrameWindow(),	
					"open",
					szURLSave,
					NULL,
					NULL,
					SW_SHOWNORMAL)) {

		// Okay.. that didn't work. Let's run the browser explicitly

		// create dummy battle.htm file
		fp = fopen(BATTLE_HTML_FILE, "wb");
		fclose(fp);

		if (32 >= (unsigned int)FindExecutable(BATTLE_HTML_FILE, NULL, szBrowserApp))  {
			// Get focus back
			UiRestoreApp();

			// Display message
			LoadString(global_hinstance, IDS_BROWSERERROR, szText, sizeof(szText));
			LoadString(global_hinstance, IDS_BATTLENET, szTitle, sizeof(szTitle));
			UiMessageBox(interfacedata->messageboxcallback, interfacedata->parentwindow, szText, szTitle, MB_OK | MB_ICONWARNING);
			bAdSuccess = FALSE;
			goto FILE_CLEANUP;
		}


		// Launch browser window
		if (32 >= (unsigned int)ShellExecute(
						SDrawGetFrameWindow(),	
						"open",
						szBrowserApp,
						szURLSave,
						NULL,
						SW_SHOWNORMAL)) {

			// Get focus back
			UiRestoreApp();

			// Display Error
			LoadString(global_hinstance, IDS_BROWSERERROR, szText, sizeof(szText));
			LoadString(global_hinstance, IDS_BATTLENET, szTitle, sizeof(szTitle));
			UiMessageBox(interfacedata->messageboxcallback, interfacedata->parentwindow, szText, szTitle, MB_OK | MB_ICONWARNING);
			bAdSuccess = FALSE;
			goto FILE_CLEANUP;
		}

	FILE_CLEANUP:
		DeleteFile(BATTLE_HTML_FILE);

	}

	SrvNotifyClickAd(sgCurrAdInfo.dwAdId, bAdSuccess);
}



/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/



//===========================================================================
BOOL CALLBACK AdDialogProc (  HWND   window,
							  UINT   message,
							  WPARAM wparam,
							  LPARAM lparam) {

	static SNETUIDATAPTR sInterfacedata = NULL;
	static HCURSOR shCursor;
	static BOOL sbNavigating = 0;

	switch (message) {
		case WM_DESTROY:
			AdDestroyWindow();
			sInterfacedata = NULL;
			DeleteObject(shCursor);
			shCursor = NULL;
			break;

   		case WM_INITDIALOG: 
            RECT testrect;
            GetClientRect(window,&testrect);

			// SAVE A POINTER TO THE USER INTERFACE PARAMETERS
			sInterfacedata = (SNETUIDATAPTR)lparam;

			if (sInterfacedata == NULL) {
				SDlgEndDialog(window, 0);
				return 0;
			}

			AdInitWindow(window, sInterfacedata->artcallback);

			// Get link cursor
			if (sInterfacedata->getdatacallback) {
				sInterfacedata->getdatacallback(
						PROVIDERID,
						SNET_DATA_CURSORLINK,
						&shCursor,
						sizeof(shCursor),
						NULL);
			}

		  	return 1;

		case WM_MOUSEACTIVATE:
			// Don't activate window, but process the mouse click anyway
			SetWindowLong(window, DWL_MSGRESULT, MA_NOACTIVATE);		
			return 1;

		case WM_LBUTTONDOWN:
			// Protect this code
			if (sbNavigating)
				return 1;

			sbNavigating = 1;
			AdNavigate(sInterfacedata);
			sbNavigating = 0;
			return 1;

		case WM_SETCURSOR:
			// Ignore cursor, if we have already clicked on the ad.
			if (sbNavigating)
				break;

			if (shCursor &&	sgCurrAdInfo.szAdURL[0] != 0)
				SetCursor(shCursor);
			return 1;
	}

	return SDlgDefDialogProc(window,message,wparam,lparam);
}



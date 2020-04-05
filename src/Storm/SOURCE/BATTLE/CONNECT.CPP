/****************************************************************************
*
*  connect.CPP
*  battle.net 
*
*  By Michael Morhaime
*
***/

#include "pch.h"

#define PREVENT_MINIMIZE   0		// 0 in final

//****************************************************************************
//****************************************************************************
#define MILLISEC_PER_SEC	1000

#define LOGO_TIMER_ID		1
#define MAX_FRAMES			32



static HTRANS sgTransHandles[MAX_FRAMES];
static int sgFrame;

static LPBYTE sgBackgroundBmp	= NULL;
static LPBYTE sgButtonBmp		= NULL;
static LPBYTE sgLogoBmp			= NULL;
static SIZE sgLogoBmpSize;

static BOOL sgbPreventMinimize	= FALSE;
static HWND sgRudeWindow		= NULL;
static HWND sgRudeWindowParent	= NULL;

extern void UiHideConnectCancel(void);
extern BOOL UiProcessWindowMessages(void);
extern HWND ghWndUiMainParent;		



//****************************************************************************
//****************************************************************************
BOOL LogoSetTimer(HWND window, int nTimer, SNETGETDATAPROC getdatacallback) {
	LPBYTE pData;
	DWORD dwSize;

	if (UiGetData(getdatacallback, SNET_DATA_BATTLE_LOGODELAY, &pData, &dwSize)) {
		if (dwSize == sizeof(DWORD)) {
			DWORD dwDelay;
			int nResult;

			dwDelay = *((DWORD *)pData);
			nResult = SDlgSetTimer(window, nTimer, dwDelay, NULL);
			FREE(pData);
			return (nResult != NULL);				
		}
	}

	return 0;
}

//****************************************************************************
//****************************************************************************
void LogoAnimate(HWND window, HWND child) {
	RECT rect;

	TPBMP tpBmp = (TPBMP) GetWindowLong(child, GWL_USERDATA);

	if (! child) return;
	if (! tpBmp) return;
	if (! sgLogoBmp) return;
	if (! sgTransHandles[0]) return;


	sgFrame++;
	if ((! sgTransHandles[sgFrame]) || (sgFrame >= MAX_FRAMES))
		sgFrame = 0;


	GetWindowRect(child, &rect);
	ScreenToClient(window, (LPPOINT)&rect.left);
	ScreenToClient(window, (LPPOINT)&rect.right);

	// Copy from Background bitmap to buffer for animating logo
   	SBltROP3 (
   		sgLogoBmp,
		tpBmp->data + (rect.top * tpBmp->datasize.cx) + rect.left,
		sgLogoBmpSize.cx,
		sgLogoBmpSize.cy,
		sgLogoBmpSize.cx,
		tpBmp->datasize.cx,
		NULL,
		SRCCOPY
   	);

	STransBlt(sgLogoBmp, 0, 0, sgLogoBmpSize.cx, sgTransHandles[sgFrame]);

	InvalidateRect(child, NULL, FALSE);
}


//****************************************************************************
//****************************************************************************
void LogoFramesDestroy(void) {
 	for (int index = 0; index < MAX_FRAMES; index++) {
		if (sgTransHandles[index]) {
			STransDelete(sgTransHandles[index]);
			sgTransHandles[index] = NULL;
		}
	}

	if (sgLogoBmp) {
		FREE(sgLogoBmp);
		sgLogoBmp = NULL;
	}
}


//****************************************************************************
//****************************************************************************
BOOL LogoInit(HWND child, SNETGETARTPROC artcallback) {
	RECT rect;
	LPBYTE srcframes;
	SIZE srcsize;

	GetClientRect(child, &rect);
	sgLogoBmpSize.cx = rect.right;
	sgLogoBmpSize.cy = rect.bottom;

	// Get logo animation
	if (!UiLoadArtwork(artcallback,
		               NULL,
		               NULL,    // don't register the artwork
		               SNET_ART_APP_LOGO_SML,
		               0,
		               0,
		               0,
		               FALSE,
		               FALSE,
		               &srcframes,
		               &srcsize)) {
        ShowWindow(child,SW_HIDE);
        sgLogoBmp = NULL;
        return FALSE;
    }

	memset(sgTransHandles, 0, sizeof(sgTransHandles));
	if (srcframes && sgLogoBmpSize.cy) {
		int frames = srcsize.cy / sgLogoBmpSize.cy;
		if (frames > MAX_FRAMES)
			frames = MAX_FRAMES;
		// create an HTRANS for each frame in the animation
		for (int index = 0; index < frames; index++) {

			rect.left = 0;
			rect.right = sgLogoBmpSize.cx - 1;
			rect.top = index * sgLogoBmpSize.cy;
			rect.bottom = rect.top + sgLogoBmpSize.cy - 1;

			STransCreate(
				srcframes,
				sgLogoBmpSize.cx,
				sgLogoBmpSize.cy,
				8,
				&rect,
				PALETTEINDEX(250),
				&sgTransHandles[index]
			);
		}

		FREE(srcframes);
	}
	sgFrame = 0;

	sgLogoBmp = (LPBYTE) ALLOC(rect.right * rect.bottom);
	if (!sgLogoBmp)
		return FALSE;

	SDlgSetBitmap(
		child,
		NULL,
		NULL,
		SDLG_STYLE_ANY,
		SDLG_USAGE_BACKGROUND,
		sgLogoBmp,
		NULL,
		rect.right,
		rect.bottom
	);
	return TRUE;
}


//===========================================================================
static void DestroyArtwork (HWND window) {
	TPBMP tpBmp = (TPBMP) GetWindowLong(GetDlgItem(window, IDC_LOGO_ANIMATE), GWL_USERDATA);

	// tpBmp->data points to a sgBAckgroundBmp, which will be freed below
	if (tpBmp) 
		FREE(tpBmp);

	if (sgBackgroundBmp) {
		FREE(sgBackgroundBmp);
		sgBackgroundBmp = NULL;
	}

	if (sgButtonBmp) {
		FREE(sgButtonBmp);
		sgButtonBmp = NULL;
	}

	LogoFramesDestroy();
}

//===========================================================================
static BOOL LoadArtwork (HWND window, SNETGETARTPROC artcallback) {
	int btn_ids[] = {
		IDCANCEL,
		0
	};

	int btn_static[] = {
		IDC_STATIC_TEXT,
		0
	};

	SIZE sizeBtns;
	SIZE bgSize;

	UiLoadArtwork(
			artcallback,
			window,
			NULL,
			SNET_ART_BATTLE_CONNECT_BKG,
			TEXT(""),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,	
			TRUE,					// Use palette from this art
			TRUE,					// Prep palette for a fade
			&sgBackgroundBmp,
			&bgSize);

	UiLoadArtwork(
			artcallback,
			NULL,
			NULL,
			SNET_ART_BUTTON_SML,
			TEXT("Button"),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,	
			FALSE,				
			FALSE,
			&sgButtonBmp,
			&sizeBtns);

	SDlgSetControlBitmaps (window, btn_ids, NULL, sgButtonBmp, &sizeBtns, SDLG_ADJUST_VERTICAL);
	SDlgSetControlBitmaps (window, btn_static, NULL, sgBackgroundBmp, &bgSize, SDLG_ADJUST_CONTROLPOS);


	// set up a TPBMP for the logo window, it will have information about the background bitmap
	HWND hWndLogo = GetDlgItem(window, IDC_LOGO_ANIMATE);

	if (hWndLogo && sgBackgroundBmp) {
		TPBMP tpBmp = (TPBMP) ALLOC(sizeof(TBMP));
		SetWindowLong(hWndLogo, GWL_USERDATA, (LONG) tpBmp);
		if (tpBmp) {
			tpBmp->data = sgBackgroundBmp;
			tpBmp->datasize = bgSize;
		}

		// set up the animating diablo logo
		LogoInit(hWndLogo,artcallback);
		LogoAnimate(window, hWndLogo);
	}

	return 1;
}


//****************************************************************************
//****************************************************************************
void ProtectMinimize(BOOL bEnable) {
#if PREVENT_MINIMIZE
	static sOldStyle;

	// Protect this code
	if (sgbPreventMinimize == bEnable)
		return;

	sgbPreventMinimize = bEnable;

	HWND hWndFrame = SDrawGetFrameWindow();

	if (bEnable) {
		sgRudeWindow = NULL;
		sgRudeWindowParent = NULL;

		sOldStyle = GetWindowLong(hWndFrame,GWL_EXSTYLE);
		SetWindowLong(hWndFrame, GWL_EXSTYLE, sOldStyle & ~WS_EX_TOPMOST);
	}
	else if (sgRudeWindow) {
		if (IsWindow(sgRudeWindow) && IsWindow(sgRudeWindowParent)) 
			SetParent(sgRudeWindow, sgRudeWindowParent);
	
		sgRudeWindow = NULL;
		sgRudeWindowParent = NULL;

		SetWindowLong(hWndFrame, GWL_EXSTYLE, sOldStyle);

		// Give our application back the focus
		SetForegroundWindow(GetActiveWindow());
	}
#else
	
	sgbPreventMinimize = bEnable;
	if (!bEnable) 
		UiRestoreApp();

#endif
}


/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/



//===========================================================================
BOOL CALLBACK ConnectDialogProc (HWND   window,
                                  UINT   message,
                                  WPARAM wparam,
                                  LPARAM lparam) {

	static UIPARAMSPTR uiparams;
	static HWND shWndCancelDlg;

	switch (message) {
		case WM_TIMER:
			if (wparam == LOGO_TIMER_ID) {
				LogoAnimate(window, GetDlgItem(window, IDC_LOGO_ANIMATE));
			}						 
			return 1;

#if PREVENT_MINIMIZE
		case WM_NCACTIVATE:
			if (!sgbPreventMinimize) 
				break;

			if (wparam == FALSE) {
				PostMessage(window, WM_USER+100, 0, 0);	

				SetWindowLong(window, DWL_MSGRESULT, 0);		// This means don't minimize this window
				return 1;								
			}
			break;

		case WM_USER+100:
			// Make new window our child so it will draw on the directX surface
			if (GetForegroundWindow() != GetActiveWindow()) {
				sgRudeWindow = GetForegroundWindow();
				sgRudeWindowParent = GetParent(sgRudeWindow);
				SetParent(sgRudeWindow, window);

				// Hide Connect Dialog. Dialup networking will provide its own.
				UiHideConnectCancel();
			}
			return 1;
#endif

		case WM_NOTIFICATION_WAITING:
			UiNotification();
			return 1;

		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			SendMessage(SDrawGetFrameWindow(), message, wparam, lparam);
			break;

		case WM_DESTROY:
			ghWndUiMainParent = NULL;

			UiVidFadeOut(DEFAULT_STEPS*2);		// Note that VidFadeOut also erase the screen

			ShowCursor(FALSE);					// Leave cursor off
			UiVidFade(1,1);						// restore palette for parent window

			SDlgKillTimer(window, LOGO_TIMER_ID);
		  	DestroyArtwork(window);
			break;



   		case WM_INITDIALOG: 
			// SAVE A POINTER TO THE USER INTERFACE PARAMETERS
			uiparams = (UIPARAMSPTR)lparam;
			if (!uiparams || !uiparams->interfacedata)
				return 0;

			if (uiparams->interfacedata)
				LoadArtwork(window, uiparams->interfacedata->artcallback);

			UiLoadCursors(window, uiparams->interfacedata);

			// Set timer to poll for messages
			LogoSetTimer(window, LOGO_TIMER_ID, uiparams->interfacedata->getdatacallback);

			// Set this window up to process SN_NOTIFY messages 
			ghWndUiMainParent = window;

			UiVidFade(1,1);

		  	return 1;
	}

	return SDlgDefDialogProc(window,message,wparam,lparam);
}



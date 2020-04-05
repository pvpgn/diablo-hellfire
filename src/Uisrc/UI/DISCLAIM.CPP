//****************************************************************************
//***
//***  DISCLAIM.CPP
//***  Diablo UI disclaimer screen dialog (for the Battle.Net beta)
//***
//***  By Frank Pearce (10/14/96)
//***
//****************************************************************************


#include "pch.h"
#include "artfont.h"


#define ALLOW_TIMEOUT	0

//****************************************************************************
//****************************************************************************
	#define TIMER_ID			1
	#define MILLISEC_PER_SEC	1000
	#define DEFAULT_TIMEOUT		10

	static int sgTextIDs0[] = {
		IDC_TEXT1,
		IDC_TEXT2,
		0
	};
	static int sgTextIDs1[] = {
		IDC_TEXT3,
		IDC_TEXT4,
		IDC_TEXT5,
		0
	};


//****************************************************************************
//****************************************************************************
static void DisclaimerDestroy (HWND window) {
	UiDoomCtrlsDestroy(window, sgTextIDs1);
	UiDoomCtrlsDestroy(window, sgTextIDs0);

	// free the background of the dlg
	UiFreeBmp((TPBMP) GetWindowLong(window, GWL_USERDATA));
	SetWindowLong(window, GWL_USERDATA, (LONG) 0);
}


//****************************************************************************
//****************************************************************************
static void DisclaimerInit(HWND window, LPARAM lparam) {
	TPBMP tpBmp;

	// load texture maps for this dialog
	tpBmp = UiAllocBmp();
	if (tpBmp) {
		SetWindowLong(window, GWL_USERDATA, (LONG) tpBmp);
		LoadArtFile(
			window,
			NULL,
			TEXT(""),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,
			TEXT("ui_art\\disclaim.pcx"),
			&tpBmp->data,
			&tpBmp->datasize,
			FALSE
		);
		UiFadeInit(window, FALSE);
	}

#if ALLOW_TIMEOUT
	if (lparam)
		SDlgSetTimer(window, TIMER_ID, lparam*MILLISEC_PER_SEC, NULL);
	else
		SDlgSetTimer(window, TIMER_ID, DEFAULT_TIMEOUT*MILLISEC_PER_SEC, NULL);
#endif

	UiDoomStaticInit(window, sgTextIDs0, AF_BIGGRAY);
	UiDoomStaticInit(window, sgTextIDs1, AF_MED);
}


//****************************************************************************
//****************************************************************************
static void DisclaimerAbort(HWND window) {
	UiFadeAbort(window);
#if ALLOW_TIMEOUT
	SDlgKillTimer(window, TIMER_ID);
#endif
	UiVidFadeOut(DEFAULT_STEPS*2);
	SDlgEndDialog(window, 1);
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK DisclaimerDialogProc (HWND   window,
                                      UINT   message,
                                      WPARAM wparam,
                                      LPARAM lparam) {
	switch (message) {
	
		case WM_DESTROY:
			DisclaimerDestroy(window);
		break;
		
		case WM_INITDIALOG:
			DisclaimerInit(window, lparam);
			PostMessage(window, WM_USER+1000, 0, 0);
		return 1;

		case WM_COMMAND:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_KEYDOWN:
			DisclaimerAbort(window);
		return 0;

		case WM_PARENTNOTIFY:
			if (LOWORD(wparam) == WM_LBUTTONDOWN || LOWORD(wparam) == WM_RBUTTONDOWN) {
				DisclaimerAbort(window);
			}
		break;

		case WM_USER+1000:
			if (! UiIsFading()) {
				UiFadeStart(window);
			}
		return 0;

#if ALLOW_TIMEOUT
		case WM_TIMER:
			switch (wparam) {
				case TIMER_ID:
					DisclaimerAbort(window);
				break;
			}
		return 0;
#endif

		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			SendMessage(SDrawGetFrameWindow(), message, wparam, lparam);
		break;
	}
	return SDlgDefDialogProc(window,message,wparam,lparam);
}


//****************************************************************************
//***
//***  EXPORTED FUNCTIONS
//***
//****************************************************************************


//****************************************************************************
//****************************************************************************
BOOL APIENTRY UiBetaDisclaimer (UINT timeoutseconds) {
	SDlgDialogBoxParam(
		global_hinstance,
		"DISCLAIMER_DIALOG",
		SDrawGetFrameWindow(),
		DisclaimerDialogProc,
		timeoutseconds
	);

  return 1;
}

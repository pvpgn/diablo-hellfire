//****************************************************************************
//	SelLoad.cpp
//	Diablo UI load/new game for a single player hero with a save file
//	
//	By Frank Pearce
//	created 11.14.96
//****************************************************************************


#include "pch.h"
#include "artfont.h"
#include "uisnd.h"


//****************************************************************************
//****************************************************************************
	extern void SelHeroSetTitle(HWND window, LPCTSTR title);


//****************************************************************************
//****************************************************************************
	#define MILLISEC_PER_SEC	1000

	#define FOCUS_TIMER_ID		1
	#define FOCUS_FPS			16		// frames per second
	#define FOCUS_TIMER_DELAY	55		//(MILLISEC_PER_SEC / FOCUS_FPS)


//****************************************************************************
//****************************************************************************
	static int sgTextIDs[] = {
		IDC_DLGTITLE,
		0
	};
	static int sgBtnIDs[] = {
		IDC_FAKEOK,
		IDC_FAKECANCEL,
		0
	};
	static int sgListIDs[] = {
		IDC_LOADBTN,
		IDC_NEWBTN,
		0
	};


//****************************************************************************
//****************************************************************************
static void SelLoadDestroy(HWND window) {
	FocusAnimateDestroy();
	UiDoomCtrlsDestroy(window, sgListIDs);
	UiDoomCtrlsDestroy(window, sgBtnIDs);
	UiDoomCtrlsDestroy(window, sgTextIDs);
	SelHeroSetTitle(GetParent(window), NULL);
}


//****************************************************************************
//****************************************************************************
static void SelLoadInit(HWND window) {
	TCHAR buf[32];
	HWND parent = GetParent(window);

	// set the title for the parent
	LoadString(global_hinstance, IDS_SINGLIST_TITLE, buf, 32-1);
	SelHeroSetTitle(parent, buf);

	// point this window at the same bmp it's parent uses
	SetWindowLong(
		window,
		GWL_USERDATA,
		GetWindowLong(parent, GWL_USERDATA)
	);

	UiOnPaintBtns(window, sgListIDs);

	// set up a doom-like interface
	UiDoomStaticInit(window, sgTextIDs, AF_BIGGRAY);
	UiDoomButtonsInit(window, sgBtnIDs, AF_BIG, FALSE);
	UiDoomButtonsInit(window, sgListIDs, AF_MED);

	// set up the animating focus indicator
	FocusAnimateInit("ui_art\\focus16.pcx");
	SDlgSetTimer(window, FOCUS_TIMER_ID, FOCUS_TIMER_DELAY, NULL);
}


//****************************************************************************
//****************************************************************************
static void SelLoadAbort(HWND window, int ReturnVal) {
	UiSndPlayEnter();
	SDlgKillTimer(window, FOCUS_TIMER_ID);

	if (ReturnVal == IDOK) {
		// return the id of the focused button
		ReturnVal = GetWindowLong(GetFocus(), GWL_ID);
	}

	SDlgEndDialog(window, ReturnVal);
}


//****************************************************************************
//****************************************************************************
BOOL CALLBACK SelLoadDialogProc(HWND window,
								UINT message,
								WPARAM wparam,
								LPARAM lparam) {
	switch (message) {

		case WM_COMMAND:
			if (HIWORD(wparam) == BN_KILLFOCUS) {
				FocusLost(window, (HWND) lparam);
			}
			else if (HIWORD(wparam) == BN_SETFOCUS) {
				FocusSnd((HWND) lparam);
				FocusAnimate(window, (HWND) lparam);
			}
			else if (HIWORD(wparam) == BN_DOUBLECLICKED) {
				SelLoadAbort(window, IDOK);
			}
			else if (LOWORD(wparam) == IDOK) {
				SelLoadAbort(window, IDOK);
			}
			else if (LOWORD(wparam) == IDCANCEL) {
				SelLoadAbort(window, IDCANCEL);
			}
		break;

		case WM_LBUTTONDOWN:
			if (
				UiIsPtInWindow(
					window,
					GetDlgItem(window, IDC_FAKEOK),
					LOWORD(lparam),
					HIWORD(lparam)
				)
			) {
				SelLoadAbort(window, IDOK);
			}
			else if (
				UiIsPtInWindow(
					window,
					GetDlgItem(window, IDC_FAKECANCEL),
					LOWORD(lparam),
					HIWORD(lparam)
				)
			) {
				SelLoadAbort(window, IDCANCEL);
			}
		break;

		case WM_DESTROY:
			SelLoadDestroy(window);
		break;

		case WM_INITDIALOG:
			SelLoadInit(window);
		return 0;

		case WM_TIMER:
			FocusAnimate(window, GetFocus());
		return 0;

		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			SendMessage(SDrawGetFrameWindow(), message, wparam, lparam);
		break;
	}
	return SDlgDefDialogProc(window,message,wparam,lparam);
}

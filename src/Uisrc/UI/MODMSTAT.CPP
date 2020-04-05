//****************************************************************************
//	ModmStat.cpp
//	Diablo UI modem status thread/dialog
//	
//	By Frank Pearce
//	created 12.1.96
//****************************************************************************


#include "pch.h"
#include "artfont.h"
#include "uisnd.h"


//****************************************************************************
//****************************************************************************
	#define MILLISEC_PER_SEC	1000

	#define FOCUS_TIMER_ID		1
	#define FOCUS_FPS			16		// frames per second
	#define FOCUS_TIMER_DELAY	55		//(MILLISEC_PER_SEC / FOCUS_FPS)


	extern BOOL ModemJoinDone(void);


//****************************************************************************
//****************************************************************************
	static int sgTextIDs[] = {
		IDC_OKCANCEL_TEXT,
		0
	};
	static int sgBtnIDs[] = {
		IDCANCEL,
		0
	};

//****************************************************************************
//****************************************************************************
	static TCHAR			sgStatStr[SNETSPI_MAXSTRINGLENGTH];
	static BOOL				sgStatChanged;
	static BOOL				sgAbortable;
	static SNETABORTPROC	sgpAbortFcn;


//****************************************************************************
//****************************************************************************
static void ModmStatDestroy(HWND window) {
	UiFreeBmp((TPBMP) GetWindowLong(window, GWL_USERDATA));
	SetWindowLong(window, GWL_USERDATA, (LONG) 0);

	FocusAnimateDestroy();
	UiDoomCtrlsDestroy(window, sgBtnIDs);
	UiDoomCtrlsDestroy(window, sgTextIDs);
}


//****************************************************************************
//****************************************************************************
static void ModmStatInit(HWND window) {
	TPBMP tpBmp;

	// load texture maps for this dialog
	tpBmp = UiAllocBmp();
	if (tpBmp) {
		SetWindowLong(window, GWL_USERDATA, (LONG) tpBmp);
		LoadArtFile(
			window,
			NULL,
			TEXT("Popup"),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,
			TEXT("ui_art\\black.pcx"),
			&tpBmp->data,
			&tpBmp->datasize
		);
	}

	// set up a doom-like interface
	UiDoomStaticInit(window, sgTextIDs, AF_MEDGRAY);
	UiDoomButtonsInit(window, sgBtnIDs, AF_BIG);

	// set up the animating focus indicator
	FocusAnimateInit("ui_art\\focus.pcx");
	SDlgSetTimer(window, FOCUS_TIMER_ID, FOCUS_TIMER_DELAY, NULL);

	UiActiveDownBtns(window, sgBtnIDs);

	// init the globals
	sgStatStr[0] = 0;
	sgStatChanged = FALSE;
	sgpAbortFcn = (SNETABORTPROC) NULL;

	// don't allow cancel right away
	HWND child = GetDlgItem(window, IDCANCEL);
	ShowWindow(child, FALSE);
	sgAbortable = FALSE;
}


//****************************************************************************
//****************************************************************************
static void ModmStatAbort(HWND window, BOOL callabortfcn) {
	if (! sgAbortable) return;

	UiSndPlayEnter();
	SDlgKillTimer(window, FOCUS_TIMER_ID);

	if (callabortfcn && sgpAbortFcn)
		sgpAbortFcn();

	SDlgEndDialog(window, 0);
}


//****************************************************************************
//****************************************************************************
static void ModmStatUpdateStatus(HWND window) {
	sgStatChanged = FALSE;

	if (! sgpAbortFcn) return;

	HWND child = GetDlgItem(window, IDC_OKCANCEL_TEXT);
	if (! child) return;
	UiSetBmpText((TPBMP)GetWindowLong(child, GWL_USERDATA), sgStatStr);
	UiDoomStaticReset(window, sgTextIDs, AF_MEDGRAY);

	child = GetDlgItem(window, IDCANCEL);
	ShowWindow(child, TRUE);
	sgAbortable = TRUE;
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK ModmStatDialogProc(HWND window,
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
			else if (LOWORD(wparam) == IDOK) {
				ModmStatAbort(window, TRUE);
			}
			else if (LOWORD(wparam) == IDCANCEL) {
				ModmStatAbort(window, TRUE);
			}
		break;

		case WM_DESTROY:
			ModmStatDestroy(window);
		break;

		case WM_INITDIALOG:
			ModmStatInit(window);
		return 0;

		case WM_TIMER:
			if (sgStatChanged) {
				ModmStatUpdateStatus(window);
			}
			if (ModemJoinDone()) {
				sgAbortable = TRUE;
				ModmStatAbort(window, FALSE);
			}
			FocusAnimate(window, GetFocus());
		return 0;

		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			SendMessage(SDrawGetFrameWindow(), message, wparam, lparam);
		break;
	}
	return SDlgDefDialogProc(window,message,wparam,lparam);
}


//****************************************************************************
//****************************************************************************
BOOL CALLBACK ModmStatStatusCallback(LPCSTR		statstr, 
							  DWORD			completed, 
							  DWORD			remaining,
							  DWORD			flags,
							  SNETABORTPROC	abortfcn) {
	strcpy(sgStatStr, statstr);
	sgStatChanged	= TRUE;
	sgpAbortFcn		= abortfcn;
	return TRUE;
}


//****************************************************************************
//****************************************************************************
int ModmStatDialog(HWND parent) {
	return SDlgDialogBox(
		global_hinstance,
		TEXT("MODMSTAT_DIALOG"),
		parent,
		ModmStatDialogProc
	);
}

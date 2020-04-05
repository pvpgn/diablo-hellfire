//****************************************************************************
//	Progress.cpp
//	Diablo UI progress bar popup dialog
//	
//	By Frank Pearce
//	created 9.26.96
//****************************************************************************


#include "pch.h"


//****************************************************************************
//****************************************************************************
	#define PROGRESS_TIMER_ID	1
	#define MILLISEC_PER_SEC	1000
	#define CALLS_PER_SEC		20
	#define USER_DELAY			(MILLISEC_PER_SEC / sgCallsPerSec)
	#define DEFAULT_DELAY		(MILLISEC_PER_SEC / CALLS_PER_SEC)

	#define FILL_X	0
	#define FILL_Y	0

	static LPBYTE sgBgBmp = NULL;
	static LPBYTE sgBtnBmp = NULL;

	// the progress bar background
	static LPBYTE sgProgBgBmp = NULL;
	static SIZE sgProgBgSize;

	// the fill image for the progress bar
	static LPBYTE sgProgFillBmp = NULL;
	static SIZE sgProgFillSize;

	// the current image based on percent complete
	static LPBYTE sgProgBmp = NULL;
	static SIZE sgProgSize;

	static BOOL sgAbortable;
	static DWORD sgCallsPerSec;
	static PROGRESSFCN sgProgressFcn;
	static int sgnResult;
	static BOOL sgbEndDialog;


//****************************************************************************
//****************************************************************************
static void ProgressDraw(HWND window, int percent) {
	RECT rect;
	HWND child = GetDlgItem(window, IDC_UIGENERIC_PROGRESS);

	// draw the bg
	if (sgProgBgBmp)
		SBltROP3(
			sgProgBmp,
			sgProgBgBmp,
			sgProgSize.cx,
			sgProgSize.cy,
			sgProgSize.cx,
			sgProgBgSize.cx,
			NULL,
			SRCCOPY
		);

	// draw the fill
	if (sgProgFillBmp)
		SBltROP3(
			sgProgBmp + (FILL_Y * sgProgSize.cx) + FILL_X,
			sgProgFillBmp,
			((sgProgSize.cx - (2 * FILL_X)) * percent) / 100,
			sgProgSize.cy - (2 * FILL_Y),
			sgProgSize.cx,
			sgProgFillSize.cx,
			NULL,
			SRCCOPY
		);

	// invalidate the region
	GetWindowRect(child, &rect);
	ScreenToClient(window, (LPPOINT)&rect.left);
	ScreenToClient(window, (LPPOINT)&rect.right);
	InvalidateRect(window, &rect, FALSE);
}


//****************************************************************************
//****************************************************************************
static void ProgressDestroy(HWND window) {
	if (sgBgBmp) {
		FREE(sgBgBmp);
		sgBgBmp = NULL;
	}
	if (sgBtnBmp) {
		FREE(sgBtnBmp);
		sgBtnBmp = NULL;
	}
	if (sgProgBgBmp) {
		FREE(sgProgBgBmp);
		sgProgBgBmp = NULL;
	}
	if (sgProgFillBmp) {
		FREE(sgProgFillBmp);
		sgProgFillBmp = NULL;
	}
	if (sgProgBmp) {
		FREE(sgProgBmp);
		sgProgBmp = NULL;
	}
}


//****************************************************************************
//****************************************************************************
static void ProgressInit(HWND window, LPCSTR progresstext) {
	PALETTEENTRY * pal;
	HWND child;
	RECT rect;
	SIZE btnsize;
	int BtnIDs[] = { IDCANCEL, 0 };

	if (sgCallsPerSec)
		SDlgSetTimer(window,PROGRESS_TIMER_ID,USER_DELAY,NULL);
	else
		SDlgSetTimer(window,PROGRESS_TIMER_ID,DEFAULT_DELAY,NULL);

	// load ctrl/dlg bmps
	LoadArtFile(
		window,
		NULL,
		TEXT(""),
		SDLG_STYLE_ANY,
		SDLG_USAGE_BACKGROUND,
		TEXT("ui_art\\spopup.pcx"),
		&sgBgBmp,
		NULL,
		FALSE
	);

	if (SDrawGetFrameWindow() == GetParent(window))
		UiAltFadeInit(window);


	// load just the part of the palette the dialog uses
	pal = UiPaletteEntry(0);
	SDrawUpdatePalette(0,    10, &pal[0]);		// Windows colors also
	SDrawUpdatePalette(0x70, 144, &pal[0x70], TRUE);


	UiLoadBmpFile(
		TEXT("ui_art\\but_sml.pcx"),
		&sgBtnBmp,
		&btnsize
	);
	if (sgBtnBmp)
		UiSetButtonBitmaps(window, BtnIDs, sgBtnBmp, &btnsize);

	// load progress bar textures
	UiLoadBmpFile(
		TEXT("ui_art\\prog_bg.pcx"),
		&sgProgBgBmp,
		&sgProgBgSize
	);
	UiLoadBmpFile(
		TEXT("ui_art\\prog_fil.pcx"),
		&sgProgFillBmp,
		&sgProgFillSize
	);

	// alloc/create a bitmap for the progress indicator
	child = GetDlgItem(window, IDC_UIGENERIC_PROGRESS);
	GetClientRect(child, &rect);

	sgProgBmp	  = (LPBYTE) ALLOC(rect.right * rect.bottom);
	if (!sgProgBmp)
		return;
		
	sgProgSize.cx = rect.right;
	sgProgSize.cy = rect.bottom;

	SDlgSetBitmap(
		child,
		NULL,
		NULL,
		SDLG_STYLE_ANY,
		SDLG_USAGE_BACKGROUND,
		sgProgBmp,
		NULL,
		rect.right,
		rect.bottom
	);
	ProgressDraw(window, 0);

	// set the text describing what progress is being monitored
	child = GetDlgItem(window, IDC_PROGRESS_TEXT);
	SetWindowText(child, progresstext);

	// show/hide cancel button based on abortable flag
	child = GetDlgItem(window, IDCANCEL);
	ShowWindow(child, (sgAbortable ? SW_SHOWNORMAL : SW_HIDE));
	EnableWindow(child, sgAbortable);
}


//****************************************************************************
//****************************************************************************
static void ProgressEndDialog(HWND window, int nResult) {
	sgnResult = IDCANCEL;
	sgbEndDialog = TRUE;
}

//****************************************************************************
//****************************************************************************
static void ProgressAbort(HWND window) {
	SDlgKillTimer(window, PROGRESS_TIMER_ID);
	if (SDrawGetFrameWindow() == GetParent(window))
		UiVidFadeOut(DEFAULT_STEPS*2);
	ProgressEndDialog(window, 0);
	sgnResult = 0;
}


//****************************************************************************
//****************************************************************************
static void ProgressTimer(HWND window) {
	int percent;

	percent = sgProgressFcn();

	if (percent >= 100) {
		ProgressAbort(window);
	}
	else {
		ProgressDraw(window, percent);
	}
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK ProgressDialogProc (HWND   window,
										UINT   message,
										WPARAM wparam,
										LPARAM lparam) {
	switch (message) {

		case WM_COMMAND:
			if (LOWORD(wparam) == IDCANCEL) {
				SDlgKillTimer(window, PROGRESS_TIMER_ID);

				if (SDrawGetFrameWindow() == GetParent(window))
					UiVidFadeOut(DEFAULT_STEPS*2);
				ProgressEndDialog(window, IDCANCEL);
			}
		break;

		case WM_DESTROY:
			ShowCursor(FALSE);
			ProgressDestroy(window);
		break;

		case WM_INITDIALOG:
			ProgressInit(window, (LPCSTR)lparam);

			// Choose our cursors
			UiSetWindowsCursors(SDrawGetFrameWindow());
			UiSetWindowsCursors(window);
			ShowCursor(TRUE);
		return 1;

		case WM_TIMER:
			ProgressTimer(window);
		break;

		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			SendMessage(SDrawGetFrameWindow(), message, wparam, lparam);
		break;
	}
	return SDlgDefDialogProc(window,message,wparam,lparam);
}


//****************************************************************************
//****************************************************************************
static void ProcessMessages(HWND hWnd) {
	while (IsWindow(hWnd) && !sgbEndDialog) {
		MSG message;
		if (PeekMessage(&message,(HWND)0,0,0,PM_REMOVE)) {
			if (message.message == WM_QUIT)
				PostQuitMessage(message.wParam);
			else if (!IsDialogMessage(hWnd,&message)) {
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}
		else {
			SDlgCheckTimers();
			SDlgUpdateCursor();
		}
	}

	// Get rid of the window if it is killing itself
	if (sgbEndDialog) {
		DestroyWindow(hWnd);
		sgbEndDialog = FALSE;
	}
}

//****************************************************************************
//*
//*  EXPORTED FUNCTIONS
//*
//****************************************************************************


//****************************************************************************
//****************************************************************************
BOOL APIENTRY UiProgressDialog(HWND			parent,
							   LPCSTR		progresstext,
							   BOOL			abortable,
							   PROGRESSFCN	progressfcn,
							   DWORD		callspersec
) {

	sgProgressFcn = progressfcn;
	sgAbortable = abortable;
	sgCallsPerSec = callspersec;
	sgnResult = -1;
	sgbEndDialog = FALSE;

	HWND hWndProgress = SDlgCreateDialogParam(
		global_hinstance,
		TEXT("PROGRESS_DIALOG"),
		parent,
		ProgressDialogProc,
		(LPARAM) progresstext
	);

	if (!hWndProgress)
		return FALSE;

	// Now process messages until dialog exits 
	// (that way we'll behave modal to the outside world of Diablo.exe)
	ProcessMessages(hWndProgress);

	if (sgnResult == IDCANCEL || sgnResult == -1)
		return FALSE;

	return TRUE;
}

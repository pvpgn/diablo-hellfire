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
	static sgszProgressText[256];


//****************************************************************************
//****************************************************************************
static void ProgressDraw(HWND window, int percent) {
	RECT rect;
	HWND child = GetDlgItem(window, IDC_UIGENERIC_PROGRESS);

	if (!sgProgBmp || !sgProgBgBmp)
		return;

	// Bound percent
	if (percent > 100)
		percent = 100;
	if (percent < 0)
		percent = 0;


	// draw the bg
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
static void ProgressInit(HWND window, SNETGETARTPROC artcallback) {
	HWND child;
	RECT rect;
	SIZE btnsize;
	int BtnIDs[] = { IDCANCEL, 0 };

	if (sgCallsPerSec)
		SDlgSetTimer(window,PROGRESS_TIMER_ID,USER_DELAY,NULL);
	else
		SDlgSetTimer(window,PROGRESS_TIMER_ID,DEFAULT_DELAY,NULL);


	// load ctrl/dlg bmps
	UiLoadArtwork(
		artcallback,
		window,
		GetParent(window),
		SNET_ART_POPUPBACKGROUND_SML,
		TEXT("Popup"),
		SDLG_STYLE_ANY,
		SDLG_USAGE_BACKGROUND,
		FALSE,
		FALSE,
		&sgBgBmp,
		NULL
	);

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
			&sgBtnBmp,
			&btnsize);

	SDlgSetControlBitmaps (window, BtnIDs, NULL, sgBtnBmp, &btnsize, SDLG_ADJUST_VERTICAL);


	// load progress bar textures
	UiLoadArtwork(
			artcallback,
			NULL,
			NULL,
			SNET_ART_PROGRESS_BACKGROUND,
			0,
			0,
			0,	
			FALSE,				
			FALSE,
			&sgProgBgBmp,
			&sgProgBgSize);

	UiLoadArtwork(
			artcallback,
			NULL,
			NULL,
			SNET_ART_PROGRESS_FILLER,
			0,
			0,
			0,	
			FALSE,				
			FALSE,
			&sgProgFillBmp,
			&sgProgFillSize);


	// alloc/create a bitmap for the progress indicator
	child = GetDlgItem(window, IDC_UIGENERIC_PROGRESS);
	GetClientRect(child, &rect);

	sgProgBmp	  = (LPBYTE) ALLOC(rect.right * rect.bottom);
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
	SetWindowText(child, (LPCSTR)sgszProgressText);

	// show/hide cancel button based on abortable flag
	child = GetDlgItem(window, IDCANCEL);
	ShowWindow(child, (sgAbortable ? SW_SHOWNORMAL : SW_HIDE));
	EnableWindow(child, sgAbortable);
}


//****************************************************************************
//****************************************************************************
static void ProgressTimer(HWND window) {
	int percent = 0;

	if (sgProgressFcn) 
		percent = sgProgressFcn();

	ProgressDraw(window, percent);
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK ProgressDialogProc (HWND   window,
										UINT   message,
										WPARAM wparam,
										LPARAM lparam) {
	static SNETUIDATAPTR interfacedata;
	switch (message) {

		case WM_COMMAND:
			if (LOWORD(wparam) == IDCANCEL) {
				SrvCancel();
				SDlgEndDialog(window, 0);
				return 0;
			}
		break;

		case WM_DESTROY:
			SDlgKillTimer(window, PROGRESS_TIMER_ID);
			ProgressDestroy(window);
		break;

		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			SendMessage(SDrawGetFrameWindow(), message, wparam, lparam);
		break;

		case WM_INITDIALOG:
			interfacedata = (SNETUIDATAPTR) lparam;
			ProgressInit(window, interfacedata->artcallback);
		return 1;

		case WM_TIMER:
			ProgressTimer(window);
		break;
	}
	return SDlgDefDialogProc(window,message,wparam,lparam);
}


//****************************************************************************
//*
//*  EXPORTED FUNCTIONS
//*
//****************************************************************************


//****************************************************************************
//****************************************************************************
HWND UiModelessProgressDialog(SNETUIDATAPTR interfacedata,
							   LPCSTR		progresstext,
							   BOOL			abortable,
							   PROGRESSFCN	progressfcn,
							   DWORD		callspersec
) {
	sgProgressFcn = progressfcn;
	sgAbortable = abortable;
	sgCallsPerSec = callspersec;

	if (strlen(progresstext) < sizeof(sgszProgressText))
		strcpy((LPSTR)sgszProgressText, progresstext);

	if (!interfacedata)
		return NULL;

	return SDlgCreateDialogParam(
				global_hinstance,
				TEXT("DIALOG_PROGRESS"),
				interfacedata->parentwindow,
				ProgressDialogProc,
				(LPARAM) interfacedata
	);

}

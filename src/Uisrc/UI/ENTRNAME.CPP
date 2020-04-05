//****************************************************************************
//	EntrName.cpp
//	Diablo UI enter name for a new hero
//	
//	By Frank Pearce
//	created 10.20.96
//****************************************************************************


#include "pch.h"
#include "artfont.h"
#include "uisnd.h"


//****************************************************************************
//****************************************************************************
	extern void SelHeroSetTitle(HWND window, LPCTSTR title);
	extern int SelHeroGetMode(void);


//****************************************************************************
//****************************************************************************
	#define MILLISEC_PER_SEC	1000

	#define FOCUS_TIMER_ID		1
	#define FOCUS_FPS			16		// frames per second
	#define FOCUS_TIMER_DELAY	55		//(MILLISEC_PER_SEC / FOCUS_FPS)

	#define RESTRICTED_CHARS	TEXT(" ,<>%&\\\"?*#/")


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
	static int sgEditIDs[] = {
		IDC_NAMEEDIT,
		0
	};

	static LPSTR sgName;


//****************************************************************************
//****************************************************************************
static void EntrNameDestroy(HWND window) {
	FocusAnimateDestroy();
	UiDoomCtrlsDestroy(window, sgEditIDs);
	UiDoomCtrlsDestroy(window, sgBtnIDs);
	UiDoomCtrlsDestroy(window, sgTextIDs);
	SelHeroSetTitle(GetParent(window), NULL);
}


//****************************************************************************
//****************************************************************************
static void EntrNameInit(HWND window) {
	TCHAR buf[32];
	HWND child;
	HWND parent = GetParent(window);

	// set the title for the parent
	if (SelHeroGetMode() == MULTIPLAYER)
		LoadString(global_hinstance, IDS_NEWMULTHERO, buf, 32-1);
	else
		LoadString(global_hinstance, IDS_NEWSINGHERO, buf, 32-1);
	SelHeroSetTitle(parent, buf);

	// point this window at the same bmp it's parent uses
	SetWindowLong(
		window,
		GWL_USERDATA,
		GetWindowLong(parent, GWL_USERDATA)
	);

	// set up a doom-like interface
	UiDoomStaticInit(window, sgTextIDs, AF_BIGGRAY);
	UiDoomButtonsInit(window, sgBtnIDs, AF_BIG, FALSE);
	UiDoomEditInit(window, sgEditIDs, AF_MED);

	// set up the animating focus indicator
	FocusAnimateInit("ui_art\\focus.pcx");
	SDlgSetTimer(window, FOCUS_TIMER_ID, FOCUS_TIMER_DELAY, NULL);

	child = GetDlgItem(window, IDC_NAMEEDIT);
	// limit the amout of text that can be typed
	SendMessage(child, DEM_LIMITTEXT, MAX_NAME_LEN-1, 0);
	// restritc chars that can be entered
	if (SelHeroGetMode() == MULTIPLAYER) {
		SendMessage(child, DEM_RESTRICTCHARS, 0, (LPARAM)RESTRICTED_CHARS);
	}
}


//****************************************************************************
//****************************************************************************
static void EntrNameAbort(HWND window, int ReturnVal) {
	UiSndPlayEnter();
	SDlgKillTimer(window, FOCUS_TIMER_ID);

	// get the name text from the edit
	GetWindowText(
		GetDlgItem(window, IDC_NAMEEDIT),
		sgName,
		MAX_NAME_LEN
	);
	sgName[MAX_NAME_LEN-1] = 0;
	
	SDlgEndDialog(window, ReturnVal);
}


//****************************************************************************
//****************************************************************************
static void EntrNameDEN(HWND window, WPARAM wparam, LPARAM lparam) {
	switch (HIWORD(wparam)) {
		case DEN_UPDATE:
			UiDoomEditReset(window, sgEditIDs, AF_MED);
		break;
		case DEN_SETFOCUS:
			FocusAnimate(window, GetFocus());
		break;
	}
}


//****************************************************************************
//****************************************************************************
BOOL CALLBACK EnterNameDialogProc(HWND window,
								UINT message,
								WPARAM wparam,
								LPARAM lparam) {
	switch (message) {

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDOK:
					EntrNameAbort(window, IDOK);
				break;
				case IDCANCEL:
					EntrNameAbort(window, IDCANCEL);
				break;
				case IDC_NAMEEDIT:
					EntrNameDEN(window, wparam, lparam);
				break;
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
				EntrNameAbort(window, IDOK);
			}
			else if (
				UiIsPtInWindow(
					window,
					GetDlgItem(window, IDC_FAKECANCEL),
					LOWORD(lparam),
					HIWORD(lparam)
				)
			) {
				EntrNameAbort(window, IDCANCEL);
			}
		break;

		case WM_DESTROY:
			EntrNameDestroy(window);
		break;

		case WM_INITDIALOG:
			sgName = (LPSTR) lparam;
			EntrNameInit(window);
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

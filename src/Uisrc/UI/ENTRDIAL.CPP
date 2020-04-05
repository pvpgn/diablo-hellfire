//****************************************************************************
//	EntrDial.cpp
//	Diablo UI enter dial string for modem
//	
//	By Frank Pearce
//	created 11.23.96
//****************************************************************************


#include "pch.h"
#include "artfont.h"
#include "uisnd.h"


//****************************************************************************
//****************************************************************************
	extern void ModemSetInfo(HWND window, LPCSTR infotitle, LPCSTR text);


//****************************************************************************
//****************************************************************************
	#define MILLISEC_PER_SEC	1000

	#define FOCUS_TIMER_ID		1
	#define FOCUS_FPS			16		// frames per second
	#define FOCUS_TIMER_DELAY	55		//(MILLISEC_PER_SEC / FOCUS_FPS)

	#define RESTRICTED_CHARS	TEXT("<>%&?")



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
		IDC_DIALEDIT,
		0
	};

	static LPSTR sgDialString;


//****************************************************************************
//****************************************************************************
static void EntrDialSetInfo(HWND window) {
	TCHAR title[64], desc[256];

	LoadString(global_hinstance, IDS_ENTERDIAL_TITLE, title, 63);
	LoadString(global_hinstance, IDS_ENTERDIAL_INFO, desc, 255);
	ModemSetInfo(GetParent(window), title, desc);
}


//****************************************************************************
//****************************************************************************
static void EntrDialDestroy(HWND window) {
	FocusAnimateDestroy();
	UiDoomCtrlsDestroy(window, sgEditIDs);
	UiDoomCtrlsDestroy(window, sgBtnIDs);
	UiDoomCtrlsDestroy(window, sgTextIDs);
	ModemSetInfo(GetParent(window), NULL, NULL);
}


//****************************************************************************
//****************************************************************************
static void EntrDialInit(HWND window) {
	HWND child;
	HWND parent = GetParent(window);

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

	child = GetDlgItem(window, IDC_DIALEDIT);
	// limit the amout of text that can be typed
	SendMessage(child, DEM_LIMITTEXT, MAX_DIAL_LEN-1, 0);

	// restritc chars that can be entered
	SendMessage(child, DEM_RESTRICTCHARS, 0, (LPARAM)RESTRICTED_CHARS);

	EntrDialSetInfo(window);
}


//****************************************************************************
//****************************************************************************
static void EntrDialAbort(HWND window, int ReturnVal) {
	UiSndPlayEnter();
	SDlgKillTimer(window, FOCUS_TIMER_ID);

	// get the dial string from the edit
	GetWindowText(
		GetDlgItem(window, IDC_DIALEDIT),
		sgDialString,
		MAX_DIAL_LEN
	);
	sgDialString[MAX_DIAL_LEN-1] = 0;
	
	SDlgEndDialog(window, ReturnVal);
}


//****************************************************************************
//****************************************************************************
static void EntrDialDEN(HWND window, WPARAM wparam, LPARAM lparam) {
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
static void EntrDialInterpretClick(HWND window, int x, int y) {
	if (UiIsPtInWindow(window, GetDlgItem(window, IDC_FAKEOK), x, y)) {
		EntrDialAbort(window, IDOK);
	}
	else if (UiIsPtInWindow(window, GetDlgItem(window, IDC_FAKECANCEL), x, y)) {
		EntrDialAbort(window, IDCANCEL);
	}
}


//****************************************************************************
//****************************************************************************
BOOL CALLBACK EnterDialDialogProc(HWND window,
								UINT message,
								WPARAM wparam,
								LPARAM lparam) {
	switch (message) {

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDOK:
					EntrDialAbort(window, IDOK);
				break;
				case IDCANCEL:
					EntrDialAbort(window, IDCANCEL);
				break;
				case IDC_DIALEDIT:
					EntrDialDEN(window, wparam, lparam);
				break;
			}
		break;

		case WM_LBUTTONDOWN:
			EntrDialInterpretClick(window, LOWORD(lparam), HIWORD(lparam));
		break;

		case WM_DESTROY:
			EntrDialDestroy(window);
		break;

		case WM_INITDIALOG:
			sgDialString = (LPSTR) lparam;
			EntrDialInit(window);
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

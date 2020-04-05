//****************************************************************************
//	SelYesNo.cpp
//	Diablo UI select hero "yes/no" dialog
//	
//	By Frank Pearce
//	created 10.21.96
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
		IDC_OKCANCEL_TEXT,
		0
	};
	static int sgBtnIDs[] = {
		IDC_OK,
		IDCANCEL,
		0
	};

	static LPCTSTR  sgTitle;
	static LPCTSTR  sgYesNoText;
	static BOOL	    sgLoadFocus;
	static BOOL		sgMainMenuBG;
	static SNETABORTPROC sgpAbortFcn;


//****************************************************************************
//****************************************************************************
static void SelYesNoDestroy(HWND window) {
	UiFreeBmp((TPBMP) GetWindowLong(window, GWL_USERDATA));
	SetWindowLong(window, GWL_USERDATA, (LONG) 0);

	if (sgLoadFocus)
		FocusAnimateDestroy();
	UiDoomCtrlsDestroy(window, sgBtnIDs);
	UiDoomCtrlsDestroy(window, sgTextIDs);
	if (sgTitle) {
		SelHeroSetTitle(GetParent(window), NULL);
	}
}


//****************************************************************************
//****************************************************************************
static void SelYesNoInit(HWND window) {
	TPBMP tpBmp;
	HWND parent = GetParent(window);

	if (sgTitle)
		SelHeroSetTitle(parent, sgTitle);

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
			(sgMainMenuBG ? 
				(UiIsSpawned() ? TEXT("ui_art\\swmmpop.pcx") : TEXT("ui_art\\mmpopup.pcx")) : 
				TEXT("ui_art\\black.pcx")
			),
			&tpBmp->data,
			&tpBmp->datasize
		);
	}

	SetWindowText(GetDlgItem(window, IDC_OKCANCEL_TEXT), sgYesNoText);

	// set up a doom-like interface
	UiDoomStaticInit(window, sgTextIDs, AF_MEDGRAY);
	UiDoomButtonsInit(window, sgBtnIDs, AF_BIG);

	// set up the animating focus indicator
	if (sgLoadFocus)
		FocusAnimateInit("ui_art\\focus.pcx");
	else
		FocusReInit();
	SDlgSetTimer(window, FOCUS_TIMER_ID, FOCUS_TIMER_DELAY, NULL);

	UiActiveDownBtns(window, sgBtnIDs);
}


//****************************************************************************
//****************************************************************************
static void SelYesNoAbort(HWND window, int ReturnVal) {
	UiSndPlayEnter();
	SDlgKillTimer(window, FOCUS_TIMER_ID);

	if ((ReturnVal == IDCANCEL) && (sgpAbortFcn))
		sgpAbortFcn();

	if (ReturnVal == IDOK)
		// return the id of the focused button
		ReturnVal = GetWindowLong(GetFocus(), GWL_ID);

	SDlgEndDialog(window, ReturnVal);
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK SelYesNoDialogProc(HWND window,
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
			   	switch (GetWindowLong(GetFocus(), GWL_ID)) {
			   		case IDC_OK:
			   			SelYesNoAbort(window, IDOK);
			   		break;
			   		default:
			   			SelYesNoAbort(window, IDCANCEL);
			   		break;
			   	}
			}
			else if (LOWORD(wparam) == IDCANCEL) {
				SelYesNoAbort(window, IDCANCEL);
			}
			else if (LOWORD(wparam) == IDC_OK) {
				SelYesNoAbort(window, IDOK);
			}
		break;

		case WM_DESTROY:
			SelYesNoDestroy(window);
		break;

		case WM_INITDIALOG:
			SelYesNoInit(window);
		return 0;

		case WM_TIMER:
			if (! FocusAnimate(window, GetFocus())) {
				HWND child = GetDlgItem(window, IDC_OK);
				if (child)
					SetFocus(child);
				else
					SetFocus(GetDlgItem(window, IDCANCEL));
			}
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
int SelYesNoDialog(HWND parent, LPCTSTR text, LPCTSTR title, BOOL loadfocus) {
	sgTitle	= title;
	sgYesNoText	= text;
	sgLoadFocus	= loadfocus;
	sgMainMenuBG = FALSE;
	sgpAbortFcn	= NULL;

	return SDlgDialogBox(
		global_hinstance,
		TEXT("SELYESNO_DIALOG"),
		parent,
		SelYesNoDialogProc
	);
}


//****************************************************************************
//****************************************************************************
int SelOkDialog(HWND parent, LPCTSTR text, LPCTSTR title, BOOL loadfocus) {
	sgTitle		= title;
	sgYesNoText	= text;
	sgLoadFocus	= loadfocus;
	sgMainMenuBG = FALSE;
	sgpAbortFcn	= NULL;

	return SDlgDialogBox(
		global_hinstance,
		TEXT("SELOK_DIALOG"),
		parent,
		SelYesNoDialogProc
	);
}


//****************************************************************************
//****************************************************************************
int SpawnErrDialog(HWND parent, int errstrid, BOOL onmainmenu) {
	TCHAR buf[256];

	LoadString(global_hinstance, errstrid, buf, 255);
	sgLoadFocus	= FALSE;
	sgMainMenuBG = onmainmenu;
	sgTitle		= NULL;
	sgYesNoText	= buf;
	sgpAbortFcn	= NULL;

	return SDlgDialogBox(
		global_hinstance,
		TEXT("SPAWNERR_DIALOG"),
		parent,
		SelYesNoDialogProc
	);
}



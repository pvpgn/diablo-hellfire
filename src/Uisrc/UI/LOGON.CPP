//****************************************************************************
//	Logon.cpp
//	Diablo UI log on to battlenet popup dialog
//	
//	By Frank Pearce
//	created 9.25.96
//****************************************************************************


#include "pch.h"


//****************************************************************************
//****************************************************************************
	#define DEFAULT_LEN		16

	static LPBYTE sgBgBmp = NULL;
	static LPBYTE sgBtnBmp = NULL;

	static LPSTR sgName;
	static LPSTR sgPassword;
	static UINT  sgNameLen;
	static UINT  sgPasswordLen;


//****************************************************************************
//****************************************************************************
static void LogonSaveInfo(HWND window) {
	HWND edit;

	// get the logon name
	if (sgName) {
		edit = GetDlgItem(window, IDC_LOGONNAME_EDIT);
		GetWindowText(edit, sgName, sgNameLen);
	}

	// get the logon password
	if (sgPassword) {
		edit = GetDlgItem(window, IDC_LOGONPASSWORD_EDIT);
		GetWindowText(edit, sgPassword, sgPasswordLen);
	}
}


//****************************************************************************
//****************************************************************************
static void LogonClearInfo(HWND window) {
	// clear the logon name
	if (sgName)
		*sgName = 0;

	// clear the logon password
	if (sgPassword)
		*sgPassword = 0;
}


//****************************************************************************
//****************************************************************************
static void LogonDestroy(HWND window) {
	if (sgBgBmp) {
		FREE(sgBgBmp);
		sgBgBmp = NULL;
	}
	if (sgBtnBmp) {
		FREE(sgBtnBmp);
		sgBtnBmp = NULL;
	}
}


//****************************************************************************
//****************************************************************************
static void LogonInit(HWND window) {
	HWND edit;

	// load ctrl/dlg bmps
	LoadArtFile(
		window,
		GetParent(window),
		TEXT("Popup"),
		SDLG_STYLE_ANY,
		SDLG_USAGE_BACKGROUND,
		TEXT("ui_art\\lpopup.pcx"),
		&sgBgBmp,
		NULL
	);
	LoadArtFile(
		NULL,
		window,
		TEXT("Button"),
		SDLG_STYLE_ANYPUSHBUTTON,
		SDLG_USAGE_BACKGROUND,
		TEXT("ui_art\\button.pcx"),
		&sgBtnBmp,
		NULL
	);

	// make sure there were some lengths set
	if (!sgNameLen) sgNameLen = DEFAULT_LEN;
	if (!sgPasswordLen) sgPasswordLen = DEFAULT_LEN;

	// limit length of logon name
	edit = GetDlgItem(window, IDC_LOGONNAME_EDIT);
	SendMessage(edit, EM_LIMITTEXT, sgNameLen, 0);
	SetFocus(edit);

	// limit length of logon password
	edit = GetDlgItem(window, IDC_LOGONPASSWORD_EDIT);
	SendMessage(edit, EM_LIMITTEXT, sgNameLen, 0);

	return;
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK LogonDialogProc (HWND   window,
								   UINT   message,
								   WPARAM wparam,
								   LPARAM lparam) {
	switch (message) {
		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDOK:
					LogonSaveInfo(window);
					SDlgEndDialog(window,IDOK);
				break;
				case IDCANCEL: 
					LogonClearInfo(window);
					SDlgEndDialog(window,IDCANCEL);
				break;
		}
		break;

		case WM_CTLCOLORSTATIC:
			if (GetWindowLong((HWND)lparam, GWL_ID) == IDC_DLGTITLE) {
				UiSetTextYellow((HDC)wparam);
				return (BOOL) GetStockObject(NULL_BRUSH);
			}
		break;

		case WM_DESTROY:
			LogonDestroy(window);
		break;

		case WM_INITDIALOG:
			LogonInit(window);
		return 1;

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
BOOL APIENTRY UiLogonDialog(HWND	parent,
							DWORD	*selection,
							LPSTR	logonname,
							UINT	maxnamelen,
							LPSTR	logonpassword,
							UINT	maxpasswordlen
) {
	DWORD result;

	sgName = logonname;
	sgPassword = logonpassword;
	sgNameLen = maxnamelen;
	sgPasswordLen = maxpasswordlen;

	result = SDlgDialogBox(
		global_hinstance,
		TEXT("LOGON_DIALOG"),
		parent,
		LogonDialogProc
	);

	if (selection)
		*selection = result;

	return 1;
}

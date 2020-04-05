//****************************************************************************
//	OkCancel.cpp
//	Diablo UI ok/cancel dialog
//	
//	By Frank Pearce
//	created 9.23.96
//****************************************************************************

#include "pch.h"

#include "uisnd.h"
//****************************************************************************
//****************************************************************************
#define TEXT_NO_FIT 0xff000000l		// indicates that dialog was too small to accomodate text

	enum _sizes {
		SIZE_NORMAL,
		SIZE_BIG,
		NUM_SIZES
	};

	typedef struct _dlg_desc {
		char szTemplate[32];
	} DLG_DESC;

	static LPBYTE sgBgBmp = NULL;
	static LPBYTE sgBtnBmp = NULL;
	static LPCSTR sgTitle;
	static int sgSize;
	static BOOL sgErrorFlag;
	static BOOL sgTestSize = FALSE;


//****************************************************************************
//****************************************************************************
static void OkCancelDestroy(HWND window) {
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
BOOL IsTextTooLarge(HWND hWnd, LPCSTR szText) {
	HDC hDC;
	RECT r, rSave;


	// If there is no text, size is ok
	if (!szText || !szText[0])
		return FALSE;

	if (!hWnd)  return TRUE;

	GetClientRect(hWnd, &r);
	r.right--;
	r.bottom--;
	rSave = r;

	hDC = GetDC(hWnd);
	// Select the font for this window
	HFONT hOldFont = (HFONT)SelectObject(hDC, (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0));
	if (hDC) {
		DrawText(hDC, szText, strlen(szText), &r, DT_CALCRECT | DT_WORDBREAK); 

		if (hOldFont)
			SelectObject(hDC, hOldFont);
		ReleaseDC(hWnd, hDC);


		if (r.bottom > rSave.bottom || r.right > rSave.right)
			return TRUE;
	}

	return FALSE;
}
//****************************************************************************
// OkCancelInit()
// 
// if sgTestSize is set, this routine will verify whether the text fits
// in the allotted text controls.  
//
// A return value of 0 indicates that the dialog is too small.
// A return value of 1 indicates success.
//****************************************************************************
static BOOL OkCancelInit(HWND window, LPCSTR okcanceltext) {
	SIZE btnsize;
	int BtnIDs[] = { IDC_OK, IDCANCEL, 0 };
	LPCTSTR popupfile;
	LPCTSTR spopup = TEXT("ui_art\\spopup.pcx");
	LPCTSTR lpopup = TEXT("ui_art\\lpopup.pcx");
	LPCTSTR srpopup = TEXT("ui_art\\srpopup.pcx");
	LPCTSTR lrpopup = TEXT("ui_art\\lrpopup.pcx");
	HWND child;

	if (sgSize == SIZE_NORMAL) {
		if (sgErrorFlag)	popupfile = srpopup;
		else				popupfile = spopup;
	}
	else { //(sgSize == SIZE_BIG)
		if (sgErrorFlag)	popupfile = lrpopup;
		else				popupfile = lpopup;
	}

	if (SDrawGetFrameWindow() == GetParent(window)) {
		PALETTEENTRY *pal;

	   	LoadArtFile(
	   		window,
	   		NULL,
	   		TEXT(""),
	   		SDLG_STYLE_ANY,
	   		SDLG_USAGE_BACKGROUND,
			popupfile,
	   		&sgBgBmp,
	   		FALSE
	   	);

		// load just the part of the palette the dialog uses
		pal = UiPaletteEntry(0);
		SDrawUpdatePalette(0,    10, &pal[0]);		// Windows colors also
		SDrawUpdatePalette(0x70, 144, &pal[0x70], TRUE);
	}
	else {
	   	LoadArtFile(
	   		window,
	   		GetParent(window),
	   		TEXT("Popup"),
	   		SDLG_STYLE_ANY,
	   		SDLG_USAGE_BACKGROUND,
			popupfile,
	   		&sgBgBmp,
	   		NULL
	   	);
	}

	if (SDrawGetFrameWindow() == GetParent(window))
		UiAltFadeInit(window);

	UiLoadBmpFile(
		TEXT("ui_art\\but_sml.pcx"),
		&sgBtnBmp,
		&btnsize
	);
	UiSetButtonBitmaps(window, BtnIDs, sgBtnBmp, &btnsize);

	child = GetDlgItem(window, IDC_OKCANCEL_TEXT);
	SetWindowText(child, okcanceltext);

	if (sgTestSize && IsTextTooLarge(child, okcanceltext)) 
		return 0;

	if (sgTitle != NULL) {
		child = GetDlgItem(window, IDC_DLGTITLE);

		if (sgTestSize && IsTextTooLarge(child, sgTitle))
			return 0;

		if (child)
			SetWindowText(child, sgTitle);
	}

	return 1;
}


//****************************************************************************
//****************************************************************************
static void OkCancelAbort(HWND window, int retval) {
	UiSndPlayEnter();
	SDlgEndDialog(window, retval);
}

//****************************************************************************
//****************************************************************************
static BOOL CALLBACK OkCancelDialogProc (HWND   window,
										UINT   message,
										WPARAM wparam,
										LPARAM lparam) {
	switch (message) {
	
		case WM_DESTROY:
			ShowCursor(FALSE);
			OkCancelDestroy(window);
			if (SDrawGetFrameWindow() == GetParent(window))
				UiVidFadeOut(DEFAULT_STEPS*2);
		break;
		
		case WM_INITDIALOG:
			// Make sure cursor is visible
			ShowCursor(TRUE);
			if (!OkCancelInit(window, (LPCSTR)lparam))
				SDlgEndDialog(window, TEXT_NO_FIT);
		return 1;
		
		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDOK:
					switch (GetWindowLong(GetFocus(), GWL_ID)) {
						case IDC_OK:
							OkCancelAbort(window, IDOK);
						break;
						default:
							OkCancelAbort(window, IDCANCEL);
						break;
					}
				break;
				case IDC_OK:
					OkCancelAbort(window, IDOK);
				break;
				case IDCANCEL:
					OkCancelAbort(window, IDCANCEL);
				break;
			}
		break;

		case WM_CTLCOLORSTATIC:
			if (GetWindowLong((HWND)lparam, GWL_ID) == IDC_DLGTITLE) {
				UiSetTextYellow((HDC)wparam);
				return (BOOL) GetStockObject(NULL_BRUSH);
			}
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
int UiOkCancelDialog(HWND parent, LPCSTR okcanceltext, BOOL errorflag) {

	sgSize = SIZE_NORMAL;
	sgTitle = NULL;
	sgErrorFlag = errorflag;
	sgTestSize = FALSE;

	return SDlgDialogBoxParam(
		global_hinstance,
		TEXT("OKCANCEL_DIALOG"),
		parent,
		OkCancelDialogProc,
		(LPARAM)okcanceltext
	);
}


//****************************************************************************
//****************************************************************************
int UiOkDialog(HWND parent, LPCSTR oktext, BOOL errorflag) {

	sgSize = SIZE_NORMAL;
	sgTitle = NULL;
	sgErrorFlag = errorflag;
	sgTestSize = FALSE;

	return SDlgDialogBoxParam(
		global_hinstance,
		TEXT("OK_DIALOG"),
		parent,
		OkCancelDialogProc,
		(LPARAM)oktext
	);
}


//****************************************************************************
//****************************************************************************
int UiBigOkCancelDialog(HWND parent, LPCSTR oktext, LPCSTR title, BOOL errorflag) {

	sgSize = SIZE_BIG;
	sgTitle = title;
	sgErrorFlag = errorflag;
	sgTestSize = FALSE;

	return SDlgDialogBoxParam(
		global_hinstance,
		TEXT("BIGOKCANCEL_DIALOG"),
		parent,
		OkCancelDialogProc,
		(LPARAM)oktext
	);
}


//****************************************************************************
//****************************************************************************
int UiBigOkDialog(HWND parent, LPCSTR oktext, LPCSTR title, BOOL errorflag) {

	sgSize = SIZE_BIG;
	sgTitle = title;
	sgErrorFlag = errorflag;
	sgTestSize = FALSE;

	return SDlgDialogBoxParam(
		global_hinstance,
		TEXT("BIGOK_DIALOG"),
		parent,
		OkCancelDialogProc,
		(LPARAM)oktext
	);
}

//****************************************************************************
//***
//***  EXPORTED FUNCTIONS
//***
//****************************************************************************

static DLG_DESC sgOkDlgs[NUM_SIZES] = {
	{ TEXT("OK_DIALOG") },
	{ TEXT("BIGOK_DIALOG") },
};

static DLG_DESC sgOkCancelDlgs[NUM_SIZES] = {
	{ TEXT("OKCANCEL_DIALOG") },
	{ TEXT("BIGOKCANCEL_DIALOG") },
};

//****************************************************************************
//****************************************************************************
BOOL CALLBACK UiMessageBoxCallback(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType) {
	DWORD dwReturn;
	DLG_DESC *pDlgInfo;

	sgTitle = lpCaption;

	// choose between MB_OK and MB_OKCANCEL
	if ((uType & MB_TYPEMASK) == MB_OK) 
		pDlgInfo = sgOkDlgs;
	else 
		pDlgInfo = sgOkCancelDlgs;

	// Set dialog error flag
	switch (uType & MB_ICONMASK) {
		case MB_ICONWARNING:
		case MB_ICONERROR:
			sgErrorFlag = TRUE;
			break;

		default:
			sgErrorFlag = FALSE;
	}


	sgTestSize = TRUE;
	for (sgSize=0; sgSize<NUM_SIZES; sgSize++) {

		dwReturn = SDlgDialogBoxParam(
							global_hinstance,
							pDlgInfo[sgSize].szTemplate,
							hWnd,
							OkCancelDialogProc,
							(LPARAM)lpText);

		// If the text did not fit, try the next size up
		if (dwReturn != TEXT_NO_FIT)
			break;
	}

	if (dwReturn == TEXT_NO_FIT)
		return MessageBox(hWnd, lpText, lpCaption, uType);
	
	return dwReturn;
}

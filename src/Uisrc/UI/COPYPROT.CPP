//****************************************************************************
//	CopyProt.cpp
//	Diablo UI copy protection dialog
//	
//	By Frank Pearce
//	created 11.18.96
//****************************************************************************

#include "pch.h"
#include "uisnd.h"


//****************************************************************************
//****************************************************************************
	static HGLOBAL sghBgRes;
	static HGLOBAL sghBtnRes;
	static HGLOBAL sghPalRes;


//****************************************************************************
//****************************************************************************
static void CopyProtDestroy(HWND window) {

	// free the art resources
	if (sghPalRes) {
		FreeResource(sghPalRes);
		sghPalRes = NULL;
	}
	if (sghBtnRes) {
		FreeResource(sghBtnRes);
		sghBtnRes = NULL;
	}
	if (sghBgRes) {
		FreeResource(sghBgRes);
		sghBgRes = NULL;
	}
}


//****************************************************************************
//****************************************************************************
static BOOL CopyProtInit(HWND window, LPCSTR copyprottext) {
	LPBYTE BgBmp;
	LPBYTE BtnBmp;
	PALETTEENTRY * BgPal;
	PALETTEENTRY LoadedPal[PALETTE_REGISTERS];
	SIZE BgSize  = { 284, 148 };
	SIZE BtnSize = { 112, 140 };

	int BtnIDs[] = { IDC_OK, IDCANCEL, 0 };
	LPCTSTR srpopup = TEXT("ui_art\\srpopup.pcx");
	HWND child;

	// load the art resources
	sghBgRes = LoadResource(
		global_hinstance,
		FindResource(global_hinstance, TEXT("IDR_POPUPART"), TEXT("ART_FILES"))
	);
	sghBtnRes = LoadResource(
		global_hinstance,
		FindResource(global_hinstance, TEXT("IDR_BTNART"), TEXT("ART_FILES"))
	);
	sghPalRes = LoadResource(
		global_hinstance,
		FindResource(global_hinstance, TEXT("IDR_ARTPAL"), TEXT("ART_FILES"))
	);

	// lock the loaded resources
	BgBmp = (LPBYTE) LockResource(sghBgRes);
	BtnBmp = (LPBYTE) LockResource(sghBtnRes);
	BgPal = (PALETTEENTRY *) LockResource(sghPalRes);

	// register the images
	if (BgBmp) {
		SDlgSetBitmap(
			window,
			NULL,
			TEXT(""),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,
			BgBmp,
			NULL,
			BgSize.cx,
			BgSize.cy
		);
	}

	ShowCursor(TRUE);
	UiAltFadeInit(window);

	if (BtnBmp)
		UiSetButtonBitmaps(window, BtnIDs, BtnBmp, &BtnSize);

	// load the palette
	if (BgPal) {
		memcpy(LoadedPal, BgPal, sizeof(PALETTEENTRY) * PALETTE_REGISTERS);
		SDrawUpdatePalette(0, PALETTE_REGISTERS, LoadedPal, TRUE);
	}

	child = GetDlgItem(window, IDC_OKCANCEL_TEXT);
	SetWindowText(child, copyprottext);

	return 1;
}


//****************************************************************************
//****************************************************************************
static void CopyProtAbort(HWND window, int retval) {
	ShowCursor(FALSE);
	SDlgEndDialog(window, retval);
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK CopyProtDialogProc (HWND   window,
										UINT   message,
										WPARAM wparam,
										LPARAM lparam) {
	switch (message) {
	
		case WM_DESTROY:
			CopyProtDestroy(window);
			UiVidFadeOut(DEFAULT_STEPS*2);
		break;
		
		case WM_INITDIALOG:
			CopyProtInit(window, (LPCSTR)lparam);
		return 1;
		
		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDOK:
					switch (GetWindowLong(GetFocus(), GWL_ID)) {
						case IDC_OK:
							CopyProtAbort(window, IDOK);
						break;
						default:
							CopyProtAbort(window, IDCANCEL);
						break;
					}
				break;
				case IDC_OK:
					CopyProtAbort(window, IDOK);
				break;
				case IDCANCEL:
					CopyProtAbort(window, IDCANCEL);
				break;
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
BOOL APIENTRY UiCopyProtError(DWORD *result) {
	TCHAR copyproterror[128];
	DWORD dwResult;

	if (! UiIsSpawned())
		LoadString(global_hinstance, IDS_COPYPROT_ERR, copyproterror, 127);
	else
		LoadString(global_hinstance, IDS_SPAWNDATA_ERR, copyproterror, 127);

	dwResult = SDlgDialogBoxParam(
		global_hinstance,
		TEXT("OKCANCEL_DIALOG"),
		SDrawGetFrameWindow(),
		CopyProtDialogProc,
		(LPARAM)copyproterror
	);

	if (result)
		*result = dwResult;

	return 1;
}
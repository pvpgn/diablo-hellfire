//****************************************************************************
//	SelSing.cpp
//	Diablo UI select dialog
//	
//	By Frank Pearce
//	created 9.17.96
//****************************************************************************


#include "pch.h"


//****************************************************************************
//****************************************************************************
static SIZE bgsize;
static LPBYTE backgroundbitmap = NULL;
static LPBYTE buttonbitmap     = NULL;

static char selectedname[NAME_LEN];


//****************************************************************************
//****************************************************************************
static void SelSingDestroy(HWND window) {
	if (buttonbitmap) {
		FREE(buttonbitmap);
		buttonbitmap = NULL;
	}
	if (backgroundbitmap) {
		FREE(backgroundbitmap);
		backgroundbitmap = NULL;
	}
}


//****************************************************************************
//****************************************************************************
static BOOL SelSingInit(HWND window) {
	SIZE artsize;

	if (!LoadArtFile(window,NULL,TEXT(""),TEXT("ui_art\\samp_bkg.pcx"),&backgroundbitmap,&bgsize))
		return 0;
	if (!LoadArtFile(NULL,window,TEXT("Button"),TEXT("ui_art\\samp_btn.pcx"),&buttonbitmap,&artsize))
		return 0;

	return 1;
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK SelSingDialogProc (HWND   window,
										UINT   message,
										WPARAM wparam,
										LPARAM lparam) {
	switch (message) {

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDC_CREATECHAR_BTN:
					// bring up the create char popup
				break;
				case IDC_DELETECHAR_BTN:
					// bring up delete confirm dlg
				break;
				case IDC_NEWDUNGEON_BTN:
					// somehow indicate which character is selected to main application
					SDlgEndDialog(window,SELSING_NEW_DUNGEON);
				break;
				case IDC_CONTINUE_BTN:
					// somehow indicate which character is selected to main application
					SDlgEndDialog(window,SELSING_CONTINUE);
				break;
				case IDCANCEL:
					SDlgEndDialog(window,SELSING_PREVIOUS);
				break;
		}
		break;

		case WM_DESTROY:
			SelSingDestroy(window);
		break;

		case WM_INITDIALOG:
			SelSingInit(window);
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
BOOL APIENTRY UiSelSingDialog (
							   ENUMHEROS	enumfcn,
							   CREATEHERO	createfcn,
							   DELETEHERO	deletefcn,
							   DWORD		*selection,
							   char			*heroname) {
	DWORD result;

	// DISPLAY THE DIALOG BOX
	result = (DWORD)SDlgDialogBox(global_hinstance,
										TEXT("SELSINGLE_DIALOG"),
										SDrawGetFrameWindow(),
										SelSingDialogProc);

	if (selection)
		*selection = result;
	if (heroname)
		strcpy(heroname, selectedname);

	return 1;
}

/****************************************************************************
*
*  cancel.CPP
*  battle.net 
*
*  By Michael Morhaime
*
***/

#include "pch.h"



//****************************************************************************
//****************************************************************************

static LPBYTE sgBackgroundBmp	= NULL;
static LPBYTE sgButtonBmp		= NULL;


//===========================================================================
static void DestroyArtwork (HWND window) {
	if (sgBackgroundBmp) {
		FREE(sgBackgroundBmp);
		sgBackgroundBmp = NULL;
	}

	if (sgButtonBmp) {
		FREE(sgButtonBmp);
		sgButtonBmp = NULL;
	}
}

//===========================================================================
static BOOL LoadArtwork (HWND window, SNETGETARTPROC artcallback) {
	int btn_ids[] = {
		IDCANCEL,
		0
	};

	int btn_static[] = {
		IDC_TITLE,
		0
	};

	SIZE sizeBtns;
	SIZE bgSize;

	UiLoadArtwork(
			artcallback,
			window,
			NULL,
			SNET_ART_POPUPBACKGROUND_SML,
			TEXT(""),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,	
			FALSE,					// Palette is already loaded
			FALSE,					
			&sgBackgroundBmp,
			&bgSize);

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
			&sgButtonBmp,
			&sizeBtns);

	SDlgSetControlBitmaps (window, btn_ids, NULL, sgButtonBmp, &sizeBtns, SDLG_ADJUST_VERTICAL);
	SDlgSetControlBitmaps (window, btn_static, NULL, sgBackgroundBmp, &bgSize, SDLG_ADJUST_CONTROLPOS);


	return 1;
}




/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/



//===========================================================================
BOOL CALLBACK ConnectCancelDialogProc (HWND   window,
                                  UINT   message,
                                  WPARAM wparam,
                                  LPARAM lparam) {

	static SNETUIDATAPTR sInterfacedata;
	static HWND shWndCancelDlg;

	switch (message) {
		case WM_COMMAND: {
			switch (LOWORD(wparam)) {			
				case IDOK: 
				case IDCANCEL:
					SrvCancel();
					return 0;
			}
			break;
		}

		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			SendMessage(SDrawGetFrameWindow(), message, wparam, lparam);
			break;

		case WM_DESTROY:
		  	DestroyArtwork(window);
			break;
		
   		case WM_INITDIALOG: 
			// SAVE A POINTER TO THE USER INTERFACE PARAMETERS
			sInterfacedata = (SNETUIDATAPTR)lparam;
			UiLoadCursors(window, sInterfacedata);

			if (sInterfacedata)
				LoadArtwork(window, sInterfacedata->artcallback);
		  	return 1;
		
	}

	return SDlgDefDialogProc(window,message,wparam,lparam);
}



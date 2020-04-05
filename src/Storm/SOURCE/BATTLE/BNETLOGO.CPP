/***************************************************************************
*
*  bnetlogo.cpp
*  Battle.net parent dialog
*
*  This routine displays the Battle.net game logo and advertisement while
*  the user is logged into Battle.net.
*
*  By Michael Morhaime
*
***/

#include "pch.h"

extern HWND ghWndUiMainParent;		
extern HWND ghWndChat;

//***************************************************************************
extern BOOL CALLBACK ChatRoomDialogProc (HWND   window,
                                  UINT   message,
                                  WPARAM wparam,
                                  LPARAM lparam);

extern BOOL LogoInit(HWND child, SNETGETARTPROC artcallback);
extern void LogoAnimate(HWND hWndParent, HWND hWndLogo);
extern void LogoFramesDestroy(void);
extern BOOL LogoSetTimer(HWND window, int nTimer, SNETGETDATAPROC getdatacallback);

extern BOOL UiLoadCustomArt(
					SNETGETARTPROC  artcallback,
                    HWND            hWnd,
                    DWORD           artid,
					int				nFirstColor,
					int				nNumColorsUsed,
					BOOL			bSetPaletteNow,
                    LPBYTE         *data,
                    SIZE           *size);

extern BOOL CALLBACK AdDialogProc (HWND   window,
							  UINT   message,
							  WPARAM wparam,
							  LPARAM lparam);


//***************************************************************************

#define LOGO_TIMER_ID		1
#define SRVMAINTAIN_TIMER_ID	2
#define SRVMAINTAIN_TIMER_DELAY 5000	// every 5 seconds

static LPBYTE backgroundbitmap = NULL;

//===========================================================================
static void DestroyArtwork (HWND window) {
	TPBMP tpBmp = (TPBMP) GetWindowLong(GetDlgItem(window, IDC_LOGO_ANIMATE), GWL_USERDATA);

	// tpBmp->data points to a sgBackgroundBmp, which will be freed below
	if (tpBmp) 
		FREE(tpBmp);

	LogoFramesDestroy();


	if (backgroundbitmap) {
		FREE(backgroundbitmap);
		backgroundbitmap = NULL;
	}

	// Free artwork for scrollbar
	ScrollbarDestroyArtwork();
}

//===========================================================================
static BOOL LoadArtwork (HWND window, SNETGETARTPROC artcallback) {
	SIZE bgSize;


	UiLoadArtwork(
			artcallback,
			window,
			NULL,
			SNET_ART_BACKGROUND,
			TEXT(""),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,	
			TRUE,				// Get palette from this artwork
			TRUE,				// Prep palette for 'fade in'
			&backgroundbitmap, 
			&bgSize);



	// set up a TPBMP for the logo window, it will have information about the background bitmap
	HWND hWndLogo = GetDlgItem(window, IDC_LOGO_ANIMATE);

	if (hWndLogo && backgroundbitmap) {
		TPBMP tpBmp = (TPBMP) ALLOC(sizeof(TBMP));
		SetWindowLong(hWndLogo, GWL_USERDATA, (LONG) tpBmp);
		if (tpBmp) {
			tpBmp->data = backgroundbitmap;
			tpBmp->datasize = bgSize;
		}

		// set up the animating application logo
		LogoInit(hWndLogo,artcallback);
		LogoAnimate(window, hWndLogo);
	}

	// Load Scrollbar art (shared by many children of this dialog)
	ScrollbarLoadArtwork(artcallback);
	return 0;
}




//===========================================================================
static BOOL DoChatRoom (UIPARAMSPTR pUIparams, HWND hWndBattleNet) {
	UIPARAMS UIparams;
	SNETUIDATA interfacedata;

	// Create a copy of the UIparams and replace the parent window.
	UIparams = *pUIparams;
	interfacedata = *UIparams.interfacedata;
	interfacedata.parentwindow = hWndBattleNet;
	UIparams.interfacedata = &interfacedata;

    LPCTSTR pszTemplate = TEXT("DIALOG_CHATROOM");
    if (interfacedata.uiflags & SNET_UIFLAG_SUPPORTS_LADDER)
        pszTemplate = TEXT("DIALOG_LADDER_CHATROOM");
        
	return (1 == SDlgDialogBoxParam(
						global_hinstance,
						pszTemplate,
						hWndBattleNet, 
						ChatRoomDialogProc, 
						(LPARAM)&UIparams));
}

//===========================================================================
void KillChildWindows(HWND hWndParent, HWND hWndTop) {
	HWND hWnd;
	BOOL bKilledChat = FALSE;

	if (hWndTop != NULL) {
		hWnd = hWndTop;
		while (hWnd != hWndParent) {
			if (hWnd == ghWndChat)
				bKilledChat = TRUE;
			 
			SDlgEndDialog(hWnd, 0);
			hWnd = GetParent(hWnd);
		}
	}

	// Make sure chat room was killed (it might have hidden itself and started another child of the big window)
	if (!bKilledChat) 
		SDlgEndDialog(ghWndChat, 0);
}

//===========================================================================
BOOL CALLBACK BattleNetDialogProc (HWND   window,
                                  UINT   message,
                                  WPARAM wparam,
                                  LPARAM lparam) {

	static UIPARAMSPTR uiparams = NULL;

	switch (message) {
		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDOK: {
					HWND hWndAd = NULL;
					SNETUIDATA Interfacedata;

					// First make sure window has been drawn 
					UpdateWindow(window);

					// This is our signal to launch the ChatRoom and the advertisement window.
					if (uiparams) {
						// Advertisement window is a modeless dialog.
						Interfacedata = *(uiparams->interfacedata);
						Interfacedata.parentwindow = window;
  						hWndAd = SDlgCreateDialogParam(
											global_hinstance,
											TEXT("DIALOG_AD"),
											window,
   											AdDialogProc,
											(LPARAM)&Interfacedata);
					}



					// Once the chatroom returns, we are finished.
					// Note that the WM_TIMER will continue firing, so we can display
					// things like advertisements and continue animating the application logo.
					SDlgEndDialog(window, DoChatRoom(uiparams, window));

					if (hWndAd && IsWindow(hWndAd))
						DestroyWindow(hWndAd);
					return 1;
			   }
			}
			break;

		case WM_ERR_NOTRESPONDING: {
			char szText[256];
			char szTitle[32];

			LoadString(global_hinstance, IDS_BATTLENET, szTitle, sizeof(szTitle));
			LoadString(global_hinstance, IDS_ERR_NOTRESPONDING, szText, sizeof(szText));
			UiMessageBox(uiparams->interfacedata->messageboxcallback, window, szText, szTitle, MB_OK | MB_ICONERROR);
			return 1;
		}

		case WM_TIMER:
			if (wparam == LOGO_TIMER_ID) {
				LogoAnimate(window, GetDlgItem(window, IDC_LOGO_ANIMATE));

				// Make sure the connection is still active.
				if (gbConnectionLost) {
					char szText[256];
					char szTitle[32];

					// Save current top level window for application
					HWND hWnd = GetActiveWindow();

					gbConnectionLost = FALSE;	// Reset flag so we can continue animating logo while dialog is up.

					LoadString(global_hinstance, IDS_BATTLENET, szTitle, sizeof(szTitle));
					LoadString(global_hinstance, IDS_ERR_NOTRESPONDING, szText, sizeof(szText));
					UiMessageBox(uiparams->interfacedata->messageboxcallback, hWnd, szText, szTitle, MB_OK | MB_ICONERROR);

					// Kill off children dialogs
					KillChildWindows(window, hWnd);
				}
			}
			else if (wparam == SRVMAINTAIN_TIMER_ID) {
				// Update lag counters in chat room
				SrvMaintainLatencies();

				// Download new ads
				SrvMaintainAds();
			}

			return 0;

		case WM_NOTIFICATION_WAITING:
			UiNotification();
			return 1;

		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			SendMessage(SDrawGetFrameWindow(), message, wparam, lparam);
			break;


		case WM_QUERYNEWPALETTE: {
			// We are gaining focus, so set our own prefs
			UpdateWindow(window);		// draw window first.. syscolor stuff may take a while
			ColorPrefActivate(TRUE);
			break;
		}
		case WM_ACTIVATEAPP:
			// If we are losing focus, restore windows system prefs
			if (!wparam)
				ColorPrefActivate(FALSE);
			break;

		case WM_DESTROY:
			ghWndUiMainParent = NULL;

			UiVidFadeOut(DEFAULT_STEPS*2);

			SDlgKillTimer(window, LOGO_TIMER_ID);
			SDlgKillTimer(window, SRVMAINTAIN_TIMER_ID);

		  	DestroyArtwork(window);
			break;
		
   		case WM_INITDIALOG: 
			// SAVE A POINTER TO THE USER INTERFACE PARAMETERS
			uiparams = (UIPARAMSPTR)lparam;
			if (!uiparams)
				return 1;

			LoadArtwork(window, uiparams->interfacedata->artcallback);

			LogoSetTimer(window, LOGO_TIMER_ID, uiparams->interfacedata->getdatacallback);
			SDlgSetTimer(window, SRVMAINTAIN_TIMER_ID, SRVMAINTAIN_TIMER_DELAY, NULL);

			// Signal for us to launch the Chatroom. We launch it here, since the chatroom is modal.
			PostMessage(window, WM_COMMAND, IDOK, (LPARAM)NULL);

			// This window will receive wm_notification_waiting message
			ghWndUiMainParent = window;

			UiLoadCursors(window, uiparams->interfacedata);

			// Turn on colors
			UiVidFade(1,1);
		  	return 1;
	}

	return SDlgDefDialogProc(window,message,wparam,lparam);

}

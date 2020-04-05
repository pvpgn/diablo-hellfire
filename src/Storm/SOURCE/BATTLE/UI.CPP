/****************************************************************************
*
*  UI.CPP
*  battle.net user interface functions
*
*  By Michael Morhaime
*
***/

#include "pch.h"

//****************************************************************************
//****************************************************************************
#define MESS_WITH_PARENT 1		// 1 in final


//****************************************************************************
BOOL gbConnectionLost = FALSE;
BOOL gbConnectionSucks = FALSE;

// This is the window that is currently ready to 
// process the Battle.net nofication messages
HWND ghWndUiMainParent = (HWND) 0;		

static HWND sghDlgConnect = (HWND)0;  
static HWND sghDlgCancel = (HWND)0;
static HWND sghDlgProgress = (HWND)0;

static SNETUIDATA sgInterfacedata = { 0 };
static int sgnDownloadProgress = 0;

#define PALETTE_REGISTERS	256

static PALETTEENTRY sgPalette[PALETTE_REGISTERS];
static PALETTEENTRY sgFadePal[PALETTE_REGISTERS];



//****************************************************************************
static BOOL UiConnectMsg(SNETUIDATAPTR interfacedata, UINT nMessage, UINT flags);

BOOL CALLBACK ConnectDialogProc (HWND   window,
                                  UINT   message,
                                  WPARAM wparam,
                                  LPARAM lparam);
BOOL CALLBACK ConnectCancelDialogProc (HWND window,
								UINT message,
								WPARAM wparam,
								LPARAM lparam);
BOOL CALLBACK LogonDialogProc (HWND   window,
                               UINT   message,
                               WPARAM wparam,
                               LPARAM lparam);


void ProtectMinimize(BOOL bEnable);
//****************************************************************************
//****************************************************************************
static void UiClearPalette(PALETTEENTRY * pe) {
	for (int palreg = 0; palreg < PALETTE_REGISTERS; palreg++) {
 		pe[palreg].peRed   = 0;
 		pe[palreg].peGreen = 0;
 		pe[palreg].peBlue  = 0;
	}
}




/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/


//****************************************************************************
//****************************************************************************
BOOL UiInitialize(SNETUIDATAPTR interfacedata) {
    if (!interfacedata->artcallback) return FALSE;
    
	// Load scrollbar and combobox art (shared by many dialogs)
	ScrollbarLoadArtwork(interfacedata->artcallback);
    ComboboxLoadArtwork(interfacedata->artcallback);
    return TRUE;
}


//****************************************************************************
//****************************************************************************
void UiDestroy(void) {
    ScrollbarDestroyArtwork();
    ComboboxDestroyArtwork();
}


//****************************************************************************
//****************************************************************************
void RestoreWindowsColors(void) {
	HPALETTE hPal;

	// Get Windows Default colors from the system
	hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

	GetPaletteEntries(hPal, 0, 10, &sgFadePal[0]);
	GetPaletteEntries(hPal, 10, 10, &sgFadePal[PALETTE_REGISTERS-10]);

	SDrawUpdatePalette(0, PALETTE_REGISTERS, &sgFadePal[0], TRUE);
}

//****************************************************************************
//****************************************************************************
int gFadeStep;
//****************************************************************************
// an async fade function
//****************************************************************************
void UiVidFade(int max, int curr) {

	if (max == curr) {
		memcpy(sgFadePal, sgPalette, (PALETTE_REGISTERS * sizeof(PALETTEENTRY)));
		ShowCursor(TRUE);
	}
	else if (curr == 0) {
		memcpy(sgFadePal, sgPalette, (PALETTE_REGISTERS * sizeof(PALETTEENTRY)));
		UiClearPalette(sgFadePal);
	}
	else {
		for (int palreg = 0; palreg < PALETTE_REGISTERS; palreg++) {
			sgFadePal[palreg].peRed   = (sgPalette[palreg].peRed * curr) / max;
			sgFadePal[palreg].peGreen = (sgPalette[palreg].peGreen * curr) / max;
			sgFadePal[palreg].peBlue  = (sgPalette[palreg].peBlue * curr) / max;
		}
	}
	SDrawUpdatePalette(0, PALETTE_REGISTERS, sgFadePal, TRUE);
}


//****************************************************************************
//****************************************************************************
void UiVidFadeOut(int steps) {
	ShowCursor(FALSE);  

	memcpy(sgFadePal, sgPalette, (PALETTE_REGISTERS * sizeof(PALETTEENTRY)));

	for (int index = 0; index < steps; index++) {
		for (int palreg = 0; palreg < PALETTE_REGISTERS; palreg++) {
			sgFadePal[palreg].peRed   -= (sgPalette[palreg].peRed / steps);
			sgFadePal[palreg].peGreen -= (sgPalette[palreg].peGreen / steps);
			sgFadePal[palreg].peBlue  -= (sgPalette[palreg].peBlue / steps);
		}
		SDrawUpdatePalette(0, PALETTE_REGISTERS, sgFadePal, TRUE);
	}

	UiClearPalette(sgFadePal);
	SDrawUpdatePalette(0, PALETTE_REGISTERS, sgFadePal, TRUE);

	SDrawClearSurface(SDRAW_SURFACE_FRONT);
	RestoreWindowsColors();
}

//===========================================================================
// UiSetCursors()
//
// This routine must be called once the first time a dialog with a new class
// is displayed, so we can set the cursor for that particular class.
//===========================================================================
void UiLoadCursors(HWND hWnd, SNETUIDATAPTR interfacedata) {
	HCURSOR OldArrow = NULL;
	HCURSOR OldIBeam = NULL;

	if (!interfacedata || !interfacedata->getdatacallback)
		return;

#if 0
	HCURSOR hCursor;

	// Get arrow cursor
	if (interfacedata->getdatacallback(
							PROVIDERID,
							SNET_DATA_CURSORARROW,
							&hCursor,
							sizeof(hCursor),
							NULL))
		SDlgSetCursor(hWnd, hCursor, OCR_NORMAL);
	

	// Get ibeam cursor
	if (interfacedata->getdatacallback(
							PROVIDERID,
							SNET_DATA_CURSORIBEAM,
							&hCursor,
							sizeof(hCursor),
							NULL))
		SDlgSetCursor(hWnd, hCursor, OCR_IBEAM);
#else

	// Use System default cursors until we solve modal dialog issues.
	// (whenever the mouse is over a disabled window (like the parent of a modal dialog)
	// windows will display the default system cursor)).
	SDlgSetCursor(hWnd, LoadCursor(NULL, IDC_ARROW), OCR_NORMAL, &OldArrow);
	SDlgSetCursor(hWnd, LoadCursor(NULL, IDC_IBEAM), OCR_IBEAM, &OldIBeam);

#endif
}

//===========================================================================
BOOL UiGetData(SNETGETDATAPROC	getdatacallback, 
			   DWORD			dataid,
			   LPBYTE			*data,
			   DWORD			*datasize) {

	DWORD dwSize;

	*data = 0;

	if (!getdatacallback)
		return 0;


	// CALL THE GET DATA CALLBACK TO DETERMINE DATA SIZE
	if (!getdatacallback(PROVIDERID,
					 dataid,
					 NULL,
					 0,
					 &dwSize))
		return 0;


	// ALLOCATE MEMORY FOR THE IMAGE
	if (!(*data = (LPBYTE)ALLOC(dwSize)))
		return 0;

	// LOAD THE DATA BLOCK
	if (!getdatacallback(PROVIDERID,
					 dataid,
					 *data,
					 dwSize,
					 NULL)) {
		FREE(*data);
		*data = NULL;
		return 0;
	}

	*datasize = dwSize;
	return 1;
}

//===========================================================================
BOOL UiLoadArtwork (SNETGETARTPROC  artcallback,
                    HWND            hWnd,
                    HWND            hWndParent,
                    DWORD           artid,
                    LPCTSTR         controltype,
                    DWORD           controlstyle,
                    LONG            usageflags,
                    BOOL            loadpalette,
					BOOL			prepfadein,
                    LPBYTE         *data,
                    SIZE           *size) {
	PALETTEENTRY pe[256];

	*data    = 0;

	// VERIFY THAT THE APPLICATION HAS REGISTERED AN ARTWORK CALLBACK
	if (!artcallback)
		return 0;

	// CALL THE ARTWORK CALLBACK TO DETERMINE THE IMAGE DIMENSIONS
	int width = 0;
	int height = 0;
	int bitdepth;
	if (!artcallback(PROVIDERID,
					 artid,
					 NULL,
					 NULL,
					 0,
					 &width,
					 &height,
					 &bitdepth))
		return 0;
	if (size) {
		size->cx = width;
		size->cy = height;
	}


	// ALLOCATE MEMORY FOR THE IMAGE
	DWORD bytes = width*height*bitdepth/8;
	if (!(*data = (LPBYTE)ALLOC(bytes)))
		return 0;

	// LOAD THE IMAGE
	if (!artcallback(PROVIDERID,
					 artid,
					 &pe[0],
					 *data,
					 bytes,
					 &width,
					 &height,
					 &bitdepth)) {
		FREE(*data);
		*data = NULL;
		return 0;
	}


	// IF THIS IS A SCREEN BACKGROUND, USE ITS PALETTE
	if (loadpalette) {
		HPALETTE hPal;

		// Get Windows Default colors from the system
		hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);
		GetPaletteEntries(hPal, 0, 10, &pe[0]);
		GetPaletteEntries(hPal, 10, 10, &pe[PALETTE_REGISTERS-10]);
		memcpy(sgPalette, pe, (PALETTE_REGISTERS * sizeof(PALETTEENTRY)));

		if (!prepfadein) {
			SDrawUpdatePalette(0, PALETTE_REGISTERS, &sgPalette[0], TRUE);
 		}
		else {
			UiClearPalette(pe);
			SDrawUpdatePalette(0, PALETTE_REGISTERS, pe, TRUE);
			gFadeStep = 0;
		}
	}


	// If we have a window to associate it with, register the bitmap, 
	if (hWnd || hWndParent) {
		SDlgSetBitmap(
				hWnd,
				hWndParent,	
				controltype,
				controlstyle,
				usageflags,
				*data,
				NULL,
				width,
				height);
	}

  return 1;
}


//===========================================================================
BOOL UiSetCustomArt (HWND           hWnd,
					PALETTEENTRY   *pe,
					int				nFirstColor,
					int				nNumColorsUsed,
					BOOL			bSetPaletteNow,
					LPBYTE          data,
					int				nWidth,
					int				nHeight) {

	if (nNumColorsUsed)
		memcpy(&sgPalette[nFirstColor], &pe[nFirstColor], (nNumColorsUsed * sizeof(PALETTEENTRY)));

	if (bSetPaletteNow)
		SDrawUpdatePalette(nFirstColor, nNumColorsUsed, &sgPalette[nFirstColor]);

	// If we have a window to associate it with, register the bitmap, 
	if (hWnd) {
		SDlgSetBitmap(
				hWnd,
				NULL,	
				NULL,
				SDLG_STYLE_ANY,
				SDLG_USAGE_BACKGROUND,	
				data,
				NULL,
				nWidth,
				nHeight);

		InvalidateRect(hWnd, NULL, FALSE);
	}

	return 1;
}


//===========================================================================
BOOL UiLoadCustomArt (
					SNETGETARTPROC  artcallback,
                    HWND            hWnd,
                    DWORD           artid,
					int				nFirstColor,
					int				nNumColorsUsed,
					BOOL			bSetPaletteNow,
                    LPBYTE         *data,
                    SIZE           *size) {
	PALETTEENTRY pe[256];

	*data    = 0;

	// VERIFY THAT THE APPLICATION HAS REGISTERED AN ARTWORK CALLBACK
	if (!artcallback)
		return 0;

	// CALL THE ARTWORK CALLBACK TO DETERMINE THE IMAGE DIMENSIONS
	int width = 0;
	int height = 0;
	int bitdepth;
	if (!artcallback(PROVIDERID,
					 artid,
					 NULL,
					 NULL,
					 0,
					 &width,
					 &height,
					 &bitdepth))
		return 0;
	if (size) {
		size->cx = width;
		size->cy = height;
	}


	// ALLOCATE MEMORY FOR THE IMAGE
	DWORD bytes = width*height*bitdepth/8;
	if (!(*data = (LPBYTE)ALLOC(bytes)))
		return 0;

	// LOAD THE IMAGE
	if (!artcallback(PROVIDERID,
					 artid,
					 &pe[0],
					 *data,
					 bytes,
					 &width,
					 &height,
					 &bitdepth)) {
		FREE(*data);
		*data = NULL;
		return 0;
	}


	UiSetCustomArt(
				hWnd, 
				&pe[0],
				nFirstColor, 
				nNumColorsUsed, 
				bSetPaletteNow, 
				*data, 
				width, 
				height);

  return 1;
}

//===========================================================================
BOOL UiLogon (SNETPROGRAMDATAPTR  programdata,
              SNETPLAYERDATAPTR   playerdata,
              SNETUIDATAPTR       interfacedata,
              SNETVERSIONDATAPTR  versiondata)
{
    return (1 == SDlgDialogBoxParam(
					global_hinstance,
					TEXT("DIALOG_LOGON"),
					(interfacedata) ? interfacedata->parentwindow : SDrawGetFrameWindow(),
					LogonDialogProc,
					(LPARAM) interfacedata));
}


//===========================================================================
BOOL UiBeginConnect (SNETPROGRAMDATAPTR programdata,
                     SNETUIDATAPTR      interfacedata) {

	static UIPARAMS suiparams;
	HWND hWndParent;


  	ZeroMemory(&suiparams,sizeof(UIPARAMS));
  	suiparams.flags         = 0;
  	suiparams.programdata   = programdata;
  	suiparams.playerdata    = NULL;
  	suiparams.interfacedata = interfacedata;
  	suiparams.versiondata   = 0;
  	suiparams.playeridptr   = 0;

	gbConnectionLost = FALSE;		// Make sure this is reset.
	gbConnectionSucks = FALSE;		// If it sucks, Storm will tell us.


	// Display a popup of some sort that has some sort of 
	// connecting animation.  

	if (!interfacedata)
		return 0;

	hWndParent = interfacedata->parentwindow;

#if MESS_WITH_PARENT
	// Hide parent window before dialog can set palette (if parent is not the frame window)
	if (SDrawGetFrameWindow() != hWndParent) {
		SetActiveWindow(GetParent(hWndParent));
		ShowWindow(hWndParent, SW_HIDE);
	}
#endif

	// Erase screen
	SDrawClearSurface(SDRAW_SURFACE_FRONT);

  	// DISPLAY THE DIALOG BOX
  	sghDlgConnect = SDlgCreateDialogParam(
							global_hinstance,
                            TEXT("DIALOG_CONNECT_BG"),
							hWndParent,
   				            ConnectDialogProc,
                            (LPARAM)&suiparams);

	// Copy interfacedata for use later by children of connect dlg
	sgInterfacedata = *interfacedata;
	sgInterfacedata.parentwindow = sghDlgConnect;

	if (sghDlgConnect) {
		// Display popup to allow user to cancel
		sghDlgCancel = SDlgCreateDialogParam(
							global_hinstance,
							TEXT("DIALOG_CONNECT_CANCEL"),
							sghDlgConnect,
							ConnectCancelDialogProc,
							(LPARAM)&sgInterfacedata);

		ProtectMinimize(TRUE);
	}

	return (sghDlgConnect != (HWND)0);
}

//===========================================================================
void UiEndConnect (BOOL connected) {
	if (sghDlgConnect) {
		ProtectMinimize(FALSE);

#if MESS_WITH_PARENT 
		if (SDrawGetFrameWindow() != GetParent(sghDlgConnect)) {
			if (connected)	{	
				// Parent is hidden right now. Make the frame window active so that
				// we don't lose the focus completely when sghDlgConnect is destroyed.
				SetActiveWindow(GetParent(GetParent(sghDlgConnect)));
			}
			else 
				ShowWindow(GetParent(sghDlgConnect), SW_SHOW);
		}
#endif

		DestroyWindow(sghDlgConnect);
		sghDlgConnect = (HWND) 0;
		sghDlgCancel = (HWND) 0;		// Cancel will be destroyed automatically

	}

	sgInterfacedata.parentwindow = NULL;
}


//****************************************************************************
//****************************************************************************
void UiRestoreApp(void) {
	// Switch back to our app and do a restore
	SetActiveWindow(SDrawGetFrameWindow());
	ShowWindow(SDrawGetFrameWindow(), SW_RESTORE);

	// Flush out messages before continuing to allow all our windows to 
	// know that we are back.
	UiProcessWindowMessages();
}


//********************************************
//********************************************
extern BOOL CALLBACK RasEnumCallback(LPCTSTR szEntryName, LPVOID rashandle, LPVOID lpContext);

BOOL UiDisconnect(void) {
	EnumNewRASConnections(RasEnumCallback, (LPVOID) &sgInterfacedata);
	return 1;
}

//===========================================================================
static BOOL UiConnectMsg(SNETUIDATAPTR interfacedata, UINT nMessage, UINT flags) {
	char szText[512];
	char szTitle[32];
	DWORD dwReturn;

	if (!interfacedata)
		return 0;

	if (!interfacedata->messageboxcallback)
		return 0;

	if (!interfacedata->parentwindow)
		return 0;
				    

	LoadString(global_hinstance, IDS_BATTLENET, szTitle, sizeof(szTitle));
	LoadString(global_hinstance, nMessage, szText, sizeof(szText));

	dwReturn = UiMessageBox(
					interfacedata->messageboxcallback,
					interfacedata->parentwindow,
					szText, 
					szTitle, 
					flags);	

	return (dwReturn == IDOK);
}


//===========================================================================
static int CALLBACK GetDownloadProgress(void) {
	return sgnDownloadProgress;
}

//===========================================================================
BOOL UiProcessWindowMessages (void) {
  MSG message;
  while (PeekMessage(&message,(HWND)0,0,0,PM_REMOVE)) {
    if (message.message == WM_QUIT) {
      PostMessage(message.hwnd,message.message,message.wParam,message.lParam);
      return 0;						 
    }
    else if (((!sghDlgConnect)  || (!IsDialogMessage(sghDlgConnect ,&message))) &&
             ((!sghDlgProgress) || (!IsDialogMessage(sghDlgProgress,&message))) && 
			 ((!sghDlgCancel)   || (!IsDialogMessage(sghDlgCancel  ,&message)))) {
      TranslateMessage(&message);
      DispatchMessage(&message);
    }
  }

  SDlgCheckTimers();
  return 1;
}

//===========================================================================
void UiHideConnectCancel(void) {
	// Hide cancel button when any popups are displayed
	if (!sghDlgCancel)
		return;

	// Get rid of Cancel Dlg
	DestroyWindow(sghDlgCancel);
	sghDlgCancel = NULL;
}

//===========================================================================
void UiWSockErrMessage(void) {
	UiHideConnectCancel();
	UiConnectMsg(&sgInterfacedata, IDS_ERR_NOWSOCK32, MB_OK | MB_ICONWARNING);
	return;
}

//===========================================================================
BOOL UiUpgradeMessage(void) {
	UiHideConnectCancel();

	// Okay minimization fears are over also
	ProtectMinimize(FALSE);
	return UiConnectMsg(&sgInterfacedata, IDS_ASK_UPDATE, MB_OKCANCEL);
}


//===========================================================================
void UiNotificationWaiting(void) {
	if (!ghWndUiMainParent) 
		return;

	PostMessage(ghWndUiMainParent, WM_NOTIFICATION_WAITING, 0, 0);
}

//===========================================================================
//===========================================================================
void UiNotification (void) {
	DWORD notifycode;
	LPVOID paramdata;
	DWORD parambytes;
	
	// NOTE:  paramdata will no longer be valid after hitting a PeekMessage loop, 
	// All routines called from here must make sure that data from paramdata is
	// copied before sent to a message box or to any routines that might yield control!!!
	while (SrvGetUiNotification (&notifycode, &paramdata, &parambytes)) {
		switch (notifycode) {
			case SN_ADDCHANNEL: 
				ChatAddChannel((LPCSTR) paramdata);
				break;
			case SN_DELETECHANNEL:
				ChatDeleteChannel((LPCSTR) paramdata);
				break;
			case SN_JOINCHANNEL:
				ChatJoinChannel((SNJOINCHANNELPTR) paramdata);
				break;
			case SN_CHANNELISFULL: 
				ChatChannelFull((LPCSTR) paramdata);
				break;
			case SN_CHANNELDOESNOTEXIST:
				ChatChannelDoesNotExist((LPCSTR) paramdata);
				break;
			case SN_CHANNELISRESTRICTED:
				ChatChannelRestricted((LPCSTR) paramdata);
				break;

			case SN_ADDUSER:
				ChatAddUser((SNADDUSERPTR) paramdata);
				break;
			case SN_DELETEUSER:
				ChatDeleteUser((SNDELETEUSERPTR) paramdata);
				break;
			case SN_USERNAME:
				ChatSetUserName((LPCSTR) paramdata);
				break;

			case SN_CHANGEUSERFLAGS:
				ChatChangeUserFlags((SNCHANGEUSERFLAGSPTR) paramdata);
				break;

			case SN_UPDATEPINGTIME:
				ChatUpdatePingTime((SNUPDATEPINGTIMEPTR) paramdata);
				break;

			case SN_DISPLAYSTRING:
				// Got a message from someone
				ChatReceiveMsg((SNDISPLAYSTRINGPTR) paramdata);
				break;

			case SN_SQUELCHUSER:
				ChatSquelchUser((SNSQUELCHUSERPTR) paramdata);
				break;

			case SN_UNSQUELCHUSER:
				ChatUnsquelchUser((SNSQUELCHUSERPTR) paramdata);
				break;


			case SN_BADCONNECTION:
				UiConnectMsg(&sgInterfacedata, IDS_ERR_BADSERVICEPROVIDER, MB_OK | MB_ICONERROR);
				gbConnectionSucks = TRUE;
				break;

			case SN_SETADINFO:		
				AdSetInfo((SNADINFOPTR) paramdata);
				break;

			case SN_DISPLAYAD:
				AdDisplay((LPVOID) paramdata, parambytes);
				break;

			case SN_DOWNLOADINGUPGRADE: {
				DWORD *pDword = (DWORD *)paramdata;
				sgnDownloadProgress = *pDword;

				if (!sghDlgProgress) {
					char szText[128]; 

					LoadString(global_hinstance, IDS_DOWNLOADPROGRESS, szText, sizeof(szText));
					sghDlgProgress = UiModelessProgressDialog(
													&sgInterfacedata,
													szText,
													TRUE,		// Allow user to abort
													GetDownloadProgress,
													20);		// Frequency of progress update (FPS)
				}

				break;
			}

			case SN_DOWNLOADFAILED: 
				if (sghDlgProgress) {
					DestroyWindow(sghDlgProgress);
					sghDlgProgress = NULL;
				}
				UiConnectMsg(&sgInterfacedata, IDS_ERR_DOWNLOADFAILED, MB_OK | MB_ICONERROR);
				break;

			case SN_DOWNLOADSUCCEEDED: 
				if (sghDlgProgress) {
					DestroyWindow(sghDlgProgress);
					sghDlgProgress = NULL;
				}

				UiConnectMsg(&sgInterfacedata, IDS_PREP_RESTART, MB_OK);
				break;


			case SN_FAILEDTOCONNECT: {
				DWORD *pdwError = (DWORD *)paramdata;
				int nMsgNo;

				switch (*pdwError) {
					case SN_ERROR_NOTRESPONDING:
						nMsgNo = IDS_ERR_NOTRESPONDING;
						break;

					case SN_ERROR_BADCONNECTION:
						nMsgNo = IDS_ERR_BADCONNECTION;
						break;

					case SN_ERROR_UNABLETOUPGRADE:
						nMsgNo = IDS_ERR_UNABLETOUPGRADE;
						break;

					case SN_ERROR_UNREACHABLE:
						nMsgNo = IDS_ERR_UNREACHABLE;
						break;

					default:
						nMsgNo = IDS_ERR_UNREACHABLE;
				}

				UiHideConnectCancel();

				// Okay minimization fears are over also
				ProtectMinimize(FALSE);
				UiConnectMsg(&sgInterfacedata, nMsgNo, MB_OK | MB_ICONERROR);
				break;
			}
			case SN_LOSTCONNECTION: 
				// We can't display a message here, so set a flag to display a message.
				// This will cause the chatroom to abort. 
				gbConnectionLost = TRUE;
				break;

			case SN_MESSAGEBOX: {
				SNMESSAGEBOXPTR ptr;

				ptr = (SNMESSAGEBOXPTR)paramdata;
				if (!sgInterfacedata.messageboxcallback)
					MessageBox(ghWndUiMainParent, ptr->text, ptr->caption, ptr->type);
				else
					sgInterfacedata.messageboxcallback(ghWndUiMainParent, ptr->text, ptr->caption, ptr->type);

				break;
			}

		}			// end switch

	}				// end while

}



//===========================================================================
BOOL UiSelectGame (DWORD               flags,
                   SNETPROGRAMDATAPTR  programdata,
                   SNETPLAYERDATAPTR   playerdata,
                   SNETUIDATAPTR       interfacedata,
                   SNETVERSIONDATAPTR  versiondata,
                   DWORD              *playerid) {

	// BUILD A USER INTERFACE DATA BLOCK
	UIPARAMS uiparams;
	ZeroMemory(&uiparams,sizeof(UIPARAMS));
	uiparams.flags         = flags;
	uiparams.programdata   = programdata;
	uiparams.playerdata    = playerdata;
	uiparams.interfacedata = interfacedata;
	uiparams.versiondata   = versiondata;
	uiparams.playeridptr   = playerid;


	if (!interfacedata)
		return 0;


	// Take control of system colors using app preferences
	ColorPrefInit(interfacedata->getdatacallback);


	// DISPLAY THE DIALOG BOX
	DWORD result = (DWORD)SDlgDialogBoxParam(global_hinstance,
										   TEXT("DIALOG_BATTLENET"),
										   interfacedata->parentwindow,
										   BattleNetDialogProc,
										   (LPARAM)&uiparams);

	// Restore Windows System Colors
	ColorPrefDestroy();

	return (result == 1);
}

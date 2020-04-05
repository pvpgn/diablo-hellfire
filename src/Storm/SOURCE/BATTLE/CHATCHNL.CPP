/****************************************************************************
*
*  chatchnl.cpp
*  battle.net user interface functions
*
*  By Michael Morhaime
*
***/

#include "pch.h"

#define MAX_STRING_LEN  256

static HWND sghWndChannel = NULL;
static char sgszChannel[MAX_CHANNEL_LEN];
static char *sgpszLastChannel;
static PTCHANNEL_LIST sgpChannelListHead = NULL;

static LPBYTE sgBackgroundBmp = NULL;
static LPBYTE sgButtonBmp = NULL;
static SNETUIDATAPTR sgInterfacedata;




//===========================================================================
// External Functions
//===========================================================================
UINT ChatGetUserFlags(void);

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
		IDOK,
		IDCANCEL,
		0
	};

	int btn_desc[] = {
		IDC_GAMEDESCRIPTION,
		0,
	};

	SIZE sizeBtns;
	SIZE bgSize;

	UiLoadArtwork(
			artcallback,
			window,
			NULL,
			SNET_ART_BATTLE_SELECT_CHNL_BKG,
			TEXT(""),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,	
			FALSE,				
			FALSE,
			&sgBackgroundBmp,
			&bgSize);

	UiLoadArtwork(
			artcallback,
			NULL,
			NULL,
			SNET_ART_BUTTON_XSML,
			TEXT("Button"),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,	
			FALSE,
			FALSE,
			&sgButtonBmp,
			&sizeBtns);

	SDlgSetControlBitmaps (window, btn_ids, NULL, sgButtonBmp, &sizeBtns, SDLG_ADJUST_VERTICAL);
	SDlgSetControlBitmaps (window, btn_desc, NULL, sgBackgroundBmp, &bgSize, SDLG_ADJUST_CONTROLPOS);
	return 1;
}


//===========================================================================
static int ProcessName(LPSTR szName) {
	char szTemp[MAX_CHANNEL_LEN];
	int nStart, nEnd;

	// Take the name and remove leading and trailing spaces
	strcpy(szTemp, szName);

	
	// Proceed until we find a non-space or null termination
	for (nStart=0; ;nStart++) {
		if (szTemp[nStart] != ' ')
			break;
		if (szTemp[nStart] == 0)			
			return 0;
	}


	// Now start at the end and look for a non-space character. Proceed until
	// we reach the beginning
	for (nEnd=strlen(szTemp)-1; ;nEnd--) {
		
		if (szTemp[nEnd] != ' ')
			break;

		if (nEnd <= nStart)
			return 0;
	}

	szTemp[++nEnd] = 0;		// NULL terminate string before trailing spaces

	strcpy(szName, &szTemp[nStart]);
	return(nEnd - nStart);	// Length of string not including NULL termination
}



//****************************************************************************
//****************************************************************************
static void convert_nasty(LPSTR nasty) {
	while (*nasty != 0) {
		(*nasty)--;
		nasty++;
	}
}

//****************************************************************************
//****************************************************************************
BOOL IsNastyName(LPCTSTR name) {
	TCHAR tempname[MAX_CHANNEL_LEN];
	TCHAR nasty[MAX_CHANNEL_LEN];

	strcpy(tempname, name);
	_strlwr(tempname);

	for (int index = IDS_FIRST_NASTY; index <= IDS_LAST_NASTY; index++) {
		LoadString(global_hinstance, index, nasty, MAX_CHANNEL_LEN);
		convert_nasty(nasty);
		_strlwr(nasty);
		if (strstr(tempname, nasty)) {
			return TRUE;
		}
	}
	return FALSE;
}


//****************************************************************************
// InvalidChars()
//
// NOTE: SPACE is not included as a bad char. SPACE and any other additional bad 
// characters should be placed into the additional string. Things like GAME NAME
// may include spaces.
//****************************************************************************
BOOL InvalidChars(LPCTSTR name, LPCTSTR additionalbad) {
	LPTSTR basicbad = ",<>%&\\\"?*#/";		
	unsigned char c;

	if (strpbrk(name, basicbad))
		return TRUE;

	if (strpbrk(name, additionalbad))
		return TRUE;

	while (0 != (c = *name)) {
		if (c < ' ') return TRUE;
		if ((c > '~') && (c < 0xc0)) return TRUE;
		name++;
	}
	return FALSE;
}



//===========================================================================
static void AddChannels(HWND window) {
	PTCHANNEL_LIST pCurr;
	HWND hWndList;

	hWndList = GetDlgItem(window, IDC_CHANNELLIST);
	if (!hWndList)
		return;


	// Add all the channels we know about to the linked list
	pCurr = sgpChannelListHead;
	while (pCurr) {
		SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)pCurr->szChannel);
		pCurr = pCurr->next;
	}

	// update our scroll bar
	ListUpdateScrollbar(hWndList);
}


//===========================================================================
// NOTE: szChannel will no longer be valid after hitting a PeekMessage loop
//===========================================================================
void ChatChannelFull(LPCSTR szChannel) {
	char szText[256];
	char szFmt[256];

	// Protect this routine
	if (!sghWndChannel || !sgInterfacedata)
		return;

	LoadString(global_hinstance,IDS_CHANNEL_FULL,szFmt, sizeof(szFmt));
	sprintf(szText, szFmt, szChannel);
	UiMessageBox(sgInterfacedata->messageboxcallback, sghWndChannel, szText, NULL, MB_OK);

	// Allow user to enter another channel name
	EnableWindow(GetDlgItem(sghWndChannel, IDOK), TRUE);
}


//===========================================================================
// NOTE: szChannel will no longer be valid after hitting a PeekMessage loop
//===========================================================================
void ChatChannelDoesNotExist(LPCSTR szChannel) {
	char szText[256];

	// Make sure window is still around
	if (!sghWndChannel)
		return;

	// Make copy before sending this to a messagebox.
	strcpy(szText, szChannel);
	SendMessage(sghWndChannel, WM_CHANNEL_DOESNOTEXIST, 0, (LPARAM)szText);
}

//===========================================================================
// NOTE: szChannel will no longer be valid after hitting a PeekMessage loop
//===========================================================================
void ChatChannelRestricted(LPCSTR szChannel) {
	TCHAR szUserName[MAXSTRINGLENGTH];
	TCHAR szUserDesc[MAXSTRINGLENGTH];
	UINT nFlags;
	char szError[256] = ""; 

	if (!sghWndChannel)
		return;

	if (sgInterfacedata->authcallback) {
		SrvGetLocalPlayerName (szUserName,sizeof(szUserName));
		SrvGetLocalPlayerDesc (szUserDesc,sizeof(szUserDesc));
		nFlags = ChatGetUserFlags();


		if (!sgInterfacedata->authcallback(SNET_AUTHTYPE_CHANNEL, szUserName, szUserDesc, nFlags, szChannel, szError, sizeof(szError))) {
			SendMessage(sghWndChannel, WM_CHANNEL_RESTRICTED, 0, (LPARAM)szError);
			return;
		}

		// Ok to join channel
		SrvJoinChannel(szChannel, TRUE);
	}

}

//===========================================================================
void ChatChannelJoined(LPCSTR szChannel) {
	if (!sghWndChannel)
		return;

	SendMessage(sghWndChannel, WM_CHANNEL_JOINED, 0, 0);
}

//===========================================================================
BOOL CALLBACK ChannelDialogProc (HWND   window,
                                  UINT   message,
                                  WPARAM wparam,
                                  LPARAM lparam) {

	switch (message) {
		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDOK: {
					char szText[MAX_STRING_LEN];

					if (sgInterfacedata->soundcallback)
						sgInterfacedata->soundcallback(PROVIDERID, SNET_SND_SELECTITEM, 0);

					SendDlgItemMessage(window, IDC_EDIT_NAME, WM_GETTEXT, MAX_CHANNEL_LEN, (LPARAM)(LPCSTR)szText);

					// Don't validate channel name if we selected a public channel from the list
					if (LB_ERR == SendDlgItemMessage(window, IDC_CHANNELLIST, LB_GETCURSEL, 0, 0)) {
						if (InvalidChars(szText, "") || !ProcessName(szText)) {
							LoadString(global_hinstance,IDS_CHANNEL_NAME_INVALID,szText,MAX_STRING_LEN);
							UiMessageBox(sgInterfacedata->messageboxcallback, window, szText, NULL, MB_OK | MB_ICONERROR);
							return 1;
						}
					}
					
					// If user has requested the current channel, just exist the dialog
					if (!strcmp(szText,sgpszLastChannel)) {
						SDlgEndDialog(window, 0);
						return 1;
					}

					// Send request to join channel
					SrvJoinChannel(szText, FALSE);

					// Name was okay, so save in the global channel string and return TRUE
					strcpy(sgszChannel, szText);

					// Disable Window, until we know if we succeeded
					EnableWindow((HWND)lparam, FALSE);	
					return 1;
				}

				case IDCANCEL:
					if (sgInterfacedata->soundcallback)
						sgInterfacedata->soundcallback(PROVIDERID, SNET_SND_SELECTITEM, 0);

					sgszChannel[0] = 0;
					SDlgEndDialog(window, 0);
					return 1;

				case IDC_EDIT_NAME:
					if (HIWORD(wparam) == EN_CHANGE) {
						// If user types in name edit control, remove selection from list box.
						if (GetFocus() == (HWND)lparam)
							SendDlgItemMessage(window,IDC_CHANNELLIST,LB_SETCURSEL,(WPARAM)-1,0);
					}								  
					break;

				case IDC_CHANNELLIST:
					if (HIWORD(wparam) == LBN_DBLCLK) {
						HWND hWndChannelList = (HWND)lparam;
						int nIndex;

						nIndex = SendMessage(hWndChannelList, LB_GETCURSEL, 0, 0);
						if (nIndex != LB_ERR) {
							// Make sure we update the edit control first
			        		SendMessage(window,WM_COMMAND,MAKELONG(IDC_CHANNELLIST,LBN_SELCHANGE),(LPARAM)hWndChannelList);

							// Behave like OK was just pressed
							SendMessage(window,WM_COMMAND,MAKELONG(IDOK,BN_CLICKED),(LPARAM)GetDlgItem(window, IDOK));
						}
					}
					else if (HIWORD(wparam) == LBN_SELCHANGE) {
						HWND hWndChannelList = (HWND)lparam;
						int nIndex = SendMessage(hWndChannelList, LB_GETCURSEL, 0, 0);
						char szText[MAX_CHANNEL_LEN];

						// Change text in Edit Control
						if (nIndex != LB_ERR) {
							SendMessage(hWndChannelList,LB_GETTEXT, nIndex, (LPARAM)(LPCSTR)szText);
							SendDlgItemMessage(window, IDC_EDIT_NAME, WM_SETTEXT, 0, (LPARAM)(LPCSTR)szText);
						}

						// update our scroll bar
						ListUpdateScrollbar(hWndChannelList);
					}
					break;
   	   		}
   	   		break;

		case WM_CHANNEL_DOESNOTEXIST: {
			char szText[256];
			char szFmt[256];
			int nResult;

			LoadString(global_hinstance,IDS_PROMPT_CREATECHANNEL,szFmt,sizeof(szFmt));
			sprintf(szText, szFmt, (LPCSTR)lparam);
			nResult = (IDOK == UiMessageBox(sgInterfacedata->messageboxcallback, window, szText, NULL, MB_OKCANCEL));

			if (!nResult) {
				// If user said no, reenable OK button.
				EnableWindow(GetDlgItem(window, IDOK), !nResult);
			}
			else {
				// Send request to join channel, this time setting 'joinalways' flag
				SrvJoinChannel((LPCSTR)lparam, TRUE);
			}

			return 1;
		}

		case WM_CHANNEL_RESTRICTED: {
			UiMessageBox(sgInterfacedata->messageboxcallback, window, (LPCSTR)lparam, NULL, MB_OK | MB_ICONWARNING);

			// Allow user to select another channel
			EnableWindow(GetDlgItem(window, IDOK), TRUE);
			return 1;
		}

		case WM_CHANNEL_JOINED: {
			// This means that we have joined a channel
			SDlgEndDialog(window, 1);
			return 1;
		}

		case WM_CTLCOLORSTATIC:
			if (GetWindowLong((HWND)lparam, GWL_ID) == IDC_TITLE) {
				SetTextColor((HDC) wparam, RGB(0xff, 0xff, 0x00));
				return (BOOL) GetStockObject(NULL_BRUSH);
			}
			break;

		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			SendMessage(SDrawGetFrameWindow(), message, wparam, lparam);
			break;


		case WM_DESTROY:
			sghWndChannel = NULL;
		  	DestroyArtwork(window);
			break;
		

   		case WM_INITDIALOG: 
			sgInterfacedata = (SNETUIDATAPTR) lparam;
			sghWndChannel = window;

			LoadArtwork(window, sgInterfacedata->artcallback);

			ScrollbarLink(GetDlgItem(window, IDC_CHANNELLIST), GetDlgItem(window, IDC_SCROLLBAR));

			SetWindowText(GetDlgItem(window, IDC_EDIT_NAME), sgpszLastChannel);
			SendDlgItemMessage(window, IDC_EDIT_NAME, EM_LIMITTEXT, MAX_CHANNEL_LEN-1, 0);

			// Set height of channel list items (some systems get strange heights)
			SendDlgItemMessage(window, IDC_CHANNELLIST, LB_SETITEMHEIGHT, 0, 19);

			AddChannels(window);
		  	return 1;
		
	}

	return SDlgDefDialogProc(window,message,wparam,lparam);

}


//===========================================================================
// Exported functions
//===========================================================================


//===========================================================================
BOOL ChatSelectChannel(SNETUIDATAPTR interfacedata, char *szChannel, PTCHANNEL_LIST pChannelListHead) {
	int nResult;

	// Clear channel string
	sgszChannel[0] = 0;
	sgpszLastChannel = szChannel;			// Save a pointer to the old channel
	sgpChannelListHead = pChannelListHead;
                                   
	nResult = SDlgDialogBoxParam(
		global_hinstance,
		TEXT("DIALOG_CHAT_CHANNEL"),
		(interfacedata) ? interfacedata->parentwindow : SDrawGetFrameWindow(),
		ChannelDialogProc,
		(LPARAM)interfacedata);

	return (nResult == 1);
}


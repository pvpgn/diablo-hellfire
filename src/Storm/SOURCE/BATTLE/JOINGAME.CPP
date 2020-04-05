/****************************************************************************
*
*  JOINGAME.CPP
*  battle.net chat room
*
*  By Michael Morhaime
*
***/

//****************************************************************************
// Modification Log:
// 
// Diablo Patch #1
// 2/3/97   MM	-	Instead of calling UpdateGameList every 10 seconds,
//					we now only call it once. Then, after waiting 3 seconds,
//					we go through the list of games calling SpiGetGameInfo()
//					for each game.
// 
// 2/5/97   MM	-	Added better error messages for not being able to join a game.
//
// 2/17/97  MM	-	Added call to SrvNotifyJoin() when successfully joining a game.
//
// Diablo Patch #2
// 3/21/97	MM	-	Added support for requesting a list of games by category.
//****************************************************************************

#include "pch.h"		   

#define MAX_STRING_LEN 128

typedef struct _CATEGORYDATA {
    DWORD categorybits;
    DWORD categorymask;
} CATEGORYDATA, *CATEGORYDATAPTR;

static LPBYTE sgBackgroundBmp = NULL;
static LPBYTE sgButtonBmp     = NULL;
static LPBYTE sgListboxBmp	  = NULL;

static char sgszGameName[MAX_STRING_LEN] = "";
static char sgszGamePassword[MAX_STRING_LEN] = "";

//### MM Diablo Patch #2  3/21/97   
static DWORD sgdwCategoryBits;
static DWORD sgdwCategoryMask;

static HWND     sghFilterCombo;

extern int gnUserFlags;
//===========================================================================
// External routines
//===========================================================================
int NormalizeNetLag(DWORD dwMilliSec);
void DrawNetLag(int nNetLag, LPDRAWITEMSTRUCT lpdis);
int ListFindName(HWND hWndList, LPCSTR szName);

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


	if (sgListboxBmp) {
		FREE(sgListboxBmp);
		sgListboxBmp = NULL;
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
			SNET_ART_JOINBACKGROUND,
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
//===========================================================================
static HFONT InitCustomFonts(HWND hWnd) {
#if 0
	HFONT hFont = NULL;
	LOGFONT lFont;

	if ((hFont = (HFONT) SendMessage(hWnd, WM_GETFONT, 0, 0L))) {
		// Start with dialog font and modify face name
		if (GetObject(hFont, sizeof(LOGFONT), (LPSTR) &lFont)) {
			//lFont.lfHeight = -MulDiv(8, 96, 72);		// height of 8 pixels
			//lFont.lfWidth = 0;						// let Windows autosize width
			//lFont.lfWeight = FW_NORMAL;				// change to non Bold
			strcpy(lFont.lfFaceName, "Arial");


			if (hFont = CreateFontIndirect((LPLOGFONT) &lFont)) {
				SendDlgItemMessage(hWnd, IDC_GAMEDESCRIPTION, WM_SETFONT, (WPARAM)hFont, 0);
			}
		}
	}
	return hFont;
#else 
	return 0;
#endif

}

//===========================================================================
static void UpdateGameList (HWND dialog, HWND listbox, BOOL bSelGame) {

	SNETSPI_GAMELISTPTR curr, gamelist;
	int nIndex;


	// Get current linked list of games
	if (!SpiLockGameList(sgdwCategoryBits, sgdwCategoryMask, &gamelist))	//### MM Diablo Patch #2  3/21/97   
		return;

	curr = gamelist;


	// MAKE SURE ALL GAMES IN THE LINKED LIST ARE REPRESENTED IN THE LIST BOX
	SendMessage(listbox, WM_SETREDRAW, FALSE, 0);
	while (curr) {
		// Ignore private games
		if (curr->gamemode & SNET_GM_PRIVATE) {
			curr = curr->next;
			continue;
		}

		nIndex = ListFindName(listbox, curr->gamename);

	   	char szString[2*SNETSPI_MAXSTRINGLENGTH];	 

		//### PATCH 2/3/97 MM Added gameid to string.
//    	sprintf(szString,"%s\t%x %x\t%s",curr->gamename, NormalizeNetLag(curr->ownerlatency), curr->creationtime, curr->gamedescription);
    	sprintf(szString,"%s\t%x %x %x\t%s",curr->gamename, curr->gameid, NormalizeNetLag(curr->ownerlatency), curr->creationtime, curr->gamedescription);

		// Is this game in the list box at all?
		if (nIndex == LB_ERR) {
   			SendMessage(listbox,LB_ADDSTRING,0,(LPARAM)szString);

			// If there is no current list box selection, and there is no text in the
			// NAME control, select this item as the default selection.
    		if ( bSelGame && 
				 SendMessage(listbox,LB_GETCURSEL,0,0) == LB_ERR  && 
				 SendDlgItemMessage(dialog,IDC_EDIT_NAME,WM_GETTEXTLENGTH,0,0) == 0) {
        		SendMessage(listbox,LB_SETCURSEL,0,0);
        		SendMessage(dialog,WM_COMMAND,MAKELONG(IDC_GAMELIST,LBN_SELCHANGE),(LPARAM)listbox);
    		}
		}
		else {
			char szOldString[2*SNETSPI_MAXSTRINGLENGTH];

			// Game is in the list box, but is the information still correct?
			SendMessage(listbox,LB_GETTEXT,nIndex,(LPARAM)szOldString);
			if (strcmp(szString, szOldString)) {
				int nSelect;

				// Save current selection for later
				nSelect = SendMessage(listbox, LB_GETCURSEL, 0, 0);

				// Set new game info
				SendMessage(listbox, LB_DELETESTRING, nIndex, 0);
				SendMessage(listbox, LB_INSERTSTRING, nIndex, (LPARAM)(LPCSTR)szString);

				// Restore selection if necessary
				if (nSelect == nIndex)
					SendMessage(listbox, LB_SETCURSEL, nIndex, 0);
			}
		}
   		curr = curr->next;
	}

	// MAKE SURE THERE ARE NO GAMES IN THE LIST BOX THAT AREN'T IN THE LINKED LIST

	{
	char liststring[2*SNETSPI_MAXSTRINGLENGTH];
	WPARAM index = 0;
	
	while (SendMessage(listbox,LB_GETTEXT,index,(LPARAM)liststring) != LB_ERR) {
		char *p;

		curr = gamelist;

		// Add NULL termination following game name
		p = strchr(liststring, '\t');
		if (p) *p = 0;

    	while (curr) {
    		if (!strcmp(curr->gamename,liststring))
        		break;
    		else
        		curr = curr->next;
    	}
    	if (!curr) {
    		if ((WPARAM)SendMessage(listbox,LB_GETCURSEL,0,0) == index) {
        		SendMessage(listbox,LB_SETCURSEL,index-1,0);
        		SendMessage(dialog,WM_COMMAND,MAKELONG(IDC_GAMELIST,LBN_SELCHANGE),(LPARAM)listbox);
			}
    		SendMessage(listbox,LB_DELETESTRING,index,0);
		}
		else 
			++index;
	}
	}
	 
	SpiUnlockGameList(gamelist, NULL);

	//### PATCH MM 2/5/97
	SrvMaintainLatencies();

	SendMessage(listbox, WM_SETREDRAW, TRUE, 0);

	// update our scroll bar
	ListUpdateScrollbar(listbox);


}


//### PATCH MM 2/3/97
//===========================================================================
static void UpdateGameLags(HWND hWndList) {
	SNETSPI_GAMELIST curr;
	char szString[2*SNETSPI_MAXSTRINGLENGTH];
	int nCount;
	int nGameId, nLag, nTime;
	char *p;
	DWORD dwLatency;

	// Update latency counters
	SrvMaintainLatencies();
	SendMessage(hWndList, WM_SETREDRAW, FALSE, 0);

	nCount = SendMessage(hWndList, LB_GETCOUNT, 0, 0);
	if (nCount == LB_ERR)
		return;

	for (int i=0; i<nCount; i++) {
		int nSelect;

		if (LB_ERR == SendMessage(hWndList, LB_GETTEXT, i, (LPARAM)szString))
			continue;

		// Get the game ID for this game
		p = strchr(szString, '\t');
		if (p == NULL)
			continue;

		p++;
		if (3 != sscanf(p, "%x %x %x", &nGameId, &nLag, &nTime))
			continue;

		if (!SpiGetGameInfo(nGameId, "", "", &curr))
			continue;

		SrvGetLatency(&curr.owner, &dwLatency);

		// Build string again with new information
   		sprintf(szString,"%s\t%x %x %x\t%s",curr.gamename, curr.gameid, NormalizeNetLag(dwLatency), curr.creationtime, curr.gamedescription);

		// Save current selection for later
		nSelect = SendMessage(hWndList, LB_GETCURSEL, 0, 0);

		// Set new game info
		SendMessage(hWndList, LB_DELETESTRING, i, 0);
		SendMessage(hWndList, LB_INSERTSTRING, i, (LPARAM)(LPCSTR)szString);

		// Restore selection if necessary
		if (nSelect == i)
			SendMessage(hWndList, LB_SETCURSEL, i, 0);
	}


	SendMessage(hWndList, WM_SETREDRAW, TRUE, 0);
}
//### END


//===========================================================================
static BOOL JoinDrawItem(UIPARAMSPTR uiparams,
                         UINT message,
                         HWND window,
                         WPARAM wparam,
                         LPARAM lparam)
{
	if (wparam == IDC_GAMEDESCRIPTION) {

		// GET THE GAME NAME AND DESCRIPTION
		char name[256] = "";
		LPSTR szLag = "";
		LPSTR description = "";
		LPSTR p;
		int nLag = 0;
		int nTime = 0;
		LRESULT sel = SendDlgItemMessage(window,IDC_GAMELIST,LB_GETCURSEL,0,0);

		if (sel != LB_ERR)
    		SendDlgItemMessage(window,IDC_GAMELIST,LB_GETTEXT,sel,(LPARAM)name);

		if (NULL != (p = strchr(name,'\t'))) {
			//### PATCH MM 2/3/97  Added gameid to string.
			int nGameId;

			*p++ = 0;

			// Get Lag and Creation Time
			szLag = p;

			//### PATCH MM 2/3/97  Added gameid to string.
			//sscanf(szLag, "%x %x", &nLag, &nTime);
			sscanf(szLag, "%x %x %x", &nGameId, &nLag, &nTime);

			if (NULL != (p = strchr(szLag,'\t'))) {
				description = p;
				*description++ = 0;	
			}
		}


    	// UPDATE THE DESCRIPTION IN THE STATIC TEXT CONTROL
    	{
    		char buffer[256] = "";
    		GetDlgItemText(window,IDC_GAMEDESCRIPTION,buffer,256);
    		buffer[255] = 0;
    		if (strcmp(buffer,description))
    		SetDlgItemText(window,IDC_GAMEDESCRIPTION,description);
    	}

		// IF A DESCRIPTION EXISTS AND IF THE APPLICATION HAS REGISTERED A DRAW DESCRIPTION CALLBACK,
		// LET IT DRAW THE DESCRIPTION
		if (uiparams->interfacedata && uiparams->interfacedata->drawdesccallback) {
			DWORD dwItemFlags, dwDrawFlags;

			dwItemFlags = 0;
			dwDrawFlags = SNET_DDF_MULTILINE;
			return uiparams->interfacedata->drawdesccallback(
												PROVIDERID,
												SNET_DRAWTYPE_GAME,
												name,
												description,
												dwItemFlags,
												dwDrawFlags,
												nTime,
												(LPDRAWITEMSTRUCT)lparam);
		}

		// OTHERWISE, LET THE DEFAULT DIALOG BOX PROCEDURE DRAW (OR ERASE) THE
		// DESCRIPTION FROM THE STATIC TEXT
		else
			goto DRAW_BREAK;

	}
    else if (wparam == IDC_GAMELIST) {
		char szString[2*SNETSPI_MAXSTRINGLENGTH];
		LPSTR szLag = "";
		LPSTR description = "";
		LPSTR p;
		int nLag = 0;
		int nTime = 0;
		COLORREF oldTextColor, oldBkColor;
		LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lparam;
		BOOL bSelected = lpdis->itemState & ODS_SELECTED;

		// Get user name/description
		if (LB_ERR == SendMessage(lpdis->hwndItem, LB_GETTEXT, lpdis->itemID, (LPARAM)(LPCSTR)szString))
			goto DRAW_BREAK;

		// make sure we have something to draw
		if (!szString[0] || !lpdis->hDC) 
			goto DRAW_BREAK;

		if (NULL != (p = strchr(szString,'\t'))) {
			//### PATCH MM 2/3/97  Added gameid to string.
			int nGameId;

			*p++ = 0;

			// Get Lag and Creation Time
			szLag = p;

			//### PATCH MM 2/3/97  Added gameid to string.
			//sscanf(szLag, "%x %x", &nLag, &nTime);
			sscanf(szLag, "%x %x %x", &nGameId, &nLag, &nTime);

			if (NULL != (p = strchr(szLag,'\t'))) {
				description = p;
				*description++ = 0;	
			}
		}

		// Draw gamename and net lag into list box

		// If the app has registered a draw description callback,
		// let it draw the game name in the listbox.
		if (uiparams->interfacedata && uiparams->interfacedata->drawdesccallback) {
			DWORD dwItemFlags, dwDrawFlags;

			dwItemFlags = 0;
			dwDrawFlags = SNET_DDF_INCLUDENAME;
			uiparams->interfacedata->drawdesccallback(
											PROVIDERID,
											SNET_DRAWTYPE_GAME,
											szString,
											description,
											dwItemFlags,
											dwDrawFlags,
											nTime,
											(LPDRAWITEMSTRUCT)lparam);
		}
		else {
			oldTextColor = SetTextColor(lpdis->hDC, RGB(0xff, 0xff, 0xff));
			oldBkColor = SetBkColor(lpdis->hDC, (bSelected) ? GetSysColor(COLOR_HIGHLIGHT) : RGB(0, 0, 0));
			ExtTextOut(
				lpdis->hDC, 
				lpdis->rcItem.left, 
				lpdis->rcItem.top, 
				ETO_CLIPPED | ETO_OPAQUE, 
				&lpdis->rcItem, 
				szString, 
				strlen(szString), 
				NULL);

			// Restore colors to hdc
			SetTextColor(lpdis->hDC, oldTextColor);
			SetBkColor(lpdis->hDC, oldBkColor);
		}

		// Draw net lag
		DrawNetLag(nLag, lpdis);
		return 1;
    }
DRAW_BREAK:
	return SDlgDefDialogProc(window,message,wparam,lparam);
}

//===========================================================================
static void CALLBACK JoinAddCategory(LPCSTR pszCategoryName,
                                     DWORD categorybits,
                                     DWORD categorymask)
{
    if (!sghFilterCombo) return;
    int nPos = SendMessage(sghFilterCombo,
                           CB_ADDSTRING,
                           0,
                           (LPARAM)pszCategoryName);
    if (nPos == CB_ERR) return;
    CATEGORYDATAPTR data = (CATEGORYDATAPTR)ALLOC(sizeof(CATEGORYDATA));
    data->categorybits = categorybits;
    data->categorymask = categorymask;
    SendMessage(sghFilterCombo,CB_SETITEMDATA,(WPARAM)nPos,(LPARAM)data);
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
BOOL CALLBACK JoinGameDialogProc (HWND   window,
                                  UINT   message,
                                  WPARAM wparam,
                                  LPARAM lparam) {

	static UIPARAMSPTR uiparams = NULL;
	static BOOL sbSecondTimerActive = FALSE;
	static HFONT shCustomFont;
	
	switch (message) {
		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDOK: {
					HWND hWndName, hWndPassword;
					char szText[MAX_STRING_LEN];
					SNETSPI_GAMELIST GameInfo;


					if (uiparams->interfacedata->soundcallback)
						uiparams->interfacedata->soundcallback(PROVIDERID, SNET_SND_SELECTITEM, 0);

					hWndName = GetDlgItem(window, IDC_EDIT_NAME);
					hWndPassword = GetDlgItem(window, IDC_EDIT_PASSWORD);



					// First, verify user entered required information
					if (!SendMessage(hWndName, WM_GETTEXTLENGTH, 0, 0L)) {
						LoadString(global_hinstance,IDS_NAME_REQUIRED,szText,MAX_STRING_LEN);
						UiMessageBox(
								uiparams->interfacedata->messageboxcallback, 
								window, 
								szText, 
								"", 
								MB_OK | MB_ICONWARNING);
						return 0;
					}


					// Fill create game structure
					SendMessage(hWndName, WM_GETTEXT, sizeof(sgszGameName), (LPARAM)((LPSTR)sgszGameName));
					SendMessage(hWndPassword, WM_GETTEXT, sizeof(sgszGamePassword), (LPARAM)((LPSTR)sgszGamePassword));

					// Don't let user click ok twice
					EnableWindow((HWND)lparam, FALSE);

					// Get Game info
                    TCHAR szPlayerName[MAX_STRING_LEN];
                    TCHAR szPlayerDesc[MAX_STRING_LEN];
                    SrvGetLocalPlayerName(szPlayerName,MAX_STRING_LEN);
                    SrvGetLocalPlayerDesc(szPlayerDesc,MAX_STRING_LEN);
					if (SpiGetGameInfo (0, sgszGameName, sgszGamePassword, &GameInfo)) {
						// Are we authorized to join this game
						if (!uiparams->interfacedata->authcallback(
								SNET_AUTHTYPE_GAME, 
								szPlayerName, 
								szPlayerDesc,
								gnUserFlags,
								GameInfo.gamedescription,
								szText,
								sizeof(szText))) {
							UiMessageBox(uiparams->interfacedata->messageboxcallback, window, szText, NULL, MB_ICONWARNING | MB_OK);
							EnableWindow((HWND)lparam, TRUE);
							return 0;
						}

						if (SNetJoinGame(0,
										 sgszGameName,
										 sgszGamePassword,
								         szPlayerName, 
								         szPlayerDesc,
										 uiparams->playeridptr)) {
							char progvers[32];
							wsprintf(progvers,"%08x%08x",global_programid,global_versionid);
							SRegSaveString("Recent Games",progvers,SREG_FLAG_BATTLENET,sgszGameName);

							SrvNotifyJoin(sgszGameName, sgszGamePassword);   //### PATCH MM 2/17/97   
							
  							SDlgEndDialog(window, TRUE);
							return 0;
						}
					}

				//### PATCH MM 2/5/97   
					// Either join failed, or we couldn't get GameInfo about it.

					int nError = GetLastError();
					int nMsg = IDS_JOIN_FAILED;		// Default Message

					if (nError == SNET_ERROR_HOST_UNREACHABLE)
						nMsg = IDS_ERR_HOST_UNREACHABLE;
					else if (nError == SNET_ERROR_GAME_FULL)
						nMsg = IDS_ERR_GAME_FULL;

					LoadString(global_hinstance,nMsg,szText,MAX_STRING_LEN);
				//### END

					UiMessageBox(uiparams->interfacedata->messageboxcallback, window, szText, "", MB_OK | MB_ICONWARNING);
					EnableWindow((HWND)lparam, TRUE);
					return 0;
	
			   }
				case IDCANCEL:
					if (uiparams->interfacedata->soundcallback)
						uiparams->interfacedata->soundcallback(PROVIDERID, SNET_SND_SELECTITEM, 0);

					SDlgEndDialog(window, FALSE);
					return 0;

				case IDC_EDIT_NAME:
					if (HIWORD(wparam) == EN_CHANGE) {
						HWND hWndGameList = GetDlgItem(window, IDC_GAMELIST);

						// If the focus is in the edit control, assume the user
						// just modified it, so remove any selection from the list box.
						if ((HWND) lparam == GetFocus()) {
							SendMessage(hWndGameList,LB_SETCURSEL,(WPARAM)-1,0);
			        		SendMessage(window,WM_COMMAND,MAKELONG(IDC_GAMELIST,LBN_SELCHANGE),(LPARAM)hWndGameList);
						}
					}
					break;

				case IDC_GAMELIST: 
					if (HIWORD(wparam) == LBN_DBLCLK) {
						HWND hWndGameList = (HWND)lparam;
						int nIndex;

						nIndex = SendMessage(hWndGameList, LB_GETCURSEL, 0, 0);
						if (nIndex != LB_ERR) {
							// Make sure we update the edit control first
			        		SendMessage(window,WM_COMMAND,MAKELONG(IDC_GAMELIST,LBN_SELCHANGE),(LPARAM)hWndGameList);

							// Behave like OK was just pressed
							SendMessage(window,WM_COMMAND,MAKELONG(IDOK,BN_CLICKED),(LPARAM)GetDlgItem(window, IDOK));
						}

					}
					else if (HIWORD(wparam) == LBN_SELCHANGE) {
						HWND hWndGameList = (HWND)lparam;
						int nIndex = SendMessage(hWndGameList, LB_GETCURSEL, 0, 0);
						char szText[MAX_STRING_LEN];

						// Change text in Edit Control
						if (nIndex != LB_ERR) {
							char *p;

							SendMessage(hWndGameList,LB_GETTEXT, nIndex, (LPARAM)(LPCSTR)szText);


							// NULL terminate game name (chop of description, and latency)
							if (p = strchr(szText,'\t')) 
								*p = 0;

							// Set name and reset password (public games don't have a password)
							SendDlgItemMessage(window, IDC_EDIT_NAME, WM_SETTEXT, 0, (LPARAM)(LPCSTR)szText);
							SendDlgItemMessage(window, IDC_EDIT_PASSWORD, WM_SETTEXT, 0, (LPARAM)(LPCSTR)"");
						}

						InvalidateRect(GetDlgItem(window, IDC_GAMEDESCRIPTION), NULL, TRUE);

						// Update scrollbar in case user scrolled the list with the mouse
						ListUpdateScrollbar(hWndGameList);
					}
					break;
					
                case IDC_FILTER: {
                    if (HIWORD(wparam) == CBN_SELCHANGE) {
                        int nPos = SendMessage((HWND)lparam,CB_GETCURSEL,0,0);
                        if (nPos == CB_ERR) break;
                        CATEGORYDATAPTR data = (CATEGORYDATAPTR)SendMessage((HWND)lparam,
                                                                            CB_GETITEMDATA,
                                                                            (WPARAM)nPos,
                                                                            0);
                        if (!data) break;
                        sgdwCategoryBits = data->categorybits;
                        sgdwCategoryMask = data->categorymask;
                        UpdateGameList(window, GetDlgItem(window, IDC_GAMELIST), TRUE);
                    }
                    break;
                }
   	   		}
   	   		break;

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


		case WM_TIMER:
			if (sbSecondTimerActive) {
				SDlgKillTimer(window, 2);
				sbSecondTimerActive = FALSE;
			}

			//### PATCH MM 2/4/97
			//UpdateGameList(window, GetDlgItem(window, IDC_GAMELIST), uiparams->programdata->selectioncriteria, FALSE);
			UpdateGameLags(GetDlgItem(window, IDC_GAMELIST));
			//### END
			break;

		case WM_DRAWITEM:
		    return JoinDrawItem(uiparams,message,window,wparam,lparam);

		case WM_DESTROY:
		  	DestroyArtwork(window);

            if (sghFilterCombo) {
                int nItems = SendMessage(sghFilterCombo,CB_GETCOUNT,0,0);
                while(nItems--) {
                    CATEGORYDATAPTR data =
                        (CATEGORYDATAPTR)SendMessage(sghFilterCombo,
                                                     CB_GETITEMDATA,
                                                     (WPARAM)nItems,
                                                     0);
                    if (data) FREE(data);
                }
            }

			//### PATCH MM 2/4/97
			//SDlgKillTimer(window, 1);

			if (sbSecondTimerActive)
				SDlgKillTimer(window, 2);

			if (shCustomFont) {
				DeleteObject(shCustomFont);
				shCustomFont = NULL;
			}
			break;
		
   		case WM_INITDIALOG: {
			HWND hWndGameList;
			RECT rect;

			// SAVE A POINTER TO THE USER INTERFACE PARAMETERS
			uiparams = (UIPARAMSPTR)lparam;

			sbSecondTimerActive = TRUE;					

			LoadArtwork(window, uiparams->interfacedata->artcallback);

			hWndGameList = GetDlgItem(window, IDC_GAMELIST);
			ScrollbarLink(hWndGameList, GetDlgItem(window, IDC_SCROLLBAR));

			//### MM Diablo Patch #2  3/21/97   
			//### Added category support
			// DML 10/16/97
			// Made "cancel" out of category callback (filter) possible
			if (uiparams->interfacedata->categorycallback &&
			    !uiparams->interfacedata->
			        categorycallback(FALSE,
							         uiparams->programdata,
							         uiparams->playerdata,
							         uiparams->interfacedata,
							         uiparams->versiondata,
							         &sgdwCategoryBits,
							         &sgdwCategoryMask))
                SDlgEndDialog(window, FALSE);

            if (uiparams->interfacedata->categorylistcallback &&
                NULL != (sghFilterCombo = GetDlgItem(window,IDC_FILTER)))
            {
                uiparams->interfacedata->categorylistcallback(uiparams->playerdata,
                                                              JoinAddCategory);
                SendMessage(sghFilterCombo,CB_SETCURSEL,0,0);
                SendMessage(window,
                            WM_COMMAND,
                            MAKEWPARAM(IDC_FILTER,CBN_SELCHANGE),
                            (LPARAM)sghFilterCombo);
                // CBN_SELCHANGE will cause a call to UpdateGameList
            }
            else {
                sghFilterCombo = NULL;
    			UpdateGameList(window, hWndGameList, TRUE);
            }

			//### PATCH MM 2/4/97
			//### Moved timer stuff below first UpdateGameList() call.
			//### Eliminated 10 second timer.
			//SDlgSetTimer(window,1,10000,NULL);		// Poll for game list every 10 seconds
			//SDlgSetTimer(window,2,1000,NULL);			// One Shot One Second timer (so we can get latency info on games)

			SDlgSetTimer(window, 2, 3000,NULL);			// Update latencies after 3 seconds
			//### END

			// SET TAB STOPS FOR LIST BOX WIDE ENOUGH TO HIDE ALL TABBED TEXT
			GetClientRect(hWndGameList,&rect);
			SendMessage(hWndGameList,LB_SETTABSTOPS,1,(LPARAM) &rect.right);

			// Set the height of the games list box items (so lines don't overlap)
			SendMessage(hWndGameList, LB_SETITEMHEIGHT, 0, 20);

			shCustomFont = InitCustomFonts(window);
		  	return 1;
		}
		
	}

	return SDlgDefDialogProc(window,message,wparam,lparam);

}

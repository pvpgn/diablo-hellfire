/***************************************************************************
*
*  chatroom.cpp
*  battle.net chat room
*
*  By Michael Morhaime
*
*
*  10/26/96 DML - added ladder button
*
***/

#include "pch.h"

#define LADDER_BUTTON   0   // 1 in final
#define TEST_USERLIST	0	// 0 in final

//***************************************************************************
BOOL CALLBACK JoinGameDialogProc (HWND   window,
                                  UINT   message,
                                  WPARAM wparam,
                                  LPARAM lparam);


void ChatChannelJoined(LPCSTR szChannel);

//***************************************************************************
#define MAX_NAME_LEN 128
#define MAX_CHAT_LEN (MAX_MSG_LEN+MAX_NAME_LEN+3)	//+3 is for "<> " around players name
#define MAX_MSG_LEN 256
#define MAX_USERLIST_LEN (2*MAX_NAME_LEN+24)

#define MAX_CHAT_LINES 5000
#define PRIVATE_CHANNEL_INDEX 0

#define MAX_NET_LAG 6
#define LAG_UNIT_WIDTH 3
#define RED_THRESHOLD 5
#define YELLOW_THRESHOLD 3

#define CHAT_INDENT 12

#define BEGIN_NAME_CHAR '<'
#define END_NAME_CHAR	'>'

#define PREFSKEY			"Preferences"
#define PREFSVALUE_VERBOSE	"Verbose"

#define USER_OFFICIAL_MASK (UF_BLIZZARD|UF_MODERATOR|UF_SPEAKER|UF_SYSOP)
#define IsUserOfficial() (gnUserFlags & USER_OFFICIAL_MASK) 





	   HWND   ghWndChat			= NULL;		
static HWND   sghWndMsgList		= NULL;
static HFONT  sghChatFont		= 0;

static LPBYTE backgroundbitmap	= NULL;
static LPBYTE bnBtnbitmap	    = NULL;
static LPBYTE SmlBtnbitmap	    = NULL;

static LPBYTE redlagbitmap		= NULL;
static LPBYTE yellowlagbitmap	= NULL;
static LPBYTE greenlagbitmap	= NULL;
static LPBYTE bmpBadConn		= NULL;

static SIZE   redSize;
static SIZE   yellowSize;
static SIZE   greenSize;
static SIZE	  sizeBadConn;

static SIZE	  sgBtnSize;

static char gszCurrentChannel[MAX_MSG_LEN] = "";
static BOOL sgbModeratedChannel = FALSE;

static int sgnTab = CHAT_INDENT;
static BOOL sgbLadderEnabled;

enum _hilite_colors {
	FIRST_COLOR_CHAR = 0x10,
	eCOLOR_NORMAL = FIRST_COLOR_CHAR,
	eCOLOR_WHISPER,
	eCOLOR_NOTIFY,
	eCOLOR_USERNAME,
	eCOLOR_MYNAME,
	eCOLOR_BLIZZARD,
	eCOLOR_SYSOP, 
	eCOLOR_MODERATOR,
	eCOLOR_SPEAKER,
	eCOLOR_BROADCAST,
	eCOLOR_INFORMATION,
	eCOLOR_ERROR,
	LAST_COLOR_CHAR = eCOLOR_ERROR,
};

static COLORREF sgHiliteColors[] = {
	RGB(0xff, 0xff, 0xff),		// COLOR_NORMAL			(white)
	RGB(0x80, 0x80, 0x80),		// COLOR_WHISPER		(gray)
	RGB(0,    0xff, 0),			// COLOR_NOTIFY			(green)
	RGB(0xff, 0xff, 0),			// COLOR_USERNAME		(yellow)
	RGB(0,    0xff, 0xff),		// COLOR_MYNAME			(cyan)
	RGB(0,    0xff, 0xff),		// COLOR_BLIZZARD		(cyan)
	RGB(0,    0xff, 0xff),		// COLOR_SYSOP			(cyan)
	RGB(0xff, 0xff, 0xff),		// COLOR_MODERATOR		(white)
	RGB(0xff, 0xff, 0),			// COLOR_SPEAKER		(yellow)
	RGB(0xff, 0,    0),			// COLOR_BROADCAST		(red)
	RGB(0xff, 0xff, 0),			// COLOR_INFORMATION	(yellow)
	RGB(0xff, 0,	0),			// COLOR_ERROR			(red)
};



typedef struct _SQUELCH_LIST {
	char szName[MAX_NAME_LEN];
    _SQUELCH_LIST  *next;
} TSQUELCH_LIST, *PTSQUELCH_LIST;

static PTCHANNEL_LIST sgpChannelListHead = NULL;
static PTSQUELCH_LIST sgpSquelchListHead = NULL;
static char sgszUserName[MAX_NAME_LEN] = "";
static char sgszUserDesc[MAX_USERLIST_LEN];
static DWORD sgdwVerboseMode = FALSE;		// Default join/leave messages to OFF
static BOOL sgbFirstChannel = FALSE;
int gnUserFlags = 0;

//===========================================================================
// External Decl
//===========================================================================
extern BOOL ChatSelectChannel(SNETUIDATAPTR interfacedata, char *szChannel, PTCHANNEL_LIST pChannelListHead);

//===========================================================================
// Forward Decl
//===========================================================================
void ChatAddUser (SNADDUSERPTR pAddUserRec);
void ChatDeleteUser(SNDELETEUSERPTR pDeleteUserRec);
void ChatAddChannel(LPCSTR szChannel);
void ChatDeleteChannel(LPCSTR szChanell);
void ChatJoinChannel(SNJOINCHANNELPTR pJoinChannelRec);
void ChatReceiveMsg(SNDISPLAYSTRINGPTR pDispStringRec);





//===========================================================================
static void DestroyArtwork (HWND window) {
	if (backgroundbitmap) {
		FREE(backgroundbitmap);
		backgroundbitmap = NULL;
	}

	if (bnBtnbitmap) {
		FREE(bnBtnbitmap);
		bnBtnbitmap = NULL;
	}

	if (SmlBtnbitmap) {
		FREE(SmlBtnbitmap);
		SmlBtnbitmap = NULL;
	}

	if (redlagbitmap) {
		FREE(redlagbitmap);
		redlagbitmap = NULL;
	}

	if (yellowlagbitmap) {
		FREE(yellowlagbitmap);
		yellowlagbitmap = NULL;
	}

	if (greenlagbitmap) {
		FREE(greenlagbitmap);
		greenlagbitmap = NULL;
	}
	if (bmpBadConn) {
		FREE(bmpBadConn);
		bmpBadConn = NULL;
	}
}

//===========================================================================
static BOOL LoadArtwork (HWND window, SNETGETARTPROC artcallback) {
	int btn_ids[] = {
		ID_CHATCHANNEL,
		ID_CHATCREATE,
		ID_CHATJOIN,
		IDQUIT,
		0,
	};
    int ladder_ids[] = {
		ID_CHATCHANNEL,
		ID_CHATCREATE,
		ID_CHATJOIN,
		ID_CHATLADDER,
		IDQUIT,
		0,
    };

	int btn_small[] = {
		IDOK,
		ID_WHISPER,
		0,
	};

	int static_txt[] = {
		IDC_STATIC_CHANNEL,
		0,
	};

	SIZE bnBtnSize;
	SIZE SmlBtnSize;
	SIZE bgSize;


	UiLoadArtwork(
			artcallback,
			window,
			NULL,
			SNET_ART_BATTLE_CHAT_BKG,
			TEXT(""),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,	
			FALSE,				// Don't get palette from this artwork
			FALSE,				// Don't set pallete
			&backgroundbitmap, 
			&bgSize);



	UiLoadArtwork(
			artcallback,
			NULL,
			NULL,
			SNET_ART_BATTLE_GREENLAG,
			TEXT(""),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,
			FALSE,
			FALSE,
			&greenlagbitmap,
			&greenSize);

	UiLoadArtwork(
			artcallback,
			NULL,
			NULL,
			SNET_ART_BATTLE_YELLOWLAG,
			TEXT(""),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,
			FALSE,
			FALSE,
			&yellowlagbitmap,
			&yellowSize);

	UiLoadArtwork(
			artcallback,
			NULL,
			NULL,
			SNET_ART_BATTLE_REDLAG,
			TEXT(""),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,
			FALSE,
			FALSE,
			&redlagbitmap,
			&redSize);

	UiLoadArtwork(
			artcallback,
			NULL,
			NULL,
			SNET_ART_BATTLE_BTNS,
			TEXT("Button"),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,
			FALSE,
			FALSE,
			&bnBtnbitmap,
			&bnBtnSize);

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
			&SmlBtnbitmap,
			&SmlBtnSize);


	UiLoadArtwork(
			artcallback,
			NULL,
			NULL,
			SNET_ART_BATTLE_BADCONNECTION,
			TEXT(""),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,
			FALSE,
			FALSE,
			&bmpBadConn,
			&sizeBadConn);

    if (sgbLadderEnabled)
        SDlgSetControlBitmaps (window,
                               ladder_ids,
                               NULL,
                               bnBtnbitmap,
                               &bnBtnSize,
                               SDLG_ADJUST_VERTICAL);
    else
    	SDlgSetControlBitmaps (window,
    	                       btn_ids,
    	                       NULL,
    	                       bnBtnbitmap,
    	                       &bnBtnSize,
    	                       SDLG_ADJUST_VERTICAL);
    	                       
	SDlgSetControlBitmaps (window, btn_small, NULL, SmlBtnbitmap, &SmlBtnSize, SDLG_ADJUST_VERTICAL);
	SDlgSetControlBitmaps (window, static_txt, NULL, backgroundbitmap, &bgSize, SDLG_ADJUST_CONTROLPOS);

	return 0;
}






//===========================================================================
static BOOL CALLBACK ChatHelpDialogProc (HWND   window,
                                       UINT   message,
                                       WPARAM wparam,
                                       LPARAM lparam) {

	static UIPARAMSPTR uiparams = NULL;
	static LPBYTE spHelpBmp = NULL;
	static LPBYTE spHelpBtnsBmp = NULL;
	static HGLOBAL hResource;


	switch (message) {
		case WM_COMMAND: 
			switch (LOWORD(wparam)) {
				case IDOK: 
				case IDCANCEL:
					if (uiparams->interfacedata->soundcallback)
						uiparams->interfacedata->soundcallback(PROVIDERID, SNET_SND_SELECTITEM, 0);

					SDlgEndDialog(window, 0);
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

		case WM_HELP:
			// Prevent parent window from bringing up another help window
			return 1;


		case WM_DESTROY:
			if (spHelpBmp) {
				FREE(spHelpBmp);
				spHelpBmp = NULL;
			}

			if (spHelpBtnsBmp) {
				FREE(spHelpBtnsBmp);
				spHelpBtnsBmp = NULL;
			}

			//This is not necessary in Win95 or WinNT
			//FreeResource(hResource);
 			break;

   		case WM_INITDIALOG: {
			char szHelpRes[20];
			char *pText;
			char *pSearch;
			char *pHelpText;
			int i;

			HRSRC hRsrc;
			SIZE sizeBtns;
			int nSize;
			int btn_ids[] = {
				IDOK,
				IDCANCEL,
				0,
			};

			// SAVE A POINTER TO THE USER INTERFACE PARAMETERS
			uiparams = (UIPARAMSPTR)lparam;

			// Load artwork from callback
			UiLoadArtwork(
					uiparams->interfacedata->artcallback,
					window,
					NULL,
					SNET_ART_HELPBACKGROUND,
					TEXT(""),
					SDLG_STYLE_ANY,
					SDLG_USAGE_BACKGROUND,	
					FALSE,
					FALSE,
					&spHelpBmp,
					NULL);


			UiLoadArtwork(
					uiparams->interfacedata->artcallback,
					NULL,
					NULL,
					SNET_ART_BUTTON_SML,
					TEXT("Button"),
					SDLG_STYLE_ANY,
					SDLG_USAGE_BACKGROUND,
					FALSE,
					FALSE,
					&spHelpBtnsBmp,
					&sizeBtns);

			SDlgSetControlBitmaps (window, btn_ids, NULL, spHelpBtnsBmp, &sizeBtns, SDLG_ADJUST_VERTICAL);


			// Now load in help text

			LoadString(global_hinstance, IDS_CHAT_HELP_RES, szHelpRes, sizeof(szHelpRes));
			hRsrc = FindResource(global_hinstance, szHelpRes, "TEXT");
			hResource = LoadResource(global_hinstance, hRsrc);

			pText = (char *) LockResource(hResource);
			nSize = SizeofResource(global_hinstance, hRsrc);

			// Make a copy of the resource, since Windows doesn't seem to 
			// like when we modify the original data.  Not sure why.
			pHelpText = (char *)ALLOC(nSize);
			if (!pHelpText)
				return 1;

			memcpy(pHelpText, pText, nSize);
			for (i=0, pSearch=pHelpText; i<nSize; i++, pSearch++) {
				// Replace '\' in text file with null termination
				if (*pSearch == '\\') {
					*pSearch = 0;
					break;
				}
			}
			
			HWND hWndScroll = GetDlgItem(window, IDC_SCROLLBAR);
			HWND hWndEdit = GetDlgItem(window, IDC_EDIT1);
			SendMessage(hWndEdit, WM_SETTEXT, 0, (LPARAM)(LPCSTR)pHelpText);
			FREE(pHelpText);

			ScrollbarLink(hWndEdit, hWndScroll);
			EditUpdateScrollbar(hWndEdit);
		  	return 1;
		}

	}

	return SDlgDefDialogProc(window,message,wparam,lparam);
}


 

//===========================================================================
static BOOL DoChatSelectChannel(SNETUIDATAPTR interfacedata, char *szChannel, PTCHANNEL_LIST pChannelListHead) {
	HWND hWndParent = interfacedata->parentwindow;
	DWORD dwReturn;


	// Hide the chat room so its controls don't get drawn when Select Channel Dlg disappears.
	SetActiveWindow(hWndParent);
	ShowWindow(ghWndChat, SW_HIDE);
	
    // Display the Select Channel Dlg
    dwReturn = ChatSelectChannel(interfacedata, szChannel, pChannelListHead);

	// Restore Chatroom always
	ShowWindow(ghWndChat, SW_SHOW);

	return dwReturn;
}
//===========================================================================
static BOOL DoCreateGame (UIPARAMSPTR uiparams) {
	DWORD dwReturn;
	HWND hWndParent = uiparams->interfacedata->parentwindow;

	if (!uiparams->interfacedata->createcallback) 
		return 0;

    // BUILD A NEW INTERFACE DATA STRUCTURE CONTAINING OUR WINDOW HANDLE
    SNETUIDATA interfacedata;
    ZeroMemory(&interfacedata,sizeof(SNETUIDATA));
    if (uiparams->interfacedata)
	    CopyMemory(&interfacedata,uiparams->interfacedata,sizeof(SNETUIDATA));
    interfacedata.size         = sizeof(SNETUIDATA);
    interfacedata.parentwindow = hWndParent;

    // BUILD A CREATION DATA STRUCTURE
    SNETCREATEDATA createdata;
    ZeroMemory(&createdata,sizeof(SNETCREATEDATA));
    createdata.size              = sizeof(SNETCREATEDATA);
    createdata.providerid        = PROVIDERID;
    createdata.maxplayers        = global_maxplayers;
    createdata.createflags       = SNET_CF_ALLOWPRIVATEGAMES;

	// Hide the chat room so its controls don't get drawn when Create Dlg disappears.
	SetActiveWindow(hWndParent);
	ShowWindow(ghWndChat, SW_HIDE);
	
    // CALL THE CREATE GAME CALLBACK 
    dwReturn = uiparams->interfacedata->createcallback(&createdata,
												uiparams->programdata,
												uiparams->playerdata,
                                                &interfacedata,
                                                uiparams->versiondata,
                                                uiparams->playeridptr);


	// If game was not created, restore chatroom.
	if (!dwReturn)
		ShowWindow(ghWndChat, SW_SHOW);

	return dwReturn;
}



//===========================================================================
static BOOL DoJoinGame (UIPARAMSPTR uiparams) {
	DWORD dwReturn;
	HWND hWndParent = uiparams->interfacedata->parentwindow;

	// Hide chat room so it's controls don't get drawn after Join Dialog disappears, unless we want
	SetActiveWindow(hWndParent);
	ShowWindow(ghWndChat, SW_HIDE);

    // Determine which join game dialog to display
	TCHAR *pszTemplate = TEXT("DIALOG_JOIN_GAME");
    if (uiparams->interfacedata->categorylistcallback)
        pszTemplate = TEXT("DIALOG_FILTER_JOIN_GAME");
	
	dwReturn = SDlgDialogBoxParam(
							global_hinstance,
							pszTemplate,
							hWndParent,
							JoinGameDialogProc, 
							(LPARAM)uiparams);

	// Convert return value to BOOLEAN
	dwReturn = (dwReturn == 1);

	// If user didn't join a game, restore chatroom.
	if (!dwReturn)
		ShowWindow(ghWndChat, SW_SHOW);

	return dwReturn;
}


//===========================================================================
static HFONT SetChatFonts (HWND window) {
	HFONT hFont = NULL;
	LOGFONT lFont;

	if ((hFont = (HFONT) SendMessage(window, WM_GETFONT, 0, 0L))) {
		// Start with dialog font and modify size and weight
		if (GetObject(hFont, sizeof(LOGFONT), (LPSTR) &lFont)) {
			lFont.lfHeight = -MulDiv(8, 96, 72);	// height of 8 pixels
			lFont.lfWidth = 0;						// let Windows autosize width
			lFont.lfWeight = FW_NORMAL;				// change to non Bold
			strcpy(lFont.lfFaceName, "Arial");


			if (hFont = CreateFontIndirect((LPLOGFONT) &lFont)) {
				SendDlgItemMessage(window, IDC_USERLIST, WM_SETFONT, (WPARAM)hFont, 0);
				SendDlgItemMessage(window, IDC_CHATWINDOW, WM_SETFONT, (WPARAM)hFont, 0);
				SendDlgItemMessage(window, IDC_CHATEDIT, WM_SETFONT, (WPARAM)hFont, 0);
				SendDlgItemMessage(window, IDC_STATIC_CHANNEL, WM_SETFONT, (WPARAM)hFont, 0);
			}
		}
	}
	return hFont;
}

//===========================================================================
static void ToggleVerboseMode(void) {
	char szText[256];
	char szFmt[128];


	sgdwVerboseMode = !sgdwVerboseMode;
	LoadString(
		global_hinstance,
		(sgdwVerboseMode) ? IDS_VERBOSE_FMT : IDS_NONVERBOSE_FMT,
		szFmt,
		sizeof(szFmt));
	sprintf(szText, szFmt, eCOLOR_NOTIFY, szText);
	SendMessage(ghWndChat, WM_CHAT_PROCESS_MSG, eCOLOR_NOTIFY, (LPARAM)szText);

    SRegSaveValue(PREFSKEY,PREFSVALUE_VERBOSE,SREG_FLAG_BATTLENET,sgdwVerboseMode);
}

//===========================================================================
static void SendChatMsg(char *szText, BOOL bWhisper) {
	if (bWhisper) {
		// Fix up text string so that message goes only to the people specified
		char szString[MAX_MSG_LEN*2];
		char szUser[128];
		char *pUserName;
		int nIdx;
		HWND hWndUserList;
		

		hWndUserList = GetDlgItem(ghWndChat, IDC_USERLIST);
		nIdx = SendMessage(hWndUserList, LB_GETCURSEL, 0, 0);
		if (nIdx == LB_ERR)
			return;

		// Send a whisper message to selected user
		SendMessage(hWndUserList, LB_GETTEXT, nIdx, (LPARAM)(LPCSTR)szUser);
		pUserName = strchr(szUser, '\t');
		if (!pUserName)
			return;

		// NULL terminate name
		*pUserName = 0;
		sprintf(szString, "/whisper %s %s", szUser, szText);
		SrvSendChatString(szString);
	}
	else
		SrvSendChatString (szText);
}

									 

//****************************************************************************
// ScrollToBtm(HWND)
//
//****************************************************************************
static void ScrollToBtm(HWND hWnd) {
	SCROLLINFO si;
	int nPos = SendMessage(hWnd, LB_GETCOUNT, 0, 0)-1;
	SendMessage(hWnd, LB_SETTOPINDEX, nPos, 0);

	// Update scrollbar that belongs to this listbox
	HWND hWndScroll = (HWND)GetWindowLong(hWnd, GWL_USERDATA);
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS;
	si.nPos = nPos;
	SendMessage(hWndScroll, SBM_SETSCROLLINFO, TRUE, (LPARAM)&si);
}

//****************************************************************************
// DisplayMsg(szMsg, bTextColor)
//
// bTextColor is the color that additional lines of text should be printed in.
//****************************************************************************
static void DisplayMsg(LPCSTR szMsg, BYTE bTextColor) {
	BYTE charSave;
	char *pszText;
	HDC hDC;
	int nFit;
	SIZE size;
	RECT windowRect;
	HFONT hOldFont;
	BOOL bScrollToBtm = TRUE;			// Normally, we'll want to always scroll to end of msg list
	BOOL bDeletedLines = FALSE;
	int nPixHt,nItems, nTopItem;
	int nTotalLines;

	// make sure we are still in the chatroom
	if (!sghWndMsgList)
		return;

	hDC = GetDC(sghWndMsgList);
	if (!hDC)
		return;
	
	pszText = (char *)szMsg;

	hOldFont = (HFONT)SelectObject(hDC, sghChatFont);

	GetClientRect(sghWndMsgList, &windowRect);


	// First set flag to indicate if list box is already scrolled to the end
	nPixHt = SendMessage(sghWndMsgList, LB_GETITEMHEIGHT, 0, 0);
	nItems = SendMessage(sghWndMsgList, LB_GETCOUNT, 0, 0);
	nTopItem = SendMessage(sghWndMsgList, LB_GETTOPINDEX, 0, 0);

	// If user has scrolled up in list, we don't want to mess with the scroll position
	if (nPixHt != LB_ERR && nItems != LB_ERR) {
		if (nTopItem + windowRect.bottom/nPixHt < nItems)
			bScrollToBtm = FALSE;
	}


	// Don't allow redraw until we have finished making changes to the listbox
	SendMessage(sghWndMsgList, WM_SETREDRAW, FALSE, 0);

	// add message to list box 
	while (TRUE) {
		int nStrLen;
		int nLine;
		int nRight;
		char *p;


		// GetTextExtentExPoint() doesn't take tabs into account, so adjust width of line manually.
		nRight = windowRect.right;
		p = pszText;
		if (pszText[0] == '\t') {
			nRight -= CHAT_INDENT;
			p++;
		}

		// If there is a color byte here, don't include it in the extent calculation
		if (*p>=FIRST_COLOR_CHAR && *p<=LAST_COLOR_CHAR)
			p++;

		nStrLen = strlen((const char *)p);
		GetTextExtentExPoint(
			hDC, 
			(const char *)p, 
			nStrLen, 
			nRight,
			&nFit, 
			NULL, 
			&size); 
		

		// Search for a good place to end the line
		if (nFit != nStrLen) {
			for (nLine=nFit; nLine>0; nLine--) {
				if (p[nLine] == ' ') {
					nFit = nLine;
					break;
				}
			}
		}

		charSave = p[nFit];
		p[nFit] = 0;		// null terminate first part of string

		// add the text line by line to the list box
		nTotalLines = SendMessage(sghWndMsgList, LB_ADDSTRING, 0, (LPARAM) ((LPSTR)pszText));


		// did we get the whole string that time?
		if (nStrLen == nFit)
			break;

		// Skip past current line
		pszText = p + nFit;
		if (charSave == ' ')  pszText++;		// Don't print a space at the beginning of a line
		else  pszText[0] = charSave;

		// Indent start of next line
		pszText-=2;
		pszText[0] = '\t';
		pszText[1] = bTextColor;
	}

	// Have we gone past our line limit?
	if (nTotalLines >= MAX_CHAT_LINES) {
		for ( ; nTotalLines >= MAX_CHAT_LINES; nTotalLines--)
		// Remove oldest messages first
		SendMessage(sghWndMsgList, LB_DELETESTRING, 0, 0);
		bDeletedLines = TRUE;
	}


	// Update position in listbox
	if (bScrollToBtm) 
		ScrollToBtm(sghWndMsgList);
	else if (bDeletedLines) {
		// restore scroll position after LB_DELETESTRING
		SendMessage(sghWndMsgList, LB_SETTOPINDEX, max(0,nTopItem-1), 0);	
	}

	// Okay, now we're ready to redraw
	SendMessage(sghWndMsgList, WM_SETREDRAW, TRUE, 0);

	// Update scrollbar that belongs to this listbox
	ListUpdateScrollbar(sghWndMsgList);	

	// Cleanup gdi objects
	SelectObject(hDC, hOldFont);
	ReleaseDC(sghWndMsgList, hDC);
}


//****************************************************************************
#if TEST_USERLIST
void CreateDummyUsers(void) {
	SNADDUSERREC AddUserRec;
	char szName[128];
	char szDesc[128];


	AddUserRec.name = (LPSTR) szName;
	AddUserRec.description = (LPSTR) szDesc;
	AddUserRec.flags = 0;

	for (int i=0;i<50; i++) {
		sprintf(szName, "Mike%d", i);
		sprintf(szDesc, "%d %d 0 0 0 0 0", i, i%3);
		ChatAddUser(&AddUserRec);
	}

}

void AddDummyUser(void) {
	SNADDUSERREC AddUserRec;
	char szName[128];
	char szDesc[128];


	AddUserRec.name = (LPSTR) szName;
	AddUserRec.description = (LPSTR) szDesc;
	AddUserRec.flags = 0;

	static int i=0;
	i++;
	if (i>99) i=0;
	sprintf(szName, "Mike%d", i);
	sprintf(szDesc, "%d %d 0 0 0 0 0", i, i%3);
	ChatAddUser(&AddUserRec);
}

void DeleteDummyUser(void) {
	SNDELETEUSERREC DeleteUserRec;
	char szName[128];

	DeleteUserRec.name = (LPSTR) szName;
	DeleteUserRec.notifyuser = 1;

	static int i = 2;
	i++;
	if (i>99) i=0;
	sprintf(szName, "Mike%d", i);
	ChatDeleteUser(&DeleteUserRec);
}
#endif


//****************************************************************************
//****************************************************************************
int ListFindName(HWND hWndList, LPCSTR szName) {
	int nItems;
	BOOL bFound;
	char *p;
	int i;

	nItems = SendMessage(hWndList, LB_GETCOUNT, 0, 0);
	if (nItems == LB_ERR)
		return LB_ERR;

	bFound = FALSE;
	for (i=0; i<nItems; i++) {
		char szText[2*SNETSPI_MAXSTRINGLENGTH];
		int nLen;

		nLen = SendMessage(hWndList, LB_GETTEXTLEN, i, 0);
		if (nLen == LB_ERR || nLen >= sizeof(szText) || nLen == 0)
			continue;

		SendMessage(hWndList, LB_GETTEXT, i, (LPARAM)(LPCSTR)szText);

		// A player name entry has a '\t' after the name before battle.net data
		p = strchr(szText, '\t');
		if (p == NULL)
			continue;

		// NULL terminate player name so we can do string compare.
		*p = 0;

		if (!strcmp((char *)szText, szName)) {
			bFound = TRUE;
			break;
		}
	}

	if (!bFound)
		i = LB_ERR;

	return i;
}

//****************************************************************************
//****************************************************************************
static void BuildMsg(LPSTR pszMsg, LPCSTR pszFromTo, LPCSTR pszSender, LPCSTR pszString, BYTE bColorName,  BYTE bColorString) {
	sprintf(pszMsg, "%c%c%s%s%c %c%s", 
				bColorName, 
				BEGIN_NAME_CHAR, 
				pszFromTo,
				pszSender, 
				END_NAME_CHAR, 
				bColorString,
				pszString);
}



//****************************************************************************
//****************************************************************************
static BOOL CopyNameToEditCtl(HWND hWndList, HWND hWndEdit) {
	char szUser[128];
	char *pUserName;
	int nIdx;
	

	nIdx = SendMessage(hWndList, LB_GETCURSEL, 0, 0);
	if (nIdx == LB_ERR)
		return 0;

	SendMessage(hWndList, LB_GETTEXT, nIdx, (LPARAM)(LPSTR)szUser);
	pUserName = strchr(szUser, '\t');
	if (!pUserName)
		return 0;

	// NULL terminate name
	*pUserName = 0;

	// Copy text to current cursor position
	SendMessage(hWndEdit, EM_REPLACESEL, TRUE, (LPARAM)(LPSTR)szUser);
	return 1;
}

//****************************************************************************
//****************************************************************************
static BOOL IsUserSquelched(LPCSTR szName) {
	PTSQUELCH_LIST pCurr;

	pCurr = sgpSquelchListHead;
	while (pCurr) {
		// Search for this name through tht squelch list
		if (!strcmp(pCurr->szName, szName))
			return TRUE;

		pCurr = pCurr->next;
	}

	// This guy is okay
	return FALSE;
}
						 

//****************************************************************************
//****************************************************************************
void ChannelWindowUpdate(void) {
	char szText[256];
	char szFmt[64];
	int nUsers;

	// Update Channel description to include # of people in room
	nUsers = SendMessage(GetDlgItem(ghWndChat, IDC_USERLIST), LB_GETCOUNT, 0, 0);
	LoadString(global_hinstance, IDS_CHANNEL_FMT, szFmt, sizeof(szFmt));
	sprintf(szText, szFmt, gszCurrentChannel, nUsers);
	SetWindowText(GetDlgItem(ghWndChat, IDC_STATIC_CHANNEL), szText);
}


/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
UINT ChatGetUserFlags(void) {
	return gnUserFlags;
}


//===========================================================================
int NormalizeNetLag(DWORD dwMilliSec) {
	// Assume if lag is <10millisecs, that we are dealing with the player himself
	if (dwMilliSec < 10)
		return 0;

	// divide millisec by 100
	dwMilliSec/=200;

	if (dwMilliSec >= MAX_NET_LAG)
		return MAX_NET_LAG;

	// Best non-zero lag is a single green bar!
	if (!dwMilliSec)
		return 1;


	return dwMilliSec;
}


//===========================================================================
void DrawBadConnection(LPDRAWITEMSTRUCT lpdis) {
	RECT srcRect;


	// init source rect
	SetRect(&srcRect, 0, 0, (LAG_UNIT_WIDTH*MAX_NET_LAG)-1, sizeBadConn.cy-1);

	// use this to center icon vertically
	int vertOff = ((lpdis->rcItem.bottom - lpdis->rcItem.top + 1) - sizeBadConn.cy)/2;

	SDlgBltToWindow(
		lpdis->hwndItem, 
		NULL, 
		lpdis->rcItem.right - (LAG_UNIT_WIDTH*(MAX_NET_LAG+1)), 
		lpdis->rcItem.top + vertOff,
		bmpBadConn,
		&srcRect,
		&sizeBadConn);
}


//===========================================================================
void DrawNetLag(int nNetLag, LPDRAWITEMSTRUCT lpdis) {
	LPBYTE bmp;
	SIZE size;
	RECT srcRect;

	if (!nNetLag)
		return;

	// Get netlag color
	if (nNetLag >= RED_THRESHOLD) {
		bmp = redlagbitmap;
		size = redSize;
	}
	else if (nNetLag < YELLOW_THRESHOLD) {
		bmp = greenlagbitmap;
		size = greenSize;
	}
	else {
		bmp = yellowlagbitmap;
		size = yellowSize;
	}

	// init source rect
	srcRect.left = 0;
	srcRect.right = LAG_UNIT_WIDTH*nNetLag - 1;
	srcRect.top = 0;
	srcRect.bottom = size.cy - 1;
	

	// We'll use a rating of 1 to MAX_NET_LAG (MAX_NET_LAG being slowest)
	int vertOff = ((lpdis->rcItem.bottom - lpdis->rcItem.top + 1) - size.cy)/2;


	SDlgBltToWindow(
		lpdis->hwndItem, 
		NULL, 
		lpdis->rcItem.right - (LAG_UNIT_WIDTH*(MAX_NET_LAG+1)), 
		lpdis->rcItem.top + vertOff,
		bmp,
		&srcRect,
		&size);
}



//****************************************************************************
void ChatAddUser (SNADDUSERPTR pAddUserRec) {
	HWND hWndList;
	char szText[MAX_USERLIST_LEN];
	char szFmt[128];

	if (!ghWndChat)
		return;


	hWndList = GetDlgItem(ghWndChat, IDC_USERLIST);

	// Make sure we haven't already added this person to the listbox
	if (LB_ERR != ListFindName(hWndList, (LPCSTR)pAddUserRec->name))
		return;

	// Add squelched flag, if this user is squelched
	if (IsUserSquelched(pAddUserRec->name))
		pAddUserRec->flags |= UF_SQUELCHED;

	sprintf(szText, "%s\t%d %d\t%s", 
		pAddUserRec->name,
		pAddUserRec->flags,
		0,								// Starting ping time
		pAddUserRec->description);

	// Add player name to user list box (official users get added at the top of the list)
	SendMessage(
			hWndList, 
			(pAddUserRec->flags & USER_OFFICIAL_MASK) ? LB_INSERTSTRING : LB_ADDSTRING, 
			0, 
			(LPARAM)(LPCSTR)szText);

	if (!sgbModeratedChannel && pAddUserRec->notifyuser && sgdwVerboseMode) {
		// display "Johnny has entered"
		LoadString(global_hinstance, IDS_CHAT_PLAYER_ENTERED_ROOM, szFmt, sizeof(szFmt));
		sprintf(szText, szFmt, eCOLOR_NOTIFY, pAddUserRec->name);
		SendMessage(ghWndChat, WM_CHAT_PROCESS_MSG, eCOLOR_NOTIFY, (LPARAM)szText);
	}

	if (!strcmp(pAddUserRec->name, sgszUserName)) {
		gnUserFlags = pAddUserRec->flags;
		strcpy(sgszUserDesc, pAddUserRec->description);
	}

	ChannelWindowUpdate();

	// Update scrollbar that belongs to this listbox
	ListUpdateScrollbar(hWndList);	
}

//****************************************************************************
void ChatDeleteUser(SNDELETEUSERPTR pDeleteUserRec) {
	HWND hWndList;
	char szText[MAX_USERLIST_LEN];
	char szFmt[128];
	int nIndex;
	int nTopIndex;


	if (!ghWndChat)
		return;

	// Remove player name from user list box
	hWndList = GetDlgItem(ghWndChat, IDC_USERLIST);
	nIndex = ListFindName(hWndList, (LPCSTR)pDeleteUserRec->name);
	if (nIndex == LB_ERR)
		return;

	// Delete the user from the list
	nTopIndex = SendMessage(hWndList, LB_GETTOPINDEX, 0, 0);
	SendMessage(hWndList, WM_SETREDRAW, FALSE, 0);
	SendMessage(hWndList, LB_DELETESTRING, nIndex, 0);
	SendMessage(hWndList, LB_SETTOPINDEX, nTopIndex, 0);	// restore scroll position after LB_DELETESTRING

	SendMessage(hWndList, WM_SETREDRAW, TRUE, 0);

	if (!sgbModeratedChannel && pDeleteUserRec->notifyuser && sgdwVerboseMode) {
		// display "Johnny has left"
		LoadString(global_hinstance, IDS_CHAT_PLAYER_LEFT_ROOM, szFmt, sizeof(szFmt));
		sprintf(szText, szFmt, eCOLOR_NOTIFY, pDeleteUserRec->name);
		SendMessage(ghWndChat, WM_CHAT_PROCESS_MSG, eCOLOR_NOTIFY, (LPARAM)szText);
	}

	// Update users in channel
	ChannelWindowUpdate();

	// Update scrollbar that belongs to this listbox
	ListUpdateScrollbar(hWndList);	
}


//****************************************************************************
void ChatUpdatePingTime(SNUPDATEPINGTIMEPTR pUpdatePingTimeRec) {
	HWND hWndList;
	int nIndex, nCurSel, nTopIndex;
	DWORD dwFlags, dwNewPingTime, dwOldPingTime;

	char szNewUserText[MAX_USERLIST_LEN];
	char szOldUserText[MAX_USERLIST_LEN];
	char *pszName;
	char *pszInfo;
	char *pszDesc;


	if (!ghWndChat)
		return;

	hWndList = GetDlgItem(ghWndChat, IDC_USERLIST);
	nIndex = ListFindName(hWndList, (LPCSTR)pUpdatePingTimeRec->name);
	if (nIndex == LB_ERR)
		return;

	// Get current entry for this user
	SendMessage(hWndList, LB_GETTEXT, nIndex, (LPARAM)(LPCSTR)szOldUserText);

	// Get pointer to user name
	pszName = &szOldUserText[0];

	// Get pointer to user info
	pszInfo = strchr(szOldUserText, '\t');
	if (!pszInfo)
		return;
	*pszInfo++ = 0;					// Null terminate name, and pointer to user info


	// Get pointer to user description
	pszDesc = strchr(pszInfo, '\t');
	if (!pszDesc)
		return;
	*pszDesc++ = 0;					// Null terminate info and point to user description

	// Get old information about the player
	if (2 != sscanf(pszInfo, "%d %d", &dwFlags, &dwOldPingTime)) 
		return;

	// Update new ping time
	dwNewPingTime = NormalizeNetLag(pUpdatePingTimeRec->pingtime);

	// Check if ping time has changed
	if (dwNewPingTime == dwOldPingTime)
		return;

	// Now rebuild text string
	sprintf(szNewUserText, "%s\t%d %d\t%s", 
		pszName,
		dwFlags,
		dwNewPingTime,								
		pszDesc);


	// Save a couple of things for later
	nTopIndex = SendMessage(hWndList, LB_GETTOPINDEX, 0, 0);
	nCurSel = SendMessage(hWndList, LB_GETCURSEL, 0, 0);

	// Actually set the new user entry
	SendMessage(hWndList, WM_SETREDRAW, FALSE, 0);
	SendMessage(hWndList, LB_DELETESTRING, nIndex, 0);
	SendMessage(hWndList, LB_INSERTSTRING, nIndex, (LPARAM)(LPCSTR)szNewUserText);
	if (nCurSel == nIndex)
		SendMessage(hWndList, LB_SETCURSEL, nCurSel, 0);

	// We need to restore the scrollbox position, since LB_DELETESTRING seems to scroll to the top.
	SendMessage(hWndList, LB_SETTOPINDEX, nTopIndex, 0);

	SendMessage(hWndList, WM_SETREDRAW, TRUE, 0);

	// Update scrollbar that belongs to this listbox
	ListUpdateScrollbar(hWndList);	
}


//****************************************************************************
void ChatSetUserName(LPCSTR szUserName) {
	SrvGetLocalPlayerName(sgszUserName,MAX_NAME_LEN);
#if 0
	if (strlen(szUserName) >= sizeof(sgszUserName)) {
		memcpy(sgszUserName, szUserName, sizeof(sgszUserName));
		sgszUserName[sizeof(sgszUserName)-1] = 0;
	}
	else
		strcpy(sgszUserName, szUserName);
#endif
}

//****************************************************************************
void ChatChangeUserFlags(SNCHANGEUSERFLAGSPTR pChangeUserFlagsRec) {
	HWND hWndList;
	int nOldIndex, nNewIndex;
	DWORD dwOldFlags, dwNewFlags, dwPingTime;
	char szNewUserText[MAX_USERLIST_LEN];
	char szOldUserText[MAX_USERLIST_LEN];
	char *pszName;
	char *pszInfo;
	char *pszDesc;
	BOOL bWasOfficial, bIsOfficial;


	if (!ghWndChat)
		return;

	hWndList = GetDlgItem(ghWndChat, IDC_USERLIST);
	nOldIndex = ListFindName(hWndList, (LPCSTR)pChangeUserFlagsRec->name);
	if (nOldIndex == LB_ERR)
		return;

	// Get current entry for this user
	SendMessage(hWndList, LB_GETTEXT, nOldIndex, (LPARAM)(LPCSTR)szOldUserText);

	// Get pointer to user name
	pszName = &szOldUserText[0];

	// Get pointer to user info
	pszInfo = strchr(szOldUserText, '\t');
	if (!pszInfo)
		return;
	*pszInfo++ = 0;					// Null terminate name, and pointer to user info


	// Get pointer to user description
	pszDesc = strchr(pszInfo, '\t');
	if (!pszDesc)
		return;
	*pszDesc++ = 0;					// Null terminate info and point to user description

	// Get old information about the player
	if (2 != sscanf(pszInfo, "%d %d", &dwOldFlags, &dwPingTime)) 
		return;

	// Check if flags have really changed
	dwNewFlags = pChangeUserFlagsRec->flags;
	if (IsUserSquelched(pChangeUserFlagsRec->name))
		dwNewFlags |= UF_SQUELCHED;

	if (dwNewFlags == dwOldFlags)
		return;

	// Now rebuild text string
	sprintf(szNewUserText, "%s\t%d %d\t%s", 
		pszName,
		dwNewFlags,
		dwPingTime,								
		pszDesc);

	// Save new user flags
	if (!strcmp(pChangeUserFlagsRec->name, sgszUserName))
		gnUserFlags = dwNewFlags;

	bWasOfficial = (0 != (dwOldFlags & USER_OFFICIAL_MASK));
	bIsOfficial  = (0 != (dwNewFlags & USER_OFFICIAL_MASK));


	// Actually set the new user entry
	int nTopIndex = SendMessage(hWndList, LB_GETTOPINDEX, 0, 0);
	int nCurSel = SendMessage(hWndList, LB_GETCURSEL, 0, 0);
	SendMessage(hWndList, WM_SETREDRAW, FALSE, 0);
	SendMessage(hWndList, LB_DELETESTRING, nOldIndex, 0);

	if (bWasOfficial == bIsOfficial) {
		nNewIndex = SendMessage(hWndList, LB_INSERTSTRING, nOldIndex, (LPARAM)(LPCSTR)szNewUserText);
	}
	else {
		// Move user to new position in list based on new flags 
		// (Special users get moved to the top, non-specials get moved to the bottom)
		nNewIndex = SendMessage(
							hWndList, 
							(bIsOfficial) ? LB_INSERTSTRING : LB_ADDSTRING, 
							0, 
							(LPARAM)(LPCSTR)szNewUserText);
	}


	// LB_DELETESTRING moves the scroll position, so reset it to where it was
	SendMessage(hWndList, LB_SETTOPINDEX, nTopIndex, 0);
	if (nCurSel == nOldIndex)
		SendMessage(hWndList, LB_SETCURSEL, nNewIndex, 0);

	SendMessage(hWndList, WM_SETREDRAW, TRUE, 0);

	// Update scrollbar that belongs to this listbox
	ListUpdateScrollbar(hWndList);	
}

//****************************************************************************
// NOTE: pJoinChannelRec will only be valid as long as we don't go into a peekmessage loop.
//****************************************************************************
void ChatJoinChannel(SNJOINCHANNELPTR pJoinChannelRec) {
	HWND hWndList;
	char szText[MAX_CHAT_LEN];
	char szFmt[128];

	// if this is the first channel we are entering, add this channel to our list of known channels
	if (sgbFirstChannel) {
		ChatAddChannel(pJoinChannelRec->name);
		sgbFirstChannel = FALSE;
	}

	// Save channel name
	strcpy(gszCurrentChannel,pJoinChannelRec->name);
	sgbModeratedChannel = pJoinChannelRec->flags & CF_MODERATED;

	if (ghWndChat) {
		// Let select channel window know, in case user is waiting for confirmation
		ChatChannelJoined(pJoinChannelRec->name);

		hWndList = GetDlgItem(ghWndChat, IDC_USERLIST);

		// Dump all users from list and prepare to add the new ones.
		SendMessage(hWndList, LB_RESETCONTENT, 0, 0);

		// Display a message that we are in a new room now
		LoadString(global_hinstance, IDS_CHAT_JOIN_ROOM, szFmt, sizeof(szFmt));
		sprintf(szText, szFmt, eCOLOR_NOTIFY, pJoinChannelRec->name);
		SendMessage(ghWndChat, WM_CHAT_PROCESS_MSG, eCOLOR_NOTIFY, (LPARAM)szText);

		// Update scrollbar that belongs to this listbox
		ListUpdateScrollbar(hWndList);	

		// Set button text to display new channel name
		ChannelWindowUpdate();
	}

}


//****************************************************************************
void ChatAddChannel(LPCSTR szChannel) {
	TCHANNEL_LIST ChannelList;
	TCHANNEL_LIST *pCurr;

	strcpy(ChannelList.szChannel, szChannel);

	pCurr = sgpChannelListHead;
	while (pCurr) {
		// If this string already exists in our list, don't bother adding it again
		if (!strcmp(pCurr->szChannel, szChannel))
			return;

		pCurr = pCurr->next;
	}

	LISTADDEND(&sgpChannelListHead, &ChannelList);
	return;
}

//****************************************************************************
void ChatDeleteChannel(LPCSTR szChannel) {
	TCHANNEL_LIST *pCurr;

	pCurr = sgpChannelListHead;
	while (pCurr) {
		// If this string already exists in our list, don't bother adding it again
		if (!strcmp(pCurr->szChannel, szChannel)) {
			LISTFREE(&sgpChannelListHead, pCurr);
			return;
		}

		pCurr = pCurr->next;
	}

	return;
}


//****************************************************************************
void ChatSquelchUser(SNSQUELCHUSERPTR pSquelchUserRec) {
	TSQUELCH_LIST SquelchList;
	TSQUELCH_LIST *pCurr;
	SNCHANGEUSERFLAGSREC ChangeUserFlagsRec;
	char szText[256];
	char szFmt[128];

	strcpy(SquelchList.szName, pSquelchUserRec->name);

	pCurr = sgpSquelchListHead;
	while (pCurr) {
		// If this string already exists in our list, don't bother adding it again
		if (!strcmp(pCurr->szName, pSquelchUserRec->name))
			return;

		pCurr = pCurr->next;
	}

	LISTADDEND(&sgpSquelchListHead, &SquelchList);

	ChangeUserFlagsRec.name = pSquelchUserRec->name;
	ChangeUserFlagsRec.flags = pSquelchUserRec->flags | UF_SQUELCHED;
	ChatChangeUserFlags(&ChangeUserFlagsRec);

	// Display a message that user is being squelched
	LoadString(global_hinstance, IDS_USERSQUELCHED_FMT, szFmt, sizeof(szFmt));
	sprintf(szText, szFmt, eCOLOR_NOTIFY, pSquelchUserRec->name);
	SendMessage(ghWndChat, WM_CHAT_PROCESS_MSG, eCOLOR_NOTIFY, (LPARAM)szText);

	return;
}

//****************************************************************************
void ChatUnsquelchUser(SNSQUELCHUSERPTR pSquelchUserRec) {
	SNCHANGEUSERFLAGSREC ChangeUserFlagsRec;
	TSQUELCH_LIST *pCurr;
	char szText[256];
	char szFmt[128];

	pCurr = sgpSquelchListHead;
	while (pCurr) {
		// If this user is currently squelched, remove him from squelch list
		if (!strcmp(pCurr->szName, pSquelchUserRec->name)) {
			LISTFREE(&sgpSquelchListHead, pCurr);
			break;
		}

		pCurr = pCurr->next;
	}

	ChangeUserFlagsRec.name = pSquelchUserRec->name;
	ChangeUserFlagsRec.flags = pSquelchUserRec->flags;
	ChatChangeUserFlags(&ChangeUserFlagsRec);

	// Display a message that user is no longer being squelched
	LoadString(global_hinstance, IDS_USERUNSQUELCHED_FMT, szFmt, sizeof(szFmt));
	sprintf(szText, szFmt, eCOLOR_NOTIFY, pSquelchUserRec->name);
	SendMessage(ghWndChat, WM_CHAT_PROCESS_MSG, eCOLOR_NOTIFY, (LPARAM)szText);
	return;
}


//****************************************************************************
void ChatReceiveMsg(SNDISPLAYSTRINGPTR pDispStringRec) {
	BYTE szFromTo[16];
	BYTE szMsg[MAX_CHAT_LEN+3+sizeof(szFromTo)];
	BYTE bColorName, bColorString;
	BYTE bShowName = TRUE;


	// Init this to null string, its only used when whispering is occuring
	szFromTo[0] = 0;

	// Set colors
	switch (pDispStringRec->stringtype) {
		case SN_STRING_WHISPER:
			// Ignore squelched people
			if (IsUserSquelched(pDispStringRec->sender)) 
				return;

			bColorName = eCOLOR_USERNAME;
			bColorString = eCOLOR_WHISPER;
			LoadString(global_hinstance, IDS_WHISPER_FROM, (char *)szFromTo, sizeof(szFromTo));
			break;

		case SN_STRING_WHISPERSENT:
			bColorName = eCOLOR_MYNAME;
			bColorString = eCOLOR_WHISPER;
			LoadString(global_hinstance, IDS_WHISPER_TO, (char *)szFromTo, sizeof(szFromTo));
			break;


		case SN_STRING_BROADCAST:
			bColorString = eCOLOR_BROADCAST;
			bShowName = FALSE;
			break;

		case SN_STRING_INFORMATION:
			bColorString = eCOLOR_INFORMATION;
			bShowName = FALSE;
			break;

		case SN_STRING_ERROR:
			bColorString = eCOLOR_ERROR;
			bShowName = FALSE;
			break;

		case SN_STRING_TALK:
			// Ignore squelched people
			if (IsUserSquelched(pDispStringRec->sender)) 
				return;

			if (pDispStringRec->senderflags & UF_BLIZZARD) {
				bColorName = eCOLOR_BLIZZARD;
				bColorString = eCOLOR_BLIZZARD;
				break;
			}
			else if (pDispStringRec->senderflags & UF_SYSOP) {
				bColorName = eCOLOR_SYSOP;
				bColorString = eCOLOR_SYSOP;
				break;
			}
			else if (pDispStringRec->senderflags & UF_SPEAKER) {
				bColorName = eCOLOR_SPEAKER;
				bColorString = eCOLOR_SPEAKER;
				break;
			}
			else if (pDispStringRec->senderflags & UF_MODERATOR) {
				bColorName = eCOLOR_MODERATOR;
				bColorString = eCOLOR_MODERATOR;
				break;
			}
			// fall through
		default:
			bColorName = eCOLOR_USERNAME;
			bColorString = eCOLOR_NORMAL;
			break;
	}



	// Format string with sender's name in front
	if (bShowName && pDispStringRec->sender[0])
		BuildMsg((LPSTR)szMsg, 
				(LPCSTR) szFromTo, 
				pDispStringRec->sender, 
				pDispStringRec->string, 
				bColorName, bColorString);
	else
		sprintf((char *)szMsg, "%c%s",
					bColorString,
					pDispStringRec->string);

	SendMessage(ghWndChat, WM_CHAT_PROCESS_MSG, bColorString, (LPARAM)szMsg);
}



//===========================================================================
BOOL CALLBACK ChatRoomDialogProc (HWND   window,
                                  UINT   message,
                                  WPARAM wparam,
                                  LPARAM lparam) {

	static UIPARAMSPTR uiparams = NULL;
	static HWND hWndEdit;


	switch (message) {
		case WM_COMMAND: 
			switch (LOWORD(wparam)) {
				case ID_WHISPER: {
					char szTitle[32];
					char szText[256];	

					// make sure we have selected a user to whisper to
					if (LB_ERR == SendDlgItemMessage(window, IDC_USERLIST, LB_GETCURSEL, 0, 0)) {

						LoadString(global_hinstance, IDS_BATTLENET, szTitle, sizeof(szTitle));
						LoadString(global_hinstance, IDS_ERR_CANTWHISPER, szText, sizeof(szText));

						// Don't let user press whisper while error message is up
						EnableWindow((HWND)lparam, FALSE);
						uiparams->interfacedata->messageboxcallback(
										window,
										szText, 
										szTitle, 
										MB_OK | MB_ICONERROR);	
						EnableWindow((HWND)lparam, TRUE);

						return 1;
					}
				}
				// Fall through to IDOK
				//
				//
				case IDOK: {
					char szText[MAX_CHAT_LEN];	

					// Get message from edit control. Only send the chat message, if the edit control contained text.
					if (SendMessage(hWndEdit, WM_GETTEXT, MAX_MSG_LEN, (LPARAM)(LPSTR)szText) ) {
						SendChatMsg((char *)szText, (LOWORD(wparam) == ID_WHISPER));

						// Clear edit text
						SendMessage(hWndEdit, WM_SETTEXT, 0, (LPARAM) ((LPSTR)""));

						// Local echo, unless we are whispering or user typed a command string
						if (LOWORD(wparam) == IDOK && szText[0] != '/') {
							char szMsg[MAX_CHAT_LEN+3];	// include space for "< >" around name
                            char szPlayerName[MAX_NAME_LEN];
							int nTextColor;

                            SrvGetLocalPlayerName(szPlayerName,MAX_NAME_LEN);
   							nTextColor = (sgbModeratedChannel && !IsUserOfficial()) ? 
													eCOLOR_WHISPER : eCOLOR_NORMAL;
							BuildMsg((char *)szMsg, 
									"",					
									szPlayerName,
									(LPCSTR)szText, 
									eCOLOR_MYNAME, 
									nTextColor);

							SendMessage(ghWndChat, WM_CHAT_PROCESS_MSG, nTextColor, (LPARAM)szMsg);
						}	

						ScrollToBtm(sghWndMsgList);
					}

					// Give focus back to edit control
					SetFocus(hWndEdit);
					return 1;

				}				

				case ID_VERBOSE: 
					ToggleVerboseMode();
					return 1;


				case ID_CHATCHANNEL: {
					if (uiparams->interfacedata->soundcallback)
						uiparams->interfacedata->soundcallback(PROVIDERID, SNET_SND_SELECTITEM, 0);

					DoChatSelectChannel(uiparams->interfacedata, gszCurrentChannel, sgpChannelListHead);
					SetFocus(hWndEdit);
					return 1;
			    }

				case IDC_USERLIST: {
					if (HIWORD(wparam) == LBN_SELCHANGE) {
						// Give the focus back to the edit control
						SetFocus(hWndEdit);

						ListUpdateScrollbar((HWND) lparam);
					}
					else if (HIWORD(wparam) == LBN_DBLCLK) {
						CopyNameToEditCtl((HWND)lparam, hWndEdit);
						SetFocus(hWndEdit);
					}
					break;
				  }
					

				case IDC_CHATWINDOW:
					SetFocus(hWndEdit);
					return 0;

				case ID_CHATCREATE: {
					if (uiparams->interfacedata->soundcallback)
						uiparams->interfacedata->soundcallback(PROVIDERID, SNET_SND_SELECTITEM, 0);

					if (DoCreateGame(uiparams)) 
						SDlgEndDialog(window,1);
					else 
						SetFocus(hWndEdit);
					return 1;
				}
	
		        case ID_CHATJOIN:
					if (uiparams->interfacedata->soundcallback)
						uiparams->interfacedata->soundcallback(PROVIDERID, SNET_SND_SELECTITEM, 0);

					if (DoJoinGame(uiparams))
						SDlgEndDialog(window, 1);
					else 
						SetFocus(hWndEdit);
					return 1;
	
				case IDQUIT:
					if (uiparams->interfacedata->soundcallback)
						uiparams->interfacedata->soundcallback(PROVIDERID, SNET_SND_SELECTITEM, 0);

					SDlgEndDialog(window, 0);
					return 1;
			
   	   		}
   	   		break;

		case WM_CLOSE:
			// Simulate a Quit button click so that we exit
			SendDlgItemMessage(window, IDQUIT, BM_CLICK, 0, 0);
			return 1;

		case WM_HELP:
#if TEST_USERLIST
			{
	// Get Users coming and going constantly (whenever a key is pressed)
	for (int i=0; i<5000; i++) {
		static BOOL sbAdd=FALSE;
		if (sbAdd) 
			AddDummyUser();
		else
			DeleteDummyUser();
		sbAdd = !sbAdd;
	}
	return 1;
			}
#endif


			if (uiparams->interfacedata->soundcallback)
				uiparams->interfacedata->soundcallback(PROVIDERID, SNET_SND_SELECTITEM, 0);

			SDlgDialogBoxParam(	global_hinstance,
								TEXT("DIALOG_CHAT_HELP"), 
								window,
								ChatHelpDialogProc,
								(LPARAM) uiparams);
			SetFocus(hWndEdit);
			return 1;


		case WM_DRAWITEM: {
			UINT idCtl = (UINT) wparam;							// control identifier 
			LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lparam; // item-drawing information 
			char szString[256];
			BOOL bFirstLine = TRUE;
			int nNetLag = 0;
			DWORD dwItemFlags = 0;

			if (idCtl == IDC_CHATWINDOW) {
				COLORREF oldTextColor, oldBkColor;
				LPSTR pText = szString;
				UINT nAlignMode;
				POINT pt;
				int nStartX;


				// Get line of text
				if (LB_ERR == SendMessage(lpdis->hwndItem, LB_GETTEXT, lpdis->itemID, (LPARAM)(LPCSTR)szString))
					return 0;

				// make sure we have something to draw
				if (!szString[0] || !lpdis->hDC) 
					return 0;


				oldTextColor = SetTextColor(lpdis->hDC, RGB(0xff, 0xff, 0xff));
				oldBkColor = SetBkColor(lpdis->hDC, RGB(0,0,0));

				// manually indent string, if necessary
				nStartX = lpdis->rcItem.left + 1;	// +1 is so letters like W will not get clipped on the left
				if (pText[0] == '\t') {
					nStartX += CHAT_INDENT;
					pText++;
					bFirstLine = FALSE;
				}
				MoveToEx(
					lpdis->hDC, 
					nStartX,
					lpdis->rcItem.top, 
					&pt);

				nAlignMode = SetTextAlign(lpdis->hDC, TA_UPDATECP);

				// Print Text
				while (*pText) {
					int nCount; 

					// Check for color control character
					if (*pText >= FIRST_COLOR_CHAR && *pText <= LAST_COLOR_CHAR) {
						SetTextColor(lpdis->hDC, sgHiliteColors[*pText - FIRST_COLOR_CHAR]);
						pText++;
					}

					// Count characters that are in this color
					char *p = pText;
					for (nCount = 0;  *p != 0; nCount++, *p++) {
						if (*p >= FIRST_COLOR_CHAR && *p <= LAST_COLOR_CHAR)
							break;
					}

					// Draw characters
					TextOut(lpdis->hDC,0,0,pText,nCount);

					// Skip characters that we've drawn
					pText+=nCount;
				}


				// Restore colors to hdc
				SetTextColor(lpdis->hDC, oldTextColor);
				SetBkColor(lpdis->hDC, oldBkColor);

				// Restore align mode
				SetTextAlign(lpdis->hDC, nAlignMode);

				// restore position also
				MoveToEx(lpdis->hDC, pt.x, pt.y, NULL);
				return 1;
			}
      		else if (idCtl == IDC_USERLIST) {
        		char *szDesc = "";
				char *szPlayerData = "";
				char *p;

				// Get user name/description
				if (LB_ERR == SendMessage(lpdis->hwndItem, LB_GETTEXT, lpdis->itemID, (LPARAM)(LPCSTR)szString))
					return 0;

				// Get player data
				p = strchr(szString,'\t');
				if (p) {
					*p++ = 0;
					szPlayerData = p;

					// Get player description
					p = strchr(szPlayerData,'\t');
					if (p) {
						*p++ = 0;
						szDesc = p;
					}
				}

				// make sure we have something to draw
				if (!szString[0] || !lpdis->hDC) 
					return 0;

				// If item count changes, change if constant
				if (2 != sscanf(szPlayerData, "%d %d", &dwItemFlags, &nNetLag)) {
//			#ifndef NDEBUG
//					char txt[256];
//					sprintf(txt, "Info: %s", szPlayerData);
//					MessageBox(SDrawGetFrameWindow(), txt, "", MB_OK);
//					return 0;
//			#endif
				}

        		// IF THE APPLICATION HAS REGISTERED A DRAW DESCRIPTION CALLBACK,
        		// LET IT DRAW THE DESCRIPTION
        		if (uiparams->interfacedata && uiparams->interfacedata->drawdesccallback) {
					DWORD dwDrawFlags;

					dwDrawFlags = SNET_DDF_INCLUDENAME;
          			uiparams->interfacedata->drawdesccallback(
									PROVIDERID,
									SNET_DRAWTYPE_PLAYER,
                                    szString,
                                    szDesc,
									dwItemFlags,
									dwDrawFlags,
									0,
                                    lpdis);
				}
				else {
	        		// OTHERWISE, DRAW THE USER'S NAME OURSELF
					COLORREF oldTextColor, oldBkColor;
					BOOL bSelected = lpdis->itemState & ODS_SELECTED;

					// Draw username and net lag into list box
					// Don't worry about text highlighting
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
				if (!(dwItemFlags & UF_BADCONNECTION))
					DrawNetLag(nNetLag, lpdis);
				else
					DrawBadConnection(lpdis);

				return 1;
      		}

			break;
		}

		case WM_CHAT_PROCESS_MSG:
			DisplayMsg((LPCSTR)lparam, wparam);
			return 1;

		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			SendMessage(SDrawGetFrameWindow(), message, wparam, lparam);
			break;


		case WM_DESTROY:
			SrvEndChat();
		  	DestroyArtwork(window);
		  	if (sghChatFont)
		  		DeleteObject(sghChatFont);
			
			ghWndChat = sghWndMsgList = NULL;
			hWndEdit = NULL;
			LISTCLEAR(&sgpChannelListHead);
			LISTCLEAR(&sgpSquelchListHead);
			break;
		

   		case WM_INITDIALOG: 

			// SAVE A POINTER TO THE USER INTERFACE PARAMETERS
			uiparams = (UIPARAMSPTR)lparam;

            sgbLadderEnabled = uiparams->interfacedata->uiflags &
                SNET_UIFLAG_SUPPORTS_LADDER ? TRUE : FALSE;
			LoadArtwork(window, uiparams->interfacedata->artcallback);

		  	sghChatFont = SetChatFonts(window);

			// save some window handles for later
			ghWndChat = window;
			sghWndMsgList = GetDlgItem(window, IDC_CHATWINDOW);
			hWndEdit = GetDlgItem(window, IDC_CHATEDIT);

			// Link Userlist scrollbar to UserList Listbox
			ScrollbarLink(GetDlgItem(window, IDC_USERLIST), GetDlgItem(window, IDC_USERLIST_SCROLLBAR));

			// Link Messagelist scrollbar to msglist listbox
			ScrollbarLink(sghWndMsgList, GetDlgItem(window, IDC_MSGLIST_SCROLLBAR));


			SendMessage(hWndEdit, EM_LIMITTEXT, MAX_MSG_LEN-1, 0L);
			SendMessage(sghWndMsgList, LB_SETTABSTOPS, 1, (LPARAM) (int *) &sgnTab);

			// Set the height of the chat text list box items (so we can fit more lines in it)
			SendMessage(sghWndMsgList, LB_SETITEMHEIGHT, 0, 14);

			// Set hight of userlist box items (some systems get strange heights)
			SendDlgItemMessage(window, IDC_USERLIST, LB_SETITEMHEIGHT, 0, 16);

			// Set flag for first time
			sgbFirstChannel = TRUE;

			// Get the local user's name
			SrvGetLocalPlayerName(sgszUserName,MAX_NAME_LEN);
			
			if (!SrvBeginChat(uiparams->programdata, uiparams->playerdata, gszCurrentChannel))	{
				// Have parent window display error message. 
				PostMessage(GetParent(sghWndMsgList), WM_ERR_NOTRESPONDING, 0, 0);
				SDlgEndDialog(window, 0);
			}


			// Don't let player with bad connection do anything (encourage them to 
			// change their ISP).
			if (gbConnectionSucks) {
				EnableWindow(GetDlgItem(window, ID_CHATCREATE), FALSE);
				EnableWindow(GetDlgItem(window, ID_CHATJOIN), FALSE);
			}

			// Hide 'verbose' button. It will still generate a WM_COMMAND
			// if user hits Alt-V hotkey.
			ShowWindow(GetDlgItem(window, ID_VERBOSE), SW_HIDE);

			// Set Verbose preference
		    SRegLoadValue(PREFSKEY,PREFSVALUE_VERBOSE,SREG_FLAG_BATTLENET,&sgdwVerboseMode);

			UiLoadCursors(window, uiparams->interfacedata);

#if TEST_USERLIST
			CreateDummyUsers();
#endif
		  	return 1;
	}

	return SDlgDefDialogProc(window,message,wparam,lparam);

}

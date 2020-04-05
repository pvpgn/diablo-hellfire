//****************************************************************************
//	SelIPX.cpp
//	Diablo UI select IPX multiplayer game
//	
//	By Frank Pearce
//	created 11.6.96
//****************************************************************************


#include "pch.h"
#include "artfont.h"
#include "uisnd.h"


#define SB_ALWAYS_ON	0		// 0 in final

#define MIN_DESC_LEN	16

//****************************************************************************
//****************************************************************************
	extern BOOL Txt2HeroInfo(char *szDesc, TUIHEROINFO *pHeroInfo);
	extern void TitleLogoDestroy(void);
	extern void TitleLogoInit(HWND window);
	extern void TitleFramesInit(HWND window, LPCTSTR filename);
	extern void TitleLogoAnimate(HWND window);

	extern int SelOkDialog(HWND parent, LPCTSTR text, LPCTSTR title, BOOL loadfocus);

	extern DWORD UiGetProviderID(void);

	extern BOOL UiDoomCreateGame (SNETCREATEDATAPTR   createdata,
								  SNETPROGRAMDATAPTR  programdata,
								  SNETPLAYERDATAPTR   playerdata,
								  SNETUIDATAPTR       interfacedata,
								  SNETVERSIONDATAPTR  versiondata,
								  DWORD              *playerid,
								  BOOL				  loadfocus,
								  LPCSTR			  gamename);


	static void SelIPXShiftTab(HWND child);

//****************************************************************************
//****************************************************************************
	#define OLDPROCPROP			TEXT("UIOLDPROC")
	#define PROVIDERID			'IPXN'

	#define MILLISEC_PER_SEC	1000

	#define FOCUS_TIMER_ID		1
	#define FOCUS_FPS			16		// frames per second
	#define FOCUS_TIMER_DELAY	55		//(MILLISEC_PER_SEC / FOCUS_FPS)

	#define LOGO_TIMER_ID		2
	#define LOGO_FPS			17
	#define LOGO_TIMER_DELAY	55

	#define ENUM_TIMER_ID		3		// how often we re-enum available games
	#define ENUM_TIMER_DELAY	1000

	#define MAX_VISIBLE			6


//****************************************************************************
//****************************************************************************
	static int sgTitleIDs[] = {
		IDC_DLGTITLE,
		IDC_DLGTITLE2,
		0
	};
	static int sgTextIDs[] = {
		IDC_DLGTITLE3,
		0
	};
	static int sgGameDescID[] = {
		IDC_GAMEDESC,
		0
	};
	static int sgBtnIDs[] = {
		IDC_FAKEOK,
		IDC_FAKECANCEL,
		0
	};
	static int sgListIDs[] = {
		IDC_GAMEBTN1,
		IDC_GAMEBTN2,
		IDC_GAMEBTN3,
		IDC_GAMEBTN4,
		IDC_GAMEBTN5,
		IDC_GAMEBTN6,
		0
	};


//****************************************************************************
//****************************************************************************
	#define GAMENAME_LEN		SNETSPI_MAXSTRINGLENGTH
	#define DESCRIPTION_LEN		SNETSPI_MAXSTRINGLENGTH

	typedef struct _ipxgame TIPXGAME;
	typedef TIPXGAME *TPIPXGAME;
	struct _ipxgame {
		TPIPXGAME next;
		DWORD gameid;
		BOOL  gamevalid;
		TCHAR gamename[GAMENAME_LEN];
		TCHAR gamedesc[DESCRIPTION_LEN];
	};
	static TPIPXGAME sgGameList;
	static int sgIPXListLen;
	static BOOL sgGamesChanged;

	static SNETPROGRAMDATAPTR  sgIPXProgramData;
	static SNETPLAYERDATAPTR   sgIPXPlayerData;
	static SNETUIDATAPTR       sgIPXInterfaceData;
	static SNETVERSIONDATAPTR  sgIPXVersionData;
	static DWORD			  *sgpIPXPlayerID;


//****************************************************************************
//****************************************************************************
TPIPXGAME SelIPXAllocNode(void) {
	return (TPIPXGAME) ALLOC(sizeof(TIPXGAME));
}
//****************************************************************************
//****************************************************************************
static void SelIPXFreeNode(TPIPXGAME node) {
	if (node) FREE(node);
}
//****************************************************************************
//****************************************************************************
static void SelIPXFreeList(TPIPXGAME list) {
	TPIPXGAME next;

	while (list) {
		next = list->next;
		SelIPXFreeNode(list);
		list = next;
	}
}
//****************************************************************************
//****************************************************************************
static TPIPXGAME SelIPXAddNode(TPIPXGAME list, TPIPXGAME node) {
	node->next = list;
	return node;
}
//****************************************************************************
// insert a node at the end of the list
//****************************************************************************
static TPIPXGAME SelIPXAddEnd(TPIPXGAME list, TPIPXGAME node) {
	TPIPXGAME head = list;
	TPIPXGAME curr = list;
	TPIPXGAME prev = NULL;

//	while (curr && curr->gameid) {
	while (curr) {
		prev = curr;
		curr = curr->next;
	}

	node->next = curr;
	if (prev)
		prev->next = node;
	else
		head = node;

	return head;
}
//****************************************************************************
//****************************************************************************
static TPIPXGAME SelIPXFindNode(TPIPXGAME list, DWORD gameid) {
	while (list) {
		if (list->gameid == gameid)
			break;
		list = list->next;
	}
	return list;
}
static TPIPXGAME SelIPXFindNodeName(TPIPXGAME list, LPCSTR name) {
	while (list) {
		if (! stricmp(list->gamename, name))
			break;
		list = list->next;
	}
	return list;
}



//****************************************************************************
//****************************************************************************
static void SelIPXInvalAll(TPIPXGAME list) {
	while (list) {
		list->gamevalid = FALSE;
		list = list->next;
	}
}


//****************************************************************************
//****************************************************************************
static TPIPXGAME SelIPXDeleteInval(TPIPXGAME list) {
	TPIPXGAME head = list;
	TPIPXGAME curr = list;
	TPIPXGAME prev = NULL;

	while (curr) {
		// skip valid nodes & the "create" node
		if ((curr->gamevalid) || (! curr->gameid)) {
			prev = curr;
			curr = curr->next;
			continue;
		}

		// set the next of the previous to point at next of invalid node
		if (prev)
			prev->next = curr->next;
		else
			head = curr->next;

		// remove the invalid node from the list
		SelIPXFreeNode(curr);
		sgGamesChanged = TRUE;
		sgIPXListLen--;

		// set the new curr
		if (prev)
			curr = prev->next;
		else
			curr = head->next;
	}

	return head;
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK SelIPXEnum(DWORD  gameid,
								LPCSTR gamename,
								LPCSTR gamedesc) {
	TPIPXGAME newgame;

	// see if the game is already in the list
	if (NULL != (newgame = SelIPXFindNode(sgGameList, gameid))) {
		newgame->gamevalid = TRUE;
		return TRUE;
	}

	// the game needs to be added to the list
	newgame = SelIPXAllocNode();
	if (! newgame) return FALSE;
	newgame->gameid = gameid;
	newgame->gamevalid = TRUE;
	newgame->next = NULL;
	strcpy(newgame->gamename, gamename);
	strcpy(newgame->gamedesc, gamedesc);
	sgGameList = SelIPXAddEnd(sgGameList, newgame);
	sgIPXListLen++;
	sgGamesChanged = TRUE;
	return TRUE;
}


//****************************************************************************
//****************************************************************************
static void SelIPXSetGames(HWND window, TPIPXGAME gamelist) {
	HWND child;
	TPBMP tpBmp;
	int * ctrlid = sgListIDs;

	for (; *ctrlid; ctrlid++) {
		child = GetDlgItem(window, *ctrlid);
		if (! child) continue;

		if (! gamelist) {
			// no list entry for this button
			if (child == GetFocus())
				SelIPXShiftTab(child);
			EnableWindow(child, FALSE);
			tpBmp = (TPBMP) GetWindowLong(child, GWL_USERDATA);
			if (tpBmp) {
				tpBmp->userdata = (LONG) NULL;
				UiSetBmpText(tpBmp, "");
			}
		}
		else {
			// set the button to use this list entry
			EnableWindow(child, TRUE);
			tpBmp = (TPBMP) GetWindowLong(child, GWL_USERDATA);
			if (tpBmp) {
				tpBmp->userdata = (LONG) gamelist;
				UiSetBmpText(tpBmp, gamelist->gamename);
			}
			gamelist = gamelist->next;
		}
	}

	UiDoomButtonsReset(window, sgListIDs, AF_MED);
}


//****************************************************************************
//****************************************************************************
static void SelIPXSetInfo(HWND window, int childid) {
	TPBMP tpBmp;
	TPIPXGAME tpGame;
	HWND child = GetDlgItem(window, childid);
	HWND gamedesc = GetDlgItem(window, IDC_GAMEDESC);

	tpBmp = (TPBMP) GetWindowLong(child, GWL_USERDATA);
	if (! tpBmp) return;
	tpGame = (TPIPXGAME) tpBmp->userdata;
	if (! tpGame) return;

	if (! tpGame->gameid) {
		// handle the "new game" option
		UiSetBmpText((TPBMP)GetWindowLong(gamedesc, GWL_USERDATA), tpGame->gamedesc);
		UiDoomStaticReset(window, sgGameDescID, AF_SMALLGRAY);
		return;
	}

	// make sure there is game desc info
	if ((! tpGame->gamedesc) || (strlen(tpGame->gamedesc) < MIN_DESC_LEN)) {
		UiSetBmpText((TPBMP)GetWindowLong(gamedesc, GWL_USERDATA), "");
		UiDoomStaticReset(window, sgGameDescID, AF_SMALLGRAY);
		return;
	}

	// convert the gamedesc into something the user understands
	TCHAR szGameDescCopy[128];
	TCHAR szGameInfo[256];
	TGAMEDATA GameData;
	TUIHEROINFO HeroInfo;
	char * szCreator = "";
	char * szCreatorDesc = "";

	strcpy(szGameDescCopy, tpGame->gamedesc);
	if (
		(! Txt2GameData(szGameDescCopy, &GameData, &szCreator, &szCreatorDesc)) ||
		(! Txt2HeroInfo(szCreatorDesc, &HeroInfo))
	) {
		UiSetBmpText((TPBMP)GetWindowLong(gamedesc, GWL_USERDATA), NULL);
		UiDoomStaticReset(window, sgGameDescID, AF_SMALLGRAY);
		return;
	}

	TCHAR szDiff[32];
	TCHAR szClass[32];
	TCHAR szDescFmt[128];
	LoadString(global_hinstance, IDS_DIFF_NORMAL + GameData.bDiff, szDiff, 31);
	LoadString(global_hinstance, IDS_WARRIOR + HeroInfo.heroclass, szClass, 31);
	LoadString(global_hinstance, IDS_GAMEDESCFMT, szDescFmt, 127);
	wsprintf(szGameInfo, szDescFmt, szDiff, szCreator, HeroInfo.level, szClass);

	UiSetBmpText((TPBMP)GetWindowLong(gamedesc, GWL_USERDATA), szGameInfo);
	UiDoomStaticReset(window, sgGameDescID, AF_SMALLGRAY);
}


//****************************************************************************
//****************************************************************************
static int SelIPXGetFocusNum(void) {
	HWND btn = GetFocus();
	if (! btn) return 0;
	TPBMP tpBmp = (TPBMP) GetWindowLong(btn, GWL_USERDATA);
	if (! tpBmp) return 0;
	TPIPXGAME list = sgGameList;
	if (! list) return 0;
	TPIPXGAME node = (TPIPXGAME) tpBmp->userdata;
	if (! node) return 0;

	int nodenum = 0;

	while (list && (list != node)) {
		nodenum++;
		list = list->next;
	}
		   
	return nodenum;
}


//****************************************************************************
//****************************************************************************
static void SelIPXUpdateSbar(HWND window) {
	SbarDraw(window, IDC_SCROLLBAR, sgIPXListLen, SelIPXGetFocusNum());
}


//****************************************************************************
//****************************************************************************
static void SelIPXUpdateGames(HWND window) {
	// assume all games are invalid
	sgGamesChanged = FALSE;
	SelIPXInvalAll(sgGameList);

	// enum the games to update the list
	SNetEnumGames(0, 0, SelIPXEnum, NULL);		//### MM Diablo Patch #2  3/21/97   

	// delete games that are no longer valid
	sgGameList = SelIPXDeleteInval(sgGameList);

	// set game buttons to contain text
	if (sgGamesChanged) {
		SelIPXSetGames(window, sgGameList);
		#if SB_ALWAYS_ON != 1
			if (sgIPXListLen <= MAX_VISIBLE) {
				ShowWindow(GetDlgItem(window, IDC_SCROLLBAR), FALSE);
			}
			else {
				ShowWindow(GetDlgItem(window, IDC_SCROLLBAR), TRUE);
			}
		#endif
		SelIPXSetInfo(window, GetWindowLong(GetFocus(), GWL_ID));
		SelIPXUpdateSbar(window);
	}
}


//****************************************************************************
//****************************************************************************
static void SelIPXDestroy(HWND window) {
	SelIPXFreeList(sgGameList);
	sgGameList = NULL;

	SbarDestroy(window, IDC_SCROLLBAR);

	UiDoomCtrlsDestroy(window, sgListIDs);
	UiDoomCtrlsDestroy(window, sgBtnIDs);
	UiDoomCtrlsDestroy(window, sgGameDescID);
	UiDoomCtrlsDestroy(window, sgTextIDs);
	UiDoomCtrlsDestroy(window, sgTitleIDs);

	// free the background of the dlg
	UiFreeBmp((TPBMP) GetWindowLong(window, GWL_USERDATA));
	SetWindowLong(window, GWL_USERDATA, (LONG) 0);

	TitleLogoDestroy();
	FocusAnimateDestroy();
	SDrawClearSurface(SDRAW_SURFACE_FRONT);
}


//****************************************************************************
//****************************************************************************
static void SelIPXTab(HWND child) {
	HWND parent, newfocus;
	int index;
	int NewID[] = {
		IDC_GAMEBTN2,
		IDC_GAMEBTN3,
		IDC_GAMEBTN4,
		IDC_GAMEBTN5,
		IDC_GAMEBTN6,
		IDC_GAMEBTN1
	};

	parent = GetParent(child);
	newfocus = child;
	do {
		index = GetWindowLong(newfocus, GWL_ID) - IDC_GAMEBTN1;
		newfocus = GetDlgItem(parent, NewID[index]);
	} while (! IsWindowEnabled(newfocus));
	SetFocus(newfocus);
}


//****************************************************************************
//****************************************************************************
static void SelIPXShiftTab(HWND child) {
	HWND parent, newfocus;
	int index;
	int NewID[] = {
		IDC_GAMEBTN6,
		IDC_GAMEBTN1,
		IDC_GAMEBTN2,
		IDC_GAMEBTN3,
		IDC_GAMEBTN4,
		IDC_GAMEBTN5
	};

	parent = GetParent(child);
	newfocus = child;
	do {
		index = GetWindowLong(newfocus, GWL_ID) - IDC_GAMEBTN1;
		newfocus = GetDlgItem(parent, NewID[index]);
	} while (! IsWindowEnabled(newfocus));
	SetFocus(newfocus);
}


//****************************************************************************
//****************************************************************************
static int GetNumIPXNode(HWND btn) {
	if (! btn) return 0;
	TPBMP tpBmp = (TPBMP) GetWindowLong(btn, GWL_USERDATA);
	if (! tpBmp) return 0;
	TPIPXGAME list = sgGameList;
	if (! list) return 0;
	TPIPXGAME node = (TPIPXGAME) tpBmp->userdata;
	if (! node) return 0;

	int nodenum = 0;

	while (list && (list != node)) {
		nodenum++;
		list = list->next;
	}

	return nodenum;
}


//****************************************************************************
//****************************************************************************
static TPIPXGAME GetGame(int index) {
	TPIPXGAME list = sgGameList;
	
	while (list && index) {
		list = list->next;
		index--;
	}
	return list;
}


//****************************************************************************
//****************************************************************************
static void SelIPXPageDown(HWND focus) {
	HWND parent = GetParent(focus);
	if (! parent) return;

	// get info of 1st btn
	HWND btn1 = GetDlgItem(parent, IDC_GAMEBTN1);
	if (! btn1) return;

	// see if there is info below the bottom
	TPBMP tpBmp = (TPBMP) GetWindowLong(GetDlgItem(parent, IDC_GAMEBTN6), GWL_USERDATA);
	if (! tpBmp) return;
	TPIPXGAME tpGame = (TPIPXGAME) tpBmp->userdata;
	if ((! tpGame) || (! tpGame->next)) {
		SelIPXShiftTab(btn1);
		return;
	}

	// calc the index of the new top
	int newtop = GetNumIPXNode(btn1) + MAX_VISIBLE;
	if (newtop > (sgIPXListLen - MAX_VISIBLE))
		newtop = sgIPXListLen - MAX_VISIBLE;

	// get the pointer to that index
	tpGame = GetGame(newtop);
	if (! tpGame) return;

	// update everything on the screen
	UiSndPlayClick();
	SelIPXSetGames(parent, tpGame);
	SelIPXSetInfo(parent, GetWindowLong(focus, GWL_ID));
	SelIPXUpdateSbar(parent);
}


//****************************************************************************
//****************************************************************************
static void SelIPXPageUp(HWND focus) {
	HWND parent = GetParent(focus);
	if (! parent) return;
	HWND btn1 = GetDlgItem(parent, IDC_GAMEBTN1);
	if (! btn1) return;

	// see if there is info off the top
	TPBMP tpBmp = (TPBMP) GetWindowLong(btn1, GWL_USERDATA);
	if (! tpBmp) return;
	TPIPXGAME tpGame = (TPIPXGAME) tpBmp->userdata;
	if (! tpGame) return;
	if (tpGame == sgGameList) {
		SelIPXTab(GetDlgItem(parent, IDC_GAMEBTN6));
		return;
	}

	// calc the index of the new top
	int newtop = GetNumIPXNode(btn1) - MAX_VISIBLE;
	if (newtop < 0)
		newtop = 0;

	// get the pointer to that index
	tpGame = GetGame(newtop);
	if (! tpGame) return;

	// update everything on the screen
	UiSndPlayClick();
	SelIPXSetGames(parent, tpGame);
	SelIPXSetInfo(parent, GetWindowLong(focus, GWL_ID));
	SelIPXUpdateSbar(parent);
}


//****************************************************************************
//****************************************************************************
static void SelIPXScrollDown(HWND focus) {
	// see if there is info below the focus
	TPBMP tpBmp = (TPBMP) GetWindowLong(focus, GWL_USERDATA);
	if (! tpBmp) return;
	TPIPXGAME tpGame = (TPIPXGAME) tpBmp->userdata;
	if (! tpGame) return;
	if (! tpGame->next) return;

	if (GetWindowLong(focus, GWL_ID) < IDC_GAMEBTN6) {
		// handle it just like a tab
		SelIPXTab(focus);
		return;
	}

	// advance the list
	HWND child = GetDlgItem(GetParent(focus), IDC_GAMEBTN2);
	if (! child) return;
	tpBmp = (TPBMP) GetWindowLong(child, GWL_USERDATA);
	if (! tpBmp) return;
	tpGame = (TPIPXGAME) tpBmp->userdata;
	if (! tpGame) return;
	UiSndPlayClick();
	SelIPXSetGames(GetParent(focus), tpGame);
	SelIPXSetInfo(GetParent(focus), GetWindowLong(focus, GWL_ID));
	SelIPXUpdateSbar(GetParent(focus));
}


//****************************************************************************
//****************************************************************************
static void SelIPXScrollUp(HWND focus) {
	if (GetWindowLong(focus, GWL_ID) > IDC_GAMEBTN1) {
		// handle it just like a shift-tab
		SelIPXShiftTab(focus);
		return;
	}

	// see if there is info off the top
	TPBMP tpBmp = (TPBMP) GetWindowLong(focus, GWL_USERDATA);
	if (! tpBmp) return;
	TPIPXGAME tpGame = (TPIPXGAME) tpBmp->userdata;
	if (! tpGame) return;
	TPIPXGAME tpGameList = sgGameList;
	if (tpGame == tpGameList) return;

	// retreat the list
	while (tpGameList && (tpGameList->next != tpGame)) {
		tpGameList = tpGameList->next;
	}
	UiSndPlayClick();
	SelIPXSetGames(GetParent(focus), tpGameList);
	SelIPXSetInfo(GetParent(focus), GetWindowLong(focus, GWL_ID));
	SelIPXUpdateSbar(GetParent(focus));
}


//****************************************************************************
//****************************************************************************
static void SelIPXShowList(HWND window, BOOL showflag) {
	HWND child;
	int childid;

	for (childid = IDC_GAMEBTN1; childid <= IDC_GAMEBTN6; childid++) {
		child = GetDlgItem(window, childid);
		if (! child) continue;
		ShowWindow(child, showflag);
	}

	child = GetDlgItem(window, IDC_SCROLLBAR);
	if (sgIPXListLen <= MAX_VISIBLE) {
		ShowWindow(child, FALSE);
	}
	else {
		ShowWindow(child, showflag);
	}
}


//****************************************************************************
//****************************************************************************
static LRESULT CALLBACK IPXListEntryWndProc (HWND   window,
											UINT   message,
											WPARAM wparam,
											LPARAM lparam) {
	LRESULT result;
	WNDPROC oldproc = (WNDPROC) GetProp(window, OLDPROCPROP);

	switch (message) {
		case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS;

		case WM_KEYDOWN:
			switch (wparam) {
				case VK_SPACE:
				case VK_RETURN:
					SendMessage(GetParent(window), WM_COMMAND, IDOK, (LPARAM)NULL);
				break;

				case VK_ESCAPE:
					SendMessage(GetParent(window), WM_COMMAND, IDCANCEL, (LPARAM)NULL);
				break;

				case VK_TAB:
					if (GetKeyState(VK_SHIFT) < 0) {
						SelIPXShiftTab(window);
					}
					else {
						SelIPXTab(window);
					}
				break;

				case VK_UP:
				case VK_LEFT:
					SelIPXScrollUp(window);
				break;

				case VK_DOWN:
				case VK_RIGHT:
					SelIPXScrollDown(window);
				break;

				case VK_PRIOR:
					SelIPXPageUp(window);
				break;

				case VK_NEXT:
					SelIPXPageDown(window);
				break;

				case VK_DELETE:
					SendMessage(GetParent(window),message,wparam,lparam);
				break;
			}
		return 0;

		case WM_DESTROY:
			RemoveProp(window, OLDPROCPROP);
			if (oldproc) {
				SetWindowLong(window, GWL_WNDPROC, (LONG) oldproc);
			}
		break;

		case WM_PAINT:
			UiPaintBtn(window);
		return 0;
	}

	if (oldproc)
		result = CallWindowProc(oldproc, window, message, wparam, lparam);
	else
		result = DefWindowProc(window, message, wparam, lparam);
	return result;
}


//****************************************************************************
//****************************************************************************
static void SelIPXSubclassEntries(HWND window) {
	HWND child;
	int *ListID;
	WNDPROC oldproc;

	for (ListID = sgListIDs; *ListID; ListID++) {
		child = GetDlgItem(window, *ListID);
		if (! child) continue;

		oldproc = (WNDPROC) GetWindowLong(child, GWL_WNDPROC);
		SetProp(child, OLDPROCPROP,(HANDLE) oldproc);
		SetWindowLong(child, GWL_WNDPROC, (LONG) IPXListEntryWndProc);
	}
}


//****************************************************************************
//****************************************************************************
static void SelIPXInit(HWND window) {
	TPBMP tpBmp;

	SelIPXSubclassEntries(window);

	// set up the animating focus indicator
	FocusAnimateInit("ui_art\\focus16.pcx");
	SDlgSetTimer(window, FOCUS_TIMER_ID, FOCUS_TIMER_DELAY, NULL);

	// set up the animating diablo logo
	TitleLogoInit(window);
	TitleFramesInit(window,TEXT("ui_art\\hf_logo2.pcx"));
	TitleLogoAnimate(window);
	SDlgSetTimer(window, LOGO_TIMER_ID, LOGO_TIMER_DELAY, NULL);

	// load texture maps for this dialog
	tpBmp = UiAllocBmp();
	if (tpBmp) {
		SetWindowLong(window, GWL_USERDATA, (LONG) tpBmp);
		LoadArtFile(
			window,
			NULL,
			TEXT(""),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,
			TEXT("ui_art\\selgame.pcx"),
			&tpBmp->data,
			&tpBmp->datasize,
			FALSE
		);
		UiFadeInit(window);
	}

	// if the provider is not IPX use a generic title
	if (UiGetProviderID() != PROVIDERID) {
		TCHAR generictitle[64];
		HWND child = GetDlgItem(window, IDC_DLGTITLE);
		LoadString(global_hinstance, IDS_JOINGAMES, generictitle, 63);
		SetWindowText(child, generictitle);
	}

	// set up a doom-like interface
	UiDoomStaticInit(window, sgTitleIDs, AF_BIGGRAY);
	UiDoomStaticInit(window, sgTextIDs, AF_MEDGRAY);
	UiDoomStaticInit(window, sgGameDescID, AF_SMALLGRAY);
	UiDoomButtonsInit(window, sgBtnIDs, AF_BIG, FALSE);
	UiDoomButtonsInit(window, sgListIDs, AF_MED);

	// set a "create game" option
	sgIPXListLen = 0;
	sgGameList = SelIPXAllocNode();
	if (sgGameList) {
		sgIPXListLen++;
		sgGameList->gameid = 0;
		sgGameList->gamedesc[0] = 0;
		sgGameList->next = NULL;
		LoadString(global_hinstance, IDS_CREATEGAME, sgGameList->gamename, GAMENAME_LEN-1);
		LoadString(global_hinstance, IDS_CREATEDESC, sgGameList->gamedesc, DESCRIPTION_LEN-1);
	}

	// get list of games
	SNetEnumGames(0, 0, SelIPXEnum, NULL);		//### MM Diablo Patch #2  3/21/97   
	SelIPXSetGames(window, sgGameList);

	// set a timer so the game list is periodically updated
	SDlgSetTimer(window, ENUM_TIMER_ID, ENUM_TIMER_DELAY, NULL);

	// init the scroll bar
	SbarInit(window, IDC_SCROLLBAR);
#if SB_ALWAYS_ON != 1
	if (sgIPXListLen <= MAX_VISIBLE) {
		ShowWindow(GetDlgItem(window, IDC_SCROLLBAR), FALSE);
	}
#endif
}


//****************************************************************************
//****************************************************************************
static void SelIPXAbort(HWND window, int ReturnVal) {
	UiFadeAbort(window);
	SDlgKillTimer(window, ENUM_TIMER_ID);
	SDlgKillTimer(window, LOGO_TIMER_ID);
	SDlgKillTimer(window, FOCUS_TIMER_ID);
	UiVidFadeOut(DEFAULT_STEPS*2);
	SDlgEndDialog(window, ReturnVal);
}


//****************************************************************************
//****************************************************************************
static void SelIPXDetermineGameName(LPSTR gamename) {
	if (! SelIPXFindNodeName(sgGameList, sgIPXPlayerData->playername)) {
		strcpy(gamename, sgIPXPlayerData->playername);
		return;
	}

	int gamenum = 2;
	do {
		wsprintf(gamename, "%s %d", sgIPXPlayerData->playername, gamenum++);
	} while (SelIPXFindNodeName(sgGameList, gamename));
}


//****************************************************************************
//****************************************************************************
static void SelIPXCreate(HWND window) {
	SNETUIDATA interfacedata;
	SNETCREATEDATA createdata;

	// make sure the list is current
	SelIPXUpdateGames(window);

	// create new interface data
	CopyMemory(&interfacedata,sgIPXInterfaceData,sizeof(SNETUIDATA));
	interfacedata.size         = sizeof(SNETUIDATA);
	interfacedata.parentwindow = window;

	ZeroMemory(&createdata,sizeof(SNETCREATEDATA));
	createdata.size        = sizeof(SNETCREATEDATA);
	createdata.providerid  = PROVIDERID;
	createdata.maxplayers  = sgIPXProgramData->maxplayers;
	createdata.createflags = 0;

	HWND focus = GetFocus();
	SelIPXShowList(window, FALSE);

	char gamename[SNETSPI_MAXSTRINGLENGTH];
	SelIPXDetermineGameName(gamename);

	if (UiDoomCreateGame(
			&createdata,
			sgIPXProgramData,
			sgIPXPlayerData,
			&interfacedata,
			sgIPXVersionData,
			sgpIPXPlayerID,
			FALSE,
			gamename
		)
	) {
		SelIPXAbort(window, IDOK);
	}
	else {
		SelIPXShowList(window, TRUE);
		SetFocus(focus);
	}
}


//****************************************************************************
//****************************************************************************
static void SelIPXJoin(HWND window, TPIPXGAME tpGame) {
	char err[256];
	if (!
		UiAuthCallback(
			SNET_AUTHTYPE_GAME, 
			sgIPXPlayerData->playername,
			sgIPXPlayerData->playerdescription,
			0,			
			tpGame->gamedesc,
			err,
			sizeof(err)
		)
	) {
		SelOkDialog(window, err, NULL, FALSE);
		return;
	}

	if (SNetJoinGame(
			tpGame->gameid,
			tpGame->gamename,
			NULL,
			sgIPXPlayerData->playername,
			sgIPXPlayerData->playerdescription,
			sgpIPXPlayerID
		)
	) {
		SelIPXAbort(window, IDOK);
	}
	else {
		// unable to join the specified game
		TCHAR format[64], errstr[128];
		LoadString(global_hinstance, IDS_JOIN_ERR, format, 63);
		wsprintf(errstr, format, tpGame->gamename);
		SelOkDialog(window, errstr, NULL, FALSE);
	}
}


//****************************************************************************
//****************************************************************************
static void SelIPXAttemptJoin(HWND window) {
	TPBMP tpBmp;
	TPIPXGAME tpGame;
	HWND child = GetFocus();

	if (window != GetParent(child)) return;
	tpBmp = (TPBMP) GetWindowLong(child, GWL_USERDATA);
	if (! tpBmp) return;
	tpGame = (TPIPXGAME) tpBmp->userdata;

	UiSndPlayEnter();
	if (tpGame->gameid)
		SelIPXJoin(window, tpGame);
	else
		SelIPXCreate(window);
}


//****************************************************************************
//****************************************************************************
static void SelIPXInterpretClick(HWND window, int x, int y) {
	if (UiIsPtInWindow(window, GetDlgItem(window, IDC_FAKEOK), x, y)) {
		SelIPXAttemptJoin(window);
	}
	else if (UiIsPtInWindow(window, GetDlgItem(window, IDC_FAKECANCEL), x, y)) {
		SelIPXAbort(window, IDCANCEL);
	}
	else if (UiIsPtInWindow(window, GetDlgItem(window, IDC_SCROLLBAR), x, y)) {
		int SbarClick = SbarInterpretClick(GetDlgItem(window, IDC_SCROLLBAR), x, y);
		switch (SbarClick) {
			case DSB_UP:
				SelIPXScrollUp(GetFocus());
			break;
			case DSB_PAGEUP:
				SelIPXPageUp(GetFocus());
			break;
			case DSB_DOWN:
				SelIPXScrollDown(GetFocus());
			break;
			case DSB_PAGEDOWN:
				SelIPXPageDown(GetFocus());
			break;
			default:
			break;
		}
	}
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK SelIPXDialogProc(HWND window,
										UINT message,
										WPARAM wparam,
										LPARAM lparam) {
	switch (message) {

		case WM_COMMAND:
			if (HIWORD(wparam) == BN_KILLFOCUS) {
				FocusLost(window, (HWND) lparam);
			}
			else if (HIWORD(wparam) == BN_SETFOCUS) {
				FocusSnd((HWND) lparam);
				FocusAnimate(window, (HWND) lparam);
				SelIPXSetInfo(window, LOWORD(wparam));
				SelIPXUpdateSbar(window);
			}
			else if (HIWORD(wparam) == BN_DOUBLECLICKED) {
				SelIPXAttemptJoin(window);
			}
			else if (LOWORD(wparam) == IDOK) {
				SelIPXAttemptJoin(window);
			}
			else if (LOWORD(wparam) == IDCANCEL) {
				SelIPXAbort(window, IDCANCEL);
			}
		break;

		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
			SelIPXInterpretClick(window, LOWORD(lparam), HIWORD(lparam));
		break;

		case WM_LBUTTONUP:
			if (SbarUpClick(GetDlgItem(window, IDC_SCROLLBAR))) {
				SelIPXUpdateSbar(window);
			}
		break;

		case WM_DESTROY:
			SelIPXDestroy(window);
		break;

		case WM_INITDIALOG:
			SelIPXInit(window);
			PostMessage(window, WM_USER+1000, 0, 0);
		return 0;

		case WM_USER+1000:
			if (! UiIsFading()) {
				UiFadeStart(window);
			}
		return 0;

		case WM_TIMER:
			switch (wparam) {
				case FOCUS_TIMER_ID:
					FocusAnimate(window, GetFocus());
				break;
				case LOGO_TIMER_ID:
					TitleLogoAnimate(window);
				break;
				case ENUM_TIMER_ID:
					SelIPXUpdateGames(window);
				break;
			}
		return 0;

		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			SendMessage(SDrawGetFrameWindow(), message, wparam, lparam);
		break;
	}
	return SDlgDefDialogProc(window,message,wparam,lparam);
}


//****************************************************************************
//****************************************************************************
BOOL UiSelectIPXGame (DWORD               flags,
					  SNETPROGRAMDATAPTR  programdata,
					  SNETPLAYERDATAPTR   playerdata,
					  SNETUIDATAPTR       interfacedata,
					  SNETVERSIONDATAPTR  versiondata,
					  DWORD              *playerid) {
	int result = 0;

	// save parameters for use throughout this file
	sgIPXProgramData	= programdata;
	sgIPXPlayerData		= playerdata;
	sgIPXInterfaceData	= interfacedata;
	sgIPXVersionData	= versiondata;
	sgpIPXPlayerID		= playerid;

	// make sure fonts are loaded
	ArtFontLoad();

	// DISPLAY THE DIALOG BOX
	result = SDlgDialogBox(global_hinstance,
						   TEXT("SELIPXGAME_DIALOG"),
						   interfacedata->parentwindow,
						   SelIPXDialogProc);

	ArtFontDestroy();

	if (result == IDOK)
		return TRUE;

	return FALSE;
}

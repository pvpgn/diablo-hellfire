//****************************************************************************
//	DirLink.cpp
//	Diablo UI create/join direct link multiplayer game
//	
//	By Frank Pearce
//	created 11.8.96
//****************************************************************************


#include "pch.h"
#include "artfont.h"
#include "uisnd.h"


#define SERIAL_JOIN_AUTHORIZE	0

//****************************************************************************
//****************************************************************************
	extern void TitleLogoDestroy(void);
	extern void TitleLogoInit(HWND window);
	extern void TitleFramesInit(HWND window, LPCTSTR filename);
	extern void TitleLogoAnimate(HWND window);

	extern int SelOkDialog(HWND parent, LPCTSTR text, LPCTSTR title, BOOL loadfocus);

	extern BOOL UiDoomCreateGame (SNETCREATEDATAPTR   createdata,
								  SNETPROGRAMDATAPTR  programdata,
								  SNETPLAYERDATAPTR   playerdata,
								  SNETUIDATAPTR       interfacedata,
								  SNETVERSIONDATAPTR  versiondata,
								  DWORD              *playerid,
								  BOOL				  loadfocus,
								  LPCSTR			  gamename);


//****************************************************************************
//****************************************************************************
	#define PROVIDERID			'SCBL'

	#define MILLISEC_PER_SEC	1000

	#define FOCUS_TIMER_ID		1
	#define FOCUS_FPS			16		// frames per second
	#define FOCUS_TIMER_DELAY	55		//(MILLISEC_PER_SEC / FOCUS_FPS)

	#define LOGO_TIMER_ID		2
	#define LOGO_FPS			17
	#define LOGO_TIMER_DELAY	55

	#define ENUM_TIMER_ID		3		// how often we re-enum available games
	#define ENUM_TIMER_DELAY	2000


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
		IDC_LINKDESC,
		0
	};
	static int sgBtnIDs[] = {
		IDC_FAKEOK,
		IDC_FAKECANCEL,
		0
	};
	static int sgListIDs[] = {
		IDC_DIRLINKBTN1,
		IDC_DIRLINKBTN2,
		0
	};


//****************************************************************************
//****************************************************************************
	#define GAMENAME_LEN		SNETSPI_MAXSTRINGLENGTH
	#define DESCRIPTION_LEN		SNETSPI_MAXSTRINGLENGTH

	typedef struct _serialgame TGAME;
	typedef TGAME *TPGAME;
	struct _serialgame {
		DWORD gameid;
		TCHAR gamename[GAMENAME_LEN];
		TCHAR gamedesc[DESCRIPTION_LEN];
	};
	static TGAME sgGame;

	static SNETPROGRAMDATAPTR  sgDLProgramData;
	static SNETPLAYERDATAPTR   sgDLPlayerData;
	static SNETUIDATAPTR       sgDLInterfaceData;
	static SNETVERSIONDATAPTR  sgDLVersionData;
	static DWORD			  *sgpDLPlayerID;

	static void DirLinkUpdateGame(HWND window);
	static void DirLinkAutoJoin(HWND window);


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK DirLinkEnum(DWORD  gameid,
								 LPCSTR gamename,
								 LPCSTR gamedesc) {
	sgGame.gameid = gameid;
	strcpy(sgGame.gamename, gamename);
	strcpy(sgGame.gamedesc, gamedesc);
	return TRUE;
}


//****************************************************************************
//****************************************************************************
static void DirLinkSetInfo(HWND window, int childid) {
	TCHAR desc[256];
	HWND child = GetDlgItem(window, IDC_LINKDESC);

	if (childid == IDC_DIRLINKBTN1)
		LoadString(global_hinstance, IDS_CREATEDESC, desc, 255);
	else
		LoadString(global_hinstance, IDS_RETRYCONNDESC, desc, 255);
	UiSetBmpText((TPBMP)GetWindowLong(child, GWL_USERDATA), desc);
	UiDoomStaticReset(window, sgGameDescID, AF_SMALLGRAY);
}


//****************************************************************************
//****************************************************************************
static void DirLinkDestroy(HWND window) {
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
}


//****************************************************************************
//****************************************************************************
static void DirLinkInit(HWND window) {
	TPBMP tpBmp;

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

	UiOnPaintBtns(window, sgListIDs);

	// set up a doom-like interface
	UiDoomStaticInit(window, sgTitleIDs, AF_BIGGRAY);
	UiDoomStaticInit(window, sgTextIDs, AF_MEDGRAY);
	UiDoomStaticInit(window, sgGameDescID, AF_SMALLGRAY);
	UiDoomButtonsInit(window, sgBtnIDs, AF_BIG, FALSE);
	UiDoomButtonsInit(window, sgListIDs, AF_MED);

	// see if there's a game
	DirLinkUpdateGame(window);
	DirLinkAutoJoin(window);

	// set a timer so the game list is periodically updated
	SDlgSetTimer(window, ENUM_TIMER_ID, ENUM_TIMER_DELAY, NULL);
}


//****************************************************************************
//****************************************************************************
static void DirLinkAbort(HWND window, int ReturnVal) {
	UiSndPlayEnter();
	UiFadeAbort(window);
	SDlgKillTimer(window, ENUM_TIMER_ID);
	SDlgKillTimer(window, LOGO_TIMER_ID);
	SDlgKillTimer(window, FOCUS_TIMER_ID);
	UiVidFadeOut(DEFAULT_STEPS*2);
	SDlgEndDialog(window, ReturnVal);
}


//****************************************************************************
//****************************************************************************
static void DirLinkCreate(HWND window) {
	SNETUIDATA interfacedata;
	SNETCREATEDATA createdata;

	// create new interface data
	CopyMemory(&interfacedata,sgDLInterfaceData,sizeof(SNETUIDATA));
	interfacedata.size         = sizeof(SNETUIDATA);
	interfacedata.parentwindow = window;

	ZeroMemory(&createdata,sizeof(SNETCREATEDATA));
	createdata.size        = sizeof(SNETCREATEDATA);
	createdata.providerid  = PROVIDERID;
	createdata.maxplayers  = sgDLProgramData->maxplayers;
	createdata.createflags = 0;

	if (UiDoomCreateGame(
			&createdata,
			sgDLProgramData,
			sgDLPlayerData,
			&interfacedata,
			sgDLVersionData,
			sgpDLPlayerID,
			FALSE,
			sgDLPlayerData->playername
		)
	) {
		DirLinkAbort(window, IDOK);
	}
}


//****************************************************************************
//****************************************************************************
static void DirLinkAutoJoin(HWND window) {
	if (! sgGame.gameid) return;

#if SERIAL_JOIN_AUTHORIZE
	if (!
		UiAuthCallback(
			SNET_AUTHTYPE_GAME, 
			sgDLPlayerData->playername,
			sgDLPlayerData->playerdescription,
			0,			
			sgGame.gamedesc,
			NULL,
			0
		)
	) {
		return;
	}
#endif

	if (SNetJoinGame(
			sgGame.gameid,
			sgGame.gamename,
			NULL,
			sgDLPlayerData->playername,
			sgDLPlayerData->playerdescription,
			sgpDLPlayerID
		)
	) {
		DirLinkAbort(window, IDOK);
	}
}


//****************************************************************************
//****************************************************************************
static void DirLinkJoin(HWND window) {
	char err[256];
	if (!
		UiAuthCallback(
			SNET_AUTHTYPE_GAME, 
			sgDLPlayerData->playername,
			sgDLPlayerData->playerdescription,
			0,			
			sgGame.gamedesc,
			err,
			sizeof(err)
		)
	) {
		SelOkDialog(window, err, NULL, FALSE);
		return;
	}

	if (SNetJoinGame(
			sgGame.gameid,
			sgGame.gamename,
			NULL,
			sgDLPlayerData->playername,
			sgDLPlayerData->playerdescription,
			sgpDLPlayerID
		)
	) {
		DirLinkAbort(window, IDOK);
	}
	else {
		// unable to join the specified game
		TCHAR format[64], errstr[128];
		LoadString(global_hinstance, IDS_JOIN_ERR, format, 63);
		wsprintf(errstr, format, sgGame.gamename);
		SelOkDialog(window, errstr, NULL, FALSE);
	}
}


//****************************************************************************
//****************************************************************************
static void DirLinkAttemptJoin(HWND window) {
	HWND child = GetFocus();

	if (window != GetParent(child)) return;

	if (IDC_DIRLINKBTN1 == GetWindowLong(child, GWL_ID)) {
		DirLinkCreate(window);
	}
	else if (sgGame.gameid) {
		DirLinkJoin(window);
	}
	else {
		// a game has not been detected
		TCHAR errstr[128];
		LoadString(global_hinstance, IDS_NOGAME_ERR, errstr, 127);
		SelOkDialog(window, errstr, NULL, FALSE);
	}
}


//****************************************************************************
//****************************************************************************
static void DirLinkUpdateGame(HWND window) {
	// clear old info
	sgGame.gameid = 0;
	sgGame.gamename[0] = 0;
	sgGame.gamedesc[0] = 0;

	// enum will fill in info if a game exists
	if (! SNetEnumGames(0, 0, DirLinkEnum, NULL)) {			//### MM Diablo Patch #2  3/21/97   
		if (GetLastError() == SNET_ERROR_VERSION_MISMATCH) {
			// display a version mismatch error
			TCHAR err[128];
			LoadString(global_hinstance, IDS_VERSION_ERR, err, 127);
			SelOkDialog(window, err, NULL, FALSE);
			DirLinkAbort(window, IDCANCEL);
		}
	}
}


//****************************************************************************
//****************************************************************************
static void DirLinkInterpretClick(HWND window, int x, int y) {
	if (UiIsPtInWindow(window, GetDlgItem(window, IDC_FAKEOK), x, y)) {
		DirLinkAttemptJoin(window);
	}
	else if (UiIsPtInWindow(window, GetDlgItem(window, IDC_FAKECANCEL), x, y)) {
		DirLinkAbort(window, IDCANCEL);
	}
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK DirLinkDialogProc(HWND window,
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
				DirLinkSetInfo(window, LOWORD(wparam));
			}
			else if (HIWORD(wparam) == BN_DOUBLECLICKED) {
				DirLinkAttemptJoin(window);
			}
			else if (LOWORD(wparam) == IDOK) {
				DirLinkAttemptJoin(window);
			}
			else if (LOWORD(wparam) == IDCANCEL) {
				DirLinkAbort(window, IDCANCEL);
			}
		break;

		case WM_LBUTTONDOWN:
			DirLinkInterpretClick(window, LOWORD(lparam), HIWORD(lparam));
		break;

		case WM_DESTROY:
			DirLinkDestroy(window);
		break;

		case WM_INITDIALOG:
			// Post this WM_USER message first so that fade gets started even if 
			// DirLinkInit() needs to go into a modal dialog to display an error.
			PostMessage(window, WM_USER+1000, 0, 0);
			DirLinkInit(window);
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
					DirLinkUpdateGame(window);
					DirLinkAutoJoin(window);
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
BOOL UiDirectLinkGame (DWORD               flags,
					   SNETPROGRAMDATAPTR  programdata,
					   SNETPLAYERDATAPTR   playerdata,
					   SNETUIDATAPTR       interfacedata,
					   SNETVERSIONDATAPTR  versiondata,
					   DWORD              *playerid) {
	int result = 0;

	// save parameters for use throughout this file
	sgDLProgramData	= programdata;
	sgDLPlayerData	= playerdata;
	sgDLInterfaceData	= interfacedata;
	sgDLVersionData	= versiondata;
	sgpDLPlayerID		= playerid;

	// make sure fonts are loaded
	ArtFontLoad();

	// DISPLAY THE DIALOG BOX
	result = SDlgDialogBox(global_hinstance,
						   TEXT("DIRLINK_DIALOG"),
						   interfacedata->parentwindow,
						   DirLinkDialogProc);

	ArtFontDestroy();

	if (result == IDOK)
		return TRUE;
	return FALSE;
}

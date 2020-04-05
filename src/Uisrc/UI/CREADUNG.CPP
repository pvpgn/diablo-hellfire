//****************************************************************************
//	CreaDung.cpp
//	Diablo UI create multi player dungeon "Doom style" popup dialog
//	
//	By Frank Pearce
//	created 11.7.96
//****************************************************************************


#include "pch.h"
#include "artfont.h"
#include "uisnd.h"


//****************************************************************************
//****************************************************************************
	extern int SelHeroGetMode(void);
	void SelHeroSetDiff(int difficulty);
	extern int SelOkDialog(HWND parent, LPCTSTR text, LPCTSTR title, BOOL loadfocus);
	extern BOOL Txt2HeroInfo(char *szDesc, TUIHEROINFO *pHeroInfo);


//****************************************************************************
//****************************************************************************
	#define MILLISEC_PER_SEC	1000

	#define FOCUS_TIMER_ID		1
	#define FOCUS_FPS			16		// frames per second
	#define FOCUS_TIMER_DELAY	55		//(MILLISEC_PER_SEC / FOCUS_FPS)


//****************************************************************************
//****************************************************************************
	static int sgTextIDs[] = {
		IDC_DLGTITLE,
		IDC_DLGTITLE2,
		0
	};
	static int sgDiffID[] = {
		IDC_DLGTITLE3,
		0
	};
	static int sgDiffDescID[] = {
		IDC_DIFFDESC,
		0
	};
	static int sgBtnIDs[] = {
		IDC_FAKEOK,
		IDC_FAKECANCEL,
		0
	};
	static int sgListIDs[] = {
		IDC_DIFFBTN1,
		IDC_DIFFBTN2,
		IDC_DIFFBTN3,
		0
	};

	static SNETCREATEDATAPTR	sgCreateData;
	static SNETPROGRAMDATAPTR	sgCre8ProgramData;
	static SNETPLAYERDATAPTR    sgCre8PlayerData;
	static DWORD			   *sgpCre8PlayerID;
	static int					sgPlayersMode;		// single or multi player
	static BOOL					sgDiffLoadFocus;
	static LPCSTR				sgGameName;
	static int					sgLastError;


//****************************************************************************
//****************************************************************************
void SelDiffLoadFocus(BOOL loadfocus) {
	sgDiffLoadFocus = loadfocus;
}


//****************************************************************************
//****************************************************************************
static void SelDiffSetInfo(HWND window, int childid) {
	TCHAR desc[256];
	HWND child;

	LoadString(
		global_hinstance,
		IDS_NORMAL + (childid - IDC_DIFFBTN1),
		desc,
		255
	);
	child = GetDlgItem(window, IDC_DLGTITLE3);
	UiSetBmpText((TPBMP)GetWindowLong(child, GWL_USERDATA), desc);
	UiDoomStaticReset(window, sgDiffID, AF_BIGGRAY);

	LoadString(
		global_hinstance,
		IDS_NORMAL_DESC_XLONG + (childid - IDC_DIFFBTN1),
		desc,
		255
	);
	child = GetDlgItem(window, IDC_DIFFDESC);
	UiSetBmpText((TPBMP)GetWindowLong(child, GWL_USERDATA), desc);
	UiDoomStaticReset(window, sgDiffDescID, AF_SMALLGRAY);
}


//****************************************************************************
//****************************************************************************
static void SelDiffDestroy(HWND window) {
	UiDoomCtrlsDestroy(window, sgListIDs);
	UiDoomCtrlsDestroy(window, sgBtnIDs);
	UiDoomCtrlsDestroy(window, sgDiffDescID);
	UiDoomCtrlsDestroy(window, sgDiffID);
	UiDoomCtrlsDestroy(window, sgTextIDs);

	// free the background of the dlg
	UiFreeBmp((TPBMP) GetWindowLong(window, GWL_USERDATA));
	SetWindowLong(window, GWL_USERDATA, (LONG) 0);

	if (sgDiffLoadFocus) {
		FocusAnimateDestroy();
	}
}


//****************************************************************************
//****************************************************************************
static void SelDiffInit(HWND window) {
	TPBMP tpBmp;

	if (sgDiffLoadFocus) {
		FocusAnimateInit("ui_art\\focus16.pcx");
	}
	else {
		FocusReInit();
	}
	SDlgSetTimer(window, FOCUS_TIMER_ID, FOCUS_TIMER_DELAY, NULL);

	// load texture maps for this dialog
	tpBmp = UiAllocBmp();
	if (tpBmp) {
		SetWindowLong(window, GWL_USERDATA, (LONG) tpBmp);
		LoadArtFile(
			window,
			NULL,
			TEXT("popup"),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,
			TEXT("ui_art\\seldiff.pcx"),
			&tpBmp->data,
			&tpBmp->datasize,
			FALSE
		);
	}

	UiOnPaintBtns(window, sgListIDs);

	// set up a doom-like interface
	UiDoomStaticInit(window, sgTextIDs, AF_BIGGRAY);
	UiDoomStaticInit(window, sgDiffID, AF_BIGGRAY);
	UiDoomStaticInit(window, sgDiffDescID, AF_SMALLGRAY);
	UiDoomButtonsInit(window, sgBtnIDs, AF_BIG, FALSE);
	UiDoomButtonsInit(window, sgListIDs, AF_MED);
}


//****************************************************************************
//****************************************************************************
static void SelDiffAbort(HWND window, int ReturnVal) {
	UiSndPlayEnter();
	SDlgKillTimer(window, FOCUS_TIMER_ID);
	SDlgEndDialog(window, ReturnVal);
}


//****************************************************************************
//****************************************************************************
static void SelDiffAttemptCreate(HWND window) {
	char szGameDesc[256];
	TGAMEDATA GameData;
	TGAMEDATA *pInitData;
	HWND child = GetFocus();

	if (window != GetParent(child)) return;

	// create a string describing the game
	GameData.bDiff = GetWindowLong(child, GWL_ID) - IDC_DIFFBTN1;
	GameData2Txt(
		&GameData,
		sgCre8PlayerData->playername,
		sgCre8PlayerData->playerdescription,
		szGameDesc,
		sizeof(szGameDesc)
	);

	// get the diff setting and stuff it into initdata
	if (sgCre8ProgramData->initdatabytes >= sizeof(TGAMEDATA)) {
		pInitData = (TGAMEDATA *)sgCre8ProgramData->initdata;
		pInitData->bDiff = GetWindowLong(child, GWL_ID) - IDC_DIFFBTN1;
	}

	// try to create the game
	if (SNetCreateGame(
			sgGameName,							// game name
			NULL,								// game password
			szGameDesc,
			0,									//### MM Diablo Patch #2  3/21/97   
			sgCre8ProgramData->initdata,
			sgCre8ProgramData->initdatabytes,								
			sgCreateData->maxplayers,
			sgCre8PlayerData->playername,		// player name
			NULL,
			sgpCre8PlayerID
		)
	) {
		SelDiffAbort(window, IDOK);
	}
	else {
		// display a creation error
		TCHAR errfmt[128], errstr[192];
		sgLastError = GetLastError();
		switch (sgLastError) {
			case SNET_ERROR_ALREADY_EXISTS:
				LoadString(global_hinstance, IDS_DUPGAMENAME_FMT, errfmt, 127);
				wsprintf(errstr, errfmt, sgGameName);
			break;
			default:
				LoadString(global_hinstance, IDS_CREATEGAME_ERR, errstr, 191);
			break;
		}
		SelOkDialog(window, errstr, NULL, FALSE);
	}
}


//****************************************************************************
//****************************************************************************
static void SelDiffMultiCreate(HWND window) {
	char szGameDesc[SNETSPI_MAXSTRINGLENGTH];
	char err[256];
	TGAMEDATA GameData;

	// Create Game description for Storm (and for display of game later)
	GameData.bDiff = GetWindowLong(GetFocus(), GWL_ID) - IDC_DIFFBTN1;
	GameData2Txt(
		&GameData, 
		sgCre8PlayerData->playername, 
		sgCre8PlayerData->playerdescription, 
		szGameDesc, 
		sizeof(szGameDesc)
	);

	if (!
		UiAuthCallback(
			SNET_AUTHTYPE_GAME, 
			sgCre8PlayerData->playername, 
			sgCre8PlayerData->playerdescription,
			0,			
			szGameDesc,
			err,
			sizeof(err)
		)
	) {
		SelOkDialog(window, err, NULL, FALSE);
	}
	else {
		SelDiffAttemptCreate(window);
	}
}


//****************************************************************************
//****************************************************************************
static void SelDiffModeCreate(HWND window) {
	if (sgPlayersMode == MULTIPLAYER) {
		SelDiffMultiCreate(window);
	}
	else {
		// let the select hero screen handle the create restrictions
		SelHeroSetDiff(GetWindowLong(GetFocus(), GWL_ID) - IDC_DIFFBTN1);
		SelDiffAbort(window, IDOK);
	}
}


//****************************************************************************
//****************************************************************************
static void SelDiffInterpretClick(HWND window, int x, int y) {
	if (UiIsPtInWindow(window, GetDlgItem(window, IDC_FAKEOK), x, y)) {
		SelDiffModeCreate(window);
	}
	else if (UiIsPtInWindow(window, GetDlgItem(window, IDC_FAKECANCEL), x, y)) {
		SelDiffAbort(window, IDCANCEL);
	}
}


//****************************************************************************
//****************************************************************************
BOOL CALLBACK SelDiffDialogProc(HWND window,
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
				SelDiffSetInfo(window, LOWORD(wparam));
			}
			else if (HIWORD(wparam) == BN_DOUBLECLICKED) {
				SelDiffModeCreate(window);
			}
			else if (LOWORD(wparam) == IDOK) {
				SelDiffModeCreate(window);
			}
			else if (LOWORD(wparam) == IDCANCEL) {
				SelDiffAbort(window, IDCANCEL);
			}
		break;

		case WM_LBUTTONDOWN:
			SelDiffInterpretClick(window, LOWORD(lparam), HIWORD(lparam));
		break;

		case WM_DESTROY:
			SelDiffDestroy(window);
		break;

		case WM_INITDIALOG:
			sgPlayersMode = lparam;
			SelDiffInit(window);
		return 0;

		case WM_TIMER:
			if (wparam == FOCUS_TIMER_ID) {
				FocusAnimate(window, GetFocus());
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
BOOL UiDoomCreateGame (SNETCREATEDATAPTR   createdata,
					   SNETPROGRAMDATAPTR  programdata,
					   SNETPLAYERDATAPTR   playerdata,
					   SNETUIDATAPTR       interfacedata,
					   SNETVERSIONDATAPTR  versiondata,
					   DWORD              *playerid,
					   BOOL				   loadfocus,
					   LPCSTR			   gamename) {
	int result;

	sgCreateData		= createdata;
	sgCre8ProgramData	= programdata;
	sgCre8PlayerData	= playerdata;
	sgpCre8PlayerID		= playerid;

	sgDiffLoadFocus		= loadfocus;

	sgGameName			= gamename;

	// DISPLAY THE DIALOG BOX
	result = SDlgDialogBoxParam(
		global_hinstance,
		TEXT("SELDIFF_DIALOG"),
		interfacedata->parentwindow,
		SelDiffDialogProc,
		SelHeroGetMode()
	);

	if (result == IDOK)
		return TRUE;
	SetLastError(sgLastError);
	return FALSE;
}

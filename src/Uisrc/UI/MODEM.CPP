//****************************************************************************
//	Modem.cpp
//	Diablo UI create/join modem multiplayer game
//	
//	By Frank Pearce
//	created 11.18.96
//****************************************************************************


#include "pch.h"
#include <process.h>
#include "artfont.h"
#include "uisnd.h"


//****************************************************************************
//****************************************************************************
	extern void TitleLogoDestroy(void);
	extern void TitleLogoInit(HWND window);
	extern void TitleFramesInit(HWND window, LPCTSTR filename);
	extern void TitleLogoAnimate(HWND window);

	extern int ModmStatDialog(HWND parent);

	extern int SelOkDialog(HWND parent, LPCTSTR text, LPCTSTR title, BOOL loadfocus);

	extern BOOL UiDoomCreateGame (
		SNETCREATEDATAPTR   createdata,
		SNETPROGRAMDATAPTR  programdata,
		SNETPLAYERDATAPTR   playerdata,
		SNETUIDATAPTR       interfacedata,
		SNETVERSIONDATAPTR  versiondata,
		DWORD              *playerid,
		BOOL				loadfocus,
		LPCSTR				gamename
	);
	extern BOOL CALLBACK EnterDialDialogProc(
		HWND window,
		UINT message,
		WPARAM wparam,
		LPARAM lparam
	);
	extern BOOL CALLBACK SelDialDialogProc(
		HWND window,
		UINT message,
		WPARAM wparam,
		LPARAM lparam
	);
	extern BOOL CALLBACK SelCre8JoinDialogProc(
		HWND window,
		UINT message,
		WPARAM wparam,
		LPARAM lparam
	);
	extern void SelDialSaveBookEntries(LPCSTR dialstring);



//****************************************************************************
//****************************************************************************
	#define PROVIDERID			'MODM'

	#define MILLISEC_PER_SEC	1000

	#define LOGO_TIMER_ID		2
	#define LOGO_FPS			17
	#define LOGO_TIMER_DELAY	55

	// messages to post to invoke child dialogs
	enum _modemstates {
		WM_STATE_SELDIALANSWER = (WM_USER+2000),
		WM_STATE_ENTERDIAL,
		WM_STATE_SELCRE8JOIN
	};


//****************************************************************************
//****************************************************************************
	#define GAMENAME_LEN		SNETSPI_MAXSTRINGLENGTH
	#define DESCRIPTION_LEN		SNETSPI_MAXSTRINGLENGTH

	typedef struct _modemgame TMGAME;
	typedef TMGAME *TPMGAME;
	struct _modemgame {
		DWORD gameid;
		TCHAR gamename[GAMENAME_LEN];
		TCHAR gamedesc[DESCRIPTION_LEN];
	};
	static TMGAME sgMGame;
	static BOOL sgConnected;
	static int sgGameCount;
	static int sgLastJoined;


//****************************************************************************
//****************************************************************************
	static int sgTextIDs[] = {
		IDC_DLGTITLE,
		0
	};
	static int sgDescTitle[] = {
		IDC_DLGTITLE2,
		0
	};
	static int sgModemDescID[] = {
		IDC_MODEMDESC,
		0
	};


//****************************************************************************
//****************************************************************************
	// storm related data
	static SNETPROGRAMDATAPTR  sgMdmProgramData;
	static SNETPLAYERDATAPTR   sgMdmPlayerData;
	static SNETUIDATAPTR       sgMdmInterfaceData;
	static SNETVERSIONDATAPTR  sgMdmVersionData;
	static DWORD			  *sgpMdmPlayerID;


//****************************************************************************
//****************************************************************************
BOOL ModemConnected(void) {
	return sgConnected;
}


//****************************************************************************
//****************************************************************************
void ModemSetInfo(HWND window, LPCSTR infotitle, LPCSTR text) {
	HWND child = GetDlgItem(window, IDC_MODEMDESC);
	if (! child) return;
	TPBMP tpBmp = (TPBMP) GetWindowLong(child, GWL_USERDATA);
	if (! tpBmp) return;
	UiSetBmpText(tpBmp, text);
	UiDoomStaticReset(window, sgModemDescID, AF_SMALLGRAY);

	child = GetDlgItem(window, IDC_DLGTITLE2);
	if (! child) return;
	tpBmp = (TPBMP) GetWindowLong(child, GWL_USERDATA);
	if (! tpBmp) return;
	UiSetBmpText(tpBmp, infotitle);
	UiDoomStaticReset(window, sgDescTitle, AF_MEDGRAY);
}


//****************************************************************************
//****************************************************************************
BOOL CALLBACK ModemGameEnum(DWORD  gameid,
							LPCSTR gamename,
							LPCSTR gamedesc) {
	sgMGame.gameid = gameid;
	strcpy(sgMGame.gamename, gamename);
	strcpy(sgMGame.gamedesc, gamedesc);
	return TRUE;
}


//****************************************************************************
//****************************************************************************
BOOL ModemEnum(void) {
	// clear old info
	sgMGame.gameid = 0;
	sgMGame.gamename[0] = 0;
	sgMGame.gamedesc[0] = 0;

	if (SNetEnumGames(0, 0, ModemGameEnum, NULL)) {		//### MM Diablo Patch #2  3/21/97   
		if (! sgMGame.gameid)
			return FALSE;
		return TRUE;
	}
	else {
		return FALSE;
	}
}


//****************************************************************************
//****************************************************************************
LPCSTR ModemGameName(void) {
	return sgMGame.gamename;
}


//****************************************************************************
//****************************************************************************
static void ModemUpdateGame(HWND window) {
	// clear old info
	sgMGame.gameid = 0;
	sgMGame.gamename[0] = 0;
	sgMGame.gamedesc[0] = 0;

	// enum will fill in info if a game exists
	if (! SNetEnumGames(0, 0, ModemGameEnum, NULL)) {		//### MM Diablo Patch #2  3/21/97   
		switch (GetLastError()) {
			case SNET_ERROR_NO_NETWORK:
				sgConnected = FALSE;
				sgGameCount = sgLastJoined = 1;
			break;
			default:
			break;
		}
	}
	else {
		sgConnected = TRUE;
	}
}


//****************************************************************************
//****************************************************************************
static void ModemDestroy(HWND window) {
	UiDoomCtrlsDestroy(window, sgModemDescID);
	UiDoomCtrlsDestroy(window, sgDescTitle);
	UiDoomCtrlsDestroy(window, sgTextIDs);

	// free the background of the dlg
	UiFreeBmp((TPBMP) GetWindowLong(window, GWL_USERDATA));
	SetWindowLong(window, GWL_USERDATA, (LONG) 0);

	TitleLogoDestroy();
}


//****************************************************************************
//****************************************************************************
static void ModemAbort(HWND window, int ReturnVal) {
	UiFadeAbort(window);
	SDlgKillTimer(window, LOGO_TIMER_ID);
	UiVidFadeOut(DEFAULT_STEPS*2);
	SDlgEndDialog(window, ReturnVal);
}


//****************************************************************************
//****************************************************************************
static void ModemInit(HWND window) {
	TPBMP tpBmp;

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

	// set up a doom-like interface
	UiDoomStaticInit(window, sgTextIDs, AF_BIGGRAY);
	UiDoomStaticInit(window, sgDescTitle, AF_MEDGRAY);
	UiDoomStaticInit(window, sgModemDescID, AF_SMALLGRAY);

	// try to enum modem games
	ModemUpdateGame(window);
	if (sgConnected) {
		PostMessage(window, WM_STATE_SELCRE8JOIN, 0, 0);
	}
	else {
		sgGameCount = sgLastJoined = 1;
		PostMessage(window, WM_STATE_SELDIALANSWER, 0, 0);
	}
}


//****************************************************************************
//****************************************************************************
static void ModemCreate(HWND window) {
	char namefmt[32];
	char gamename[64];
	SNETUIDATA interfacedata;
	SNETCREATEDATA createdata;

	// create new interface data
	CopyMemory(&interfacedata,sgMdmInterfaceData,sizeof(SNETUIDATA));
	interfacedata.size         = sizeof(SNETUIDATA);
	interfacedata.parentwindow = window;

	ZeroMemory(&createdata,sizeof(SNETCREATEDATA));
	createdata.size        = sizeof(SNETCREATEDATA);
	createdata.providerid  = PROVIDERID;
	createdata.maxplayers  = sgMdmProgramData->maxplayers;
	createdata.createflags = 0;

	LoadString(global_hinstance, IDS_MODEMGAMENAME_FMT, namefmt, 31);
	wsprintf(gamename, namefmt, sgGameCount);

	if (UiDoomCreateGame(
			&createdata,
			sgMdmProgramData,
			sgMdmPlayerData,
			&interfacedata,
			sgMdmVersionData,
			sgpMdmPlayerID,
			TRUE,
			gamename
		)
	) {
		sgGameCount++;
		ModemAbort(window, IDOK);
	}
	else {
		if (! sgConnected) {
   			PostMessage(window, WM_STATE_SELDIALANSWER, 0, 0);
			return;
		}
		if (GetLastError() == SNET_ERROR_ALREADY_EXISTS) {
			sgGameCount++;
		}
		PostMessage(window, WM_STATE_SELCRE8JOIN, 0, 0);
	}
}


//****************************************************************************
//****************************************************************************
static BOOL sgSNetJoinDone;
static BOOL sgJoinResult;
static int  sgSNetJoinErr;
static void __cdecl ModemJoinThread(void *dialstring) {
	sgSNetJoinDone = FALSE;
	sgJoinResult = SNetJoinGame(
		0,
		(sgConnected ? sgMGame.gamename: (LPCSTR)dialstring),
		NULL,
		sgMdmPlayerData->playername,
		sgMdmPlayerData->playerdescription,
		sgpMdmPlayerID
	);
	if (! sgJoinResult)
		sgSNetJoinErr = GetLastError();
	sgSNetJoinDone = TRUE;
	_endthread();
}


//****************************************************************************
//****************************************************************************
BOOL ModemJoinDone(void) {
	return sgSNetJoinDone;
}


//****************************************************************************
//****************************************************************************
int ModemLastJoinedNumber(void) {
	return sgLastJoined;
}


//****************************************************************************
//****************************************************************************
int ModemGameNumber(LPCSTR gamename) {

	BYTE * szName = (BYTE *)gamename;

	// locate the game number in the string
	while ((*szName) && ((*szName < '0') || (*szName > '9'))) {
		szName++;
	}

	if (*szName) {
		return atoi((LPSTR)szName);
	}
	return 0;
}


//****************************************************************************
//****************************************************************************
static void ModemJoin(HWND window, LPCSTR DialString) {

	sgSNetJoinDone = FALSE;
	_beginthread(ModemJoinThread, 0, (void *)DialString);

	ModmStatDialog(window);

	if (sgJoinResult) {
		// is this the first link up?
		if (! sgConnected) {
			SelDialSaveBookEntries(DialString);
			ModemEnum();
		}
		// update the local game number
		sgGameCount = sgLastJoined = ModemGameNumber(sgMGame.gamename);
		sgGameCount++;

		ModemAbort(window, IDOK);
	}
	else {
   		TCHAR err[128];

		switch (sgSNetJoinErr) {
			case SNET_ERROR_VERSION_MISMATCH:
   				LoadString(global_hinstance, IDS_VERSION_ERR, err, 127);
   				SelOkDialog(window, err, NULL, TRUE);
			break;
			case SNET_ERROR_CANCELLED:
				// just bring up previous menu
			break;
			case SNET_ERROR_NOT_CONNECTED:
				// no answer
   				LoadString(global_hinstance, IDS_NOANSWER_ERR, err, 127);
   				SelOkDialog(window, err, NULL, TRUE);
			break;
			case SNET_ERROR_NO_NETWORK:
				// no dial tone
   				LoadString(global_hinstance, IDS_NOTONE_ERR, err, 127);
   				SelOkDialog(window, err, NULL, TRUE);
			break;
// fgp.patch1.start.1/17/96
//			case SNET_ERROR_HOST_UNREACHABLE:
			case SNET_ERROR_NETWORK_BUSY:
// fgp.patch1.end.1/17/96
				// busy signal
   				LoadString(global_hinstance, IDS_BUSY_ERR, err, 127);
   				SelOkDialog(window, err, NULL, TRUE);
			break;
			case SNET_ERROR_BAD_PROVIDER:
				// unable to establish a connection to the modem
   				LoadString(global_hinstance, IDS_MODEMOFF_ERR, err, 127);
   				SelOkDialog(window, err, NULL, TRUE);
			break;
			default:
				LoadString(global_hinstance, IDS_MODEMJOIN_ERR, err, 127);
				SelOkDialog(window, err, NULL, TRUE);
			break;
		}

		if (sgConnected) {
			PostMessage(window, WM_STATE_SELCRE8JOIN, 0, 0);
		}
		else {
			PostMessage(window, WM_STATE_SELDIALANSWER, 0, 0);
		}
	}
}


//****************************************************************************
//****************************************************************************
static void ModemSelDialAnswer(HWND window) {
	TCHAR dialstring[MAX_DIAL_LEN];
	int result;

  	// bring up the enter name screen
   	result = SDlgDialogBoxParam(
   		global_hinstance,
   		TEXT("SELDIAL_DIALOG"),
   		window,
   		SelDialDialogProc,
		(LPARAM) dialstring
   	);

	switch (result) {
		case MODE_DIALNEW:
			PostMessage(window, WM_STATE_ENTERDIAL, 0, 0);
		break;
		case MODE_DIALOLD:
			ModemJoin(window, dialstring);
		break;
		case MODE_ANSWER:
			ModemCreate(window);
		break;
		default:
			ModemAbort(window, IDCANCEL);
		break;
	}
}


//****************************************************************************
//****************************************************************************
static void ModemEnterDial(HWND window) {
	TCHAR dialstring[MAX_DIAL_LEN];
	int result;

  	// bring up the enter name screen
   	result = SDlgDialogBoxParam(
   		global_hinstance,
   		TEXT("ENTERDIAL_DIALOG"),
   		window,
   		EnterDialDialogProc,
		(LPARAM) &dialstring
   	);

	if (result == IDOK) {
		ModemJoin(window, dialstring);
	}
	else {
		PostMessage(window, WM_STATE_SELDIALANSWER, 0, 0);
	}
}


//****************************************************************************
//****************************************************************************
static void ModemSelCre8Join(HWND window) {
	int result;

  	// bring up the enter name screen
   	result = SDlgDialogBox(
   		global_hinstance,
   		TEXT("SELCRE8JOIN_DIALOG"),
   		window,
   		SelDialDialogProc
   	);

	switch (result) {
		case MODE_DIALNEW:
			ModemJoin(window, sgMGame.gamename);
		break;
		case MODE_ANSWER:
			ModemCreate(window);
		break;
		case SNET_ERROR_NO_NETWORK:
			sgConnected = FALSE;
			PostMessage(window, WM_STATE_SELDIALANSWER, 0, 0);
		break;
		default:
			ModemAbort(window, IDCANCEL);
		break;
	}
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK ModemDialogProc(HWND window,
									 UINT message,
									 WPARAM wparam,
									 LPARAM lparam) {
	switch (message) {

		case WM_DESTROY:
			ModemDestroy(window);
		break;

		case WM_INITDIALOG:
			ModemInit(window);
			PostMessage(window, WM_USER+1000, 0, 0);
		return 0;

		case WM_STATE_SELDIALANSWER:
			ModemSelDialAnswer(window);
		return 0;

		case WM_STATE_ENTERDIAL:
			ModemEnterDial(window);
		return 0;

		case WM_STATE_SELCRE8JOIN:
			ModemSelCre8Join(window);
		return 0;

		case WM_USER+1000:
			if (! UiIsFading()) {
				UiFadeStart(window);
			}
		return 0;

		case WM_TIMER:
			TitleLogoAnimate(window);
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
BOOL UiModemGame (DWORD               flags,
				  SNETPROGRAMDATAPTR  programdata,
				  SNETPLAYERDATAPTR   playerdata,
				  SNETUIDATAPTR       interfacedata,
				  SNETVERSIONDATAPTR  versiondata,
				  DWORD              *playerid) {
	int result = 0;

	// save parameters for use throughout this file
	sgMdmProgramData	= programdata;
	sgMdmPlayerData		= playerdata;
	sgMdmInterfaceData	= interfacedata;
	sgMdmVersionData	= versiondata;
	sgpMdmPlayerID		= playerid;

	// make sure fonts are loaded
	ArtFontLoad();

	// DISPLAY THE DIALOG BOX
	result = SDlgDialogBox(global_hinstance,
						   TEXT("MODEM_DIALOG"),
						   interfacedata->parentwindow,
						   ModemDialogProc);

	ArtFontDestroy();

	if (result == IDOK)
		return TRUE;
	return FALSE;
}

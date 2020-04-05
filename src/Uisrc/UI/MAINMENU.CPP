//****************************************************************************
//	MainMenu.cpp
//	Diablo UI main menu dialog
//	
//	By Frank Pearce
//	created 9.17.96
//****************************************************************************

#define OEMRESOURCE

#include "pch.h"
#include "artfont.h"
#include "uisnd.h"


#define ALLOW_SINGLE		1		// 1 in final

#define MIN_MEMORY_REQ		0x00dac000L		// 14MB


//****************************************************************************
//****************************************************************************
	extern void TitleLogoDestroy(void);
	extern void TitleLogoInit(HWND window);
	extern void TitleFramesInit(HWND window, LPCTSTR filename);
	extern void TitleLogoAnimate(HWND window);

	extern int SpawnErrDialog(HWND parent, int errstrid, BOOL onmainmenu);


//****************************************************************************
//****************************************************************************
	#define MILLISEC_PER_SEC	1000

	#define FOCUS_TIMER_ID		1
	#define FOCUS_FPS			16		// frames per second
	#define FOCUS_TIMER_DELAY	55		//(MILLISEC_PER_SEC / FOCUS_FPS)

	#define LOGO_TIMER_ID		2
	#define LOGO_FPS			17		// frames per second
	#define LOGO_TIMER_DELAY	55		//(MILLISEC_PER_SEC / LOGO_FPS)

	#define ATTRACT_TIMER_ID	3

	#define MAX_REG_LEN			64

	static TCHAR sgRegistration[MAX_REG_LEN];

//****************************************************************************
//****************************************************************************
	static int sgTextIDs[] = {
		IDC_REGISTRATION,
		0
	};
#if ALLOW_SINGLE
	static int sgBtnIDs[] = {
		IDC_SINGLEPLAYER_BTN,
		IDC_MULTIPLAYER_BTN,
		IDC_REPLAYINTRO_BTN,
		IDC_SUPPORT_BTN,
		IDC_SHOWCREDITS_BTN,
		IDCANCEL,
		0
	};
#else
	static int sgGrayBtnIDs[] = {
		IDC_SINGLEPLAYER_BTN,
		0
	};
	static int sgBtnIDs[] = {
		IDC_MULTIPLAYER_BTN,
		IDC_REPLAYINTRO_BTN,
		IDC_SUPPORT_BTN,
		IDC_SHOWCREDITS_BTN,
		IDCANCEL,
		0
	};
#endif

	static UINT sgAttractTimeout;


//****************************************************************************
//****************************************************************************
LPCTSTR GetRegStr(void) {
	return sgRegistration;
}


//****************************************************************************
//****************************************************************************
static void MainMenuDestroy(HWND window) {
	TitleLogoDestroy();

	FocusAnimateDestroy();

#if ALLOW_SINGLE
	UiDoomCtrlsDestroy(window, sgBtnIDs);
#else
	UiDoomCtrlsDestroy(window, sgGrayBtnIDs);
	UiDoomCtrlsDestroy(window, sgBtnIDs);
#endif
	UiDoomCtrlsDestroy(window, sgTextIDs);

	UiFreeBmp((TPBMP) GetWindowLong(window, GWL_USERDATA));
	SetWindowLong(window, GWL_USERDATA, (LONG) 0);

	UiClearCursor();
}


//****************************************************************************
//****************************************************************************
static void MainMenuResetAttractTimer(HWND window) {
	// Crashes so we pulled this.
	//SDlgSetTimer(window, ATTRACT_TIMER_ID, sgAttractTimeout * MILLISEC_PER_SEC, NULL);
}


//****************************************************************************
//****************************************************************************
static void MainMenuInit(HWND window, UINT attracttimeout) {
	TPBMP tpBmp;
	HWND child;

	// init a bmp for the window
	tpBmp = UiAllocBmp();
	if (tpBmp) {
		SetWindowLong(window, GWL_USERDATA, (LONG) tpBmp);
		LoadArtFile(
			window,
			NULL,
			TEXT(""),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,
			UiIsSpawned() ? TEXT("ui_art\\swmmenu.pcx") : TEXT("ui_art\\mainmenu.pcx"),
			&tpBmp->data,
			&tpBmp->datasize,
			FALSE
		);
		UiFadeInit(window);
	}

	// set the reg string
	child = GetDlgItem(window, IDC_REGISTRATION);
	SetWindowText(child, sgRegistration);

	// set up a doom-like interface
	UiDoomStaticInit(window, sgTextIDs, AF_SMALLGRAY);
#if ALLOW_SINGLE
	UiDoomButtonsInit(window, sgBtnIDs, AF_HUGE);
#else
	UiDoomButtonsInit(window, sgBtnIDs, AF_HUGE);
	UiDoomButtonsInit(window, sgGrayBtnIDs, AF_HUGEGRAY);
	EnableWindow(GetDlgItem(window, IDC_SINGLEPLAYER_BTN), FALSE);
#endif

	// set up the animating focus indicator
	FocusAnimateInit("ui_art\\focus42.pcx");
	SDlgSetTimer(window, FOCUS_TIMER_ID, FOCUS_TIMER_DELAY, NULL);

	// set up the animating diablo logo
	TitleLogoInit(window);
	TitleFramesInit(window,TEXT("ui_art\\hf_logo2.pcx"));
	TitleLogoAnimate(window);
	SDlgSetTimer(window, LOGO_TIMER_ID, LOGO_TIMER_DELAY, NULL);

	MainMenuResetAttractTimer(window);

	UiActiveDownBtns(window, sgBtnIDs);
}


//****************************************************************************
//****************************************************************************
static BOOL HasMultiplayerMemory(void) {
	MEMORYSTATUS memstat;

	memstat.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&memstat);
	return(memstat.dwTotalPhys > MIN_MEMORY_REQ);
}


//****************************************************************************
//****************************************************************************
static void MainMenuAbort(HWND window, int ReturnVal, BOOL playsnd) {
	//Crashes so we pulled this.
	//SDlgKillTimer(window, ATTRACT_TIMER_ID);

	if (UiIsSpawned()) {
		if (ReturnVal == MAINMENU_REPLAY_INTRO) {
			SpawnErrDialog(window, IDS_SPAWNINTRO_ERR, TRUE);
			MainMenuResetAttractTimer(window);
			return;
		}
	}

	// if user selected multiplayer confirm memory requirements
	if (ReturnVal == MAINMENU_MULTIPLAYER) {
		if (! HasMultiplayerMemory()) {
			SpawnErrDialog(window, IDS_MEMORY_ERR, TRUE);
			MainMenuResetAttractTimer(window);
			return;
		}
	}

	UiFadeAbort(window);
	if (playsnd)
		UiSndPlayEnter();
	SDlgKillTimer(window, FOCUS_TIMER_ID);
	SDlgKillTimer(window, LOGO_TIMER_ID);

	UiVidFadeOut(DEFAULT_STEPS*2);
	SDlgEndDialog(window, ReturnVal);
}


//****************************************************************************
//****************************************************************************
static void MainMenuBtnAction(HWND window, int id) {
	switch (id) {
		case IDC_SINGLEPLAYER_BTN:
			MainMenuAbort(window,MAINMENU_SINGLE_PLAYER, TRUE);
		break;
		case IDC_MULTIPLAYER_BTN:
			MainMenuAbort(window,MAINMENU_MULTIPLAYER, TRUE);
		break;
		case IDC_REPLAYINTRO_BTN:
			MainMenuAbort(window,MAINMENU_REPLAY_INTRO, TRUE);
		break;
		case IDC_SUPPORT_BTN:
			MainMenuAbort(window,MAINMENU_SUPPORT, TRUE);
		break;
		case IDC_SHOWCREDITS_BTN:
			MainMenuAbort(window,MAINMENU_SHOW_CREDITS, TRUE);
		break;
		case IDCANCEL:
			MainMenuAbort(window,MAINMENU_EXIT_DIABLO, TRUE);
		break;
		case IDOK: {
			// enter key was pressed
			HWND FocusBtn = GetFocus();
			int BtnID = GetWindowLong(FocusBtn, GWL_ID);
			SendMessage(window, WM_COMMAND, BtnID, NULL);
		}
		break;
	}
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK MainMenuDialogProc (HWND   window,
										UINT   message,
										WPARAM wparam,
										LPARAM lparam) {
	switch (message) {

		case WM_COMMAND:
			MainMenuResetAttractTimer(window);
			if (HIWORD(wparam) == BN_KILLFOCUS) {
				FocusLost(window, (HWND) lparam);
			}
			else if (HIWORD(wparam) == BN_SETFOCUS) {
				FocusSnd((HWND) lparam);
				FocusAnimate(window, (HWND) lparam);
			}
			else if (HIWORD(wparam) == BN_CLICKED) {
				MainMenuBtnAction(window, LOWORD(wparam));
			}
		break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
			MainMenuResetAttractTimer(window);
		break;

		case WM_DESTROY:
			MainMenuDestroy(window);
		break;

		case WM_INITDIALOG:
			MainMenuInit(window, lparam);
			PostMessage(window, WM_USER+1000, 0, 0);
		return 1;

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
				case ATTRACT_TIMER_ID:
					if (! UiIsSpawned()) {
						MainMenuAbort(window,MAINMENU_ATTRACT_MODE, FALSE);
					}
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
//*
//*  EXPORTED FUNCTIONS
//*
//****************************************************************************


//****************************************************************************
//****************************************************************************
BOOL APIENTRY UiMainMenuDialog(LPCTSTR registration,
							   DWORD * selection,
							   bool	   allowMultiplayer,
							   PLAYSND sndfcn,
							   UINT    attracttimeoutseconds
							   ) {
	DWORD result;

	sgAttractTimeout = attracttimeoutseconds;

	// init the play sound routine for diabloui
	UiSndSetFcn(sndfcn);

	// init the custom non-windows font
	ArtFontLoad();

	sgRegistration[0] = 0;
	if (registration)
		strncpy(sgRegistration, registration, MAX_REG_LEN);

	// DISPLAY THE DIALOG BOX
	result = (DWORD)SDlgDialogBox(
		global_hinstance,
		(allowMultiplayer) ? TEXT("MAINMENU_DIALOG2") : TEXT("MAINMENU_DIALOG"),
		SDrawGetFrameWindow(),
		MainMenuDialogProc
	);

	if (result == MAINMENU_EXIT_DIABLO) {
		ArtFontDestroy();
	}

	// set the results
	if (selection)
		*selection = result;

	return 1;
}

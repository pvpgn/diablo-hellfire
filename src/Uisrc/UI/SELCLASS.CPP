//****************************************************************************
//	SelClass.cpp
//	Diablo UI select class for a new hero
//	
//	By Frank Pearce
//	created 10.18.96
//****************************************************************************


#include "pch.h"
#include "artfont.h"
#include "uisnd.h"


//****************************************************************************
//****************************************************************************
	extern void SelHeroGetDefStats(int heroclass, TPUIDEFSTATS stats);
	extern void SelHeroSetStats(HWND window, TPUIHEROINFO hero);
	extern void SelHeroClearStats(HWND window);
	extern void SelHeroSetTitle(HWND window, LPCTSTR title);
	extern int SelHeroGetMode(void);

	extern int SpawnErrDialog(HWND parent, int errstrid, BOOL onmainmenu);


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
		0
	};
	static int sgBtnIDs[] = {
		IDC_FAKEOK,
		IDC_FAKECANCEL,
		0
	};
	static int sgListIDs[] = {
		IDC_WARRIORBTN,
		IDC_ROGUEBTN,
		IDC_SORCERERBTN,
		IDC_MONKBTN,
		IDC_BARDBTN,
		IDC_BARBARIANBTN,
		0
	};


//****************************************************************************
//****************************************************************************
static void SelClassDestroy(HWND window) {
	FocusAnimateDestroy();
	UiDoomCtrlsDestroy(window, sgListIDs);
	UiDoomCtrlsDestroy(window, sgBtnIDs);
	UiDoomCtrlsDestroy(window, sgTextIDs);
	SelHeroSetTitle(GetParent(window), NULL);
}


//****************************************************************************
//****************************************************************************
static void SelClassInit(HWND window) {
	TCHAR buf[32];
	HWND parent = GetParent(window);

	// set the title for the parent
	if (SelHeroGetMode() == MULTIPLAYER)
		LoadString(global_hinstance, IDS_NEWMULTHERO, buf, 32-1);
	else
		LoadString(global_hinstance, IDS_NEWSINGHERO, buf, 32-1);
	SelHeroSetTitle(parent, buf);

	// point this window at the same bmp it's parent uses
	SetWindowLong(
		window,
		GWL_USERDATA,
		GetWindowLong(parent, GWL_USERDATA)
	);

	UiOnPaintBtns(window, sgListIDs);

	// set up a doom-like interface
	UiDoomStaticInit(window, sgTextIDs, AF_BIGGRAY);
	UiDoomButtonsInit(window, sgBtnIDs, AF_BIG, FALSE);
	UiDoomButtonsInit(window, sgListIDs, AF_MED);

	// set up the animating focus indicator
	FocusAnimateInit("ui_art\\focus16.pcx");
	SDlgSetTimer(window, FOCUS_TIMER_ID, FOCUS_TIMER_DELAY, NULL);
}


//****************************************************************************
//****************************************************************************
static void UpdateStats(HWND window, int childid) {
	TUIHEROINFO focused;
	TUIDEFSTATS defstats;


	// MM: make sure we init name field to zero since SelHeroSetStats() will 
	// be using it.
	ZeroMemory((LPBYTE)&focused, sizeof(focused));

	focused.level = 1;
	switch (childid)
	{
	case IDC_WARRIORBTN:	
		focused.heroclass = UI_WARRIOR;
		break;
	case IDC_ROGUEBTN:
		focused.heroclass = UI_ROGUE;
		break;
	case IDC_SORCERERBTN:
		focused.heroclass = UI_SORCERER;
		break;
	case IDC_MONKBTN:
		focused.heroclass = UI_MONK;
		break;
	case IDC_BARDBTN:
		focused.heroclass = UI_BARD;
		break;
	case IDC_BARBARIANBTN:
		focused.heroclass = UI_BARBARIAN;
		break;
	}
	SelHeroGetDefStats(focused.heroclass, &defstats);

	focused.strength = defstats.strength;
	focused.magic = defstats.magic;
	focused.dexterity = defstats.dexterity;
	focused.vitality = defstats.vitality;

	SelHeroSetStats(GetParent(window), &focused);
}


//****************************************************************************
//****************************************************************************
static void SelClassAbort(HWND window, int ReturnVal) {
	if (UiIsSpawned() && (ReturnVal == IDOK)) {
		if (IDC_WARRIORBTN != GetWindowLong(GetFocus(), GWL_ID)) {
			SpawnErrDialog(window, IDS_SPAWNCLASS_ERR, FALSE);
			return;
		}
	}

	UiSndPlayEnter();
	SDlgKillTimer(window, FOCUS_TIMER_ID);

	if (ReturnVal == IDOK) {
		// return the id of the focused button
		ReturnVal = GetWindowLong(GetFocus(), GWL_ID);
	}

	SDlgEndDialog(window, ReturnVal);
}


//****************************************************************************
//****************************************************************************
BOOL CALLBACK SelClassDialogProc(HWND window,
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
				UpdateStats(window, LOWORD(wparam));
			}
			else if (HIWORD(wparam) == BN_DOUBLECLICKED) {
				SelClassAbort(window, IDOK);
			}
			else if (LOWORD(wparam) == IDOK) {
				SelClassAbort(window, IDOK);
			}
			else if (LOWORD(wparam) == IDCANCEL) {
				SelClassAbort(window, IDCANCEL);
			}
		break;

		case WM_LBUTTONDOWN:
			if (
				UiIsPtInWindow(
					window,
					GetDlgItem(window, IDC_FAKEOK),
					LOWORD(lparam),
					HIWORD(lparam)
				)
			) {
				SelClassAbort(window, IDOK);
			}
			else if (
				UiIsPtInWindow(
					window,
					GetDlgItem(window, IDC_FAKECANCEL),
					LOWORD(lparam),
					HIWORD(lparam)
				)
			) {
				SelClassAbort(window, IDCANCEL);
			}
		break;

		case WM_DESTROY:
			SelClassDestroy(window);
		break;

		case WM_INITDIALOG:
			SelClassInit(window);
		return 0;

		case WM_TIMER:
			FocusAnimate(window, GetFocus());
		return 0;

		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			SendMessage(SDrawGetFrameWindow(), message, wparam, lparam);
		break;
	}
	return SDlgDefDialogProc(window,message,wparam,lparam);
}

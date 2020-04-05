//****************************************************************************
//	SelList.cpp
//	Diablo UI select hero dialogs
//	
//	By Frank Pearce
//	created 10.18.96
//****************************************************************************


#include "pch.h"
#include "artfont.h"
#include "uisnd.h"


//****************************************************************************
//****************************************************************************
	extern void SelHeroSetStats(HWND window, TPUIHEROINFO hero);
	extern void SelHeroClearStats(HWND window);
	extern void SelHeroSetTitle(HWND window, LPCTSTR title);
	extern int SelHeroGetMode(void);
	extern TPUIHEROINFO GetHeroList(void);
	extern int GetNumHeros(void);
	extern LPCTSTR SelectedName(void);


//****************************************************************************
//****************************************************************************
	#define OLDPROCPROP			TEXT("UIOLDPROC")

	#define MILLISEC_PER_SEC	1000

	#define FOCUS_TIMER_ID		1
	#define FOCUS_FPS			16		// frames per second
	#define FOCUS_TIMER_DELAY	55		//(MILLISEC_PER_SEC / FOCUS_FPS)

	#define MAX_VISIBLE			(IDC_HEROBTN6 - IDC_HEROBTN1 + 1)


//****************************************************************************
//****************************************************************************
	static TPUIHEROINFO sgFirstHero = NULL;

	static int sgTextIDs[] = {
		IDC_DLGTITLE,
		0
	};
	static int sgBtnIDs[] = {
		IDC_FAKEOK,
		IDC_FAKECANCEL,
		0
	};
	static int sgDeleteIDs[] = {
		IDDELETE,
		0
	};
	static int sgListIDs[] = {
		IDC_HEROBTN1,
		IDC_HEROBTN2,
		IDC_HEROBTN3,
		IDC_HEROBTN4,
		IDC_HEROBTN5,
		IDC_HEROBTN6,
		0
	};


//****************************************************************************
//****************************************************************************
static void SelListDestroy(HWND window) {
	SbarDestroy(window, IDC_SCROLLBAR);
	FocusAnimateDestroy();
	UiDoomCtrlsDestroy(window, sgListIDs);
	UiDoomCtrlsDestroy(window, sgDeleteIDs);
	UiDoomCtrlsDestroy(window, sgBtnIDs);
	UiDoomCtrlsDestroy(window, sgTextIDs);
	SelHeroSetTitle(GetParent(window), NULL);
}


//****************************************************************************
//****************************************************************************
static void SelListSetList(HWND window, TPUIHEROINFO firsthero) {
	HWND child;
	TPBMP tpBmp;
	int *ListID;

	for (ListID = sgListIDs; *ListID; ListID++) {
		child = GetDlgItem(window, *ListID);
		if (! child)
			continue;

		if (! firsthero) {
			EnableWindow(child, FALSE);
			continue;
		}

		EnableWindow(child, TRUE);
		tpBmp = (TPBMP) GetWindowLong(child, GWL_USERDATA);
		UiSetBmpText(tpBmp, firsthero->name);
		if (tpBmp)
			tpBmp->userdata = (LONG) firsthero;

		firsthero = firsthero->next;
	}

	UiDoomButtonsReset(window, sgListIDs, AF_MED);
}


//****************************************************************************
//****************************************************************************
static void UpdateStats(HWND window, int childid) {
	HWND child = GetDlgItem(window, childid);
	if (! child) return;
	TPBMP tpBmp = (TPBMP) GetWindowLong(child, GWL_USERDATA);
	if (! tpBmp) return;
	TPUIHEROINFO focusedhero = (TPUIHEROINFO) tpBmp->userdata;

	if (focusedhero) {
		if (focusedhero->level)
			UiDoomButtonsReset(window, sgDeleteIDs, AF_BIG, FALSE);
		else
			UiDoomButtonsReset(window, sgDeleteIDs, AF_BIGGRAY, FALSE);
		SelHeroSetStats(GetParent(window), focusedhero);
	}
	else {
		UiDoomButtonsReset(window, sgDeleteIDs, AF_BIGGRAY, FALSE);
		SelHeroClearStats(GetParent(window));
	}
}


//****************************************************************************
//****************************************************************************
static void SelListTab(HWND child) {
	HWND parent, newfocus;
	int index;
	int NewID[] = {
		IDC_HEROBTN2,
		IDC_HEROBTN3,
		IDC_HEROBTN4,
		IDC_HEROBTN5,
		IDC_HEROBTN6,
		IDC_HEROBTN1
	};

	parent = GetParent(child);
	newfocus = child;
	do {
		index = GetWindowLong(newfocus, GWL_ID) - IDC_HEROBTN1;
		newfocus = GetDlgItem(parent, NewID[index]);
	} while (! IsWindowEnabled(newfocus));
	SetFocus(newfocus);
}


//****************************************************************************
//****************************************************************************
static void SelListShiftTab(HWND child) {
	HWND parent, newfocus;
	int index;
	int NewID[] = {
		IDC_HEROBTN6,
		IDC_HEROBTN1,
		IDC_HEROBTN2,
		IDC_HEROBTN3,
		IDC_HEROBTN4,
		IDC_HEROBTN5
	};

	parent = GetParent(child);
	newfocus = child;
	do {
		index = GetWindowLong(newfocus, GWL_ID) - IDC_HEROBTN1;
		newfocus = GetDlgItem(parent, NewID[index]);
	} while (! IsWindowEnabled(newfocus));
	SetFocus(newfocus);
}


//****************************************************************************
//****************************************************************************
static int GetNumNode(HWND btn) {
	if (! btn) return 0;
	TPBMP tpBmp = (TPBMP) GetWindowLong(btn, GWL_USERDATA);
	if (! tpBmp) return 0;
	TPUIHEROINFO list = GetHeroList();
	if (! list) return 0;
	TPUIHEROINFO node = (TPUIHEROINFO) tpBmp->userdata;
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
static TPUIHEROINFO GetHero(int index) {
	TPUIHEROINFO list = GetHeroList();
	
	while (list && index) {
		list = list->next;
		index--;
	}
	return list;
}


//****************************************************************************
//****************************************************************************
static void SelListUpdateSbar(HWND window) {
	SbarDraw(window, IDC_SCROLLBAR, GetNumHeros(), GetNumNode(GetFocus()));
}


//****************************************************************************
//****************************************************************************
static void SelListPageDown(HWND focus) {
	HWND parent = GetParent(focus);
	if (! parent) return;

	// get info of 1st btn
	HWND btn1 = GetDlgItem(parent, IDC_HEROBTN1);
	if (! btn1) return;

	// see if there is info below the bottom
	TPBMP tpBmp = (TPBMP) GetWindowLong(GetDlgItem(parent, IDC_HEROBTN6), GWL_USERDATA);
	if (! tpBmp) return;
	TPUIHEROINFO tpHero = (TPUIHEROINFO) tpBmp->userdata;
	if ((! tpHero) || (! tpHero->next)) {
		SelListShiftTab(btn1);
		return;
	}

	// calc the index of the new top
	int newtop = GetNumNode(btn1) + MAX_VISIBLE;
	if (newtop > (GetNumHeros() - MAX_VISIBLE))
		newtop = GetNumHeros() - MAX_VISIBLE;

	// get the pointer to that index
	tpHero = GetHero(newtop);
	if (! tpHero) return;

	// update everything on the screen
	UiSndPlayClick();
	SelListSetList(parent, tpHero);
	UpdateStats(parent, GetWindowLong(focus, GWL_ID));
	SelListUpdateSbar(parent);
}


//****************************************************************************
//****************************************************************************
static void SelListPageUp(HWND focus) {
	HWND parent = GetParent(focus);
	if (! parent) return;
	HWND btn1 = GetDlgItem(parent, IDC_HEROBTN1);
	if (! btn1) return;

	// see if there is info off the top
	TPBMP tpBmp = (TPBMP) GetWindowLong(btn1, GWL_USERDATA);
	if (! tpBmp) return;
	TPUIHEROINFO tpHero = (TPUIHEROINFO) tpBmp->userdata;
	if (! tpHero) return;
	if (tpHero == GetHeroList()) {
		SelListTab(GetDlgItem(parent, IDC_HEROBTN6));
		return;
	}

	// calc the index of the new top
	int newtop = GetNumNode(btn1) - MAX_VISIBLE;
	if (newtop < 0)
		newtop = 0;

	// get the pointer to that index
	tpHero = GetHero(newtop);
	if (! tpHero) return;

	// update everything on the screen
	UiSndPlayClick();
	SelListSetList(parent, tpHero);
	UpdateStats(parent, GetWindowLong(focus, GWL_ID));
	SelListUpdateSbar(parent);
}


//****************************************************************************
//****************************************************************************
static void SelListScrollDown(HWND focus) {
	// see if there is info below the focus
	TPBMP tpBmp = (TPBMP) GetWindowLong(focus, GWL_USERDATA);
	if (! tpBmp) return;
	TPUIHEROINFO tpHero = (TPUIHEROINFO) tpBmp->userdata;
	if (! tpHero) return;
	if (! tpHero->next) return;

	if (GetWindowLong(focus, GWL_ID) < IDC_HEROBTN6) {
		// handle it just like a tab
		SelListTab(focus);
		return;
	}

	// advance the list
	HWND child = GetDlgItem(GetParent(focus), IDC_HEROBTN2);
	if (! child) return;
	tpBmp = (TPBMP) GetWindowLong(child, GWL_USERDATA);
	if (! tpBmp) return;
	tpHero = (TPUIHEROINFO) tpBmp->userdata;
	if (! tpHero) return;
	UiSndPlayClick();
	SelListSetList(GetParent(focus), tpHero);
	UpdateStats(GetParent(focus), GetWindowLong(focus, GWL_ID));
	SelListUpdateSbar(GetParent(focus));
}


//****************************************************************************
//****************************************************************************
static void SelListScrollUp(HWND focus) {
	if (GetWindowLong(focus, GWL_ID) > IDC_HEROBTN1) {
		// handle it just like a shift-tab
		SelListShiftTab(focus);
		return;
	}

	// see if there is info off the top
	TPBMP tpBmp = (TPBMP) GetWindowLong(focus, GWL_USERDATA);
	if (! tpBmp) return;
	TPUIHEROINFO tpHero = (TPUIHEROINFO) tpBmp->userdata;
	TPUIHEROINFO tpHeroList;
	if (! tpHero) return;
	if (tpHero == (tpHeroList = GetHeroList())) return;

	// retreat the list
	while (tpHeroList && (tpHeroList->next != tpHero)) {
		tpHeroList = tpHeroList->next;
	}
	UiSndPlayClick();
	SelListSetList(GetParent(focus), tpHeroList);
	UpdateStats(GetParent(focus), GetWindowLong(focus, GWL_ID));
	SelListUpdateSbar(GetParent(focus));
}


//****************************************************************************
//****************************************************************************
static LRESULT CALLBACK ListEntryWndProc (HWND   window,
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
						SelListShiftTab(window);
					}
					else {
						SelListTab(window);
					}
				break;

				case VK_UP:
				case VK_LEFT:
					SelListScrollUp(window);
				break;

				case VK_DOWN:
				case VK_RIGHT:
					SelListScrollDown(window);
				break;

				case VK_PRIOR:
					SelListPageUp(window);
				break;

				case VK_NEXT:
					SelListPageDown(window);
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
static void SelListSubclassEntries(HWND window) {
	HWND child;
	int *ListID;
	WNDPROC oldproc;

	for (ListID = sgListIDs; *ListID; ListID++) {
		child = GetDlgItem(window, *ListID);
		if (! child) continue;

		oldproc = (WNDPROC) GetWindowLong(child, GWL_WNDPROC);
		SetProp(child, OLDPROCPROP,(HANDLE) oldproc);
		SetWindowLong(child, GWL_WNDPROC, (LONG) ListEntryWndProc);
	}
}


//****************************************************************************
//****************************************************************************
static void SelListInit(HWND window) {
	TCHAR buf[32];
	HWND parent = GetParent(window);

	SelListSubclassEntries(window);

	// set the title for the parent
	if (SelHeroGetMode() == MULTIPLAYER)
		LoadString(global_hinstance, IDS_MULTILIST_TITLE, buf, 32-1);
	else
		LoadString(global_hinstance, IDS_SINGLIST_TITLE, buf, 32-1);
	SelHeroSetTitle(parent, buf);

	// point this window at the same bmp it's parent uses
	SetWindowLong(
		window,
		GWL_USERDATA,
		GetWindowLong(parent, GWL_USERDATA)
	);

	// set up a doom-like interface
	UiDoomStaticInit(window, sgTextIDs, AF_BIGGRAY);
	UiDoomButtonsInit(window, sgBtnIDs, AF_BIG, FALSE);
	UiDoomButtonsInit(window, sgDeleteIDs, AF_BIG, FALSE);
	UiDoomButtonsInit(window, sgListIDs, AF_MED);

	// set the names into the fake listbox
	sgFirstHero = GetHeroList();
	SelListSetList(window, sgFirstHero);

	// set up the animating focus indicator
	FocusAnimateInit("ui_art\\focus16.pcx");
	SDlgSetTimer(window, FOCUS_TIMER_ID, FOCUS_TIMER_DELAY, NULL);

	// init the scroll bar
	SbarInit(window, IDC_SCROLLBAR);
	if (GetNumHeros() <= MAX_VISIBLE) {
		ShowWindow(GetDlgItem(window, IDC_SCROLLBAR), FALSE);
	}
}


//****************************************************************************
//****************************************************************************
static void SelListAbort(HWND window, int ReturnVal) {
	UiSndPlayEnter();
	SDlgKillTimer(window, FOCUS_TIMER_ID);
	SDlgEndDialog(window, ReturnVal);
}


//****************************************************************************
//****************************************************************************
static void SelListInterpretClick(HWND window, int x, int y) {
	if (UiIsPtInWindow(window, GetDlgItem(window, IDC_FAKEOK), x, y)) {
		SelListAbort(window, IDOK);
	}
	else if (UiIsPtInWindow(window,	GetDlgItem(window, IDC_FAKECANCEL), x, y)) {
		SelListAbort(window, IDCANCEL);
	}
	else if (UiIsPtInWindow(window,	GetDlgItem(window, IDDELETE), x, y)) {
		if (strlen(SelectedName())) {
			SelListAbort(window, IDDELETE);
		}
	}
	else if (UiIsPtInWindow(window, GetDlgItem(window, IDC_SCROLLBAR), x, y)) {
		int SbarClick = SbarInterpretClick(GetDlgItem(window, IDC_SCROLLBAR), x, y);
		switch (SbarClick) {
			case DSB_UP:
				SelListScrollUp(GetFocus());
			break;

			case DSB_DOWN:
				SelListScrollDown(GetFocus());
			break;

			case DSB_PAGEUP:
				SelListPageUp(GetFocus());
			break;

			case DSB_PAGEDOWN:
				SelListPageDown(GetFocus());
			break;

			default:
			break;
		}
	}
}


//****************************************************************************
//****************************************************************************
BOOL CALLBACK SelListDialogProc(HWND window,
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
				SelListUpdateSbar(window);
			}
			else if (HIWORD(wparam) == BN_DOUBLECLICKED) {
				SelListAbort(window, IDOK);
			}
			else if (LOWORD(wparam) == IDOK) {
				SelListAbort(window, IDOK);
			}
			else if (LOWORD(wparam) == IDCANCEL) {
				SelListAbort(window, IDCANCEL);
			}
		break;

		case WM_KEYDOWN:
			if (wparam == VK_DELETE) {
				if (strlen(SelectedName())) {
					SelListAbort(window, IDDELETE);
				}
			}
		break;

		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
			SelListInterpretClick(window, LOWORD(lparam), HIWORD(lparam));
		break;

		case WM_LBUTTONUP:
			if (SbarUpClick(GetDlgItem(window, IDC_SCROLLBAR))) {
				SelListUpdateSbar(window);
			}
		break;

		case WM_DESTROY:
			SelListDestroy(window);
		break;

		case WM_INITDIALOG:
			SelListInit(window);
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

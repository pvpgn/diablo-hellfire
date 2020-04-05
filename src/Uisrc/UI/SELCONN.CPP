//****************************************************************************
//	SelConn.cpp
//	Diablo UI select connection method (in "doom-style" menu format)
//	
//	By Frank Pearce
//	created 10.22.96
//****************************************************************************


#include "pch.h"
#include "artfont.h"
#include "uisnd.h"


#define SB_ALWAYS_ON	0		// 0 in final


//****************************************************************************
//****************************************************************************
	extern void TitleLogoDestroy(void);
	extern void TitleLogoInit(HWND window);
	extern void TitleFramesInit(HWND window, LPCTSTR filename);
	extern void TitleLogoAnimate(HWND window);

	extern int SelOkDialog(HWND parent, LPCTSTR text, LPCTSTR title, BOOL loadfocus);

	extern BOOL UiSelectDevice(
		DWORD				providerid,
		SNETPROGRAMDATAPTR  programdata,
		SNETPLAYERDATAPTR   playerdata,
		SNETUIDATAPTR       interfacedata,
		SNETVERSIONDATAPTR  versiondata
	);

	extern void UiSetProviderID(DWORD id);
	extern void UiSetBasicInterface(BOOL basic);
	extern DWORD UiGetProviderID();

//****************************************************************************
//****************************************************************************
	#define OLDPROCPROP			TEXT("UIOLDPROC")

	#define MILLISEC_PER_SEC	1000

	#define FOCUS_TIMER_ID		1
	#define FOCUS_FPS			16		// frames per second
	#define FOCUS_TIMER_DELAY	55		//(MILLISEC_PER_SEC / FOCUS_FPS)

	#define LOGO_TIMER_ID		2
	#define LOGO_FPS			17
	#define LOGO_TIMER_DELAY	55

	#define MAX_VISIBLE			(IDC_CONNECTBTN6 - IDC_CONNECTBTN1 + 1)

	#define DESCRIPTION_LEN		SNETSPI_MAXSTRINGLENGTH
	#define REQUIREMENTS_LEN	SNETSPI_MAXSTRINGLENGTH

	typedef struct _provider TPROVIDERINFO;
	typedef TPROVIDERINFO *TPPROVIDERINFO;
	struct _provider {
		TPPROVIDERINFO next;
		BOOL  isbasicinterface;
		DWORD id;								// provider id
		DWORD players;							// players supported
		TCHAR description[DESCRIPTION_LEN];		// provider name
		TCHAR requirements[REQUIREMENTS_LEN];	// provider info
	};


//****************************************************************************
//****************************************************************************
	static int sgBigTextIDs[] = {
		IDC_DLGTITLE,
		IDC_DLGTITLE2,
		0
	};
	static int sgSmallTextIDs[] = {
		IDC_CONNECTTEXT,
		IDC_PLAYERSTEXT,
		0
	};
	static int sgLblTextIDs[] = {
		IDC_REQTITLE,
		0
	};
	static int sgBtnIDs[] = {
		IDC_FAKEOK,
		IDC_FAKECANCEL,
		0
	};
	static int sgListIDs[] = {
		IDC_CONNECTBTN1,
		IDC_CONNECTBTN2,
		IDC_CONNECTBTN3,
		IDC_CONNECTBTN4,
		IDC_CONNECTBTN5,
		IDC_CONNECTBTN6,
		0
	};

	static TPPROVIDERINFO sgProviderList;
	static int sgProvListLen;

	static SNETCAPSPTR         sgmincaps;
	static SNETPROGRAMDATAPTR  sgprogramdata;
	static SNETPLAYERDATAPTR   sgplayerdata;
	static SNETUIDATAPTR       sginterfacedata;
	static SNETVERSIONDATAPTR  sgversiondata;

	static BOOL	sgRequireUpgrade;


//****************************************************************************
//****************************************************************************
TPPROVIDERINFO SelConnAllocNode(void) {
	return (TPPROVIDERINFO) ALLOC(sizeof(TPROVIDERINFO));
}
//****************************************************************************
//****************************************************************************
static void SelConnFreeNode(TPPROVIDERINFO node) {
	if (node) {
		FREE(node);
	}
}
//****************************************************************************
//****************************************************************************
static void SelConnFreeList(TPPROVIDERINFO list) {
	TPPROVIDERINFO next;

	while (list) {
		next = list->next;
		SelConnFreeNode(list);
		list = next;
	}
}
//****************************************************************************
//****************************************************************************
TPPROVIDERINFO SelConnAddNode(TPPROVIDERINFO list, TPPROVIDERINFO node) {
	node->next = list;
	return node;
}


//****************************************************************************
//****************************************************************************
static  BOOL CALLBACK SelConnEnum(DWORD id,
								  LPCSTR description,
								  LPCSTR requirements,
								  SNETCAPSPTR caps) {
	TPPROVIDERINFO newprovider = SelConnAllocNode();
	if (! newprovider) return 0;
	newprovider->next = NULL;
	newprovider->isbasicinterface = caps->flags & SNET_CAPS_BASICINTERFACE;
	newprovider->id = id;
	newprovider->players = caps->maxplayers;
	strcpy(newprovider->description, description);
	strcpy(newprovider->requirements, requirements);
	sgProviderList = SelConnAddNode(sgProviderList, newprovider);
	sgProvListLen++;
	return 1;
}


//****************************************************************************
//****************************************************************************
static void SelConnSetInfo(HWND window, int childid) {
	TCHAR format[64];
	TCHAR supported[72];
	TPPROVIDERINFO tpProvider;
	HWND child;
	TPBMP tpBmp;
	
	child = GetDlgItem(window, childid);
	if (! child) return;

	tpBmp = (TPBMP) GetWindowLong(child, GWL_USERDATA);
	if (! tpBmp) return;

	tpProvider = (TPPROVIDERINFO) tpBmp->userdata;
	if (! tpProvider) return;

	child = GetDlgItem(window, IDC_CONNECTTEXT);
	if (! child) return;
	UiSetBmpText((TPBMP)GetWindowLong(child, GWL_USERDATA), tpProvider->requirements);

	child = GetDlgItem(window, IDC_PLAYERSTEXT);
	if (! child) return;
	LoadString(global_hinstance, IDS_PLAYERSSUPPORTED, format, 63);
	if (sgprogramdata)
		wsprintf(supported, format, min(sgprogramdata->maxplayers, tpProvider->players));
	else
		wsprintf(supported, format, tpProvider->players);
	UiSetBmpText((TPBMP)GetWindowLong(child, GWL_USERDATA), supported);

	UiDoomStaticReset(window, sgSmallTextIDs, AF_SMALLGRAY);
}


//****************************************************************************
//****************************************************************************
static void SelConnSetProviders(HWND window, TPPROVIDERINFO provider) {
	HWND child;
	TPBMP tpBmp;
	int * ctrlid = sgListIDs;

	for (; *ctrlid; ctrlid++) {
		child = GetDlgItem(window, *ctrlid);
		if (! child) continue;
		if (! provider) {
			EnableWindow(child, FALSE);
			continue;
		}
		
		EnableWindow(child, TRUE);
		tpBmp = (TPBMP) GetWindowLong(child, GWL_USERDATA);
		if (! tpBmp) continue;

		tpBmp->userdata = (LONG) provider;
		UiSetBmpText(tpBmp, provider->description);

		provider = provider->next;
	}

	UiDoomButtonsReset(window, sgListIDs, AF_SMALL);
}


//****************************************************************************
//****************************************************************************
static int SelConnGetFocusNum(void) {
	HWND btn = GetFocus();
	if (! btn) return 0;
	TPBMP tpBmp = (TPBMP) GetWindowLong(btn, GWL_USERDATA);
	if (! tpBmp) return 0;
	TPPROVIDERINFO list = sgProviderList;
	if (! list) return 0;
	TPPROVIDERINFO node = (TPPROVIDERINFO) tpBmp->userdata;
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
static void SelConnUpdateSbar(HWND window) {
	SbarDraw(window, IDC_SCROLLBAR, sgProvListLen, SelConnGetFocusNum());
}


//****************************************************************************
//****************************************************************************
static void SelConnTab(HWND child) {
	HWND parent, newfocus;
	int index;
	int NewID[] = {
		IDC_CONNECTBTN2,
		IDC_CONNECTBTN3,
		IDC_CONNECTBTN4,
		IDC_CONNECTBTN5,
		IDC_CONNECTBTN6,
		IDC_CONNECTBTN1
	};

	parent = GetParent(child);
	newfocus = child;
	do {
		index = GetWindowLong(newfocus, GWL_ID) - IDC_CONNECTBTN1;
		newfocus = GetDlgItem(parent, NewID[index]);
	} while (! IsWindowEnabled(newfocus));
	SetFocus(newfocus);
}


//****************************************************************************
//****************************************************************************
static void SelConnShiftTab(HWND child) {
	HWND parent, newfocus;
	int index;
	int NewID[] = {
		IDC_CONNECTBTN6,
		IDC_CONNECTBTN1,
		IDC_CONNECTBTN2,
		IDC_CONNECTBTN3,
		IDC_CONNECTBTN4,
		IDC_CONNECTBTN5
	};

	parent = GetParent(child);
	newfocus = child;
	do {
		index = GetWindowLong(newfocus, GWL_ID) - IDC_CONNECTBTN1;
		newfocus = GetDlgItem(parent, NewID[index]);
	} while (! IsWindowEnabled(newfocus));
	SetFocus(newfocus);
}


//****************************************************************************
//****************************************************************************
static int GetNumProvNode(HWND btn) {
	if (! btn) return 0;
	TPBMP tpBmp = (TPBMP) GetWindowLong(btn, GWL_USERDATA);
	if (! tpBmp) return 0;
	TPPROVIDERINFO list = sgProviderList;
	if (! list) return 0;
	TPPROVIDERINFO node = (TPPROVIDERINFO) tpBmp->userdata;
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
static TPPROVIDERINFO GetProvider(int index) {
	TPPROVIDERINFO list = sgProviderList;
	
	while (list && index) {
		list = list->next;
		index--;
	}
	return list;
}


//****************************************************************************
//****************************************************************************
static void SelConnPageDown(HWND focus) {
	HWND parent = GetParent(focus);
	if (! parent) return;

	// get info of 1st btn
	HWND btn1 = GetDlgItem(parent, IDC_CONNECTBTN1);
	if (! btn1) return;

	// see if there is info below the bottom
	TPBMP tpBmp = (TPBMP) GetWindowLong(GetDlgItem(parent, IDC_CONNECTBTN6), GWL_USERDATA);
	if (! tpBmp) return;
	TPPROVIDERINFO tpProvider = (TPPROVIDERINFO) tpBmp->userdata;
	if ((! tpProvider) || (! tpProvider->next)) {
		SelConnShiftTab(btn1);
		return;
	}

	// calc the index of the new top
	int newtop = GetNumProvNode(btn1) + MAX_VISIBLE;
	if (newtop > (sgProvListLen - MAX_VISIBLE))
		newtop = sgProvListLen - MAX_VISIBLE;

	// get the pointer to that index
	tpProvider = GetProvider(newtop);
	if (! tpProvider) return;

	// update everything on the screen
	UiSndPlayClick();
	SelConnSetProviders(parent, tpProvider);
	SelConnSetInfo(parent, GetWindowLong(focus, GWL_ID));
	SelConnUpdateSbar(parent);
}


//****************************************************************************
//****************************************************************************
static void SelConnPageUp(HWND focus) {
	HWND parent = GetParent(focus);
	if (! parent) return;
	HWND btn1 = GetDlgItem(parent, IDC_CONNECTBTN1);
	if (! btn1) return;

	// see if there is info off the top
	TPBMP tpBmp = (TPBMP) GetWindowLong(btn1, GWL_USERDATA);
	if (! tpBmp) return;
	TPPROVIDERINFO tpProvider = (TPPROVIDERINFO) tpBmp->userdata;
	if (! tpProvider) return;
	if (tpProvider == sgProviderList) {
		SelConnTab(GetDlgItem(parent, IDC_CONNECTBTN6));
		return;
	}

	// calc the index of the new top
	int newtop = GetNumProvNode(btn1) - MAX_VISIBLE;
	if (newtop < 0)
		newtop = 0;

	// get the pointer to that index
	tpProvider = GetProvider(newtop);
	if (! tpProvider) return;

	// update everything on the screen
	UiSndPlayClick();
	SelConnSetProviders(parent, tpProvider);
	SelConnSetInfo(parent, GetWindowLong(focus, GWL_ID));
	SelConnUpdateSbar(parent);
}


//****************************************************************************
//****************************************************************************
static void SelConnScrollDown(HWND focus) {
	// see if there is info below the focus
	TPBMP tpBmp = (TPBMP) GetWindowLong(focus, GWL_USERDATA);
	if (! tpBmp) return;
	TPPROVIDERINFO tpProvider = (TPPROVIDERINFO) tpBmp->userdata;
	if (! tpProvider) return;
	if (! tpProvider->next) return;

	if (GetWindowLong(focus, GWL_ID) < IDC_CONNECTBTN6) {
		// handle it just like a tab
		SelConnTab(focus);
		return;
	}

	// advance the list
	HWND child = GetDlgItem(GetParent(focus), IDC_CONNECTBTN2);
	if (! child) return;
	tpBmp = (TPBMP) GetWindowLong(child, GWL_USERDATA);
	if (! tpBmp) return;
	tpProvider = (TPPROVIDERINFO) tpBmp->userdata;
	if (! tpProvider) return;
	UiSndPlayClick();
	SelConnSetProviders(GetParent(focus), tpProvider);
	SelConnSetInfo(GetParent(focus), GetWindowLong(focus, GWL_ID));
	SelConnUpdateSbar(GetParent(focus));
}


//****************************************************************************
//****************************************************************************
static void SelConnScrollUp(HWND focus) {
	if (GetWindowLong(focus, GWL_ID) > IDC_CONNECTBTN1) {
		// handle it just like a shift-tab
		SelConnShiftTab(focus);
		return;
	}

	// see if there is info off the top
	TPBMP tpBmp = (TPBMP) GetWindowLong(focus, GWL_USERDATA);
	if (! tpBmp) return;
	TPPROVIDERINFO tpProvider = (TPPROVIDERINFO) tpBmp->userdata;
	if (! tpProvider) return;
	TPPROVIDERINFO tpProviderList = sgProviderList;
	if (tpProvider == tpProviderList) return;

	// retreat the list
	while (tpProviderList && (tpProviderList->next != tpProvider)) {
		tpProviderList = tpProviderList->next;
	}
	UiSndPlayClick();
	SelConnSetProviders(GetParent(focus), tpProviderList);
	SelConnSetInfo(GetParent(focus), GetWindowLong(focus, GWL_ID));
	SelConnUpdateSbar(GetParent(focus));
}


//****************************************************************************
//****************************************************************************
static void SelConnDestroy(HWND window) {
	SbarDestroy(window, IDC_SCROLLBAR);
	SelConnFreeList(sgProviderList);

	UiDoomCtrlsDestroy(window, sgListIDs);
	UiDoomCtrlsDestroy(window, sgBtnIDs);
	UiDoomCtrlsDestroy(window, sgBigTextIDs);
	UiDoomCtrlsDestroy(window, sgLblTextIDs);
	UiDoomCtrlsDestroy(window, sgSmallTextIDs);

	// free the background of the dlg
	UiFreeBmp((TPBMP) GetWindowLong(window, GWL_USERDATA));
	SetWindowLong(window, GWL_USERDATA, (LONG) 0);

	TitleLogoDestroy();
	FocusAnimateDestroy();
}


//****************************************************************************
//****************************************************************************
static LRESULT CALLBACK SelConnListEntryWndProc (HWND   window,
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
						SelConnShiftTab(window);
					}
					else {
						SelConnTab(window);
					}
				break;
				case VK_UP:
				case VK_LEFT:
					SelConnScrollUp(window);
				break;
				case VK_DOWN:
				case VK_RIGHT:
					SelConnScrollDown(window);
				break;
				case VK_PRIOR:
					SelConnPageUp(window);
				break;
				case VK_NEXT:
					SelConnPageDown(window);
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
static void SelConnSubclassEntries(HWND window) {
	HWND child;
	int *ListID;
	WNDPROC oldproc;

	for (ListID = sgListIDs; *ListID; ListID++) {
		child = GetDlgItem(window, *ListID);
		if (! child) continue;

		oldproc = (WNDPROC) GetWindowLong(child, GWL_WNDPROC);
		SetProp(child, OLDPROCPROP,(HANDLE) oldproc);
		SetWindowLong(child, GWL_WNDPROC, (LONG) SelConnListEntryWndProc);
	}
}


//****************************************************************************
//****************************************************************************
static void SelConnInit(HWND window) {
	TPBMP tpBmp;

	SelConnSubclassEntries(window);

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
			TEXT("ui_art\\selconn.pcx"),
			&tpBmp->data,
			&tpBmp->datasize,
			FALSE
		);
		UiFadeInit(window);
	}

	// set up a doom-like interface
	UiDoomStaticInit(window, sgSmallTextIDs, AF_SMALLGRAY);
	UiDoomStaticInit(window, sgLblTextIDs, AF_SMALLGRAY);
	UiDoomStaticInit(window, sgBigTextIDs, AF_BIGGRAY);
	UiDoomButtonsInit(window, sgBtnIDs, AF_BIG, FALSE);
	UiDoomButtonsInit(window, sgListIDs, AF_SMALL);

	// get provider list
	sgProvListLen = 0;
	sgProviderList = NULL;
	SNetEnumProviders(NULL, SelConnEnum);

	// set provider buttons to contain text
	SelConnSetProviders(window, sgProviderList);

	// init the scroll bar
	SbarInit(window, IDC_SCROLLBAR);
#if SB_ALWAYS_ON != 1
	if (sgProvListLen <= MAX_VISIBLE) {
		ShowWindow(GetDlgItem(window, IDC_SCROLLBAR), FALSE);
	}
#endif
}


//****************************************************************************
//****************************************************************************
static void SelConnAbort(HWND window, int ReturnVal) {
	if (ReturnVal == IDCANCEL)
		UiSndPlayEnter();
	UiFadeAbort(window);
	SDlgKillTimer(window, LOGO_TIMER_ID);
	SDlgKillTimer(window, FOCUS_TIMER_ID);
	UiVidFadeOut(DEFAULT_STEPS*2);
	SDlgEndDialog(window, ReturnVal);
}


//****************************************************************************
//****************************************************************************
static BOOL SelConnInitProvider(HWND window) {
	TPPROVIDERINFO tpProvider;
	SNETUIDATA tUiData;

	UiSndPlayEnter();
	UiSetProviderID(0);
	UiSetBasicInterface(FALSE);

	TPBMP tpBmp = (TPBMP) GetWindowLong(GetFocus(), GWL_USERDATA);
	if (! tpBmp)
		return FALSE;

	tpProvider = (TPPROVIDERINFO) tpBmp->userdata;
	if (! tpProvider)
		return FALSE;
	UiSetProviderID(tpProvider->id);
	UiSetBasicInterface(tpProvider->isbasicinterface);

	if (sginterfacedata)
		tUiData = *sginterfacedata;
	tUiData.parentwindow = window;

	// Hide cursor while inside Initialize() routine. 
	// That way the .snp can take control of the cursor and do whatever it wants.
	UiClearCursor();
	BOOL bResult = SNetInitializeProvider(
			UiGetProviderID(),
			sgprogramdata,
			sgplayerdata,
			&tUiData,
			sgversiondata);
	UiSetCursor(); 

	if (bResult) {
		return UiSelectDevice(
			UiGetProviderID(),
			sgprogramdata,
			sgplayerdata,
			&tUiData,
			sgversiondata
		);
	}
	else {
		return FALSE;
	}
}


//****************************************************************************
//****************************************************************************
static void SelConnInitError(HWND window) {
	switch (GetLastError()) {
	   	case SNET_ERROR_CANCELLED:
	   		// do nothing if the user cancelled during connect
	   	break;
	   	default:
	   		// error initializing connection method
	   		TCHAR errstr[128];
	   		LoadString(global_hinstance, IDS_SELCONN_ERR, errstr, 127);
	   		SelOkDialog(window, errstr, NULL, FALSE);
	   	break;
	}
}


//****************************************************************************
//****************************************************************************
static void SelConnAttemptInit(HWND window) {
	if (SelConnInitProvider(window)) {
		SelConnAbort(window, IDOK);
	}
	else if (GetLastError() == SNET_ERROR_REQUIRES_UPGRADE) {
		sgRequireUpgrade = TRUE;
		SelConnAbort(window, 0);
	}
	else {
		switch (UiGetProviderID()) {
			case 'MODM':
			case 'IPXN':
			case 'SCBL':
				SelConnInitError(window);
			break;

			default:
				// do nothing, and hope the provider notified the user of the error
			break;
		}
	}
}


//****************************************************************************
//****************************************************************************
static void SelConnInterpretClick(HWND window, int x, int y) {
	if (UiIsPtInWindow(window, GetDlgItem(window, IDC_FAKEOK), x, y)) {
		SelConnAttemptInit(window);
	}
	else if (UiIsPtInWindow(window, GetDlgItem(window, IDC_FAKECANCEL), x, y)) {
		SelConnAbort(window, IDCANCEL);
	}
	else if (UiIsPtInWindow(window, GetDlgItem(window, IDC_SCROLLBAR), x, y)) {
		int SbarClick = SbarInterpretClick(GetDlgItem(window, IDC_SCROLLBAR), x, y);
		switch (SbarClick) {
			case DSB_UP:
				SelConnScrollUp(GetFocus());
			break;
			case DSB_PAGEUP:
				SelConnPageUp(GetFocus());
			break;
			case DSB_DOWN:
				SelConnScrollDown(GetFocus());
			break;
			case DSB_PAGEDOWN:
				SelConnPageDown(GetFocus());
			break;
			default:
			break;
		}
	}
}


//****************************************************************************
//****************************************************************************
BOOL CALLBACK SelConnDialogProc(HWND window,
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
				SelConnSetInfo(window, LOWORD(wparam));
				SelConnUpdateSbar(window);
			}
			else if (HIWORD(wparam) == BN_DOUBLECLICKED) {
				SelConnAttemptInit(window);
			}
			else if (LOWORD(wparam) == IDOK) {
				SelConnAttemptInit(window);
			}
			else if (LOWORD(wparam) == IDCANCEL) {
				SelConnAbort(window, IDCANCEL);
			}
		break;

		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
			SelConnInterpretClick(window, LOWORD(lparam), HIWORD(lparam));
		break;

		case WM_LBUTTONUP:
			if (SbarUpClick(GetDlgItem(window, IDC_SCROLLBAR))) {
				SelConnUpdateSbar(window);
			}
		break;

		case WM_DESTROY:
			SelConnDestroy(window);
		break;

		case WM_INITDIALOG:
			SelConnInit(window);
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
BOOL APIENTRY UiSelectProvider (SNETCAPSPTR         mincaps,
                                SNETPROGRAMDATAPTR  programdata,
                                SNETPLAYERDATAPTR   playerdata,
                                SNETUIDATAPTR       interfacedata,
                                SNETVERSIONDATAPTR  versiondata,
                                DWORD              *providerid) {

	DWORD result = 0;

	sgmincaps		= mincaps;
	sgprogramdata	= programdata;
	sgplayerdata	= playerdata;
	sginterfacedata	= interfacedata;
	sgversiondata	= versiondata;
	sgRequireUpgrade = FALSE;

	// make sure fonts are loaded
	ArtFontLoad();

	// DISPLAY THE DIALOG BOX
	result = (DWORD)SDlgDialogBox(global_hinstance,
									  TEXT("SELCONNECT_DIALOG"),
									  SDrawGetFrameWindow(),
									  SelConnDialogProc);


	if (result == IDOK) {
		ArtFontDestroy();
	}


	if (providerid)
		*providerid = UiGetProviderID();

	if (sgRequireUpgrade) {
		SetLastError(SNET_ERROR_REQUIRES_UPGRADE);
		return FALSE;
	}
	else if (result == IDOK) {
		return TRUE;
	}
	else {
		SetLastError(SNET_ERROR_CANCELLED);
		return FALSE;
	}
}

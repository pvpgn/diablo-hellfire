//****************************************************************************
//	SelModem.cpp
//	Diablo UI select modem from a list of modems
//	
//	By Frank Pearce
//	created 11.22.96
//****************************************************************************


#include "pch.h"
#include "artfont.h"
#include "uisnd.h"


//****************************************************************************
//****************************************************************************
	extern int SelOkDialog(HWND parent, LPCTSTR text, LPCTSTR title, BOOL loadfocus);
	extern BOOL CALLBACK ModmStatStatusCallback(
		LPCSTR		statstr, 
		DWORD			completed, 
		DWORD			remaining,
		DWORD			flags,
		SNETABORTPROC	abortfcn
	);


//****************************************************************************
//****************************************************************************
	#define OLDPROCPROP			TEXT("UIOLDPROC")

	#define MILLISEC_PER_SEC	1000

	#define FOCUS_TIMER_ID		1
	#define FOCUS_FPS			16		// frames per second
	#define FOCUS_TIMER_DELAY	55		//(MILLISEC_PER_SEC / FOCUS_FPS)

	#define MAX_VISIBLE			(IDC_MODEMBTN6 - IDC_MODEMBTN1 + 1)


//****************************************************************************
//****************************************************************************
	static TPMODEM sgModemList;
	static int sgNumModems;
	static DWORD sgDeviceID;

	static SNETPROGRAMDATAPTR  sgSMProgramData;
	static SNETPLAYERDATAPTR   sgSMPlayerData;
	static SNETUIDATAPTR       sgSMInterfaceData;
	static SNETVERSIONDATAPTR  sgSMVersionData;

	static int sgTextIDs[] = {
		IDC_DLGTITLE,
		0
	};
	static int sgText2IDs[] = {
		IDC_DLGTITLE2,
		IDC_DLGTITLE3,
		0
	};
	static int sgDescID[] = {
		IDC_REQDESC,
		0
	};
	static int sgBtnIDs[] = {
		IDC_FAKEOK,
		IDC_FAKECANCEL,
		0
	};
	static int sgListIDs[] = {
		IDC_MODEMBTN1,
		IDC_MODEMBTN2,
		IDC_MODEMBTN3,
		IDC_MODEMBTN4,
		IDC_MODEMBTN5,
		IDC_MODEMBTN6,
		0
	};


//****************************************************************************
//****************************************************************************
static TPMODEM ModemAllocNode(void) {
	return (TPMODEM) ALLOC(sizeof(TMODEM));
}
//****************************************************************************
//****************************************************************************
static void ModemFreeNode(TPMODEM node) {
	if (node) FREE(node);
}
//****************************************************************************
//****************************************************************************
static void ModemFreeList(TPMODEM list) {
	TPMODEM next;

	while (list) {
		next = list->next;
		ModemFreeNode(list);
		list = next;
	}
}
//****************************************************************************
//****************************************************************************
static TPMODEM ModemAddNode(TPMODEM list, TPMODEM node) {
	node->next = list;
	return node;
}
//****************************************************************************
//****************************************************************************
static TPMODEM ModemFindNode(TPMODEM list, DWORD deviceid) {
	while (list) {
		if (list->deviceid == deviceid)
			break;
		list = list->next;
	}
	return list;
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK SelModemEnum(DWORD  deviceid,
							   LPCSTR devicename,
							   LPCSTR devicedesc) {
	TPMODEM modem;

	modem = ModemAllocNode();
	if (! modem) return FALSE;
	modem->next = NULL;
	modem->deviceid = deviceid;
	strcpy(modem->devicename, devicename);
	strcpy(modem->devicedesc, devicedesc);
	sgModemList = ModemAddNode(sgModemList, modem);
	sgNumModems++;
	return TRUE;
}


//****************************************************************************
//****************************************************************************
static void SelModemDestroy(HWND window) {
	SbarDestroy(window, IDC_SCROLLBAR);
	UiDoomCtrlsDestroy(window, sgListIDs);
	UiDoomCtrlsDestroy(window, sgBtnIDs);
	UiDoomCtrlsDestroy(window, sgTextIDs);
	UiDoomCtrlsDestroy(window, sgText2IDs);
	UiDoomCtrlsDestroy(window, sgDescID);

	// free the background of the dlg
	UiFreeBmp((TPBMP) GetWindowLong(window, GWL_USERDATA));
	SetWindowLong(window, GWL_USERDATA, (LONG) 0);
}


//****************************************************************************
//****************************************************************************
static void SelModemSetList(HWND window, TPMODEM firstmodem) {
	HWND child;
	TPBMP tpBmp;
	int *ListID;

	for (ListID = sgListIDs; *ListID; ListID++) {
		child = GetDlgItem(window, *ListID);
		if (! child)
			continue;

		if (! firstmodem) {
			EnableWindow(child, FALSE);
			continue;
		}

		EnableWindow(child, TRUE);
		tpBmp = (TPBMP) GetWindowLong(child, GWL_USERDATA);
		UiSetBmpText(tpBmp, firstmodem->devicename);
		if (tpBmp)
			tpBmp->userdata = (LONG) firstmodem;

		firstmodem = firstmodem->next;
	}

	UiDoomButtonsReset(window, sgListIDs, AF_SMALL);
}


//****************************************************************************
//****************************************************************************
static void SelModemTab(HWND child) {
	HWND parent, newfocus;
	int index;
	int NewID[] = {
		IDC_MODEMBTN2,
		IDC_MODEMBTN3,
		IDC_MODEMBTN4,
		IDC_MODEMBTN5,
		IDC_MODEMBTN6,
		IDC_MODEMBTN1
	};

	parent = GetParent(child);
	newfocus = child;
	do {
		index = GetWindowLong(newfocus, GWL_ID) - IDC_MODEMBTN1;
		newfocus = GetDlgItem(parent, NewID[index]);
	} while (! IsWindowEnabled(newfocus));
	SetFocus(newfocus);
}


//****************************************************************************
//****************************************************************************
static void SelModemShiftTab(HWND child) {
	HWND parent, newfocus;
	int index;
	int NewID[] = {
		IDC_MODEMBTN6,
		IDC_MODEMBTN1,
		IDC_MODEMBTN2,
		IDC_MODEMBTN3,
		IDC_MODEMBTN4,
		IDC_MODEMBTN5
	};

	parent = GetParent(child);
	newfocus = child;
	do {
		index = GetWindowLong(newfocus, GWL_ID) - IDC_MODEMBTN1;
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
	TPMODEM list = sgModemList;
	if (! list) return 0;
	TPMODEM node = (TPMODEM) tpBmp->userdata;
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
static TPMODEM GetModem(int index) {
	TPMODEM list = sgModemList;
	
	while (list && index) {
		list = list->next;
		index--;
	}
	return list;
}


//****************************************************************************
//****************************************************************************
static void SelModemUpdateSbar(HWND window) {
	SbarDraw(window, IDC_SCROLLBAR, sgNumModems, GetNumNode(GetFocus()));
}


//****************************************************************************
//****************************************************************************
static void SelModemPageDown(HWND focus) {
	HWND parent = GetParent(focus);
	if (! parent) return;

	// get info of 1st btn
	HWND btn1 = GetDlgItem(parent, IDC_MODEMBTN1);
	if (! btn1) return;

	// see if there is info below the bottom
	TPBMP tpBmp = (TPBMP) GetWindowLong(GetDlgItem(parent, IDC_MODEMBTN6), GWL_USERDATA);
	if (! tpBmp) return;
	TPMODEM tpModem = (TPMODEM) tpBmp->userdata;
	if ((! tpModem) || (! tpModem->next)) {
		SelModemShiftTab(btn1);
		return;
	}

	// calc the index of the new top
	int newtop = GetNumNode(btn1) + MAX_VISIBLE;
	if (newtop > (sgNumModems - MAX_VISIBLE))
		newtop = sgNumModems - MAX_VISIBLE;

	// get the pointer to that index
	tpModem = GetModem(newtop);
	if (! tpModem) return;

	// update everything on the screen
	UiSndPlayClick();
	SelModemSetList(parent, tpModem);
	SelModemUpdateSbar(parent);
}


//****************************************************************************
//****************************************************************************
static void SelModemPageUp(HWND focus) {
	HWND parent = GetParent(focus);
	if (! parent) return;
	HWND btn1 = GetDlgItem(parent, IDC_MODEMBTN1);
	if (! btn1) return;

	// see if there is info off the top
	TPBMP tpBmp = (TPBMP) GetWindowLong(btn1, GWL_USERDATA);
	if (! tpBmp) return;
	TPMODEM tpModem = (TPMODEM) tpBmp->userdata;
	if (! tpModem) return;
	if (tpModem == sgModemList) {
		SelModemTab(GetDlgItem(parent, IDC_MODEMBTN6));
		return;
	}

	// calc the index of the new top
	int newtop = GetNumNode(btn1) - MAX_VISIBLE;
	if (newtop < 0)
		newtop = 0;

	// get the pointer to that index
	tpModem = GetModem(newtop);
	if (! tpModem) return;

	// update everything on the screen
	UiSndPlayClick();
	SelModemSetList(parent, tpModem);
	SelModemUpdateSbar(parent);
}


//****************************************************************************
//****************************************************************************
static void SelModemScrollDown(HWND focus) {
	// see if there is info below the focus
	TPBMP tpBmp = (TPBMP) GetWindowLong(focus, GWL_USERDATA);
	if (! tpBmp) return;
	TPMODEM tpModem = (TPMODEM) tpBmp->userdata;
	if (! tpModem) return;
	if (! tpModem->next) return;

	if (GetWindowLong(focus, GWL_ID) < IDC_MODEMBTN6) {
		// handle it just like a tab
		SelModemTab(focus);
		return;
	}

	// advance the list
	HWND child = GetDlgItem(GetParent(focus), IDC_MODEMBTN2);
	if (! child) return;
	tpBmp = (TPBMP) GetWindowLong(child, GWL_USERDATA);
	if (! tpBmp) return;
	tpModem = (TPMODEM) tpBmp->userdata;
	if (! tpModem) return;
	UiSndPlayClick();
	SelModemSetList(GetParent(focus), tpModem);
	SelModemUpdateSbar(GetParent(focus));
}


//****************************************************************************
//****************************************************************************
static void SelModemScrollUp(HWND focus) {
	if (GetWindowLong(focus, GWL_ID) > IDC_MODEMBTN1) {
		// handle it just like a shift-tab
		SelModemShiftTab(focus);
		return;
	}

	// see if there is info off the top
	TPBMP tpBmp = (TPBMP) GetWindowLong(focus, GWL_USERDATA);
	if (! tpBmp) return;
	TPMODEM tpModem = (TPMODEM) tpBmp->userdata;
	TPMODEM tpModemList;
	if (! tpModem) return;
	if (tpModem == (tpModemList = sgModemList)) return;

	// retreat the list
	while (tpModemList && (tpModemList->next != tpModem)) {
		tpModemList = tpModemList->next;
	}
	UiSndPlayClick();
	SelModemSetList(GetParent(focus), tpModemList);
	SelModemUpdateSbar(GetParent(focus));
}


//****************************************************************************
//****************************************************************************
static LRESULT CALLBACK ModemListWndProc (HWND   window,
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
						SelModemShiftTab(window);
					}
					else {
						SelModemTab(window);
					}
				break;

				case VK_UP:
				case VK_LEFT:
					SelModemScrollUp(window);
				break;

				case VK_DOWN:
				case VK_RIGHT:
					SelModemScrollDown(window);
				break;

				case VK_PRIOR:
					SelModemPageUp(window);
				break;

				case VK_NEXT:
					SelModemPageDown(window);
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
static void SelModemSubclassEntries(HWND window) {
	HWND child;
	int *ListID;
	WNDPROC oldproc;

	for (ListID = sgListIDs; *ListID; ListID++) {
		child = GetDlgItem(window, *ListID);
		if (! child) continue;

		oldproc = (WNDPROC) GetWindowLong(child, GWL_WNDPROC);
		SetProp(child, OLDPROCPROP,(HANDLE) oldproc);
		SetWindowLong(child, GWL_WNDPROC, (LONG) ModemListWndProc);
	}
}


//****************************************************************************
//****************************************************************************
static void SelModemInit(HWND window) {
	HWND parent = GetParent(window);

	SelModemSubclassEntries(window);

	// load texture maps for this dialog
	TPBMP tpBmp = UiAllocBmp();
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


	// set up a doom-like interface
	UiDoomStaticInit(window, sgDescID, AF_SMALLGRAY);
	UiDoomStaticInit(window, sgText2IDs, AF_MEDGRAY);
	UiDoomStaticInit(window, sgTextIDs, AF_BIGGRAY);
	UiDoomButtonsInit(window, sgBtnIDs, AF_BIG, FALSE);
	UiDoomButtonsInit(window, sgListIDs, AF_SMALL);

	// set the names into the fake listbox
	SelModemSetList(window, sgModemList);

	// set up the animating focus indicator
	SDlgSetTimer(window, FOCUS_TIMER_ID, FOCUS_TIMER_DELAY, NULL);

	// init the scroll bar
	SbarInit(window, IDC_SCROLLBAR);
	if (sgNumModems <= MAX_VISIBLE) {
		ShowWindow(GetDlgItem(window, IDC_SCROLLBAR), FALSE);
	}
}


//****************************************************************************
//****************************************************************************
static void SelModemAbort(HWND window, int ReturnVal) {
	UiSndPlayEnter();
	SDlgKillTimer(window, FOCUS_TIMER_ID);

	if (ReturnVal == IDOK) {
		// get the id of the selected device
		HWND focus = GetFocus();
		if (focus) {
			TPBMP tpBmp = (TPBMP) GetWindowLong(focus, GWL_USERDATA);
			if (tpBmp) {
				TPMODEM tpModem = (TPMODEM) tpBmp->userdata;
				if (tpModem) {
					sgDeviceID = tpModem->deviceid;
				}
			}
		}
	}

	SDlgEndDialog(window, ReturnVal);
}


//****************************************************************************
//****************************************************************************
static void SelModemInterpretClick(HWND window, int x, int y) {
	if (UiIsPtInWindow(window, GetDlgItem(window, IDC_FAKEOK), x, y)) {
		SelModemAbort(window, IDOK);
	}
	else if (UiIsPtInWindow(window,	GetDlgItem(window, IDC_FAKECANCEL), x, y)) {
		SelModemAbort(window, IDCANCEL);
	}
	else if (UiIsPtInWindow(window, GetDlgItem(window, IDC_SCROLLBAR), x, y)) {
		int SbarClick = SbarInterpretClick(GetDlgItem(window, IDC_SCROLLBAR), x, y);
		switch (SbarClick) {
			case DSB_UP:
				SelModemScrollUp(GetFocus());
			break;

			case DSB_DOWN:
				SelModemScrollDown(GetFocus());
			break;

			case DSB_PAGEUP:
				SelModemPageUp(GetFocus());
			break;

			case DSB_PAGEDOWN:
				SelModemPageDown(GetFocus());
			break;

			default:
			break;
		}
	}
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK SelModemDialogProc(HWND window,
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
				SelModemUpdateSbar(window);
			}
			else if (HIWORD(wparam) == BN_DOUBLECLICKED) {
				SelModemAbort(window, IDOK);
			}
			else if (LOWORD(wparam) == IDOK) {
				SelModemAbort(window, IDOK);
			}
			else if (LOWORD(wparam) == IDCANCEL) {
				SelModemAbort(window, IDCANCEL);
			}
		break;

		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
			SelModemInterpretClick(window, LOWORD(lparam), HIWORD(lparam));
		break;

		case WM_LBUTTONUP:
			if (SbarUpClick(GetDlgItem(window, IDC_SCROLLBAR))) {
				SelModemUpdateSbar(window);
			}
		break;

		case WM_DESTROY:
			SelModemDestroy(window);
		break;

		case WM_INITDIALOG:
			SelModemInit(window);
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


//****************************************************************************
//****************************************************************************
static BOOL SelModemInitDevice(HWND window, DWORD deviceid) {
	SNETUIDATA uidata;

	// create new interface data
	CopyMemory(&uidata,sgSMInterfaceData,sizeof(SNETUIDATA));
	uidata.statuscallback = ModmStatStatusCallback;
	uidata.size           = sizeof(SNETUIDATA);
	uidata.parentwindow   = window;

	return SNetInitializeDevice(
		deviceid,
		sgSMProgramData,
		sgSMPlayerData,
		&uidata,
		sgSMVersionData
	);
}


//****************************************************************************
//****************************************************************************
static BOOL SelModemNoModem(DWORD providerid) {
	if (providerid == 'MODM') {
		TCHAR err[128];
		LoadString(global_hinstance, IDS_NOMODEM_ERR, err, 127);
		SelOkDialog(sgSMInterfaceData->parentwindow, err, NULL, FALSE);
		return FALSE;
	}
	else {
		return TRUE;
	}
}


//****************************************************************************
//****************************************************************************
static BOOL SelModemOneModem(void) {
   	if (SelModemInitDevice(sgSMInterfaceData->parentwindow, sgModemList->deviceid)) {
		ModemFreeList(sgModemList);
		return TRUE;
   	}
   	else {
   		// error initializing modem
		TCHAR initerr[128];
		LoadString(global_hinstance, IDS_INITMODEM_ERR, initerr, 127);
		SelOkDialog(sgSMInterfaceData->parentwindow, initerr, NULL, FALSE);
		ModemFreeList(sgModemList);
		return TRUE;
   	}
}


//****************************************************************************
//****************************************************************************
static BOOL SelModemListModems(void) {
   	int result = SDlgDialogBox(
  		global_hinstance,
  		TEXT("SELMODEM_DIALOG"),
  		sgSMInterfaceData->parentwindow,
  		SelModemDialogProc
   	);

   	if (result == IDOK) {
   		if (SelModemInitDevice(sgSMInterfaceData->parentwindow, sgDeviceID)) {
			ModemFreeList(sgModemList);
			return TRUE;
   		}
   		else {
   			// error initializing device
			TCHAR initerr[128];
			LoadString(global_hinstance, IDS_INITMODEM_ERR, initerr, 127);
			SelOkDialog(sgSMInterfaceData->parentwindow, initerr, NULL, FALSE);
			ModemFreeList(sgModemList);
			return FALSE;
		}
	}
	else {
		// user cancelled
		ModemFreeList(sgModemList);
		SetLastError(SNET_ERROR_CANCELLED);
		return FALSE;
	}
}


//****************************************************************************
//****************************************************************************
BOOL UiSelectDevice(DWORD				providerid,
                    SNETPROGRAMDATAPTR  programdata,
                    SNETPLAYERDATAPTR   playerdata,
                    SNETUIDATAPTR       interfacedata,
                    SNETVERSIONDATAPTR  versiondata) {
	sgSMProgramData   = programdata;
	sgSMPlayerData    = playerdata;
	sgSMInterfaceData = interfacedata;
	sgSMVersionData   = versiondata;

	// enum all devices
	sgModemList = NULL;
	sgNumModems = 0;
	SNetEnumDevices(SelModemEnum);

	switch (sgNumModems) {
		case 0:
		return SelModemNoModem(providerid);

		case 1:
		return SelModemOneModem();

		default:
		return SelModemListModems();
	}
}

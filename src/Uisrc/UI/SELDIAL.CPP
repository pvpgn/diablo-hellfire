//****************************************************************************
//	SelDial.cpp
//	Diablo UI select "dial" or "answer" for modem setup
//	
//	By Frank Pearce
//	created 11.23.96
//****************************************************************************


#include "pch.h"
#include "artfont.h"
#include "uisnd.h"


//****************************************************************************
//****************************************************************************
	extern void ModemSetInfo(HWND window, LPCSTR infotitle, LPCSTR text);
	extern BOOL ModemConnected(void);
	extern BOOL ModemEnum(void);
	extern LPCSTR ModemGameName(void);
	extern int ModemLastJoinedNumber(void);
	extern int ModemGameNumber(LPCSTR gamename);


//****************************************************************************
//****************************************************************************
	#define PHONEBOOK_KEY		"Diablo\\Phone Book"
	#define PHONEBOOK_ENTRY1	"Entry1"
	#define PHONEBOOK_ENTRY2	"Entry2"
	#define PHONEBOOK_ENTRY3	"Entry3"
	#define PHONEBOOK_ENTRY4	"Entry4"

	#define MILLISEC_PER_SEC	1000

	#define FOCUS_TIMER_ID		1
	#define FOCUS_FPS			16		// frames per second
	#define FOCUS_TIMER_DELAY	55		//(MILLISEC_PER_SEC / FOCUS_FPS)

	#define	ENUM_TIMER_ID		2
	#define ENUM_TIMER_DELAY	2000

	#define PHONEBOOK_SIZE		4
	typedef struct _phonenumber {
		TCHAR number[MAX_DIAL_LEN];
	} TNUMBER, * TPNUMBER;

	
//****************************************************************************
//****************************************************************************
	static char * sgPhoneEntryNames[PHONEBOOK_SIZE] = {
		PHONEBOOK_ENTRY1,
		PHONEBOOK_ENTRY2,
		PHONEBOOK_ENTRY3,
		PHONEBOOK_ENTRY4,
	};

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
		IDC_DIALBTN1,
		IDC_DIALBTN2,
		IDC_DIALBTN3,
		IDC_DIALBTN4,
		IDC_DIALBTN5,
		IDC_DIALBTN6,
		0
	};

	static TNUMBER sgPhoneBook[PHONEBOOK_SIZE];
	static LPTSTR sgDialString;


//****************************************************************************
//****************************************************************************
void SelDialSaveBookEntries(LPCSTR dialstring) {
	TCHAR entry[MAX_DIAL_LEN];
	int i, lasttomove;

	// is the string already in the list?
	for (i = 0; i < PHONEBOOK_SIZE; i++) {
		if (
			SRegLoadString(
				PHONEBOOK_KEY,
				sgPhoneEntryNames[i],
				SREG_FLAG_USERSPECIFIC,
				entry,
				MAX_DIAL_LEN
			)
		) {
			entry[MAX_DIAL_LEN-1] = 0;
			if (0 == strcmp(entry, dialstring)) {
				break;
			}
		}
	}

	if (i == PHONEBOOK_SIZE)
		lasttomove = PHONEBOOK_SIZE - 1;
	else
		lasttomove = i;

	// move all other numbers down in the phone book
   	for (i = lasttomove-1; i >= 0; i--) {
		entry[0] = 0;
   		if (
			SRegLoadString(
   				PHONEBOOK_KEY,
   				sgPhoneEntryNames[i],
   				SREG_FLAG_USERSPECIFIC,
   				entry,
   				MAX_DIAL_LEN
   			)
		) {
   			entry[MAX_DIAL_LEN-1] = 0;
			if (strlen(entry)) {
	   			SRegSaveString(
	   				PHONEBOOK_KEY,
	   				sgPhoneEntryNames[i+1],
	   				SREG_FLAG_USERSPECIFIC,
	   				entry
	   			);
			}
		}
   	}

	// save the number in the first slot
   	SRegSaveString(
   		PHONEBOOK_KEY,
   		sgPhoneEntryNames[0],
   		SREG_FLAG_USERSPECIFIC,
   		dialstring
   	);
}


//****************************************************************************
//****************************************************************************
static void SelDialGetBookEntries(void) {
	for (int i = 0; i < PHONEBOOK_SIZE; i++) {
		if (
			SRegLoadString(
				PHONEBOOK_KEY,
				sgPhoneEntryNames[i],
				SREG_FLAG_USERSPECIFIC,
				sgPhoneBook[i].number,
				MAX_DIAL_LEN
			)
		) {
			sgPhoneBook[i].number[MAX_DIAL_LEN-1] = 0;
		}
		else {
			sgPhoneBook[i].number[0] = 0;
		}
	}
}


//****************************************************************************
//****************************************************************************
static void SelDialPhoneBook(HWND window) {
	int i;
	HWND child;
	TPBMP tpBmp;
	TCHAR fmt[32];
	TCHAR callstr[64];

	// make sure my phone book is empty
	for (i = 0; i < PHONEBOOK_SIZE; i++)
		sgPhoneBook[i].number[0] = 0;

	// get the phone book numbers
	SelDialGetBookEntries();

	// put the info into the buttons
	LoadString(global_hinstance, IDS_CALL_FMT, fmt, 31);
	for (i = 0; i < PHONEBOOK_SIZE; i++) {
		child = GetDlgItem(window, IDC_DIALBTN3 + i);
		if (! child) continue;
		tpBmp = (TPBMP) GetWindowLong(child, GWL_USERDATA);
		if (! tpBmp) continue;
		if (strlen(sgPhoneBook[i].number)) {
			wsprintf(callstr, fmt, sgPhoneBook[i].number);
			UiSetBmpText(tpBmp, callstr);
		}
		else {
			EnableWindow(child, FALSE);
			UiSetBmpText(tpBmp, NULL);
		}
	}

	UiDoomButtonsReset(window, sgListIDs, AF_SMALL);
}


//****************************************************************************
//****************************************************************************
static void SelDialSetInfo(HWND window, int childid) {
	TCHAR title[64], desc[256];
	LoadString(global_hinstance, IDS_SELDIAL_INFOTITLE, title, 63);
	switch (childid) {
		case IDC_DIALBTN1:
			if (ModemConnected()) {
				LoadString(global_hinstance, IDS_MODEM_CREATE2, desc, 255);
			}
			else {
				LoadString(global_hinstance, IDS_MODEM_CREATE, desc, 255);
			}
		break;
		case IDC_DIALBTN2:
			if (ModemConnected()) {
				LoadString(global_hinstance, IDS_MODEM_JOIN, desc, 255);
			}
			else {
				LoadString(global_hinstance, IDS_MODEM_DIALNEW, desc, 255);
			}
		break;
		default:
			LoadString(global_hinstance, IDS_MODEM_DIALOLD, desc, 255);
		break;
	}
	ModemSetInfo(GetParent(window), title, desc);
}


//****************************************************************************
//****************************************************************************
static void SelDialDestroy(HWND window) {
	UiDoomCtrlsDestroy(window, sgListIDs);
	UiDoomCtrlsDestroy(window, sgBtnIDs);
	UiDoomCtrlsDestroy(window, sgTextIDs);
	FocusAnimateDestroy();
	ModemSetInfo(GetParent(window), NULL, NULL);
}


//****************************************************************************
//****************************************************************************
static void SelDialSetJoinBtn(HWND window) {
	char joinstr[64];
	HWND child = GetDlgItem(window, IDC_DIALBTN2);
	if (! child) return;

	if (ModemGameNumber(ModemGameName()) > ModemLastJoinedNumber())
		LoadString(global_hinstance, IDS_MODEMJOINNEW, joinstr, 63);
	else
		LoadString(global_hinstance, IDS_MODEMJOINPREV, joinstr, 63);

	UiSetBmpText((TPBMP)GetWindowLong(child, GWL_USERDATA), joinstr);
	UiDoomButtonsReset(window, sgListIDs, AF_SMALL);
}


//****************************************************************************
//****************************************************************************
static void SelDialClearJoinBtn(HWND window) {
	HWND child = GetDlgItem(window, IDC_DIALBTN2);
	if (! child) return;

	UiSetBmpText((TPBMP)GetWindowLong(child, GWL_USERDATA), NULL);

	UiDoomButtonsReset(window, sgListIDs, AF_SMALL);
}


//****************************************************************************
//****************************************************************************
static void SelDialAbort(HWND window, int ReturnVal) {
	if ((ReturnVal == IDOK) || (ReturnVal == IDCANCEL))
		UiSndPlayEnter();
	SDlgKillTimer(window, FOCUS_TIMER_ID);
	SDlgKillTimer(window, ENUM_TIMER_ID);
	if (IDOK == ReturnVal) {
		// see what the selection was
		int ID = GetWindowLong(GetFocus(), GWL_ID);
		switch (ID) {
			case IDC_DIALBTN1:
				SDlgEndDialog(window, MODE_ANSWER);
			break;
			case IDC_DIALBTN2:
				SDlgEndDialog(window, MODE_DIALNEW);
			break;
			default: {
				if (sgDialString)
					strcpy(sgDialString, sgPhoneBook[ID - IDC_DIALBTN3].number);
				SDlgEndDialog(window, MODE_DIALOLD);
			}
			break;
		}
	}
	else {
		SDlgEndDialog(window, ReturnVal);
	}
}


//****************************************************************************
//****************************************************************************
static void SelDialEnum(HWND window) {
	HWND child = GetDlgItem(window, IDC_DIALBTN2);
	if (! child) return;

	if (ModemEnum()) {
		SelDialSetJoinBtn(window);
		EnableWindow(child, TRUE);
		ShowWindow(child, TRUE);
	}
	else {
		if (GetLastError() == SNET_ERROR_NO_NETWORK) {
			SelDialAbort(window, SNET_ERROR_NO_NETWORK);
		}
		else {
			if (GetFocus() == child) {
				SetFocus(GetDlgItem(window, IDC_DIALBTN1));
			}
			SelDialClearJoinBtn(window);
			EnableWindow(child, FALSE);
			ShowWindow(child, FALSE);
		}
	}
}


//****************************************************************************
//****************************************************************************
static void SelDialInit(HWND window) {
	HWND parent = GetParent(window);

	// point this window at the same bmp it's parent uses
	SetWindowLong(
		window,
		GWL_USERDATA,
		GetWindowLong(parent, GWL_USERDATA)
	);

	FocusAnimateInit("ui_art\\focus16.pcx");
	SDlgSetTimer(window, FOCUS_TIMER_ID, FOCUS_TIMER_DELAY, NULL);

	UiOnPaintBtns(window, sgListIDs);

	// set up a doom-like interface
	UiDoomStaticInit(window, sgTextIDs, AF_BIGGRAY);
	UiDoomButtonsInit(window, sgBtnIDs, AF_BIG, FALSE);
	UiDoomButtonsInit(window, sgListIDs, AF_SMALL);

	if (! ModemConnected()) {
		SelDialPhoneBook(window);
	}
	else {
		SDlgSetTimer(window, ENUM_TIMER_ID, ENUM_TIMER_DELAY, NULL);
		SelDialEnum(window);
	}
}


//****************************************************************************
//****************************************************************************
static void SelDialInterpretClick(HWND window, int x, int y) {
	if (UiIsPtInWindow(window, GetDlgItem(window, IDC_FAKEOK), x, y)) {
		SelDialAbort(window, IDOK);
	}
	else if (UiIsPtInWindow(window, GetDlgItem(window, IDC_FAKECANCEL), x, y)) {
		SelDialAbort(window, IDCANCEL);
	}
}


//****************************************************************************
//****************************************************************************
BOOL CALLBACK SelDialDialogProc(HWND window,
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
				SelDialSetInfo(window, LOWORD(wparam));
			}
			else if (HIWORD(wparam) == BN_DOUBLECLICKED) {
				SelDialAbort(window, IDOK);
			}
			else if (LOWORD(wparam) == IDOK) {
				SelDialAbort(window, IDOK);
			}
			else if (LOWORD(wparam) == IDCANCEL) {
				SelDialAbort(window, IDCANCEL);
			}
		break;

		case WM_LBUTTONDOWN:
			SelDialInterpretClick(window, LOWORD(lparam), HIWORD(lparam));
		break;

		case WM_DESTROY:
			SelDialDestroy(window);
		break;

		case WM_INITDIALOG:
			sgDialString = (LPSTR) lparam;
			SelDialInit(window);
		return 0;

		case WM_TIMER:
			switch (wparam) {
				case FOCUS_TIMER_ID:
					FocusAnimate(window, GetFocus());
				break;
				case ENUM_TIMER_ID:
					SelDialEnum(window);
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

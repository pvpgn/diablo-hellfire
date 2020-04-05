//****************************************************************************
//***
//***  TITLE.CPP
//***  Diablo UI title screen dialog
//***
//***  By Frank Pearce (9/13/96)
//***
//****************************************************************************


#include "pch.h"
#include "artfont.h"


//****************************************************************************
//****************************************************************************
	#define MILLISEC_PER_SEC	1000

	#define TIMEOUT_TIMER_ID	1
	#define DEFAULT_TIMEOUT		5

	#define LOGO_TIMER_ID		2
	#define LOGO_FPS			17
	#define LOGO_TIMER_DELAY	55		// (MILLISEC_PER_SEC / LOGO_FPS)

	#define MAX_FRAMES			30

	static LPBYTE sgLogoBmp = NULL;
	static SIZE sgLogoBmpSize;

	static HTRANS sgTransHandles[MAX_FRAMES];
	static int sgFrame;

	static int sgBlizIDs[] = {
		IDC_BLIZZARD,
		0
	};


//****************************************************************************
//****************************************************************************
void TitleLogoAnimate(HWND window) {
	RECT rect;
	TPBMP tpBmp = (TPBMP) GetWindowLong(window, GWL_USERDATA);
	HWND child = GetDlgItem(window, IDC_LOGO);

	if (! IsWindowVisible(window)) return;
	if (! child) return;
	if (! tpBmp) return;
	if (! tpBmp->data) return;
	if (! sgTransHandles[0]) return;

	sgFrame++;
	if ((! sgTransHandles[sgFrame]) || (sgFrame >= MAX_FRAMES))
		sgFrame = 0;

	GetWindowRect(child, &rect);
	ScreenToClient(window, (LPPOINT)&rect.left);
	ScreenToClient(window, (LPPOINT)&rect.right);

	// Copy from Background bitmap to buffer for animating logo
   	SBltROP3 (
   		sgLogoBmp,
		tpBmp->data + (rect.top * tpBmp->datasize.cx) + rect.left,
		sgLogoBmpSize.cx,
		(sgLogoBmpSize.cy > 480) ? 480 : sgLogoBmpSize.cy,
		sgLogoBmpSize.cx,
		tpBmp->datasize.cx,
		NULL,
		SRCCOPY
   	);

	STransBlt(sgLogoBmp, 0, 0, sgLogoBmpSize.cx, sgTransHandles[sgFrame]);

	InvalidateRect(child, NULL, FALSE);
}


//****************************************************************************
//****************************************************************************
void TitleLogoDestroy(void) {
 	for (int index = 0; index < MAX_FRAMES; index++) {
		if (sgTransHandles[index]) {
			STransDelete(sgTransHandles[index]);
			sgTransHandles[index] = NULL;
		}
	}

	if (sgLogoBmp) {
		FREE(sgLogoBmp);
		sgLogoBmp = NULL;
	}
}


//****************************************************************************
//****************************************************************************
static void TitleDestroy (HWND window) {
	TitleLogoDestroy();

	UiDoomCtrlsDestroy(window, sgBlizIDs);

	UiFreeBmp((TPBMP) GetWindowLong(window, GWL_USERDATA));
	SetWindowLong(window, GWL_USERDATA, (LONG) 0);
}


//****************************************************************************
//****************************************************************************
void TitleLogoInit(HWND window) {
	RECT rect;
	HWND child;

	// alloc/create a bitmap for the animating logo
	child = GetDlgItem(window, IDC_LOGO);
	if (! child) return;
	GetClientRect(child, &rect);

	sgLogoBmp = (LPBYTE) ALLOC(rect.right * rect.bottom);
	if (!sgLogoBmp) {
		return;
	}
	sgLogoBmpSize.cx = rect.right;
	sgLogoBmpSize.cy = rect.bottom;

	SDlgSetBitmap(
		child,
		NULL,
		NULL,
		SDLG_STYLE_ANY,
		SDLG_USAGE_BACKGROUND,
		sgLogoBmp,
		NULL,
		rect.right,
		rect.bottom
	);
}


//****************************************************************************
//****************************************************************************
void TitleFramesInit(HWND window, LPCTSTR filename) {
	LPBYTE srcframes = NULL;
	SIZE srcsize;
	RECT rect;

	UiLoadBmpFile(
		filename,
		&srcframes,
		&srcsize
	);

	memset(sgTransHandles, 0, sizeof(sgTransHandles));

	if (srcframes) {
		int frames = srcsize.cy / sgLogoBmpSize.cy;
		if (frames > MAX_FRAMES)
			frames = MAX_FRAMES;
		// create an HTRANS for each frame in the animation
		for (int index = 0; index < frames; index++) {

			rect.left = 0;
			rect.right = sgLogoBmpSize.cx - 1;
			rect.top = index * sgLogoBmpSize.cy;
			rect.bottom = rect.top + sgLogoBmpSize.cy - 1;

			STransCreate(
				srcframes,
				sgLogoBmpSize.cx,
				sgLogoBmpSize.cy,
				8,
				&rect,
				PALETTEINDEX(250),
				&sgTransHandles[index]
			);

		}

		FREE(srcframes);
	}
	sgFrame = 0;
}


//****************************************************************************
//****************************************************************************
static void TitleInit(HWND window, LPARAM lparam) {
	TPBMP tpBmp;

	// set up a bmp for the window
	tpBmp = UiAllocBmp();
	SetWindowLong(window, GWL_USERDATA, (LONG) tpBmp);
	if (tpBmp) {
		LoadArtFile(
			window,
			NULL,
			TEXT(""),
			SDLG_STYLE_ANY,
			SDLG_USAGE_BACKGROUND,
			TEXT("ui_art\\hf_title.pcx"),
			&tpBmp->data,
			&tpBmp->datasize,
			FALSE
		);
		UiFadeInit(window, FALSE);
	}

	UiDoomStaticInit(window, sgBlizIDs, AF_MEDGRAY);

	// set up the animating diablo logo
	TitleLogoInit(window);
	TitleFramesInit(window,TEXT("ui_art\\hf_logo1.pcx"));
	TitleLogoAnimate(window);
	SDlgSetTimer(window, LOGO_TIMER_ID, LOGO_TIMER_DELAY, NULL);

	// set a timer so the screen will timeout
	if (lparam)
		SDlgSetTimer(window, TIMEOUT_TIMER_ID, lparam*MILLISEC_PER_SEC, NULL);
	else
		SDlgSetTimer(window, TIMEOUT_TIMER_ID, DEFAULT_TIMEOUT*MILLISEC_PER_SEC, NULL);
}


//****************************************************************************
//****************************************************************************
static void TitleAbort(HWND window) {
	UiFadeAbort(window);
	SDlgKillTimer(window, LOGO_TIMER_ID);
	SDlgKillTimer(window, TIMEOUT_TIMER_ID);
	UiVidFadeOut(DEFAULT_STEPS*2);
	SDlgEndDialog(window, 1);
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK TitleDialogProc (HWND   window,
                                      UINT   message,
                                      WPARAM wparam,
                                      LPARAM lparam) {
	switch (message) {

		case WM_DESTROY:
			TitleDestroy(window);
		break;
		
		case WM_INITDIALOG:
			TitleInit(window, lparam);
			PostMessage(window, WM_USER+1000, 0, 0);
		return 1;

		case WM_PARENTNOTIFY:
			if (LOWORD(wparam) == WM_LBUTTONDOWN || LOWORD(wparam) == WM_RBUTTONDOWN) {
				TitleAbort(window);
			}
		break;

		case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS;

		case WM_COMMAND:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_KEYDOWN:
			TitleAbort(window);
		return 0;

		case WM_USER+1000:
			if (! UiIsFading()) {
				UiFadeStart(window);
			}
		return 0;

		case WM_TIMER:
			switch (wparam) {
				case TIMEOUT_TIMER_ID:
					TitleAbort(window);
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
//***
//***  EXPORTED FUNCTIONS
//***
//****************************************************************************


//****************************************************************************
//****************************************************************************
BOOL APIENTRY UiTitleDialog (UINT timeoutseconds) {
	// init the custom non-windows font
	ArtFontLoad();

	SDlgDialogBoxParam(
		global_hinstance,
		"TITLESCREEN_DIALOG",
		SDrawGetFrameWindow(),
		TitleDialogProc,
		timeoutseconds
	);

  return 1;
}

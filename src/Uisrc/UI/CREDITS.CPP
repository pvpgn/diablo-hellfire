//****************************************************************************
//
//  Credits.CPP
//  Diablo UI credits screen
//
//  By Michael Morhaime (9/16/96)
//	Modified 10.3.96 by Frank Pearce
//****************************************************************************


#include "pch.h"


//****************************************************************************
//****************************************************************************
	#define PIXELS_PER_SCROLL	1
	#define ID_CREDITS_TIMER	1
	#define MILLISEC_PER_SEC	1000
	#define PIXELS_PER_SEC		20
	#define DEFAULT_TIMER		(MILLISEC_PER_SEC / (PIXELS_PER_SEC / PIXELS_PER_SCROLL))
	#define USER_TIMER			(MILLISEC_PER_SEC / (pixelspersec / PIXELS_PER_SCROLL))

	#define VERTICAL_PAD		30		// Padding above and below viewable part of offscreen rect
	#define TAB_PIXEL_SIZE		40
	#define LINE_HEIGHT			22
	
	static LPBYTE sgTextBits = NULL;	// bitmap for scrolling text ctrl
	static SIZE   sgTextBitsSize;
	static HSGDIFONT sgTextFont = NULL;
	static int sgiTextLoc;
	
	static HGLOBAL sghCredits;			// handle for the credits string resource
	static int     sgCreditsSize;		// length of resource in bytes
	
	static void ScrollCredits(HWND window);


//****************************************************************************
//****************************************************************************
static void CreditsDestroy (HWND window) {
	if (sgTextFont) {
		SGdiDeleteObject(sgTextFont);
		sgTextFont = NULL;
	}
	if (sgTextBits) {
		FREE(sgTextBits);
		sgTextBits = NULL;
	}

	UiFreeBmp((TPBMP) GetWindowLong(window, GWL_USERDATA));
	SetWindowLong(window, GWL_USERDATA, (LONG) 0);

	if (sghCredits) {
		// free the credits text
		FreeResource(sghCredits);
		sghCredits = NULL;
	}
}


//****************************************************************************
//****************************************************************************
static void CreditsAbort(HWND window) {
	UiFadeAbort(window);
	SDlgKillTimer(window,ID_CREDITS_TIMER);
	UiVidFadeOut(DEFAULT_STEPS*2);
	SDlgEndDialog(window, 1);
}


//****************************************************************************
//****************************************************************************
static void CreditsInit (HWND window, LPARAM pixelspersec) {
	TPBMP tpBmp;
	HWND child;
	HFONT winfont;
	RECT rect;
	int nWidth, nHeight;

	// set up the timer based on the specified pixel rate
	SDlgSetTimer(
		window,
		ID_CREDITS_TIMER,
		(pixelspersec ? USER_TIMER : DEFAULT_TIMER),
		NULL
	);

	// load the credits into memory
	sghCredits = LoadResource(
		global_hinstance,
		FindResource(global_hinstance, TEXT("IDR_CREDITS"), TEXT("TEXT_FILES"))
	);
	sgCreditsSize = SizeofResource(
		global_hinstance,
		FindResource(global_hinstance, TEXT("IDR_CREDITS"), TEXT("TEXT_FILES"))
	);

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
			TEXT("ui_art\\credits.pcx"),
			&tpBmp->data,
			&tpBmp->datasize,
			FALSE
		);
		UiFadeInit(window, FALSE);
	}

	// alloc/create a bitmap for the scrolling text ctrl
	child = GetDlgItem(window, ID_SCROLL_WINDOW);
	GetWindowRect(child, &rect);

	nWidth = rect.right - rect.left;
	nHeight = rect.bottom - rect.top + (VERTICAL_PAD*2);

	sgTextBits = (LPBYTE) ALLOC(nWidth * nHeight);
	if (!sgTextBits) {
		CreditsAbort(window);
		return;
	}
	sgTextBitsSize.cx = nWidth;
	sgTextBitsSize.cy = nHeight;

	UiCalcNewRect(&rect, 0, VERTICAL_PAD);

	SDlgSetBitmap(
		child,
		NULL,
		NULL,
		SDLG_STYLE_ANY,
		SDLG_USAGE_BACKGROUND,
		sgTextBits,
		&rect,
		nWidth,
		nHeight
	);

	// init the starting text coordinate and begin drawing the credits
	sgiTextLoc = nHeight - VERTICAL_PAD;
	ScrollCredits(window);

	// create the font for SGdiTextOut
	winfont = CreateFont(
		-17,0,0,0,FW_BOLD,0,0,0,ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
        VARIABLE_PITCH | FF_ROMAN,TEXT("Times New Roman")
	);
	if (winfont) {
		BOOL result = SGdiImportFont(winfont,&sgTextFont);
		DeleteObject(winfont);
		if (! result) {
			CreditsAbort(window);
		}
	}
	else {
		CreditsAbort(window);
	}
}


//****************************************************************************
//****************************************************************************
static int sgCreditsLen;

static int credit_length(LPTSTR credit) {
	int len = 0;

	// find the length (in characters) of a single line
	while ((*credit != '\r') && (*credit != '\n')) {
		len++;
		credit++;
		if (len > sgCreditsLen) {
			len = -1;
			break;
		}
	}
	return len;
}


//****************************************************************************
//****************************************************************************
static LPTSTR credit_next(LPTSTR lastcredit, int lastlen) {
	// skip over last credit and cr/lf
	lastcredit += (lastlen + 2);
	sgCreditsLen -= (lastlen + 2);

	return lastcredit;
}


//****************************************************************************
//****************************************************************************
static void CreditsDraw(HWND window) {
	int x, y, stringlen;
	LPTSTR credits;

	// lock the resource and get it's size
	credits = (LPTSTR)LockResource(sghCredits);
	sgCreditsLen = sgCreditsSize;

	SGdiSelectObject(sgTextFont);
	SGdiSetPitch(sgTextBitsSize.cx);

	for ( 
		y = sgiTextLoc;
		sgCreditsLen > 0;
		y += LINE_HEIGHT, credits = credit_next(credits, stringlen)
	) {
		// init left coord
		x = 0;
		// handle tabs at the begining of a line
		while (*credits == '\t') {
			x += TAB_PIXEL_SIZE;
			credits++;
			sgCreditsLen--;
		}
		stringlen = credit_length(credits);

		// make sure the dest coords are in the buffer
		if (stringlen == -1) break;
		if (y < 0) continue;
		if (y > (sgTextBitsSize.cy - VERTICAL_PAD)) break;

		if (stringlen) {
#if 1
			// draw a drop shadow
			SGdiTextOut(
				sgTextBits,
				x + 2,
				y + 2,
				PALETTEINDEX(0),
				credits,
				stringlen
			);
#endif

			// draw the text
			SGdiTextOut(
				sgTextBits,
				x,
				y,
				PALETTEINDEX(224),
				credits,
				stringlen
			);
		}
	}

	// has the bottom of the credits scrolled past the top?
	if (y < 0) {
		CreditsAbort(window);
	}
}


//****************************************************************************
//****************************************************************************
static void ScrollCredits (HWND window) {
	RECT rect;
	HWND child = GetDlgItem(window, ID_SCROLL_WINDOW);
	TPBMP tpBmp = (TPBMP) GetWindowLong(window, GWL_USERDATA);

	GetWindowRect(child, &rect);
	ScreenToClient(window, (LPPOINT)&rect.left);
	ScreenToClient(window, (LPPOINT)&rect.right);

	// Copy from Background bitmap to buffer for scrolling text ctrl
   	SBltROP3 (
   		sgTextBits + (sgTextBitsSize.cx * VERTICAL_PAD),
		tpBmp->data + (rect.top * tpBmp->datasize.cx) + rect.left,
		sgTextBitsSize.cx,
		rect.bottom - rect.top,
		sgTextBitsSize.cx,
		tpBmp->datasize.cx,
		NULL,
		SRCCOPY
   	);

	sgiTextLoc -= PIXELS_PER_SCROLL;
	CreditsDraw(window);

	InvalidateRect(child, NULL, FALSE);
	UpdateWindow(child);
}


//****************************************************************************
//****************************************************************************
static BOOL CALLBACK CreditsDialogProc (HWND   window,
                                      UINT   message,
                                      WPARAM wparam,
                                      LPARAM lparam) {

	switch (message) {

		case WM_DESTROY:
			CreditsDestroy(window);
		break;
		
		case WM_INITDIALOG:
			CreditsInit(window, lparam);
			PostMessage(window, WM_USER+1000, 0, 0);
		return 1;

		case WM_TIMER:
			ScrollCredits(window);
		break;

		case WM_USER+1000:
			if (! UiIsFading()) {
				UiFadeStart(window);
			}
		return 0;

		case WM_PARENTNOTIFY:
			if (LOWORD(wparam) == WM_LBUTTONDOWN || LOWORD(wparam) == WM_RBUTTONDOWN) {
				CreditsAbort(window);
			}
		break;

		case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS;

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			CreditsAbort(window);
		return 0;

		case WM_KEYDOWN:
			if (wparam == VK_SPACE) {
				CreditsAbort(window);
			}
		return 0;

		case WM_COMMAND:
			CreditsAbort(window);
		break;

		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			SendMessage(SDrawGetFrameWindow(), message, wparam, lparam);
		break;
	}

	return SDlgDefDialogProc(window,message,wparam,lparam);
}


/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/


//****************************************************************************
//****************************************************************************
BOOL APIENTRY UiCreditsDialog (UINT pixelspersec) {
	// fuck allowing customizable speed
	pixelspersec = 25;

	// start the dialog
	SDlgDialogBoxParam(
		global_hinstance,
		TEXT("CREDITS_DIALOG"),
		SDrawGetFrameWindow(),
		CreditsDialogProc,
		pixelspersec
	);

	return 1;
}

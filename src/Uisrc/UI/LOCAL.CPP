//****************************************************************************
//	local.cpp
//	created 9.13.96
//****************************************************************************

#define OEMRESOURCE

#include "pch.h"
#include "artfont.h"


#define TEST_REDRAW		0		// 0 in final


//****************************************************************************
//****************************************************************************
	#define WNDPROCPROP			TEXT("UIWNDPROC")

	static PALETTEENTRY sgPalette[PALETTE_REGISTERS];
	static HPALETTE sghPalIdentity = NULL;

	
//****************************************************************************
void UiCreateIdentityPal(void) {
	LOGPALETTE *lPalette;
	PALETTEENTRY *pe;
	DWORD *pDword;

	lPalette = (LOGPALETTE *)ALLOC(sizeof(LOGPALETTE) + (PALETTE_REGISTERS-1)*sizeof(PALETTEENTRY));
	if (lPalette == NULL)
		return;

	lPalette->palVersion = 0x300;
	lPalette->palNumEntries = PALETTE_REGISTERS;
	pe = &lPalette->palPalEntry[0];
	pDword = (DWORD *)pe;
	for (int i=0; i<PALETTE_REGISTERS; i++, pe++, pDword++) {
		*pDword = i;				
		pe->peFlags = PC_EXPLICIT;
	}

	sghPalIdentity = CreatePalette(lPalette);
	FREE(lPalette);
}

//****************************************************************************
void UiFreeIdentityPal(void) {
	if (sghPalIdentity) {
		DeleteObject(sghPalIdentity);
		sghPalIdentity = NULL;
	}
}

//****************************************************************************
HPALETTE UiGetIdentityPal(void) { 
	return sghPalIdentity;
}


//****************************************************************************
//****************************************************************************
PALETTEENTRY * UiPaletteEntry(int index) {
	return &sgPalette[index];
}


//****************************************************************************
//****************************************************************************
void UiClearPalette(PALETTEENTRY * pe) {
	for (int palreg = 0; palreg < PALETTE_REGISTERS; palreg++) {
 		pe[palreg].peRed   = 0;
 		pe[palreg].peGreen = 0;
 		pe[palreg].peBlue  = 0;
	}
}


//****************************************************************************
//****************************************************************************
BOOL UiLoadBmpFile (LPCTSTR filename,
					LPBYTE  *bitmapbits,
					SIZE    *bitmapsize) {

	int width, height;

	// DETERMINE THE IMAGE DIMENSIONS
	if (!SBmpLoadImage(filename,NULL,NULL,0,&width,&height)) {
		if (bitmapbits) {
			*bitmapbits = 0;
		}
		if (bitmapsize) {
			bitmapsize->cx = 0;
			bitmapsize->cy = 0;
		}
		return 0;
	}
	
	if (bitmapsize) {
		bitmapsize->cx = width;
		bitmapsize->cy = height;
	}

	// LOAD THE IMAGE
	*bitmapbits = (LPBYTE)ALLOC(width*height);
	
	if (!*bitmapbits) {
		if (bitmapsize) {
			bitmapsize->cx = 0;
			bitmapsize->cy = 0;
		}
		return 0;
	}
	
	if (!SBmpLoadImage(filename,NULL,*bitmapbits,width*height)) {
		FREE(*bitmapbits);
		*bitmapbits = NULL;
		
		bitmapsize->cx = 0;
		bitmapsize->cy = 0;
		return 0;
	}

	return 1;
}


//****************************************************************************
//****************************************************************************
BOOL LoadArtFile (	HWND	window,
					HWND	parent,
					LPCTSTR	controltype,
					DWORD	controlstyle,
					LONG	usageflags,
					LPCTSTR	filename,
					LPBYTE	*bitmapbits,
					SIZE	*bitmapsize,
					BOOL	updatepal) {

	PALETTEENTRY pe[PALETTE_REGISTERS];
	int width, height;

	// DETERMINE THE IMAGE DIMENSIONS
	if (!SBmpLoadImage(filename,NULL,NULL,0,&width,&height)) {
		if (bitmapbits) {
			*bitmapbits = 0;
		}
		if (bitmapsize) {
			bitmapsize->cx = 0;
			bitmapsize->cy = 0;
		}
		return 0;
	}
	
	if (bitmapsize) {
		bitmapsize->cx = width;
		bitmapsize->cy = height;
	}

	// LOAD THE IMAGE
	*bitmapbits = (LPBYTE)ALLOC(width*height);
	if (!*bitmapbits) {
		if (bitmapsize) {
			bitmapsize->cx = 0;
			bitmapsize->cy = 0;
		}
		return 0;
	}
	if (!SBmpLoadImage(filename, pe, *bitmapbits, width*height)) {
		if (bitmapsize) {
			bitmapsize->cx = 0;
			bitmapsize->cy = 0;
		}
		FREE(*bitmapbits);
		*bitmapbits = 0;
		return 0;
	}
	
	// REGISTER THE IMAGE WITH THE DIALOG BOX MANAGER
	if (!SDlgSetBitmap(
			window,
			parent,
			controltype,
			controlstyle,
			usageflags,
			*bitmapbits,
			NULL,
			width,
			height
	)) {
		if (bitmapsize) {
			bitmapsize->cx = 0;
			bitmapsize->cy = 0;
		}
		FREE(*bitmapbits);
		*bitmapbits = 0;
		return 0;
	}

	// IF THIS IS A SCREEN BACKGROUND, USE ITS PALETTE
	if ((!controltype) || (!*controltype)) {
		HPALETTE hPal;

		// Get Windows Default colors from the system
		hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);
		GetPaletteEntries(hPal, 0, 10, &pe[0]);
		GetPaletteEntries(hPal, 10, 10, &pe[PALETTE_REGISTERS-10]);

		memcpy(sgPalette, pe, (PALETTE_REGISTERS * sizeof(PALETTEENTRY)));
		if (updatepal) {
			SDrawUpdatePalette(0, 255, &sgPalette[0], TRUE);
		}
		else {
			UiClearPalette(pe);
			SDrawUpdatePalette(0, PALETTE_REGISTERS, pe, TRUE);
		}
	}

	return 1;
}


//****************************************************************************
// given a rect, calculate a new rect of the same size
// that uses the new given upper left coordinates
//****************************************************************************
void UiCalcNewRect(RECT *rect, int xoffset, int yoffset) {
	rect->right -= rect->left;
	rect->right += xoffset - 1;
	rect->left = xoffset;

	rect->bottom -= rect->top;
	rect->bottom += yoffset - 1;
	rect->top = yoffset;
}


//****************************************************************************
//****************************************************************************
BOOL UiSetStaticBmp(HWND parent, int child_id, LPBYTE bmpbits, SIZE *bmpsize) {
	HWND child = GetDlgItem(parent, child_id);
	RECT rect;

	GetWindowRect(child,&rect);
	ScreenToClient(parent, (LPPOINT)&rect.left);
	ScreenToClient(parent, (LPPOINT)&rect.right);

	SDlgSetBitmap(
		child,
		NULL,
		TEXT("Static"),
		SDLG_STYLE_ANY,
		SDLG_USAGE_BACKGROUND,
		bmpbits,
		&rect,
		bmpsize->cx,
		bmpsize->cy
	);
	return 1;
}


//****************************************************************************
//****************************************************************************
void UiSetBtnBmp(HWND button, DWORD usage, RECT *rect, LPBYTE bmpbits, SIZE *bmpsize) {
	SDlgSetBitmap(
		button,
		NULL,
		TEXT("Button"),
		SDLG_STYLE_ANY,
		usage,
		bmpbits,
		rect,
		bmpsize->cx,
		bmpsize->cy
	);
}


//****************************************************************************
//****************************************************************************
void UiSetButtonBitmaps(HWND parent, int *btn_ids, LPBYTE bmpbits, SIZE *bmpsize) {
	LONG yoffset = 0;
	HWND child;
	RECT rect;

	for ( ; *btn_ids; btn_ids++) {
		// set up the button bitmaps so the use different parts of the texture
		child = GetDlgItem(parent, *btn_ids);
		if (! child) continue;

		GetClientRect(child, &rect);
		
		UiCalcNewRect(&rect, 0, yoffset);
		UiSetBtnBmp(child, SDLG_USAGE_NORMAL_UNFOCUSED, &rect, bmpbits, bmpsize);
		rect.bottom++;
		rect.right++;
		yoffset = rect.bottom;

		UiCalcNewRect(&rect, 0, yoffset);
		UiSetBtnBmp(child, SDLG_USAGE_SELECTED_UNFOCUSED, &rect, bmpbits, bmpsize);
		rect.bottom++;
		rect.right++;
		yoffset = rect.bottom;

		UiCalcNewRect(&rect, 0, yoffset);
		UiSetBtnBmp(child, SDLG_USAGE_NORMAL_FOCUSED, &rect, bmpbits, bmpsize);
		rect.bottom++;
		rect.right++;
		yoffset = rect.bottom;

		UiCalcNewRect(&rect, 0, yoffset);
		UiSetBtnBmp(child, SDLG_USAGE_SELECTED_FOCUSED, &rect, bmpbits, bmpsize);
		rect.bottom++;
		rect.right++;
		yoffset = rect.bottom;

		UiCalcNewRect(&rect, 0, yoffset);
		UiSetBtnBmp(child, SDLG_USAGE_GRAYED, &rect, bmpbits, bmpsize);
		rect.bottom++;
		rect.right++;
		yoffset = rect.bottom;
	}
}


//****************************************************************************
//****************************************************************************
void UiSetTextYellow(HDC hdc) {
	SetTextColor(hdc, RGB(0xff, 0xff, 0x00));
}


//****************************************************************************
//****************************************************************************
BOOL UiIsPtInWindow(HWND parent, HWND window, int xpos, int ypos) {
	RECT rect;

	if (! parent) return FALSE;
	if (! window) return FALSE;

	GetWindowRect(window, &rect);
	ScreenToClient(parent, (LPPOINT)&rect.left);
	ScreenToClient(parent, (LPPOINT)&rect.right);

	if (xpos < rect.left) return FALSE;
	if (xpos >= rect.right) return FALSE;
	if (ypos < rect.top) return FALSE;
	if (ypos >= rect.bottom) return FALSE;

	return TRUE;
}


//****************************************************************************
//****************************************************************************
void UiPaintBtn(HWND window) {
	PAINTSTRUCT ps;
	HDC dc = SDlgBeginPaint(window, &ps);
	SDlgEndPaint(window, &ps);
}


//****************************************************************************
//****************************************************************************
static LRESULT CALLBACK OnPaintBtnProc (HWND   window,
										UINT   message,
										WPARAM wparam,
										LPARAM lparam) {
	LRESULT result;
	WNDPROC oldproc = (WNDPROC) GetProp(window, WNDPROCPROP);

	switch (message) {
		case WM_DESTROY:
			RemoveProp(window, WNDPROCPROP);
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
void UiOnPaintBtns(HWND window, int * BtnIDs) {
	HWND child;
	WNDPROC oldproc;

	for (; *BtnIDs; BtnIDs++) {
		child = GetDlgItem(window, *BtnIDs);
		if (! child) continue;

		oldproc = (WNDPROC) GetWindowLong(child, GWL_WNDPROC);
		SetProp(child, WNDPROCPROP,(HANDLE) oldproc);
		SetWindowLong(child, GWL_WNDPROC, (LONG) OnPaintBtnProc);
	}
}


//****************************************************************************
//****************************************************************************
static LRESULT CALLBACK ActiveDownBtnProc (HWND   window,
											UINT   message,
											WPARAM wparam,
											LPARAM lparam) {
	LRESULT result;
	HWND oldfocus;
	WNDPROC oldproc = (WNDPROC) GetProp(window, WNDPROCPROP);

	switch (message) {
#if TEST_REDRAW
		case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS;

		case WM_KEYDOWN:
			if (wparam == VK_SCROLL) {
				SDrawClearSurface(SDRAW_SURFACE_FRONT);
			}
		break;
#endif
		case WM_LBUTTONDOWN:
			// set the focus to this window
			oldfocus = GetFocus();
			SetFocus(window);

			// make sure the windows involved in focus changing update
			InvalidateRect(oldfocus, NULL, FALSE);
			InvalidateRect(window, NULL, FALSE);
			UpdateWindow(oldfocus);
			UpdateWindow(window);

			// notify the parent of the click
			PostMessage(
				GetParent(window),
				WM_COMMAND,
				MAKELONG(GetWindowLong(window, GWL_ID), BN_CLICKED),
				(LPARAM) window
			);
		return 0;

		case WM_DESTROY:
			RemoveProp(window, WNDPROCPROP);
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
void UiActiveDownBtns(HWND window, int * BtnIDs) {
	HWND child;
	WNDPROC oldproc;

	for (; *BtnIDs; BtnIDs++) {
		child = GetDlgItem(window, *BtnIDs);
		if (! child) continue;

		oldproc = (WNDPROC) GetWindowLong(child, GWL_WNDPROC);
		SetProp(child, WNDPROCPROP,(HANDLE) oldproc);
		SetWindowLong(child, GWL_WNDPROC, (LONG) ActiveDownBtnProc);
	}
}


//****************************************************************************
//****************************************************************************
void UiFlushIOMessages(HWND window) {
	MSG msg;

	while (
		PeekMessage(&msg, window, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE) ||
		PeekMessage(&msg, window, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE)
	);
}


//****************************************************************************
//****************************************************************************
TPBMP UiAllocBmp(void) {
	TPBMP tpBmp = (TPBMP) ALLOC(sizeof(TBMP));

	if (tpBmp) {
		tpBmp->data			= NULL;
		tpBmp->datasize.cx	= 0;
		tpBmp->datasize.cy	= 0;
		tpBmp->userdata		= 0;
		tpBmp->text[0]		= 0;
	}
	return tpBmp;
}


//****************************************************************************
//****************************************************************************
void UiFreeBmp(TPBMP tpBmp) {
	if (tpBmp) {
		if (tpBmp->data) {
			FREE(tpBmp->data);
			tpBmp->data = 0;
		}
		FREE(tpBmp);
	}
}


//****************************************************************************
//****************************************************************************
void UiSetBmpText(TPBMP tpBmp, LPCSTR text) {
	if (! tpBmp) return;

	if (text) {
		strncpy(tpBmp->text, text, BMP_TEXT_LEN-1);
		tpBmp->text[BMP_TEXT_LEN-1] = 0;
	}
	else {
		tpBmp->text[0] = 0;
	}
}


//****************************************************************************
//****************************************************************************
#define CURSOR_FILE		TEXT("ui_art\\cursor.pcx")
static LPBYTE sgCurData = NULL;
static LPBYTE sgCurMask = NULL;
static SIZE   sgCurSize;

//****************************************************************************
static void UiGenCursorMask(void) {
	LPBYTE data = sgCurData;
	LPBYTE mask = sgCurMask;

	if (! mask) return;
	if (! data) return;

	for (int i = 0; i < (sgCurSize.cx * sgCurSize.cy); i++) {
		if (*data++) {
			*mask++ = 0;
		}
		else {
			*mask++ = 255;
		}
	}
}

//****************************************************************************
void UiLoadCursor(void) {
	int width, height;

	// determine the size of the curspr
	if (!SBmpLoadImage(CURSOR_FILE,NULL,NULL,0,&width,&height))
		return;
	
	// allocate space for the data and mask
	sgCurData = (LPBYTE)ALLOC(width*height);
	
	if (!sgCurData)
		return;
		
	sgCurMask = (LPBYTE)ALLOC(width*height);
	if (!sgCurMask) {
		FREE(sgCurData);
		sgCurData = NULL;
		return;
	}

	// load the cursor data
	if (!SBmpLoadImage(CURSOR_FILE, NULL, sgCurData, width*height)) {
		FREE(sgCurData);
		sgCurData = NULL;
		FREE(sgCurMask);
		sgCurMask = NULL;
		return;
	}

	// save the cursor size
	sgCurSize.cx = width;
	sgCurSize.cy = height;

	// generate a mask for the cursor
	UiGenCursorMask();
}

//****************************************************************************
void UiDestroyCursor(void) {
	if (sgCurData) {
		FREE(sgCurData);
		sgCurData = NULL;
	}
	if (sgCurMask) {
		FREE(sgCurMask);
		sgCurMask = NULL;
	}
}

//****************************************************************************
void UiSetCursor(void) {
	if (! sgCurData) {
		UiLoadCursor();
	}
	SDlgSetSystemCursor(sgCurMask, sgCurData, &sgCurSize, OCR_NORMAL);
}

//****************************************************************************
void UiClearCursor(void) {
	SDlgSetSystemCursor(NULL, NULL, NULL, OCR_NORMAL);
}


//****************************************************************************
void UiSetWindowsCursors(HWND hWnd) {
	HCURSOR OldArrow = NULL;

	UiClearCursor();	// Make sure Storm cursor has been turned off
	SDlgSetCursor(hWnd, LoadCursor(global_hinstance, "DIABLOCURSOR"), OCR_NORMAL, &OldArrow);
}


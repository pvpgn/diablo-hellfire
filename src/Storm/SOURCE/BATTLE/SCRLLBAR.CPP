/***************************************************************************
*
*  scrllbar.cpp
*
*  By Michael Morhaime
*
*  Custom Scrollbar control used by listbox.
*
*
*
* NOTES: ScrollbarLink()  must be called to setup the link between the scrollbar and the
*						  window that will receive notify messages.
*
*		 ListUpdateScrollbar()  This routine should be called whenever the scroll position,
*								scroll range, or page size changes. The scrollbar must
*								be notified whenever the listbox generates LBN_SELCHANGE
*								messages as they may be the result of a scroll. The scrollbar 
*								will query the listbox to get the appropriate information.
*								You could also send the scrollbar SBM_SETSCROLLINFO messages
*								manually if you wish. 
*
*		EditUpdateScrollbar()   This is the equivalent of the above routine when the linked
*								window is an Edittext control.
***/

#include "pch.h"



#define NUM_ARROWS 4		// Number of arrows expected in art file (up/down)
#define REPEAT_RATE	100		// repeat every 100 millisec's

static int sgnCreateCnt = 0;		// Allow more than one scrollbar to share artwork
static LPBYTE sgBmpArrows = NULL;
static LPBYTE sgBmpThumb = NULL;
static LPBYTE sgBmpBar = NULL;

static SIZE sgSizeArrows;
static SIZE sgSizeThumb;
static SIZE sgSizeBar;

static int sgnArrowHgt;
static int sgnThumbHgt;
static int sgnBarHgt;

static HWND sghPrevCapture;

typedef struct _ScrollBmp {
	LPBYTE data;
	SIZE datasize;
} TSCROLLBMP, * PTSCROLLBMP;

typedef struct _ScrollRects {
	RECT rectArrowUp;
	RECT rectArrowDown;
	RECT rectThumb;
	RECT rectBar;
} TSCROLLRECTS, * PTSCROLLRECTS;

typedef struct _ScrollDrag {
	union {
		unsigned fModes;
		struct _clicked {
			unsigned ArrowUp	:1;
			unsigned ArrowDown	:1;
			unsigned PageUp		:1;
			unsigned PageDown	:1;
			unsigned Thumb		:1;
		} Clicked;
	} dm;
} TSCROLLDRAG, * PTSCROLLDRAG;

#define PROP_SCROLLBMP		"ScrollBitmap"
#define PROP_SCROLLRECTS	"ScrollRects"
#define PROP_SCROLLPARENT	"ScrollParent"
#define PROP_SCROLLINFO		"ScrollInfo"
#define PROP_SCROLLDRAG		"ScrollDrag"
										

//****************************************************************************
//****************************************************************************
void ScrollbarLoadArtwork(SNETGETARTPROC artcallback) {
	if (!sgnCreateCnt++) {

		UiLoadArtwork(
				artcallback,
				NULL,
				NULL,
				SNET_ART_SCROLLBARARROWS,
				TEXT(""),
				SDLG_STYLE_ANY,
				SDLG_USAGE_BACKGROUND,
				FALSE,
				FALSE,
				&sgBmpArrows,
				&sgSizeArrows);

		UiLoadArtwork(
				artcallback,
				NULL,
				NULL,
				SNET_ART_SCROLLTHUMB,
				TEXT(""),
				SDLG_STYLE_ANY,
				SDLG_USAGE_BACKGROUND,
				FALSE,
				FALSE,
				&sgBmpThumb,
				&sgSizeThumb);

		UiLoadArtwork(
				artcallback,
				NULL,
				NULL,
				SNET_ART_SCROLLBAR,
				TEXT(""),
				SDLG_STYLE_ANY,
				SDLG_USAGE_BACKGROUND,
				FALSE,
				FALSE,
				&sgBmpBar,
				&sgSizeBar);

		// Calculate pixel heights of components
		sgnThumbHgt = sgSizeThumb.cy;
		sgnArrowHgt = sgSizeArrows.cy/NUM_ARROWS;
		sgnBarHgt = sgSizeBar.cy;
	}

}

//****************************************************************************
//****************************************************************************
void ScrollbarDestroyArtwork(void) {
	// Only Destroy if this if the only scrollbar currently using the artwork
	if (--sgnCreateCnt)
		return;

	if (sgBmpArrows) {
		FREE(sgBmpArrows);
		sgBmpArrows = NULL;
	}

	if (sgBmpThumb) {
		FREE(sgBmpThumb);
		sgBmpThumb = NULL;
	}

	if (sgBmpBar) {
		FREE(sgBmpBar);
		sgBmpBar = NULL;
	}

}
//****************************************************************************
//****************************************************************************
static BOOL ScrollbarCreate(HWND window) {
	RECT rWnd;
	PTSCROLLBMP	pTBmp = NULL;
	PTSCROLLRECTS	pTRects = NULL;
	LPSCROLLINFO pScrollInfo = NULL;
	PTSCROLLDRAG pScrollDrag = NULL;


	if (!sgBmpArrows || !sgBmpThumb | !sgBmpBar) 
		return 0;

	GetClientRect(window, &rWnd);
	pTBmp = (PTSCROLLBMP) ALLOC(sizeof(TSCROLLBMP));
	if (!pTBmp) 
		return 0;

	pTBmp->data = (LPBYTE) ALLOC(rWnd.right*rWnd.bottom);
	if (!pTBmp->data) {
		FREE(pTBmp);
		return 0;
	}

	ZeroMemory(pTBmp->data, rWnd.right*rWnd.bottom);
	memset(pTBmp->data, 2, rWnd.right*rWnd.bottom);  //xxx del

	pTBmp->datasize.cx = rWnd.right;
	pTBmp->datasize.cy = rWnd.bottom;
	SetProp(window, PROP_SCROLLBMP, (HANDLE) pTBmp);


	SDlgSetBitmap(
		window,
		NULL,
		NULL,
		SDLG_STYLE_ANY,
		SDLG_USAGE_BACKGROUND,
		pTBmp->data,
		NULL,
		rWnd.right,
		rWnd.bottom
	);


	// Initialize hit detection rects
	pTRects = (PTSCROLLRECTS) ALLOC(sizeof(TSCROLLRECTS));
	if (!pTRects) 
		return 0;

	SetRect(&pTRects->rectArrowUp, 0, 0, rWnd.right, sgnArrowHgt);
	SetRect(&pTRects->rectArrowDown, 0, rWnd.bottom - sgnArrowHgt, rWnd.right, rWnd.bottom);
	SetRect(&pTRects->rectThumb, 0, sgnArrowHgt, rWnd.right, sgnArrowHgt + sgnThumbHgt);
	SetRect(&pTRects->rectBar, 0, sgnArrowHgt, rWnd.right, rWnd.bottom - sgnArrowHgt);
	SetProp(window, PROP_SCROLLRECTS, (HANDLE) pTRects);


	pScrollInfo = (SCROLLINFO *)ALLOC(sizeof(SCROLLINFO));
	if (!pScrollInfo) 
		return 0;
	ZeroMemory(pScrollInfo, sizeof(SCROLLINFO));
	pScrollInfo->cbSize = sizeof(SCROLLINFO);
	SetProp(window, PROP_SCROLLINFO, (HANDLE) pScrollInfo);


	pScrollDrag = (PTSCROLLDRAG)ALLOC(sizeof(TSCROLLDRAG));
	if (!pScrollDrag)
		return 0;
	ZeroMemory(pScrollDrag, sizeof(TSCROLLDRAG));
	SetProp(window, PROP_SCROLLDRAG, (HANDLE) pScrollDrag);

	return 1;
}


//****************************************************************************
//****************************************************************************
static void ScrollbarDestroy(HWND window) {
	PTSCROLLBMP pTBmp;
	PTSCROLLRECTS pTRects;
	PTSCROLLDRAG pScrollDrag;
	SCROLLINFO *pScrollInfo;


	pTBmp = (PTSCROLLBMP)RemoveProp(window, PROP_SCROLLBMP);
	if (pTBmp) {
		if (pTBmp->data) {
			FREE(pTBmp->data);
		}
		FREE(pTBmp);
	}

	pTRects = (PTSCROLLRECTS)RemoveProp(window, PROP_SCROLLRECTS);
	if (pTRects) 
		FREE(pTRects);

	pScrollInfo = (SCROLLINFO *)RemoveProp(window, PROP_SCROLLINFO);
	if (pScrollInfo)
		FREE(pScrollInfo);

	pScrollDrag = (PTSCROLLDRAG)RemoveProp(window, PROP_SCROLLDRAG);
	if (pScrollDrag)
		FREE(pScrollDrag);

	RemoveProp(window, PROP_SCROLLPARENT);
}

//****************************************************************************
//****************************************************************************
static void ScrollbarDraw (HWND window) {
	PTSCROLLBMP pTBmp;
	PTSCROLLRECTS pTRects;
	
	RECT r;

	pTBmp = (PTSCROLLBMP)GetProp(window, PROP_SCROLLBMP);
	pTRects = (PTSCROLLRECTS)GetProp(window, PROP_SCROLLRECTS);

	if (!pTBmp || !pTRects)
		return;

	// Draw Arrows
	r = pTRects->rectArrowUp;
   	SBltROP3 (
		pTBmp->data + (r.top * pTBmp->datasize.cx) + r.left,
   		sgBmpArrows,
		r.right - r.left,
		r.bottom - r.top,
		pTBmp->datasize.cx,
		sgSizeArrows.cx,
		NULL,
		SRCCOPY
   	);

	r = pTRects->rectArrowDown;
   	SBltROP3 (
		pTBmp->data + (r.top * pTBmp->datasize.cx) + r.left,
   		sgBmpArrows + (sgnArrowHgt*sgSizeArrows.cx),
		r.right - r.left,
		r.bottom - r.top,
		pTBmp->datasize.cx,
		sgSizeArrows.cx,
		NULL,
		SRCCOPY
   	);

	RECT rSrc, rDst;
	SetRect(&rSrc, 0, 0, pTBmp->datasize.cx, sgSizeBar.cy);
	rDst = pTRects->rectBar;

	SBltROP3Tiled (
		pTBmp->data, 
		&rDst,
		pTBmp->datasize.cx,
		sgBmpBar,
		&rSrc,
		sgSizeBar.cx,
		0,
		0,
		NULL,
		SRCCOPY);

	r = pTRects->rectThumb;
   	SBltROP3 (
		pTBmp->data + (r.top * pTBmp->datasize.cx) + r.left,
   		sgBmpThumb,
		r.right - r.left,
		r.bottom - r.top,
		pTBmp->datasize.cx,
		sgSizeThumb.cx,
		NULL,
		SRCCOPY
   	);
}


//****************************************************************************
//****************************************************************************
int MouseToScrollPos(POINT *pt, PTSCROLLRECTS  pTRects, LPSCROLLINFO pScrollInfo) {
	int nScrollRange;
	int nPixRange;
	int nScrollPos;
	int y;



	nScrollRange = pScrollInfo->nMax - pScrollInfo->nMin;
	if (nScrollRange == 0)
		return 0;

	// Make sure we don't treat a negative y value as a large positive value.
	y = (int)pt->y;
	if (y < pTRects->rectBar.top)
		y = pTRects->rectBar.top;

	nPixRange = pTRects->rectBar.bottom - pTRects->rectBar.top - sgnThumbHgt;
	nScrollPos = (y - pTRects->rectBar.top) * nScrollRange / nPixRange;
	nScrollPos += pScrollInfo->nMin;

	// bound scrollpos to scroll range
	if (nScrollPos < pScrollInfo->nMin)  
		nScrollPos = pScrollInfo->nMin;
	if (nScrollPos > pScrollInfo->nMax)  
		nScrollPos = pScrollInfo->nMax;

	return nScrollPos;
}


//****************************************************************************
//****************************************************************************
static void ScrollbarPaint (HWND window) {
	PAINTSTRUCT ps;
	HDC         dc = BeginPaint(window,&ps);

	// PAINT THE BITMAP
	SDlgDrawBitmap(window,SDLG_USAGE_BACKGROUND,(HRGN)0);
	EndPaint(window,&ps);
}



//****************************************************************************
//****************************************************************************
static void ScrollbarUpdate(HWND window) {
	SCROLLINFO *pScrollInfo = (SCROLLINFO *) GetProp(window, PROP_SCROLLINFO);
	PTSCROLLRECTS pTRects = (PTSCROLLRECTS) GetProp(window, PROP_SCROLLRECTS);
	int nScrollRange, nPixRange;
	int nNewPos;

	if (!pScrollInfo)
		return;

	// Update Thumb position
	nScrollRange = pScrollInfo->nMax - pScrollInfo->nMin;
	if (nScrollRange == 0) {
		ShowWindow(window, SW_HIDE);
		return;
	}

	nPixRange = pTRects->rectBar.bottom - pTRects->rectBar.top - sgnThumbHgt;
	nNewPos = (pScrollInfo->nPos-pScrollInfo->nMin) * nPixRange / nScrollRange;
	nNewPos += pTRects->rectBar.top;
	if (nNewPos == pTRects->rectThumb.top)
		return;

	OffsetRect(&pTRects->rectThumb, 0, nNewPos - pTRects->rectThumb.top);
	ScrollbarDraw(window);
	InvalidateRect(window, NULL, FALSE);
}


//****************************************************************************
//****************************************************************************
void ScrollDragThumb(HWND window, POINT *pt, PTSCROLLRECTS pTRects, LPSCROLLINFO pScrollInfo) {
	int nTarget;

	// Position cursor in middle of thumb
	pt->y -= sgnThumbHgt/2;

	// Move the thumb to where the mouse is.
	if (pt->y < pTRects->rectBar.top)
		nTarget = pTRects->rectBar.top;
	else if (pt->y > pTRects->rectBar.bottom - sgnThumbHgt)
		nTarget = pTRects->rectBar.bottom - sgnThumbHgt;
	else
		nTarget = pt->y;

	OffsetRect(&pTRects->rectThumb, 0, nTarget - pTRects->rectThumb.top);
	ScrollbarDraw(window);
	InvalidateRect(window, NULL, FALSE);  //&pTRects->rectThumb, FALSE);
}

//****************************************************************************
//****************************************************************************
static void ScrollbarScroll(HWND window, int nAmount) {
	SCROLLINFO *pScrollInfo = (SCROLLINFO *) GetProp(window, PROP_SCROLLINFO);
	int nPosSave;

	if (!pScrollInfo)
		return;

	nPosSave = pScrollInfo->nPos;
	pScrollInfo->nPos += nAmount;
	if (pScrollInfo->nPos < pScrollInfo->nMin)  
		pScrollInfo->nPos = pScrollInfo->nMin;
	if (pScrollInfo->nPos > pScrollInfo->nMax)  
		pScrollInfo->nPos = pScrollInfo->nMax;

	if (nPosSave != pScrollInfo->nPos) 
		ScrollbarUpdate(window);
}



//****************************************************************************
//****************************************************************************
static BOOL ScrollbarSetPos(HWND window, int nPos, BOOL bRedraw) {
	LPSCROLLINFO pScrollInfo = (LPSCROLLINFO) GetProp(window, PROP_SCROLLINFO);
	int nPosSave;

	if (!pScrollInfo)
		return 0;

	nPosSave = pScrollInfo->nPos;
	pScrollInfo->nPos = nPos;
	if (pScrollInfo->nPos < pScrollInfo->nMin)  
		pScrollInfo->nPos = pScrollInfo->nMin;
	if (pScrollInfo->nPos > pScrollInfo->nMax)  
		pScrollInfo->nPos = pScrollInfo->nMax;

	if (nPosSave != pScrollInfo->nPos) {
		ScrollbarUpdate(window);
		return nPosSave;
	}

	return 0;

}

//****************************************************************************
//****************************************************************************
static BOOL ScrollbarSetRange(HWND window, int nMin, int nMax) {
	LPSCROLLINFO pScrollInfo = (LPSCROLLINFO) GetProp(window, PROP_SCROLLINFO);
	int nSavePos;

	if (!pScrollInfo)
		return 0;

	if (pScrollInfo->nMin == nMin && pScrollInfo->nMax == nMax) {
		return 0;
	}

	if (nMin == nMax && pScrollInfo->nMin != pScrollInfo->nMax) {
		if (!(pScrollInfo->fMask & SIF_DISABLENOSCROLL)) 
			ShowWindow(window, SW_HIDE);
		else
			EnableWindow(window, FALSE);
	}
	else if (nMin != nMax && pScrollInfo->nMin == pScrollInfo->nMax) {
		EnableWindow(window, TRUE);
		ShowWindow(window, SW_SHOW);
	}

	nSavePos = pScrollInfo->nPos;
	pScrollInfo->nMin = nMin;
	pScrollInfo->nMax = nMax;
	if (pScrollInfo->nPos < pScrollInfo->nMin)
		pScrollInfo->nPos = pScrollInfo->nMin;
	if (pScrollInfo->nPos > pScrollInfo->nMax)
		pScrollInfo->nPos = pScrollInfo->nMax;
	ScrollbarUpdate(window);
	return nSavePos;
}

//****************************************************************************
//****************************************************************************
static BOOL ScrollbarSetInfo(HWND window, LPSCROLLINFO lpsi) {
	LPSCROLLINFO pScrollInfo = (LPSCROLLINFO) GetProp(window, PROP_SCROLLINFO);

	if (!pScrollInfo)
		return 0;

	if (lpsi->fMask & SIF_PAGE) 
		pScrollInfo->nPage = lpsi->nPage;

	if (lpsi->fMask & SIF_RANGE) 
		ScrollbarSetRange(window, lpsi->nMin, lpsi->nMax);

	if (lpsi->fMask & SIF_POS) 
		ScrollbarSetPos(window, lpsi->nPos, TRUE);


	return pScrollInfo->nPos;
}


//****************************************************************************
//****************************************************************************
static void ScrollbarBtnDown(HWND window, int x, int y) {
	PTSCROLLRECTS pTRects;
	SCROLLINFO *pScrollInfo;
	PTSCROLLDRAG pScrollDrag;
	POINT pt;
	HWND hWndParent;
	int nNotify;

	// do hit detection
	pTRects = (PTSCROLLRECTS) GetProp(window, PROP_SCROLLRECTS);
	hWndParent = (HWND) GetProp(window, PROP_SCROLLPARENT);
	pScrollInfo = (SCROLLINFO *) GetProp(window, PROP_SCROLLINFO);
	pScrollDrag = (PTSCROLLDRAG) GetProp(window, PROP_SCROLLDRAG);

	if (!pTRects || !hWndParent || !pScrollInfo || !pScrollDrag)
		return;


	pt.x = x;
	pt.y = y;

	// Capture the mouse until the user releases the button
	if (window != GetCapture()) 
		sghPrevCapture = SetCapture(window);
	else {
		// Are we scrolling using the thumbnail?
		if (pScrollDrag->dm.Clicked.Thumb) {
			ScrollDragThumb(window, &pt, pTRects, pScrollInfo);
			SendMessage(
				hWndParent, 
				WM_VSCROLL, 
				MAKEWPARAM(SB_THUMBTRACK,MouseToScrollPos(&pt, pTRects, pScrollInfo)),
				(LPARAM)window);
			return;
		}
	}



	if (PtInRect(&pTRects->rectArrowUp, pt)) {
		if (pScrollDrag->dm.fModes && !pScrollDrag->dm.Clicked.ArrowUp)
			return;

		if (!pScrollDrag->dm.Clicked.ArrowUp) {
			// Set repeat rate for scrolling, and set mode flag
			SDlgSetTimer(window, 0, REPEAT_RATE, NULL);
			pScrollDrag->dm.Clicked.ArrowUp = 1;
		}

		ScrollbarScroll(window, -1);
		nNotify = MAKEWPARAM(SB_LINEUP,0);
	}
	else if (PtInRect(&pTRects->rectArrowDown, pt)) {
		if (pScrollDrag->dm.fModes && !pScrollDrag->dm.Clicked.ArrowDown)
			return;

		if (!pScrollDrag->dm.Clicked.ArrowDown) {
			// Set repeat rate for scrolling, and set mode flag
			SDlgSetTimer(window, 0, REPEAT_RATE, NULL);
			pScrollDrag->dm.Clicked.ArrowDown = 1;
		}

		ScrollbarScroll(window, 1);
		nNotify = MAKEWPARAM(SB_LINEDOWN,0);
	}
	else if (PtInRect(&pTRects->rectBar, pt)) {
		if (PtInRect(&pTRects->rectThumb, pt)) {
			if (!pScrollDrag->dm.fModes) {
				// Thumb doesn't use timer, just set a flag, we'll get mousemove because of GetCapture()
				pScrollDrag->dm.Clicked.Thumb = 1;
				return;
			}
		}
		else if (pt.y < pTRects->rectThumb.top) {
			if (pScrollDrag->dm.fModes && !pScrollDrag->dm.Clicked.PageUp) 
				return;

			if (!pScrollDrag->dm.Clicked.PageUp) {
				// Set repeat rate for scrolling, and set mode flag
				SDlgSetTimer(window, 0, REPEAT_RATE, NULL);
				pScrollDrag->dm.Clicked.PageUp = 1;
			}

			ScrollbarScroll(window, -(int)pScrollInfo->nPage);
			nNotify = MAKEWPARAM(SB_THUMBPOSITION,pScrollInfo->nPos);
		}
		else {
			if (pScrollDrag->dm.fModes && !pScrollDrag->dm.Clicked.PageDown)
				return;

			if (!pScrollDrag->dm.Clicked.PageDown) {
				// Set repeat rate for scrolling, and set mode flag
				SDlgSetTimer(window, 0, REPEAT_RATE, NULL);
				pScrollDrag->dm.Clicked.PageDown = 1;
			}

			// page down
			ScrollbarScroll(window, pScrollInfo->nPage);
			nNotify = MAKEWPARAM(SB_THUMBPOSITION,pScrollInfo->nPos);
		}
	}
	else {
		// User didn't click in any hot regions
		return;
	}


	// Notify attached  window what to do
	SendMessage(hWndParent, WM_VSCROLL, nNotify, (LPARAM) window);
}

//****************************************************************************
//****************************************************************************
static void ScrollbarBtnUp(HWND window, int x, int y) {
	// End capture, 
	if (window == GetCapture())	{
		PTSCROLLDRAG pScrollDrag;
		LPSCROLLINFO pScrollInfo;
		PTSCROLLRECTS pTRects;
		HWND hWndParent;

		pScrollDrag = (PTSCROLLDRAG) GetProp(window, PROP_SCROLLDRAG);
		pScrollInfo = (LPSCROLLINFO) GetProp(window, PROP_SCROLLINFO);
		pTRects     = (PTSCROLLRECTS) GetProp(window, PROP_SCROLLRECTS);
		hWndParent = (HWND) GetProp(window, PROP_SCROLLPARENT);

		ReleaseCapture();
		if (sghPrevCapture) SetCapture(sghPrevCapture);
		SDlgKillTimer(window, 0);

		if (!pScrollDrag || !pScrollInfo || !pTRects || !hWndParent)
			return;

		if (pScrollDrag->dm.Clicked.Thumb) {
			POINT pt;
			pt.x = x;
			pt.y = y;

			ScrollDragThumb(window, &pt, pTRects, pScrollInfo);
			pScrollInfo->nPos = MouseToScrollPos(&pt, pTRects, pScrollInfo);

			SendMessage(hWndParent, WM_VSCROLL, MAKEWPARAM(SB_ENDSCROLL,pScrollInfo->nPos), (LPARAM) window);
		}

		// Clear Drag Modes
		pScrollDrag->dm.fModes = 0;
	}

}

	
//****************************************************************************
//****************************************************************************
static LRESULT CALLBACK StormScrollbarWndProc (HWND   window,
												 UINT   message,
												 WPARAM wparam,
												 LPARAM lparam) {
	switch (message) {

        case WM_COMMAND:
            if (HIWORD(wparam) == LBN_SELCHANGE) {
                ListUpdateScrollbar((HWND)lparam);
                return 0;
            }
            break;
            
		case WM_CREATE:
			if (!ScrollbarCreate(window))
				return -1;

			ScrollbarDraw(window);
		break;

		case WM_DESTROY:
			ScrollbarDestroy(window);
		break;

		case WM_LBUTTONDOWN:
			// cast mouse coords to short to force sign extension
			ScrollbarBtnDown(window, (short)LOWORD(lparam), (short)HIWORD(lparam));
			return 0;
									 
		case WM_LBUTTONUP:
			// cast mouse coords to short to force sign extension
			ScrollbarBtnUp(window, (short)LOWORD(lparam), (short)HIWORD(lparam));
		break;

		case WM_MOUSEMOVE:
			if (window == GetCapture()) {
				PTSCROLLDRAG pScrollDrag = (PTSCROLLDRAG) GetProp(window, PROP_SCROLLDRAG);

				if (!pScrollDrag)
					break;

				// Only worry about MouseMove messages if we're dragging the thumb around
				if (pScrollDrag->dm.Clicked.Thumb) {
					// cast mouse coords to short to force sign extension
					ScrollbarBtnDown(window, (short)LOWORD(lparam), (short)HIWORD(lparam));
				}
			}
		break;

		case WM_PAINT:
			ScrollbarPaint(window);
		return 0;

		case WM_STORMSCROLL_INIT:
			SetProp(window, PROP_SCROLLPARENT, (HANDLE)lparam);
			ShowWindow(window, SW_HIDE);		// Default to hidden unless we support 'disabled' scrollbar
		return 0;

		case WM_TIMER:
			POINT pt;

			// Make sure we are still capturing the mouse
			if (window != GetCapture())
				return 0;
			GetCursorPos(&pt);
			ScreenToClient(window, &pt);
			ScrollbarBtnDown(window, pt.x, pt.y);
		return 0;

		case SBM_GETPOS: 
		break;

		case SBM_GETRANGE:
		break;

		case SBM_SETPOS:
		return ScrollbarSetPos(window, wparam, (BOOL)lparam);

		case SBM_SETRANGE:
		return ScrollbarSetRange(window, wparam, lparam);

		case SBM_SETRANGEREDRAW:
			BOOL bReturn;
			bReturn = ScrollbarSetRange(window, wparam, lparam);
		return bReturn;


		case SBM_SETSCROLLINFO:
		return ScrollbarSetInfo(window, (LPSCROLLINFO)lparam);

		case SBM_GETSCROLLINFO:
		return FALSE;



	}
	return DefWindowProc(window,message,wparam,lparam);
}


//****************************************************************************
//*
//*  EXPORTED FUNCTIONS
//*
//****************************************************************************

//****************************************************************************
int ScrollbarGetWidth(void) {
    return (int)sgSizeArrows.cx;
}

//****************************************************************************
void ScrollbarLink(HWND hWndList, HWND hWndScroll) {
	if (hWndList) {
		SendMessage(hWndScroll, WM_STORMSCROLL_INIT, 0, (LPARAM)hWndList);
		SetWindowLong(hWndList, GWL_USERDATA, (LONG)hWndScroll);
	}
}

//****************************************************************************
void ListUpdateScrollbar(HWND hWndList) {
	HWND hWndScroll;
	RECT r;
	int min, max;
	int nPixHt, nItems, nItemsInWindow;

	hWndScroll = (HWND)GetWindowLong(hWndList, GWL_USERDATA);
	if (hWndScroll == NULL)
		return;

	GetClientRect(hWndList, &r);
	nPixHt = SendMessage(hWndList, LB_GETITEMHEIGHT, 0, 0);
	nItems = SendMessage(hWndList, LB_GETCOUNT, 0, 0);
	if (!nPixHt)
		return;
	nItemsInWindow = r.bottom/nPixHt;

	GetClientRect(hWndList, &r);

	min = max = 0;
	if (nPixHt != LB_ERR && nItems != LB_ERR) {
		max = nItems - nItemsInWindow;

		if (max < 0)
			max = 0;
	}
	else {
		max = 0;
	}


	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
	si.nPos = SendMessage(hWndList, LB_GETTOPINDEX, 0, 0);
	si.nMin = min;
	si.nMax = max;
	si.nPage = nItemsInWindow-1;
	SendMessage(hWndScroll, SBM_SETSCROLLINFO, TRUE, (LPARAM)&si);
}

//****************************************************************************
void EditUpdateScrollbar(HWND hWndEdit) {
	HWND hWndScroll;
	int min, max;
	int nItems;

	hWndScroll = (HWND)GetWindowLong(hWndEdit, GWL_USERDATA);
	if (hWndScroll == NULL)
		return;

	nItems = SendMessage(hWndEdit, EM_GETLINECOUNT, 0, 0);

	min = 0;
	max = nItems-1;

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
	si.nPos = 0;	// don't know how to get current line from edit control!
	si.nMin = min;
	si.nMax = max;
	si.nPage = 10;	// arbitrary number
	SendMessage(hWndScroll, SBM_SETSCROLLINFO, TRUE, (LPARAM)&si);
}

//****************************************************************************
//****************************************************************************
void ScrollbarRegisterClass (void) {
	WNDCLASS wndclass;
	ZeroMemory(&wndclass,sizeof(WNDCLASS));
	wndclass.style         = CS_GLOBALCLASS; 
	wndclass.lpfnWndProc   = StormScrollbarWndProc;
	wndclass.hInstance     = global_hinstance;
	wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = "StormScrollbar";
	RegisterClass(&wndclass);
}

//****************************************************************************
//****************************************************************************
void ScrollbarUnregisterClass (void) {
	UnregisterClass("StormScrollbar", global_hinstance);
}

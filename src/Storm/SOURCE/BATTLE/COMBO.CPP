//===========================================================================//
// combo.cpp
// created 10.14.97
// written by Dan Liebgold
//
// Custom Storm SDlg ComboBox
//===========================================================================//


#include "pch.h"
#pragma hdrstop

#include <tchar.h>

// these values depend on the artwork
#define BEVEL_THICKNESS         3

#define PROP_COMBOLBOX          "ComboLbox"
#define PROP_COMBOEDIT          "ComboEdit"
#define PROP_COMBOEDITBKG       "ComboEditBkg"
#define PROP_COMBOARROWRCT      "ComboArrowRct"
#define PROP_LBOXSCROLL         "LboxScroll"
#define PROP_OLDPROC            "OldProc"

#define CUSTOM_EN_SCROLLDOWN    (WM_USER+2032)
#define CUSTOM_EN_SCROLLUP      (WM_USER+2033)

#define EDIT_ID                 7173
#define LBOX_ID                 7174
#define SCROLL_ID               7175

//---------------------------------------------------------------------------//
// PRIVATE
//---------------------------------------------------------------------------//

enum {
    LEFT = 0,
    MIDDLE,
    RIGHT,
    NUM_TILES
};

typedef struct _COMBOBMP {
    LPBYTE  bitmap;
    SIZE    size;
} COMBOBMP, *COMBOBMPPTR;

// Allow more than one combobox to share artwork
static int      sgnComboCnt = 0;
static LPBYTE   sgComboBmp[3];
static SIZE     sgComboSize[3];
static LPBYTE   sgSComboBmp[3];
static SIZE     sgSComboSize[3];
static BOOL     sgbPopup = FALSE;

//---------------------------------------------------------------------------//
static BOOL ClosePopup(HWND window) {
    if (!sgbPopup) return FALSE;
    sgbPopup = FALSE;

    HWND hLbox = (HWND)GetProp(window,PROP_COMBOLBOX);
    RECT comboRect,lboxRect;    
    GetWindowRect(window,&comboRect);
    GetWindowRect(hLbox,&lboxRect);

    ShowWindow(hLbox,SW_HIDE);
    ShowWindow((HWND)GetProp(window,PROP_LBOXSCROLL),SW_HIDE);

    SetWindowPos(window,
                 0,
                 0,
                 0,
                 comboRect.right - comboRect.left,
                 comboRect.bottom - comboRect.top -
                    (lboxRect.bottom - lboxRect.top + BEVEL_THICKNESS*2),
                 SWP_NOZORDER|SWP_NOMOVE);

    SendMessage(GetParent(window),
                WM_COMMAND,
                MAKEWPARAM(GetWindowLong(window,GWL_ID),
                           CBN_SELCHANGE),
                (LPARAM)window);
    return TRUE;
}

//---------------------------------------------------------------------------//
static BOOL OpenPopup(HWND window) {
    if (sgbPopup) return FALSE;
    sgbPopup = TRUE;

    HWND hLbox = (HWND)GetProp(window,PROP_COMBOLBOX);
    RECT comboRect,lboxRect;    
    GetWindowRect(window,&comboRect);
    GetWindowRect(hLbox,&lboxRect);
    
    SetWindowPos(window,
                 HWND_TOP,
                 0,
                 0,
                 comboRect.right - comboRect.left,
                 comboRect.bottom - comboRect.top +
                    (lboxRect.bottom - lboxRect.top + BEVEL_THICKNESS*2),
                 SWP_NOMOVE);

    ShowWindow(hLbox,SW_SHOWNORMAL);
    ShowWindow((HWND)GetProp(window,PROP_LBOXSCROLL),SW_SHOWNORMAL);

    SetFocus(hLbox);
    SetCapture(hLbox);
    return TRUE;
}

//---------------------------------------------------------------------------//
static LRESULT CALLBACK ComboEditWndProc(HWND hEdit,
                                         UINT message,
                                         WPARAM wparam,
                                         LPARAM lparam)
{
    WNDPROC oldProc = (WNDPROC)GetProp(hEdit,PROP_OLDPROC);
    if (message == WM_KEYDOWN) {
  	    HWND hCombo = GetParent(hEdit);
  	    if (wparam == VK_UP) {
  	        SendMessage(hCombo,
  	                    WM_COMMAND,
  	                    MAKEWPARAM(EDIT_ID,CUSTOM_EN_SCROLLUP),
  	                    (LPARAM)hEdit);
  	        return 0;
  	    }
  	    else if (wparam == VK_DOWN) {
  	        SendMessage(hCombo,
  	                    WM_COMMAND,
  	                    MAKEWPARAM(EDIT_ID,CUSTOM_EN_SCROLLDOWN),
  	                    (LPARAM)hEdit);
  	        return 0;
  	    }
    }
    return CallWindowProc(oldProc,hEdit,message,wparam,lparam);
}

//---------------------------------------------------------------------------//
static LRESULT CALLBACK ComboLboxWndProc(HWND hLbox,
                                         UINT message,
                                         WPARAM wparam,
                                         LPARAM lparam)
{
    static bInScroll = FALSE;
    WNDPROC oldProc = (WNDPROC)GetProp(hLbox,PROP_OLDPROC);
    switch (message) {
        case WM_SHOWWINDOW:
            bInScroll = FALSE;
            break;

        case WM_LBUTTONDOWN: {
            RECT lboxRect,scrollRect;
            HWND hScroll = (HWND)GetProp(GetParent(hLbox),PROP_LBOXSCROLL);

            GetClientRect(hLbox,&lboxRect);
            GetClientRect(hScroll,&scrollRect);
            scrollRect.left += lboxRect.right;
            scrollRect.right += lboxRect.right;

            POINT pt = { (short)LOWORD(lparam), (short)HIWORD(lparam) };
            if (PtInRect(&scrollRect,pt)) {
                LPARAM newlparam = MAKELPARAM(pt.x - lboxRect.right,
                                              pt.y - lboxRect.top);
                return SendMessage(hScroll,message,wparam,newlparam);
            }
            if (!PtInRect(&lboxRect,pt)) {
                ReleaseCapture();
                ClosePopup(GetParent(hLbox));
                return 0;
            }
            break;
        }

        case WM_LBUTTONUP: {
            LRESULT result = CallWindowProc(oldProc,
                                            hLbox,
                                            message,
                                            wparam,
                                            lparam);
            ReleaseCapture();
            ClosePopup(GetParent(hLbox));
            return result;
        }

        case WM_KILLFOCUS:
            bInScroll = FALSE;
            ReleaseCapture();
            ClosePopup(GetParent(hLbox));
            return 0;
    }
    return CallWindowProc(oldProc,hLbox,message,wparam,lparam);
}

//---------------------------------------------------------------------------//
static void UpdateSelection(HWND hLbox, HWND hEdit) {
    int idx = SendMessage(hLbox,LB_GETCURSEL,0,0);
    if (idx == LB_ERR) return;
    TCHAR szBuf[256];
    SendMessage(hLbox,LB_GETTEXT,(WPARAM)idx,(LPARAM)szBuf);
    SendMessage(hEdit,WM_SETTEXT,0,(LPARAM)szBuf);
    if (!(GetWindowLong(GetParent(hLbox),GWL_STYLE) & CBS_DROPDOWNLIST))
        SendMessage(hEdit,EM_SETSEL,0,(LPARAM)-1);
    ListUpdateScrollbar(hLbox);
}

//---------------------------------------------------------------------------//
static BOOL ComboCreate(HWND window) {
    RECT comboRect,dlgRect,comboRelRect;
    LPBYTE pBmp[3];
    LPSIZE pSize[3];
    int editHgt;
    BOOL bUp = TRUE;

    GetWindowRect(GetParent(window),&dlgRect);
    GetWindowRect(window,&comboRect);
    GetClientRect(window,&comboRelRect);

    //
    // Reposition the combo to its full size. We'll hide the popup
    //  (bottom) later.
    //

    editHgt = comboRelRect.bottom - BEVEL_THICKNESS*2;
    if (comboRect.top + sgComboSize[0].cy > GetSystemMetrics(SM_CYSCREEN)) {
        // use short popup
        comboRelRect.bottom = sgSComboSize[0].cy;
        for (int i = 0; i < 3; ++i) {
            pBmp[i] = sgSComboBmp[i];
            pSize[i] = &sgSComboSize[i];
        }
    }
    else {
        comboRelRect.bottom = sgComboSize[0].cy;
        for (int i = 0; i < 3; ++i) {
            pBmp[i] = sgComboBmp[i];
            pSize[i] = &sgComboSize[i];
        }
    }

    int mod;
    if (0 != (mod = comboRelRect.right % pSize[0]->cx))
        comboRelRect.right += pSize[0]->cx - mod;

    SetWindowPos(window,
                 NULL,                  // z-order
                 0,                     // x position
                 0,                     // y position
                 comboRelRect.right,    // width
                 comboRelRect.bottom,   // height
                 SWP_NOMOVE|SWP_NOZORDER);

    //
    // Create the backgrounds for the combo box.
    //
                 
    COMBOBMPPTR pComboBmp = (COMBOBMPPTR)ALLOC(sizeof(COMBOBMP));
    pComboBmp->size.cx = comboRelRect.right;
    pComboBmp->size.cy = comboRelRect.bottom;
    pComboBmp->bitmap = (LPBYTE)ALLOC(comboRelRect.right*comboRelRect.bottom);
    SetProp(window,PROP_COMBOEDITBKG,(HANDLE)pComboBmp);

    //
    // Draw left
    SBltROP3(pComboBmp->bitmap,
             pBmp[0],
             pSize[0]->cx,
             pSize[0]->cy,
             pComboBmp->size.cx,
             pSize[0]->cx,
             NULL,
             SRCCOPY);

    //
    // Draw middle
    for (int tile = comboRelRect.right/pSize[1]->cx - 2,
            xoffset = pSize[1]->cx;
         tile;
         --tile, xoffset += pSize[1]->cx)
    {
        SBltROP3(pComboBmp->bitmap + xoffset,
                 pBmp[1],
                 pSize[1]->cx,
                 pSize[1]->cy,
                 pComboBmp->size.cx,
                 pSize[1]->cx,
                 NULL,
                 SRCCOPY);
    }

    //
    // Draw right
    SBltROP3(pComboBmp->bitmap + xoffset,
             pBmp[2],
             pSize[2]->cx,
             pSize[2]->cy,
             pComboBmp->size.cx,
             pSize[2]->cx,
             NULL,
             SRCCOPY);

    //
    // Set as control background
    SDlgSetBitmap(window,
        		  GetParent(window),
		          TEXT("ComboBox"),
		          SDLG_STYLE_ANY,
		          SDLG_USAGE_BACKGROUND,
		          pComboBmp->bitmap,
		          NULL,
		          pComboBmp->size.cx,
		          pComboBmp->size.cy);

    SetWindowPos(window,
                 0,0,0,         // z,x,y
                 comboRelRect.right,
                 editHgt + BEVEL_THICKNESS*2,
                 SWP_NOMOVE|SWP_NOZORDER);

    //
    // Create the edit box control
    HWND hEdit = CreateWindow(GetWindowLong(window,GWL_STYLE) &
                                CBS_DROPDOWNLIST ?
                                  TEXT("STATIC") : TEXT("EDIT"),
                              NULL,
                              WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                              BEVEL_THICKNESS,
                              BEVEL_THICKNESS,
                              comboRelRect.right - pSize[2]->cx -
                                BEVEL_THICKNESS,
                              editHgt,
                              window,
                              (HMENU)EDIT_ID,
                              global_hinstance,
                              NULL);

    // Replace the wndproc so we can receive the arrow key msgs
    WNDPROC oldProc = (WNDPROC)SetWindowLong(hEdit,
                                             GWL_WNDPROC,
                                             (LONG)ComboEditWndProc);
    SetProp(hEdit,PROP_OLDPROC,(HANDLE)oldProc);
    SetProp(window,PROP_COMBOEDIT,(HANDLE)hEdit);

    RECT lboxRect;
    // pop DOWN
    SetRect(&lboxRect,
            comboRelRect.left + BEVEL_THICKNESS,
            comboRelRect.top + editHgt + BEVEL_THICKNESS*2,
            comboRelRect.right - BEVEL_THICKNESS - ScrollbarGetWidth(),
            comboRelRect.bottom - BEVEL_THICKNESS);

    //
    // Create the listbox
    HWND hLbox = CreateWindowEx(0,
                                TEXT("LISTBOX"),
                                NULL,
                                WS_CHILD | WS_TABSTOP |
                                    LBS_NOTIFY | WS_CLIPSIBLINGS |
                                    LBS_NOINTEGRALHEIGHT,
                                lboxRect.left,
                                lboxRect.top,
                                lboxRect.right - lboxRect.left,
                                lboxRect.bottom - lboxRect.top,
                                window,
                                (HMENU)LBOX_ID,
                                global_hinstance,
                                NULL);
    if (!hLbox) return FALSE;
    SetProp(window,PROP_COMBOLBOX, (HANDLE)hLbox);

    // Replace the wndproc so we can receive the focus and capture msgs
    oldProc = (WNDPROC)SetWindowLong(hLbox,
                                     GWL_WNDPROC,
                                     (LONG)ComboLboxWndProc);
    SetProp(hLbox,PROP_OLDPROC,(HANDLE)oldProc);

    //
    // Create the listbox scrollbar
    HWND hScroll = CreateWindowEx(0,
                                  TEXT("StormScrollbar"),
                                  NULL,
                                  WS_CHILD,
                                  lboxRect.right,
                                  lboxRect.top,
                                  ScrollbarGetWidth(),
                                  lboxRect.bottom - lboxRect.top,
                                  window,
                                  (HMENU)SCROLL_ID,
                                  global_hinstance,
                                  NULL);
    if (!hScroll) return FALSE;

    // Link listbox scrollbar to combo listbox
    ScrollbarLink(hLbox, hScroll);
    SetProp(window,PROP_LBOXSCROLL,(HANDLE)hScroll);

    LPRECT pDropRect = (LPRECT)ALLOC(sizeof(RECT));
    SetRect(pDropRect,
            (GetWindowLong(window,GWL_STYLE) & CBS_DROPDOWNLIST) ?
                comboRelRect.left + BEVEL_THICKNESS :
                    comboRelRect.right - BEVEL_THICKNESS - ScrollbarGetWidth(),
            comboRelRect.top   + BEVEL_THICKNESS,
            comboRelRect.right - BEVEL_THICKNESS,
            comboRelRect.top   + BEVEL_THICKNESS + editHgt);
    SetProp(window,PROP_COMBOARROWRCT,(HANDLE)pDropRect);

    sgbPopup = FALSE;
    return TRUE;
}

//---------------------------------------------------------------------------//
static LRESULT CALLBACK StormComboWndProc(HWND window,
                                          UINT message,
                                          WPARAM wparam,
                                          LPARAM lparam)
{
	switch (message) {
		case WM_COMMAND:
			switch (LOWORD(wparam)) {
                case EDIT_ID:
                    if (HIWORD(wparam) == EN_CHANGE) {
                        wparam = MAKEWPARAM(GetWindowLong(window,GWL_ID),
                                            CBN_EDITCHANGE);
                        return SendMessage(GetParent(window),
                                           message,
                                           wparam,
                                           (LPARAM)window);
                    }
                    else if (HIWORD(wparam) == CUSTOM_EN_SCROLLDOWN) {
                        HWND hLbox = (HWND)GetProp(window,PROP_COMBOLBOX);
                        int nSel = SendMessage(hLbox,LB_GETCURSEL,0,0);
                        if (nSel == LB_ERR) nSel = -1;
                        if (LB_ERR != SendMessage(hLbox,
                                                  LB_SETCURSEL,
                                                  (WPARAM)nSel + 1,
                                                  0))
                        {
                            UpdateSelection(hLbox,
                                            (HWND)GetProp(window,
                                                          PROP_COMBOEDIT));
                        }
                        return 0;
                    }
                    else if (HIWORD(wparam) == CUSTOM_EN_SCROLLUP) {
                        HWND hLbox = (HWND)GetProp(window,PROP_COMBOLBOX);
                        int nSel = SendMessage(hLbox,LB_GETCURSEL,0,0);
                        if (nSel == LB_ERR) nSel = 1;
                        if (nSel && LB_ERR != SendMessage(hLbox,
                                                          LB_SETCURSEL,
                                                          (WPARAM)nSel - 1,
                                                          0))
                        {
                            UpdateSelection(hLbox,
                                            (HWND)GetProp(window,
                                                          PROP_COMBOEDIT));
                        }
                        return 0;
                    }
                    break;
                case LBOX_ID:
                    if (HIWORD(wparam) == LBN_SELCHANGE) {
                        UpdateSelection((HWND)lparam,
                                        (HWND)GetProp(window,PROP_COMBOEDIT));
                        return 0;
                    }
                    break;
			}
		    break;

        case WM_CREATE:
            if (!ComboCreate(window))
                return -1;
            break;

        case WM_DESTROY: {
            // free artwork
            COMBOBMPPTR pBmp =
                (COMBOBMPPTR)RemoveProp(window,PROP_COMBOEDITBKG);

            if (pBmp) {
                if (pBmp->bitmap) FREE(pBmp->bitmap);
                FREE(pBmp);
            }

            RemoveProp(window,PROP_COMBOEDIT);
            LPRECT pRect = (LPRECT)RemoveProp(window,PROP_COMBOARROWRCT);
            if (pRect) FREE(pRect);
            break;
        } 

        case WM_PAINT: {
        	PAINTSTRUCT ps;
        	HDC         dc = BeginPaint(window,&ps);

        	// PAINT THE BITMAP(S)
        	SDlgDrawBitmap(window,SDLG_USAGE_BACKGROUND,(HRGN)0);
        	EndPaint(window,&ps);
        	return 0;
        }

        case WM_SETFOCUS:
            SetFocus((HWND)GetProp(window,PROP_COMBOEDIT));
            return 0;

		case WM_LBUTTONDOWN:
		    SetCapture(window);
            return 0;

		case WM_LBUTTONUP: {
            if (window != GetCapture()) break;
            ReleaseCapture();

            if (ClosePopup(window)) return 0;
        
            HWND hLbox = (HWND)GetProp(window,PROP_COMBOLBOX);
            POINT pt = { (short)LOWORD(lparam), (short)HIWORD(lparam) };
            if (PtInRect((LPRECT)GetProp(window,PROP_COMBOARROWRCT),pt))
                OpenPopup(window);
            return 0;
        } 

        case WM_SETFONT:
            SendMessage((HWND)GetProp(window,PROP_COMBOEDIT),
                        message,
                        wparam,
                        lparam);
            SendMessage((HWND)GetProp(window,PROP_COMBOLBOX),
                        message,
                        wparam,
                        lparam);
            break;

        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLOREDIT:
            SetTextColor((HDC)wparam,0xFFFFFF);
            SetBkColor((HDC)wparam,0);
            SetBkMode((HDC)wparam,OPAQUE);
            return (BOOL)GetStockObject(BLACK_BRUSH);

        case CB_ADDSTRING: {
            HWND hLbox = (HWND)GetProp(window,PROP_COMBOLBOX);
            LRESULT result = SendMessage(hLbox,LB_ADDSTRING,wparam,lparam);
            ListUpdateScrollbar(hLbox);
            ShowWindow((HWND)GetProp(window,PROP_LBOXSCROLL),SW_HIDE);
            return result;
        }
        case WM_SETTEXT: {
            HWND hEdit = (HWND)GetProp(window,PROP_COMBOEDIT);
            if (TRUE == SendMessage(hEdit,message,wparam,lparam)) {
                if (GetWindowLong(window,GWL_STYLE) & CBS_DROPDOWNLIST)
                    return TRUE;
                SendMessage(hEdit,
                            EM_SETSEL,
                            0,
                            (LPARAM)_tcslen((LPCTSTR)lparam));
                return TRUE;
            }
            return CB_ERRSPACE;
        }
        case WM_GETTEXT:
        case WM_GETTEXTLENGTH:
            return SendMessage((HWND)GetProp(window,PROP_COMBOEDIT),
                               message,
                               wparam,
                               lparam);
        case CB_GETCOUNT:
            return SendMessage((HWND)GetProp(window,PROP_COMBOLBOX),
                               LB_GETCOUNT,
                               wparam,
                               lparam);
        case CB_GETCURSEL:
            return SendMessage((HWND)GetProp(window,PROP_COMBOLBOX),
                               LB_GETCURSEL,
                               wparam,
                               lparam);
        case CB_GETITEMDATA:
            return SendMessage((HWND)GetProp(window,PROP_COMBOLBOX),
                               LB_GETITEMDATA,
                               wparam,
                               lparam);
        case CB_GETLBTEXT:
            return SendMessage((HWND)GetProp(window,PROP_COMBOLBOX),
                               LB_GETTEXT,
                               wparam,
                               lparam);            
        case CB_LIMITTEXT:
            message = EM_LIMITTEXT;
            return SendMessage((HWND)GetProp(window,PROP_COMBOEDIT),
                               message,
                               wparam,
                               lparam);
        case CB_SETCURSEL: {
            HWND hLbox = (HWND)GetProp(window,PROP_COMBOLBOX);
            SendMessage(hLbox,LB_SETCURSEL,wparam,lparam);
            UpdateSelection(hLbox,(HWND)GetProp(window,PROP_COMBOEDIT));
            return (LRESULT)wparam;
        }
        case CB_SETEDITSEL:
            message = EM_SETSEL;
            return SendMessage((HWND)GetProp(window,PROP_COMBOEDIT),
                               message,
                               wparam,
                               lparam);
        case CB_SETITEMDATA:
            return SendMessage((HWND)GetProp(window,PROP_COMBOLBOX),
                               LB_SETITEMDATA,
                               wparam,
                               lparam);
        case CB_RESETCONTENT:
            return SendMessage((HWND)GetProp(window,PROP_COMBOLBOX),
                               LB_RESETCONTENT,
                               wparam,
                               lparam);
	}
	return DefWindowProc(window,message,wparam,lparam);
}

//---------------------------------------------------------------------------//
//  EXPORTED FUNCTIONS
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void ComboboxLoadArtwork(SNETGETARTPROC artcallback) {
    if (!sgnComboCnt++) {

        static int COMBO_ARTID[3] = {
            SNET_ART_COMBOLEFT,
            SNET_ART_COMBOMIDDLE,
            SNET_ART_COMBORIGHT
        };
        static int SCOMBO_ARTID[3] = {
            SNET_ART_SCOMBOLEFT,
            SNET_ART_SCOMBOMIDDLE,
            SNET_ART_SCOMBORIGHT
        };

        for (int pos = LEFT; pos < NUM_TILES; ++pos) {
            UiLoadArtwork(
                    artcallback,
                    NULL,
                    NULL,
                    COMBO_ARTID[pos],
                    TEXT(""),
                    SDLG_STYLE_ANY,
                    SDLG_USAGE_BACKGROUND,
                    FALSE,
                    FALSE,
                    &sgComboBmp[pos],
                    &sgComboSize[pos]);

            UiLoadArtwork(
                    artcallback,
                    NULL,
                    NULL,
                    SCOMBO_ARTID[pos],
                    TEXT(""),
                    SDLG_STYLE_ANY,
                    SDLG_USAGE_BACKGROUND,
                    FALSE,
                    FALSE,
                    &sgSComboBmp[pos],
                    &sgSComboSize[pos]);
        }

    }
}

//---------------------------------------------------------------------------//
void ComboboxDestroyArtwork(void) {
	// Only Destroy if this if the only combobox currently using the artwork
	if (!--sgnComboCnt) {
        for (int pos = LEFT; pos < NUM_TILES; ++pos) {
            if (sgComboBmp[pos]) {
                FREE(sgComboBmp[pos]);  
                sgComboBmp[pos] = NULL;
            }
            if (sgSComboBmp[pos]) {
                FREE(sgSComboBmp[pos]); 
                sgSComboBmp[pos] = NULL;
            }
        }
	}
}

//---------------------------------------------------------------------------//
void ComboRegisterClass (void) {
	WNDCLASS wndclass;
	ZeroMemory(&wndclass,sizeof(WNDCLASS));
	wndclass.style         = CS_GLOBALCLASS; 
	wndclass.lpfnWndProc   = StormComboWndProc;
	wndclass.hInstance     = global_hinstance;
	wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = "StormCombobox";
	RegisterClass(&wndclass);
}

//---------------------------------------------------------------------------//
void ComboUnregisterClass (void) {
	UnregisterClass("StormCombobox", global_hinstance);
}

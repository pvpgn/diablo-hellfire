/****************************************************************************
*
*  SDLG.CPP
*  Storm dialog box functions
*
*  By Michael O'Brien (5/22/96)
*
***/

#include "pch.h"
#pragma  hdrstop

#define  ANYBITMAP                      0x0000FFFF
#define  CONTROLTYPELENGTH              32

#define  BLT_FLAG_TILED                 0x00000001
#define  BLT_FLAG_LOCKSURFACE           0x00000002
#define  BLT_FLAG_DESTRECTCLIENTCOORDS  0x00000100
#define  BLT_FLAG_HIDECURSOR            0x00001000
#define  BLT_FLAG_RESTORECURSOR         0x00002000

#define  CONVDLGX(x)                    (((x)*baseunitx)/4)
#define  CONVDLGY(y)                    (((y)*baseunity)/8)

#define  NOTIFY(window,code)            SendMessage(GetParent(window),                   \
                                                    WM_COMMAND,                          \
                                                    MAKELONG(GetDlgCtrlID(window),code), \
                                                    (LPARAM)window)

typedef struct _BASEFONT {
    int   pointsize;
    int   weight;
    DWORD flags;
    DWORD family;
    char  face[32];
} BASEFONT, *BASEFONTPTR;

NODEDECL(BITMAPREC) {
    HWND   window;
    HWND   parentwindow;
    DWORD  usage;
    DWORD  controlstyle;
    LPBYTE bitmapbits;
    RECT   rect;
    int    width;
    int    height;
    int    offsetx;
    int    offsety;
    char   controltype[CONTROLTYPELENGTH];
} *BITMAPPTR;

NODEDECL(TIMERREC) {
    HWND      window;
    UINT      id;
    DWORD     elapse;
    TIMERPROC callback;
    DWORD     lasttime;
} *TIMERPTR;

static BASEFONTPTR     s_basefont     = NULL;
static LIST(BITMAPREC) s_bitmaplist;
static BOOL            s_cursorhidden = FALSE;
static LPBYTE          s_cursorimage  = NULL;
static LPBYTE          s_cursormask   = NULL;
static POINT           s_cursorpos    = {-1,-1};
static SIZE            s_cursorsize   = {32,32};
static BOOL            s_initialized  = FALSE;
static int             s_inpaint      = 0;
static BOOL            s_nodefproc    = FALSE;
static LIST(TIMERREC)  s_timerlist;

static BOOL DrawButton (LPDRAWITEMSTRUCT item);
static BITMAPPTR FindBitmap (HWND window, DWORD usage);
static void IntersectRgnWithWindow (HWND updatewindow,
                                    HRGN region,
                                    HWND window);
static BOOL IsButton (HWND window, BOOL *ownerdraw);
static BOOL IsPointInWindow (LPPOINTS point, HWND window);
static void ParseDlgTemplate (LPCDLGTEMPLATE     templatedata,
                              LPDLGTEMPLATE      parsedtemplate,
                              LPCWSTR           *title,
                              short             *fontsize,
                              short             *fontweight,
                              short             *fontitalics,
                              LPCWSTR           *fontname,
                              LPDLGITEMTEMPLATE *firstitem);
static void ParseDlgTemplateEx (LPCDLGTEMPLATE     templatedata,
                                LPDLGTEMPLATE      parsedtemplate,
                                LPCWSTR           *title,
                                short             *fontsize,
                                short             *fontweight,
                                short             *fontitalics,
                                LPCWSTR           *fontname,
                                LPDLGITEMTEMPLATE *firstitem);
static LRESULT SendMessageNoDefProc (HWND   window,
                                     UINT   message,
                                     WPARAM wparam,
                                     LPARAM lparam);

//===========================================================================
static void AdjustCursorPos (HWND window, int x, int y) {
  if (window) {
    DWORD processid;
    GetWindowThreadProcessId(window,&processid);
    if (processid == GetCurrentProcessId()) {
      RECT cursorrect = {x,y,x+s_cursorsize.cx,y+s_cursorsize.cy};
      RECT windowrect;  GetWindowRect(window,&windowrect);
      RECT rect;
      if (IntersectRect(&rect,&cursorrect,&windowrect)) {
        ScreenToClient(window,(LPPOINT)&rect.left);
        ScreenToClient(window,(LPPOINT)&rect.right);
        InvalidateRect(window,&rect,0);
      }
    }
    window = GetTopWindow(window);
    while (window) {
      AdjustCursorPos(window,x,y);
      window = GetNextWindow(window,GW_HWNDNEXT);
    }
  }
  else {
    AdjustCursorPos(GetDesktopWindow(),s_cursorpos.x,s_cursorpos.y);
    s_cursorpos.x = x;
    s_cursorpos.y = y;
    AdjustCursorPos(GetDesktopWindow(),s_cursorpos.x,s_cursorpos.y);
  }
}

//===========================================================================
static void CheckCursorPos () {
  POINT pt;
  GetCursorPos(&pt);
  if ((s_cursorpos.x >= 0) &&
      (s_cursorpos.y >= 0) &&
      ((pt.x != s_cursorpos.x) ||
       (pt.y != s_cursorpos.y)))
    AdjustCursorPos((HWND)0,pt.x,pt.y);
}

//===========================================================================
static void ComputeBaseUnits (HFONT font, int *baseunitx, int *baseunity) {
  HDC   screendc = GetDC(GetDesktopWindow());
  HDC   memdc    = CreateCompatibleDC(screendc);
  HFONT oldfont  = (HFONT)SelectObject(memdc,font);

  // USE GETTEXTEXTENT() TO DETERMINE THE AVERAGE CHARACTER WIDTH
  SIZE size;
  if (GetTextExtentPoint32(memdc,
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "abcdefghijklmnopqrstuvwxyz",
                           52,
                           &size))
    *baseunitx = (size.cx/26+1)/2;

  // USE GETTEXTMETRICS() TO DETERMINE THE CHARACTER HEIGHT
  TEXTMETRIC tm;
  if (GetTextMetrics(memdc,&tm))
    *baseunity = tm.tmHeight;

  SelectObject(memdc,oldfont);
  DeleteDC(memdc);
  ReleaseDC(GetDesktopWindow(),screendc);
}

//===========================================================================
static LRESULT CALLBACK ControlSubclassWndProc (HWND   window,
                                                UINT   message,
                                                WPARAM wparam,
                                                LPARAM lparam) {
  LONG origstyle = 0;

  // PERFORM INTERNAL PROCESSING ON SOME MESSAGES
  switch (message) {

    case BM_SETCHECK:
      if (wparam == BST_CHECKED)
        break;
      // fall through to BM_SETSTATE

    case BM_SETSTATE:
    case WM_PAINT:
      if (IsButton(window,NULL)) {
        DWORD style = GetWindowLong(window,GWL_STYLE);
        if (((style & 0x0000000F) == BS_RADIOBUTTON) ||
            ((style & 0x0000000F) == BS_AUTORADIOBUTTON)) {
          origstyle = GetWindowLong(window,GWL_STYLE);
          SetWindowLong(window,GWL_STYLE,(origstyle & 0xFFFFFFF0) | BS_OWNERDRAW);
        }
      }
      break;

    case WM_ERASEBKGND:
      if ((GetWindowLong(window,GWL_EXSTYLE) & WS_EX_TRANSPARENT) ||
          FindBitmap(window,ANYBITMAP))
        return 0;
      break;

    case WM_KEYUP:
      if (wparam == VK_SNAPSHOT) {
        SDrawCaptureScreen();
        SetFocus(window);
      }
      break;

    case WM_NCDESTROY:
      RemoveProp(window,"SDlg_WndProc");
      if (GetProp(window,"SDlg_OrigStyle"))
        RemoveProp(window,"SDlg_OrigStyle");
      break;

    case WM_STYLECHANGED:
    case WM_STYLECHANGING:
      return 0;

    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
      SendMessage(GetParent(window),message,wparam,lparam);
      break;

  }

  // CALL THE ORIGINAL WINDOW PROCEDURE
  LRESULT result;
  {
    WNDPROC wndproc = NULL;
    if (message != WM_NCDESTROY)
      wndproc = (WNDPROC)GetProp(window,"SDlg_WndProc");
    if (wndproc)
      result = CallWindowProc(wndproc,window,message,wparam,lparam);
    else
      result = DefWindowProc(window,message,wparam,lparam);
  }

  if (origstyle)
    SetWindowLong(window,GWL_STYLE,origstyle);
  return result;
}

//===========================================================================
static LRESULT CALLBACK ControlStaticWndProc (HWND   window,
                                              UINT   message,
                                              WPARAM wparam,
                                              LPARAM lparam) {
  switch (message) {

    case WM_DESTROY:
      RemoveProp(window,"SDlg_Font");
      break;

    case WM_ERASEBKGND:
      if ((GetWindowLong(window,GWL_EXSTYLE) & WS_EX_TRANSPARENT) ||
          FindBitmap(window,ANYBITMAP))
        return 0;
      break;

    case WM_GETFONT:
      return (LRESULT)GetProp(window,"SDlg_Font");

    case WM_PAINT:
      {
        PAINTSTRUCT ps;
        HDC         dc = SDlgBeginPaint(window,&ps);

        // SELECT THE FONT
        SelectObject(dc,(HFONT)GetProp(window,"SDlg_Font"));
        SetBkMode(dc,TRANSPARENT);

        // SEND A WM_DRAWITEM MESSAGE, IN CASE THE APPLICATION WANTS TO
        // DRAW THE TEXT MANUALLY
        BOOL drawn = 0;
        {
          DRAWITEMSTRUCT drawitem;
          ZeroMemory(&drawitem,sizeof(DRAWITEMSTRUCT));
          drawitem.CtlType    = ODT_STATIC;
          drawitem.CtlID      = GetDlgCtrlID(window);
          drawitem.itemAction = ODA_DRAWENTIRE;
          drawitem.hwndItem   = window;
          drawitem.hDC        = dc;
          GetClientRect(window,&drawitem.rcItem);
          drawn = (BOOL)SendMessageNoDefProc(GetParent(window),
                                             WM_DRAWITEM,
                                             GetDlgCtrlID(window),
                                             (LPARAM)&drawitem);
        }

        // IF THE APPLICATION DIDN'T DRAW THE TEXT, DO IT OURSELF
        if (!drawn) {
          int   chars = GetWindowTextLength(window);
          char *text  = (char *)ALLOC(chars+1);
          GetWindowText(window,text,chars+1);

          // DETERMINE THE FORMAT
          DWORD format = DT_EXPANDTABS | DT_WORDBREAK;
          {
            DWORD style = (DWORD)GetWindowLong(window,GWL_STYLE);
            if (style & SS_LEFT)
              format |= DT_LEFT;
            else if (style & SS_CENTER)
              format |= DT_CENTER;
            else if (style & SS_RIGHT)
              format |= DT_RIGHT;
          }

          // DRAW THE DROP SHADOW
          {
            RECT rect;
            GetClientRect(window,&rect);
            ++rect.left;
            ++rect.top;
            SetTextColor(dc,0);
            DrawText(dc,text,chars,&rect,format);
          }

          // DRAW THE TEXT
          {
            RECT rect;
            GetClientRect(window,&rect);
            --rect.right;
            --rect.bottom;
            SetTextColor(dc,0xFFFFFF);
            SendMessage(GetParent(window),WM_CTLCOLORSTATIC,(WPARAM)dc,(LPARAM)window);
            DrawText(dc,text,chars,&rect,format);
          }

          FREE(text);
        }

        SDlgEndPaint(window,&ps);
      }
      return 0;

    case WM_SETFONT:
      SetProp(window,"SDlg_Font",(HANDLE)wparam);
      if (lparam & 1)
        InvalidateRect(window,NULL,1);
      break;

    case WM_SETTEXT:
      InvalidateRect(window,NULL,1);
      break;

  }
  return DefWindowProc(window,message,wparam,lparam);
}

//===========================================================================
static void DeleteBitmaps (HWND window) {

  // DELETE ALL BITMAPS ASSOCIATED WITH THIS WINDOW
  ITERATELIST(BITMAPREC,s_bitmaplist,curr)
    if ((curr->window == window) ||
        (curr->parentwindow == window))
      ITERATE_DELETE;

  // RECURSE ALL CHILD WINDOWS
  if ((window = GetTopWindow(window)) != (HWND)0)
    do
      DeleteBitmaps(window);
    while ((window = GetNextWindow(window,GW_HWNDNEXT)) != (HWND)0);

}

//===========================================================================
static BOOL CALLBACK DlgProc (HWND   window,
                              UINT   message,
                              WPARAM wparam,
                              LPARAM lparam) {

  // PERFORM INTERNAL PROCESSING ON SOME MESSAGES
  switch (message) {

    case WM_CREATE:
      SetCursor(LoadCursor(0,IDC_ARROW));
      break;

    case WM_DESTROY:
      RemoveProp(window,"SDlg_Font");
      DeleteBitmaps(window);
      break;

    case WM_DRAWITEM:
      {
        LPDRAWITEMSTRUCT item = (LPDRAWITEMSTRUCT)lparam;
        if (item->CtlType == ODT_BUTTON)
          return DrawButton(item);
      }
      break;

    case WM_ERASEBKGND:
      if ((GetWindowLong(window,GWL_EXSTYLE) & WS_EX_TRANSPARENT) ||
          FindBitmap(window,ANYBITMAP))
        return 0;
      break;

    case WM_GETFONT:
      return (LRESULT)GetProp(window,"SDlg_Font");

    case WM_NCDESTROY:
      {
        HDC dc = GetDC(window);
        SelectObject(dc,GetStockObject(SYSTEM_FONT));
        ReleaseDC(window,dc);
      }
      DeleteObject((HFONT)SendMessage(window,WM_GETFONT,0,0));
      break;

    case WM_PAINT:
      {
        PAINTSTRUCT ps;
        HDC         dc = SDlgBeginPaint(window,&ps);
        SDlgEndPaint(window,&ps);
      }
      if ((GetProp(window,"SDlg_Modal")) &&
          IsWindowEnabled(GetParent(window))) {
        EnableWindow(GetParent(window),0);
        if (GetActiveWindow() == GetParent(window))
          SetActiveWindow(window);
      }
      return 0;

    case WM_SETFONT:
      SetProp(window,"SDlg_Font",(HANDLE)wparam);
      break;

    case WM_SHOWWINDOW:
      if ((!wparam) &&
          ((!lparam) ||
           (lparam == SW_PARENTCLOSING)) &&
          (GetProp(window,"SDlg_Modal")))
        EnableWindow(GetParent(window),1);
      break;

  }

  // CALL THE DEFAULT DIALOG BOX PROCEDURE, WHICH WILL CALL THE
  // APPLICATION'S DIALOG BOX PROCEDURE
  BOOL result;
  if (s_nodefproc) {
    s_nodefproc = 0;
    result = CallWindowProc((WNDPROC)GetWindowLong(window,DWL_DLGPROC),window,message,wparam,lparam);
  }
  else
    result = DefDlgProc(window,message,wparam,lparam);

  // ON A WM_NCCREATE MESSAGE, SET THE POINTER TO THE APPLICATION'S
  // DIALOG BOX PROCEDURE 
  if (message == WM_NCCREATE)
    SetWindowLong(window,DWL_DLGPROC,(LONG)((LPCREATESTRUCT)lparam)->lpCreateParams);

  return result;
}

//===========================================================================
static BOOL DoMessageLoop (HWND dialogwindow) {
  MSG message;
  if (PeekMessage(&message,(HWND)0,0,0,PM_REMOVE)) {
    if (message.message == WM_QUIT)
      PostQuitMessage(message.wParam);
    else if ((!dialogwindow) ||
             (!IsDialogMessage(dialogwindow,&message))) {
      TranslateMessage(&message);
      DispatchMessage(&message);
    }
    return TRUE;
  }
  else {
    SDlgCheckTimers();
    SDlgUpdateCursor();
    return FALSE;
  }
}

//===========================================================================
static BOOL DrawButton (LPDRAWITEMSTRUCT item) {
  RECT  clientrect;  GetClientRect(item->hwndItem,&clientrect);
  HDC   dc         = GetDC(item->hwndItem);
  BOOL  selected   = ((item->itemState & ODS_SELECTED) != 0);
  if (SendMessage(item->hwndItem,BM_GETSTATE,0,0) & BST_CHECKED)
    selected = TRUE;
  BOOL  grayed     = ((item->itemState & (ODS_DISABLED | ODS_GRAYED)) != 0);
  DWORD style      = (DWORD)GetWindowLong(item->hwndItem,GWL_STYLE);
  if (GetProp(item->hwndItem,"SDlg_OrigStyle"))
    style = (DWORD)GetProp(item->hwndItem,"SDlg_OrigStyle");
  BOOL  pushbutton = ((style & 0x0000000F) == BS_PUSHBUTTON) ||
                     ((style & 0x0000000F) == BS_DEFPUSHBUTTON) ||
                     ((style & 0x0000000F) == BS_OWNERDRAW);

  // DETERMINE THE BOUNDING OFFSET
  RECT boundingoffset = {0,0,0,0};
  if (!pushbutton) {
    boundingoffset.left   =  1;
    boundingoffset.bottom = -1;
  }

  // IF A BITMAP HAS BEEN REGISTERED SPECIFICALLY FOR THIS ITEM STATE,
  // DRAW THAT
  DWORD     usage  = selected
                       ? grayed
                         ? SDLG_USAGE_SELECTED_GRAYED
                         : (item->itemState & ODS_FOCUS)
                           ? SDLG_USAGE_SELECTED_FOCUSED
                           : SDLG_USAGE_SELECTED_UNFOCUSED
                       : grayed
                         ? SDLG_USAGE_NORMAL_GRAYED
                         : (item->itemState & ODS_FOCUS)
                           ? SDLG_USAGE_NORMAL_FOCUSED
                           : SDLG_USAGE_NORMAL_UNFOCUSED;
  BITMAPPTR bitmap = FindBitmap(item->hwndItem,usage);
  if (bitmap)
    SDlgDrawBitmap(item->hwndItem,
                   usage,
                   (HRGN)0,
                   0,
                   0,
                   &boundingoffset,
                   pushbutton ? (SDLG_DBF_TILE | SDLG_DBF_VCENTER) : 0);

  // OTHERWISE, ERASE THE BACKGROUND AND DRAW A BEVEL AROUND IT
  else {
    SDlgDrawBitmap(item->hwndItem,
                   SDLG_USAGE_BACKGROUND,
                   (HRGN)0,
                   1-selected,
                   1-selected,
                   &boundingoffset,
                   pushbutton ? (SDLG_DBF_TILE | SDLG_DBF_VCENTER) : 0);
    if (pushbutton) {
      UINT edge = selected ? (BDR_SUNKENINNER | BDR_SUNKENOUTER)
                           : (BDR_RAISEDOUTER | BDR_RAISEDOUTER);
      DrawEdge(dc,&clientrect,edge,BF_RECT);
    }
  }

  // DRAW THE TEXT
  {

    // GET THE TEXT
    int   chars = GetWindowTextLength(item->hwndItem);
    char *text  = (char *)ALLOC(chars+1);
    GetWindowText(item->hwndItem,text,chars+1);

    // SELECT THE FONT
    SelectObject(dc,GetCurrentObject(item->hDC,OBJ_FONT));
    SetTextAlign(dc,TA_TOP | TA_LEFT);
    SetBkMode(dc,TRANSPARENT);

    // DETERMINE THE TEXT LOCATION
    RECT rect   = {clientrect.left  +3,
                   clientrect.top   +3,
                   clientrect.right -1,
                   clientrect.bottom-1};
    if (pushbutton) {
      DWORD style = (DWORD)GetWindowLong(item->hwndItem,GWL_STYLE);
      SIZE  size  = {16,16};
      GetTextExtentPoint32(dc,"~,_Oy",5,&size);
      if (style & BS_BOTTOM)
        rect.top += rect.bottom-(size.cy+6);
      else if ((style & BS_VCENTER) || !(style & BS_TOP))
        rect.top += (rect.bottom-(rect.top+size.cy))/2+1;
      if (selected) {
        ++rect.left;
        ++rect.top;
        ++rect.right;
        ++rect.bottom;
      }
    }
    else if (bitmap) {
      rect.top  -= 1;
      rect.left += 6+bitmap->rect.right-bitmap->rect.left;
    }

    // DRAW THE DROP SHADOW
    SetTextColor(dc,0);
    DrawText(dc,text,chars,&rect,
             (pushbutton ? DT_CENTER : DT_LEFT) | DT_TOP);

    // DRAW THE TEXT
    --rect.left;
    --rect.top;
    --rect.right;
    --rect.bottom;
    SetTextColor(dc,grayed ? 0x808080 : 0xFFFFFF);
    DrawText(dc,text,chars,&rect,
             (pushbutton ? DT_CENTER : DT_LEFT) | DT_TOP);

    FREE(text);
  }

  // IF THIS IS A PUSHBUTTON, DRAW THE FOCUS RECTANGLE
  if (pushbutton &&
      (item->itemState & ODS_FOCUS) &&
      !FindBitmap(item->hwndItem,SDLG_USAGE_NORMAL_FOCUSED | SDLG_USAGE_SELECTED_FOCUSED)) {
    RECT rect = {clientrect.left  +selected+4,
                 clientrect.top   +selected+4,
                 clientrect.right +selected-3,
                 clientrect.bottom+selected-3};
    DrawFocusRect(dc,&rect);
  }

  ReleaseDC(item->hwndItem,dc);
  return TRUE;
}

//===========================================================================
static BITMAPPTR FindBitmap (HWND window, DWORD usage) {

  // FIND THE BITMAP FOR THIS WINDOW
  BITMAPPTR curr = s_bitmaplist.Head();
  while (curr &&
         ((curr->window != window) ||
          (!(curr->usage & usage))))
    curr = curr->Next();

  // IF THIS WINDOW DOESN'T HAVE ITS OWN BITMAP, FIND THE DEFAULT
  // BITMAP FOR THIS CONTROL TYPE AND STYLE
  if (!curr) {
    HWND parentwindow   = GetParent(window);
    char classname[256] = "";
    GetClassName(window,classname,256);
    LPCTSTR classnameptr = classname;
    if (!_strnicmp(classnameptr,"SDlg",4))
      classnameptr += 4;
    DWORD style = (DWORD)GetWindowLong(window,GWL_STYLE);
    if (GetProp(window,"SDlg_OrigStyle"))
      style = (DWORD)GetProp(window,"SDlg_OrigStyle");
    curr = s_bitmaplist.Head();
    while (curr &&
           (curr->window ||
            (curr->parentwindow && (curr->parentwindow != parentwindow)) ||
            (!(usage & curr->usage)) ||
            ((style & 0x0000000F) & ~curr->controlstyle) ||
            ((!(curr->controlstyle & 0x00010000)) &&
             ((style & curr->controlstyle) != curr->controlstyle)) ||
            _stricmp(curr->controltype,classnameptr)))
      curr = curr->Next();
  }

  return curr;
}

//===========================================================================
static LPCDLGTEMPLATE GetTemplateData (HINSTANCE instance,
                                       LPCTSTR   templatename) {
  HRSRC resourcehandle = FindResource(instance,templatename,RT_DIALOG);
  if (!resourcehandle)
    return NULL;
  HGLOBAL resourcedata = LoadResource(instance,resourcehandle);
  if (!resourcedata)
    return NULL;
  return (LPCDLGTEMPLATE)LockResource(resourcedata);
}

//===========================================================================
static BOOL InitializeDialogManager () {
  s_initialized = TRUE;

  // REGISTER OUR OWN DIALOG BOX CLASS
  {
    WNDCLASS wndclass;
    ZeroMemory(&wndclass,sizeof(WNDCLASS));
    wndclass.style         = CS_DBLCLKS;
    wndclass.lpfnWndProc   = (WNDPROC)DlgProc;
    wndclass.cbWndExtra    = DLGWINDOWEXTRA;
    wndclass.hInstance     = (HINSTANCE)GetModuleHandle(NULL);
    wndclass.hCursor       = LoadCursor(0,IDC_ARROW);
    wndclass.lpszClassName = "SDlgDialog";
    if (!RegisterClass(&wndclass))
      return FALSE;
  }

  // REGISTER OUR OWN STATIC CONTROL CLASS
  {
    WNDCLASS wndclass;
    ZeroMemory(&wndclass,sizeof(WNDCLASS));
    GetClassInfo((HINSTANCE)GetModuleHandle(NULL),"Static",&wndclass);
    wndclass.lpfnWndProc   = (WNDPROC)ControlStaticWndProc;
    wndclass.hInstance     = (HINSTANCE)GetModuleHandle(NULL);
    wndclass.hCursor       = LoadCursor(0,IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    wndclass.lpszClassName = "SDlgStatic";
    if (!RegisterClass(&wndclass))
      return FALSE;
  }

  return TRUE;
}

//===========================================================================
static BOOL InternalBltClippedToRgn (LPBYTE  dest,
                                     LPRECT  destrect,
                                     LPSIZE  destsize,
                                     int     destpitch,
                                     LPBYTE  source,
                                     LPRECT  sourcerect,
                                     LPSIZE  sourcesize,
                                     int     sourcepitch,
                                     int     sourceoffsetx,
                                     int     sourceoffsety,
                                     DWORD   pattern,
                                     DWORD   rop3,
                                     LPPOINT clientpos,
                                     HRGN    region,
                                     DWORD   flags,
                                     int     surfacenumber,
                                     int    *cursorhidden) {

  // RETRIEVE A LIST OF RECTANGLES THAT MAKE UP THE CLIPPING REGION
  LPRGNDATA data;
  DWORD     numrects;
  LPRECT    rectarray;
  {
    DWORD bytes = GetRegionData(region,0,NULL);
    data = (LPRGNDATA)ALLOC(bytes);
    GetRegionData(region,bytes,data);
    numrects  = data->rdh.nCount;
    rectarray = (LPRECT)&data->Buffer[0];
  }

  // VERIFY THAT WE HAVE AT LEAST ONE RECTANGLE TO DRAW
  if (!(numrects && rectarray)) {
    if (data)
      FREE(data);
    return TRUE;
  }

  // MODIFY THE RECTANGLES SO THAT THEY ARE IN SCREEN COORDINATES, THEN CLIP
  // THEM AGAINST THE BOUNDING RECTANGLE.
  {
    for (DWORD loop = 0; loop < numrects; ++loop) {
      (rectarray+loop)->left   = max(destrect->left  ,(rectarray+loop)->left  +clientpos->x);
      (rectarray+loop)->top    = max(destrect->top   ,(rectarray+loop)->top   +clientpos->y);
      (rectarray+loop)->right  = min(destrect->right ,(rectarray+loop)->right +clientpos->x);
      (rectarray+loop)->bottom = min(destrect->bottom,(rectarray+loop)->bottom+clientpos->y);
    }
  }

  // IF THE SYSTEM CURSOR IS VISIBLE AND IS WITHIN ONE OF THE RECTANGLES,
  // AND WE ARE PROTECTING IT, THEN HIDE IT
  int hidden = 0;
  if (flags & BLT_FLAG_HIDECURSOR) {
    POINT cursor;
    GetCursorPos(&cursor);
    for (DWORD loop = 0; loop < numrects; ++loop)
      if ((cursor.x+32 >= (rectarray+loop)->left) &&
          (cursor.y+32 >= (rectarray+loop)->top) &&
          (cursor.x-32 <= (rectarray+loop)->right) &&
          (cursor.y-32 <= (rectarray+loop)->bottom)) {
        do
          ++hidden;
        while (ShowCursor(0) >= 0);
        break;
      }
    GdiFlush();
  }

  // IF NECESSARY, LOCK THE SURFACE
  if (flags & BLT_FLAG_LOCKSURFACE)
    SDrawLockSurface(surfacenumber,NULL,&dest,&destpitch);

  // PAINT THE REGION WITH THE SELECTED BITMAP, TILING IT IF NECESSARY
  if (dest && destpitch) {
    for (DWORD loop = 0; loop < numrects; ++loop) {
      if (flags & BLT_FLAG_TILED)
        SBltROP3Tiled(dest,
                      rectarray+loop,
                      destpitch,
                      source,
                      sourcerect,
                      sourcepitch,
                      (rectarray+loop)->left+sourceoffsetx-clientpos->x,
                      (rectarray+loop)->top +sourceoffsety-clientpos->y,
                      pattern,
                      rop3);
      else {
        RECT offsetsourcerect = {sourcerect->left,
                                 sourcerect->top,
                                 sourcerect->right,
                                 sourcerect->bottom};
        offsetsourcerect.left += (rectarray+loop)->left+sourceoffsetx-destrect->left;
        offsetsourcerect.top  += (rectarray+loop)->top +sourceoffsety-destrect->top;
        SBltROP3Clipped(dest,
                        rectarray+loop,
                        NULL,
                        destpitch,
                        source,
                        &offsetsourcerect,
                        sourcesize,
                        sourcepitch,
                        pattern,
                        rop3);
      }
    }
  }

  // IF NECESSARY, UNLOCK THE SURFACE
  if (flags & BLT_FLAG_LOCKSURFACE)
    SDrawUnlockSurface(surfacenumber,dest,numrects,rectarray);

  // IF WE HID THE CURSOR, UNHIDE IT
  if (flags & BLT_FLAG_RESTORECURSOR)
    if (hidden)
      do
        ShowCursor(1);
      while (--hidden);
  if (cursorhidden)
    *cursorhidden += hidden;

  // FREE THE LIST OF RECTANGLES
  FREE(data);

  return TRUE;
}

//===========================================================================
static BOOL InternalBltClippedToWindow (LPBYTE  dest,
                                        LPRECT  destrect,
                                        LPSIZE  destsize,
                                        int     destpitch,
                                        LPBYTE  source,
                                        LPRECT  sourcerect,
                                        LPSIZE  sourcesize,
                                        int     sourcepitch,
                                        int     sourceoffsetx,
                                        int     sourceoffsety,
                                        DWORD   pattern,
                                        DWORD   rop3,
                                        HWND    window,
                                        HRGN    region,
                                        DWORD   flags,
                                        int     surfacenumber,
                                        int    *cursorhidden) {
  if (cursorhidden)
    *cursorhidden = 0;

  VALIDATEBEGIN;
  VALIDATE(destrect);
  VALIDATE(window);
  VALIDATEEND;

  if (!((flags & BLT_FLAG_LOCKSURFACE) || (dest && (destpitch > 0))))
    return FALSE;

  // INTERSECT THE DESTINATION RECTANGLE WITH THE DESTINATION SIZE
  RECT moddestrect = {destrect->left,destrect->top,destrect->right,destrect->bottom};
  if (destsize) {
    if ((moddestrect.right-moddestrect.left) > destsize->cx)
      moddestrect.right = moddestrect.left+destsize->cx;
    if ((moddestrect.bottom-moddestrect.top) > destsize->cy)
      moddestrect.bottom = sourcerect->bottom+destsize->cy;
  }

  // IF WE'RE NOT TILING, INTERSECT THE DESTINATION RECTANGLE WITH THE
  // SOURCE SIZE
  if (sourcerect && !(flags & BLT_FLAG_TILED)) {
    if ((moddestrect.right-moddestrect.left) > (sourcerect->right-sourcerect->left))
      moddestrect.right = sourcerect->right+moddestrect.left-sourcerect->left;
    if ((moddestrect.bottom-moddestrect.top) > (sourcerect->bottom-sourcerect->top))
      moddestrect.bottom = sourcerect->bottom+moddestrect.top-sourcerect->top;
  }

  // GET THE TARGET WINDOW'S CLIENT RECTANGLE, IN SCREEN COORDINATES
  RECT boundingrect;
  GetClientRect(window,&boundingrect);
  ClientToScreen(window,(LPPOINT)&boundingrect.left);
  ClientToScreen(window,(LPPOINT)&boundingrect.right);
  POINT clientpos = {boundingrect.left,boundingrect.top};

  // IF NECESSARY, CONVERT THE DESTINATION RECTANGLE TO SCREEN COORDINATES
  if (flags & BLT_FLAG_DESTRECTCLIENTCOORDS) {
    moddestrect.left   += clientpos.x;
    moddestrect.top    += clientpos.y;
    moddestrect.right  += clientpos.x;
    moddestrect.bottom += clientpos.y;
  }

  // INTERSECT THE CLIENT RECTANGLE WITH THE DESTINATION RECTANGLE
  {
    RECT intersectrect = {moddestrect.left,
                          moddestrect.top,
                          moddestrect.right,
                          moddestrect.bottom};
    IntersectRect(&boundingrect,&boundingrect,&intersectrect);
  }

  // IF THE RESULTING RECTANGLE IS COMPLETELY CLIPPED OUT, RETURN WITHOUT
  // ATTEMPTING TO DRAW
  if ((boundingrect.right <= boundingrect.left) ||
      (boundingrect.bottom <= boundingrect.top))
    return 1;

  // CREATE A CLIPPING REGION THAT INITIALLY CONTAINS THE INTERSECTION OF
  // THE REGION WE WERE PASSED (IF ANY) WITH THE CLIENT AREA OF THIS WINDOW
  HRGN clipregion;
  {
    RECT clientrect;
    GetClientRect(window,&clientrect);
    clipregion = CreateRectRgn(clientrect.left,
                               clientrect.top,
                               clientrect.right,
                               clientrect.bottom);
    if (!clipregion)
      return FALSE;
    if (region)
      CombineRgn(clipregion,clipregion,region,RGN_AND);
  }

  // REMOVE FROM THE CLIPPING REGION THOSE PORTIONS OF THE WINDOW COVERED BY
  // OTHER OVERLAPPING, NON-TRANSPARENT WINDOWS
  {
    IntersectRgnWithWindow(window,clipregion,window);
    HWND startwindow = window;
    do {
      HWND sibling = startwindow;
      while ((sibling = GetNextWindow(sibling,GW_HWNDPREV)) != (HWND)0)
        IntersectRgnWithWindow(window,clipregion,sibling);
      if (GetWindowLong(startwindow,GWL_STYLE) & WS_CHILD)
        startwindow = GetParent(startwindow);
      else
        startwindow = (HWND)0;
    } while (startwindow && (startwindow != GetDesktopWindow()));
  }

  // DETERMINE WHETHER WE'RE DISPLAYING A CUSTOM CURSOR
  BOOL customcursor = (s_cursormask && s_cursorimage &&
                       (s_cursorpos.x >= 0) && (s_cursorpos.y >= 0));

  // SPLIT THE CLIPPING REGION INTO TWO DIFFERENT REGIONS: ONE THAT CONTAINS
  // ONLY THE PORTION OF THE REGION WHICH IS COVERED BY THE CURSOR, AND ONE
  // THAT CONTAINS EVERYTHING THAT IS NOT COVERED BY THE CURSOR
  HRGN cursorregion;
  if (customcursor) {
    cursorregion = CreateRectRgn(s_cursorpos.x-clientpos.x,
                                 s_cursorpos.y-clientpos.y,
                                 s_cursorpos.x+s_cursorsize.cx-clientpos.x,
                                 s_cursorpos.y+s_cursorsize.cy-clientpos.y);
    HRGN normalregion = CreateRectRgn(0,0,1,1);
    CombineRgn(normalregion,clipregion,cursorregion,RGN_DIFF);
    CombineRgn(cursorregion,clipregion,cursorregion,RGN_AND);
    DeleteObject(clipregion);
    clipregion = normalregion;
  }

  // BLT THE NON-CURSOR REGION ONTO THE SCREEN
  InternalBltClippedToRgn(dest,
                          &boundingrect,
                          destsize,
                          destpitch,
                          source,
                          sourcerect,
                          sourcesize,
                          sourcepitch,
                          sourceoffsetx,
                          sourceoffsety,
                          pattern,
                          rop3,
                          &clientpos,
                          clipregion,
                          flags,
                          surfacenumber,
                          cursorhidden);
  DeleteObject(clipregion);

  // IF WE'RE DISPLAYING A CUSTOM CURSOR THEN PROCESS THE CURSOR REGION
  if (customcursor) {

    // ALLOCATE AN OFFSCREEN BUFFER TO HOLD THE COMPOSITE IMAGE
    LPBYTE compositebuffer = (LPBYTE)ALLOC(s_cursorsize.cx*s_cursorsize.cy);
    RECT   cursorrect      = {0,0,s_cursorsize.cx,s_cursorsize.cy};

    // BLT THE CURSOR REGION ONTO THE OFFSCREEN BUFFER
    POINT offset = {clientpos.x-s_cursorpos.x,
                    clientpos.y-s_cursorpos.y};
    InternalBltClippedToRgn(compositebuffer,
                            &cursorrect,
                            &s_cursorsize,
                            s_cursorsize.cx,
                            source,
                            sourcerect,
                            sourcesize,
                            sourcepitch,
                            sourceoffsetx,
                            sourceoffsety,
                            pattern,
                            rop3,
                            &offset,
                            cursorregion,
                            flags & BLT_FLAG_TILED,
                            surfacenumber,
                            NULL);

    // DRAW THE CURSOR ONTO THE OFFSCREEN BUFFER
    SBltROP3(compositebuffer,
             s_cursormask,
             s_cursorsize.cx,
             s_cursorsize.cy,
             s_cursorsize.cx,
             s_cursorsize.cx,
             0,
             SRCAND);
    SBltROP3(compositebuffer,
             s_cursorimage,
             s_cursorsize.cx,
             s_cursorsize.cy,
             s_cursorsize.cx,
             s_cursorsize.cx,
             0,
             SRCPAINT);

    // BLT THE OFFSCREEN BUFFER ONTO THE SCREEN
    boundingrect.left = s_cursorpos.x;
    boundingrect.top  = s_cursorpos.y;
    InternalBltClippedToRgn(dest,
                            &boundingrect,
                            destsize,
                            destpitch,
                            compositebuffer,
                            &cursorrect,
                            &s_cursorsize,
                            s_cursorsize.cx,
                            0,
                            0,
                            0,
                            SRCCOPY,
                            &clientpos,
                            cursorregion,
                            flags & ~BLT_FLAG_TILED,
                            surfacenumber,
                            cursorhidden);
    DeleteObject(cursorregion);

    // FREE THE OFFSCREEN BUFFER
    FREE(compositebuffer);

  }

  return TRUE;
}

//===========================================================================
static HWND InternalCreateDialogBox (HINSTANCE      instance,
                                     LPCDLGTEMPLATE templatedata,
                                     HWND           parentwindow,
                                     DLGPROC        dialogproc,
                                     LPARAM         initparam) {

  // IF NO PARENT WINDOW WAS GIVEN, THEN USE THE APPLICATION'S FRAME WINDOW
  // AS THE PARENT.  IT'S IMPORTANT TO HAVE A PARENT WINDOW BECAUSE IF
  // WE CREATE A NEW TOP-LEVEL WINDOW WHILE A DIRECTDRAW APPLICATION IS
  // ACTIVE IN EXCLUSIVE MODE, THE DIRECTDRAW APPLICATION WILL BE MINIMIZED.
  HWND framewindow = SDrawGetFrameWindow();
  if (!parentwindow)
    parentwindow = framewindow;

  // PARSE THE TEMPLATE
  BOOL              extendedformat = 0;
  LPCWSTR           title;
  short             fontsize;
  short             fontweight;
  short             fontitalics;
  LPCWSTR           fontname;
  LPDLGITEMTEMPLATE firstitem;
  DLGTEMPLATE       parsedtemplate;
  if (templatedata->style == 0xFFFF0001) {
    extendedformat = 1;
    ParseDlgTemplateEx(templatedata,
                       &parsedtemplate,
                       &title,
                       &fontsize,
                       &fontweight,
                       &fontitalics,
                       &fontname,
                       &firstitem);
  }
  else
    ParseDlgTemplate(templatedata,
                     &parsedtemplate,
                     &title,
                     &fontsize,
                     &fontweight,
                     &fontitalics,
                     &fontname,
                     &firstitem);

  // CONVERT THE TITLE AND FONT NAME TO ANSI
  char ansititle[256]    = "";
  char ansifontname[256] = "";
  if (title && *title && (*title != 0xFFFF))
    wcstombs(ansititle,title,255);
  if (fontname && *fontname && (*fontname != 0xFFFF))
    wcstombs(ansifontname,fontname,255);

  // CREATE A FONT FOR THE DIALOG BOX CONTROLS.  WE USE A HARD-CODED VALUE OF
  // 96 VERTICAL PIXELS PER LOGICAL INCH INSTEAD OF QUERYING THE DEVICE CAPS
  // BECAUSE WE WANT OUR DIALOG BOXES TO LOOK EXACTLY THE SAME ON ALL
  // DISPLAYS.
  HFONT font = (HFONT)0;
  if (ansifontname[0])
    if (s_basefont)
      font = CreateFont(-MulDiv(s_basefont->pointsize,96,72),0,0,0,
                        s_basefont->weight,0,0,0,ANSI_CHARSET,
                        OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
                        DEFAULT_PITCH | s_basefont->family,
                        s_basefont->face);
    else
      font = CreateFont(-MulDiv(fontsize,96,72),0,0,0,fontweight,fontitalics,0,0,ANSI_CHARSET,
                        OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
                        DEFAULT_PITCH | FF_DONTCARE,ansifontname);

  // VERIFY THAT THE FONT WE CREATED WAS A TRUETYPE FONT, BECAUSE ONLY
  // TRUETYPE FONTS BE SIZED THE SAME ON ALL SYSTEMS.  IF THE FONT ISN'T A
  // TRUETYPE FONT, DESTROY IT AND CREATE A DEFAULT TRUETYPE FONT.
  if (font) {
    BOOL truetype;
    {
      HDC   parentdc = GetDC(parentwindow);
      HDC   memdc    = CreateCompatibleDC(parentdc);
      HFONT oldfont  = (HFONT)SelectObject(memdc,font);
      TEXTMETRIC tm;
      ZeroMemory(&tm,sizeof(TEXTMETRIC));
      GetTextMetrics(memdc,&tm);
      truetype = ((tm.tmPitchAndFamily & TMPF_TRUETYPE) != 0);
      SelectObject(memdc,oldfont);
      DeleteDC(memdc);
      ReleaseDC(parentwindow,parentdc);
    }
    if (!truetype) {
      DeleteObject(font);
      if (s_basefont)
        font = CreateFont(-MulDiv(s_basefont->pointsize,96,72),0,0,0,
                          s_basefont->weight,0,0,0,ANSI_CHARSET,
                          OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
                          DEFAULT_PITCH | FF_SWISS,"Arial");
      else
        font = CreateFont(-MulDiv(fontsize,96,72),0,0,0,fontweight,fontitalics,0,0,ANSI_CHARSET,
                          OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
                          DEFAULT_PITCH | FF_SWISS,"Arial");
    }
  }

  // DETERMINE THE FONT'S AVERAGE CHARACTER SIZE, WHICH WE USE FOR CONVERTING
  // DIALOG UNITS TO SCREEN COORDINATES
  int baseunitx = 8;
  int baseunity = 16;
  if (font)
    ComputeBaseUnits(font,&baseunitx,&baseunity);

  // DETERMINE THE WINDOW STYLE
  DWORD windowstyle   = parsedtemplate.style & ~WS_VISIBLE;
  DWORD windowexstyle = parsedtemplate.dwExtendedStyle
                          | WS_EX_CONTROLPARENT
                          | ((windowstyle & DS_MODALFRAME) ? WS_EX_DLGMODALFRAME
                                                           : 0);

  // DETERMINE THE WINDOW POSITION
  RECT windowrect = {CONVDLGX(parsedtemplate.x),
                     CONVDLGY(parsedtemplate.y),
                     CONVDLGX(parsedtemplate.x)+CONVDLGX(parsedtemplate.cx)-1,
                     CONVDLGY(parsedtemplate.y)+CONVDLGY(parsedtemplate.cy)-1};
  AdjustWindowRectEx(&windowrect,windowstyle & 0xFFFF0000,0,windowexstyle);
  int screencx;
  int screency;
  SDrawGetScreenSize(&screencx,&screency);
  int windowcx = windowrect.right+1-windowrect.left;
  int windowcy = windowrect.bottom+1-windowrect.top;
  int windowx  = (windowstyle & DS_CENTER) ? ((screencx-windowcx) >> 1)
                                           : CONVDLGX(parsedtemplate.x);
  int windowy  = (windowstyle & DS_CENTER) ? ((screency-windowcy) >> 1)
                                           : CONVDLGY(parsedtemplate.y);
  windowx = max(0,min(windowx,screencx-windowcx));
  windowy = max(0,min(windowy,screency-windowcy));

  // IF THE APPLICATION'S FRAME WINDOW IS MINIMIZED, RESTORE IT
  {
    if (framewindow && IsIconic(framewindow)) {
      ShowWindow(framewindow,SW_SHOWMAXIMIZED);
      while (DoMessageLoop((HWND)0))
        ;
      SetActiveWindow(parentwindow);
      SetFocus(parentwindow);
    }
  }

  // CREATE THE DIALOG BOX WINDOW
  HWND dialogwindow = CreateWindowEx(windowexstyle,
                                     "SDlgDialog",
                                     ansititle,
                                     windowstyle,
                                     windowx,
                                     windowy,
                                     windowcx,
                                     windowcy,
                                     parentwindow,
                                     (HMENU)0,
                                     instance,
                                     dialogproc);
  if (!dialogwindow)
    return dialogwindow;

  // SELECT THE FONT INTO THE DIALOG WINDOW
  {
    HDC dc = GetDC(dialogwindow);
    SelectObject(dc,font);
    ReleaseDC(dialogwindow,dc);
  }

  // SET THE DEFAULT RETURN VALUE
  SetProp(dialogwindow,"SDlg_EndDialog",(HANDLE)0);
  SetProp(dialogwindow,"SDlg_EndResult",(HANDLE)0);

  // CREATE THE CONTROLS
  HWND focuswindow = dialogwindow;
  {
    LPDLGITEMTEMPLATE itemdata = firstitem;
    for (int itemnum = 0; itemnum < parsedtemplate.cdit; ++itemnum) {

      // ADVANCE TO THE NEXT DOUBLEWORD BOUNDARY
      {
        int misaligned = ((LPBYTE)itemdata-(LPBYTE)templatedata) & 3;
        if (misaligned)
          itemdata = (LPDLGITEMTEMPLATE)(((LPBYTE)itemdata)+4-misaligned);
      }

      // PARSE THE CONTROL TEMPLATE
      if (extendedformat)
        itemdata = (LPDLGITEMTEMPLATE)(((LPBYTE)itemdata)+sizeof(DWORD));
      LPCWSTR controlclassname = (LPCWSTR)(itemdata+1);
      if (extendedformat)
        ++controlclassname;
      LPCWSTR controltitle;
      LPCVOID controldata;
      if (*controlclassname == 0xFFFF)
        controltitle = controlclassname+2;
      else
        controltitle = controlclassname+wcslen(controlclassname)+1;
      if (*controltitle == 0xFFFF)
        controldata = controltitle+3;
      else
        controldata = controltitle+wcslen(controltitle)+2;

      // CONVERT THE CLASS NAME AND TITLE TO ANSI
      char ansicontrolclassname[256] = "";
      char ansicontroltitle[256]     = "";
      if (*controlclassname == 0xFFFF)
        switch (*(controlclassname+1)) {
          case 0x0080: SStrCopy(ansicontrolclassname,"Button"    ,256);  break;
          case 0x0081: SStrCopy(ansicontrolclassname,"Edit"      ,256);  break;
          case 0x0082: SStrCopy(ansicontrolclassname,"SDlgStatic",256);  break;
          case 0x0083: SStrCopy(ansicontrolclassname,"Listbox"   ,256);  break;
          case 0x0084: SStrCopy(ansicontrolclassname,"Scrollbar" ,256);  break;
          case 0x0085: SStrCopy(ansicontrolclassname,"Combobox"  ,256);  break;
        }
      else
        wcstombs(ansicontrolclassname,controlclassname,255);
      if (*controltitle != 0xFFFF)
        wcstombs(ansicontroltitle,controltitle,255);

      // CREATE THE CONTROL WINDOW
      DWORD style   = extendedformat ? itemdata->dwExtendedStyle : itemdata->style;
      DWORD exstyle = extendedformat ? itemdata->style : itemdata->dwExtendedStyle;
      HWND  window  = CreateWindowEx(exstyle,
                                     ansicontrolclassname,
                                     ansicontroltitle,
                                     style,
                                     CONVDLGX(itemdata->x),
                                     CONVDLGY(itemdata->y),
                                     CONVDLGX(itemdata->cx),
                                     CONVDLGY(itemdata->cy),
                                     dialogwindow,
                                     (HMENU)itemdata->id,
                                     instance,
                                     (LPVOID)controldata);

      // SUBCLASS THE CONTROL, SET ITS INTERNAL PROPERTIES, THEN SHOW IT
      SetProp(window,"SDlg_WndProc",(HANDLE)GetWindowLong(window,GWL_WNDPROC));
      SetWindowLong(window,GWL_WNDPROC,(LONG)ControlSubclassWndProc);
      if (window && font)
        SendMessage(window,WM_SETFONT,(WPARAM)font,0);
      ShowWindow(window,SW_SHOW);

      // DETERMINE WHETHER THIS CONTROL SHOULD RECEIVE THE KEYBOARD FOCUS
      if (window &&
          (focuswindow == dialogwindow) &&
          (!(itemdata->style & WS_DISABLED)) &&
          _stricmp(ansicontrolclassname,"Static") &&
          _stricmp(ansicontrolclassname,"SDlgStatic"))
        focuswindow = window;

      itemdata = (LPDLGITEMTEMPLATE)(((LPBYTE)controldata)+*((LPWORD)controldata-1));
    }
  }

  // SEND THE WM_SETFONT MESSAGE
  if (font)
    SendMessage(dialogwindow,WM_SETFONT,(WPARAM)font,0);

  // SEND THE WM_INITDIALOG MESSAGE
  if (!SendMessage(dialogwindow,WM_INITDIALOG,(WPARAM)focuswindow,initparam))
    focuswindow = (HWND)0;

  // TURN ON THE OWNER DRAW STYLE FOR ANY CONTROLS FOR WHICH WE HAVE TEXTURES
  {
    HWND window = GetTopWindow(dialogwindow);
    while (window) {
      if (FindBitmap(window,ANYBITMAP)) {
        DWORD origstyle      = (DWORD)GetWindowLong(window,GWL_STYLE);
        SetProp(window,"SDlg_OrigStyle",(HANDLE)origstyle);
        DWORD style          = origstyle;
        char  classname[256] = "";
        GetClassName(window,classname,256);
        if ((!_stricmp(classname,"Button")) &&
            (((origstyle & 0x0000000F) == BS_PUSHBUTTON) ||
             ((origstyle & 0x0000000F) == BS_DEFPUSHBUTTON)))
          style |= BS_OWNERDRAW;
        else if (!_stricmp(classname,"ComboBox"))
          style |= CBS_OWNERDRAWFIXED;
        else if (!_stricmp(classname,"ListBox"))
          style |= LBS_OWNERDRAWFIXED;
        if (style != origstyle)
          SetWindowLong(window,GWL_STYLE,(LONG)style);
      }
      window = GetNextWindow(window,GW_HWNDNEXT);
    }
  }

  // UNLESS THE APPLICATION CALLED ENDDIALOG() DURING THE WM_INITDIALOG
  // MESSAGE, SHOW THE DIALOG WINDOW AND SET THE FOCUS
  if (!GetProp(dialogwindow,"SDlg_EndDialog")) {
    ShowWindow(dialogwindow,SW_SHOWNORMAL);
    if (focuswindow)
      SetFocus(focuswindow);
  }

  return dialogwindow;
}

//===========================================================================
static void IntersectRgnWithWindow (HWND updatewindow,
                                    HRGN region,
                                    HWND window) {
  VALIDATEBEGIN;
  VALIDATE(updatewindow);
  VALIDATE(region);
  VALIDATE(window);
  VALIDATEENDVOID;

  if (!IsWindowVisible(window))
    return;

  // INTERSECT THIS WINDOW WITH THE REGION
  {
    DWORD exstyle = GetWindowLong(window,GWL_EXSTYLE);
    if ((window != updatewindow) &&
        !(exstyle & WS_EX_TRANSPARENT)) {
      RECT rect;
      GetWindowRect(window,&rect);
      ScreenToClient(updatewindow,(LPPOINT)&rect.left);
      ScreenToClient(updatewindow,(LPPOINT)&rect.right);
      if (RectInRegion(region,&rect)) {
        HRGN rectregion = CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);
        CombineRgn(region,region,rectregion,RGN_DIFF);
        DeleteObject(rectregion);
      }
    }
  }

  // RECURSIVELY INTERSECT CHILD WINDOWS
  {
    HWND childwindow = GetTopWindow(window);
    while (childwindow) {
      IntersectRgnWithWindow(updatewindow,region,childwindow);
      childwindow = GetNextWindow(childwindow,GW_HWNDNEXT);
    }
  }

}

//===========================================================================
static BOOL IsButton (HWND window, BOOL *ownerdraw) {
  if (ownerdraw)
    *ownerdraw = 0;

  // DETERMINE WHETHER THIS IS A BUTTON WINDOW
  if (!IsWindow(window))
    return FALSE;
  char classname[256] = "";
  GetClassName(window,classname,256);
  if (_stricmp(classname,"Button"))
    return FALSE;

  // DETERMINE WHETHER IT IS OWNER DRAWN
  DWORD style = (DWORD)GetWindowLong(window,GWL_STYLE);
  if (ownerdraw)
    *ownerdraw = ((style & 0x0000000F) == BS_OWNERDRAW);

  return TRUE;
}

//===========================================================================
static BOOL IsPointInWindow (LPPOINTS point, HWND window) {
  RECT rect;
  GetClientRect(window,&rect);
  return ((point->x >= rect.left) &&
          (point->x <  rect.right) &&
          (point->y >= rect.top) &&
          (point->y <  rect.bottom));
}

//===========================================================================
static void ParseDlgTemplate (LPCDLGTEMPLATE     templatedata,
                              LPDLGTEMPLATE      parsedtemplate,
                              LPCWSTR           *title,
                              short             *fontsize,
                              short             *fontweight,
                              short             *fontitalics,
                              LPCWSTR           *fontname,
                              LPDLGITEMTEMPLATE *firstitem) {

  // PREPARE THE PARSED TEMPLATE STRUCTURE
  CopyMemory(parsedtemplate,templatedata,sizeof(DLGTEMPLATE));

  // FIND THE TITLE
  {
    LPCWSTR menuname = (LPCWSTR)(templatedata+1);
    LPCWSTR classname;
    if ((*menuname == 0x0000) || (*menuname == 0xFFFF))
      classname = menuname+1;
    else
      classname = menuname+wcslen(menuname)+1;
    if ((*classname == 0x0000) || (*classname == 0xFFFF))
      *title = classname+1;
    else
      *title = classname+wcslen(classname)+1;
  }

  // FIND THE FONT INFORMATION
  if (parsedtemplate->style & DS_SETFONT) {
    *fontsize    = *((*title)+wcslen(*title)+1);
    *fontweight  = 0;
    *fontitalics = 0;
    *fontname    = (*title)+wcslen(*title)+2;
    *firstitem   = (LPDLGITEMTEMPLATE)((*fontname)+wcslen(*fontname)+1);
  }
  else {
    *fontsize    = 0;
    *fontweight  = 0;
    *fontitalics = 0;
    *fontname    = NULL;
    *firstitem   = (LPDLGITEMTEMPLATE)((*title)+wcslen(*title)+1);
  }

}

//===========================================================================
static void ParseDlgTemplateEx (LPCDLGTEMPLATE     templatedata,
                                LPDLGTEMPLATE      parsedtemplate,
                                LPCWSTR           *title,
                                short             *fontsize,
                                short             *fontweight,
                                short             *fontitalics,
                                LPCWSTR           *fontname,
                                LPDLGITEMTEMPLATE *firstitem) {

  // PREPARE THE PARSED TEMPLATE STRUCTURE
  {
    CopyMemory(parsedtemplate,((LPBYTE)templatedata)+8,sizeof(DLGTEMPLATE));
    DWORD temp = parsedtemplate->style;
    parsedtemplate->style           = parsedtemplate->dwExtendedStyle;
    parsedtemplate->dwExtendedStyle = temp;
  }

  // FIND THE TITLE
  {
    LPCWSTR menuname = (LPCWSTR)(((LPBYTE)templatedata)+26);
    LPCWSTR classname;
    if ((*menuname == 0x0000) || (*menuname == 0xFFFF))
      classname = menuname+1;
    else
      classname = menuname+wcslen(menuname)+1;
    if ((*classname == 0x0000) || (*classname == 0xFFFF))
      *title = classname+1;
    else
      *title = classname+wcslen(classname)+1;
  }

  // FIND THE FONT INFORMATION
  if (parsedtemplate->style & DS_SETFONT) {
    const short *ptr = (const short *)((*title)+wcslen(*title)+1);
    *fontsize    = *ptr++;
    *fontweight  = *ptr++;
    *fontitalics = (*ptr++) & 1;
    *fontname    = (LPCWSTR)ptr;
    *firstitem   = (LPDLGITEMTEMPLATE)((*fontname)+wcslen(*fontname)+1);
  }
  else {
    *fontsize    = 0;
    *fontweight  = 0;
    *fontitalics = 0;
    *fontname    = NULL;
    *firstitem   = (LPDLGITEMTEMPLATE)((*title)+wcslen(*title)+1);
  }
}

//===========================================================================
static LRESULT SendMessageNoDefProc (HWND   window,
                                     UINT   message,
                                     WPARAM wparam,
                                     LPARAM lparam) {
  s_nodefproc = 1;
  return SendMessage(window,message,wparam,lparam);
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
HDC APIENTRY SDlgBeginPaint (HWND window, LPPAINTSTRUCT ps) {
  VALIDATEBEGIN;
  VALIDATE(window);
  VALIDATE(ps);
  VALIDATEEND;

  if (!IsWindow(window))
    return (HDC)0;
  ++s_inpaint;

  // IF THIS WINDOW DOESN'T HAVE A SELECTED BACKGROUND, OR IF IT IS
  // TRANSPARENT, JUST CALL THE NORMAL BEGINPAINT() FUNCTION AND LET IT
  // PAINT THE BACKGROUND WITH THE BRUSH REGISTERED IN THE WINDOW CLASS
  if ((GetWindowLong(window,GWL_EXSTYLE) & WS_EX_TRANSPARENT) ||
      !FindBitmap(window,SDLG_USAGE_BACKGROUND))
    return BeginPaint(window,ps);

  // SAVE THE UPDATE REGION FOR THIS WINDOW
  HRGN region = CreateRectRgn(0,0,1,1);
  GetUpdateRgn(window,region,0);

  // TEMPORARILY SAVE AND REMOVE THE BACKGROUND BRUSH FOR THIS WINDOW'S
  // CLASS, SO THAT BEGINPAINT() WILL NOT TRY TO ERASE THE BACKGROUND
  DWORD brush = GetClassLong(window,GCL_HBRBACKGROUND);
  SetClassLong(window,GCL_HBRBACKGROUND,0);

  // CALL BEGINPAINT()
  HDC dc = BeginPaint(window,ps);

  // RESTORE THE BACKGROUND BRUSH
  SetClassLong(window,GCL_HBRBACKGROUND,brush);

  // IF THE BACKGROUND DOES NOT NEED ERASING, RETURN NOW
  if (!ps->fErase) {
    DeleteObject(region);
    return dc;
  }

  // ERASE THE BACKGROUND
  SDlgDrawBitmap(window,
                 SDLG_USAGE_BACKGROUND,
                 region,
                 0,
                 0,
                 NULL,
                 SDLG_DBF_TILE | SDLG_DBF_VCENTER);
  ps->fErase = 0;

  DeleteObject(region);
  return dc;
}

//===========================================================================
BOOL APIENTRY SDlgBltToWindowE (HWND   window,
                                HRGN   region,
                                int    x,
                                int    y,
                                LPBYTE bitmapbits,
                                LPRECT bitmaprect,
                                LPSIZE bitmapsize,
                                DWORD  colorkey,
                                DWORD  pattern,
                                DWORD  rop3) {
  VALIDATEBEGIN;
  VALIDATE(window);
  VALIDATEEND;

  if (!(IsWindow(window) && IsWindowVisible(window) && !IsIconic(window)))
    return FALSE;

  // REJECT ANY ATTEMPT TO SET A TRANSPARENCY COLOR, SINCE TRANSPARENCY
  // IS NOT CURRENTLY IMPLEMENTED
  if (colorkey != 0xFFFFFFFF)
    return FALSE;

  // DETERMINE THE SCREEN DIMENSIONS
  SIZE screensize;
  SDrawGetScreenSize((int *)&screensize.cx,(int *)&screensize.cy);

  // DETERMINE THE DESTINATION LOCATION
  RECT clientrect;
  GetClientRect(window,&clientrect);
  clientrect.left += x;
  clientrect.top  += y;

  // DETERMINE WHETHER WE WILL HIDE AND/OR RESTORE THE CURSOR
  DWORD bltflags = BLT_FLAG_LOCKSURFACE | BLT_FLAG_DESTRECTCLIENTCOORDS;
  if (!s_cursorhidden) {
    bltflags |= BLT_FLAG_HIDECURSOR;
    if (!s_inpaint)
      bltflags |= BLT_FLAG_RESTORECURSOR;
  }

  // PERFORM THE BITBLT
  InternalBltClippedToWindow(NULL,
                             &clientrect,
                             &screensize,
                             0,
                             bitmapbits,
                             bitmaprect,
                             bitmapsize,
                             bitmapsize->cx,
                             0,
                             0,
                             pattern,
                             rop3,
                             window,
                             region,
                             bltflags,
                             SDRAW_SURFACE_FRONT,
                             s_cursorhidden ? NULL : &s_cursorhidden);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SDlgBltToWindowI (HWND   window,
                                HRGN   region,
                                int    x,
                                int    y,
                                LPBYTE bitmapbits,
                                LPRECT bitmaprect,
                                LPSIZE bitmapsize,
                                DWORD  colorkey,
                                DWORD  pattern,
                                DWORD  rop3) {
  RECT   exclrect;
  LPRECT exclrectptr = bitmaprect;
  if (bitmaprect) {
    exclrect.left   = bitmaprect->left;
    exclrect.top    = bitmaprect->top;
    exclrect.right  = bitmaprect->right+1;
    exclrect.bottom = bitmaprect->bottom+1;
    exclrectptr = &exclrect;
  }
  return SDlgBltToWindowE(window,
                          region,
                          x,
                          y,
                          bitmapbits,
                          exclrectptr,
                          bitmapsize,
                          colorkey,
                          pattern,
                          rop3);
}

//===========================================================================
BOOL APIENTRY SDlgCheckTimers () {
  TIMERPTR callback = NULL;
  DWORD    currtime = GetTickCount();
  ITERATELIST(TIMERREC,s_timerlist,curr)
    if (currtime-curr->lasttime >= curr->elapse) {
      if (currtime-curr->lasttime > 2*curr->elapse)
        curr->lasttime = currtime;
      else
        curr->lasttime += curr->elapse;
      if (curr->callback) {
        callback = curr;
        break;
      }
      else
        PostMessage(curr->window,WM_TIMER,curr->id,0);
    }
  if (callback)
    callback->callback(callback->window,WM_TIMER,callback->id,currtime);
  return TRUE;
}

//===========================================================================
HWND APIENTRY SDlgCreateDialogIndirectParam (HINSTANCE      instance,
                                             LPCDLGTEMPLATE templatedata,
                                             HWND           parentwindow,
                                             DLGPROC        dialogproc,
                                             LPARAM         initparam) {
  if (!s_initialized)
    if (!InitializeDialogManager())
      return (HWND)0;

  return InternalCreateDialogBox(instance,
                                 templatedata,
                                 parentwindow,
                                 dialogproc,
                                 initparam);
}

//===========================================================================
HWND APIENTRY SDlgCreateDialogParam (HINSTANCE instance,
                                     LPCTSTR   templatename,
                                     HWND      parentwindow,
                                     DLGPROC   dialogproc,
                                     LPARAM    initparam) {
  LPCDLGTEMPLATE templatedata = GetTemplateData(instance,templatename);
  if (!templatedata)
    return (HWND)0;

  return SDlgCreateDialogIndirectParam((HINSTANCE)GetModuleHandle(NULL),
                                       templatedata,
                                       parentwindow,
                                       dialogproc,
                                       initparam);
}

//===========================================================================
BOOL APIENTRY SDlgDefDialogProc (HWND   window,
                                 UINT   message,
                                 WPARAM wparam,
                                 LPARAM lparam) {
  switch (message) {

    case WM_CTLCOLORSTATIC:
      {
        char classname[256] = "";
        GetClassName((HWND)lparam,classname,256);
        if (!_stricmp(classname,"SDlgStatic")) {
          SetTextColor((HDC)wparam,0xFFFFFF);
          SetBkMode((HDC)wparam,TRANSPARENT);
          return (BOOL)GetStockObject(NULL_BRUSH);
        }
      }
      // IF IT'S NOT A STATIC CONTROL, FALL THROUGH TO WM_CTLCOLOREDIT

    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
      SetTextColor((HDC)wparam,0xFFFFFF);
      SetBkColor((HDC)wparam,0);
      SetBkMode((HDC)wparam,OPAQUE);
      return (BOOL)GetStockObject(BLACK_BRUSH);

    case WM_INITDIALOG:
      return 1;

  }
  return 0;
}

//===========================================================================
BOOL APIENTRY SDlgDestroy () {
  SDlgSetSystemCursor(NULL,NULL,NULL,OCR_NORMAL);
  TIMERPTR curr;
  while ((curr = s_timerlist.Head()) != NULL)
    SDlgKillTimer(curr->window,curr->id);
  s_bitmaplist.Clear();
  if (s_basefont) {
    DEL(s_basefont);
    s_basefont = NULL;
  }
  return TRUE;
}

//===========================================================================
int APIENTRY SDlgDialogBoxIndirectParam (HINSTANCE      instance,
                                         LPCDLGTEMPLATE templatedata,
                                         HWND           parentwindow,
                                         DLGPROC        dialogproc,
                                         LPARAM         initparam) {
  if (!s_initialized)
    if (!InitializeDialogManager())
      return -1;

  // CREATE THE DIALOG BOX
  HWND dialogwindow = InternalCreateDialogBox(instance,
                                              templatedata,
                                              parentwindow,
                                              dialogproc,
                                              initparam);
  if (!dialogwindow)
    return -1;

  // FLAG THE DIALOG BOX AS MODAL AND DISABLE THE PARENT WINDOW
  if (parentwindow && (parentwindow != GetDesktopWindow())) {
    SetProp(dialogwindow,"SDlg_Modal",(HANDLE)1);
    if (!IsIconic(parentwindow))
      EnableWindow(parentwindow,0);
  }

  // ENTER THE MESSAGE LOOP
  while (!GetProp(dialogwindow,"SDlg_EndDialog"))
    DoMessageLoop(dialogwindow);
  int result = (int)GetProp(dialogwindow,"SDlg_EndResult");
  RemoveProp(dialogwindow,"SDlg_EndDialog");
  RemoveProp(dialogwindow,"SDlg_EndResult");
  RemoveProp(dialogwindow,"SDlg_Modal");
  DestroyWindow(dialogwindow);

  // REENABLE THE PARENT WINDOW
  if (parentwindow && (parentwindow != GetDesktopWindow()))
    EnableWindow(parentwindow,1);

  // RETURN THE VALUE THAT WAS PASSED TO SDLGENDDIALOG()
  return result;
}

//===========================================================================
int APIENTRY SDlgDialogBoxParam (HINSTANCE instance,
                                 LPCTSTR   templatename,
                                 HWND      parentwindow,
                                 DLGPROC   dialogproc,
                                 LPARAM    initparam) {
  LPCDLGTEMPLATE templatedata = GetTemplateData(instance,templatename);
  if (!templatedata)
    return -1;

  return SDlgDialogBoxIndirectParam((HINSTANCE)GetModuleHandle(NULL),
                                    templatedata,
                                    parentwindow,
                                    dialogproc,
                                    initparam);
}

//===========================================================================
BOOL APIENTRY SDlgDrawBitmap (HWND   window,
                              DWORD  usage,
                              HRGN   region,
                              int    offsetx,
                              int    offsety,
                              LPRECT boundingoffset,
                              DWORD  flags) {
  VALIDATEBEGIN;
  VALIDATE(window);
  VALIDATE(usage);
  VALIDATEEND;

  if (!IsWindow(window))
    return FALSE;
  if ((usage == SDLG_USAGE_BACKGROUND) &&
      (GetWindowLong(window,GWL_EXSTYLE) & WS_EX_TRANSPARENT))
    return TRUE;

  // FIND THE REQUESTED BITMAP
  BITMAPPTR bitmap = FindBitmap(window,usage);
  if (!bitmap)
    return FALSE;

  // DETERMINE THE BOUNDING RECTANGLE, IN SCREEN COORDINATES, FOR THIS
  // WINDOW'S CLIENT AREA
  RECT boundingrect;
  GetClientRect(window,&boundingrect);
  ClientToScreen(window,(LPPOINT)&boundingrect.left);
  ClientToScreen(window,(LPPOINT)&boundingrect.right);

  // APPLY ANY BOUNDING RECTANGLE OFFSETS
  if (boundingoffset) {
    boundingrect.left   += max(0,boundingoffset->left);
    boundingrect.top    += max(0,boundingoffset->top);
    boundingrect.right  += min(0,boundingoffset->right);
    boundingrect.bottom += min(0,boundingoffset->bottom);
  }

  // IF WE'RE NOT TILING THEN CLIP THE SIZE OF THE BOUNDING RECTANGLE
  // TO THE SIZE OF THE BITMAP
  if (!(flags & SDLG_DBF_TILE)) {
    boundingrect.right = min(boundingrect.right,
                             boundingrect.left+bitmap->rect.right-bitmap->rect.left);
    if (flags & SDLG_DBF_VCENTER) {
      int vertspace = ((boundingrect.bottom-boundingrect.top)
                         -(bitmap->rect.bottom-bitmap->rect.top))/2;
      if (vertspace > 0) {
        boundingrect.top    += vertspace;
        boundingrect.bottom  = boundingrect.top+bitmap->rect.bottom-bitmap->rect.top;
      }
    }
    else
      boundingrect.bottom = min(boundingrect.bottom,
                                boundingrect.top+bitmap->rect.bottom-bitmap->rect.top);
  }

  // DETERMINE THE BITMAP AND SCREEN DIMENSIONS
  SIZE bitmapsize  = {bitmap->width,bitmap->height};
  SIZE screensize;
  SDrawGetScreenSize((int *)&screensize.cx,(int *)&screensize.cy);

  // DETERMINE WHETHER WE WILL HIDE AND/OR RESTORE THE CURSOR
  DWORD bltflags = BLT_FLAG_LOCKSURFACE | BLT_FLAG_TILED;
  if (!s_cursorhidden) {
    bltflags |= BLT_FLAG_HIDECURSOR;
    if (!s_inpaint)
      bltflags |= BLT_FLAG_RESTORECURSOR;
  }

  // PERFORM THE BITBLT
  InternalBltClippedToWindow(NULL,
                             &boundingrect,
                             &screensize,
                             0,
                             bitmap->bitmapbits,
                             &bitmap->rect,
                             &bitmapsize,
                             bitmap->width,
                             offsetx,
                             offsety,
                             0,
                             SRCCOPY,
                             window,
                             region,
                             bltflags,
                             SDRAW_SURFACE_FRONT,
                             s_cursorhidden ? NULL : &s_cursorhidden);

  return 1;
}

//===========================================================================
BOOL APIENTRY SDlgEndDialog (HWND window,
                             int  result) {
  SetProp(window,"SDlg_EndDialog",(HANDLE)1);
  SetProp(window,"SDlg_EndResult",(HANDLE)result);
  return EndDialog(window,result);
}

//===========================================================================
BOOL APIENTRY SDlgEndPaint (HWND window, LPPAINTSTRUCT ps) {
  if (s_cursorhidden)
    do
      ShowCursor(1);
    while (--s_cursorhidden);
  --s_inpaint;
  return EndPaint(window,ps);
}

//===========================================================================
BOOL APIENTRY SDlgKillTimer (HWND window,
                             UINT event) {
  BOOL found = FALSE;
  ITERATELIST(TIMERREC,s_timerlist,curr)
    if ((curr->window == window) &&
        (curr->id     == event)) {
      found = TRUE;
      ITERATE_DELETE;
    }
  return found;
}

//===========================================================================
BOOL APIENTRY SDlgSetBaseFont (int     pointsize,
                               int     weight,
                               DWORD   flags,
                               DWORD   family,
                               LPCTSTR face) {
  if (!(pointsize && weight && face && *face))
    return FALSE;
  if (!s_basefont)
    s_basefont = NEW(BASEFONT);
  s_basefont->pointsize = pointsize;
  s_basefont->weight    = weight;
  s_basefont->flags     = flags;
  s_basefont->family    = family;
  SStrCopy(s_basefont->face,face,32);
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SDlgSetBitmapE (HWND     window,
                              HWND     parentwindow,
                              LPCTSTR  controltype,
                              DWORD    controlstyle,
                              DWORD    usage,
                              LPBYTE   bitmapbits,
                              LPRECT   rect,
                              int      width,
                              int      height,
                              COLORREF colorkey) {
  VALIDATEBEGIN;
  VALIDATE(window || (controltype && *controltype));
  VALIDATE(usage);
  VALIDATE(!(usage & ~ANYBITMAP));
  VALIDATEEND;

  if (!controltype)
    controltype = "";

  // REJECT ANY ATTEMPT TO SET A TRANSPARENCY COLOR, SINCE TRANSPARENCY
  // IS NOT CURRENTLY IMPLEMENTED
  if (colorkey != 0xFFFFFFFF)
    return FALSE;

  // REMOVE ANY BITMAPS ASSOCIATED WITH WINDOWS THAT NO LONGER EXIST
  {
    ITERATELIST(BITMAPREC,s_bitmaplist,curr)
      if ((curr->window       && !IsWindow(curr->window)) ||
          (curr->parentwindow && !IsWindow(curr->parentwindow)))
        ITERATE_DELETE;
  }

  // REMOVE THE PREVIOUS BITMAP FOR THIS WINDOW
  {
    BITMAPPTR curr = s_bitmaplist.Head();
    while (curr &&
           ((curr->window       != window)       ||
            (curr->parentwindow != parentwindow) ||
            (curr->usage        != usage)        ||
            (curr->controlstyle != controlstyle) ||
            _stricmp(curr->controltype,controltype)))
      curr = curr->Next();
    if (curr)
      s_bitmaplist.DeleteNode(curr);
  }

  // ADD A NEW RECORD FOR THIS BITMAP
  if (bitmapbits && (width > 0) && (height > 0)) {
    BITMAPPTR bitmap = s_bitmaplist.NewNode(LIST_HEAD);
    bitmap->window       = window;
    bitmap->parentwindow = parentwindow;
    bitmap->usage        = usage;
    bitmap->controlstyle = controlstyle;
    bitmap->bitmapbits   = bitmapbits;
    bitmap->width        = width;
    bitmap->height       = height;
    bitmap->offsetx      = 0;
    bitmap->offsety      = 0;
    BOOL overflow = 0;
    if (rect) {
      bitmap->rect.left    = rect->left % width;
      bitmap->rect.top     = rect->top  % height;
      bitmap->rect.right   = bitmap->rect.left+rect->right-rect->left;
      bitmap->rect.bottom  = bitmap->rect.top+rect->bottom-rect->top;
      if ((bitmap->rect.right  > width) ||
          (bitmap->rect.bottom > height)) {
        overflow = 1;
        bitmap->offsetx = bitmap->rect.left;
        bitmap->offsety = bitmap->rect.top;
      }
    }
    if ((!rect) || overflow) {
      bitmap->rect.left   = 0;
      bitmap->rect.top    = 0;
      bitmap->rect.right  = width;
      bitmap->rect.bottom = height;
    }
    SStrCopy(bitmap->controltype,controltype,CONTROLTYPELENGTH);
  }

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SDlgSetBitmapI (HWND     window,
                              HWND     parentwindow,
                              LPCTSTR  controltype,
                              DWORD    controlstyle,
                              DWORD    usage,
                              LPBYTE   bitmapbits,
                              LPRECT   rect,
                              int      width,
                              int      height,
                              COLORREF colorkey) {
  RECT   exclrect;
  LPRECT exclrectptr = rect;
  if (rect) {
    exclrect.left   = rect->left;
    exclrect.top    = rect->top;
    exclrect.right  = rect->right+1;
    exclrect.bottom = rect->bottom+1;
    exclrectptr = &exclrect;
  }
  return SDlgSetBitmapE(window,
                        parentwindow,
                        controltype,
                        controlstyle,
                        usage,
                        bitmapbits,
                        exclrectptr,
                        width,
                        height,
                        colorkey);
}

//===========================================================================
BOOL APIENTRY SDlgSetControlBitmaps (HWND    parentwindow,
                                     LPINT   controllist,
                                     LPDWORD usagelist,
                                     LPBYTE  bitmapbits,
                                     LPSIZE  bitmapsize,
                                     DWORD   adjusttype,
                                     DWORD   colorkey) {
  VALIDATEBEGIN;
  VALIDATE(parentwindow);
  VALIDATE(controllist);
  VALIDATE(bitmapbits);
  VALIDATE(bitmapsize);
  VALIDATE(bitmapsize->cx > 0);
  VALIDATE(bitmapsize->cy > 0);
  VALIDATEEND;

  // IF THE USAGE LIST WAS NOT PROVIDED, SUPPLY A DEFAULT LIST
  // BASED ON THE ADJUSTMENT TYPE
  static const DWORD defaultcontrolposlist[2] = {SDLG_USAGE_BACKGROUND,
                                                 0};
  static const DWORD defaultverticallist[6]   = {SDLG_USAGE_NORMAL_UNFOCUSED,
                                                 SDLG_USAGE_SELECTED_UNFOCUSED,
                                                 SDLG_USAGE_NORMAL_FOCUSED,
                                                 SDLG_USAGE_SELECTED_FOCUSED,
                                                 SDLG_USAGE_GRAYED,
                                                 0};
  if (!usagelist)
    if (adjusttype == SDLG_ADJUST_VERTICAL)
      usagelist = (LPDWORD)&defaultverticallist[0];
    else
      usagelist = (LPDWORD)&defaultcontrolposlist[0];

  // PROCESS ALL CONTROLS AND USAGE TYPES
  RECT rect    = {0,0,0,0};
  BOOL success = 1;
  for (LPINT currcontrol = controllist; *currcontrol; ++currcontrol) {
    HWND window = GetDlgItem(parentwindow,*currcontrol);
    if (window) {
      RECT clientrect;  
      RECT windowrect;
      GetClientRect(window,&clientrect);
      GetWindowRect(window,&windowrect);
      for (LPDWORD currusage = usagelist; *currusage; ++currusage) {

        // IF REQUESTED, ADJUST THE RECTANGLE BASED ON THE CONTROL POSITION
        if (adjusttype == SDLG_ADJUST_CONTROLPOS) {
          rect.left = windowrect.left;
          rect.top  = windowrect.top;
          ScreenToClient(parentwindow,(LPPOINT)&rect);
        }

        // DETERMINE THE WIDTH AND HEIGHT OF THE RECTANGLE BASED ON THE
        // SIZE OF THE WINDOW
        rect.right  = rect.left+clientrect.right-clientrect.left;
        rect.bottom = rect.top+clientrect.bottom-clientrect.top;

        // REGISTER THE BITMAP
        if (!SDlgSetBitmapE(window,
                            (HWND)0,
                            NULL,
                            SDLG_STYLE_ANY,
                            *currusage,
                            bitmapbits,
                            &rect,
                            bitmapsize->cx,
                            bitmapsize->cy,
                            colorkey))
          success = 0;

        // IF REQUESTED, OFFSET THE RECTANGLE VERTICALLY
        if (adjusttype == SDLG_ADJUST_VERTICAL) {
          rect.top = rect.bottom;
          if (rect.top >= bitmapsize->cy)
            rect.top = 0;
        }

      }
    }
    else
      success = 0;
  }

  return success;
}

//===========================================================================
BOOL APIENTRY SDlgSetCursor (HWND     window,
                             HCURSOR  cursor,
                             DWORD    id,
                             HCURSOR *oldcursor) {
  if (oldcursor)
    *oldcursor = (HCURSOR)0;
  LPCSTR  normalclasslist[6] = {"Button","SDlgStatic","ListBox","Scrollbar","ComboBox",NULL};
  LPCSTR  ibeamclasslist[2]  = {"Edit",NULL};
  LPCSTR *classlist;
  switch (id) {
    case OCR_NORMAL: classlist = normalclasslist;  break;
    case OCR_IBEAM:  classlist = ibeamclasslist;   break;
    default:         return 0;
  }
  if (id == OCR_NORMAL) {
    if (oldcursor)
      *oldcursor = (HCURSOR)GetClassLong(window,GCL_HCURSOR);
    SetClassLong(window,GCL_HCURSOR,(LONG)cursor);
  }
  while (*classlist) {
    HWND classwindow = FindWindowEx(window,(HWND)0,*classlist,NULL);
    if (classwindow) {
      if (oldcursor && (id != OCR_NORMAL))
        *oldcursor = (HCURSOR)GetClassLong(window,GCL_HCURSOR);
      SetClassLong(classwindow,GCL_HCURSOR,(LONG)cursor);
    }
    ++classlist;
  }
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SDlgSetSystemCursor (LPBYTE maskbitmap,
                                   LPBYTE imagebitmap,
                                   LPSIZE size,
                                   DWORD  id) {
  if (id != OCR_NORMAL)
    return FALSE;
  if (maskbitmap && imagebitmap) {
    SDlgSetSystemCursor(NULL,NULL,NULL,id);
    s_cursormask    = maskbitmap;
    s_cursorimage   = imagebitmap;
    s_cursorsize.cx = size->cx;
    s_cursorsize.cy = size->cy;
    POINT pt;
    GetCursorPos(&pt);
    AdjustCursorPos((HWND)0,pt.x,pt.y);
  }
  else {
    s_cursormask  = NULL;
    s_cursorimage = NULL;
    s_cursorpos.x = -1;
    s_cursorpos.y = -1;
  }
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SDlgSetTimer (HWND      window,
                            UINT      event,
                            UINT      elapse,
                            TIMERPROC timerfunc) {
  SDlgKillTimer(window,event);
  TIMERPTR ptr = s_timerlist.NewNode();
  ptr->window   = window;
  ptr->id       = event;
  ptr->elapse   = elapse;
  ptr->callback = timerfunc;
  ptr->lasttime = GetTickCount();
  return event;
}

//===========================================================================
BOOL APIENTRY SDlgUpdateCursor () {
  CheckCursorPos();
  return TRUE;
}

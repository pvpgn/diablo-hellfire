/****************************************************************************
*
*  SDRAW.CPP
*  Storm DirectDraw functions
*
*  By Michael O'Brien (2/8/96)
*
***/

#include "pch.h"
#pragma  hdrstop

#define  SURFACES  4

#define  REGKEY             "Internal"
#define  REGVAL_WINDOWMODE  "Window Mode"

typedef HRESULT (WINAPI *ddcreatetype)(GUID *,LPDIRECTDRAW *,IUnknown *);

static DWORD               s_asyncsystemblt      = 0;
static BOOL                s_autoinit            = FALSE;
static BOOL                s_createdgdipalette   = FALSE;
static HCURSOR             s_cursor              = (HCURSOR)0;
static BOOL                s_dlgactive           = FALSE;
static HWND                s_framewindow         = (HWND)0;
static HPALETTE            s_gdipalette          = (HPALETTE)0;
static HINSTANCE           s_libinst             = (HINSTANCE)0;
static LPDIRECTDRAW        s_lpdd                = NULL;
static LPDIRECTDRAWPALETTE s_palette             = NULL;
static PALETTEENTRY        s_paletteentries[256] = {0};
static BOOL                s_redirectingprimary  = FALSE;
static RECT                s_redirectingrect     = {0};
static int                 s_screenbitdepth      = 8;
static SIZE                s_screensize          = {640,480};
static LPDIRECTDRAWSURFACE s_surface[SURFACES]   = {NULL};

//===========================================================================
static BOOL CheckUseWindowMode () {
  static BOOL  init   = FALSE;
  static DWORD window = 0;
  if (!init) {
    init = TRUE;
    SRegLoadValue(REGKEY,REGVAL_WINDOWMODE,0,&window);
#ifdef _DEBUG
    SRegSaveValue(REGKEY,REGVAL_WINDOWMODE,0,window);
#endif
  }
  return (window != 0);
}

//===========================================================================
static void CreateGdiPalette () {
  if (s_palette)
    s_palette->GetEntries(0,0,256,&s_paletteentries[0]);
  {
    LOGPALETTE *gdipal = (LOGPALETTE *)ALLOC(sizeof(LOGPALETTE)+255*sizeof(PALETTEENTRY));
    gdipal->palVersion    = 0x300;
    gdipal->palNumEntries = 256;
    CopyMemory(gdipal->palPalEntry,&s_paletteentries[0],256*sizeof(PALETTEENTRY));
    s_gdipalette        = CreatePalette(gdipal);
    s_createdgdipalette = TRUE;
    FREE(gdipal);
  }
  if (s_framewindow && s_gdipalette) {
    HDC dc = GetDC(s_framewindow);
    SelectPalette(dc,s_gdipalette,0);
    ReleaseDC(s_framewindow,dc);
  }
}

//===========================================================================
static BOOL inline IsRunningInWindow (HWND window) {
  if (GetWindowLong(window,GWL_STYLE) & WS_MAXIMIZE)
    return FALSE;
  if (!(GetWindowLong(window,GWL_EXSTYLE) & WS_EX_TOPMOST))
    return TRUE;
  RECT rect;
  GetWindowRect(window,&rect);
  return ((rect.right  < GetSystemMetrics(SM_CXFULLSCREEN)) &&
          (rect.bottom < GetSystemMetrics(SM_CYFULLSCREEN)));
}

//===========================================================================
static void CALLBACK OnDestroy (LPPARAMS) {
  SDrawDestroy();
  PostQuitMessage(0);
}

//===========================================================================
static void CALLBACK OnPaletteChanged (LPPARAMS params) {
  if ((GetForegroundWindow() == SDrawGetFrameWindow()) &&
      ((HWND)(params->wparam) != SDrawGetFrameWindow()))
    SDrawRealizePalette();
}

//===========================================================================
static void CALLBACK OnQueryNewPalette (LPPARAMS params) {
  SDrawRealizePalette();
  params->useresult = TRUE;
  params->result    = TRUE;
}

//===========================================================================
static void CALLBACK OnVkSnapshot (LPPARAMS) {
  SDrawCaptureScreen(NULL);
}

//===========================================================================
static LRESULT CALLBACK WndProc (HWND   window,
                                 UINT   message,
                                 WPARAM wparam,
                                 LPARAM lparam) {
  BOOL    useresult = FALSE;
  LRESULT result    = 0;
  if (SMsgDispatchMessage(window,message,wparam,lparam,&useresult,&result))
    if (useresult)
      return result;
  return DefWindowProc(window,message,wparam,lparam);
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
BOOL APIENTRY SDrawAutoInitialize (HINSTANCE instance,
                                   LPCTSTR   classname,
                                   LPCTSTR   title,
                                   WNDPROC   wndproc,
                                   int       servicelevel,
                                   int       width,
                                   int       height,
                                   int       bitdepth) {
  VALIDATEBEGIN;
  VALIDATE(instance);
  VALIDATE(classname);
  VALIDATE(*classname);
  VALIDATE(title);
  VALIDATE(servicelevel > 0);
  VALIDATE(servicelevel <= SDRAW_SERVICE_MAX);
  VALIDATEEND;

  // STORE THE FACT THE WE AUTO-INITIALIZED DIRECTDRAW, SO THAT WE CAN
  // UNDO EVERYTHING AT DESTRUCTION TIME
  s_autoinit  = TRUE;
  s_dlgactive = FALSE;

  // CREATE A BLANK CURSOR
  if (!s_cursor) {
    int    cursorcx = GetSystemMetrics(SM_CXCURSOR);
    int    cursorcy = GetSystemMetrics(SM_CYCURSOR);
    int    bytes    = ((cursorcx+31)/32)*4*cursorcy;
    LPVOID buffer1  = ALLOC(bytes);
    LPVOID buffer2  = ALLOCZERO(bytes);
    FillMemory(buffer1,bytes,0xFF);
    s_cursor = CreateCursor(instance,
                                0,
                                0,
                                cursorcx,
                                cursorcy,
                                buffer1,
                                buffer2);
    FREE(buffer1);
    FREE(buffer2);
  }

  // REGISTER THE FRAME WINDOW CLASS
  {
    WNDCLASS wndclass;
    ZeroMemory(&wndclass,sizeof(WNDCLASS));
    wndclass.lpfnWndProc   = wndproc ? wndproc : WndProc;
    wndclass.hInstance     = instance;
    wndclass.hIcon         = LoadIcon(0,IDI_APPLICATION);
    wndclass.hCursor       = s_cursor;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.lpszClassName = classname;
    RegisterClass(&wndclass);
  }

  // CREATE THE FRAME WINDOW
  {
    DWORD exstyle;
    int   windowwidth, windowheight;
    if (CheckUseWindowMode()) {
      exstyle      = 0;
      windowwidth  = width;
      windowheight = height;
    }
    else {
      exstyle      = WS_EX_TOPMOST;
      windowwidth  = GetSystemMetrics(SM_CXSCREEN);
      windowheight = GetSystemMetrics(SM_CYSCREEN);
    }
    if (!(s_framewindow = CreateWindowEx(exstyle,
                                             classname,
                                             title,
                                             WS_POPUP | WS_VISIBLE,
                                             0,
                                             0,
                                             windowwidth,
                                             windowheight,
                                             (HWND)0,
                                             (HMENU)0,
                                             instance,
                                             NULL)))
      return FALSE;
  }

  // REGISTER MESSAGES FOR THE FRAME WINDOW
  SMsgRegisterMessage(s_framewindow,WM_DESTROY        ,OnDestroy);
  SMsgRegisterMessage(s_framewindow,WM_PALETTECHANGED ,OnPaletteChanged);
  SMsgRegisterMessage(s_framewindow,WM_QUERYNEWPALETTE,OnQueryNewPalette);
  SMsgRegisterKeyUp(s_framewindow,VK_SNAPSHOT,OnVkSnapshot);
  SMsgPushRegisterState(s_framewindow);

  // LOAD THE DIRECTDRAW LIBRARY
  if (!s_libinst)
    s_libinst = LoadLibrary("ddraw.dll");

  // INITIALIZE DIRECTDRAW
  if (s_libinst) {
    ddcreatetype ddcreatefunc = 
      (ddcreatetype)GetProcAddress(s_libinst,"DirectDrawCreate");
    if (ddcreatefunc)
      ddcreatefunc(NULL,&s_lpdd,NULL);
    if (s_lpdd) {
      if (CheckUseWindowMode())
        s_lpdd->SetCooperativeLevel(s_framewindow,DDSCL_NORMAL);
      else {
        s_lpdd->SetCooperativeLevel(s_framewindow,
                                        DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
        s_lpdd->SetDisplayMode(width,height,bitdepth);
      }
      s_screenbitdepth = bitdepth;
      s_screensize.cx  = width;
      s_screensize.cy  = height;
    }
  }

  // IF WE FAILED TO LOAD THE LIBRARY OR INITIALIZE DIRECTDRAW, INFORM
  // THE USER
  if (!s_lpdd) {
    MessageBox(s_framewindow,
               "DirectDraw services are not available.  You must install "
               "Microsoft DirectX version 2.0 or higher.  If you have "
               "difficulty installing or using Microsoft DirectX on your "
               "computer, please contact Microsoft Product Support "
               "Services.",
               title,
               MB_SETFOREGROUND | MB_ICONSTOP);
    return FALSE;
  }

  // DETERMINE THE CAPABILITIES OF THE HARDWARE
  s_asyncsystemblt = 0;
  {
    DDCAPS caps;
    ZeroMemory(&caps,sizeof(DDCAPS));
    caps.dwSize = sizeof(DDCAPS);
    if (s_lpdd->GetCaps(&caps,NULL) == DD_OK)
      if ((caps.dwCaps & DDCAPS_CANBLTSYSMEM) &&
          (caps.dwSVBCaps & DDCAPS_BLTQUEUE))
        s_asyncsystemblt = 1;
  }

  // CREATE THE VIDEO MEMORY SURFACES
  {
    DDSURFACEDESC desc;
    ZeroMemory(&desc,sizeof(DDSURFACEDESC));
    desc.dwSize         = sizeof(DDSURFACEDESC);
    desc.dwFlags        = DDSD_CAPS;
    desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    if (servicelevel == SDRAW_SERVICE_PAGEFLIP) {
      desc.dwFlags           |= DDSD_BACKBUFFERCOUNT;
      desc.dwBackBufferCount  = 1;
      desc.ddsCaps.dwCaps    |= DDSCAPS_FLIP | DDSCAPS_COMPLEX;
    }
    if (s_lpdd->CreateSurface(&desc,&s_surface[SDRAW_SURFACE_FRONT],NULL) != DD_OK)
      return FALSE;
    if (servicelevel == SDRAW_SERVICE_PAGEFLIP) {
      DDSCAPS caps;
      ZeroMemory(&caps,sizeof(DDSCAPS));
      caps.dwCaps = DDSCAPS_BACKBUFFER;
      if (s_surface[SDRAW_SURFACE_FRONT])
        s_surface[SDRAW_SURFACE_FRONT]->GetAttachedSurface(&caps,&s_surface[SDRAW_SURFACE_BACK]);
    }
  }

  // CREATE THE SYSTEM MEMORY SURFACE
  if (servicelevel == SDRAW_SERVICE_DOUBLEBUFFER) {
    DDSURFACEDESC desc;
    ZeroMemory(&desc,sizeof(DDSURFACEDESC));
    desc.dwSize         = sizeof(DDSURFACEDESC);
    desc.dwFlags        = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    desc.dwHeight       = height;
    desc.dwWidth        = width;
    s_lpdd->CreateSurface(&desc,&s_surface[SDRAW_SURFACE_SYSTEM],NULL);
  }

  // VERIFY THAT ALL SURFACES WERE CREATED SUCCESSFULLY
  {
    if (!s_surface[SDRAW_SURFACE_FRONT])
      return FALSE;
    if ((servicelevel == SDRAW_SERVICE_PAGEFLIP) &&
        !s_surface[SDRAW_SURFACE_BACK])
      return FALSE;
    if ((servicelevel == SDRAW_SERVICE_DOUBLEBUFFER) &&
        !s_surface[SDRAW_SURFACE_SYSTEM])
      return FALSE;
  }

  // ATTEMPT TO LOCK THE PRIMARY VIDEO SURFACE.  IF WE'RE UNABLE TO LOCK
  // IT, THEN CREATE A TEMPORARY SURFACE IN SYSTEM MEMORY TO BLT FROM.
  {
    DDSURFACEDESC desc;
    ZeroMemory(&desc,sizeof(DDSURFACEDESC));
    desc.dwSize = sizeof(DDSURFACEDESC);
    HRESULT result = s_surface[SDRAW_SURFACE_FRONT]->Lock(NULL,
                                                              &desc,
                                                              DDLOCK_WAIT,
                                                              NULL);
    if (desc.lpSurface)
      s_surface[SDRAW_SURFACE_FRONT]->Unlock(desc.lpSurface);
    else if ((result != DDERR_SURFACELOST) &&
             (result != DDERR_WASSTILLDRAWING)) {
      DDSURFACEDESC desc;
      ZeroMemory(&desc,sizeof(DDSURFACEDESC));
      desc.dwSize         = sizeof(DDSURFACEDESC);
      desc.dwFlags        = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
      desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
      desc.dwHeight       = height;
      desc.dwWidth        = width;
      s_lpdd->CreateSurface(&desc,&s_surface[SDRAW_SURFACE_TEMPORARY],NULL);
    }
  }

  // CREATE A DIRECTDRAW PALETTE AND A MATCHING GDI PALETTE
  if (bitdepth == 8) {
    {
      HDC dc = GetDC((HWND)0);
      GetSystemPaletteEntries(dc,0,256,&s_paletteentries[0]);
      ReleaseDC((HWND)0,dc);
    }
    {
      for (int loop = 10; loop < 246; ++loop)
        s_paletteentries[loop].peFlags = PC_RESERVED | PC_NOCOLLAPSE;
    }
    s_lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256
                                           | DDPCAPS_INITIALIZE,
                              &s_paletteentries[0],
                              &s_palette,
                              NULL);
    if (!s_palette) {
      SDrawDestroy();
      return FALSE;
    }
    CreateGdiPalette();
  }

  // BLANK OUT THE NEWLY CREATED SURFACES, AND SELECT THE NEW PALETTE
  // INTO EACH ONE
  {
    for (int loop = 0; loop < SURFACES; ++loop)
      if (s_surface[loop]) {
        SDrawClearSurface(loop);
        if (s_palette)
          s_surface[loop]->SetPalette(s_palette);
      }
  }

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SDrawCaptureScreen (LPCTSTR filename) {
  if (!(s_lpdd &&
        s_framewindow &&
        s_surface[SDRAW_SURFACE_FRONT]))
    return FALSE;
  if (s_screenbitdepth != 8)
    return FALSE;

  // SWITCH TO THE GDI SCREEN IF NECESSARY
  SDrawSelectGdiSurface(1,1);

  // SAVE THE CURRENT SURFACE
  LPBYTE savebuffer = (LPBYTE)ALLOC(s_screensize.cx*s_screensize.cy);
  if (s_palette)
    s_palette->GetEntries(0,0,256,&s_paletteentries[0]);
  {
    LPBYTE videobuffer;
    int    pitch;
    if (SDrawLockSurface(SDRAW_SURFACE_FRONT,NULL,&videobuffer,&pitch)) {
      SBltROP3(savebuffer,
               videobuffer,
               s_screensize.cx,
               s_screensize.cy,
               s_screensize.cx,
               pitch,
               0,
               SRCCOPY);
      SDrawUnlockSurface(SDRAW_SURFACE_FRONT,videobuffer);
    }
  }

  // IF NO FILENAME WAS PROVIDED, PROMPT THE USER FOR ONE
  char localfilename[MAX_PATH] = "";
  if (filename && *filename)
    SStrCopy(localfilename,filename,MAX_PATH);
  else {
    OPENFILENAME ofn;
    ZeroMemory(&ofn,sizeof(OPENFILENAME));
    ofn.lStructSize   = sizeof(OPENFILENAME);
    ofn.hwndOwner     = s_framewindow;
    ofn.hInstance     = (HINSTANCE)GetModuleHandle(NULL);
    ofn.lpstrFilter   = "Graphics Interchange (*.gif)\0*.gif\0"
                        "PC Paintbrush (*.pcx)\0*.pcx\0"
                        "Windows Bitmap (*.bmp)\0*.bmp\0"
                        "All Files (*.*)\0*.*\0";
    ofn.nFilterIndex  = 1;
    ofn.lpstrFile     = localfilename;
    ofn.nMaxFile      = MAX_PATH;
    ofn.lpstrTitle    = "Save Screen Capture";
    ofn.Flags         = OFN_HIDEREADONLY
                          | OFN_NOCHANGEDIR
                          | OFN_OVERWRITEPROMPT
                          | OFN_PATHMUSTEXIST;
    ofn.lpstrDefExt   = ".gif";
    s_dlgactive = 1;
    BOOL result = GetSaveFileName(&ofn);
    s_dlgactive = 0;
    if (!result) {
      FREE(savebuffer);
      return FALSE;
    }
  }

  // SAVE THE FILE
  SBmpSaveImage(localfilename,
                &s_paletteentries[0],
                savebuffer,
                s_screensize.cx,
                s_screensize.cy,
                s_screenbitdepth);

  FREE(savebuffer);
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SDrawClearSurface (int surfacenumber) {
  VALIDATEBEGIN;
  VALIDATE(surfacenumber >= 0);
  VALIDATE(surfacenumber < SURFACES);
  VALIDATEEND;

  if ((!s_lpdd) ||
      (!s_surface[surfacenumber]))
    return FALSE;

  // HIDE THE CURSOR IF IT IS VISIBLE
  int hidden = 0;
  do
    ++hidden;
  while (ShowCursor(0) >= 0);

  // CLEAR THE SURFACE
  {
    LPBYTE videobuffer;
    int    videopitch;
    if (SDrawLockSurface(surfacenumber,NULL,&videobuffer,&videopitch)) {
      SGdiSetPitch(videopitch);
      SGdiRectangle(videobuffer,
                    0,
                    0,
                    s_screensize.cx,
                    s_screensize.cy,
                    PALETTEINDEX(0));
      SDrawUnlockSurface(surfacenumber,videobuffer);
    }
  }

  // RESTORE THE CURSOR
  while (hidden--)
    ShowCursor(1);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SDrawDestroy () {

  // DESTROY OTHER MODULES WHICH RELY ON THIS MODULE
  BOOL result = TRUE;
#ifndef STATICLIB
  result &= SDlgDestroy();
  result &= SGdiDestroy();
  result &= SVidDestroy();
#endif

  // DESTROY THE GDI PALETTE IF WE CREATED ONE
  if (s_gdipalette && s_createdgdipalette) {
    s_createdgdipalette = 0;
    DeleteObject(s_gdipalette);
  }

  // RETURN THE SCREEN DIMENSIONS TO THEIR DEFAULTS
  s_screenbitdepth = 8;
  s_screensize.cx  = 640;
  s_screensize.cy  = 480;

  // IF DIRECTDRAW WAS INITIALIZED THROUGH SDRAWAUTOINITIALIZE(), RELEASE
  // ALL DIRECTDRAW OBJECTS THAT WE CREATED
  if (s_autoinit) {
    s_autoinit = FALSE;
    if (s_cursor)
      DestroyCursor(s_cursor);
  }

  // ZERO OUT ALL OBJECT POINTERS AND HANDLES
  s_gdipalette = (HPALETTE)0;
  s_palette    = NULL;
  for (int loop = SURFACES-1; loop >= 0; --loop)
    s_surface[loop] = NULL;
  s_lpdd        = NULL;
  s_framewindow = (HWND)0;
  s_cursor      = (HCURSOR)0;

  return result;
}

//===========================================================================
BOOL APIENTRY SDrawFlipPage () {
  if (s_dlgactive ||
      !(s_lpdd &&
        s_surface[SDRAW_SURFACE_FRONT] &&
        s_surface[SDRAW_SURFACE_BACK]))
    return FALSE;

  HRESULT result;
  do {
    result = s_surface[SDRAW_SURFACE_FRONT]->Flip(NULL,DDFLIP_WAIT);
    if ((result == DDERR_SURFACELOST) && s_surface[SDRAW_SURFACE_FRONT])
      if (s_surface[SDRAW_SURFACE_FRONT]->Restore() == DD_OK)
        InvalidateRect(s_framewindow,NULL,0);
      else
        return FALSE;
  } while ((result == DDERR_WASSTILLDRAWING) ||
           (result == DDERR_SURFACELOST));

  return TRUE;
}

//===========================================================================
HWND APIENTRY SDrawGetFrameWindow (HWND *window) {
  if (window)
    *window = s_framewindow;
  return s_framewindow;
}

//===========================================================================
BOOL APIENTRY SDrawGetObjects (LPDIRECTDRAW        *directdraw,
                               LPDIRECTDRAWSURFACE *frontbuffer,
                               LPDIRECTDRAWSURFACE *backbuffer,
                               LPDIRECTDRAWSURFACE *systembuffer,
                               LPDIRECTDRAWSURFACE *temporarybuffer,
                               LPDIRECTDRAWPALETTE *palette,
                               HPALETTE            *gdipalette) {
  if (directdraw)
    *directdraw = s_lpdd;
  if (frontbuffer)
    *frontbuffer = s_surface[SDRAW_SURFACE_FRONT];
  if (backbuffer)
    *backbuffer = s_surface[SDRAW_SURFACE_BACK];
  if (systembuffer)
    *systembuffer = s_surface[SDRAW_SURFACE_SYSTEM];
  if (palette)
    *palette = s_palette;
  if (gdipalette)
    *gdipalette = s_gdipalette;
  return (s_lpdd != NULL);
}

//===========================================================================
BOOL APIENTRY SDrawGetScreenSize (int *width,
                                  int *height,
                                  int *bitdepth) {
  if (width)
    *width  = s_lpdd ? s_screensize.cx : GetSystemMetrics(SM_CXSCREEN);
  if (height)
    *height = s_lpdd ? s_screensize.cy : GetSystemMetrics(SM_CYSCREEN);
  if (bitdepth)
    if (s_lpdd)
      *bitdepth = s_screenbitdepth;
    else {
      HDC dc = GetDC(GetDesktopWindow());
      *bitdepth = GetDeviceCaps(dc,BITSPIXEL)*GetDeviceCaps(dc,PLANES);
      ReleaseDC(GetDesktopWindow(),dc);
    }
  return (width || height || bitdepth);
}

//===========================================================================
BOOL APIENTRY SDrawGetServiceLevel (int *servicelevel) {
  VALIDATEBEGIN;
  VALIDATE(servicelevel);
  VALIDATEEND;

  if (!s_surface[SDRAW_SURFACE_FRONT])
    return FALSE;
  if (s_surface[SDRAW_SURFACE_SYSTEM])
    *servicelevel = SDRAW_SERVICE_DOUBLEBUFFER;
  else if (s_surface[SDRAW_SURFACE_BACK])
    *servicelevel = SDRAW_SERVICE_PAGEFLIP;
  else 
    *servicelevel = SDRAW_SERVICE_BASIC;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SDrawLockSurface (int      surfacenumber,
                                LPCRECT  rect,
                                LPBYTE  *ptr,
                                int     *pitch,
                                DWORD    flags) {
  if (ptr)
    *ptr = NULL;
  if (pitch)
    *pitch = 0;

  VALIDATEBEGIN;
  VALIDATE(surfacenumber >= 0);
  VALIDATE(surfacenumber < SURFACES);
  VALIDATE(ptr);
  VALIDATEEND;

  if ((!s_lpdd) ||
      (!s_surface[surfacenumber]))
    return FALSE;

  // LOCK THE SURFACE AND RETURN A POINTER TO VIDEO MEMORY
  {
    DDSURFACEDESC desc;
    ZeroMemory(&desc,sizeof(DDSURFACEDESC));
    desc.dwSize = sizeof(DDSURFACEDESC);
    do {
      HRESULT result = s_surface[surfacenumber]->Lock((LPRECT)rect,
                                                      &desc,
                                                      DDLOCK_WAIT,
                                                      NULL);
      if ((result == DDERR_SURFACELOST) && s_surface[SDRAW_SURFACE_FRONT])
        if (s_surface[SDRAW_SURFACE_FRONT]->Restore() == DD_OK)
          InvalidateRect(s_framewindow,NULL,0);
        else
          return FALSE;
      else if ((result != DD_OK) &&
               (result != DDERR_WASSTILLDRAWING) &&
               !desc.lpSurface) {

        // IF WE COULDN'T LOCK THE PRIMARY SURFACE BUT WE HAVE BEEN GIVEN
        // A TEMPORARY SURFACE, REDIRECT OUTPUT TO THE TEMPORARY SURFACE
        if ((surfacenumber == SDRAW_SURFACE_FRONT) &&
            s_surface[SDRAW_SURFACE_TEMPORARY] &&
            !s_redirectingprimary) {
          s_redirectingprimary = TRUE;
          if (rect)
            CopyMemory(&s_redirectingrect,rect,sizeof(RECT));
          else {
            s_redirectingrect.left   = 0;
            s_redirectingrect.top    = 0;
            s_redirectingrect.right  = s_screensize.cx;
            s_redirectingrect.bottom = s_screensize.cy;
          }
          return SDrawLockSurface(SDRAW_SURFACE_TEMPORARY,rect,ptr,pitch,flags);
        }

        return FALSE;
      }
    } while (!desc.lpSurface);
    *ptr = (LPBYTE)desc.lpSurface;
    if (pitch)
      *pitch = desc.lPitch;
  }

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SDrawManualInitialize (HWND                framewindow,
                                     LPDIRECTDRAW        directdraw,
                                     LPDIRECTDRAWSURFACE frontbuffer,
                                     LPDIRECTDRAWSURFACE backbuffer,
                                     LPDIRECTDRAWSURFACE systembuffer,
                                     LPDIRECTDRAWSURFACE temporarybuffer,
                                     LPDIRECTDRAWPALETTE palette,
                                     HPALETTE            gdipalette) {

  // SAVE THE OBJECT HANDLES AND POINTERS
  s_autoinit                         = 0;
  s_dlgactive                        = 0;
  s_framewindow                      = framewindow;
  s_gdipalette                       = gdipalette;
  s_lpdd                             = directdraw;
  s_surface[SDRAW_SURFACE_FRONT]     = frontbuffer;
  s_surface[SDRAW_SURFACE_BACK]      = backbuffer;
  s_surface[SDRAW_SURFACE_SYSTEM]    = systembuffer;
  s_surface[SDRAW_SURFACE_TEMPORARY] = temporarybuffer;
  s_palette                          = palette;

  // IF THE APPLICATION PASSED US A DIRECTDRAW PALETTE BUT NOT A GDI PALETTE,
  // CREATE ONE OURSELF FOR USE BY SDLG
  if (s_palette && !s_gdipalette)
    CreateGdiPalette();

  return TRUE;
}

//===========================================================================
int APIENTRY SDrawMessageBox (LPCTSTR text,
                              LPCTSTR title,
                              UINT    flags) {

  // SWITCH TO THE GDI SCREEN IF NECESSARY
  SDrawSelectGdiSurface(TRUE,TRUE);

  // IF THE APPLICATION IS CONTROLLING THE SYSTEM PALETTE DIRECTLY
  // THROUGH DIRECTDRAW, NORMALIZE THE PALETTE SO THAT THE MESSAGEBOX()
  // FUNCTION CAN DISPLAY ITS TEXT
  PALETTEENTRY pe[256];
  if (s_palette) {

    // SAVE THE APPLICATION'S PALETTE
    s_palette->GetEntries(0,0,256,&pe[0]);

    // MODIFY THE IMPORTANT SYSTEM COLORS TO USE ONLY BLACK AND WHITE
    {
      HRSRC   resource = FindResource(StormGetInstance(),"SYSTEMPALETTE","#256");
      HGLOBAL handle   = LoadResource(StormGetInstance(),resource);
      DWORD   bytes    = SizeofResource(StormGetInstance(),resource);
      LPBYTE  ptr      = (LPBYTE)LockResource(handle);
      if (bytes && ptr) {
        PALETTEENTRY modpe[256];
        SBmpDecodeImage(SBMP_IMAGETYPE_PCX,
                        ptr,
                        bytes,
                        &modpe[0],
                        NULL,
                        0);
        s_palette->SetEntries(0,0,256,&modpe[0]);
      }
      FreeResource(handle);
    }

  }

  // DISPLAY THE MESSAGE BOX
  s_dlgactive = TRUE;
  int result = MessageBox(GetActiveWindow(),
                          text,
                          title,
                          flags);
  s_dlgactive = FALSE;

  // RESTORE THE ORIGINAL PALETTE
  if (s_palette)
    s_palette->SetEntries(0,0,256,&pe[0]);

  return result;
}

//===========================================================================
BOOL APIENTRY SDrawPostClose () {
  if (!s_framewindow)
    return FALSE;

  PostMessage(s_framewindow,WM_CLOSE,0,0);
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SDrawRealizePalette () {
  if ((!s_lpdd) || (!s_palette))
    return FALSE;

  // UPDATE THE DIRECTDRAW PALETTE
  BOOL success = (s_palette->SetEntries(0,0,256,&s_paletteentries[0]) == DD_OK);

  // UPDATE THE GDI PALETTE
  if (s_gdipalette)
    SetPaletteEntries(s_gdipalette,0,256,&s_paletteentries[0]);

  // IF WE ARE RUNNING IN A WINDOW, REALIZE THE PALETTE THROUGH GDI
  if (s_gdipalette && s_framewindow &&
      IsRunningInWindow(s_framewindow)) {
    HDC dc = GetDC(s_framewindow);
    SelectPalette(dc,s_gdipalette,0);
    RealizePalette(dc);
    ReleaseDC(s_framewindow,dc);
  }

  return success;
}

//===========================================================================
BOOL APIENTRY SDrawSelectGdiSurface (BOOL select, BOOL copy) {
  if (s_dlgactive ||
      !(s_lpdd &&
        s_surface[SDRAW_SURFACE_FRONT] &&
        s_surface[SDRAW_SURFACE_BACK]))
    return FALSE;

  // IF THE GDI SURFACE IS NOT ALREADY IN THE REQUESTED POSITION, DO A
  // PAGE FLIP
  LPDIRECTDRAWSURFACE gdisurface = NULL;
  s_lpdd->GetGDISurface(&gdisurface);
  if ((gdisurface != s_surface[SDRAW_SURFACE_FRONT]) == (select != 0)) {
    if (copy)
      s_surface[SDRAW_SURFACE_BACK]->Blt(NULL,
                                             s_surface[SDRAW_SURFACE_FRONT],
                                             NULL,
                                             0,
                                             NULL);
    return SDrawFlipPage();
  }
  else
    return TRUE;
}

//===========================================================================
BOOL APIENTRY SDrawUnlockSurface (int     surfacenumber,
                                  LPBYTE  ptr,
                                  DWORD   numrects,
                                  LPCRECT rectarray) {
  VALIDATEBEGIN;
  VALIDATE(surfacenumber >= 0);
  VALIDATE(surfacenumber < SURFACES);
  VALIDATEEND;

  if ((!s_lpdd) ||
      (!s_surface[surfacenumber]))
    return FALSE;

  // PROCESS REDIRECTED CALLS TO LOCK THE PRIMARY SURFACE
  if ((surfacenumber == SDRAW_SURFACE_FRONT) &&
      s_redirectingprimary)
    surfacenumber = SDRAW_SURFACE_TEMPORARY;

  // UNLOCK THE SURFACE
  if (s_surface[surfacenumber]->Unlock(ptr) != DD_OK)
    return FALSE;

  // IF WE JUST UNLOCKED THE TEMPORARY SURFACE, AND WE WERE REDIRECTING
  // OUTPUT TO THE TEMPORARY SURFACE BECAUSE WE WERE UNABLE TO LOCK THE
  // PRIMARY SURFACE, THEN COPY THE MODIFIED CONTENTS OF THE TEMPORARY
  // SURFACE INTO THE PRIMARY SURFACE
  if ((surfacenumber == SDRAW_SURFACE_TEMPORARY) &&
      s_redirectingprimary) {
    s_redirectingprimary = FALSE;
    if (s_surface[SDRAW_SURFACE_FRONT])
      if (numrects)
        for (DWORD loop = 0; loop < numrects; ++loop)
          s_surface[SDRAW_SURFACE_FRONT]->Blt((LPRECT)&rectarray[loop],
                                              s_surface[SDRAW_SURFACE_TEMPORARY],
                                              (LPRECT)&rectarray[loop],
                                              DDBLT_WAIT,
                                              NULL);
      else
        s_surface[SDRAW_SURFACE_FRONT]->Blt(&s_redirectingrect,
                                            s_surface[SDRAW_SURFACE_TEMPORARY],
                                            &s_redirectingrect,
                                            DDBLT_WAIT,
                                            NULL);
  }

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SDrawUpdatePalette (DWORD          firstentry,
                                  DWORD          numentries,
                                  LPPALETTEENTRY entries,
                                  BOOL           reservedentries) {
  VALIDATEBEGIN;
  VALIDATE(firstentry+numentries <= 256);
  VALIDATE(entries);
  VALIDATEEND;

  if ((!s_lpdd) || (!s_palette))
    return FALSE;

  // PREVENT THE APPLICATION FROM SETTING THE FIRST OR LAST PALETTE
  // ENTRIES (BLACK AND WHITE)
  if ((!firstentry) && !reservedentries) {
    ++firstentry;
    --numentries;
    ++entries;
  }
  if ((firstentry+numentries == 256) && !reservedentries)
    --numentries;

  // SAVE THE NEW PALETTE ENTRIES
  CopyMemory(&s_paletteentries[firstentry],entries,numentries*sizeof(PALETTEENTRY));

  // UPDATE THE DIRECTDRAW PALETTE
  BOOL success = (s_palette->SetEntries(0,firstentry,numentries,entries) == DD_OK);

  // UPDATE THE GDI PALETTE
  if (s_gdipalette) {
    for (DWORD loop = 0; loop < numentries; ++loop)
      (entries+loop)->peFlags = (((firstentry+loop) >= 10) && ((firstentry+loop) <= 245))
                                  ? PC_RESERVED | PC_NOCOLLAPSE
                                  : 0;
    SetPaletteEntries(s_gdipalette,firstentry,numentries,entries);
  }

  // IF WE ARE RUNNING IN A WINDOW, REALIZE THE PALETTE THROUGH GDI
  if (s_gdipalette && s_framewindow &&
      IsRunningInWindow(s_framewindow)) {
    HDC dc = GetDC(s_framewindow);
    SelectPalette(dc,s_gdipalette,0);
    RealizePalette(dc);
    ReleaseDC(s_framewindow,dc);
  }

  return success;
}

//===========================================================================
BOOL APIENTRY SDrawUpdateScreen (LPCRECT rect) {
  if (!(s_lpdd &&
        s_surface[SDRAW_SURFACE_FRONT] &&
        s_surface[SDRAW_SURFACE_SYSTEM]))
    return FALSE;

  // IF THE HARDWARE IS CAPABLE OF DOING ASYNCHRONOUS BLTS FROM SYSTEM
  // MEMORY, USE THE HARDWARE BLITTER
  if (s_asyncsystemblt) {
    DDBLTFX fx;
    ZeroMemory(&fx,sizeof(DDBLTFX));
    fx.dwSize = sizeof(DDBLTFX);
    fx.dwDDFX = DDBLTFX_NOTEARING;
    return (s_surface[SDRAW_SURFACE_FRONT]->Blt((LPRECT)rect,
                                                s_surface[SDRAW_SURFACE_SYSTEM],
                                                (LPRECT)rect,
                                                DDBLT_ASYNC | DDBLT_DDFX,
                                                &fx) == DD_OK);
  }

  // OTHERWISE, USE OUR OWN BLT FUNCTION, WHICH IS FASTER THAT DIRECTDRAW'S
  else {
    BOOL   success = FALSE;
    LPBYTE videobuffer;
    int    videopitch;
    if (SDrawLockSurface(SDRAW_SURFACE_FRONT,NULL,&videobuffer,&videopitch)) {
      LPBYTE systembuffer;
      int    systempitch;
      if (SDrawLockSurface(SDRAW_SURFACE_SYSTEM,NULL,&systembuffer,&systempitch)) {
        success = SBltROP3(videobuffer,
                           systembuffer,
                           (s_screenbitdepth == 8)
                             ? s_screensize.cx
                             : (s_screensize.cx*s_screenbitdepth) >> 3,
                           s_screensize.cy,
                           videopitch,
                           systempitch,
                           0,
                           SRCCOPY);
        SDrawUnlockSurface(SDRAW_SURFACE_SYSTEM,systembuffer);
      }
      SDrawUnlockSurface(SDRAW_SURFACE_FRONT,videobuffer);
    }
    return success;
  }

}

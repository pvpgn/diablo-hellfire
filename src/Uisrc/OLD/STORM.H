#ifndef  _STORM_H_
#define  _STORM_H_

/****************************************************************************
*
*  BitBlt functions
*
***/

extern "C" BOOL APIENTRY SBltDestroy ();
extern "C" BOOL APIENTRY SBltGetSCode (DWORD rop3,
                                       LPSTR buffer,
                                       DWORD buffersize,
                                       BOOL  optimize = 1);
extern "C" BOOL APIENTRY SBltROP3 (LPBYTE dest,
                                   LPBYTE source,
                                   int    width,
                                   int    height,
                                   int    destcx,
                                   int    sourcecx,
                                   DWORD  pattern,
                                   DWORD  rop3);
extern "C" BOOL APIENTRY SBltROP3Clipped (LPBYTE dest,
                                          LPRECT destrect,
                                          LPSIZE destsize,
                                          int    destpitch,
                                          LPBYTE source,
                                          LPRECT sourcerect,
                                          LPSIZE sourcesize,
                                          int    sourcepitch,
                                          DWORD  pattern,
                                          DWORD  rop3);
extern "C" BOOL APIENTRY SBltROP3Tiled (LPBYTE dest,
                                        LPRECT destrect,
                                        int    destpitch,
                                        LPBYTE source,
                                        LPRECT sourcerect,
                                        int    sourcepitch,
                                        int    sourceoffsetx,
                                        int    sourceoffsety,
                                        DWORD  pattern,
                                        DWORD  rop3);

/****************************************************************************
*
*  Bitmap functions
*
***/

#define  SBMP_IMAGETYPE_BMP  1
#define  SBMP_IMAGETYPE_PCX  2

extern "C" BOOL APIENTRY SBmpDecodeImage (DWORD           imagetype,
                                          LPBYTE          imagedata,
                                          DWORD           imagebytes,
                                          LPPALETTEENTRY  paletteentries,
                                          LPBYTE          bitmapbits,
                                          DWORD           buffersize,
                                          int            *width,
                                          int            *height,
                                          int            *bitdepth);
extern "C" BOOL APIENTRY SBmpDestroy ();
extern "C" BOOL APIENTRY SBmpLoadImage (LPCTSTR         filename,
                                        LPPALETTEENTRY  paletteentries,
                                        LPBYTE          bitmapbits,
                                        DWORD           buffersize,
                                        int            *width    = NULL,
                                        int            *height   = NULL,
                                        int            *bitdepth = NULL);
extern "C" BOOL APIENTRY SBmpSaveImage (LPCTSTR         filename,
                                        LPPALETTEENTRY  paletteentries,
                                        LPBYTE          bitmapbits,
                                        int             width,
                                        int             height,
                                        int             bitdepth);

/****************************************************************************
*
*  S-Code functions
*
***/

#define  SCODE_CF_AUTOALIGNDWORD  0x00040000
#define  SCODE_CF_USESALTADJUSTS  0x04000000

typedef struct _HSCODESTREAM { int unused; } *HSCODESTREAM;

typedef struct _SCODEEXECUTEDATA {
    DWORD  size;
    DWORD  flags;
    int    xiterations;
    int    yiterations;
    int    adjustdest;
    int    adjustsource;
    LPVOID dest;
    LPVOID source;
    LPVOID table;
    DWORD  a;
    DWORD  b;
    DWORD  c;
    int    adjustdestalt;
    int    adjustsourcealt;
    DWORD  reserved[2];
} SCODEEXECUTEDATA, *SCODEEXECUTEDATAPTR;

extern "C" BOOL APIENTRY SCodeCompile (LPCSTR        prologstring,
                                       LPCSTR        loopstring,
                                       LPCSTR       *firsterror,
                                       DWORD         maxiterations,
                                       DWORD         flags,
                                       HSCODESTREAM *handle);
extern "C" BOOL APIENTRY SCodeDelete (HSCODESTREAM handle);
extern "C" BOOL APIENTRY SCodeDestroy ();
extern "C" BOOL APIENTRY SCodeExecute (HSCODESTREAM        handle,
                                       SCODEEXECUTEDATAPTR executedata);
extern "C" BOOL APIENTRY SCodeGetJumpTable (HSCODESTREAM   handle,
                                            LPBYTE       **jumptableptr,
                                            LPDWORD       *prologpatchlocation,
                                            LPDWORD       *looppatchlocation,
                                            LPDWORD       *epilogpatchlocation);
extern "C" BOOL APIENTRY SCodeGetPseudocode (LPCSTR scodestring,
                                             LPSTR  buffer,
                                             DWORD  buffersize);

/****************************************************************************
*
*  Dialog box functions
*
***/

#define  SDLG_ADJUST_NONE               0
#define  SDLG_ADJUST_VERTICAL           1
#define  SDLG_ADJUST_CONTROLPOS         2

#define  SDLG_DBF_TILE                  0x00000001
#define  SDLG_DBF_VCENTER               0x00000002

#define  SDLG_STYLE_ANY                 0xFFFFFFFF
#define  SDLG_STYLE_ANYPUSHBUTTON       0x00010001

#define  SDLG_USAGE_BACKGROUND          0x00000001
#define  SDLG_USAGE_NORMAL_UNFOCUSED    0x00000010
#define  SDLG_USAGE_NORMAL_FOCUSED      0x00000020
#define  SDLG_USAGE_NORMAL              (SDLG_USAGE_NORMAL_UNFOCUSED | SDLG_USAGE_NORMAL_FOCUSED)
#define  SDLG_USAGE_SELECTED_UNFOCUSED  0x00000040
#define  SDLG_USAGE_SELECTED_FOCUSED    0x00000080
#define  SDLG_USAGE_SELECTED            (SDLG_USAGE_SELECTED_UNFOCUSED | SDLG_USAGE_SELECTED_FOCUSED)
#define  SDLG_USAGE_NORMAL_GRAYED       0x00000100
#define  SDLG_USAGE_SELECTED_GRAYED     0x00000400
#define  SDLG_USAGE_GRAYED              (SDLG_USAGE_NORMAL_GRAYED | SDLG_USAGE_SELECTED_GRAYED)
#define  SDLG_USAGE_CURSORMASK          0x00001000
#define  SDLG_USAGE_CURSORIMAGE         0x00002000

extern "C" HDC  APIENTRY SDlgBeginPaint (HWND window, LPPAINTSTRUCT ps);
extern "C" BOOL APIENTRY SDlgBltToWindow (HWND   window,
                                          HRGN   region,
                                          int    x,
                                          int    y,
                                          LPBYTE bitmapbits,
                                          LPRECT bitmaprect,
                                          LPSIZE bitmapsize,
                                          DWORD  colorkey = 0xFFFFFFFF,
                                          DWORD  pattern  = 0,
                                          DWORD  rop3     = SRCCOPY);
extern "C" BOOL APIENTRY SDlgCheckTimers ();
extern "C" HWND APIENTRY SDlgCreateDialogIndirectParam (HINSTANCE      instance,
                                                        LPCDLGTEMPLATE templatedata,
                                                        HWND           parentwindow,
                                                        DLGPROC        dialogproc,
                                                        LPARAM         initparam);
extern "C" HWND APIENTRY SDlgCreateDialogParam (HINSTANCE instance,
                                                LPCTSTR   templatename,
                                                HWND      parentwindow,
                                                DLGPROC   dialogproc,
                                                LPARAM    initparam);
extern "C" BOOL APIENTRY SDlgDefDialogProc (HWND   window,
                                            UINT   message,
                                            WPARAM wparam,
                                            LPARAM lparam);
extern "C" BOOL APIENTRY SDlgDestroy ();
extern "C" int  APIENTRY SDlgDialogBoxIndirectParam (HINSTANCE      instance,
                                                     LPCDLGTEMPLATE templatedata,
                                                     HWND           parentwindow,
                                                     DLGPROC        dialogproc,
                                                     LPARAM         initparam);
extern "C" int  APIENTRY SDlgDialogBoxParam (HINSTANCE instance,
                                             LPCTSTR   templatename,
                                             HWND      parentwindow,
                                             DLGPROC   dialogproc,
                                             LPARAM    initparam);
extern "C" BOOL APIENTRY SDlgDrawBitmap (HWND   window,
                                         DWORD  usage,
                                         HRGN   region,
                                         int    offsetx        = 0,
                                         int    offsety        = 0,
                                         LPRECT boundingoffset = NULL,
                                         DWORD  flags          = 0);
extern "C" BOOL APIENTRY SDlgEndDialog (HWND window,
                                        int  result);
extern "C" BOOL APIENTRY SDlgEndPaint (HWND window, LPPAINTSTRUCT ps);
extern "C" BOOL APIENTRY SDlgKillTimer (HWND window,
                                        UINT event);
extern "C" BOOL APIENTRY SDlgSetBaseFont (int     pointsize,
                                          int     weight,
                                          DWORD   flags,
                                          DWORD   family,
                                          LPCTSTR face);
extern "C" BOOL APIENTRY SDlgSetBitmap (HWND     window,
                                        HWND     parentwindow,
                                        LPCTSTR  controltype,
                                        DWORD    controlstyle,
                                        DWORD    usage,
                                        LPBYTE   bitmapbits,
                                        LPRECT   rect,
                                        int      width,
                                        int      height,
                                        COLORREF colorkey = 0xFFFFFFFF);
extern "C" BOOL APIENTRY SDlgSetControlBitmaps (HWND     parentwindow,
                                                LPINT    controllist,
                                                LPDWORD  usagelist,
                                                LPBYTE   bitmapbits,
                                                LPSIZE   bitmapsize,
                                                DWORD    adjusttype,
                                                COLORREF colorkey = 0xFFFFFFFF);
extern "C" BOOL APIENTRY SDlgSetCursor (HWND     window,
                                        HCURSOR  cursor,
                                        DWORD    id,
                                        HCURSOR *oldcursor);
extern "C" BOOL APIENTRY SDlgSetSystemCursor (LPBYTE maskbitmap,
                                              LPBYTE imagebitmap,
                                              LPSIZE size,
                                              DWORD  id = 32512);
extern "C" BOOL APIENTRY SDlgSetTimer (HWND      window,
                                       UINT      event,
                                       UINT      elapse,
                                       TIMERPROC timerfunc);
extern "C" BOOL APIENTRY SDlgUpdateCursor ();

#define  SDlgCreateDialog(ins,tpl,wnd,prc)          SDlgCreateDialogParam(ins,tpl,wnd,prc,0)
#define  SDlgCreateDialogIndirect(ins,tpl,wnd,prc)  SDlgCreateDialogIndirectParam(ins,tpl,wnd,prc,0)
#define  SDlgDialogBox(ins,tpl,wnd,prc)             SDlgDialogBoxParam(ins,tpl,wnd,prc,0)
#define  SDlgDialogBoxIndirect(ins,tpl,wnd,prc)     SDlgDialogBoxIndirectParam(ins,tpl,wnd,prc,0)

/****************************************************************************
*
*  DirectDraw functions
*
***/

#define  SDRAW_SERVICE_BASIC         1
#define  SDRAW_SERVICE_PAGEFLIP      2
#define  SDRAW_SERVICE_DOUBLEBUFFER  3
#define  SDRAW_SERVICE_MAX           3

#define  SDRAW_SURFACE_FRONT         0
#define  SDRAW_SURFACE_BACK          1
#define  SDRAW_SURFACE_SYSTEM        2
#define  SDRAW_SURFACE_TEMPORARY     3

extern "C" BOOL APIENTRY SDrawAutoInitialize (HINSTANCE instance,
                                              LPCTSTR   classname,
                                              LPCTSTR   title,
                                              WNDPROC   wndproc      = NULL,
                                              int       servicelevel = SDRAW_SERVICE_BASIC,
                                              int       width        = 640,
                                              int       height       = 480,
                                              int       bitdepth     = 8);
extern "C" BOOL APIENTRY SDrawCaptureScreen (LPCTSTR filename = NULL);
extern "C" BOOL APIENTRY SDrawClearSurface (int surfacenumber);
extern "C" BOOL APIENTRY SDrawDestroy ();
extern "C" BOOL APIENTRY SDrawFlipPage ();
extern "C" HWND APIENTRY SDrawGetFrameWindow (HWND *window = NULL);
#ifdef __DDRAW_INCLUDED__
extern "C" BOOL APIENTRY SDrawGetObjects (LPDIRECTDRAW        *directdraw,
                                          LPDIRECTDRAWSURFACE *frontbuffer,
                                          LPDIRECTDRAWSURFACE *backbuffer,
                                          LPDIRECTDRAWSURFACE *systembuffer,
                                          LPDIRECTDRAWSURFACE *temporarybuffer,
                                          LPDIRECTDRAWPALETTE *palette,
                                          HPALETTE            *gdipalette);
#endif
extern "C" BOOL APIENTRY SDrawGetScreenSize (int *width,
                                             int *height,
                                             int *bitdepth = NULL);
extern "C" BOOL APIENTRY SDrawGetServiceLevel (int *servicelevel = NULL);
extern "C" BOOL APIENTRY SDrawLockSurface (int      surfacenumber,
                                           LPCRECT  rect,
                                           LPBYTE  *ptr,
                                           int     *pitch = NULL,
                                           DWORD    flags = 0);
#ifdef __DDRAW_INCLUDED__
extern "C" BOOL APIENTRY SDrawManualInitialize (HWND                framewindow,
                                                LPDIRECTDRAW        directdraw,
                                                LPDIRECTDRAWSURFACE frontbuffer,
                                                LPDIRECTDRAWSURFACE backbuffer,
                                                LPDIRECTDRAWSURFACE systembuffer,
                                                LPDIRECTDRAWSURFACE temporarybuffer,
                                                LPDIRECTDRAWPALETTE palette,
                                                HPALETTE            gdipalette);
#endif
extern "C" BOOL APIENTRY SDrawMessageBox (LPCTSTR text,
                                          LPCTSTR title,
                                          UINT    flags);
extern "C" BOOL APIENTRY SDrawPostClose ();
extern "C" BOOL APIENTRY SDrawRealizePalette ();
extern "C" BOOL APIENTRY SDrawSelectGdiSurface (BOOL select, BOOL copy);
extern "C" BOOL APIENTRY SDrawUnlockSurface (int     surfacenumber,
                                             LPBYTE  ptr,
                                             DWORD   numrects  = 0,
                                             LPCRECT rectarray = NULL);
extern "C" BOOL APIENTRY SDrawUpdatePalette (DWORD          firstentry,
                                             DWORD          numentries,
                                             LPPALETTEENTRY entries,
                                             BOOL           reservedentries = 0);
extern "C" BOOL APIENTRY SDrawUpdateScreen (LPCRECT rect);

/****************************************************************************
*
*  Event dispatching functions
*
***/

typedef void (CALLBACK *SEVTHANDLER)(LPVOID);

extern "C" BOOL APIENTRY SEvtDestroy ();
extern "C" BOOL APIENTRY SEvtDispatch (DWORD  type,
                                       DWORD  subtype,
                                       DWORD  id,
                                       LPVOID data);
extern "C" BOOL APIENTRY SEvtRegisterHandler (DWORD       type,
                                              DWORD       subtype,
                                              DWORD       id,
                                              DWORD       flags,
                                              SEVTHANDLER handler);
extern "C" BOOL APIENTRY SEvtUnregisterHandler (DWORD       type,
                                                DWORD       subtype,
                                                DWORD       id,
                                                SEVTHANDLER handler);
extern "C" BOOL APIENTRY SEvtUnregisterType (DWORD type,
                                             DWORD subtype);

/****************************************************************************
*
*  File I/O functions
*
***/

#define  SFILE_AUTH_UNABLETOAUTHENTICATE   0
#define  SFILE_AUTH_NOSIGNATURE            1
#define  SFILE_AUTH_BADSIGNATURE           2
#define  SFILE_AUTH_UNKNOWNSIGNATURE       3
#define  SFILE_AUTH_FIRSTAUTHENTIC         5
#define  SFILE_AUTH_AUTHENTICBLIZZARD      5

#define  SFILE_DDA_LOOP                    0x00040000

#define  SFILE_ERROR_BASE                  0x21030000
#define  SFILE_ERROR_BAD_FORMAT            ERROR_BAD_FORMAT
#define  SFILE_ERROR_BAD_PATHNAME          ERROR_BAD_PATHNAME
#define  SFILE_ERROR_CALL_NOT_IMPLEMENTED  ERROR_CALL_NOT_IMPLEMENTED
#define  SFILE_ERROR_FILE_INVALID          ERROR_FILE_INVALID
#define  SFILE_ERROR_FILE_NOT_FOUND        ERROR_FILE_NOT_FOUND
#define  SFILE_ERROR_HANDLE_EOF            ERROR_HANDLE_EOF
#define  SFILE_ERROR_INVALID_DATA          ERROR_INVALID_DATA
#define  SFILE_ERROR_INVALID_DRIVE         ERROR_INVALID_DRIVE
#define  SFILE_ERROR_INVALID_HANDLE        ERROR_INVALID_HANDLE
#define  SFILE_ERROR_INVALID_PARAMETER     ERROR_INVALID_PARAMETER
#define  SFILE_ERROR_NOT_ARCHIVE           (SFILE_ERROR_BASE+1)
#define  SFILE_ERROR_NOT_AUTHENTICATED     ERROR_NOT_AUTHENTICATED
#define  SFILE_ERROR_NOT_ENOUGH_MEMORY     ERROR_NOT_ENOUGH_MEMORY
#define  SFILE_ERROR_NOT_IN_ARCHIVE        (SFILE_ERROR_BASE+2)
#define  SFILE_ERROR_NOT_PLAYING           (SFILE_ERROR_BASE+3)

#define  SFILE_FIND_FILES                  0x00000001
#define  SFILE_FIND_DIRECTORIES            0x00000002

typedef struct _HSARCHIVE  { int unused; } *HSARCHIVE;
typedef struct _HSFILE     { int unused; } *HSFILE;
typedef struct _HSFINDFILE { int unused; } *HSFINDFILE;

extern "C" BOOL  APIENTRY SFileAuthenticateArchive (HSARCHIVE  archive,
                                                    DWORD     *extendedresult);
extern "C" BOOL  APIENTRY SFileCloseArchive (HSARCHIVE handle);
extern "C" BOOL  APIENTRY SFileCloseFile (HSFILE handle);
extern "C" BOOL  APIENTRY SFileDdaBegin (HSFILE handle,
                                         DWORD  buffersize,
                                         DWORD  flags);
extern "C" BOOL  APIENTRY SFileDdaBeginEx (HSFILE handle,
                                           DWORD  buffersize,
                                           DWORD  flags,
                                           DWORD  offset,
                                           LONG   volume,
                                           LONG   pan,
                                           LPVOID reserved);
extern "C" BOOL  APIENTRY SFileDdaDestroy ();
extern "C" BOOL  APIENTRY SFileDdaEnd (HSFILE handle);
extern "C" BOOL  APIENTRY SFileDdaGetPos (HSFILE  handle,
                                          DWORD  *position,
                                          DWORD  *maxposition);
extern "C" BOOL  APIENTRY SFileDdaGetVolume (HSFILE  handle,
                                             LONG   *volume,
                                             LONG   *pan);
#ifdef __DSOUND_INCLUDED__
extern "C" BOOL  APIENTRY SFileDdaInitialize (LPDIRECTSOUND directsound);
#endif
extern "C" BOOL  APIENTRY SFileDdaSetVolume (HSFILE handle,
                                             LONG   volume,
                                             LONG   pan);
extern "C" BOOL  APIENTRY SFileDestroy ();
extern "C" BOOL  APIENTRY SFileEnableDirectAccess (BOOL enable);
extern "C" BOOL  APIENTRY SFileGetFileArchive (HSFILE     file,
                                               HSARCHIVE *archive);
extern "C" DWORD APIENTRY SFileGetFileSize (HSFILE  handle,
                                            LPDWORD filesizehigh = NULL);
extern "C" BOOL  APIENTRY SFileOpenArchive (LPCTSTR    archivename,
                                            int        priority,
                                            BOOL       cdonly,
                                            HSARCHIVE *handle);
extern "C" BOOL  APIENTRY SFileOpenFile (LPCTSTR  filename,
                                         HSFILE  *handle);
extern "C" BOOL  APIENTRY SFileOpenFileEx (HSARCHIVE  archivehandle,
                                           LPCTSTR    filename,
                                           DWORD      flags,
                                           HSFILE    *handle);
extern "C" BOOL  APIENTRY SFileOpenFileWin32 (LPCTSTR  filename,
                                              HANDLE  *handle);
extern "C" BOOL  APIENTRY SFileReadFile (HSFILE  handle,
                                         LPVOID  buffer,
                                         DWORD   bytestoread,
                                         LPDWORD bytesread = NULL,
                                         LPVOID  reserved  = NULL);
extern "C" BOOL  APIENTRY SFileSetBasePath (LPCTSTR path);
extern "C" DWORD APIENTRY SFileSetFilePointer (HSFILE handle,
                                               LONG   distancetomove,
                                               PLONG  distancetomovehigh,
                                               DWORD  movemethod);
extern "C" BOOL  APIENTRY SFileSetLocale (LCID lcid);

/****************************************************************************
*
*  GDI functions
*
***/

#define  ETO_TEXT_TRANSPARENT  0
#define  ETO_TEXT_COLOR        1
#define  ETO_TEXT_BLACK        2
#define  ETO_TEXT_WHITE        3
#define  ETO_BKG_TRANSPARENT   0
#define  ETO_BKG_COLOR         1
#define  ETO_BKG_BLACK         2
#define  ETO_BKG_WHITE         3

typedef struct _HSGDIOBJ                     { int unused; } *HSGDIOBJ;
typedef struct _HSGDIFONT : public _HSGDIOBJ { int unused; } *HSGDIFONT;

extern "C" BOOL APIENTRY SGdiBitBlt (LPBYTE   videobuffer,
                                     int      destx,
                                     int      desty,
                                     LPBYTE   sourcedata,
                                     LPRECT   sourcerect,
                                     int      sourcecx,
                                     int      sourcecy,
                                     COLORREF color = 0,
                                     DWORD    rop   = SRCCOPY);
extern "C" BOOL APIENTRY SGdiCreateFont (LPBYTE     bits,
                                         int        width,
                                         int        height,
                                         int        bitdepth,
                                         int        filecharwidth,
                                         int        filecharheight,
                                         LPSIZE     charsizetable,
                                         HSGDIFONT *handle);
extern "C" BOOL APIENTRY SGdiDeleteObject (HSGDIOBJ handle);
extern "C" BOOL APIENTRY SGdiDestroy ();
extern "C" BOOL APIENTRY SGdiExtTextOut (LPBYTE   videobuffer,
                                         int      x,
                                         int      y,
                                         LPRECT   rect,
                                         COLORREF color,
                                         int      textcoloruse,
                                         int      bkgcoloruse,
                                         LPCTSTR  string,
                                         int      chars = -1);
extern "C" BOOL APIENTRY SGdiImportFont (HFONT      windowsfont,
                                         HSGDIFONT *handle);
extern "C" BOOL APIENTRY SGdiLoadFont (LPCTSTR    filename,
                                       int        filecharwidth,
                                       int        filecharheight,
                                       int        basecharwidth,
                                       LPSIZE     charsizetable,
                                       HSGDIFONT *handle);
extern "C" BOOL APIENTRY SGdiRectangle (LPBYTE   videobuffer,
                                        int      left,
                                        int      top,
                                        int      right,
                                        int      bottom,
                                        COLORREF color);
extern "C" BOOL APIENTRY SGdiSelectObject (HSGDIOBJ handle);
extern "C" BOOL APIENTRY SGdiSetPitch (int pitch);
extern "C" BOOL APIENTRY SGdiTextOut (LPBYTE   videobuffer,
                                      int      x,
                                      int      y,
                                      COLORREF color,
                                      LPCTSTR  string,
                                      int      chars = -1);

/****************************************************************************
*
*  Memory allocation functions
*
***/

#define  SMEM_FLAG_ZEROMEMORY  0x00000008

extern "C" LPVOID APIENTRY SMemAlloc (DWORD  bytes,
                                      LPCSTR filename   = NULL,
                                      int    linenumber = 0,
                                      DWORD  flags      = 0);
extern "C" BOOL   APIENTRY SMemDestroy ();
extern "C" BOOL   APIENTRY SMemFree (LPVOID ptr,
                                     LPCSTR filename   = NULL,
                                     int    linenumber = 0,
                                     DWORD  flags      = 0);
extern "C" void   APIENTRY SMemInitialize ();

#define  ALLOC(bytes)      SMemAlloc(bytes,__FILE__,__LINE__,0)
#define  ALLOCZERO(bytes)  SMemAlloc(bytes,__FILE__,__LINE__,SMEM_FLAG_ZEROMEMORY)
#define  FREE(ptr)         SMemFree(ptr,__FILE__,__LINE__,0)
#define  NEW(struct)       (struct *)SMemAlloc(sizeof(struct),__FILE__,__LINE__,0)
#define  NEWZERO(struct)   (struct *)SMemAlloc(sizeof(struct),__FILE__,__LINE__,SMEM_FLAG_ZEROMEMORY)

/****************************************************************************
*
*  Message functions
*
***/

typedef struct _PARAMS {
    HWND    window;
    UINT    message;
    WPARAM  wparam;
    LPARAM  lparam;
    UINT    notifycode;
    LPVOID  extra;
    BOOL    useresult;
    LRESULT result;
} PARAMS, *PARAMSPTR, *LPPARAMS;

typedef BOOL (CALLBACK *SMSGIDLEPROC)(DWORD);
typedef void (CALLBACK *SMSGHANDLER)(LPPARAMS);

extern "C" BOOL APIENTRY SMsgDestroy ();
extern "C" BOOL APIENTRY SMsgDispatchMessage (HWND     window,
                                              UINT     message,
                                              WPARAM   wparam,
                                              LPARAM   lparam,
                                              BOOL    *useresult,
                                              LRESULT *result);
extern "C" BOOL APIENTRY SMsgDoMessageLoop (SMSGIDLEPROC idleproc      = NULL,
                                            BOOL         cleanuponquit = 1);
extern "C" BOOL APIENTRY SMsgRegisterCommand (HWND        window,
                                              UINT        id,
                                              SMSGHANDLER handler);
extern "C" BOOL APIENTRY SMsgRegisterKeyDown (HWND        window,
                                              UINT        id,
                                              SMSGHANDLER handler);
extern "C" BOOL APIENTRY SMsgRegisterKeyUp (HWND        window,
                                            UINT        id,
                                            SMSGHANDLER handler);
extern "C" BOOL APIENTRY SMsgRegisterMessage (HWND        window,
                                              UINT        id,
                                              SMSGHANDLER handler);

/****************************************************************************
*
*  Networking functions
*
***/

#define  SNET_ART_BACKGROUND              0
#define  SNET_ART_BUTTONTEXTURE           1
#define  SNET_ART_JOINBACKGROUND          2
#define  SNET_ART_HELPBACKGROUND          3
#define  SNET_ART_POPUPBACKGROUND         4
#define  SNET_ART_BUTTON_XSML             5
#define  SNET_ART_BUTTON_SML              6
#define  SNET_ART_BUTTON_MED              7
#define  SNET_ART_BUTTON_LRG              8
#define  SNET_ART_APP_LOGO_SML            9
#define  SNET_ART_PROGRESS_BACKGROUND     10
#define  SNET_ART_PROGRESS_FILLER         11
#define  SNET_ART_POPUPBACKGROUND_SML     12
#define  SNET_ART_SCROLLBARARROWS         13
#define  SNET_ART_SCROLLTHUMB             14
#define  SNET_ART_SCROLLBAR               15

#define  SNET_AUTHTYPE_CHANNEL            1
#define  SNET_AUTHTYPE_GAME               2

#define  SNET_BROADCASTNONLOCALPLAYERID   0xFFFFFFFE
#define  SNET_BROADCASTPLAYERID           0xFFFFFFFF
#define  SNET_INVALIDPLAYERID             0xFFFFFFFF

#define  SNET_CAPS_PAGELOCKEDBUFFERS      0x00000001
#define  SNET_CAPS_BASICINTERFACE         0x00000002
      
#define  SNET_CF_ALLOWPRIVATEGAMES        0x00000001

#define  SNET_DATA_SYSCOLORS              1
#define  SNET_DATA_CURSORLINK             2
#define  SNET_DATA_CURSORARROW            3
#define  SNET_DATA_CURSORIBEAM            4

#define  SNET_DDF_INCLUDENAME             0x00000001
#define  SNET_DDF_MULTILINE               0x00000002

#define  SNET_DDPF_BLIZZARD               0x00000001
#define  SNET_DDPF_MODERATOR              0x00000002
#define  SNET_DDPF_SPEAKER                0x00000004
#define  SNET_DDPF_SYSOP                  0x00000008
#define  SNET_DDPF_SQUELCHED              0x00000020

#define  SNET_DRAWTYPE_GAME               1
#define  SNET_DRAWTYPE_PLAYER             2

#define  SNET_ERROR_BASE                  0x21050000
#define  SNET_ERROR_ALREADY_EXISTS        ERROR_ALREADY_EXISTS
#define  SNET_ERROR_BAD_PROVIDER          ERROR_BAD_PROVIDER
#define  SNET_ERROR_CANCELLED             ERROR_CANCELLED
#define  SNET_ERROR_INVALID_PARAMETER     ERROR_INVALID_PARAMETER
#define  SNET_ERROR_INVALID_PLAYER        (SNET_ERROR_BASE+1)
#define  SNET_ERROR_GAME_ALREADY_STARTED  (SNET_ERROR_BASE+2)
#define  SNET_ERROR_GAME_FULL             (SNET_ERROR_BASE+3)
#define  SNET_ERROR_GAME_NOT_FOUND        (SNET_ERROR_BASE+4)
#define  SNET_ERROR_GAME_TERMINATED       (SNET_ERROR_BASE+5)
#define  SNET_ERROR_HOST_UNREACHABLE      ERROR_HOST_UNREACHABLE
#define  SNET_ERROR_MAX_THRDS_REACHED     ERROR_MAX_THRDS_REACHED
#define  SNET_ERROR_NETWORK_BUSY          ERROR_NETWORK_BUSY
#define  SNET_ERROR_NO_MESSAGES_WAITING   (SNET_ERROR_BASE+6)
#define  SNET_ERROR_NO_NETWORK            ERROR_NO_NETWORK
#define  SNET_ERROR_NOT_CONNECTED         ERROR_NOT_CONNECTED
#define  SNET_ERROR_NOT_ENOUGH_MEMORY     ERROR_NOT_ENOUGH_MEMORY
#define  SNET_ERROR_NOT_IMPLEMENTED       (SNET_ERROR_BASE+7)
#define  SNET_ERROR_NOT_IN_GAME           (SNET_ERROR_BASE+8)
#define  SNET_ERROR_NOT_OWNER             ERROR_NOT_OWNER
#define  SNET_ERROR_NOT_REGISTERED        (SNET_ERROR_BASE+9)
#define  SNET_ERROR_REQUIRES_UPGRADE      (SNET_ERROR_BASE+10)
#define  SNET_ERROR_STILL_ACTIVE          (SNET_ERROR_BASE+11)
#define  SNET_ERROR_TOO_MANY_NAMES        ERROR_TOO_MANY_NAMES
#define  SNET_ERROR_VERSION_MISMATCH      (SNET_ERROR_BASE+12)

#define  SNET_EVENT_INITDATA              1
#define  SNET_EVENT_PLAYERJOIN            2
#define  SNET_EVENT_PLAYERLEAVE           3
#define  SNET_EVENT_SERVERMESSAGE         4

#define  SNET_EXIT_AUTO_JOINING           0x00000001
#define  SNET_EXIT_AUTO_NEWGAME           0x00000002
#define  SNET_EXIT_AUTO_SHUTDOWN          0x00000003
#define  SNET_EXIT_PLAYERQUIT             0x40000001
#define  SNET_EXIT_PLAYERKILLED           0x40000002
#define  SNET_EXIT_PLAYERWON              0x40000004
#define  SNET_EXIT_GAMEOVER               0x40000005
#define  SNET_EXIT_NOTRESPONDING          0x40000006

#define  SNET_GM_PRIVATE                  0x00000001
#define  SNET_GM_FULL                     0x00000002
#define  SNET_GM_ADVERTISED               0x00000004

#define  SNET_INFO_GAMENAME               1
#define  SNET_INFO_GAMEPASSWORD           2
#define  SNET_INFO_GAMEDESCRIPTION        3
#define  SNET_INFO_GAMEMODE               4
#define  SNET_INFO_INITDATA               5
#define  SNET_INFO_MAXPLAYERS             6

#define  SNET_LMT_EXPECTED                1
#define  SNET_LMT_CURRENT                 2
#define  SNET_LMT_PEAK                    4

#define  SNET_PERFID_TURN                 1
#define  SNET_PERFID_TURNSSENT            4
#define  SNET_PERFID_TURNSRECV            5
#define  SNET_PERFID_MSGSENT              6
#define  SNET_PERFID_MSGRECV              7
#define  SNET_PERFID_USERBYTESSENT        8
#define  SNET_PERFID_USERBYTESRECV        9
#define  SNET_PERFID_TOTALBYTESSENT       10
#define  SNET_PERFID_TOTALBYTESRECV       11
#define  SNET_PERFID_PKTSENTONWIRE        12
#define  SNET_PERFID_PKTRECVONWIRE        13
#define  SNET_PERFID_BYTESSENTONWIRE      14
#define  SNET_PERFID_BYTESRECVONWIRE      15
#define  SNET_PERFIDNUM                   16

#define  SNET_PERFTYPE_COUNTER            0x10410400
#define  SNET_PERFTYPE_RAWCOUNT           0x00010000

#define  SNET_PSF_ACTIVE                  0x00010000
#define  SNET_PSF_TURNAVAILABLE           0x00020000
#define  SNET_PSF_RESPONDING              0x00040000

#define  SNET_SF_ALLOWCREATE              0x00000001

#define  SNET_SND_CHANGEFOCUS             0
#define  SNET_SND_SELECTITEM              1

#define  SNET_UPGRADE_FAILED              -1
#define  SNET_UPGRADE_NOT_NEEDED          0
#define  SNET_UPGRADE_SUCCEEDED           1
#define  SNET_UPGRADING_TERMINATE         2

#define  SNET_MAXNAMELENGTH               128
#define  SNET_MAXDESCLENGTH               128

typedef struct _SNETCAPS {
    DWORD size;
    DWORD flags;
    DWORD maxmessagesize;
    DWORD maxqueuesize;
    DWORD maxplayers;
    DWORD bytessec;
    DWORD latencyms;
    DWORD defaultturnssec;
    DWORD defaultturnsintransit;
} SNETCAPS, *SNETCAPSPTR;

typedef struct _SNETCREATEDATA {
    DWORD  size;
    DWORD  providerid;
    DWORD  maxplayers;
    DWORD  createflags;
} SNETCREATEDATA, *SNETCREATEDATAPTR;

typedef struct _SNET_DATA_SYSCOLORTABLE {
	DWORD    syscolor;
	COLORREF rgb;
} SNET_DATA_SYSCOLORTABLE, *SNET_DATA_SYSCOLORTABLEPTR;

typedef struct _SNETEVENT {
    DWORD  eventid;
    DWORD  playerid;
    LPVOID data;
    DWORD  databytes;
} SNETEVENT, *SNETEVENTPTR;

struct _SNETPROGRAMDATA;
struct _SNETPLAYERDATA;
struct _SNETUIDATA;
struct _SNETVERSIONDATA;

typedef BOOL (CALLBACK *SNETABORTPROC        )();
typedef BOOL (CALLBACK *SNETCATEGORYPROC     )(BOOL,_SNETPROGRAMDATA *,_SNETPLAYERDATA *,_SNETUIDATA *,_SNETVERSIONDATA *,DWORD *,DWORD *);
typedef BOOL (CALLBACK *SNETCHECKAUTHPROC    )(DWORD,LPCSTR,LPCSTR,DWORD,LPCSTR,LPSTR,DWORD);
typedef BOOL (CALLBACK *SNETCREATEPROC       )(SNETCREATEDATAPTR,_SNETPROGRAMDATA *,_SNETPLAYERDATA *,_SNETUIDATA *,_SNETVERSIONDATA *,DWORD *);
typedef BOOL (CALLBACK *SNETDRAWDESCPROC     )(DWORD,DWORD,LPCSTR,LPCSTR,DWORD,DWORD,DWORD,LPDRAWITEMSTRUCT);
typedef BOOL (CALLBACK *SNETENUMDEVICESPROC  )(DWORD,LPCSTR,LPCSTR);
typedef BOOL (CALLBACK *SNETENUMGAMESPROC    )(DWORD,LPCSTR,LPCSTR);
typedef BOOL (CALLBACK *SNETENUMPROVIDERSPROC)(DWORD,LPCSTR,LPCSTR,SNETCAPSPTR);
typedef void (CALLBACK *SNETEVENTPROC        )(SNETEVENTPTR);
typedef BOOL (CALLBACK *SNETGETARTPROC       )(DWORD,DWORD,LPPALETTEENTRY,LPBYTE,DWORD,int *,int *,int *);
typedef BOOL (CALLBACK *SNETGETDATAPROC      )(DWORD,DWORD,LPVOID,DWORD,DWORD *);
typedef int  (CALLBACK *SNETMESSAGEBOXPROC   )(HWND,LPCSTR,LPCSTR,UINT);
typedef BOOL (CALLBACK *SNETPLAYSOUNDPROC    )(DWORD,DWORD,DWORD);
typedef BOOL (CALLBACK *SNETSELECTEDPROC     )(DWORD,SNETCAPSPTR,_SNETUIDATA *,_SNETVERSIONDATA *);
typedef BOOL (CALLBACK *SNETSTATUSPROC       )(LPCSTR,DWORD,DWORD,DWORD,SNETABORTPROC);

typedef struct _SNETPLAYERDATA {
    DWORD  size;
    LPCSTR playername;
    LPCSTR playerdescription;
} SNETPLAYERDATA, *SNETPLAYERDATAPTR;

typedef struct _SNETPROGRAMDATA {
    DWORD  size;
    LPCSTR programname;
    LPCSTR programdescription;
    DWORD  programid;
    DWORD  versionid;
    DWORD  reserved1;
    DWORD  maxplayers;
    LPVOID initdata;
    DWORD  initdatabytes;
    LPVOID reserved2;
    DWORD  optcategorybits;
} SNETPROGRAMDATA, *SNETPROGRAMDATAPTR;

typedef struct _SNETUIDATA {
    DWORD              size;
    DWORD              uiflags;
    HWND               parentwindow;
    SNETGETARTPROC     artcallback;
    SNETCHECKAUTHPROC  authcallback;
    SNETCREATEPROC     createcallback;
    SNETDRAWDESCPROC   drawdesccallback;
    SNETSELECTEDPROC   selectedcallback;
    SNETMESSAGEBOXPROC messageboxcallback;
    SNETPLAYSOUNDPROC  soundcallback;
    SNETSTATUSPROC     statuscallback;
    SNETGETDATAPROC    getdatacallback;
    SNETCATEGORYPROC   categorycallback;
} SNETUIDATA, *SNETUIDATAPTR;

typedef struct _SNETVERSIONDATA {
    DWORD  size;
    LPCSTR versionstring;
    LPCSTR executablefile;
    LPCSTR originalarchivefile;
    LPCSTR patcharchivefile;
} SNETVERSIONDATA, *SNETVERSIONDATAPTR;

extern "C" BOOL APIENTRY SNetCreateGame (LPCSTR  gamename,
                                         LPCSTR  gamepassword,
                                         LPCSTR  gamedescription,
                                         DWORD   gamecategorybits,
                                         LPVOID  initdata,
                                         DWORD   initdatabytes,
                                         DWORD   maxplayers,
                                         LPCSTR  playername,
                                         LPCSTR  playerdescription,
                                         DWORD  *playerid);
extern "C" BOOL APIENTRY SNetDestroy ();
extern "C" BOOL APIENTRY SNetDropPlayer (DWORD playerid, DWORD exitcode);
extern "C" BOOL APIENTRY SNetEnumDevices (SNETENUMDEVICESPROC callback);
extern "C" BOOL APIENTRY SNetEnumGames (DWORD              categorybits,
                                        DWORD              categorymask,
                                        SNETENUMGAMESPROC  callback,
                                        DWORD             *hintnextcall);
extern "C" BOOL APIENTRY SNetEnumProviders (SNETCAPSPTR           mincaps,
                                            SNETENUMPROVIDERSPROC callback);
extern "C" BOOL APIENTRY SNetGetGameInfo (DWORD   index,
                                          LPVOID  buffer,
                                          DWORD   buffersize,
                                          DWORD  *byteswritten);
extern "C" BOOL APIENTRY SNetGetNetworkLatency (DWORD  measurementtype,
                                                DWORD *result);
extern "C" BOOL APIENTRY SNetGetNumPlayers (DWORD *firstplayerid,
                                            DWORD *lastplayerid,
                                            DWORD *activeplayers);
extern "C" BOOL APIENTRY SNetGetOwnerTurnsWaiting (DWORD *turns);
extern "C" BOOL APIENTRY SNetGetPerformanceData (DWORD          counterid,
                                                 DWORD         *countervalue,
                                                 DWORD         *countertype,
                                                 LONG          *counterscale,
                                                 LARGE_INTEGER *measurementtime,
                                                 LARGE_INTEGER *measurementfreq);
extern "C" BOOL APIENTRY SNetGetPlayerCaps (DWORD       playerid,
                                            SNETCAPSPTR caps);
extern "C" BOOL APIENTRY SNetGetPlayerName (DWORD playerid,
                                            LPSTR buffer,
                                            DWORD buffersize);
extern "C" BOOL APIENTRY SNetGetProviderCaps (SNETCAPSPTR caps);
extern "C" BOOL APIENTRY SNetGetTurnsInTransit (DWORD *turns);
extern "C" BOOL APIENTRY SNetInitializeDevice (DWORD              deviceid,
                                               SNETPROGRAMDATAPTR programdata,
                                               SNETPLAYERDATAPTR  playerdata,
                                               SNETUIDATAPTR      interfacedata,
                                               SNETVERSIONDATAPTR versiondata);
extern "C" BOOL APIENTRY SNetInitializeProvider (DWORD              providerid,
                                                 SNETPROGRAMDATAPTR programdata,
                                                 SNETPLAYERDATAPTR  playerdata,
                                                 SNETUIDATAPTR      interfacedata,
                                                 SNETVERSIONDATAPTR versiondata);
extern "C" BOOL APIENTRY SNetJoinGame (DWORD   gameid,
                                       LPCSTR  gamename,
                                       LPCSTR  gamepassword,
                                       LPCSTR  playername,
                                       LPCSTR  playerdescription,
                                       DWORD  *playerid);
extern "C" BOOL APIENTRY SNetLeaveGame (DWORD exitcode);
extern "C" BOOL APIENTRY SNetPerformUpgrade (DWORD *upgradestatus);
extern "C" BOOL APIENTRY SNetReceiveMessage (DWORD  *senderplayerid,
                                             LPVOID *data,
                                             DWORD  *databytes);
extern "C" BOOL APIENTRY SNetReceiveTurns (DWORD    firstplayerid,
                                           DWORD    arraysize,
                                           LPVOID  *arraydata,
                                           LPDWORD  arraydatabytes,
                                           LPDWORD  arrayplayerstatus);
extern "C" BOOL APIENTRY SNetRegisterEventHandler (DWORD         eventid,
                                                   SNETEVENTPROC callback);
extern "C" BOOL APIENTRY SNetResetLatencyMeasurements ();
extern "C" BOOL APIENTRY SNetSelectGame (DWORD               flags,
                                         SNETPROGRAMDATAPTR  programdata,
                                         SNETPLAYERDATAPTR   playerdata,
                                         SNETUIDATAPTR       interfacedata,
                                         SNETVERSIONDATAPTR  versiondata,
                                         DWORD              *playerid);
extern "C" BOOL APIENTRY SNetSelectProvider (SNETCAPSPTR         mincaps,
                                             SNETPROGRAMDATAPTR  programdata,
                                             SNETPLAYERDATAPTR   playerdata,
                                             SNETUIDATAPTR       interfacedata,
                                             SNETVERSIONDATAPTR  versiondata,
                                             DWORD              *providerid);
extern "C" BOOL APIENTRY SNetSendMessage (DWORD  targetplayerid,
                                          LPVOID data,
                                          DWORD  databytes);
extern "C" BOOL APIENTRY SNetSendTurn (LPVOID data,
                                       DWORD  databytes);
extern "C" BOOL APIENTRY SNetSetBasePlayer (DWORD playerid);
extern "C" BOOL APIENTRY SNetSetGameMode (DWORD modeflags);
extern "C" BOOL APIENTRY SNetUnregisterEventHandler (DWORD         eventid,
                                                     SNETEVENTPROC callback);

/****************************************************************************
*
*  Networking service provider interface
*
***/

#define  SNETSPI_MAXSTRINGLENGTH  128

typedef struct _SNETADDR {
    BYTE address[16];
} SNETADDR, *SNETADDRPTR;

typedef struct _SNETSPI_DEVICELIST {
    DWORD                deviceid;
    SNETCAPS             devicecaps;
    char                 devicename[SNETSPI_MAXSTRINGLENGTH];
    char                 devicedescription[SNETSPI_MAXSTRINGLENGTH];
    DWORD                reserved;
    _SNETSPI_DEVICELIST *next;
} SNETSPI_DEVICELIST, *SNETSPI_DEVICELISTPTR;

typedef struct _SNETSPI_GAMELIST {
    DWORD              gameid;
    DWORD              gamemode;
    DWORD              creationtime;
    SNETADDR           owner;
    DWORD              ownerlatency;
    DWORD              ownerlasttime;
    DWORD              gamecategorybits;
    char               gamename[SNETSPI_MAXSTRINGLENGTH];
    char               gamedescription[SNETSPI_MAXSTRINGLENGTH];
    _SNETSPI_GAMELIST *next;
} SNETSPI_GAMELIST, *SNETSPI_GAMELISTPTR;

typedef struct _SNETSPI {
    DWORD size;
    BOOL (CALLBACK *CompareNetAddresses)(SNETADDRPTR,SNETADDRPTR,DWORD *);
    BOOL (CALLBACK *Destroy)();
    BOOL (CALLBACK *Free)(SNETADDRPTR,LPVOID,DWORD);
    BOOL (CALLBACK *FreeExternalMessage)(LPCSTR,LPCSTR,LPCSTR);
    BOOL (CALLBACK *GetGameInfo)(DWORD,LPCSTR,LPCSTR,SNETSPI_GAMELIST *);
    BOOL (CALLBACK *GetPerformanceData)(DWORD,DWORD *,LARGE_INTEGER *,LARGE_INTEGER *);
    BOOL (CALLBACK *Initialize)(SNETPROGRAMDATAPTR,SNETPLAYERDATAPTR,SNETUIDATAPTR,SNETVERSIONDATAPTR,HANDLE);
    BOOL (CALLBACK *InitializeDevice)(DWORD,SNETPROGRAMDATAPTR,SNETPLAYERDATAPTR,SNETUIDATAPTR,SNETVERSIONDATAPTR);
    BOOL (CALLBACK *LockDeviceList)(SNETSPI_DEVICELISTPTR *);
    BOOL (CALLBACK *LockGameList)(DWORD,DWORD,SNETSPI_GAMELISTPTR *);
    BOOL (CALLBACK *Receive)(SNETADDRPTR *,LPVOID *,DWORD *);
    BOOL (CALLBACK *ReceiveExternalMessage)(LPCSTR *,LPCSTR *,LPCSTR *);
    BOOL (CALLBACK *SelectGame)(DWORD,SNETPROGRAMDATAPTR,SNETPLAYERDATAPTR,SNETUIDATAPTR,SNETVERSIONDATAPTR,DWORD *);
    BOOL (CALLBACK *Send)(DWORD,SNETADDRPTR *,LPVOID,DWORD);
    BOOL (CALLBACK *SendExternalMessage)(LPCSTR,LPCSTR,LPCSTR,LPCSTR,LPCSTR);
    BOOL (CALLBACK *StartAdvertisingGame)(LPCSTR,LPCSTR,LPCSTR,DWORD,DWORD,DWORD,DWORD);
    BOOL (CALLBACK *StopAdvertisingGame)();
    BOOL (CALLBACK *UnlockDeviceList)(SNETSPI_DEVICELISTPTR);
    BOOL (CALLBACK *UnlockGameList)(SNETSPI_GAMELISTPTR,DWORD *);
} SNETSPI, *SNETSPIPTR;

typedef BOOL (APIENTRY *SNETSPIBIND )(DWORD,SNETSPIPTR *);
typedef BOOL (APIENTRY *SNETSPIQUERY)(DWORD,DWORD *,LPCSTR *,LPCSTR *,SNETCAPSPTR *);

/****************************************************************************
*
*  Registry functions
*
***/

#define  SREG_FLAG_USERSPECIFIC  0x00000001
#define  SREG_FLAG_BATTLENET     0x00000002
#define  SREG_FLAG_FLUSHTODISK   0x00000008
#define  SREG_FLAG_MULTISZ       0x00000080

extern "C" BOOL APIENTRY SRegLoadData (LPCTSTR  keyname,
                                       LPCTSTR  valuename,
                                       DWORD    flags,
                                       LPVOID   buffer,
                                       DWORD    buffersize,
                                       DWORD   *bytesread);
extern "C" BOOL APIENTRY SRegLoadString (LPCTSTR keyname,
                                         LPCTSTR valuename,
                                         DWORD   flags,
                                         LPTSTR  buffer,
                                         DWORD   bufferchars);
extern "C" BOOL APIENTRY SRegLoadValue (LPCTSTR  keyname,
                                        LPCTSTR  valuename,
                                        DWORD    flags,
                                        DWORD   *value);
extern "C" BOOL APIENTRY SRegSaveData (LPCTSTR keyname,
                                       LPCTSTR valuename,
                                       DWORD   flags,
                                       LPVOID  data,
                                       DWORD   databytes);
extern "C" BOOL APIENTRY SRegSaveString (LPCTSTR keyname,
                                         LPCTSTR valuename,
                                         DWORD   flags,
                                         LPCTSTR string);
extern "C" BOOL APIENTRY SRegSaveValue (LPCTSTR keyname,
                                        LPCTSTR valuename,
                                        DWORD   flags,
                                        DWORD   value);

/****************************************************************************
*
*  Run-time library functions
*
***/

#define  TRY      goto trylabel; trylabel:
#define  LEAVE    goto finallylabel
#define  FINALLY  goto finallylabel; finallylabel:

/****************************************************************************
*
*  Transparency functions
*
***/

typedef struct _HSTRANS { int unused; } *HSTRANS;
typedef HSTRANS HTRANS;

extern "C" BOOL APIENTRY STransBlt (LPBYTE  dest,
                                    int     destx,
                                    int     desty,
                                    int     destpitch,
                                    HSTRANS transparency);
extern "C" BOOL APIENTRY STransBltUsingMask (LPBYTE  dest,
                                             LPBYTE  source,
                                             int     destpitch,
                                             int     sourcepitch,
                                             HSTRANS mask);
extern "C" BOOL APIENTRY STransCreate (LPBYTE    bits,
                                       int       width,
                                       int       height,
                                       int       bitdepth,
                                       LPRECT    rect,
                                       COLORREF  colorkey,
                                       HSTRANS  *handle);
extern "C" BOOL APIENTRY STransDelete (HSTRANS handle);
extern "C" BOOL APIENTRY STransDestroy ();
extern "C" BOOL APIENTRY STransDuplicate (HSTRANS  source,
                                          HSTRANS *handle);
extern "C" BOOL APIENTRY STransIntersectDirtyArray (HSTRANS  sourcemask,
                                                    LPBYTE   dirtyarray,
                                                    BYTE     dirtyarraymask,
                                                    HSTRANS *handle);
extern "C" BOOL APIENTRY STransInvertMask (HSTRANS  sourcemask,
                                           HSTRANS *handle);
extern "C" BOOL APIENTRY STransLoad (LPCTSTR   filename,
                                     LPRECT    rect,
                                     COLORREF  colorkey,
                                     HSTRANS  *handle);
extern "C" BOOL APIENTRY STransSetDirtyArrayInfo (int screencx,
                                                  int screency,
                                                  int cellcx,
                                                  int cellcy);
extern "C" BOOL APIENTRY STransUpdateDirtyArray (LPBYTE  dirtyarray,
                                                 BYTE    dirtyvalue,
                                                 int     destx,
                                                 int     desty,
                                                 HSTRANS transparency,
                                                 BOOL    tracecontour);

/****************************************************************************
*
*  Video functions
*
***/

#define  SVID_FLAG_DOUBLESCANS            0x00000001
#define  SVID_FLAG_INTERPOLATE            0x00000002
#define  SVID_FLAG_AUTOQUALITY            0x00000008
#define  SVID_FLAG_1XSIZE                 0x00000100
#define  SVID_FLAG_2XSIZE                 0x00000200
#define  SVID_FLAG_AUTOSIZE               0x00000800
#define  SVID_FLAG_FILEHANDLE             0x00010000
#define  SVID_FLAG_PRELOAD                0x00020000
#define  SVID_FLAG_LOOP                   0x00040000
#define  SVID_FLAG_FULLSCREEN             0x00080000
#define  SVID_FLAG_USECURRENTPALETTE      0x00100000
#define  SVID_FLAG_CLEARSCREEN            0x00200000
#define  SVID_FLAG_TOSCREEN               0x10000000
#define  SVID_FLAG_TOBUFFER               0x20000000

#define  SVID_CUTSCENE                    (SVID_FLAG_TOSCREEN | SVID_FLAG_FULLSCREEN | SVID_FLAG_CLEARSCREEN | SVID_FLAG_2XSIZE)
#define  SVID_AUTOCUTSCENE                (SVID_FLAG_TOSCREEN | SVID_FLAG_FULLSCREEN | SVID_FLAG_CLEARSCREEN | SVID_FLAG_AUTOSIZE | SVID_FLAG_AUTOQUALITY)

#define  SVID_QUALITY_LOW_SKIPSCANS       SVID_FLAG_2XSIZE
#define  SVID_QUALITY_LOW                 (SVID_FLAG_2XSIZE | SVID_FLAG_DOUBLESCANS)
#define  SVID_QUALITY_HIGH_SKIPSCANS      (SVID_FLAG_2XSIZE | SVID_FLAG_INTERPOLATE)
#define  SVID_QUALITY_HIGH                (SVID_FLAG_2XSIZE | SVID_FLAG_INTERPOLATE | SVID_FLAG_DOUBLESCANS)

typedef struct _HSVIDEO { int unused; } *HSVIDEO;

typedef struct _SVIDPALETTEUSE {
    DWORD size;
    DWORD firstentry;
    DWORD numentries;
} SVIDPALETTEUSE, *SVIDPALETTEUSEPTR;

extern "C" BOOL APIENTRY SVidDestroy ();
extern "C" BOOL APIENTRY SVidGetPerformanceData (HSVIDEO  video,
                                                 BOOL     averageframems,
                                                 DWORD   *framems,
                                                 BOOL     averagepalettems,
                                                 DWORD   *palettems);
extern "C" BOOL APIENTRY SVidGetSize (HSVIDEO  video,
                                      int     *width,
                                      int     *height,
                                      int     *bitdepth = NULL);
extern "C" BOOL APIENTRY SVidInitialize (LPVOID directsound);
extern "C" BOOL APIENTRY SVidPlayBegin (LPCTSTR            filename,
                                        LPVOID             destbuffer,
                                        LPCRECT            destrect,
                                        LPSIZE             destsize,
                                        SVIDPALETTEUSEPTR  paletteuse,
                                        DWORD              flags,
                                        HSVIDEO           *handle);
extern "C" BOOL APIENTRY SVidPlayBeginFromMemory (LPVOID             sourceptr,
                                                  DWORD              sourcebytes,
                                                  LPVOID             destbuffer,
                                                  LPCRECT            destrect,
                                                  LPSIZE             destsize,
                                                  SVIDPALETTEUSEPTR  paletteuse,
                                                  DWORD              flags,
                                                  HSVIDEO           *handle);
extern "C" BOOL APIENTRY SVidPlayContinue ();
extern "C" BOOL APIENTRY SVidPlayContinueSingle (HSVIDEO  video,
                                                 BOOL     forceupdate,
                                                 BOOL    *updated);
extern "C" BOOL APIENTRY SVidPlayEnd (HSVIDEO video);

/****************************************************************************
*
*  Storm global functions
*
***/

extern "C" BOOL APIENTRY StormDestroy ();

/****************************************************************************
*
*  C++ Utility Classes and Templates
*
***/

#ifndef __cplusplus
extern "C++"
#endif

/****************************************************************************
*
*  CCRITSECT -- Critical section class
*
***/

class CCritSect {
  private:
    CRITICAL_SECTION m_critsect;
  public:
    CCritSect () { InitializeCriticalSection(&m_critsect); }
    ~CCritSect () { DeleteCriticalSection(&m_critsect); }
    void inline Enter () { EnterCriticalSection(&m_critsect); }
    void inline Leave () { LeaveCriticalSection(&m_critsect); }
};

/****************************************************************************
*
*  CLOCK -- Lock class
*
***/

class CLock {

  private:

    HANDLE m_mutexevent;
    HANDLE m_readerevent;
    LONG   m_readercount;

  public:

    CLock () {
      m_mutexevent  = CreateEvent(NULL,FALSE,TRUE,NULL);
      m_readerevent = CreateEvent(NULL,TRUE,FALSE,NULL);
      m_readercount = -1;
    }

    ~CLock () {
      CloseHandle(m_readerevent);
      CloseHandle(m_mutexevent);
    }

    void inline Enter (BOOL forwriting) {
      if (forwriting)
        WaitForSingleObject(m_mutexevent,INFINITE);
      else if (!InterlockedIncrement(&m_readercount)) {
        WaitForSingleObject(m_mutexevent,INFINITE);
        SetEvent(m_readerevent);
      }
      else
        WaitForSingleObject(m_readerevent,INFINITE);
    }

    void inline Leave (BOOL fromwriting) {
      if (fromwriting)
        SetEvent(m_mutexevent);
      else if (InterlockedDecrement(&m_readercount) < 0) {
        ResetEvent(m_readerevent);
        SetEvent(m_mutexevent);
      }
    }

};

/****************************************************************************
*
*  TLIST -- Linked list function templates
*
***/

//===========================================================================
template <class T>
BOOL inline TListAdd (T **head, T *rec, LPCSTR filename = NULL, int linenumber = 0) {
  if (!(head && rec))
    return 0;

  T *newptr = (T *)SMemAlloc(sizeof(T),filename,linenumber,0);
  if (!newptr)
    return 0;
  CopyMemory(newptr,rec,sizeof(T));
  newptr->next = *head;
  *head        = newptr;
  return 1;
}
#define  LISTADD(a,b)  TListAdd(a,b,(LPCSTR)__FILE__,__LINE__)

//===========================================================================
template <class T>
BOOL inline TListAddEnd (T **head, T *rec, LPCSTR filename = NULL, int linenumber = 0) {
  if (!(head && rec))
    return 0;

  T *newptr = (T *)SMemAlloc(sizeof(T),filename,linenumber,0);
  if (!newptr)
    return 0;
  CopyMemory(newptr,rec,sizeof(T));
  newptr->next = NULL;

  T **next = head;
  while (*next)
    next = &(*next)->next;
  *next = newptr;

  return 1;
}
#define  LISTADDEND(a,b)  TListAddEnd(a,b,(LPCSTR)__FILE__,__LINE__)

//===========================================================================
template <class T>
BOOL inline TListAddPtr (T **head, T *ptr) {
  if (!(head && ptr))
    return 0;

  ptr->next = *head;
  *head     = ptr;
  return 1;
}
#define  LISTADDPTR(a,b)  TListAddPtr(a,b)

//===========================================================================
template <class T>
BOOL inline TListAddPtrEnd (T **head, T *ptr) {
  if (!(head && ptr))
    return 0;

  ptr->next = NULL;
  T **next = head;
  while (*next)
    next = &(*next)->next;
  *next = ptr;

  return 1;
}
#define  LISTADDPTREND(a,b)  TListAddPtrEnd(a,b)

//===========================================================================
template <class T>
BOOL inline TListClear (T **head, LPCSTR filename = NULL, int linenumber = 0) {
  if (!head)
    return 0;

  while (*head) {
    T *next = (*head)->next;
    SMemFree(*head,filename,linenumber,0);
    *head = next;
  }
  return 1;
}
#define  LISTCLEAR(a)  TListClear(a,(LPCSTR)__FILE__,__LINE__)

//===========================================================================
template <class T>
BOOL inline TListFree (T **head, T *ptr, LPCSTR filename = NULL, int linenumber = 0) {
  if (!(head && ptr))
    return 0;

  T **next = head;
  while (*next && (*next != ptr))
    next = &(*next)->next;
  if (*next)
    *next = (*next)->next;

  SMemFree(ptr,filename,linenumber,0);
  return (*next != NULL);
}
#define  LISTFREE(a,b)  TListFree(a,b,(LPCSTR)__FILE__,__LINE__)

//===========================================================================
template <class T>
BOOL inline TListFreePtr (T **head, T *ptr, LPCSTR filename = NULL, int linenumber = 0) {
  if (!(head && ptr))
    return 0;

  T **next = head;
  while (*next && (*next != ptr))
    next = &(*next)->next;
  if (*next)
    *next = (*next)->next;

  return (*next != NULL);
}
#define  LISTFREEPTR(a,b)  TListFreePtr(a,b,(LPCSTR)__FILE__,__LINE__)

#ifndef __cplusplus
} // extern "C++"
#endif

#endif // ifndef _STORM_H_

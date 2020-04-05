#ifndef  _STORM_H_
#define  _STORM_H_

#if PRAGMA_IMPORT_SUPPORTED
#pragma import on
#endif


//#########################################################################//
//#########################################################################//
//                                                                         //
//                                                                         //
//                     STANDARD PROGRAMMING INTERFACE                      //
//                                                                         //
//                                                                         //
//#########################################################################//
//#########################################################################//


#define  DECLARE_STRICT_HANDLE(name)        typedef struct name##__ { int unused; } *name
#define  DECLARE_DERIVED_HANDLE(name,base)  typedef struct name##__ : public base##__ { int unused; } *name


/****************************************************************************
*
*  Error codes
*  (Error text is defined in Stormerr.mc)
*
***/

#define  STORMFAC                             0x510
#define  STORMERROR(code)                     (0x80000000 | (STORMFAC << 16) | ((code) & 0xFFFF))

#define  STORM_ERROR_ASSERTION                STORMERROR(0)
#define  STORM_ERROR_BAD_ARGUMENT             STORMERROR(101)
#define  STORM_ERROR_GAME_ALREADY_STARTED     STORMERROR(102)
#define  STORM_ERROR_GAME_FULL                STORMERROR(103)
#define  STORM_ERROR_GAME_NOT_FOUND           STORMERROR(104)
#define  STORM_ERROR_GAME_TERMINATED          STORMERROR(105)
#define  STORM_ERROR_INVALID_PLAYER           STORMERROR(106)
#define  STORM_ERROR_NO_MESSAGES_WAITING      STORMERROR(107)
#define  STORM_ERROR_NOT_ARCHIVE              STORMERROR(108)
#define  STORM_ERROR_NOT_ENOUGH_ARGUMENTS     STORMERROR(109)
#define  STORM_ERROR_NOT_IMPLEMENTED          STORMERROR(110)
#define  STORM_ERROR_NOT_IN_ARCHIVE           STORMERROR(111)
#define  STORM_ERROR_NOT_IN_GAME              STORMERROR(112)
#define  STORM_ERROR_NOT_INITIALIZED          STORMERROR(113)
#define  STORM_ERROR_NOT_PLAYING              STORMERROR(114)
#define  STORM_ERROR_NOT_REGISTERED           STORMERROR(115)
#define  STORM_ERROR_REQUIRES_CODEC           STORMERROR(116)
#define  STORM_ERROR_REQUIRES_DDRAW           STORMERROR(117)
#define  STORM_ERROR_REQUIRES_DSOUND          STORMERROR(118)
#define  STORM_ERROR_REQUIRES_UPGRADE         STORMERROR(119)
#define  STORM_ERROR_STILL_ACTIVE             STORMERROR(120)
#define  STORM_ERROR_VERSION_MISMATCH         STORMERROR(121)
#define  STORM_ERROR_MEMORY_ALREADY_FREED     STORMERROR(122)
#define  STORM_ERROR_MEMORY_CORRUPT           STORMERROR(123)
#define  STORM_ERROR_MEMORY_INVALID_BLOCK     STORMERROR(124)
#define  STORM_ERROR_MEMORY_MANAGER_INACTIVE  STORMERROR(125)
#define  STORM_ERROR_MEMORY_NEVER_RELEASED    STORMERROR(126)
#define  STORM_ERROR_HANDLE_NEVER_RELEASED    STORMERROR(127)
#define  STORM_ERROR_ACCESS_OUT_OF_BOUNDS     STORMERROR(128)
#define  STORM_ERROR_MEMORY_NULL_POINTER      STORMERROR(129)


/****************************************************************************
*
*  BitBlt functions
*
***/

#ifdef STORMSTATIC
extern "C" BOOL APIENTRY SBltDestroy ();
#endif
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

#define  SBMP_IMAGETYPE_AUTO  0
#define  SBMP_IMAGETYPE_BMP   1
#define  SBMP_IMAGETYPE_PCX   2

typedef LPVOID (APIENTRY *SBMPALLOCPROC)(DWORD);

extern "C" BOOL APIENTRY SBmpAllocLoadImage (LPCTSTR         filename,
                                             LPPALETTEENTRY  paletteentries,
                                             LPBYTE         *returnedbuffer,
                                             int            *width             = NULL,
                                             int            *height            = NULL,
                                             int            *bitdepth          = NULL,
                                             int             requestedbitdepth = 0,
                                             SBMPALLOCPROC   allocproc         = NULL);
extern "C" BOOL APIENTRY SBmpDecodeImage (DWORD           imagetype,
                                          LPBYTE          imagedata,
                                          DWORD           imagebytes,
                                          LPPALETTEENTRY  paletteentries,
                                          LPBYTE          bitmapbits,
                                          DWORD           buffersize,
                                          int            *width    = NULL,
                                          int            *height   = NULL,
                                          int            *bitdepth = NULL);
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
*  Command line parsing functions
*
***/

#define  SCMD_ARG_FLAGGED    (0 << 24)
#define  SCMD_ARG_OPTIONAL   (1 << 24)
#define  SCMD_ARG_REQUIRED   (2 << 24)
#define  SCMD_ARG_MASK       (SCMD_ARG_FLAGGED | SCMD_ARG_OPTIONAL | SCMD_ARG_REQUIRED)

#define  SCMD_BOOL_SET       0
#define  SCMD_BOOL_CLEAR     1
#define  SCMD_BOOL_MASK      (SCMD_BOOL_CLEAR | SCMD_BOOL_SET)

#define  SCMD_CASESENSITIVE  (0x01 << 8)

#define  SCMD_NUM_UNSIGNED   0
#define  SCMD_NUM_SIGNED     1
#define  SCMD_NUM_MASK       (SCMD_NUM_UNSIGNED | SCMD_NUM_SIGNED)

#define  SCMD_TYPE_BOOL      (0 << 16)
#define  SCMD_TYPE_NUMERIC   (1 << 16)
#define  SCMD_TYPE_STRING    (2 << 16)
#define  SCMD_TYPE_MASK      (SCMD_TYPE_BOOL | SCMD_TYPE_NUMERIC | SCMD_TYPE_STRING)

#define  SCMD_ERROR_BAD_ARGUMENT          STORM_ERROR_BAD_ARGUMENT
#define  SCMD_ERROR_NOT_ENOUGH_ARGUMENTS  STORM_ERROR_NOT_ENOUGH_ARGUMENTS
#define  SCMD_ERROR_OPEN_FAILED           ERROR_OPEN_FAILED

typedef struct _CMDERROR {
    DWORD   errorcode;
    LPCTSTR itemstr;
    LPCTSTR errorstr;
} CMDERROR, *CMDERRORPTR;

typedef struct _CMDPARAMS {
    DWORD   flags;
    DWORD   id;
    LPCTSTR name;
    LPVOID  variable;
    DWORD   setvalue;
    DWORD   setmask;
    union {
      BOOL    boolvalue;
      LONG    signedvalue;
      DWORD   unsignedvalue;
      LPCTSTR stringvalue;
    };
} CMDPARAMS, *CMDPARAMSPTR;

typedef BOOL (CALLBACK *SCMDCALLBACK)(CMDPARAMSPTR,LPCTSTR);
typedef void (CALLBACK *SCMDERRORCALLBACK)(CMDERRORPTR);
typedef BOOL (CALLBACK *SCMDEXTRACALLBACK)(LPCTSTR);

typedef struct _ARGLIST {
    DWORD        flags;
    DWORD        id;
    LPCTSTR      name;
    SCMDCALLBACK callback;
} ARGLIST, *ARGLISTPTR;

extern "C" BOOL  APIENTRY SCmdCheckId (DWORD id);
#ifdef STORMSTATIC
extern "C" BOOL  APIENTRY SCmdDestroy ();
#endif
extern "C" BOOL  APIENTRY SCmdGetBool (DWORD id);
extern "C" DWORD APIENTRY SCmdGetNum (DWORD id);
extern "C" BOOL  APIENTRY SCmdGetString (DWORD  id,
                                         LPTSTR buffer,
                                         DWORD  bufferchars);
extern "C" BOOL  APIENTRY SCmdProcess (LPCTSTR           cmdline,
                                       BOOL              skipprogname,
                                       SCMDEXTRACALLBACK extracallback,
                                       SCMDERRORCALLBACK errorcallback);
extern "C" BOOL  APIENTRY SCmdRegisterArgList (const ARGLIST *listptr,
                                               DWORD          numargs);
extern "C" BOOL  APIENTRY SCmdRegisterArgument (DWORD        flags,
                                                DWORD        id,
                                                LPCTSTR      name,
                                                LPVOID       variableptr   = NULL,
                                                DWORD        variablebytes = 0,
                                                DWORD        setvalue      = TRUE,
                                                DWORD        setmask       = 0xFFFFFFFF,
                                                SCMDCALLBACK callback      = NULL);

#define  SCmdProcessCommandLine(ext,err)              SCmdProcess(GetCommandLine(),TRUE,(ext),(err))

#define  ARGBOOL(flags,name,var,callback)             SCmdRegisterArgument(SCMD_TYPE_BOOL    | (flags),0xFFFFFFFF,name,var,sizeof(var),TRUE,0xFFFFFFFF,callback)
#define  ARGFLAG(flags,name,var,valuecallback)        SCmdRegisterArgument(SCMD_TYPE_BOOL    | (flags),0xFFFFFFFF,name,var,sizeof(var),value,value,callback)
#define  ARGNUMBER(flags,name,var,callback)           SCmdRegisterArgument(SCMD_TYPE_NUMERIC | (flags),0xFFFFFFFF,name,var,sizeof(var),0,0,callback)
#define  ARGSTRING(flags,name,buffer,chars,callback)  SCmdRegisterArgument(SCMD_TYPE_STRING  | (flags),0xFFFFFFFF,name,buffer,(chars),0,0,callback)


/****************************************************************************
*
*  S-Code functions
*
***/

#define  SCODE_CF_AUTOALIGNDWORD  0x00040000
#define  SCODE_CF_USESALTADJUSTS  0x04000000

DECLARE_STRICT_HANDLE(HSCODESTREAM);

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
#ifdef STORMSTATIC
extern "C" BOOL APIENTRY SCodeDestroy ();
#endif
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
*  Compression functions
*
***/

#define  SCOMP_HINT_NONE                0
#define  SCOMP_HINT_BINARY              1
#define  SCOMP_HINT_TEXT                2
#define  SCOMP_HINT_EXECUTABLE          3
#define  SCOMP_HINT_ADPCM4              4
#define  SCOMP_HINT_ADPCM6              5
#define  SCOMP_HINTS                    6

#define  SCOMP_OPT_DEFAULT              0
#define  SCOMP_OPT_COMPRESSION          1
#define  SCOMP_OPT_SPEED                2
#define  SCOMP_OPT_QUALITY              3

#define  SCOMP_TYPE_HUFFMAN             0x01
#define  SCOMP_TYPE_PKWARE              0x08
#define  SCOMP_TYPE_LOSSY_ADPCM_MONO    0x10
#define  SCOMP_TYPE_LOSSY_ADPCM_STEREO  0x20

extern "C" BOOL APIENTRY SCompCompress (LPVOID   dest,
                                        DWORD   *destsize,
                                        LPCVOID  source,
                                        DWORD    sourcesize,
                                        DWORD    compressiontypes,
                                        DWORD    hint,
                                        DWORD    optimization);
extern "C" BOOL APIENTRY SCompDecompress (LPVOID   dest,
                                          DWORD   *destsize,
                                          LPCVOID  source,
                                          DWORD    sourcesize);


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
extern "C" BOOL APIENTRY SDlgBltToWindowE (HWND   window,
                                           HRGN   region,
                                           int    x,
                                           int    y,
                                           LPBYTE bitmapbits,
                                           LPRECT bitmaprect,
                                           LPSIZE bitmapsize,
                                           DWORD  colorkey = 0xFFFFFFFF,
                                           DWORD  pattern  = 0,
                                           DWORD  rop3     = SRCCOPY);
extern "C" BOOL APIENTRY SDlgBltToWindowI (HWND   window,
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
#ifdef STORMSTATIC
extern "C" BOOL APIENTRY SDlgDestroy ();
#endif
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
extern "C" BOOL APIENTRY SDlgSetBitmapE (HWND     window,
                                         HWND     parentwindow,
                                         LPCTSTR  controltype,
                                         DWORD    controlstyle,
                                         DWORD    usage,
                                         LPBYTE   bitmapbits,
                                         LPRECT   rect,
                                         int      width,
                                         int      height,
                                         COLORREF colorkey = 0xFFFFFFFF);
extern "C" BOOL APIENTRY SDlgSetBitmapI (HWND     window,
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

#ifdef   SDLG_USE_INCLUSIVE_RECTS
#define  SDlgBltToWindow  SDlgBltToWindowI
#define  SDlgSetBitmap    SDlgSetBitmapI
#else
#define  SDlgBltToWindow  SDlgBltToWindowE
#define  SDlgSetBitmap    SDlgSetBitmapE
#endif

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

#ifndef MAC
extern "C" BOOL APIENTRY SDrawAutoInitialize (HINSTANCE instance,
                                              LPCTSTR   classname,
                                              LPCTSTR   title,
                                              WNDPROC   wndproc      = NULL,
                                              int       servicelevel = SDRAW_SERVICE_BASIC,
                                              int       width        = 640,
                                              int       height       = 480,
                                              int       bitdepth     = 8);
extern "C" BOOL APIENTRY SDrawCaptureScreen (LPCTSTR filename = NULL);
#endif
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
extern "C" int  APIENTRY SDrawMessageBox (LPCTSTR text,
                                          LPCTSTR title,
                                          UINT    flags);
extern "C" BOOL APIENTRY SDrawPostClose ();
extern "C" BOOL APIENTRY SDrawRealizePalette ();
#ifndef MAC
extern "C" BOOL APIENTRY SDrawSelectGdiSurface (BOOL select, BOOL copy);
#endif
extern "C" BOOL APIENTRY SDrawUnlockSurface (int     surfacenumber,
                                             LPBYTE  ptr,
                                             DWORD   numrects  = 0,
                                             LPCRECT rectarray = NULL);
extern "C" BOOL APIENTRY SDrawUpdatePalette (DWORD          firstentry,
                                             DWORD          numentries,
                                             LPPALETTEENTRY entries,
                                             BOOL           reservedentries = FALSE);
extern "C" BOOL APIENTRY SDrawUpdateScreen (LPCRECT rect);
#ifdef MAC
extern "C" BOOL APIENTRY SDrawVidDriverInitialize (HWND framewindow,
                                                   int  servicelevel);
#endif


/****************************************************************************
*
*  Error handling functions
*
***/

#define  SERR_LINECODE_FUNCTION  -1
#define  SERR_LINECODE_OBJECT    -2
#define  SERR_LINECODE_HANDLE    -3
#define  SERR_LINECODE_FILE      -4

#ifdef STORMSTATIC
extern "C" BOOL  APIENTRY SErrDestroy ();
#endif
extern "C" BOOL  APIENTRY SErrDisplayError (DWORD   errorcode,
                                            LPCTSTR filename,
                                            int     linenumber,
                                            LPCTSTR description,
                                            BOOL    recoverable,
                                            UINT    exitcode = 1);
extern "C" BOOL  APIENTRY SErrGetErrorStr (DWORD  errorcode,
                                           LPTSTR buffer,
                                           DWORD  bufferchars);
extern "C" DWORD APIENTRY SErrGetLastError ();
extern "C" BOOL  APIENTRY SErrRegisterMessageSource (WORD    facility,
                                                     HMODULE module,
                                                     LPVOID  reserved = NULL);
extern "C" void  APIENTRY SErrReportResourceLeak (LPCTSTR handlename);
extern "C" void  APIENTRY SErrSetLastError (DWORD errorcode);
extern "C" void  APIENTRY SErrSuppressErrors (BOOL suppress);

#define  SErrGetLastErrorStr(buf,len)  SErrGetErrorStr(SErrGetLastError(),buf,len)
#define  FATALRESULT(str)              SErrDisplayError(SErrGetLastError(),str,SERR_LINECODE_FUNCTION,NULL,FALSE)
#define  REPORTRESOURCELEAK(handle)    SErrReportResourceLeak(#handle)

#ifdef _DEBUG
#define  ASSERT(a)            if (!(a))                                    \
                                SErrDisplayError(STORM_ERROR_ASSERTION,    \
                                                 __FILE__,                 \
                                                 __LINE__,                 \
                                                 #a,                       \
                                                 FALSE)
#define  VALIDATEBEGIN        do {
#define  VALIDATE(a)            ASSERT(a)
#define  VALIDATEANDBLANK(a)    do {                                       \
                                  ASSERT(a);                               \
                                  *(a) = 0;                                \
                                } while (0)
#define  VALIDATEEND          } while (0)
#define  VALIDATEENDVOID      } while (0)
#else
#define  ASSERT(a)
#define  VALIDATEBEGIN        do {                                         \
                                int intrn_valresult = -1
#define  VALIDATE(a)            intrn_valresult &= (a) ? -1 : 0
#define  VALIDATEANDBLANK(a)    if (a)                                     \
                                  *a = 0;                                  \
                                else                                       \
                                  intrn_valresult = 0
#define  VALIDATEEND            if (!intrn_valresult) {                    \
                                  SErrSetLastError(                        \
                                    ERROR_INVALID_PARAMETER);              \
                                  return 0;                                \
                                }                                          \
                              } while (0)
#define  VALIDATEENDVOID        if (!intrn_valresult) {                    \
                                  SErrSetLastError(                        \
                                    ERROR_INVALID_PARAMETER);              \
                                  return;                                  \
                                }                                          \
                              } while (0)
#endif


/****************************************************************************
*
*  Event dispatching functions
*
***/

typedef void (CALLBACK *SEVTHANDLER)(LPVOID);

extern "C" BOOL APIENTRY SEvtBreakHandlerChain (LPVOID data);
#ifdef STORMSTATIC
extern "C" BOOL APIENTRY SEvtDestroy ();
#endif
extern "C" BOOL APIENTRY SEvtDispatch (DWORD  type,
                                       DWORD  subtype,
                                       DWORD  id,
                                       LPVOID data);
extern "C" BOOL APIENTRY SEvtPopState (DWORD type,
                                       DWORD subtype);
extern "C" BOOL APIENTRY SEvtPushState (DWORD type,
                                        DWORD subtype);
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
#define  SFILE_ERROR_NOT_ARCHIVE           STORM_ERROR_NOT_ARCHIVE
#define  SFILE_ERROR_NOT_AUTHENTICATED     ERROR_NOT_AUTHENTICATED
#define  SFILE_ERROR_NOT_ENOUGH_MEMORY     ERROR_NOT_ENOUGH_MEMORY
#define  SFILE_ERROR_NOT_IN_ARCHIVE        STORM_ERROR_NOT_IN_ARCHIVE
#define  SFILE_ERROR_NOT_INITIALIZED       STORM_ERROR_NOT_INITIALIZED
#define  SFILE_ERROR_NOT_PLAYING           STORM_ERROR_NOT_PLAYING

#define  SFILE_ERRORMODE_RETURNCODE        0
#define  SFILE_ERRORMODE_CUSTOM            1
#define  SFILE_ERRORMODE_FATAL             2

#define  SFILE_FIND_FILES                  0x00000001
#define  SFILE_FIND_DIRECTORIES            0x00000002

DECLARE_STRICT_HANDLE(HSARCHIVE);
DECLARE_STRICT_HANDLE(HSFILE);

typedef BOOL (CALLBACK *SFILEERRORPROC)(LPCTSTR,DWORD);

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
#ifdef STORMSTATIC
extern "C" BOOL  APIENTRY SFileDestroy ();
#endif
extern "C" BOOL  APIENTRY SFileEnableDirectAccess (BOOL enable);
extern "C" BOOL  APIENTRY SFileGetArchiveInfo (HSARCHIVE  archive,
                                               int       *priority,
                                               BOOL      *cdrom);
extern "C" BOOL  APIENTRY SFileGetArchiveName (HSARCHIVE archive,
                                               LPTSTR    buffer,
                                               DWORD     bufferchars);
extern "C" BOOL  APIENTRY SFileGetBasePath (LPTSTR buffer,
                                            DWORD  bufferchars);
extern "C" BOOL  APIENTRY SFileGetFileArchive (HSFILE     file,
                                               HSARCHIVE *archive);
extern "C" BOOL  APIENTRY SFileGetFileName (HSFILE file,
                                            LPTSTR buffer,
                                            DWORD  bufferchars);
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
extern "C" BOOL  APIENTRY SFileReadFile (HSFILE       handle,
                                         LPVOID       buffer,
                                         DWORD        bytestoread,
                                         LPDWORD      bytesread  = NULL,
                                         LPOVERLAPPED overlapped = NULL);
extern "C" BOOL  APIENTRY SFileSetBasePath (LPCTSTR path);
extern "C" DWORD APIENTRY SFileSetFilePointer (HSFILE handle,
                                               LONG   distancetomove,
                                               PLONG  distancetomovehigh,
                                               DWORD  movemethod);
extern "C" BOOL  APIENTRY SFileSetIoErrorMode (DWORD          errormode,
                                               SFILEERRORPROC errorproc = NULL);
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

DECLARE_STRICT_HANDLE(HSGDIOBJ);
DECLARE_DERIVED_HANDLE(HSGDIFONT,HSGDIOBJ);

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
#ifdef STORMSTATIC
extern "C" BOOL APIENTRY SGdiDestroy ();
#endif
extern "C" BOOL APIENTRY SGdiExtTextOut (LPBYTE   videobuffer,
                                         int      x,
                                         int      y,
                                         LPRECT   rect,
                                         COLORREF color,
                                         int      textcoloruse,
                                         int      bkgcoloruse,
                                         LPCTSTR  string,
                                         int      chars = -1);
extern "C" BOOL APIENTRY SGdiGetTextExtent (LPCTSTR string,
                                            int     chars,
                                            LPSIZE  size);
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
extern "C" BOOL APIENTRY SGdiSetTargetDimensions (int width,
                                                  int height,
                                                  int bitdepth,
                                                  int pitch);
extern "C" BOOL APIENTRY SGdiTextOut (LPBYTE   videobuffer,
                                      int      x,
                                      int      y,
                                      COLORREF color,
                                      LPCTSTR  string,
                                      int      chars = -1);


/****************************************************************************
*
*  Logging functions
*
***/

DECLARE_STRICT_HANDLE(HSLOG);

extern "C" void APIENTRY SLogClose (HSLOG log);
extern "C" BOOL APIENTRY SLogCreate (LPCTSTR  filename,
                                     DWORD    flags,
                                     HSLOG   *log);
#ifdef STORMSTATIC
extern "C" void APIENTRY SLogDestroy ();
#endif
extern "C" void APIENTRY SLogDump (HSLOG   log,
                                   LPCVOID data,
                                   DWORD   bytes);
extern "C" void APIENTRY SLogFlush (HSLOG log);
extern "C" void APIENTRY SLogFlushAll ();
#ifdef STORMSTATIC
extern "C" void APIENTRY SLogInitialize ();
#endif
extern "C" void __cdecl  SLogPend (HSLOG   log,
                                   LPCTSTR format,
                                   ...);
extern "C" void __cdecl  SLogWrite (HSLOG   log,
                                    LPCTSTR format,
                                    ...);


/****************************************************************************
*
*  Memory allocation functions
*
***/

#define  SMEM_FLAG_ZEROMEMORY         0x00000008
#define  SMEM_FLAG_PRESERVEONDESTROY  0x08000000

DECLARE_STRICT_HANDLE(HSHEAP);

typedef struct _SMEMBLOCKDETAILS {
    DWORD  size;
    LPVOID ptr;
    BOOL   allocated;
    BOOL   valid;
    DWORD  bytes;
    DWORD  overhead;
} SMEMBLOCKDETAILS, *LPSMEMBLOCKDETAILS;

typedef struct _SMEMHEAPDETAILS {
    DWORD  size;
    HSHEAP handle;
    char   filename[MAX_PATH];
    int    linenumber;
    DWORD  regions;
    DWORD  committedbytes;
    DWORD  reservedbytes;
    DWORD  maximumsize;
    DWORD  allocatedblocks;
} SMEMHEAPDETAILS, *LPSMEMHEAPDETAILS;

extern "C" LPVOID APIENTRY SMemAlloc (DWORD  bytes,
                                      LPCSTR filename   = NULL,
                                      int    linenumber = 0,
                                      DWORD  flags      = 0);
#ifdef STORMSTATIC
extern "C" BOOL   APIENTRY SMemDestroy ();
#endif
extern "C" BOOL   APIENTRY SMemFindNextBlock (HSHEAP              heap,
                                              LPVOID              prevblock,
                                              LPVOID             *nextblock,
                                              LPSMEMBLOCKDETAILS  details);
extern "C" BOOL   APIENTRY SMemFindNextHeap (HSHEAP             prevheap,
                                             HSHEAP            *nextheap,
                                             LPSMEMHEAPDETAILS  details);
extern "C" BOOL   APIENTRY SMemFree (LPVOID ptr,
                                     LPCSTR filename   = NULL,
                                     int    linenumber = 0,
                                     DWORD  flags      = 0);
extern "C" HSHEAP APIENTRY SMemGetHeapByCaller (LPCSTR filename,
                                                int    linenumber);
extern "C" HSHEAP APIENTRY SMemGetHeapByPtr (LPVOID ptr);
extern "C" LPVOID APIENTRY SMemHeapAlloc (HSHEAP handle,
                                          DWORD  flags,
                                          DWORD  bytes);
extern "C" HSHEAP APIENTRY SMemHeapCreate (DWORD options,
                                           DWORD initialsize,
                                           DWORD maximumsize);
extern "C" BOOL   APIENTRY SMemHeapDestroy (HSHEAP handle);
extern "C" BOOL   APIENTRY SMemHeapFree (HSHEAP handle,
                                         DWORD  flags,
                                         LPVOID ptr);
#ifdef STORMSTATIC
extern "C" void   APIENTRY SMemInitialize ();
#endif

inline void __cdecl operator delete (void *ptr) {
  if (ptr)
    SMemFree(ptr,__FILE__,__LINE__,0);
}

inline void * __cdecl operator new (size_t bytes) {
  return SMemAlloc(bytes,__FILE__,__LINE__,0);
}

#ifndef __ICL
#ifndef __MWERKS__
inline void __cdecl operator delete[] (void *ptr) {
  if (ptr)
    SMemFree(ptr,__FILE__,__LINE__,0);
}

inline void * __cdecl operator new[] (size_t bytes) {
  return SMemAlloc(bytes,__FILE__,__LINE__,0);
}
#endif
#endif

#ifndef __PLACEMENT_NEW_INLINE
#define __PLACEMENT_NEW_INLINE
inline void * __cdecl operator new (size_t, void *ptr) {
  return (ptr);
}

inline void * __cdecl operator new[] (size_t, void *ptr) {
  return (ptr);
}
#endif

#define  ALLOC(bytes)      SMemAlloc(bytes,__FILE__,__LINE__,0)
#define  ALLOCZERO(bytes)  SMemAlloc(bytes,__FILE__,__LINE__,SMEM_FLAG_ZEROMEMORY)
#define  DEL(ptr)          delete(ptr)
#define  DELIFUSED(ptr)    delete(ptr)
#define  FREE(ptr)         SMemFree(ptr,__FILE__,__LINE__,0)
#define  FREEIFUSED(ptr)   do if (ptr) SMemFree(ptr,__FILE__,__LINE__,0); while (0)
#define  NEW(struct)       (new(SMemAlloc(sizeof(struct),__FILE__,__LINE__,0)) struct)
#define  NEWZERO(struct)   (new(SMemAlloc(sizeof(struct),__FILE__,__LINE__,SMEM_FLAG_ZEROMEMORY)) struct)


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

extern "C" BOOL APIENTRY SMsgBreakHandlerChain (LPPARAMS params);
#ifdef STORMSTATIC
extern "C" BOOL APIENTRY SMsgDestroy ();
#endif
extern "C" BOOL APIENTRY SMsgDispatchMessage (HWND     window,
                                              UINT     message,
                                              WPARAM   wparam,
                                              LPARAM   lparam,
                                              BOOL    *useresult,
                                              LRESULT *result);
extern "C" BOOL APIENTRY SMsgDoMessageLoop (SMSGIDLEPROC idleproc      = NULL,
                                            BOOL         cleanuponquit = TRUE);
extern "C" BOOL APIENTRY SMsgPopRegisterState (HWND window);
extern "C" BOOL APIENTRY SMsgPushRegisterState (HWND window);
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
extern "C" BOOL APIENTRY SMsgUnregisterCommand (HWND        window,
                                                UINT        id,
                                                SMSGHANDLER handler);
extern "C" BOOL APIENTRY SMsgUnregisterKeyDown (HWND        window,
                                                UINT        id,
                                                SMSGHANDLER handler);
extern "C" BOOL APIENTRY SMsgUnregisterKeyUp (HWND        window,
                                              UINT        id,
                                              SMSGHANDLER handler);
extern "C" BOOL APIENTRY SMsgUnregisterMessage (HWND        window,
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
#define  SNET_ART_COMBOLEFT               16
#define  SNET_ART_COMBOMIDDLE             17
#define  SNET_ART_COMBORIGHT              18

#define  SNET_AUTHTYPE_CHANNEL            1
#define  SNET_AUTHTYPE_GAME               2

#define  SNET_BROADCASTNONLOCALPLAYERID   0xFFFFFFFE
#define  SNET_BROADCASTPLAYERID           0xFFFFFFFF
#define  SNET_INVALIDPLAYERID             0xFFFFFFFF

#define  SNET_CAPS_PAGELOCKEDBUFFERS      0x00000001
#define  SNET_CAPS_BASICINTERFACE         0x00000002
#define  SNET_CAPS_DEBUGONLY              0x10000000
#define  SNET_CAPS_RETAILONLY             0x20000000
      
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

#define  SNET_ERROR_ALREADY_EXISTS        ERROR_ALREADY_EXISTS
#define  SNET_ERROR_BAD_PROVIDER          ERROR_BAD_PROVIDER
#define  SNET_ERROR_CANCELLED             ERROR_CANCELLED
#define  SNET_ERROR_INVALID_PARAMETER     ERROR_INVALID_PARAMETER
#define  SNET_ERROR_INVALID_PLAYER        STORM_ERROR_INVALID_PLAYER
#define  SNET_ERROR_GAME_ALREADY_STARTED  STORM_ERROR_GAME_ALREADY_STARTED
#define  SNET_ERROR_GAME_FULL             STORM_ERROR_GAME_FULL
#define  SNET_ERROR_GAME_NOT_FOUND        STORM_ERROR_GAME_NOT_FOUND
#define  SNET_ERROR_GAME_TERMINATED       STORM_ERROR_GAME_TERMINATED
#define  SNET_ERROR_HOST_UNREACHABLE      ERROR_HOST_UNREACHABLE
#define  SNET_ERROR_MAX_THRDS_REACHED     ERROR_MAX_THRDS_REACHED
#define  SNET_ERROR_NETWORK_BUSY          ERROR_NETWORK_BUSY
#define  SNET_ERROR_NO_MESSAGES_WAITING   STORM_ERROR_NO_MESSAGES_WAITING
#define  SNET_ERROR_NO_NETWORK            ERROR_NO_NETWORK
#define  SNET_ERROR_NOT_CONNECTED         ERROR_NOT_CONNECTED
#define  SNET_ERROR_NOT_ENOUGH_MEMORY     ERROR_NOT_ENOUGH_MEMORY
#define  SNET_ERROR_NOT_IMPLEMENTED       STORM_ERROR_NOT_IMPLEMENTED
#define  SNET_ERROR_NOT_IN_GAME           STORM_ERROR_NOT_IN_GAME
#define  SNET_ERROR_NOT_OWNER             ERROR_NOT_OWNER
#define  SNET_ERROR_NOT_REGISTERED        STORM_ERROR_NOT_REGISTERED
#define  SNET_ERROR_REQUIRES_UPGRADE      STORM_ERROR_REQUIRES_UPGRADE
#define  SNET_ERROR_STILL_ACTIVE          STORM_ERROR_STILL_ACTIVE
#define  SNET_ERROR_TOO_MANY_NAMES        ERROR_TOO_MANY_NAMES
#define  SNET_ERROR_VERSION_MISMATCH      STORM_ERROR_VERSION_MISMATCH

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
#define  SNET_GM_UNJOINABLE               0x00000008
#define  SNET_GM_UNLISTEDMASK             (SNET_GM_PRIVATE | SNET_GM_FULL | SNET_GM_UNJOINABLE)

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
    DWORD size;
    DWORD providerid;
    DWORD maxplayers;
    DWORD createflags;
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

typedef struct _SNETGAME {
    DWORD  size;
    DWORD  id;
    LPCSTR gamename;
    LPCSTR gamedescription;
    DWORD  categorybits;
    DWORD  numplayers;
    DWORD  maxplayers;
} SNETGAME, *SNETGAMEPTR;

struct _SNETPROGRAMDATA;
struct _SNETPLAYERDATA;
struct _SNETUIDATA;
struct _SNETVERSIONDATA;

typedef BOOL (CALLBACK *SNETABORTPROC        )();
typedef void (CALLBACK *SNETADDCATEGORYPROC  )(LPCSTR,DWORD,DWORD);
typedef void (CALLBACK *SNETCATEGORYLISTPROC )(_SNETPLAYERDATA *,SNETADDCATEGORYPROC);
typedef BOOL (CALLBACK *SNETCATEGORYPROC     )(BOOL,_SNETPROGRAMDATA *,_SNETPLAYERDATA *,_SNETUIDATA *,_SNETVERSIONDATA *,DWORD *,DWORD *);
typedef BOOL (CALLBACK *SNETCHECKAUTHPROC    )(DWORD,LPCSTR,LPCSTR,DWORD,LPCSTR,LPSTR,DWORD);
typedef BOOL (CALLBACK *SNETCREATEPROC       )(SNETCREATEDATAPTR,_SNETPROGRAMDATA *,_SNETPLAYERDATA *,_SNETUIDATA *,_SNETVERSIONDATA *,DWORD *);
typedef BOOL (CALLBACK *SNETDRAWDESCPROC     )(DWORD,DWORD,LPCSTR,LPCSTR,DWORD,DWORD,DWORD,LPDRAWITEMSTRUCT);
typedef BOOL (CALLBACK *SNETENUMDEVICESPROC  )(DWORD,LPCSTR,LPCSTR);
typedef BOOL (CALLBACK *SNETENUMGAMESEXPROC  )(SNETGAMEPTR);
typedef BOOL (CALLBACK *SNETENUMGAMESPROC    )(DWORD,LPCSTR,LPCSTR);
typedef BOOL (CALLBACK *SNETENUMPROVIDERSPROC)(DWORD,LPCSTR,LPCSTR,SNETCAPSPTR);
typedef void (CALLBACK *SNETEVENTPROC        )(SNETEVENTPTR);
typedef BOOL (CALLBACK *SNETGETARTPROC       )(DWORD,DWORD,LPPALETTEENTRY,LPBYTE,DWORD,int *,int *,int *);
typedef BOOL (CALLBACK *SNETGETDATAPROC      )(DWORD,DWORD,LPVOID,DWORD,DWORD *);
typedef int  (CALLBACK *SNETMESSAGEBOXPROC   )(HWND,LPCSTR,LPCSTR,UINT);
typedef BOOL (CALLBACK *SNETPLAYSOUNDPROC    )(DWORD,DWORD,DWORD);
typedef BOOL (CALLBACK *SNETSELECTEDPROC     )(DWORD,SNETCAPSPTR,_SNETUIDATA *,_SNETVERSIONDATA *);
typedef BOOL (CALLBACK *SNETSTATUSPROC       )(LPCSTR,DWORD,DWORD,DWORD,SNETABORTPROC);
typedef BOOL (CALLBACK *SNETPROFILEPROC      )(); // tbd -- include callback to save info
typedef BOOL (CALLBACK *SNETNEWACCOUNTPROC   )(); // tbd -- include callback to try to create

typedef struct _SNETPLAYERDATA {
    DWORD  size;
    LPCSTR playername;
    LPCSTR playerdescription;
    // new for 1.05:
    LPCSTR displayedfields;
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
    DWORD                size;
    DWORD                uiflags;
    HWND                 parentwindow;
    SNETGETARTPROC       artcallback;
    SNETCHECKAUTHPROC    authcallback;
    SNETCREATEPROC       createcallback;
    SNETDRAWDESCPROC     drawdesccallback;
    SNETSELECTEDPROC     selectedcallback;
    SNETMESSAGEBOXPROC   messageboxcallback;
    SNETPLAYSOUNDPROC    soundcallback;
    SNETSTATUSPROC       statuscallback;
    SNETGETDATAPROC      getdatacallback;
    SNETCATEGORYPROC     categorycallback;
    // new for 1.05:
    SNETCATEGORYLISTPROC categorylistcallback;
    SNETNEWACCOUNTPROC   newaccountcallback;
    SNETPROFILEPROC      profilecallback;
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
extern "C" BOOL APIENTRY SNetEnumGamesEx (DWORD                categorybits,
                                          DWORD                categorymask,
                                          SNETENUMGAMESEXPROC  callback,
                                          DWORD               *hintnextcall);
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
extern "C" BOOL APIENTRY SNetGetOwnerId (DWORD *playerid);
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
extern "C" BOOL APIENTRY SNetSendServerChatCommand (LPCSTR command);
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

#define  SNETSPI_MAXCLIENTDATA    256
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
    // new for 1.05:
    LPVOID             clientdata;
    DWORD              clientdatabytes;
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

/* note: this is the way that the Receive call should look...

    BOOL (CALLBACK *Receive)(SNETADDRPTR *,LPVOID *,DWORD *);

         below is the receive call with two parameters switched around
         to make it incompatible with older .snp's during the beta... */

    BOOL (CALLBACK *Receive)(LPVOID *,DWORD *,SNETADDRPTR *);

    BOOL (CALLBACK *ReceiveExternalMessage)(LPCSTR *,LPCSTR *,LPCSTR *);
    BOOL (CALLBACK *SelectGame)(DWORD,SNETPROGRAMDATAPTR,SNETPLAYERDATAPTR,SNETUIDATAPTR,SNETVERSIONDATAPTR,DWORD *);
    BOOL (CALLBACK *Send)(DWORD,SNETADDRPTR *,LPVOID,DWORD);
    BOOL (CALLBACK *SendExternalMessage)(LPCSTR,LPCSTR,LPCSTR,LPCSTR,LPCSTR);
    BOOL (CALLBACK *StartAdvertisingGame)(LPCSTR,LPCSTR,LPCSTR,DWORD,DWORD,DWORD,DWORD,LPCVOID,DWORD);
    BOOL (CALLBACK *StopAdvertisingGame)();
    BOOL (CALLBACK *UnlockDeviceList)(SNETSPI_DEVICELISTPTR);
    BOOL (CALLBACK *UnlockGameList)(SNETSPI_GAMELISTPTR,DWORD *);
    // new for 1.05:
    BOOL (CALLBACK *GetLocalPlayerName)(LPSTR,DWORD,LPSTR,DWORD);
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

extern "C" BOOL APIENTRY SRegGetBaseKey (DWORD flags,
                                         LPSTR buffer,
                                         DWORD buffersize);
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
*  Region manager functions
*
***/

#define  SRGN_AND        RGN_AND
#define  SRGN_COPY       RGN_COPY
#define  SRGN_DIFF       RGN_DIFF
#define  SRGN_OR         RGN_OR
#define  SRGN_XOR        RGN_XOR
#define  SRGN_PARAMONLY  6
#define  SRGN_MIN        1
#define  SRGN_MAX        6

DECLARE_STRICT_HANDLE(HSRGN);

extern "C" void APIENTRY SRgnClear (HSRGN handle);
extern "C" void APIENTRY SRgnCombineRect (HSRGN   handle,
                                          LPCRECT rect,
                                          LPVOID  param,
                                          int     combinemode);
extern "C" void APIENTRY SRgnCreate (HSRGN *handle,
                                     DWORD  reserved = 0);
extern "C" void APIENTRY SRgnDelete (HSRGN handle);
#ifdef STORMSTATIC
extern "C" void APIENTRY SRgnDestroy ();
#endif
extern "C" void APIENTRY SRgnDuplicate (HSRGN  orighandle,
                                        HSRGN *handle,
                                        DWORD  reserved = 0);
extern "C" void APIENTRY SRgnGetBoundingRect (HSRGN  handle,
                                              LPRECT rect);
extern "C" void APIENTRY SRgnGetRectParams (HSRGN    handle,
                                            LPCRECT  rect,
                                            DWORD   *numparams,
                                            LPVOID  *buffer);
extern "C" void APIENTRY SRgnGetRects (HSRGN   handle,
                                       DWORD  *numrects,
                                       LPRECT  buffer);

#define  SRgnAddParam(handle,rect,param)  SRgnCombineRect(handle,rect,param,SRGN_PARAMONLY);
#define  SRgnAddRect(handle,rect,param)   SRgnCombineRect(handle,rect,param,SRGN_OR)


/****************************************************************************
*
*  Run-time library functions
*
***/

#define  ONCE           ONCEEXPAND(__FILE__##__##__LINE__##__once)
#define  ONCEEXPAND(a)  BOOL a = TRUE; a; a = FALSE

#define  TRY            goto trylabel; trylabel:
#define  LEAVE          goto finallylabel
#define  FINALLY        goto finallylabel; finallylabel:


/****************************************************************************
*
*  String functions
*
***/

#define  SSTR_HASH_CASESENSITIVE  0x00000001

#define  SSTR_UNBOUNDED           0x7FFFFFFF

extern "C" LPTSTR APIENTRY SStrChr (LPCTSTR string,
                                    char    ch,
                                    BOOL    reverse = FALSE);
extern "C" DWORD  APIENTRY SStrCopy (LPTSTR  dest,
                                     LPCTSTR source,
                                     DWORD   destsize = SSTR_UNBOUNDED);
extern "C" DWORD  APIENTRY SStrHash (LPCTSTR string,
                                     DWORD   flags = 0,
                                     DWORD   seed  = 0);
extern "C" DWORD  APIENTRY SStrLen (LPCTSTR string);
extern "C" void   APIENTRY SStrPack (LPTSTR  dest,
                                     LPCTSTR source,
                                     DWORD   destsize = SSTR_UNBOUNDED);
extern "C" void   APIENTRY SStrTokenize (LPCTSTR *string,
                                         LPTSTR   buffer,
                                         DWORD    bufferchars,
                                         LPCTSTR  whitespace,
                                         BOOL    *quoted = NULL);


/****************************************************************************
*
*  Transparency functions
*
***/

#define  STRANS_CF_INTERSECT     0x00000001
#define  STRANS_CF_INVERTSECOND  0x00000002
#define  STRANS_CF_SUBTRACT      (STRANS_CF_INTERSECT | STRANS_CF_INVERTSECOND)

DECLARE_STRICT_HANDLE(HSTRANS);
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
extern "C" BOOL APIENTRY STransCombineMasks (HSTRANS  basemask,
                                             HSTRANS  secondmask,
                                             int      offsetx,
                                             int      offsety,
                                             DWORD    flags,
                                             HSTRANS *handle);
extern "C" BOOL APIENTRY STransCreateE (LPBYTE    bits,
                                        int       width,
                                        int       height,
                                        int       bitdepth,
                                        LPRECT    rect,
                                        COLORREF  colorkey,
                                        HSTRANS  *handle);
extern "C" BOOL APIENTRY STransCreateI (LPBYTE    bits,
                                        int       width,
                                        int       height,
                                        int       bitdepth,
                                        LPRECT    rect,
                                        COLORREF  colorkey,
                                        HSTRANS  *handle);
extern "C" BOOL APIENTRY STransCreateMaskE (LPBYTE    bits,
                                            int       width,
                                            int       height,
                                            int       bitdepth,
                                            LPRECT    rect,
                                            COLORREF  colorkey,
                                            HSTRANS  *handle);
extern "C" BOOL APIENTRY STransCreateMaskI (LPBYTE    bits,
                                            int       width,
                                            int       height,
                                            int       bitdepth,
                                            LPRECT    rect,
                                            COLORREF  colorkey,
                                            HSTRANS  *handle);
extern "C" BOOL APIENTRY STransDelete (HSTRANS handle);
#ifdef STORMSTATIC
extern "C" BOOL APIENTRY STransDestroy ();
#endif
extern "C" BOOL APIENTRY STransDuplicate (HSTRANS  source,
                                          HSTRANS *handle);
extern "C" BOOL APIENTRY STransIntersectDirtyArray (HSTRANS  sourcemask,
                                                    LPBYTE   dirtyarray,
                                                    BYTE     dirtyarraymask,
                                                    HSTRANS *handle);
extern "C" BOOL APIENTRY STransInvertMask (HSTRANS  sourcemask,
                                           HSTRANS *handle);
extern "C" BOOL APIENTRY STransIsPixelInMask (HSTRANS mask,
                                              int     offsetx,
                                              int     offsety);
extern "C" BOOL APIENTRY STransLoadE (LPCTSTR   filename,
                                      LPRECT    rect,
                                      COLORREF  colorkey,
                                      HSTRANS  *handle);
extern "C" BOOL APIENTRY STransLoadI (LPCTSTR   filename,
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

#ifdef   STRANS_USE_INCLUSIVE_RECTS
#define  STransCreate      STransCreateI
#define  STransCreateMask  STransCreateMaskI
#define  STransLoad        STransLoadI
#else
#define  STransCreate      STransCreateE
#define  STransCreateMask  STransCreateMaskE
#define  STransLoad        STransLoadE
#endif


/****************************************************************************
*
*  Video functions
*
***/

#define  SVID_FLAG_DOUBLESCANS            0x00000001
#define  SVID_FLAG_INTERPOLATE            0x00000002
#define  SVID_FLAG_INTERLACE              0x00000004
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
#define  SVID_FLAG_NOSKIP                 0x00400000
#define  SVID_FLAG_NEEDPAN                0x02000000
#define  SVID_FLAG_NEEDVOLUME             0x04000000
#define  SVID_FLAG_TOSCREEN               0x10000000
#define  SVID_FLAG_TOBUFFER               0x20000000

#define  SVID_CUTSCENE                    (SVID_FLAG_TOSCREEN | SVID_FLAG_FULLSCREEN | SVID_FLAG_CLEARSCREEN | SVID_FLAG_2XSIZE)
#define  SVID_AUTOCUTSCENE                (SVID_FLAG_TOSCREEN | SVID_FLAG_FULLSCREEN | SVID_FLAG_CLEARSCREEN | SVID_FLAG_AUTOSIZE | SVID_FLAG_AUTOQUALITY)

#define  SVID_QUALITY_LOW_SKIPSCANS       SVID_FLAG_2XSIZE
#define  SVID_QUALITY_LOW                 (SVID_FLAG_2XSIZE | SVID_FLAG_DOUBLESCANS)
#define  SVID_QUALITY_HIGH_SKIPSCANS      (SVID_FLAG_2XSIZE | SVID_FLAG_INTERPOLATE)
#define  SVID_QUALITY_HIGH                (SVID_FLAG_2XSIZE | SVID_FLAG_INTERPOLATE | SVID_FLAG_DOUBLESCANS)

DECLARE_STRICT_HANDLE(HSVIDEO);

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
extern "C" BOOL APIENTRY SVidSetVolume (HSVIDEO video,
                                        LONG    volume,
                                        LONG    pan,
                                        DWORD   track = 0);


/****************************************************************************
*
*  Storm global functions
*
***/

extern "C" BOOL APIENTRY StormDestroy ();

#ifdef MAC
extern "C" void StormStartup();
extern "C" void StormShutdown();
#endif


//#########################################################################//
//#########################################################################//
//                                                                         //
//                                                                         //
//                    CLASS-BASED PROGRAMMING INTERFACE                    //
//                          (under construction)                           //
//                                                                         //
//                                                                         //
//#########################################################################//
//#########################################################################//


/****************************************************************************
*
*  CSLog
*
***/

class CSLog {

  private:
    HSLOG m_handle;

  public:

    //=======================================================================
    CSLog (LPCTSTR filename) {
      SLogCreate(filename,0,&m_handle);
    }

    //=======================================================================
    CSLog (LPCTSTR keyname,
           LPCTSTR valuename) {
      char filename[MAX_PATH] = "";
      SRegLoadString(keyname,valuename,0,filename,MAX_PATH);
#ifdef _DEBUG
      SRegSaveString(keyname,valuename,0,filename);
#endif
      if (filename[0])
        SLogCreate(filename,0,&m_handle);
    }

    //=======================================================================
    ~CSLog () {
      SLogClose(m_handle);
    }

    //=======================================================================
    void Dump (LPCVOID data,
               DWORD   bytes) {
      SLogDump(m_handle,
               data,
               bytes);
    }

    //=======================================================================
    void Flush () {
      SLogFlush(m_handle);
    }

    //=======================================================================
    HSLOG GetHandle () {
      return m_handle;
    }

    //=======================================================================
    void Pend (LPCSTR string) {
      SLogPend(m_handle,
               string);
    }

    //=======================================================================
    void Write (LPCSTR string) {
      SLogWrite(m_handle,
                string);
    }

};


/****************************************************************************
*
*  CSRgn
*
***/

class CSRgn {

  private:
    HSRGN m_handle;

    //=======================================================================
    void CopyConstructor (const CSRgn & source) {
      SRgnDuplicate(source.m_handle,
                    &m_handle);
    }

  public:

    //=======================================================================
    CSRgn () {
      SRgnCreate(&m_handle);
    }

    //=======================================================================
    CSRgn (const CSRgn & source) {
      CopyConstructor(source);
    }

    //=======================================================================
    ~CSRgn () {
      SRgnDelete(m_handle);
    }

    //=======================================================================
    CSRgn & operator= (const CSRgn &source) {
      if (this != &source) {
        this->~CSRgn();
        CopyConstructor(source);
      }
      return *this;
    }

    //=======================================================================
    void AddParam (LPCRECT rect,
                   LPVOID  param) {
      SRgnAddParam(m_handle,
                   rect,
                   param);
    }

    //=======================================================================
    void AddRect (LPCRECT rect,
                  LPVOID  param) {
      SRgnAddRect(m_handle,
                  rect,
                  param);
    }

    //=======================================================================
    void Clear () {
      SRgnClear(m_handle);
    }

    //=======================================================================
    void CombineRect (LPCRECT rect,
                      LPVOID  param,
                      int     combinemode) {
      SRgnCombineRect(m_handle,
                      rect,
                      param,
                      combinemode);
    }

    //=======================================================================
    void GetBoundingRect (LPRECT rect) {
      SRgnGetBoundingRect(m_handle,
                          rect);
    }

    //=======================================================================
    void GetRects (DWORD  *numrects,
                   LPRECT  buffer) {
      SRgnGetRects(m_handle,
                   numrects,
                   buffer);
    }

    //=======================================================================
    void GetRectParams (LPCRECT  rect,
                        DWORD   *numparams,
                        LPVOID  *buffer) {
      SRgnGetRectParams(m_handle,
                        rect,
                        numparams,
                        buffer);
    }

};


//#########################################################################//
//#########################################################################//
//                                                                         //
//                                                                         //
//                      UTILITY CLASSES AND TEMPLATES                      //
//                                                                         //
//                                                                         //
//#########################################################################//
//#########################################################################//


/****************************************************************************
*
*  CCritSect -- Critical section class
*
*  Methods:
*
*    void Enter ()
*    void Leave ()
*
***/

class CCritSect {
  private:
    CRITICAL_SECTION m_critsect;
  public:
    CCritSect () { InitializeCriticalSection(&m_critsect); }
    ~CCritSect () { DeleteCriticalSection(&m_critsect); }
    void Enter () { EnterCriticalSection(&m_critsect); }
    void Enter (BOOL) { EnterCriticalSection(&m_critsect); }
    void Leave () { LeaveCriticalSection(&m_critsect); }
    void Leave (BOOL) { LeaveCriticalSection(&m_critsect); }
};


/****************************************************************************
*
*  CLock -- Reader/writer lock class
*
*  Methods:
*
*    void Enter (BOOL forwriting)
*    void Leave (BOOL fromwriting)
*
***/

class CLock {

  private:

    HANDLE m_mutexevent;
    HANDLE m_readerevent;
    LONG   m_readercount;

  public:

    //=======================================================================
    CLock () {
      m_mutexevent  = CreateEvent(NULL,FALSE,TRUE,NULL);
      m_readerevent = CreateEvent(NULL,TRUE,FALSE,NULL);
      m_readercount = -1;
    }

    //=======================================================================
    ~CLock () {
      CloseHandle(m_readerevent);
      CloseHandle(m_mutexevent);
    }

    //=======================================================================
    void Enter (BOOL forwriting) {
      if (forwriting)
        WaitForSingleObject(m_mutexevent,INFINITE);
      else if (!InterlockedIncrement(&m_readercount)) {
        WaitForSingleObject(m_mutexevent,INFINITE);
        SetEvent(m_readerevent);
      }
      else
        WaitForSingleObject(m_readerevent,INFINITE);
    }

    //=======================================================================
    void Leave (BOOL fromwriting) {
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
*  CNullSync -- Null synchronization class
*
*  (used for templates that take a synchronization class as a parameter)
*
***/

class CNullSync {
  public:
    void Enter (BOOL) { }
    void Leave (BOOL) { }
};


/****************************************************************************
*
*  type_info
*
*  (used by templates to obtain the name of an object)
*
***/

#ifdef _MSC_VER
  #ifdef _INC_TYPEINFO
    #define  INTERNALRAWNAME  raw_name
  #else
    #define  INTERNALRAWNAME  internal_raw_name
    class type_info {
      public:
        virtual ~type_info ();
        const char * internal_raw_name () const { return _m_d_name; };
      private:
        void *_m_data;
        char _m_d_name[1];
        type_info (const type_info& rhs);
        type_info& operator= (const type_info& rhs);
    };
  #endif
#else
  #if defined(MAC) && !defined(__typeinfo__)
     #include <typeinfo>
  #endif
  #define  INTERNALRAWNAME  name
#endif


/****************************************************************************
*
*  ARRAY -- Dynamically allocated array template
*
*  Types:
*
*    ARRAY(structname) -- dynamically sized array of struct
*
*  Pointers to types:
*
*    ARRAYPTR(structname)
*
*  Array methods:
*
*    void       AddDiscontiguousElements (DWORD           count,
*                                         int             stride,
*                                         const <struct> *newelements);
*    void       AddElement (const <struct> *newelement);
*    void       AddElements (DWORD           count,
*                            const <struct> *newelements);
*    DWORD      NumElements ();
*    <struct> * NewElement ();
*    <struct> * Ptr ();
*    void       ReserveSpace (DWORD count);
*    void       SetNumElements (DWORD totalcount);
*
***/

template<class T>
class TSArray {

  private:
    DWORD  m_allocchunksize;
    T     *m_data;
    DWORD  m_elements;
    DWORD  m_elementsalloc;

    //=======================================================================
    BOOL CheckSpace (DWORD count) {
      return (m_elements+count <= m_elementsalloc);
    }

    //=======================================================================
    void Constructor () {
      m_allocchunksize = max(16,256/sizeof(T));
      m_data           = NULL;
      m_elements       = 0;
      m_elementsalloc  = 0;
    }

    //=======================================================================
    void CopyConstructor (const TSArray & source) {
      Constructor();
      m_allocchunksize = source.m_allocchunksize;
      ReserveSpace(source.m_elementsalloc);
      AddElements(source.m_elements,
                  source.m_data);
    }

  public:

    //=======================================================================
    TSArray () {
      Constructor();
    }

    //=======================================================================
    TSArray (const TSArray & source) {
      CopyConstructor(source);
    }

    //=======================================================================
    ~TSArray () {
      if (m_data) {
        while (m_elements) {
          --m_elements;
          m_data[m_elements].~T();
        }
        SMemFree(m_data,__FILE__,__LINE__,0);
        m_data = NULL;
      }
    }

    //=======================================================================
    TSArray & operator= (const TSArray &source) {
      if (this != &source) {
        this->~TSArray();
        CopyConstructor(source);
      }
      return *this;
    }

    //=======================================================================
    T & operator[] (DWORD num) {
      if (num >= m_elements)
        SErrDisplayError(STORM_ERROR_ACCESS_OUT_OF_BOUNDS,
                         typeid(T).INTERNALRAWNAME(),
                         SERR_LINECODE_OBJECT,
                         NULL,
                         TRUE);
      return m_data[num];
    }

    //=======================================================================
    void AddDiscontiguousElements (DWORD    count,
                                   int      stride,
                                   const T *newelements) {
      if (!CheckSpace(count))
        ReserveSpace(count);
      if (stride == sizeof(T))
        AddElements(count,newelements);
      else
        for (DWORD loop = 0; loop < count; ++loop) {
          new(m_data+m_elements++) T(*newelements);
          newelements = (const T *)((LPBYTE)newelements+stride);
        }
    }

    //=======================================================================
    void AddElement (const T *newelement) {
      if (!CheckSpace(1))
        ReserveSpace(1);
      new(m_data+m_elements++) T(*newelement);
    }

    //=======================================================================
    void AddElements (DWORD    count,
                      const T *newelements) {
      if (!CheckSpace(count))
        ReserveSpace(count);
      for (DWORD loop = 0; loop < count; ++loop)
        new(m_data+m_elements++) T(newelements[loop]);
    }

    //=======================================================================
    DWORD NumElements () {
      return m_elements;
    }

    //=======================================================================
    T * NewElement () {
      if (!CheckSpace(1))
        ReserveSpace(1);
      return new(m_data+m_elements++) T;
    }

    //=======================================================================
    T * Ptr () {
      if (!m_data)
        ReserveSpace(1);
      return m_data;
    }

    //=======================================================================
    void ReserveSpace (DWORD count) {
      if (CheckSpace(count))
        return;

      // DETERMINE THE NUMBER OF NEW ELEMENTS TO ALLOCATE
      DWORD newelements  = m_elements+count;
      DWORD partialchunk = newelements & (m_allocchunksize-1);
      if (partialchunk)
        newelements += m_allocchunksize-partialchunk;

      // ALLOCATE THE NEW ARRAY AND COPY DATA FROM THE OLD ARRAY
      T *newdata = (T *)ALLOC(newelements*sizeof(T));
      if (m_data) {
        for (DWORD loop = 0; loop < m_elements; ++loop)
          new(newdata+loop) T(m_data[loop]);
        FREE(m_data);
      }
      m_data          = newdata;
      m_elementsalloc = newelements;

    }

    //=======================================================================
    void SetNumElements (DWORD totalcount) {
      if (totalcount > m_elements) {
        ReserveSpace(totalcount-m_elements);
        for (DWORD loop = m_elements; loop < totalcount; ++loop)
          new(m_data+loop) T;
      }
      else if (totalcount < m_elements) {
        for (DWORD loop = totalcount; loop < m_elements; ++loop)
          m_data[loop].~T();
      }
      m_elements = totalcount;
    }

};

#define  ARRAY(structname)              TSArray< structname >
#define  ARRAYDECL(structname,varname)  TSArray< structname > varname
#define  ARRAYPTR(structname)           TSArray< structname > *


/****************************************************************************
*
*  NODE/LIST -- Linked list template
*
*  Types:
*
*    LINKEX(structname)          -- explicit link field
*    LIST(structname)            -- linked list of implicitly linked nodes
*    LISTEX(structname,linkname) -- linked list of explicitly linked nodes
*    LISTEXDYN(structname)       -- linked list of explicitly linked nodes,
*                                     where the link field to be used by
*                                     this list is not known at compile time
*    NODEDECL(structname)        -- implicitly linked node
*    NODEDECLEX(structname)      -- explicitly linked node (must contain one
*                                     or more LINKEX fields)
*
*  Pointers to types:
*
*    LISTPTR(structname)
*    LISTPTREX(structname)
*
*  Link methods:
*
*    <struct> * Next ();
*    <struct> * Prev ();
*    void       Unlink ();
*
*  Explicitly linked node methods:
*
*    None.  Use link methods for the link you want to manipulate.
*
*  Implicitly linked node methods:
*
*    <struct> * Next ();
*    <struct> * Prev ();
*    void       Unlink ();
*
*  List methods:
*
*    void       Clear ();
*    <struct> * DeleteNode (<struct> *ptr);
*    <struct> * Head () const;
*    BOOL       IsEmpty () const;
*    void       LinkNode (<struct> *ptr,
*                         DWORD     linktype    = LIST_TAIL,
*                         <struct> *existingptr = NULL);
*    <struct> * NewNode (DWORD location   = LIST_TAIL,
*                        DWORD extrabytes = 0,
*                        DWORD flags      = 0);
*    <struct> * Next (const <struct> *ptr) const;
*    <struct> * Prev (const <struct> *ptr) const;
*    <struct> * Tail () const;
*    void       UnlinkAll ();
*    void       UnlinkNode (<struct> *ptr);
*
*  Constants for use with LinkNode() and NewNode():
*
*    LIST_UNLINKED
*    LIST_LINK_AFTER
*    LIST_LINK_BEFORE
*    LIST_HEAD
*    LIST_TAIL
*
*  Macros:
*
*    LISTEXSETLINK(structname,listname,linkname)
*    ITERATELIST(structname,listname,ptrname)
*    ITERATELISTPTR(structname,listname,ptrname)
*    ITERATEPARTIALLIST(structname,listname,start,ptrname)
*    ITERATEPARTIALLISTPTR(structname,listname,start,ptrname)
*    ITERATELISTREVERSE(structname,listname,ptrname)
*    ITERATELISTREVERSEPTR(structname,listname,ptrname)
*    ITERATEPARTIALLISTREVERSE(structname,listname,start,ptrname)
*    ITERATEPARTIALLISTREVERSEPTR(structname,listname,start,ptrname)
*    ITERATE_DELETE
*    ITERATE_DELETEANDBREAK
*
***/

#define  LIST_UNLINKED     0
#define  LIST_LINK_AFTER   1
#define  LIST_LINK_BEFORE  2
#define  LIST_HEAD         LIST_LINK_AFTER
#define  LIST_TAIL         LIST_LINK_BEFORE

template<class T, BOOL explicitlink>
class TSList;

template<class T>
class TSLink {
  friend class TSList<T,FALSE>;
  friend class TSList<T,TRUE>;

  private:
    TSLink *m_prevlink;
    T      *m_next;

    //=======================================================================
    void Constructor () {
      m_prevlink = NULL;
      m_next     = NULL;
    }

    //=======================================================================
    void CopyConstructor (const TSLink &) {
      Constructor();
    }

    //=======================================================================
    TSLink *NextLink () const {

      // IF THE NEXT NODE IS A TERMINATOR, ITS LINK POINTER IS THE SAME AS
      // ITS NODE POINTER.
      if ((int)m_next < 0)
        return (TSLink *)~(DWORD)m_next;

      // OTHERWISE, COMPUTE THE LINK ADDRESS BY USING THE OFFSET OF THIS
      // NODE'S LINK AND POINTER ADDRESSES.  (THIS NODE MUST NOT BE A
      // TERMINATOR.)
      else {
        DWORD linkoffset = (DWORD)this-(DWORD)(m_prevlink->m_next);
        return (TSLink *)(linkoffset+(DWORD)m_next);
      }

    }

  protected:

    //=======================================================================
    T *NextThroughTerminator () const {
      return ((int)m_next > 0) ? m_next : (T *)~(DWORD)m_next;
    }

  public:

    //=======================================================================
    TSLink () {
      Constructor();
    }

    //=======================================================================
    TSLink (const TSLink & source) {
      CopyConstructor(source);
    }

    //=======================================================================
    ~TSLink () {
      Unlink();
    }

    //=======================================================================
    TSLink & operator= (const TSLink &) {
      // LEAVE THE DESTINATION NODE LINKED INTO ITS CURRENT LIST
      return *this;
    }

    //=======================================================================
    T * Next () const {
      return ((int)m_next > 0) ? m_next : NULL;
    }

    //=======================================================================
    T * Prev () const {
      return m_prevlink->m_prevlink->Next();
    }

    //=======================================================================
    void Unlink () {
      if (!m_prevlink)
        return;
      NextLink()->m_prevlink = m_prevlink;
      m_prevlink->m_next     = m_next;
      m_prevlink = NULL;
      m_next     = NULL;
    }

};

template<class T>
class TSBaseNode {
  public:

    //=======================================================================
    inline void * __cdecl operator new (size_t bytes,
                                        size_t extra,
                                        DWORD  flags) {
      void *ptr = SMemAlloc(bytes+extra,
                            typeid(T).INTERNALRAWNAME(),
                            SERR_LINECODE_OBJECT,
                            flags | SMEM_FLAG_ZEROMEMORY);
      return ptr;
    }

};

template<class T>
class TSExplicitNode : public TSBaseNode<T> {
  friend class TSList<T,TRUE>;

  private:

    //=======================================================================
    TSLink<T> *Link (BOOL explicitlink, int linkoffset) const {
      ASSERT(explicitlink);
      explicitlink;
      return (TSLink<T> *)((LPBYTE)this+linkoffset);
    }

};

template<class T>
class TSLinkedNode : public TSBaseNode<T> {
  friend class TSList<T,FALSE>;

  private:
    TSLink<T> m_link;

    //=======================================================================
    TSLink<T> *Link (BOOL explicitlink, int linkoffset) const {
      if (explicitlink)
        return (TSLink<T> *)((LPBYTE)this+linkoffset);
      else
        return (TSLink<T> *)&m_link;
    }

  public:

    //=======================================================================
    ~TSLinkedNode () {
      Unlink();
    }

    //=======================================================================
    T * Next () const {
      return m_link.Next();
    }

    //=======================================================================
    T * Prev () const {
      return m_link.Prev();
    }

    //=======================================================================
    void Unlink () {
      m_link.Unlink();
    }

};

template<class T, BOOL explicitlink>
class TSList {

  private:
    int       m_linkoffset;
    TSLink<T> m_terminator;

    //=======================================================================
    void Constructor () {
      m_linkoffset = 0;
      InitializeTerminator();
    }

    //=======================================================================
    void CopyConstructor (const TSList & source) {
      m_linkoffset = source.m_linkoffset;
      InitializeTerminator();
    }

    //=======================================================================
    void InitializeTerminator () {
      m_terminator.m_prevlink = &m_terminator;
      m_terminator.m_next     = (T *)~(DWORD)&m_terminator;
    }

    //=======================================================================
    TSLink<T> *Link (const T *ptr) const {
      // THIS FUNCTION CALLS THE LINK METHOD IN EITHER THE NODE OR THE LINK
      // TO WHICH THIS LIST REFERS.  IF THIS FUNCTION WON'T COMPILE, IT'S
      // BECAUSE A NODE WITH EXPLICIT LINKS WAS DEFINED WITH NODEDECL()
      // INSTEAD OF NODEDECLEX().
      return ptr->Link(explicitlink,m_linkoffset);
    }

  protected:

    //=======================================================================
    void SetLinkOffset (int linkoffset) {
      m_linkoffset = linkoffset;
      InitializeTerminator();
    }

  public:

    //=======================================================================
    TSList () {
      Constructor();
    };

    //=======================================================================
    TSList (const TSList &source) {
      CopyConstructor(source);
    };

    //=======================================================================
    TSList (int linkoffset) {
      m_linkoffset = linkoffset;
      InitializeTerminator();
    };

    //=======================================================================
    ~TSList () {
      UnlinkAll();
    }

    //=======================================================================
    TSList & operator= (const TSList &source) {
      if (this != &source) {
        this->~TSList();
        CopyConstructor(source);
      }
      return *this;
    }

    //=======================================================================
    void ChangeLinkOffset (int linkoffset) {
      UnlinkAll();
      SetLinkOffset(linkoffset);
    }

    //=======================================================================
    void Clear () {
      T *curr;
      while ((curr = Head()) != NULL)
        delete curr;
    }

    //=======================================================================
    T * DeleteNode (T *ptr) {
      T *nextptr = Next(ptr);
      delete ptr;
      return nextptr;
    }

    //=======================================================================
    T * Head () const {
      return m_terminator.Next();
    }

    //=======================================================================
    BOOL IsEmpty () const {
      return !m_terminator.Next();
    }

    //=======================================================================
    T * Iterate_RawNext (const T *ptr) const {
      return Link(ptr)->m_next;
    }

    //=======================================================================
    void LinkNode (T     *ptr,
                   DWORD  linktype    = LIST_TAIL,
                   T     *existingptr = NULL) {
      TSLink<T> *link = Link(ptr);

      // IF THIS NODE IS ALREADY LINKED INTO THE LIST, UNLINK IT
      if (link->m_prevlink)
        link->Unlink();

      // FIND THE NODE THAT WE WILL LINK BEFORE OR AFTER.  USE THE
      // TERMINATOR NODE IF WE'RE LINKING ONTO THE HEAD OR TAIL.
      TSLink<T> *existinglink;
      if (existingptr)
        existinglink = Link(existingptr);
      else
        existinglink = &m_terminator;

      // LINK THIS NODE INTO THE LIST
      switch (linktype) {

        case LIST_LINK_AFTER:
          {
            link->m_prevlink = existinglink;
            link->m_next     = existinglink->m_next;
            Link(existinglink->NextThroughTerminator())->m_prevlink = link;
            existinglink->m_next = ptr;
          }
        break;

        case LIST_LINK_BEFORE:
          {
            TSLink<T> *prevlink = existinglink->m_prevlink;
            link->m_prevlink = prevlink;
            link->m_next     = prevlink->m_next;
            prevlink->m_next = ptr;
            existinglink->m_prevlink = link;
          }
        break;

      }
    }

    //=======================================================================
    T * NewNode (DWORD location   = LIST_TAIL,
                 DWORD extrabytes = 0,
                 DWORD flags      = 0) {
      T *ptr = new(extrabytes,flags) T;
      if (location != LIST_UNLINKED)
        LinkNode(ptr,location);
      return ptr;
    }

    //=======================================================================
    T * Next (const T *ptr) const {
      return Link(ptr)->Next();
    }

    //=======================================================================
    T * Prev (const T *ptr) const {
      return Link(ptr)->Prev();
    }

    //=======================================================================
    T * Tail () const {
      return m_terminator.Prev();
    }

    //=======================================================================
    void UnlinkAll () {
      T *curr;
      while ((curr = Head()) != NULL)
        UnlinkNode(curr);
    }

    //=======================================================================
    void UnlinkNode (T *ptr) {
      Link(ptr)->Unlink();
    }

};

template<class T, int linkoffset>
class TSExplicitList : public TSList<T,TRUE> {
  public:

    //=======================================================================
    TSExplicitList () {
      SetLinkOffset(linkoffset);
    }

};


#define  LINKEX(structname)              TSLink< structname >
#define  LINKDECLEX(structname,varname)  TSLink< structname > varname
#define  LIST(structname)                TSList< structname ,FALSE>
#define  LISTDECL(structname,varname)    TSList< structname ,FALSE> varname
#define  LISTPTR(structname)             TSList< structname ,FALSE> *
#define  LISTPTREX(structname)           TSList< structname ,TRUE> *
#define  NODEDECL(structname)            typedef struct structname : public TSLinkedNode< structname >
#define  NODEDECLEX(structname)          typedef struct structname : public TSExplicitNode< structname >

#define  LISTEX(structname,linkname)                                     \
  TSExplicitList< structname ,(int)&(((structname *)0)->linkname)>

#define  LISTEXDYN(structname)                                           \
  TSExplicitList< structname ,(int)0xDDDDDDDD>

#define  LISTEXSETLINK(structname,listname,linkname)                     \
  listname.ChangeLinkOffset((int)&(((structname *)0)->linkname));

#define  LISTDECLEX(structname,linkname,varname)                         \
  TSExplicitList< structname ,(int)&(((structname *)0)->linkname)> varname

#define  ITERATEFORWARDTEMPLATE(structname,listname,start,ptrname,op)    \
  for (structname *ptrname = start,                                      \
         *iterate_delete = NULL;                                         \
       (int)ptrname > 0;                                                 \
       iterate_delete                                                    \
         ? (ptrname = (listname)##op##DeleteNode(ptrname),               \
            ptrname = ((int)iterate_delete > 0) ? ptrname : NULL,        \
            iterate_delete = NULL,                                       \
            ptrname)                                                     \
         : ptrname = (listname)##op##Iterate_RawNext(ptrname))

#define  ITERATEREVERSETEMPLATE(structname,listname,start,ptrname,op)    \
  for (structname *ptrname = start,                                      \
         *iterate_delete = NULL,                                         \
         *iterate_delete_temp = NULL;                                    \
       ptrname;                                                          \
       iterate_delete                                                    \
         ? (iterate_delete_temp = ((int)iterate_delete > 0)              \
              ? (listname)##op##Prev(ptrname)                            \
              : NULL,                                                    \
            (listname)##op##DeleteNode(ptrname),                         \
            iterate_delete = NULL,                                       \
            ptrname        = iterate_delete_temp)                        \
         : ptrname = (listname)##op##Prev(ptrname))

#define  ITERATELIST(structname,listname,ptrname)                        \
  ITERATEFORWARDTEMPLATE(structname,listname,(listname).Head(),ptrname,.)

#define  ITERATELISTPTR(structname,listname,ptrname)                     \
  ITERATEFORWARDTEMPLATE(structname,listname,(listname)->Head(),ptrname,->)

#define  ITERATEPARTIALLIST(structname,listname,start,ptrname)           \
  ITERATEFORWARDTEMPLATE(structname,listname,start,ptrname,.)

#define  ITERATEPARTIALLISTPTR(structname,listname,start,ptrname)        \
  ITERATEFORWARDTEMPLATE(structname,listname,start,ptrname,->)

#define  ITERATELISTREVERSE(structname,listname,ptrname)                 \
  ITERATEREVERSETEMPLATE(structname,listname,(listname).Tail(),ptrname,.)

#define  ITERATELISTREVERSEPTR(structname,listname,ptrname)              \
  ITERATEREVERSETEMPLATE(structname,listname,(listname)->Tail(),ptrname,->)

#define  ITERATEPARTIALLISTREVERSE(structname,listname,start,ptrname)    \
  ITERATEREVERSETEMPLATE(structname,listname,start,ptrname,.)

#define  ITERATEPARTIALLISTREVERSEPTR(structname,listname,start,ptrname) \
  ITERATEREVERSETEMPLATE(structname,listname,start,ptrname,->)

#define  ITERATE_DELETE                                                  \
  {                                                                      \
    ++iterate_delete;                                                    \
    continue;                                                            \
  }

#define  ITERATE_DELETEANDBREAK                                          \
  {                                                                      \
    --iterate_delete;                                                    \
    continue;                                                            \
  }


/****************************************************************************
*
*  EXPORTOBJECT/EXPORTTABLE -- Export manager template
*
*  Types:
*
*    DECLARE_STRICT_HANDLE(handle)       -- handle to object
*    DECLARE_STRICT_HANDLE(lockedhandle) -- handle to locked object
*    EXPORTOBJECTDECL(structname)        -- object to be exported
*    EXPORTTABLE(structname,handlename,lockedhandlename,synctype)
*    EXPORTTABLEREUSE(structname,handlename,lockedhandlename,synctype)
*
*  Export table methods:
*
*    void       Delete (<handle> handle);
*    void       DeleteUnlock (<struct>       *ptr,
*                             <lockedhandle>  lockedhandle);
*    void       Destroy ();
*    <struct> * Lock (<handle>        handle,
*                     <lockedhandle> *lockedhandle,
*                     BOOL            forwriting = FALSE);
*    void       New (<handle> *handle);
*    <struct> * NewLock (<handle>       *handle,
*                        <lockedhandle> *lockedhandle);
*    void       Unlock (<lockedhandle> lockedhandle);
*
*  Synchronization types for use with EXPORTTABLE():
*
*    SYNC_NONE      -- use no synchronization
*    SYNC_READWRITE -- use a reader/writer lock
*    SYNC_ALWAYS    -- use a critical section
*
***/

template <class T>
class TSExportTableBase {

  protected:

    //=======================================================================
    T * BaseFindByHandle (LISTPTREX(T) list, unsigned handle) {
      ITERATELISTPTR(T,list,curr)
        if (curr->m_handle == handle)
          return curr;
      return NULL;
    }

    //=======================================================================
    T * BaseFindByHandleEx (LISTPTREX(T) list, unsigned handle, unsigned *count) {
      *count = 0;
      ITERATELISTPTR(T,list,curr)
        if (curr->m_handle == handle)
          return curr;
        else
          ++*count;
      return NULL;
    }

    //=======================================================================
    unsigned BaseGetHandle (const T *ptr) {
      return ptr->m_handle;
    }

    //=======================================================================
    int BaseGetLinkOffset () {
      return (int)&(((T *)0)->m_linktoslot);
    }

    //=======================================================================
    void BaseSetHandle (T *ptr, unsigned handle) {
      ptr->m_handle = handle;
    }

};

template <class T, class H, class LH, class SYNC, BOOL REUSE>
class TSExportTable : public TSExportTableBase<T> {

  private:
    ARRAY(LISTEXDYN(T))  m_listarray;
    LISTEXDYN(T)         m_reuselist;
    H                    m_sequence;
    unsigned             m_slotmask;
    SYNC                 m_sync;

    //=======================================================================
    unsigned ComputeSlot (H handle) {
      return (unsigned)handle & m_slotmask;
    }

    //=======================================================================
    H GenerateUniqueHandle () {
      unsigned count;
      for (;;) {
        m_sequence = (H)((unsigned)m_sequence+1);
        if (!BaseFindByHandleEx(&m_listarray[ComputeSlot(m_sequence)],
                                (unsigned)m_sequence,
                                &count))
          break;
      }
      if (count >= 4)
        GrowListArray();
      return m_sequence;
    }

    //=======================================================================
    void GrowListArray () {
return; // note: out for testing
      if (m_slotmask >= 1023)
        return;

      // DETERMINE THE NEW ARRAY SIZE
      unsigned oldarraysize = m_slotmask+1;
      unsigned newarraysize = oldarraysize*2;

      // GROW THE ARRAY
      {
        m_listarray.SetNumElements(newarraysize);
        int linkoffset = BaseGetLinkOffset();
        for (unsigned slot = oldarraysize; slot < newarraysize; ++slot)
          m_listarray[slot].ChangeLinkOffset(linkoffset);
      }

      // MOVE ALL RECORDS FROM THE OLD LISTS TO THE NEW LISTS
      m_slotmask = newarraysize-1;
      {
        for (unsigned slot = 0; slot < oldarraysize; ++slot) {
          T *currptr = m_listarray[slot].Head();
          while (currptr) {
            T *nextptr = m_listarray[slot].Next(currptr);
            unsigned newslot = ComputeSlot((H)BaseGetHandle(currptr));
            if (newslot != slot) {
              m_listarray[slot].UnlinkNode(currptr);
              m_listarray[newslot].LinkNode(currptr);
            }
            currptr = nextptr;
          }
        }
      }

    }

    //=======================================================================
    BOOL IsForWriting (LH lockedhandle) {
      return (lockedhandle == (LH)1);
    }

    //=======================================================================
    void SyncEnterLock (LH *lockedhandle, BOOL forwriting) {
      m_sync.Enter(forwriting);
      *lockedhandle = (LH)(forwriting ? 1 : -1);
    }

    //=======================================================================
    void SyncLeaveLock (LH lockedhandle) {
      if (lockedhandle)
        m_sync.Leave(IsForWriting(lockedhandle));
    }

  public:

    //=======================================================================
    TSExportTable () {
      m_sequence = (H)0;
      m_slotmask = 3;
      m_listarray.SetNumElements(m_slotmask+1);
      int linkoffset = BaseGetLinkOffset();
      for (unsigned slot = 0; slot <= m_slotmask; ++slot)
        m_listarray[slot].ChangeLinkOffset(linkoffset);
      m_reuselist.ChangeLinkOffset(linkoffset);
    }

    //=======================================================================
    ~TSExportTable () {
      Destroy();
    }

    //=======================================================================
    void Delete (H handle) {
      LH lockedhandle;
      T *ptr = Lock(handle,&lockedhandle,TRUE);
      DeleteUnlock(ptr,lockedhandle);
    }

    //=======================================================================
    void DeleteUnlock (T  *ptr,
                       LH  lockedhandle) {
      if (ptr)
        if (REUSE)
          m_reuselist.LinkNode(ptr);
        else
          delete ptr;
      Unlock(lockedhandle);
    }

    //=======================================================================
    void Destroy () {
      LH lockedhandle;
      SyncEnterLock(&lockedhandle,TRUE);
      for (unsigned slot = 0; slot <= m_slotmask; ++slot) {
        T *curr;
        while ((curr = m_listarray[slot].Head()) != NULL) {
          delete curr;
          SErrReportResourceLeak(typeid(H).INTERNALRAWNAME());
        }
      }
      m_reuselist.Clear();
      SyncLeaveLock(lockedhandle);
    }

    //=======================================================================
    T * Lock (H     handle,
              LH   *lockedhandle,
              BOOL  forwriting = FALSE) {
      SyncEnterLock(lockedhandle,forwriting);
      T *result = BaseFindByHandle(&m_listarray[ComputeSlot(handle)],
                                   (unsigned)handle);
      if (!result) {
        SyncLeaveLock(*lockedhandle);
        *lockedhandle = (LH)0;
      }
      return result;
    }

    //=======================================================================
    void New (H *handle) {
      LH lockedhandle;
      NewLock(handle,&lockedhandle);
      Unlock(lockedhandle);
    }

    //=======================================================================
    T * NewLock (H *handle, LH *lockedhandle) {
      SyncEnterLock(lockedhandle,TRUE);
      H  newhandle = GenerateUniqueHandle();
      T *ptr       = NULL;
      if (REUSE) {
        ptr = m_reuselist.Head();
        if (ptr)
          m_listarray[ComputeSlot(newhandle)].LinkNode(ptr);
      }
      if (!ptr)
        ptr = m_listarray[ComputeSlot(newhandle)].NewNode();
      BaseSetHandle(ptr,(unsigned)newhandle);
      *handle = newhandle;
      return ptr;
    }

    //=======================================================================
    void Unlock (LH lockedhandle) {
      SyncLeaveLock(lockedhandle);
    }

};

template <class T>
class TSExportObject : public TSExplicitNode<T> {
  friend class TSExportTableBase<T>;

  private:
    unsigned  m_handle;
    LINKEX(T) m_linktoslot;

  public:

    //=======================================================================
    TSExportObject () {
    }

    //=======================================================================
    TSExportObject (const TSExportObject &source) {
    }

    //=======================================================================
    TSExportObject & operator= (const TSExportObject &source) {
      // COPY THE OBJECT DATA, BUT DON'T OVERWRITE THE DESTINATION OBJECT'S
      // HANDLE OR LINK
      return *this;
    }

};

#define  SYNC_NONE       CNullSync
#define  SYNC_READWRITE  CLock
#define  SYNC_ALWAYS     CCritSect

#define  EXPORTOBJECTDECL(structname)  \
  typedef struct structname : public TSExportObject< structname >

#define  EXPORTTABLE(structname,handlename,lockedhandlename,synctype) \
  TSExportTable<structname,handlename,lockedhandlename,synctype,FALSE>

#define  EXPORTTABLEREUSE(structname,handlename,lockedhandlename,synctype) \
  TSExportTable<structname,handlename,lockedhandlename,synctype,TRUE>


/****************************************************************************
*
*  SWAP -- Swap template
*
*  Macros:
*
*    SWAP(a,b)
*
***/

//===========================================================================
template <class T>
void inline TSSwap (T &a, T &b) {
  T temp = a;
  a = b;
  b = temp;
}
#define  SWAP(a,b)  TSSwap(a,b)


/****************************************************************************
*
*  Old TList Template -- Obsolete!!
*
***/

//===========================================================================
template <class T>
BOOL inline TListAdd (T **head, T *rec, LPCSTR filename = NULL, int linenumber = 0) {
  if (!(head && rec))
    return FALSE;

  T *newptr = (T *)SMemAlloc(sizeof(T),filename,linenumber,0);
  if (!newptr)
    return FALSE;
  CopyMemory(newptr,rec,sizeof(T));
  newptr->next = *head;
  *head        = newptr;
  return TRUE;
}
#define  LISTADD(a,b)  TListAdd(a,b,(LPCSTR)__FILE__,__LINE__)

//===========================================================================
template <class T>
BOOL inline TListAddEnd (T **head, T *rec, LPCSTR filename = NULL, int linenumber = 0) {
  if (!(head && rec))
    return FALSE;

  T *newptr = (T *)SMemAlloc(sizeof(T),filename,linenumber,0);
  if (!newptr)
    return FALSE;
  CopyMemory(newptr,rec,sizeof(T));
  newptr->next = NULL;

  T **next = head;
  while (*next)
    next = &(*next)->next;
  *next = newptr;

  return TRUE;
}
#define  LISTADDEND(a,b)  TListAddEnd(a,b,(LPCSTR)__FILE__,__LINE__)

//===========================================================================
template <class T>
BOOL inline TListAddPtr (T **head, T *ptr) {
  if (!(head && ptr))
    return FALSE;

  ptr->next = *head;
  *head     = ptr;
  return TRUE;
}
#define  LISTADDPTR(a,b)  TListAddPtr(a,b)

//===========================================================================
template <class T>
BOOL inline TListAddPtrEnd (T **head, T *ptr) {
  if (!(head && ptr))
    return FALSE;

  ptr->next = NULL;
  T **next = head;
  while (*next)
    next = &(*next)->next;
  *next = ptr;

  return TRUE;
}
#define  LISTADDPTREND(a,b)  TListAddPtrEnd(a,b)

//===========================================================================
template <class T>
BOOL inline TListClear (T **head, LPCSTR filename = NULL, int linenumber = 0) {
  if (!head)
    return FALSE;

  while (*head) {
    T *next = (*head)->next;
    SMemFree(*head,filename,linenumber,0);
    *head = next;
  }
  return TRUE;
}
#define  LISTCLEAR(a)  TListClear(a,(LPCSTR)__FILE__,__LINE__)

//===========================================================================
template <class T>
BOOL inline TListFree (T **head, T *ptr, LPCSTR filename = NULL, int linenumber = 0) {
  if (!(head && ptr))
    return FALSE;

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
    return FALSE;

  T **next = head;
  while (*next && (*next != ptr))
    next = &(*next)->next;
  if (*next)
    *next = (*next)->next;

  return (*next != NULL);
}
#define  LISTFREEPTR(a,b)  TListFreePtr(a,b,(LPCSTR)__FILE__,__LINE__)


#if PRAGMA_IMPORT_SUPPORTED
#pragma import off
#endif

#endif // ifndef _STORM_H_

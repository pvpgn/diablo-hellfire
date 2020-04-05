/****************************************************************************
*
*  SGDI.CPP
*  Storm GDI functions
*  (direct replacements for popular Windows GDI functions)
*
*  By Michael O'Brien (2/21/96)
*
***/

#include "pch.h"
#pragma  hdrstop

#define  SIGNATURE   0x4F4D

#define  TYPE_FONT   0
#define  NUMTYPES    1

// USE A PITCH TABLE TO AVOID INTEGER MULTIPLIES, EXCEPT ON POWERPC, WHERE
// AN INTEGER MULTIPLY IS FASTER THAN A MEMORY ACCESS
#if defined(powerc) || defined(__powerc)
#define  USEPITCHTABLE     0
#define  LINEOFFSET(line)  (s_pitch*(line))
#define  ISPITCHVALID      (s_pitch != 0)
#else
#define  USEPITCHTABLE     1
#define  LINEOFFSET(line)  (*(s_pitchtable+(line)))
#define  ISPITCHVALID      (s_pitchtable != NULL)
#endif

typedef struct _BITMAPINFO256 {
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD          bmiColors[256];
} BITMAPINFO256, *LPBITMAPINFO256;

NODEDECL(SGDIOBJ) {
    WORD   signature;
    WORD   type;
    LPBYTE bits;
} *LPSGDIOBJ;

typedef struct SGDIFONT : public SGDIOBJ  {
    int  filecharwidth;
    int  filecharheight;
    int  filepitch;
    int  filecolumnand;
    int  filerowshift;
    SIZE charsize[256];
} *LPSGDIFONT;

static LIST(SGDIOBJ) s_objectlist;
static int           s_pitch              = 640;
#if USEPITCHTABLE
static LPDWORD       s_pitchtable         = NULL;
static int           s_pitchtablealloc    = 0;
static int           s_pitchtablelines    = 0;
#endif
static int           s_screenbpp          = 8;
static int           s_screenbppshift     = 0;
static int           s_screencx           = 640;
static int           s_screency           = 480;
static LPSGDIOBJ     s_selected[NUMTYPES] = {NULL};
static LPBYTE        s_tempbuffer         = NULL;
static int           s_tempbuffersize     = 0;

//===========================================================================
static BOOL AllocateTempBuffer (int size) {

  // USE THE EXISTING BUFFER IF IT IS BIG ENOUGH
  if (size && (size <= s_tempbuffersize))
    return 1;

  // OTHERWISE, FREE THE OLD BUFFER
  if (s_tempbuffer) {
    FREE(s_tempbuffer);
    s_tempbuffer     = NULL;
    s_tempbuffersize = 0;
  }

  // ALLOCATE A NEW BUFFER
  if (size) {
    s_tempbuffer     = (LPBYTE)ALLOC(size);
    s_tempbuffersize = size;
    return TRUE;
  }

  return FALSE;
}

//===========================================================================
static void inline ClipRectangle (LPRECT rect) {
  if (rect->left < 0)
    rect->left = 0;
  if (rect->top < 0)
    rect->top = 0;
  if (rect->right > s_screencx)
    rect->right = s_screencx;
  if (rect->bottom > s_screency)
    rect->bottom = s_screency;
}

//===========================================================================
static DWORD inline ConvertColorRefToPattern (COLORREF colorref) {
  DWORD colordata = 0;

  // IF WE WERE GIVEN A PALETTE INDEX, USE IT
  if (colorref & 0x01000000)
    if (s_screenbpp == 8)
      colordata = colorref & 0x00FFFFFF;
    else
      colordata = 0;

  // OTHERWISE, CONVERT THE COLOR TO THE CURRENT SCREEN FORMAT
  else switch (s_screenbpp) {

    case 8:
      colordata = 0;
      break;

    case 16:
      colordata = ((GetBValue(colorref) >> 3) << 10)
                    | ((GetGValue(colorref) >> 3) << 5)
                    | (GetRValue(colorref) >> 3);
      break;

  }

  // PROPAGATE THE COLOR INTO THE PATTERN
  switch (s_screenbpp) {

    case 8:
      colordata |= (colordata << 8) | (colordata << 16) | (colordata << 24);
      break;

    case 16:
      colordata |= (colordata << 16);
      break;

  }

  return colordata;
}

//===========================================================================
static LPSGDIOBJ InternalCreateGdiObject (int size, WORD type) {
  LPSGDIOBJ ptr = s_objectlist.NewNode(LIST_HEAD,size-sizeof(SGDIOBJ));
  ptr->signature = SIGNATURE;
  ptr->type      = type;
  ptr->bits      = NULL;
  return ptr;
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
BOOL APIENTRY SGdiBitBlt (LPBYTE   videobuffer,
                          int      destx,
                          int      desty,
                          LPBYTE   sourcedata,
                          LPRECT   sourcerect,
                          int      sourcecx,
                          int      sourcecy,
                          COLORREF color,
                          DWORD    rop) {
  VALIDATEBEGIN;
  VALIDATE(videobuffer);
  VALIDATE(sourcedata);
  VALIDATE(s_pitch);
  VALIDATEEND;

  // MAKE LOCAL COPIES OF THE SOURCE AND DESTINATION RECTANGLES
  RECT localdestrect   = {destx,desty,4095,4095};
  SIZE localdestsize   = {s_screencx,s_screency};
  SIZE localsourcesize = {sourcecx,sourcecy};
  RECT localsourcerect;
  if (sourcerect)
    CopyMemory(&localsourcerect,sourcerect,sizeof(RECT));

  // IF NECESSARY, CONVERT THE COLORREF TO A PATTERN
  DWORD localpattern = 0;
  if (color && (rop != SRCCOPY))
    localpattern = ConvertColorRefToPattern(color);

  // IF WE ARE NOT IN 8-BIT MODE, CONVERT PIXELS TO BYTES
  if (s_screenbppshift) {
    localdestrect.left    <<= s_screenbppshift;
    localdestrect.right   <<= s_screenbppshift;
    localdestsize.cx      <<= s_screenbppshift;
    localsourcerect.left  <<= s_screenbppshift;
    localsourcerect.right <<= s_screenbppshift;
    localsourcesize.cx    <<= s_screenbppshift;
  }

  // PERFORM THE BITBLT
  return SBltROP3Clipped(videobuffer,
                         &localdestrect,
                         &localdestsize,
                         s_pitch,
                         sourcedata,
                         sourcerect ? &localsourcerect : NULL,
                         &localsourcesize,
                         sourcecx << s_screenbppshift,
                         localpattern,
                         rop);
}

//===========================================================================
BOOL APIENTRY SGdiCreateFont (LPBYTE     bits,
                              int        width,
                              int        height,
                              int        bitdepth,
                              int        filecharwidth,
                              int        filecharheight,
                              LPSIZE     charsizetable,
                              HSGDIFONT *handle) {
  if (handle)
    *handle = (HSGDIFONT)0;

  VALIDATEBEGIN;
  VALIDATE(bits);
  VALIDATE(width);
  VALIDATE(height);
  VALIDATE(filecharwidth);
  VALIDATE(filecharheight);
  VALIDATE(charsizetable);
  VALIDATE(handle);
  VALIDATEEND;

  // CREATE A RECORD FOR THE FONT
  LPSGDIFONT newptr = (LPSGDIFONT)InternalCreateGdiObject(sizeof(SGDIFONT),
                                                          TYPE_FONT);
  if (!newptr)
    return FALSE;
  LPBYTE savebits = (LPBYTE)ALLOC((width*height*bitdepth) >> 3);
  CopyMemory(savebits,bits,(width*height*bitdepth) >> 3);
  CopyMemory(&newptr->charsize[0],charsizetable,256*sizeof(SIZE));
  newptr->bits           = savebits;
  newptr->filecharwidth  = filecharwidth;
  newptr->filecharheight = filecharheight;
  newptr->filepitch      = width;
  newptr->filecolumnand  = 1;
  newptr->filerowshift   = 0;
  int charspercolumn = 1;
  while (charspercolumn < width/filecharwidth) {
    charspercolumn        <<= 1;
    newptr->filecolumnand <<= 1;
    ++newptr->filerowshift;
  }
  --newptr->filecolumnand;

  // RETURN A HANDLE TO THE FONT
  if (handle)
    *handle = (HSGDIFONT)newptr;

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SGdiDeleteObject (HSGDIOBJ handle) {

  // GET A POINTER TO THE OBJECT
  LPSGDIOBJ ptr = (LPSGDIOBJ)handle;
  if ((!ptr) || (ptr->signature != SIGNATURE) || (ptr->type >= NUMTYPES))
    return FALSE;

  // UNSELECT THE OBJECT
  if (s_selected[ptr->type] == ptr)
    s_selected[ptr->type] = NULL;

  // FREE ANY DATA ALLOCATED FOR THIS OBJECT
  if (ptr->bits) {
    FREE(ptr->bits);
    ptr->bits = NULL;
  }

  // UNLINK AND FREE THE OBJECT
  s_objectlist.DeleteNode(ptr);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SGdiDestroy () {
  LPSGDIOBJ curr;
  while ((curr = s_objectlist.Head()) != NULL) {
    switch (curr->type) {

      case TYPE_FONT:
        REPORTRESOURCELEAK(HSGDIFONT);
      break;

      default:
        REPORTRESOURCELEAK(HSGDIOBJ);
      break;

    }
    SGdiDeleteObject((HSGDIOBJ)curr);
  }
  AllocateTempBuffer(0);
#if USEPITCHTABLE
  if (s_pitchtable) {
    FREE(s_pitchtable);
    s_pitchtable      = NULL;
    s_pitchtablealloc = 0;
    s_pitchtablelines = 0;
  }
#endif
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SGdiExtTextOut (LPBYTE   videobuffer,
                              int      x,
                              int      y,
                              LPRECT   rect,
                              COLORREF color,
                              int      textcoloruse,
                              int      bkgcoloruse,
                              LPCTSTR  string,
                              int      chars) {
  VALIDATEBEGIN;
  VALIDATE(videobuffer);
  VALIDATE(rect);
  VALIDATE(string);
  VALIDATEEND;

  if (!(ISPITCHVALID && s_selected[TYPE_FONT]))
    return FALSE;
  if (chars < 0)
    chars = SStrLen(string);

  // CLIP THE RECTANGLE AND COORDINATES AGAINST THE SCREEN
  RECT clippedrect;
  clippedrect.left   = max(rect->left,0);
  clippedrect.top    = max(rect->top,0);
  clippedrect.right  = max(rect->left,min(rect->right,s_screencx));
  clippedrect.bottom = max(rect->top,min(rect->bottom,s_screency));
  x                  = max(clippedrect.left,min(x,s_screencx));
  y                  = max(clippedrect.top ,min(y,s_screency));

  // IF THE TEXT AND BACKGROUND ARE TO BE THE SAME COLOR, USE THE RECTANGLE()
  // FUNCTION INSTEAD
  if (textcoloruse == bkgcoloruse)
    if (textcoloruse == ETO_TEXT_TRANSPARENT)
      return TRUE;
    else
      return SGdiRectangle(videobuffer,
                           clippedrect.left,
                           clippedrect.right,
                           clippedrect.top,
                           clippedrect.bottom,
                           (textcoloruse == ETO_TEXT_COLOR)
                             ? color
                             : (textcoloruse == ETO_TEXT_BLACK)
                                ? PALETTEINDEX(0x00)
                                : PALETTEINDEX(0xFF));

  // DETERMINE THE PATTERN AND ROP CODE
  DWORD pattern = ConvertColorRefToPattern(color);
  DWORD rop     = 0;
  switch (textcoloruse) {

    case ETO_TEXT_TRANSPARENT:
      switch (bkgcoloruse) {
        case ETO_BKG_COLOR:        rop = 0x00E20746;  break;
        case ETO_BKG_BLACK:        rop = 0x00220326;  break;
        case ETO_BKG_WHITE:        rop = 0x00EE0086;  break;
      }
      break;

    case ETO_TEXT_COLOR:
      switch (bkgcoloruse) {
        case ETO_BKG_TRANSPARENT:  rop = 0x00B8074A;  break;
        case ETO_BKG_BLACK:        rop = 0x0030032A;  break;
        case ETO_BKG_WHITE:        rop = 0x00FC008A;  break;
      }
      break;

    case ETO_TEXT_BLACK:
      switch (bkgcoloruse) {
        case ETO_BKG_TRANSPARENT:  rop = 0x008800C6;  break;
        case ETO_BKG_COLOR:        rop = 0x00C000CA;  break;
        case ETO_BKG_WHITE:        rop = 0x00CC0020;  break;
      }
      break;

    case ETO_TEXT_WHITE:
      switch (bkgcoloruse) {
        case ETO_BKG_TRANSPARENT:  rop = 0x00BB0226;  break;
        case ETO_BKG_COLOR:        rop = 0x00F3022A;  break;
        case ETO_BKG_BLACK:        rop = 0x00330008;  break;
      }
      break;

  }
  if (!rop)
    return FALSE;

  // DRAW THE TEXT
  LPSGDIFONT currfont = (LPSGDIFONT)s_selected[TYPE_FONT];
  int        textx    = x;
  {
    SIZE destsize = {s_screencx,s_screency};
    while (chars--) {
      RECT sourcerect;
      sourcerect.left   = (int)(((BYTE)*string) & currfont->filecolumnand)*currfont->filecharwidth;
      sourcerect.top    = (int)(((BYTE)*string) >> currfont->filerowshift)*currfont->filecharheight;
      sourcerect.right  = sourcerect.left+min(clippedrect.right-textx,
                                              currfont->filecharwidth);
      sourcerect.bottom = sourcerect.top+min(clippedrect.bottom-y,
                                             currfont->filecharheight);
      SGdiBitBlt(videobuffer,
                 textx,
                 y,
                 currfont->bits,
                 &sourcerect,
                 currfont->filepitch,
                 0x7FFF,
                 color,
                 rop);
      textx += currfont->charsize[(BYTE)*(string++)].cx;
    }
  }
  textx = min(textx,s_screencx-1);

  // IF THE BACKGROUND IS TRANSPARENT, RETURN NOW
  if (bkgcoloruse == ETO_BKG_TRANSPARENT)
    return TRUE;

  // DETERMINE A ROP CODE FOR DRAWING THE UNOBSTRUCTED BACKGROUND AREA
  switch (bkgcoloruse) {
    case ETO_BKG_COLOR: rop = PATCOPY;    break;
    case ETO_BKG_BLACK: rop = BLACKNESS;  break;
    case ETO_BKG_WHITE: rop = WHITENESS;  break;
  }

  // DRAW THE AREA TO THE LEFT OF THE TEXT
  if (x > rect->left)
    SBltROP3(videobuffer+LINEOFFSET(y)+rect->left,
             NULL,
             x-rect->left,
             min(currfont->filecharheight,rect->bottom-y),
             s_pitch,
             0,
             pattern,
             rop);

  // DRAW THE AREA TO THE RIGHT OF THE TEXT
  if (textx < rect->right)
    SBltROP3(videobuffer+LINEOFFSET(y)+textx,
             NULL,
             rect->right-textx,
             min(currfont->filecharheight,rect->bottom-y),
             s_pitch,
             0,
             pattern,
             rop);

  // DRAW THE AREA ABOVE THE TEXT
  if (y > rect->top)
    SBltROP3(videobuffer+LINEOFFSET(rect->top)+rect->left,
             NULL,
             rect->right-rect->left,
             y-rect->top,
             s_pitch,
             0,
             pattern,
             rop);

  // DRAW THE AREA BELOW THE TEXT
  if (y+currfont->filecharheight < rect->bottom)
    SBltROP3(videobuffer+LINEOFFSET(y+currfont->filecharheight)+rect->left,
             NULL,
             rect->right-rect->left,
             rect->bottom-(y+currfont->filecharheight),
             s_pitch,
             0,
             pattern,
             rop);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SGdiGetTextExtent (LPCTSTR string,
                                 int     chars,
                                 LPSIZE  size) {
  if (size) {
    size->cx = 0;
    size->cy = 0;
  }

  VALIDATEBEGIN;
  VALIDATE(string);
  VALIDATE(size);
  VALIDATEEND;

  LPSGDIFONT currfont = (LPSGDIFONT)s_selected[TYPE_FONT];
  if (!currfont)
    return FALSE;
  if (chars < 0)
    chars = SStrLen(string);

  while (chars--) {
    size->cx += currfont->charsize[(BYTE)*string].cx;
    size->cy  = max(size->cy,currfont->charsize[(BYTE)*string].cy);
    ++string;
  }

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SGdiImportFont (HFONT      windowsfont,
                              HSGDIFONT *handle) {
  if (handle)
    *handle = (HSGDIFONT)0;

  VALIDATEBEGIN;
  VALIDATE(windowsfont);
  VALIDATE(handle);
  VALIDATEEND;

  HBITMAP bitmap        = (HBITMAP)0;
  LPSIZE  charsizetable = NULL;
  HDC     dc            = (HDC)0;
  HDC     memdc         = (HDC)0;
  HFONT   oldfont       = (HFONT)0;
  LPBYTE  packedbits    = NULL;
  BOOL    success       = FALSE;
  LPBYTE  unpackedbits  = NULL;

  TRY {

    // CREATE A DC AND SELECT THE FONT INTO IT
    dc    = GetDC(GetDesktopWindow());
    memdc = CreateCompatibleDC(dc);
    if (!(dc && memdc))
      LEAVE;
    oldfont = (HFONT)SelectObject(memdc,windowsfont);

    // VERIFY THAT THE FONT IS A TRUETYPE FONT
    {
      TEXTMETRIC tm;
      ZeroMemory(&tm,sizeof(TEXTMETRIC));
      GetTextMetrics(memdc,&tm);
      if (!(tm.tmPitchAndFamily & TMPF_TRUETYPE))
        LEAVE;
    }

    // BUILD THE CHARACTER SIZE TABLE, AND DETERMINE THE MAXIMUM SIZE OF ANY
    // CHARACTER IN THE FONT, AS WELL AS THE MAXIMUM EXPECTED HEIGHT OF ANY
    // COMBINATION OF CHARACTERS
    SIZE maxsize = {0,0};
    {
      LPCTSTR testchars = "W_y,|'";
      GetTextExtentPoint32(memdc,testchars,SStrLen(testchars),&maxsize);
      maxsize.cx = 0;
    }
    charsizetable = (LPSIZE)ALLOC(256*sizeof(SIZE));
    {
      for (int loop = 0; loop < 256; ++loop) {
        char teststring[2] = {loop,0};
        GetTextExtentPoint32(memdc,teststring,1,charsizetable+loop);
        maxsize.cx = max(maxsize.cx,(charsizetable+loop)->cx+1);
        maxsize.cy = max(maxsize.cy,(charsizetable+loop)->cy+1);
      }
    }
    if (!(maxsize.cx && maxsize.cy))
      LEAVE;

    // ADD ONE TO THE WIDTH TO LEAVE A SPACE BETWEEN CHARACTERS, AND ROUND
    // THE RESULT UP TO A MULTIPLE OF FOUR PIXELS
    if (maxsize.cx & 3)
      maxsize.cx += 4-(maxsize.cx & 3);
    SIZE bitmapsize = {maxsize.cx*16,maxsize.cy*16};

    // PREPARE A BITMAP HEADER AND COLOR TABLE
    BITMAPINFO256 info;
    ZeroMemory(&info,sizeof(BITMAPINFO256));
    info.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth       = bitmapsize.cx;
    info.bmiHeader.biHeight      = bitmapsize.cy;
    info.bmiHeader.biPlanes      = 1;
    info.bmiHeader.biBitCount    = 1;
    info.bmiHeader.biCompression = BI_RGB;

    // CREATE A DEVICE INDEPENDENT BITMAP AND SELECT IT INTO THE MEMORY DC
    bitmap = CreateDIBitmap(memdc,&info.bmiHeader,0,NULL,(LPBITMAPINFO)&info,DIB_RGB_COLORS);
    if (!bitmap)
      LEAVE;
    HBITMAP oldbitmap = (HBITMAP)SelectObject(memdc,bitmap);

    // BLANK OUT THE BITMAP
    {
      HBRUSH oldbrush = (HBRUSH)SelectObject(memdc,GetStockObject(WHITE_BRUSH));
      HPEN   oldpen   = (HPEN)  SelectObject(memdc,GetStockObject(WHITE_PEN));
      Rectangle(memdc,0,0,bitmapsize.cx,bitmapsize.cy);
      SelectObject(memdc,oldbrush);
      SelectObject(memdc,oldpen);
    }

    // DRAW THE FONT INTO THE BITMAP
    SetTextAlign(memdc,TA_LEFT | TA_TOP);
    SetTextColor(memdc,0);
    SetBkColor(memdc,0xFFFFFF);
    SetBkMode(memdc,OPAQUE);
    {
      char string[2] = "?";
      for (int y = 0; y < 16; ++y)
        for (int x = 0; x < 16; ++x) {
          string[0] = y*16+x;
          RECT rect = {x*maxsize.cx,
                       y*maxsize.cy,
                       (x+1)*maxsize.cx,
                       (y+1)*maxsize.cy};
          ExtTextOut(memdc,
                     x*maxsize.cx+1,
                     y*maxsize.cy+1,
                     ETO_OPAQUE,
                     &rect,
                     string,
                     1,
                     NULL);
        }
    }

    // UNSELECT THE BITMAP
    SelectObject(memdc,oldbitmap);

    // GET THE BITMAP BITS
    packedbits = (LPBYTE)ALLOC(bitmapsize.cx*bitmapsize.cy/8);
    if (!GetDIBits(memdc,bitmap,0,bitmapsize.cy,packedbits,(LPBITMAPINFO)&info,DIB_RGB_COLORS))
      LEAVE;

    // UNPACK THE BITMAP BITS INTO 8-BIT TOP-DOWN FORMAT
    unpackedbits = (LPBYTE)ALLOC(bitmapsize.cx*bitmapsize.cy);
    {
      LPBYTE source = packedbits+(bitmapsize.cy-1)*bitmapsize.cx/8;
      LPBYTE dest   = unpackedbits;
      for (int y = 0; y < bitmapsize.cy; ++y) {
        for (int x = 0; x < bitmapsize.cx/8; ++x) {
          for (int bit = 128; bit; bit >>= 1)
            *dest++ = ((*source) & bit) ? 0xFF : 0;
          ++source;
        }
        source -= bitmapsize.cx/4;
      }
    }

    // CREATE A STORM FONT FROM THE BITMAP BITS
    success = SGdiCreateFont(unpackedbits,
                             bitmapsize.cx,
                             bitmapsize.cy,
                             8,
                             maxsize.cx,
                             maxsize.cy,
                             charsizetable,
                             handle);

  }
  FINALLY {
    if (unpackedbits)
      FREE(unpackedbits);
    if (packedbits)
      FREE(packedbits);
    if (charsizetable)
      FREE(charsizetable);
    if (memdc && oldfont)
      SelectObject(memdc,oldfont);
    if (bitmap)
      DeleteObject(bitmap);
    if (memdc)
      DeleteDC(memdc);
    if (dc)
      ReleaseDC(GetDesktopWindow(),dc);
  }

  return success;
}

//===========================================================================
BOOL APIENTRY SGdiLoadFont (LPCTSTR    filename,
                            int        filecharwidth,
                            int        filecharheight,
                            int        basecharwidth,
                            LPSIZE     charsizetable,
                            HSGDIFONT *handle) {
  VALIDATEBEGIN;
  VALIDATE(filename);
  VALIDATE(*filename);
  VALIDATE(filecharwidth);
  VALIDATE(filecharheight);
  VALIDATE(basecharwidth || charsizetable);
  VALIDATE(handle);
  VALIDATEEND;

  // DETERMINE THE SIZE OF THE FONT FILE
  int width;
  int height;
  int bitdepth;
  if (!SBmpLoadImage(filename,NULL,NULL,0,&width,&height,&bitdepth))
    return FALSE;

  // ALLOCATE MEMORY FOR THE FONT BITS
  if (!AllocateTempBuffer(width*height))
    return FALSE;

  // READ THE FONT BITS
  if (!SBmpLoadImage(filename,NULL,s_tempbuffer,width*height))
    return FALSE;

  // IF WE WEREN'T PASSED A CHARACTER SIZE TABLE, CREATE ONE BASED ON THE
  // BASE CHARACTER WIDTH
  SIZE charsize[256];
  if (!charsizetable)
    for (int loop = 0; loop < 256; ++loop) {
      charsize[loop].cx = basecharwidth;
      charsize[loop].cy = basecharwidth;
    }

  // CREATE THE FONT
  return SGdiCreateFont(s_tempbuffer,
                        width,
                        height,
                        bitdepth,
                        filecharwidth,
                        filecharheight,
                        charsizetable ? charsizetable : &charsize[0],
                        handle);
}

//===========================================================================
BOOL APIENTRY SGdiRectangle (LPBYTE   videobuffer,
                             int      left,
                             int      top,
                             int      right,
                             int      bottom,
                             COLORREF color) {
  VALIDATEBEGIN;
  VALIDATE(videobuffer);
  VALIDATEEND;

  if (!ISPITCHVALID)
    return FALSE;
  RECT  destrect = {left,top,right,bottom};
  DWORD pattern  = ConvertColorRefToPattern(color);
  ClipRectangle(&destrect);
  return SBltROP3(videobuffer+LINEOFFSET(destrect.top)+(destrect.left << s_screenbppshift),
                  NULL,
                  (destrect.right-destrect.left) << s_screenbppshift,
                  destrect.bottom-destrect.top,
                  s_pitch,
                  0,
                  pattern,
                  PATCOPY);
}

//===========================================================================
BOOL APIENTRY SGdiSelectObject (HSGDIOBJ handle) {

  // GET A POINTER TO THE OBJECT
  LPSGDIOBJ ptr = (LPSGDIOBJ)handle;
  if ((!ptr) || (ptr->signature != SIGNATURE) || (ptr->type >= NUMTYPES))
    return FALSE;

  // SELECT THE OBJECT
  s_selected[ptr->type] = ptr;

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SGdiSetPitch (int pitch) {
  VALIDATEBEGIN;
  VALIDATE(pitch > 0);
  VALIDATEEND;

  SDrawGetScreenSize(&s_screencx,
                     &s_screency,
                     &s_screenbpp);
  return SGdiSetTargetDimensions(s_screencx,
                                 s_screency,
                                 s_screenbpp,
                                 pitch);
}

//===========================================================================
BOOL APIENTRY SGdiSetTargetDimensions (int width,
                                       int height,
                                       int bitdepth,
                                       int pitch) {

  // UPDATE THE CACHED DIMENSIONS
  s_screencx       = width;
  s_screency       = height;
  s_screenbpp      = bitdepth;
  s_screenbppshift = (s_screenbpp == 16);

  // DON'T RECOMPUTE THE PITCH IF IT HASN'T CHANGED
  if ((pitch  == s_pitch) &&
      (height == s_pitchtablelines) &&
      ISPITCHVALID)
    return TRUE;
  s_pitch = pitch;

#if USEPITCHTABLE

  // REALLOCATE THE PITCH TABLE IF NECESSARY
  if (s_screency > s_pitchtablealloc) {
    if (s_pitchtable)
      FREE(s_pitchtable);
    s_pitchtablealloc = s_screency;
    s_pitchtable      = (LPDWORD)ALLOC(s_screency*sizeof(DWORD));
  }

  // BUILD THE NEW PITCH TABLE
  s_pitchtablelines = s_screency;
  {
    DWORD offset = 0;
    for (int loop = 0; loop < s_pitchtablelines; ++loop) {
      *(s_pitchtable+loop) = offset;
      offset += pitch;
    }
  }

#endif

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SGdiTextOut (LPBYTE   videobuffer,
                           int      x,
                           int      y,
                           COLORREF color,
                           LPCTSTR  string,
                           int      chars) {
  int textcoloruse;
  if (!color)
    textcoloruse = ETO_TEXT_BLACK;
  else if (color == PALETTEINDEX(0xFF))
    textcoloruse = ETO_TEXT_WHITE;
  else
    textcoloruse = ETO_TEXT_COLOR;
  RECT rect = {0,0,INT_MAX,INT_MAX};
  return SGdiExtTextOut(videobuffer,
                        x,
                        y,
                        &rect,
                        color,
                        textcoloruse,
                        ETO_BKG_TRANSPARENT,
                        string,
                        chars);
}

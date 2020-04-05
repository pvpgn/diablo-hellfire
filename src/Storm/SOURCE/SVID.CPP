/****************************************************************************
*
*  SVID.CPP
*  Storm video playback functions
*
*  By Michael O'Brien (3/28/96)
*
***/

#include "pch.h"
#pragma  hdrstop

#define  BENCHMARKVERSION  3
#define  MAXSOURCEWIDTH    320
#define  TOLERANCE         768

#define  COL_RED           0
#define  COL_GREEN         1
#define  COL_BLUE          2

#define  REGKEY            "Video Player"
#define  REGVALUE_MODE     "Mode"

typedef struct _COLORNODE {
    _COLORNODE *next;
    BYTE        color;
} COLORNODE, *COLORNODEPTR;

typedef struct _SCODE {
    HSCODESTREAM horizontalinterpolate;
    HSCODESTREAM verticalinterpolate;
    HSCODESTREAM pixeldouble;
} SCODEREC, *SCODEPTR;

typedef struct _SMACKERAPI {
    void       (RADEXPLINK *SmackBufferClose)(SmackBuf *);
    void       (RADEXPLINK *SmackBufferNewPalette)(SmackBuf *,void *,u32);
    SmackBuf * (RADEXPLINK *SmackBufferOpen)(HWND,u32,u32,u32,u32,u32);
    void       (RADEXPLINK *SmackClose)(Smack *);
    u32        (RADEXPLINK *SmackDoFrame)(Smack *);
    void       (RADEXPLINK *SmackGoto)(Smack *,u32);
    void       (RADEXPLINK *SmackNextFrame)(Smack *);
    Smack *    (RADEXPLINK *SmackOpen)(char *,u32,u32);
    u8         (RADEXPLINK *SmackSoundUseDirectSound)(LPVOID);
    void       (RADEXPLINK *SmackToBuffer)(Smack *,u32,u32,u32,u32,void *,u32);
    u32        (RADEXPLINK *SmackToBufferRect)(Smack *,u32);
    void       (RADEXPLINK *SmackVolumePan)(Smack *,u32,u32,u32);
    u32        (RADEXPLINK *SmackWait)(Smack *);
} SMACKERAPI, *SMACKERAPIPTR;

typedef struct _SMACKERDATA {
    SmackBuf *buffer;
    Smack    *file;
    int       top;
} SMACKERDATA, *SMACKERDATAPTR;

NODEDECL(VIDEOREC) {
    HANDLE           filehandle;
    SCODEEXECUTEDATA executedata;
    LPBYTE           interptable;
    DWORD            palettefirstentry;
    DWORD            palettenumentries;
    SMACKERDATAPTR   smackerdata;
    LPBYTE           destbuffer;
    RECT             destrect;
    LPCRECT          ddrect;
    SIZE             destsize;
    LPBYTE           lockedbuffer;
    DWORD            flags;
    BOOL             vertzoom;
    BOOL             vertinterp;
    int              altline;
    CSRgn            rgn[2];
    DWORD            allocrects;
    DWORD            numrects;
    LPRECT           rect;
} *VIDEOPTR;

static SCODEPTR       s_scode      = NULL;
static SMACKERAPIPTR  s_smackerapi = NULL;
static HINSTANCE      s_smackerlib = (HINSTANCE)0;
static LIST(VIDEOREC) s_videolist;

static void UnlockBuffer (VIDEOPTR video);

//===========================================================================
static void AdjustGamma (LPPALETTEENTRY pe, double gamma) {
  for (int loop = 0; loop < 256; ++loop) {
    double red   = 256.0*pow(((double)(pe+loop)->peRed  )/256.0,gamma);
    double green = 256.0*pow(((double)(pe+loop)->peGreen)/256.0,gamma);
    double blue  = 256.0*pow(((double)(pe+loop)->peBlue )/256.0,gamma);
    (pe+loop)->peRed   = (red   > 255.0) ? 255 : (BYTE)red;
    (pe+loop)->peGreen = (green > 255.0) ? 255 : (BYTE)green;
    (pe+loop)->peBlue  = (blue  > 255.0) ? 255 : (BYTE)blue;
  }
}

//===========================================================================
static BOOL inline BindToSmacker () {

  // LOAD THE SMACKER LIBRARY
  if (!s_smackerlib) {
    s_smackerlib = LoadLibrary("smackw32.dll");
    if (!s_smackerlib)
      return FALSE;
  }

  // ALLOCATE AN API STRUCTURE
  if (!s_smackerapi)
    s_smackerapi = NEW(SMACKERAPI);

  // BIND TO THE INDIVIDUAL FUNCTIONS
  BOOL success = TRUE;
#define  BIND(a,b)  *(void **)&s_smackerapi->##a        \
                      = GetProcAddress(s_smackerlib,b); \
                    if (!s_smackerapi->##a)             \
                      success = FALSE;
  BIND(SmackBufferClose        ,"_SmackBufferClose@4");
  BIND(SmackBufferNewPalette   ,"_SmackBufferNewPalette@12");
  BIND(SmackBufferOpen         ,"_SmackBufferOpen@24");
  BIND(SmackClose              ,"_SmackClose@4");
  BIND(SmackDoFrame            ,"_SmackDoFrame@4");
  BIND(SmackGoto               ,"_SmackGoto@8");
  BIND(SmackNextFrame          ,"_SmackNextFrame@4");
  BIND(SmackOpen               ,"_SmackOpen@12");
  BIND(SmackSoundUseDirectSound,"_SmackSoundUseDirectSound@4");
  BIND(SmackToBuffer           ,"_SmackToBuffer@28");
  BIND(SmackToBufferRect       ,"_SmackToBufferRect@8");
  BIND(SmackVolumePan          ,"_SmackVolumePan@16");
  BIND(SmackWait               ,"_SmackWait@4");
#undef  BIND
  return success;
}

//===========================================================================
static BOOL CreateLinearInterpolationTable (VIDEOPTR video, LPPALETTEENTRY pe) {

  // ALLOCATE 64K FOR THE INTERPOLATION TABLE IF IT'S NOT ALREADY ALLOCATED
  if (!video->interptable) {
    video->interptable       = (LPBYTE)ALLOC(0x10000);
    video->executedata.table = video->interptable;
  }

  // BUILD A SQUARE TABLE
  static DWORD squaretable[511];
  static BOOL  squareinit = FALSE;
  if (!squareinit) {
    squareinit = TRUE;
    for (int loop = -255; loop <= 255; ++loop)
      squaretable[loop+255] = (DWORD)(loop*loop);
  }

  // BUILD A SET OF POINTERS INTO THE SQUARE TABLE FOR EACH PALETTE ENTRY
  LPBYTE palcolor[3][256];
  DWORD  loop;
  for (loop = 0; loop < 256; ++loop) {
    palcolor[COL_RED  ][loop] = (LPBYTE)&squaretable[255+(pe+loop)->peRed];
    palcolor[COL_GREEN][loop] = (LPBYTE)&squaretable[255+(pe+loop)->peGreen];
    palcolor[COL_BLUE ][loop] = (LPBYTE)&squaretable[255+(pe+loop)->peBlue];
  }

  // FILL IN THE COLOR MATCHING TABLE
  COLORNODEPTR interpcolortable[18][18][18];
  COLORNODE    interpnode[256];
  ZeroMemory(&interpcolortable,18*18*18*sizeof(COLORNODEPTR));
  for (loop = 0; loop < 256; ++loop) {
    if (loop &&
        ((pe+loop)->peRed   == (pe+loop-1)->peRed) &&
        ((pe+loop)->peGreen == (pe+loop-1)->peGreen) &&
        ((pe+loop)->peBlue  == (pe+loop-1)->peBlue))
      continue;
    DWORD red   = ((DWORD)(pe+loop)->peRed   >> 4)+1;
    DWORD green = ((DWORD)(pe+loop)->peGreen >> 4)+1;
    DWORD blue  = ((DWORD)(pe+loop)->peBlue  >> 4)+1;
    interpnode[loop].color = (BYTE)loop;
    interpnode[loop].next  = interpcolortable[red][green][blue];
    interpcolortable[red][green][blue] = &interpnode[loop];
  }

  // BUILD THE INTERPOLATION TABLE.  FOR EVERY PAIR OF COLORS, WE LOOK
  // FOR A COLOR THAT IS AS CLOSE AS POSSIBLE TO EQUIDISTANT BETWEEN THE
  // TWO COLORS.
  for (DWORD loop1 = 0; loop1 < 256; ++loop1) {
    for (DWORD loop2 = 0; loop2 < loop1; ++loop2) {
      BYTE  minnum  = (BYTE)max(loop1,loop2);
      DWORD mindist = UINT_MAX;

      // FIND THE DESIRED COLOR
      DWORD red   = ((DWORD)(pe+loop1)->peRed  +(DWORD)(pe+loop2)->peRed  ) >> 1;
      DWORD green = ((DWORD)(pe+loop1)->peGreen+(DWORD)(pe+loop2)->peGreen) >> 1;
      DWORD blue  = ((DWORD)(pe+loop1)->peBlue +(DWORD)(pe+loop2)->peBlue ) >> 1;

      // FIND THE SLOT IN THE COLOR MATCHING TABLE WHICH WOULD CONTAIN
      // THE DESIRED COLOR
      DWORD redslot   = (red   >> 4)+1;
      DWORD greenslot = (green >> 4)+1;
      DWORD blueslot  = (blue  >> 4)+1;

      // ADJUST THE COLOR VALUES OF THE DESIRED COLOR SO THEY CAN BE
      // USED AS BYTE POINTERS
      red   <<= 2;
      green <<= 2;
      blue  <<= 2;

      // IF THERE IS AT LEAST ONE COLOR IN THE COLOR MATCHING TABLE
      // FOR THE TARGET AREA, USE ONLY COLORS FROM THAT AREA.
      COLORNODEPTR curr = interpcolortable[redslot][greenslot][blueslot];
      if (curr)
        if (curr->next)
          while (curr) {
            DWORD dist = *(LPDWORD)(palcolor[COL_RED][curr->color]-red)
                           +*(LPDWORD)(palcolor[COL_GREEN][curr->color]-green)
                           +*(LPDWORD)(palcolor[COL_BLUE][curr->color]-blue);
            if (dist < mindist) {
              mindist = dist;
              minnum  = curr->color;
            }
            curr = curr->next;
          }
        else
          minnum = curr->color;

      // OTHERWISE, SEARCH A 3X3X3 GRID IN THE COLOR MATCHING TABLE.
      else {
        static const int offs[26][3] = {{-1,0,0},{0,-1,0},{0,0,-1},
                                        {1,0,0},{0,1,0},{0,0,1},
                                        {-1,1,0},{-1,0,1},
                                        {1,-1,0},{0,-1,1},
                                        {1,0,-1},{0,1,-1},
                                        {-1,-1,0},{-1,0,-1},{0,-1,-1},
                                        {1,1,0},{1,0,1},{0,1,1},
                                        {-1,-1,1},{-1,1,-1},{1,-1,-1},
                                        {1,1,-1},{1,-1,1},{-1,1,1},
                                        {-1,-1,-1},{1,1,1}};
        for (BYTE loop = 0; (loop < 26) && (mindist > TOLERANCE); ++loop) {
          curr = interpcolortable[redslot  +offs[loop][0]]
                                 [greenslot+offs[loop][1]]
                                 [blueslot +offs[loop][2]];
          while (curr) {
            DWORD dist = *(LPDWORD)(palcolor[COL_RED][curr->color]-red)
                           +*(LPDWORD)(palcolor[COL_GREEN][curr->color]-green)
                           +*(LPDWORD)(palcolor[COL_BLUE][curr->color]-blue);
            if (dist < mindist) {
              mindist = dist;
              minnum  = curr->color;
            }
            curr = curr->next;
          }
        }
      }

      *(video->interptable+(loop1 << 8)+loop2) = (BYTE)minnum;
      *(video->interptable+(loop2 << 8)+loop1) = (BYTE)minnum;
    }
    *(video->interptable+(loop1 << 8)+loop1) = (BYTE)loop1;
  }

  return TRUE;
}

//===========================================================================
static inline void ExtractRects (VIDEOPTR  videoptr,
                                 CSRgn    *rgn) {
  rgn->GetRects(&videoptr->numrects,NULL);
  if (videoptr->numrects > videoptr->allocrects) {
    FREEIFUSED(videoptr->rect);
    videoptr->allocrects = videoptr->numrects;
    videoptr->rect       = (LPRECT)ALLOC(videoptr->allocrects*sizeof(RECT));
  }
  rgn->GetRects(&videoptr->numrects,videoptr->rect);
}

//===========================================================================
static BOOL IsValidVideoHandle (HSVIDEO handle) {
  ITERATELIST(VIDEOREC,s_videolist,currvideo)
    if (currvideo == (VIDEOPTR)handle)
      return TRUE;
  return FALSE;
}

//===========================================================================
static BOOL LockBuffer (VIDEOPTR video, LPBYTE *buffer, LPSIZE size) {
  if (video->flags & SVID_FLAG_TOSCREEN) {
    size->cx = 640;
    size->cy = 480;
    SDrawGetScreenSize((int *)&size->cx,(int *)&size->cy);
    BOOL result = SDrawLockSurface(SDRAW_SURFACE_FRONT,
                                   video->ddrect,
                                   &video->lockedbuffer,
                                   (int *)&size->cx);
    *buffer = video->lockedbuffer+video->smackerdata->top*size->cx;
    if ((!video->ddrect) && (video->flags & SVID_FLAG_1XSIZE))
      *buffer += (min(640,size->cx)-video->smackerdata->file->Width) >> 1;
    return result;
  }
  else {
    *buffer  = video->destbuffer
                 +(video->destrect.top*video->destsize.cx)
                 +video->destrect.left;
    size->cx = video->destsize.cx;
    size->cy = video->destsize.cy;
    return TRUE;
  }
}

//===========================================================================
static BOOL ProcessFrameDirect (VIDEOPTR videoptr) {
  BOOL   skip = TRUE;
  LPBYTE videobuffer;
  SIZE   videosize;
  if (LockBuffer(videoptr,&videobuffer,&videosize)) {
    s_smackerapi->SmackToBuffer(videoptr->smackerdata->file,
                                0,
                                0,
                                videosize.cx,
                                videosize.cy,
                                videobuffer,
                                0);
    skip = s_smackerapi->SmackDoFrame(videoptr->smackerdata->file);
    UnlockBuffer(videoptr);
  }
  return skip;
}

//===========================================================================
static void ProcessFrameEffects (VIDEOPTR videoptr) {

  // SAVE POINTERS TO COMMONLY ACCESSED DATA STRUCTURES
  SCODEEXECUTEDATA *exec = &videoptr->executedata;
  LPBYTE source      = (LPBYTE)videoptr->smackerdata->buffer->Buffer;
  SIZE   sourcesz    = {videoptr->smackerdata->buffer->Width,
                        videoptr->smackerdata->buffer->Height};
  BOOL   vertzoom    = videoptr->vertzoom;
  BOOL   vertinterp  = videoptr->vertinterp;
  BOOL   horzinterp  = ((videoptr->flags & SVID_FLAG_INTERPOLATE) != 0);
  BOOL   skip        = vertzoom && !(videoptr->flags & SVID_FLAG_DOUBLESCANS);
  BOOL   interlace   = (videoptr->flags & SVID_FLAG_INTERLACE) && !skip;

  // IF WE ARE INTERLACING, ALTERNATE THE SCAN LINE ADJUSTMENT
  if (interlace)
    videoptr->altline = !videoptr->altline;
  int altline = videoptr->altline;

  // UNCOMPRESS THE FRAME INTO THE OFFSCREEN BUFFER, AND BUILD A REGION
  // CONTAINING ALL OF THE RECTANGLES THAT WERE MODIFIED DURING THE
  // DECOMPRESSION PROCESS
  videoptr->rgn[altline].Clear();
  while (s_smackerapi->SmackToBufferRect(videoptr->smackerdata->file,
                                         SMACKSURFACEFAST)) {
    RECT rect = {videoptr->smackerdata->file->LastRectx,
                 videoptr->smackerdata->file->LastRecty,
                 videoptr->smackerdata->file->LastRectx
                   +videoptr->smackerdata->file->LastRectw,
                 videoptr->smackerdata->file->LastRecty
                   +videoptr->smackerdata->file->LastRecth};
    videoptr->rgn[altline].AddRect(&rect,NULL);
  }

  // IF WE ARE INTERLACING, THEN ADD ALL THE RECTANGLES FROM THE PREVIOUS
  // FRAME
  CSRgn *rgn;
  if (interlace) {
    rgn = new CSRgn(videoptr->rgn[altline]);
    ExtractRects(videoptr,&videoptr->rgn[!altline]);
    for (DWORD num = 0; num < videoptr->numrects; ++num)
      rgn->AddRect(&videoptr->rect[num],NULL);
  }
  else
    rgn = &videoptr->rgn[altline];

  // EXTRACT AN ARRAY OF RECTANGLES FROM THE REGION
  ExtractRects(videoptr,rgn);
  if (interlace)
    delete rgn;

  // LOCK THE VIDEO BUFFER
  LPBYTE dest;
  SIZE   destsz;
  if (!LockBuffer(videoptr,&dest,&destsz))
    return;

  // PROCESS EACH RECTANGLE
  for (DWORD num = 0; num < videoptr->numrects; ++num) {
    POINT pos = {videoptr->rect[num].left,
                 videoptr->rect[num].top};
    SIZE  sz  = {videoptr->rect[num].right-videoptr->rect[num].left,
                 videoptr->rect[num].bottom-videoptr->rect[num].top};
    if ((sz.cx < 2) || (sz.cy < 2))
      continue;

    // PERFORM VERTICAL INTERPOLATION IF NECESSARY
    if (vertinterp) {
      exec->xiterations  = sz.cx;
      exec->yiterations  = sz.cy-1;
      exec->source       = source+(pos.y*sourcesz.cx)+pos.x;
      exec->dest         = source+(pos.y*sourcesz.cx)+pos.x+(sourcesz.cx/2);
      exec->adjustsource = sourcesz.cx-sz.cx;
      exec->adjustdest   = sourcesz.cx-sz.cx;
      exec->c            = sourcesz.cx;
      if (pos.y) {
        exec->yiterations++;
        exec->source = (LPBYTE)exec->source-sourcesz.cx;
        exec->dest   = (LPBYTE)exec->dest-sourcesz.cx;
      }
      SCodeExecute(s_scode->verticalinterpolate,exec);
    }

    // SETUP THE OPERATION TO BLT THE OFFSCREEN BUFFER ONTO THE SCREEN
    exec->xiterations  = sz.cx;
    exec->yiterations  = sz.cy;
    exec->source       = source+pos.y*sourcesz.cx+pos.x;
    exec->dest         = dest+pos.y*destsz.cx+pos.x*2;
    exec->adjustsource = sourcesz.cx-sz.cx;
    exec->adjustdest   = destsz.cx-sz.cx*2;

    // ADJUST THE OPERATION FOR HORIZONTAL INTERPOLATION
    exec->xiterations -= horzinterp;

    // ADJUST THE OPERATION FOR VERTICAL ZOOMING
    if (vertzoom) {
      exec->dest         = (LPBYTE)exec->dest+pos.y*destsz.cx;
      exec->yiterations *= 2;
    }

    // ADJUST THE OPERATION FOR VERTICAL INTERPOLATION
    if (vertinterp) {
      exec->adjustsource -= sourcesz.cx/2;
      if (interlace && altline && (pos.y >= 2)) {
        exec->yiterations += 2;
        exec->source       = (LPBYTE)exec->source-sourcesz.cx;
        exec->dest         = (LPBYTE)exec->dest-destsz.cx*2;
      }
      else if ((!interlace) && pos.y) {
        exec->yiterations++;
        exec->source = (LPBYTE)exec->source-sourcesz.cx/2;
        exec->dest   = (LPBYTE)exec->dest-destsz.cx;
      }
    }

    // ADJUST THE OPERATION FOR SCAN LINE SKIPPING
    if (skip) {
      exec->yiterations /= 2;
      exec->adjustdest  += destsz.cx;
    }

    // ADJUST THE OPERATION FOR INTERLACING
    if (interlace) {
      exec->yiterations /= 2;
      exec->adjustdest  += destsz.cx;
      if (altline && (vertinterp || !vertzoom))
        exec->source = (LPBYTE)exec->source+(sourcesz.cx >> vertinterp);
      if (altline)
        exec->dest = (LPBYTE)exec->dest+destsz.cx;
      if (vertinterp)
        exec->adjustsource += sourcesz.cx/2;
      else if (!vertzoom)
        exec->adjustsource += sourcesz.cx;
    }

    // ADJUST THE OPERATION FOR LINE DOUBLING
    exec->adjustsourcealt = exec->adjustsource;
    exec->adjustdestalt   = exec->adjustdest;
    if (vertzoom && !(vertinterp || skip || interlace))
      exec->adjustsource = -sz.cx;

    // PERFORM THE BLT
    SCodeExecute(horzinterp
                   ? s_scode->horizontalinterpolate
                   : s_scode->pixeldouble,
                 exec);


  }

  // UNLOCK THE VIDEO BUFFER
  UnlockBuffer(videoptr);

}

//===========================================================================
static void UnlockBuffer (VIDEOPTR video) {
  if (video->flags & SVID_FLAG_TOSCREEN) {
    SDrawUnlockSurface(SDRAW_SURFACE_FRONT,video->lockedbuffer);
    video->lockedbuffer = NULL;
  }
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
BOOL APIENTRY SVidDestroy () {

  // CLOSE ALL ACTIVE VIDEOS
  VIDEOPTR curr;
  while ((curr = s_videolist.Head()) != NULL) {
    REPORTRESOURCELEAK(HSVIDEO);
    SVidPlayEnd((HSVIDEO)curr);
  }

  // DELETE S-CODE STREAMS
  if (s_scode) {
    SCodeDelete(s_scode->horizontalinterpolate);
    SCodeDelete(s_scode->verticalinterpolate);
    SCodeDelete(s_scode->pixeldouble);
    FREE(s_scode);
    s_scode = NULL;
  }

  // UNBIND FROM SMACKER
  if (s_smackerapi) {
    FREE(s_smackerapi);
    s_smackerapi = NULL;
  }
  if (s_smackerlib) {
    FreeLibrary(s_smackerlib);
    s_smackerlib = (HINSTANCE)0;
  }

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SVidGetSize (HSVIDEO  video,
                           int     *width,
                           int     *height,
                           int     *bitdepth) {
  if (width)
    *width = 0;
  if (height)
    *height = 0;
  if (bitdepth)
    *bitdepth = 8;

  VALIDATEBEGIN;
  VALIDATE(video);
  VALIDATEEND;

  if (!IsValidVideoHandle(video))
    return FALSE;
  VIDEOPTR videoptr = (VIDEOPTR)video;
  if (width)
    *width = videoptr->smackerdata->file->Width;
  if (height)
    *height = videoptr->smackerdata->file->Height;
  return (width || height || bitdepth);
}

//===========================================================================
BOOL APIENTRY SVidInitialize (LPVOID directsound) {

  // BIND TO SMACKER
  if (!BindToSmacker())
    return FALSE;

  // CREATE S-CODE STREAMS FOR THE INNER LOOPS
  if (!s_scode) {
    s_scode = NEW(SCODEREC);
    SCodeCompile("1 W2=S W1=W2 2 D=W","1 W1=W2 W2=S W1=TW 2 D=W",NULL,
                 MAXSOURCEWIDTH,
                 0,
                 &s_scode->horizontalinterpolate);
    SCodeCompile(NULL,"1 W1=S W2=SC D=TW",NULL,
                 MAXSOURCEWIDTH,
                 0,
                 &s_scode->verticalinterpolate);
    SCodeCompile(NULL,"1 W1=S W2=W1 2 D=W",NULL,
                 MAXSOURCEWIDTH,
                 SCODE_CF_USESALTADJUSTS,
                 &s_scode->pixeldouble);
  }
  if (!(s_scode->horizontalinterpolate &&
        s_scode->verticalinterpolate &&
        s_scode->pixeldouble))
    return FALSE;

  // INITIALIZE SOUND
  s_smackerapi->SmackSoundUseDirectSound(directsound);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SVidPlayBegin (LPCTSTR            filename,
                             LPVOID             destbuffer,
                             LPCRECT            destrect,
                             LPSIZE             destsize,
                             SVIDPALETTEUSEPTR  paletteuse,
                             DWORD              flags,
                             HSVIDEO           *handle) {
  if (handle)
    *handle = (HSVIDEO)0;

  VALIDATEBEGIN;
  VALIDATE(filename);
  VALIDATE(handle);
  VALIDATE(destbuffer || !(flags & SVID_FLAG_TOBUFFER));
  VALIDATE(destsize   || !destbuffer);
  VALIDATE((!paletteuse) || (paletteuse->size == sizeof(SVIDPALETTEUSE)));
  VALIDATEEND;

  // DETERMINE THE SCREEN SIZE, WHICH IS NEEDED BY MANY OF THE SIZING AND
  // POSITIONING CALCULATIONS PERFORMED BY THIS FUNCTION
  int screencx, screency;
  SDrawGetScreenSize(&screencx,&screency);

  // IF THE AUTOSIZE OR AUTOQUALITY FLAGS ARE SET, READ THE CURRENT VIDEO
  // MODE OUT OF THE REGISTRY, OR DEFAULT TO HIGH QUALITY MODE
  if (flags & (SVID_FLAG_AUTOSIZE | SVID_FLAG_AUTOQUALITY)) {

    // LOAD THE BASE SETTING FROM THE REGISTRY
    DWORD newflags = 0;
    SRegLoadValue(REGKEY,REGVALUE_MODE,0,&newflags);

    // ADJUST IT TO THE DEFAULT QUALITY LEVEL
    if (!(newflags & SVID_FLAG_1XSIZE))
      newflags = SVID_QUALITY_HIGH;

    // ADJUST THE FLAGS
    if (flags & SVID_FLAG_AUTOSIZE) {
      flags &= ~(SVID_FLAG_1XSIZE | SVID_FLAG_2XSIZE);
      flags |= (newflags & (SVID_FLAG_1XSIZE | SVID_FLAG_2XSIZE));
    }
    if (flags & SVID_FLAG_AUTOQUALITY) {
      flags &= ~(SVID_FLAG_DOUBLESCANS | SVID_FLAG_INTERPOLATE | SVID_FLAG_INTERLACE);
      flags |= (newflags & (SVID_FLAG_DOUBLESCANS | SVID_FLAG_INTERPOLATE | SVID_FLAG_INTERLACE));
    }

  }

  // INITIALIZE THIS MODULE IF NECESSARY
  if (!(s_smackerlib && s_smackerapi && s_scode))
    if (!SVidInitialize(NULL))
      return FALSE;

  // ALLOCATE A NEW VIDEO RECORD
  VIDEOPTR videoptr = s_videolist.NewNode();
  videoptr->filehandle = INVALID_HANDLE_VALUE;

  // PERFORM INITIAL PROCESSING ON THE FLAGS
  flags = (flags & ~SVID_FLAG_TOSCREEN)
            | ((flags & SVID_FLAG_TOBUFFER) ? 0 : SVID_FLAG_TOSCREEN);
  if (flags & SVID_FLAG_TOBUFFER)
    flags &= ~SVID_FLAG_FULLSCREEN;
  if (flags & SVID_FLAG_TOSCREEN)
    destbuffer = NULL;

  // PROCESS AND SAVE THE DESTINATION 
  videoptr->destbuffer = (LPBYTE)destbuffer;
  if (destrect) {
    CopyMemory(&videoptr->destrect,destrect,sizeof(RECT));
    videoptr->ddrect = &videoptr->destrect;
  }
  if (destsize)
    CopyMemory(&videoptr->destsize,destsize,sizeof(SIZE));

  // PROCESS AND SAVE THE PALETTE INFORMATION
  if (paletteuse) {
    videoptr->palettefirstentry = paletteuse->firstentry;
    videoptr->palettenumentries = paletteuse->numentries;
  }
  else {
    videoptr->palettefirstentry = 1;
    videoptr->palettenumentries = 254;
  }

  // ALLOCATE A SMACKER DATA RECORD
  videoptr->smackerdata = NEWZERO(SMACKERDATA);

  // OPEN THE FILE
  {
    DWORD  smackerflags = SMACKFILEHANDLE | SMACKTRACKS;
    HANDLE filehandle   = INVALID_HANDLE_VALUE;
    if (flags & SVID_FLAG_PRELOAD)
      smackerflags |= SMACKPRELOADALL;
    if (flags & SVID_FLAG_NOSKIP)
      smackerflags |= SMACKNOSKIP;
    if (flags & SVID_FLAG_NEEDPAN)
      smackerflags |= SMACKNEEDPAN;
    if (flags & SVID_FLAG_NEEDVOLUME)
      smackerflags |= SMACKNEEDVOLUME;
    if (flags & SVID_FLAG_FILEHANDLE)
      filehandle = (HANDLE)filename;
    else if (SFileOpenFileWin32(filename,&filehandle))
      videoptr->filehandle = filehandle;
    else {
      SVidPlayEnd((HSVIDEO)videoptr);
      return FALSE;
    }
    if (!(videoptr->smackerdata->file = s_smackerapi->SmackOpen((char *)filehandle,
                                                                smackerflags,
                                                                SMACKAUTOEXTRA))) {
      SVidPlayEnd((HSVIDEO)videoptr);
      return FALSE;
    }
  }

  // COMPARE THE VIDEO SIZE TO THE SCREEN SIZE, AND FORCE OFF VERTICAL ZOOMING
  // AND/OR HORIZONTAL ZOOMING AS NECESSARY TO FIT THE VIDEO ON THE SCREEN
  if ((int)videoptr->smackerdata->file->Width*2 > screencx)
    flags &= ~SVID_FLAG_2XSIZE;
  videoptr->vertzoom = ((flags & SVID_FLAG_2XSIZE) != 0);
  if ((int)videoptr->smackerdata->file->Height*2 > screency)
    videoptr->vertzoom = FALSE;

  // DETERMINE THE TOP OUTPUT SCAN LINE
  if (flags & SVID_FLAG_FULLSCREEN)
    videoptr->smackerdata->top = (screency-(videoptr->smackerdata->file->Height*(videoptr->vertzoom+1)))/2;
  else
    videoptr->smackerdata->top = 0;

  // CREATE A SMACKER OFFSCREEN BUFFER
  if (flags & SVID_FLAG_2XSIZE) {
    videoptr->vertinterp = videoptr->vertzoom &&
                           (flags & SVID_FLAG_DOUBLESCANS) &&
                           (flags & SVID_FLAG_INTERPOLATE);
    if (!(videoptr->smackerdata->buffer =
            s_smackerapi->SmackBufferOpen((HWND)0,
                                          SMACKSTANDARDBLIT,
                                          videoptr->smackerdata->file->Width*(videoptr->vertinterp+1),
                                          videoptr->smackerdata->file->Height,
                                          videoptr->smackerdata->file->Width*(videoptr->vertinterp+1),
                                          videoptr->smackerdata->file->Height))) {
      SVidPlayEnd((HSVIDEO)videoptr);
      return FALSE;
    }
    s_smackerapi->SmackToBuffer(videoptr->smackerdata->file,
                                0,
                                0,
                                videoptr->smackerdata->file->Width*(videoptr->vertinterp+1),
                                videoptr->smackerdata->file->Height,
                                videoptr->smackerdata->buffer->Buffer,
                                0);
  }
  else
    videoptr->vertinterp = FALSE;

  // PROCESS AND SAVE THE FINAL FLAGS
  flags = (flags & ~SVID_FLAG_1XSIZE)
            | ((flags & SVID_FLAG_2XSIZE) ? 0 : SVID_FLAG_1XSIZE);
  if (flags & SVID_FLAG_1XSIZE)
    flags &= ~(SVID_FLAG_DOUBLESCANS | SVID_FLAG_INTERPOLATE | SVID_FLAG_INTERLACE);
  videoptr->flags = flags;

  // IF WE HAVE BEEN ASKED TO LINEAR INTERPOLATE BUT HAVE NOT BEEN GIVEN
  // AUTHORITY TO MODIFY THE PALETTE, THEN CREATE A LINEAR INTERPOLATION
  // TABLE BASED ON THE CURRENT PALETTE
  if ((flags & (SVID_FLAG_INTERPOLATE | SVID_FLAG_USECURRENTPALETTE))
        == (SVID_FLAG_INTERPOLATE | SVID_FLAG_USECURRENTPALETTE) &&
      SDrawGetFrameWindow()) {
    HDC          dc = GetDC(SDrawGetFrameWindow());
    PALETTEENTRY pe[256];
    GetSystemPaletteEntries(dc,0,256,&pe[0]);
    ReleaseDC(SDrawGetFrameWindow(),dc);
    CreateLinearInterpolationTable(videoptr,&pe[0]);
  }

  // INITIALIZE THE EXECUTE DATA
  videoptr->executedata.size = sizeof(SCODEEXECUTEDATA);

  // RETURN A HANDLE TO THE VIDEO
  *handle = (HSVIDEO)videoptr;

  // CLEAR THE VIDEO SURFACE
  if (flags & SVID_FLAG_CLEARSCREEN)
    SDrawClearSurface(SDRAW_SURFACE_FRONT);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SVidPlayBeginFromMemory (LPVOID             sourceptr,
                                       DWORD              sourcebytes,
                                       LPVOID             destbuffer,
                                       LPCRECT            destrect,
                                       LPSIZE             destsize,
                                       SVIDPALETTEUSEPTR  paletteuse,
                                       DWORD              flags,
                                       HSVIDEO           *handle) {
  if (handle)
    *handle = (HSVIDEO)0;

  VALIDATEBEGIN;
  VALIDATE(sourceptr);
  VALIDATE(sourcebytes);
  VALIDATE(destbuffer || !(flags & SVID_FLAG_TOBUFFER));
  VALIDATE(destsize   || !destbuffer);
  VALIDATE((!paletteuse) || (paletteuse->size == sizeof(SVIDPALETTEUSE)));
  VALIDATEEND;

  // SMACKER DOESN'T HAVE THE CAPABILITY TO PLAY FROM MEMORY, SO WE
  // SAVE THE DATA TO A TEMPORARY FILE ON DISK AND THEN ASK SMACKER
  // TO PLAY THAT.
  char temppath[MAX_PATH]     = ".";
  char tempfilename[MAX_PATH] = "";
  GetTempPath(MAX_PATH,temppath);
  GetTempFileName(temppath,"Vid",0,tempfilename);
  HANDLE tempfile = CreateFile(tempfilename,
                               GENERIC_READ | GENERIC_WRITE,
                               FILE_SHARE_READ | FILE_SHARE_WRITE,
                               NULL,
                               CREATE_ALWAYS,
                               FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
                               NULL);
  if (tempfile == INVALID_HANDLE_VALUE)
    return FALSE;
  DWORD byteswritten;
  WriteFile(tempfile,sourceptr,sourcebytes,&byteswritten,NULL);
  SetFilePointer(tempfile,0,NULL,FILE_BEGIN);
  BOOL result = SVidPlayBegin((LPCTSTR)tempfile,
                              destbuffer,
                              destrect,
                              destsize,
                              paletteuse,
                              flags | SVID_FLAG_FILEHANDLE | SVID_FLAG_PRELOAD,
                              handle);
  CloseHandle(tempfile);
  DeleteFile(tempfilename);

  return result;
}

//===========================================================================
BOOL APIENTRY SVidPlayContinue () {
  BOOL     moreframes = FALSE;
  VIDEOPTR currvideo  = s_videolist.Head();
  while (currvideo) {
    VIDEOPTR nextvideo = currvideo->Next();
    if (SVidPlayContinueSingle((HSVIDEO)currvideo,0,NULL))
      moreframes = TRUE;
    currvideo = nextvideo;
  }
  return moreframes;
}

//===========================================================================
BOOL APIENTRY SVidPlayContinueSingle (HSVIDEO  video,
                                      BOOL     forceupdate,
                                      BOOL    *updated) {
  if (updated)
    *updated = 0;

  VALIDATEBEGIN;
  VALIDATE(video);
  VALIDATEEND;

  if (!IsValidVideoHandle(video))
    return FALSE;
  VIDEOPTR videoptr = (VIDEOPTR)video;
  if (!(videoptr->smackerdata && videoptr->smackerdata->file))
    return FALSE;

  // UNLESS THE FORCE UPDATE FLAG IS ON, DO NOT PLAY THE NEXT FRAME
  // UNTIL THE REQUIRED DELAY HAS ELAPSED
  if ((!forceupdate) &&
      s_smackerapi->SmackWait(videoptr->smackerdata->file))
    return TRUE;

  // IF THE PALETTE HAS CHANGED, PROCESS THE NEW PALETTE
  LPPALETTEENTRY newpalette = NULL;
  if (videoptr->smackerdata->file->NewPalette &&
      !(videoptr->flags & SVID_FLAG_USECURRENTPALETTE)) {
    if (videoptr->smackerdata->buffer)
      s_smackerapi->SmackBufferNewPalette(videoptr->smackerdata->buffer,
                                             videoptr->smackerdata->file->Palette,
                                             0);
    PALETTEENTRY pe[256];
    int          index = 0;
    for (int loop = 0; loop < 256; ++loop) {
      pe[loop].peRed   = videoptr->smackerdata->file->Palette[index++];
      pe[loop].peGreen = videoptr->smackerdata->file->Palette[index++];
      pe[loop].peBlue  = videoptr->smackerdata->file->Palette[index++];
    }
    newpalette = &pe[0];
    if (videoptr->flags & SVID_FLAG_CLEARSCREEN)
      SDrawClearSurface(SDRAW_SURFACE_FRONT);
    if ((videoptr->flags & (SVID_FLAG_2XSIZE | SVID_FLAG_DOUBLESCANS)) == SVID_FLAG_2XSIZE)
      AdjustGamma(newpalette,0.8);
    if (videoptr->flags & SVID_FLAG_INTERPOLATE)
      CreateLinearInterpolationTable(videoptr,newpalette);
  }

  // UNCOMPRESS THE FRAME AND SET ITS PALETTE
  BOOL skip;
  if (videoptr->flags & SVID_FLAG_2XSIZE) {
    skip = s_smackerapi->SmackDoFrame(videoptr->smackerdata->file);
    if (newpalette)
      SDrawUpdatePalette(videoptr->palettefirstentry,
                         videoptr->palettenumentries,
                         newpalette+videoptr->palettefirstentry);
  }
  else {
    if (newpalette)
      SDrawUpdatePalette(videoptr->palettefirstentry,
                         videoptr->palettenumentries,
                         newpalette+videoptr->palettefirstentry);
    skip = ProcessFrameDirect(videoptr);
  }
  if (updated && !skip)
    *updated = TRUE;

  // IF WE JUST SKIPPED A FRAME AND STORM IS IN CHARGE OF SETTING THE
  // QUALITY, DEGRADE THE CURRENT QUALITY SETTING
  if (skip)
    if (videoptr->flags & SVID_FLAG_INTERPOLATE) {
      videoptr->flags      &= ~SVID_FLAG_INTERPOLATE;
      videoptr->vertinterp  = FALSE;
    }
    else
      videoptr->flags |= SVID_FLAG_INTERLACE;

  // IF THIS FRAME WAS NOT SKIPPED AND WAS NOT UNCOMPRESSED DIRECTLY
  // ONTO THE SCREEN, UPDATE THE SCREEN
  if ((videoptr->flags & SVID_FLAG_2XSIZE) && !skip)
    ProcessFrameEffects(videoptr);

  // STEP TO THE NEXT FRAME, AND RETURN A BOOLEAN TELLING WHETHER THERE
  // ARE MORE FRAMES TO BE PROCESSED
  if ((videoptr->smackerdata->file->FrameNum+1 < videoptr->smackerdata->file->Frames) ||
      (videoptr->flags & SVID_FLAG_LOOP)) {
    s_smackerapi->SmackNextFrame(videoptr->smackerdata->file);
    return TRUE;
  }
  else {
    SVidPlayEnd((HSVIDEO)videoptr);
    return FALSE;
  }
}

//===========================================================================
BOOL APIENTRY SVidPlayEnd (HSVIDEO video) {
  VALIDATEBEGIN;
  VALIDATE(video);
  VALIDATEEND;

  if (!IsValidVideoHandle(video))
    return FALSE;
  VIDEOPTR videoptr = (VIDEOPTR)video;
  if (!videoptr->smackerdata)
    return FALSE;

  // CLOSE THE FILE
  if (videoptr->smackerdata->buffer)
    s_smackerapi->SmackBufferClose(videoptr->smackerdata->buffer);
  if (videoptr->smackerdata->file)
    s_smackerapi->SmackClose(videoptr->smackerdata->file);
  FREE(videoptr->smackerdata);
  videoptr->smackerdata = NULL;
  if (videoptr->filehandle != INVALID_HANDLE_VALUE) {
    CloseHandle(videoptr->filehandle);
    videoptr->filehandle = INVALID_HANDLE_VALUE;
  }

  // DELETE THE INTERPOLATION TABLE
  if (videoptr->interptable) {
    FREE(videoptr->interptable);
    videoptr->interptable       = NULL;
    videoptr->executedata.table = NULL;
  }

  // DELETE THE ARRAY OF RECTANGLES
  FREEIFUSED(videoptr->rect);

  // REMOVE THE VIDEO RECORD FROM OUR LIST
  s_videolist.DeleteNode(videoptr);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SVidSetVolume (HSVIDEO video,
                             LONG    volume,
                             LONG    pan,
                             DWORD   track) {
  VALIDATEBEGIN;
  VALIDATE(video);
  VALIDATEEND;

  if (!IsValidVideoHandle(video))
    return FALSE;
  VIDEOPTR videoptr = (VIDEOPTR)video;
  if (!videoptr->smackerdata)
    return FALSE;

  // CONVERT THE VOLUME AND PAN PARAMETERS TO SMACKER FORMAT
  DWORD smackervolume = (DWORD)(volume+10000)*65536/20000;
  DWORD smackerpan    = (DWORD)(pan   +10000)*65536/20000;

  // CONVERT THE TRACK PARAMETER TO SMACKER FORMAT
  DWORD smackertracks;
  switch (track) {
    case 1:  smackertracks = SMACKTRACK1;  break;
    case 2:  smackertracks = SMACKTRACK2;  break;
    case 3:  smackertracks = SMACKTRACK3;  break;
    case 4:  smackertracks = SMACKTRACK4;  break;
    case 5:  smackertracks = SMACKTRACK5;  break;
    case 6:  smackertracks = SMACKTRACK6;  break;
    case 7:  smackertracks = SMACKTRACK7;  break;
    default: smackertracks = SMACKTRACKS;  break;
  }

  // CALL SMACKER TO SET THE VOLUME AND PAN FOR THE REQUESTED TRACK(S)
  s_smackerapi->SmackVolumePan(videoptr->smackerdata->file,
                               smackertracks,
                               smackervolume,
                               smackerpan);

  return TRUE;
}

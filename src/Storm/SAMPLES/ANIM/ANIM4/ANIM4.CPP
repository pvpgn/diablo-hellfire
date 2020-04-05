/****************************************************************************
*
*  ANIM4.CPP
*
*  This sample application introduces the use of dirty rectangles.
*
*  The application keeps an array of dirty cells, each one representing a
*  16x16 pixel area of the screen.  Each cell contains one bit telling
*  whether the cell has been updated this frame, and another bit telling
*  whether it was updated last frame.  For each frame we need to blit to
*  the screen cells that have either bit set, so that sprites will be erased
*  from their old positions and drawn to their new positions.
*
*  The screen drawing function starts by clearing the update bits in each
*  cell.  Next, it erases the sprites from their old positions on the
*  offscreen buffer, using an erase mask which will be described later.
*  Next, it draws the sprites to their new positions on the offscreen buffer,
*  and marks each cell that's touched by a sprite as dirty.  Finally, it
*  creates an update mask which is the intersection of the screen mask
*  (that portion of the screen which is not covered by the status bar) and
*  the array of dirty cells.  It uses this update mask to blit the offscreen
*  buffer onto the screen.  Then it saves the update mask to use as the
*  erase mask for the next frame.
*
*  In addition to the use of a dirty rectangle system, the fact that we have
*  a screen update mask which we are able to use as an erase mask for the
*  next frame substantially decreases the amount of time required to erase
*  the offscreen buffer.  There is never a case when a portion of the
*  offscreen buffer is erased more than once, as in Anim3.  Also, the
*  application never wastes time erasing the portion of the offscreen buffer
*  which is covered up by the status bar.
*
***/

#include <windows.h>
#include <storm.h>
#include <math.h>

#define  SPRITES  4

#define  SIN(a)  sintable[((a) & 255)]
#define  COS(a)  sintable[(((a)+64) & 255)]

static LPBYTE    backgroundbuffer = NULL;
static HSTRANS   baseupdatemask   = 0;
static HSTRANS   currupdatemask   = 0;
static LPBYTE    dirtycells       = NULL;
static HSGDIFONT font             = 0;
static LPBYTE    offscreenbuffer  = NULL;
static HSTRANS   overlay          = 0;
static int       sintable[256];
static HSTRANS   sprite[SPRITES]  = {0,0,0,0};

//===========================================================================
static BOOL CreateBackgroundBuffer () {
  backgroundbuffer = (LPBYTE)ALLOC(640*480);
  if (!backgroundbuffer)
    return 0;
  PALETTEENTRY pe[256];
  if (!SBmpLoadImage("..\\demodata\\bkg.pcx",&pe[0],backgroundbuffer,640*480))
    return 0;
  SDrawUpdatePalette(0,256,&pe[0]);
  return 1;
}

//===========================================================================
static BOOL CreateDirtyCellArray () {
  dirtycells = (LPBYTE)ALLOC(40*30);
  if (!dirtycells)
    return 0;
  ZeroMemory(dirtycells,40*30);
  STransSetDirtyArrayInfo(640,480,16,16);
  return 1;
}

//===========================================================================
static BOOL CreateOffscreenBuffer () {
  offscreenbuffer = (LPBYTE)ALLOC(640*480);
  if (!offscreenbuffer)
    return 0;
  CopyMemory(offscreenbuffer,backgroundbuffer,640*480);
  return 1;
}

//===========================================================================
static void CreateSinTable () {
  for (int loop = 0; loop < 256; ++loop) {
    double angle = (loop*3.14159265359)/128.0;
    sintable[loop] = (int)(sin(angle)*128);
  }
}

//===========================================================================
static void DisplayFramesPerSecond (LPBYTE buffer, int pitch) {
  static DWORD frames   = 0;
  static DWORD lastfps  = 0;
  static DWORD lasttime = GetTickCount();

  // UPDATE THE NUMBER OF FRAMES THIS SECOND
  if (GetTickCount()-lasttime >= 1000) {
    lastfps   = frames;
    frames    = 0;
    lasttime += 1000;
  }
  ++frames;

  // DISPLAY THE LAST NUMBER OF FRAMES PER SECOND
  char outstr[16];
  wsprintf(outstr,"%3u FPS",lastfps);
  RECT rect = {0,20,60,32};
  SGdiSetPitch(pitch);
  SGdiExtTextOut(buffer,
                 0,
                 20,
                 &rect,
                 0,
                 ETO_TEXT_WHITE,
                 ETO_BKG_BLACK,
                 outstr);

}

//===========================================================================
static BOOL CALLBACK IdleProc (DWORD) {
  static BYTE dirtyvalue = 1;

  // [1] BLT THE BACKGROUND BUFFER OVER THE DIRTY PORTIONS OF THE OFFSCREEN
  //     BUFFER
  STransBltUsingMask(offscreenbuffer,backgroundbuffer,640,640,currupdatemask);

  // [2] CLEAR THE ARRAY OF DIRTY CELLS
  dirtyvalue = 3-dirtyvalue;
  {
    for (int loop = 0; loop < 40*30; ++loop)
      *(dirtycells+loop) &= ~dirtyvalue;
  }

  // [3] DRAW THE SPRITES ONTO THE OFFSCREEN BUFFER, UPDATING THE ARRAY OF
  //     DIRTY CELLS
  {
    static BYTE distance = 0;
    static BYTE rotation = 0;
    for (int loop = 0; loop < SPRITES; ++loop) {
      int x = 170+(SIN(rotation+64*loop)*SIN(distance/2)/100);
      int y = 165+(COS(rotation+64*loop)*SIN(distance/2)/100);
      STransBlt(offscreenbuffer,
                x,
                y,
                640,
                sprite[loop]);
      STransUpdateDirtyArray(dirtycells,
                             dirtyvalue,
                             x,
                             y,
                             sprite[loop],
                             0);
    }
    ++distance;
    rotation += 3;
  }

  // [4] DELETE THE OLD UPDATE MASK, AND CREATE A NEW ONE WHICH IS THE
  //     INTERSECTION OF THE ARRAY OF THE CURRENTLY DIRTY CELLS AND THE
  //     AREA NOT COVERED BY THE OVERLAY
  STransDelete(currupdatemask);
  STransIntersectDirtyArray(baseupdatemask,dirtycells,3,&currupdatemask);

  // [5] BLT THE OFFSCREEN BUFFER ONTO THE SCREEN USING THE NEW MASK
  {
    LPBYTE videobuffer;
    int    videopitch;
    if (SDrawLockSurface(SDRAW_SURFACE_FRONT,NULL,&videobuffer,&videopitch)) {
      STransBltUsingMask(videobuffer,
                         offscreenbuffer,
                         videopitch,
                         640,
                         currupdatemask);
      DisplayFramesPerSecond(videobuffer,videopitch);
      SDrawUnlockSurface(SDRAW_SURFACE_FRONT,videobuffer);
    }
  }

  return TRUE;
}

//===========================================================================
static BOOL LoadFont () {
  {
    HFONT winfont = CreateFont(-12,0,0,0,FW_BOLD,0,0,0,ANSI_CHARSET,
                               OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
                               VARIABLE_PITCH | FF_SWISS,TEXT("Arial"));
    if (!SGdiImportFont(winfont,&font))
      return 0;
    DeleteObject(winfont);
  }
  if (!SGdiSelectObject(font))
    return 0;
  return 1;
}

//===========================================================================
static BOOL LoadOverlay () {
  LPBYTE temp = (LPBYTE)ALLOC(640*480);
  if (!SBmpLoadImage("..\\demodata\\overlay.pcx",NULL,temp,640*480))
    return 0;
  if (!STransCreate(temp,640,480,8,NULL,PALETTEINDEX(111),&overlay))
    return 0;
  if (!STransInvertMask(overlay,&baseupdatemask))
    return 0;
  if (!STransDuplicate(baseupdatemask,&currupdatemask))
    return 0;
  FREE(temp);
  return 1;
}

//===========================================================================
static BOOL LoadSprites () {
  LPBYTE temp = (LPBYTE)ALLOC(300*500);
  if (!SBmpLoadImage("..\\demodata\\sprites.pcx",NULL,temp,300*500))
    return 0;
  for (int loop = 0; loop < SPRITES; ++loop) {
    RECT rect = {0,loop*125,299,loop*125+124};
    if (!STransCreate(temp,300,500,8,&rect,PALETTEINDEX(*temp),&sprite[loop]))
      return 0;
  }
  FREE(temp);
  return 1;
}

//===========================================================================
static void CALLBACK OnClose (LPPARAMS) {
  FREE(backgroundbuffer);
  FREE(dirtycells);
  FREE(offscreenbuffer);
  SGdiDeleteObject(font);
  STransDelete(overlay);
  STransDelete(baseupdatemask);
  STransDelete(currupdatemask);
  for (int loop = 0; loop < SPRITES; ++loop)
    STransDelete(sprite[loop]);
}

//===========================================================================
static void CALLBACK OnPaint (LPPARAMS params) {
  LPBYTE videobuffer;
  int    videopitch;
  if (SDrawLockSurface(SDRAW_SURFACE_FRONT,NULL,&videobuffer,&videopitch)) {
    STransBltUsingMask(videobuffer,
                       offscreenbuffer,
                       videopitch,
                       640,
                       baseupdatemask);
    STransBlt(videobuffer,
              0,
              0,
              videopitch,
              overlay);
    SDrawUnlockSurface(SDRAW_SURFACE_FRONT,videobuffer);
  }
  ValidateRect(params->window,NULL);
  params->useresult = TRUE;
  params->result    = 0;
}

//===========================================================================
static void CALLBACK OnVkEscape (LPPARAMS) {
  SDrawPostClose();
}

//===========================================================================
int APIENTRY WinMain (HINSTANCE instance, HINSTANCE, LPSTR, int) {
  CreateSinTable();
  if (!SDrawAutoInitialize(instance,
                           TEXT("ANIM4"),
                           TEXT("Animation Example 4")))
    FATALRESULT("SDrawAutoInitialize()");
  if (!CreateBackgroundBuffer())
    FATALRESULT("CreateBackgroundBuffer()");
  if (!CreateDirtyCellArray())
    FATALRESULT("CreateDirtyCellArray()");
  if (!CreateOffscreenBuffer())
    FATALRESULT("CreateOffsreenBuffer()");
  if (!LoadFont())
    FATALRESULT("LoadFont()");
  if (!LoadOverlay())
    FATALRESULT("LoadOverlay()");
  if (!LoadSprites())
    FATALRESULT("LoadSprites()");
  SMsgRegisterMessage(NULL,WM_CLOSE ,OnClose);
  SMsgRegisterMessage(NULL,WM_PAINT ,OnPaint);
  SMsgRegisterKeyDown(NULL,VK_ESCAPE,OnVkEscape);
  return SMsgDoMessageLoop(IdleProc);
}

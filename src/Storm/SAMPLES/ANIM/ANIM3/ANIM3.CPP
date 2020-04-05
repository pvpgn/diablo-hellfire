/****************************************************************************
*
*  ANIM3.CPP
*
*  This sample application improves on the performance of Anim2 by reducing
*  the amount of time it has to spend repainting the offscreen buffer.
*  Instead of erasing the entire offscreen buffer with the background every
*  frame, it only erases those portions that were covered by a sprite last
*  frame.
*
*  A weakness of this sample application is that it doesn't test whether
*  a portion of the offscreen buffer has already been erased before erasing
*  it again.  This means its performance will degrade substantially as more
*  of the screen gets covered by sprites, potentially at some point becoming
*  slower than Anim2.
*
***/

#include <windows.h>
#include <storm.h>
#include <math.h>

#define  SPRITES  4

#define  SIN(a)  sintable[((a) & 255)]
#define  COS(a)  sintable[(((a)+64) & 255)]

static LPBYTE    backgroundbuffer = NULL;
static HSGDIFONT font             = 0;
static LPBYTE    offscreenbuffer  = NULL;
static HSTRANS   overlay          = 0;
static HSTRANS   updatemask       = 0;
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
  static BYTE distance = 0;
  static BYTE rotation = 0;

  // ERASE EACH SPRITE THAT WAS DRAWN LAST FRAME FROM THE OFFSCREEN BUFFER
  // BY OVERWRITING IT WITH DATA FROM THE BACKGROUND
  {
    for (int loop = 0; loop < SPRITES; ++loop) {
      int x = 170+(SIN(rotation+64*loop)*SIN(distance/2)/100);
      int y = 165+(COS(rotation+64*loop)*SIN(distance/2)/100);
      STransBltUsingMask(offscreenbuffer+y*640+x,
                         backgroundbuffer+y*640+x,
                         640,
                         640,
                         sprite[loop]);
    }
  }

  // DRAW EACH SPRITE IN ITS NEW POSITION INTO THE OFFSCREEN BUFFER
  ++distance;
  rotation += 3;
  {
    for (int loop = 0; loop < SPRITES; ++loop) {
      int x = 170+(SIN(rotation+64*loop)*SIN(distance/2)/100);
      int y = 165+(COS(rotation+64*loop)*SIN(distance/2)/100);
      STransBlt(offscreenbuffer,
                x,
                y,
                640,
                sprite[loop]);
    }
  }

  // DRAW THE FRAMES PER SECOND COUNTER INTO THE OFFSCREEN BUFFER
  DisplayFramesPerSecond(offscreenbuffer,640);

  // DRAW THE OFFSCREEN BUFFER ONTO THE SCREEN, BLITTING AROUND THE
  // STATUS BAR
  {
    LPBYTE videobuffer;
    int    videopitch;
    if (SDrawLockSurface(SDRAW_SURFACE_FRONT,NULL,&videobuffer,&videopitch)) {
      STransBltUsingMask(videobuffer,
                         offscreenbuffer,
                         videopitch,
                         640,
                         updatemask);
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
  if (!STransInvertMask(overlay,&updatemask))
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
  FREE(offscreenbuffer);
  SGdiDeleteObject(font);
  STransDelete(overlay);
  STransDelete(updatemask);
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
                       updatemask);
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
                           TEXT("ANIM3"),
                           TEXT("Animation Example 3")))
    FATALRESULT("SDrawAutoInitialize()");
  if (!CreateBackgroundBuffer())
    FATALRESULT("CreateBackgroundBuffer()");
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

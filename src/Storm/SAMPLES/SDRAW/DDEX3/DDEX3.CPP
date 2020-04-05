/****************************************************************************
*
*  DDEX3.CPP
*
*  This sample application demonstrates the use of transparent bitmaps.
*  For each frame, the application performs the following steps:
*
*  1. Draws the background on an offscreen video page
*  2. Overlays transparent bitmaps onto the background
*  3. Flips the video page
*
*  The use of page flipping prevents tearing.
*
***/

#include <windows.h>
#include <storm.h>
#include <math.h>

#define  SPRITES  4

#define  SIN(a)  sintable[((a) & 255)]
#define  COS(a)  sintable[(((a)+64) & 255)]

LPBYTE  backgroundbitmap = NULL;
int     backgroundwidth  = 0;
int     backgroundheight = 0;
BYTE    rotation         = 0;
int     sintable[256];
HSTRANS sprite[SPRITES]  = {0,0,0,0};
int     tilingoffset     = 0;

//===========================================================================
void CreateSinTable () {
  for (int loop = 0; loop < 256; ++loop) {
    double angle = (loop*3.14159265359)/128.0;
    sintable[loop] = (int)(sin(angle)*128);
  }
}

//===========================================================================
BOOL CALLBACK IdleProc (DWORD) {
  LPBYTE videobuffer;
  int    pitch;
  if (SDrawLockSurface(SDRAW_SURFACE_BACK,NULL,&videobuffer,&pitch)) {
    RECT destrect   = {0,0,639,479};
    RECT sourcerect = {0,0,backgroundwidth-1,backgroundheight-1};
    SBltROP3Tiled(videobuffer,&destrect,pitch,
                  backgroundbitmap,&sourcerect,backgroundwidth,
                  tilingoffset,tilingoffset,0,SRCCOPY);
    for (int loop = 0; loop < SPRITES; ++loop)
      STransBlt(videobuffer,
                170+SIN(rotation+64*loop),
                165+COS(rotation+64*loop),
                pitch,
                sprite[loop]);
    SDrawUnlockSurface(SDRAW_SURFACE_BACK,videobuffer);
    SDrawFlipPage();
  }
  rotation     += 3;
  tilingoffset += 8;
  return 1;
}

//===========================================================================
BOOL LoadBackgroundBitmap () {
  if (!SBmpLoadImage("..\\demodata\\flag.pcx",NULL,NULL,NULL,&backgroundwidth,&backgroundheight))
    return 0;
  backgroundbitmap = (LPBYTE)ALLOC(backgroundwidth*backgroundheight);
  PALETTEENTRY pe[256];
  if (!SBmpLoadImage("..\\demodata\\flag.pcx",&pe[0],backgroundbitmap,backgroundwidth*backgroundheight))
    return 0;
  SDrawUpdatePalette(0,256,&pe[0]);
  return 1;
}

//===========================================================================
BOOL LoadSprites () {
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
void CALLBACK OnClose (LPPARAMS) {
  FREE(backgroundbitmap);
  for (int loop = 0; loop < SPRITES; ++loop)
    STransDelete(sprite[loop]);
}

//===========================================================================
void CALLBACK OnVkEscape (LPPARAMS) {
  SDrawPostClose();
}

//===========================================================================
int APIENTRY WinMain (HINSTANCE instance, HINSTANCE, LPSTR, int) {
  CreateSinTable();
  if (!SDrawAutoInitialize(instance,
                           TEXT("DDEX3"),
                           TEXT("DirectDraw Example 3"),
                           NULL,
                           SDRAW_SERVICE_PAGEFLIP))
    FATALRESULT("SDrawAutoInitialize");
  if (!LoadBackgroundBitmap())
    FATALRESULT("LoadBackgroundBitmap");
  if (!LoadSprites())
    FATALRESULT("LoadSprites");
  SMsgRegisterMessage(NULL,WM_CLOSE ,OnClose);
  SMsgRegisterKeyDown(NULL,VK_ESCAPE,OnVkEscape);
  return SMsgDoMessageLoop(IdleProc);
}

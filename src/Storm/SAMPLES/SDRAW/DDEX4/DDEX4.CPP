/****************************************************************************
*
*  DDEX4.CPP
*
*  This sample application produces the same output as DDEX3, but it goes
*  about it a different way.  Instead of drawing to an offscreen page and
*  then page flipping, it draws into system memory and the blts the entire
*  screen at once onto video memory.  This allows it to work on video cards
*  that don't support page flipping in high-res mode.
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
  LPBYTE systembuffer;
  int    pitch;
  if (SDrawLockSurface(SDRAW_SURFACE_SYSTEM,NULL,&systembuffer,&pitch)) {
    RECT destrect   = {0,0,639,479};
    RECT sourcerect = {0,0,backgroundwidth-1,backgroundheight-1};
    SBltROP3Tiled(systembuffer,&destrect,pitch,
                  backgroundbitmap,&sourcerect,backgroundwidth,
                  tilingoffset,tilingoffset,0,SRCCOPY);
    for (int loop = 0; loop < SPRITES; ++loop)
      STransBlt(systembuffer,
                170+SIN(rotation+64*loop),
                165+COS(rotation+64*loop),
                pitch,
                sprite[loop]);
    SDrawUnlockSurface(SDRAW_SURFACE_SYSTEM,systembuffer);
    SDrawUpdateScreen(NULL);
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
                           TEXT("DDEX4"),
                           TEXT("DirectDraw Example 4"),
                           NULL,
                           SDRAW_SERVICE_DOUBLEBUFFER))
    FATALRESULT("SDrawAutoInitialize");
  if (!LoadBackgroundBitmap())
    FATALRESULT("LoadBackgroundBitmap");
  if (!LoadSprites())
    FATALRESULT("LoadSprites");
  SMsgRegisterMessage(NULL,WM_CLOSE ,OnClose);
  SMsgRegisterKeyDown(NULL,VK_ESCAPE,OnVkEscape);
  return SMsgDoMessageLoop(IdleProc);
}

/****************************************************************************
*
*  DDEX1.CPP
*
*  This simple DirectDraw application loads a bitmap and draws it on the
*  screen.
*
***/

#include <windows.h>
#include <storm.h>

LPBYTE backgroundbitmap = NULL;
int    backgroundwidth  = 0;
int    backgroundheight = 0;

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
void CALLBACK OnClose (LPPARAMS) {
  FREE(backgroundbitmap);
}

//===========================================================================
void CALLBACK OnPaint (LPPARAMS) {
  LPBYTE videobuffer;
  int    videopitch;
  if (SDrawLockSurface(SDRAW_SURFACE_FRONT,NULL,&videobuffer,&videopitch)) {
    SGdiSetPitch(videopitch);
    SGdiRectangle(videobuffer,0,0,639,479,PALETTEINDEX(255));
    SGdiBitBlt(videobuffer,0,0,backgroundbitmap,NULL,backgroundwidth,backgroundheight);
    SDrawUnlockSurface(SDRAW_SURFACE_FRONT,videobuffer);
  }
}

//===========================================================================
void CALLBACK OnVkEscape (LPPARAMS) {
  SDrawPostClose();
}

//===========================================================================
int APIENTRY WinMain (HINSTANCE instance, HINSTANCE, LPSTR, int) {
  if (!SDrawAutoInitialize(instance,
                           TEXT("DDEX1"),
                           TEXT("DirectDraw Example 1")))
    FATALRESULT("SDrawAutoInitialize");
  if (!LoadBackgroundBitmap())
    FATALRESULT("LoadBackgroundBitmap");
  SMsgRegisterMessage(NULL,WM_CLOSE ,OnClose);
  SMsgRegisterMessage(NULL,WM_PAINT ,OnPaint);
  SMsgRegisterKeyDown(NULL,VK_ESCAPE,OnVkEscape);
  return SMsgDoMessageLoop(NULL);
}

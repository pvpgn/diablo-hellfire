/****************************************************************************
*
*  DDEX2.CPP
*
*  This sample application loads a bitmap and repeatedly tiles it onto the
*  screen, changing the tiling offset each frame.
*
***/

#include <windows.h>
#include <storm.h>

LPBYTE backgroundbitmap = NULL;
int    backgroundwidth  = 0;
int    backgroundheight = 0;
int    tilingoffset     = 0;

//===========================================================================
BOOL CALLBACK IdleProc (DWORD) {
  LPBYTE videobuffer;
  int    pitch;
  if (SDrawLockSurface(SDRAW_SURFACE_FRONT,NULL,&videobuffer,&pitch)) {
    RECT destrect   = {0,0,639,479};
    RECT sourcerect = {0,0,backgroundwidth-1,backgroundheight-1};
    SBltROP3Tiled(videobuffer,&destrect,pitch,
                  backgroundbitmap,&sourcerect,backgroundwidth,
                  tilingoffset,tilingoffset,0,SRCCOPY);
    SDrawUnlockSurface(SDRAW_SURFACE_FRONT,videobuffer);
  }
  tilingoffset += 4;
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
void CALLBACK OnClose (LPPARAMS) {
  FREE(backgroundbitmap);
}

//===========================================================================
void CALLBACK OnVkEscape (LPPARAMS) {
  SDrawPostClose();
}

//===========================================================================
int APIENTRY WinMain (HINSTANCE instance, HINSTANCE, LPSTR, int) {
  if (!SDrawAutoInitialize(instance,
                           TEXT("DDEX2"),
                           TEXT("DirectDraw Example 2")))
    FATALRESULT("SDrawAutoInitialize");
  if (!LoadBackgroundBitmap())
    FATALRESULT("LoadBackgroundBitmap");
  SMsgRegisterMessage(NULL,WM_CLOSE ,OnClose);
  SMsgRegisterKeyDown(NULL,VK_ESCAPE,OnVkEscape);
  return SMsgDoMessageLoop(IdleProc);
}

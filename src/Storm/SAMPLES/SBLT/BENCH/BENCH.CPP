/****************************************************************************
*
*  BENCH.CPP
*
*  Benchmarks Storm's bitblt performance.
*
***/

#include <windows.h>
#include <storm.h>

#define  ALIGNED

LPBYTE    bitmap     = NULL;
HSGDIFONT font       = (HSGDIFONT)0;
BOOL      paused     = 0;
DWORD     iterations = 0;

//===========================================================================
BOOL CALLBACK IdleProc (DWORD) {
  if (paused)
    return 0;

  LPBYTE videobuffer;
  int    pitch;
  if (SDrawLockSurface(SDRAW_SURFACE_FRONT,NULL,&videobuffer,&pitch)) {
    for (int loop = 0; loop < 100; ++loop) {
      int left = (rand() % 640)-20;
      int top  = (rand() % 450)+12;
#ifdef ALIGNED
      left = (left >> 2) << 2;
#endif
      SGdiSetPitch(pitch);
      SGdiBitBlt(videobuffer,left,top,bitmap,NULL,40,40);
      ++iterations;
    }
    SDrawUnlockSurface(SDRAW_SURFACE_FRONT,videobuffer);
  }
  return 1;
}

//===========================================================================
BOOL LoadBitmap () {
  PALETTEENTRY pe[256];
  if (!SBmpAllocLoadImage("blizlogo.pcx",&pe[0],&bitmap))
    return 0;
  SDrawUpdatePalette(0,256,&pe[0]);
  return 1;
}

//===========================================================================
BOOL LoadFont () {
  {
    HFONT winfont = CreateFont(-10,0,0,0,FW_BOLD,0,0,0,ANSI_CHARSET,
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
void CALLBACK OnClose (LPPARAMS params) {
  KillTimer(params->window,1);
  SGdiDeleteObject(font);
  FREE(bitmap);
}

//===========================================================================
void CALLBACK OnTimer (LPPARAMS) {
  LPBYTE videobuffer;
  int    pitch;
  if (SDrawLockSurface(SDRAW_SURFACE_FRONT,NULL,&videobuffer,&pitch)) {
    char outstr[64];
    wsprintf(outstr,"%u bitblts per second",iterations);
    RECT rect = {0,0,320,12};
    SGdiSetPitch(pitch);
    SGdiExtTextOut(videobuffer,
                   0,
                   0,
                   &rect,
                   0,
                   ETO_TEXT_WHITE,
                   ETO_BKG_BLACK,
                   outstr);
    SDrawUnlockSurface(SDRAW_SURFACE_FRONT,videobuffer);
  }
  iterations = 0;
}

//===========================================================================
void CALLBACK OnVkEscape (LPPARAMS) {
  SDrawPostClose();
}

//===========================================================================
void CALLBACK OnVkSpace (LPPARAMS) {
  paused = !paused;
}

//===========================================================================
int APIENTRY WinMain (HINSTANCE instance, HINSTANCE, LPSTR, int) {
  if (!SDrawAutoInitialize(instance,
                           TEXT("DDBENCH"),
                           TEXT("Benchmark")))
    return 1;
  if (!LoadBitmap())
    return 1;
  if (!LoadFont())
    return 1;
  SetTimer(SDrawGetFrameWindow(),1,1000,NULL);
  SMsgRegisterMessage(NULL,WM_CLOSE ,OnClose);
  SMsgRegisterMessage(NULL,WM_TIMER ,OnTimer);
  SMsgRegisterKeyDown(NULL,VK_ESCAPE,OnVkEscape);
  SMsgRegisterKeyDown(NULL,VK_SPACE ,OnVkSpace);
  return SMsgDoMessageLoop(IdleProc);
}

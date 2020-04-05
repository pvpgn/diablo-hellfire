/****************************************************************************
*
*  DIABLOUI.CPP
*  Diablo UI library common code
*
*  By Michael O'Brien (9/12/96)
*
***/

#define OEMRESOURCE

#include "pch.h"
#include "artfont.h"
#include "uisnd.h"


HINSTANCE global_hinstance  = (HINSTANCE)0;

LPBYTE    ui_background     = NULL;
SIZE      ui_backgroundsize = {0,0};


extern BOOL LoadGlobalHeroArt(void);
extern void FreeGlobalHeroArt(void);

//****************************************************************************
//****************************************************************************
static BOOL sgSpawned = FALSE;


/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
BOOL UiIsSpawned() {
	return sgSpawned;
}


//===========================================================================
void APIENTRY UiOnPaint (LPPARAMS params) {
#if 0
  LPBYTE videobuffer;
  int    pitch;
  if (SDrawLockSurface(SDRAW_SURFACE_FRONT,NULL,&videobuffer,&pitch)) {
    int screencx, screency;
    SDrawGetScreenSize(&screencx,&screency,NULL);
    if (ui_background) {
      RECT destrect   = {0,0,screencx-1,screency-1};
      RECT sourcerect = {0,0,ui_backgroundsize.cx-1,ui_backgroundsize.cy-1};
      SBltROP3Tiled(videobuffer,
                    &destrect,
                    pitch,
                    ui_background,
                    &sourcerect,
                    ui_backgroundsize.cx,
                    0,
                    0,
                    0,
                    SRCCOPY);
    }
    else
      SGdiRectangle(videobuffer,0,0,screencx-1,screency-1,PALETTEINDEX(0));
    SDrawUnlockSurface(SDRAW_SURFACE_FRONT,videobuffer);
  }
#endif
}

//===========================================================================
BOOL APIENTRY UiSetBackgroundBitmap (HWND           window,
                                     LPPALETTEENTRY palette,
                                     LPBYTE         bitmapbits,
                                     int            width,
                                     int            height) {
  ui_background        = bitmapbits;
  ui_backgroundsize.cx = width;
  ui_backgroundsize.cy = height;
  return SDrawUpdatePalette(10,236,palette+10);
}


//===========================================================================
//===========================================================================
void APIENTRY UiSetSpawned(BOOL spawned) {
	sgSpawned = spawned;
}


//===========================================================================
//===========================================================================
static BOOL sgInUI;
void APIENTRY UiInitialize(void) {
	sgInUI = TRUE;

	UiSndInit();
	ArtFontInit();
	LoadGlobalHeroArt();

	UiLoadCursor();
}


//===========================================================================
//===========================================================================
void APIENTRY UiDestroy() {

	UiDestroyCursor();
	FreeGlobalHeroArt();

	sgInUI = FALSE;
}


//===========================================================================
//===========================================================================
void APIENTRY UiAppActivate(BOOL activating) {
	if (! sgInUI) return;

}


//===========================================================================
//===========================================================================
//===========================================================================
extern "C" BOOL APIENTRY DllMain (HINSTANCE instance, DWORD reason, LPVOID) {
	switch (reason) {
		case DLL_PROCESS_ATTACH: 
			global_hinstance = instance;
			DiabEditRegisterClass();
			UiCreateIdentityPal();
			break;
		case DLL_PROCESS_DETACH:
			UiFreeIdentityPal();
			break;
	}
  return 1;
}


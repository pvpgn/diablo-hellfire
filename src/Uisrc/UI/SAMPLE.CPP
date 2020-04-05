/****************************************************************************
*
*  SAMPLE.CPP
*  Diablo UI sample dialog
*
*  By Michael O'Brien (9/12/96)
*
***/

#include "pch.h"

static SIZE bgsize;
static LPBYTE backgroundbitmap = NULL;
static LPBYTE buttonbitmap     = NULL;

//===========================================================================
static void DestroyArtwork (HWND window) {
  if (buttonbitmap) {
    FREE(buttonbitmap);
    buttonbitmap = NULL;
  }
  if (backgroundbitmap) {
    FREE(backgroundbitmap);
    backgroundbitmap = NULL;
  }
}

//===========================================================================
static BOOL LoadArtwork (HWND window) {
	SIZE artsize;

	LoadArtFile(
		window,
		NULL,
		TEXT(""),
		SDLG_STYLE_ANY,
		SDLG_USAGE_BACKGROUND,
		TEXT("ui_art\\samp_bkg.pcx"),
		&backgroundbitmap,
		&bgsize
	);

	LoadArtFile(
		NULL,
		window,
		TEXT("Button"),
		SDLG_STYLE_ANY,
		SDLG_USAGE_BACKGROUND,
		TEXT("ui_art\\samp_btn.pcx"),
		&buttonbitmap,
		&artsize
	);

	return 1;
}

//===========================================================================
static BOOL CALLBACK SampleDialogProc (HWND   window,
                                       UINT   message,
                                       WPARAM wparam,
                                       LPARAM lparam) {
  switch (message) {

    case WM_COMMAND:
      switch (LOWORD(wparam)) {

        case IDOK:
          SDlgEndDialog(window,1);
          break;

        case IDCANCEL:
          SDlgEndDialog(window,0);
          break;

      }
      break;

    case WM_DESTROY:
      DestroyArtwork(window);
      break;

    case WM_INITDIALOG:
      LoadArtwork(window);
      return 1;

  }
  return SDlgDefDialogProc(window,message,wparam,lparam);
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
BOOL APIENTRY UiSampleDialog (DWORD *selection) {
  if (!selection)
    return 0;
  *selection = 0;

  // DISPLAY THE DIALOG BOX
  *selection = (DWORD)SDlgDialogBox(global_hinstance,
                                    TEXT("SAMPLE_DIALOG"),
                                    SDrawGetFrameWindow(),
                                    SampleDialogProc);

  return 1;
}

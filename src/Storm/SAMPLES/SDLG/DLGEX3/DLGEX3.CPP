/****************************************************************************
*
*  DLGEX3.CPP
*
*  Demonstrates the use of background bitmaps.
*
*  This sample displays the same simple dialog box again.  However, it
*  has been modified to use a background bitmap.  It loads the bitmap from
*  disk during the WM_INITDIALOG message, and registers it with Storm's
*  dialog manager.  It frees the bitmap on WM_DESTROY.
*
***/

#define  STRICT
#include <windows.h>
#include <storm.h>
#include "resource.h"

LPBYTE LoadBitmap (LPCTSTR filename, int *width, int *height);

//===========================================================================
BOOL CALLBACK DlgProc (HWND   window,
                       UINT   message,
                       WPARAM wparam,
                       LPARAM lparam) {
  static LPBYTE backgroundbitmap = NULL;
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
      if (backgroundbitmap) {
        FREE(backgroundbitmap);
        backgroundbitmap = NULL;
      }
      break;

    case WM_INITDIALOG:
      {
        int width, height;
        backgroundbitmap = LoadBitmap(TEXT("bkg1.pcx"),&width,&height);
        if (backgroundbitmap)
          SDlgSetBitmap(window,
                        NULL,
                        NULL,
                        SDLG_STYLE_ANY,
                        SDLG_USAGE_BACKGROUND,
                        backgroundbitmap,
                        NULL,
                        width,
                        height);
      }
      return 1;

  }
  return SDlgDefDialogProc(window,message,wparam,lparam);
}

//===========================================================================
LPBYTE LoadBitmap (LPCTSTR filename, int *width, int *height) {
  if (!SBmpLoadImage(filename,NULL,NULL,0,width,height))
    return NULL;
  LPBYTE bitmapbits = (LPBYTE)ALLOC((*width)*(*height));
  if (!bitmapbits)
    return NULL;
  PALETTEENTRY pe[256];
  if (SBmpLoadImage(filename,&pe[0],bitmapbits,(*width)*(*height)))
    SDrawUpdatePalette(10,236,&pe[10]);
  return bitmapbits;
}

//===========================================================================
int APIENTRY WinMain (HINSTANCE instance, HINSTANCE, LPSTR, int) {
  if (!SDrawAutoInitialize(instance,
                           TEXT("DLGEX3"),
                           TEXT("Dialog Example 3")))
    return 1;
  SDlgDialogBox(instance,
                MAKEINTRESOURCE(IDD_DIALOG1),
                SDrawGetFrameWindow(),
                DlgProc);
  return 0;
}

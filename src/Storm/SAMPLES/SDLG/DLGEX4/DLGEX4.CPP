/****************************************************************************
*
*  DLGEX4.CPP
*
*  Demonstrates multiple overlapping dialog boxes.
*
*  The dialog box has been given a new button, named "Create...", which
*  creates a child dialog.  The dialogs all share a common dialog box
*  procedure, but each has a different background bitmap.  Since each dialog
*  box must have a unique pointer to its bitmap data, the pointers are now
*  kept as window properties rather than static variables.
*
***/

#define  STRICT
#include <windows.h>
#include <storm.h>
#include "resource.h"

HINSTANCE instance = (HINSTANCE)0;

LPBYTE LoadBitmap (LPCTSTR filename, int *width, int *height);

//===========================================================================
BOOL CALLBACK DlgProc (HWND   window,
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

        case IDC_BUTTON1:
          SDlgDialogBoxParam(instance,
                             MAKEINTRESOURCE(IDD_DIALOG1),
                             window,
                             DlgProc,
                             (LPARAM)GetProp(window,TEXT("RecursionDepth"))+1);
          break;

      }
      break;

    case WM_DESTROY:
      if (GetProp(window,TEXT("BackgroundBitmap")))
        FREE((LPVOID)GetProp(window,TEXT("BackgroundBitmap")));
      break;

    case WM_INITDIALOG:

      // SAVE THE RECURSION DEPTH
      SetProp(window,TEXT("RecursionDepth"),(HANDLE)lparam);

      // POSITION THE DIALOG BOX ON THE SCREEN
      SetWindowPos(window,NULL,lparam*22,lparam*22,0,0,SWP_NOSIZE | SWP_NOZORDER);

      // LOAD THE BACKGROUND BITMAP
      {
        TCHAR filename[16];
        wsprintf(filename,TEXT("bkg%u.pcx"),(lparam & 3)+1);
        int    width, height;
        LPBYTE backgroundbitmap = LoadBitmap(filename,&width,&height);
        if (backgroundbitmap) {
          SetProp(window,TEXT("BackgroundBitmap"),(HANDLE)backgroundbitmap);
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
int APIENTRY WinMain (HINSTANCE passinstance, HINSTANCE, LPSTR, int) {
  instance = passinstance;
  if (!SDrawAutoInitialize(instance,
                           TEXT("DLGEX4"),
                           TEXT("Dialog Example 4")))
    return 1;
  SDlgDialogBoxParam(instance,
                     MAKEINTRESOURCE(IDD_DIALOG1),
                     SDrawGetFrameWindow(),
                     DlgProc,
                     0);
  return 0;
}

/****************************************************************************
*
*  DLGEX2.CPP
*
*  This sample displays the same dialog as DLGEX1, but now using Storm's
*  dialog manager instead of Windows'.
*
*  No changes were required to the dialog resource.  The only changes
*  required in the program itself were changing Windows API calls to
*  Storm equivalents (i.e. prepending everything with "SDlg") and making
*  the dialog box procedure call SDlgDefDialogProc() instead of returning
*  zero.
*
***/

#define  STRICT
#include <windows.h>
#include <storm.h>
#include "resource.h"

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

      }
      break;

    case WM_INITDIALOG:
      return 1;

  }
  return SDlgDefDialogProc(window,message,wparam,lparam);
}

//===========================================================================
int APIENTRY WinMain (HINSTANCE instance, HINSTANCE, LPSTR, int) {
  if (!SDrawAutoInitialize(instance,
                           TEXT("DLGEX2"),
                           TEXT("Dialog Example 2")))
    return 1;
  SDlgDialogBox(instance,
                MAKEINTRESOURCE(IDD_DIALOG1),
                SDrawGetFrameWindow(),
                DlgProc);
  return 0;
}

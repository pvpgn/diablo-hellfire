/****************************************************************************
*
*  DLGEX1.CPP
*
*  Creates a dialog box using the normal Windows dialog manager.
*
*  The dialog looks functional enough, but there are some not-so-obvious
*  problems using it in a DirectDraw game:
*
*  1. The dialog automatically adjusts itself to the display device's font
*     size and pixels-per-inch ratio.  This is a nice feature for normal
*     Windows applications, but problematic for games that want their glue
*     screens to always exactly fill the screen.
*
*  2. The dialog also displays itself using the system's currently selected
*     color scheme and metrics.  This, again, reduces the ability of the
*     application to control exactly how its glue screens look.
*
*  3. It is difficult to customize the look of Windows dialog boxes.  There
*     is no support for background bitmaps or button textures.
*
*  4. Windows' dialog manager is not well integrated with DirectDraw.  For
*     example, if you alt+tab away from the application while the dialog is
*     on the screen, you will never be able to switch back.
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
          EndDialog(window,1);
          break;

        case IDCANCEL:
          EndDialog(window,0);
          break;

      }
      break;

    case WM_INITDIALOG:
      return 1;

  }
  return 0;
}

//===========================================================================
int APIENTRY WinMain (HINSTANCE instance, HINSTANCE, LPSTR, int) {
  if (!SDrawAutoInitialize(instance,
                           TEXT("DLGEX1"),
                           TEXT("Dialog Example 1")))
    return 1;
  DialogBox(instance,
            MAKEINTRESOURCE(IDD_DIALOG1),
            SDrawGetFrameWindow(),
            DlgProc);
  return 0;
}

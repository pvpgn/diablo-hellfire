/****************************************************************************
*
*  PAN.CPP
*
*  Demonstrates panning the sound for a video.
*
***/

#include <windows.h>
#include <storm.h>

#define  VIDEOS  2

HSVIDEO video[VIDEOS] = {0,0};

//===========================================================================
BOOL CALLBACK IdleProc (DWORD) {

  // CHECK WHETHER WE NEED TO START A NEW VIDEO
  static DWORD laststart = 0;
  DWORD currtime = GetTickCount();
  for (DWORD loop = 0; loop < VIDEOS; ++loop)
    if (video[loop]) {
      BOOL updated;
      if (!SVidPlayContinueSingle(video[loop],FALSE,&updated)) {
        SVidPlayEnd(video[loop]);
        video[loop] = (HSVIDEO)0;
      }
    }
    else if (currtime-laststart > 3000) {
      laststart = currtime;
      RECT rect;
      rect.left   = (loop & 1)*320;
      rect.top    = (loop & 1)*240;
      rect.right  = rect.left+319;
      rect.bottom = rect.top+143;
      SVidPlayBegin(TEXT("blizzard.smk"),
                    NULL,&rect,NULL,NULL,
                    SVID_FLAG_TOSCREEN
                      | SVID_FLAG_1XSIZE
                      | SVID_FLAG_NEEDPAN,
                    &video[loop]);
      SVidSetVolume(video[loop],
                    0,
                    loop ? 10000 : -10000);
    }

  return TRUE;
}

//===========================================================================
void CALLBACK OnClose (LPPARAMS params) {
  for (int loop = 0; loop < VIDEOS; ++loop)
    if (video[loop])
      SVidPlayEnd(video[loop]);
}

//===========================================================================
void CALLBACK OnVkEscape (LPPARAMS params) {
  SDrawPostClose();
}

//===========================================================================
int APIENTRY WinMain (HINSTANCE instance, HINSTANCE, LPSTR cmdline, int) {

  // INTIALIZE DIRECTDRAW
  if (!SDrawAutoInitialize(instance,
                           TEXT("VIDEOCLASS"),
                           TEXT("Video Player")))
    return 1;
  ShowCursor(0);

  // REGISTER WINDOW MESSAGES
  SMsgRegisterMessage(NULL,WM_CLOSE ,OnClose);
  SMsgRegisterKeyDown(NULL,VK_ESCAPE,OnVkEscape);

  // INITIALIZE THE VIDEO PLAYER
  if (!SVidInitialize(NULL))
    return 1;

  // PLAY VIDEOS
  SMsgDoMessageLoop(IdleProc);

  // CLOSE ALL VIDEOS
  for (DWORD loop = 0; loop < VIDEOS; ++loop)
    if (video[loop])
      SVidPlayEnd(video[loop]);

  ShowCursor(1);
  return 0;
}

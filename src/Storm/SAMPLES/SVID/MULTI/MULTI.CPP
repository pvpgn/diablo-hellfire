/****************************************************************************
*
*  MULTI.CPP
*
*  Demonstrates playing back multiple video streams simultaneously.
*
***/

#include <windows.h>
#include <storm.h>

#define  VIDEOS  4

DWORD   started       = 0;
HSVIDEO video[VIDEOS] = {0,0,0,0};

//===========================================================================
BOOL CALLBACK IdleProc (DWORD) {

  // CHECK WHETHER WE NEED TO START A NEW VIDEO
  static DWORD lasttime = 0;
  DWORD currtime = GetTickCount();
  if ((currtime-lasttime > 1000) && (started < VIDEOS)) {
    RECT rect;
    rect.left   = (started & 1)*320;
    rect.top    = (started & 2)*120;
    rect.right  = rect.left+319;
    rect.bottom = rect.top+143;
    SVidPlayBegin(TEXT("blizzard.smk"),
                  NULL,&rect,NULL,NULL,
                  SVID_FLAG_TOSCREEN | SVID_FLAG_1XSIZE,
                  &video[started++]);
    lasttime = currtime;
  }

  // CONTINUE PLAYING VIDEOS
  if (!SVidPlayContinue())
    SDrawPostClose();

  return 1;
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

  // PLAY VIDEOS, STARTING A NEW ONE EACH SECOND UNTIL THEY ARE ALL STARTED,
  // CONTINUING UNTIL THEY ARE ALL FINISHED
  SMsgDoMessageLoop(IdleProc);

  // CLOSE ALL VIDEOS
  {
    for (DWORD loop = 0; loop < started; ++loop)
      SVidPlayEnd(video[loop]);
  }

  ShowCursor(1);
  return 0;
}

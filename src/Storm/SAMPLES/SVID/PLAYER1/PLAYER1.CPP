/****************************************************************************
*
*  PLAYER2.CPP
*
*  A simple video player.
*
***/

#include <windows.h>
#include <dsound.h>
#include <storm.h>

HSVIDEO video = (HSVIDEO)0;

//===========================================================================
BOOL CALLBACK IdleProc (DWORD) {
  if (!SVidPlayContinue())
    SDrawPostClose();
  return 1;
}

//===========================================================================
void CALLBACK OnClose (LPPARAMS params) {
  if (video)
    SVidPlayEnd(video);
}

//===========================================================================
void CALLBACK OnVkEscape (LPPARAMS params) {
  SDrawPostClose();
}

//===========================================================================
int APIENTRY WinMain (HINSTANCE instance, HINSTANCE, LPSTR cmdline, int) {

  // CHECK THE COMMAND LINE
  if (!(cmdline && *cmdline)) {
    MessageBox(0,
               "Usage: PLAYER1 filename.smk",
               "Video Player",
               MB_ICONSTOP);
    return 1;
  }

  // INITIALIZE DIRECTDRAW
  if (!SDrawAutoInitialize(instance,
                           TEXT("VIDEOCLASS"),
                           TEXT("Video Player")))
    return 1;

  // INITIALIZE DIRECTSOUND
  LPDIRECTSOUND lpds = NULL;
  if (DirectSoundCreate(NULL,&lpds,NULL) == DS_OK) {

    // SET THE COOPERATIVE LEVEL
    if (lpds->SetCooperativeLevel(SDrawGetFrameWindow(),DSSCL_PRIORITY) != DS_OK)
      if (lpds->SetCooperativeLevel(SDrawGetFrameWindow(),DSSCL_NORMAL) != DS_OK)
        return 1;

    // CREAT A PRIMARY SOUND BUFFER
    DSBUFFERDESC desc;
    ZeroMemory(&desc,sizeof(DSBUFFERDESC));
    desc.dwSize  = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN;
    LPDIRECTSOUNDBUFFER lpbuf;
    lpds->CreateSoundBuffer(&desc,&lpbuf,NULL);

    // SET THE FORMAT OF THE PRIMARY SOUND BUFFER
    WAVEFORMATEX format;
    ZeroMemory(&format,sizeof(WAVEFORMATEX));
    format.wFormatTag      = WAVE_FORMAT_PCM;
    format.nChannels       = 2;
    format.nSamplesPerSec  = 22050;
    format.wBitsPerSample  = 16;
    format.nBlockAlign     = (2*16)/8;          // (nChannels*wBitsPerSample)/8
    format.nAvgBytesPerSec = ((2*16)/8)*22050;  // nBlockAlign*nSamplesPerSec
    format.cbSize          = 0;
    lpbuf->SetFormat(&format);

  }

  // REGISTER WINDOW MESSAGES
  SMsgRegisterMessage(NULL,WM_CLOSE ,OnClose);
  SMsgRegisterKeyDown(NULL,VK_ESCAPE,OnVkEscape);

  // INITIALIZE THE VIDEO PLAYER
  TCHAR dir[MAX_PATH] = TEXT("");
  GetCurrentDirectory(MAX_PATH,dir);
  SFileSetBasePath(dir);
  if (!SVidInitialize(lpds))
    return 1;

  // PLAY THE VIDEO
  ShowCursor(0);
  SVidPlayBegin(cmdline,
                NULL,NULL,NULL,NULL,
                SVID_AUTOCUTSCENE,
                &video);
  int result = SMsgDoMessageLoop(IdleProc);
  ShowCursor(1);

  return result;
}

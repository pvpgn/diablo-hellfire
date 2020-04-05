/****************************************************************************
*
*  PLAYER2.CPP
*
*  A sample video player.  This version allows the user to pause and to set
*  the playback quality level.
*
***/

#include <windows.h>
#include <dsound.h>
#include <storm.h>

BOOL    paused = 0;
HSVIDEO video  = (HSVIDEO)0;

//===========================================================================
BOOL CALLBACK IdleProc (DWORD) {
  if (!paused) {
    if (!SVidPlayContinue())
      SDrawPostClose();
  }
  return !paused;
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
void CALLBACK OnVkSpace (LPPARAMS params) {
  paused = !paused;
}

//===========================================================================
int APIENTRY WinMain (HINSTANCE instance, HINSTANCE, LPSTR cmdline, int) {

  // PARSE THE COMMAND LINE
  if (!(cmdline && *cmdline)) {
    MessageBox(0,
               "Usage: PLAYER2 [qualitylevel] filename.smk\n\n"
               "Quality Levels:\n"
               "1 = Low (skip scans)\n"
               "2 = Low (zoom)\n"
               "3 = High (skip scans)\n"
               "4 = High (zoom)",
               "Video Player",
               MB_ICONSTOP);
    return 1;
  }
  int qualitylevel = SVID_QUALITY_HIGH;
  if (atoi(cmdline)) {
    switch (atoi(cmdline)) {
      case 1: qualitylevel = SVID_QUALITY_LOW_SKIPSCANS;   break;
      case 2: qualitylevel = SVID_QUALITY_LOW;             break;
      case 3: qualitylevel = SVID_QUALITY_HIGH_SKIPSCANS;  break;
      case 4: qualitylevel = SVID_QUALITY_HIGH;            break;
    }
    if (strchr(cmdline,' '))
      cmdline = strchr(cmdline,' ')+1;
    else
      cmdline += strlen(cmdline);
  }

  // INTIALIZE DIRECTDRAW
  if (!SDrawAutoInitialize(instance,
                           TEXT("VIDEOCLASS"),
                           TEXT("Video Player")))
    return 1;

  // INITIALIZE DIRECTSOUND
  LPDIRECTSOUND lpds = NULL;
  if (DirectSoundCreate(NULL,&lpds,NULL) == DS_OK) {

    // SET THE COOPERATIVE LEVEL
    lpds->SetCooperativeLevel(SDrawGetFrameWindow(),
                              DSSCL_NORMAL);

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
  SMsgRegisterKeyDown(NULL,VK_SPACE ,OnVkSpace);

  // INITIALIZE THE VIDEO PLAYER
  if (!SVidInitialize(lpds))
    return 1;

  // PLAY THE VIDEO
  ShowCursor(0);
  SVidPlayBegin(cmdline,
                NULL,NULL,NULL,NULL,
                SVID_CUTSCENE | qualitylevel,
                &video);
  SMsgDoMessageLoop(IdleProc);
  ShowCursor(1);

  return 0;
}

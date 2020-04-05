/****************************************************************************
*
*  DDATEST.CPP
*
*  This sample program demonstrates the use of direct digital audio streaming
*  from a CD.
*
***/

#include <windows.h>
#include <dsound.h>
#include <storm.h>

#define  ARCHIVE  "e:\\diabdat.mpq"
#define  SONGS    6
#define  TITLE    "DDA Test"

static const LPCSTR songname[SONGS] = {"sfx\\towners\\drunk25.wav",
                                       "sfx\\towners\\witch40.wav",
                                       "sfx\\towners\\bsmith47.wav",
                                       "sfx\\towners\\healer39.wav",
                                       "sfx\\towners\\wound01.wav",
                                       "sfx\\towners\\deadguy2.wav"};

HSARCHIVE     archive          = (HSARCHIVE)0;
DWORD         currsong         = SONGS-1;
HSGDIFONT     font             = (HSGDIFONT)0;
DWORD         iterations       = 0;
LPDIRECTSOUND lpds             = NULL;
HSFILE        soundfile[SONGS] = {0};
HSTRANS       transparency     = (HSTRANS)0;

//===========================================================================
BOOL LoadTransparency () {
  LPBYTE bitmap = (LPBYTE)ALLOC(32*32);
  if (!bitmap)
    return 0;
  PALETTEENTRY pe[256];
  if (!SBmpLoadImage("cdrom.pcx",&pe[0],bitmap,32*32))
    return 0;
  SDrawUpdatePalette(0,256,&pe[0]);
  STransCreate(bitmap,32,32,8,NULL,PALETTEINDEX(*bitmap),&transparency);
  FREE(bitmap);
  return 1;
}

//===========================================================================
BOOL CALLBACK IdleProc (DWORD) {
  LPBYTE videobuffer;
  int    pitch;
  if (SDrawLockSurface(SDRAW_SURFACE_FRONT,NULL,&videobuffer,&pitch)) {
    for (int loop = 0; loop < 100; ++loop) {
      int left = (rand() % 640)-20;
      int top  = (rand() % 420)+30;
#ifdef ALIGNED
      left = (left >> 2) << 2;
#endif
      STransBlt(videobuffer,left,top,pitch,transparency);
      ++iterations;
    }
    SDrawUnlockSurface(SDRAW_SURFACE_FRONT,videobuffer);
  }
  return 1;
}

//===========================================================================
BOOL InitializeDirectSound () {

  // CREATE A DIRECTSOUND OBJECT
  DirectSoundCreate(NULL,&lpds,NULL);
  if (!lpds)
    return 0;

  // CREATE A PRIMARY BUFFER
  LPDIRECTSOUNDBUFFER primarybuffer = NULL;
  {
    DSBUFFERDESC desc;
    ZeroMemory(&desc,sizeof(DSBUFFERDESC));
    desc.dwSize  = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_PRIMARYBUFFER;
    lpds->CreateSoundBuffer(&desc,&primarybuffer,NULL);
  }
  if (!primarybuffer)
    return 0;

  // START PLAYING THE PRIMARY BUFFER
  if (lpds->SetCooperativeLevel(SDrawGetFrameWindow(),DSSCL_EXCLUSIVE) != DS_OK)
    return 0;
  if (primarybuffer->Play(0,0,DSBPLAY_LOOPING) != DS_OK)
    return 0;

  // GIVE OUR DIRECTSOUND POINTER TO STORM
  SFileDdaInitialize(lpds);

  return 1;
}

//===========================================================================
BOOL LoadFont () {
  {
    HFONT winfont = CreateFont(-10,0,0,0,FW_BOLD,0,0,0,ANSI_CHARSET,
                               OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
                               VARIABLE_PITCH | FF_SWISS,TEXT("Arial"));
    if (!SGdiImportFont(winfont,&font))
      return 0;
    DeleteObject(winfont);
  }
  if (!SGdiSelectObject(font))
    return 0;
  return 1;
}

//===========================================================================
void CALLBACK OnClose (LPPARAMS params) {
  KillTimer(params->window,1);
  SGdiDeleteObject(font);
  STransDelete(transparency);
  SFileCloseArchive(archive);
}

//===========================================================================
void CALLBACK OnTimer (LPPARAMS) {
  LPBYTE videobuffer;
  int    videopitch;
  if (SDrawLockSurface(SDRAW_SURFACE_FRONT,NULL,&videobuffer,&videopitch)) {
    SGdiSetPitch(videopitch);

    {
      char outstr[64];
      wsprintf(outstr,"%u transparent bitblts per second",iterations);
      RECT rect = {0,0,320,15};
      SGdiExtTextOut(videobuffer,
                     0,
                     0,
                     &rect,
                     0,
                     ETO_TEXT_WHITE,
                     ETO_BKG_BLACK,
                     outstr);
    }

    if (soundfile[currsong]) {
      DWORD position;
      DWORD maxposition;
      char  outstr[64] = "";
      if (SFileDdaGetPos(soundfile[currsong],&position,&maxposition))
        wsprintf(outstr,"%u%% complete",position*100/maxposition);
      RECT rect = {320,0,639,15};
      SGdiExtTextOut(videobuffer,
                     320,
                     0,
                     &rect,
                     0,
                     ETO_TEXT_WHITE,
                     ETO_BKG_BLACK,
                     outstr);
    }

    SGdiTextOut(videobuffer,  0,10,PALETTEINDEX(255),"[S] Start DDA playback");
    SGdiTextOut(videobuffer,  0,20,PALETTEINDEX(255),"[X] Terminate DDA playback");
    SGdiTextOut(videobuffer,320,10,PALETTEINDEX(255),"[L] Left  [C] Center  [R] Right");
    SGdiTextOut(videobuffer,320,20,PALETTEINDEX(255),"[Q] Toggle Quiet Mode");
    SDrawUnlockSurface(SDRAW_SURFACE_FRONT,videobuffer);
  }
  iterations = 0;
}

//===========================================================================
void CALLBACK OnVkEscape (LPPARAMS) {
  SDrawPostClose();
}

//===========================================================================
void CALLBACK OnVkC (LPPARAMS) {
  SFileDdaSetVolume(soundfile[currsong],0x7FFFFFFF,0);
}

//===========================================================================
void CALLBACK OnVkL (LPPARAMS) {
  SFileDdaSetVolume(soundfile[currsong],0x7FFFFFFF,10000);
}

//===========================================================================
void CALLBACK OnVkQ (LPPARAMS) {
  LONG volume;
  SFileDdaGetVolume(soundfile[currsong],&volume,NULL);
  SFileDdaSetVolume(soundfile[currsong],volume ? 0 : -2000,0x7FFFFFFF);
}

//===========================================================================
void CALLBACK OnVkR (LPPARAMS) {
  SFileDdaSetVolume(soundfile[currsong],0x7FFFFFFF,-10000);
}

//===========================================================================
void CALLBACK OnVkS (LPPARAMS) {
  currsong = (currsong+1) % SONGS;
  if (!soundfile[currsong])
    SFileOpenFile(songname[currsong],&soundfile[currsong]);
  if (soundfile[currsong])
    if (!SFileDdaBegin(soundfile[currsong],0x40000,0)) {
      SFileCloseFile(soundfile[currsong]);
      soundfile[currsong] = (HSFILE)0;
    }
}


//===========================================================================
void CALLBACK OnVkX (LPPARAMS) {
  if (soundfile[currsong])
    SFileDdaEnd(soundfile[currsong]);
}

//===========================================================================
int APIENTRY WinMain (HINSTANCE instance, HINSTANCE, LPSTR, int) {
  if (!SDrawAutoInitialize(instance,
                           TEXT("DDATEST"),
                           TITLE)) {
    SDrawMessageBox("Unable to initialize DirectDraw.",TITLE,0);
    return 1;
  }
  if (!InitializeDirectSound()) {
    SDrawMessageBox("Unable to initialize DirectSound.",TITLE,0);
    return 1;
  }
  if (!SFileOpenArchive(ARCHIVE,0,0,&archive)) {
    SDrawMessageBox("Unable to open MPQ data file.",TITLE,0);
    return 0;
  }
  SFileEnableDirectAccess(1);
  if (!LoadTransparency()) {
    SDrawMessageBox("Unable to load transparency.",TITLE,0);
    return 1;
  }
  if (!LoadFont()) {
    SDrawMessageBox("Unable to load font.",TITLE,0);
    return 1;
  }
  SetTimer(SDrawGetFrameWindow(),1,1000,NULL);
  SMsgRegisterMessage(NULL,WM_CLOSE ,OnClose);
  SMsgRegisterMessage(NULL,WM_TIMER ,OnTimer);
  SMsgRegisterKeyDown(NULL,VK_ESCAPE,OnVkEscape);
  SMsgRegisterKeyDown(NULL,'C'      ,OnVkC);
  SMsgRegisterKeyDown(NULL,'L'      ,OnVkL);
  SMsgRegisterKeyDown(NULL,'Q'      ,OnVkQ);
  SMsgRegisterKeyDown(NULL,'R'      ,OnVkR);
  SMsgRegisterKeyDown(NULL,'S'      ,OnVkS);
  SMsgRegisterKeyDown(NULL,'X'      ,OnVkX);
  return SMsgDoMessageLoop(IdleProc);
}

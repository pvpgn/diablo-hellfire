/****************************************************************************
*
*  LIGHT2.CPP
*
*  This next example also uses an assembly language bitblt function, but this
*  time it tries to read and write as many whole doublewords as possible.
*  The inner loop of the function is taken from Diablo.
*
***/

#include <windows.h>
#include <storm.h>

BYTE      lightingtable[16][256];

LPBYTE    bitmapbits = NULL;
HSGDIFONT font       = (HSGDIFONT)0;
BOOL      paused     = 0;
DWORD     rectangles = 0;

int inline square (int a) { return a*a; }

//===========================================================================
void BitBltLighting (LPBYTE dest,
                     LPBYTE source,
                     int    width,
                     int    height,
                     int    destcx,
                     int    sourcecx,
                     int    lightlevel) {
  LPVOID table        = lightingtable;
  DWORD  destadjust   = destcx-width;
  DWORD  sourceadjust = sourcecx-width;
  __asm {

                   // SAVE IMPORTANT REGISTERS
                   push      esi
                   push      edi

                   // SETUP THE SOURCE AND DESTINATION
                   mov       esi,source
                   mov       edi,dest

                   // SETUP REGISTERS
                   mov       eax,lightlevel
                   mov       ebx,table
                   shl       eax,8
                   add       ebx,eax
                   mov       edx,width

                   // START THE NEXT LINE
    startline:     mov       ecx,edx
                   and       ecx,3
                   jz        startdword

                   // USE BYTES FOR ODD PIXELS
    nextbyte:      lodsb
                   xlatb
                   stosb
                   loop      nextbyte

                   // USE DWORDS FOR THE REST
    startdword:    mov       ecx,edx
                   shr       ecx,2
    nextdword:     lodsd
                   xlatb
                   ror       eax,8
                   xlatb
                   ror       eax,8
                   xlatb
                   ror       eax,8
                   xlatb
                   ror       eax,8
                   stosd
                   loop      nextdword

                   // ADJUST FOR THE NEXT LINE
                   add       esi,sourceadjust
                   add       edi,destadjust
                   dec       height
                   jnz       startline

                   // RESTORE REGISTERS
                   pop       edi
                   pop       esi

  }
}

//===========================================================================
void CreateLightingTable (LPPALETTEENTRY pe) {
  for (int lightlevel = 0; lightlevel < 16; ++lightlevel)
    for (int index = 0; index < 256; ++index) {
      int red   = ((int)(pe+index)->peRed  )*lightlevel/8;
      int green = ((int)(pe+index)->peGreen)*lightlevel/8;
      int blue  = ((int)(pe+index)->peBlue )*lightlevel/8;
      int minval   = 0x7FFFFFFF;
      int minindex = 0;
      for (int testindex = 0; testindex < 256; ++testindex) {
        int proximity = square(red-(int)(pe+testindex)->peRed)
                          +square(green-(int)(pe+testindex)->peGreen)
                          +square(blue-(int)(pe+testindex)->peBlue);
        if (proximity < minval) {
          minval   = proximity;
          minindex = testindex;
        }
      }
      lightingtable[lightlevel][index] = minindex;
    }
}

//===========================================================================
BOOL CALLBACK IdleProc (DWORD) {
  if (paused)
    return 0;

  static BYTE lightlevel = 0;
  LPBYTE videobuffer;
  int    pitch;
  if (SDrawLockSurface(SDRAW_SURFACE_FRONT,NULL,&videobuffer,&pitch)) {
    for (int loop = 0; loop < 100; ++loop) {
      int left = (rand() % 600);
      int top  = (rand() % 428)+12;
      BitBltLighting(videobuffer+top*pitch+left,bitmapbits,40,40,pitch,40,lightlevel & 0x0F);
      ++lightlevel;
      ++rectangles;
    }
    SDrawUnlockSurface(SDRAW_SURFACE_FRONT,videobuffer);
  }
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
  FREE(bitmapbits);
  bitmapbits = NULL;
  KillTimer(params->window,1);
  SGdiDeleteObject(font);
}

//===========================================================================
void CALLBACK OnTimer (LPPARAMS) {
  LPBYTE videobuffer;
  int    videopitch;
  if (SDrawLockSurface(SDRAW_SURFACE_FRONT,NULL,&videobuffer,&videopitch)) {
    SGdiSetPitch(videopitch);
    char outstr[80];
    wsprintf(outstr,"%u bitblts per second using assembly (dwords, complex instructions)",rectangles);
    RECT rect = {0,0,639,12};
    SGdiExtTextOut(videobuffer,
                   0,
                   0,
                   &rect,
                   0,
                   ETO_TEXT_WHITE,
                   ETO_BKG_BLACK,
                   outstr);
    SDrawUnlockSurface(SDRAW_SURFACE_FRONT,videobuffer);
  }
  rectangles = 0;
}

//===========================================================================
void CALLBACK OnVkEscape (LPPARAMS) {
  SDrawPostClose();
}

//===========================================================================
void CALLBACK OnVkSpace (LPPARAMS) {
  paused = !paused;
}

//===========================================================================
int APIENTRY WinMain (HINSTANCE instance, HINSTANCE, LPSTR, int) {

  // INITIALIZE DIRECTDRAW
  if (!SDrawAutoInitialize(instance,
                           TEXT("LIGHTING"),
                           TEXT("Lighting Test")))
    return 1;

  // LOAD THE TEST BITMAP AND SET THE PALETTE
  {
    bitmapbits = (LPBYTE)ALLOC(40*40);
    if (!bitmapbits)
      return 1;
    PALETTEENTRY pe[256];
    if (!SBmpLoadImage(TEXT("test.pcx"),&pe[0],bitmapbits,40*40))
      return 1;
    if (!SDrawUpdatePalette(0,256,&pe[0]))
      return 1;
    CreateLightingTable(&pe[0]);
  }

  // LOAD AND SELECT THE FONT
  if (!LoadFont())
    return 1;

  // REGISTER MESSAGE HANDLERS AND SET THE TIMER
  SetTimer(SDrawGetFrameWindow(),1,1000,NULL);
  SMsgRegisterMessage(NULL,WM_CLOSE ,OnClose);
  SMsgRegisterMessage(NULL,WM_TIMER ,OnTimer);
  SMsgRegisterKeyDown(NULL,VK_ESCAPE,OnVkEscape);
  SMsgRegisterKeyDown(NULL,VK_SPACE ,OnVkSpace);

  // RUN THE MESSAGE LOOP
  return SMsgDoMessageLoop(IdleProc);
}

/****************************************************************************
*
*  LIGHT5.CPP
*
*  This final version uses Storm's S-code compiler instead of assembly
*  language to perform the blt.  The advantages are:
*  1. Storm compiles S-code into fully unrolled machine language loops,
*     which can process an entire scan line with no loop counter or
*     conditional jumps.
*  2. Storm ensures that all dword operations are aligned on dword
*     boundaries.
*  3. Since there is no loop counter, an additional general purpose register
*     is available for the code to use.  (In this particular case, there was
*     no need of the extra register, but in many programs this would provide
*     an additional speed boost.)
*  4. Using S-code instead of assembly language simplifies the program.
*     (In this case, the number of lines of code involved with the lighting
*     blt was reduced from 87 to 16.)
*  5. Using S-code instead of assembly language simplifies porting the
*     program to other platforms.
*
***/

#include <windows.h>
#include <storm.h>

BYTE             lightingtable[16][256];

LPBYTE           bitmapbits  = NULL;
HSGDIFONT        font        = (HSGDIFONT)0;
BOOL             paused      = 0;
DWORD            rectangles  = 0;
HSCODESTREAM     scodestream = (HSCODESTREAM)0;
SCODEEXECUTEDATA scodeexec;

int inline square (int a) { return a*a; }

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
    scodeexec.adjustdest = pitch-40;
    for (int loop = 0; loop < 100; ++loop) {
      int left = (rand() % 600);
      int top  = (rand() % 428)+12;
      scodeexec.dest  = videobuffer+top*pitch+left;
      scodeexec.table = &lightingtable[lightlevel & 0x0F][0];
      SCodeExecute(scodestream,&scodeexec);
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
  SCodeDelete(scodestream);
}

//===========================================================================
void CALLBACK OnTimer (LPPARAMS) {
  LPBYTE videobuffer;
  int    videopitch;
  if (SDrawLockSurface(SDRAW_SURFACE_FRONT,NULL,&videobuffer,&videopitch)) {
    SGdiSetPitch(videopitch);
    char outstr[80];
    wsprintf(outstr,"%u bitblts per second using S-code",rectangles);
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

  // CREATE THE S-CODE STREAM AND EXECUTE DATA
  if (!SCodeCompile(TEXT("1 B=S D=TB"),
                    TEXT("4 W=S A1=W1 B1=W2 W1=TA W2=TB A1=W3 B1=W4 W3=TA W4=TB D=W"),
                    NULL,
                    64,
                    SCODE_CF_AUTOALIGNDWORD,
                    &scodestream))
    return 1;
  ZeroMemory(&scodeexec,sizeof(SCODEEXECUTEDATA));
  scodeexec.size        = sizeof(SCODEEXECUTEDATA);
  scodeexec.xiterations = 40;
  scodeexec.yiterations = 40;
  scodeexec.source      = bitmapbits;

  // REGISTER MESSAGE HANDLERS AND SET THE TIMER
  SetTimer(SDrawGetFrameWindow(),1,1000,NULL);
  SMsgRegisterMessage(NULL,WM_CLOSE ,OnClose);
  SMsgRegisterMessage(NULL,WM_TIMER ,OnTimer);
  SMsgRegisterKeyDown(NULL,VK_ESCAPE,OnVkEscape);
  SMsgRegisterKeyDown(NULL,VK_SPACE ,OnVkSpace);

  // RUN THE MESSAGE LOOP
  return SMsgDoMessageLoop(IdleProc);
}

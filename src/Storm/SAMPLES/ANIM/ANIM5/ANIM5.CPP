/****************************************************************************
*
*  ANIM5.CPP
*
*  Under construction...
*
***/

#include "pch.h"
#pragma  hdrstop

#include <stdio.h> // note: take this out!!

//#define  COMBINEDOVERLAY

#define  ANIMSPRITES  4
#define  SPRITE_FPS   (ANIMSPRITES+0)
#define  SPRITE_BKG   (ANIMSPRITES+1)
#define  SPRITE_OVL1  (ANIMSPRITES+2)
#ifdef COMBINEDOVERLAY
#define  SPRITES      (ANIMSPRITES+3)
#else
#define  SPRITE_OVL2  (ANIMSPRITES+3)
#define  SPRITES      (ANIMSPRITES+4)
#endif

#define  FPSWIDTH     60
#define  FPSHEIGHT    16

#define  TRANSCOLOR   PALETTEINDEX(111)

#define  SIN(a)       s_sintable[((a) & 255)]
#define  COS(a)       s_sintable[(((a)+64) & 255)]

static HSGDIFONT s_font;
static HSIMAGE   s_fpsimage;
static int       s_sintable[256];
static HSSPRITE  s_sprite[SPRITES];
static HSTARGET  s_target;

static void UpdateFramesPerSecond ();

//===========================================================================
static void CreateFont () {
  HFONT winfont = CreateFont(-12,0,0,0,FW_BOLD,0,0,0,ANSI_CHARSET,
                             OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
                             VARIABLE_PITCH | FF_SWISS,TEXT("Arial"));
  SGdiImportFont(winfont,&s_font);
  DeleteObject(winfont);
  SGdiSelectObject(s_font);
}

//===========================================================================
static BOOL CreatePalette () {
  PALETTEENTRY pe[256];
  if (!SBmpLoadImage("..\\demodata\\bkg.pcx",&pe[0],NULL,0))
    return FALSE;
  SDrawUpdatePalette(0,256,&pe[0]);
  return TRUE;
}

//===========================================================================
static void CreateSinTable () {
  for (int loop = 0; loop < 256; ++loop) {
    double angle = (loop*3.14159265359)/128.0;
    s_sintable[loop] = (int)(sin(angle)*128);
  }
}

//===========================================================================
static BOOL CreateSprites () {
  DWORD   loop;
  HSIMAGE imagearray[SPRITES];

  // LOAD THE ANIMATED SPRITE IMAGES
  {
    RECT rectarray[ANIMSPRITES];
    for (loop = 0; loop < ANIMSPRITES; ++loop) {
      rectarray[loop].left   = 0;
      rectarray[loop].top    = 125*loop;
      rectarray[loop].right  = 300;
      rectarray[loop].bottom = 125*loop+125;
    }
    if (!S2dImageCreateFromFile("..\\demodata\\sprites.pcx",
                                TRANSCOLOR,
                                ANIMSPRITES,
                                rectarray,
                                imagearray))
      return FALSE;
  }

  // CREATE THE FPS SPRITE IMAGE
  S2dImageCreate(FPSWIDTH,
                 FPSHEIGHT,
                 8,
                 TRANSCOLOR,
                 &imagearray[SPRITE_FPS]);

  // LOAD THE BACKGROUND SPRITE IMAGE
  if (!S2dImageCreateFromFile("..\\demodata\\bkg.pcx",
                              TRANSCOLOR,
                              1,
                              NULL,
                              &imagearray[SPRITE_BKG]))
    return FALSE;

  // LOAD THE OVERLAY SPRITE IMAGES
#ifdef COMBINEDOVERLAY
  if (!S2dImageCreateFromFile("..\\demodata\\overlay.pcx",
                              TRANSCOLOR,
                              1,
                              NULL,
                              &imagearray[SPRITE_OVL1]))
    return FALSE;
#else
  {
    RECT rectarray[2] = {{0,0,167,18},{0,295,640,480}};
    if (!S2dImageCreateFromFile("..\\demodata\\overlay.pcx",
                                TRANSCOLOR,
                                2,
                                rectarray,
                                &imagearray[SPRITE_OVL1]))
      return FALSE;
  }
#endif

  // ATTACH THE IMAGES TO SPRITES, AND THEN CLOSE OR SAVE THE IMAGE HANDLES
  for (loop = 0; loop < SPRITES; ++loop) {
    S2dSpriteCreate(&s_sprite[loop]);
    S2dSpriteSetImage(s_sprite[loop],imagearray[loop]);
    if (loop == SPRITE_FPS) {
      S2dSpriteSetPos(s_sprite[loop],0,20);
      s_fpsimage = imagearray[loop];
    }
    else {
      S2dSpriteSetPos(s_sprite[loop],0,0);
#ifndef COMBINEDOVERLAY
if (loop == SPRITE_OVL2) S2dSpriteSetPos(s_sprite[loop],0,295);
#endif
      S2dImageDelete(imagearray[loop]);
    }
  }

  // CREATE A TARGET, AND ATTACH THE SPRITES TO THE TARGET
  S2dTargetCreate(&s_target);
  for (loop = 0; loop < SPRITES; ++loop)
    S2dSpriteSetTarget(s_sprite[loop],
                       s_target,
                       (loop >= SPRITE_OVL1) ? -1 : loop);

  return TRUE;
}

//===========================================================================
static BOOL CALLBACK IdleProc (DWORD) {
  static BYTE rotation = 0;
  static BYTE distance = 0;
  ++distance;
  rotation += 3;
  for (DWORD loop = 0; loop < ANIMSPRITES; ++loop)
    S2dSpriteSetPos(s_sprite[loop],
                    170+(SIN(rotation+64*loop)*SIN(distance/2)/100),
                    165+(COS(rotation+64*loop)*SIN(distance/2)/100));
  LPBYTE videobuffer;
  int    videopitch;
  if (SDrawLockSurface(SDRAW_SURFACE_FRONT,NULL,&videobuffer,&videopitch)) {
//SGdiSetPitch(videopitch);
//SGdiRectangle(videobuffer,0,0,640,480,PALETTEINDEX(255));
    S2dTestDrawTarget(s_target,videobuffer,videopitch);
    SDrawUnlockSurface(SDRAW_SURFACE_FRONT,videobuffer);
  }
  UpdateFramesPerSecond();
//return FALSE;
  return TRUE;
}

//===========================================================================
static void CALLBACK OnClose (LPPARAMS) {
  for (DWORD loop = 0; loop < SPRITES; ++loop) {
    S2dSpriteDelete(s_sprite[loop]);
    s_sprite[loop] = (HSSPRITE)0;
  }
  S2dImageDelete(s_fpsimage);
  s_fpsimage = (HSIMAGE)0;
  S2dTargetDelete(s_target);
  s_target = (HSTARGET)0;
  SGdiDeleteObject(s_font);
  s_font = (HSGDIFONT)0;
}

//===========================================================================
static void CALLBACK OnPaint (LPPARAMS params) {
  LPBYTE videobuffer;
  int    videopitch;
  if (SDrawLockSurface(SDRAW_SURFACE_FRONT,NULL,&videobuffer,&videopitch)) {
    S2dTestDrawTarget(s_target,videobuffer,videopitch);
    SDrawUnlockSurface(SDRAW_SURFACE_FRONT,videobuffer);
  }
  ValidateRect(params->window,NULL);
  params->useresult = TRUE;
  params->result    = 0;
}

//===========================================================================
static void CALLBACK OnVkEscape (LPPARAMS) {
  SDrawPostClose();
}

//===========================================================================
static void UpdateFramesPerSecond () {
  static DWORD frames    = 0;
  static DWORD starttime = GetTickCount();
  static DWORD lasttime  = starttime;

  // INCREMENT THE NUMBER OF FRAMES
  ++frames;

  // RETURN IF A SECOND HAS NOT YET ELAPSED
  if (GetTickCount()-lasttime < 1000)
    return;
  lasttime += 1000;

  // DRAW THE AVERAGE NUMBER OF FRAMES PER SECOND
  LPBYTE ptr;
  if (S2dImageLockBuffer(s_fpsimage,FALSE,&ptr)) {
    char outstr[16];
    wsprintf(outstr,"%3u FPS",frames/((lasttime-starttime)/1000));
    RECT rect = {0,0,FPSWIDTH,FPSHEIGHT};
    SGdiSetTargetDimensions(FPSWIDTH,
                            FPSHEIGHT,
                            8,
                            FPSWIDTH);
    SGdiExtTextOut(ptr,
                   0,
                   0,
                   &rect,
                   TRANSCOLOR,
                   ETO_TEXT_WHITE,
                   ETO_BKG_COLOR,
                   outstr);
    S2dImageUnlockBuffer(s_fpsimage,ptr);
  }

  // note: shouldn't be needed but it is for now...
  S2dSpriteSetPos(s_sprite[SPRITE_FPS],0,20);

static FILE *f = fopen("trace.txt","wt");
fprintf(f,"%u\n",frames/((lasttime-starttime)/1000));
fflush(f);

}

//===========================================================================
int APIENTRY WinMain (HINSTANCE instance, HINSTANCE, LPSTR, int) {
  CreateFont();
  CreateSinTable();
  if (!SDrawAutoInitialize(instance,
                           "ANIM5",
                           "Animation Example 5"))
    FATALRESULT("SDrawAutoInitialize()");
  if (!CreatePalette())
    FATALRESULT("CreatePalette()");
  if (!CreateSprites())
    FATALRESULT("CreateSprites()");

  SMsgRegisterMessage(NULL,WM_CLOSE ,OnClose);
  SMsgRegisterMessage(NULL,WM_PAINT ,OnPaint);
  SMsgRegisterKeyDown(NULL,VK_ESCAPE,OnVkEscape);
  return SMsgDoMessageLoop(IdleProc);
}

#ifndef SVGAH
#define SVGAH

RADDEFSTART
  
  typedef void (SETBANKTYPE)(u16);
  #pragma aux SETBANKTYPE "*" parm [eax] modify [eax];

  typedef void (SETINITTYPE)();
  #pragma aux SETINITTYPE "*" modify [eax ebx ecx edx];

  #define SVGAMode() (curmodetouse)
  u8    RADLINK SVGASetup(u32 width,u32 height);
  char* RADLINK SVGADetected(char* buf);
  void  RADLINK SVGASetText();
  void  RADLINK SVGASetGraph();
  void  RADLINK SVGAClear(u8 col);
  void  RADLINK SVGADetect(u32 flags);
  u32   RADLINK SVGABytesPS();
  u16*  RADLINK SVGAWinTbl();
  void* RADLINK SVGASetBank();
  u32   RADLINK SVGAFlags();

  void RADASMLINK SVGABlit(void* buf,u32 bleft,u32 btop,u32 subbwidth,u32 subbheight,u32 bPitch,u32 screenleft,u32 screentop);

  void RADASMLINK SVGABlitTrans(void* buf,u32 bleft,u32 btop,u32 subbwidth,u32 subbheight,u32 bwidth,u32 screenleft,u32 screentop,u8 trans);
  void RADASMLINK SVGABlitFrom (void* buf,u32 bleft,u32 btop,u32 subbwidth,u32 subbheight,u32 bwidth,u32 screenleft,u32 screentop);
  void RADASMLINK SVGABlitMask (void* buf,u32 bleft,u32 btop,u32 subbwidth,u32 subbheight,u32 bwidth,u32 screenleft,u32 screentop,u8 maskcolor,void*backbuf);
  void RADASMLINK SVGABlitMerge(void* buf,u32 bleft,u32 btop,u32 subbwidth,u32 subbheight,u32 bwidth,u32 screenleft,u32 screentop,u8 mergecolor,void*backbuf);

  void RADASMLINK SVGASetPalette(void PTR4* pal);

  extern u8 curmodetouse;
  extern u32 BytesPerScanToUse;
  extern u32 WidthToUse;
  extern u32 HeightToUse;
  extern SETBANKTYPE* SetBankToUse;
  extern u16* WinTblToUse;
  extern u32 FlagsToUse;
  extern void* StartVGA;
  extern void* EndVGA;
  extern u32 VGASize;
  extern u8 PaletteSkipZero;
  extern u8 IsBanked;
  extern u8 SVGAVESASetting;


#define SVGAFINDSVGA 1
#define SVGAUSE16BIT 2

RADDEFEND

#endif

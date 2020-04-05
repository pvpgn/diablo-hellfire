/****************************************************************************
*
*  2D graphics engine functions
*
***/

DECLARE_STRICT_HANDLE(HSIMAGE);
DECLARE_STRICT_HANDLE(HSSPRITE);
DECLARE_STRICT_HANDLE(HSTARGET);

void APIENTRY S2dImageCreate (int       width,
                              int       height,
                              int       bitdepth,
                              COLORREF  colorkey,
                              HSIMAGE  *handle);
BOOL APIENTRY S2dImageCreateFromFile (LPCTSTR   filename,
                                      COLORREF  colorkey,
                                      DWORD     numimages,
                                      LPCRECT   rectarray,
                                      HSIMAGE  *handlearray);
void APIENTRY S2dImageDelete (HSIMAGE handle);
BOOL APIENTRY S2dImageLockBuffer (HSIMAGE  handle,
                                  BOOL     blank,
                                  LPBYTE  *ptr);
void APIENTRY S2dImageUnlockBuffer (HSIMAGE handle,
                                    LPBYTE  ptr);
void APIENTRY S2dSpriteCreate (HSSPRITE *handle);
void APIENTRY S2dSpriteDelete (HSSPRITE handle);
void APIENTRY S2dSpriteSetImage (HSSPRITE handle,
                                 HSIMAGE  imagehandle);
void APIENTRY S2dSpriteSetPos (HSSPRITE handle,
                               int      x,
                               int      y);
void APIENTRY S2dSpriteSetTarget (HSSPRITE handle,
                                  HSTARGET targethandle,
                                  int      zorder);
void APIENTRY S2dTargetCreate (HSTARGET *handle);
void APIENTRY S2dTargetDelete (HSTARGET handle);

void APIENTRY S2dTestDrawTarget (HSTARGET handle,
                                 LPBYTE   ptr,
                                 int      pitch);

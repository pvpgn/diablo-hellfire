/****************************************************************************
*
*  S2D.CPP
*  Storm 2d graphics engine
*
*  By Michael O'Brien (8/8/97)
*
***/

#include "pch.h"
#pragma  hdrstop

#define  MAXSPANLENGTH   0xFC

#define  SF_DISPLAYED    0x00000001
#define  SF_ERASEMARKER  0x00000002

DECLARE_STRICT_HANDLE(HLOCKEDIMAGE);
DECLARE_STRICT_HANDLE(HLOCKEDSPRITE);
DECLARE_STRICT_HANDLE(HLOCKEDTARGET);

struct IMAGE;
struct SPRITE;
struct TARGET;

static void SpriteRemoveImageReference (SPRITE *spriteptr);

typedef BYTE SPAN;

typedef struct _SPANPAIR {
    SPAN copyspan;
    SPAN skipspan;
} SPANPAIR, *SPANPAIRPTR;

typedef struct _COMPOSITEDATA {
    SPRITE *spriteptr;
    BOOL    stable;
    SPAN   *spanptr;
    int     currspan;
    LPBYTE  dataptr;
} COMPOSITEDATA, *COMPOSITEDATAPTR;

EXPORTOBJECTDECL(SPRITE) {
    POINT           pos;
    RECT            boundingrect;
    DWORD           flags;
    int             zorder;
    IMAGE          *image;
    TARGET         *target;
    LINKEX(SPRITE)  linkimage;
    LINKEX(SPRITE)  linkzlist;

    inline ~SPRITE () {
      if (image)
        SpriteRemoveImageReference(this);
    }

} *SPRITEPTR;

typedef LISTEX(SPRITE,linkimage) SPRITEIMAGELIST;
typedef LISTEX(SPRITE,linkzlist) SPRITEZLIST;

EXPORTOBJECTDECL(IMAGE) {
    SIZE            size;
    RECT            boundingrect;
    BOOL            transparent;
    BYTE            transidx;
    BYTE            reserved[3];
    LPBYTE          imagedata;
    DWORD           refcount;
    ARRAY(SPANPAIR) spandata;
    ARRAY(DWORD)    spanindex;
    SPRITEIMAGELIST spritelist;

    inline ~IMAGE () {
      FREEIFUSED(imagedata);
    }

} *IMAGEPTR;

EXPORTOBJECTDECL(TARGET) {
    ARRAY(COMPOSITEDATA) compositedata;
    CSRgn                dirtyrgn;
    ARRAY(RECT)          dirtyrects;
    ARRAY(LPVOID)        dirtyrectparams;
//    SPRITEZLIST          sparelist;
    SPRITEZLIST          spritezlist;
} *TARGETPTR;

typedef EXPORTTABLE(IMAGE ,HSIMAGE ,HLOCKEDIMAGE ,SYNC_NONE) IMAGETABLE;
typedef EXPORTTABLE(SPRITE,HSSPRITE,HLOCKEDSPRITE,SYNC_NONE) SPRITETABLE;
typedef EXPORTTABLE(TARGET,HSTARGET,HLOCKEDTARGET,SYNC_NONE) TARGETTABLE;

static CCritSect   s_apicritsect;
static IMAGETABLE  s_imagetable;
static SPRITETABLE s_spritetable;
static TARGETTABLE s_targettable;

//===========================================================================
static inline BOOL CheckForSupportedColorFormat (int      bitdepth,
                                                 COLORREF colorkey) {
  return ((bitdepth == 8) &&
          (colorkey & 0x01000000));
}

//===========================================================================
static inline void ImageAddSpanPair (IMAGEPTR    imageptr,
                                     SPANPAIRPTR pair,
                                     int         nextx,
                                     int         y) {

  // ADD THIS SPAN PAIR TO THE ARRAY
  imageptr->spandata.AddElement(pair);

  // IF THIS SPAN PAIR CONTAINS A COPY SPAN, COMPUTE THE STARTING AND
  // ENDING X COORDINATES IN THE COPY SPAN, AND ADD THAT RANGE TO THE
  // BOUNDING RECTANGLE
  if (pair->copyspan) {
    int x2 = nextx-pair->skipspan;
    int x1 = x2-pair->copyspan;
    imageptr->boundingrect.left   = min(x1 ,imageptr->boundingrect.left);
    imageptr->boundingrect.top    = min(y  ,imageptr->boundingrect.top);
    imageptr->boundingrect.right  = max(x2 ,imageptr->boundingrect.right);
    imageptr->boundingrect.bottom = max(y+1,imageptr->boundingrect.bottom);
  }

  // BLANK THE SPAN PAIR
  pair->copyspan = pair->skipspan = 0;
}

//===========================================================================
static void ImageCreateSpanData (IMAGEPTR imageptr) {

  // RESET THE COMPUTED IMAGE DATA
  imageptr->boundingrect.left   = imageptr->size.cx;
  imageptr->boundingrect.top    = imageptr->size.cy;
  imageptr->boundingrect.right  = 0;
  imageptr->boundingrect.bottom = 0;
  imageptr->transparent         = FALSE;
  imageptr->spandata.SetNumElements(0);
  imageptr->spandata.ReserveSpace(imageptr->size.cx*4);
  imageptr->spanindex.SetNumElements(imageptr->size.cy);

  // BUILD THE SPAN ARRAY
  LPBYTE currdata = imageptr->imagedata;
  for (int y = 0; y < imageptr->size.cy; ++y) {

    // SAVE THE SPAN INDEX THE CORRESPONDS WITH THE BEGINNING OF THIS
    // SCAN LINE
    imageptr->spanindex[y] = imageptr->spandata.NumElements();

    // INITIALIZE THE OUTPUT VARIABLES
    SPANPAIR  currpair = {0,0};
    SPAN     *currspan = &currpair.copyspan;

    // IF THIS SCAN LINE HAS ZERO WIDTH, THEN JUST OUTPUT THE LINE TERMINATOR
    if (imageptr->size.cx <= 0) {
      ImageAddSpanPair(imageptr,&currpair,0,y);
      continue;
    }

    // OTHERWISE, BUILD AND OUTPUT SPANS
    int x;
    for (x = 0; x < imageptr->size.cx; ++x) {
      if ((*(currdata++) != imageptr->transidx) != (currspan == &currpair.copyspan))
        if (currspan == &currpair.copyspan) {
          imageptr->transparent = TRUE;
          currspan = &currpair.skipspan;
        }
        else {
          ImageAddSpanPair(imageptr,&currpair,x,y);
          currspan = &currpair.copyspan;
        }
      if (*currspan == MAXSPANLENGTH)
        ImageAddSpanPair(imageptr,&currpair,x,y);
      ++*currspan;
    }
    ImageAddSpanPair(imageptr,&currpair,x,y);

    // OUTPUT THE LINE TERMINATOR
    ImageAddSpanPair(imageptr,&currpair,x,y);

  }

}

//===========================================================================
static void SpriteLinkToZList (TARGETPTR targetptr,
                               SPRITEPTR spriteptr) {
  spriteptr->linkzlist.Unlink();
  SPRITEPTR before = NULL;
  ITERATELIST(SPRITE,targetptr->spritezlist,curr)
    if (curr->zorder > spriteptr->zorder) {
      before = curr;
      break;
    }
  targetptr->spritezlist.LinkNode(spriteptr,LIST_LINK_BEFORE,before);
}

//===========================================================================
static void SpriteRemoveImageReference (SPRITE *spriteptr) {
  if (!--spriteptr->image->refcount)
    delete spriteptr->image;
  spriteptr->image = NULL;
  spriteptr->linkimage.Unlink();
}

//===========================================================================
static void TargetBuildDirtyRects (TARGETPTR targetptr) {
  targetptr->dirtyrgn.Clear();
  ITERATELIST(SPRITE,targetptr->spritezlist,spriteptr)
    if ((spriteptr->flags & SF_ERASEMARKER) ||
        !(spriteptr->flags & SF_DISPLAYED))
      targetptr->dirtyrgn.AddRect(&spriteptr->boundingrect,spriteptr);
    else
      targetptr->dirtyrgn.AddParam(&spriteptr->boundingrect,spriteptr);
  DWORD numrects;
  targetptr->dirtyrgn.GetRects(&numrects,NULL);
  targetptr->dirtyrects.SetNumElements(numrects);
  targetptr->dirtyrgn.GetRects(&numrects,targetptr->dirtyrects.Ptr());
}

//===========================================================================
static void TargetCompositeRect (LPBYTE     videoptr,
                                 int        pitch,
                                 TARGETPTR  targetptr,
                                 LPCRECT    rect,
                                 DWORD      sprites,
                                 SPRITEPTR *spritearray) {
  DWORD loop;

  // BUILD A LIST OF SPRITES TO COMPOSITE
  targetptr->compositedata.SetNumElements(sprites);
  COMPOSITEDATAPTR compositedata = targetptr->compositedata.Ptr();
  DWORD            compositenum  = 0;
  {
    BOOL  aboveallchanges = TRUE;
    BOOL  erasing         = FALSE;
    DWORD lastdisplayed   = UINT_MAX;
    for (loop = 0; loop < sprites; ++loop) {
      SPRITEPTR spriteptr = spritearray[loop];
      if (spriteptr->flags & SF_ERASEMARKER) {
        aboveallchanges = FALSE;
        erasing         = TRUE;
      }
      else {
        if (!(spriteptr->flags & SF_DISPLAYED)) {
          aboveallchanges = FALSE;
          lastdisplayed   = compositenum;
        }
        compositedata[compositenum].spriteptr = spriteptr;
        compositedata[compositenum].stable    = aboveallchanges;
        compositedata[compositenum].dataptr
          = spriteptr->image->imagedata
              +((rect->top-spriteptr->pos.y-1)*spriteptr->image->size.cx)
              +(rect->left-spriteptr->pos.x)
              +(rect->right-rect->left);
        ++compositenum;
      }
    }
    if (!erasing)
      compositenum = lastdisplayed+1;
  }

  LPBYTE destptr = videoptr+rect->top*pitch+rect->left;
  for (int y = rect->top; y < rect->bottom; ++y) {

    // SETUP COMPOSITING INFO FOR THIS SCAN LINE FOR EACH SPRITE
    for (loop = 0; loop < compositenum; ++loop) {
      SPRITEPTR spriteptr = compositedata[loop].spriteptr;
      compositedata[loop].dataptr += spriteptr->image->size.cx-(rect->right-rect->left);
      if ((y >= spriteptr->boundingrect.top) &&
          (y <  spriteptr->boundingrect.bottom)) {
        compositedata[loop].spanptr  = (SPAN *)(spriteptr->image->spandata.Ptr()
                                         +spriteptr->image->spanindex[y-spriteptr->pos.y]);
        compositedata[loop].currspan = spriteptr->pos.x-rect->left;
        while (compositedata[loop].currspan <= 0)
          if ((!((DWORD)compositedata[loop].spanptr & 1)) &&
              !*(LPWORD)compositedata[loop].spanptr)
            compositedata[loop].currspan = INT_MAX;
          else
            compositedata[loop].currspan += *compositedata[loop].spanptr++;
      }
      else {
        compositedata[loop].spanptr  = NULL;
        compositedata[loop].currspan = INT_MAX;
      }
    }

    int x = rect->left;
    int xspan;
    while ((xspan = rect->right-x) > 0) {

      for (loop = 0; loop < compositenum; ++loop) {
        xspan = min(xspan,compositedata[loop].currspan);
        if ((DWORD)compositedata[loop].spanptr & 1)
          break;
      }

      if ((loop < compositenum) &&
          !compositedata[loop].stable) {
        LPBYTE sourceptr = compositedata[loop].dataptr;
#ifdef _X86_
  __asm {

                   // SETUP REGISTERS
                   push      esi
                   push      edi
                   mov       eax,[xspan]
                   xor       ecx,ecx
                   mov       esi,[sourceptr]
                   mov       edi,[destptr]

                   // PERFORM THE COPY
                   cmp       al,3
                   jbe       ds_done4

                   // IF NECESSARY, MOVE A SINGLE BYTE TO WORD-ALIGN THE
                   // DESTINATION
                   test      edi,1
                   jz        ds_aligned2
                   mov       cl,[esi]
                   inc       esi
                   mov       [edi],cl
                   inc       edi
                   dec       al
      ds_aligned2:

                   // IF NECESSARY, MOVE A SINGLE WORD TO DWORD-ALIGN THE
                   // DESTINATION
                   test      edi,2
                   jz        ds_aligned4
                   mov       cx,[esi]
                   add       esi,2
                   mov       [edi],cx
                   add       edi,2
                   sub       al,2
      ds_aligned4:

                   // MOVE AS MANY ALIGNED DWORDS AS POSSIBLE
                   mov       ecx,eax
                   and       ecx,0FCh
                   shr       ecx,2
                   rep       movsd
      ds_done4:

                   // MOVE ONE MORE WORD IF NECESSARY
                   test      al,2
                   jz        ds_done2
                   mov       cx,[esi]
                   add       esi,2
                   mov       [edi],cx
                   add       edi,2
      ds_done2:

                   // MOVE ONE MORE BYTE IF NECESSARY
                   test      al,1
                   jz        ds_done1
                   mov       cl,[esi]
                   inc       esi
                   mov       [edi],cl
                   inc       edi
      ds_done1:

                   pop       edi
                   pop       esi
  }
#else
        CopyMemory(destptr,
                   sourceptr,
                   xspan);
#endif
      }

      destptr += xspan;
      x       += xspan;

      for (loop = 0; loop < compositenum; ++loop) {
        compositedata[loop].dataptr += xspan;
        int advance = xspan;
        while (advance >= compositedata[loop].currspan) {
          advance -= compositedata[loop].currspan;
          if ((!((DWORD)compositedata[loop].spanptr & 1)) &&
              !*(LPWORD)compositedata[loop].spanptr)
            compositedata[loop].currspan = INT_MAX;
          else
            compositedata[loop].currspan = *compositedata[loop].spanptr++;
        }
        compositedata[loop].currspan -= advance;
      }

    }

    destptr += pitch-(rect->right-rect->left);
  }
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
void APIENTRY S2dImageCreate (int       width,
                              int       height,
                              int       bitdepth,
                              COLORREF  colorkey,
                              HSIMAGE  *handle) {
  VALIDATEBEGIN;
  VALIDATEANDBLANK(handle);
  VALIDATEENDVOID;

  if (!CheckForSupportedColorFormat(bitdepth,colorkey))
    return;

  s_apicritsect.Enter();

  // CREATE A NEW IMAGE RECORD
  HLOCKEDIMAGE lockedhandle;
  IMAGEPTR     imageptr = s_imagetable.NewLock(handle,&lockedhandle);
  imageptr->size.cx  = width;
  imageptr->size.cy  = height;
  imageptr->boundingrect.left   = UINT_MAX;
  imageptr->boundingrect.top    = UINT_MAX;
  imageptr->boundingrect.right  = UINT_MAX;
  imageptr->boundingrect.bottom = UINT_MAX;
  imageptr->transidx = (BYTE)(colorkey & 0xFF);
  imageptr->refcount = 1;
  s_imagetable.Unlock(lockedhandle);

  s_apicritsect.Leave();
}

//===========================================================================
BOOL APIENTRY S2dImageCreateFromFile (LPCTSTR   filename,
                                      COLORREF  colorkey,
                                      DWORD     numimages,
                                      LPCRECT   rectarray,
                                      HSIMAGE  *handlearray) {
  if (numimages && handlearray)
    ZeroMemory(handlearray,numimages*sizeof(HSIMAGE));

  VALIDATEBEGIN;
  VALIDATE(filename && *filename);
  VALIDATE(numimages);
  VALIDATE(handlearray);
  VALIDATEEND;

  s_apicritsect.Enter();
  HSFILE file       = (HSFILE)0;
  LPBYTE filebuffer = NULL;
  BOOL   result     = FALSE;
  TRY {

    // LOAD THE FILE FROM DISK
    if (!SFileOpenFile(filename,&file))
      LEAVE;
    DWORD filesize = SFileGetFileSize(file,NULL);
    filebuffer = (LPBYTE)ALLOC(filesize);
    if (!SFileReadFile(file,filebuffer,filesize))
      LEAVE;

    // DETERMINE THE SOURCE IMAGE DIMENSIONS
    int width, height, bitdepth;
    if (!SBmpDecodeImage(SBMP_IMAGETYPE_AUTO,
                         filebuffer,
                         filesize,
                         NULL,
                         NULL,
                         0,
                         &width,
                         &height,
                         &bitdepth))
      LEAVE;

    // VERIFY THAT THE FILE IS IN A SUPPORTED COLOR FORMAT
    if (!CheckForSupportedColorFormat(bitdepth,colorkey))
      LEAVE;

    // DECODE THE FILE INTO A TEMPORARY BUFFER
    int    bytedepth   = bitdepth/8;
    DWORD  sourcebytes = width*height*(DWORD)bytedepth;
    LPBYTE buffer      = (LPBYTE)ALLOC(sourcebytes);
    SBmpDecodeImage(SBMP_IMAGETYPE_AUTO,
                    filebuffer,
                    filesize,
                    NULL,
                    buffer,
                    sourcebytes);

    // PROCESS EACH IMAGE
    for (DWORD loop = 0;
         loop < numimages;
         ++loop) {

      // DETERMINE THE IMAGE DIMENSIONS
      int destwidth  = width;
      int destheight = height;
      if (rectarray) {
        destwidth  = min(destwidth ,rectarray[loop].right+1-rectarray[loop].left);
        destheight = min(destheight,rectarray[loop].bottom+1-rectarray[loop].top);
      }

      // CREATE THE IMAGE
      S2dImageCreate(destwidth,
                     destheight,
                     bitdepth,
                     colorkey,
                     &handlearray[loop]);

      // LOCK THE IMAGE BUFFER
      LPBYTE ptr;
      S2dImageLockBuffer(handlearray[loop],TRUE,&ptr);

      // BLT THE SOURCE RECTANGLE INTO THE IMAGE BUFFER
      SIZE adjdestsize   = {destwidth*bytedepth,
                            destheight};
      SIZE adjsourcesize = {width*bytedepth,
                            height};
      RECT adjsourcerect = {0,
                            0,
                            adjsourcesize.cx,
                            adjsourcesize.cy};
      if (rectarray) {
        adjsourcerect.left   = rectarray[loop].left*bytedepth;
        adjsourcerect.top    = rectarray[loop].top;
        adjsourcerect.right  = rectarray[loop].right*bytedepth;
        adjsourcerect.bottom = rectarray[loop].bottom;
      }
      SBltROP3Clipped(ptr,
                      NULL,
                      &adjdestsize,
                      adjdestsize.cx,
                      buffer,
                      &adjsourcerect,
                      &adjsourcesize,
                      adjsourcesize.cx,
                      0,
                      SRCCOPY);

      // UNLOCK THE IMAGE BUFFER
      S2dImageUnlockBuffer(handlearray[loop],ptr);

    }

    // FREE THE TEMPORARY BUFFER
    FREE(buffer);

    result = TRUE;
  }
  FINALLY {
    FREEIFUSED(filebuffer);
    if (file)
      SFileCloseFile(file);
  }
  s_apicritsect.Leave();
  return result;
}

//===========================================================================
void APIENTRY S2dImageDelete (HSIMAGE handle) {
  s_apicritsect.Enter();
  HLOCKEDIMAGE lockedhandle;
  IMAGEPTR     imageptr = s_imagetable.Lock(handle,&lockedhandle);
  if (!--imageptr->refcount)
    s_imagetable.DeleteUnlock(imageptr,lockedhandle);
  else
    s_imagetable.Unlock(lockedhandle);
  s_apicritsect.Leave();
}

//===========================================================================
BOOL APIENTRY S2dImageLockBuffer (HSIMAGE  handle,
                                  BOOL     blank,
                                  LPBYTE  *ptr) {
  VALIDATEBEGIN;
  VALIDATEANDBLANK(ptr);
  VALIDATEEND;

  s_apicritsect.Enter();
  for (ONCE) {

    // FIND THE IMAGE RECORD
    HLOCKEDIMAGE lockedhandle;
    IMAGEPTR     imageptr = s_imagetable.Lock(handle,&lockedhandle);
    if (!imageptr)
      break;

    // ALLOCATE A BUFFER IF NECESSARY
    DWORD bytes = imageptr->size.cx*imageptr->size.cy;
    if (!imageptr->imagedata)
      imageptr->imagedata = (LPBYTE)ALLOC(bytes);

    // BLANK THE BUFFER IF REQUESTED
    if (blank)
      FillMemory(imageptr->imagedata,bytes,imageptr->transidx);

    // RETURN THE BUFFER
    *ptr = imageptr->imagedata;

    s_imagetable.Unlock(lockedhandle);
  }
  s_apicritsect.Leave();
  return (*ptr != NULL);
}

//===========================================================================
void APIENTRY S2dImageUnlockBuffer (HSIMAGE handle,
                                    LPBYTE  ptr) {
  s_apicritsect.Enter();
  for (ONCE) {

    // FIND THE IMAGE RECORD
    HLOCKEDIMAGE lockedhandle;
    IMAGEPTR     imageptr = s_imagetable.Lock(handle,&lockedhandle);
    if (!imageptr)
      break;

    // PRODUCE THE SPAN DATA
    ImageCreateSpanData(imageptr);

    s_imagetable.Unlock(lockedhandle);
  }
  s_apicritsect.Leave();
}

//===========================================================================
void APIENTRY S2dSpriteCreate (HSSPRITE *handle) {
  VALIDATEBEGIN;
  VALIDATEANDBLANK(handle);
  VALIDATEENDVOID;

  s_apicritsect.Enter();
  s_spritetable.New(handle);
  s_apicritsect.Leave();
}

//===========================================================================
void APIENTRY S2dSpriteDelete (HSSPRITE handle) {
  s_apicritsect.Enter();
  s_spritetable.Delete(handle);
  s_apicritsect.Leave();
}

//===========================================================================
void APIENTRY S2dSpriteSetImage (HSSPRITE handle,
                                 HSIMAGE  imagehandle) {
  VALIDATEBEGIN;
  VALIDATE(handle);
  VALIDATE(imagehandle);
  VALIDATEENDVOID;

  s_apicritsect.Enter();

  // LOCK THE SPRITE AND IMAGE
  HLOCKEDSPRITE lockedsprite;
  HLOCKEDIMAGE  lockedimage;
  SPRITEPTR     spriteptr = s_spritetable.Lock(handle,&lockedsprite);
  IMAGEPTR      imageptr  = s_imagetable.Lock(imagehandle,&lockedimage);
  if (spriteptr->image != imageptr) {

    // REMOVE THE REFERENCE TO THE SPRITE'S OLD IMAGE
    if (spriteptr->image)
      SpriteRemoveImageReference(spriteptr);

    // ASSOCIATE THE IMAGE WITH THE SPRITE
    if (imageptr) {
      ++imageptr->refcount;
      spriteptr->image = imageptr;
      imageptr->spritelist.LinkNode(spriteptr);
    }

  }

  // UNLOCK THE SPRITE AND IMAGE
  s_imagetable.Unlock(lockedimage);
  s_spritetable.Unlock(lockedsprite);

  s_apicritsect.Leave();
}

//===========================================================================
void APIENTRY S2dSpriteSetPos (HSSPRITE handle,
                               int      x,
                               int      y) {
  VALIDATEBEGIN;
  VALIDATE(handle);
  VALIDATEENDVOID;

  s_apicritsect.Enter();
  for (ONCE) {

    // LOCK THE SPRITE
    HLOCKEDSPRITE lockedhandle;
    SPRITEPTR     spriteptr = s_spritetable.Lock(handle,&lockedhandle);
    if (!spriteptr)
      break;

    // PLACE AN ERASE MARKER AT THE SPRITE'S OLD POSITION
    if (spriteptr->flags & SF_DISPLAYED) {
      SPRITEPTR newptr = spriteptr->target->spritezlist.NewNode(LIST_UNLINKED);
      newptr->pos          = spriteptr->pos;
      newptr->boundingrect = spriteptr->boundingrect;
      newptr->flags        = SF_ERASEMARKER;
      newptr->zorder       = spriteptr->zorder;
      newptr->target       = spriteptr->target;
      spriteptr->target->spritezlist.LinkNode(newptr,LIST_LINK_AFTER,spriteptr);
    }

    // SET THE SPRITE'S POSITION AND BOUNDING RECTANGLE
    spriteptr->pos.x = x;
    spriteptr->pos.y = y;
    spriteptr->boundingrect.left   = x+spriteptr->image->boundingrect.left;
    spriteptr->boundingrect.top    = y+spriteptr->image->boundingrect.top;
    spriteptr->boundingrect.right  = x+spriteptr->image->boundingrect.right;
    spriteptr->boundingrect.bottom = y+spriteptr->image->boundingrect.bottom;
    spriteptr->flags &= ~SF_DISPLAYED;

    // UNLOCK THE SPRITE
    s_spritetable.Unlock(lockedhandle);

  }
  s_apicritsect.Leave();
}

//===========================================================================
void APIENTRY S2dSpriteSetTarget (HSSPRITE handle,
                                  HSTARGET targethandle,
                                  int      zorder) {
  VALIDATEBEGIN;
  VALIDATE(handle);
  VALIDATE(targethandle);
  VALIDATEENDVOID;

  s_apicritsect.Enter();

  // LOCK THE SPRITE AND TARGET
  HLOCKEDSPRITE lockedsprite;
  HLOCKEDTARGET lockedtarget;
  SPRITEPTR     spriteptr = s_spritetable.Lock(handle,&lockedsprite);
  TARGETPTR     targetptr = s_targettable.Lock(targethandle,&lockedtarget);

  // SET THE SPRITE'S Z ORDER AND LINK IT INTO THE TARGET'S Z LIST
  spriteptr->zorder = zorder;
  spriteptr->target = targetptr;
  SpriteLinkToZList(targetptr,spriteptr);

  // UNLOCK THE SPRITE AND TARGET
  s_targettable.Unlock(lockedtarget);
  s_spritetable.Unlock(lockedsprite);

  s_apicritsect.Leave();
}

//===========================================================================
void APIENTRY S2dTargetCreate (HSTARGET *handle) {
  VALIDATEBEGIN;
  VALIDATEANDBLANK(handle);
  VALIDATEENDVOID;

  s_apicritsect.Enter();
  s_targettable.New(handle);
  s_apicritsect.Leave();
}

//===========================================================================
void APIENTRY S2dTargetDelete (HSTARGET handle) {
  s_apicritsect.Enter();
  s_targettable.Delete(handle);
  s_apicritsect.Leave();
}

//===========================================================================
void APIENTRY S2dTestDrawTarget (HSTARGET handle,
                                 LPBYTE   ptr,
                                 int      pitch) {
  s_apicritsect.Enter();
  for (ONCE) {

    HLOCKEDTARGET lockedhandle;
    TARGETPTR     targetptr = s_targettable.Lock(handle,&lockedhandle);
    if (!targetptr)
      break;

    TargetBuildDirtyRects(targetptr);
    for (DWORD rectindex = 0;
         rectindex < targetptr->dirtyrects.NumElements();
         ++rectindex) {
      DWORD totalparams;
      targetptr->dirtyrgn.GetRectParams(&targetptr->dirtyrects[rectindex],
                                        &totalparams,
                                        NULL);
      targetptr->dirtyrectparams.SetNumElements(totalparams);
      targetptr->dirtyrgn.GetRectParams(&targetptr->dirtyrects[rectindex],
                                        &totalparams,
                                        targetptr->dirtyrectparams.Ptr());
      TargetCompositeRect(ptr,
                          pitch,
                          targetptr,
                          &targetptr->dirtyrects[rectindex],
                          totalparams,
                          (SPRITEPTR *)(targetptr->dirtyrectparams.Ptr()));
    }

    ITERATELIST(SPRITE,targetptr->spritezlist,spriteptr)
      if (spriteptr->flags & SF_ERASEMARKER)
        ITERATE_DELETE
      else
        spriteptr->flags |= SF_DISPLAYED;

    s_targettable.Unlock(lockedhandle);
  }
  s_apicritsect.Leave();
}

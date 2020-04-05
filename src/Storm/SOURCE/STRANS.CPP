/****************************************************************************
*
*  STRANS.CPP
*  Storm transparency functions
*
*  By Michael O'Brien (6/19/96)
*
***/

#include "pch.h"
#pragma  hdrstop

#define  COPY           0
#define  SKIP           1
#define  MAXSPANLENGTH  0xFC

typedef struct _BUFFER {
    LPBYTE data;
    DWORD  bytesalloc;
    DWORD  bytesused;
    DWORD  chunksize;
} BUFFER, *BUFFERPTR;

typedef union _INSTPTR {
    LPBYTE byteptr;
    LPWORD wordptr;
} INSTPTR;

typedef union _SPANPAIR {
    BYTE span[2];
    WORD pair;
} SPANPAIR;

NODEDECL(TRANS) {
    LPBYTE data;
    DWORD  dataalloc;
    DWORD  databytes;
    DWORD  instructionoffset;
    int    width;
    int    height;
    RECT   boundrect;
} *TRANSPTR;

static LPDWORD     s_dirtyoffset   = NULL;
static SIZE        s_dirtysize     = {40,30};
static int         s_dirtyxshift   = 4;
static int         s_dirtyxsize    = 16;
static int         s_dirtyyshift   = 4;
static int         s_dirtyysize    = 16;
static LPBYTE      s_savedata      = NULL;
static int         s_savedataalloc = 0;
static LIST(TRANS) s_translist;

//===========================================================================
static inline void BufferCreate (BUFFERPTR buffer) {
  buffer->chunksize = 4096;
  buffer->bytesused = 0;
  if (s_savedata) {
    buffer->data       = s_savedata;
    buffer->bytesalloc = s_savedataalloc;
    s_savedata      = NULL;
    s_savedataalloc = 0;
  }
  else {
    buffer->bytesalloc = 4096;
    buffer->data       = (LPBYTE)ALLOC(buffer->bytesalloc);
  }
}

//===========================================================================
static inline void BufferReserve (BUFFERPTR  buffer, 
                                  DWORD      bytes,
                                  LPBYTE    *adjptr1,
                                  LPBYTE    *adjptr2) {
  DWORD newalloc = buffer->bytesalloc;
  while ((buffer->bytesused+bytes) > newalloc)
    newalloc += buffer->chunksize;
  if (newalloc != buffer->bytesalloc) {
    LPBYTE newdata = (LPBYTE)ALLOC(newalloc);
    CopyMemory(newdata,buffer->data,buffer->bytesused);
    FREE(buffer->data);
    if (adjptr1 && *adjptr1)
      *adjptr1 = newdata+((*adjptr1)-buffer->data);
    if (adjptr2 && *adjptr2)
      *adjptr2 = newdata+((*adjptr2)-buffer->data);
    buffer->bytesalloc = newalloc;
    buffer->data       = newdata;
  }
}

//===========================================================================
static void ConvertBitmapToTransparency (LPBYTE  bits,
                                         int     width,
                                         int     height,
                                         int     bitdepth,
                                         LPRECT  rect,
                                         LPRECT  boundrect,
                                         BYTE    colorkey,
                                         BOOL    maskonly,
                                         LPBYTE  data,
                                         DWORD  *databytes,
                                         DWORD  *instructionoffset) {
  DWORD size = 0;

  // DETERMINE THE SOURCE BITMAP LOCATION AND EXTENTS
  LPBYTE start   = bits+(rect ? rect->top*width+rect->left : 0);
  int    cx      = rect ? rect->right-rect->left : width;
  int    cy      = rect ? rect->bottom-rect->top : height;
  int    adjust  = width-cx;

  // INITIALIZE THE BOUNDING RECTANGLE
  boundrect->left   = INT_MAX;
  boundrect->top    = INT_MAX;
  boundrect->right  = 0;
  boundrect->bottom = 0;

  // STORE ALL OF THE NON-TRANSPARENT BITMAP BITS IN THE BUFFER
  if (!maskonly) {
    LPBYTE source = start;
    for (int y = 0; y < cy; ++y) {
      BOOL found = 0;
      for (int x = 0; x < cx; ++x) {
        if (*source != colorkey) {
          ++size;
          if (data)
            *(data++) = *source;
          if (x < boundrect->left)
            boundrect->left = x;
          if (x >= boundrect->right)
            boundrect->right = x+1;
          found = 1;
        }
        ++source;
      }
      if (found) {
        if (y < boundrect->top)
          boundrect->top = y;
        if (y >= boundrect->bottom)
          boundrect->bottom = y+1;
      }
      source += adjust;
    }
  }
  if (boundrect->left > boundrect->right)
    boundrect->left = boundrect->right;
  if (boundrect->top > boundrect->bottom)
    boundrect->top = boundrect->bottom;

  // SAVE THE OFFSET TO THE INSTRUCTION STREAM
  if (size & 3) {
    if (data)
      data += 4-(size & 3);
    size += 4-(size & 3);
  }
  if (instructionoffset)
    *instructionoffset = size;

  // CREATE THE INSTRUCTION STREAM.  EACH INSTRUCTION CONSISTS OF A BYTE
  // SPECIFYING THE NUMBER OF BYTES TO COPY, FOLLOWED BY A BYTE SPECIFYING
  // THE NUMBER OF BYTES TO SKIP.  A PAIR OF ZEROES IS USED TO INDICATE THE
  // END OF THE CURRENT SCAN LINE.
  {
    LPBYTE source = start;
    int    y      = cy;
    while (y--) {
      BYTE copybytes = 0;
      BYTE skipbytes = 0;
      BOOL copymode  = TRUE;
      int  x         = cx;
      while (x--) {
        BOOL output = FALSE;
        if ((*(source++) != colorkey) == copymode)
          if (copymode)
            ++copybytes;
          else
            ++skipbytes;
        else
          if (copymode) {
            copymode = FALSE;
            ++skipbytes;
          }
          else {
            output = TRUE;
            --source;
            ++x;
          }
        if (output ||
            (copybytes == MAXSPANLENGTH) ||
            (skipbytes == MAXSPANLENGTH) ||
            !x) {
          size += 2;
          if (data) {
            *(data++) = copybytes;
            *(data++) = skipbytes;
          }
          copybytes = 0;
          skipbytes = 0;
          copymode  = TRUE;
        }
      }
      size += 2;
      if (data) {
        *(data++) = 0;
        *(data++) = 0;
      }
      source += adjust;
    }
  }

  // RETURN THE NUMBER OF BYTES WRITTEN
  if (databytes)
    *databytes = size;
}

//===========================================================================
static DWORD ConvertColorRefToColorData (COLORREF colorref, int bitdepth) {
  if (colorref & 0x01000000)
    return (colorref & 0x00FFFFFF);

  return 0;
}

//===========================================================================
static TRANSPTR CreateTransparencyRecord (TRANSPTR baseptr) {
  TRANSPTR transptr = s_translist.NewNode();
  if (baseptr) {
    transptr->dataalloc         = baseptr->dataalloc;
    transptr->databytes         = baseptr->databytes;
    transptr->instructionoffset = baseptr->instructionoffset;
    transptr->width             = baseptr->width;
    transptr->height            = baseptr->height;
    transptr->boundrect         = baseptr->boundrect;
  }
  return transptr;
}

//===========================================================================
static BOOL DetermineShift (int value, int *shift) {
  int bits = 0;
  int curr = 1;
  while (curr < value) {
    ++bits;
    curr <<= 1;
  }
  *shift = bits;
  return (curr == value);
}

//===========================================================================
static BOOL InternalCreateTransparency (LPBYTE    bits,
                                        int       width,
                                        int       height,
                                        int       bitdepth,
                                        LPRECT    rect,
                                        COLORREF  colorkey,
                                        BOOL      maskonly,
                                        HSTRANS  *handle) {

  // STORM CURRENTLY ONLY SUPPORTS 256-COLOR MODE, SO FOR THE TIME BEING,
  // REJECT ALL BIT DEPTHS EXCEPT 8BPP
  if (bitdepth != 8)
    return FALSE;
  BYTE paletteindex = (BYTE)ConvertColorRefToColorData(colorkey,bitdepth);

  // DETERMINE THE SIZE OF THE TRANSPARENCY DATA
  DWORD transbytes        = 0;
  DWORD instructionoffset = 0;
  RECT  boundrect;
  ConvertBitmapToTransparency(bits,width,height,bitdepth,rect,&boundrect,paletteindex,
                              maskonly,NULL,&transbytes,&instructionoffset);

  // ALLOCATE MEMORY FOR THE TRANSPARENCY DATA
  LPBYTE transdata = (LPBYTE)ALLOC(transbytes);

  // GENERATE THE TRANSPARENCY DATA
  ConvertBitmapToTransparency(bits,width,height,bitdepth,rect,&boundrect,paletteindex,
                              maskonly,transdata,NULL,NULL);

  // CREATE A RECORD FOR THE TRANSPARENCY
  TRANSPTR newptr = CreateTransparencyRecord(NULL);
  CopyMemory(&newptr->boundrect,&boundrect,sizeof(RECT));
  newptr->data              = transdata;
  newptr->dataalloc         = transbytes;
  newptr->databytes         = transbytes;
  newptr->instructionoffset = instructionoffset;
  newptr->width             = rect ? rect->right-rect->left : width;
  newptr->height            = rect ? rect->bottom-rect->top : height;

  // RETURN A HANDLE TO THE TRANSPARENCY
  *handle = (HSTRANS)newptr;

  return TRUE;
}

//===========================================================================
static void inline InternalDrawTransparency (TRANSPTR trans,
                                             LPBYTE   dest,
                                             int      destadjust) {
  LPBYTE sourcedata = trans->data;
  LPBYTE sourceinst = trans->data+trans->instructionoffset;
  int    cy         = trans->height;
#ifdef _X86_
  __asm {

                   // SETUP REGISTERS
                   mov       edx,[cy]
                   mov       ebx,[sourceinst]
                   mov       esi,[sourcedata]
                   mov       edi,[dest]
                   xor       eax,eax
                   xor       ecx,ecx
                   test      edx,edx
                   jz        dt_done
                   jmp       dt_nextinst

                   // PERFORM THE COPY
      dt_copy:     cmp       al,3
                   jbe       dt_done4

                   // IF NECESSARY, MOVE A SINGLE BYTE TO WORD-ALIGN THE
                   // DESTINATION
                   test      edi,1
                   jz        dt_aligned2
                   mov       cl,[esi]
                   inc       esi
                   mov       [edi],cl
                   inc       edi
                   dec       al
      dt_aligned2:

                   // IF NECESSARY, MOVE A SINGLE WORD TO DWORD-ALIGN THE
                   // DESTINATION
                   test      edi,2
                   jz        dt_aligned4
                   mov       cx,[esi]
                   add       esi,2
                   mov       [edi],cx
                   add       edi,2
                   sub       al,2
      dt_aligned4:

                   // MOVE AS MANY ALIGNED DWORDS AS POSSIBLE
                   mov       ecx,eax
                   and       ecx,0FCh
                   shr       ecx,2
                   rep       movsd
      dt_done4:

                   // MOVE ONE MORE WORD IF NECESSARY
                   test      al,2
                   jz        dt_done2
                   mov       cx,[esi]
                   add       esi,2
                   mov       [edi],cx
                   add       edi,2
      dt_done2:

                   // MOVE ONE MORE BYTE IF NECESSARY
                   test      al,1
                   jz        dt_done1
                   mov       cl,[esi]
                   inc       esi
                   mov       [edi],cl
                   inc       edi
      dt_done1:

                   // PERFORM THE SKIP
                   xor       ecx,ecx
                   mov       cl,ah
                   add       edi,ecx

                   // LOAD IN THE NEXT SET OF COPY/SKIP LENGTHS
      dt_nextinst: xor       eax,eax
                   mov       ax,[ebx]
                   add       ebx,2
                   test      eax,eax
                   jnz       dt_copy

                   // IF THIS LINE IS COMPLETE, MOVE TO THE NEXT LINE
                   add       edi,[destadjust]
                   dec       edx
                   jnz       dt_nextinst

      dt_done:
  }
#else

  // DRAW EACH LINE AS DEFINED IN THE INSTRUCTION STREAM
  while (cy--) {
    BYTE copybytes = *(sourceinst++);
    BYTE skipbytes = *(sourceinst++);
    while (copybytes || skipbytes) {

      // IF WE ARE COPYING AT LEAST SEVEN BYTES, ARRANGE THE BLT SO WE CAN
      // DO AS MANY ALIGNED DWORD MOVES AS POSSIBLE
      if (copybytes >= 7) {
        while ((DWORD)dest & 3) {
          *(dest++) = *(sourcedata++);
          --copybytes;
        }
        while (copybytes >= 4) {
          *(LPDWORD)dest = *(LPDWORD)sourcedata;
          dest       += 4;
          sourcedata += 4;
          copybytes  -= 4;
        }
      }

      // OTHERWISE, JUST DO BYTE MOVES
      while (copybytes) {
        *(dest++) = *(sourcedata++);
        --copybytes;
      }

      // SKIP PAST THE NEXT TRANSPARENCY SPAN
      dest += skipbytes;

      // LOAD IN THE NEXT SPAN DATA
      copybytes = *(sourceinst++);
      skipbytes = *(sourceinst++);

    } 
    dest += destadjust;
  }

#endif
}

//===========================================================================
static void inline InternalDrawTransparencyFromSource (TRANSPTR trans,
                                                       LPBYTE   dest,
                                                       LPBYTE   source,
                                                       int      destadjust,
                                                       int      sourceadjust) {
  LPBYTE sourceinst = trans->data+trans->instructionoffset;
  int    cy         = trans->height;
#ifdef _X86_
  __asm {

                   // SETUP REGISTERS
                   mov       edx,[cy]
                   mov       ebx,[sourceinst]
                   mov       esi,[source]
                   mov       edi,[dest]
                   xor       eax,eax
                   xor       ecx,ecx
                   test      edx,edx
                   jz        dt_done
                   jmp       dt_nextinst

                   // PERFORM THE COPY
      dt_copy:     cmp       al,3
                   jbe       dt_done4

                   // IF NECESSARY, MOVE A SINGLE BYTE TO WORD-ALIGN THE
                   // DESTINATION
                   test      edi,1
                   jz        dt_aligned2
                   mov       cl,[esi]
                   inc       esi
                   mov       [edi],cl
                   inc       edi
                   dec       al
      dt_aligned2:

                   // IF NECESSARY, MOVE A SINGLE WORD TO DWORD-ALIGN THE
                   // DESTINATION
                   test      edi,2
                   jz        dt_aligned4
                   mov       cx,[esi]
                   add       esi,2
                   mov       [edi],cx
                   add       edi,2
                   sub       al,2
      dt_aligned4:

                   // MOVE AS MANY ALIGNED DWORDS AS POSSIBLE
                   mov       ecx,eax
                   and       ecx,0FCh
                   shr       ecx,2
                   rep       movsd
      dt_done4:

                   // MOVE ONE MORE WORD IF NECESSARY
                   test      al,2
                   jz        dt_done2
                   mov       cx,[esi]
                   add       esi,2
                   mov       [edi],cx
                   add       edi,2
      dt_done2:

                   // MOVE ONE MORE BYTE IF NECESSARY
                   test      al,1
                   jz        dt_done1
                   mov       cl,[esi]
                   inc       esi
                   mov       [edi],cl
                   inc       edi
      dt_done1:

                   // PERFORM THE SKIP
                   xor       ecx,ecx
                   mov       cl,ah
                   add       esi,ecx
                   add       edi,ecx

                   // LOAD IN THE NEXT SET OF COPY/SKIP LENGTHS
      dt_nextinst: xor       eax,eax
                   mov       ax,[ebx]
                   add       ebx,2
                   test      eax,eax
                   jnz       dt_copy

                   // IF THIS LINE IS COMPLETE, MOVE TO THE NEXT LINE
                   add       edi,[destadjust]
                   add       esi,[sourceadjust]
                   dec       edx
                   jnz       dt_nextinst

      dt_done:
  }
#else

  // DRAW EACH LINE AS DEFINED IN THE INSTRUCTION STREAM
  while (cy--) {
    BYTE copybytes = *(sourceinst++);
    BYTE skipbytes = *(sourceinst++);
    while (copybytes || skipbytes) {

      // IF WE ARE COPYING AT LEAST SEVEN BYTES, ARRANGE THE BLT SO WE CAN
      // DO AS MANY ALIGNED DWORD MOVES AS POSSIBLE
      if (copybytes >= 7) {
        while ((DWORD)dest & 3) {
          *(dest++) = *(source++);
          --copybytes;
        }
        while (copybytes >= 4) {
          *(LPDWORD)dest = *(LPDWORD)source;
          dest      += 4;
          source    += 4;
          copybytes -= 4;
        }
      }

      // OTHERWISE, JUST DO BYTE MOVES
      while (copybytes) {
        *(dest++) = *(source++);
        --copybytes;
      }

      // SKIP PAST THE NEXT TRANSPARENCY SPAN
      dest   += skipbytes;
      source += skipbytes;

      // LOAD IN THE NEXT SPAN DATA
      copybytes = *(sourceinst++);
      skipbytes = *(sourceinst++);

    } 
    dest   += destadjust;
    source += sourceadjust;
  }

#endif
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
BOOL APIENTRY STransBlt (LPBYTE  dest,
                         int     destx,
                         int     desty,
                         int     destpitch,
                         HSTRANS transparency) {
  // to minimize function call overhead for this time critical function,
  // we use assert instead of validate so that the retail version has
  // no parameter checking code
  ASSERT(dest);
  ASSERT(destpitch > 0);
  ASSERT(transparency);

  TRANSPTR transptr = (TRANSPTR)transparency;
  ASSERT(transptr->instructionoffset);

  // COMPUTE THE DESTINATION X ADJUSTMENT FOR EACH SCAN LINE
  int xadjust = destpitch-transptr->width;

  // DRAW THE TRANSPARENCY 
  InternalDrawTransparency(transptr,
                           dest+(desty*destpitch)+destx,
                           xadjust);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY STransBltUsingMask (LPBYTE  dest,
                                  LPBYTE  source,
                                  int     destpitch,
                                  int     sourcepitch,
                                  HSTRANS mask) {
  // to minimize function call overhead for this time critical function,
  // we use assert instead of validate so that the retail version has
  // no parameter checking code
  ASSERT(dest);
  ASSERT(source);
  ASSERT(destpitch > 0);
  ASSERT(mask);

  TRANSPTR transptr = (TRANSPTR)mask;

  // COMPUTE THE X ADJUSTMENTS FOR EACH SCAN LINE
  int destadjust   = destpitch-transptr->width;
  int sourceadjust = sourcepitch ? sourcepitch-transptr->width : 0;

  // DRAW USING THE TRANSPARENCY MASK
  InternalDrawTransparencyFromSource(transptr,
                                     dest,
                                     source,
                                     destadjust,
                                     sourceadjust);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY STransCreateE (LPBYTE    bits,
                             int       width,
                             int       height,
                             int       bitdepth,
                             LPRECT    rect,
                             COLORREF  colorkey,
                             HSTRANS  *handle) {
  if (handle)
    *handle = (HSTRANS)0;

  VALIDATEBEGIN;
  VALIDATE(bits);
  VALIDATE(width);
  VALIDATE(height);
  VALIDATE(handle);
  VALIDATEEND;

  return InternalCreateTransparency(bits,
                                    width,
                                    height,
                                    bitdepth,
                                    rect,
                                    colorkey,
                                    FALSE,
                                    handle);
}

//===========================================================================
BOOL APIENTRY STransCreateI (LPBYTE    bits,
                             int       width,
                             int       height,
                             int       bitdepth,
                             LPRECT    rect,
                             COLORREF  colorkey,
                             HSTRANS  *handle) {
  RECT   exclrect;
  LPRECT exclrectptr = rect;
  if (rect) {
    exclrect.left   = rect->left;
    exclrect.top    = rect->top;
    exclrect.right  = rect->right+1;
    exclrect.bottom = rect->bottom+1;
    exclrectptr = &exclrect;
  }
  return STransCreateE(bits,
                       width,
                       height,
                       bitdepth,
                       exclrectptr,
                       colorkey,
                       handle);
}

//===========================================================================
BOOL APIENTRY STransCreateMaskE (LPBYTE    bits,
                                 int       width,
                                 int       height,
                                 int       bitdepth,
                                 LPRECT    rect,
                                 COLORREF  colorkey,
                                 HSTRANS  *handle) {
  if (handle)
    *handle = (HSTRANS)0;

  VALIDATEBEGIN;
  VALIDATE(bits);
  VALIDATE(width);
  VALIDATE(height);
  VALIDATE(handle);
  VALIDATEEND;

  return InternalCreateTransparency(bits,
                                    width,
                                    height,
                                    bitdepth,
                                    rect,
                                    colorkey,
                                    TRUE,
                                    handle);
}

//===========================================================================
BOOL APIENTRY STransCreateMaskI (LPBYTE    bits,
                                 int       width,
                                 int       height,
                                 int       bitdepth,
                                 LPRECT    rect,
                                 COLORREF  colorkey,
                                 HSTRANS  *handle) {
  RECT   exclrect;
  LPRECT exclrectptr = rect;
  if (rect) {
    exclrect.left   = rect->left;
    exclrect.top    = rect->top;
    exclrect.right  = rect->right+1;
    exclrect.bottom = rect->bottom+1;
    exclrectptr = &exclrect;
  }
  return STransCreateMaskI(bits,
                           width,
                           height,
                           bitdepth,
                           exclrectptr,
                           colorkey,
                           handle);
}

//===========================================================================
BOOL APIENTRY STransDelete (HSTRANS handle) {
  TRANSPTR transptr = (TRANSPTR)handle;
  if (transptr->data) {
    if (s_savedata)
      FREE(s_savedata);
    s_savedata      = transptr->data;
    s_savedataalloc = transptr->dataalloc;
    transptr->data       = NULL;
    transptr->dataalloc  = 0;
  }
  s_translist.DeleteNode(transptr);
  return TRUE;
}

//===========================================================================
BOOL APIENTRY STransDestroy () {
  TRANSPTR curr;
  while ((curr = s_translist.Head()) != NULL) {
    REPORTRESOURCELEAK(HSTRANS);
    STransDelete((HSTRANS)curr);
  }
  if (s_dirtyoffset) {
    FREE(s_dirtyoffset);
    s_dirtyoffset = NULL;
  }
  if (s_savedata) {
    FREE(s_savedata);
    s_savedata      = NULL;
    s_savedataalloc = 0;
  }
  return TRUE;
}

//===========================================================================
BOOL APIENTRY STransDuplicate (HSTRANS  source,
                               HSTRANS *handle) {
  if (handle)
    *handle = (HSTRANS)0;

  VALIDATEBEGIN;
  VALIDATE(source);
  VALIDATE(handle);
  VALIDATEEND;

  TRANSPTR sourceptr = (TRANSPTR)source;
  LPBYTE   data      = (LPBYTE)ALLOC(sourceptr->dataalloc);
  CopyMemory(data,sourceptr->data,sourceptr->databytes);
  TRANSPTR newptr = CreateTransparencyRecord(sourceptr);
  newptr->data = data;
  *handle = (HSTRANS)newptr;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY STransInvertMask (HSTRANS  sourcemask,
                                HSTRANS *handle) {
  if (handle)
    *handle = (HSTRANS)0;

  VALIDATEBEGIN;
  VALIDATE(sourcemask);
  VALIDATE(handle);
  VALIDATEEND;

  TRANSPTR sourceptr = (TRANSPTR)sourcemask;

  // ALLOCATE MEMORY FOR THE NEW MASK DATA
  DWORD  dataalloc = (sourceptr->databytes-sourceptr->instructionoffset)
                       +2*sourceptr->height;
  LPBYTE data      = (LPBYTE)ALLOC(dataalloc);

  // COPY THE SOURCE MASK DATA, INVERTED, INTO THE DESTINATION BUFFER
  DWORD bytes = 0;
  {
    LPBYTE source = sourceptr->data+sourceptr->instructionoffset;
    LPBYTE dest   = data;
    int    cy     = sourceptr->height;
    while (cy--) {
      BYTE copybytes = 0;
      BYTE skipbytes = 0;
      do {
        skipbytes = *(source++);
        if (copybytes || skipbytes) {
          *(dest++) = copybytes;
          *(dest++) = skipbytes;
          bytes += 2;
        }
        copybytes = *(source++);
      } while (copybytes || skipbytes);
      *(dest++) = 0;
      *(dest++) = 0;
      bytes += 2;
    }
  }

  // CREATE A RECORD FOR THE TRANSPARENCY
  TRANSPTR newptr = CreateTransparencyRecord(sourceptr);
  newptr->data              = data;
  newptr->dataalloc         = dataalloc;
  newptr->databytes         = bytes;
  newptr->instructionoffset = 0;

  // RETURN A HANDLE TO THE TRANSPARENCY
  *handle = (HSTRANS)newptr;

  return TRUE;
}

//===========================================================================
BOOL APIENTRY STransIntersectDirtyArray (HSTRANS  sourcemask,
                                         LPBYTE   dirtyarray,
                                         BYTE     dirtyarraymask,
                                         HSTRANS *handle) {
  if (handle)
    *handle = (HSTRANS)0;

  VALIDATEBEGIN;
  VALIDATE(sourcemask);
  VALIDATE(dirtyarray);
  VALIDATE(dirtyarraymask);
  VALIDATE(handle);
  VALIDATEEND;

  if (!s_dirtyoffset)
    return FALSE;
  TRANSPTR sourceptr = (TRANSPTR)sourcemask;

  // ALLOCATE MEMORY FOR THE RESULT
  BUFFER buffer;
  BufferCreate(&buffer);

  // PERFORM THE INTERSECTION
  {
    LPBYTE source     = sourceptr->data+sourceptr->instructionoffset;
    LPBYTE dest       = buffer.data;
    LPBYTE lastsource = source;
    LPBYTE lastdest   = dest;
    int    y          = 0;
    while (y < sourceptr->height) {

      // MAKE SURE THERE IS ENOUGH MEMORY IN THE DESTINATION BUFFER FOR
      // THE LINE
      BufferReserve(&buffer,
                    sourceptr->width*2+2,
                    &dest,
                    &lastdest);

      // IF THIS LINE MATCHES THE PREVIOUS ONE, JUST COPY THE RESULT
      if ((y & (s_dirtyysize-1)) &&
          (*(LPDWORD)source == *(LPDWORD)lastsource) &&
          !memcmp(source+4,lastsource+4,source-lastsource-4)) {
        DWORD sourcebytes = source-lastsource;
        DWORD destbytes   = dest-lastdest;
        CopyMemory(dest,lastdest,destbytes);
        lastsource        = source;
        lastdest          = dest;
        source           += sourcebytes;
        dest             += destbytes;
        buffer.bytesused += destbytes;
      }

      // OTHERWISE, EXAMINE EACH SPAN FOR THIS LINE, SPLITTING COPY SPANS
      // AND TURNING PORTIONS OF THEM INTO SKIP SPANS AS NECESSARY
      else {
        lastsource = source;
        lastdest   = dest;
        LPBYTE dirty   = dirtyarray+*(s_dirtyoffset+(y >> s_dirtyyshift));
        DWORD  xoffset = 0;
        BYTE   copybytes;
        BYTE   skipbytes;
        do {
          copybytes = *(source++);
          skipbytes = *(source++);
          if (copybytes) {
            BYTE bytesleft = copybytes;
            BYTE length    = 0;
            BOOL copymode  = TRUE;
            while (bytesleft || length)
              if (bytesleft && (((*dirty & dirtyarraymask) != 0) == copymode)) {
                DWORD cellleft = s_dirtyxsize-xoffset;
                if (bytesleft < cellleft) {
                  length    += bytesleft;
                  xoffset   += bytesleft;
                  bytesleft  = 0;
                }
                else {
                  length    += (BYTE)cellleft;
                  bytesleft -= (BYTE)cellleft;
                  xoffset    = 0;
                  ++dirty;
                }
              }
              else {
                *(dest++) = length;
                buffer.bytesused++;
                length   = 0;
                copymode = !copymode;
              }
            if (skipbytes || !copymode) {
              if (copymode) {
                *(dest++) = 0;
                buffer.bytesused++;
              }
              *(dest++) = skipbytes;
              buffer.bytesused++;
            }
          }
          else {
            *(dest++) = 0;
            *(dest++) = skipbytes;
            buffer.bytesused += 2;
          }
          if (skipbytes) {
            xoffset += skipbytes;
            dirty   += (xoffset >> s_dirtyxshift);
            xoffset &= (s_dirtyxsize-1);
          }
        } while (copybytes || skipbytes);
      }

      ++y;
    }
  }

  // CREATE A RECORD FOR THE TRANSPARENCY
  TRANSPTR newptr = CreateTransparencyRecord(sourceptr);
  newptr->data              = buffer.data;
  newptr->dataalloc         = buffer.bytesalloc;
  newptr->databytes         = buffer.bytesused;
  newptr->instructionoffset = 0;

  // RETURN A HANDLE TO THE TRANSPARENCY
  *handle = (HSTRANS)newptr;

  return TRUE;
}

//===========================================================================
BOOL APIENTRY STransCombineMasks (HSTRANS  basemask,
                                  HSTRANS  secondmask,
                                  int      offsetx,
                                  int      offsety,
                                  DWORD    flags,
                                  HSTRANS *handle) {
  if (handle)
    *handle = (HSTRANS)0;

  VALIDATEBEGIN;
  VALIDATE(basemask);
  VALIDATE(secondmask);
  VALIDATE(handle);
  VALIDATEEND;

  // PARSE THE FLAGS
  BOOL intersect    = ((flags & 0x00000001) != 0);
  BOOL invertsecond = ((flags & 0x00000002) != 0);

  // GENERATE A TRUTH TABLE TO DEFINE WHICH SOURCE SPANS WILL BE COMBINED INTO WHICH
  // OUTPUT SPANS
  BOOL usespan[2][2][2];
  {
    for (int spantype0 = COPY; spantype0 <= SKIP; ++spantype0)
      for (int spantypedest = COPY; spantypedest <= SKIP; ++spantypedest) 
        if (intersect) {
          usespan[spantype0][spantypedest][invertsecond ? SKIP : COPY] = (spantype0 == spantypedest);
          usespan[spantype0][spantypedest][invertsecond ? COPY : SKIP] = (spantypedest == SKIP);
        }
        else {
          usespan[spantype0][spantypedest][invertsecond ? SKIP : COPY] = (spantypedest == COPY);
          usespan[spantype0][spantypedest][invertsecond ? COPY : SKIP] = (spantype0 == spantypedest);
        }
  }

  // GET POINTERS TO THE SOURCE TRANSPARENCY INFORMATION
  TRANSPTR sourceptr[2] = {(TRANSPTR)basemask,
                           (TRANSPTR)secondmask};
  INSTPTR  instptr[2];
  instptr[0].byteptr = sourceptr[0]->data+sourceptr[0]->instructionoffset;
  instptr[1].byteptr = sourceptr[1]->data+sourceptr[1]->instructionoffset;

  // SKIP PAST ANY SCAN LINES IN THE SECOND MASK WHICH ARE ABOVE THE TOP
  // OF THE BASE MASK
  if (offsety < 0)
    for (int line = offsety; line < 0; ++line)
      while (*instptr[1].wordptr++)
        ;

  // ALLOCATE MEMORY FOR THE RESULT
  BUFFER buffer;
  BufferCreate(&buffer);
  INSTPTR dest;
  dest.byteptr = buffer.data; 

  // PROCESS EACH SCAN LINE
  for (int line = 0; line < sourceptr[0]->height; ++line) {

    // MAKE SURE THERE IS ENOUGH MEMORY IN THE DESTINATION BUFFER FOR
    // THE LINE
    BufferReserve(&buffer,
                  sourceptr[0]->width*2+2,
                  &dest.byteptr,
                  NULL);

    // IF THIS LINE IS NOT WITHIN THE RANGE THAT THE SECOND MASK INTERSECTS,
    // THEN GENERATE A COMPLETE DUPLICATE OR A COMPLETE INTERSECTION OF THE
    // BASE MASK
    if ((line < offsety) ||
        (line > offsety+sourceptr[1]->height-1))
      if (intersect == invertsecond) {
        do
          buffer.bytesused += 2;
        while ((*dest.wordptr++ = *instptr[0].wordptr++) != 0);
      }
      else {
        int width = sourceptr[0]->width;
        while (width) {
          int skipspan = min(MAXSPANLENGTH,width);
          *dest.byteptr++ = 0;
          *dest.byteptr++ = (BYTE)skipspan;
          buffer.bytesused += 2;
          width -= skipspan;
        }
        *dest.wordptr++ = 0;
        buffer.bytesused += 2;
        while (*instptr[0].wordptr++)
          ;
      }

    // OTHERWISE, BUILD NEW INSTRUCTIONS FOR THIS SCAN LINE BY COMBINING THE
    // INSTRUCTIONS FROM THE BASE AND SECOND MASKS
    else {
      int  span[2][2] = {{0,0},{0,max(0,offsetx)}};
      BOOL hitend = FALSE;

      // SKIP PAST ANY PORTIONS OF THE SECOND MASK WHICH ARE ENTIRELY
      // TO THE LEFT OF THE BASE MASK
      if (offsetx < 0) {
        int bytesleft = -offsetx;
        while (bytesleft) {
          span[1][COPY] = *instptr[1].byteptr++;
          span[1][SKIP] = *instptr[1].byteptr++;
          if (!(span[1][COPY] || span[1][SKIP])) {
            span[1][SKIP] = INT_MAX;
            hitend = TRUE;
            break;
          }
          for (int adjustremaining = COPY; adjustremaining <= SKIP; ++adjustremaining) {
            int adjustment = min(bytesleft,span[1][adjustremaining]);
            span[1][adjustremaining] -= adjustment;
            bytesleft                -= adjustment;
          }
        }
      }


      // COMBINE THE BASE AND SECOND MASKS UNTIL THE BASE MASK IS EXHAUSTED
      for (;;) {

        // LOAD THE NEXT COPY/SKIP PAIR FROM THE FIRST MASK
        span[0][COPY] = *instptr[0].byteptr++;
        span[0][SKIP] = *instptr[0].byteptr++;
        if (!(span[0][COPY] || span[0][SKIP])) {
          *dest.wordptr++   = 0;
          buffer.bytesused += 2;
          break;
        }

        // COMBINE IT WITH INFORMATION FROM THE SECOND MASK STARTING
        // AT THE CURRENT COPY/SKIP POSITION
        for (int spantype0 = COPY; spantype0 <= SKIP; ++spantype0)
          while (span[0][spantype0]) {

            // IF WE'VE USED UP THE CURRENT SET OF SPANS FROM THE SECOND MASK,
            // LOAD THE NEXT SET
            if (!(span[1][COPY] || span[1][SKIP])) {
              span[1][COPY] = *instptr[1].byteptr++;
              span[1][SKIP] = *instptr[1].byteptr++;
              if (!(span[1][COPY] || span[1][SKIP])) {
                span[1][SKIP] = INT_MAX;
                hitend = TRUE;
              }
            }

            // GENERATE A NEW COPY SPAN AND A NEW SKIP SPAN
            SPANPAIR inst;
            for (int spantypedest = COPY; spantypedest <= SKIP; ++spantypedest) {
              int spanlength = 0;
              if (usespan[spantype0][spantypedest][COPY])
                spanlength = span[1][COPY];
              if (usespan[spantype0][spantypedest][SKIP] &&
                  (usespan[spantype0][spantypedest][COPY] || !span[1][COPY]))
                spanlength += span[1][SKIP];
              spanlength = min(spanlength,span[0][spantype0]);
              inst.span[spantypedest] = (BYTE)spanlength;
              span[0][spantype0] -= spanlength;
              for (int adjustremaining = COPY; adjustremaining <= SKIP; ++adjustremaining) 
                if (usespan[adjustremaining]) {
                  int adjustment = min(spanlength,span[1][adjustremaining]);
                  span[1][adjustremaining] -= adjustment;
                  spanlength               -= adjustment;
                }
            }
            if (inst.pair) {
              *dest.wordptr++   = inst.pair;
              buffer.bytesused += 2;
            }

          }

      }
      if (!hitend)
        while (*instptr[1].wordptr++)
          ;
    }

  }

  // CREATE A RECORD FOR THE TRANSPARENCY
  TRANSPTR newptr = CreateTransparencyRecord(sourceptr[0]);
  newptr->data              = buffer.data;
  newptr->dataalloc         = buffer.bytesalloc;
  newptr->databytes         = buffer.bytesused;
  newptr->instructionoffset = 0;

  // RETURN A HANDLE TO THE TRANSPARENCY
  *handle = (HSTRANS)newptr;

  return TRUE;
}

//===========================================================================
BOOL APIENTRY STransIsPixelInMask (HSTRANS mask,
                                   int     offsetx,
                                   int     offsety) {
  VALIDATEBEGIN;
  VALIDATE(mask);
  VALIDATEEND;

  // MAKE SURE THAT THE OFFSETS ARE WITHIN THE BOUNDS OF THE TRANSPARENCY
  TRANSPTR transptr = (TRANSPTR)mask;
  if ((offsetx < 0) ||
      (offsety < 0) ||
      (offsetx >= transptr->width) ||
      (offsety >= transptr->height))
    return FALSE;

  // SKIP TO THE CORRECT SCAN LINE
  LPWORD instwordptr = (LPWORD)(transptr->data+transptr->instructionoffset);
  while (offsety--)
    while (*instwordptr++)
      ;

  // SKIP TO THE CORRECT PIXEL
  LPBYTE instbyteptr = (LPBYTE)instwordptr;
  for (;;) {
    BYTE copybytes = *instbyteptr++;
    BYTE skipbytes = *instbyteptr++;

    // IF WE HIT THE END OF THE SCAN LINE, RETURN FALSE
    if (!(copybytes || skipbytes))
      return FALSE;

    // IF THE REQUESTED PIXEL FALLS WITHIN A COPY SPAN, RETURN TRUE
    if (copybytes > offsetx)
      return TRUE;
    offsetx -= copybytes;

    // IF THE REQUESTED PIXEL FALLS WITHIN A SKIP SPAN, RETURN FALSE
    if (skipbytes > offsetx)
      return FALSE;
    offsetx -= skipbytes;

  }

}

//===========================================================================
BOOL APIENTRY STransLoadE (LPCTSTR   filename,
                           LPRECT    rect,
                           COLORREF  colorkey,
                           HSTRANS  *handle) {
  if (handle)
    *handle = (HSTRANS)0;

  VALIDATEBEGIN;
  VALIDATE(filename);
  VALIDATE(*filename);
  VALIDATE(handle);
  VALIDATEEND;

  // DETERMINE THE SIZE OF THE BITMAP FILE
  int width  = 0;
  int height = 0;
  if (!SBmpLoadImage(filename,NULL,NULL,0,&width,&height))
    return FALSE;

  // READ THE BITMAP BITS
  LPBYTE buffer = (LPBYTE)ALLOC(width*height);
  if (!SBmpLoadImage(filename,NULL,buffer,width*height))
    return FALSE;

  // CREATE THE TRANSPARENCY
  STransCreate(buffer,
               width,
               height,
               8,
               rect,
               colorkey,
               handle);

  // FREE THE BITMAP BITS
  FREE(buffer);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY STransLoadI (LPCTSTR   filename,
                           LPRECT    rect,
                           COLORREF  colorkey,
                           HSTRANS  *handle) {
  RECT   exclrect;
  LPRECT exclrectptr = rect;
  if (rect) {
    exclrect.left   = rect->left;
    exclrect.top    = rect->top;
    exclrect.right  = rect->right+1;
    exclrect.bottom = rect->bottom+1;
    exclrectptr = &exclrect;
  }
  return STransLoadE(filename,
                     exclrectptr,
                     colorkey,
                     handle);
}

//===========================================================================
BOOL APIENTRY STransSetDirtyArrayInfo (int screencx,
                                       int screency,
                                       int cellcx,
                                       int cellcy) {

  // FREE THE OLD DIRTY ARRAY OFFSET TABLE
  if (s_dirtyoffset) {
    FREE(s_dirtyoffset);
    s_dirtyoffset = NULL;
  }

  // SAVE THE NEW CELL SIZES AND SHIFT VALUES
  if (!DetermineShift(cellcx,&s_dirtyxshift))
    return 0;
  if (!DetermineShift(cellcy,&s_dirtyyshift))
    return 0;
  s_dirtysize.cx = (screencx+(1 << s_dirtyxshift)-1) >> s_dirtyxshift;
  s_dirtysize.cy = (screency+(1 << s_dirtyyshift)-1) >> s_dirtyyshift;
  s_dirtyxsize   = cellcx;
  s_dirtyysize   = cellcy;

  // CREATE A NEW DIRTY ARRAY OFFSET TABLE
  s_dirtyoffset = (LPDWORD)ALLOC(s_dirtysize.cy*sizeof(DWORD));
  DWORD offset = 0;
  for (int loop = 0; loop < s_dirtysize.cy; ++loop) {
    *(s_dirtyoffset+loop) = offset;
    offset += s_dirtysize.cx;
  }

  return TRUE;
}

//===========================================================================
BOOL APIENTRY STransUpdateDirtyArray (LPBYTE  dirtyarray,
                                      BYTE    dirtyvalue,
                                      int     destx,
                                      int     desty,
                                      HSTRANS transparency,
                                      BOOL    tracecontour) {
  VALIDATEBEGIN;
  VALIDATE(dirtyarray);
  VALIDATE(dirtyvalue);
  VALIDATE(transparency);
  VALIDATEEND;

  if (!s_dirtyoffset)
    return FALSE;
  TRANSPTR transptr = (TRANSPTR)transparency;
  if (!((transptr->width > 0) && (transptr->height > 0)))
    return FALSE;

  // IF WE HAVE BEEN ASKED TO TRACE THE CONTOUR, THEN ONLY UPDATE THE VALUES
  // OF THOSE CELLS THAT ARE TOUCHED BY A NON-TRANSPARENT PORTION OF THE
  // TRANSPARENCY
  if (tracecontour) {
    // note: write this
    return FALSE;
  }

  // OTHERWISE, UPDATE THE WHOLE RECTANGLE CONTAINING THE TRANSPARENCY
  else {
    int lastx = (destx+transptr->boundrect.right)  >> s_dirtyxshift;
    int lasty = (desty+transptr->boundrect.bottom) >> s_dirtyyshift;
    destx = (destx+transptr->boundrect.left) >> s_dirtyxshift;
    desty = (desty+transptr->boundrect.top)  >> s_dirtyyshift;
    for (int y = desty; y < lasty; ++y) {
      LPBYTE dirty = dirtyarray+*(s_dirtyoffset+y)+destx;
      for (int x = destx; x < lastx; ++x)
        *(dirty++) |= dirtyvalue;
    }
  }

  return TRUE;
}

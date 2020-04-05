/****************************************************************************
*
*  SBMP.CPP
*  Storm bitmap functions
*
*  By Michael O'Brien (2/8/96)
*
***/

#include "pch.h"
#pragma  hdrstop

#define  BMPSIGNATURE  0x4D42
#define  PCXSIGNATURE  0x050A


/****************************************************************************
*
*  BMP ENCODER/DECODER
*
***/

static void FlipImage (LPBYTE dest,
                       LPBYTE source,
                       DWORD  destbytes,
                       int    destwidth,
                       int    sourcewidth,
                       int    height);

//===========================================================================
static BOOL DecodeBmpFile (HSFILE          file,
                           LPPALETTEENTRY  paletteentries,
                           LPBYTE          bitmapbits,
                           DWORD           buffersize,
                           int            *width,
                           int            *height,
                           int            *bitdepth) {

  // READ THE FILE HEADER
  BITMAPFILEHEADER fileheader;
  if (!SFileReadFile(file,&fileheader,sizeof(BITMAPFILEHEADER)))
    return FALSE;

  // READ THE BITMAP INFO HEADER
  BITMAPINFOHEADER infoheader;
  if (!SFileReadFile(file,&infoheader,sizeof(BITMAPINFOHEADER)))
    return FALSE;
  if (width)
    *width = infoheader.biWidth;
  if (height)
    *height = infoheader.biHeight;
  if (bitdepth)
    *bitdepth = infoheader.biBitCount;

  // READ THE PALETTE ENTRIES
  if (infoheader.biBitCount > 8)
    paletteentries = NULL;
  if (paletteentries) {
    RGBQUAD palettedata[256];
    if (!SFileReadFile(file,palettedata,256*sizeof(RGBQUAD)))
      return FALSE;

    // CONVERT FROM RGBQUAD TO PALETTEENTRY FORMAT
    for (int loop = 0; loop < 256; ++loop) {
      (paletteentries+loop)->peRed   = palettedata[loop].rgbRed;
      (paletteentries+loop)->peGreen = palettedata[loop].rgbGreen;
      (paletteentries+loop)->peBlue  = palettedata[loop].rgbBlue;
      (paletteentries+loop)->peFlags = 0;
    }
  }

  // READ THE BITMAP BITS
  if (bitmapbits) {
    SFileSetFilePointer(file,
                        fileheader.bfOffBits
                          -sizeof(BITMAPFILEHEADER)
                          -sizeof(BITMAPINFOHEADER)
                          -(paletteentries ? 256*sizeof(RGBQUAD) : 0),
                        NULL,
                        FILE_CURRENT);
    DWORD bytesread = 0;

    {
      LPBYTE temp = (LPBYTE)ALLOC(infoheader.biSizeImage);
      SFileReadFile(file,temp,infoheader.biSizeImage,&bytesread);
      int destwidth   = (infoheader.biWidth*infoheader.biBitCount) >> 3;
      int sourcewidth = destwidth;
      if (sourcewidth & 3)
        sourcewidth += 4-(sourcewidth & 3);
      FlipImage(bitmapbits,
                temp,
                buffersize,
                destwidth,
                sourcewidth,
                infoheader.biHeight);
      FREE(temp);
    }

    if (bytesread < infoheader.biSizeImage)
      return FALSE;
  }

  return TRUE;
}

//===========================================================================
static BOOL DecodeBmpMem (LPBYTE          imagedata,
                          DWORD           imagebytes,
                          LPPALETTEENTRY  paletteentries,
                          LPBYTE          bitmapbits,
                          DWORD           buffersize,
                          int            *width,
                          int            *height,
                          int            *bitdepth) {

  // PROCESS THE HEADERS
  LPBITMAPFILEHEADER fileheader = (LPBITMAPFILEHEADER)imagedata;
  LPBITMAPINFOHEADER infoheader = (LPBITMAPINFOHEADER)(fileheader+1);
  if (width)
    *width = infoheader->biWidth;
  if (height)
    *height = infoheader->biHeight;
  if (bitdepth)
    *bitdepth = infoheader->biBitCount;

  // PROCESS THE PALETTE ENTRIES
  if (infoheader->biBitCount > 8)
    paletteentries = NULL;
  if (paletteentries) {
    RGBQUAD *palettedata = (RGBQUAD *)(infoheader+1);
    for (int loop = 0; loop < 256; ++loop) {
      (paletteentries+loop)->peRed   = (palettedata+loop)->rgbRed;
      (paletteentries+loop)->peGreen = (palettedata+loop)->rgbGreen;
      (paletteentries+loop)->peBlue  = (palettedata+loop)->rgbBlue;
      (paletteentries+loop)->peFlags = 0;
    }
  }

  // PROCESS THE BITMAP BITS
  if (bitmapbits) {
    DWORD offset      = fileheader->bfOffBits
                          -sizeof(BITMAPFILEHEADER)
                          -sizeof(BITMAPINFOHEADER)
                          -(paletteentries ? 256*sizeof(RGBQUAD) : 0);
    int   destwidth   = (infoheader->biWidth*infoheader->biBitCount) >> 3;
    int   sourcewidth = destwidth;
    if (sourcewidth & 3)
      sourcewidth += 4-(sourcewidth & 3);
    FlipImage(bitmapbits,
              imagedata+offset,
              buffersize,
              destwidth,
              sourcewidth,
              infoheader->biHeight);
  }

  return TRUE;
}

//===========================================================================
static BOOL EncodeBmp256File (HANDLE         file,
                              LPPALETTEENTRY paletteentries,
                              LPBYTE         bitmapbits,
                              int            width,
                              int            height) {

  // WRITE THE FILE HEADER
  {
    BITMAPFILEHEADER fileheader;
    ZeroMemory(&fileheader,sizeof(BITMAPFILEHEADER));
    fileheader.bfType    = BMPSIGNATURE;
    fileheader.bfOffBits = sizeof(BITMAPFILEHEADER)
                             +sizeof(BITMAPINFOHEADER)
                             +256*sizeof(RGBQUAD);
    fileheader.bfSize    = fileheader.bfOffBits+width*height;
    DWORD byteswritten;
    WriteFile(file,&fileheader,sizeof(BITMAPFILEHEADER),&byteswritten,NULL);
  }

  // WRITE THE BITMAP INFO HEADER
  {
    BITMAPINFOHEADER infoheader;
    ZeroMemory(&infoheader,sizeof(BITMAPINFOHEADER));
    infoheader.biSize          = sizeof(BITMAPINFOHEADER);
    infoheader.biWidth         = width;
    infoheader.biHeight        = height;
    infoheader.biPlanes        = 1;
    infoheader.biBitCount      = 8;
    infoheader.biCompression   = BI_RGB;
    infoheader.biSizeImage     = width*height;
    infoheader.biClrUsed       = 256;
    infoheader.biClrImportant  = 256;
    DWORD byteswritten;
    WriteFile(file,&infoheader,sizeof(BITMAPINFOHEADER),&byteswritten,NULL);
  }

  // WRITE THE PALETTE ENTRIES
  {
    RGBQUAD palettedata[256];
    for (int loop = 0; loop < 256; ++loop) {
      palettedata[loop].rgbRed      = (paletteentries+loop)->peRed;
      palettedata[loop].rgbGreen    = (paletteentries+loop)->peGreen;
      palettedata[loop].rgbBlue     = (paletteentries+loop)->peBlue;
      palettedata[loop].rgbReserved = 0;
    }
    DWORD byteswritten;
    WriteFile(file,palettedata,256*sizeof(RGBQUAD),&byteswritten,NULL);
  }

  // WRITE THE BITMAP BITS
  {
    DWORD byteswritten;
    int   destwidth = width;
    if (destwidth & 3)
      destwidth += 4-(destwidth & 3);
    LPBYTE temp = (LPBYTE)ALLOC(destwidth*height);
    FlipImage(temp,
              bitmapbits,
              destwidth*height,
              destwidth,
              width,
              height);
    WriteFile(file,temp,destwidth*height,&byteswritten,NULL);
    FREE(temp);
  }

  return TRUE;
}

//===========================================================================
static void FlipImage (LPBYTE dest,
                       LPBYTE source,
                       DWORD  destbytes,
                       int    destwidth,
                       int    sourcewidth,
                       int    height) {
  VALIDATEBEGIN;
  VALIDATE(dest);
  VALIDATE(source);
  VALIDATE(destbytes);
  VALIDATE(destwidth);
  VALIDATE(sourcewidth);
  VALIDATE(height);
  VALIDATEENDVOID;

  source += (height-1)*sourcewidth;

  LPBYTE destterm = dest+destbytes;
  while (height--) {
    DWORD bytestocopy = min(destwidth,destterm-dest);
    if (!bytestocopy)
      break;
    CopyMemory(dest,source,bytestocopy);
    dest   += bytestocopy;
    source -= sourcewidth;
  }

}


/****************************************************************************
*
*  GIF ENCODER/DECODER
*
***/

typedef struct _GIFCOMPRESSREC {
    int  bits;
    int  max_code;
    int  init_bits;
    int  cur_accum;
    int  cur_bits;

    int  waiting_code;
    BOOL first_byte;

    int  clearcode;
    int  EOFcode;
    int  freecode;

    int *hash_code;
    int *hash_value;

    int  bytesinpkt;
    BYTE packetbuf[256];
} GIFCOMPRESSREC;

#pragma pack(1)
typedef struct _GIFHEADERREC {
    char signature[6];
    WORD width;
    WORD height;
    BYTE flags;
    BYTE bgidx;
    BYTE reserved;
} GIFHEADERREC;
#pragma pack()

typedef struct _GIFIMAGEDESCREC {
    WORD x;
    WORD y;
    WORD width;
    WORD height;
    BYTE flags;
    BYTE codesize;
} GIFIMAGEDESCREC;

typedef struct _GIFPALREC {
    struct {
        BYTE red;
        BYTE green;
        BYTE blue;
    } idx[256];
} GIFPALREC;

typedef struct _GIFMARKERREC {
    char ch;
} GIFMARKERREC;

#define HASH_ENTRY(prefix,suffix)  ((((int) (prefix)) << 8) | (suffix))
#define	MAX_LZW_BITS	12	/* maximum LZW code size (4096 symbols) */
#define LZW_TABLE_SIZE	((int) 1 << MAX_LZW_BITS)
#define HSIZE		5003	/* hash table size for 80% occupancy */
#define MAXCODE(bits)	(((int) 1 << (bits)) - 1)

static inline void Output (GIFCOMPRESSREC *compress,
                           LPVOID         *dest,
                           int             code);

//===========================================================================
static void ClearBlock (GIFCOMPRESSREC *compress,
                        LPVOID         *dest) {
  ZeroMemory(compress->hash_code,HSIZE*sizeof(int));
  compress->freecode = compress->clearcode + 2;
  Output(compress,
         dest,
         compress->clearcode);
  compress->bits = compress->init_bits;	/* reset code size */
  compress->max_code = MAXCODE(compress->bits);
}

//===========================================================================
static void FlushPacket (GIFCOMPRESSREC *compress,
                         LPVOID         *dest) {
  if (!compress->bytesinpkt)
    return;
  compress->packetbuf[0] = (BYTE)(compress->bytesinpkt++);
  CopyMemory(*dest,compress->packetbuf,compress->bytesinpkt);
  *dest = (LPBYTE)*dest + compress->bytesinpkt;
  compress->bytesinpkt = 0;
}

//===========================================================================
static inline void Output (GIFCOMPRESSREC *compress,
                           LPVOID         *dest,
                           int             code) {
  compress->cur_accum |= code << compress->cur_bits;
  compress->cur_bits  += compress->bits;

  while (compress->cur_bits >= 8) {
    compress->packetbuf[++compress->bytesinpkt] =
      (BYTE)(compress->cur_accum & 0xFF);
    if (compress->bytesinpkt >= 255)
      FlushPacket(compress,
                  dest);
    compress->cur_accum >>= 8;
    compress->cur_bits  -= 8;
  }

  /*
   * If the next entry is going to be too big for the code size,
   * then increase it, if possible.  We do this here to ensure
   * that it's done in sync with the decoder's codesize increases.
   */
  if (compress->freecode > compress->max_code) {
    compress->bits++;
    if (compress->bits == MAX_LZW_BITS)
      compress->max_code = LZW_TABLE_SIZE; /* freecode will never exceed this */
    else
      compress->max_code = MAXCODE(compress->bits);
  }
}

//===========================================================================
static inline void CompressGifByte (GIFCOMPRESSREC *compress,
                                    LPVOID         *dest,
                                    int             val) {
  register int i;
  register int disp;
  register int probe_value;

  if (compress->first_byte) {	/* need to initialize waiting_code */
    compress->waiting_code = val;
    compress->first_byte = FALSE;
    return;
  }

  /* Probe hash table to see if a symbol exists for
   * waiting_code followed by val.
   * If so, replace waiting_code by that symbol and return.
   */
  i = ((int) val << (MAX_LZW_BITS-8)) + compress->waiting_code;
  /* i is less than twice 2**MAX_LZW_BITS, therefore less than twice HSIZE */
  if (i >= HSIZE)
    i -= HSIZE;

  probe_value = HASH_ENTRY(compress->waiting_code, val);
  
  if (compress->hash_code[i] != 0) { /* is first probed slot empty? */
    if (compress->hash_value[i] == probe_value) {
      compress->waiting_code = compress->hash_code[i];
      return;
    }
    if (i == 0)			/* secondary hash (after G. Knott) */
      disp = 1;
    else
      disp = HSIZE - i;
    for (;;) {
      i -= disp;
      if (i < 0)
	i += HSIZE;
      if (compress->hash_code[i] == 0)
	break;			/* hit empty slot */
      if (compress->hash_value[i] == probe_value) {
	compress->waiting_code = compress->hash_code[i];
	return;
      }
    }
  }

  /* here when hashtable[i] is an empty slot; desired symbol not in table */
  Output(compress,
         dest,
         compress->waiting_code);
  if (compress->freecode < LZW_TABLE_SIZE) {
    compress->hash_code[i] = compress->freecode++; /* add symbol to hashtable */
    compress->hash_value[i] = probe_value;
  } else
    ClearBlock(compress,
               dest);
  compress->waiting_code = val;
}

//===========================================================================
static DWORD CompressGifImage (LPVOID  dest,
                               LPCVOID source,
                               DWORD   sourcebytes) {
  LPVOID basedest = dest;

  // INITIALIZE COMPRESSION
  GIFCOMPRESSREC compress;
  ZeroMemory(&compress,sizeof(GIFCOMPRESSREC));
  compress.bits       = 9;
  compress.init_bits  = 9;
  compress.max_code   = MAXCODE(compress.bits);
  compress.clearcode  = 1 << (compress.bits-1);
  compress.EOFcode    = compress.clearcode+1;
  compress.freecode   = compress.clearcode+2;
  compress.first_byte = TRUE;
  compress.hash_code  = (int *)ALLOCZERO(HSIZE*sizeof(int));
  compress.hash_value = (int *)ALLOCZERO(HSIZE*sizeof(int));
  Output(&compress,
         &dest,
         compress.clearcode);

  // COMPRESS THE IMAGE
  while (sourcebytes--) {
    CompressGifByte(&compress,
                    &dest,
                    *(LPBYTE)source);
    source = (LPBYTE)source+1;
  }

  // CLEANUP
  if (!compress.first_byte)
    Output(&compress,
           &dest,
           compress.waiting_code);
  Output(&compress,
         &dest,
         compress.EOFcode);
  if (compress.cur_bits > 0)
    compress.packetbuf[++compress.bytesinpkt] =
      (BYTE)(compress.cur_accum & 0xFF);
  FlushPacket(&compress,
              &dest);

  FREE(compress.hash_code);
  FREE(compress.hash_value);

  return (LPBYTE)dest-(LPBYTE)basedest;
}

//===========================================================================
static BOOL EncodeGif256File (HANDLE         file,
                              LPPALETTEENTRY paletteentries,
                              LPBYTE         bitmapbits,
                              int            width,
                              int            height) {

  // WRITE THE HEADER
  {
    GIFHEADERREC rec;
    rec.signature[0] = 'G';
    rec.signature[1] = 'I';
    rec.signature[2] = 'F';
    rec.signature[3] = '8';
    rec.signature[4] = '7';
    rec.signature[5] = 'a';
    rec.width        = (WORD)width;
    rec.height       = (WORD)height;
    rec.flags        = 0x80       // global color table
                         | 0x70   // 256 color image
                         | 0x07;  // 256 colors in global color table;
    rec.bgidx        = 0;
    rec.reserved     = 0;
    DWORD byteswritten;
    WriteFile(file,&rec,sizeof(GIFHEADERREC),&byteswritten,NULL);
  }

  // WRITE THE COLOR TABLE
  {
    GIFPALREC pal;
    for (int loop = 0; loop < 256; ++loop) {
      pal.idx[loop].red   = (paletteentries+loop)->peRed;
      pal.idx[loop].green = (paletteentries+loop)->peGreen;
      pal.idx[loop].blue  = (paletteentries+loop)->peBlue;
    }
    DWORD byteswritten;
    WriteFile(file,&pal,sizeof(GIFPALREC),&byteswritten,NULL);
  }

  // WRITE THE SEPARATOR
  {
    GIFMARKERREC rec;
    rec.ch = ',';
    DWORD byteswritten;
    WriteFile(file,&rec,sizeof(GIFMARKERREC),&byteswritten,NULL);
  }

  // WRITE THE IMAGE DESCRIPTOR
  {
    GIFIMAGEDESCREC rec;
    rec.x        = 0;
    rec.y        = 0;
    rec.width    = (WORD)width;
    rec.height   = (WORD)height;
    rec.flags    = 0;
    rec.codesize = 8;
    DWORD byteswritten;
    WriteFile(file,&rec,sizeof(GIFIMAGEDESCREC),&byteswritten,NULL);
  }

  // WRITE THE BITMAP BITS
  {
    LPBYTE compressed = (LPBYTE)ALLOC(width*height*2);
    DWORD  bytes      = CompressGifImage(compressed,
                                         bitmapbits,
                                         width*height);
    DWORD byteswritten;
    WriteFile(file,compressed,bytes,&byteswritten,NULL);
    FREE(compressed);
  }

  // WRITE THE TERMINATOR
  {
    GIFMARKERREC rec;
    rec.ch = 0;
    DWORD byteswritten;
    WriteFile(file,&rec,sizeof(GIFMARKERREC),&byteswritten,NULL);
    rec.ch = ';';
    WriteFile(file,&rec,sizeof(GIFMARKERREC),&byteswritten,NULL);
  }

  return TRUE;
}


/****************************************************************************
*
*  PCX ENCODER/DECODER
*
***/

typedef struct _PCXHEADERREC {
    WORD signature;
    BYTE encoding;     
    BYTE bitsperpixel; 
    WORD x1;
    WORD y1;
    WORD x2;
    WORD y2;
    WORD screenwidth;
    WORD screenheight;
} PCXHEADERREC;

typedef struct _PCXINFOREC {
    BYTE mode;
    BYTE planes;
    WORD bytesperline;
    BYTE unused[60];
} PCXINFOREC;

typedef struct _PCXRGBREC {
    BYTE red;
    BYTE green;
    BYTE blue;
} PCXRGBREC;

typedef struct _PCXFILEREC {
    PCXHEADERREC header;
    PCXRGBREC    pal16[16];
    PCXINFOREC   info;
} PCXFILEREC;

typedef struct _PCXEXTPALREC {
    BYTE      number;
    PCXRGBREC pal256[256];
} PCXEXTPALREC;

static void UncompressPcxImage (LPBYTE dest,
                                LPBYTE source,
                                DWORD  destbytes,
                                DWORD  sourcebytes);

//===========================================================================
static void CompressPcxRow (LPBYTE *dest, LPBYTE *source, int width) {
  do {
    BYTE ch    = *(*source)++;
    int  count = 1;
    --width;
    while (width && (ch == **source) && (count < 63)) {
      ++count;
      --width;
      ++*source;
    }
    if ((count > 1) || (ch >= 0xC0)) {
      count |= 0xC0;
      *(*dest)++ = (BYTE)(count & 0xFF);
    }
    *(*dest)++ = ch;
  } while (width);
}

//===========================================================================
static BOOL DecodePcxFile (HSFILE          file,
                           LPPALETTEENTRY  paletteentries,
                           LPBYTE          bitmapbits,
                           DWORD           buffersize,
                           int            *width,
                           int            *height,
                           int            *bitdepth) {

  // READ THE HEADER
  PCXFILEREC rec;
  if (!SFileReadFile(file,&rec,sizeof(PCXFILEREC)))
    return 0;
  int imagewidth    = rec.header.x2+1-rec.header.x1;
  int imageheight   = rec.header.y2+1-rec.header.y1;
  int imagebitdepth = rec.header.bitsperpixel;
  if (width)
    *width  = imagewidth;
  if (height)
    *height = imageheight;
  if (bitdepth)
    *bitdepth = imagebitdepth;

  // READ THE BITMAP BITS
  if (bitmapbits) {
    DWORD  bytestoread = SFileGetFileSize(file)-SFileSetFilePointer(file,0,NULL,FILE_CURRENT);
    LPBYTE compressed  = (LPBYTE)ALLOC(bytestoread);
    SFileReadFile(file,compressed,bytestoread);
    UncompressPcxImage(bitmapbits,
                       compressed,
                       buffersize,
                       bytestoread);
    FREE(compressed);
  }
  else
    SFileSetFilePointer(file,0,NULL,FILE_END);

  // READ THE PALETTE ENTRIES
  if (paletteentries && (imagebitdepth == 8)) {
    PCXRGBREC paldata[256];
    SFileSetFilePointer(file,-256*(int)sizeof(PCXRGBREC),NULL,FILE_CURRENT);
    SFileReadFile(file,paldata,256*sizeof(PCXRGBREC));

    // CONVERT FROM PCXRGBREC TO PALETTEENTRY FORMAT
    for (int loop = 0; loop < 256; ++loop) {
      (paletteentries+loop)->peRed   = paldata[loop].red;
      (paletteentries+loop)->peGreen = paldata[loop].green;
      (paletteentries+loop)->peBlue  = paldata[loop].blue;
      (paletteentries+loop)->peFlags = 0;
    }
  }

  return TRUE;
}

//===========================================================================
static BOOL DecodePcxMem (LPBYTE          imagedata,
                          DWORD           imagebytes,
                          LPPALETTEENTRY  paletteentries,
                          LPBYTE          bitmapbits,
                          DWORD           buffersize,
                          int            *width,
                          int            *height,
                          int            *bitdepth) {

  // PROCESS THE HEADER
  PCXFILEREC *rec = (PCXFILEREC *)imagedata;
  int imagewidth    = rec->header.x2+1-rec->header.x1;
  int imageheight   = rec->header.y2+1-rec->header.y1;
  int imagebitdepth = rec->header.bitsperpixel;
  if (width)
    *width  = imagewidth;
  if (height)
    *height = imageheight;
  if (bitdepth)
    *bitdepth = imagebitdepth;

  // PROCESS THE BITMAP BITS
  if (bitmapbits)
    UncompressPcxImage(bitmapbits,
                       (LPBYTE)(rec+1),
                       min(buffersize,(DWORD)(imagewidth*imageheight)),
                       imagebytes-sizeof(PCXFILEREC));

  // PROCESS THE PALETTE ENTRIES
  if (paletteentries && (imagebitdepth == 8)) {
    PCXRGBREC *paldata = (PCXRGBREC *)(imagedata+imagebytes-256*sizeof(PCXRGBREC));
    for (int loop = 0; loop < 256; ++loop) {
      (paletteentries+loop)->peRed   = (paldata+loop)->red;
      (paletteentries+loop)->peGreen = (paldata+loop)->green;
      (paletteentries+loop)->peBlue  = (paldata+loop)->blue;
      (paletteentries+loop)->peFlags = 0;
    }
  }

  return TRUE;
}

//===========================================================================
static BOOL EncodePcx256File (HANDLE         file,
                              LPPALETTEENTRY paletteentries,
                              LPBYTE         bitmapbits,
                              int            width,
                              int            height) {

  // WRITE THE HEADER
  {
    PCXFILEREC rec;
    ZeroMemory(&rec,sizeof(PCXFILEREC));
    rec.header.signature    = PCXSIGNATURE;
    rec.header.encoding     = 1;
    rec.header.bitsperpixel = 8;
    rec.header.x2           = width-1;
    rec.header.y2           = height-1;
    rec.header.screenwidth  = width;
    rec.header.screenheight = height;
    rec.info.planes         = 1;
    rec.info.bytesperline   = width;
    DWORD byteswritten;
    WriteFile(file,&rec,sizeof(PCXFILEREC),&byteswritten,NULL);
  }

  // WRITE THE BITMAP BITS
  {
    LPBYTE compressed = (LPBYTE)ALLOC(width*height*2);
    LPBYTE source     = bitmapbits;
    LPBYTE dest       = compressed;
    for (int loop = 0; loop < height; ++loop)
      CompressPcxRow(&dest,&source,width);
    DWORD byteswritten;
    WriteFile(file,compressed,dest-compressed,&byteswritten,NULL);
    FREE(compressed);
  }

  // WRITE THE PALETTE ENTRIES
  {
    PCXEXTPALREC pal;
    pal.number = 12;
    for (int loop = 0; loop < 256; ++loop) {
      pal.pal256[loop].red   = (paletteentries+loop)->peRed;
      pal.pal256[loop].green = (paletteentries+loop)->peGreen;
      pal.pal256[loop].blue  = (paletteentries+loop)->peBlue;
    }
    DWORD byteswritten;
    WriteFile(file,&pal,sizeof(PCXEXTPALREC),&byteswritten,NULL);
  }

  return TRUE;
}

//===========================================================================
static void UncompressPcxImage (LPBYTE dest,
                                LPBYTE source,
                                DWORD  destbytes,
                                DWORD  sourcebytes) {
  LPBYTE destterm   = dest+destbytes;
  LPBYTE sourceterm = source+sourcebytes;
  while ((dest < destterm) && (source < sourceterm)) {
    BYTE val = *source++;
    if (val >= 0xC0) {
      DWORD count = val & 0x3F;
      count = min(count,(DWORD)(destterm-dest));
      val = *source++;
      FillMemory(dest,count,val);
      dest += count;
    }
    else
      *dest++ = val;
  }
}


/****************************************************************************
*
*  SUPPORT FUNCTIONS
*
***/

//===========================================================================
static LPCTSTR FindExtension (LPCTSTR filename) {
  LPCTSTR result = filename;
  while (SStrChr(result,'\\'))
    result = SStrChr(result,'\\')+1;
  while (SStrChr(result+1,'.'))
    result = SStrChr(result+1,'.');
  return result;
}

//===========================================================================
static DWORD DetermineImageType (WORD signature) {
  switch (signature) {

    case BMPSIGNATURE:
    return SBMP_IMAGETYPE_BMP;

    case PCXSIGNATURE:
    return SBMP_IMAGETYPE_PCX;

    default:
    return 0;

  }
}


/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
BOOL APIENTRY SBmpAllocLoadImage (LPCTSTR         filename,
                                  LPPALETTEENTRY  paletteentries,
                                  LPBYTE         *returnedbuffer,
                                  int            *width,
                                  int            *height,
                                  int            *bitdepth,
                                  int             requestedbitdepth,
                                  SBMPALLOCPROC   allocproc) {
  if (returnedbuffer)
    *returnedbuffer = NULL;
  if (width)
    *width = 0;
  if (height)
    *height = 0;
  if (bitdepth)
    *bitdepth = 0;

  VALIDATEBEGIN;
  VALIDATE(filename);
  VALIDATE(*filename);
  VALIDATE(returnedbuffer);
  VALIDATEEND;

  // OPEN THE FILE
  HSFILE file;
  if (!SFileOpenFile(filename,&file))
    return FALSE;

  // LOAD THE FILE INTO MEMORY
  DWORD  filesize = SFileGetFileSize(file);
  LPBYTE filedata = (LPBYTE)ALLOC(filesize);
  SFileReadFile(file,
                filedata,
                filesize);
  SFileCloseFile(file);

  BOOL success = FALSE;
  TRY {

    // DETERMINE THE SOURCE IMAGE DIMENSIONS
    int localwidth, localheight, localbitdepth;
    if (!SBmpDecodeImage(SBMP_IMAGETYPE_AUTO,
                         filedata,
                         filesize,
                         NULL,
                         NULL,
                         0,
                         &localwidth,
                         &localheight,
                         &localbitdepth))
      LEAVE;

    // IF THE REQUESTED BIT DEPTH DOES NOT MATCH THE ACTUAL BIT DEPTH,
    // RETURN AN ERROR, SINCE WE DO NOT CURRENTLY IMPLEMENT IMAGE FORMAT
    // CONVERSION
    if (!requestedbitdepth)
      requestedbitdepth = localbitdepth;
    if (requestedbitdepth != localbitdepth)
      LEAVE;

    // ALLOCATE A BUFFER TO HOLD THE IMAGE
    DWORD imagesize = localwidth*localheight*requestedbitdepth/8;
    if (allocproc)
      *returnedbuffer = (LPBYTE)allocproc(imagesize);
    else
      *returnedbuffer = (LPBYTE)SMemAlloc(imagesize,
                                          filename,
                                          SERR_LINECODE_FILE,
                                          0);
    if (!*returnedbuffer)
      LEAVE;

    // DECODE THE IMAGE INTO THE BUFFER
    SBmpDecodeImage(SBMP_IMAGETYPE_AUTO,
                    filedata,
                    filesize,
                    paletteentries,
                    *returnedbuffer,
                    imagesize);

    // RETURN THE WIDTH, HEIGHT, AND BITDEPTH
    if (width)
      *width = localwidth;
    if (height)
      *height = localheight;
    if (bitdepth)
      *bitdepth = requestedbitdepth;

    success = TRUE;
  }
  FINALLY {
    FREE(filedata);
  }

  return success;
}

//===========================================================================
BOOL APIENTRY SBmpDecodeImage (DWORD           imagetype,
                               LPBYTE          imagedata,
                               DWORD           imagebytes,
                               LPPALETTEENTRY  paletteentries,
                               LPBYTE          bitmapbits,
                               DWORD           buffersize,
                               int            *width,
                               int            *height,
                               int            *bitdepth) {
  if (width)
    *width = 0;
  if (height)
    *height = 0;
  if (bitdepth)
    *bitdepth = 0;

  VALIDATEBEGIN;
  VALIDATE(imagedata);
  VALIDATE(imagebytes);
  VALIDATE(buffersize || !bitmapbits);
  VALIDATEEND;

  // IF NO IMAGE TYPE WAS GIVEN, TRY TO DETERMINE IT FROM THE SIGNATURE
  if ((imagetype == SBMP_IMAGETYPE_AUTO) &&
      (imagebytes >= sizeof(WORD)))
    imagetype = DetermineImageType(*(LPWORD)imagedata);

  // DECODE THE IMAGE
  switch (imagetype) {

    case SBMP_IMAGETYPE_BMP:
    return DecodeBmpMem(imagedata,
                        imagebytes,
                        paletteentries,
                        bitmapbits,
                        buffersize,
                        width,
                        height,
                        bitdepth);

    case SBMP_IMAGETYPE_PCX:
    return DecodePcxMem(imagedata,
                        imagebytes,
                        paletteentries,
                        bitmapbits,
                        buffersize,
                        width,
                        height,
                        bitdepth);

    default:
    return FALSE;

  }
}

//===========================================================================
BOOL APIENTRY SBmpLoadImage (LPCTSTR         filename,
                             LPPALETTEENTRY  paletteentries,
                             LPBYTE          bitmapbits,
                             DWORD           buffersize,
                             int            *width,
                             int            *height,
                             int            *bitdepth) {
  if (width)
    *width = 0;
  if (height)
    *height = 0;
  if (bitdepth)
    *bitdepth = 0;

  VALIDATEBEGIN;
  VALIDATE(filename);
  VALIDATE(*filename);
  VALIDATE(buffersize || !bitmapbits);
  VALIDATEEND;

  // OPEN THE FILE
  HSFILE file;
  if (!SFileOpenFile(filename,&file))
    return FALSE;

  // DETERMINE THE IMAGE TYPE
  DWORD imagetype = 0;
  {
    WORD signature;
    if (SFileReadFile(file,&signature,sizeof(WORD))) {
      SFileSetFilePointer(file,-2,NULL,FILE_CURRENT);
      imagetype = DetermineImageType(signature);
    }
  }

  // READ AND DECODE THE IMAGE
  BOOL result = FALSE;
  switch (imagetype) {

    case SBMP_IMAGETYPE_BMP:
      result = DecodeBmpFile(file,
                             paletteentries,
                             bitmapbits,
                             buffersize,
                             width,
                             height,
                             bitdepth);
    break;

    case SBMP_IMAGETYPE_PCX:
      result = DecodePcxFile(file,
                             paletteentries,
                             bitmapbits,
                             buffersize,
                             width,
                             height,
                             bitdepth);
    break;

  }

  // CLOSE THE FILE
  SFileCloseFile(file);

  return result;
}

//===========================================================================
BOOL APIENTRY SBmpSaveImage (LPCTSTR         filename,
                             LPPALETTEENTRY  paletteentries,
                             LPBYTE          bitmapbits,
                             int             width,
                             int             height,
                             int             bitdepth) {
  VALIDATEBEGIN;
  VALIDATE(filename);
  VALIDATE(*filename);
  VALIDATE(paletteentries);
  VALIDATE(bitmapbits);
  VALIDATE(width > 0);
  VALIDATE(height > 0);
  VALIDATEEND;

  // STORM CURRENTLY ONLY SUPPORTS 256-COLOR MODE, SO FOR THE TIME BEING,
  // REJECT ALL BIT DEPTHS EXCEPT 8BPP
  if (bitdepth != 8)
    return FALSE;

  // OPEN THE FILE
  HANDLE file = CreateFile(filename,
                           GENERIC_WRITE,
                           0,
                           (LPSECURITY_ATTRIBUTES)NULL,
                           CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL);
  if (file == INVALID_HANDLE_VALUE)
    return FALSE;

  // WRITE THE IMAGE
  LPCTSTR ext    = FindExtension(filename);
  BOOL    result = 0;
  if (ext && !_stricmp(ext,".pcx"))
    result = EncodePcx256File(file,
                              paletteentries,
                              bitmapbits,
                              width,
                              height);
  else if (ext && !_stricmp(ext,".gif"))
    result = EncodeGif256File(file,
                              paletteentries,
                              bitmapbits,
                              width,
                              height);
  else
    result = EncodeBmp256File(file,
                              paletteentries,
                              bitmapbits,
                              width,
                              height);

  // CLOSE THE FILE
  CloseHandle(file);

  return result;
}

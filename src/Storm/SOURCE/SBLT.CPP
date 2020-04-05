/****************************************************************************
*
*  SBLT.CPP
*  Storm bitblt engine
*
*  By Michael O'Brien (3/13/96)
*
***/

#include "pch.h"
#pragma  hdrstop

#define  OPT_SRCCOPY  0
#define  OPT_PATCOPY  1
#define  OPTSTREAMS   2

#define  UNROLL       180

typedef struct _OPTSTREAMDATA {
    HSCODESTREAM  stream;
    LPBYTE       *jumptable;
    LPDWORD       patchlocation;
} OPTSTREAMDATA, *OPTSTREAMDATAPTR;

typedef struct _BLTINFO {
    BOOL         used;
    HSCODESTREAM stream;
    DWORD        time;
    DWORD        reserved;
} BLTINFO, *BLTINFOPTR;

static OPTSTREAMDATA s_optstream[OPTSTREAMS] = {{0},{0}};
static BLTINFOPTR    s_scode                 = NULL;

//===========================================================================
static BOOL InitOptStream (LPCSTR           sourcestring,
                           OPTSTREAMDATAPTR optdata) {
  if (!SCodeCompile(NULL,sourcestring,NULL,UNROLL,0,&optdata->stream))
    return FALSE;
  if (!SCodeGetJumpTable(optdata->stream,
                         &optdata->jumptable,
                         NULL,
                         &optdata->patchlocation,
                         NULL))
    return FALSE;
  return TRUE;
}

//===========================================================================
static void inline OptimizeROP3 (DWORD *rop, DWORD *pattern) {
  switch (*rop) {

    case BLACKNESS:
      *rop     = PATCOPY;
      *pattern = 0;
      break;

    case WHITENESS:
      *rop     = PATCOPY;
      *pattern = 0xFFFFFFFF;
      break;

    case 0x000F0001:
      *rop     = PATCOPY;
      *pattern = !*pattern;
      break;
    
  }
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
BOOL APIENTRY SBltDestroy () {
  if (s_scode) {
    int loop;
    for (loop = 0; loop < OPTSTREAMS; ++loop) {
      SCodeDelete(s_optstream[loop].stream);
      ZeroMemory(&s_optstream[loop],sizeof(OPTSTREAMDATA));
    }
    for (loop = 0; loop < 256; ++loop)
      if ((s_scode+loop)->stream)
        SCodeDelete((s_scode+loop)->stream);
    FREE(s_scode);
    s_scode = NULL;
  }
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SBltGetSCode (DWORD rop3,
                            LPSTR buffer,
                            DWORD buffersize,
                            BOOL  optimize) {
  VALIDATEBEGIN;
  VALIDATE(rop3);
  VALIDATE(buffer);
  VALIDATE(buffersize);
  VALIDATEEND;

  // OPTIMIZE THE RASTER OPERATION CODE IF REQUESTED
  if (optimize) {
    DWORD pattern = 0;
    OptimizeROP3(&rop3,&pattern);
  }

  // DEFINE THE SOURCE AND OPERATION TEMPLATE STRINGS
  static const char optemplate[]          = "~^|&";
  static const char sourcetemplate[8][16] = {"SCDDDDDD",
                                             "SCDSCDSC",
                                             "SDCSDCSD",
                                             "DDDDDDDD",
                                             "DDDDDDDD",
                                             "SASCADSS",
                                             "SASCACDS",
                                             "SASDACDS"};

  // CREATE THE SOURCE STRING IN RPN ORDER
  char sourcestring[16];
  SStrCopy(sourcestring,sourcetemplate[(rop3 >> 2) & 7]+(rop3 & 3),16);

  // CREATE THE OPERATION STRING
  char opstring[16];
  {
    DWORD currrop = rop3 >> 6;
    for (int loop = 0; loop < 5; ++loop) {
      opstring[loop] = optemplate[currrop & 3];
      currrop >>= 2;
    }
    opstring[loop] = 0;
    if (rop3 & 32)
      SStrPack(opstring,"~",16);
  }

  // ELIMINATE REDUNDANT NOT OPERATIONS
  while ((SStrLen(opstring) >= 2) &&
         (!strcmp(opstring+SStrLen(opstring)-2,"~~")))
    opstring[SStrLen(opstring)-2] = 0;

  // COUNT BINARY OPERATIONS
  int binops = 0;
  {
    LPCSTR curr = opstring;
    while (*curr) {
      if (*curr != '~')
        ++binops;
      ++curr;
    }
  }

  // COMPUTE THE NUMBER OF SOURCES THAT WILL BE USED BY THE BINARY OPERATIONS
  int sources = binops+1;
  {
    int loop = 0;
    while (loop < sources)
      if (sourcestring[loop++] == 'A')
        ++sources;
  }

  // COMPUTE THE SCODE STRING
  char  scode[32] = "# D=";
  DWORD scodelen  = 0;
  {
    char *curr     = scode+SStrLen(scode);
    char *currop   = opstring;
    BOOL  savedval = 0;
    *curr++ = *(sourcestring+(--sources));
    while (*currop)
      if (*currop == '~') {
        *curr++ = '^';
        *curr++ = '1';
        ++currop;
      }
      else if ((*(sourcestring+sources-1) == 'A') && !savedval) {
        --sources;
        scode[2] = 'A';
        *curr++ = ' ';
        *curr++ = 'D';
        *curr++ = '=';
        *curr++ = *(sourcestring+(--sources));
        savedval = 1;
      }
      else {
        *curr++ = *currop++;
        *curr++ = *(sourcestring+(--sources));
      }
    *curr++ = 0;
    scodelen = curr-scode;
  }

  // COPY THE SCODE STRING INTO THE RESULT BUFFER
  SStrCopy(buffer,scode,buffersize);

  return (scodelen <= buffersize);
}

//===========================================================================
BOOL APIENTRY SBltROP3 (LPBYTE dest,
                        LPBYTE source,
                        int    width,
                        int    height,
                        int    destcx,
                        int    sourcecx,
                        DWORD  pattern,
                        DWORD  rop3) {
  // to minimize function call overhead for this time critical function,
  // we use assert instead of validate so that the retail version has
  // no parameter checking code
  ASSERT(dest);
  ASSERT(destcx >= width);

//__asm int		0x3;

  // IGNORE ZERO-SIZED BLTS
  if ((width <= 0) || (height <= 0))
    return TRUE;

  // INITIALIZE THIS MODULE IF NECESSARY
  if (!s_scode) {
    s_scode = (BLTINFOPTR)ALLOCZERO(256*sizeof(BLTINFO));
    if (!(InitOptStream("4 D=S",&s_optstream[OPT_SRCCOPY]) &&
          InitOptStream("4 D=A",&s_optstream[OPT_PATCOPY]))) {
      SCodeDestroy();
      return FALSE;
    }
  }

  // FOR ALIGNED SRCCOPY AND PATCOPY OPERATIONS ON X86 SYSTEMS ONLY,
  // PERFORM THE OPERATION DIRECTLY WITHOUT CALLING SCODEEXECUTE() AND
  // WITHOUT WORRYING ABOUT BYTE OPERATIONS.  THIS IS AN IMPORTANT
  // OPTIMIZATION BECAUSE THESE BLT TYPES ARE VERY COMMON AND TEND TO BE
  // THE MOST TIME CRITICAL BLTS IN A GAME.

#ifdef _X86_
  if (((rop3 == SRCCOPY) || (rop3 == PATCOPY)) && !(((DWORD)dest | width) & 3)) {
    DWORD adjustdest   = destcx-width;
    DWORD adjustsource = sourcecx-width;
    LPBYTE retptr;
    __asm mov retptr,OFFSET opt_loop
    OPTSTREAMDATAPTR optdata = &s_optstream[rop3 == PATCOPY];
    LPBYTE           jumpptr = *(optdata->jumptable+(width >> 2));
    *optdata->patchlocation = retptr-(LPBYTE)(optdata->patchlocation+1);
    __asm {
                   push      edi
                   push      esi

					//int		0x3
					// mov		eax,eax

                   // FILL IN THE REGISTERS
                   mov       edi,dest
                   mov       esi,source
                   mov       ebx,jumpptr
                   mov       ecx,pattern
                   mov       edx,height

                   // EXECUTE THE FIRST SCAN LINE
                   jmp       ebx

                   // EXECUTE SUBSEQUENT SCAN LINES
                   align     16
      opt_loop:    dec       edx
                   jz        opt_done
                   add       edi,adjustdest
                   add       esi,adjustsource
                   jmp       ebx

      opt_done:    pop       esi
                   pop       edi
    }

    return TRUE;
  }
#endif

  // CONVERT SELECTED RASTER OPERATIONS TO MORE EFFICIENT EQUIVALENTS
  if ((rop3 != SRCCOPY) && (rop3 != PATCOPY))
    OptimizeROP3(&rop3,&pattern);

  // FIND OR CREATE TWO SCODE STREAMS TO CARRY OUT THIS RASTER OPERATION
  DWORD index = (rop3 >> 16) & 0xFF;
  if (!(s_scode+index)->used) {
    DWORD currtime = GetTickCount();
// note: write this
//    FreeOldCodeStreams(currtime);
    char scodestring[64] = "";
    SBltGetSCode(rop3,scodestring,64,0);
    SCodeCompile(scodestring,
                 scodestring,
                 NULL,
                 UNROLL,
                 SCODE_CF_AUTOALIGNDWORD,
                 &(s_scode+index)->stream);
    if ((s_scode+index)->stream) {
      (s_scode+index)->used = 1;
      (s_scode+index)->time = currtime;
    }
    else
      return FALSE;
  }

  // PERFORM THE BLT USING SCODE
  SCODEEXECUTEDATA executedata;
  executedata.size         = sizeof(SCODEEXECUTEDATA);
  executedata.flags        = 0;
  executedata.xiterations  = width;
  executedata.yiterations  = height;
  executedata.dest         = dest;
  executedata.source       = source;
  executedata.adjustdest   = destcx-width;
  executedata.adjustsource = sourcecx-width;
  executedata.c            = pattern;
  return SCodeExecute((s_scode+index)->stream,&executedata);
}

//===========================================================================
BOOL APIENTRY SBltROP3Clipped (LPBYTE dest,
                               LPRECT destrect,
                               LPSIZE destsize,
                               int    destpitch,
                               LPBYTE source,
                               LPRECT sourcerect,
                               LPSIZE sourcesize,
                               int    sourcepitch,
                               DWORD  pattern,
                               DWORD  rop3) {
  // to minimize function call overhead for this time critical function,
  // we use assert instead of validate so that the retail version has
  // no parameter checking code
  ASSERT(dest);
  ASSERT(destpitch > 0);

  int destx        = 0;
  int desty        = 0;
  int destwidth    = INT_MAX;
  int destheight   = INT_MAX;
  int sourcex      = 0;
  int sourcey      = 0;
  int sourcewidth  = INT_MAX;
  int sourceheight = INT_MAX;
  if (destrect) {
    destx        = max(0,destrect->left);
    desty        = max(0,destrect->top);
    destwidth    = destrect->right-destrect->left;
    destheight   = destrect->bottom-destrect->top;
  }
  if (destsize) {
    destwidth    = min(destwidth ,max(0,destsize->cx-destx));
    destheight   = min(destheight,max(0,destsize->cy-desty));
  }
  if (sourcerect) {
    sourcex      = max(0,sourcerect->left);
    sourcey      = max(0,sourcerect->top);
    sourcewidth  = sourcerect->right-sourcerect->left;
    sourceheight = sourcerect->bottom-sourcerect->top;
  }
  if (sourcesize) {
    sourcewidth  = min(sourcewidth ,max(0,sourcesize->cx-sourcex));
    sourceheight = min(sourceheight,max(0,sourcesize->cy-sourcey));
  }
  destwidth  = min(destwidth ,sourcewidth );
  destheight = min(destheight,sourceheight);
  if ((destwidth < 1) || (destheight < 1))
    return TRUE;
  else
    return SBltROP3(dest+desty*destpitch+destx,
                    source ? source+sourcey*sourcepitch+sourcex : NULL,
                    destwidth,
                    destheight,
                    destpitch,
                    sourcepitch,
                    pattern,
                    rop3);
}

//===========================================================================
BOOL APIENTRY SBltROP3Tiled (LPBYTE dest,
                             LPRECT destrect,
                             int    destpitch,
                             LPBYTE source,
                             LPRECT sourcerect,
                             int    sourcepitch,
                             int    sourceoffsetx,
                             int    sourceoffsety,
                             DWORD  pattern,
                             DWORD  rop3) {
  // to minimize function call overhead for this time critical function,
  // we use assert instead of validate so that the retail version has
  // no parameter checking code
  ASSERT(dest);
  ASSERT(destrect);
  ASSERT(destpitch > 0);

  int sourcecx = sourcerect->right-sourcerect->left;
  int sourcecy = sourcerect->bottom-sourcerect->top;
  int y = destrect->top;
  while (y < destrect->bottom) {
    int tiley = sourcerect->top;
    if (y == destrect->top)
      tiley += (sourceoffsety+sourcecy) % sourcecy;
    int tilecy = min(sourcerect->bottom-tiley,
                     destrect->bottom-y);
    LPBYTE basedest   = dest+y*destpitch;
    LPBYTE basesource = source+tiley*sourcepitch;
    int x = destrect->left;
    while (x < destrect->right) {
      int tilex = sourcerect->left;
      if (x == destrect->left)
        tilex += (sourceoffsetx+sourcecx) % sourcecx;
      int tilecx = min(sourcerect->right-tilex,
                       destrect->right-x);
      if (!SBltROP3(basedest+x,
                    basesource+tilex,
                    tilecx,
                    tilecy,
                    destpitch,
                    sourcepitch,
                    pattern,
                    rop3))
        return FALSE;
      x += tilecx;
    }
    y += tilecy;
  }

  return TRUE;
}

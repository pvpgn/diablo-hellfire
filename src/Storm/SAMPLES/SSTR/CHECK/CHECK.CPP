/****************************************************************************
*
*  CHECK.CPP
*
*  Checks the functionality of SStr functions
*
***/

#include <windows.h>
#include <stdio.h>
#include <storm.h>

#define  FILLCHAR1   0x11
#define  FILLCHAR2   0x22
#define  FINDCHAR    100
#define  MAXLEN      260
#define  BUFFERSIZE  (MAXLEN+2*sizeof(DWORD)+2)
#define  FMTSTRING   "Checking %-12s "

#define  FATALERROR                                                         \
  do {                                                                      \
    printf("FAILURE!\n"                                                     \
           "  str1len=%u\n"                                                 \
           "  str1ofs=%u\n"                                                 \
           "  str2len=%u\n"                                                 \
           "  str2ofs=%u\n",                                                \
           str1len,                                                         \
           str1ofs,                                                         \
           str2len,                                                         \
           str2ofs);                                                        \
    for (DWORD loop = 0; loop < BUFFERSIZE; ++loop) {                       \
      printf("%02x ",(DWORD)(BYTE)str1[loop]);                              \
      if ((loop & 15) == 15)                                                \
        printf("\n");                                                       \
    }                                                                       \
    exit(1);                                                                \
  } while (0)

#define  ALLOCBUF                                                           \
    LPTSTR str1 = (LPTSTR)ALLOC(BUFFERSIZE);                                \
    LPTSTR str2 = (LPTSTR)ALLOC(BUFFERSIZE);                                \
    LPTSTR src  = (LPTSTR)ALLOC(BUFFERSIZE)

#define  FREEBUF                                                            \
    FREE(str1);                                                             \
    FREE(str2);                                                             \
    FREE(src)

#define  ITERATE_1LEVEL                                                     \
    DWORD str2len = 0;                                                      \
    DWORD str2ofs = 0;                                                      \
    for (DWORD str1len = 0; str1len < MAXLEN; ++str1len)                    \
      for (DWORD str1ofs = 0; str1ofs < sizeof(DWORD); ++str1ofs)

#define  ITERATE_2LEVEL                                                     \
    for (DWORD str1len = 0; str1len < MAXLEN; ++str1len)                    \
      for (DWORD str1ofs = 0; str1ofs < sizeof(DWORD); ++str1ofs)           \
        for (DWORD str2len = 0; str2len < MAXLEN; ++str2len)                \
          for (DWORD str2ofs = 0; str2ofs <= sizeof(DWORD); ++str2ofs)

#define  RESETBUF                                                           \
    FillMemory(str1,BUFFERSIZE,FILLCHAR1);                                  \
    FillMemory(str2,BUFFERSIZE,FILLCHAR2)

#define  BUILDSOURCE                                                        \
    do {                                                                    \
      for (DWORD loop = 0; loop < BUFFERSIZE; ++loop)                       \
        src[loop] = ' '+(BYTE)(loop & 0x3F);                                \
    } while (0)

#define  COPYSOURCE(buffer,length)                                          \
    if (length > 0)                                                         \
      CopyMemory((buffer),src,(length));                                    \
    (buffer)[(length)] = 0

#define  SHOWPROGRESS                                                       \
    static DWORD iterations;                                                \
    if (!(++iterations & 0xFFFF))                                           \
      printf(".")

#define  CHECKBEGIN                                                         \
    DWORD check = 0;                                                        \
    DWORD checkstart

#define  CHECK_FILLCHAR(num)                                                \
    for (checkstart = check; check < checkstart+(num); ++check)             \
      if (str1[check] != FILLCHAR1)                                         \
        FATALERROR

#define  CHECK_SOURCE(num)                                                  \
    for (checkstart = check; check+1 < checkstart+(num); ++check)           \
      if (str1[check] != src[check-checkstart])                             \
        FATALERROR

#define  CHECK_NULL(num)                                                    \
    for (checkstart = check; check < checkstart+(num); ++check)             \
      if (str1[check])                                                      \
        FATALERROR

#define  CHECKEND

//===========================================================================
static void CheckSStrChr () {
  printf(FMTSTRING,"SStrChr()");
  ALLOCBUF;
  BUILDSOURCE;
  ITERATE_2LEVEL {
    RESETBUF;
    COPYSOURCE(str1+str1ofs,BUFFERSIZE-str1ofs-1);
    str1[str1ofs+str1len] = FINDCHAR;
    str1[str1ofs+str2len] = FINDCHAR;
    LPTSTR first = str1+str1ofs+min(str1len,str2len);
    LPTSTR last  = str1+str1ofs+max(str1len,str2len);
    if (SStrChr(str1+str1ofs,FINDCHAR) != first)
      FATALERROR;
    if (SStrChr(str1+str1ofs,FINDCHAR,TRUE) != last)
      FATALERROR;
    SHOWPROGRESS;
  }
  FREEBUF;
  printf(" done\n");
}

//===========================================================================
static void CheckSStrCopy () {
  printf(FMTSTRING,"SStrCopy()");
  ALLOCBUF;
  BUILDSOURCE;
  ITERATE_2LEVEL {
    RESETBUF;
    COPYSOURCE(str2+str2ofs,str2len);
    SStrCopy(str1+str1ofs,str2+str2ofs,str1len);
    CHECKBEGIN;
    CHECK_FILLCHAR(str1ofs);
    CHECK_SOURCE(min(str1len,str2len+1));
    CHECK_NULL(str1len ? 1 : 0);
    CHECK_FILLCHAR(BUFFERSIZE-checkstart);
    CHECKEND;
    SHOWPROGRESS;
  }
  FREEBUF;
  printf(" done\n");
}

//===========================================================================
static void CheckSStrLen () {
  printf(FMTSTRING,"SStrLen()");
  ALLOCBUF;
  BUILDSOURCE;
  ITERATE_1LEVEL {
    RESETBUF;
    COPYSOURCE(str1+str1ofs,str1len);
    if (SStrLen(str1+str1ofs) != str1len)
      FATALERROR;
    SHOWPROGRESS;
  }
  FREEBUF;
  printf("done\n");
}

//===========================================================================
static void CheckSStrPack () {
  printf(FMTSTRING,"SStrPack()");
  ALLOCBUF;
  BUILDSOURCE;
  ITERATE_2LEVEL {
    RESETBUF;
    COPYSOURCE(str1+str1ofs,str1len);
    COPYSOURCE(str2+str2ofs,str2len);
    DWORD bufferlen = BUFFERSIZE-str1ofs;
    SStrPack(str1+str1ofs,str2+str2ofs,bufferlen);
    CHECKBEGIN;
    CHECK_FILLCHAR(str1ofs);
    CHECK_SOURCE(str1len+1);
    CHECK_SOURCE(min(bufferlen-str1len,str2len+1));
    CHECK_NULL(1);
    CHECK_FILLCHAR(BUFFERSIZE-checkstart-1);
    if (check < BUFFERSIZE)
      CHECK_NULL(1);
    CHECKEND;
    SHOWPROGRESS;
  }
  FREEBUF;
  printf(" done\n");
}

//===========================================================================
int __cdecl main () {
  CheckSStrChr();
  CheckSStrCopy();
  CheckSStrLen();
  CheckSStrPack();
  printf("No errors detected.\n");
  return 0;
}

/****************************************************************************
*
*  SSTR.CPP
*
*  Benchmarks Storm's pattern blt performance.
*
***/

#include <windows.h>
#include <stdio.h>
#include <storm.h>

#define  BENCHMARK(name,expr,reset)              \
  do {                                           \
    DWORD start = GetTickCount();                \
    DWORD curr;                                  \
    while ((curr = GetTickCount()) == start)     \
      ;                                          \
    start = curr;                                \
    int   dummy      = 0;                        \
    DWORD iterations = 0;                        \
    while (GetTickCount()-start < 1000) {        \
      for (int loop = 0; loop < 1000; ++loop) {  \
        (expr);                                  \
        (reset);                                 \
      }                                          \
      iterations += 1000;                        \
    }                                            \
    printf("%-20s %7u/sec\n",(name),iterations); \
  } while (0)


//===========================================================================
int __cdecl main () {
  char buffer[] = "this is a test";
  char buffer2[256];
  BENCHMARK("strcpy()"  ,strcpy(buffer2,buffer)      ,0);
  BENCHMARK("strncpy()" ,strncpy(buffer2,buffer,256) ,0);
  BENCHMARK("lstrcpy()" ,lstrcpy(buffer2,buffer)     ,0);
  BENCHMARK("lstrcpyn()",lstrcpyn(buffer2,buffer,256),0);
  BENCHMARK("SStrCopy()",SStrCopy(buffer2,buffer,256),0);
  printf("\n");
  BENCHMARK("strcat()"  ,strcat(buffer2,buffer)        ,buffer2[14] = 0);
  BENCHMARK("strncat()" ,strncat(buffer2,buffer,256-14),buffer2[14] = 0);
  BENCHMARK("lstrcat()" ,lstrcat(buffer2,buffer)       ,buffer2[14] = 0);
  BENCHMARK("SStrPack()",SStrPack(buffer2,buffer,256)  ,buffer2[14] = 0);
  printf("\n");
  BENCHMARK("strlen()"  ,dummy += strlen(buffer) ,0);
  BENCHMARK("lstrlen()" ,dummy += lstrlen(buffer),0);
  BENCHMARK("SStrLen()" ,dummy += SStrLen(buffer),0);
  printf("\n");
  BENCHMARK("strchr()"  ,dummy += (strchr(buffer,' ') != NULL) ,0);
  BENCHMARK("SStrChr()" ,dummy += (SStrChr(buffer,' ') != NULL),0);
  printf("\n");
  return 0;
}


/****************************************************************************
*
*  BENCH.CPP
*
*  This program benchmarks the performance of Storm's region manager.
*
***/

#include <windows.h>
#include <stdio.h>
#include <storm.h>

#define  TESTRECTS        3
#define  MAXCOMBINEDRECS  64

const RECT testrect[TESTRECTS] = {{100,100,300,300},
                                  {200,200,400,400},
                                  {100,300,300,500}};

//===========================================================================
static void TestStorm (BOOL print) {
  CSRgn rgn;

  for (DWORD loop = 0; loop < TESTRECTS; ++loop)
    rgn.AddRect(&testrect[loop],NULL);

  RECT  rectarray[MAXCOMBINEDRECS];
  DWORD numrects = MAXCOMBINEDRECS;
  rgn.GetRects(&numrects,&rectarray[0]);

  if (print) {
    printf("Storm\n");
    for (DWORD loop = 0; loop < numrects; ++loop)
      printf("%u (%d,%d)-(%d,%d)\n",
             loop,
             rectarray[loop].left,
             rectarray[loop].top,
             rectarray[loop].right,
             rectarray[loop].bottom);
    printf("\n");
  }
}

//===========================================================================
static void TestWindows (BOOL print) {
  HRGN rgn = CreateRectRgn(testrect[0].left,
                           testrect[0].top,
                           testrect[0].right,
                           testrect[0].bottom);

  for (DWORD loop = 1; loop < TESTRECTS; ++loop) {
    HRGN newrgn = CreateRectRgn(testrect[loop].left,
                                testrect[loop].top,
                                testrect[loop].right,
                                testrect[loop].bottom);
    CombineRgn(rgn,rgn,newrgn,RGN_OR);
    DeleteObject(newrgn);
  }

#define  BUFFERSIZE  sizeof(RGNDATA)+MAXCOMBINEDRECS*sizeof(RECT)
  BYTE     buffer[BUFFERSIZE];
  RGNDATA *data     = (RGNDATA *)&buffer[0];
  DWORD    numrects = GetRegionData(rgn,BUFFERSIZE,data);
#undef  BUFFERSIZE

  if (print) {
    printf("Windows\n");
    for (DWORD loop = 0; loop < data->rdh.nCount; ++loop)
      printf("%u (%d,%d)-(%d,%d)\n",
             loop,
             (((LPRECT)&data->Buffer[0])+loop)->left,
             (((LPRECT)&data->Buffer[0])+loop)->top,
             (((LPRECT)&data->Buffer[0])+loop)->right,
             (((LPRECT)&data->Buffer[0])+loop)->bottom);
    printf("\n");
  }

  DeleteObject(rgn);
}

//===========================================================================
static DWORD Benchmark (void (*func)(BOOL)) {
  DWORD iterations = 0;
  DWORD start      = GetTickCount();
  while (GetTickCount()-start < 1000)
    for (DWORD loop = 0; loop < 10; ++loop) {
      func(FALSE);
      ++iterations;
    }
  return iterations;
}

//===========================================================================
int __cdecl main () {
  TestStorm(TRUE);
  TestWindows(TRUE);
  printf("Storm:   %u iterations/sec\n",Benchmark(TestStorm));
  printf("Windows: %u iterations/sec\n",Benchmark(TestWindows));
  return 0;
}

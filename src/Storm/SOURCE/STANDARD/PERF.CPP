/****************************************************************************
*
*  PERF.CPP
*  Common performance monitoring functions
*
*  By Michael O'Brien (12/1/96)
*
***/

#include "pch.h"
#pragma  hdrstop

static DWORD perfdata[PERFNUM] = {0};

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
void PerfAdd (DWORD id, DWORD value) {
   if (id < PERFNUM)
     perfdata[id] += value;
}

//===========================================================================
BOOL PerfGetPerformanceData (DWORD          counterid,
                             DWORD         *countervalue,
                             LARGE_INTEGER *measurementtime,
                             LARGE_INTEGER *measurementfreq) {
  switch (counterid) {

    case SNET_PERFID_PKTSENTONWIRE:
      *countervalue = perfdata[PERF_PKTSENT];
      return 1;

    case SNET_PERFID_PKTRECVONWIRE:
      *countervalue = perfdata[PERF_PKTRECV];
      return 1;

    case SNET_PERFID_BYTESSENTONWIRE:
      *countervalue = perfdata[PERF_BYTESSENT];
      return 1;

    case SNET_PERFID_BYTESRECVONWIRE:
      *countervalue = perfdata[PERF_BYTESRECV];
      return 1;

  }
  return 0;
}

//===========================================================================
void PerfIncrement (DWORD id) {
  if (id < PERFNUM)
    InterlockedIncrement((LONG *)&perfdata[id]);
}

//===========================================================================
void PerfReset () {
  ZeroMemory(&perfdata[0],PERFNUM*sizeof(DWORD));
}

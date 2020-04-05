/****************************************************************************
*
*  SLOG.CPP
*  Storm logging functions
*
*  By Michael O'Brien (10/14/97)
*
***/

#include "pch.h"
#pragma  hdrstop

#define  BUFFERSIZE  0x10000
#define  FLUSHMARK   0xC000
#define  SLOTS       4  // must be a power of two

DECLARE_STRICT_HANDLE(HLOCKEDLOG);

typedef struct _LOG {
    HSLOG   log;
    _LOG   *next;
    HANDLE  file;
    DWORD   bufferused;
    DWORD   pendpoint;
    char    buffer[BUFFERSIZE];
} LOG, *LOGPTR;

static CRITICAL_SECTION s_critsect[SLOTS];
static LOGPTR           s_loghead[SLOTS];
static HSLOG            s_sequence;

//===========================================================================
static void FlushLog (LOGPTR logptr) {
  if (!logptr->bufferused)
    return;
  DWORD byteswritten;
  WriteFile(logptr->file,
            logptr->buffer,
            logptr->bufferused,
            &byteswritten,
            NULL);
  logptr->bufferused = 0;
  logptr->pendpoint  = 0;
}

//===========================================================================
static LOGPTR LockLog (HSLOG       log,
                       HLOCKEDLOG *lockedhandle,
                       BOOL        createifnecessary) {

  // IF THE LOG HANDLE IS NULL, JUST RETURN A NULL POINTER, INDICATING TO
  // THE CALLER THAT IT SHOULD SILENTLY FAIL.  THIS ALLOWS APPLICATIONS
  // TO USE THE SAME CODE WHETHER LOGGING IS ENABLED OR DISABLED.
  if (!log) {
    *lockedhandle = (HLOCKEDLOG)0xFFFFFFFF;
    return NULL;
  }

  // DETERMINE WHICH SLOT THE LOG RECORD SHOULD BE IN
  DWORD slot = (DWORD)log & (SLOTS-1);
  EnterCriticalSection(&s_critsect[slot]);
  *lockedhandle = (HLOCKEDLOG)slot;

  // SEARCH FOR AN EXISTING LOG RECORD
  LOGPTR *nextptr = &s_loghead[slot];
  LOGPTR  currptr;
  while ((currptr = *nextptr) != NULL)
    if (currptr->log == log)
      return currptr;
    else
      nextptr = &currptr->next;

  // IF WE DIDN'T FIND ONE, CREATE A NEW RECORD
  if (!createifnecessary) {
    LeaveCriticalSection(&s_critsect[slot]);
    *lockedhandle = (HLOCKEDLOG)0xFFFFFFFF;
    return NULL;
  }
  currptr = *nextptr = (LOGPTR)VirtualAlloc(NULL,sizeof(LOG),MEM_COMMIT,PAGE_READWRITE);
  currptr->log        = log;
  currptr->next       = NULL;
  currptr->file       = INVALID_HANDLE_VALUE;
  currptr->bufferused = 0;
  currptr->pendpoint  = 0;
  return currptr;

}

//===========================================================================
static void OutputReturn (LOGPTR logptr) {
  CopyMemory(logptr->buffer+logptr->bufferused,"\r\n",3);
  logptr->bufferused += 2;
}

//===========================================================================
static void OutputTime (LOGPTR logptr, BOOL show) {

  // GENERATE A NEW TIME STRING IF NECESSARY
  static char  timestr[64] = "";
  static DWORD timestrlen  = 0;
  static DWORD lasttime    = 0;
  DWORD currtime = GetTickCount();
  if (currtime != lasttime) {
    lasttime = currtime;
    SYSTEMTIME systime;
    GetLocalTime(&systime);
    wsprintf(timestr,
             "%u/%u %02u:%02u:%02u.%03u  ",
             systime.wMonth,
             systime.wDay,
             systime.wHour,
             systime.wMinute,
             systime.wSecond,
             systime.wMilliseconds);
    timestrlen = SStrLen(timestr);
  }

  // COPY THE TIME STRING TO THE OUTPUT BUFFER
  if (show)
    CopyMemory(logptr->buffer+logptr->bufferused,timestr,timestrlen+1);
  else {
    FillMemory(logptr->buffer+logptr->bufferused,timestrlen,' ');
    logptr->buffer[logptr->bufferused+timestrlen] = 0;
  }
  logptr->bufferused += timestrlen;

}

//===========================================================================
static void UnlockDeleteLog (LOGPTR logptr, HLOCKEDLOG lockedhandle) {
  DWORD   slot    = (DWORD)lockedhandle;
  LOGPTR *nextptr = &s_loghead[slot];
  LOGPTR  currptr;
  while ((currptr = *nextptr) != NULL)
    if (currptr == logptr) {
      *nextptr = currptr->next;
      VirtualFree(currptr,0,MEM_RELEASE);
      break;
    }
    else
      nextptr = &currptr->next;
  LeaveCriticalSection(&s_critsect[slot]);
}

//===========================================================================
static void UnlockLog (HLOCKEDLOG lockedhandle) {
  DWORD slot = (DWORD)lockedhandle;
  LeaveCriticalSection(&s_critsect[slot]);
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
void APIENTRY SLogClose (HSLOG log) {

  // LOCK THE LOG RECORD
  HLOCKEDLOG lockedhandle;
  LOGPTR     logptr = LockLog(log,&lockedhandle,FALSE);
  if (!logptr)
    return;

  // FLUSH THE LOG RECORD AND CLOSE THE FILE
  FlushLog(logptr);
  CloseHandle(logptr->file);

  // UNLOCK THE LOG RECORD
  UnlockDeleteLog(logptr,lockedhandle);

}

//===========================================================================
BOOL APIENTRY SLogCreate (LPCTSTR  filename,
                          DWORD    flags,
                          HSLOG   *log) {
  VALIDATEBEGIN;
  VALIDATE(filename);
  VALIDATE(*filename);
  VALIDATEANDBLANK(log);
  VALIDATEEND;

  // OPEN THE FILE
  HANDLE file = CreateFile(filename,
                           GENERIC_WRITE,
                           FILE_SHARE_READ,
                           NULL,
                           CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL);
  if (file == INVALID_HANDLE_VALUE)
    return FALSE;

  // CREATE THE LOG RECORD
  *log = s_sequence = (HSLOG)(((DWORD)s_sequence)+1);
  HLOCKEDLOG lockedhandle;
  LOGPTR     logptr = LockLog(*log,&lockedhandle,TRUE);
  logptr->file = file;
  UnlockLog(lockedhandle);

  return TRUE;
}

//===========================================================================
void APIENTRY SLogDestroy () {
  SLogFlushAll();
  for (DWORD slot = 0; slot < SLOTS; ++slot) {
    EnterCriticalSection(&s_critsect[slot]);
    while (s_loghead[slot]) {
      SLogClose(s_loghead[slot]->log);
      REPORTRESOURCELEAK(HSLOG);
    }
    LeaveCriticalSection(&s_critsect[slot]);
    DeleteCriticalSection(&s_critsect[slot]);
  }
}

//===========================================================================
void APIENTRY SLogDump (HSLOG   log,
                        LPCVOID data,
                        DWORD   bytes) {

  // LOCK THE LOG RECORD
  HLOCKEDLOG lockedhandle;
  LOGPTR     logptr = LockLog(log,&lockedhandle,FALSE);
  if (!logptr)
    return;

#define  SPRINTFTOBUFFER(fmt,val)                                        \
    do {                                                                 \
      wsprintf(logptr->buffer+logptr->bufferused,(fmt),(val));           \
      logptr->bufferused += SStrLen(logptr->buffer+logptr->bufferused);  \
    } while (0)
#define  STRCPYTOBUFFER(str)                                             \
    logptr->bufferused += SStrCopy(logptr->buffer+logptr->bufferused,(str))

  // OUTPUT THE DATA TO THE LOG IN HEX FORMAT, EIGHT BYTES PER LINE
  DWORD offset = 0;
  while (offset < bytes) {
    OutputTime(logptr,FALSE);
    SPRINTFTOBUFFER("%04x  ",offset);
    DWORD loop;
    for (loop = offset; loop < offset+8; ++loop) {
      if (loop < bytes)
        SPRINTFTOBUFFER("%02x ",(unsigned)*((LPBYTE)data+loop));
      else
        STRCPYTOBUFFER("   ");
      if ((loop & 3) == 3)
        STRCPYTOBUFFER("  ");
    }
    for (loop = offset; loop < offset+8; ++loop) {
      char value = (loop < bytes) ? *((char *)data+loop) : 0;
      if ((value >= 32) && (value <= 126))
        SPRINTFTOBUFFER("%c",value);
      else
        STRCPYTOBUFFER(value ? "." : " ");
      if ((loop & 7) == 3)
        STRCPYTOBUFFER(" ");
    }
    OutputReturn(logptr);
    offset += 8;
  }

#undef  STRCPYTOBUFFER
#undef  SPRINTFTOBUFFER

  // ADVANCE THE PENDING POINT
  logptr->pendpoint = logptr->bufferused;

  // IF THE BUFFER IS GETTING FULL, FLUSH IT
  if (logptr->bufferused >= FLUSHMARK)
    FlushLog(logptr);

  // UNLOCK THE LOG RECORD
  UnlockLog(lockedhandle);

}

//===========================================================================
void APIENTRY SLogFlush (HSLOG log) {

  // LOCK THE LOG RECORD
  HLOCKEDLOG lockedhandle;
  LOGPTR     logptr = LockLog(log,&lockedhandle,FALSE);
  if (!logptr)
    return;

  // FLUSH THE LOG
  FlushLog(logptr);

  // UNLOCK THE LOG RECORD
  UnlockLog(lockedhandle);

}

//===========================================================================
void APIENTRY SLogFlushAll () {
  for (DWORD slot = 0; slot < SLOTS; ++slot) {
    EnterCriticalSection(&s_critsect[slot]);
    LOGPTR curr = s_loghead[slot];
    while (curr) {
      FlushLog(curr);
      curr = curr->next;
    }
    LeaveCriticalSection(&s_critsect[slot]);
  }
}

//===========================================================================
void APIENTRY SLogInitialize () {
  for (DWORD slot = 0; slot < SLOTS; ++slot)
    InitializeCriticalSection(&s_critsect[slot]);
}

//===========================================================================
void __cdecl SLogPend (HSLOG   log,
                       LPCTSTR format,
                       ...) {

  // LOCK THE LOG RECORD
  HLOCKEDLOG lockedhandle;
  LOGPTR     logptr = LockLog(log,&lockedhandle,FALSE);
  if (!logptr)
    return;

  // RESET THE OUTPUT LOCATION TO THE PENDING POINT
  logptr->bufferused = logptr->pendpoint;

  // OUTPUT THE CURRENT TIME TO THE LOG
  OutputTime(logptr,TRUE);

  // OUTPUT THE ARGUMENT STRING TO THE LOG
  va_list arglist;
  va_start(arglist,format);
  vsprintf(logptr->buffer+logptr->bufferused,
           format,
           arglist);
  va_end(arglist);
  logptr->bufferused += SStrLen(logptr->buffer+logptr->bufferused);

  // OUTPUT A CARRIAGE RETURN TO THE LOG
  OutputReturn(logptr);

  // UNLOCK THE LOG RECORD
  UnlockLog(lockedhandle);

}

//===========================================================================
void __cdecl SLogWrite (HSLOG   log,
                        LPCTSTR format,
                        ...) {

  // LOCK THE LOG RECORD
  HLOCKEDLOG lockedhandle;
  LOGPTR     logptr = LockLog(log,&lockedhandle,FALSE);
  if (!logptr)
    return;

  // OUTPUT THE CURRENT TIME TO THE LOG
  OutputTime(logptr,TRUE);

  // OUTPUT THE ARGUMENT STRING TO THE LOG
  va_list arglist;
  va_start(arglist,format);
  vsprintf(logptr->buffer+logptr->bufferused,
           format,
           arglist);
  va_end(arglist);
  logptr->bufferused += SStrLen(logptr->buffer+logptr->bufferused);

  // OUTPUT A CARRIAGE RETURN TO THE LOG
  OutputReturn(logptr);

  // ADVANCE THE PENDING POINT
  logptr->pendpoint = logptr->bufferused;

  // IF THE BUFFER IS GETTING FULL, FLUSH IT
  if (logptr->bufferused >= FLUSHMARK)
    FlushLog(logptr);

  // UNLOCK THE LOG RECORD
  UnlockLog(lockedhandle);

}

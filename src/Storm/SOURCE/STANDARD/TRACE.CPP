/****************************************************************************
*
*  TRACE.CPP
*  Modem trace functions
*
*  Written by Mike O'Brien
*  Modified by Jeff Strain (1/9/97)
*
***/
	
#include "pch.h"
#pragma  hdrstop
#include "trace.h"

// DEFAULT DATA FILE FOR ALL TRACE COMMANDS
#define  TRACEFILE "c:\\mdmtrace.txt"

/****************************************************************************
*
*  TRACE FUNCTIONS
*
***/

static CCritSect  trace_critsect;
static FILE      *trace_file         = NULL;
static DWORD      trace_init         = 0;
static DWORD      trace_pendcount    = 0;
static char       trace_pending[256] = "";

//===========================================================================
void TraceBegin (BOOL writepending) {
#ifdef _DEBUG

  // OPEN THE FILE
  if (!trace_file) {
    trace_file = fopen(TRACEFILE,trace_init ? "at" : "wt");
    if (!trace_file)
      return;
    else if (!trace_init)
      trace_init = GetTickCount();
  }

  // WRITE AND CLEAR ANY PENDING LINES
  if (trace_pending[0]) {
    if (trace_pendcount > 1)
      fprintf(trace_file,"         [...]\n");
    if (writepending)
      fprintf(trace_file,"%s",trace_pending);
    trace_pendcount  = 0;
    trace_pending[0] = 0;
  }

#endif
}

//===========================================================================
void TraceDestroy () {
#ifdef _DEBUG
  trace_critsect.Enter();
  if (trace_file) {
    fclose(trace_file);
    trace_file = NULL;
  }
  trace_critsect.Leave();
#endif
}

//===========================================================================
void TraceDumpAddr (LPCSTR addrname, LPVOID addr, unsigned size) {
#ifdef _DEBUG
#define LINESIZE 80
  char outstr[LINESIZE + 1];
  sprintf(outstr,"  %s=", addrname);
  size = min(size, (LINESIZE - strlen(outstr)) / 2);
  for (unsigned loop = 0; loop < size; ++loop)
    sprintf(outstr+strlen(outstr),"%02x",(DWORD)*((LPBYTE)addr+loop));
  TraceOut(outstr);
#endif
}

//===========================================================================
void __cdecl TraceOut (const char *format, ...) {
#ifdef _DEBUG
  trace_critsect.Enter();
  TraceBegin((strlen(format) > 1) && (format[0] == ' '));

  // WRITE THE LINE
  DWORD eventtime = GetTickCount()-trace_init;
  fprintf(trace_file,"%4u.%02u  ",eventtime/1000,(eventtime % 1000)/10);
  va_list arglist;
  va_start(arglist,format);
  vfprintf(trace_file,format,arglist);
  va_end(arglist);
  fprintf(trace_file,"\n");
  fflush(trace_file);

  trace_critsect.Leave();
#endif
}

//===========================================================================
void __cdecl TracePend (const char *format, ...) {
#ifdef _DEBUG
  trace_critsect.Enter();

  // SAVE THE LINE
  ++trace_pendcount;
  DWORD eventtime = GetTickCount()-trace_init;
  sprintf(trace_pending,"%4u.%02u  ",eventtime/1000,(eventtime % 1000)/10);
  va_list arglist;
  va_start(arglist,format);
  vsprintf(trace_pending+strlen(trace_pending),format,arglist);
  va_end(arglist);
  strcat(trace_pending,"\n");

  trace_critsect.Leave();
#endif
}

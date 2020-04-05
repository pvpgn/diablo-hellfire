/****************************************************************************
*
*  STORM.CPP
*  Storm main module
*
*  By Michael O'Brien (2/8/96)
*
***/

#include "pch.h"
#pragma  hdrstop

static HINSTANCE                    s_instance  = (HINSTANCE)0;
static LPTOP_LEVEL_EXCEPTION_FILTER s_oldfilter = NULL;

LONG CALLBACK ExceptionFilter (EXCEPTION_POINTERS *exceptinfo);

//===========================================================================
#ifndef STATICLIB
extern "C" BOOL APIENTRY DllMain (HINSTANCE instance,
                                  DWORD     reason,
                                  LPVOID) {
  if (reason == DLL_PROCESS_ATTACH) {
    s_instance  = instance;
    s_oldfilter = SetUnhandledExceptionFilter(ExceptionFilter);
  }
  else if (reason == DLL_PROCESS_DETACH) {
    SetUnhandledExceptionFilter(s_oldfilter);
    StormDestroy();
  }
  return TRUE;
}
#endif

//===========================================================================
LONG CALLBACK ExceptionFilter (EXCEPTION_POINTERS *exceptinfo) {
  SErrSuppressErrors(TRUE);
  SLogFlushAll();
  if (s_oldfilter)
    s_oldfilter(exceptinfo);
  return EXCEPTION_CONTINUE_SEARCH;
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
#ifndef STATICLIB
BOOL APIENTRY StormDestroy () {

  // DESTROY MODULES WHICH DEPEND ON SDRAW
  SDlgDestroy();
  SGdiDestroy();
  SVidDestroy();
  SDrawDestroy();

  // DESTROY MODULES WHICH DEPEND ON SRGN
  SRgnDestroy();

  // DESTROY MODULES WHICH DEPEND ON SEVT
  SMsgDestroy();
  SNetDestroy();
  SEvtDestroy();

  // DESTROY MODULES WHICH DEPEND ON SCODE
  SBltDestroy();
  SCodeDestroy();

  // DESTROY OTHER HIGH LEVEL MODULES
  SCmdDestroy();
  SFileDestroy();
  STransDestroy();

  // THE MEMORY MANAGER, ERROR HANDLER, AND LOGGING MODULE ARE NOT
  // DESTROYED UNTIL AFTER ALL DESTRUCTORS HAVE BEEN CALLED BY THE
  // RUNTIME LIBRARY CLEANUP CODE

  return TRUE;
}
#endif

//===========================================================================
HINSTANCE StormGetInstance () {
#ifdef STATICLIB
  return (HINSTANCE)GetModuleHandle(NULL);
#else
  return s_instance;
#endif
}

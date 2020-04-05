/****************************************************************************
*
*  SRTL.CPP
*  Replacements for select C runtime library functions
*
*  By Michael O'Brien (3/6/97)
*
***/

#include "pch.h"
#pragma  hdrstop

#ifndef  STATICLIB
#define  REPLACESTARTUP   1
#define  REPLACETYPEINFO  1
#endif

/****************************************************************************
*
*  STARTUP AND SHUTDOWN FUNCTIONS
*
***/

#if REPLACESTARTUP

typedef void (__cdecl *_PVFV)(void);

#pragma data_seg(".CRT$XIA")
_PVFV __xi_a[] = { NULL };    // C initializers (begin)
#pragma data_seg(".CRT$XIZ")
_PVFV __xi_z[] = { NULL };    // C initializers (end)
#pragma data_seg(".CRT$XCA")
_PVFV __xc_a[] = { NULL };    // C++ initializers (begin)
#pragma data_seg(".CRT$XCZ")
_PVFV __xc_z[] = { NULL };    // C++ initializers (end)
#pragma data_seg()

static CRITICAL_SECTION  s_critsect;
static DWORD             s_termalloc = 0;
static _PVFV            *s_termlist  = NULL;
static DWORD             s_termused  = 0;

extern BOOL APIENTRY DllMain (HINSTANCE, DWORD, LPVOID);

//===========================================================================
static void CallInitList (_PVFV *begin, _PVFV *end) {
  while (begin < end) {
    if (*begin)
      (**begin)();
    ++begin;
  }
}

//===========================================================================
static void RtlDestroy () {

  // CALL REGISTERED EXIT PROCEDURES (INCLUDING DESTRUCTORS FOR STATIC
  // OBJECTS) IN LIFO ORDER
  while (s_termused)
    (**(s_termlist+(--s_termused)))();

  // CLEAR THE LIST OF EXIT PROCEDURES
  VirtualFree(s_termlist,0,MEM_RELEASE);
  s_termalloc = 0;
  s_termlist  = NULL;

  // DESTROY LOW-LEVEL MODULES.  THESE ARE DESTROYED NOW RATHER THAN DURING
  // DLLMAIN() SO THAT THEY CAN BE USED BY DESTRUCTORS.
  SMemDestroy();
  SErrDestroy();
  SLogDestroy();

  // DELETE OUR CRITICAL SECTION
  DeleteCriticalSection(&s_critsect);

}

//===========================================================================
static void RtlInitialize () {

  // INITIALIZE OUR CRITICAL SECTION
  InitializeCriticalSection(&s_critsect);

  // INITIALIZE STORM'S LOG AND MEMORY MANAGERS, SO THAT THEY CAN BE USED BY
  // CLASS CONSTRUCTORS
  SLogInitialize();
  SMemInitialize();

  // CALL C/C++ INITIALIZERS (INCLUDING CONSTRUCTORS FOR STATIC OBJECTS)
  CallInitList(__xi_a,__xi_z);
  CallInitList(__xc_a,__xc_z);

}

//===========================================================================
extern "C" BOOL APIENTRY _DllMainCRTStartup (HINSTANCE instance,
                                             DWORD     reason,
                                             LPVOID    reserved) {
  if (reason == DLL_PROCESS_ATTACH)
    RtlInitialize();
  BOOL result = DllMain(instance,reason,reserved);
  if ((reason == DLL_PROCESS_DETACH) ||
      ((reason == DLL_PROCESS_ATTACH) && !result))
    RtlDestroy();
  return result;
}

#endif // if REPLACESTARTUP

/****************************************************************************
*
*  EXPORTED STARTUP/SHUTDOWN FUNCTIONS
*
***/

#if REPLACESTARTUP

//===========================================================================
int __cdecl atexit (_PVFV func) {
  EnterCriticalSection(&s_critsect);

  // GROW THE TERMINATOR LIST IF NECESSARY
  if (s_termused >= s_termalloc) {
    DWORD  newalloc = s_termalloc+1024;
    _PVFV *newlist  = (_PVFV *)VirtualAlloc(NULL,newalloc*sizeof(_PVFV),MEM_COMMIT,PAGE_READWRITE);
    if (!newlist) {
      LeaveCriticalSection(&s_critsect);
      return EXIT_FAILURE;
    }
    if (s_termlist) {
      CopyMemory(newlist,s_termlist,s_termalloc*sizeof(_PVFV));
      VirtualFree(s_termlist,0,MEM_RELEASE);
    }
    s_termalloc = newalloc;
    s_termlist  = newlist;
  }

  // ADD THIS ENTRY TO THE TERMINATOR LIST
  *(s_termlist+s_termused++) = func;

  LeaveCriticalSection(&s_critsect);
  return EXIT_SUCCESS;
}

#endif // if REPLACESTARTUP

/****************************************************************************
*
*  EXPORTED TYPEINFO FUNCTIONS
*
***/

#if REPLACETYPEINFO

//===========================================================================
type_info::type_info (const type_info& rhs) {
}

//===========================================================================
type_info::~type_info() {
}

//===========================================================================
type_info& type_info::operator= (const type_info& rhs) {
  return *this;
}

#endif // if REPLACETYPEINFO

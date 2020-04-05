/****************************************************************************
*
*  SERR.CPP
*  Storm error handling functions
*
*  By Michael O'Brien (4/10/97)
*
***/

#include "pch.h"
#pragma  hdrstop

#define  STR_ERROR        0
#define  STR_HEADER       1
#define  STR_PROGRAM      2
#define  STR_FILELINE     3
#define  STR_FUNCTION     4
#define  STR_OBJECT       5
#define  STR_HANDLE       6
#define  STR_EXPRESSION   7
#define  STR_DESCRIPTION  8
#define  STR_TERMINATE    9
#define  STR_RECOVERABLE  10
#define  STR_FILE         11
#define  STRINGS          12

static const LPCTSTR s_displaystr[STRINGS] =
                       {"ERROR #%u (0x%08x)",
                        "This application has encountered a critical error:\n\n%s\n",
                        "Program:\t%s\n",
                        "File:\t%s\nLine:\t%d\n",
                        "Function:\t%s\n",
                        "Object:\t%s\n",
                        "Handle:\t%s\n",
                        "Expr:\t%s\n\n",
                        "\n%s\n\n",
                        "Press OK to terminate the application.",
                        "Do you wish to terminate the application?",
                        "File:\t%s\n"};

typedef struct _MSGSRC {
    WORD     facility;
    WORD     reserved;
    HMODULE  module;
    _MSGSRC *next;
} MSGSRC, *MSGSRCPTR;

static CRITICAL_SECTION s_critsect;
static LONG             s_critsectinit = -1;
static MSGSRCPTR        s_msgsrchead   = NULL;
static BOOL             s_msgsrcinit   = FALSE;
static BOOL             s_suppress     = FALSE;
//#ifdef _MAC
static DWORD            s_lasterror    = 0;
//#endif

static void InternalEnterCriticalSection ();
static void InternalLeaveCriticalSection ();

//===========================================================================
static void AddStormFacility (WORD facility) {

  // ADD THE DEFINITION OF STORM'S ERROR CODES TO THE END OF THE LIST
  MSGSRCPTR *nextptr = &s_msgsrchead;
  while (*nextptr)
    nextptr = &(*nextptr)->next;
  *nextptr = (MSGSRCPTR)HeapAlloc(GetProcessHeap(),
                                  HEAP_GENERATE_EXCEPTIONS,
                                  sizeof(MSGSRC));
  (*nextptr)->facility = facility;
  (*nextptr)->module   = StormGetInstance();
  (*nextptr)->next     = NULL;

}

//===========================================================================
static void AddStormMessages () {
  AddStormFacility(STORMFAC);
#ifdef _FACDD
  AddStormFacility(_FACDD);
#else
  AddStormFacility(0x876);
#endif
#ifdef _FACDS
  AddStormFacility(_FACDS);
#else
  AddstormFacility(0x878);
#endif
}

//===========================================================================
static LPCTSTR GetString (UINT id) {
  static char buffer[256];
  if (LoadString(StormGetInstance(),IDS_BASE+id,buffer,256))
    return buffer;
  else
    return s_displaystr[id];
}

//===========================================================================
static void InternalEnterCriticalSection () {
  if (!InterlockedIncrement(&s_critsectinit))
    InitializeCriticalSection(&s_critsect);
  else
    InterlockedDecrement(&s_critsectinit);
  EnterCriticalSection(&s_critsect);
}

//===========================================================================
static void InternalLeaveCriticalSection () {
  LeaveCriticalSection(&s_critsect);
}

//===========================================================================
static BOOL UndecorateObjectName (LPCSTR source,
                                  LPSTR  dest,
                                  DWORD  destchars) {

  // THIS IS A NON-CRITICAL FUNCTION WHOSE ONLY PURPOSE IS TO MAKE THE
  // DISPLAYED ERROR MESSAGES EASIER TO READ.  SINCE NAME DECORATION IS
  // PERFORMED DIFFERENTLY BY EACH COMPILER, THIS FUNCTION ONLY ATTEMPTS
  // TO UNDECORATE NAMES UNDER MICROSOFT VISUAL C++.
#ifndef _MSC_VER
  return FALSE;
#endif

  // CHECK THE SOURCE NAME TO VERIFY THAT IT APPEARS TO BE A STRUCTURE
  // OR OBJECT
  if ((SStrLen(source) < 6) ||
      (source[0] != '.') ||
      (source[3] != 'U') ||
      !strpbrk(source+4,"?@"))
    return FALSE;

  // COPY THE SOURCE NAME, MINUS THE PREFIX, INTO THE DESTINATION BUFFER
  SStrCopy(dest,source+4,destchars);

  // STRIP OFF THE DETAILED TYPE INFORMATION
  LPSTR separator = strpbrk(dest,"?@");
  if (!separator)
    return FALSE;
  *separator-- = 0;

  // STRIP OFF ANY TRAILING UNDERSCORES
  while ((separator >= dest) &&
         (*separator == '_'))
    *separator-- = 0;

  return TRUE;
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
BOOL APIENTRY SErrDestroy () {
  s_msgsrcinit = FALSE;
  while (s_msgsrchead) {
    MSGSRCPTR next = s_msgsrchead->next;
    HeapFree(GetProcessHeap(),0,s_msgsrchead);
    s_msgsrchead = next;
  }
  if (s_critsectinit != -1) {
    DeleteCriticalSection(&s_critsect);
    s_critsectinit = -1;
  }
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SErrDisplayError (DWORD   errorcode,
                                LPCTSTR filename,
                                int     linenumber,
                                LPCTSTR description,
                                BOOL    recoverable,
                                UINT    exitcode) {
  if (s_suppress)
    return FALSE;

  // ENTER THE CRITICAL SECTION
  InternalEnterCriticalSection();

  // FLUSH ALL OPEN LOGS
#ifndef STATICLIB
  SLogFlushAll();
#endif

  // DETERMINE THE NAME OF THE APPLICATION
  char appfilename[MAX_PATH] = "";
  char appname[MAX_PATH]  = "";
  {
    GetModuleFileName((HMODULE)0,appfilename,MAX_PATH);
    WIN32_FIND_DATA finddata;
    ZeroMemory(&finddata,sizeof(WIN32_FIND_DATA));
    HANDLE findhandle = FindFirstFile(appfilename,&finddata);
    if (findhandle)
      FindClose(findhandle);
    SStrCopy(appname,finddata.cFileName,MAX_PATH);
    if (SStrChr(appname,'.',TRUE))
      *SStrChr(appname,'.',TRUE) = 0;
  }

  // GET THE ERROR STRING
  char errorstr[256] = "";
  SErrGetErrorStr(errorcode,
                  errorstr,
                  256);
  if (!errorstr[0])
    wsprintf(errorstr,GetString(STR_ERROR),errorcode & 0xFFFF,errorcode);

  // UNDECORATE THE HANDLE OR OBJECT NAME IF APPLICABLE
  char   localnamebuffer[256];
  LPCSTR localnameptr = filename;
  if (localnameptr && *localnameptr &&
      ((linenumber == SERR_LINECODE_OBJECT) ||
       (linenumber == SERR_LINECODE_HANDLE)))
    if (UndecorateObjectName(localnameptr,
                             localnamebuffer,
                             256))
      localnameptr = localnamebuffer;

  // LOG THE ERROR TO THE ACTIVE DEBUGGER IF APPLICABLE
  char  outstr[1024];
  LPSTR curr = outstr;
  curr += SStrCopy(curr,localnameptr);
  if (linenumber > 0) {
    wsprintf(curr,"(%u)",linenumber);
    curr += SStrLen(curr);
  }
  wsprintf(curr," : error %u: ",errorcode & 0xFFFF);
  curr += SStrLen(curr);
  SStrCopy(curr,errorstr);
  OutputDebugString(outstr);

  // BUILD THE FULL ERROR MESSAGE
  curr = outstr;
  wsprintf(curr,GetString(STR_HEADER),errorstr);
  curr += SStrLen(curr);
  wsprintf(curr,GetString(STR_PROGRAM),appfilename);
  curr += SStrLen(curr);
  if (localnameptr && *localnameptr) {
    switch (linenumber) {

      case SERR_LINECODE_FUNCTION:
        wsprintf(curr,GetString(STR_FUNCTION),localnameptr);
      break;

      case SERR_LINECODE_OBJECT:
        wsprintf(curr,GetString(STR_OBJECT),localnameptr);
      break;

      case SERR_LINECODE_HANDLE:
        wsprintf(curr,GetString(STR_HANDLE),localnameptr);
      break;

      case SERR_LINECODE_FILE:
        wsprintf(curr,GetString(STR_FILE),localnameptr);
      break;

      default:
        wsprintf(curr,GetString(STR_FILELINE),localnameptr,linenumber);
      break;

    }
    curr += SStrLen(curr);
  }
  if (errorcode == STORM_ERROR_ASSERTION)
    wsprintf(curr,GetString(STR_EXPRESSION),description ? description : "");
  else
    wsprintf(curr,GetString(STR_DESCRIPTION),description ? description : "");
  curr += SStrLen(curr);
  if (recoverable)
    SStrCopy(curr,GetString(STR_RECOVERABLE));
  else
    SStrCopy(curr,GetString(STR_TERMINATE));

  // DISPLAY THE MESSAGE BOX
  UINT buttons = recoverable ? MB_YESNOCANCEL
                             : MB_OKCANCEL;
  UINT flags   = MB_ICONSTOP
                   | MB_SETFOREGROUND
                   | MB_TASKMODAL
                   | MB_TOPMOST
                   | buttons;
#ifdef STATICLIB
  int result = MessageBox((HWND)0,outstr,appname,flags);
#else
  int result = SDrawMessageBox(outstr,appname,flags);
#endif

  // LEAVE THE CRITICAL SECTION
  InternalLeaveCriticalSection();

  // IF THE USER WANTS TO TRY TO RECOVER FROM THE ERROR THEN RETURN
  // CONTROL TO THE APPLICATION
  if (recoverable && (result == IDNO))
    return TRUE;

  // IF THE USER CLICKED CANCEL, GENERATE A DEBUG BREAK
  if (result == IDCANCEL)
#ifdef _X86_
    __asm int 3;
#else
    DebugBreak();
#endif

  // OTHERWISE, ATTEMPT TO STOP THE PROCESS.  WE USE TERMINATEPROCESS()
  // RATHER THAN EXITPROCESS() TO PREVENT STORM FROM GETTING A
  // DLL_PROCESS_DETACH NOTIFICATION AND POTENTIALLY RECURSING INTO ITS
  // CLEANUP FUNCTIONS.  IF WE ARE RUNNING ON A WIN95/98 SYSTEM AND THE
  // PROCESS IS ALREADY BEING TERMINATED, THEN CALLING TERMINATEPROCESS()
  // AGAIN WILL CAUSE A KERNEL EXCEPTION.  IN THIS CASE, WE JUST SUPPRESS
  // ALL ERROR MESSAGES AND LET THE CLEANUP PROCESS CONTINUE TO ITS
  // CONCLUSION.
  SErrSuppressErrors(TRUE);
  {
    DWORD terminationcode;
    if ((!(GetVersion() & 0x80000000)) ||
        (!GetExitCodeProcess(GetCurrentProcess(),&terminationcode)) ||
        (terminationcode == STILL_ACTIVE))
      TerminateProcess(GetCurrentProcess(),exitcode);
  }

  return FALSE;
}

//===========================================================================
BOOL APIENTRY SErrGetErrorStr (DWORD  errorcode,
                               LPTSTR buffer,
                               DWORD  bufferchars) {
  VALIDATEBEGIN;
  VALIDATE(buffer);
  VALIDATE(bufferchars);
  VALIDATEEND;

  // ENTER THE CRITICAL SECTION
  InternalEnterCriticalSection();

  // IF WE HAVEN'T ADDED OUR OWN MESSAGE FACILITIES TO THE MODULE LIST,
  // DO SO NOW
  if (!s_msgsrcinit) {
    s_msgsrcinit = TRUE;
    AddStormMessages();
  }

  // EXTRACT THE ERROR'S FACILITY CODE, AND LOOK UP THE MODULE CONTAINING
  // ERROR STRINGS FOR THAT FACILITY
  WORD    facility = (WORD)((errorcode >> 16) & 0xFFF);
  HMODULE module   = (HMODULE)0;
  {
    MSGSRCPTR curr = s_msgsrchead;
    while (curr && (curr->facility != facility))
      curr = curr->next;
    if (curr)
      module = curr->module;
  }

  // LEAVE THE CRITICAL SECTION
  InternalLeaveCriticalSection();

  // EXTRACT AND RETURN THE ERROR STRING
  *buffer = 0;
  return (FormatMessage(module
                          ? FORMAT_MESSAGE_FROM_HMODULE
                          : FORMAT_MESSAGE_FROM_SYSTEM,
                        module,
                        errorcode,
                        MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
                        buffer,
                        bufferchars,
                        NULL) != 0);

}

//===========================================================================
DWORD APIENTRY SErrGetLastError () {
//#ifdef _MAC
  return s_lasterror;
//#else
//  return GetLastError();
//#endif
}

//===========================================================================
BOOL APIENTRY SErrRegisterMessageSource (WORD    facility,
                                         HMODULE module,
                                         LPVOID  reserved) {
  InternalEnterCriticalSection();
  MSGSRCPTR ptr = (MSGSRCPTR)HeapAlloc(GetProcessHeap(),
                                       HEAP_GENERATE_EXCEPTIONS,
                                       sizeof(MSGSRC));
  ptr->facility = facility;
  ptr->module   = module;
  ptr->next     = s_msgsrchead;
  s_msgsrchead  = ptr;
  InternalLeaveCriticalSection();
  return TRUE;
}

//===========================================================================
void APIENTRY SErrReportResourceLeak (LPCTSTR handlename) {

  // DETERMINE WHETHER MEMORY TRACKING IS ENABLED
#ifndef _DEBUG
  static BOOL checked   = FALSE;
  static BOOL debugmode = FALSE;
  if (!checked) {
    checked = TRUE;
    SRegLoadValue("Internal","Debug Memory",0,(LPDWORD)&debugmode);
  }
  if (!debugmode)
    return;
#endif

  // DISPLAY THE ERROR
  SErrDisplayError(STORM_ERROR_HANDLE_NEVER_RELEASED,
                   handlename,
                   SERR_LINECODE_HANDLE,
                   NULL,
                   TRUE);

}

//===========================================================================
void APIENTRY SErrSetLastError (DWORD errorcode) {
//#ifdef _MAC
  s_lasterror = errorcode;
//#else
//  SetLastError(errorcode);
//#endif
}

//===========================================================================
void APIENTRY SErrSuppressErrors (BOOL suppress) {
  s_suppress = suppress;
}


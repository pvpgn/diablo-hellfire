/****************************************************************************
*
*  SMSG.CPP
*  Storm message processing and dispatching functions
*
*  By Michael O'Brien (3/5/96)
*
***/

#include "pch.h"
#pragma  hdrstop

#define  REGISTERTYPE_BASE     'SMSG'
#define  REGISTERTYPE_MESSAGE  (REGISTERTYPE_BASE+0)
#define  REGISTERTYPE_COMMAND  (REGISTERTYPE_BASE+1)
#define  REGISTERTYPE_KEYDOWN  (REGISTERTYPE_BASE+2)
#define  REGISTERTYPE_KEYUP    (REGISTERTYPE_BASE+3)

NODEDECL(WNDREC) {
    HWND window;
} *WNDPTR;

static LIST(WNDREC) s_wndlist;

//===========================================================================
static void AddWindow (HWND window) {
  s_wndlist.NewNode()->window = window;
}

//===========================================================================
static WNDPTR FindWindow (HWND window) {
  ITERATELIST(WNDREC,s_wndlist,curr)
    if (curr->window == window)
      return curr;
  return NULL;
}

//===========================================================================
static void DeleteWindow (HWND window) {
  WNDPTR ptr = FindWindow(window);
  if (ptr) {
    SEvtUnregisterType(REGISTERTYPE_MESSAGE,(DWORD)window);
    SEvtUnregisterType(REGISTERTYPE_COMMAND,(DWORD)window);
    SEvtUnregisterType(REGISTERTYPE_KEYUP  ,(DWORD)window);
    SEvtUnregisterType(REGISTERTYPE_KEYDOWN,(DWORD)window);
    s_wndlist.DeleteNode(ptr);
  }
}

//===========================================================================
static BOOL InternalRegister (DWORD       type,
                              HWND        window,
                              DWORD       id,
                              SMSGHANDLER handler) {
  VALIDATEBEGIN;
  VALIDATE(handler);
  VALIDATEEND;

  // FIND THE WINDOW
  if (!window)
    SDrawGetFrameWindow(&window);
  if (!FindWindow(window))
    AddWindow(window);

  // REGISTER THE MESSAGE
  return SEvtRegisterHandler(type,(DWORD)window,id,0,(SEVTHANDLER)handler);
}

//===========================================================================
static BOOL InternalUnregister (DWORD       type,
                                HWND        window,
                                DWORD       id,
                                SMSGHANDLER handler) {
  VALIDATEBEGIN;
  VALIDATE(handler);
  VALIDATEEND;

  // FIND THE WINDOW
  if (!window)
    SDrawGetFrameWindow(&window);
  if (!FindWindow(window))
    AddWindow(window);

  // UNREGISTER THE MESSAGE
  return SEvtUnregisterHandler(type,(DWORD)window,id,(SEVTHANDLER)handler);
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
BOOL APIENTRY SMsgBreakHandlerChain (LPPARAMS params) {
  return SEvtBreakHandlerChain(params);
}

//===========================================================================
BOOL APIENTRY SMsgDestroy () {
  while (!s_wndlist.IsEmpty())
    DeleteWindow(s_wndlist.Head()->window);
  return 1;
}

//===========================================================================
BOOL APIENTRY SMsgDispatchMessage (HWND     window,
                                   UINT     message,
                                   WPARAM   wparam,
                                   LPARAM   lparam,
                                   BOOL    *useresult,
                                   LRESULT *result) {
  if (useresult)
    *useresult = 0;
  if (result)
    *result = 0;

  // CREATE A PARAMETER RECORD TO PASS TO THE HANDLER FUNCTION
  PARAMS params;
  params.window     = window;
  params.message    = message;
  params.wparam     = wparam;
  params.lparam     = lparam;
  params.notifycode = (message == WM_COMMAND) ? HIWORD(wparam) : 0;
  params.useresult  = 0;
  params.result     = 0;

  // DISPATCH THE MESSAGE
  SEvtDispatch(REGISTERTYPE_MESSAGE,(DWORD)window,message,&params);
  if (message == WM_COMMAND)
    SEvtDispatch(REGISTERTYPE_COMMAND,(DWORD)window,LOWORD(wparam),&params);
  else if (message == WM_KEYDOWN)
    SEvtDispatch(REGISTERTYPE_KEYDOWN,(DWORD)window,wparam,&params);
  else if (message == WM_KEYUP)
    SEvtDispatch(REGISTERTYPE_KEYUP,(DWORD)window,wparam,&params);

  // IF THIS MESSAGE IS WM_NCDESTROY, DEALLOCATE THIS WINDOW'S RECORD
  if (message == WM_NCDESTROY)
    DeleteWindow(window);

  // RETURN THE DISPATCHING RESULTS
  if (useresult)
    *useresult = params.useresult;
  if (result)
    *result = params.result;

  return 1;
}

//===========================================================================
BOOL APIENTRY SMsgDoMessageLoop (SMSGIDLEPROC idleproc,
                                 BOOL         cleanuponquit) {
  DWORD count = 0;
  MSG   message;
  for (;;)
    if (PeekMessage(&message,(HWND)0,0,0,PM_NOREMOVE) ||
        !(idleproc && idleproc(count++))) {
      count = 0;
      if (!GetMessage(&message,(HWND)0,0,0))
        break;
      TranslateMessage(&message);
      DispatchMessage(&message);
    }
#ifndef STATICLIB
  if (cleanuponquit)
    StormDestroy();
#endif
  return message.wParam;
}

//===========================================================================
BOOL APIENTRY SMsgPopRegisterState (HWND window) {
  if (!window)
    SDrawGetFrameWindow(&window);
  SEvtPopState(REGISTERTYPE_COMMAND,(DWORD)window);
  SEvtPopState(REGISTERTYPE_KEYDOWN,(DWORD)window);
  SEvtPopState(REGISTERTYPE_KEYUP  ,(DWORD)window);
  SEvtPopState(REGISTERTYPE_MESSAGE,(DWORD)window);
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SMsgPushRegisterState (HWND window) {
  if (!window)
    SDrawGetFrameWindow(&window);
  SEvtPushState(REGISTERTYPE_COMMAND,(DWORD)window);
  SEvtPushState(REGISTERTYPE_KEYDOWN,(DWORD)window);
  SEvtPushState(REGISTERTYPE_KEYUP  ,(DWORD)window);
  SEvtPushState(REGISTERTYPE_MESSAGE,(DWORD)window);
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SMsgRegisterCommand (HWND        window,
                                   UINT        id,
                                   SMSGHANDLER handler) {
  return InternalRegister(REGISTERTYPE_COMMAND,window,id,handler);
}

//===========================================================================
BOOL APIENTRY SMsgRegisterKeyDown (HWND        window,
                                   UINT        id,
                                   SMSGHANDLER handler) {
  return InternalRegister(REGISTERTYPE_KEYDOWN,window,id,handler);
}

//===========================================================================
BOOL APIENTRY SMsgRegisterKeyUp (HWND        window,
                                 UINT        id,
                                 SMSGHANDLER handler) {
  return InternalRegister(REGISTERTYPE_KEYUP,window,id,handler);
}

//===========================================================================
BOOL APIENTRY SMsgRegisterMessage (HWND        window,
                                   UINT        id,
                                   SMSGHANDLER handler) {
  return InternalRegister(REGISTERTYPE_MESSAGE,window,id,handler);
}

//===========================================================================
BOOL APIENTRY SMsgUnregisterCommand (HWND        window,
                                     UINT        id,
                                     SMSGHANDLER handler) {
  return InternalUnregister(REGISTERTYPE_COMMAND,window,id,handler);
}

//===========================================================================
BOOL APIENTRY SMsgUnregisterKeyDown (HWND        window,
                                     UINT        id,
                                     SMSGHANDLER handler) {
  return InternalUnregister(REGISTERTYPE_KEYDOWN,window,id,handler);
}

//===========================================================================
BOOL APIENTRY SMsgUnregisterKeyUp (HWND        window,
                                   UINT        id,
                                   SMSGHANDLER handler) {
  return InternalUnregister(REGISTERTYPE_KEYUP,window,id,handler);
}

//===========================================================================
BOOL APIENTRY SMsgUnregisterMessage (HWND        window,
                                     UINT        id,
                                     SMSGHANDLER handler) {
  return InternalUnregister(REGISTERTYPE_MESSAGE,window,id,handler);
}

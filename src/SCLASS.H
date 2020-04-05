#ifndef _STORM_SCLASS_H_
#define _STORM_SCLASS_H_

extern "C++" {

/****************************************************************************
*
*  CCRITSECT -- Critical section class
*
***/

class CCritSect {
  private:
    CRITICAL_SECTION m_critsect;
  public:
    CCritSect () { InitializeCriticalSection(&m_critsect); }
    ~CCritSect () { DeleteCriticalSection(&m_critsect); }
    void inline Enter () { EnterCriticalSection(&m_critsect); }
    void inline Leave () { LeaveCriticalSection(&m_critsect); }
};

/****************************************************************************
*
*  CLOCK -- Lock class
*
***/

class CLock {
  private:
    SSYNCLOCK m_lock;
  public:
    CLock () { SSyncInitializeLock(&m_lock); }
    ~CLock () { SSyncDeleteLock(&m_lock); }
    void inline Enter (BOOL forwriting) { SSyncEnterLock(&m_lock,forwriting); }
    void inline Leave (BOOL fromwriting) { SSyncLeaveLock(&m_lock,fromwriting); }
};

/****************************************************************************
*
*  TLIST -- Linked list function templates
*
***/

//===========================================================================
template <class T>
BOOL inline TListAdd (T **head, T *rec, LPCSTR filename = NULL, int linenumber = 0) {
  if (!(head && rec))
    return 0;

  T *newptr = (T *)SMemAlloc(sizeof(T),filename,linenumber);
  if (!newptr)
    return 0;
  CopyMemory(newptr,rec,sizeof(T));
  newptr->next = *head;
  *head        = newptr;
  return 1;
}
#define  LISTADD(a,b)  TListAdd(a,b,(LPCSTR)__FILE__,__LINE__)

//===========================================================================
template <class T>
BOOL inline TListAddEnd (T **head, T *rec, LPCSTR filename = NULL, int linenumber = 0) {
  if (!(head && rec))
    return 0;

  T *newptr = (T *)SMemAlloc(sizeof(T),filename,linenumber);
  if (!newptr)
    return 0;
  CopyMemory(newptr,rec,sizeof(T));
  newptr->next = NULL;

  T **next = head;
  while (*next)
    next = &(*next)->next;
  *next = newptr;

  return 1;
}
#define  LISTADDEND(a,b)  TListAddEnd(a,b,(LPCSTR)__FILE__,__LINE__)

//===========================================================================
template <class T>
BOOL inline TListAddPtr (T **head, T *ptr) {
  if (!(head && ptr))
    return 0;

  ptr->next = *head;
  *head     = ptr;
  return 1;
}
#define  LISTADDPTR(a,b)  TListAddPtr(a,b)

//===========================================================================
template <class T>
BOOL inline TListAddPtrEnd (T **head, T *ptr) {
  if (!(head && ptr))
    return 0;

  ptr->next = NULL;
  T **next = head;
  while (*next)
    next = &(*next)->next;
  *next = ptr;

  return 1;
}
#define  LISTADDPTREND(a,b)  TListAddPtrEnd(a,b)

//===========================================================================
template <class T>
BOOL inline TListClear (T **head, LPCSTR filename = NULL, int linenumber = 0) {
  if (!head)
    return 0;

  while (*head) {
    T *next = (*head)->next;
    SMemFree(*head,filename,linenumber);
    *head = next;
  }
  return 1;
}
#define  LISTCLEAR(a)  TListClear(a,(LPCSTR)__FILE__,__LINE__)

//===========================================================================
template <class T>
BOOL inline TListFree (T **head, T *ptr, LPCSTR filename = NULL, int linenumber = 0) {
  if (!(head && ptr))
    return 0;

  T **next = head;
  while (*next && (*next != ptr))
    next = &(*next)->next;
  if (*next)
    *next = (*next)->next;

  SMemFree(ptr,filename,linenumber);
  return (*next != NULL);
}
#define  LISTFREE(a,b)  TListFree(a,b,(LPCSTR)__FILE__,__LINE__)

//===========================================================================
template <class T>
BOOL inline TListFreePtr (T **head, T *ptr, LPCSTR filename = NULL, int linenumber = 0) {
  if (!(head && ptr))
    return 0;

  T **next = head;
  while (*next && (*next != ptr))
    next = &(*next)->next;
  if (*next)
    *next = (*next)->next;

  return (*next != NULL);
}
#define  LISTFREEPTR(a,b)  TListFreePtr(a,b,(LPCSTR)__FILE__,__LINE__)

}      // extern "C++"
#endif // ifndef _STORM_SCLASS_H_

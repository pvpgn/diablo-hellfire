/****************************************************************************
*
*  SMEM.CPP
*  Storm memory manager
*
*  By Michael O'Brien (3/18/97)
*
*  This module cannot use constructors or destructors, because it is called
*  by the runtime library startup code prior to construction and after
*  destruction.
*
*  The allocation functions implemented in this module are guaranteed not
*  to return NULL.  Storm always displays a fatal error if an allocation
*  can not succeed, so that the application does not have to have failure
*  code paths for each allocation.  However, Storm does not display errors
*  for failures to free memory unless debug mode is enabled.
*
***/

#include "pch.h"
#pragma  hdrstop

#define  FIRSTUSERHEAP     0x80000000  // must be a power of two
#define  MAXALLOCSIZE      (0xFFFF-(sizeof(HEAP)+MAX_PATH+sizeof(BLOCK)+2*sizeof(DWORD)))
#define  MAXFREEMAINT      4
#define  MAXHEAPSIZE       0x7FFFFFFF
#define  MINBLOCKSIZE      (sizeof(BLOCK)+2*sizeof(DWORD))
#define  PAGESIZE          0x1000      // must be a power of two
#define  RESERVESIZE       0x10000
#define  SIGNATURE1        0x6F6D
#define  SIGNATURE2        0xB112
#define  TABLESIZE         256         // must be a power of two

#define  REGKEY            "Internal"
#define  REGVAL_DEBUG      "Debug Memory"
#define  REGVAL_GUARD      "Protect Memory"
#define  REGVAL_TRACEFILE  "SMem Trace File"

#define  BF_BOUNDINGSIG    0x01
#define  BF_FREEBLOCK      0x02
#define  BF_LARGEALLOC     0x04
#define  BF_OUTSIDEHEAP    0x08
#define  BF_PRESERVE       0x80

typedef struct _BLOCK {
    WORD bytes;
    BYTE padding;
    BYTE flags;
    WORD heapaddr;
    WORD signature1;
} BLOCK, *BLOCKPTR;

typedef struct _FASTBLOCK {
    WORD  bytes;
    BYTE  padding;
    BYTE  flags;
    DWORD addrsig;
} FASTBLOCK, *FASTBLOCKPTR;

typedef struct _FREEBLOCK {
    WORD        bytes;
    BYTE        padding;
    BYTE        flags;
    _FREEBLOCK *next;
} FREEBLOCK, *FREEBLOCKPTR;

typedef struct _HEAP {
    _HEAP        *next;
    HSHEAP        handle;
    DWORD         slot;
    DWORD         addrsig;
    BOOL          active;
    DWORD         allocatedblocks;
    BLOCKPTR      firstblock;
    BLOCKPTR      termblock;
    FREEBLOCKPTR  firstfreeblock;
    DWORD         maintainfreelist;
    DWORD         chunksize;
    DWORD         committedbytes;
    DWORD         reservedbytes;
    int           linenumber;
    char          filename[1];
} HEAP, *HEAPPTR;

DECLARE_STRICT_HANDLE(HLOCKEDHEAP);

static BOOL             s_emptyheap[TABLESIZE];
static CRITICAL_SECTION s_critsect[TABLESIZE];
static BOOL             s_debugmode;
static BOOL             s_guardmode;
static HEAPPTR          s_heaphead[TABLESIZE];
static BOOL             s_initialized;
static HEAPPTR          s_lastemptyheap;
static DWORD            s_pagesize;

/****************************************************************************
*
*  TRACING FUNCTIONS
*
***/

#ifdef  _DEBUG

static CSLog s_log(REGKEY,REGVAL_TRACEFILE);

//===========================================================================
static inline void Trace (LPVOID  ptr,
                          LPCTSTR funcname,
                          LPCTSTR filename,
                          int     linenumber) {
  if (!s_log.GetHandle())
    return;
  SLogWrite(s_log.GetHandle(),
            "[0x%08x]  %-20s  %s (%d)",
            ptr,
            funcname,
            filename ? filename : "",
            linenumber);
}

#define  TRACE  Trace

#else

#define  TRACE

#endif

/****************************************************************************
*
*  SYNCHRONIZATION AND CONVERSION FUNCTIONS
*
***/

//===========================================================================
static inline BOOL CheckInitialized () {
#ifdef STATICLIB
  if (!s_initialized)
    SMemInitialize();
#endif
  return s_initialized;
}

//===========================================================================
static void FatalError (DWORD  errorcode,
                        LPCSTR filename,
                        int    linenumber) {
  SErrDisplayError(errorcode,
                   filename,
                   linenumber,
                   NULL,
                   FALSE);
  ExitProcess(1);
}

//===========================================================================
static inline BLOCKPTR GetBlockPtrByPtr (LPVOID ptr) {
  if (!ptr)
    return NULL;
  BLOCKPTR blockptr = (BLOCKPTR)ptr-1;
  if (blockptr->flags & BF_OUTSIDEHEAP)
    blockptr = *(BLOCKPTR *)((LPBYTE)blockptr-sizeof(BLOCKPTR));
  return blockptr;
}

//===========================================================================
static inline HSHEAP GetHandleByBlockPtr (BLOCKPTR blockptr) {
  HEAPPTR heapptr = (HEAPPTR)((DWORD)(blockptr->heapaddr) << 16);
  return heapptr->handle;
}

//===========================================================================
static inline HSHEAP GetHandleByCaller (LPCSTR filename,
                                        int    linenumber) {
  static BOOL   cacheenabled = TRUE;
  static DWORD  lastchars    = 0;
  static int    lastline     = 0;
  static LPCSTR lastptr      = NULL;
  static HSHEAP lasthandle   = (HSHEAP)0;

  // IF CACHING IS ENABLED AND THIS CALLER MATCHES THE PREVIOUS ONE,
  // RETURN THE PREVIOUSLY COMPUTED HANDLE
  DWORD filenamechars = *(LPDWORD)filename;
  if (cacheenabled &&
      (filename   == lastptr) &&
      (linenumber == lastline)) {

    // VERIFY THAT THE CALLER IS NOT JUST CHANGING FILENAMES WITHIN A
    // STATIC BUFFER BY CHECKING THE FIRST FOUR CHARACTERS.  IF THEY
    // ARE DIFFERENT FROM WHAT WE EXPECT, DISABLE CACHING.
    if (filenamechars != lastchars)
      cacheenabled = FALSE;
    else
      return lasthandle;

  }

  // OTHERWISE, COMPUTE THE HANDLE FOR THIS CALLER
  DWORD  hashval = SStrHash(filename,
                            TRUE,
                            (DWORD)linenumber);
  HSHEAP handle  = (HSHEAP)(hashval & (FIRSTUSERHEAP-1));
  if (!handle)
    handle = (HSHEAP)1;

  // SAVE IT IN THE CACHE
  lastchars  = filenamechars;
  lastptr    = filename;
  lastline   = linenumber;
  lasthandle = handle;

  return handle;
}

//===========================================================================
static inline LPVOID GetPtrByBlockPtr (BLOCKPTR blockptr) {
  if (!blockptr)
    return NULL;
  LPVOID ptr = blockptr+1;
  if (blockptr->flags & BF_LARGEALLOC)
    ptr = *(LPVOID *)ptr;
  return ptr;
}

//===========================================================================
static inline DWORD GetSlotByHandle (HSHEAP handle) {
  return (DWORD)handle & (TABLESIZE-1);
}

//===========================================================================
static inline HEAPPTR LockHeapByBlockPtr (BLOCKPTR     blockptr,
                                          HLOCKEDHEAP *lockedhandle) {

  // CONVERT THE COMPACT FORM OF THE HEAP ADDRESS TO A FULL ADDRESS
  HEAPPTR heapptr = (HEAPPTR)((DWORD)(blockptr->heapaddr) << 16);

  // LOCK THE HEAP'S CRITICAL SECTION
  EnterCriticalSection(&s_critsect[heapptr->slot]);
  *lockedhandle = (HLOCKEDHEAP)heapptr->slot;

  return heapptr;
}

//===========================================================================
static inline HEAPPTR LockHeapByHandle (HSHEAP       handle,
                                        HLOCKEDHEAP *lockedhandle,
                                        BOOL         heapmustexist) {

  // LOCK THE HEAP'S CRITICAL SECTION
  DWORD slot = GetSlotByHandle(handle);
  EnterCriticalSection(&s_critsect[slot]);
  *lockedhandle = (HLOCKEDHEAP)slot;

  // FIND AND RETURN THE HEAP HEADER
  HEAPPTR heapptr = s_heaphead[slot];
  while (heapptr)
    if (heapptr->handle == handle)
      return heapptr;
    else
      heapptr = heapptr->next;

  // IF WE DIDN'T FIND THE HEAP AND THE CALLER REQUIRES THAT THE HEAP
  // EXIST, UNLOCK THE CRITICAL SECTION
  if (heapmustexist) {
    LeaveCriticalSection(&s_critsect[slot]);
    *lockedhandle = (HLOCKEDHEAP)INVALID_HANDLE_VALUE;
  }

  return NULL;
}

//===========================================================================
static void Warning (DWORD  errorcode,
                     LPCSTR filename,
                     int    linenumber) {
  SErrSetLastError(errorcode);
  if (s_debugmode)
    SErrDisplayError(errorcode,
                     filename,
                     linenumber,
                     NULL,
                     TRUE);
}

//===========================================================================
static inline void UnlockHeap (HLOCKEDHEAP lockedhandle) {
  DWORD slot = (DWORD)lockedhandle;
  LeaveCriticalSection(&s_critsect[slot]);
}

/****************************************************************************
*
*  BLOCK ALLOCATION/DEALLOCATION FUNCTIONS
*
***/

static void CombineFreeBlocks (HEAPPTR heapptr);
static void ComputePageSize ();
static void FreeHeap (HEAPPTR *nextptr);
static BOOL FreeHeapBlock (HEAPPTR  heapptr,
                           BLOCKPTR block);

//===========================================================================
static HEAPPTR AllocateHeap (LPCSTR filename,
                             int    linenumber,
                             HSHEAP handle,
                             DWORD  slot,
                             DWORD  chunksize,
                             DWORD  commitsize,
                             DWORD  reservesize) {

  // RESERVE MEMORY FOR THE NEW HEAP
  HEAPPTR newheap = (HEAPPTR)VirtualAlloc(NULL,
                                          reservesize,
                                          MEM_RESERVE,
                                          PAGE_NOACCESS);

  if (!newheap)
    FatalError(ERROR_NOT_ENOUGH_MEMORY,
               filename,
               linenumber);
  if (!VirtualAlloc(newheap,
                    commitsize,
                    MEM_COMMIT,
                    PAGE_READWRITE))
    FatalError(ERROR_NOT_ENOUGH_MEMORY,
               filename,
               linenumber);

  // DETERMINE THE SIZE OF THE NEW HEAP HEADER
  DWORD filenamebytes = (filename ? SStrLen(filename) : 0)+1;
  DWORD headerbytes   = sizeof(HEAP)+filenamebytes-1;
  if (headerbytes & 3)
    headerbytes += 4-(headerbytes & 3);

  // FILL IN THE NEW HEAP HEADER
  newheap->handle           = handle;
  newheap->next             = s_heaphead[slot];
  newheap->slot             = slot;
  newheap->active           = TRUE;
  newheap->firstblock       = (BLOCKPTR)((LPBYTE)newheap+headerbytes);
  newheap->termblock        = (BLOCKPTR)((LPBYTE)newheap+headerbytes);
  newheap->firstfreeblock   = NULL;
  newheap->maintainfreelist = MAXFREEMAINT;
  newheap->chunksize        = chunksize;
  newheap->committedbytes   = commitsize;
  newheap->reservedbytes    = reservesize;
  newheap->linenumber       = linenumber;

  // FILL IN THE HEAP'S FILENAME
  if (filename)
    CopyMemory(newheap->filename,filename,filenamebytes);
  else
    newheap->filename[0] = 0;

  // FILL IN THE HEAP'S ADDRESS AND SIGNATURE OPTIMIZED DWORD
  {
    BLOCK block;
    block.heapaddr   = (WORD)((DWORD)newheap >> 16);
    block.signature1 = SIGNATURE1;
    FASTBLOCKPTR fastblockptr = (FASTBLOCKPTR)&block;
    newheap->addrsig = fastblockptr->addrsig;
  }

  // ADD THE HEAP TO THE LIST OF HEAPS
  s_heaphead[slot] = newheap;

  return newheap;
}

//===========================================================================
static LPVOID AllocateHeapBlock (HEAPPTR heapptr,
                                 DWORD   bytes,
                                 BYTE    baseflags) {

  // DETERMINE THE BLOCK SIZE REQUIRED TO SATISFY THIS ALLOCATION REQUEST
  BOOL  largealloc  = s_guardmode || (bytes > MAXALLOCSIZE);
  BOOL  boundingsig = s_debugmode && !largealloc;
  DWORD reqblocksize;
  {
    DWORD userbytes = largealloc ? sizeof(LPVOID) : bytes;
    DWORD overhead  = sizeof(BLOCK)+(boundingsig ? sizeof(WORD) : 0);
    reqblocksize = userbytes+overhead;
  }
  DWORD blocksize = reqblocksize;
  if (blocksize & 7)
    blocksize += 8-(blocksize & 7);

  // REBUILD THIS HEAP'S FREE LIST IF NECESSARY
  if (heapptr->firstfreeblock &&
      !heapptr->maintainfreelist)
    CombineFreeBlocks(heapptr);
  heapptr->maintainfreelist = MAXFREEMAINT;

  // SEARCH THIS HEAP FOR THE CLOSEST MATCHING FREE BLOCK WHICH IS
  // LARGE ENOUGH TO SATISFY THE REQUEST
  DWORD         bestdelta     = LONG_MAX;
  FREEBLOCKPTR *bestfreeblock = NULL;
  {
    FREEBLOCKPTR *nextfreeblock = &heapptr->firstfreeblock;
    while (*nextfreeblock) {
      DWORD delta = (*nextfreeblock)->bytes-blocksize;
      if (delta < bestdelta) {
        bestdelta     = delta;
        bestfreeblock = nextfreeblock;
        if (delta < MINBLOCKSIZE)
          break;
      }
      nextfreeblock = &(*nextfreeblock)->next;
    }
  }

  // IF WE FOUND A FREE BLOCK THAT CAN SATISFY THE REQUEST, SUBDIVIDE IT
  // AS NECESSARY AND USE IT
  BLOCKPTR newblock;
  if (bestfreeblock) {
    newblock = (BLOCKPTR)*bestfreeblock;
    if (bestdelta >= MINBLOCKSIZE) {
      FREEBLOCKPTR newfreeblock = (FREEBLOCKPTR)((LPBYTE)newblock+blocksize);
      newfreeblock->bytes   = (WORD)bestdelta;
      newfreeblock->padding = 0;
      newfreeblock->flags   = BF_FREEBLOCK;
      newfreeblock->next    = (*bestfreeblock)->next;
      newblock->bytes = (WORD)blocksize;
      *bestfreeblock = newfreeblock;
    }
    else
      *bestfreeblock = (*bestfreeblock)->next;
  }

  // OTHERWISE, ALLOCATE A NEW BLOCK ON THE END OF THE HEAP
  else {
    DWORD newheapsize = ((LPBYTE)heapptr->termblock-(LPBYTE)heapptr)+blocksize;

    // IF THIS NEW BLOCK WON'T FIT IN THE SPACE WE HAVE RESERVED FOR THIS
    // HEAP, CREATE A NEW HEAP AND SET IT AS THE ACTIVE HEAP
    if (newheapsize > heapptr->reservedbytes) {
      DWORD newreservesize = (heapptr->reservedbytes < 0x10000000)
                               ? heapptr->reservedbytes*2
                               : heapptr->reservedbytes;
      DWORD newchunksize   = newreservesize >> 3;
      HEAPPTR newheapptr = AllocateHeap(heapptr->filename,
                                        heapptr->linenumber,
                                        heapptr->handle,
                                        heapptr->slot,
                                        newchunksize,
                                        newchunksize,
                                        newreservesize);
      if (!newheapptr)
        return NULL;
      heapptr->active = FALSE;
      heapptr = newheapptr;
      newheapsize = ((LPBYTE)heapptr->termblock-(LPBYTE)heapptr)+blocksize;
    }

    // IF WE HAVEN'T YET COMMITTED THE MEMORY THAT WILL BE NEEDED FOR THIS
    // NEW BLOCK, DO SO NOW
    if (newheapsize > heapptr->committedbytes) {
      DWORD commitsize = newheapsize-heapptr->committedbytes;
      if (commitsize & (heapptr->chunksize-1))
        commitsize += heapptr->chunksize-(commitsize & (heapptr->chunksize-1));
      if (heapptr->committedbytes+commitsize > heapptr->reservedbytes)
        commitsize = heapptr->reservedbytes-heapptr->committedbytes;
      VirtualAlloc((LPBYTE)heapptr+heapptr->committedbytes,
                   commitsize,
                   MEM_COMMIT,
                   PAGE_READWRITE);
      heapptr->committedbytes += commitsize;
    }

    // DETERMINE THE LOCATION AND SIZE OF THE NEW BLOCK
    newblock = heapptr->termblock;
    newblock->bytes = (WORD)blocksize;

    // CREATE A NEW TERMINATOR BLOCK
    heapptr->termblock = (BLOCKPTR)((LPBYTE)newblock+blocksize);

  }

  // FILL IN THE NEW BLOCK'S HEADER
  newblock->padding = (BYTE)(newblock->bytes-reqblocksize);
  newblock->flags   = baseflags | (largealloc ? BF_LARGEALLOC : 0);
  ((FASTBLOCKPTR)newblock)->addrsig = heapptr->addrsig;
  ++heapptr->allocatedblocks;

  // IF REQUESTED, ADD A SECOND SIGNATURE TO BOUND THE BLOCK
  if (boundingsig) {
    newblock->flags |= BF_BOUNDINGSIG;
    *(LPWORD)((LPBYTE)newblock+reqblocksize-sizeof(WORD)) = SIGNATURE2;
  }

  // IF THIS IS A LARGE ALLOCATION, THEN ALLOCATE A BLOCK OF USER MEMORY
  // OUTSIDE THE HEAP, AND MAKE THE BLOCK INSIDE THE HEAP POINT TO THE
  // EXTERNAL BLOCK.  SAVE A POINTER TO THE USER PORTION OF THE EXTERNAL
  // BLOCK.
  LPVOID result;
  if (largealloc) {
    if (!s_pagesize)
      ComputePageSize();
    DWORD  largeallocbytes  = sizeof(BLOCKPTR)+sizeof(BLOCK)+bytes;
    DWORD  largeallocoffset = 0;

    // IF WE ARE IN DEBUG MODE, ALIGN THE ALLOCATION AT THE END OF A PAGE,
    // SO THAT IF THE APPLICATION OVERWRITES THE ALLOCATION IT WILL TRIGGER
    // AN EXCEPTION.  (HOWEVER, KEEP THE ALLOCATION ALIGNED ON A DWORD
    // BOUNDARY.)
    LPBYTE largeallocptr = NULL;
    if (s_debugmode || s_guardmode) {
      largeallocoffset = s_pagesize-(largeallocbytes & (s_pagesize-1));
      if (s_guardmode)
        largeallocoffset &= (s_pagesize-1);
      else
        largeallocoffset &= (s_pagesize-4);
      if (s_guardmode)
        largeallocptr = (LPBYTE)VirtualAlloc(NULL,
                                             largeallocbytes+largeallocoffset+4,
                                             MEM_RESERVE,
                                             PAGE_NOACCESS);
    }

    largeallocptr = (LPBYTE)VirtualAlloc(largeallocptr,
                                         largeallocbytes+largeallocoffset,
                                         MEM_COMMIT,
                                         PAGE_READWRITE);
    if (!largeallocptr) {
      FreeHeapBlock(heapptr,newblock);
      return NULL;
    }
    largeallocptr = (LPBYTE)largeallocptr+largeallocoffset;
    *(BLOCKPTR *)largeallocptr = newblock;
    BLOCKPTR largeallocblock = (BLOCKPTR)((LPBYTE)largeallocptr+sizeof(BLOCKPTR));
    largeallocblock->bytes      = (WORD)((bytes+0xFFFF) >> 16);
    largeallocblock->padding    = 0;
    largeallocblock->flags      = BF_LARGEALLOC | BF_OUTSIDEHEAP;
    ((FASTBLOCKPTR)largeallocblock)->addrsig = heapptr->addrsig;
    result = largeallocblock+1;
    *(LPVOID *)(newblock+1) = result;
  }

  // OTHERWISE, SAVE A POINTER TO THE USER PORTION OF THE HEAP BLOCK
  else
    result = newblock+1;

  return result;
}

//===========================================================================
static BOOL CheckValidBlock (LPVOID ptr,
                             BOOL   displayerror,
                             LPCSTR filename,
                             int    linenumber) {

  // VERIFY THAT THIS ISN'T A NULL POINTER
  if (!ptr) {
    if (displayerror)
      Warning(STORM_ERROR_MEMORY_NULL_POINTER,
              filename,
              linenumber);
    return FALSE;
  }

  // VERIFY THAT THIS IS A VALID HEAP BLOCK
  BLOCKPTR block = (BLOCKPTR)ptr-1;
  if (block->signature1 != SIGNATURE1) {
    if (displayerror)
      Warning(STORM_ERROR_MEMORY_INVALID_BLOCK,
              filename,
              linenumber);
    return FALSE;
  }

  // VERIFY THAT THIS BLOCK IS ALLOCATED
  if (block->flags & BF_FREEBLOCK) {
    if (displayerror)
      Warning(STORM_ERROR_MEMORY_ALREADY_FREED,
              filename,
              linenumber);
    return FALSE;
  }

  // IF THIS BLOCK HAS A BOUNDING SIGNATURE, VERIFY THAT IT IS INTACT
  if ((block->flags & BF_BOUNDINGSIG) &&
      (*(LPWORD)((LPBYTE)block+block->bytes-block->padding-sizeof(WORD)) != SIGNATURE2) &&
      displayerror)
    Warning(STORM_ERROR_MEMORY_CORRUPT,
            filename,
            linenumber);

  return TRUE;
}

//===========================================================================
static void CombineFreeBlocks (HEAPPTR heapptr) {

  // RESET THE LIST OF FREE BLOCKS
  FREEBLOCKPTR  prevfreeblock = NULL;
  FREEBLOCKPTR *nextfreeblock = &heapptr->firstfreeblock;

  // SEARCH THE ENTIRE HEAP FOR FREE BLOCKS
  for (BLOCKPTR blockptr = heapptr->firstblock;
       blockptr != heapptr->termblock;
       blockptr = (BLOCKPTR)((LPBYTE)blockptr+blockptr->bytes))
    if (blockptr->flags & BF_FREEBLOCK) {
      FREEBLOCKPTR freeblockptr = (FREEBLOCKPTR)blockptr;
      freeblockptr->next = NULL;

      // IF THIS FREE BLOCK IS ADJACENT TO THE PREVIOUS ONE, COMBINE THEM
      if (prevfreeblock &&
          (freeblockptr == (FREEBLOCKPTR)((LPBYTE)prevfreeblock+prevfreeblock->bytes)) &&
          ((DWORD)freeblockptr->bytes+(DWORD)prevfreeblock->bytes <= 0xFFFF))
        prevfreeblock->bytes += freeblockptr->bytes;

      // OTHERWISE, ADD THIS FREE BLOCK TO THE LIST
      else {
        *nextfreeblock = freeblockptr;
        nextfreeblock = &freeblockptr->next;
        prevfreeblock = freeblockptr;
      }

    }

  // TERMINATE THE LIST OF FREE BLOCKS
  *nextfreeblock = NULL;

}

//===========================================================================
static void ComputePageSize () {

  // GET THE SYSTEM'S PAGE SIZE
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);

  // FORCE THE PAGE SIZE TO BE A POWER OF TWO (JUST IN CASE IT ISN'T ALREADY)
  s_pagesize = 1;
  while (s_pagesize < sysinfo.dwPageSize)
    s_pagesize <<= 1;

}

//===========================================================================
static HEAPPTR * DestroyHeap (HEAPPTR *nextptr) {
  BOOL preserve = FALSE;

  // IF THERE ARE ANY ALLOCATED BLOCKS IN THIS HEAP WHICH AREN'T MARKED
  // PRESERVE-ON-DESTROY, DISPLAY A WARNING AND DELETE THEM
  BLOCKPTR blockptr = (*nextptr)->firstblock;
  while (blockptr != (*nextptr)->termblock)
    if (blockptr->flags & (BF_FREEBLOCK | BF_PRESERVE)) {
      preserve |= (blockptr->flags & BF_PRESERVE);
      blockptr  = (BLOCKPTR)((LPBYTE)blockptr+blockptr->bytes);
    }
    else {
      Warning(STORM_ERROR_MEMORY_NEVER_RELEASED,
              (*nextptr)->filename,
              (*nextptr)->linenumber);
      FreeHeapBlock(*nextptr,blockptr);
      blockptr = (*nextptr)->firstblock;
    }

  // IF THERE WERE NO PRESERVE-ON-DESTROY BLOCKS, FREE THE HEAP
  if (!preserve) {
    FreeHeap(nextptr);
    return nextptr;
  }
  else
    return &(*nextptr)->next;

}

//===========================================================================
static void FreeEmptyHeaps () {
  s_lastemptyheap = NULL;
  for (DWORD slot = 0; slot < TABLESIZE; ++slot)
    if (s_emptyheap[slot]) {
      EnterCriticalSection(&s_critsect[slot]);
      s_emptyheap[slot] = FALSE;
      HEAPPTR *nextheap = &s_heaphead[slot];
      while (*nextheap)
        if ((!(*nextheap)->allocatedblocks) &&
            ((DWORD)((*nextheap)->handle) < FIRSTUSERHEAP))
          FreeHeap(nextheap);
        else
          nextheap = &(*nextheap)->next;
      LeaveCriticalSection(&s_critsect[slot]);
    }
}

//===========================================================================
static void FreeHeap (HEAPPTR *nextptr) {

  // UNLINK THE HEAP
  HEAPPTR heapptr = *nextptr;
  *nextptr = heapptr->next;

  // FREE THE HEAP
  VirtualFree(heapptr,0,MEM_RELEASE);

}

//===========================================================================
static BOOL FreeHeapBlock (HEAPPTR  heapptr,
                           BLOCKPTR block) {

  // MARK THE BLOCK AS FREE
  FREEBLOCKPTR freeblock = (FREEBLOCKPTR)block;
  freeblock->flags   = BF_FREEBLOCK;
  freeblock->padding = 0;
  freeblock->next    = NULL;

  // IF WE ARE MAINTAING A FULLY COMBINED AND SORTED FREE LIST, THE COMBINE
  // THIS BLOCK WITH CONTIGUOUS FREE BLOCKS AND ADD FIND THE CORRECT LOCATION
  // FOR IT IN THE FREE LIST.  OTHERWISE, JUST DO MINIMAL PROCESSING FOR NOW,
  // DELAYING THE COMBINING AND SORTING OPERATIONS UNTIL THE NEXT BLOCK
  // ALLOCATION ON THIS HEAP.
  FREEBLOCKPTR  endblock      = (FREEBLOCKPTR)((LPBYTE)freeblock+freeblock->bytes);
  FREEBLOCKPTR *nextfreeblock = &heapptr->firstfreeblock;
  FREEBLOCKPTR  currfreeblock;
  if (heapptr->maintainfreelist) {
    --heapptr->maintainfreelist;
    for (;;) {
      currfreeblock = *nextfreeblock;
      if ((!currfreeblock) || (currfreeblock > endblock))
        break;
      BOOL unlink = FALSE;
      if ((DWORD)freeblock->bytes+(DWORD)currfreeblock->bytes <= 0xFFFF)
        if (currfreeblock == endblock) {
          freeblock->bytes += currfreeblock->bytes;
          endblock = (FREEBLOCKPTR)((LPBYTE)block+block->bytes);
          unlink   = TRUE;
        }
        else if (((FREEBLOCKPTR)((LPBYTE)currfreeblock+currfreeblock->bytes)) == freeblock) {
          currfreeblock->bytes += freeblock->bytes;
          freeblock = currfreeblock;
          unlink    = TRUE;
        }
      if (unlink)
        *nextfreeblock = currfreeblock->next;
      else
        nextfreeblock = &currfreeblock->next;
    }
  }

  // IF THIS BLOCK IS AT THE END OF THE HEAP, SHRINK THE HEAP
  if (heapptr->termblock == (BLOCKPTR)endblock)
    heapptr->termblock = (BLOCKPTR)freeblock;

  // OTHERWISE, ADD THIS BLOCK TO THE LINKED LIST OF FREE BLOCKS
  else {
    freeblock->next = currfreeblock;
    *nextfreeblock = freeblock;
  }

  // IF THIS HEAP IS NOW EMPTY, SET OURSELVES A REMINDER TO REMOVE IT DURING
  // THE NEXT CLEANUP
  --heapptr->allocatedblocks;
  if ((!heapptr->allocatedblocks) &&
      ((DWORD)(heapptr->handle) < FIRSTUSERHEAP)) {
    s_emptyheap[heapptr->slot] = TRUE;
    s_lastemptyheap = heapptr;
  }

  return TRUE;
}

//===========================================================================
static inline LPVOID SatisfyAllocRequest (HLOCKEDHEAP lockedhandle,
                                          HEAPPTR     heapptr,
                                          DWORD       flags,
                                          DWORD       bytes) {

  // ALLOCATE THE REQUESTED BLOCK OF MEMORY FROM THE CALLER'S HEAP
  LPVOID result = NULL;
  if (heapptr) {
    BYTE baseflags = 0;
    if (flags & SMEM_FLAG_PRESERVEONDESTROY)
      baseflags |= BF_PRESERVE;
    result = AllocateHeapBlock(heapptr,
                               bytes,
                               baseflags);
  }

  // IF THERE IS AN EMPTY HEAP WAITING TO BE CLEANED UP, AND WE DIDN'T
  // JUST ALLOCATE A BLOCK IN IT, THEN PERFORM THE CLEANUP
  if (s_lastemptyheap && (s_lastemptyheap != heapptr))
    FreeEmptyHeaps();

  // UNLOCK THE HEAP
  UnlockHeap(lockedhandle);

  // IF THE ALLOCATION FAILED, DISPLAY A FATAL ERROR
  if (!result)
    if (heapptr->filename[0])
      FatalError(ERROR_NOT_ENOUGH_MEMORY,
                 heapptr->filename,
                 heapptr->linenumber);
    else
      FatalError(ERROR_NOT_ENOUGH_MEMORY,
                 "SMemHeapAlloc()",
                 SERR_LINECODE_FUNCTION);

  // FILL THE NEW BLOCK WITH ITS REQUIRED STARTING VALUE
  if (flags & SMEM_FLAG_ZEROMEMORY)
    ZeroMemory(result,bytes);
  else if (s_debugmode)
    FillMemory(result,bytes,0xEE);

  return result;
}

//===========================================================================
static inline BOOL SatisfyFreeRequest (HLOCKEDHEAP lockedhandle,
                                       HEAPPTR     heapptr,
                                       LPVOID      ptr,
                                       BLOCKPTR    blockptr) {

  // IF THIS IS A LARGE BLOCK ALLOCATED OUTSIDE OF A HEAP, FREE IT
  if (blockptr->flags & BF_LARGEALLOC) {
    LPVOID largeallocptr = (LPBYTE)ptr-sizeof(BLOCK)-sizeof(BLOCKPTR);
    largeallocptr = (LPVOID)((DWORD)largeallocptr & ~(s_pagesize-1));
    VirtualFree(largeallocptr,0,MEM_RELEASE);
  }

  // IF DEBUG MODE IS ENABLED AND THIS IS NOT A LARGE BLOCK ALLOCATED
  // OUTSIDE THE HEAP, WIPE OUT THE USER PORTION OF THE DATA
  else if (s_debugmode) {
    DWORD userbytes = blockptr->bytes
                        -blockptr->padding
                        -sizeof(BLOCK)
                        -((blockptr->flags & BF_BOUNDINGSIG) ? sizeof(WORD) : 0);
    FillMemory(ptr,userbytes,0xDD);
  }

  // FREE THIS BLOCK FROM THE HEAP
  BOOL success = FALSE;
  if (heapptr)
    success = FreeHeapBlock(heapptr,blockptr);

  // UNLOCK THE HEAP
  UnlockHeap(lockedhandle);

  return success;
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

#define  CHECKINITIALIZED(name,errortype,retval)     \
  do                                                 \
    if (!CheckInitialized()) {                       \
      errortype(STORM_ERROR_MEMORY_MANAGER_INACTIVE, \
                name,                                \
                SERR_LINECODE_FUNCTION);             \
      return retval;                                 \
    }                                                \
  while (0)

//===========================================================================
LPVOID APIENTRY SMemAlloc (DWORD  bytes,
                           LPCSTR filename,
                           int    linenumber,
                           DWORD  flags) {
  CHECKINITIALIZED("SMemAlloc()",FatalError,NULL);

  // DETERMINE THE HEAP THAT WILL BE USED FOR THIS ALLOCATION
  HSHEAP handle = GetHandleByCaller(filename,
                                    linenumber);

  // LOCK THE HEAP
  HLOCKEDHEAP lockedhandle;
  HEAPPTR     heapptr = LockHeapByHandle(handle,
                                         &lockedhandle,
                                         FALSE);

  // IF THE HEAP DOES NOT EXIST, ALLOCATE ONE.  THE SLOT CONTAINING
  // THE HEAP IS STILL LOCKED AFTER THE CALL TO LOCKHEAPBYHANDLE().
  if (!heapptr)
    heapptr = AllocateHeap(filename,
                           linenumber,
                           handle,
                           GetSlotByHandle(handle),
                           PAGESIZE,
                           PAGESIZE,
                           RESERVESIZE);

  // ALLOCATE MEMORY AND UNLOCK THE HEAP
  LPVOID result = SatisfyAllocRequest(lockedhandle,
                                      heapptr,
                                      flags,
                                      bytes);

  // TRACE THE ALLOCATION IF NECESSARY
  TRACE(result,"SMemAlloc()",filename,linenumber);

  return result;

}

//===========================================================================
BOOL APIENTRY SMemDestroy () {
  if (!s_initialized)
    return TRUE;
  s_initialized = FALSE;

  // REMOVE ALL EMPTY HEAPS, AND ALL CRITICAL SECTIONS
  for (DWORD loop = 0; loop < TABLESIZE; ++loop) {
    EnterCriticalSection(&s_critsect[loop]);
    s_emptyheap[loop] = FALSE;
    HEAPPTR *nextheap = &s_heaphead[loop];
    while (*nextheap)
      if ((*nextheap)->allocatedblocks)
        nextheap = DestroyHeap(nextheap);
      else {
        if ((*nextheap)->active &&
            ((DWORD)((*nextheap)->handle) >= FIRSTUSERHEAP))
          REPORTRESOURCELEAK(HSHEAP);
        FreeHeap(nextheap);
      }
    LeaveCriticalSection(&s_critsect[loop]);
    DeleteCriticalSection(&s_critsect[loop]);
  }

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SMemFindNextBlock (HSHEAP              heap,
                                 LPVOID              prevblock,
                                 LPVOID             *nextblock,
                                 LPSMEMBLOCKDETAILS  details) {
  CHECKINITIALIZED("SMemFindNextBlock()",Warning,FALSE);

  VALIDATEBEGIN;
  VALIDATE(heap);
  VALIDATE(nextblock);
  VALIDATE(details);
  VALIDATE(details->size == sizeof(SMEMBLOCKDETAILS));
  VALIDATEEND;

  // BLANK OUT THE BLOCK DETAILS STRUCTURE
  ZeroMemory((LPBYTE)details+sizeof(DWORD),
             details->size-sizeof(DWORD));

  // CLAIM THE CRITICAL SECTION FOR THE SLOT THAT CONTAINS THIS HEAP
  DWORD slot = GetSlotByHandle(heap);
  EnterCriticalSection(&s_critsect[slot]);

  // GENERATE A POINTER TO THE BLOCK HEADER OF THE PREVIOUS BLOCK
  BLOCKPTR prevblockptr = GetBlockPtrByPtr(prevblock);

  // SEARCH ALL REGIONS OF THIS HEAP FOR THE NEXT BLOCK.  SEARCH REGIONS
  // IN REVERSE ORDER SO THAT BLOCKS WHICH WERE ALLOCATED FIRST WILL TEND
  // TO BE LISTED FIRST.
  BLOCKPTR lastblockptr = NULL;
  BLOCKPTR blockptr     = NULL;
  BOOL     found        = FALSE;
  HEAPPTR  heapptr      = s_heaphead[slot];
  while (heapptr && heapptr->next)
    heapptr = heapptr->next;
  while (heapptr && !found) {

    // IF THIS REGION IS PART OF THE HEAP, SEARCH ALL OF ITS BLOCKS
    if (heapptr->handle == heap) {
      blockptr = heapptr->firstblock;
      while (blockptr != heapptr->termblock) {
        if (lastblockptr == prevblockptr) {
          found = TRUE;
          break;
        }
        lastblockptr = blockptr;
        blockptr     = (BLOCKPTR)((LPBYTE)blockptr+blockptr->bytes);
      }
    }

    // MOVE TO THE PREVIOUS REGION
    if (heapptr == s_heaphead[slot])
      break;
    HEAPPTR lastheapptr = heapptr;
    heapptr = s_heaphead[slot];
    while (heapptr->next != lastheapptr)
      heapptr = heapptr->next;

  }

  // IF WE DIDN'T FIND ONE, RETURN FALSE TO INDICATE THE ITERATION IS
  // COMPLETE
  if (!found) {
    *nextblock = NULL;
    LeaveCriticalSection(&s_critsect[slot]);
    return FALSE;
  }

  // FILL IN INFORMATION ABOUT THE BLOCK
  LPVOID ptr = GetPtrByBlockPtr(blockptr);
  *nextblock = ptr;
  details->ptr       = ptr;
  details->allocated = !(blockptr->flags & BF_FREEBLOCK);
  details->valid     = CheckValidBlock(ptr,
                                       FALSE,
                                       NULL,
                                       0);
  if (blockptr->flags & BF_LARGEALLOC) {
    BLOCKPTR largeblockptr = (BLOCKPTR)ptr-1;
    DWORD largeblockoverhead = sizeof(BLOCKPTR)+sizeof(BLOCK);
    MEMORY_BASIC_INFORMATION info;
    VirtualQuery((LPBYTE)ptr-largeblockoverhead,
                 &info,
                 sizeof(MEMORY_BASIC_INFORMATION));
    details->bytes    = info.RegionSize-largeblockoverhead;
    details->overhead = sizeof(BLOCK)+sizeof(LPVOID)+blockptr->padding
                          +largeblockoverhead;
  }
  else {
    details->overhead  = sizeof(BLOCK)+blockptr->padding;
    details->bytes     = blockptr->bytes-details->overhead;
  }

  // LEAVE THE CRITICAL SECTION
  LeaveCriticalSection(&s_critsect[slot]);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SMemFindNextHeap (HSHEAP             prevheap,
                                HSHEAP            *nextheap,
                                LPSMEMHEAPDETAILS  details) {
  CHECKINITIALIZED("SMemFindNextHeap()",Warning,FALSE);

  VALIDATEBEGIN;
  VALIDATE(nextheap);
  VALIDATE(details);
  VALIDATE(details->size == sizeof(SMEMHEAPDETAILS));
  VALIDATEEND;

  // BLANK OUT THE HEAP DETAILS STRUCTURE
  ZeroMemory((LPBYTE)details+sizeof(DWORD),
             details->size-sizeof(DWORD));

  // DETERMINE THE FIRST SLOT TO CHECK
  DWORD slot = 0;
  if (prevheap)
    slot = GetSlotByHandle(prevheap);

  // FIND THE NEXT HEAP
  HSHEAP  lastheap = (HSHEAP)0;
  HEAPPTR heapptr  = NULL;
  for (; slot < TABLESIZE; ++slot) {
    EnterCriticalSection(&s_critsect[slot]);
    heapptr = s_heaphead[slot];
    while (heapptr) {
      if (heapptr->active) {
        if (lastheap == prevheap)
          break;
        lastheap = heapptr->handle;
      }
      heapptr = heapptr->next;
    }
    if (heapptr)
      break;
    LeaveCriticalSection(&s_critsect[slot]);
  }

  // IF WE DIDN'T FIND ONE, RETURN FALSE TO INDICATE THE ITERATION IS
  // COMPLETE
  if (!heapptr) {
    *nextheap = NULL;
    return FALSE;
  }

  // FILL IN INFORMATION ABOUT THE HEAP
  *nextheap = heapptr->handle;
  details->handle      = heapptr->handle;
  details->linenumber  = heapptr->linenumber;
  details->maximumsize = MAXHEAPSIZE; // note: change this
  SStrCopy(details->filename,heapptr->filename,MAX_PATH);

  // SUM THE ALLOCATION STATISTICS FOR EACH REGION THAT MAKES UP THE HEAP
  while (heapptr) {
    CombineFreeBlocks(heapptr);
    if (heapptr->handle == *nextheap) {
      details->committedbytes  += heapptr->committedbytes;
      details->reservedbytes   += heapptr->reservedbytes;
      details->allocatedblocks += heapptr->allocatedblocks;
    }
    heapptr = heapptr->next;
  }

  // LEAVE THE CRITICAL SECTION
  LeaveCriticalSection(&s_critsect[slot]);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SMemFree (LPVOID ptr,
                        LPCSTR filename,
                        int    linenumber,
                        DWORD  flags) {
  CHECKINITIALIZED("SMemFree()",Warning,FALSE);

  // TRACE THE DEALLOCATION IF NECESSARY
  TRACE(ptr,"SMemFree()",filename,linenumber);

  // VERIFY THAT THIS BLOCK IS VALID AND ALLOCATED
  if (!CheckValidBlock(ptr,
                       TRUE,
                       filename,
                       linenumber))
    return FALSE;

  // LOCK THE HEAP WHICH CONTAINS THE BLOCK
  BLOCKPTR    blockptr = GetBlockPtrByPtr(ptr);
  HLOCKEDHEAP lockedhandle;
  HEAPPTR     heapptr  = LockHeapByBlockPtr(blockptr,&lockedhandle);

  // FREE THIS MEMORY BLOCK AND UNLOCK THE HEAP
  return SatisfyFreeRequest(lockedhandle,
                            heapptr,
                            ptr,
                            blockptr);
}

//===========================================================================
HSHEAP APIENTRY SMemGetHeapByCaller (LPCSTR filename,
                                     int    linenumber) {
  CHECKINITIALIZED("SMemGetHeapByCaller()",Warning,(HSHEAP)0);

  return GetHandleByCaller(filename,linenumber);
}

//===========================================================================
HSHEAP APIENTRY SMemGetHeapByPtr (LPVOID ptr) {
  CHECKINITIALIZED("SMemGetHeapByPtr()",Warning,(HSHEAP)0);

  BLOCKPTR blockptr = GetBlockPtrByPtr(ptr);
  if (CheckValidBlock(blockptr,
                      FALSE,
                      NULL,
                      0))
    return GetHandleByBlockPtr(blockptr);
  else
    return (HSHEAP)0;
}

//===========================================================================
LPVOID APIENTRY SMemHeapAlloc (HSHEAP handle,
                               DWORD  flags,
                               DWORD  bytes) {
  CHECKINITIALIZED("SMemHeapAlloc()",FatalError,NULL);

  // LOCK THE HEAP
  HLOCKEDHEAP lockedhandle;
  HEAPPTR     heapptr = LockHeapByHandle(handle,
                                         &lockedhandle,
                                         TRUE);
  if (!heapptr)
    FatalError(ERROR_INVALID_HANDLE,
               "SMemHeapAlloc()",
               SERR_LINECODE_FUNCTION);

  // ALLOCATE MEMORY AND UNLOCK THE HEAP
  LPVOID result = SatisfyAllocRequest(lockedhandle,
                                      heapptr,
                                      flags,
                                      bytes);

  // TRACE THE ALLOCATION IF NECESSARY
  TRACE(result,"SMemHeapAlloc()",NULL,0);

  return result;
}

//===========================================================================
HSHEAP APIENTRY SMemHeapCreate (DWORD options,
                                DWORD initialsize,
                                DWORD maximumsize) {
  CHECKINITIALIZED("SMemHeapCreate()",Warning,(HSHEAP)0);

  // VERIFY THAT THE RESERVED OPTIONS PARAMETER IS NOT BEING USED
  if (options) {
    Warning(ERROR_INVALID_PARAMETER,
            "SMemHeapCreate()",
            SERR_LINECODE_FUNCTION);
    return FALSE;
  }

  // ROUND THE REQUESTED INITIAL SIZE UP TO THE NEXT PAGE BOUNDARY
  if (initialsize & (PAGESIZE-1))
    initialsize += PAGESIZE-(initialsize & (PAGESIZE-1));
  initialsize = max(initialsize,PAGESIZE);
  maximumsize = max(maximumsize,initialsize);

  // FIND AN UNUSED HANDLE FOR THIS HEAP
  static HSHEAP handle = (HSHEAP)FIRSTUSERHEAP;
  for (;;) {

    // INCREMENT THE HANDLE SEQUENCE
    handle = (HSHEAP)((DWORD)handle+1);
    if (!handle)
      handle = (HSHEAP)FIRSTUSERHEAP;

    // CHECK TO SEE IF THIS HANDLE IS IN USE
    HLOCKEDHEAP lockedhandle = (HLOCKEDHEAP)INVALID_HANDLE_VALUE;
    if (LockHeapByHandle(handle,
                         &lockedhandle,
                         TRUE))
      UnlockHeap(lockedhandle);
    else
      break;

  }

  // ALLOCATE THE HEAP
  DWORD slot = GetSlotByHandle(handle);
  EnterCriticalSection(&s_critsect[slot]);
  AllocateHeap(NULL,
               0,
               handle,
               slot,
               PAGESIZE,
               initialsize,
               RESERVESIZE);
  LeaveCriticalSection(&s_critsect[slot]);
  if (!handle)
    Warning(ERROR_NOT_ENOUGH_MEMORY,
            "SMemHeapCreate()",
            SERR_LINECODE_FUNCTION);

  // RETURN THE HEAP HANDLE
  return handle;
}

//===========================================================================
BOOL APIENTRY SMemHeapDestroy (HSHEAP handle) {
  CHECKINITIALIZED("SMemHeapDestroy()",Warning,FALSE);

  // LOCK THE HEAP'S CRITICAL SECTION
  DWORD slot = GetSlotByHandle(handle);
  EnterCriticalSection(&s_critsect[slot]);

  // DESTROY ALL REGIONS OF THE HEAP
  BOOL     found   = FALSE;
  HEAPPTR *nextptr = &s_heaphead[slot];
  while (*nextptr)
    if ((*nextptr)->handle == handle) {
      found   = TRUE;
      nextptr = DestroyHeap(nextptr);
    }
    else
      nextptr = &(*nextptr)->next;

  // UNLOCK THE CRITICAL SECTION
  LeaveCriticalSection(&s_critsect[slot]);

  return found;
}

//===========================================================================
BOOL APIENTRY SMemHeapFree (HSHEAP handle,
                            DWORD  flags,
                            LPVOID ptr) {
  CHECKINITIALIZED("SMemHeapFree()",Warning,FALSE);

  // TRACE THE DEALLOCATION IF NECESSARY
  TRACE(ptr,"SMemHeapFree()",NULL,0);

  // VERIFY THAT THIS BLOCK IS VALID AND ALLOCATED
  if (!CheckValidBlock(ptr,
                       TRUE,
                       NULL,
                       0))
    return FALSE;

  // VERIFY THAT THE HEAP HANDLE IS CORRECT
  BLOCKPTR blockptr = GetBlockPtrByPtr(ptr);
  if (GetHandleByBlockPtr(blockptr) != handle)
    return FALSE;

  // LOCK THE HEAP WHICH CONTAINS THE BLOCK
  HLOCKEDHEAP lockedhandle;
  HEAPPTR     heapptr  = LockHeapByBlockPtr(blockptr,&lockedhandle);

  // FREE THIS MEMORY BLOCK AND UNLOCK THE HEAP
  return SatisfyFreeRequest(lockedhandle,
                            heapptr,
                            ptr,
                            blockptr);
}

//===========================================================================
void APIENTRY SMemInitialize () {
  if (s_initialized)
    return;

  // DETERMINE WHETHER TO PERFORM HEAP CHECKS
#ifdef _DEBUG
  s_debugmode = TRUE;
#endif
  SRegLoadValue(REGKEY,REGVAL_DEBUG,0,(LPDWORD)&s_debugmode);
  SRegLoadValue(REGKEY,REGVAL_GUARD,0,(LPDWORD)&s_guardmode);
#ifdef _DEBUG
  SRegSaveValue(REGKEY,REGVAL_DEBUG,0,(DWORD)s_debugmode);
  SRegSaveValue(REGKEY,REGVAL_GUARD,0,(DWORD)s_guardmode);
  s_debugmode = TRUE;
#endif

  // INITIALIZE CRITICAL SECTIONS
  for (DWORD loop = 0; loop < TABLESIZE; ++loop)
    InitializeCriticalSection(&s_critsect[loop]);

  s_initialized = TRUE;
}

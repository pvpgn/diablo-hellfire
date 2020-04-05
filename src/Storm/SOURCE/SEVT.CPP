/****************************************************************************
*
*  SEVT.CPP
*  Storm event dispatching functions
*
*  By Michael O'Brien (5/9/96)
*
***/

#include "pch.h"
#pragma  hdrstop

NODEDECL(BREAKCMD) {
    LPVOID data;
} *BREAKCMDPTR;

typedef struct _IDHASHENTRY {
    DWORD         id;
    DWORD         sequence;
    SEVTHANDLER   handler;
    _IDHASHENTRY *next;
} IDHASHENTRY, *IDHASHENTRYPTR;

typedef struct _IDHASHTABLE {
    IDHASHENTRYPTR *data;
    DWORD           size;
    DWORD           used;
    _IDHASHTABLE   *next;
} IDHASHTABLE, *IDHASHTABLEPTR;

typedef struct _TYPEHASHENTRY {
    DWORD           type;
    DWORD           subtype;
    DWORD           sequence;
    IDHASHTABLEPTR  idhashtable;
    _TYPEHASHENTRY *next;
} TYPEHASHENTRY, *TYPEHASHENTRYPTR;

static LIST(BREAKCMD)    s_breakcmdlist;
static CCritSect         s_critsect;
static LONG              s_dispatchesinprogress = 0;
static BOOL              s_modified;
static TYPEHASHENTRYPTR *s_typehashtable        = NULL;
static DWORD             s_typehashtablesize    = 0;
static DWORD             s_typehashtableused    = 0;

//===========================================================================
static DWORD inline ComputeNewTableSize (DWORD currentused) {
  DWORD newsize = 1;
  while (newsize <= ((s_typehashtableused+1) << 1))
    newsize <<= 1;
  return newsize;
}

//===========================================================================
static void CopyIdHashTable (IDHASHTABLEPTR dest,
                             IDHASHTABLEPTR source) {

  // ALLOCATE SPACE FOR THE TABLE
  dest->size = source->size;
  dest->used = source->used;
  dest->data = (IDHASHENTRYPTR *)ALLOC(dest->size*sizeof(IDHASHENTRYPTR));

  // FILL IN EACH TABLE SLOT
  for (DWORD id = 0; id < source->size; ++id) {
    IDHASHENTRYPTR  sourcecurr = *(source->data+id);
    IDHASHENTRYPTR *destnext   = dest->data+id;
    while (sourcecurr) {
      *destnext = NEW(IDHASHENTRY);
      CopyMemory(*destnext,sourcecurr,sizeof(IDHASHENTRY));
      sourcecurr = sourcecurr->next;
      destnext   = &(*destnext)->next;
    }
    *destnext = NULL;
  }

}

//===========================================================================
static void DeleteIdHashTable (IDHASHTABLEPTR idhashtable) {
  for (DWORD id = 0; id < idhashtable->size; ++id) {
    IDHASHENTRYPTR currid;
    while ((currid = *(idhashtable->data+id)) != NULL) {
      *(idhashtable->data+id) = currid->next;
      DEL(currid);
    }
  }
  FREE(idhashtable->data);
  DEL(idhashtable);
}

//===========================================================================
static TYPEHASHENTRYPTR inline FindTypeHashEntry (DWORD type, DWORD subtype) {
  if (!(s_typehashtable && s_typehashtablesize))
    return NULL;

  DWORD            slot = (type ^ subtype) & (s_typehashtablesize-1);
  TYPEHASHENTRYPTR ptr  = *(s_typehashtable+slot);
  while (ptr && ((ptr->type != type) || (ptr->subtype != subtype)))
    ptr = ptr->next;
  return ptr;
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
BOOL APIENTRY SEvtBreakHandlerChain (LPVOID data) {
  s_critsect.Enter();
  BREAKCMDPTR newcmd = s_breakcmdlist.NewNode();
  newcmd->data = data;
  s_critsect.Leave();
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SEvtDestroy () {
  s_critsect.Enter();
  for (DWORD type = 0; type < s_typehashtablesize; ++type) {
    TYPEHASHENTRYPTR currtype;
    while ((currtype = *(s_typehashtable+type)) != NULL) {
      IDHASHTABLEPTR currhashtable;
      while ((currhashtable = currtype->idhashtable) != NULL) {
        currtype->idhashtable = currhashtable->next;
        DeleteIdHashTable(currhashtable);
      }
      *(s_typehashtable+type) = currtype->next;
      DEL(currtype);
    }
  }
  if (s_typehashtable)
    DEL(s_typehashtable);
  s_typehashtable     = NULL;
  s_typehashtablesize = 0;
  s_typehashtableused = 0;
  s_modified          = TRUE;
  s_critsect.Leave();
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SEvtDispatch (DWORD  type,
                            DWORD  subtype,
                            DWORD  id,
                            LPVOID data) {
  InterlockedIncrement(&s_dispatchesinprogress);
  BOOL           success      = FALSE;
  DWORD          currsequence = 0xFFFFFFFF;
  IDHASHENTRYPTR currptr      = NULL;
  for (;;) {

    // ENTER THE CRITICAL SECTION
    s_critsect.Enter();

    // IF THERE IS A BREAK COMMAND FOR THIS HANDLER, EXIT
    BOOL breakcmd = FALSE;
    ITERATELIST(BREAKCMD,s_breakcmdlist,curr)
      if (curr->data == data) {
        breakcmd = TRUE;
        ITERATE_DELETEANDBREAK;
      }
    if (breakcmd) {
      s_critsect.Leave();
      break;
    }

    // IF WE DON'T HAVE A VALID POINTER TO THE NEXT HANDLER RECORD,
    // FIND ONE USING THE TYPE, SUBTYPE, ID, AND SEQUENCE
    if ((!currptr) || s_modified) {
      currptr = NULL;
      for (ONCE) {

        // FIND THE TYPE/SUBTYPE ENTRY IN THE TYPE HASH TABLE
        TYPEHASHENTRYPTR typeentry = FindTypeHashEntry(type,subtype);
        if (!typeentry)
          break;

        // FIND THE SLOT FOR THIS ID IN THE TYPE'S ID HASH TABLE
        if (!(typeentry->idhashtable->data && typeentry->idhashtable->size))
          break;
        DWORD slot = id & (typeentry->idhashtable->size-1);
        currptr = *(typeentry->idhashtable->data+slot);

        // FIND THE NEXT HANDLER AFTER THE CURRENT SEQUENCE
        while (currptr &&
               ((currptr->id != id) ||
                (currptr->sequence >= currsequence)))
          currptr = currptr->next;

        // IF WE ARE THE ONLY ACTIVE THREAD CURRENTLY DISPATCHING, THEN
        // RESET THE MODIFIED VARIABLE
        if (s_dispatchesinprogress == 1)
          s_modified = FALSE;

      }
    }

    // SAVE THE HANDLER FOR THIS POINTER, AND MOVE THE POINTER AND SEQUENCE
    // NUMBER TO THE NEXT RECORD
    SEVTHANDLER handler = NULL;
    if (currptr) {
      handler      = currptr->handler;
      currsequence = currptr->sequence;
      do
        currptr = currptr->next;
      while (currptr && (currptr->id != id));
    }

    // LEAVE THE CRITICAL SECTION
    s_critsect.Leave();

    // DISPATCH THE EVENT
    if (handler) {
      success = TRUE;
      handler(data);
    }

    // IF THIS IS THE LAST HANDLER IN THE CHAIN, EXIT THE LOOP
    if (!currptr)
      break;

  }
  InterlockedDecrement(&s_dispatchesinprogress);

  // CLEAR OUT ANY LEFT-OVER BREAK COMMANDS FOR THIS HANDLER CHAIN
  if (s_breakcmdlist.Head()) {
    s_critsect.Enter();
    ITERATELIST(BREAKCMD,s_breakcmdlist,curr)
      if (curr->data == data)
        ITERATE_DELETE;
    s_critsect.Leave();
  }

  return success;
}

//===========================================================================
BOOL APIENTRY SEvtPopState (DWORD type,
                            DWORD subtype) {
  BOOL success = FALSE;
  s_critsect.Enter();
  for (ONCE) {

    // FIND THE TYPE/SUBTYPE ENTRY IN THE TYPE HASH TABLE
    TYPEHASHENTRYPTR typeentry = FindTypeHashEntry(type,subtype);
    if (!typeentry)
      break;

    // CHECK FOR A STACK UNDERFLOW
    if (!typeentry->idhashtable->next) {
      success = SEvtUnregisterType(type,subtype);
      break;
    }

    // FREE THE ID HASH TABLE AT THE TOP OF THE STACK
    IDHASHTABLEPTR next = typeentry->idhashtable->next;
    DeleteIdHashTable(typeentry->idhashtable);
    typeentry->idhashtable = next;

  }
  s_modified = TRUE;
  s_critsect.Leave();
  return success;
}

//===========================================================================
BOOL APIENTRY SEvtPushState (DWORD type,
                             DWORD subtype) {
  BOOL success = FALSE;
  s_critsect.Enter();
  for (ONCE) {

    // FIND THE TYPE/SUBTYPE ENTRY IN THE TYPE HASH TABLE
    TYPEHASHENTRYPTR typeentry = FindTypeHashEntry(type,subtype);
    if (!typeentry)
      break;

    // MAKE A COPY OF THE ID HASH TABLE
    IDHASHTABLEPTR newtable = NEW(IDHASHTABLE);
    CopyIdHashTable(newtable,typeentry->idhashtable);

    // LINK IT AT THE HEAD OF THE LIST
    newtable->next = typeentry->idhashtable;
    typeentry->idhashtable = newtable;

    s_modified = TRUE;
    success    = TRUE;
  }
  s_critsect.Leave();
  return success;
}

//===========================================================================
BOOL APIENTRY SEvtRegisterHandler (DWORD       type,
                                   DWORD       subtype,
                                   DWORD       id,
                                   DWORD       flags,
                                   SEVTHANDLER handler) {
  VALIDATEBEGIN;
  VALIDATE(handler);
  VALIDATE(!flags);
  VALIDATEEND;

  s_critsect.Enter();

  // FIND THE TYPE/SUBTYPE ENTRY IN THE TYPE HASH TABLE
  TYPEHASHENTRYPTR typeentry = FindTypeHashEntry(type,subtype);

  // IF WE COULDN'T FIND AN ENTRY, ADD ONE
  if (!typeentry) {

    // GROW THE TYPE HASH TABLE IF NECESSARY
    if (s_typehashtableused >= (s_typehashtablesize >> 1)) {

      // ALLOCATE A NEW TABLE
      DWORD             newsize  = ComputeNewTableSize(s_typehashtableused);
      TYPEHASHENTRYPTR *newtable = (TYPEHASHENTRYPTR *)ALLOCZERO(newsize*sizeof(TYPEHASHENTRYPTR));

      // REHASH THE OLD TABLE INTO THE NEW TABLE
      if (s_typehashtable && s_typehashtablesize)
        for (DWORD loop = 0; loop < s_typehashtablesize; ++loop) {
          TYPEHASHENTRYPTR ptr = *(s_typehashtable+loop);
          while (ptr) {
            DWORD            slot = (ptr->type ^ ptr->subtype) & (newsize-1);
            TYPEHASHENTRYPTR next = ptr->next;
            ptr->next        = *(newtable+slot);
            *(newtable+slot) = ptr;
            ptr              = next;
          }
        }

      // REPLACE THE OLD TABLE WITH THE NEW TABLE
      if (s_typehashtable)
        DEL(s_typehashtable);
      s_typehashtable     = newtable;
      s_typehashtablesize = newsize;

    }

    // CREATE A NEW ENTRY FOR THIS TYPE/SUBTYPE
    {
      DWORD            slot  = (type ^ subtype) & (s_typehashtablesize-1);
      TYPEHASHENTRYPTR entry = NEWZERO(TYPEHASHENTRY);
      entry->type        = type;
      entry->subtype     = subtype;
      entry->idhashtable = NEWZERO(IDHASHTABLE);
      entry->next        = *(s_typehashtable+slot);
      *(s_typehashtable+slot) = entry;
      ++s_typehashtableused;
      typeentry = *(s_typehashtable+slot);
    }

  }

  // GROW THE TYPE'S ID HASH TABLE IF NECESSARY
  if (typeentry->idhashtable->used >= (typeentry->idhashtable->size >> 1)) {

    // ALLOCATE A NEW TABLE
    DWORD           newsize  = ComputeNewTableSize(typeentry->idhashtable->size);
    IDHASHENTRYPTR *newtable = (IDHASHENTRYPTR *)ALLOCZERO(newsize*sizeof(IDHASHENTRYPTR));

    // REHASH THE OLD TABLE INTO THE NEW TABLE
    if (typeentry->idhashtable->data && typeentry->idhashtable->size)
      for (DWORD loop = 0; loop < typeentry->idhashtable->size; ++loop) {
        IDHASHENTRYPTR ptr = *(typeentry->idhashtable->data+loop);
        while (ptr) {
          DWORD          slot = ptr->id & (newsize-1);
          IDHASHENTRYPTR next = ptr->next;
          ptr->next        = *(newtable+slot);
          *(newtable+slot) = ptr;
          ptr              = next;
        }
      }

    // REPLACE THE OLD TABLE WITH THE NEW TABLE
    if (typeentry->idhashtable->data)
      FREE(typeentry->idhashtable->data);
    typeentry->idhashtable->data = newtable;
    typeentry->idhashtable->size = newsize;

  }

  // CREATE A NEW ENTRY FOR THIS ID
  {
    DWORD          slot  = id & (typeentry->idhashtable->size-1);
    IDHASHENTRYPTR entry = NEWZERO(IDHASHENTRY);
    entry->id       = id;
    entry->sequence = ++typeentry->sequence;
    entry->handler  = handler;
    entry->next     = *(typeentry->idhashtable->data+slot);
    *(typeentry->idhashtable->data+slot) = entry;
    ++typeentry->idhashtable->used;
  }

  s_modified = TRUE;
  s_critsect.Leave();
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SEvtUnregisterHandler (DWORD       type,
                                     DWORD       subtype,
                                     DWORD       id,
                                     SEVTHANDLER handler) {
  BOOL success = FALSE;
  s_critsect.Enter();
  for (ONCE) {

    // FIND THE TYPE/SUBTYPE ENTRY IN THE TYPE HASH TABLE
    TYPEHASHENTRYPTR typeentry = FindTypeHashEntry(type,subtype);
    if (!typeentry)
      return FALSE;

    // FIND THE SLOT FOR THIS ID IN THE TYPE'S ID HASH TABLE
    if (!(typeentry->idhashtable->data && typeentry->idhashtable->size))
      return FALSE;
    DWORD slot = id & (typeentry->idhashtable->size-1);

    // IF WE WERE GIVEN A POINTER TO A HANDLER, FREE THE ID ENTRY MATCHING
    // THE ID AND HANDLER.  OTHERWISE, FREE ALL ENTRIES MATCHING THE ID.
    IDHASHENTRYPTR *nextptr = typeentry->idhashtable->data+slot;
    while (*nextptr)
      if (((*nextptr)->id == id) &&
          ((!handler) || ((*nextptr)->handler == handler))) {
        IDHASHENTRYPTR curr = *nextptr;
        *nextptr = curr->next;
        DEL(curr);
        s_modified = TRUE;
        success    = TRUE;
        --typeentry->idhashtable->used;
      }
      else
        nextptr = &(*nextptr)->next;

  }
  s_critsect.Leave();
  return success;
}

//===========================================================================
BOOL APIENTRY SEvtUnregisterType (DWORD type,
                                  DWORD subtype) {
  BOOL success = FALSE;
  s_critsect.Enter();
  for (ONCE) {

    // FIND THE TYPE/SUBTYPE ENTRY IN THE TYPE HASH TABLE
    TYPEHASHENTRYPTR typeentry = FindTypeHashEntry(type,subtype);
    if (!typeentry)
      break;

    // FREE ALL ID ENTRIES AND THE ID HASH TABLE FOR THIS TYPE/SUBTYPE
    {
      IDHASHTABLEPTR currhashtable;
      while ((currhashtable = typeentry->idhashtable) != NULL) {
        typeentry->idhashtable = currhashtable->next;
        DeleteIdHashTable(currhashtable);
      }
    }

    // FREE THE RECORD FOR THIS TYPE/SUBTYPE
    DWORD slot = (type ^ subtype) & (s_typehashtablesize-1);
    TYPEHASHENTRYPTR *nextptr = s_typehashtable+slot;
    while (*nextptr)
      if ((*nextptr) == typeentry) {
        TYPEHASHENTRYPTR curr = *nextptr;
        *nextptr = curr->next;
        DEL(curr);
        --s_typehashtableused;
      }
      else
        nextptr = &(*nextptr)->next;

    s_modified = TRUE;
    success    = TRUE;
  }
  s_critsect.Leave();
  return success;
}

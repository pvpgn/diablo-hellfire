/****************************************************************************
*
*  SRGN.CPP
*  Storm region manager
*
*  By Michael O'Brien (9/3/97)
*
***/

#include "pch.h"
#pragma  hdrstop

DECLARE_STRICT_HANDLE(HLOCKEDRGN);

#define  SF_ADDING     0x00000001
#define  SF_OVERLAPS   0x00000002
#define  SF_TEMPMASK   0x00000003
#define  SF_PARAMONLY  0x00010000

typedef struct _SOURCE {
    RECT   rect;
    LPVOID param;
    int    sequence;
    DWORD  flags;
} SOURCE, *SOURCEPTR;

typedef struct _FOUNDPARAM {
    LPVOID param;
    int    sequence;
} FOUNDPARAM, *FOUNDPARAMPTR;

EXPORTOBJECTDECL(RGN) {
    ARRAY(SOURCE)     source;
    ARRAY(RECT)       combined;
    ARRAY(FOUNDPARAM) foundparams;
    RECT              foundparamsrect;
    int               sequence;
    BOOL              dirty;
} *RGNPTR;

typedef EXPORTTABLEREUSE(RGN,HSRGN,HLOCKEDRGN,SYNC_ALWAYS) RGNTABLE;

static RGNTABLE s_rgntable;

static inline void DeleteCombinedRect (ARRAYPTR(RECT) combinedarray,
                                       DWORD          index);
static inline void DeleteRect (LPRECT rect);
static inline BOOL IsNullRect (LPCRECT rect);
static int __cdecl SortFoundParamsCallback (const void *elem1,
                                            const void *elem2);
static int __cdecl SortRectCallback (const void *elem1,
                                     const void *elem2);

//===========================================================================
static inline void AddCombinedRect (ARRAYPTR(RECT) combinedarray,
                                    LPCRECT        rect) {
  LPRECT newrect = combinedarray->NewElement();
  CopyMemory(newrect,rect,sizeof(RECT));
}

//===========================================================================
static inline void AddSourceRect (ARRAYPTR(SOURCE) sourcearray,
                                  LPCRECT          rect,
                                  LPVOID           param,
                                  int              sequence,
                                  DWORD            flags) {
  SOURCEPTR newptr = sourcearray->NewElement();
  CopyMemory(&newptr->rect,rect,sizeof(RECT));
  newptr->param    = param;
  newptr->sequence = sequence;
  newptr->flags    = flags;
}

//===========================================================================
static inline BOOL CheckForIntersection (LPCRECT sourcerect,
                                         LPCRECT targetrect) {
  return (sourcerect->left   < targetrect->right)  &&
         (sourcerect->top    < targetrect->bottom) &&
         (sourcerect->right  > targetrect->left)   &&
         (sourcerect->bottom > targetrect->top);
}

//===========================================================================
static void ClearRegion (RGNPTR rgnptr) {
  rgnptr->source.SetNumElements(0);
  rgnptr->combined.SetNumElements(0);
  rgnptr->foundparams.SetNumElements(0);
  DeleteRect(&rgnptr->foundparamsrect);
  rgnptr->sequence = 0;
  rgnptr->dirty    = FALSE;
}

//===========================================================================
static void CombineRectangles (ARRAYPTR(RECT) combinedarray) {
  for (DWORD loop0 = 1; loop0 < combinedarray->NumElements(); ++loop0)
    for (DWORD loop1 = 0; loop1 < loop0; ++loop1) {
      LPRECT rect[2] = {&(*combinedarray)[loop0],
                        &(*combinedarray)[loop1]};

      // IF THESE TWO RECTANGLES ARE VERTICALLY ADJACENT AND LINE UP
      // HORIZONTALLY, COMBINE THEM
      if ((rect[0]->left  == rect[1]->left) &&
          (rect[0]->right == rect[1]->right) &&
          ((rect[0]->top == rect[1]->bottom) ||
           (rect[1]->top == rect[0]->bottom))) {
        rect[0]->top    = min(rect[0]->top   ,rect[1]->top);
        rect[0]->bottom = max(rect[0]->bottom,rect[1]->bottom);
        DeleteRect(rect[1]);
        break;
      }

      // IF THESE TWO RECTANGLES ARE NOT HORIZONTALLY ADJACENT, GO TO
      // THE NEXT PAIR
      if ((rect[0]->left != rect[1]->right) &&
          (rect[1]->left != rect[0]->right))
        continue;

      // IF THESE TWO RECTANGLES LINE UP VERTICALLY, COMBINE THEM
      if ((rect[0]->top    == rect[1]->top) &&
          (rect[0]->bottom == rect[1]->bottom)) {
        rect[0]->left  = min(rect[0]->left ,rect[1]->left);
        rect[0]->right = max(rect[0]->right,rect[1]->right);
        DeleteRect(rect[1]);
        break;
      }

      // OTHERWISE, IF THEY DON'T LINE UP BUT DO AT LEAST TOUCH EACH OTHER,
      // THEN SPLIT THEM AS NECESSARY TO CREATE ONE WIDE RECTANGLE
      else if ((rect[0]->top < rect[1]->bottom) &&
               (rect[1]->top < rect[0]->bottom)) {
        RECT newrect[5] = {{rect[0]->left,
                            rect[0]->top,
                            rect[0]->right,
                            rect[1]->top},
                           {rect[1]->left,
                            rect[1]->top,
                            rect[1]->right,
                            rect[0]->top},
                           {rect[0]->left,
                            rect[1]->bottom,
                            rect[0]->right,
                            rect[0]->bottom},
                           {rect[1]->left,
                            rect[0]->bottom,
                            rect[1]->right,
                            rect[1]->bottom},
                           {min(rect[0]->left  ,rect[1]->left),
                            max(rect[0]->top   ,rect[1]->top),
                            max(rect[0]->right ,rect[1]->right),
                            min(rect[0]->bottom,rect[1]->bottom)}};
        for (DWORD loop = 0; loop < 5; ++loop)
          if (!IsNullRect(&newrect[loop]))
            AddCombinedRect(combinedarray,
                            &newrect[loop]);
        DeleteCombinedRect(combinedarray,
                           loop0);
        DeleteCombinedRect(combinedarray,
                           loop1);
        break;
      }

    }
}

//===========================================================================
static inline BOOL CompareRects (LPCRECT rect1,
                                 LPCRECT rect2) {
  return !memcmp(rect1,rect2,sizeof(RECT));
}

//===========================================================================
static inline void DeleteCombinedRect (ARRAYPTR(RECT) combinedarray,
                                       DWORD          index) {
  LPRECT rect = &(*combinedarray)[index];
  DeleteRect(rect);
}

//===========================================================================
static inline void DeleteRect (LPRECT rect) {
  rect->left   = INT_MAX;
  rect->top    = INT_MAX;
  rect->right  = INT_MAX;
  rect->bottom = INT_MAX;
}

//===========================================================================
static inline void DeleteSourceRect (ARRAYPTR(SOURCE) sourcearray,
                                     DWORD            index) {
  SOURCEPTR sourceptr = &(*sourcearray)[index];
  DeleteRect(&sourceptr->rect);
  sourceptr->param    = NULL;
  sourceptr->sequence = -1;
  sourceptr->flags    = 0;
}

//===========================================================================
static void FindSourceParams (RGNPTR  rgnptr,
                              LPCRECT rect) {

  // IF THE RESULTS FOR THIS SEARCH ARE ALREADY CACHED, JUST RETURN
  if (CompareRects(rect,&rgnptr->foundparamsrect))
    return;

  // OTHERWISE, RESET THE SEARCH RESULTS
  rgnptr->foundparams.SetNumElements(0);

  // FIND ALL SOURCE PARAMETERS WHICH MATCH THE GIVEN RECTANGLE
  DWORD sourcerects = rgnptr->source.NumElements();
  DWORD params      = 0;
  for (DWORD loop1 = 0; loop1 < sourcerects; ++loop1)
    if (CheckForIntersection(rect,&rgnptr->source[loop1].rect)) {
      int  sequence = rgnptr->source[loop1].sequence;
      BOOL found    = FALSE;
      for (DWORD loop2 = 0; loop2 < params; ++loop2) {
        FOUNDPARAMPTR checkptr = &rgnptr->foundparams[loop2];
        if (checkptr->sequence == sequence) {
          found = TRUE;
          break;
        }
      }
      if (!found) {
        FOUNDPARAMPTR newptr = rgnptr->foundparams.NewElement();
        newptr->param    = rgnptr->source[loop1].param;
        newptr->sequence = sequence;
        ++params;
      }
    }

  // SORT THE PARAMETERS
  qsort(rgnptr->foundparams.Ptr(),
        rgnptr->foundparams.NumElements(),
        sizeof(FOUNDPARAM),
        SortFoundParamsCallback);

  // SAVE THIS RECTANGLE AS THE RECTANGLE TO WHICH THE CACHE NOW APPLIES
  CopyMemory(&rgnptr->foundparamsrect,
             rect,
             sizeof(RECT));

}

//===========================================================================
static void FragmentCombinedRectangles (ARRAYPTR(RECT) combinedarray,
                                        DWORD          firstindex,
                                        DWORD          lastindex,
                                        LPCRECT        rect) {
  for (DWORD index = firstindex; index < lastindex; ++index) {
    LPCRECT checkrect = &(*combinedarray)[index];
    if (CheckForIntersection(rect,checkrect)) {
      RECT newrect[4] = {{rect->left,
                          rect->top,
                          rect->right,
                          checkrect->top},
                         {rect->left,
                          checkrect->bottom,
                          rect->right,
                          rect->bottom},
                         {rect->left,
                          max(rect->top,checkrect->top),
                          checkrect->left,
                          min(rect->bottom,checkrect->bottom)},
                         {checkrect->right,
                          max(rect->top,checkrect->top),
                          rect->right,
                          min(rect->bottom,checkrect->bottom)}};
      for (DWORD loop = 0; loop < 4; ++loop)
        if (!IsNullRect(&newrect[loop]))
          FragmentCombinedRectangles(combinedarray,
                                     index+1,
                                     lastindex,
                                     &newrect[loop]);
      return;
    }
  }
  AddCombinedRect(combinedarray,
                  rect);
}

//===========================================================================
static void FragmentSourceRectangles (ARRAYPTR(SOURCE) sourcearray,
                                      DWORD            firstindex,
                                      DWORD            lastindex,
                                      BOOL             previousoverlap,
                                      LPCRECT          rect,
                                      LPVOID           param,
                                      int              sequence) {

  // IF THIS RECTANGLE INTERSECTS ANY OTHER RECTANGLE IN THE ARRAY,
  // BREAK UP EITHER OR BOTH OF THE RECTANGLES TO ELIMINATE OVERLAP,
  // AND CALL THIS FUNCTION RECURSIVELY WITH EACH PIECE.
  BOOL overlapsexisting = previousoverlap;
  for (DWORD index = firstindex; index < lastindex; ++index) {
    LPCRECT checkrect = &(*sourcearray)[index].rect;
    if (CheckForIntersection(rect,checkrect)) {

      // IF THE TWO RECTANGLES ARE IDENTICAL, DON'T TREAT THIS AS AN
      // INTERSECTION
      if (CompareRects(rect,checkrect)) {
        (*sourcearray)[index].flags |= SF_OVERLAPS;
        overlapsexisting = TRUE;
        continue;
      }

      // OTHERWISE, BUILD NEW RECTANGLES WHICH EACH EITHER COMPLETELY
      // OVERLAP OR DON'T OVERLAP AT ALL
      LPCRECT overlaprect[2] = {rect,checkrect};
      int  minleft    = (int)(overlaprect[0]->left   > overlaprect[1]->left);
      int  maxleft    = (int)(overlaprect[1]->left   > overlaprect[0]->left);
      int  mintop     = (int)(overlaprect[0]->top    > overlaprect[1]->top);
      int  maxtop     = (int)(overlaprect[1]->top    > overlaprect[0]->top);
      int  minright   = (int)(overlaprect[0]->right  > overlaprect[1]->right);
      int  maxright   = (int)(overlaprect[1]->right  > overlaprect[0]->right);
      int  minbottom  = (int)(overlaprect[0]->bottom > overlaprect[1]->bottom);
      int  maxbottom  = (int)(overlaprect[1]->bottom > overlaprect[0]->bottom);
      RECT newrect[5] = {{overlaprect[mintop]->left,
                          overlaprect[mintop]->top,
                          overlaprect[mintop]->right,
                          overlaprect[maxtop]->top},
                         {overlaprect[maxbottom]->left,
                          overlaprect[minbottom]->bottom,
                          overlaprect[maxbottom]->right,
                          overlaprect[maxbottom]->bottom},
                         {overlaprect[minleft]->left,
                          overlaprect[maxtop]->top,
                          overlaprect[maxleft]->left,
                          overlaprect[minbottom]->bottom},
                         {overlaprect[minright]->right,
                          overlaprect[maxtop]->top,
                          overlaprect[maxright]->right,
                          overlaprect[minbottom]->bottom},
                         {overlaprect[maxleft]->left,
                          overlaprect[maxtop]->top,
                          overlaprect[minright]->right,
                          overlaprect[minbottom]->bottom}};

      // DETERMINE WHICH NEW RECTANGLES OVERLAP WHICH OF THE ORIGINAL
      // RECTANGLES
      BOOL overlaps[5][2];
      {
        for (DWORD loop1 = 0; loop1 < 5; ++loop1)
          if (IsNullRect(&newrect[loop1]))
            overlaps[loop1][0] = overlaps[loop1][1] = FALSE;
          else
            for (DWORD loop2 = 0; loop2 < 2; ++loop2)
              overlaps[loop1][loop2] = CheckForIntersection(&newrect[loop1],
                                                            overlaprect[loop2]);
      }

      // ADD THE NEW RECTANGLES TO THE ARRAY.  (OUR POINTERS TO THE SOURCE
      // RECTANGLES BECOME INVALID AT THIS POINT BECAUSE THE ARRAY MAY BE
      // RESIZED, CAUSING IT TO MOVE IN MEMORY.)
      for (DWORD loop = 0; loop < 5; ++loop) {
        if (overlaps[loop][0])
          FragmentSourceRectangles(sourcearray,
                                   index+1,
                                   lastindex,
                                   overlapsexisting || overlaps[loop][1],
                                   &newrect[loop],
                                   param,
                                   sequence);
        if (overlaps[loop][1])
          AddSourceRect(sourcearray,
                        &newrect[loop],
                        (*sourcearray)[index].param,
                        (*sourcearray)[index].sequence,
                        ((*sourcearray)[index].flags & ~SF_TEMPMASK)
                          | (overlaps[loop][0] ? SF_OVERLAPS : 0));
      }

      // REMOVE THE ORIGINAL RECTANGLES FROM THE ARRAY AND RETURN
      DeleteSourceRect(sourcearray,index);
      return;

    }
  }

  // IF THIS RECTANGLE DIDN'T PARTIALLY OVERLAP ANY OTHER RECTANGLE,
  // ADD IT TO THE ARRAY, KEEPING TRACK OF WHETHER OR NOT IT FULLY
  // OVERLAPPED ANY OTHER RECTANGLE.
  AddSourceRect(sourcearray,
                rect,
                param,
                sequence,
                SF_ADDING | (overlapsexisting ? SF_OVERLAPS : 0));

}

//===========================================================================
static inline BOOL IsNullRect (LPCRECT rect) {
  return ((rect->left >= rect->right) ||
          (rect->top  >= rect->bottom));
}

//===========================================================================
static void OptimizeSource (ARRAYPTR(SOURCE) sourcearray) {

  // REMOVE NULL RECTANGLES
  DWORD index = 0;
  DWORD numelements;
  while (index < (numelements = sourcearray->NumElements()))
    if (IsNullRect(&(*sourcearray)[index].rect)) {
      CopyMemory(&(*sourcearray)[index],
                 &(*sourcearray)[numelements-1],
                 sizeof(SOURCE));
      sourcearray->SetNumElements(numelements-1);
    }
    else
      ++index;

}

//===========================================================================
static void ProcessBooleanOperation (ARRAYPTR(SOURCE) sourcearray,
                                     LPCRECT          rect,
                                     int              combinemode) {
  for (DWORD index = 0; index < sourcearray->NumElements(); ++index) {
    SOURCEPTR sourceptr = &(*sourcearray)[index];
    BOOL remove = FALSE;
    switch (combinemode) {

      case SRGN_AND:
        remove = !(sourceptr->flags & SF_OVERLAPS);
      break;

      case SRGN_COPY:
        remove = (sourceptr->flags & SF_ADDING);
      break;

      case SRGN_DIFF:
        remove = (sourceptr->flags & (SF_ADDING | SF_OVERLAPS));
      break;

      case SRGN_XOR:
        remove = (sourceptr->flags & SF_OVERLAPS);
      break;

    }
    if (remove)
      DeleteSourceRect(sourcearray,index);
    sourceptr->flags = 0;
  }
}

//===========================================================================
static void ProduceCombinedRectangles (RGNPTR rgnptr) {
  DWORD sourcerects = rgnptr->source.NumElements();
  rgnptr->combined.SetNumElements(0);

  // FRAGMENT THE SOURCE RECTANGLES INTO SMALLER NON-OVERLAPPING RECTANGLES
  {
    SOURCEPTR sourcearray = rgnptr->source.Ptr();
    for (DWORD loop = 0; loop < sourcerects; ++loop)
      if (!(sourcearray[loop].flags & SF_PARAMONLY))
        FragmentCombinedRectangles(&rgnptr->combined,
                                   0,
                                   rgnptr->combined.NumElements(),
                                   &sourcearray[loop].rect);
  }

  // RECOMBINE ADJACENT RECTANGLES
  CombineRectangles(&rgnptr->combined);

  // SORT THE RECTANGLES VERTICALLY
  qsort(rgnptr->combined.Ptr(),
        rgnptr->combined.NumElements(),
        sizeof(RECT),
        SortRectCallback);

  // REMOVE ANY DELETED RECTANGLES (WHICH WILL HAVE BEEN SORTED TO THE
  // BOTTOM OF THE ARRAY)
  DWORD numrects;
  while ((numrects = rgnptr->combined.NumElements()) != 0)
    if (IsNullRect(&rgnptr->combined[--numrects]))
      rgnptr->combined.SetNumElements(numrects);
    else
      break;

}

//===========================================================================
static int __cdecl SortFoundParamsCallback (const void *elem1,
                                            const void *elem2) {
  FOUNDPARAMPTR ptr1 = (FOUNDPARAMPTR)elem1;
  FOUNDPARAMPTR ptr2 = (FOUNDPARAMPTR)elem2;
  return (ptr1->sequence-ptr2->sequence);
}

//===========================================================================
static int __cdecl SortRectCallback (const void *elem1,
                                     const void *elem2) {
  LPRECT rect1 = (LPRECT)elem1;
  LPRECT rect2 = (LPRECT)elem2;
  if (rect1->top == rect2->top)
    return rect1->left-rect2->left;
  else
    return rect1->top-rect2->top;
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
void APIENTRY SRgnClear (HSRGN handle) {
  VALIDATEBEGIN;
  VALIDATE(handle);
  VALIDATEENDVOID;

  // LOCK THE REGION
  HLOCKEDRGN lockedhandle;
  RGNPTR     rgnptr = s_rgntable.Lock(handle,&lockedhandle);
  if (!rgnptr)
    return;

  // CLEAR THE REGION
  ClearRegion(rgnptr);

  // UNLOCK THE REGION
  s_rgntable.Unlock(lockedhandle);

}

//===========================================================================
void APIENTRY SRgnCombineRect (HSRGN   handle,
                               LPCRECT rect,
                               LPVOID  param,
                               int     combinemode) {
  VALIDATEBEGIN;
  VALIDATE(handle);
  VALIDATE(rect);
  VALIDATE(combinemode >= SRGN_MIN);
  VALIDATE(combinemode <= SRGN_MAX);
  VALIDATEENDVOID;

  // LOCK THE REGION
  HLOCKEDRGN lockedhandle;
  RGNPTR     rgnptr = s_rgntable.Lock(handle,&lockedhandle);
  if (!rgnptr)
    return;

  // IF THIS IS A SIMPLE 'OR' OPERATION, SKIP THE ITERSECTION STEP AND JUST
  // ADD THE RECTANGLE TO THE REGION
  if ((combinemode == SRGN_OR) ||
      (combinemode == SRGN_PARAMONLY)) {
    if (!IsNullRect(rect))
      AddSourceRect(&rgnptr->source,
                    rect,
                    param,
                    ++rgnptr->sequence,
                    (combinemode == SRGN_PARAMONLY)
                      ? SF_PARAMONLY
                      : 0);
  }

  // OTHERWISE, PERFORM AN INTERSECTION, PROCESS THE BOOLEAN OPERATOR ON
  // THE RESULTING SOURCE ELEMENTS, AND ADD OR REMOVE THEM AS NECESSARY
  else {
    if (!IsNullRect(rect))
      FragmentSourceRectangles(&rgnptr->source,
                               0,
                               rgnptr->source.NumElements(),
                               FALSE,
                               rect,
                               param,
                               ++rgnptr->sequence);
    ProcessBooleanOperation(&rgnptr->source,
                            rect,
                            combinemode);
    OptimizeSource(&rgnptr->source);
  }

  // MARK THE REGION AS DIRTY
  rgnptr->dirty = TRUE;

  // INVALIDATE THE CACHE OF FOUND PARAMETERS
  DeleteRect(&rgnptr->foundparamsrect);

  // UNLOCK THE REGION
  s_rgntable.Unlock(lockedhandle);

}

//===========================================================================
void APIENTRY SRgnCreate (HSRGN *handle,
                          DWORD  reserved) {
  VALIDATEBEGIN;
  VALIDATEANDBLANK(handle);
  VALIDATE(!reserved);
  VALIDATEENDVOID;

  HLOCKEDRGN lockedhandle;
  RGNPTR     rgnptr = s_rgntable.NewLock(handle,&lockedhandle);
  ClearRegion(rgnptr);
  s_rgntable.Unlock(lockedhandle);
}

//===========================================================================
void APIENTRY SRgnDelete (HSRGN handle) {
  VALIDATEBEGIN;
  VALIDATE(handle);
  VALIDATEENDVOID;

  s_rgntable.Delete(handle);
}

//===========================================================================
void APIENTRY SRgnDestroy () {
  s_rgntable.Destroy();
}

//===========================================================================
void APIENTRY SRgnDuplicate (HSRGN  orighandle,
                             HSRGN *handle,
                             DWORD  reserved) {
  VALIDATEBEGIN;
  VALIDATEANDBLANK(handle);
  VALIDATE(orighandle);
  VALIDATE(!reserved);
  VALIDATEENDVOID;

  HLOCKEDRGN origlockedhandle;
  RGNPTR     origrgnptr = s_rgntable.Lock(orighandle,&origlockedhandle);
  if (!origrgnptr)
    return;

  HLOCKEDRGN lockedhandle;
  RGNPTR     rgnptr = s_rgntable.NewLock(handle,&lockedhandle);
  *rgnptr = *origrgnptr;
  s_rgntable.Unlock(lockedhandle);
  s_rgntable.Unlock(origlockedhandle);
}

//===========================================================================
void APIENTRY SRgnGetBoundingRect (HSRGN  handle,
                                   LPRECT rect) {
  VALIDATEBEGIN;
  VALIDATE(handle);
  VALIDATE(rect);
  VALIDATEENDVOID;

  rect->left   = INT_MAX;
  rect->top    = INT_MAX;
  rect->right  = INT_MIN;
  rect->bottom = INT_MIN;

  // LOCK THE REGION
  HLOCKEDRGN lockedhandle;
  RGNPTR     rgnptr = s_rgntable.Lock(handle,&lockedhandle);
  if (!rgnptr)
    return;

  // ADJUST THE BOUNDING RECTANGLE FOR EACH SOURCE RECTANGLE IN THE REGION
  SOURCEPTR sourcearray = rgnptr->source.Ptr();
  DWORD     sourcerects = rgnptr->source.NumElements();
  for (DWORD loop = 0; loop < sourcerects; ++loop)
    if (!(sourcearray[loop].flags & SF_PARAMONLY)) {
      rect->left   = min(rect->left  ,sourcearray[loop].rect.left  );
      rect->top    = min(rect->top   ,sourcearray[loop].rect.top   );
      rect->right  = max(rect->right ,sourcearray[loop].rect.right );
      rect->bottom = max(rect->bottom,sourcearray[loop].rect.bottom);
    }

  // UNLOCK THE REGION
  s_rgntable.Unlock(lockedhandle);

  // IF THE BOUNDING RECTANGLE IS EMPTY, SET IT TO A NORMALIZED EMPTY
  // RECTANGLE
  if (IsNullRect(rect))
    ZeroMemory(rect,sizeof(RECT));

}

//===========================================================================
void APIENTRY SRgnGetRectParams (HSRGN    handle,
                                 LPCRECT  rect,
                                 DWORD   *numparams,
                                 LPVOID  *buffer) {
  VALIDATEBEGIN;
  VALIDATE(handle);
  VALIDATE(rect);
  VALIDATE(numparams);
  VALIDATEENDVOID;

  // VERIFY THAT THE RECTANGLE IS VALID
  if (IsNullRect(rect)) {
    *numparams = 0;
    return;
  }

  // LOCK THE REGION
  HLOCKEDRGN lockedhandle;
  RGNPTR     rgnptr = s_rgntable.Lock(handle,&lockedhandle);
  if (!rgnptr) {
    *numparams = 0;
    return;
  }

  // COMBINE THE SOURCE ELEMENTS INTO AN ARRAY OF NONOVERLAPPING RECTANGLES
  if (rgnptr->dirty) {
    ProduceCombinedRectangles(rgnptr);
    rgnptr->dirty = FALSE;
  }

  // FIND ALL SOURCE PARAMETERS WHICH OVERLAP THE GIVEN RECTANGLE
  FindSourceParams(rgnptr,rect);

  // DETERMINE THE NUMBER OF SOURCE PARAMETERS TO COPY
  if (buffer)
    *numparams = min(*numparams,rgnptr->foundparams.NumElements());
  else
    *numparams = rgnptr->foundparams.NumElements();

  // COPY THE SOURCE PARAMETERS
  if (buffer) {
    FOUNDPARAMPTR foundarray = rgnptr->foundparams.Ptr();
    for (DWORD loop = 0; loop < *numparams; ++loop)
      *buffer++ = (foundarray++)->param;
  }

  // UNLOCK THE REGION
  s_rgntable.Unlock(lockedhandle);

}

//===========================================================================
void APIENTRY SRgnGetRects (HSRGN   handle,
                            DWORD  *numrects,
                            LPRECT  buffer) {
  VALIDATEBEGIN;
  VALIDATE(handle);
  VALIDATE(numrects);
  VALIDATEENDVOID;

  // LOCK THE REGION
  HLOCKEDRGN lockedhandle;
  RGNPTR     rgnptr = s_rgntable.Lock(handle,&lockedhandle);
  if (!rgnptr) {
    *numrects = 0;
    return;
  }

  // COMBINE THE SOURCE ELEMENTS INTO AN ARRAY OF NONOVERLAPPING RECTANGLES
  if (rgnptr->dirty) {
    ProduceCombinedRectangles(rgnptr);
    rgnptr->dirty = FALSE;
  }

  // DETERMINE THE NUMBER OF RECTANGLES TO COPY
  if (buffer)
    *numrects = min(*numrects,rgnptr->combined.NumElements());
  else
    *numrects = rgnptr->combined.NumElements();

  // COPY THE RECTANGLES
  if (buffer)
    CopyMemory(buffer,
               rgnptr->combined.Ptr(),
               (*numrects)*sizeof(RECT));

  // UNLOCK THE REGION
  s_rgntable.Unlock(lockedhandle);

}

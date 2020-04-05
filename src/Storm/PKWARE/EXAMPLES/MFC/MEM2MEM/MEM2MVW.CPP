/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
// mem2mvw.cpp : implementation of the CMem2memView class
//

#include "stdafx.h"
#include "mem2mem.h"

#include "mem2mdoc.h"
#include "mem2mvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMem2memView

IMPLEMENT_DYNCREATE(CMem2memView, CView)

BEGIN_MESSAGE_MAP(CMem2memView, CView)
   //{{AFX_MSG_MAP(CMem2memView)
      // NOTE - the ClassWizard will add and remove mapping macros here.
      //    DO NOT EDIT what you see in these blocks of generated code!
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMem2memView construction/destruction

CMem2memView::CMem2memView()
{
   // TODO: add construction code here
}

CMem2memView::~CMem2memView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CMem2memView drawing

void CMem2memView::OnDraw(CDC* pDC)
{
   CMem2memDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);

   // TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CMem2memView diagnostics

#ifdef _DEBUG
void CMem2memView::AssertValid() const
{
   CView::AssertValid();
}

void CMem2memView::Dump(CDumpContext& dc) const
{
   CView::Dump(dc);
}

CMem2memDoc* CMem2memView::GetDocument() // non-debug version is inline
{
   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMem2memDoc)));
   return (CMem2memDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMem2memView message handlers

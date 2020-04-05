/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1994,1995 by PKWARE Inc. All Rights Reserved.     ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
// dclview.cpp : implementation of the CDclView class
//

#include "stdafx.h"
#include "dcl.h"

#include "dcldoc.h"
#include "dclview.h"
#include "compdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDclView

IMPLEMENT_DYNCREATE(CDclView, CView)

BEGIN_MESSAGE_MAP(CDclView, CView)
   //{{AFX_MSG_MAP(CDclView)
   ON_COMMAND(COMP_DIALOG, OnDialog)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDclView construction/destruction

CDclView::CDclView()
{
   // TODO: add construction code here
}

CDclView::~CDclView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDclView drawing

void CDclView::OnDraw(CDC* pDC)
{
   CDclDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);

   // TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CDclView diagnostics

#ifdef _DEBUG
void CDclView::AssertValid() const
{
   CView::AssertValid();
}

void CDclView::Dump(CDumpContext& dc) const
{
   CView::Dump(dc);
}

CDclDoc* CDclView::GetDocument() // non-debug version is inline
{
   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDclDoc)));
   return (CDclDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDclView message handlers

void CDclView::OnDialog()
{
   // TODO: Add your command handler code here
   CCompDlg dlg;
   dlg.DoModal();   
}

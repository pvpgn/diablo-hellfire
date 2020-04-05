/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */

// multfvw.cpp : implementation of the CMultfileView class
//

#include "stdafx.h"
#include "multfile.h"

#include "multfdoc.h"
#include "multfvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


char szHelpMsg[] = {
      "To compress multiple files: Select Compress Files from the File "
      "menu, and use the shift and control keys with the mouse to "
      "highlight multiple files. Press Ok after selecting the files. "
      "Enter the path and name of file to compress the selected files "
      "to.\n\n"
      "To uncompress a file: Select Uncompress Files from the File menu, "
      "and enter or select the file to uncompress, then press the Ok "
      "button. The files will be uncompressed in the same directory." };


/////////////////////////////////////////////////////////////////////////////
// CMultfileView

IMPLEMENT_DYNCREATE(CMultfileView, CView)

BEGIN_MESSAGE_MAP(CMultfileView, CView)
   //{{AFX_MSG_MAP(CMultfileView)
      // NOTE - the ClassWizard will add and remove mapping macros here.
      //    DO NOT EDIT what you see in these blocks of generated code!
   ON_MESSAGE( WM_TURN_OFF_HELP, OnTurnOffHelp )
   ON_MESSAGE( WM_TURN_ON_HELP, OnTurnOnHelp )
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultfileView construction/destruction

CMultfileView::CMultfileView()
{
   bDisplayHelp = TRUE;
}

CMultfileView::~CMultfileView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CMultfileView drawing

void CMultfileView::OnDraw(CDC* pDC)
{
   CMultfileDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);

   // TODO: add draw code for native data here
   if( bDisplayHelp )
   {
      RECT rect;

      pDC->SetBkMode( TRANSPARENT );
      GetClientRect( &rect );
      pDC->DrawText( szHelpMsg, -1, &rect, DT_LEFT | DT_WORDBREAK );
   }
}

/////////////////////////////////////////////////////////////////////////////
// CMultfileView diagnostics

#ifdef _DEBUG
void CMultfileView::AssertValid() const
{
   CView::AssertValid();
}

void CMultfileView::Dump(CDumpContext& dc) const
{
   CView::Dump(dc);
}

CMultfileDoc* CMultfileView::GetDocument() // non-debug version is inline
{
   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMultfileDoc)));
   return (CMultfileDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMultfileView message handlers

LRESULT CMultfileView::OnTurnOffHelp( WPARAM wParam, LPARAM lParam )
{
   bDisplayHelp = FALSE;
   InvalidateRect( NULL, TRUE );

   return 0;
}

LRESULT CMultfileView::OnTurnOnHelp( WPARAM wParam, LPARAM lParam )
{
   bDisplayHelp = TRUE;
   InvalidateRect( NULL, TRUE );

   return 0;
}



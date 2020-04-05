/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
// spanview.cpp : implementation of the CSpanView class
//

#include "stdafx.h"
#include "span.h"

#include "spandoc.h"
#include "spanview.h"

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
      "button. The files will be uncompressed to " };

extern char far *UncompressDir;

/////////////////////////////////////////////////////////////////////////////
// CSpanView

IMPLEMENT_DYNCREATE(CSpanView, CView)

BEGIN_MESSAGE_MAP(CSpanView, CView)
   //{{AFX_MSG_MAP(CSpanView)
   ON_MESSAGE( WM_TURN_OFF_HELP, OnTurnOffHelp )
   ON_MESSAGE( WM_TURN_ON_HELP, OnTurnOnHelp )
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpanView construction/destruction

CSpanView::CSpanView()
{
   bDisplayHelp = TRUE;
}

CSpanView::~CSpanView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSpanView drawing

void CSpanView::OnDraw(CDC* pDC)
{
   CSpanDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);

   // TODO: add draw code for native data here
   if( bDisplayHelp )
   {
      RECT rect;
      char *pszHelpString;

      pszHelpString = new char[1024];

      strcpy( pszHelpString, szHelpMsg );
      strcat( pszHelpString, UncompressDir );

      pDC->SetBkMode( TRANSPARENT );
      GetClientRect( &rect );
      pDC->DrawText( pszHelpString, -1, &rect, DT_LEFT | DT_WORDBREAK );
   }
}

/////////////////////////////////////////////////////////////////////////////
// CSpanView diagnostics

#ifdef _DEBUG
void CSpanView::AssertValid() const
{
   CView::AssertValid();
}

void CSpanView::Dump(CDumpContext& dc) const
{
   CView::Dump(dc);
}

CSpanDoc* CSpanView::GetDocument() // non-debug version is inline
{
   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSpanDoc)));
   return (CSpanDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSpanView message handlers

LRESULT CSpanView::OnTurnOffHelp( WPARAM wParam, LPARAM lParam )
{
   bDisplayHelp = FALSE;
   InvalidateRect( NULL, TRUE );

   return 0;
}

LRESULT CSpanView::OnTurnOnHelp( WPARAM wParam, LPARAM lParam )
{
   bDisplayHelp = TRUE;
   InvalidateRect( NULL, TRUE );

   return 0;
}



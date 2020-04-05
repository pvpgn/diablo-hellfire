/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
// spandoc.cpp : implementation of the CSpanDoc class
//

#include "stdafx.h"
#include "span.h"

#include "spandoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpanDoc

IMPLEMENT_DYNCREATE(CSpanDoc, CDocument)

BEGIN_MESSAGE_MAP(CSpanDoc, CDocument)
   //{{AFX_MSG_MAP(CSpanDoc)
      // NOTE - the ClassWizard will add and remove mapping macros here.
      //    DO NOT EDIT what you see in these blocks of generated code!
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpanDoc construction/destruction

CSpanDoc::CSpanDoc()
{
   // TODO: add one-time construction code here
}

CSpanDoc::~CSpanDoc()
{
}

BOOL CSpanDoc::OnNewDocument()
{
   if (!CDocument::OnNewDocument())
      return FALSE;

   // TODO: add reinitialization code here
   // (SDI documents will reuse this document)

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSpanDoc serialization

void CSpanDoc::Serialize(CArchive& ar)
{
   if (ar.IsStoring())
   {
      // TODO: add storing code here
   }
   else
   {
      // TODO: add loading code here
   }
}

/////////////////////////////////////////////////////////////////////////////
// CSpanDoc diagnostics

#ifdef _DEBUG
void CSpanDoc::AssertValid() const
{
   CDocument::AssertValid();
}

void CSpanDoc::Dump(CDumpContext& dc) const
{
   CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSpanDoc commands

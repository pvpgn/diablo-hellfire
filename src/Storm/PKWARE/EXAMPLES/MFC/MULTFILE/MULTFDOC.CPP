/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */

// multfdoc.cpp : implementation of the CMultfileDoc class
//

#include "stdafx.h"
#include "multfile.h"

#include "multfdoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMultfileDoc

IMPLEMENT_DYNCREATE(CMultfileDoc, CDocument)

BEGIN_MESSAGE_MAP(CMultfileDoc, CDocument)
   //{{AFX_MSG_MAP(CMultfileDoc)
      // NOTE - the ClassWizard will add and remove mapping macros here.
      //    DO NOT EDIT what you see in these blocks of generated code!
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultfileDoc construction/destruction

CMultfileDoc::CMultfileDoc()
{
   // TODO: add one-time construction code here
}

CMultfileDoc::~CMultfileDoc()
{
}

BOOL CMultfileDoc::OnNewDocument()
{
   if (!CDocument::OnNewDocument())
      return FALSE;

   // TODO: add reinitialization code here
   // (SDI documents will reuse this document)

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMultfileDoc serialization

void CMultfileDoc::Serialize(CArchive& ar)
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
// CMultfileDoc diagnostics

#ifdef _DEBUG
void CMultfileDoc::AssertValid() const
{
   CDocument::AssertValid();
}

void CMultfileDoc::Dump(CDumpContext& dc) const
{
   CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMultfileDoc commands

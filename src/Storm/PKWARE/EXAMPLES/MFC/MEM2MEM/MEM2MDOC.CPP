/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
// mem2mdoc.cpp : implementation of the CMem2memDoc class
//

#include "stdafx.h"
#include "mem2mem.h"

#include "mem2mdoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMem2memDoc

IMPLEMENT_DYNCREATE(CMem2memDoc, CDocument)

BEGIN_MESSAGE_MAP(CMem2memDoc, CDocument)
   //{{AFX_MSG_MAP(CMem2memDoc)
      // NOTE - the ClassWizard will add and remove mapping macros here.
      //    DO NOT EDIT what you see in these blocks of generated code!
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMem2memDoc construction/destruction

CMem2memDoc::CMem2memDoc()
{
   // TODO: add one-time construction code here
}

CMem2memDoc::~CMem2memDoc()
{
}

BOOL CMem2memDoc::OnNewDocument()
{
   if (!CDocument::OnNewDocument())
      return FALSE;

   // TODO: add reinitialization code here
   // (SDI documents will reuse this document)

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMem2memDoc serialization

void CMem2memDoc::Serialize(CArchive& ar)
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
// CMem2memDoc diagnostics

#ifdef _DEBUG
void CMem2memDoc::AssertValid() const
{
   CDocument::AssertValid();
}

void CMem2memDoc::Dump(CDumpContext& dc) const
{
   CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMem2memDoc commands

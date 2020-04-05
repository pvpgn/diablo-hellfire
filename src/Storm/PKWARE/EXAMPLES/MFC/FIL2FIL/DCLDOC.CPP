/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1994,1995 by PKWARE Inc. All Rights Reserved.     ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
// dcldoc.cpp : implementation of the CDclDoc class
//

#include "stdafx.h"
#include "dcl.h"

#include "dcldoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDclDoc

IMPLEMENT_DYNCREATE(CDclDoc, CDocument)

BEGIN_MESSAGE_MAP(CDclDoc, CDocument)
   //{{AFX_MSG_MAP(CDclDoc)
      // NOTE - the ClassWizard will add and remove mapping macros here.
      //    DO NOT EDIT what you see in these blocks of generated code!
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDclDoc construction/destruction

CDclDoc::CDclDoc()
{
   // TODO: add one-time construction code here
}

CDclDoc::~CDclDoc()
{
}

BOOL CDclDoc::OnNewDocument()
{
   if (!CDocument::OnNewDocument())
      return FALSE;

   // TODO: add reinitialization code here
   // (SDI documents will reuse this document)

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDclDoc serialization

void CDclDoc::Serialize(CArchive& ar)
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
// CDclDoc diagnostics

#ifdef _DEBUG
void CDclDoc::AssertValid() const
{
   CDocument::AssertValid();
}

void CDclDoc::Dump(CDumpContext& dc) const
{
   CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDclDoc commands

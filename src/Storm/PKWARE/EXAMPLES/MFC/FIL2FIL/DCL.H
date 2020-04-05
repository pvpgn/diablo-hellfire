/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1994,1995 by PKWARE Inc. All Rights Reserved.     ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
// dcl.h : main header file for the DCL application
//

#ifndef __AFXWIN_H__
   #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDclApp:
// See dcl.cpp for the implementation of this class
//

class CDclApp : public CWinApp
{
public:
   CDclApp();

// Overrides
   virtual BOOL InitInstance();

// Implementation

   //{{AFX_MSG(CDclApp)
   afx_msg void OnAppAbout();
      // NOTE - the ClassWizard will add and remove member functions here.
      //    DO NOT EDIT what you see in these blocks of generated code !
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

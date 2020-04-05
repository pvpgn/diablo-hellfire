/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
// span.h : main header file for the SPAN application
//

#ifndef __AFXWIN_H__
   #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#define  WM_TURN_OFF_HELP  WM_USER+1
#define  WM_TURN_ON_HELP   WM_USER+2

/////////////////////////////////////////////////////////////////////////////
// CSpanApp:
// See span.cpp for the implementation of this class
//

class CSpanApp : public CWinApp
{
public:
   CSpanApp();

   BOOL OkToExit;

// Overrides
   virtual BOOL InitInstance();

// Implementation

   //{{AFX_MSG(CSpanApp)
   afx_msg void OnAppAbout();
      // NOTE - the ClassWizard will add and remove member functions here.
      //    DO NOT EDIT what you see in these blocks of generated code !
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

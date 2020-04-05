/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1994,1995 by PKWARE Inc. All Rights Reserved.     ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
// compdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCompDlg dialog

#include "pkstruct.h"

class CCompDlg : public CDialog
{
// Construction
public:
   CCompDlg(CWnd* pParent = NULL);  // standard constructor
   
// Dialog Data
   //{{AFX_DATA(CCompDlg)
   enum { IDD = IDD_DIALOG1 };
      // NOTE: the ClassWizard will add data members here
   //}}AFX_DATA

// Implementation
protected:
   virtual void DoDataExchange(CDataExchange* pDX);   // DDX/DDV support

   // Generated message map functions
   //{{AFX_MSG(CCompDlg)
   afx_msg void OnCompressButton();
   afx_msg void OnTestButton();
   afx_msg void OnClearButton();
   virtual void OnCancel();
   virtual void PKCompressFile();         // Added by PKWARE
   virtual void PKExtractFile();          // Added by PKWARE
   virtual void ClearMessages();          // Added by PKWARE
   virtual void PKLBMessage(char *);      // Added by PKWARE
   afx_msg void OnDebugButton();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
   BOOL DebugMessages;                    // Added by PKWARE
   unsigned long InputCRC;                // Added by PKWARE
   IOFILEBLOCK FileIOBlock;               // Added by PKWARE
};

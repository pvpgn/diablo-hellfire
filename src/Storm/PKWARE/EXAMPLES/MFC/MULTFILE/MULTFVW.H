/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */

// multfvw.h : interface of the CMultfileView class
//
/////////////////////////////////////////////////////////////////////////////

class CMultfileView : public CView
{
private:
   BOOL bDisplayHelp;

protected: // create from serialization only
   CMultfileView();
   DECLARE_DYNCREATE(CMultfileView)

// Attributes
public:
   CMultfileDoc* GetDocument();

// Operations
public:

// Implementation
public:
   virtual ~CMultfileView();
   virtual void OnDraw(CDC* pDC);  // overridden to draw this view
   afx_msg LRESULT OnTurnOffHelp( WPARAM wParam, LPARAM lParam );
   afx_msg LRESULT OnTurnOnHelp( WPARAM wParam, LPARAM lParam );
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
   //{{AFX_MSG(CMultfileView)
      // NOTE - the ClassWizard will add and remove member functions here.
      //    DO NOT EDIT what you see in these blocks of generated code !
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in multfvw.cpp
inline CMultfileDoc* CMultfileView::GetDocument()
   { return (CMultfileDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

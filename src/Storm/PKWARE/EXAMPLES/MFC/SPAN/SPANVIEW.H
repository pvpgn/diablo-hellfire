/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
// spanview.h : interface of the CSpanView class
//
/////////////////////////////////////////////////////////////////////////////

class CSpanView : public CView
{
private:
   BOOL bDisplayHelp;

protected: // create from serialization only
   CSpanView();
   DECLARE_DYNCREATE(CSpanView)

// Attributes
public:
   CSpanDoc* GetDocument();

// Operations
public:

// Implementation
public:
   virtual ~CSpanView();
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
   //{{AFX_MSG(CSpanView)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in spanview.cpp
inline CSpanDoc* CSpanView::GetDocument()
   { return (CSpanDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1994,1995 by PKWARE Inc. All Rights Reserved.     ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
// dclview.h : interface of the CDclView class
//
/////////////////////////////////////////////////////////////////////////////

class CDclView : public CView
{
protected: // create from serialization only
   CDclView();
   DECLARE_DYNCREATE(CDclView)

// Attributes
public:
   CDclDoc* GetDocument();

// Operations
public:

// Implementation
public:
   virtual ~CDclView();
   virtual void OnDraw(CDC* pDC);  // overridden to draw this view
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
   //{{AFX_MSG(CDclView)
   afx_msg void OnDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in dclview.cpp
inline CDclDoc* CDclView::GetDocument()
   { return (CDclDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
// mem2mvw.h : interface of the CMem2memView class
//
/////////////////////////////////////////////////////////////////////////////

class CMem2memView : public CView
{
protected: // create from serialization only
   CMem2memView();
   DECLARE_DYNCREATE(CMem2memView)

// Attributes
public:
   CMem2memDoc* GetDocument();

// Operations
public:

// Implementation
public:
   virtual ~CMem2memView();
   virtual void OnDraw(CDC* pDC);  // overridden to draw this view
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
   //{{AFX_MSG(CMem2memView)
      // NOTE - the ClassWizard will add and remove member functions here.
      //    DO NOT EDIT what you see in these blocks of generated code !
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in mem2mvw.cpp
inline CMem2memDoc* CMem2memView::GetDocument()
   { return (CMem2memDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

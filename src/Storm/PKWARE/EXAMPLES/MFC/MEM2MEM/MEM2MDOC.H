/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
// mem2mdoc.h : interface of the CMem2memDoc class
//
/////////////////////////////////////////////////////////////////////////////

class CMem2memDoc : public CDocument
{
protected: // create from serialization only
   CMem2memDoc();
   DECLARE_DYNCREATE(CMem2memDoc)

// Attributes
public:
// Operations
public:

// Implementation
public:
   virtual ~CMem2memDoc();
   virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

protected:
   virtual BOOL OnNewDocument();

// Generated message map functions
protected:
   //{{AFX_MSG(CMem2memDoc)
      // NOTE - the ClassWizard will add and remove member functions here.
      //    DO NOT EDIT what you see in these blocks of generated code !
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

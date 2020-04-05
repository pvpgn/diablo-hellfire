/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
// mainfrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
   CMainFrame();
   DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Implementation
public:
   virtual ~CMainFrame();
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
   CStatusBar  m_wndStatusBar;
   CToolBar    m_wndToolBar;

// Generated message map functions
protected:
   //{{AFX_MSG(CMainFrame)
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnCmpAscii();
   afx_msg void OnUpdateCmpAscii(CCmdUI* pCmdUI);
   afx_msg void OnCmpBinary();
   afx_msg void OnUpdateCmpBinary(CCmdUI* pCmdUI);
   afx_msg void OnCompressFiles();
   afx_msg void OnUpdateCompressFiles(CCmdUI* pCmdUI);
   afx_msg void OnDictSize1024();
   afx_msg void OnUpdateDictSize1024(CCmdUI* pCmdUI);
   afx_msg void OnDictSize2048();
   afx_msg void OnUpdateDictSize2048(CCmdUI* pCmdUI);
   afx_msg void OnDictSize4096();
   afx_msg void OnUpdateDictSize4096(CCmdUI* pCmdUI);
   afx_msg void OnUncompressFiles();
   afx_msg void OnUpdateUncompressFiles(CCmdUI* pCmdUI);
   afx_msg void OnClose();
   afx_msg BOOL OnQueryEndSession();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

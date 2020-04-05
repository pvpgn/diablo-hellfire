/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */

// mainfrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include <stdio.h>
#include "multfile.h"

#include "mainfrm.h"
#include "dcl.h"
#include "multfdlg.h"
#include "implode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern UINT DataType;               // GLOBAL FOR DATA TYPE FOR COMPRESSION
extern UINT DictSize;               // GLOBAL FOR DICTIONARY SIZE FOR COMPRESSION

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
   //{{AFX_MSG_MAP(CMainFrame)
   ON_WM_CREATE()
   ON_WM_QUERYENDSESSION()
   ON_WM_CLOSE()
   ON_COMMAND(IDM_COMPRESS_FILES, OnCompressFiles)
   ON_COMMAND(IDM_UNCOMPRESS_FILES, OnUncompressFiles)
   ON_UPDATE_COMMAND_UI(IDM_COMPRESS_FILES, OnUpdateCompressFiles)
   ON_UPDATE_COMMAND_UI(IDM_UNCOMPRESS_FILES, OnUpdateUncompressFiles)
   ON_COMMAND(IDM_CMP_ASCII, OnCmpAscii)
   ON_COMMAND(IDM_CMP_BINARY, OnCmpBinary)
   ON_COMMAND(IDM_DICT_SIZE_1024, OnDictSize1024)
   ON_COMMAND(IDM_DICT_SIZE_2048, OnDictSize2048)
   ON_COMMAND(IDM_DICT_SIZE_4096, OnDictSize4096)
   ON_UPDATE_COMMAND_UI(IDM_CMP_ASCII, OnUpdateCmpAscii)
   ON_UPDATE_COMMAND_UI(IDM_CMP_BINARY, OnUpdateCmpBinary)
   ON_UPDATE_COMMAND_UI(IDM_DICT_SIZE_1024, OnUpdateDictSize1024)
   ON_UPDATE_COMMAND_UI(IDM_DICT_SIZE_2048, OnUpdateDictSize2048)
   ON_UPDATE_COMMAND_UI(IDM_DICT_SIZE_4096, OnUpdateDictSize4096)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars

// toolbar buttons - IDs are command buttons
static UINT BASED_CODE buttons[] =
{
   // same order as in the bitmap 'toolbar.bmp'
   ID_FILE_NEW,
   ID_FILE_OPEN,
   ID_FILE_SAVE,
      ID_SEPARATOR,
   ID_EDIT_CUT,
   ID_EDIT_COPY,
   ID_EDIT_PASTE,
      ID_SEPARATOR,
   ID_FILE_PRINT,
   ID_APP_ABOUT,
};

static UINT BASED_CODE indicators[] =
{
   ID_SEPARATOR,           // status line indicator
   ID_INDICATOR_CAPS,
   ID_INDICATOR_NUM,
   ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
   // TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
      return -1;

   if (!m_wndStatusBar.Create(this) ||
       !m_wndStatusBar.SetIndicators(indicators,
        sizeof(indicators)/sizeof(UINT)))
   {
      TRACE("Failed to create status bar\n");
      return -1;      // fail to create
   }

   return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
   CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
   CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnCompressFiles()
{
   int rc;
   HWND hWnd;

   // GET HANDLE TO WINDOW AND INSTANCE HANDLE
   hWnd = CWnd::GetSafeHwnd();

   // TURN OFF HELP MESSAGE SCREEN AND CLEAR SCREEN
   SendMessageToDescendants( WM_TURN_OFF_HELP );
   
   // CREATE FILE DIALOG THAT CAN USE CAN SELECT MULTIPLE FILES
   CMultiSelFileDialog *FileDlg = new CMultiSelFileDialog( this );
   
   // IF USER PRESSED OK BUTTON
   if( (rc = FileDlg->DoModal()) == IDOK )
   {
      // CREATE SAVE AS FILE DIALOG
      CFileDialog SaveFileDlg( FALSE, "MCF", "*.MCF",
             OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, 
             "Mult. Compressed Files (*.MCF) |*.MCF ||", this );

      // GET FILENAME OF FILE TO PUT COMPRESSED FILES IN
      if( (SaveFileDlg.DoModal()) == IDOK )
      {
         BOOL GotFilenameOk,
              bError = FALSE,
              CreateMcfFile;
         UINT nNumCmpFiles = 0;
         char szFilename[13];             // BUFFER FOR FILENAME ONLY
         char szFullPathname[128];        // BUFFER FOR FULL PATH FOR FILE
         char szOutBuff[64];              // TEMP OUTPUT BUFFER

         // SET FLAG TO PREVENT EXITING IN THE MIDDLE OF THE COMPRESSION
         ((CMultfileApp *) AfxGetApp())->OkToExit = FALSE;

         // GET DC AND SET THE TEXT BACKGROUND COLOR TO WINDOW BACKGROUND COLOR
         CClientDC dc(this);
         COLORREF bkGroundColor = dc.GetPixel( 0, 0 );
         dc.SetBkColor( bkGroundColor );

         wsprintf( szOutBuff, "Compressing to: %s          ", 
                   (LPSTR) (const char *) SaveFileDlg.GetPathName() );
         dc.TextOut( 10,20, szOutBuff );

         // SET CREATE .MCF FILE FLAG TO TRUE, SO THAT THE FIRST TIME
         // CompressFileToMCF IS CALLED THE .MCF WILL BE CREATED INSTEAD
         // OF APPENDED TO
         CreateMcfFile = TRUE;

         // GET THE FIRST FILENAME IN THE LIST
         GotFilenameOk = FileDlg->GetFirstFilename( szFullPathname, 
                                                    sizeof(szFullPathname),
                                                    szFilename );
         // WHILE GOT A FILENAME FROM THE LIST
         while( GotFilenameOk )
         {
            // COMPRESS THE FILE AND ADD IT TO THE .MCF FILE
            if( !CompressFileToMCF( hWnd, &dc,  
                        (LPSTR) (const char *) SaveFileDlg.GetPathName(),
                        szFullPathname, szFilename, CreateMcfFile ) )
            {
               // ERROR OCCURRED SO DELETE THE .MCF FILE
               remove( (const char *) SaveFileDlg.GetPathName() );
               bError = TRUE;
               break;
            }
            // INCREMENT TOTAL
            nNumCmpFiles++;

            // RESET .MCF FILE FLAG SO THAT .MCF FILE WILL NOT BE CREATED
            CreateMcfFile = FALSE;

            // GET THE FIRST FILENAME IN THE LIST
            GotFilenameOk = FileDlg->GetNextFilename( szFullPathname, 
                                                      sizeof(szFullPathname),
                                                      szFilename );
         }

         // IF THERE WAS NOT ERROR, THEN DISPLAY MESSAGE
         if( !bError )
         {
            wsprintf( szOutBuff, "Compressed %u file(s)", nNumCmpFiles );
            MessageBox( szOutBuff );
         }

         // DONE WITH COMPRESION SO ALLOW THE USER TO EXIT
         ((CMultfileApp *) AfxGetApp())->OkToExit = TRUE;
      }
   }

   // CLEAN-UP
   delete FileDlg;

   // TURN ON HELP MESSAGE AND CLEAR SCREEN
   SendMessageToDescendants( WM_TURN_ON_HELP );
}

void CMainFrame::OnUncompressFiles()
{
   HWND hWnd;

   hWnd = CWnd::GetSafeHwnd();

   // TURN OFF HELP MESSAGE AND CLEAR SCREEN
   SendMessageToDescendants( WM_TURN_OFF_HELP );
   
   // OPENFILENAME
   CFileDialog OpenFileDlg( TRUE, "MCF", "*.MCF",
          OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
          "Mult. Compressed Files (*.MCF) |*.MCF ||", this );

   // GET FILENAME OF FILE TO PUT COMPRESSED FILES IN
   if( (OpenFileDlg.DoModal()) == IDOK )
   {
      char szOutBuff[64];              // TEMP OUTPUT BUFFER

      // SET FLAG TO PREVENT EXITING IN THE MIDDLE OF THE UNCOMPRESSION
      ((CMultfileApp *) AfxGetApp())->OkToExit = FALSE;

      // GET DC AND SET THE TEXT BACKGROUND COLOR TO WINDOW BACKGROUND COLOR
      CClientDC dc(this);
      COLORREF bkGroundColor = dc.GetPixel( 0, 0 );
      dc.SetBkColor( bkGroundColor );

      wsprintf( szOutBuff, "Uncompressing: %s          ",
                (LPSTR) (const char *) OpenFileDlg.GetPathName() );
      dc.TextOut( 10,20, szOutBuff );

      // UNCOMPRESS THE FILE
      UncompressMcfFile( hWnd, &dc, (LPSTR) (const char *) OpenFileDlg.GetPathName() );
                         
      // DONE WITH UNCOMPRESION SO ALLOW THE USER TO EXIT
      ((CMultfileApp *) AfxGetApp())->OkToExit = TRUE;
   }

   // TURN ON HELP MESSAGE AND CLEAR SCREEN
   SendMessageToDescendants( WM_TURN_ON_HELP );
}

BOOL CMainFrame::OnQueryEndSession()
{
   if (!CFrameWnd::OnQueryEndSession())
      return FALSE;
   
   // RETURN FALSE IF CANNOT EXIT RIGHT NOW
   if( !((CMultfileApp *) AfxGetApp())->OkToExit )
      return FALSE;
   
   return TRUE;
}

void CMainFrame::OnClose()
{
   // RETURN IF CANNOT EXIT RIGHT NOW
   if( !((CMultfileApp *) AfxGetApp())->OkToExit )
      return;
   
   CFrameWnd::OnClose();
}

void CMainFrame::OnUpdateCompressFiles(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( ((CMultfileApp *) AfxGetApp())->OkToExit );
}

void CMainFrame::OnUpdateUncompressFiles(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( ((CMultfileApp *) AfxGetApp())->OkToExit );
}


void CMainFrame::OnCmpAscii()
{
   DataType = CMP_ASCII;
}

void CMainFrame::OnCmpBinary()
{
   DataType = CMP_BINARY;
}

void CMainFrame::OnDictSize1024()
{
   DictSize = 1024;
}

void CMainFrame::OnDictSize2048()
{
   DictSize = 2048;
}

void CMainFrame::OnDictSize4096()
{
   DictSize = 4096;
}

void CMainFrame::OnUpdateCmpAscii(CCmdUI* pCmdUI)
{
   // IF PROGRAM IS COMPRESSING OR UNCOMPRESSING RIGHT NOW
   if( !((CMultfileApp *) AfxGetApp())->OkToExit )
   {
      pCmdUI->Enable( FALSE );
      return;
   }

   if( DataType == CMP_ASCII )
   {
      pCmdUI->SetCheck( 1 );
   }
   else
   {
      pCmdUI->SetCheck( 0 );
   }
}

void CMainFrame::OnUpdateCmpBinary(CCmdUI* pCmdUI)
{
   // IF PROGRAM IS COMPRESSING OR UNCOMPRESSING RIGHT NOW
   if( !((CMultfileApp *) AfxGetApp())->OkToExit )
   {
      pCmdUI->Enable( FALSE );
      return;
   }
   
   if( DataType == CMP_BINARY )
   {
      pCmdUI->SetCheck( 1 );
   }
   else
   {
      pCmdUI->SetCheck( 0 );
   }
}

void CMainFrame::OnUpdateDictSize1024(CCmdUI* pCmdUI)
{
   // IF PROGRAM IS COMPRESSING OR UNCOMPRESSING RIGHT NOW
   if( !((CMultfileApp *) AfxGetApp())->OkToExit )
   {
      pCmdUI->Enable( FALSE );
      return;
   }
   
   if( DictSize == 1024 )
   {
      pCmdUI->SetCheck( 1 );
   }
   else
   {
      pCmdUI->SetCheck( 0 );
   }
}

void CMainFrame::OnUpdateDictSize2048(CCmdUI* pCmdUI)
{
   // IF PROGRAM IS COMPRESSING OR UNCOMPRESSING RIGHT NOW
   if( !((CMultfileApp *) AfxGetApp())->OkToExit )
   {
      pCmdUI->Enable( FALSE );
      return;
   }
   
   if( DictSize == 2048 )
   {
      pCmdUI->SetCheck( 1 );
   }
   else
   {
      pCmdUI->SetCheck( 0 );
   }
}

void CMainFrame::OnUpdateDictSize4096(CCmdUI* pCmdUI)
{
   // IF PROGRAM IS COMPRESSING OR UNCOMPRESSING RIGHT NOW
   if( !((CMultfileApp *) AfxGetApp())->OkToExit )
   {
      pCmdUI->Enable( FALSE );
      return;
   }
   
   if( DictSize == 4096 )
   {
      pCmdUI->SetCheck( 1 );
   }
   else
   {
      pCmdUI->SetCheck( 0 );
   }
}

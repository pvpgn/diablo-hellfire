/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */

#include <owl\owlpch.h>
#include <owl\opensave.h>
#pragma hdrstop

#include "spanapp.h"
#include "spnedtvw.h"                        // Definition of client class.
#include "spnabtdl.h"                        // Definition of about dialog.
#include "multfdlg.h"
#include "dcl.h"
#include "implode.h"

#include <stdio.h>

//{{spanApp Implementation}}

//{{DOC_VIEW}}
DEFINE_DOC_TEMPLATE_CLASS(TFileDocument, spanEditView, DocType1);
//{{DOC_VIEW_END}}

//{{DOC_MANAGER}}
DocType1 __dvt1("All Files (*.*)", "*.*", 0, "TXT", dtAutoDelete | dtUpdateDir);
//{{DOC_MANAGER_END}}


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(spanApp, TApplication)
//{{spanAppRSP_TBL_BEGIN}}
    EV_OWLVIEW(dnCreate, EvNewView),
    EV_OWLVIEW(dnClose,  EvCloseView),
    EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
//{{spanAppRSP_TBL_END}}
END_RESPONSE_TABLE;


extern UINT DataType;               // GLOBAL FOR DATA TYPE FOR COMPRESSION
extern UINT DictSize;               // GLOBAL FOR DICTIONARY SIZE FOR COMPRESSION

// THE UNCOMPRESS DIRECTORY MUST END WITH A BACKSLASH
char far *UncompressDir = "C:\\TEMP\\";

// FLAG TO DETERMINE IF CURRENTLY COMPRESSING OR UNCOMPRESSING
static BOOL bActive = FALSE;

//////////////////////////////////////////////////////////
// spanApp
// =====
//
spanApp::spanApp () : TApplication("span")
{
    SetDocManager(new TDocManager(dmSDI, this));

    // INSERT>> Your constructor code here.
}


spanApp::~spanApp ()
{
    // INSERT>> Your destructor code here.
}




//////////////////////////////////////////////////////////
// spanApp
// =====
// Application intialization.
//
void spanApp::InitMainWindow ()
{
    if (nCmdShow != SW_HIDE)
        nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWNORMAL : nCmdShow;

    SDIDecFrame *frame = new SDIDecFrame(0, GetName(), 0, true, this);

    //
    // Assign ICON w/ this application.
    //
    frame->SetIcon(this, IDI_SDIAPPLICATION);

    //
    // Menu associated with window and accelerator table associated with table.
    //
    frame->AssignMenu(SDI_MENU);

    //
    // Associate with the accelerator table.
    //
    frame->Attr.AccelTable = SDI_MENU;


    TStatusBar *sb = new TStatusBar(frame, TGadget::Recessed,
                                    TStatusBar::CapsLock        |
                                    TStatusBar::NumLock         |
                                    TStatusBar::ScrollLock      |
                                    TStatusBar::Overtype);
    frame->Insert(*sb, TDecoratedFrame::Bottom);
  
    SetMainWindow(frame);

    frame->SetMenuDescr(TMenuDescr(SDI_MENU));

}


//////////////////////////////////////////////////////////
// spanApp
// =====
// Response Table handlers:
//
void spanApp::EvNewView (TView& view)
{
    GetMainWindow()->SetClientWindow(view.GetWindow());
    if (!view.IsOK())
        GetMainWindow()->SetClientWindow(0);
    else if (view.GetViewMenu())
        GetMainWindow()->MergeMenu(*view.GetViewMenu());
}


void spanApp::EvCloseView (TView&)
{
    GetMainWindow()->SetClientWindow(0);
    GetMainWindow()->SetCaption("span");
}

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(SDIDecFrame, TDecoratedFrame)
//{{SDIDecFrameRSP_TBL_BEGIN}}
    EV_COMMAND(CM_COMPRESS_FILES, OnCompressFiles),
    EV_COMMAND_ENABLE(CM_COMPRESS_FILES, OnCompressFilesEnable),
    EV_COMMAND(CM_UNCOMPRESS_FILES, OnUncompressFiles),
    EV_COMMAND_ENABLE(CM_UNCOMPRESS_FILES, OnUncompressFilesEnable),
    EV_COMMAND(CM_COMP_TYPE_ASCII, OnCompTypeAscii),
    EV_COMMAND_ENABLE(CM_COMP_TYPE_ASCII, OnCompTypeAsciiEnable),
    EV_COMMAND(CM_COMP_TYPE_BINARY, OnCompTypeBinary),
    EV_COMMAND_ENABLE(CM_COMP_TYPE_BINARY, OnCompTypeBinaryEnable),
    EV_COMMAND(CM_DICT_SIZE_1024, OnDictSize1024),
    EV_COMMAND_ENABLE(CM_DICT_SIZE_1024, OnDictSize1024Enable),
    EV_COMMAND(CM_DICT_SIZE_2048, OnDictSize2048),
    EV_COMMAND_ENABLE(CM_DICT_SIZE_2048, OnDictSize2048Enable),
    EV_COMMAND(CM_DICT_SIZE_4096, OnDictSize4096),
    EV_COMMAND_ENABLE(CM_DICT_SIZE_4096, OnDictSize4096Enable),
//{{SDIDecFrameRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{SDIDecFrame Implementation}}


SDIDecFrame::SDIDecFrame (TWindow *parent, const char far *title, TWindow *clientWnd, bool trackMenuSelection, TModule *module)
    : TDecoratedFrame(parent, title, clientWnd, trackMenuSelection, module)
{
}


SDIDecFrame::~SDIDecFrame ()
{
    // INSERT>> Your destructor code here.

}


//////////////////////////////////////////////////////////
// spanApp
// ===========
// Menu Help About span.exe command
void spanApp::CmHelpAbout ()
{
    //
    // Show the modal dialog.
    //
    spanAboutDlg(GetMainWindow()).Execute();
}


int OwlMain (int , char* [])
{
    try {
        spanApp    app;
        return app.Run();
    }
    catch (xmsg& x) {
        ::MessageBox(0, x.why().c_str(), "Exception", MB_OK);
    }

    return -1;
}

void SDIDecFrame::OnCompressFiles ()
{
   // TURN OFF HELP MESSAGE AND CLEAR SCREEN
// ChildBroadcastMessage( WM_TURN_OFF_HELP, 0, 0L );

   TOpenSaveDialog::TData data( OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|
            OFN_PATHMUSTEXIST|OFN_NOCHANGEDIR|OFN_ALLOWMULTISELECT,
            "All Files (*.*)|*.*||", 0, "", "*" );

   MultiSelFileDialog FileDlg( this, data );

   if( FileDlg.Execute() == IDOK )
   {
      TOpenSaveDialog::TData SaveData( OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|
               OFN_NOCHANGEDIR|OFN_OVERWRITEPROMPT,
               "Mult. Compressed Files (*.MCF)|*.MCF||", 0, "*.MCF", "MCF" );

      if( TFileSaveDialog( this, SaveData ).Execute() == IDOK )
      {
         BOOL GotFilenameOk,
              bError = FALSE,
              CreateMcfFile;
         UINT nNumCmpFiles = 0;
         char szFilename[13];             // BUFFER FOR FILENAME ONLY
         char szFullPathname[128];        // BUFFER FOR FULL PATH FOR FILE
         char szOutBuff[64];              // TEMP OUTPUT BUFFER

         // GET DC AND SET THE TEXT BACKGROUND COLOR TO WINDOW BACKGROUND COLOR
         TWindow *ClientWnd = GetClientWindow();
         TWindowDC dc( ClientWnd->HWindow );
         TColor bkGroundColor = dc.GetPixel( 0, 0 );
         dc.SetBkColor( bkGroundColor );

         // SET FLAG TO PREVENT EXITING IN THE MIDDLE OF THE COMPRESSION
         bActive = TRUE;

         wsprintf( szOutBuff, "Compressing to: %s          ", (LPSTR)
                   SaveData.FileName );
         dc.TextOut( 10,20, szOutBuff );

         // SET CREATE .MCF FILE FLAG TO TRUE, SO THAT THE FIRST TIME
         // CompressFileToMCF IS CALLED THE .MCF WILL BE CREATED INSTEAD
         // OF APPENDED TO
         CreateMcfFile = TRUE;

         // GET THE FIRST FILENAME IN THE LIST
         GotFilenameOk = FileDlg.GetFirstFilename( szFullPathname,
                                                   sizeof(szFullPathname),
                                                   szFilename );
         // WHILE GOT A FILENAME FROM THE LIST
         while( GotFilenameOk )
         {
            // COMPRESS THE FILE AND ADD IT TO THE .MCF FILE
            if( !CompressFileToMCF( ClientWnd->HWindow, &dc,
                                    SaveData.FileName, szFullPathname,
                                    szFilename, CreateMcfFile ) )
            {
               // ERROR OCCURRED SO DELETE THE .MCF FILE
               remove( SaveData.FileName );
               bError = TRUE;
               break;
            }
            // INCREMENT TOTAL
            nNumCmpFiles++;

            // RESET .MCF FILE FLAG SO THAT .MCF FILE WILL NOT BE CREATED
            CreateMcfFile = FALSE;

            // GET THE FIRST FILENAME IN THE LIST
            GotFilenameOk = FileDlg.GetNextFilename( szFullPathname,
                                                     sizeof(szFullPathname),
                                                     szFilename );
         }

         // IF THERE WAS NOT ERROR, THEN DISPLAY MESSAGE
         if( !bError )
         {
            wsprintf( szOutBuff, "Compressed %u file(s)", nNumCmpFiles );
            MessageBox( szOutBuff, "Span" );
         }

         // DONE WITH COMPRESION SO ALLOW THE USER TO EXIT
         bActive = FALSE;
      }
   }

   // TURN OFF HELP MESSAGE AND CLEAR SCREEN
// ChildBroadcastMessage( WM_TURN_ON_HELP, 0, 0L );
}


void SDIDecFrame::OnUncompressFiles ()
{
   // TURN OFF HELP MESSAGE AND CLEAR SCREEN
// ChildBroadcastMessage( WM_TURN_OFF_HELP, 0, 0L );

   TOpenSaveDialog::TData data( OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|
            OFN_PATHMUSTEXIST|OFN_NOCHANGEDIR,
            "Mult. Compressed Files (*.MCF)|*.MCF||", 0, "*.MCF", "MCF" );

   if( TFileOpenDialog( this, data ).Execute() == IDOK )
   {
      char szOutBuff[64];              // TEMP OUTPUT BUFFER

      // SET FLAG TO PREVENT EXITING IN THE MIDDLE OF THE UNCOMPRESSION
      bActive = TRUE;

      // GET DC AND SET THE TEXT BACKGROUND COLOR TO WINDOW BACKGROUND COLOR
      TWindow *ClientWnd = GetClientWindow();
      TWindowDC dc( ClientWnd->HWindow );
      TColor bkGroundColor = dc.GetPixel( 0, 0 );
      dc.SetBkColor( bkGroundColor );

      wsprintf( szOutBuff, "Uncompressing: %s          ",(LPSTR) data.FileName );
      dc.TextOut( 10,20, szOutBuff );

      // UNCOMPRESS THE FILE
      UncompressMcfFile( ClientWnd->HWindow, &dc, data.FileName, UncompressDir );

      // DONE WITH UNCOMPRESION SO ALLOW THE USER TO EXIT
      bActive = FALSE;
   }

   // TURN OFF HELP MESSAGE AND CLEAR SCREEN
// ChildBroadcastMessage( WM_TURN_ON_HELP, 0, 0L );
}



void SDIDecFrame::OnCompressFilesEnable (TCommandEnabler &tce)
{
   tce.Enable( TRUE && !bActive );
}


void SDIDecFrame::OnUncompressFilesEnable (TCommandEnabler &tce)
{
   tce.Enable( TRUE && !bActive );
}


void SDIDecFrame::OnCompTypeAscii ()
{
   DataType = CMP_ASCII;
}


void SDIDecFrame::OnCompTypeAsciiEnable (TCommandEnabler &tce)
{
   tce.Enable( (DataType != CMP_ASCII) && !bActive );
}


void SDIDecFrame::OnCompTypeBinary ()
{
   DataType = CMP_BINARY;
}


void SDIDecFrame::OnCompTypeBinaryEnable (TCommandEnabler &tce)
{
   tce.Enable( (DataType != CMP_BINARY) && !bActive );
}


void SDIDecFrame::OnDictSize1024 ()
{
   DictSize = 1024;
}


void SDIDecFrame::OnDictSize1024Enable (TCommandEnabler &tce)
{
   tce.Enable( (DictSize != 1024) && !bActive );
}


void SDIDecFrame::OnDictSize2048 ()
{
   DictSize = 2048;
}


void SDIDecFrame::OnDictSize2048Enable (TCommandEnabler &tce)
{
   tce.Enable( (DictSize != 2048) && !bActive );
}


void SDIDecFrame::OnDictSize4096 ()
{
   DictSize = 4096;
}


void SDIDecFrame::OnDictSize4096Enable (TCommandEnabler &tce)
{
   tce.Enable( (DictSize != 4096) && !bActive );
}


bool SDIDecFrame::CanClose ()
{
   bool result;

   if( bActive )
      return FALSE;

   result = TDecoratedFrame::CanClose();

   return result;
}




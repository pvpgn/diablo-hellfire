/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */

#include <owl\owlpch.h>
#pragma hdrstop

#include "mm2mmapp.h"
#include "mm2mmedv.h"                        // Definition of client class.
#include "mm2mmabd.h"                        // Definition of about dialog.
#include "dcl.h"

//{{mem2memApp Implementation}}



//{{DOC_VIEW}}
DEFINE_DOC_TEMPLATE_CLASS(TFileDocument, mem2memEditView, DocType1);
//{{DOC_VIEW_END}}

//{{DOC_MANAGER}}
DocType1 __dvt1("All Files (*.*)", "*.*", 0, "TXT", dtAutoDelete | dtUpdateDir);
//{{DOC_MANAGER_END}}


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(mem2memApp, TApplication)
//{{mem2memAppRSP_TBL_BEGIN}}
    EV_OWLVIEW(dnCreate, EvNewView),
    EV_OWLVIEW(dnClose,  EvCloseView),
    EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
//{{mem2memAppRSP_TBL_END}}
END_RESPONSE_TABLE;


//////////////////////////////////////////////////////////
// mem2memApp
// =====
//
mem2memApp::mem2memApp () : TApplication("mem2mem")
{
    SetDocManager(new TDocManager(dmSDI, this));

    // INSERT>> Your constructor code here.
}


mem2memApp::~mem2memApp ()
{
    // INSERT>> Your destructor code here.
}




//////////////////////////////////////////////////////////
// mem2memApp
// =====
// Application intialization.
//
void mem2memApp::InitMainWindow ()
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
// mem2memApp
// =====
// Response Table handlers:
//
void mem2memApp::EvNewView (TView& view)
{
    GetMainWindow()->SetClientWindow(view.GetWindow());
    if (!view.IsOK())
        GetMainWindow()->SetClientWindow(0);
    else if (view.GetViewMenu())
        GetMainWindow()->MergeMenu(*view.GetViewMenu());
}


void mem2memApp::EvCloseView (TView&)
{
    GetMainWindow()->SetClientWindow(0);
    GetMainWindow()->SetCaption("mem2mem");
}


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(SDIDecFrame, TDecoratedFrame)
//{{SDIDecFrameRSP_TBL_BEGIN}}
    EV_COMMAND(CM_TEST_DCL, OnTestDcl),
    EV_COMMAND_ENABLE(CM_TEST_DCL, OnTestDclEnable),
//{{SDIDecFrameRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{SDIDecFrame Implementation}}


SDIDecFrame::SDIDecFrame (TWindow *parent, const char far *title, TWindow *clientWnd, bool trackMenuSelection, TModule *module)
    : TDecoratedFrame(parent, title, clientWnd, trackMenuSelection, module)
{
    // INSERT>> Your constructor code here.

}


SDIDecFrame::~SDIDecFrame ()
{
    // INSERT>> Your destructor code here.

}


//////////////////////////////////////////////////////////
// mem2memApp
// ===========
// Menu Help About mem2mem.exe command
void mem2memApp::CmHelpAbout ()
{
    //
    // Show the modal dialog.
    //
    mem2memAboutDlg(GetMainWindow()).Execute();
}


int OwlMain (int , char* [])
{
    try {
        mem2memApp    app;
        return app.Run();
    }
    catch (xmsg& x) {
        ::MessageBox(0, x.why().c_str(), "Exception", MB_OK);
    }

    return -1;
}

void SDIDecFrame::OnTestDcl ()
{
   TOpenSaveDialog::TData data( OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|
            OFN_PATHMUSTEXIST|OFN_NOCHANGEDIR,
            "All Files (*.*)|*.*|", 0, "", "*" );

   if( TFileOpenDialog( this, data ).Execute() == IDOK )
   {
      TWindow *ClientWnd = GetClientWindow();

      // GET DC AND SET THE TEXT BACKGROUND COLOR TO WINDOW BACKGROUND COLOR
      TWindowDC dc( ClientWnd->HWindow );
      TColor bkGroundColor = dc.GetPixel( 0, 0 );
      dc.SetBkColor( bkGroundColor );

      MemToMemExample( ClientWnd->HWindow, &dc, data.FileName );
   }
}


void SDIDecFrame::OnTestDclEnable (TCommandEnabler &tce)
{
   tce.Enable( TRUE );
}


/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
#if !defined(__mm2mmapp_h)              // Sentry, use file only if it's not already included.
#define __mm2mmapp_h

#include <owl\owlpch.h>
#pragma hdrstop


#include "mm2mmapp.rh"            // Definition of all resources.


//
// FrameWindow must be derived to override Paint for Preview and Print.
//
//{{TDecoratedFrame = SDIDecFrame}}
class SDIDecFrame : public TDecoratedFrame {
public:
    SDIDecFrame (TWindow *parent, const char far *title, TWindow *clientWnd, bool trackMenuSelection = false, TModule *module = 0);
    ~SDIDecFrame ();

//{{SDIDecFrameRSP_TBL_BEGIN}}
protected:
    void OnTestDcl ();
    void OnTestDclEnable (TCommandEnabler &tce);
//{{SDIDecFrameRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(SDIDecFrame);
};    //{{SDIDecFrame}}


//{{TApplication = mem2memApp}}
class mem2memApp : public TApplication {
private:

public:
    mem2memApp ();
    virtual ~mem2memApp ();

//{{mem2memAppVIRTUAL_BEGIN}}
public:
    virtual void InitMainWindow ();
//{{mem2memAppVIRTUAL_END}}

//{{mem2memAppRSP_TBL_BEGIN}}
protected:
    void EvNewView (TView& view);
    void EvCloseView (TView& view);
    void CmHelpAbout ();
//{{mem2memAppRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(mem2memApp);
};    //{{mem2memApp}}


#endif                                      // __mm2mmapp_h sentry.

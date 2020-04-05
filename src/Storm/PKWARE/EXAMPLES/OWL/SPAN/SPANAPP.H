/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
#if !defined(__spanapp_h)              // Sentry, use file only if it's not already included.
#define __spanapp_h

#include <owl\owlpch.h>
#pragma hdrstop


#include "spanapp.rh"            // Definition of all resources.


#define  WM_TURN_OFF_HELP  WM_USER+1
#define  WM_TURN_ON_HELP   WM_USER+2


//
// FrameWindow must be derived to override Paint for Preview and Print.
//
//{{TDecoratedFrame = SDIDecFrame}}
class SDIDecFrame : public TDecoratedFrame {
public:
    SDIDecFrame (TWindow *parent, const char far *title, TWindow *clientWnd, bool trackMenuSelection = false, TModule *module = 0);
    ~SDIDecFrame ();

//{{SDIDecFrameVIRTUAL_BEGIN}}
public:
    virtual bool CanClose ();
//{{SDIDecFrameVIRTUAL_END}}

//{{SDIDecFrameRSP_TBL_BEGIN}}
protected:
    void OnCompressFiles ();
    void OnCompressFilesEnable (TCommandEnabler &tce);
    void OnUncompressFiles ();
    void OnUncompressFilesEnable (TCommandEnabler &tce);
    void OnCompTypeAscii ();
    void OnCompTypeAsciiEnable (TCommandEnabler &tce);
    void OnCompTypeBinary ();
    void OnCompTypeBinaryEnable (TCommandEnabler &tce);
    void OnDictSize1024 ();
    void OnDictSize1024Enable (TCommandEnabler &tce);
    void OnDictSize2048 ();
    void OnDictSize2048Enable (TCommandEnabler &tce);
    void OnDictSize4096 ();
    void OnDictSize4096Enable (TCommandEnabler &tce);
//{{SDIDecFrameRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(SDIDecFrame);
};    //{{SDIDecFrame}}


//{{TApplication = spanApp}}
class spanApp : public TApplication {
private:

public:
    spanApp ();
    virtual ~spanApp ();

//{{spanAppVIRTUAL_BEGIN}}
public:
    virtual void InitMainWindow ();
//{{spanAppVIRTUAL_END}}

//{{spanAppRSP_TBL_BEGIN}}
protected:
    void EvNewView (TView& view);
    void EvCloseView (TView& view);
    void CmHelpAbout ();
//{{spanAppRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(spanApp);
};    //{{spanApp}}


#endif                                      // __spanapp_h sentry.

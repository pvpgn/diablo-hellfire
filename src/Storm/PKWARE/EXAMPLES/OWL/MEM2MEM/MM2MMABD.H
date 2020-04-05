/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
#if !defined(__mm2mmabd_h)              // Sentry, use file only if it's not already included.
#define __mm2mmabd_h

#include <owl\owlpch.h>
#pragma hdrstop

#include "mm2mmapp.rh"                  // Definition of all resources.


//{{TDialog = mem2memAboutDlg}}
class mem2memAboutDlg : public TDialog {
public:
    mem2memAboutDlg (TWindow *parent, TResId resId = IDD_ABOUT, TModule *module = 0);
    virtual ~mem2memAboutDlg ();

//{{mem2memAboutDlgVIRTUAL_BEGIN}}
public:
    void SetupWindow ();
//{{mem2memAboutDlgVIRTUAL_END}}
};    //{{mem2memAboutDlg}}


// Reading the VERSIONINFO resource.
class ProjectRCVersion {
public:
    ProjectRCVersion (TModule *module);
    virtual ~ProjectRCVersion ();

    bool GetProductName (LPSTR &prodName);
    bool GetProductVersion (LPSTR &prodVersion);
    bool GetCopyright (LPSTR &copyright);
    bool GetDebug (LPSTR &debug);

protected:
    LPBYTE      TransBlock;
    void FAR    *FVData;

private:
    // Don't allow this object to be copied.
    ProjectRCVersion (const ProjectRCVersion &);
    ProjectRCVersion & operator =(const ProjectRCVersion &);
};


#endif                                      // __mm2mmabd_h sentry.

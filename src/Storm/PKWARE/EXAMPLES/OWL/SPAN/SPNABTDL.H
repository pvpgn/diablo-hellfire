/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
#if !defined(__spnabtdl_h)              // Sentry, use file only if it's not already included.
#define __spnabtdl_h

#include <owl\owlpch.h>
#pragma hdrstop

#include "spanapp.rh"                  // Definition of all resources.


//{{TDialog = spanAboutDlg}}
class spanAboutDlg : public TDialog {
public:
    spanAboutDlg (TWindow *parent, TResId resId = IDD_ABOUT, TModule *module = 0);
    virtual ~spanAboutDlg ();

//{{spanAboutDlgVIRTUAL_BEGIN}}
public:
    void SetupWindow ();
//{{spanAboutDlgVIRTUAL_END}}
};    //{{spanAboutDlg}}


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


#endif                                      // __spnabtdl_h sentry.

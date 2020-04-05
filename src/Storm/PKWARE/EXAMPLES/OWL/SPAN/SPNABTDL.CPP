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

#if !defined(__FLAT__)
#include <ver.h>
#endif

#include "spanapp.h"
#include "spnabtdl.h"


ProjectRCVersion::ProjectRCVersion (TModule *module)
{
    char    appFName[255];
    char    subBlockName[255];
    DWORD   fvHandle;
    UINT    vSize;

    FVData = 0;

    module->GetModuleFileName(appFName, sizeof(appFName));
    OemToAnsi(appFName, appFName);
    DWORD dwSize = ::GetFileVersionInfoSize(appFName, &fvHandle);
    if (dwSize) {
        FVData  = (void FAR *)new char[(UINT)dwSize];
        if (::GetFileVersionInfo(appFName, fvHandle, dwSize, FVData)) {
            // Copy string to buffer so if the -dc compiler switch (Put constant strings in code segments)
            // is on VerQueryValue will work under Win16.  This works around a problem in Microsoft's ver.dll
            // which writes to the string pointed to by subBlockName.
            strcpy(subBlockName, "\\VarFileInfo\\Translation"); 
            if (!::VerQueryValue(FVData, subBlockName, (void FAR* FAR*)&TransBlock, &vSize)) {
                delete FVData;
                FVData = 0;
            } else
                // Swap the words so wsprintf will print the lang-charset in the correct format.
                *(DWORD *)TransBlock = MAKELONG(HIWORD(*(DWORD *)TransBlock), LOWORD(*(DWORD *)TransBlock));
        }
    }
}


ProjectRCVersion::~ProjectRCVersion ()
{
    if (FVData)
        delete FVData;
}


bool ProjectRCVersion::GetProductName (LPSTR &prodName)
{
    UINT    vSize;
    char    subBlockName[255];

    wsprintf(subBlockName, "\\StringFileInfo\\%08lx\\%s", *(DWORD *)TransBlock, (LPSTR)"ProductName");
    return FVData ? ::VerQueryValue(FVData, subBlockName, (void FAR* FAR*)&prodName, &vSize) : false;
}


bool ProjectRCVersion::GetProductVersion (LPSTR &prodVersion)
{
    UINT    vSize;
    char    subBlockName[255];

    wsprintf(subBlockName, "\\StringFileInfo\\%08lx\\%s", *(DWORD *)TransBlock, (LPSTR)"ProductVersion");
    return FVData ? ::VerQueryValue(FVData, subBlockName, (void FAR* FAR*)&prodVersion, &vSize) : false;
}


bool ProjectRCVersion::GetCopyright (LPSTR &copyright)
{
    UINT    vSize;
    char    subBlockName[255];

    wsprintf(subBlockName, "\\StringFileInfo\\%08lx\\%s", *(DWORD *)TransBlock, (LPSTR)"LegalCopyright");
    return FVData ? ::VerQueryValue(FVData, subBlockName, (void FAR* FAR*)&copyright, &vSize) : false;
}


bool ProjectRCVersion::GetDebug (LPSTR &debug)
{
    UINT    vSize;
    char    subBlockName[255];

    wsprintf(subBlockName, "\\StringFileInfo\\%08lx\\%s", *(DWORD *)TransBlock, (LPSTR)"SpecialBuild");
    return FVData ? ::VerQueryValue(FVData, subBlockName, (void FAR* FAR*)&debug, &vSize) : false;
}


//{{spanAboutDlg Implementation}}


//////////////////////////////////////////////////////////
// spanAboutDlg
// ==========
// Construction/Destruction handling.
spanAboutDlg::spanAboutDlg (TWindow *parent, TResId resId, TModule *module)
    : TDialog(parent, resId, module)
{
    // INSERT>> Your constructor code here.
}


spanAboutDlg::~spanAboutDlg ()
{
    Destroy();

    // INSERT>> Your destructor code here.
}


void spanAboutDlg::SetupWindow ()
{
    LPSTR prodName = 0, prodVersion = 0, copyright = 0, debug = 0;

    // Get the static text for the value based on VERSIONINFO.
    TStatic *versionCtrl = new TStatic(this, IDC_VERSION, 255);
    TStatic *copyrightCtrl = new TStatic(this, IDC_COPYRIGHT, 255);
    TStatic *debugCtrl = new TStatic(this, IDC_DEBUG, 255);

    TDialog::SetupWindow();

    // Process the VERSIONINFO.
    ProjectRCVersion applVersion(GetModule());

    // Get the product name and product version strings.
    if (applVersion.GetProductName(prodName) && applVersion.GetProductVersion(prodVersion)) {
        // IDC_VERSION is the product name and version number, the initial value of IDC_VERSION is
        // the word Version (in whatever language) product name VERSION product version.
        char    buffer[255];
        char    versionName[128];

        buffer[0] = '\0';
        versionName[0] = '\0';

        versionCtrl->GetText(versionName, sizeof(versionName));
        wsprintf(buffer, "%s %s %s", prodName, versionName, prodVersion);

        versionCtrl->SetText(buffer);
    }

    //Get the legal copyright string.
    if (applVersion.GetCopyright(copyright))
        copyrightCtrl->SetText(copyright);

    // Only get the SpecialBuild text if the VERSIONINFO resource is there.
    if (applVersion.GetDebug(debug))
        debugCtrl->SetText(debug);
}

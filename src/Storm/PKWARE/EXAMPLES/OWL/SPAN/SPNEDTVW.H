/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
#if !defined(__spnedtvw_h)              // Sentry, use file only if it's not already included.
#define __spnedtvw_h

#include <owl\owlpch.h>
#pragma hdrstop


#include "spanapp.rh"            // Definition of all resources.


//{{TEditView = spanEditView}}
class spanEditView : public TEditView {
public:
    spanEditView (TDocument& doc, TWindow* parent = 0);
    virtual ~spanEditView ();
};    //{{spanEditView}}


#endif                                      // __spnedtvw_h sentry.

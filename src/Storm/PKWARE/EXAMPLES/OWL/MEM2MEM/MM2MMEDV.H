/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
#if !defined(__mm2mmedv_h)              // Sentry, use file only if it's not already included.
#define __mm2mmedv_h

#include <owl\owlpch.h>
#pragma hdrstop


#include "mm2mmapp.rh"            // Definition of all resources.


//{{TEditView = mem2memEditView}}
class mem2memEditView : public TEditView {
public:
    mem2memEditView (TDocument& doc, TWindow* parent = 0);
    virtual ~mem2memEditView ();
};    //{{mem2memEditView}}


#endif                                      // __mm2mmedv_h sentry.

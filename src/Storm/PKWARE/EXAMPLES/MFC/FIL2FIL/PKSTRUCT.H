/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1994,1995 by PKWARE Inc. All Rights Reserved.     ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
#ifndef __PKSTRUCT__
#define __PKSTRUCT__ 1

typedef struct IOFILEBLOCK 
{
   FILE *InFile;
   FILE *OutFile;
   BOOL bDoCRC;
   DWORD dwCRC;
   BOOL DebugMessages;
   HWND hWindow;
   BOOL PKAbortOperation;
} 
*PIOFILEBLOCK;

#endif

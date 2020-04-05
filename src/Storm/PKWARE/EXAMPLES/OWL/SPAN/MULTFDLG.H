/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */

#include <owl\opensave.h>

class MultiSelFileDialog : public TFileOpenDialog
{
   private:
      PCHAR pszFileList;
      PCHAR pszOldPtr;
      int   nPathLen;
      char  szPath[80];
      BOOL  Done;

   public:
      MultiSelFileDialog( TWindow *pParentWnd, TData& data );
      ~MultiSelFileDialog();
      BOOL GetFirstFilename( PCHAR lpszFullPathBuff,
                             UINT nPathBuffSize,
                             PCHAR lpszFilenameBuff );
      BOOL GetNextFilename( PCHAR lpszFullPathBuff,
                            UINT nPathBuffSize,
                            PCHAR lpszFilenameBuff );
};

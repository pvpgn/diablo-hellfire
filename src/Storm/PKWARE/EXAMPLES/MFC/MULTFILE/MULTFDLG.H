/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */

#include <afxdlgs.h>

class CMultiSelFileDialog : public CFileDialog
{
   private:
      PCHAR pszFileList;
      PCHAR pszOldPtr;
      int   nPathLen;
      char  szPath[80];
      BOOL  Done;

   public:
      CMultiSelFileDialog( CWnd *pParentWnd );
      ~CMultiSelFileDialog();
      BOOL GetFirstFilename( PCHAR pszFullPathBuff,
                             UINT nPathBuffSize,
                             PCHAR pszFilenameBuff );
      BOOL GetNextFilename( PCHAR pszFullPathBuff,
                            UINT nPathBuffSize,
                            PCHAR pszFilenameBuff );
};

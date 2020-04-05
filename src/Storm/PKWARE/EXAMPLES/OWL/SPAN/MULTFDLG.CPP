/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */

#include <string.h>

#include "multfdlg.h"

#define FILELIST_BUFFSIZE  4096     // AMOUNT OF MEMORY TO ALLOCATE FOR FILE LIST


MultiSelFileDialog::MultiSelFileDialog( TWindow *pParentWnd, TData& data )
   :TFileOpenDialog( pParentWnd, data )
{
   // ALLOCATE MEMORY FOR FILE LIST
   pszFileList = (PCHAR) new char[FILELIST_BUFFSIZE];

   // SET ALLOCATED BUFFER AS FILENAME BUFER IN OPENFILENAME STRUCT
   // AND REPLACE IT IN THE OPENFILENAME STRUCT
   pszOldPtr = ofn.lpstrFile;
   ofn.lpstrFile = pszFileList;
   ofn.nMaxFile = FILELIST_BUFFSIZE;

   // DO SOME INITIALIZATION
   memset( pszFileList, 0, FILELIST_BUFFSIZE );
   strcpy( pszFileList, "*.*" );
   memset( szPath, 0, sizeof(szPath) );
   nPathLen = 0;
   Done = TRUE;
}


MultiSelFileDialog::~MultiSelFileDialog()
{
   // REPLACE OLD POINTER AND FREE MEMORY
   ofn.lpstrFile = pszOldPtr;
   delete pszFileList;
}


BOOL MultiSelFileDialog::GetFirstFilename( PCHAR pszFullPathBuff,
                                            UINT nPathBuffSize,
                                            PCHAR pszFilenameBuff )
{
   PCHAR pszToken;

   nPathLen = 0;
   Done = FALSE;

   // GET THE FIRST TOKEN WHICH SHOULD BE THE PATH
   if( strchr( pszFileList, ' ' ) == NULL )
   {
      // COULD NOT FIND TOKEN SO MUST BE PATH AND FILENAME

      Done = TRUE;

      memset( szPath, 0, sizeof(szPath) );
      nPathLen = 0;

      // MAKE SURE THE FILENAME + PATH WILL FIT
      if( strlen( pszFileList ) > nPathBuffSize )
      {
         *pszFullPathBuff = '\0';
         *pszFilenameBuff = '\0';
         return FALSE;
      }

      // COPY FILENAME ONLY
      char *NameOnly = strrchr( pszFileList, '\\' );
      if( NameOnly == NULL || strlen( NameOnly ) > 13 )
      {
         *pszFullPathBuff = '\0';
         *pszFilenameBuff = '\0';
         return FALSE;
      }
      strcpy( pszFilenameBuff, ++NameOnly );

      // COPY PATH AND FILENAME TO BUFFER
      strcpy( pszFullPathBuff, pszFileList );

      return TRUE;
   }

   // GET THE FIRST TOKEN WHICH SHOULD BE THE PATH
   if( (pszToken = strtok( pszFileList, " " )) == NULL )
   {
      Done = TRUE;
      *pszFullPathBuff = '\0';
      *pszFilenameBuff = '\0';
      return FALSE;
   }

   memset( szPath, 0, sizeof(szPath) );
   strcpy( szPath, pszToken );
   strcat( szPath, "\\" );

   nPathLen = strlen( szPath );

   return GetNextFilename( pszFullPathBuff, nPathBuffSize, pszFilenameBuff );
}


// pszFilenameBuff MUST BE AT LEAST 13 BYTES
BOOL MultiSelFileDialog::GetNextFilename( PCHAR pszFullPathBuff,
                                           UINT nPathBuffSize,
                                           PCHAR pszFilenameBuff )
{
   PCHAR pszToken;

   if( Done )
   {
      return FALSE;
   }

   // GET THE NEXT TOKEN WHICH SHOULD BE A FILENAME
   if( (pszToken = strtok( NULL, " " )) == NULL )
   {
      Done = TRUE;
      *pszFullPathBuff = '\0';
      *pszFilenameBuff = '\0';
      return FALSE;
   }

   // MAKE SURE THE FILENAME + PATH WILL FIT
   if( (strlen( pszToken ) + nPathLen) > nPathBuffSize )
   {
      Done = TRUE;
      *pszFullPathBuff = '\0';
      *pszFilenameBuff = '\0';
      return FALSE;
   }

   // COPY PATH AND FILENAME TO BUFFER
   strcpy( pszFullPathBuff, szPath );
   strcat( pszFullPathBuff, pszToken );

   PCHAR pszNameOnly = strrchr( pszFullPathBuff, '\\' );

   if( (pszNameOnly == NULL) || (strlen(pszNameOnly) > 13) )
   {
      Done = TRUE;
      *pszFullPathBuff = '\0';
      *pszFilenameBuff = '\0';
      return FALSE;
   }

   strcpy( pszFilenameBuff, ++pszNameOnly );

   return TRUE;
}
   





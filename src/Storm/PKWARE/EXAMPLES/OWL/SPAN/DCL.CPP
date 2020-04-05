/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */

/*
 * DCL.cpp - File to call various DCL DLL functions
 */

#include <owl\dc.h>
#include <classlib\file.h>
#include "implode.h"

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <errno.h>

#define TEMPFILENAME    "~~~.$$$"      // TEMPORARY FILENAME TO CREATE
#define APPENDBUFSIZE   32000          // SIZE OF BUFFER TO ALLOCATE FOR APPENDING
                                       // A COMPRESSED FILE TO .MCF FILE

typedef enum
{
   COMPRESSING = 1,
   UNCOMPRESSING
} FILEMODE;

typedef enum
{
   NO_ERRORS   = 0,
   ERROR_OCCURRED
} ERRORFLAG;


// STRUCT TO PASS TO THE FILE IO FUNCTIONS
typedef struct
{
   TFile    *InFile;
   TFile    *OutFile;
   TDC      *pDC;
   UINT      nPrevNdx;
   UINT      nCnt;
   DWORD     dwCompressSize;
   FILEMODE  mode;
   ERRORFLAG ErrorOccurred;
   DWORD     dwCrc;
}IOFILEBLOCK, *PIOFILEBLOCK;

// FOUR LETTER IDENTIFIER TO IDENTIFY A FILE AS
// A .MCF (MULTIPLE COMPRESSED FILES) FILE
char MCF_FILEHEADER[] = { "MCFX" };

#pragma pack(2)

// HEADER FOR EACH FILE COMPRESSED INTO THE .MCF FILE
typedef struct
{
   DWORD    dwCompressSize;               // SIZE OF FILE COMPRESSED
   DWORD    dwCrc;                        // THE CRC OF THE FILE BEFORE COMPRESSION
   char     filename[13];                 // NAME OF THE FILE
}CMP_FILEHEADER, *PCMP_FILEHEADER;

#pragma pack(8)

/*
 * FORMAT OF .MCF FILE:
 *
 *    .MCF FILEHEADER               =>    MCFX
 *    COMPRESSED FILE FILEHEADER    =>    { CMP_FILEHEADER }
 *       FOLLOWED BY                      { ... COMPRESSED FILE DATA ... }
 *    COMPRESSED FILE FILEHEADER    =>    { CMP_FILEHEADER }
 *       FOLLOWED BY                      { ... COMPRESSED FILE DATA ... }
 *    COMPRESSED FILE FILEHEADER    =>    { CMP_FILEHEADER }
 *       FOLLOWED BY                      { ... COMPRESSED FILE DATA ... }
 *    AND SO ON
 *
 */

static char *pszActiveString[] = { "|", "/", "-", "\\" };

UINT DataType = CMP_ASCII;          // GLOBAL FOR DATA TYPE FOR COMPRESSION
UINT DictSize = 4096;               // GLOBAL FOR DICTIONARY SIZE FOR COMPRESSION

static PCHAR pszFilename;           // NAME OF THE .MCF FILE


/*********************************************************************
 *
 * Function:   ProcessMessages()
 *
 * Purpose:    To allow Windows to process window messages which
 *             allows the user to multitask will compressing and
 *             uncompressing files.
 *
 * Returns:    Nothing
 *
 *********************************************************************/
void ProcessMessages(void)
{
   MSG msg;

   while( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
   {
      if (msg.message == WM_QUIT)
         return;

      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
}


/*********************************************************************
 *
 * Function:   ReadFromDisk()
 *
 * Purpose:    To read data from disk, and prompt user for another
 *             disk when needed. This function makes a large assumption
 *             that if when reading a header zero bytes are read, then
 *             EOF has been reached.
 *
 * Parameters: Infile -> Pointer to already opened .MCF file
 *             pBuffer -> input buffer
 *             nSize -> number of bytes to read
 *             numread -> pointer to return actual number of bytes read
 *
 * Returns:    1 -> Successful completion
 *             0 -> Error occurred (A FileException is thrown)
 *
 *********************************************************************/
int ReadFromDisk( TFile *InFile, PCHAR pBuffer, UINT nSize, UINT *numread,
                  BOOL ReadingHeader )
{
   UINT bytesread;

   // READ DATA FROM DISK
   *numread = bytesread = InFile->Read( pBuffer, nSize );

   // IF READING A HEADER AND READ 0 BYTES, THEN ASSUME THIS IS THE END
   if( bytesread == 0 && ReadingHeader )
   {
      return 1;
   }

   // IF COULD NOT READ ALL BYTES
   if( bytesread < nSize )
   {
      // CALCULATE NUMBER OF BYTES TO READ
      nSize -= bytesread;

      // CLOSE THE FILE
      InFile->Close();

      // PROMPT USER FOR ANOTHER DISK
      MessageBox( NULL, "Insert Next Disk", "Uncompress", MB_OK );

      // OPEN .MCF FILE ON NEW DISK
      if( !InFile->Open( pszFilename,
                         TFile::ReadOnly, TFile::PermRdWr ) )
      {
         return 0;
      }

      // READ DATA FROM DISK
      bytesread = InFile->Read( (pBuffer + bytesread), nSize );

      *numread += bytesread;
   }

   return 1;
}


/*********************************************************************
 *
 * Function:   ProcessInBuffer()
 *
 * Purpose:    To handle calls from the Data Compression Library for
 *             read requests. If compressing, then the data read is
 *             in uncompressed form.  If compressing, then the data
 *             read is data that was previously compressed. This
 *             function is called until zero is returned.
 *
 * Parameters: buffer ->   Address of buffer to read the data into
 *             iSize ->    Number of bytes to read into buffer
 *             dwParam ->  User-defined parameter, in this case a
 *                         pointer to the IOFILEBLOCK
 *
 * Returns:    Number of bytes actually read, or zero on EOF
 *
 *********************************************************************/
UINT ProcessInBuffer( PCHAR buffer, UINT *iSize, void *pParam )
{
   PIOFILEBLOCK pFileIOBlock;
   UINT iRead;
   UINT ndx;

   pFileIOBlock = (PIOFILEBLOCK) pParam;

   if( pFileIOBlock->ErrorOccurred )
   {
      return 0;
   }

   // DISPLAY ROTATING LINE
   ndx = (pFileIOBlock->nCnt >> 4) & 3;
   if( ndx != pFileIOBlock->nPrevNdx )
   {
      pFileIOBlock->pDC->TextOut( 10,2, pszActiveString[ndx] );
      pFileIOBlock->nPrevNdx = ndx;
   }
   pFileIOBlock->nCnt++;

   // THIS FUNCTION MAY ASK FOR UP TO 4K OF DATA AT A TIME. IF YOUR
   // ARCHIVE FILE CONTAINS SEVERAL COMPRESSED FILES, YOU MAY READ TOO
   // MUCH.  FOR EXAMPLE, YOUR FIRST COMPRESSED FILE IN THE ARCHIVE MAY
   // BE 100 BYTES.  SO YOU DO NOT WANT TO READ MORE THAN 100 BYTES OR
   // YOU WILL BE UNABLE TO UNCOMPRESS THE SECOND FILE, SINCE YOU WILL
   // NOT BE LOCATED AT THE BEGINNING OF THE FILE ANY LONGER.
   // WE WILL USE THE VARIABLE "dwCompressSize" TO CHECK FOR THIS
   // CONDITION.
   if( pFileIOBlock->mode == UNCOMPRESSING )
   {
      // IF DCL REQUESTED MORE BYTES THAN ARE LEFT IN COMPRESSED FILE, THEN
      // SET THE NUMBER OF BYTES TO READ TO THE AMOUNT LEFT IN THE BUFFER
      if( (DWORD) *iSize > pFileIOBlock->dwCompressSize )
         *iSize = (UINT) pFileIOBlock->dwCompressSize;

      pFileIOBlock->dwCompressSize -= (DWORD) *iSize;

      // READ COMPRESSED FILE FILEHEADER
      if( !ReadFromDisk( pFileIOBlock->InFile, buffer, *iSize, &iRead, FALSE ) )
      {
         pFileIOBlock->ErrorOccurred = ERROR_OCCURRED;
         return 0;
      }
   }
   else
   {
      // READ BUFFER FROM DISK
      iRead = (UINT)pFileIOBlock->InFile->Read( buffer, *iSize );
   }

   // IF COMPRESSING, THEN CALCULATE THE CRC
   if( pFileIOBlock->mode == COMPRESSING )
   {
       pFileIOBlock->dwCrc = crc32( buffer, &iRead, &pFileIOBlock->dwCrc );
   }

   // ENTER MESSAGE LOOP TO PROCESS BACKGROUND MESSAGES
   // AND SIMULATE MULTITASKING
   ProcessMessages();

   return iRead;
}

/*********************************************************************
 *
 * Function:   ProcessOutBuffer()
 *
 * Purpose:    To handle calls from the Data Compression Library for
 *             write requests.
 *
 * Parameters: buffer ->   Address of buffer to write data from
 *             iSize ->    Number of bytes to write
 *             dwParam ->  User-defined parameter, in this case a
 *                         pointer to the IOFILEBLOCK
 *
 * Returns:    Zero, the return value is not used by the Data
 *             Compression Library
 *
 *********************************************************************/
void ProcessOutBuffer( PCHAR buffer, UINT *iSize, void *pParam )
{
   PIOFILEBLOCK pFileIOBlock;
   UINT ndx;

   pFileIOBlock = (PIOFILEBLOCK) pParam;

   if( pFileIOBlock->ErrorOccurred )
   {
      return;
   }

   // DISPLAY ROTATING LINE PACIFIER
   ndx = (pFileIOBlock->nCnt >> 4) & 3;
   if( ndx != pFileIOBlock->nPrevNdx )
   {
      pFileIOBlock->pDC->TextOut( 10,2, pszActiveString[ndx] );
      pFileIOBlock->nPrevNdx = ndx;
   }
   pFileIOBlock->nCnt++;

   // WRITE BUFFER TO DISK
    if( pFileIOBlock->OutFile->Write(buffer, *iSize) != *iSize )
   {
      pFileIOBlock->ErrorOccurred = ERROR_OCCURRED;
      return;
   }

   // IF COMPRESSING, THEN KEEP A TOTAL OF THE COMPRESSED FILE SIZE
   if (pFileIOBlock->mode == COMPRESSING )
   {
      pFileIOBlock->dwCompressSize += (DWORD) *iSize;
   }
   else // ELSE UNCOMPRESSING, SO CALCULATE CRC ON THE UNCOMPRESSED DATA
   {
       pFileIOBlock->dwCrc = crc32(buffer, iSize, &pFileIOBlock->dwCrc);
   }

   // ENTER MESSAGE LOOP TO PROCESS BACKGROUND MESSAGES
   // AND SIMULATE MULTITASKING
   ProcessMessages();

   return;
}

/*********************************************************************
 *
 * Function:   CompressFile()
 *
 * Purpose:    To compress file to a separate temporary file.
 *
 *
 * Parameters: HWnd   ->   Handle to window
 *             pDC    ->   Pointer to a device context
 *             pdwCrc ->   Pointer to DWORD buffer to return the CRC
 *                         of the compressed file before compression
 *             pdwCompFileSize -> Pointer to DWORD buffer to return 
 *                                the size of the compressed file
 *             pszFileToCompress -> Name of file to compress
 *             OutputFile -> Name of file to write compressed data to
 *
 * Returns:    1 -> Successful completion
 *             0 -> Error occurred
 *
 *********************************************************************/
int CompressFile( HWND hWnd, TDC *pDC, DWORD *pdwCrc, DWORD *pdwCompFileSize,
                  PCHAR pszFileToCompress, PCHAR OutputFile )
{
   int iStatus;
   int rc = 1;
   char szVerbose[128];
   IOFILEBLOCK FileIOBlock;
   PCHAR  pScratchPad;

   if( (pScratchPad = (PCHAR) new char[CMP_BUFFER_SIZE]) == NULL )
   {
      return 0;
   }

   // OPEN THE INPUT AND OUTPUT FILES
   FileIOBlock.InFile  = new TFile;
   FileIOBlock.OutFile = new TFile;

   // SETUP STRUCTURE USED BY ProcessReadBuffer() AND ProcessWriteBuffer()
   FileIOBlock.mode = COMPRESSING;
   FileIOBlock.dwCompressSize = 0;
   FileIOBlock.pDC = pDC;
   FileIOBlock.nCnt = 0;
   FileIOBlock.nPrevNdx = 0;
   FileIOBlock.ErrorOccurred = NO_ERRORS;
   FileIOBlock.dwCrc = ~((DWORD)0);  // Pre-condition CRC

   pDC->TextOut( 10,2, "   " );

   // OPEN THE FILES
   if (FileIOBlock.InFile->Open( pszFileToCompress,  TFile::ReadOnly, TFile::PermRdWr ) &&
       FileIOBlock.OutFile->Open( OutputFile, TFile::Create | TFile::ReadWrite, TFile::PermRdWr ))
   {
      wsprintf( szVerbose, "Compressing file: %s                     ", pszFileToCompress );
      pDC->TextOut( 10,40, szVerbose );

      // ONLY COMPRESS IF FILE IS NOT A ZERO LENGTH FILE
      if( FileIOBlock.InFile->Length() )
      {
         // COMPRESS THE FILE
         iStatus = implode( ProcessInBuffer, ProcessOutBuffer, pScratchPad,
                            &FileIOBlock, &DataType, &DictSize );
      }
      else
      {
         // SINCE THIS IS A ZERO LENGTH FILE, THERE IS NOTHING TO COMPRESS
         // SET STATUS TO NO ERROR
         iStatus = 0;
      }

      // IF THERE WAS AN ERROR COMPRESSING FILE
      if( iStatus )
      {
         // DISPLAY ERROR STRING FROM DLL
         wsprintf( szVerbose, "Error occurred while imploding - %d ", iStatus );
         MessageBox( hWnd, szVerbose, "Error", MB_OK );
         rc = 0;
      }
      else if( FileIOBlock.ErrorOccurred )
      {
         MessageBox( hWnd, "Error occurred during compression", "Error", MB_OK );
         rc = 0;
      }
      else // ELSE - COMPRESSION WAS SUCCESSFUL
      {
         // POST-CONDITION CRC
         FileIOBlock.dwCrc = ~FileIOBlock.dwCrc;

         // RETURN CRC
         *pdwCrc = FileIOBlock.dwCrc;

         // RETURN COMPRESSED FILE SIZE
         *pdwCompFileSize = FileIOBlock.dwCompressSize;
      }
   }
   else // ELSE - ERROR OPENING FILES
   {
      MessageBox( hWnd, "Error opening files for compression", "Error", MB_OK );
      rc = 0;
   }

   // CLEAN-UP
   FileIOBlock.OutFile->Close();
   FileIOBlock.InFile->Close();
   delete FileIOBlock.InFile;
   delete FileIOBlock.OutFile;
   delete pScratchPad;

   return rc;
}


/*********************************************************************
 *
 * Function:   ExpandFile()
 *
 * Purpose:    To uncompress file from a .MCF file. The .MCF file will
 *             have been read upto the compressed data stream.
 *
 * Parameters: HWnd   ->   Handle to window
 *             pDC    ->   Pointer to a device context
 *             pdwCrc ->   Pointer to DWORD buffer to return the CRC 
 *                         of the compressed file before compression
 *             dwCompFileSize -> Size of the compressed file
 *             pMcfFile -> Pointer to already opened .MCF file
 *             OutputFile -> Name of file to write uncompressed data to
 *
 * Returns:    1 -> Successful completion
 *             0 -> Error occurred
 *
 *********************************************************************/
int ExpandFile( HWND hWnd, TDC *pDC, DWORD *pdwCrc, DWORD dwCompFileSize,
                TFile *pMcfFile, PCHAR OutputFile )
{
   int iStatus;
   int rc = 1;
   char szVerbose[128];
   IOFILEBLOCK FileIOBlock;
   PCHAR  pScratchPad;

   if( (pScratchPad = (PCHAR) new char[CMP_BUFFER_SIZE]) == NULL )
   {
      return 0;
   }

   FileIOBlock.InFile  = pMcfFile;
   FileIOBlock.OutFile = new TFile;

   // SETUP STRUCTURE USED BY ProcessReadBuffer() and ProcessWriteBuffer()
   FileIOBlock.mode = UNCOMPRESSING;
   FileIOBlock.dwCompressSize = dwCompFileSize;
   FileIOBlock.pDC = pDC;
   FileIOBlock.nCnt = 0;
   FileIOBlock.nPrevNdx = 0;
   FileIOBlock.ErrorOccurred = NO_ERRORS;
   FileIOBlock.dwCrc = ~((DWORD)0);  // Pre-condition CRC

   pDC->TextOut( 10,2, "   " );

   if( FileIOBlock.OutFile->Open(OutputFile, TFile::Create | TFile::ReadWrite, TFile::PermRdWr ) )
   {
      // ONLY UNCOMPRESS IF FILE IS NOT A ZERO LENGTH FILE
      if( FileIOBlock.dwCompressSize )
      {
         // UNCOMPRESS FILE
         iStatus = explode( ProcessInBuffer, ProcessOutBuffer, pScratchPad, &FileIOBlock );
      }
      else
      {
         // SINCE THIS IS A ZERO LENGTH FILE, THERE IS NOTHING TO UNCOMPRESS
         // SET STATUS TO NO ERROR
         iStatus = 0;
      }

      if( iStatus )
      {
         wsprintf( szVerbose, "Error occurred while exploding - %d ", iStatus );
         MessageBox( hWnd, szVerbose, "Error", MB_OK );
         rc = 0;
      }
      else if( FileIOBlock.ErrorOccurred )
      {
         MessageBox( hWnd, "Error occurred during uncompression", "Error", MB_OK );
         rc = 0;
      }
      else
      {
         FileIOBlock.dwCrc = ~FileIOBlock.dwCrc;
         *pdwCrc = FileIOBlock.dwCrc;

         if( FileIOBlock.dwCompressSize != 0 )
         {
            wsprintf( szVerbose, "Error uncompressing file: %s", OutputFile );
            MessageBox( hWnd, szVerbose, "Error", MB_OK );
            rc = 0;
         }
      }
   }
   else
   {
      MessageBox( hWnd, "Error opening files for uncompression", "Error", MB_OK );
      rc = 0;
   }

   FileIOBlock.OutFile->Close();
   delete FileIOBlock.OutFile;
   delete pScratchPad;

   return rc;
}


/*********************************************************************
 *
 * Function:   AmountDriveSpaceFree()
 *
 * Purpose:    To calculate the number of bytes free on drive
 *
 * Parameters: drivenum     -> Drive to check (A=1, B=2, ...)
 *             pdwBytesFree -> DWORD to return number of bytes free
 *
 * Returns:    1 -> Successful completion
 *             0 -> Error occurred
 *
 *********************************************************************/
int AmountDriveSpaceFree( UINT drivenum, DWORD *pdwBytesFree )
{
   static char RootStr[] = { "?:\\" };
   DWORD  SectorsPerCluster,
          BytesPerSector,
          FreeClusters,
          Clusters;

   *RootStr = (char) ('A' + drivenum - 1);

   if( GetDiskFreeSpace( RootStr, &SectorsPerCluster, &BytesPerSector,
                         &FreeClusters, &Clusters ) == FALSE )
   {
      return 0;
   }

   *pdwBytesFree = SectorsPerCluster * BytesPerSector * FreeClusters;

   return 1;
}


/*********************************************************************
 *
 * Function:   WriteToDisk()
 *
 * Purpose:    To write data to disk, and prompt user for another
 *             disk when full.
 *
 * Parameters: OutFile -> Pointer to already opened file
 *             pBuffer -> output buffer
 *             nSize -> number of bytes to write
 *             pdwBytesFree -> number of bytes left on disk
 *
 * Returns:    1 -> Successful completion
 *             0 -> Error occurred
 *
 *********************************************************************/
int WriteToDisk( TFile *OutFile, PCHAR pBuffer, UINT nSize, DWORD *pdwBytesFree )
{
   UINT  drivenum;

   // IF THERE IS SPACE TO WRITE FILE
   if( (ULONG) nSize <= (ULONG) *pdwBytesFree )
   {
      OutFile->Write( pBuffer, nSize );
      *pdwBytesFree -= nSize;
   }
   else // ELSE - NOT ENOUGH SPACE TO WRITE FILE
   {
      // FILL UP REMAINING DISKSPACE IF ANY
      if( *pdwBytesFree > 0 )
      {
         OutFile->Write( pBuffer, (UINT) *pdwBytesFree );
         nSize -= (UINT) *pdwBytesFree;
      }
      OutFile->Close();

      drivenum = *pszFilename - 'A' + 1;

      // IF ON A FIXED DRIVE, THEN GIVE DISKFULL ERROR
      if( drivenum > 2 )
      {
         return 0;
      }

      UINT nBytesRead = (UINT) *pdwBytesFree;

      // PROMPT USER FOR ANOTHER DISK
      MessageBox( NULL, "Insert Another Disk", "Compress", MB_OK );

      // GET NUMBER OF BYTES FREE ON DISK
      if( !AmountDriveSpaceFree( drivenum, pdwBytesFree ) )
      {
         return 0;
      }

      // CREATE NEW .MCF FILE
      if( !OutFile->Open( pszFilename, TFile::Create | TFile::ReadWrite, TFile::PermRdWr ) )
      {
         return 0;
      }

      // WRITE REMAINING BYTES
      if( OutFile->Write( (pBuffer + nBytesRead), nSize ) != nSize )
      {
         return 0;
      }
      *pdwBytesFree -= nSize;
   }

   return 1;
}


/*********************************************************************
 *
 * Function:   AddFileToMcfFile()
 *
 * Purpose:    To add a compressed file with header to a .MCF file.
 *             The file header is written followed by the compressed
 *             file data
 *
 * Parameters: pFileHeader -> File header for the compressed file
 *             pszInput   -> Filename of the compressed file's data
 *             pszOutput  -> Filename of .MCF file
 *             CompressedFileSize -> Size of the compressed file
 *             NewMcfFile  -> Flag used to create a new .MCF file
 *                            TRUE  - A new .MCF file will be created
 *                            FALSE - The .MCF file will appended to
 *
 * Returns:    1 -> Successful completion
 *             0 -> Error occurred
 *
 *********************************************************************/
int AddFileToMcfFile( PCMP_FILEHEADER pFileHeader, PCHAR pszInput,
                      DWORD CompressedFileSize, BOOL NewMcfFile )
{
   PCHAR buf;
   UINT  read;
   TFile InFile;
   TFile OutFile;
   UINT  drivenum;
   DWORD BytesFree;

   // ALLOCATE I/O BUFFER
   if( (buf = new char[APPENDBUFSIZE]) == NULL )
   {
      return 0;
   }

   drivenum = *pszFilename - 'A' + 1;

   // OPEN THE FILES
   InFile.Open( pszInput, TFile::ReadOnly, TFile::PermRdWr );

   // IF NEW FILE, THEN CREATE MULTIPLE COMPRESSED FILES FILE
   if( NewMcfFile )
   {
      // CREATE NEW .MCF FILE
      if( !OutFile.Open( pszFilename, TFile::Create | TFile::ReadWrite, TFile::PermRdWr ) )
      {
         InFile.Close();
         delete buf;
         return 0;
      }

      // GET NUMBER OF BYTES FREE ON DISK
      if( !AmountDriveSpaceFree( drivenum, &BytesFree ) )
      {
         delete buf;
         return 0;
      }

      // WRITE .MCF FILE HEADER
      if( !WriteToDisk( &OutFile, MCF_FILEHEADER, 4, &BytesFree ) )
      {
         OutFile.Close();
         InFile.Close();
         delete buf;
         return 0;
      }
   }
   else
   {
      // OPEN OLD .MCF FILE
      if( !OutFile.Open( pszFilename, TFile::ReadWrite, TFile::PermRdWr ) )
      {
         InFile.Close();
         delete buf;
         return 0;
      }

      // GET NUMBER OF BYTES FREE ON DISK
      if( !AmountDriveSpaceFree( drivenum, &BytesFree ) )
      {
         delete buf;
         return 0;
      }

      // GO TO END OF FILE
      OutFile.SeekToEnd();
   }

   // WRITE THE COMPRESSED FILE'S FILEHEADER
   if( !WriteToDisk( &OutFile, (PCHAR) pFileHeader, sizeof(CMP_FILEHEADER),
                     &BytesFree ) )
   {
      OutFile.Close();
      InFile.Close();
      delete buf;
      return 0;
   }

   do
   {
      // READ FROM COMPRESSED FILE
      read = InFile.Read( buf, APPENDBUFSIZE );

      // WRITE DATA TO .MCF FILE
      if( !WriteToDisk( &OutFile, buf, read, &BytesFree ) )
      {
         OutFile.Close();
         InFile.Close();
         delete buf;
         return 0;
      }

      // IF ERROR OCCURRED
      if( CompressedFileSize < (DWORD) read )
      {
         OutFile.Close();
         InFile.Close();
         delete buf;
         return 0;
      }

      CompressedFileSize -= (DWORD) read;
   }
   while( CompressedFileSize > 0 );

   OutFile.Close();
   InFile.Close();

   delete buf;

   return 1;
}

/*********************************************************************
 *
 * Function:   CompressFileToMCF()
 *
 * Purpose:    To compress a file then add it to a .MCF file
 *
 *
 * Parameters: HWnd   ->   Handle to window
 *             pDC    ->   Pointer to a device context
 *             pszMcfFile -> Filename of .MCF file
 *             pszFileToCompress -> Filename of file with full path
 *             pszFilenameOnly -> Filename of file without any path
 *                                 This is the filename that is stored
 *                                 in the compressed file's file header
 *             NewMcfFile  -> Flag used to create a new .MCF file
 *                            TRUE  - A new .MCF file will be created
 *                            FALSE - The .MCF file will appended to
 *
 * Returns:    1 -> Successful completion
 *             0 -> Error occurred
 *
 *********************************************************************/
int CompressFileToMCF( HWND hWnd, TDC *pDC, PCHAR pszMcfFile, PCHAR pszFileToCompress,
                       PCHAR pszFilenameOnly, BOOL NewMcfFile )
{
   CMP_FILEHEADER FileHeader;

   pszFilename = pszMcfFile;

   memset( &FileHeader, 0, sizeof(FileHeader) );

   // ATTEMPT TO COMPRESS THE FILE
   if( !CompressFile( hWnd, pDC, &FileHeader.dwCrc,
                      &FileHeader.dwCompressSize, pszFileToCompress,
                      TEMPFILENAME ) )
   {
      remove( TEMPFILENAME );
      MessageBox( hWnd, "Compress File Failed", "Error", MB_OK );
      return 0;
   }

   _fstrcpy( FileHeader.filename, pszFilenameOnly );

   if( !AddFileToMcfFile( &FileHeader, TEMPFILENAME, FileHeader.dwCompressSize, NewMcfFile ) )
   {
      remove( TEMPFILENAME );
      MessageBox( hWnd, "Compress File Failed", "Error", MB_OK );
      return 0;
   }

   remove( TEMPFILENAME );

   return 1;
}

/*********************************************************************
 *
 * Function:   UncompressFileToMCF()
 *
 * Purpose:    To uncompress a .MCF file
 *
 *
 * Parameters: HWnd   ->   Handle to window
 *             pDC    ->   Pointer to a device context
 *             pszMcfFilename -> Filename of .MCF file
 *             pszSaveDir -> Directory to save uncompressed files
 *                            a backslash must be the last character
 *
 * Returns:    1 -> Successful completion
 *             0 -> Error occurred
 *
 *********************************************************************/
int UncompressMcfFile( HWND hWnd, TDC *pDC, PCHAR pszMcfFilename, PCHAR pszSaveDir )
{
   CMP_FILEHEADER FileHeader;
   char McfFileheader[5];
   char szOutMsg[128];
   char szOutputFilename[100];
   UINT nNumFiles = 0;
   DWORD dwCrc;                     // CRC OF FILE BEFORE COMPRESSION
   UINT read;
   TFile McfFile;

   pszFilename = pszMcfFilename;

   // OPEN THE FILES
   McfFile.Open( pszFilename,  TFile::ReadOnly, TFile::PermRdWr );

   memset( McfFileheader, 0, sizeof(McfFileheader) );

   // MAKE SURE FILE IS A MULTIPLE COMPRESSED FILE
   if( (!ReadFromDisk( &McfFile, McfFileheader, 4, &read, FALSE )) ||
       (read != 4) ||
       (strcmp( McfFileheader, MCF_FILEHEADER ) != 0) )
   {
      MessageBox( hWnd, "Invalid file format", "Error", MB_OK );
      return 0;
   }

   do
   {
      // READ COMPRESSED FILE FILEHEADER
      if( !ReadFromDisk( &McfFile, (PCHAR)&FileHeader, sizeof(FileHeader),
                         &read, TRUE ) )
      {
         McfFile.Close();
         MessageBox( hWnd, "Error Uncompressing file", "Error", MB_OK );
         return 0;
      }

      // IF SUCCESSFULLY READ COMPRESSED FILE FILEHEADER
      if( read == sizeof(FileHeader) )
      {
         // CREATE OUTPUT FILENAME
         strcpy( szOutputFilename, pszSaveDir );
         strcat( szOutputFilename, FileHeader.filename );

         wsprintf( szOutMsg, "Uncompressing file: %s               ",
                   FileHeader.filename );
         pDC->TextOut( 10,60, szOutMsg );

         // ATTEMPT TO EXPAND THE FILE
         if( !ExpandFile( hWnd, pDC, &dwCrc,
                          FileHeader.dwCompressSize,
                          &McfFile, szOutputFilename ) )
         {
            McfFile.Close();
            wsprintf( szOutMsg, "Error Uncompressing file: %s",
                      szOutputFilename );
            MessageBox( hWnd, szOutMsg, "Error", MB_OK );
            return 0;
         }

         // CHECK THE CRC OF THE FILE
         if( dwCrc != FileHeader.dwCrc )
         {
            wsprintf( szOutMsg, "There is an error in the CRC of %s",
                      szOutputFilename );
            MessageBox( hWnd, szOutMsg, "Error", MB_OK );
         }

         nNumFiles++;
      }
   }
   while( read == sizeof(FileHeader) );

   McfFile.Close();

   if( read != 0 )
   {
      MessageBox( hWnd, "Invalid file format", "Error", MB_OK );
      return 0;
   }
   else
   {
      wsprintf( szOutMsg, "Uncompressed %u file(s)", nNumFiles );
      MessageBox( hWnd, szOutMsg, "Span", MB_OK );
   }

   return 1;
}



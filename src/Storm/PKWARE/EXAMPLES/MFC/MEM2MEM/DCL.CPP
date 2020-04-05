/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1994,1995 by PKWARE Inc. All Rights Reserved.     ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */

/*
 * DCL.cpp - File to call various DCL DLL functions
 */

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#include "implode.h"
 
typedef enum 
{
   COMPRESSING = 1,
   UNCOMPRESSING
} FILEMODE;

typedef struct
{
   PBYTE Buffer;                 // POINTER TO BUFFER
   UINT     CurPos;                 // CURRENT POSITION IN BUFFER
   UINT     BuffSize;               // SIZE OF THE BUFFER
} BUFFER_BLOCK, *PBUFFER_BLOCK;

// STRUCT TO PASS TO THE FILE IO FUNCTIONS
typedef struct
{
   BUFFER_BLOCK   FileBuff;         // FILE BUFFER
   BUFFER_BLOCK   cmpBuff;          // COMPRESSION BUFFER
   BUFFER_BLOCK   uncmpBuff;        // UNCOMPRESSION BUFFER
   FILEMODE       mode;
   DWORD          dwCrc;            // CRC
   UINT           nCompressSize;
   BOOL           ErrorOccurred;    // ERROR FLAG
} DATABLOCK, *PDATABLOCK;

UINT DataType = CMP_ASCII;          // GLOBAL FOR DATA TYPE FOR COMPRESSION
UINT DictSize = 4096;               // GLOBAL FOR DICTIONARY SIZE FOR COMPRESSION

static int iLineCnt;                // CURRENT LINE TO OUTPUT STRING

/*********************************************************************
 *
 * Function:   ReadBuffer()
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
 *                         pointer to the DATABLOCK
 *
 * Returns:    Number of bytes actually read, or zero on EOF
 *
 *********************************************************************/
UINT ReadBuffer(PCHAR buffer, UINT *iSize, void *pParam)
{
   PDATABLOCK pDataBlock;
   PBUFFER_BLOCK pBufferBlock;
   UINT iRead;

   pDataBlock = (PDATABLOCK) pParam;

   // IF AN ERROR OCCURRED
   if( pDataBlock->ErrorOccurred == TRUE )
   {
      return 0;
   }

   if( pDataBlock->mode == COMPRESSING )
   {
      // SINCE COMPRESSING THEN WANT TO WRITE DATA TO THE COMPRESSION BUFFER
      pBufferBlock = &pDataBlock->FileBuff;
   }
   else
   {
      // SINCE COMPRESSING THEN WANT TO WRITE DATA TO THE UNCOMPRESSION BUFFER
      pBufferBlock = &pDataBlock->cmpBuff;
   }

   if( pBufferBlock->CurPos < pBufferBlock->BuffSize )
   {
      UINT BytesLeft = pBufferBlock->BuffSize - pBufferBlock->CurPos;

      // IF REQUESTING MORE BYTES THAN ARE LEFT
      if( BytesLeft < *iSize )
      {
         // SET NUMBER OF BYTES TO COPY TO WHAT IS LEFT
         *iSize = BytesLeft;
      }

      // COPY BYTES AND UPDATE COUNTER
      memcpy( buffer, (pBufferBlock->Buffer + pBufferBlock->CurPos), *iSize );
      pBufferBlock->CurPos += *iSize;

      iRead = *iSize;
   }
   else // ELSE - NOTHING LEFT IN BUFFER SO RETURN 0
   {
      iRead = 0;
   }

   // IF COMPRESSING, THEN CALCULATE THE CRC
   if( pDataBlock->mode == COMPRESSING )
   {
      pDataBlock->dwCrc = crc32( buffer, &iRead, &pDataBlock->dwCrc );
   }

   return iRead;
}

/*********************************************************************
 *
 * Function:   WriteBuffer()
 *
 * Purpose:    To handle calls from the Data Compression Library for
 *             write requests. 
 *                                   
 * Parameters: buffer ->   Address of buffer to write data from
 *             iSize ->    Number of bytes to write
 *             dwParam ->  User-defined parameter, in this case a
 *                         pointer to the DATABLOCK
 *
 * Returns:    Zero, the return value is not used by the Data 
 *             Compression Library
 *
 *********************************************************************/
void WriteBuffer(PCHAR buffer, UINT *iSize, void *pParam)
{
   PDATABLOCK  pDataBlock;
   PBUFFER_BLOCK pBufferBlock;

   pDataBlock = (PDATABLOCK) pParam;

   // IF AN ERROR OCCURRED
   if( pDataBlock->ErrorOccurred == TRUE )
   {
      return;
   }

   if( pDataBlock->mode == COMPRESSING )
   {
      // SINCE COMPRESSING THEN WANT TO WRITE DATA TO THE COMPRESSION BUFFER
      pBufferBlock = &pDataBlock->cmpBuff;

      // SINCE COMPRESSING, KEEP A TOTAL OF THE COMPRESSED FILE SIZE
      pDataBlock->nCompressSize += *iSize;
   }
   else
   {
      // SINCE COMPRESSING THEN WANT TO WRITE DATA TO THE UNCOMPRESSION BUFFER
      pBufferBlock = &pDataBlock->uncmpBuff;
   }

   // IF NOT OUT OF BUFFER SPACE
   if( pBufferBlock->CurPos < pBufferBlock->BuffSize )
   {
      // IF WRITING MORE BYTES THAN ARE LEFT
      if( (pBufferBlock->BuffSize - pBufferBlock->CurPos) < *iSize )
      {
         MessageBox( NULL, "Out of buffer space - #1", "Compression Error", MB_OK );
         pDataBlock->ErrorOccurred = TRUE;
         return;
      }

      // COPY BYTES AND UPDATE COUNTER
      memcpy( (pBufferBlock->Buffer + pBufferBlock->CurPos),
               buffer, *iSize );
      pBufferBlock->CurPos += *iSize;
   }
   else // ELSE - NOTHING LEFT IN BUFFER SO RETURN 0
   {
      MessageBox( NULL, "Out of buffer space - #2", "Compression Error", MB_OK );
      pDataBlock->ErrorOccurred = TRUE;
      return;
   }

   // IF COMPRESSING, THEN CALCULATE THE CRC
   if (pDataBlock->mode == UNCOMPRESSING )
   {
      pDataBlock->dwCrc = crc32( buffer, iSize, &pDataBlock->dwCrc );
   }

   return;
}

/*********************************************************************
 *
 * Function:   CompressMemToMem()
 *
 * Purpose:    To compress a buffer to another buffer in memory.
 *
 *
 * Parameters: HWnd   ->   Handle to window
 *             pDC    ->   Pointer to a device context
 *             pdwCrc ->   Pointer to DWORD buffer to return the CRC 
 *                         of the compressed file before compression
 *             pnCompressedSize -> Number of bytes in the compressed
 *                                 buffer
 *             pFileBuffer -> Pointer to buffer to compress
 *             pCompressedBuffer -> Pointer to buffer to place 
 *                                   compressed data
 *             BuffSize -> Size of the buffers (both are allocated
 *                         for same number of bytes)
 *
 * Returns:    1 -> Successful completion
 *             0 -> Error occurred
 *
 *********************************************************************/
int CompressMemToMem( HWND hWnd, CDC *pDC, DWORD *pdwCrc, 
                      UINT *pnCompressedSize, PBYTE pFileBuffer, 
                      PBYTE pCompressedBuffer, UINT BuffSize )
{
   int iStatus;
   int rc = 1;
   char szVerbose[128];
   DATABLOCK DataBlock;
   PCHAR  pScratchPad;

   if( (pScratchPad = (PCHAR) new char[CMP_BUFFER_SIZE]) == NULL )
   {
      return 0;
   }

   memset( &DataBlock, 0, sizeof(DataBlock) );
    
   // SETUP STRUCTURE USED BY ProcessReadBuffer() AND ProcessWriteBuffer()
   DataBlock.mode = COMPRESSING;
   DataBlock.dwCrc = ~((DWORD)0);            // Pre-condition CRC

   // SETUP BUFFER BLOCK FOR FILE BUFFER
   DataBlock.FileBuff.Buffer = pFileBuffer;
   DataBlock.FileBuff.BuffSize = BuffSize;

   // SETUP BUFFER BLOCK FOR COMPRESSION BUFFER
   DataBlock.cmpBuff.Buffer = pCompressedBuffer;
   DataBlock.cmpBuff.BuffSize = BuffSize;

   wsprintf( szVerbose, "Compressing %u byte buffer to memory    ", BuffSize );
   pDC->TextOut( 10, (iLineCnt++ * 20) + 5, szVerbose );

   // COMPRESS THE FILE
   iStatus = implode( ReadBuffer, WriteBuffer,
                      pScratchPad, &DataBlock, &DataType, &DictSize );

   // IF THERE WAS AN ERROR COMPRESSING FILE
   if( iStatus || DataBlock.ErrorOccurred )
   {
      // DISPLAY ERROR STRING IF ERROR OCCURRED IN IMPLODE
      wsprintf( szVerbose, "Error occurred while imploding - %d ", iStatus );
      MessageBox( hWnd, szVerbose, "Error", MB_OK );
      rc = 0;
   }
   else // ELSE - COMPRESSION WAS SUCCESSFUL
   {
      // POST-CONDITION CRC
      DataBlock.dwCrc = ~DataBlock.dwCrc;

      // RETURN CRC
      *pdwCrc = DataBlock.dwCrc;

      // RETURN COMPRESSED BUFFER SIZE
      *pnCompressedSize = DataBlock.nCompressSize;
      
      wsprintf( szVerbose, "Compressed file to memory -> CRC = %08lX    ", 
                DataBlock.dwCrc );
      pDC->TextOut( 10, (iLineCnt++ * 20) + 5, szVerbose );
   }

   delete pScratchPad;

   return rc;
}


/*********************************************************************
 *
 * Function:   ExpandMemToMem()
 *
 * Purpose:    To expand a compressed buffer to a buffer in memory.
 *
 *
 * Parameters: HWnd   ->   Handle to window
 *             pDC    ->   Pointer to a device context
 *             pdwCrc ->   Pointer to DWORD buffer to return the CRC 
 *                         of the compressed file after uncompression
 *             pCompressedBuffer -> Pointer to buffer to place 
 *                                   compressed data
 *             nCompressedSize -> Number of bytes in the compressed
 *                                buffer
 *             pUncompressedBuffer -> Pointer to buffer to place 
 *                                     uncompressed data
 *             BuffSize -> Size of the uncompressed buffer
 *
 * Returns:    1 -> Successful completion
 *             0 -> Error occurred
 *
 *********************************************************************/
int ExpandMemToMem( HWND hWnd, CDC *pDC, DWORD *pdwCrc,
                    PBYTE pCompressedBuffer, UINT nCompressedSize, 
                    PBYTE pUncompressedBuffer, UINT BuffSize )
{
   int iStatus;
   int rc = 1;
   char szVerbose[128];
   DATABLOCK DataBlock;
   PCHAR  pScratchPad;

   if( (pScratchPad = (PCHAR) new char[CMP_BUFFER_SIZE]) == NULL )
   {
      return 0;
   }

   memset( &DataBlock, 0, sizeof(DataBlock) );
    
   // SETUP STRUCTURE USED BY ProcessReadBuffer() AND ProcessWriteBuffer()
   DataBlock.mode = UNCOMPRESSING;
   DataBlock.dwCrc = ~((DWORD)0);            // Pre-condition CRC

   // SETUP BUFFER BLOCK FOR COMPRESSION BUFFER
   DataBlock.cmpBuff.Buffer = pCompressedBuffer;
   DataBlock.cmpBuff.BuffSize = nCompressedSize;

   // SETUP BUFFER BLOCK FOR UNCOMPRESSION BUFFER
   DataBlock.uncmpBuff.Buffer = pUncompressedBuffer;
   DataBlock.uncmpBuff.BuffSize = BuffSize;

   wsprintf( szVerbose, "Compressed buffer size = %u    ", nCompressedSize );
   pDC->TextOut( 10, (iLineCnt++ * 20) + 5, szVerbose );

   pDC->TextOut( 10, (iLineCnt++ * 20) + 5, "Uncompressing buffer to memory  " );

   // UNCOMPRESS THE FILE
   iStatus = explode( ReadBuffer, WriteBuffer, pScratchPad, &DataBlock );

   // IF THERE WAS AN ERROR UNCOMPRESSING FILE
   if( iStatus || DataBlock.ErrorOccurred )
   {
      wsprintf( szVerbose, "Error occurred while exploding - %d ", iStatus );
      MessageBox( hWnd, szVerbose, "Error", MB_OK );
      rc = 0;
   }
   else // ELSE - UNCOMPRESSION WAS SUCCESSFUL
   {
      // POST-CONDITION CRC
      DataBlock.dwCrc = ~DataBlock.dwCrc;

      // RETURN CRC
      *pdwCrc = DataBlock.dwCrc;

      wsprintf( szVerbose, "Uncompressed file to memory -> CRC = %08lX   ", 
                DataBlock.dwCrc );
      pDC->TextOut( 10, (iLineCnt++ * 20) + 5, szVerbose );
   }

   delete pScratchPad;

   return rc;
}


/*********************************************************************
 *
 * Function:   MemToMemExample()
 *
 * Purpose:    To load a file into memory. Then compress and uncompress 
 *             the buffer in memory.
 *
 *
 * Parameters: HWnd   ->   Handle to window
 *             pDC    ->   Pointer to a device context
 *             pszFilename -> Name of file to load
 *
 * Returns:    1 -> Successful completion
 *             0 -> Error occurred
 *
 *********************************************************************/
int MemToMemExample( HWND hWnd, CDC *pDC, PCHAR pszFilename )
{
   ASSERT( hWnd );
   ASSERT_VALID( pDC );

   CFile  InFile;
   int    rc=1;                        // RETURN CODE
   UINT   BufferSize;
   UINT   cmpSize;
   PBYTE pFileBuffer;                  // BUFFER FOR FILE DATA
   PBYTE pCompressedBuffer;            // BUFFER FOR THE COMPRESSED DATA
   PBYTE pUncompressedBuffer;       // BUFFER FOR THE UNCOMPRESSED DATA
   DWORD  cmpCrc;                      // CRC OF FILE BEFORE COMPRESSION
   DWORD  uncmpCrc;                    // CRC OF FILE AFTER UNCOMPRESSION


   iLineCnt = 0;
   
   // OPEN THE FILE
   if( !InFile.Open( pszFilename, 
                     CFile::modeRead | CFile::shareExclusive | CFile::typeBinary ) )
   {
      MessageBox( hWnd, "Error opening file for compression", "Error", MB_OK );
      return 0;
   }

   // CHECK IF FILE IS TOO LARGE
   if( InFile.GetLength() > 64000U )
   {
      MessageBox( hWnd, "File is too large to compress to memory", "Error", MB_OK );
      return 0;
   }

   BufferSize = (UINT) InFile.GetLength();

   // ALLOCATE BUFFER MEMORY
   pFileBuffer = (PBYTE) new char[BufferSize];
   pCompressedBuffer = (PBYTE) new char[BufferSize];
   pUncompressedBuffer = (PBYTE) new char[BufferSize];

   // IF SUCCESSFULLY ALLOCATED MEMORY
   if( (pFileBuffer != NULL) &&
       (pCompressedBuffer != NULL) &&
       (pUncompressedBuffer != NULL)  )
   {
      // READ FILE
      InFile.Read( pFileBuffer, BufferSize );

      // IF COMPRESSED OK
      if( CompressMemToMem( hWnd, pDC, &cmpCrc, &cmpSize,
                            pFileBuffer, pCompressedBuffer, BufferSize ) )
      {
         // IF ERROR UNCOMPRESSING
         if( !ExpandMemToMem( hWnd, pDC, &uncmpCrc,
                              pCompressedBuffer, cmpSize, 
                              pUncompressedBuffer, BufferSize ) )
         {
            MessageBox( hWnd, "Error uncompressing to memory", "Error", MB_OK );
            rc = 0;
         }
      }
      else
      {
         MessageBox( hWnd, "Error compressing to memory", "Error", MB_OK );
         rc = 0;
      }
   }


   if( pFileBuffer != NULL )
   {
      delete pFileBuffer;
   }

   if( pCompressedBuffer != NULL )
   {
      delete pCompressedBuffer;
   }

   if( pUncompressedBuffer != NULL )
   {
      delete pUncompressedBuffer;
   }
   
   return rc;
}



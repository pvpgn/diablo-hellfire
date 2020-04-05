/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1994,1995 by PKWARE Inc. All Rights Reserved.     ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
#include <windows.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "resource.h"
#include "WinDCL.h"
#include "implode.h"

void TestDLL(void);

#ifndef _MSC_VER
   #pragma argsused
#endif

int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
   WNDCLASS  wndclass;
   MSG msg;

   // Get rid of any compiler warnings
   lpszCmdLine = lpszCmdLine;

   hInst = hInstance;

   // Register the DCL Test window Class
   if(!hPrevInstance)
   {
      wndclass.style = CS_HREDRAW | CS_VREDRAW;
      wndclass.lpfnWndProc = WndProc;

      wndclass.cbClsExtra = 0;
      wndclass.cbWndExtra = 0;
      wndclass.hInstance = hInst;
      wndclass.hIcon = LoadIcon(hInst, "WinDCL");
      wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
      wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
      wndclass.lpszMenuName = "WinDCL";
      wndclass.lpszClassName = "WinDCL";

      if (!RegisterClass(&wndclass))
         return FALSE;
   }

   // create Main window
   hWndMain = CreateWindow(
      "WinDCL",
      "DCL Example",           // no title
      WS_CAPTION      |        // Title and Min/Max
      WS_SYSMENU      |        // Add system menu box
      WS_MINIMIZEBOX  |        // Add minimize box
      WS_MAXIMIZEBOX  |        // Add maximize box
      WS_THICKFRAME   |        // thick sizeable frame
      WS_CLIPCHILDREN |        // don't draw in child windows areas
      WS_VISIBLE      |        // window created visible
      WS_OVERLAPPED,
      CW_USEDEFAULT, 0,        // Use default X, Y
      CW_USEDEFAULT, 0,        // Use default X, Y
      NULL,                    // Parent window's handle
      NULL,                    // Default to Class Menu
      hInst,                   // Instance of window
      NULL);                   // Create struct for WM_CREATE

   // Did the Create Work??
   if(hWndMain == NULL)
      return 0;

   ShowWindow(hWndMain, nCmdShow);

   while (GetMessage(&msg, NULL, 0, 0))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
   return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch (Message)
   {
      case WM_COMMAND:
         if (wParam == IDM_TEST_DCL)
            TestDLL();
         break;
      case WM_CLOSE:
         DestroyWindow(hWnd);
         break;
      case WM_DESTROY:
         PostQuitMessage(0);
         break;
      default:
         return DefWindowProc(hWnd, Message, wParam, lParam);
   }
   return 0L;
}

UINT ProcessInBuffer(PCHAR buffer, UINT *iSize, void *pParam)
{
   LPIOFILEBLOCK lpFileIOBlock;
   unsigned int iRead;

   lpFileIOBlock = (LPIOFILEBLOCK) pParam;

   iRead = fread(buffer, 1, *iSize, lpFileIOBlock->InFile );

   if( iRead > 0 && lpFileIOBlock->bDoCRC == DO_CRC_INSTREAM )
      lpFileIOBlock->dwCRC = crc32(buffer, &iRead, &lpFileIOBlock->dwCRC);

   return iRead;
}

void ProcessOutBuffer(PCHAR buffer, UINT *iSize, void *pParam)
{
   LPIOFILEBLOCK lpFileIOBlock;
   unsigned int iWrite;

   lpFileIOBlock = (LPIOFILEBLOCK) pParam;

   iWrite = fwrite( buffer, 1, *iSize, lpFileIOBlock->OutFile );

   if( lpFileIOBlock->bDoCRC == DO_CRC_OUTSTREAM )
      lpFileIOBlock->dwCRC = crc32(buffer, &iWrite, &lpFileIOBlock->dwCRC);
}

void TestDLL()
{
   int iStatus;
   char szVerbose[128];
   IOFILEBLOCK FileIOBlock;
   HGLOBAL hWorkBuff;
   PCHAR  pWorkBuff;
   unsigned int type;            /* ASCII or Binary compression   */
   unsigned int dsize;           /* Dictionary Size. 1,2 or 4K    */

   type  = CMP_ASCII;                  /* Use ASCII compression */
   dsize = 4096;                       /* Use 4K dictionary     */

   // allocate the memory block for the scratch pad
   if( (hWorkBuff = GlobalAlloc(GHND, CMP_BUFFER_SIZE)) == NULL )
   {
      return;
   }

   if ((pWorkBuff = (LPSTR) GlobalLock(hWorkBuff)) == NULL)
   {
      GlobalFree(hWorkBuff);
      return;
   }

   // setup structure used by ProcessReadBuffer() and ProcessWriteBuffer()
   FileIOBlock.InFile = fopen( "Test.in", "rb" );
   FileIOBlock.OutFile = fopen( "Test.cmp", "wb" );
   FileIOBlock.bDoCRC = DO_CRC_INSTREAM;
   FileIOBlock.dwCRC = ~((DWORD)0);  // Pre-condition CRC

   if( (FileIOBlock.InFile != NULL) && (FileIOBlock.OutFile != NULL) )
   {
      MessageBox(NULL, "Ready to implode", "Notice", MB_OK);

      iStatus = implode(ProcessInBuffer,
                        ProcessOutBuffer,
                        pWorkBuff,
                        &FileIOBlock,
                        &type, &dsize );

      if( iStatus != 0 )
      {
         wsprintf(szVerbose, "Implode Error: %d", iStatus );
         MessageBox(NULL, szVerbose, "Error", MB_OK);
      }
      else
      {
         // Post-condition CRC
         if (FileIOBlock.bDoCRC == DO_CRC_INSTREAM)
         {
            FileIOBlock.dwCRC = ~FileIOBlock.dwCRC;
            wsprintf(szVerbose, "CRC of input file: %lX", FileIOBlock.dwCRC);
            MessageBox(NULL, szVerbose, "Notice", MB_OK);
         }
      }

      fclose(FileIOBlock.OutFile);
      fclose(FileIOBlock.InFile);

      if( iStatus == 0 )
      {
         // setup structure used by ProcessReadBuffer() and ProcessWriteBuffer()
         FileIOBlock.InFile = fopen( "Test.cmp", "rb" );
         FileIOBlock.OutFile = fopen( "Test.ext", "wb" );
         FileIOBlock.bDoCRC = DO_CRC_OUTSTREAM;
         FileIOBlock.dwCRC = ~((DWORD)0);  // Pre-condition CRC

         MessageBox(NULL, "Ready to explode", "Notice", MB_OK);
         iStatus = explode(ProcessInBuffer,
                           ProcessOutBuffer,
                           pWorkBuff,
                           &FileIOBlock );

         if( iStatus != 0 )
         {
            wsprintf(szVerbose, "Explode Error: %d", iStatus );
            MessageBox(NULL, szVerbose, "Error", MB_OK);
         }
         else
         {
            // Post-condition CRC
            if (FileIOBlock.bDoCRC == DO_CRC_OUTSTREAM)
            {
               FileIOBlock.dwCRC = ~FileIOBlock.dwCRC;
               wsprintf(szVerbose, "CRC of exploded file: %lX", FileIOBlock.dwCRC);
               MessageBox(NULL, szVerbose, "Notice", MB_OK);
            }
         }

         fclose(FileIOBlock.OutFile);
         fclose(FileIOBlock.InFile);
      }
   }
   else
   {
      if( FileIOBlock.InFile != NULL )
      {
         fclose( FileIOBlock.InFile );
      }

      if( FileIOBlock.OutFile != NULL )
      {
         fclose( FileIOBlock.OutFile );
      }

      MessageBox(NULL, "The file TEST.IN must be in the current directory", "Error", MB_OK);
   }

   GlobalUnlock(hWorkBuff);
   GlobalFree(hWorkBuff);
}


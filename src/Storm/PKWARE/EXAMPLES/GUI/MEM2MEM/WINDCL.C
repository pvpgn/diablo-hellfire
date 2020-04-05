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
         {
            HDC hDC;

            hDC = GetDC( hWnd );
            SetBkMode( hDC, TRANSPARENT );
            MemToMemExample( hWnd, hDC, "TEST.IN" );
            ReleaseDC( hWnd, hDC );
         }
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



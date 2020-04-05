/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1994,1995 by PKWARE Inc. All Rights Reserved.     ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */

#define DO_CRC_INSTREAM          1
#define DO_CRC_OUTSTREAM         2

#define WM_FAILEDVALIDATE (WM_USER + 1)

typedef struct IOFILEBLOCK {
    FILE *InFile;
    FILE *OutFile;
    BOOL bDoCRC;
    DWORD dwCRC;
}IOFILEBLOCK, *LPIOFILEBLOCK;

HWND hInst;
HWND hWndMain;

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);


/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1995 by PKWARE Inc. All Rights Reserved.          ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */

int CompressFileToMCF( HWND hWnd, CDC *pDC, 
                       LPSTR lpszMcfFile, LPSTR lpszFileToCompress, 
                       LPSTR lpszFilenameOnly, BOOL NewMcfFile );
int UncompressMcfFile( HWND hWnd, CDC *pDC, LPSTR lpszMcfFile );

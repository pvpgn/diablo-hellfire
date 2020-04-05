/****************************************************************************
*
*  message.cpp
*  
*  MessageBox routines.
*
*  By Michael Morhaime
*
***/

#include "pch.h"




//****************************************************************************
BOOL UiMessageBox(SNETMESSAGEBOXPROC messageboxcallback, HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType) {
	BOOL bResult;

	// Make sure we have a visible cursor for the message box.
	ShowCursor(TRUE);

	if (messageboxcallback)
		bResult = messageboxcallback(hWnd, lpText, lpCaption, uType);
	else
		bResult = MessageBox(SDrawGetFrameWindow(), lpText, lpCaption, uType);

	ShowCursor(FALSE);

	return bResult;
}

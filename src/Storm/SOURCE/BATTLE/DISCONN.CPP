/****************************************************************************
*
*  disconn.CPP
*  battle.net user interface functions
*
*  By Michael Morhaime
*
***/

#include "pch.h"



//********************************************
//********************************************
BOOL CALLBACK RasEnumCallback(LPCTSTR szEntryName, LPVOID rashandle, LPVOID lpContext) {
	SNETUIDATAPTR interfacedata;
	char szFmt[256];
	char szMsg[512];
	char szTitle[32];
	BOOL bResult;

	interfacedata = (SNETUIDATAPTR) lpContext;

	LoadString(global_hinstance, IDS_QUERYDISCONNECT, szFmt, sizeof(szFmt));
	LoadString(global_hinstance, IDS_BATTLENET, szTitle, sizeof(szTitle));
	sprintf(szMsg, szFmt, szEntryName);

	if (interfacedata->messageboxcallback && GetActiveWindow())
		bResult = UiMessageBox(interfacedata->messageboxcallback, GetActiveWindow(), szMsg, szTitle, MB_OKCANCEL);
	else 
		bResult = MessageBox(GetActiveWindow(), szMsg, szTitle, MB_OKCANCEL);

	if (bResult == IDOK)
		HangupRASConnection(rashandle);

	return 1;
}


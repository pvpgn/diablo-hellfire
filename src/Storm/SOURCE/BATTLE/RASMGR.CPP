/****************************************************************************
*
*  RASMGR.CPP
*  Remote Access Service Connection Manager
*
*  By Jeff Strain (12/9/96)
*
***/

#include "pch.h"

#define MAXACTIVECONNECTIONS 10

typedef DWORD (CALLBACK *RASHANGUP)(HRASCONN);
typedef DWORD (CALLBACK *RASENUMCONN)(LPRASCONN, LPDWORD, LPDWORD);
typedef DWORD (CALLBACK *RASGETSTATUS)(HRASCONN, LPRASCONNSTATUS);

static HRASCONN     sg_ahActiveConn[MAXACTIVECONNECTIONS] = {0};
static HINSTANCE    sg_hRasDLL = NULL;
static RASENUMCONN  sg_pRasEnumConnections = NULL;
static RASHANGUP    sg_pRasHangUp = NULL;
static RASGETSTATUS sg_pRasGetConnectStatus = NULL;

/****************************************************************************
*
*  Private Functions
*
***/

// GETS A SAFE RASCONN STRUCTURE. IT IS UP TO THE CALLING FUNCTION TO
// FREE THE STRUCTURE WHEN IT IS DONE WITH IT!
static LPRASCONN WINAPI InternalRasEnumConnections(LPDWORD dwConnections)
{
    LPRASCONN lpRasConn = NULL;
    DWORD dwBytes;
    DWORD dwReturn;

    // FILL OUT A SINGLE RASCONN STRUCTURE
    dwBytes = sizeof(RASCONN);
    // LOOP UNTIL WE HAVE ALLOCATED A LARGE ENOUGH BUFFER TO HOLD ALL
    // ACTIVE RAS CONNECTIONS
    do {
        if (lpRasConn)
            LocalFree(lpRasConn);
        lpRasConn = (LPRASCONN) LocalAlloc(LPTR, dwBytes);
        ASSERT(lpRasConn);
        if (!lpRasConn)
           return NULL;

        lpRasConn[0].dwSize = sizeof(RASCONN);
        dwReturn = sg_pRasEnumConnections(lpRasConn, &dwBytes, dwConnections);

        if (!(!dwReturn || dwReturn == ERROR_BUFFER_TOO_SMALL)) {
            if (lpRasConn)
                LocalFree(lpRasConn);
            return NULL;
        }
    } while (dwReturn == ERROR_BUFFER_TOO_SMALL);

    return lpRasConn;
}

/****************************************************************************
*
*  Public Session Management Functions
*
***/

// SHOULD BE CALLED ONE TIME ONLY AT STARTUP. STORES ALL ACTIVE
// RAS CONNECTIONS AND USES THIS AS A FILTER FOR ENUMNEWCALLS()
BOOL WINAPI InitRASManager()
{
    LPRASCONN lpRasConn = NULL;
    DWORD dwConnections;

    // MANUALLY LOAD RAS DLL SINCE SYSTEMS WITHOUT DIALUP NETWORKING
    // INSTALLED WON'T HAVE IT.
    if (!sg_hRasDLL) {
        sg_hRasDLL = LoadLibrary(TEXT("rasapi32"));
        if (!sg_hRasDLL)
            return FALSE;
    }
#ifdef _UNICODE
    sg_pRasEnumConnections =
        (RASENUMCONN)GetProcAddress(sg_hRasDLL, TEXT("RasEnumConnectionsW"));
    sg_pRasHangUp =
        (RASHANGUP)GetProcAddress(sg_hRasDLL, TEXT("RasHangUpW"));
    sg_pRasGetConnectStatus =
        (RASGETSTATUS)GetProcAddress(sg_hRasDLL, TEXT("RasGetConnectStatusW"));
#else
    sg_pRasEnumConnections =
        (RASENUMCONN)GetProcAddress(sg_hRasDLL, TEXT("RasEnumConnectionsA"));
    sg_pRasHangUp =
        (RASHANGUP)GetProcAddress(sg_hRasDLL, TEXT("RasHangUpA"));
    sg_pRasGetConnectStatus =
        (RASGETSTATUS)GetProcAddress(sg_hRasDLL, TEXT("RasGetConnectStatusA"));
#endif
    ASSERT(sg_pRasEnumConnections && sg_pRasHangUp && sg_pRasGetConnectStatus);
    if (!(sg_pRasEnumConnections && sg_pRasHangUp && sg_pRasGetConnectStatus))
        return FALSE;

    lpRasConn = InternalRasEnumConnections(&dwConnections);
    if (!lpRasConn)
        return FALSE;

    for (DWORD i = 0; i < min(dwConnections, MAXACTIVECONNECTIONS); i++)
        sg_ahActiveConn[i] = lpRasConn[i].hrasconn;
    if (i < MAXACTIVECONNECTIONS)
        sg_ahActiveConn[i] = NULL;

    if (lpRasConn)
        LocalFree(lpRasConn);

    return TRUE;
}

//===========================================================================
DWORD WINAPI GetNumActiveRASConnections()
{
    LPRASCONN lpRasConn = NULL;
    DWORD dwConnections;

    if (!sg_hRasDLL)
        return 0;

    lpRasConn = InternalRasEnumConnections(&dwConnections);
    ASSERT(lpRasConn);
    if (!lpRasConn)
        return 0;

    if (lpRasConn)
        LocalFree(lpRasConn);

    return dwConnections;
}

//===========================================================================
BOOL WINAPI EnumNewRASConnections(LPRASENUMCALLBACK lpCallback,
                                  LPVOID lpContext)
{
    LPRASCONN lpRasConn = NULL;
    DWORD dwConnections;

    if (!sg_hRasDLL)
        return FALSE;

    ASSERT(lpCallback);
    if (!lpCallback)
        return FALSE;

    lpRasConn = InternalRasEnumConnections(&dwConnections);
    if (!lpRasConn)
        return FALSE;

    // CALL THE CALLBACK WITH THE NAME AND HANDLE OF EACH RAS CONNECTION
    BOOL bReturn;
    for (DWORD i = 0; i < dwConnections; i++) {
        for (DWORD j = 0; j < MAXACTIVECONNECTIONS; j++) {
            if (!sg_ahActiveConn[j])
                break;
            if (sg_ahActiveConn[j] == lpRasConn[i].hrasconn)
                break;
        }
        if (j == MAXACTIVECONNECTIONS || !sg_ahActiveConn[j]) {
            bReturn = lpCallback((LPCTSTR)lpRasConn[i].szEntryName,
                                 (LPVOID)lpRasConn[i].hrasconn, lpContext);
            if (bReturn)
                break;
        }
    }

    if (lpRasConn)
        LocalFree(lpRasConn);

    return TRUE;
}

//===========================================================================
BOOL WINAPI EnumActiveRASConnections(LPRASENUMCALLBACK lpCallback,
                                     LPVOID lpContext)
{
    LPRASCONN lpRasConn = NULL;
    DWORD dwConnections;

    if (!sg_hRasDLL)
        return FALSE;

    ASSERT(lpCallback);
    if (!lpCallback)
        return FALSE;

    lpRasConn = InternalRasEnumConnections(&dwConnections);
    ASSERT(lpRasConn);
    if (!lpRasConn)
        return FALSE;

    // CALL THE CALLBACK WITH THE NAME AND HANDLE OF EACH RAS CONNECTION
    BOOL bReturn;
    for (DWORD i = 0; i < dwConnections; i++) {
        bReturn = lpCallback((LPCTSTR)lpRasConn[i].szEntryName,
                             (LPVOID)lpRasConn[i].hrasconn, lpContext);
        if (bReturn)
            break;
    }

    if (lpRasConn)
        LocalFree(lpRasConn);

    return TRUE;
}

//===========================================================================
// NOTE THAT THIS IS POTENTIALLY A BLOCKING CALL
BOOL WINAPI HangupRASConnection(LPVOID rasconn)
{
    DWORD dwReturn;
    RASCONNSTATUS rcStatus;
    HRASCONN hRasConn = (HRASCONN)rasconn;

    ASSERT(sg_hRasDLL);
    if (!sg_hRasDLL)
        return FALSE;

    ASSERT(hRasConn);
    if (!hRasConn)
        return FALSE;

    // HANGUP LINE
    dwReturn = sg_pRasHangUp(hRasConn);
    ASSERT(!dwReturn);
    if (dwReturn)
        return FALSE;

    // WAIT FOR STATE MACHINE TO RESET PORT PROPERLY
    rcStatus.dwSize = sizeof(RASCONNSTATUS);
    while (sg_pRasGetConnectStatus(hRasConn, &rcStatus) != ERROR_INVALID_HANDLE)
        Sleep(0);

    return TRUE;
}

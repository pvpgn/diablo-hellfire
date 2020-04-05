/****************************************************************************
*
*  NULL.CPP
*  Null provider
*
*  By Michael O'Brien (6/27/96)
*
***/

#include "pch.h"
#pragma  hdrstop

#define  MAXMESSAGESIZE  512

/****************************************************************************
*
*  SERVICE PROVIDER INTERFACE FUNCTIONS
*
***/

//===========================================================================
BOOL CALLBACK NullCompareNetAddresses (SNETADDRPTR  addr1,
                                       SNETADDRPTR  addr2,
                                       DWORD       *diffmagnitude) {
  if (diffmagnitude)
    *diffmagnitude = 0;
  if (!(addr1 && addr2)) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }
  if (diffmagnitude)
    *diffmagnitude = !memcmp(addr1,addr2,sizeof(SNETADDR));
  return 1;
}

//===========================================================================
BOOL CALLBACK NullDestroy () {
  return 1;
}

//===========================================================================
BOOL CALLBACK NullFree (SNETADDRPTR addr,
                        LPVOID      data,
                        DWORD       databytes) {
  if (!(addr && data)) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }
  return 1;
}

//===========================================================================
BOOL CALLBACK NullFreeExternalMessage (LPCSTR senderpath,
                                       LPCSTR sendername,
                                       LPCSTR mesage) {
  SetLastError(SNET_ERROR_INVALID_PARAMETER);
  return 0;
}

//===========================================================================
BOOL CALLBACK NullGetGameInfo (DWORD             gameid,
                               LPCSTR            gamename,
                               LPCSTR            gamepassword,
                               SNETSPI_GAMELIST *gameinfo) {
  SetLastError(SNET_ERROR_GAME_NOT_FOUND);
  return 0;
}

//===========================================================================
BOOL CALLBACK NullGetPerformanceData (DWORD          counterid,
                                      DWORD         *countervalue,
                                      LARGE_INTEGER *measurementtime,
                                      LARGE_INTEGER *measurementfreq) {
  return PerfGetPerformanceData(counterid,
                                countervalue,
                                measurementtime,
                                measurementfreq);
}

//===========================================================================
BOOL CALLBACK NullInitialize (SNETPROGRAMDATAPTR programdata,
                              SNETPLAYERDATAPTR  playerdata,
                              SNETUIDATAPTR      interfacedata,
                              SNETVERSIONDATAPTR versiondata,
                              HANDLE             event) {
  PerfReset();
  return 1;
}

//===========================================================================
BOOL CALLBACK NullInitializeDevice (DWORD              deviceid,
                                    SNETPROGRAMDATAPTR programdata,
                                    SNETPLAYERDATAPTR  playerdata,
                                    SNETUIDATAPTR      interfacedata,
                                    SNETVERSIONDATAPTR versiondata) {

  // WE NEVER RETURN ANY DEVICES, SO THIS FUNCTION SHOULD NEVER BE CALLED
  return 0;
}

//===========================================================================
BOOL CALLBACK NullLockDeviceList (SNETSPI_DEVICELISTPTR *devicelist) {
  *devicelist = NULL;
  return 1;
}

//===========================================================================
BOOL CALLBACK NullLockGameList (DWORD                categorybits,
                                DWORD                categorymask,
                                SNETSPI_GAMELISTPTR *gamelist) {
  if (gamelist)
    *gamelist = NULL;
  if (!gamelist) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }
  return 1;
}

//===========================================================================
/*
BOOL CALLBACK NullReceive (SNETADDRPTR *addr,
                           LPVOID      *data,
                           DWORD       *databytes) {
*/
BOOL CALLBACK NullReceive (LPVOID      *data,
                           DWORD       *databytes,
                           SNETADDRPTR *addr) {
  if (addr)
    *addr = NULL;
  if (data)
    *data = NULL;
  if (databytes)
    *databytes = NULL;
  SetLastError(SNET_ERROR_NO_MESSAGES_WAITING);
  return 0;
}

//===========================================================================
BOOL CALLBACK NullReceiveExternalMessage (LPCSTR *senderpath,
                                          LPCSTR *sendername,
                                          LPCSTR *message) {
  if (senderpath)
    *senderpath = NULL;
  if (sendername)
    *sendername = NULL;
  if (message)
    *message = NULL;

  SetLastError(SNET_ERROR_NO_MESSAGES_WAITING);
  return 0;
}

//===========================================================================
BOOL CALLBACK NullSelectGame (DWORD               flags,
                              SNETPROGRAMDATAPTR  programdata,
                              SNETPLAYERDATAPTR   playerdata,
                              SNETUIDATAPTR       interfacedata,
                              SNETVERSIONDATAPTR  versiondata,
                              DWORD              *playerid) {
  return 0;
}

//===========================================================================
BOOL CALLBACK NullSend (DWORD        addresses,
                        SNETADDRPTR *addrlist,
                        LPVOID       data,
                        DWORD        databytes) {
  if (!(addresses && addrlist && data && databytes)) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }
  return 1;
}

//===========================================================================
BOOL CALLBACK NullSendExternalMessage (LPCSTR senderpath,
                                       LPCSTR sendername,
                                       LPCSTR targetpath,
                                       LPCSTR targetname,
                                       LPCSTR message) {
  return 0;
}

//===========================================================================
BOOL CALLBACK NullStartAdvertisingGame (LPCSTR  gamename,
                                        LPCSTR  gamepassword,
                                        LPCSTR  gamedescription,
                                        DWORD   gamemode,
                                        DWORD   gameage,
                                        DWORD   gamecategorybits,
                                        DWORD   optcategorybits,
                                        LPCVOID clientdata,
                                        DWORD   clientdatabytes) {
  if (!(gamename && gamedescription)) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }
  return 1;
}

//===========================================================================
BOOL CALLBACK NullStopAdvertisingGame () {
  return 1;
}

//===========================================================================
BOOL CALLBACK NullUnlockDeviceList (SNETSPI_DEVICELISTPTR devicelist) {
  return 1;
}

//===========================================================================
BOOL CALLBACK NullUnlockGameList (SNETSPI_GAMELISTPTR  gamelist,
                                  DWORD               *hintnextcall) {
  if (hintnextcall)
    *hintnextcall = 0;
  return 1;
}

/****************************************************************************
*
*  EXPORTED STRUCTURES
*
***/

DWORD    null_id   = 0;
LPCSTR   null_desc = "";
LPCSTR   null_req  = "";
SNETCAPS null_caps = {sizeof(SNETCAPS),     // size
#ifdef _DEBUG
                      SNET_CAPS_DEBUGONLY,
#else
                      SNET_CAPS_RETAILONLY,
#endif
                      MAXMESSAGESIZE,       // max message size
                      16,                   // max queue size,
                      1,                    // max players,
                      0x300000,             // bytes per second
                      0,                    // latency (ms)
                      30,                   // default turns per second
                      0};                   // default turns in transit
SNETSPI  null_spi  = {sizeof(SNETSPI),
                      NullCompareNetAddresses,
                      NullDestroy,
                      NullFree,
                      NullFreeExternalMessage,
                      NullGetGameInfo,
                      NullGetPerformanceData,
                      NullInitialize,
                      NullInitializeDevice,
                      NullLockDeviceList,
                      NullLockGameList,
                      NullReceive,
                      NullReceiveExternalMessage,
                      NullSelectGame,
                      NullSend,
                      NullSendExternalMessage,
                      NullStartAdvertisingGame,
                      NullStopAdvertisingGame,
                      NullUnlockDeviceList,
                      NullUnlockGameList};

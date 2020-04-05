/****************************************************************************
*
*  BATTLE.CPP
*  battle.net network provider
*
*  By Michael O'Brien (6/10/96)
*
***/

#include "pch.h"

HINSTANCE global_hinstance  = (HINSTANCE)0;
DWORD     global_maxplayers = MAXPLAYERS;
DWORD     global_programid  = 0;
DWORD     global_versionid  = 0;

/****************************************************************************
*
*  EXPORTED DATA AND FUNCTIONS
*
***/

DWORD    bn_id   = 'BNET';
LPCSTR   bn_desc = "Battle.net";
LPCSTR   bn_req  = "An active connection to an Internet provider, or a "
                   "direct connection to the Internet.";
SNETCAPS bn_caps = {sizeof(SNETCAPS),     // size
#ifdef _DEBUG
                    SNET_CAPS_DEBUGONLY,  // flags
#else
                    SNET_CAPS_RETAILONLY, // flags
#endif
                    MAXMESSAGESIZE,       // max message size
                    16,                   // max queue size,
                    MAXPLAYERS,           // max players,
                    1500,                 // bytes per second
                    500,                  // latency (ms)
                    4,                    // default turns per second
                    2};                   // default turns in transit
SNETSPI  bn_spi  = {sizeof(SNETSPI),
                    SpiCompareNetAddresses,
                    SpiDestroy,
                    SpiFree,
                    SpiFreeExternalMessage,
                    SpiGetGameInfo,
                    SpiGetPerformanceData,
                    SpiInitialize,
                    SpiInitializeDevice,
                    SpiLockDeviceList,
                    SpiLockGameList,
                    SpiReceive,
                    SpiReceiveExternalMessage,
                    SpiSelectGame,
                    SpiSend,
                    SpiSendExternalMessage,
                    SpiStartAdvertisingGame,
                    SpiStopAdvertisingGame,
                    SpiUnlockDeviceList,
                    SpiUnlockGameList,
                    SpiGetLocalPlayerName};

//===========================================================================
extern "C" BOOL APIENTRY SnpQuery (DWORD        index,
                                   DWORD       *id,
                                   LPCSTR      *description,
                                   LPCSTR      *requirements,
                                   SNETCAPSPTR *caps) {
  if ((index != 0) || !(id && description && requirements && caps))
    return 0;
  *id           = bn_id;
  *description  = bn_desc;
  *requirements = bn_req;
  *caps         = &bn_caps;
  return 1;
}

//===========================================================================
extern "C" BOOL APIENTRY SnpBind (DWORD       index,
                                  SNETSPIPTR *spi) {
  if ((index != 0) || !spi)
    return 0;
  *spi = &bn_spi;
  return 1;
}

//===========================================================================
extern "C" BOOL APIENTRY DllMain (HINSTANCE instance, DWORD reason, LPVOID) {

	switch (reason) {
		case DLL_PROCESS_ATTACH:	
            ComboRegisterClass();
			ScrollbarRegisterClass();
			break;

		case DLL_PROCESS_DETACH:
			ScrollbarUnregisterClass();
            ComboUnregisterClass();
			break;
	}

	global_hinstance = instance;
	return 1;
}

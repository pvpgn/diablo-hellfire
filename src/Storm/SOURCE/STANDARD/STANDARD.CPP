/****************************************************************************
*
*  STANDARD.CPP
*  Standard storm network providers
*
*  By Michael O'Brien (4/22/96)
*
***/

#include "pch.h"
#pragma  hdrstop

HINSTANCE global_instance = (HINSTANCE)0;

//===========================================================================
extern "C" BOOL APIENTRY SnpQuery (DWORD        index,
                                   DWORD       *id,
                                   LPCSTR      *description,
                                   LPCSTR      *requirements,
                                   SNETCAPSPTR *caps) {
  if (!(id && description && requirements && caps))
    return 0;
  switch (index) {

    case 0:
      *id           = ipx_id;
      *description  = ipx_desc;
      *requirements = ipx_req;
      *caps         = &ipx_caps;
      return 1;

    case 1:
      *id           = serial_id;
      *description  = serial_desc;
      *requirements = serial_req;
      *caps         = &serial_caps;
      return 1;

    case 2:
      *id           = null_id;
      *description  = null_desc;
      *requirements = null_req;
      *caps         = &null_caps;
      return 1;

    case 3:
      *id           = modem_id;
      *description  = modem_desc;
      *requirements = modem_req;
      *caps         = &modem_caps;
      return 1;

    default:
      return 0;

  }
}

//===========================================================================
extern "C" BOOL APIENTRY SnpBind (DWORD       index,
                                  SNETSPIPTR *spi) {
  if (!spi)
    return 0;

  switch (index) {

    case 0:
      *spi = &ipx_spi;
      return 1;

    case 1:
      *spi = &serial_spi;
      return 1;

    case 2:
      *spi = &null_spi;
      return 1;

    case 3:
      *spi = &modem_spi;
      return 1;

    default:
      return 0;

  }
}

//===========================================================================
extern "C" BOOL APIENTRY DllMain (HINSTANCE passinstance, DWORD reason, LPVOID) {
  if (reason == DLL_PROCESS_ATTACH)
    global_instance = passinstance;
  return 1;
}

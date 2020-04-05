/****************************************************************************
*
*  SREG.CPP
*  Storm registry functions
*
*  By Michael O'Brien (9/28/96)
*
***/

#include "pch.h"
#pragma  hdrstop

#define  BASEKEY       "Software\\Blizzard Entertainment\\"
#define  BATTLENETKEY  "Software\\Battle.net\\"

//===========================================================================
static BOOL InternalLoadEntry (LPCTSTR  keyname,
                               LPCTSTR  valuename,
                               DWORD    flags,
                               DWORD   *datatype,
                               LPVOID   buffer,
                               DWORD    bytes,
                               DWORD   *bytesread) {
  *bytesread = 0;
  BOOL success = 0;
  {
    char fullkeyname[MAX_PATH];
    SRegGetBaseKey(flags,fullkeyname,MAX_PATH);
    SStrPack(fullkeyname,keyname,MAX_PATH);
    HKEY keyhandle;
    if (!RegOpenKeyEx((flags & SREG_FLAG_USERSPECIFIC)
                        ? HKEY_CURRENT_USER
                        : HKEY_LOCAL_MACHINE,
                      fullkeyname,
                      0,
                      KEY_READ,
                      &keyhandle)) {
      *bytesread = bytes;
      success = !RegQueryValueEx(keyhandle,
                                 valuename,
                                 0,
                                 datatype,
                                 (LPBYTE)buffer,
                                 bytesread);
      RegCloseKey(keyhandle);
    }
  }
  return success;
}

//===========================================================================
static BOOL InternalSaveEntry (LPCTSTR  keyname,
                               LPCTSTR  valuename,
                               DWORD    flags,
                               DWORD    datatype,
                               LPCVOID  buffer,
                               DWORD    bytes) {
  BOOL success = 0;
  {
    char fullkeyname[MAX_PATH];
    if (flags & SREG_FLAG_BATTLENET)
      SStrCopy(fullkeyname,BATTLENETKEY,MAX_PATH);
    else
      SStrCopy(fullkeyname,BASEKEY,MAX_PATH);
    SStrPack(fullkeyname,keyname,MAX_PATH);
    HKEY  keyhandle;
    DWORD disposition;
    if (!RegCreateKeyEx((flags & SREG_FLAG_USERSPECIFIC)
                          ? HKEY_CURRENT_USER
                          : HKEY_LOCAL_MACHINE,
                        fullkeyname,
                        0,
                        NULL,
                        REG_OPTION_NON_VOLATILE,
                        KEY_WRITE,
                        (LPSECURITY_ATTRIBUTES)NULL,
                        &keyhandle,
                        &disposition)) {
      success = !RegSetValueEx(keyhandle,
                               valuename,
                               0,
                               datatype,
                               (const BYTE *)buffer,
                               bytes);
      if (flags & SREG_FLAG_FLUSHTODISK)
        RegFlushKey(keyhandle);
      RegCloseKey(keyhandle);
    }
  }
  return success;
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
BOOL APIENTRY SRegGetBaseKey (DWORD flags,
                              LPSTR buffer,
                              DWORD buffersize) {
  VALIDATEBEGIN;
  VALIDATE(buffer);
  VALIDATE(buffersize);
  VALIDATEEND;

  if (flags & SREG_FLAG_BATTLENET)
    SStrCopy(buffer,BATTLENETKEY,buffersize);
  else
    SStrCopy(buffer,BASEKEY,buffersize);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SRegLoadData (LPCTSTR  keyname,
                            LPCTSTR  valuename,
                            DWORD    flags,
                            LPVOID   buffer,
                            DWORD    buffersize,
                            DWORD   *bytesread) {
  VALIDATEBEGIN;
  VALIDATE(keyname);
  VALIDATE(*keyname);
  VALIDATE(valuename);
  VALIDATE(*valuename);
  VALIDATEEND;

  DWORD datatype;
  DWORD localbytesread;
  if (!bytesread)
    bytesread = &localbytesread;
  return InternalLoadEntry(keyname,
                           valuename,
                           flags,
                           &datatype,
                           buffer,
                           buffersize,
                           bytesread);
}

//===========================================================================
BOOL APIENTRY SRegLoadString (LPCTSTR keyname,
                              LPCTSTR valuename,
                              DWORD   flags,
                              LPTSTR  buffer,
                              DWORD   bufferchars) {
  VALIDATEBEGIN;
  VALIDATE(keyname);
  VALIDATE(*keyname);
  VALIDATE(valuename);
  VALIDATE(*valuename);
  VALIDATE(buffer);
  VALIDATE(bufferchars);
  VALIDATEEND;

  DWORD datatype;
  DWORD bytesread;
  if (!InternalLoadEntry(keyname,
                         valuename,
                         flags,
                         &datatype,
                         buffer,
                         bufferchars,
                         &bytesread))
    return FALSE;

  switch (datatype) {

    case REG_DWORD:
      {
        DWORD value = *(LPDWORD)buffer;
        wsprintf(buffer,"%u",value);
      }
      break;

    case REG_SZ:
      *(buffer+min(bufferchars-1,bytesread)) = 0;
      break;

  }

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SRegLoadValue (LPCTSTR  keyname,
                             LPCTSTR  valuename,
                             DWORD    flags,
                             DWORD   *value) {
  VALIDATEBEGIN;
  VALIDATE(keyname);
  VALIDATE(*keyname);
  VALIDATE(valuename);
  VALIDATE(*valuename);
  VALIDATE(value);
  VALIDATEEND;

  DWORD datatype;
  char  buffer[256] = "";
  DWORD bytesread;
  if (!InternalLoadEntry(keyname,
                         valuename,
                         flags,
                         &datatype,
                         buffer,
                         256,
                         &bytesread))
    return FALSE;

  switch (datatype) {

    case REG_DWORD:
      *value = *(LPDWORD)buffer;
      break;

    case REG_SZ:
      *value = strtoul(buffer,NULL,0);
      break;

  }

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SRegSaveData (LPCTSTR keyname,
                            LPCTSTR valuename,
                            DWORD   flags,
                            LPVOID  data,
                            DWORD   databytes) {
  VALIDATEBEGIN;
  VALIDATE(keyname);
  VALIDATE(*keyname);
  VALIDATE(valuename);
  VALIDATE(*valuename);
  VALIDATEEND;

  return InternalSaveEntry(keyname,
                           valuename,
                           flags,
                           (flags & SREG_FLAG_MULTISZ)
                             ? REG_MULTI_SZ
                             : REG_BINARY,
                           data ? data : "",
                           databytes);
}

//===========================================================================
BOOL APIENTRY SRegSaveString (LPCTSTR keyname,
                              LPCTSTR valuename,
                              DWORD   flags,
                              LPCTSTR string) {
  VALIDATEBEGIN;
  VALIDATE(keyname);
  VALIDATE(*keyname);
  VALIDATE(valuename);
  VALIDATE(*valuename);
  VALIDATE(string);
  VALIDATEEND;

  return InternalSaveEntry(keyname,
                           valuename,
                           flags,
                           REG_SZ,
                           string,
                           SStrLen(string)+1);
}

//===========================================================================
BOOL APIENTRY SRegSaveValue (LPCTSTR keyname,
                             LPCTSTR valuename,
                             DWORD   flags,
                             DWORD   value) {
  VALIDATEBEGIN;
  VALIDATE(keyname);
  VALIDATE(*keyname);
  VALIDATE(valuename);
  VALIDATE(*valuename);
  VALIDATEEND;

  return InternalSaveEntry(keyname,
                           valuename,
                           flags,
                           REG_DWORD,
                           &value,
                           sizeof(DWORD));
}

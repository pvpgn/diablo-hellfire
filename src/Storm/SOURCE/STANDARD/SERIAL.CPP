/****************************************************************************
*
*  SERIAL.CPP
*  Serial provider
*
*  By Michael O'Brien (8/29/96)
*
***/

#include "pch.h"
#pragma  hdrstop

#define  MAXMESSAGESIZE     512
#define  MAXPLAYERS         4
#define  PORTS              8
#define  PROVIDERID         'SCBL'
#define  READBUFFERSIZE     64
#define  STARTBYTE          0xFE

#define  STATE_NOGAME       0
#define  STATE_FOUNDGAME    1
#define  STATE_INGAME       2
#define  STATE_SHUTDOWN     3

#define  TYPE_USERDATA      0
#define  TYPE_SYSTEMDATA    1

#define  SYS_UNUSED         0
#define  SYS_QUERYID        1
#define  SYS_ASSERTID       2
#define  SYS_QUERYGAME      9
#define  SYS_GAMEINFO       10

typedef struct _ADVREC {
    DWORD networkid;
    DWORD programid;
    DWORD versionid;
    char  strings[SNETSPI_MAXSTRINGLENGTH*2];
    DWORD bytes;
} ADVREC, *ADVPTR;

typedef struct _PACKETHEADER {
    BYTE startbyte_type;  // must be first field
    BYTE sizedwords;      // must be second field
    BYTE targetmask;
    union {
      struct {
        BYTE fromid:4;
        BYTE timetolive:4;
      };
      struct {
        BYTE sysmsgtype:4;
        BYTE timetolive:4;
      };
    };
} PACKETHEADER, *PACKETHEADERPTR;

typedef struct _MESSAGEREC {
    SNETADDR      addr;    // must be first field
    PACKETHEADER  header;  // must immediately precede data
    BYTE          data[MAXMESSAGESIZE];
    DWORD         bytesneeded;
    DWORD         bytesread;
    DWORD         inport;
    _MESSAGEREC  *next;
} MESSAGEREC, *MESSAGEPTR;

typedef struct _MESSAGEDATAREC {
    PACKETHEADER header; // must immediately precede data
    BYTE         data[MAXMESSAGESIZE];
} MESSAGEDATAREC, *MESSAGEDATAPTR;

typedef struct _PORTREC {
    HANDLE     handle;
    OVERLAPPED overlapped;
    BYTE       readbuffer[READBUFFERSIZE];
    DWORD      bytesread;
    MESSAGEPTR partialmessage;
    BYTE       networkids;
} PORTREC, *PORTPTR;

static CCritSect           serial_critsect;
static HANDLE              serial_event[PORTS]    = {0};
static ADVPTR              serial_gameadvinfo     = NULL;
static SNETSPI_GAMELISTPTR serial_gamelist        = NULL;
static DWORD               serial_maxplayers      = MAXPLAYERS;
static MESSAGEPTR          serial_messagehead     = NULL;
static BYTE                serial_networkid       = 0;
static BOOL                serial_networkidlocked = 0;
static PORTPTR             serial_port[PORTS]     = {0};
static DWORD               serial_programid       = 0;
static HANDLE              serial_recvevent       = NULL;
static DWORD               serial_routeloopcheck  = 0;
static DWORD               serial_state           = STATE_NOGAME;
static HANDLE              serial_thread          = NULL;
static DWORD               serial_versionid       = 0;
static BOOL                serial_versionmismatch = 0;

static DWORD PickRandomNumber ();
static void SendFormedMessage (MESSAGEDATAPTR messageptr,
                               DWORD          exceptport,
                               BOOL           wait);
static BOOL SendMessage (BYTE   sysmsgtype,
                         BYTE   targetmask,
                         LPVOID data,
                         DWORD  databytes);
BOOL CALLBACK SerialStopAdvertisingGame ();

//===========================================================================
static BOOL FindNetworkId () {
  DWORD timeout = 16;
  while (timeout--) {

    // PICK A RANDOM NUMBER WHICH WE CAN USE TO RECOGNIZE AND DISCARD OUR
    // OWN PACKETS IN THE CASE OF A ROUTING LOOP.  MAKE SURE THE NUMBER IS
    // NOT ZERO, BECAUSE COMPUTERS WITH ESTABLISHED NETWORK IDS WILL USE ZERO.
    serial_routeloopcheck = 0;
    while (!serial_routeloopcheck)
      serial_routeloopcheck = PickRandomNumber();

    // PICK A NETWORK ID AT RANDOM, AND SEND OUT A QUERY TO SEE IF ANYONE
    // ELSE IS USING IT.
    serial_networkid = 0;
    while (!serial_networkid)
      serial_networkid = (BYTE)(PickRandomNumber() & 7);
    DWORD messagedata[2] = {serial_networkid,serial_routeloopcheck};
    SendMessage(SYS_QUERYID,0xFF,&messagedata[0],2*sizeof(DWORD));

    // WAIT 500 MILLISECONDS FOR A RESPONSE.  IF ANY OTHER COMPUTER ASSERTS
    // THAT IT IS USING THE ID WE PICKED, OR IF ANY OTHER COMPUTER QUERIES
    // FOR THE SAME ID, THEN GIVE UP THAT ID AND PICK A NEW ONE.
    DWORD starttime = GetTickCount();
    while (serial_networkid && (GetTickCount()-starttime < 500))
      Sleep(1);
    if (!serial_networkid)
      continue;

    // NO OTHER COMPUTER CONTESTED OUR NETWORK ID.  LOCK IT DOWN, ASSERT IT
    // ON THE NETWORK, AND RETURN SUCCESS.
    serial_networkidlocked = 1;
    serial_routeloopcheck  = 0;
    SendMessage(SYS_ASSERTID,0xFF,&serial_networkid,sizeof(DWORD));
    return 1;

  }
  return 0;
}

//===========================================================================
static BOOL InitializePort (DWORD port) {

  // ALLOCATE MEMORY FOR THE PORT RECORD
  if (!serial_port[port]) {
    serial_port[port] = NEW(PORTREC);
    if (serial_port[port])
      ZeroMemory(serial_port[port],sizeof(PORTREC));
    else
      return 0;
  }

  // CREATE AN EVENT FOR OVERLAPPED I/O
  if (!serial_event[port]) {
    serial_event[port] = CreateEvent(NULL,1,0,NULL);
    if (serial_event[port])
      serial_port[port]->overlapped.hEvent = serial_event[port];
    else
      return 0;
  }

  // OPEN THE COMMUNICATIONS PORT
  {
    TCHAR portname[16];
    wsprintf(portname,TEXT("\\\\.\\COM%u"),(port+1));
    serial_port[port]->handle = CreateFile(portname,
                                           GENERIC_READ | GENERIC_WRITE,
                                           FILE_SHARE_READ,
                                           NULL,
                                           OPEN_EXISTING,
                                           FILE_FLAG_OVERLAPPED,
                                           NULL);
  }

  // IF WE COULDN'T OPEN THE PORT THEN IT IS PROBABLY UNCONFIGURED OR IN
  // USE BY ANOTHER APPLICATION.  RETURN SUCCESS BECAUSE THIS ISN'T A FATAL
  // ERROR; WE WILL JUST NOT USE THIS PARTICULAR PORT.
  if (serial_port[port]->handle == INVALID_HANDLE_VALUE)
    return 1;

  // SET THE COMMUNICATIONS STATE
  // (WE DO THIS THE HARD WAY BECAUSE BUILDCOMMDCB() DOESN'T WORK RIGHT
  //  ON WINDOWS 95)
  {
    DCB dcb;
    ZeroMemory(&dcb,sizeof(DCB));
    dcb.DCBlength   = sizeof(DCB);
    dcb.BaudRate    = CBR_56000;
    SRegLoadValue("Network Providers\\Serial","Baud Rate",0,&dcb.BaudRate);
    dcb.fBinary     = 1;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    dcb.ByteSize    = 8;
    dcb.Parity      = NOPARITY;
    dcb.StopBits    = ONESTOPBIT;
    dcb.ErrorChar   = 63;
    dcb.EofChar     = 26;
    SetCommState(serial_port[port]->handle,&dcb);
  }

  // SET THE COMMUNICATIONS TIMEOUT VALUES
  {
    COMMTIMEOUTS timeouts;
    ZeroMemory(&timeouts,sizeof(COMMTIMEOUTS));
    timeouts.ReadIntervalTimeout = 5;
    SetCommTimeouts(serial_port[port]->handle,&timeouts);
  }

  // EXECUTE AN OVERLAPPED READ
  ReadFile(serial_port[port]->handle,
           serial_port[port]->readbuffer,
           READBUFFERSIZE,
           &serial_port[port]->bytesread,
           &serial_port[port]->overlapped);

  return 1;
}

//===========================================================================
static DWORD PickRandomNumber () {
  // RETURN A DWORD-SIZED RANDOM NUMBER.  IT IS IMPORTANT THAT WE DON'T
  // USE THE RUNTIME LIBRARY RANDOM GENERATOR, BECAUSE WE DON'T WANT TO
  // INTERFERE WITH THE APPLICATION'S RANDOM SEQUENCE.
  LARGE_INTEGER perfcount;
  POINT         pos;
  QueryPerformanceCounter(&perfcount);
  GetCursorPos(&pos);

  static DWORD seed = 0x100001;
  seed ^= perfcount.LowPart ^ GetTickCount() ^ pos.x ^ pos.y;
  seed  = (seed*0x7D+3) % 0x2AAAAB;
  DWORD rand1 = seed & 0xFFFF;
  seed  = (seed*0x7D+3) % 0x2AAAAB;
  DWORD rand2 = seed & 0xFFFF;
  return (rand1 << 16) | rand2;
}

//===========================================================================
static void ProcessIncomingMessage (DWORD port, MESSAGEPTR messageptr) {
  PerfIncrement(PERF_PKTRECV);
  PerfAdd(PERF_BYTESRECV,messageptr->bytesread);
  BOOL systemmessage = (messageptr->header.startbyte_type-STARTBYTE == TYPE_SYSTEMDATA);

  // VALIDATE THE PORT NUMBER
  if (port >= PORTS) {
    FREE(messageptr);
    return;
  }

  // IF THIS MESSAGE HAS A NON-ZERO TIME TO LIVE AND HAS ADDITIONAL
  // RECIPIENTS, SEND IT OUT ON ALL ACTIVE PORTS WHICH LEAD TO THE
  // OTHER TARGET RECIPIENTS, EXCEPT THE PORT THE MESSAGE CAME IN ON
  BYTE origtargetmask = messageptr->header.targetmask;
  if (messageptr->header.timetolive--) {
    if (serial_networkid && serial_networkidlocked)
      messageptr->header.targetmask &= ~(1 << serial_networkid);
    if (messageptr->header.targetmask)
      SendFormedMessage((MESSAGEDATAPTR)&messageptr->header,
                        port,
                        1);
  }

  // VERIFY THAT WE ARE A RECIPIENT OF THIS MESSAGE, AND NOT THE SENDER
  if (serial_networkid && serial_networkidlocked &&
      ((!(origtargetmask & (1 << serial_networkid))) ||
       ((!systemmessage) &&
        (messageptr->header.fromid == serial_networkid)))) {
    FREE(messageptr);
    return;
  }

  // PROCESS USER MESSAGES
  if (!systemmessage)
    if (serial_networkid && serial_networkidlocked) {
      *(LPBYTE)&messageptr->addr = messageptr->header.fromid;
      LISTADDPTREND(&serial_messagehead,messageptr);
      if (serial_recvevent)
        SetEvent(serial_recvevent);
    }
    else
      FREE(messageptr);

  // PROCESS SYSTEM MESSAGES
  else {
    switch (messageptr->header.sysmsgtype) {

      case SYS_QUERYID:
        if (messageptr->bytesread >= sizeof(PACKETHEADER)+2*sizeof(DWORD)) {
          DWORD networkid      = *(LPDWORD)&messageptr->data[0];
          DWORD routeloopcheck = *(LPDWORD)&messageptr->data[sizeof(DWORD)];
          if (routeloopcheck != serial_routeloopcheck)
            if (serial_networkidlocked)
              SendMessage(SYS_ASSERTID,0xFF,&serial_networkid,sizeof(DWORD));
            else if (networkid == serial_networkid)
              serial_networkid = 0;
        }
        break;

      case SYS_ASSERTID:
        if (messageptr->bytesread >= sizeof(PACKETHEADER)+sizeof(DWORD)) {
          DWORD networkid = *(LPDWORD)&messageptr->data[0];
          serial_port[port]->networkids |= (1 << networkid);
          if ((!serial_networkidlocked) && (networkid == serial_networkid))
            serial_networkid = 0;
        }
        break;

      case SYS_QUERYGAME:
        if (serial_gameadvinfo)
          SendMessage(SYS_GAMEINFO,0xFF,serial_gameadvinfo,serial_gameadvinfo->bytes);
        break;

      case SYS_GAMEINFO:
        {
          ADVPTR advinfo = (ADVPTR)&messageptr->data[0];
          if ((advinfo->programid == serial_programid) &&
              (advinfo->versionid == serial_versionid)) {
            if (!serial_gamelist) {
              serial_gamelist = NEW(SNETSPI_GAMELIST);
              ZeroMemory(serial_gamelist,sizeof(SNETSPI_GAMELIST));
            }
            if (serial_gamelist) {
              *(LPBYTE)&serial_gamelist->owner = (BYTE)advinfo->networkid;
              serial_gamelist->gameid          = 1;
              strcpy(serial_gamelist->gamename,advinfo->strings);
              strcpy(serial_gamelist->gamedescription,advinfo->strings+strlen(advinfo->strings)+1);
            }
          }
          else
            serial_versionmismatch = 1;
        }
        break;

    }
    FREE(messageptr);
  }

}

//===========================================================================
static void SendFormedMessage (MESSAGEDATAPTR messageptr,
                               DWORD          exceptport,
                               BOOL           wait) {
  BOOL systemmessage = (messageptr->header.startbyte_type == STARTBYTE+TYPE_SYSTEMDATA);

  // IF THIS IS A SYSTEM MESSAGE, WRITE IT OUT ON ALL ACTIVE PORTS.
  // OTHERWISE, WRITE IT OUT ON ALL PORTS WHICH LEAD TO ANY OF THE
  // DESIRED NETWORK IDS.
  HANDLE     event[PORTS];
  OVERLAPPED overlapped[PORTS];
  DWORD      byteswritten[PORTS];
  DWORD      numsends = 0;
  {
    for (DWORD port = 0; port < PORTS; ++port)
      if ((port != exceptport) &&
          (serial_port[port]->handle != INVALID_HANDLE_VALUE) &&
          (systemmessage ||
           (serial_port[port]->networkids & messageptr->header.targetmask))) {
         ZeroMemory(&overlapped[numsends],sizeof(OVERLAPPED));
         if (wait)
           event[numsends] = CreateEvent(NULL,0,0,NULL);
         overlapped[numsends].hEvent = event[numsends];
         WriteFile(serial_port[port]->handle,
                   messageptr,
                   messageptr->header.sizedwords*sizeof(DWORD),
                   &byteswritten[numsends],
                   &overlapped[numsends]);
         ++numsends;
         PerfIncrement(PERF_PKTSENT);
         PerfAdd(PERF_BYTESSENT,messageptr->header.sizedwords*sizeof(DWORD));
      }
  }

  // WAIT FOR ALL WRITES TO COMPLETE
  if (numsends && wait) {
    WaitForMultipleObjects(numsends,&event[0],1,INFINITE);
    while (numsends--)
      CloseHandle(event[numsends]);
  }

}

//===========================================================================
static BOOL SendMessage (BYTE   sysmsgtype,
                         BYTE   targetmask,
                         LPVOID data,
                         DWORD  databytes) {
  BOOL systemmessage = (sysmsgtype != SYS_UNUSED);
  if (systemmessage)
    targetmask = 0xFF;

  // REFUSE TO SEND A NON-SYSTEM MESSAGE IF WE DON'T HAVE A VALID NETWORK ID
  if ((sysmsgtype == SYS_UNUSED) && (!(serial_networkid && serial_networkidlocked)))
    return 0;

  // CREATE A FULLY-FORMED MESSAGE
  MESSAGEDATAREC messagedata;
  ZeroMemory(&messagedata,sizeof(MESSAGEDATAREC));
  messagedata.header.startbyte_type = STARTBYTE+((sysmsgtype == SYS_UNUSED) ? TYPE_USERDATA : TYPE_SYSTEMDATA);
  messagedata.header.sizedwords     = (sizeof(PACKETHEADER)+databytes+sizeof(DWORD)-1)/sizeof(DWORD);
  messagedata.header.timetolive     = MAXPLAYERS;
  messagedata.header.targetmask     = targetmask;
  if (systemmessage)
    messagedata.header.sysmsgtype = sysmsgtype;
  else
    messagedata.header.fromid     = serial_networkid;
  if (data && databytes)
    CopyMemory(&messagedata.data[0],data,databytes);

  // SEND IT
  SendFormedMessage(&messagedata,0xFFFFFFFF,1);

  return 1;
}

//===========================================================================
static unsigned CALLBACK ThreadProc (LPVOID) {
  while (serial_state != STATE_SHUTDOWN) {
    serial_critsect.Enter();
    for (DWORD port = 0; port < PORTS; ++port)
      if (serial_port[port]->handle != INVALID_HANDLE_VALUE) {

        // CHECK TO SEE IF AN I/O OPERATION HAS COMPLETED ON THIS PORT
        if (GetOverlappedResult(serial_port[port]->handle,
                                &serial_port[port]->overlapped,
                                &serial_port[port]->bytesread,
                                0)) {

          // IF SO, PARSE THE INCOMING DATA
          DWORD  bytesleft = serial_port[port]->bytesread;
          LPBYTE dataptr   = &serial_port[port]->readbuffer[0];
          while (bytesleft) {

            // IF WE HAVE A PARTIAL MESSAGE WAITING, TRY TO COMPLETE THE
            // MESSAGE
            if (serial_port[port]->partialmessage) {
              if (!serial_port[port]->partialmessage->bytesneeded)
                serial_port[port]->partialmessage->bytesneeded
                  = min(sizeof(PACKETHEADER)+MAXMESSAGESIZE,
                        (*dataptr)*sizeof(DWORD));
              LPBYTE writeptr    = (LPBYTE)&serial_port[port]->partialmessage->header;
              DWORD  bytestocopy = min(bytesleft,
                                       serial_port[port]->partialmessage->bytesneeded
                                         -serial_port[port]->partialmessage->bytesread);
              CopyMemory(writeptr+serial_port[port]->partialmessage->bytesread,
                         dataptr,
                         bytestocopy);
              serial_port[port]->partialmessage->bytesread += bytestocopy;
              dataptr                                      += bytestocopy;
              bytesleft                                    -= bytestocopy;
              if (serial_port[port]->partialmessage->bytesread >= serial_port[port]->partialmessage->bytesneeded) {
                ProcessIncomingMessage(port,serial_port[port]->partialmessage);
                serial_port[port]->partialmessage = NULL;
              }
            }

            // SCAN TO THE BEGINNING OF THE NEXT MESSAGE
            while (bytesleft && (*dataptr < STARTBYTE)) {
              ++dataptr;
              --bytesleft;
            }

            // IF A MESSAGE IS AVAILABLE, CREATE A NEW MESSAGE RECORD TO
            // HOLD IT
            if (bytesleft) {
              MESSAGEPTR messageptr = NEW(MESSAGEREC);
              if (messageptr) {
                ZeroMemory(messageptr,sizeof(MESSAGEREC));
                messageptr->inport      = port;
                messageptr->bytesneeded = (bytesleft >= 2) ? (*(dataptr+1))*sizeof(DWORD)
                                                           : 0;
                if (messageptr->bytesneeded > sizeof(PACKETHEADER)+MAXMESSAGESIZE)
                  messageptr->bytesneeded = sizeof(PACKETHEADER)+MAXMESSAGESIZE;
                DWORD  bytestocopy = messageptr->bytesneeded ? min(bytesleft,messageptr->bytesneeded)
                                                             : 1;
                LPBYTE writeptr    = (LPBYTE)&messageptr->header;
                CopyMemory(writeptr,dataptr,bytestocopy);
                dataptr   += bytestocopy;
                bytesleft -= bytestocopy;
                messageptr->bytesread = bytestocopy;
                if (messageptr->bytesneeded &&
                    (messageptr->bytesread >= messageptr->bytesneeded))
                  ProcessIncomingMessage(port,messageptr);
                else
                  serial_port[port]->partialmessage = messageptr;
              }
              else
                bytesleft = 0;
            }

          }

          // RETURN THIS PORT'S EVENT TO A NONSIGNALED STATE
          ResetEvent(serial_event[port]);

          // POST ANOTHER OVERLAPPED READ FOR THIS PORT
          ReadFile(serial_port[port]->handle,
                   serial_port[port]->readbuffer,
                   READBUFFERSIZE,
                   &serial_port[port]->bytesread,
                   &serial_port[port]->overlapped);

        }

      }
    serial_critsect.Leave();
    WaitForMultipleObjects(PORTS,&serial_event[0],0,INFINITE);
  }
  serial_state = STATE_NOGAME;
  _endthreadex(0);
  return 0;
}

/****************************************************************************
*
*  SERVICE PROVIDER INTERFACE FUNCTIONS
*
***/

//===========================================================================
BOOL CALLBACK SerialCompareNetAddresses (SNETADDRPTR  addr1,
                                         SNETADDRPTR  addr2,
                                         DWORD       *diffmagnitude) {
  if (diffmagnitude)
    *diffmagnitude = 0;
  if (!(addr1 && addr2)) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }
  *diffmagnitude = (memcmp(addr1,addr2,sizeof(SNETADDR)) != 0);
  return 1;
}

//===========================================================================
BOOL CALLBACK SerialDestroy () {

  // TERMINATE THE READ THREAD
  if (serial_thread) {
    serial_state = STATE_SHUTDOWN;
    for (DWORD port = 0; port < PORTS; ++port)
      if (serial_port[port]->handle != INVALID_HANDLE_VALUE)
        SetEvent(serial_port[port]->overlapped.hEvent);
    WaitForSingleObject(serial_thread,INFINITE);
    serial_thread = NULL;
  }

  // WRITE A FINAL CR/LF TO EVERY PORT WE OPENED, TO PUT ANY MODEMS WE MAY
  // HAVE SENT GARBAGE TO DURING THE AUTO-DETECTION PROCESS BACK INTO A
  // KNOWN STATE
  {
    HANDLE     event[PORTS];
    OVERLAPPED overlapped[PORTS];
    DWORD      byteswritten[PORTS];
    DWORD      numsends = 0;
    {
      for (DWORD port = 0; port < PORTS; ++port)
        if (serial_port[port]->handle != INVALID_HANDLE_VALUE) {
           ZeroMemory(&overlapped[numsends],sizeof(OVERLAPPED));
           event[numsends]             = CreateEvent(NULL,0,0,NULL);
           overlapped[numsends].hEvent = event[numsends];
           WriteFile(serial_port[port]->handle,
                     "\n",
                     2,
                     &byteswritten[numsends],
                     &overlapped[numsends]);
           ++numsends;
         PerfIncrement(PERF_PKTSENT);
         PerfAdd(PERF_BYTESSENT,2);
        }
    }
    if (numsends) {
      WaitForMultipleObjects(numsends,&event[0],1,INFINITE);
      while (numsends--)
        CloseHandle(event[numsends]);
    }
  }

  // CLOSE AND FREE ALL PORTS
  for (DWORD port = 0; port < PORTS; ++port) {
    CloseHandle(serial_event[port]);
    if (serial_port[port]->handle != INVALID_HANDLE_VALUE)
      CloseHandle(serial_port[port]->handle);
    if (serial_port[port]->partialmessage)
      FREE(serial_port[port]->partialmessage);
    FREE(serial_port[port]);
    serial_port[port] = NULL;
  }

  // STOP ADVERTISING GAMES
  SerialStopAdvertisingGame();

  // FREE ALL MESSAGES
  LISTCLEAR(&serial_messagehead);

  // FREE THE GAME LIST
  if (serial_gamelist) {
    FREE(serial_gamelist);
    serial_gamelist = NULL;
  }

  // RESET OUR NETWORK ID
  serial_networkid       = 0;
  serial_networkidlocked = 0;

  return 1;
}

//===========================================================================
BOOL CALLBACK SerialFree (SNETADDRPTR addr,
                          LPVOID      data,
                          DWORD       databytes) {
  if (!(addr && data)) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }
  FREE(addr);
  return 1;
}

//===========================================================================
BOOL CALLBACK SerialFreeExternalMessage (LPCSTR senderpath,
                                         LPCSTR sendername,
                                         LPCSTR mesage) {
  SetLastError(SNET_ERROR_INVALID_PARAMETER);
  return 0;
}

//===========================================================================
BOOL CALLBACK SerialGetGameInfo (DWORD             gameid,
                                 LPCSTR            gamename,
                                 LPCSTR            gamepassword,
                                 SNETSPI_GAMELIST *gameinfo) {
  if (gameinfo)
    ZeroMemory(gameinfo,sizeof(SNETSPI_GAMELIST));
  if (!(gamename && gameinfo && (gameid || *gamename))) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }

  // IF THE GAME IN THE GAME LIST MATCHES THE QUERY PARAMETERS, RETURN
  // ITS INFORMATION
  serial_critsect.Enter();
  if (serial_gamelist &&
      ((!gameid) || (gameid == serial_gamelist->gameid)) &&
      ((!*gamename) || !_stricmp(gamename,serial_gamelist->gamename)))
    CopyMemory(gameinfo,serial_gamelist,sizeof(SNETSPI_GAMELIST));
  serial_critsect.Leave();

  if (gameinfo->gameid)
    return 1;
  else {
    SetLastError(SNET_ERROR_GAME_NOT_FOUND);
    return 0;
  }
}

//===========================================================================
BOOL CALLBACK SerialGetPerformanceData (DWORD          counterid,
                                        DWORD         *countervalue,
                                        LARGE_INTEGER *measurementtime,
                                        LARGE_INTEGER *measurementfreq) {
  return PerfGetPerformanceData(counterid,
                                countervalue,
                                measurementtime,
                                measurementfreq);
}

//===========================================================================
BOOL CALLBACK SerialInitialize (SNETPROGRAMDATAPTR programdata,
                                SNETPLAYERDATAPTR  playerdata,
                                SNETUIDATAPTR      interfacedata,
                                SNETVERSIONDATAPTR versiondata,
                                HANDLE             event) {

  // SAVE THE PROGRAM AND VERSION IDS AND THE RECEIVE EVENT HANDLE
  serial_programid  = programdata->programid;
  serial_versionid  = programdata->versionid;
  serial_maxplayers = min(programdata->maxplayers,MAXPLAYERS);
  serial_recvevent  = event;

  // RESET PERFORMANCE DATA
  PerfReset();

  // INITIALIZE THE SERIAL PORTS
  {
    for (DWORD loop = 0; loop < PORTS; ++loop)
      if (!InitializePort(loop)) {
        SetLastError(SNET_ERROR_NOT_ENOUGH_MEMORY);
        return 0;
      }
  }

  // CREATE A THREAD TO PROCESS INCOMING PACKETS
  if (!serial_thread) {
    unsigned threadid;
    serial_thread = (HANDLE)_beginthreadex(NULL,
                                           0,
                                           ThreadProc,
                                           NULL,
                                           0,
                                           &threadid);
    if (serial_thread)
      SetThreadPriority(serial_thread,THREAD_PRIORITY_HIGHEST);
    else {
      SerialDestroy();
      SetLastError(SNET_ERROR_MAX_THRDS_REACHED);
      return 0;
    }
  }

  // FIND AN UNUSED NETWORK ID
  if (!FindNetworkId()) {
    SetLastError(SNET_ERROR_TOO_MANY_NAMES);
    return 0;
  }

  // SEND OUT A QUERY FOR ACTIVE GAMES
  SendMessage(SYS_QUERYGAME,0xFF,NULL,0);

  return 1;
}

//===========================================================================
BOOL CALLBACK SerialInitializeDevice (DWORD              deviceid,
                                      SNETPROGRAMDATAPTR programdata,
                                      SNETPLAYERDATAPTR  playerdata,
                                      SNETUIDATAPTR      interfacedata,
                                      SNETVERSIONDATAPTR versiondata) {

  // WE NEVER RETURN ANY DEVICES, SO THIS FUNCTION SHOULD NEVER BE CALLED
  return 0;
}

//===========================================================================
BOOL CALLBACK SerialLockDeviceList (SNETSPI_DEVICELISTPTR *devicelist) {
  if (!devicelist) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }
  *devicelist = NULL;
  return 1;
}

//===========================================================================
BOOL CALLBACK SerialLockGameList (DWORD                categorybits,
                                  DWORD                categorymask,
                                  SNETSPI_GAMELISTPTR *gamelist) {
  if (!gamelist) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }
  if (serial_versionmismatch) {
    SetLastError(SNET_ERROR_VERSION_MISMATCH);
    return 0;
  }
  serial_critsect.Enter();
  *gamelist = serial_gamelist;
  return 1;
}

//===========================================================================
/*
BOOL CALLBACK SerialReceive (SNETADDRPTR *addr,
                             LPVOID      *data,
                             DWORD       *databytes) {
*/
BOOL CALLBACK SerialReceive (LPVOID      *data,
                             DWORD       *databytes,
                             SNETADDRPTR *addr) {
  if (addr)
    *addr = NULL;
  if (data)
    *data = NULL;
  if (databytes)
    *databytes = NULL;
  if (!(addr && data && databytes)) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }

  if (serial_messagehead) {
    serial_critsect.Enter();
    *addr      = &serial_messagehead->addr;
    *data      = &serial_messagehead->data[0];
    *databytes = serial_messagehead->bytesread-sizeof(PACKETHEADER);
    LISTFREEPTR(&serial_messagehead,serial_messagehead);
    serial_critsect.Leave();
    return 1;
  }
  else {
    SetLastError(SNET_ERROR_NO_MESSAGES_WAITING);
    return 0;
  }
}

//===========================================================================
BOOL CALLBACK SerialReceiveExternalMessage (LPCSTR *senderpath,
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
BOOL CALLBACK SerialSelectGame (DWORD               flags,
                                SNETPROGRAMDATAPTR  programdata,
                                SNETPLAYERDATAPTR   playerdata,
                                SNETUIDATAPTR       interfacedata,
                                SNETVERSIONDATAPTR  versiondata,
                                DWORD              *playerid) {

  // IF WE DON'T YET KNOW OF ANY GAMES, WAIT A REASONABLE AMOUNT OF TIME
  // FOR ADVERTISEMENTS TO COME IN
  if (!serial_gamelist)
    Sleep(500);

  // GET THE GAME INFORMATION FROM THE FIRST (AND ONLY) GAME IN THE
  // GAME LIST
  SNETSPI_GAMELIST gameinfo;
  serial_critsect.Enter();
  if (serial_gamelist)
    CopyMemory(&gameinfo,serial_gamelist,sizeof(SNETSPI_GAMELIST));
  else
    ZeroMemory(&gameinfo,sizeof(SNETSPI_GAMELIST));
  serial_critsect.Leave();

  // IF THERE IS A COMPATIBLE GAME IN THE GAME LIST, JOIN IT
  if (serial_gamelist)
    return SNetJoinGame(gameinfo.gameid,
                        gameinfo.gamename,
                        NULL,
                        playerdata->playername,
                        playerdata->playerdescription,
                        playerid);

  // OTHERWISE, CALL THE CREATE GAME CALLBACK TO CREATE A NEW ONE
  else if (interfacedata && interfacedata->createcallback) {
    SNETCREATEDATA createdata;
    ZeroMemory(&createdata,sizeof(SNETCREATEDATA));
    createdata.size        = sizeof(SNETCREATEDATA);
    createdata.providerid  = PROVIDERID;
    createdata.maxplayers  = serial_maxplayers;
    createdata.createflags = 0;
    return interfacedata->createcallback(&createdata,
                                         programdata,
                                         playerdata,
                                         interfacedata,
                                         versiondata,
                                         playerid);
  }
  else
    return 0;

}

//===========================================================================
BOOL CALLBACK SerialSend (DWORD        addresses,
                          SNETADDRPTR *addrlist,
                          LPVOID       data,
                          DWORD        databytes) {
  if (!(addresses && addrlist && data && databytes)) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }

  // BUILD A TARGET MASK OUT OF THE LIST OF ADDRESSES
  BYTE targetmask = 0;
  while (addresses--)
    targetmask |= (1 << *(LPBYTE)*(addrlist+addresses));

  // SEND THE MESSAGE
  return SendMessage(SYS_UNUSED,targetmask,data,databytes);
}

//===========================================================================
BOOL CALLBACK SerialSendExternalMessage (LPCSTR senderpath,
                                         LPCSTR sendername,
                                         LPCSTR targetpath,
                                         LPCSTR targetname,
                                         LPCSTR message) {
  return 0;
}

//===========================================================================
BOOL CALLBACK SerialStartAdvertisingGame (LPCSTR  gamename,
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

  // STOP ADVERTISING ANY GAME WE ARE CURRENTLY ADVERTISING
  if (serial_gameadvinfo)
    SerialStopAdvertisingGame();

  // REFUSE TO ADVERTISE A NEW GAME IF ANY OTHER GAME IS BEING
  // ADVERTISED ON THIS SERIAL NETWORK
  if (serial_gamelist)
    if (gamemode & SNET_GM_ADVERTISED) {
      FREE(serial_gamelist);
      serial_gamelist = NULL;
    }
    else {
      SetLastError(SNET_ERROR_TOO_MANY_NAMES);
      return 0;
    }

  // CREATE RECORDS TO ADVERTISE THE GAME
  serial_gameadvinfo = NEW(ADVREC);
  serial_gamelist    = NEW(SNETSPI_GAMELIST);
  if (serial_gameadvinfo && serial_gamelist) {
    ZeroMemory(serial_gameadvinfo,sizeof(ADVREC));
    ZeroMemory(serial_gamelist   ,sizeof(SNETSPI_GAMELIST));
  }
  else {
    if (serial_gameadvinfo) {
      FREE(serial_gameadvinfo);
      serial_gameadvinfo = NULL;
    }
    if (serial_gamelist) {
      FREE(serial_gamelist);
      serial_gamelist = NULL;
    }
    SetLastError(SNET_ERROR_NOT_ENOUGH_MEMORY);
    return 0;
  }

  // FILL IN THE ADVERTISING INFORMATION
  serial_critsect.Enter();
  serial_gameadvinfo->networkid = serial_networkid;
  serial_gameadvinfo->programid = serial_programid;
  serial_gameadvinfo->versionid = serial_versionid;
  strcpy(serial_gameadvinfo->strings,gamename);
  strcpy(serial_gameadvinfo->strings+strlen(gamename)+1,gamedescription);
  serial_gameadvinfo->bytes = 2*sizeof(DWORD)+strlen(gamename)+strlen(gamedescription)+2;

  // FILL IN THE GAME LIST
  serial_gamelist->gameid = 1;
  *(LPBYTE)&serial_gamelist->owner = serial_networkid;
  strcpy(serial_gamelist->gamename,gamename);
  strcpy(serial_gamelist->gamedescription,gamedescription);
  serial_critsect.Leave();

  // SEND OUT THE GAME INFORMATION
  SendMessage(SYS_GAMEINFO,0xFF,serial_gameadvinfo,serial_gameadvinfo->bytes);

  return 1;
}

//===========================================================================
BOOL CALLBACK SerialStopAdvertisingGame () {
  if (!serial_gameadvinfo) {
    SetLastError(SNET_ERROR_NOT_OWNER);
    return 0;
  }

  // STOP ADVERTISING THE GAME
  serial_critsect.Enter();
  if (serial_gameadvinfo) {
    FREE(serial_gameadvinfo);
    serial_gameadvinfo = NULL;
  }
  if (serial_gamelist) {
    FREE(serial_gamelist);
    serial_gamelist = NULL;
  }
  serial_critsect.Leave();

  // SEND OUT A NEW QUERY
  Sleep(500);
  SendMessage(SYS_QUERYGAME,0xFF,NULL,0);

  return 1;
}

//===========================================================================
BOOL CALLBACK SerialUnlockDeviceList (SNETSPI_DEVICELISTPTR devicelist) {
  return 1;
}

//===========================================================================
BOOL CALLBACK SerialUnlockGameList (SNETSPI_GAMELISTPTR  gamelist,
                                    DWORD               *hintnextcall) {
  serial_critsect.Leave();
  if (hintnextcall)
    *hintnextcall = gamelist ? 0 : 500;
  return 1;
}

/****************************************************************************
*
*  EXPORTED STRUCTURES
*
***/

DWORD    serial_id   = PROVIDERID;
LPCSTR   serial_desc = "Direct Cable Connection";
LPCSTR   serial_req  = "Two or more computers connected together with serial cables and null-modems.";
SNETCAPS serial_caps = {sizeof(SNETCAPS),     // size
#ifdef _DEBUG
                        SNET_CAPS_DEBUGONLY,
#else
                        SNET_CAPS_RETAILONLY,
#endif
                        MAXMESSAGESIZE,       // max message size
                        16,                   // max queue size,
                        MAXPLAYERS,           // max players,
                        1500,                 // bytes per second
                        500,                  // latency (ms)
                        4,                    // default turns per second
                        2};                   // default turns in transit
SNETSPI  serial_spi  = {sizeof(SNETSPI),
                        SerialCompareNetAddresses,
                        SerialDestroy,
                        SerialFree,
                        SerialFreeExternalMessage,
                        SerialGetGameInfo,
                        SerialGetPerformanceData,
                        SerialInitialize,
                        SerialInitializeDevice,
                        SerialLockDeviceList,
                        SerialLockGameList,
                        SerialReceive,
                        SerialReceiveExternalMessage,
                        SerialSelectGame,
                        SerialSend,
                        SerialSendExternalMessage,
                        SerialStartAdvertisingGame,
                        SerialStopAdvertisingGame,
                        SerialUnlockDeviceList,
                        SerialUnlockGameList};

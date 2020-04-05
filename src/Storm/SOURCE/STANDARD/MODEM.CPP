/****************************************************************************
*
*  MODEM.CPP
*  Modem provider
*
*  By Jeff Strain (11/21/96)
*
*  File History:
*  11/28/96  1.00 (js) Initial version
*  02/03/97  1.10 (js) Fixes for shitty non-error correcting modems
*                      and noisy lines
*                 (js) Now returning unique busy status
*                 (js) Full packet signatures
*                 (js) Pulse dialing
*
***/
	
#include "pch.h"

// This is just so that functions will show up in the globals list in
// MSDEV Studio.
#define STATIC static

// Prevents compilation of internal UI code
#define PROVIDERUI

// CATCHES INTERNAL TEST VERSIONS
// EXPIRES MARCH 01, 1997
// #define EXPIRATION {1997, 3, 0, 1} // YEAR, MONTH, NULL, DAY

// Need this when compiling under NT4 - otherwise does not generate
// a Win95 compatible TAPI version, This *must* be included before
// tapi.h!
#define TAPI_CURRENT_VERSION 0x00010004
#include <tapi.h>

#include <tchar.h>

// MAX TIME (MS) BETWEEN CLIENT AND HOST CONNECTION NOTIFICATIONS
#define  MAX_CONNECT_DIFFERENTIAL  10000

#define  HANDSHAKE_TIMEOUT         5000
#define  ANSWERTIMEOUT             INFINITE

// Time in ms to wait for game info after connection
#define  FINDGAMETIMEOUT           MAX_CONNECT_DIFFERENTIAL + HANDSHAKE_TIMEOUT

#define  READTIMEOUT               25 // in ms

#define  MAXMESSAGESIZE     180  // BE CAREFUL NOT TO FALL BELOW THE APP MIN
#define  MAXPLAYERS         2
#define  PORTS              1
#define  PROVIDERID         'MODM'
#define  READBUFFERSIZE     64
#define  HEADER_KEY_SEQ     {'N','R','M','L'}
#define  HEADER_KEYS        4

#define  STATE_NOGAME       0
#define  STATE_SHUTDOWN     1

#define  TAPISTATE_SHUTDOWN 0
#define  TAPISTATE_ACTIVE   1

#define  TYPE_USERDATA      0
#define  TYPE_SYSTEMDATA    1
#define  TYPE_LASTDATA      1

#define  SYS_UNUSED         0
#define  SYS_QUERYID        1
#define  SYS_ASSERTID       2
#define  SYS_QUERYGAME      9
#define  SYS_GAMEINFO       10
#define  SYS_REMOVE         11

// SYSTEM MESSAGE SYMBOLS FOR 2-WAY HANDSHAKE. THIS IS NECCESSARY
// SINCE THERE CAN BE A DIFFERENTIAL IN CONNECTION NOTIFICATION
// BETWEEN CALLER AND HOST OF UP TO 5 SECONDS AND THE QUERY
// GAME CAN BE LOST. NOTE THAT A 3-WAY HANDSHAKE IS NOT NECCESSARY
// SINCE WE DO NOT NEED TO TEST THE LINE, JUST THE STARTUP OF THE
// REMOTE SYSEM.
#define  SYS_QUERYLINE       3
#define  SYS_ASSERTLINE      4

// Possible return error for resynchronization functions.
#define WAITERR_WAITABORTED  1
#define WAITERR_WAITTIMEDOUT 2

// LOCAL DATA TYPES

typedef struct _UIPARAMS {
  DWORD              flags;
  SNETPROGRAMDATAPTR programdata;
  SNETPLAYERDATAPTR  playerdata;
  SNETUIDATAPTR      interfacedata;
  SNETVERSIONDATAPTR versiondata;
  LPDWORD            playeridptr;
} UIPARAMS, *UIPARAMSPTR;

typedef struct _ADVREC {
    DWORD networkid;
    DWORD programid;
    DWORD versionid;
    char  strings[SNETSPI_MAXSTRINGLENGTH*2];
    DWORD bytes;
} ADVREC, *ADVPTR;

typedef struct _PACKETHEADER {
    BYTE key_sequence[HEADER_KEYS];    // must be first field
    BYTE sizedwords;                   // must be second field
    BYTE startbyte_type;
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

typedef BOOL (CALLBACK *DEVENUMPROC)(LPLINEDEVCAPS, DWORD, LPARAM);
typedef BOOL (CALLBACK *LOCENUMPROC)(LPLINETRANSLATECAPS,
                                     LPLINELOCATIONENTRY, LPARAM);
typedef BOOL (CALLBACK *COUNTRYENUMPROC)(LPLINECOUNTRYLIST,
                                         LPLINECOUNTRYENTRY, LPARAM);

// GLOBAL STATIC VARIABLES

static BYTE gs_arrHeaderKeys[HEADER_KEYS]           = HEADER_KEY_SEQ;
#ifdef EXPIRATION
static SYSTEMTIME          sg_expDate               = EXPIRATION;
#endif
static CCritSect           modem_critsect;
static HANDLE              modem_event[PORTS]       = {0};
static ADVPTR              modem_gameadvinfo        = NULL;
static SNETSPI_GAMELISTPTR modem_gamelist           = NULL;
static DWORD               modem_maxplayers         = MAXPLAYERS;
static MESSAGEPTR          modem_messagehead        = NULL;
static BOOL                modem_lineconfirmed      = 0;
static BOOL                modem_lineestablished    = 0;
static BYTE                modem_networkid          = 0;
static BOOL                modem_networkidlocked    = 0;
static PORTPTR             modem_port[PORTS]        = {0};
static DWORD               modem_programid          = 0;
static HANDLE              modem_recvevent          = NULL;
static HANDLE              modem_TAPIHangupEvent    = NULL;
static HANDLE              modem_TAPIHangupNotify   = NULL;
static HANDLE              modem_TAPIAnswerEvent    = NULL;
static HANDLE              modem_TAPIEvent          = NULL;
static HANDLE              modem_TAPINotifyEvent    = NULL;
static HANDLE              modem_TAPICallStateEvent = NULL;
static HANDLE              modem_TAPITerminateEvent = NULL;
static HANDLE              modem_TAPILineReplyEvent = NULL;
static DWORD               modem_routeloopcheck     = 0;
static DWORD               modem_state              = STATE_NOGAME;
static DWORD               modem_tapistate          = TAPISTATE_SHUTDOWN;
static HANDLE              modem_readthread         = NULL;
static HANDLE              modem_tapithread         = NULL;
static HANDLE              modem_tapianswerthread   = NULL;
static HANDLE              modem_tapihangupthread   = NULL;
static DWORD               modem_versionid          = 0;
static BOOL                modem_versionmismatch    = 0;
static DWORD               modem_callstatus         = NO_ERROR;
static SNETSTATUSPROC      modem_status             = NULL;

// TAPI GLOBALS

static HLINEAPP            g_hLineApp            = NULL;
static HCALL               g_hCall               = NULL;
static HLINE               g_hLine               = NULL;
static DWORD               g_dwNumDevices        = 0;
static DWORD               g_dwDeviceID          = 0;

static LONG                g_lRequestedID;
static LONG                g_lAsyncReply;
static DWORD               g_dwCallState;
static DWORD               g_dwDesiredCallState;

// FORWARD DECLARATIONS

static BOOL StartCom();
static void StopCom();
static BOOL HangupCall();
static BOOL SendDataMessage (BYTE   sysmsgtype,
                             BYTE   targetmask,
                             LPVOID data,
                             DWORD  databytes);
BOOL CALLBACK ModemDestroy();
BOOL CALLBACK ModemStopAdvertisingGame();
static LONG WaitForReply(LONG lRequestedID);
static LONG WaitForCallState(DWORD dwDesiredCallState, DWORD dwWaitTime);
static BOOL TakeCall();
static void InsertHeaderKey(PACKETHEADERPTR header);

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
static BOOL FindNetworkId () {
  DWORD timeout = 16;
  while (timeout--) {

    // PICK A RANDOM NUMBER WHICH WE CAN USE TO RECOGNIZE AND DISCARD OUR
    // OWN PACKETS IN THE CASE OF A ROUTING LOOP.  MAKE SURE THE NUMBER IS
    // NOT ZERO, BECAUSE COMPUTERS WITH ESTABLISHED NETWORK IDS WILL USE ZERO.
    modem_routeloopcheck = 0;
    while (!modem_routeloopcheck)
      modem_routeloopcheck = PickRandomNumber();

    // PICK A NETWORK ID AT RANDOM, AND SEND OUT A QUERY TO SEE IF ANYONE
    // ELSE IS USING IT. IF WE ARE THE GAME OWNER WE CANNOT QUERY SINCE WE
    // HAVE ALREADY RETURNED A NETWORK ID IN ModemStartAdvertisingGame().
    if (!modem_networkidlocked) {
      modem_critsect.Enter();
      modem_networkid = 0;
      while (!modem_networkid)
        modem_networkid = (BYTE)((PickRandomNumber() % MAXPLAYERS) + 1);
      modem_critsect.Leave();
      DWORD messagedata[2] = {modem_networkid,modem_routeloopcheck};
      TraceOut("Snd: SYS_QUERYID");
      SendDataMessage(SYS_QUERYID,0xFF,&messagedata[0],2*sizeof(DWORD));

      // WAIT 1000 MILLISECONDS FOR A RESPONSE.  IF ANY OTHER COMPUTER ASSERTS
      // THAT IT IS USING THE ID WE PICKED, OR IF ANY OTHER COMPUTER QUERIES
      // FOR THE SAME ID, THEN GIVE UP THAT ID AND PICK A NEW ONE.
      DWORD starttime = GetTickCount();
      while (modem_networkid && (GetTickCount()-starttime < 1000))
        Sleep(10);

      // IF SOMEONE CONTESTED OUR ID, DUMP IT AND TRY AGAIN.
      if (!modem_networkid)
        continue;
    }

    // NO OTHER COMPUTER CONTESTED OUR NETWORK ID.  LOCK IT DOWN, ASSERT IT
    // ON THE NETWORK, AND RETURN SUCCESS.
    modem_critsect.Enter();
    modem_networkidlocked = 1;
    modem_routeloopcheck  = 0;
    modem_critsect.Leave();
    TraceOut("Snd: SYS_ASSERTID %d", modem_networkid);
    SendDataMessage(SYS_ASSERTID,0xFF,&modem_networkid,sizeof(DWORD));
    return 1;
  } // while (timeout--)
  return 0;
}

//===========================================================================
static void SendFormedMessage (MESSAGEDATAPTR messageptr,
                               DWORD          exceptport,
                               BOOL           wait) {
  BOOL systemmessage = (messageptr->header.startbyte_type == TYPE_SYSTEMDATA);

  // IF THIS IS A SYSTEM MESSAGE, WRITE IT OUT ON ALL ACTIVE PORTS.
  // OTHERWISE, WRITE IT OUT ON ALL PORTS WHICH LEAD TO ANY OF THE
  // DESIRED NETWORK IDS.
  HANDLE     event[PORTS];
  OVERLAPPED overlapped[PORTS];
  DWORD      byteswritten[PORTS];
  DWORD      numsends = 0;
  BOOL       bIOResult;
  ZeroMemory(&event[0],PORTS*sizeof(HANDLE));

  for (DWORD port = 0; port < PORTS; ++port) {
    if ((port != exceptport) && modem_port[port] &&
        (modem_port[port]->handle != INVALID_HANDLE_VALUE) &&
        (systemmessage ||
         (modem_port[port]->networkids & messageptr->header.targetmask))) {
      ZeroMemory(&overlapped[numsends],sizeof(OVERLAPPED));

      if (wait)
        event[numsends] = CreateEvent(NULL,0,0,NULL);
      ASSERT(event[numsends]);
      if (!event[numsends])
        return;

      overlapped[numsends].hEvent = event[numsends];
      bIOResult = WriteFile(modem_port[port]->handle,
                            messageptr,
                            messageptr->header.sizedwords*sizeof(DWORD),
                            &byteswritten[numsends],
                            &overlapped[numsends]);

      TraceDumpAddr("write data", messageptr, messageptr->header.sizedwords*sizeof(DWORD));
      // If WriteFile() sets the last error to something other than
      // ERROR_IO_PENDING we probably have a bad file handle, which
      // means that the remote party dropped the line. In this case
      // we do not want to wait on the event, so don't increment numsends
      if (bIOResult || GetLastError() == ERROR_IO_PENDING) {
        PerfIncrement(PERF_PKTSENT);
        PerfAdd(PERF_BYTESSENT,messageptr->header.sizedwords*sizeof(DWORD));
        ++numsends;
      }
      else if (wait && event[numsends])
        CloseHandle(event[numsends]);
    } // if ((port != exceptport) && modem_port[port] && ...
  } // for (DWORD port = 0; port < PORTS; ++port)
  if (numsends && wait) {
    // WAIT FOR ALL WRITES TO COMPLETE
    WaitForMultipleObjects(numsends,&event[0],1,INFINITE);
  }
  while (numsends--)
    CloseHandle(event[numsends]);
}

//===========================================================================
static BOOL SendDataMessage (BYTE   sysmsgtype,
                             BYTE   targetmask,
                             LPVOID data,
                             DWORD  databytes) {
  BOOL systemmessage = (sysmsgtype != SYS_UNUSED);
  if (systemmessage)
    targetmask = 0xFF;

  // DON'T SEND DATA WITHOUT A VALID CONNECTION
  if (!modem_lineestablished)
    return 0;

  // REFUSE TO SEND A NON-SYSTEM MESSAGE IF WE DON'T HAVE A VALID NETWORK ID
  if ((sysmsgtype == SYS_UNUSED) && (!(modem_networkid && modem_networkidlocked)))
    return 0;

  // CREATE A FULLY-FORMED MESSAGE
  MESSAGEDATAREC messagedata;
  ZeroMemory(&messagedata,sizeof(MESSAGEDATAREC));
  InsertHeaderKey(&messagedata.header);
  messagedata.header.startbyte_type = (sysmsgtype == SYS_UNUSED) ? TYPE_USERDATA : TYPE_SYSTEMDATA;
  messagedata.header.sizedwords     = (sizeof(PACKETHEADER)+databytes+sizeof(DWORD)-1)/sizeof(DWORD);
  messagedata.header.timetolive     = MAXPLAYERS;
  messagedata.header.targetmask     = targetmask;
  if (systemmessage)
    messagedata.header.sysmsgtype = sysmsgtype;
  else
    messagedata.header.fromid     = modem_networkid;
  if (data && databytes)
    CopyMemory(&messagedata.data[0],data,databytes);

  // SEND IT
  SendFormedMessage(&messagedata,0xFFFFFFFF,1);

  return 1;
}

//===========================================================================
static void ProcessIncomingMessage (DWORD port, MESSAGEPTR messageptr) {
  BOOL systemmessage = (messageptr->header.startbyte_type == TYPE_SYSTEMDATA);
  BYTE origtargetmask = messageptr->header.targetmask;

  TraceOut("ProcessIncomingMessage()");
  PerfIncrement(PERF_PKTRECV);
  PerfAdd(PERF_BYTESRECV,messageptr->bytesread);

  // VALIDATE THE PORT NUMBER
  if (port >= PORTS) {
    FREE(messageptr);
    return;
  }

  // VERIFY THAT WE ARE A RECIPIENT OF THIS MESSAGE, AND NOT THE SENDER
  if (modem_networkid && modem_networkidlocked &&
      ((!(origtargetmask & (1 << modem_networkid))) ||
       ((!systemmessage) &&
        (messageptr->header.fromid == modem_networkid)))) {
    FREE(messageptr);
    return;
  }

  // PROCESS USER MESSAGES
  if (!systemmessage) {
    TraceOut("USER MESSAGE");
    if (modem_networkid && modem_networkidlocked) {
      *(LPBYTE)&messageptr->addr = messageptr->header.fromid;
      LISTADDPTREND(&modem_messagehead,messageptr);
      if (modem_recvevent)
        SetEvent(modem_recvevent);
    }
    else
      FREE(messageptr);
  }

  // PROCESS SYSTEM MESSAGES
  else {
    TraceOut("SYSTEM MESSAGE");
    switch (messageptr->header.sysmsgtype) {

      case SYS_QUERYLINE:
        TraceOut("Rcv: SYS_QUERYLINE");
        TraceOut("Snd: SYS_ASSERTLINE");
        SendDataMessage(SYS_ASSERTLINE,0xFF,NULL,0);
        modem_critsect.Enter();
        modem_lineconfirmed = 1;
        modem_critsect.Leave();
        break;

      case SYS_ASSERTLINE:
        TraceOut("Rcv: SYS_ASSERTLINE");
        modem_critsect.Enter();
        modem_lineconfirmed = 1;
        modem_critsect.Leave();
        break;

      case SYS_QUERYID:
        TraceOut("Rcv: SYS_QUERYID");
        if (messageptr->bytesread >= sizeof(PACKETHEADER)+2*sizeof(DWORD)) {
          DWORD networkid      = *(LPDWORD)&messageptr->data[0];
          DWORD routeloopcheck = *(LPDWORD)&messageptr->data[sizeof(DWORD)];
          if (routeloopcheck != modem_routeloopcheck) {
            if (modem_networkidlocked) {
              TraceOut("Snd: SYS_ASSERTID");
              SendDataMessage(SYS_ASSERTID,0xFF,&modem_networkid,sizeof(DWORD));
            }
            else if (networkid == modem_networkid)
              modem_networkid = 0;
          }
        }
        break;

      case SYS_ASSERTID:
        TraceOut("Rcv: SYS_ASSERTID");
        if (messageptr->bytesread >= sizeof(PACKETHEADER)+sizeof(DWORD)) {
          DWORD networkid = *(LPDWORD)&messageptr->data[0];
          modem_port[port]->networkids |= (1 << networkid);
          if ((!modem_networkidlocked) && (networkid == modem_networkid))
            modem_networkid = 0;
        }
        break;

      case SYS_QUERYGAME:
        TraceOut("Rcv: SYS_QUERYGAME");
        if (modem_gameadvinfo) {
          TraceOut("Snd: SYS_GAMEINFO");
          SendDataMessage(SYS_GAMEINFO,0xFF,modem_gameadvinfo,modem_gameadvinfo->bytes);
        }
        break;

      case SYS_GAMEINFO:
        TraceOut("Rcv: SYS_GAMEINFO");
        {
          modem_critsect.Enter();
          ADVPTR advinfo = (ADVPTR)&messageptr->data[0];
          if ((advinfo->programid == modem_programid) &&
              (advinfo->versionid == modem_versionid)) {
            if (!modem_gamelist) {
              modem_gamelist = NEW(SNETSPI_GAMELIST);
              ASSERT(modem_gamelist);
              if (!modem_gamelist)
                break;
              ZeroMemory(modem_gamelist,sizeof(SNETSPI_GAMELIST));
            }
            *(LPBYTE)&modem_gamelist->owner = (BYTE)advinfo->networkid;
            modem_gamelist->gameid = 1;
            strcpy(modem_gamelist->gamename,advinfo->strings);
            strcpy(modem_gamelist->gamedescription,advinfo->strings+strlen(advinfo->strings)+1);
          }
          else
            modem_versionmismatch = 1;
          modem_critsect.Leave();
        }
        break;

      case SYS_REMOVE:
        TraceOut("Rcv: SYS_REMOVE");
        modem_critsect.Enter();
        ADVPTR advinfo = (ADVPTR)&messageptr->data[0];
        if (modem_networkid != advinfo->networkid && modem_gamelist) {
          FREE(modem_gamelist);
          modem_gamelist = NULL;
        }
        modem_critsect.Leave();
        break;
    }
    FREE(messageptr);
  }
}

//===========================================================================
BOOL CALLBACK CancelCall()
{
  modem_critsect.Enter();
  modem_callstatus = SNET_ERROR_CANCELLED;
  modem_critsect.Leave();
  TraceOut("CancelCall()");
  SetEvent(modem_TAPIHangupEvent);
  return 1;
}

//===========================================================================
static void UpdateCallStatus(DWORD dwStringResource)
{
  char buf[SNETSPI_MAXSTRINGLENGTH];
  if (!modem_status)
    return;
  int ret = LoadString(global_instance, dwStringResource, buf,
                       SNETSPI_MAXSTRINGLENGTH);
  ASSERT(ret);
  if (!ret)
    return;
  modem_status(buf, 0, 0, 0, CancelCall);
}

//===========================================================================
static void HandleLineCallState(
    DWORD dwDevice, DWORD dwMessage, DWORD dwCallbackInstance,
    DWORD dwParam1, DWORD dwParam2, DWORD dwParam3)
{
  // dwParam1 is the specific CALLSTATE change that is occurring.
  g_dwCallState = dwParam1;
  if (g_dwCallState == g_dwDesiredCallState)
    SetEvent(modem_TAPICallStateEvent);
  TraceOut("LINE_CALLSTATE: %d", g_dwCallState);
  switch (dwParam1)
  {
    // ANSWER NEW CALL
    case LINECALLSTATE_OFFERING:
      modem_critsect.Enter();
      g_hCall = (HCALL)dwDevice;
      modem_critsect.Leave();
      SetEvent(modem_TAPIAnswerEvent);
      break;

    case LINECALLSTATE_IDLE:
      TraceOut("LINECALLSTATE_IDLE");
      break;

    case LINECALLSTATE_BUSY:
      TraceOut("LINECALLSTATE_BUSY");
      break;

    case LINECALLSTATE_DISCONNECTED:
      switch (dwParam2)
      {
        case LINEDISCONNECTMODE_NORMAL:
          TraceOut("LINEDISCONNECTMODE_NORMAL");
          break;

        case LINEDISCONNECTMODE_BUSY:
          modem_critsect.Enter();
          modem_callstatus = SNET_ERROR_NETWORK_BUSY;
          modem_critsect.Leave();
          break;

        case LINEDISCONNECTMODE_NOANSWER:
          modem_critsect.Enter();
          modem_callstatus = SNET_ERROR_NOT_CONNECTED;
          modem_critsect.Leave();
          break;

        case LINEDISCONNECTMODE_NODIALTONE:
          modem_critsect.Enter();
          modem_callstatus = SNET_ERROR_NO_NETWORK;
          modem_critsect.Leave();
          break;

        case LINEDISCONNECTMODE_BADADDRESS:
        case LINEDISCONNECTMODE_UNREACHABLE:
        case LINEDISCONNECTMODE_CONGESTION:
        case LINEDISCONNECTMODE_INCOMPATIBLE:
        case LINEDISCONNECTMODE_UNAVAIL:
        case LINEDISCONNECTMODE_UNKNOWN:
        case LINEDISCONNECTMODE_REJECT:
        case LINEDISCONNECTMODE_PICKUP:
        case LINEDISCONNECTMODE_FORWARDED:
        default:
          break;
      }
      // THERE ARE 3 CASES HERE:
      // 1) WE WERE THE ORIGINAL GAME OWNER, AND THE CALLER DROPPED
      //    THE LINE. IN THIS CASE WE WILL RECONFIGURE THE LINE TO
      //    ACCEPT CALLS
      // 2) WE ARE THE CALLER, AND THE SERVER DROPPED. WE HAVE
      //    RECEIVED THIS ASYNC MESSAGE BEFORE SPISTARTADVERTISE.
      //    IN THIS CASE WE DO NOT CONFIGURE FOR CALL RECEIPT SINCE
      //    WE DO NOT TECHNICALLY KNOW WE ARE NOW THE GAME OWNER, I.E.
      //    THERE IS NO VALID ADVINFO RECORD. SPISTARTADVERTISE WILL
      //    DO THIS ON FINDING TAPI SHUTDOWN AFTER OUR HANGUP CALL.
      // 3) AS ABOVE, BUT WE ARE HERE AFTER THE SPISTARTADVERTISE.
      //    THE MODEM WAS NOT CONFIGURED THERE SINCE TAPI WAS STILL
      //    RUNNING, SO WE NEED TO START IT UP!

      SetEvent(modem_TAPIHangupEvent);
      break;

    case LINECALLSTATE_CONNECTED:
      TraceOut("LINECALLSTATE_CONNECTED");
      StartCom();
      UpdateCallStatus(IDS_CONNECTING);
      break;

    case LINECALLSTATE_DIALING:
      UpdateCallStatus(IDS_DIALING);
      break;

    case LINECALLSTATE_RINGBACK:
      UpdateCallStatus(IDS_RINGING);
      break;

    case LINECALLSTATE_PROCEEDING:
      UpdateCallStatus(IDS_PROCEEDING);
      break;

    case LINECALLSTATE_DIALTONE:
      UpdateCallStatus(IDS_DIALTONE);
      break;

    case LINECALLSTATE_SPECIALINFO:
    default:
      break;
  }
}

//===========================================================================
static void CALLBACK TAPIEventCallback (
	DWORD hDevice,
	DWORD dwMsg,
	DWORD dwCallbackInstance,
	DWORD dwParam1,
	DWORD dwParam2,
	DWORD dwParam3
	)
{

  // Handle the line messages.
  switch(dwMsg)
  {
    case LINE_CALLSTATE:
      HandleLineCallState(hDevice, dwMsg, dwCallbackInstance,
                          dwParam1, dwParam2, dwParam3);
      break;

    case LINE_CLOSE:
      TraceOut("LINE_CLOSE");
      // Line has been shut down.  
      break;

    case LINE_REPLY:
      TraceOut("LINE_REPLY");
      modem_critsect.Enter();
      if (g_lRequestedID == (LONG)dwParam1) {
        g_lAsyncReply = (LONG)dwParam2;
        SetEvent(modem_TAPILineReplyEvent);
      }
      modem_critsect.Leave();
      break;

    case LINE_CREATE:
    default:
      break;
  }
  return;
}

//===========================================================================
static DWORD CALLBACK TAPIProc(LPVOID) {

  HANDLE events[] = {modem_TAPITerminateEvent, modem_TAPIEvent};
  DWORD dwEvent;
  MSG msg; 

  modem_critsect.Enter();
  LONG result = lineInitialize(&g_hLineApp, global_instance,
                               TAPIEventCallback, NULL, &g_dwNumDevices);
  modem_critsect.Leave();
  ASSERT(!result);
  // NOTIFY OF INIT COMPLETION
  SetEvent(modem_TAPINotifyEvent);

  while (1) {
    if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    ResetEvent(modem_TAPIEvent);
    dwEvent = MsgWaitForMultipleObjects(2, events, FALSE, INFINITE, QS_ALLINPUT);
    TraceOut("TAPIProc awake");
    if (dwEvent == WAIT_FAILED || !(dwEvent - WAIT_OBJECT_0))
      break;
  }
  ExitThread(0);
  return 0;
}

//===========================================================================
static DWORD CALLBACK TAPIAnswerProc(LPVOID) {
  HANDLE events[] = {modem_TAPITerminateEvent, modem_TAPIAnswerEvent};
  DWORD dwEvent;
  LONG lResult;

  while (1) {
    ResetEvent(modem_TAPIAnswerEvent);
    dwEvent = WaitForMultipleObjects(2, events, FALSE, INFINITE);
    if (dwEvent == WAIT_FAILED || !(dwEvent - WAIT_OBJECT_0))
      break;

    lResult = WaitForReply(lineAnswer(g_hCall, NULL, 0));
    if (lResult)
      continue;

	// THIS MAY NOT BE NECESSARY, BUT IT MIGHT BE NICE TO DETECT IF
	// THE CALLER IS RUNNING DIABLO AND HAGUP IF NOT, SO GO AHEAD
	// AND LEAVE THIS AS A SEPARATE THREAD
#if 0
    // TIMEOUT IF THE CALLER HUNG UP
    lResult = WaitForCallState(LINECALLSTATE_CONNECTED, ANSWERTIMEOUT);
    if (lResult)
      continue;
#endif
  }
  ExitThread(0);
  return 0;
}

//===========================================================================
static DWORD CALLBACK TAPIHangupProc(LPVOID) {
  HANDLE events[] = {modem_TAPITerminateEvent, modem_TAPIHangupEvent};
  DWORD dwEvent;
  while (1) {
    ResetEvent(modem_TAPIHangupEvent);
    dwEvent = WaitForMultipleObjects(2, events, FALSE, INFINITE);
    if (dwEvent == WAIT_FAILED || !(dwEvent - WAIT_OBJECT_0))
      break;
    HangupCall();
    SetEvent(modem_TAPIHangupNotify);
    if (modem_gameadvinfo)
      TakeCall();
  }
  ExitThread(0);
  return 0;
}

//===========================================================================
static void InsertHeaderKey(PACKETHEADERPTR header)
{
  ASSERT(header);
  if (!header)
    return;
  CopyMemory(header->key_sequence, gs_arrHeaderKeys, HEADER_KEYS);
}

//===========================================================================
static BOOL ScanForHeader(LPDWORD bytesleft, LPBYTE *dataptr)
{
  ASSERT(bytesleft && *dataptr);
  if (!(bytesleft && *dataptr))
    return FALSE;

  static int matchlevel = 0;
  while (*bytesleft && matchlevel < HEADER_KEYS) {
    if (**dataptr == gs_arrHeaderKeys[matchlevel])
      matchlevel++;
    else if (matchlevel) {
      matchlevel = 0;
      TraceOut("Reset Match");
      continue;
    }
    (*bytesleft)--;
    (*dataptr)++;
  }
  if (matchlevel == HEADER_KEYS) {
    matchlevel = 0;
    return TRUE;
  }
  return FALSE;
}

//===========================================================================
unsigned CALLBACK ThreadProc (LPVOID) {
  while (modem_state != STATE_SHUTDOWN) {
    TraceOut("Read Thread Awake");
    modem_critsect.Enter();
    for (DWORD port = 0; port < PORTS; ++port) {

      if (modem_port[port]->handle == INVALID_HANDLE_VALUE)
        continue;

      // CHECK TO SEE IF AN I/O OPERATION HAS COMPLETED ON THIS PORT
      if (!GetOverlappedResult(modem_port[port]->handle,
                               &modem_port[port]->overlapped,
                               &modem_port[port]->bytesread,
                               0)) {
        if (GetLastError() == ERROR_OPERATION_ABORTED)
          ResetEvent(modem_event[port]);
        continue;
      }

      // IF SO, PARSE THE INCOMING DATA
      DWORD bytesleft = modem_port[port]->bytesread;
      LPBYTE dataptr = &modem_port[port]->readbuffer[0];
      TraceDumpAddr("read data", dataptr, bytesleft);

      MESSAGEPTR messageptr;
      while (bytesleft) {
        messageptr = NULL;
        if (modem_port[port]->partialmessage) {
          TraceOut("Partial Message");
          messageptr = modem_port[port]->partialmessage;
        }
        else if (ScanForHeader(&bytesleft, &dataptr)) {
          // ALLOCATE NEW RECORD
          messageptr = NEW(MESSAGEREC);
          ASSERT(messageptr);
          if (!messageptr) {
            SetLastError(SNET_ERROR_NOT_ENOUGH_MEMORY);
            return 0;
          }
          TraceOut("NEW(MESSAGEREC)");
          ZeroMemory(messageptr, sizeof(MESSAGEREC));
          messageptr->inport = port;

          // INSERT MULTIBYTE HEADER KEY
          InsertHeaderKey(&messageptr->header);
          messageptr->bytesread = HEADER_KEYS;
          modem_port[port]->partialmessage = messageptr;
        }

        // PROCESS NEW AND PARTIAL PACKETS
        if (!bytesleft)
          continue;

        if (!messageptr->bytesneeded) {
          messageptr->bytesneeded = (*dataptr) * sizeof(DWORD);
          if (messageptr->bytesneeded > sizeof(PACKETHEADER) + MAXMESSAGESIZE) {
            TraceOut("Bogus Packet");
            FREE(messageptr);
            modem_port[port]->partialmessage = NULL;
            continue;
          }
        }
        LPBYTE writeptr = (LPBYTE)&messageptr->header;
        DWORD bytestocopy =
          min(bytesleft, messageptr->bytesneeded - messageptr->bytesread);
        CopyMemory(writeptr + messageptr->bytesread, dataptr, bytestocopy);
        messageptr->bytesread += bytestocopy;
        dataptr += bytestocopy;
        bytesleft -= bytestocopy;
        if (!(messageptr->bytesread < messageptr->bytesneeded)) {
          ProcessIncomingMessage(port, messageptr);
          modem_port[port]->partialmessage = NULL;
        }
      } // while (bytesleft)

      // RETURN THIS PORT'S EVENT TO A NONSIGNALED STATE
      ResetEvent(modem_event[port]);

      // POST ANOTHER OVERLAPPED READ FOR THIS PORT
      ReadFile(modem_port[port]->handle,
               modem_port[port]->readbuffer,
               READBUFFERSIZE,
               &modem_port[port]->bytesread,
               &modem_port[port]->overlapped);
    } // for (DWORD port = 0; port < PORTS; ++port)
    modem_critsect.Leave();
    WaitForMultipleObjects(PORTS,&modem_event[0],0,INFINITE);
  } // while (modem_state != STATE_SHUTDOWN)
  modem_state = STATE_NOGAME;
  _endthreadex(0);
  return 0;
}

//===========================================================================
static void CleanupEvents()
{
  // CLEANUP EVENTS
  if (modem_TAPITerminateEvent) {
    CloseHandle(modem_TAPITerminateEvent);
    modem_TAPITerminateEvent = NULL;
  }
  if (modem_TAPIEvent) {
    CloseHandle(modem_TAPIEvent);
    modem_TAPIEvent = NULL;
  }
  if (modem_TAPINotifyEvent) {
    CloseHandle(modem_TAPINotifyEvent);
    modem_TAPINotifyEvent = NULL;
  }
  if (modem_TAPIHangupEvent) {
    CloseHandle(modem_TAPIHangupEvent);
    modem_TAPIHangupEvent = NULL;
  }
  if (modem_TAPIHangupNotify) {
    CloseHandle(modem_TAPIHangupNotify);
    modem_TAPIHangupNotify = NULL;
  }
  if (modem_TAPIAnswerEvent) {
    CloseHandle(modem_TAPIAnswerEvent);
    modem_TAPIAnswerEvent = NULL;
  }
  if (modem_TAPICallStateEvent) {
    CloseHandle(modem_TAPICallStateEvent);
    modem_TAPICallStateEvent = NULL;
  }
  if (modem_TAPILineReplyEvent) {
    CloseHandle(modem_TAPILineReplyEvent);
    modem_TAPILineReplyEvent = NULL;
  }
}

//===========================================================================
static BOOL ShutdownTAPI()
{
  long lReturn;

  // If we aren't initialized, then Shutdown is unnecessary.
  if (!g_hLineApp)
    return TRUE;

  // POST HANGUP AND SHUTDOWN THREADS. HANGUPPROC WILL ALWAYS GIVE
  // PREFERENCE TO HANGUP EVENTS OVER DESTROY EVENTS, SO WE KNOW THE
  // LINE IS CLEAN AFTER THE WAIT.

  SetEvent(modem_TAPIHangupEvent);
  ResetEvent(modem_TAPIHangupNotify);
  WaitForSingleObject(modem_TAPIHangupNotify, 2000);

  SetEvent(modem_TAPITerminateEvent);
  HANDLE hHarray[] = {modem_tapithread, modem_tapianswerthread,
                      modem_tapihangupthread};
  WaitForMultipleObjects(3, hHarray, TRUE, 2000);
  modem_tapithread = NULL;
  modem_tapianswerthread = NULL;
  modem_tapihangupthread = NULL;

  CleanupEvents();

  // SHUTDOWN TAPI INTERFACE
  if (g_hLineApp) {
    lReturn = lineShutdown(g_hLineApp);
    g_hLineApp = NULL;
  }

  return TRUE;
}

//===========================================================================
static BOOL StartupTAPI()
{
  // If we aren initialized, then Startup is unnecessary.
  if (g_hLineApp)
    return TRUE;

  modem_TAPITerminateEvent = CreateEvent(NULL, 1, 0, NULL);
  modem_TAPIEvent          = CreateEvent(NULL, 1, 0, NULL);
  modem_TAPINotifyEvent    = CreateEvent(NULL, 1, 0, NULL);
  modem_TAPIAnswerEvent    = CreateEvent(NULL, 1, 0, NULL);
  modem_TAPIHangupEvent    = CreateEvent(NULL, 1, 0, NULL);
  modem_TAPIHangupNotify   = CreateEvent(NULL, 1, 0, NULL);
  modem_TAPICallStateEvent = CreateEvent(NULL, 1, 0, NULL);
  modem_TAPILineReplyEvent = CreateEvent(NULL, 1, 0, NULL);

  // CREATE A THREAD TO HANDLE TAPI MESSAGES. THIS IS NECESSARY
  // SINCE MANY SPI FUNCTIONS REQUIRE BLOCKING, AND THIS BRAIN DEAD
  // VERSION OF TAPI USES A MESSAGE LOOP FOR STATUS.
  ResetEvent(modem_TAPINotifyEvent);
  if (modem_TAPIEvent && !modem_tapithread) {
    DWORD threadid;
    modem_tapithread = CreateThread(NULL, 0, TAPIProc,
                                    NULL, 0, &threadid);
    ASSERT(modem_tapithread);
    if (!modem_tapithread) {
      SetEvent(modem_TAPITerminateEvent);
      modem_tapithread = NULL;
      SetLastError(SNET_ERROR_MAX_THRDS_REACHED);
      return 0;
    }
  }
  WaitForSingleObject(modem_TAPINotifyEvent, INFINITE);
  if (!g_hLineApp) {
    SetEvent(modem_TAPITerminateEvent);
    WaitForSingleObject(modem_tapithread, INFINITE);
    modem_tapithread = NULL;
    CleanupEvents();
    return 0;
  }

  // CREATE THE ANSWER THREAD
  if (modem_TAPIAnswerEvent && !modem_tapianswerthread) {
    DWORD threadid;
    modem_tapianswerthread = CreateThread(NULL, 0, TAPIAnswerProc,
                                          NULL, 0, &threadid);
    ASSERT(modem_tapianswerthread);
    if (!modem_tapianswerthread) {
      SetEvent(modem_TAPITerminateEvent);
      modem_tapianswerthread = NULL;
      SetLastError(SNET_ERROR_MAX_THRDS_REACHED);
      return 0;
    }
  }

  // CREATE THE HANGUP THREAD
  if (modem_TAPIHangupEvent && modem_TAPIHangupNotify &&
      !modem_tapihangupthread) {
    DWORD threadid;
    modem_tapihangupthread = CreateThread(NULL, 0, TAPIHangupProc,
                                          NULL, 0, &threadid);
    ASSERT(modem_tapihangupthread);
    if (!modem_tapihangupthread) {
      SetEvent(modem_TAPITerminateEvent);
      modem_tapihangupthread = NULL;
      SetLastError(SNET_ERROR_MAX_THRDS_REACHED);
      return 0;
    }
  }

  return 1;
}

//===========================================================================
static LONG WaitForReply(LONG lRequestedID)
{
  if (!(lRequestedID > 0))
    return lRequestedID;

  modem_critsect.Enter();
  g_lRequestedID = lRequestedID;
  g_lAsyncReply = lRequestedID;
  ResetEvent(modem_TAPILineReplyEvent);
  modem_critsect.Leave();
  WaitForSingleObject(modem_TAPILineReplyEvent, INFINITE);

  return g_lAsyncReply;
}

//===========================================================================
static LONG WaitForCallState(DWORD dwDesiredCallState, DWORD dwWaitTime)
{
  modem_critsect.Enter();
  g_dwDesiredCallState = dwDesiredCallState;
  ResetEvent(modem_TAPICallStateEvent);
  modem_critsect.Leave();
  DWORD dwRes = WaitForSingleObject(modem_TAPICallStateEvent, dwWaitTime);

  if (dwRes == WAIT_TIMEOUT)
      return WAITERR_WAITTIMEDOUT;
  if (g_dwDesiredCallState != dwDesiredCallState)
      return WAITERR_WAITABORTED;

  return 0;
}

//===========================================================================
static BOOL HangupCall()
{         
  LPLINECALLSTATUS pLineCallStatus = NULL;
  long lReturn;

  TraceOut("HangupCall()");

  StopCom();

  // SET WAIT EVENTS SO THAT ANY THREAD WAITING ON TAPI EVENTS WILL
  // TERMINATE CLEANLY
  modem_critsect.Enter();
  g_dwDesiredCallState = LINECALLSTATE_DISCONNECTED;
  SetEvent(modem_TAPICallStateEvent);
  SetEvent(modem_TAPILineReplyEvent);
  modem_critsect.Leave();

  // If there is a call in progress, drop and deallocate it.
  if (g_hCall) {

    pLineCallStatus = (LPLINECALLSTATUS)LocalAlloc(LPTR, sizeof(LINECALLSTATUS));
    ASSERT(pLineCallStatus);
    if (!pLineCallStatus)
      return FALSE;
    lReturn = lineGetCallStatus(g_hCall, pLineCallStatus);

    // Only drop the call when the line is not IDLE.
    if (!((pLineCallStatus->dwCallState) & LINECALLSTATE_IDLE)) {
      lReturn = WaitForReply(lineDrop(g_hCall, NULL, 0));
      ASSERT(!lReturn);
      if (lReturn)
        return FALSE;
    }

    // Need to free buffer returned from lineGetCallStatus
    if (pLineCallStatus)
      LocalFree(pLineCallStatus);  

    modem_critsect.Enter();
    lReturn = lineDeallocateCall(g_hCall);
    ASSERT(!lReturn);
    if (lReturn) {
      modem_critsect.Leave();
      return FALSE;
    }
    g_hCall = NULL;
    modem_critsect.Leave();
  }

  // if we have a line open, close it.
  if (g_hLine) {
    modem_critsect.Enter();
    lReturn = lineClose(g_hLine);
    ASSERT(!lReturn);
    if (lReturn) {
      modem_critsect.Leave();
      return FALSE;
    }
    g_hLine = NULL;
    modem_critsect.Leave();
  }

  if (modem_gamelist) {
    modem_critsect.Enter();
    FREE(modem_gamelist);
    modem_gamelist = NULL;
    modem_critsect.Leave();
  }
  return TRUE;
}

//===========================================================================
static BOOL GetDeviceAPIVersion(DWORD id, LPDWORD lpdwTAPIVersion)
{
  LINEEXTENSIONID lineExtId;

  ASSERT(g_hLineApp && g_dwNumDevices);
  LONG lReturn = lineNegotiateAPIVersion(g_hLineApp, id,
                                         TAPI_CURRENT_VERSION,
			                                   TAPI_CURRENT_VERSION,
                                         lpdwTAPIVersion,
                                         &lineExtId);
  return (!lReturn);
}

//===========================================================================
static LPLINEDEVCAPS GetDeviceCaps(DWORD id, DWORD dwTAPIVersion)
{
  LINEDEVCAPS tmpLineDevCaps;
  LPLINEDEVCAPS lpLineDevCaps;
  LONG lReturn;

  ASSERT(g_hLineApp && g_dwNumDevices);

	tmpLineDevCaps.dwTotalSize = sizeof(tmpLineDevCaps);
	lReturn = lineGetDevCaps(g_hLineApp, id, dwTAPIVersion, 0, &tmpLineDevCaps);
  if (lReturn) {
    return NULL;
  }

  lpLineDevCaps = (LPLINEDEVCAPS)LocalAlloc(LPTR, tmpLineDevCaps.dwNeededSize);
  ASSERT(lpLineDevCaps);
  if (!lpLineDevCaps) {
    SetLastError(SNET_ERROR_NOT_ENOUGH_MEMORY);
    return NULL;
  }

	lpLineDevCaps->dwTotalSize = tmpLineDevCaps.dwNeededSize;
	lReturn = lineGetDevCaps(g_hLineApp, id, dwTAPIVersion, 0, lpLineDevCaps);
  if (lReturn) {
    LocalFree(lpLineDevCaps);
    return NULL;
  }
  return lpLineDevCaps;
}

//===========================================================================
static LPLINECALLPARAMS CreateCallParams(LPCSTR szAddress)
{
  LPLINECALLPARAMS lpCallParams = NULL;
  DWORD dwAddressSize;

  ASSERT(szAddress && *szAddress);

  dwAddressSize = strlen(szAddress) + 1;
                        
  lpCallParams = (LPLINECALLPARAMS)LocalAlloc(LPTR, sizeof(LINECALLPARAMS) +
                                              dwAddressSize);
  ASSERT(lpCallParams);
  if (!lpCallParams) {
    SetLastError(SNET_ERROR_NOT_ENOUGH_MEMORY);
    return NULL;
  }

  lpCallParams->dwTotalSize = sizeof(LINECALLPARAMS) + dwAddressSize;
  // This is where we configure the line.
  lpCallParams->dwBearerMode = LINEBEARERMODE_VOICE;
  lpCallParams->dwMediaMode  = LINEMEDIAMODE_DATAMODEM;

  // This specifies that we want to use only IDLE calls and
  // don't want to cut into a call that might not be IDLE (ie, in use).
  lpCallParams->dwCallParamFlags = LINECALLPARAMFLAGS_IDLE;
                                  
  // if there are multiple addresses on line, use first anyway.
  // It will take a more complex application than a simple tty app
  // to use multiple addresses on a line anyway.
  lpCallParams->dwAddressMode = LINEADDRESSMODE_ADDRESSID;

  // Address we are dialing.
  lpCallParams->dwDisplayableAddressOffset = sizeof(LINECALLPARAMS);
  lpCallParams->dwDisplayableAddressSize = dwAddressSize;
  strcpy((LPSTR)lpCallParams + sizeof(LINECALLPARAMS), szAddress);

  return lpCallParams;
}

//===========================================================================
static LPLINETRANSLATEOUTPUT TranslateAddress(
  DWORD id, DWORD dwTAPIVersion, LPCTSTR szAddress)
{
  LPLINETRANSLATEOUTPUT lpOutput = NULL;
  DWORD dwSize;
  LONG lReturn;

  ASSERT(g_hLineApp && g_dwNumDevices);

  // LOOP UNTIL WE HAVE ALLOCATED A STRUCTURE LARGE ENOUGH TO
  // HOLD THE ENTIRE ADDRESS INFORMATION BLOCK
  dwSize = sizeof(LINETRANSLATEOUTPUT);
  do {
    if (lpOutput) {
      dwSize = lpOutput->dwNeededSize;
      LocalFree(lpOutput);
    }
    lpOutput = (LPLINETRANSLATEOUTPUT)LocalAlloc(LPTR, dwSize);
    ASSERT(lpOutput);
    if (!lpOutput) {
      SetLastError(SNET_ERROR_NOT_ENOUGH_MEMORY);
      return NULL;
    }
    lpOutput->dwTotalSize = dwSize;

	  lReturn = lineTranslateAddress(g_hLineApp, id, dwTAPIVersion,
                                   szAddress, 0, 0, lpOutput);
    ASSERT(!lReturn);
    if (lReturn) {
      LocalFree(lpOutput);
      return NULL;
    }
  } while (lpOutput->dwTotalSize < lpOutput->dwNeededSize);
 
  return lpOutput;
}

//===========================================================================
static BOOL MakeCall(LPCSTR szAddress)
{
  BOOL bVal;
  DWORD dwTAPIVersion;
  LPLINEDEVCAPS lpLineDevCaps = NULL;
  LPLINECALLPARAMS lpCallParams = NULL;
  LPLINETRANSLATEOUTPUT lpOutput = NULL;
  LPCTSTR lpDialString = NULL;
  LONG lResult;

  TraceOut("MakeCall()");

  // MAKE SURE LINE SUPPORTS DIAL-OUT
  bVal = GetDeviceAPIVersion(g_dwDeviceID, &dwTAPIVersion);
  ASSERT(bVal);
  if (!bVal)
    return 0;

  lpLineDevCaps = GetDeviceCaps(g_dwDeviceID, dwTAPIVersion);
  ASSERT(lpLineDevCaps);
  if (!lpLineDevCaps)
    return 0;

  if (!(lpLineDevCaps->dwLineFeatures & LINEFEATURE_MAKECALL))
    return 0;

  if (lpLineDevCaps)
    LocalFree(lpLineDevCaps);

  modem_critsect.Enter();
  lResult = lineOpen(g_hLineApp, g_dwDeviceID, &g_hLine, dwTAPIVersion, 0, 0,
                     LINECALLPRIVILEGE_NONE, LINEMEDIAMODE_DATAMODEM, NULL);
  modem_critsect.Leave();
	ASSERT(!lResult);
	if (lResult == LINEERR_ALLOCATED)
    return 0;
	if (lResult)
    return 0;

  lpCallParams = CreateCallParams(szAddress);
	ASSERT(lpCallParams);
  if (!lpCallParams)
    return 0;

  // CALL TRANSLATE ADDRESS TO CAPTURE ANY LINE PROPERITES SUCH
  // AS PULSE DIALING. NOTE THAT COUNTRY AND AREA CODES ARE NOT
  // HANDLED - IT IS UP TO THE USER TO ENTER A FULLY QUALIFIED
  // DIALING STRING
  lpOutput = TranslateAddress(g_dwDeviceID, dwTAPIVersion, szAddress);
	ASSERT(lpOutput);
  if (!lpOutput)
    return 0;
  lpDialString = (LPCTSTR)lpOutput + lpOutput->dwDialableStringOffset;
  TraceOut("Dialing %s", lpDialString);

  lResult = WaitForReply(lineMakeCall(g_hLine, &g_hCall, lpDialString,
                                      0, lpCallParams));

  if (lpCallParams)
    LocalFree(lpCallParams);
  if (lpOutput)
    LocalFree(lpOutput);
  
  return (!lResult);
}

//===========================================================================
static BOOL TakeCall()
{
  BOOL bVal;
  DWORD dwTAPIVersion;
  LONG lResult;

  TraceOut("TakeCall()");
  bVal = GetDeviceAPIVersion(g_dwDeviceID, &dwTAPIVersion);
  if (!bVal)
    return 0;

  modem_critsect.Enter();
  lResult = lineOpen(g_hLineApp, g_dwDeviceID, &g_hLine, dwTAPIVersion, 0, 0,
                     LINECALLPRIVILEGE_OWNER, LINEMEDIAMODE_DATAMODEM, NULL);
  modem_critsect.Leave();
	ASSERT(!lResult);
	if (lResult)
    return 0;

  return 1;
}

//===========================================================================
static LPVARSTRING GetVarString()
{
  VARSTRING tmpVarString;
  LPVARSTRING lpVarString = NULL;
  long lReturn;

  ASSERT(g_hCall);
  if (!g_hCall)
    return NULL;

  tmpVarString.dwTotalSize = sizeof(tmpVarString);
  lReturn = lineGetID(0, 0, g_hCall, LINECALLSELECT_CALL, &tmpVarString,
                      "comm/datamodem");
  ASSERT(!lReturn);
  if (lReturn)
    return NULL;

  lpVarString = (LPVARSTRING)LocalAlloc(LPTR, tmpVarString.dwNeededSize);
  ASSERT(lpVarString);
  if (!lpVarString)
    return NULL;

  lpVarString->dwTotalSize = tmpVarString.dwNeededSize;
  lReturn = lineGetID(0, 0, g_hCall, LINECALLSELECT_CALL, lpVarString,
                      "comm/datamodem");
  ASSERT(!lReturn);
  if (lReturn) {
    LocalFree(lpVarString);
    return NULL;
  }
  return lpVarString;
}

//===========================================================================
static BOOL InitializePort (DWORD port) {

  // ALLOCATE MEMORY FOR THE PORT RECORD
  ASSERT(!modem_port[port]);
  if (!modem_port[port]) {
    modem_port[port] = NEW(PORTREC);
    ASSERT(modem_port[port]);
    if (!modem_port[port])
      return 0;
    ZeroMemory(modem_port[port],sizeof(PORTREC));
    TraceOut("Allocated port %d", port);
  }

  // CREATE AN EVENT FOR OVERLAPPED I/O
  ASSERT(!modem_event[port]);
  if (!modem_event[port]) {
    modem_event[port] = CreateEvent(NULL,1,0,NULL);
    if (!modem_event[port])
      return 0;
    modem_port[port]->overlapped.hEvent = modem_event[port];
  }

  // Get the handle to the comm port from the driver so we can start
  // communicating.  This is returned in a LPVARSTRING structure.
  LPVARSTRING lpVarString = GetVarString();
  ASSERT(lpVarString);
  if (!lpVarString)
    return 0;

  // Again, the handle to the comm port is contained in a
  // LPVARSTRING structure.  Thus, the handle is the very first
  // thing after the end of the structure.  Note that the name of
  // the comm port is right after the handle, but I don't want it.
  modem_port[port]->handle =  *((LPHANDLE)((LPBYTE)lpVarString +
                               lpVarString->dwStringOffset));
  ASSERT(modem_port[port]->handle);

  if (lpVarString)
      LocalFree(lpVarString);

  // IF WE COULDN'T OPEN THE PORT THEN IT IS PROBABLY UNCONFIGURED OR IN
  // USE BY ANOTHER APPLICATION.  RETURN SUCCESS BECAUSE THIS ISN'T A FATAL
  // ERROR; WE WILL JUST NOT USE THIS PARTICULAR PORT.
  if (modem_port[port]->handle == INVALID_HANDLE_VALUE)
    return 1;

  // SET THE COMMUNICATIONS TIMEOUT VALUES
  {
    COMMTIMEOUTS timeouts;
    ZeroMemory(&timeouts,sizeof(COMMTIMEOUTS));
    timeouts.ReadIntervalTimeout = READTIMEOUT;
    SetCommTimeouts(modem_port[port]->handle, &timeouts);
  }

  // fAbortOnError is the only DCB dependancy in TapiComm.
  // Can't guarentee that the SP will set this to what we expect.
  {
    DCB dcb;
    GetCommState(modem_port[port]->handle, &dcb);
    dcb.fAbortOnError = FALSE;
    SetCommState(modem_port[port]->handle, &dcb);
  }

  // EXECUTE AN OVERLAPPED READ
  ReadFile(modem_port[port]->handle,
           modem_port[port]->readbuffer,
           READBUFFERSIZE,
           &modem_port[port]->bytesread,
           &modem_port[port]->overlapped);

  return 1;
}

//===========================================================================
static void StopCom()
{
  DWORD port;

  TraceOut("StopCom()");
  // TERMINATE THE READ THREAD
  if (!modem_lineestablished)
    return;

  modem_state = STATE_SHUTDOWN;
  for (port = 0; port < PORTS; ++port)
    if (modem_port[port] && modem_port[port]->handle != INVALID_HANDLE_VALUE)
      SetEvent(modem_port[port]->overlapped.hEvent);
  WaitForSingleObject(modem_readthread,INFINITE);
  modem_readthread = NULL;

  // CLOSE AND FREE ALL PORTS
  for (port = 0; port < PORTS; ++port) {
    if (modem_event[port]) {
      CloseHandle(modem_event[port]);
      modem_event[port] = NULL;
    }
    if (modem_port[port]) {
      if (modem_port[port]->handle != INVALID_HANDLE_VALUE)
        CloseHandle(modem_port[port]->handle);
      if (modem_port[port]->partialmessage)
        FREE(modem_port[port]->partialmessage);
      FREE(modem_port[port]);
      modem_port[port] = NULL;
    }
  }
  modem_lineconfirmed = 0;
  modem_lineestablished = 0;
}

//===========================================================================
static BOOL StartCom()
{
  // Very first, make sure this isn't a duplicated message.
  // A CALLSTATE message can be sent whenever there is a
  // change to the capabilities of a line, meaning that it is
  // possible to receive multiple CONNECTED messages per call.
  // The CONNECTED CALLSTATE message is the only one in TapiComm
  // where it would cause problems if it where sent more
  // than once.

  TraceOut("StartCom()");
  if (modem_lineestablished)
    return 1;

  modem_critsect.Enter();
  // INITIALIZE THE SERIAL PORTS
  for (DWORD loop = 0; loop < PORTS; ++loop)
    if (!InitializePort(loop)) {
      // Free the previously allocated port memory
      SetLastError(SNET_ERROR_NOT_ENOUGH_MEMORY);
      SetEvent(modem_TAPIHangupEvent);
      return 0;
    }

  // CREATE A THREAD TO PROCESS INCOMING PACKETS
  ASSERT(!modem_readthread);
  if (!modem_readthread) {
    unsigned threadid;
    modem_readthread = (HANDLE)_beginthreadex(NULL,
                                           0,
                                           ThreadProc,
                                           NULL,
                                           0,
                                           &threadid);
    ASSERT(modem_readthread);
    if (!modem_readthread) {
      SetEvent(modem_TAPIHangupEvent);
      SetLastError(SNET_ERROR_MAX_THRDS_REACHED);
      return 0;
    }
    SetThreadPriority(modem_readthread,THREAD_PRIORITY_HIGHEST);
  }
  modem_critsect.Leave();

  modem_lineestablished = 1;
  SendDataMessage(SYS_QUERYLINE,0xFF,NULL,0);
  DWORD starttime = GetTickCount();
  while (!modem_lineconfirmed && (GetTickCount() - starttime < MAX_CONNECT_DIFFERENTIAL))
    Sleep(10);
  if (!modem_lineconfirmed)
    return 0;

  // FIND AN UNUSED NETWORK ID
  TraceOut("Finding ID");
  if (!FindNetworkId()) {
    TraceOut("Finding ID: Failed!");
    SetEvent(modem_TAPIHangupEvent);
    SetLastError(SNET_ERROR_TOO_MANY_NAMES);
    return 0;
  }

  // SEND OUT A QUERY FOR ACTIVE GAMES
  TraceOut("Sending SYS_QUERYGAME");
  SendDataMessage(SYS_QUERYGAME,0xFF,NULL,0);
  return 1;
}

//===========================================================================
static BOOL CALLBACK DevFillDeviceList(LPLINEDEVCAPS lpLineDevCaps, DWORD dwDevID,
                                       LPARAM appData)
{
  SNETSPI_DEVICELISTPTR *devicelist = (SNETSPI_DEVICELISTPTR*)appData;
  SNETSPI_DEVICELIST device;

  strncpy(device.devicename,
          ((LPCTSTR)lpLineDevCaps) + lpLineDevCaps->dwLineNameOffset,
          SNETSPI_MAXSTRINGLENGTH);
  device.deviceid = dwDevID;
  LISTADD(devicelist, &device);
  return 1;
}

//===========================================================================
static void EnumerateDevices(DEVENUMPROC func, LPARAM appData)
{
  LPLINEDEVCAPS lpLineDevCaps = NULL;
  DWORD dwTAPIVersion;
  DWORD i;
  BOOL bVal;

  ASSERT(func);
  if (!func) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return;
  }

	for (i = 0; i < g_dwNumDevices; i++) {
    // Check that we can talk to this TAPI version
    bVal = GetDeviceAPIVersion(i, &dwTAPIVersion);
    if (!bVal)
      continue;

    lpLineDevCaps = GetDeviceCaps(i, dwTAPIVersion);
    if (!lpLineDevCaps)
      continue;
		if (func && !func(lpLineDevCaps, i, appData))
		  break;
    if (lpLineDevCaps)
      LocalFree(lpLineDevCaps);
	}
}

//===========================================================================
static void EnumerateLocations(LOCENUMPROC func, LPARAM appData)
{
  LINETRANSLATECAPS tmpTranslateCaps;
  LPLINETRANSLATECAPS lpTranslateCaps = NULL;
  long lReturn;
  DWORD dwCounter;
  LPLINELOCATIONENTRY lpLocationEntry = NULL;

  // First, get the TRANSLATECAPS
  ZeroMemory(&tmpTranslateCaps, sizeof(tmpTranslateCaps));
  tmpTranslateCaps.dwTotalSize = sizeof(tmpTranslateCaps);
  lReturn = lineGetTranslateCaps(g_hLineApp, TAPI_CURRENT_VERSION,
                                 &tmpTranslateCaps);
  ASSERT(!(lReturn < 0));
  if(lReturn < 0)
    return;
	lpTranslateCaps = (LPLINETRANSLATECAPS)
                      LocalAlloc(0, tmpTranslateCaps.dwNeededSize);
  ASSERT(lpTranslateCaps);
  if(!lpTranslateCaps)
    return;
  ZeroMemory(lpTranslateCaps, tmpTranslateCaps.dwNeededSize);
  lpTranslateCaps->dwTotalSize = tmpTranslateCaps.dwNeededSize;
  lReturn = lineGetTranslateCaps(g_hLineApp, TAPI_CURRENT_VERSION,
                                 lpTranslateCaps);
  ASSERT(!(lReturn < 0));
  if(lReturn < 0) {
    if (lpTranslateCaps)
      LocalFree(lpTranslateCaps);
    return;
  }

  // Find the location information in the TRANSLATECAPS
  lpLocationEntry = (LPLINELOCATIONENTRY)
      (((LPBYTE) lpTranslateCaps) + lpTranslateCaps->dwLocationListOffset);

  // enumerate all the locations
  for (dwCounter = 0; dwCounter < lpTranslateCaps->dwNumLocations;
       dwCounter++) {
    if (func && !func(lpTranslateCaps, lpLocationEntry + dwCounter, appData))
      break;
  }
}

//===========================================================================
static void EnumerateCountries(DWORD dwCountryID, COUNTRYENUMPROC func,
                               LPARAM appData)
{
  LINECOUNTRYLIST tmpLineCountryList;
  LPLINECOUNTRYLIST lpLineCountryList = NULL;
  DWORD dwSizeofCountryList = sizeof(LINECOUNTRYLIST);
  long lReturn;
  LPLINECOUNTRYENTRY lpLineCountryEntries = NULL;
  DWORD dwCountry;

  // Get the country information stored in TAPI
  ZeroMemory(&tmpLineCountryList, sizeof(tmpLineCountryList));
  tmpLineCountryList.dwTotalSize = sizeof(tmpLineCountryList);
  lReturn = lineGetCountry (dwCountryID, TAPI_CURRENT_VERSION,
                            &tmpLineCountryList);
  ASSERT(!(lReturn < 0));
  if(lReturn < 0)
    return;
	lpLineCountryList = (LPLINECOUNTRYLIST)
                      LocalAlloc(0, tmpLineCountryList.dwNeededSize);
  ASSERT(lpLineCountryList);
  if(!lpLineCountryList)
    return;
  ZeroMemory(lpLineCountryList, tmpLineCountryList.dwNeededSize);
  lpLineCountryList->dwTotalSize = tmpLineCountryList.dwNeededSize;
  lReturn = lineGetCountry (dwCountryID, TAPI_CURRENT_VERSION,
                            lpLineCountryList);
  ASSERT(!(lReturn < 0));
  if(lReturn < 0) {
    if (lpLineCountryList)
      LocalFree(lpLineCountryList);
    return;
  }

  lpLineCountryEntries = (LPLINECOUNTRYENTRY)(((LPBYTE)lpLineCountryList) + 
                         lpLineCountryList->dwCountryListOffset);

  // Now enumerate through all the countries
  for (dwCountry = 0; dwCountry < lpLineCountryList->dwNumCountries;
       dwCountry++) {
    if (func && !func(lpLineCountryList, lpLineCountryEntries + dwCountry,
                      appData))
      break;
  }
}

/****************************************************************************
*
*  SERVICE PROVIDER INTERFACE FUNCTIONS
*
***/

//===========================================================================
BOOL CALLBACK ModemCompareNetAddresses (SNETADDRPTR  addr1,
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
BOOL CALLBACK ModemDestroy() {

  TraceOut("ModemDestroy()");
  // STOP ADVERTISING GAMES
  ModemStopAdvertisingGame();

  // KILL ALL TAPI RELATED THREADS
  ShutdownTAPI();

  // FREE ALL MESSAGES
  LISTCLEAR(&modem_messagehead);

  // FREE THE GAME LIST
  if (modem_gamelist) {
    FREE(modem_gamelist);
    modem_gamelist = NULL;
  }
  return 1;
}

//===========================================================================
BOOL CALLBACK ModemFree (SNETADDRPTR addr,
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
BOOL CALLBACK ModemFreeExternalMessage (LPCSTR senderpath,
                                        LPCSTR sendername,
                                        LPCSTR mesage) {
  SetLastError(SNET_ERROR_INVALID_PARAMETER);
  return 0;
}

//===========================================================================
BOOL CALLBACK ModemGetGameInfo (DWORD             gameid,
                                LPCSTR            gamename,
                                LPCSTR            gamepassword,
                                SNETSPI_GAMELIST *gameinfo) {
  LONG lResult;

  if (!(gamename && gameinfo && (gameid || *gamename))) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }
  if (gameinfo)
    ZeroMemory(gameinfo,sizeof(SNETSPI_GAMELIST));

  if (!modem_lineestablished) {
    modem_networkidlocked = 0;
    if (!MakeCall(gamename)) {
      SetEvent(modem_TAPIHangupEvent);
      SetLastError(SNET_ERROR_BAD_PROVIDER);
      return 0;
    }

    lResult = WaitForCallState(LINECALLSTATE_CONNECTED, INFINITE);
    if (lResult) {
      SetLastError(modem_callstatus);
      return 0;
    }

    // Wait up to FINDGAMETIMEOUT milliseconds for a game
    DWORD starttime = GetTickCount();
    while (!modem_gamelist && (GetTickCount() - starttime < FINDGAMETIMEOUT))
      Sleep(10);
  }

  if (modem_versionmismatch) {
    SetLastError(SNET_ERROR_VERSION_MISMATCH);
    return 0;
  }

  // IF THE GAME IN THE GAME LIST MATCHES THE QUERY PARAMETERS, RETURN
  // ITS INFORMATION
  modem_critsect.Enter();
  if (modem_gamelist)
    CopyMemory(gameinfo,modem_gamelist,sizeof(SNETSPI_GAMELIST));
  modem_critsect.Leave();

  if (!gameinfo->gameid) {
    SetEvent(modem_TAPIHangupEvent);
    SetLastError(SNET_ERROR_GAME_NOT_FOUND);
    return 0;
  }
  return 1;
}

//===========================================================================
BOOL CALLBACK ModemGetPerformanceData (DWORD          counterid,
                                        DWORD         *countervalue,
                                        LARGE_INTEGER *measurementtime,
                                        LARGE_INTEGER *measurementfreq) {
  return PerfGetPerformanceData(counterid,
                                countervalue,
                                measurementtime,
                                measurementfreq);
}

//===========================================================================
BOOL CALLBACK ModemInitialize (SNETPROGRAMDATAPTR programdata,
                               SNETPLAYERDATAPTR  playerdata,
                               SNETUIDATAPTR      interfacedata,
                               SNETVERSIONDATAPTR versiondata,
                               HANDLE             event) {

#ifdef EXPIRATION
  SYSTEMTIME sysTime;
  GetLocalTime(&sysTime);
  if (sysTime.wYear > sg_expDate.wYear ||
      (sysTime.wYear == sg_expDate.wYear && sysTime.wMonth > sg_expDate.wMonth) ||
      (sysTime.wYear == sg_expDate.wYear && sysTime.wMonth == sg_expDate.wMonth &&
       !(sysTime.wDay < sg_expDate.wDay))) {
    char buf[256];
    LoadString(global_instance, IDS_EXPIRATION, buf, 256);
    SDrawMessageBox(buf, "Version Error", MB_ICONEXCLAMATION);
    return 0;
  }
#endif

  // SAVE THE PROGRAM AND VERSION IDS AND THE RECEIVE EVENT HANDLE
  modem_programid  = programdata->programid;
  modem_versionid  = programdata->versionid;
  modem_maxplayers = min(programdata->maxplayers,MAXPLAYERS);
  modem_recvevent  = event;

  // RESET PERFORMANCE DATA
  PerfReset();

  return StartupTAPI();
}

//===========================================================================
BOOL CALLBACK ModemInitializeDevice (DWORD              deviceid,
                                     SNETPROGRAMDATAPTR programdata,
                                     SNETPLAYERDATAPTR  playerdata,
                                     SNETUIDATAPTR      interfacedata,
                                     SNETVERSIONDATAPTR versiondata) {
  BOOL bVal;
  LPLINEDEVCAPS lpLineDevCaps;
  DWORD dwTAPIVersion;

  ASSERT(g_hLineApp && g_dwNumDevices);
  
  ASSERT(interfacedata->statuscallback);
  modem_critsect.Enter();
  modem_status = interfacedata->statuscallback;
  modem_critsect.Leave();

  // Make sure that the line is available and supports data capabilities
  bVal = GetDeviceAPIVersion(deviceid, &dwTAPIVersion);
  if (!bVal)
    return 0;

  lpLineDevCaps = GetDeviceCaps(deviceid, dwTAPIVersion);
  if (!lpLineDevCaps)
    return 0;
  if (!(lpLineDevCaps->dwMediaModes & LINEMEDIAMODE_DATAMODEM)) {
    SetLastError(SNET_ERROR_BAD_PROVIDER);
    return 0;
  }
  if (lpLineDevCaps)
    LocalFree(lpLineDevCaps);

  modem_critsect.Enter();
  g_dwDeviceID = deviceid;
  modem_critsect.Leave();
  return 1;
}

//===========================================================================
BOOL CALLBACK ModemLockDeviceList (SNETSPI_DEVICELISTPTR *devicelist) {
  ASSERT(devicelist);
  if (!devicelist) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }
  *devicelist = NULL;
  EnumerateDevices(DevFillDeviceList, (LPARAM)devicelist);
  return 1;
}

//===========================================================================
BOOL CALLBACK ModemLockGameList (DWORD                categorybits,
                                 DWORD                categorymask,
                                 SNETSPI_GAMELISTPTR *gamelist) {
  if (!gamelist) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }
  // IF WE ARE NOT CURRENTLY CONNECTED RETURN FAIL SO THAT THE
  // PROVIDER KNOWS TO BYPASS THE GAME SELECTION SCREEN AND
  // DISPLAY THE PHONEBOOK IF AVAILABLE
  if (!modem_lineestablished) {
    SetLastError(SNET_ERROR_NO_NETWORK);
    *gamelist = NULL;
    return 0;
  }
  modem_critsect.Enter();
  *gamelist = modem_gamelist;
  return 1;
}

//===========================================================================
/*
BOOL CALLBACK ModemReceive (SNETADDRPTR *addr,
                            LPVOID      *data,
                            DWORD       *databytes) {
*/
BOOL CALLBACK ModemReceive (LPVOID      *data,
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

  if (modem_messagehead) {
    modem_critsect.Enter();
    *addr      = &modem_messagehead->addr;
    *data      = &modem_messagehead->data[0];
    *databytes = modem_messagehead->bytesread-sizeof(PACKETHEADER);
    LISTFREEPTR(&modem_messagehead,modem_messagehead);
    modem_critsect.Leave();
    return 1;
  }
  else {
    SetLastError(SNET_ERROR_NO_MESSAGES_WAITING);
    return 0;
  }
}

//===========================================================================
BOOL CALLBACK ModemReceiveExternalMessage (LPCSTR *senderpath,
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
BOOL CALLBACK ModemSelectGame (DWORD               flags,
                               SNETPROGRAMDATAPTR  programdata,
                               SNETPLAYERDATAPTR   playerdata,
                               SNETUIDATAPTR       interfacedata,
                               SNETVERSIONDATAPTR  versiondata,
                               DWORD              *playerid) {
#ifdef PROVIDERUI
  return 0;

#else

  // BUILD A USER INTERFACE DATA BLOCK
  UIPARAMS uiparams;
  ZeroMemory(&uiparams,sizeof(UIPARAMS));
  uiparams.flags         = flags;
  uiparams.programdata   = programdata;
  uiparams.playerdata    = playerdata;
  uiparams.interfacedata = interfacedata;
  uiparams.versiondata   = versiondata;
  uiparams.playeridptr   = playerid;

  // DISPLAY THE DIALOG BOX
  DWORD result = (DWORD)SDlgDialogBoxParam(global_instance,
                                           "MODEM_DIALOG",
                                           interfacedata ?
                                           interfacedata->parentwindow :
                                           SDrawGetFrameWindow(),
                                           ModemDialogProc,
                                           (LPARAM)&uiparams);
  result = 
  ASSERT(!(result == -1));
  if (result == -1)
    return 0;

  if (result == IDCANCEL)
    return 0;

  // Add capabilities checking here!

  if (result == IDC_MODEMCREATE) {
    ASSERT(interfacedata && interfacedata->createcallback);
    if (!(interfacedata && interfacedata->createcallback))
      return 0;
    // return ModemCreateGame(interfacedata->createcallback);
    return 1;
  }
  return (!(result == -1));
#endif
}

//===========================================================================
BOOL CALLBACK ModemSend (DWORD        addresses,
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
  return SendDataMessage(SYS_UNUSED,targetmask,data,databytes);
}

//===========================================================================
BOOL CALLBACK ModemSendExternalMessage (LPCSTR senderpath,
                                        LPCSTR sendername,
                                        LPCSTR targetpath,
                                        LPCSTR targetname,
                                        LPCSTR message) {
  return 0;
}

//===========================================================================
BOOL CALLBACK ModemStartAdvertisingGame (LPCSTR  gamename,
                                         LPCSTR  gamepassword,
                                         LPCSTR  gamedescription,
                                         DWORD   gamemode,
                                         DWORD   gameage,
                                         DWORD   gamecategorybits,
                                         DWORD   optcategorybits,
                                         LPCVOID clientdata,
                                         DWORD   clientdatabytes) {
  TraceOut("ModemStartAdvertisingGame()");
  if (!(gamename && gamedescription)) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }

  // STOP ADVERTISING ANY GAME WE ARE CURRENTLY ADVERTISING
  if (modem_gameadvinfo)
    ModemStopAdvertisingGame();

  modem_critsect.Enter();
  // CREATE RECORDS TO ADVERTISE THE GAME
  modem_gameadvinfo = NEW(ADVREC);
  ASSERT(modem_gameadvinfo);
  if (!modem_gameadvinfo) {
    SetLastError(SNET_ERROR_NOT_ENOUGH_MEMORY);
    modem_critsect.Leave();
    return 0;
  }
  ZeroMemory(modem_gameadvinfo,sizeof(ADVREC));

  // PICK AN INITIAL ID. THIS WILL NOT BE NEGOTIABLE FOR THE RECEIVING
  // MACHINE SINCE WE NEED IT FOR SPISTARTADVERTISINGGAME(). DO NOT INIT
  // THIS TO 0 IF WE ARE  STARTING TO ADVERTIZE ON AN EXISTING
  // CONNECTION
  if (!modem_lineestablished) {
    modem_networkid = 0;
    while (!modem_networkid)
      modem_networkid = (BYTE)((PickRandomNumber() % MAXPLAYERS) + 1);
    modem_networkidlocked = 1;
  }

  // FILL IN THE ADVERTISING INFORMATION
  modem_gameadvinfo->networkid = modem_networkid;
  modem_gameadvinfo->programid = modem_programid;
  modem_gameadvinfo->versionid = modem_versionid;
  strcpy(modem_gameadvinfo->strings,gamename);
  strcpy(modem_gameadvinfo->strings+strlen(gamename)+1,gamedescription);
  modem_gameadvinfo->bytes = 2*sizeof(DWORD)+strlen(gamename)+strlen(gamedescription)+2;

  modem_critsect.Leave();

  // IF WE ARE TRYING TO TAKE OVER A GAME FROM A DROPPED SERVER,
  // WE WILL CONFIGURE THE LINE TO RECEIVE CALLS IN THE UPCOMING
  // LINESTATUS_DISCONNECT
  if (!modem_lineestablished) {
    if (!TakeCall()) {
      SetLastError(SNET_ERROR_NO_NETWORK);
      return 0;
    }
  }
  else
    SendDataMessage(SYS_GAMEINFO,0xFF,modem_gameadvinfo,modem_gameadvinfo->bytes);
  return 1;
}

//===========================================================================
BOOL CALLBACK ModemStopAdvertisingGame () {
  TraceOut("ModemStopAdvertisingGame enter");
  if (!modem_gameadvinfo) {
    SetLastError(SNET_ERROR_NOT_OWNER);
    return 0;
  }
  SendDataMessage(SYS_REMOVE,0xFF,modem_gameadvinfo,modem_gameadvinfo->bytes);
  modem_critsect.Enter();
  if (modem_gameadvinfo) {
    FREE(modem_gameadvinfo);
    modem_gameadvinfo = NULL;
  }
  modem_critsect.Leave();
  return 1;
}

//===========================================================================
BOOL CALLBACK ModemUnlockDeviceList (SNETSPI_DEVICELISTPTR devicelist) {
  if (devicelist)
    LISTCLEAR(&devicelist);
  return 1;
}

//===========================================================================
BOOL CALLBACK ModemUnlockGameList (SNETSPI_GAMELISTPTR  gamelist,
                                   DWORD               *hintnextcall) {
  modem_critsect.Leave();
  if (hintnextcall)
    *hintnextcall = 1000;
  SendDataMessage(SYS_QUERYGAME,0xFF,NULL,0);
  return 1;
}

/****************************************************************************
*
*  EXPORTED STRUCTURES
*
***/

DWORD    modem_id   = PROVIDERID;
LPCSTR   modem_desc = "Modem";
LPCSTR   modem_req  = "Two computers, each with its own modem and phone line.";
SNETCAPS modem_caps = {sizeof(SNETCAPS),     // size
#ifdef _DEBUG
                       SNET_CAPS_DEBUGONLY,
#else
                       SNET_CAPS_RETAILONLY,
#endif
                       MAXMESSAGESIZE,       // max message size
                       16,                   // max queue size,
                       MAXPLAYERS,           // max players,
                       1000,                 // bytes per second
                       250,                  // latency (ms)
                       4,                    // default turns per second
                       2};                   // default turns in transit
SNETSPI  modem_spi  = {sizeof(SNETSPI),
                       ModemCompareNetAddresses,
                       ModemDestroy,
                       ModemFree,
                       ModemFreeExternalMessage,
                       ModemGetGameInfo,
                       ModemGetPerformanceData,
                       ModemInitialize,
                       ModemInitializeDevice,
                       ModemLockDeviceList,
                       ModemLockGameList,
                       ModemReceive,
                       ModemReceiveExternalMessage,
                       ModemSelectGame,
                       ModemSend,
                       ModemSendExternalMessage,
                       ModemStartAdvertisingGame,
                       ModemStopAdvertisingGame,
                       ModemUnlockDeviceList,
                       ModemUnlockGameList};

/****************************************************************************
*
*  SPI.CPP
*  battle.net service provider interface functinos
*
*  By Michael O'Brien (10/9/96)
*
***/

#include "pch.h"

#define  OVERHEADESTIMATE  39  // PPP, IP, and UDP headers

#define  PERF_PKTSENT      0
#define  PERF_PKTRECV      1
#define  PERF_BYTESSENT    2
#define  PERF_BYTESRECV    3
#define  PERFNUM           4

#define  RECVDATATHREADS   2

typedef struct _EXTMSG {
    char     senderpath[SNETSPI_MAXSTRINGLENGTH];  // must be first field in structure
    char     sendername[SNETSPI_MAXSTRINGLENGTH];
    char     message[MAXMESSAGESIZE];
    _EXTMSG *next;
} EXTMSG, *EXTMSGPTR;

typedef struct _PACKET {
    SNETADDR  addr;        // must be first field in structure
    DWORD     packettype;  // must be immediately prior to data
    BYTE      data[MAXMESSAGESIZE];
    DWORD     databytes;
    _PACKET  *next;
} PACKET, *PACKETPTR;

typedef struct _THREAD {
    unsigned  id;
    HANDLE    handle;
    _THREAD  *next;
} THREAD, *THREADPTR;

static SOCKET              spi_datasocket        = (SOCKET)0;
static CCritSect           spi_extmsgcritsect;
static EXTMSGPTR           spi_extmsghead        = NULL;
static CCritSect           spi_gamecritsect;
static SNETSPI_GAMELISTPTR spi_gamehead          = NULL;
static CCritSect           spi_packetcritsect;
static PACKETPTR           spi_packethead        = NULL;
static DWORD               spi_perfdata[PERFNUM] = {0};
static HANDLE              spi_recvevent         = (HANDLE)0;
static BOOL                spi_shutdown          = 0;
static THREADPTR           spi_threadhead        = NULL;

static unsigned CALLBACK RecvDataThreadProc (LPVOID param);

//===========================================================================
static BOOL InitializeSockets () {

  // INITIALIZE THE RAS MANAGER
  InitRASManager();

  // INITIALIZE WINDOWS SOCKETS
  {
    WSADATA data;
    if (WSAStartup(MAKEWORD(1,1),&data)) {
      SpiDestroy();
      SetLastError(SNET_ERROR_NO_NETWORK);
      return 0;
    }
  }

  // CREATE A LISTENING SOCKET
  spi_datasocket = socket(AF_INET,SOCK_DGRAM,0);
  if (!spi_datasocket) {
    SpiDestroy();
    SetLastError(SNET_ERROR_NO_NETWORK);
    return 0;
  }

  // BIND TO THE SOCKET
  {
    sockaddr_in addr;
    ZeroMemory(&addr,sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(DATAPORT);
    if (bind(spi_datasocket,
             (const struct sockaddr *)&addr,
             sizeof(sockaddr_in))) {
      SpiDestroy();
      SetLastError(SNET_ERROR_NO_NETWORK);
      return 0;
    }
  }

  // CREATE THREADS TO READ PACKETS FROM THE DATA SOCKET
  {
    BOOL win95   = GetVersion() & 0x80000000;
    int  threads = win95 ? 1 : RECVDATATHREADS;
    for (int loop = 0; loop < threads; ++loop) {
      THREAD thread;
      thread.handle = (HANDLE)_beginthreadex((LPSECURITY_ATTRIBUTES)NULL,
                                             0,
                                             RecvDataThreadProc,
                                             NULL,
                                             0,
                                             &thread.id);
      if (thread.handle) {
        SetThreadPriority(thread.handle,THREAD_PRIORITY_HIGHEST);
        LISTADD(&spi_threadhead,&thread);
      }
      else {
        SetLastError(SNET_ERROR_NOT_ENOUGH_MEMORY);
        return 0;
      }
    }
  }

  return 1;
}

//===========================================================================
static unsigned CALLBACK RecvDataThreadProc (LPVOID param) {
  while (spi_datasocket && !spi_shutdown) {

    // ALLOCATE MEMORY FOR THE NEXT INCOMING PACKET
    PACKETPTR pkt = NEW(PACKET);

    // RECEIVE A PACKET, BLOCKING IF ONE IS NOT AVAILABLE YET.  WHEN THE
    // NETWORK DRIVER HAS INCOMING DATA ON A PORT, IT WILL COPY IT DIRECTLY
    // TO THE APPLICATION'S ADDRESS SPACE IF THE APPLICATION IS BLOCKING ON
    // A READ.  FOR THIS REASON, WE TRY TO ALWAYS HAVE AT LEAST ONE READ
    // PENDING.
    int addrsize  = sizeof(sockaddr_in);
    int bytesread = recvfrom(spi_datasocket,
                             (char *)&pkt->packettype,
                             sizeof(DWORD)+MAXMESSAGESIZE,
                             0,
                             (sockaddr *)&pkt->addr,
                             &addrsize);
    pkt->databytes = (bytesread >= sizeof(DWORD))
                       ? (DWORD)bytesread-sizeof(DWORD)
                       : 0;
    ZeroMemory(((LPBYTE)&pkt->addr)+min(8,addrsize),sizeof(SNETADDR)-min(8,addrsize));

    // SINCE WE DON'T TIME OUT ON READS, THE ONLY WAY A READ CAN FAIL IS
    // IF THE SOCKET WAS CLOSED.  IN THIS CASE, SHUT DOWN THE THREAD.
    if ((bytesread < 0) || spi_shutdown) {
      FREE(pkt);
      _endthreadex(0);
      return 0;
    }

    // ON A SUCCESSFUL READ, PROCESS THE PACKET
    ++spi_perfdata[PERF_PKTRECV];
    spi_perfdata[PERF_BYTESRECV] += bytesread+OVERHEADESTIMATE;
    switch (pkt->packettype) {

      case PKT_GAMEDATA:
        spi_packetcritsect.Enter();
        LISTADDPTREND(&spi_packethead,pkt);
        spi_packetcritsect.Leave();
        SetEvent(spi_recvevent);
        break;

      case PKT_CLIENTREQ:
        SrvProcessClientReq(&pkt->addr,
                            &pkt->data[0],
                            pkt->databytes);
        FREE(pkt);
        break;

      case PKT_SERVERPING:
        SrvProcessServerPing(&pkt->data[0],
                             pkt->databytes);
        FREE(pkt);
        break;

      default:
        FREE(pkt);
        break;

    }

  }
  _endthreadex(0);
  return 0;
}


/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
void SpiAddGame (SNETSPI_GAMELISTPTR game) {

  // CREATE A RECORD FOR THIS GAME
  SNETSPI_GAMELISTPTR newgame = NEW(SNETSPI_GAMELIST);
  if (!newgame)
    return;
  CopyMemory(newgame,game,sizeof(SNETSPI_GAMELIST));

  // IF THIS GAME IS ALREADY IN THE LINKED LIST, REMOVE IT
  spi_gamecritsect.Enter();
  SNETSPI_GAMELISTPTR *nextptr = &spi_gamehead;
  while (*nextptr)
    if (!memcmp(&(*nextptr)->owner,&newgame->owner,sizeof(sockaddr_in))) {
      SNETSPI_GAMELISTPTR curr = *nextptr;
      *nextptr = curr->next;
      FREE(curr);
    }
    else
      nextptr = &(*nextptr)->next;

  // ADD THIS GAME TO THE LINKED LIST OF GAMES
  static DWORD idsequence = 0;
  if (!(newgame->gameid = ++idsequence))
    newgame->gameid = ++idsequence;
  newgame->next = NULL;
  *nextptr      = newgame;
  spi_gamecritsect.Leave();

}

//===========================================================================
BOOL CALLBACK SpiCompareNetAddresses (SNETADDRPTR  addr1,
                                      SNETADDRPTR  addr2,
                                      DWORD       *diffmagnitude) {
  if (diffmagnitude)
    *diffmagnitude = 0;
  if (!(addr1 && addr2 && diffmagnitude)) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }

  sockaddr_in *inaddr1 = (sockaddr_in *)addr1;
  sockaddr_in *inaddr2 = (sockaddr_in *)addr2;
  if (inaddr1->sin_addr.S_un.S_un_b.s_b1 !=
      inaddr2->sin_addr.S_un.S_un_b.s_b1)
    *diffmagnitude = 4;
  else if (inaddr1->sin_addr.S_un.S_un_b.s_b2 !=
           inaddr2->sin_addr.S_un.S_un_b.s_b2)
    *diffmagnitude = 3;
  else if (inaddr1->sin_addr.S_un.S_un_b.s_b3 !=
           inaddr2->sin_addr.S_un.S_un_b.s_b3)
    *diffmagnitude = 2;
  else if (inaddr1->sin_addr.S_un.S_un_b.s_b4 !=
           inaddr2->sin_addr.S_un.S_un_b.s_b4)
    *diffmagnitude = 1;
  else
    *diffmagnitude = 0;

  return 1;
}

//===========================================================================
BOOL CALLBACK SpiDestroy () {

  // START THE SHUTDOWN PROCESS
  spi_shutdown = 1;

  // STOP ADVERTISING OUR GAME
  SpiStopAdvertisingGame();

  // DISCONNECT FROM THE BATTLENET SERVER
  SrvDestroy();

  // CLEAR THE EXTERNAL MESSAGE QUEUE
  spi_extmsgcritsect.Enter();
  LISTCLEAR(&spi_extmsghead);
  spi_extmsgcritsect.Leave();

  // SEND DATA TO THE RECEIVE THREADS TO WAKE THEM UP
  {
    sockaddr_in addr;
    ZeroMemory(&addr,sizeof(sockaddr_in));
    addr.sin_family           = AF_INET;
    addr.sin_port             = htons(DATAPORT);
    addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    char buffer[] = " ";
    SOCKET newsocket = socket(AF_INET,SOCK_DGRAM,0);
    for (int loop = 0; loop < RECVDATATHREADS; ++loop)
      sendto(newsocket,buffer,1,0,(sockaddr *)&addr,sizeof(sockaddr_in));
    closesocket(newsocket);
  }

  // WAIT FOR ALL THREADS TO TERMINATE
  while (spi_threadhead) {
    WaitForSingleObject(spi_threadhead->handle,INFINITE);
    CloseHandle(spi_threadhead->handle);
    LISTFREE(&spi_threadhead,spi_threadhead);
  }

  // CLOSE THE DATA SOCKET
  if (spi_datasocket) {
    closesocket(spi_datasocket);
    spi_datasocket = (SOCKET)0;
  }

  // TAKE CONTROL OF THE PACKET CRITICAL SECTION
  spi_packetcritsect.Enter();

  // FREE ALL UNPROCESSED PACKETS
  LISTCLEAR(&spi_packethead);

  // FREE THE GAME LIST
  LISTCLEAR(&spi_gamehead);

  // CLEAN UP WINDOWS SOCKETS
  WSACleanup();

  // DESTROY OTHER MODULES
  UiDestroy();
  CacheDestroy();

  // LEAVE THE PACKET CRITICAL SECTION
  spi_packetcritsect.Leave();

  // FINISH THE SHUTDOWN PROCESS
  spi_shutdown = 0;

  // ASK THE USER WHETHER HE WANTS TO HANG UP
  UiDisconnect();

  return 1;
}

//===========================================================================
BOOL CALLBACK SpiFree (SNETADDRPTR addr,
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
BOOL CALLBACK SpiFreeExternalMessage (LPCSTR senderpath,
                                      LPCSTR sendername,
                                      LPCSTR message) {
  if (!(senderpath && sendername && message)) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }
  FREE((LPVOID)senderpath);
  return 1;
}

//===========================================================================
BOOL CALLBACK SpiGetGameInfo (DWORD             gameid,
                              LPCSTR            gamename,
                              LPCSTR            gamepassword,
                              SNETSPI_GAMELIST *gameinfo) {
  if (gameinfo)
    ZeroMemory(gameinfo,sizeof(SNETSPI_GAMELIST));
  if (!(gamename && gameinfo && (gameid || *gamename))) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }

  // CLEAR THE EXTERNAL MESSAGE QUEUE
  spi_extmsgcritsect.Enter();
  LISTCLEAR(&spi_extmsghead);
  spi_extmsgcritsect.Leave();

  // ENTER THE CRITICAL SECTION
  spi_gamecritsect.Enter();

  // SEARCH FOR A GAME IN THE GAME LIST MATCHING THE QUERY PARAMETERS
  SNETSPI_GAMELISTPTR found  = NULL;
  BOOL                remove = 0;
  {
    SNETSPI_GAMELISTPTR curr = spi_gamehead;
    while (curr)
      if (((!gameid) || (gameid == curr->gameid)) &&
          ((!*gamename) || !_stricmp(gamename,curr->gamename))) {
        found = curr;
        break;
      }
      else
        curr = curr->next;
  }

  // IF WE DIDN'T FIND A MATCH, AND IF A GAME NAME BUT NO GAME ID WAS
  // GIVEN, THEN REQUEST INFORMATION ABOUT THAT GAME FROM THE SERVER
  // AND THEN SEARCH AGAIN.
  if ((!found) && (gamename && !gameid)) {
    if (!SrvIsWaitingForResponse()) {
      spi_gamecritsect.Leave();
      SrvGetGameList(gamename,gamepassword,0,0,1);
      spi_gamecritsect.Enter();
    }
    SNETSPI_GAMELISTPTR curr = spi_gamehead;
    while (curr)
      if (!_stricmp(gamename,curr->gamename)) {
        found  = curr;
        remove = 1;
        break;
      }
      else
        curr = curr->next;
  }

  // IF WE FOUND A MATCH, COPY INFORMATION ABOUT THE MATCH INTO THE
  // CALLER'S INFORMATION STRUCTURE
  if (found)
    CopyMemory(gameinfo,found,sizeof(SNETSPI_GAMELIST));
  if (remove)
    LISTFREE(&spi_gamehead,found);

  // LEAVE THE GAME CRITICAL SECTION
  spi_gamecritsect.Leave();

  if (found)
    return 1;
  else {
    SetLastError(SNET_ERROR_GAME_NOT_FOUND);
    return 0;
  }
}

//===========================================================================
BOOL CALLBACK SpiGetLocalPlayerName (LPSTR namebuffer,
                                     DWORD namechars,
                                     LPSTR descbuffer,
                                     DWORD descchars) {
  if (namebuffer && namechars)
    SrvGetLocalPlayerName(namebuffer,
                          namechars);
  if (descbuffer && descchars)
    SrvGetLocalPlayerDesc(descbuffer,
                          descchars);
  return TRUE;
}


//===========================================================================
BOOL CALLBACK SpiGetPerformanceData (DWORD          counterid,
                                     DWORD         *countervalue,
                                     LARGE_INTEGER *measurementtime,
                                     LARGE_INTEGER *measurementfreq) {
  switch (counterid) {

    case SNET_PERFID_PKTSENTONWIRE:
      *countervalue = spi_perfdata[PERF_PKTSENT];
      return 1;

    case SNET_PERFID_PKTRECVONWIRE:
      *countervalue = spi_perfdata[PERF_PKTRECV];
      return 1;

    case SNET_PERFID_BYTESSENTONWIRE:
      *countervalue = spi_perfdata[PERF_BYTESSENT];
      return 1;

    case SNET_PERFID_BYTESRECVONWIRE:
      *countervalue = spi_perfdata[PERF_BYTESRECV];
      return 1;

  }
  return 0;
}

//===========================================================================
BOOL CALLBACK SpiInitialize (SNETPROGRAMDATAPTR programdata,
                             SNETPLAYERDATAPTR  playerdata,
                             SNETUIDATAPTR      interfacedata,
                             SNETVERSIONDATAPTR versiondata,
                             HANDLE             event) {

  // SAVE THE PROGRAM AND VERSION IDS AND THE RECEIVE EVENT HANDLE
  global_programid  = programdata->programid;
  global_versionid  = programdata->versionid;
  global_maxplayers = min(programdata->maxplayers,MAXPLAYERS);
  spi_recvevent     = event;

  // RESET PERFORMANCE DATA
  ZeroMemory(&spi_perfdata[0],PERFNUM*sizeof(DWORD));

  // INITIALIZE OTHER MODULES
  CacheInitialize();
  UiInitialize(interfacedata);

  // BRING UP THE CONNECTION SCREEN
  if (!UiBeginConnect(programdata,
                      interfacedata)) {
    SrvDestroy();
    SpiDestroy();
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
  }

  // INITIALIZE SOCKETS
  BOOL  result    = 1;
  DWORD lasterror = 0;
  UiProcessWindowMessages();
  if (!InitializeSockets()) {
    UiWSockErrMessage();
    result    = 0;
    lasterror = GetLastError();
  }

  // CONNECT TO THE BATTLENET SERVER
  if (result) {
    UiProcessWindowMessages();
    if (!SrvInitialize(programdata,playerdata,versiondata)) {
      result    = 0;
      lasterror = GetLastError();
    }
  }

  // CLOSE THE CONNECTION SCREEN
  UiEndConnect(result);
  if (result) {
    if (!UiLogon(programdata,
                 playerdata,
                 interfacedata,
                 versiondata)) {
      lasterror = GetLastError();
      result = FALSE;
    }
  }

  // IF WE DIDN'T SUCCEED IN CONNECTING TO THE BATTLE.NET SERVER,
  // SHUT DOWN THE SNP
  if (!result) {
    SpiDestroy();
    SetLastError(lasterror);
    return 0;
  }

  return 1;
}

//===========================================================================
BOOL CALLBACK SpiInitializeDevice (DWORD              deviceid,
                                   SNETPROGRAMDATAPTR programdata,
                                   SNETPLAYERDATAPTR  playerdata,
                                   SNETUIDATAPTR      interfacedata,
                                   SNETVERSIONDATAPTR versiondata) {

  // WE NEVER RETURN ANY DEVICES, SO THIS FUNCTION SHOULD NEVER BE CALLED
  return 0;
}

//===========================================================================
BOOL CALLBACK SpiLockDeviceList (SNETSPI_DEVICELISTPTR *devicelist) {
  *devicelist = NULL;
  return 1;
}

//===========================================================================
BOOL CALLBACK SpiLockGameList (DWORD                categorybits,
                               DWORD                categorymask,
                               SNETSPI_GAMELISTPTR *gamelist) {
  if (!gamelist) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }

  // CLEAR THE EXTERNAL MESSAGE QUEUE
  spi_extmsgcritsect.Enter();
  LISTCLEAR(&spi_extmsghead);
  spi_extmsgcritsect.Leave();

  // GET A CURRENT LIST OF GAMES FROM THE SERVER
  if (!SrvIsWaitingForResponse()) {
    LISTCLEAR(&spi_gamehead);
    char progvers[32];
    wsprintf(progvers,"%08x%08x",global_programid,global_versionid);
    char lastgame[SNETSPI_MAXSTRINGLENGTH] = "";
    SRegLoadString("Recent Games",progvers,SREG_FLAG_BATTLENET,lastgame,SNETSPI_MAXSTRINGLENGTH);
    if (lastgame[0])
      SrvGetGameList(lastgame,NULL,categorybits,categorymask,1);
    SrvGetGameList(NULL,NULL,categorybits,categorymask,spi_gamehead ? 12 : 13);
  }

  // ENTER THE GAME CRITICAL SECTION
  spi_gamecritsect.Enter();

  // UPDATE THE LATENCY FOR EACH GAME
  {
    SNETSPI_GAMELISTPTR curr = spi_gamehead;
    while (curr) {
      SrvGetLatency(&curr->owner,&curr->ownerlatency);
      curr = curr->next;
    }
  }

  // RETURN THE GAME LIST TO THE CALLER
  *gamelist = spi_gamehead;

  return 1;
}

//===========================================================================
void SpiQueueExternalMessage (LPCSTR senderpath,
                              LPCSTR sendername,
                              LPCSTR message) {
  spi_extmsgcritsect.Enter();
  EXTMSGPTR newptr = NEW(EXTMSG);
  if (newptr) {
    strncpy(newptr->senderpath,senderpath,SNETSPI_MAXSTRINGLENGTH);
    strncpy(newptr->sendername,sendername,SNETSPI_MAXSTRINGLENGTH);
    strncpy(newptr->message,message,MAXMESSAGESIZE);
    newptr->senderpath[SNETSPI_MAXSTRINGLENGTH-1] = 0;
    newptr->sendername[SNETSPI_MAXSTRINGLENGTH-1] = 0;
    newptr->message[MAXMESSAGESIZE-1] = 0;
    LISTADDPTREND(&spi_extmsghead,newptr);
  }
  spi_extmsgcritsect.Leave();
  SetEvent(spi_recvevent);
}

//===========================================================================
BOOL CALLBACK SpiReceive (SNETADDRPTR *addr,
                          LPVOID      *data,
                          DWORD       *databytes) {
  if (addr)
    *addr = NULL;
  if (data)
    *data = NULL;
  if (databytes)
    *databytes = NULL;
  if (!(addr && data && databytes && spi_datasocket)) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }

  // IF THERE IS A PACKET QUEUED, REMOVE IT FROM THE QUEUE AND RETURN
  // POINTERS TO THE CALLER.  NOTE THAT WE UNLINK THE PACKET BUT DON'T
  // FREE IT FROM MEMORY; IT IS THE CALLER'S RESPONSIBILITY TO CALL
  // OUR FREE FUNCTION WHEN IT IS DONE WITH THE PACKET.
  if (spi_packethead) {
    spi_packetcritsect.Enter();
    *addr      = &spi_packethead->addr;
    *data      = spi_packethead->data;
    *databytes = spi_packethead->databytes;
    spi_packethead = spi_packethead->next;
    spi_packetcritsect.Leave();
    return 1;
  }
  else {
    SetLastError(SNET_ERROR_NO_MESSAGES_WAITING);
    return 0;
  }
}

//===========================================================================
BOOL CALLBACK SpiReceiveExternalMessage (LPCSTR *senderpath,
                                         LPCSTR *sendername,
                                         LPCSTR *message) {
  if (senderpath)
    *senderpath = NULL;
  if (sendername)
    *sendername = NULL;
  if (message)
    *message = NULL;
  if (!(senderpath && sendername && message)) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }

  // IF THERE IS A MESSAGE QUEUED, REMOVE IT FROM THE QUEUE AND RETURN
  // POINTERS TO THE CALLER.  NOTE THAT WE UNLINK THE MESSAGE BUT DON'T
  // FREE IT FROM MEMORY; IT IS THE CALLER'S RESPONSIBILITY TO CALL
  // OUR FREE FUNCTION WHEN IT IS DONE WITH THE PACKET.
  if (spi_extmsghead) {
    spi_extmsgcritsect.Enter();
    *senderpath = spi_extmsghead->senderpath;
    *sendername = spi_extmsghead->sendername;
    *message    = spi_extmsghead->message;
    spi_extmsghead = spi_extmsghead->next;
    spi_extmsgcritsect.Leave();
    return 1;
  }
  else {
    SetLastError(SNET_ERROR_NO_MESSAGES_WAITING);
    return 0;
  }
}

//===========================================================================
BOOL CALLBACK SpiSelectGame (DWORD               flags,
                             SNETPROGRAMDATAPTR  programdata,
                             SNETPLAYERDATAPTR   playerdata,
                             SNETUIDATAPTR       interfacedata,
                             SNETVERSIONDATAPTR  versiondata,
                             DWORD              *playerid) {
  return UiSelectGame(flags,
                      programdata,
                      playerdata,
                      interfacedata,
                      versiondata,
                      playerid);
}

//===========================================================================
BOOL CALLBACK SpiSend (DWORD        addresses,
                       SNETADDRPTR *addrlist,
                       LPVOID       data,
                       DWORD        databytes) {
  if (!(addresses &&
        addrlist && data && databytes && (databytes <= MAXMESSAGESIZE) &&
        spi_datasocket)) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }

  // CREATE A FULLY FORMED PACKET
  PACKET pkt;
  pkt.packettype = PKT_GAMEDATA;
  CopyMemory(&pkt.data[0],data,databytes);

  // SEND IT TO EACH TARGET ADDRESS
  while (addresses--) {
    sendto(spi_datasocket,
           (const char *)&pkt.packettype,
           sizeof(DWORD)+databytes,
           0,
           (const sockaddr *)*(addrlist+addresses),
           sizeof(sockaddr_in));
    ++spi_perfdata[PERF_PKTSENT];
    spi_perfdata[PERF_BYTESSENT] += sizeof(DWORD)+databytes+OVERHEADESTIMATE;
  }

  return 1;
}

//===========================================================================
BOOL CALLBACK SpiSendExternalMessage (LPCSTR senderpath,
                                      LPCSTR sendername,
                                      LPCSTR targetpath,
                                      LPCSTR targetname,
                                      LPCSTR message) {
  if ((!*targetpath) && (!*targetname))
    SrvSendChatString(message);
  return 1;
}

//===========================================================================
BOOL SpiSendSpecial (SNETADDRPTR addr,
                     DWORD       packettype,
                     LPVOID      data,
                     DWORD       databytes) {
  if (!(addr && data && databytes && (databytes <= MAXMESSAGESIZE) &&
        spi_datasocket)) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }

  // CREATE A FULLY FORMED PACKET
  PACKET pkt;
  pkt.packettype = packettype;
  CopyMemory(&pkt.data[0],data,databytes);

  // SEND IT TO THE TARGET ADDRESS
  sendto(spi_datasocket,
         (const char *)&pkt.packettype,
         sizeof(DWORD)+databytes,
         0,
         (const sockaddr *)addr,
         sizeof(sockaddr_in));
  ++spi_perfdata[PERF_PKTSENT];
  spi_perfdata[PERF_BYTESSENT] += sizeof(DWORD)+databytes+OVERHEADESTIMATE;

  return 1;
}

//===========================================================================
BOOL CALLBACK SpiStartAdvertisingGame (LPCSTR  gamename,
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

  // IF WE'RE NOT CONNECTED TO A BATTLENET SERVER, RETURN FAILURE
  if (!SrvIsConnected()) {
    SetLastError(SNET_ERROR_NOT_CONNECTED);
    return 0;
  }

  // SEND THE GAME INFORMATION TO THE BATTLENET SERVER
  if (!SrvStartAdvertisingGame(gamename,
                               gamepassword,
                               gamedescription,
                               gamemode,
                               gameage,
                               gamecategorybits,
                               optcategorybits)) {
    SetLastError(SNET_ERROR_ALREADY_EXISTS);
    return 0;
  }

  return 1;
}

//===========================================================================
BOOL CALLBACK SpiStopAdvertisingGame () {

  // IF WE'RE NOT CONNECTED TO A BATTLENET SERVER, RETURN FAILURE
  if (!SrvIsConnected()) {
    SetLastError(SNET_ERROR_NOT_CONNECTED);
    return 0;
  }

  // TELL THE BATTLENET SERVER TO REMOVE THE GAME
  SrvStopAdvertisingGame();

  return 1;
}

//===========================================================================
BOOL CALLBACK SpiUnlockDeviceList (SNETSPI_DEVICELISTPTR devicelist) {
  return 1;
}

//===========================================================================
BOOL CALLBACK SpiUnlockGameList (SNETSPI_GAMELISTPTR  gamelist,
                                 DWORD               *hintnextcall) {

  // LEAVE THE GAME CRITICAL SECTION
  spi_gamecritsect.Leave();

  // TELL THE CALLER TO CALL BACK IN A SECOND TO GET AN UPDATED LIST
  // OF LATENCIES
  if (hintnextcall)
    *hintnextcall = 1000;

  return 1;
}

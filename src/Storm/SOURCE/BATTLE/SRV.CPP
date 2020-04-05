/****************************************************************************
*
*  SRV.CPP
*  battle.net server communication functions
*
*  By Michael O'Brien (10/9/96)
*
***/

#include "pch.h"

#ifdef _X86_
#define  PLATFORMID                  'IX86'
#endif

#define  MAXLATENCY                  2000
#define  PINGFREQUENCY               30000
#define  SERVERPORT                  6112
#define  CONNTYPE_CLIENT             0x01
#define  CONNTYPE_FILE               0x02
#define  SERVERBUFFERSIZE            16384
#define  SERVERVERSION               1
#define  CONFIGREGKEY                "Configuration"
#define  CONFIGREGVALUE_PINGLIST     "Ping List"
#define  CONFIGREGVALUE_SERVERLIST   "Server List"
#define  CONFIGREGVALUE_SERVERVER    "Server Version"
#define  CONFIGREGVALUE_REGAUTH      "Registration Authority"
#define  CONFIGREGVALUE_REGVER       "Registration Version"
#define  CONFIGREGVALUE_CLIENTID     "Client ID"
#define  CONFIGREGVALUE_CLIENTCHECK  "Client Token"
#define  COOKIEREGKEY                "Cookies"
#define  PATCHREGKEY                 "Patch"
#define  PATCHREGVALUE_LAUNCHER      "Launcher"
#define  PATCHREGVALUE_SRCDATA       "SrcData"
#define  PATCHREGVALUE_DSTDATA       "DstData"
#define  PATCHREGVALUE_PATCHES       "Patches"

#define  CHAT_DISPLAYUSER            1
#define  CHAT_ADDUSER                2
#define  CHAT_REMOVEUSER             3
#define  CHAT_WHISPER                4
#define  CHAT_TALK                   5
#define  CHAT_BROADCAST              6
#define  CHAT_JOINCHANNEL            7
#define  CHAT_USERFLAGS              9
#define  CHAT_WHISPERSENT            10
#define  CHAT_CHANNELISFULL          13
#define  CHAT_CHANNELDOESNOTEXIST    14
#define  CHAT_CHANNELISRESTRICTED    15
#define  CHAT_INFORMATION            18
#define  CHAT_ERROR                  19
#define  CHAT_SQUELCH                21
#define  CHAT_UNSQUELCH              22

#define  CLI_PING                    0
#define  CLI_PINGRESPONSE            1

#define  DOWNLOAD_VERSIONING         1
#define  DOWNLOAD_PATCH              2
#define  DOWNLOAD_AD                 3

#define  JCF_DEFAULTCHANNEL          0x00000001
#define  JCF_JOINALWAYS              0x00000002

#define  SAVE_CACHE                  0x00000001
#define  SAVE_DISK                   0x00000002

#define  SID_NULL                    0x00
#define  SID_STARTADV                0x01
#define  SID_STOPADV                 0x02
#define  SID_GETADVLIST              0x03
#define  SID_SERVERLIST              0x04
#define  SID_CLIENTID                0x05
#define  SID_STARTVERSIONING         0x06
#define  SID_REPORTVERSION           0x07
#define  SID_STARTADVEX              0x08
#define  SID_GETADVLISTEX            0x09
#define  SID_ENTERCHAT               0x0A
#define  SID_GETCHANNELLIST          0x0B
#define  SID_JOINCHANNEL             0x0C
#define  SID_CHATCOMMAND             0x0E
#define  SID_CHATEVENT               0x0F
#define  SID_LEAVECHAT               0x10
#define  SID_LOCALEINFO              0x12
#define  SID_FLOODDETECTED           0x13
#define  SID_UDPPINGRESPONSE         0x14
#define  SID_CHECKAD                 0x15
#define  SID_CLICKAD                 0x16
#define  SID_QUERYMEM                0x17
#define  SID_QUERYREG                0x18
#define  SID_MESSAGEBOX              0x19
#define  SID_BROADCAST               0x20
#define  SID_DISPLAYAD               0x21
#define  SID_NOTIFYJOIN              0x22
#define  SID_SETCOOKIE               0x23
#define  SID_GETCOOKIE               0x24
#define  SID_PING                    0x25
#define  SERVERIDS                   0x26

#define  VER_RESULT_BADVERSION       0
#define  VER_RESULT_UPGRADEREQUIRED  1
#define  VER_RESULT_CURRENT          2

#define  WAIT_FLAG_INFINITE          0x00000001
#define  WAIT_FLAG_NOMESSAGELOOP     0x00000004

typedef struct _CONNECTREC {
    SOCKET       socket;
    HANDLE       thread;
    _CONNECTREC *next;
} CONNECTREC, *CONNECTPTR;

typedef struct _FILEREQ {
    DWORD    requestbytes;
    DWORD    platformid;
    DWORD    programid;
    DWORD    fileid;
    DWORD    filedatatype;
    FILETIME filetime;
    char     filename[MAX_PATH];
    // the following fields are not sent to the server, and should not
    // be reflected in requestbytes
    char     url[MAX_PATH];
    DWORD    downloadtype;
    DWORD    savetype;
} FILEREQ, *FILEREQPTR;

typedef struct _FILERESPONSE {
    DWORD    headerbytes;
    DWORD    filebytes;
    DWORD    fileid;
    DWORD    filedatatype;
    FILETIME filetime;
    char     filename[MAX_PATH];
} FILERESPONSE, *FILERESPONSEPTR;

typedef struct _PINGREC {
    sockaddr_in  addr;
    char         username[SNETSPI_MAXSTRINGLENGTH];
    DWORD        latency;
    DWORD        lastpingtime;
    DWORD        lastresponsetime;
    _PINGREC    *next;
} PINGREC, *PINGPTR;

typedef struct _SRVMSG {
    BYTE signature;
    BYTE id;
    WORD bytes;
} SRVMSG, *SRVMSGPTR;

typedef struct _UINOTIFICATION {
    DWORD            notifycode;
    LPVOID           buffer;
    DWORD            parambytes;
    _UINOTIFICATION *next;
} UINOTIFICATION, *UINOTIFICATIONPTR;

static DWORD             srv_adnumber                  = 0;
static DWORD             srv_addisplaytime             = 0;
static char              srv_argstring[256]            = "";
static DWORD             srv_authenticated             = 0;
static BOOL              srv_cancelwait                = 0;
static CCritSect         srv_connectcritsect;
static BOOL              srv_connected                 = 0;
static CONNECTPTR        srv_connecthead               = NULL;
static LONG              srv_connectthreads            = 0;
static LONG              srv_downloadthreads           = 0;
static BOOL              srv_inchat                    = 0;
static HANDLE            srv_keepalivethread           = (HANDLE)0;
static CCritSect         srv_patchcritsect;
static char              srv_patchfiles[256]           = "\0";
static DWORD             srv_patchhighcomplete         = 0;
static DWORD             srv_patchpercent              = 0;
static CCritSect         srv_pingcritsect;
static PINGPTR           srv_pinghead                  = NULL;
static BOOL              srv_pingsuccess               = 0;
static LONG              srv_pingthreads               = 0;
static BOOL              srv_responded[SERVERIDS]      = {0};
static SOCKET            srv_serversocket              = (SOCKET)0;
static HANDLE            srv_serverthread              = (HANDLE)0;
static BOOL              srv_shutdown                  = 0;
static HANDLE            srv_shutdownevent             = 0;
static BOOL              srv_startadvsuccess           = 0;
static DWORD             srv_udppingdata               = 0;
static CCritSect         srv_uinotificationcritsect;
static UINOTIFICATIONPTR srv_uinotificationhead        = NULL;
static LPVOID            srv_uinotificationhold        = NULL;
static char              srv_userdesc[MAXSTRINGLENGTH] = "";
static char              srv_username[MAXSTRINGLENGTH] = "";
static char              srv_versionfile[MAX_PATH]     = "";
static HANDLE            srv_waitevent                 = (HANDLE)0;
static LONG              srv_waiting                   = 0;

static void DeleteConnectThread (SOCKET socket);
static unsigned CALLBACK PingThreadProc (LPVOID param);
static void ProcessFile (DWORD     downloadtype,
                         DWORD     savetype,
                         DWORD     fileid,
                         DWORD     filedatatype,
                         FILETIME *filetime,
                         LPCSTR    filename,
                         LPCSTR    url,
                         LPVOID    data,
                         DWORD     databytes);
static void QueueUiNotification (DWORD   notifycode,
                                 LPVOID  param,
                                 DWORD   parambytes,
                                 LPCSTR *string1ptr,
                                 LPCSTR *string2ptr);
static void RemovePingUser (LPCSTR username, sockaddr_in *addr);
static void RequestFile (DWORD     downloadtype,
                         DWORD     savetype,
                         DWORD     fileid,
                         DWORD     filedatatype,
                         FILETIME *filetime,
                         LPCSTR    filename,
                         LPCSTR    url);
static void SaveFile (LPCSTR filename,
                      LPVOID buffer,
                      DWORD  bytes);
static BOOL SendServerMessage (BYTE id, LPVOID data, DWORD databytes);
static void UpdatePatchPercent (LPCSTR filename,
                                DWORD  offset,
                                DWORD  totalsize);
static BOOL WaitOnce (DWORD flags);

//===========================================================================
static void AddConnectThread (SOCKET socket) {
  CONNECTPTR newptr = NEW(CONNECTREC);
  if (!newptr)
    return;
  DuplicateHandle(GetCurrentProcess(),
                  GetCurrentThread(),
                  GetCurrentProcess(),
                  &newptr->thread,
                  0,
                  0,
                  DUPLICATE_SAME_ACCESS);
  newptr->socket = socket;
  srv_connectcritsect.Enter();
  LISTADDPTR(&srv_connecthead,newptr);
  srv_connectcritsect.Leave();
}

//===========================================================================
static void AddPingUser (LPCSTR username, sockaddr_in *addr) {
  RemovePingUser(username,addr);

  // NORMALIZE THE ADDRESS TO POINT TO THE DATA PORT
  sockaddr_in dataportaddr;
  CopyMemory(&dataportaddr,addr,sizeof(sockaddr_in));
  dataportaddr.sin_port = htons(DATAPORT);
  ZeroMemory(&dataportaddr.sin_zero[0],8);

  // CREATE A NEW RECORD FOR THIS USER
  PINGPTR newptr = NEW(PINGREC);
  if (newptr)
    ZeroMemory(newptr,sizeof(PINGREC));
  else
    return;
  CopyMemory(&newptr->addr,&dataportaddr,sizeof(sockaddr_in));
  if (username) {
    strncpy(newptr->username,username,SNETSPI_MAXSTRINGLENGTH);
    newptr->username[SNETSPI_MAXSTRINGLENGTH-1] = 0;
  }
  newptr->lastpingtime = GetTickCount();

  // ADD IT TO THE LINKED LIST OF PING USERS
  srv_pingcritsect.Enter();
  newptr->next = srv_pinghead;
  srv_pinghead = newptr;
  srv_pingcritsect.Leave();

  // SEND AN INITIAL PING MESSAGE TO THE USER
  DWORD request = CLI_PING;
  SpiSendSpecial((SNETADDRPTR)&dataportaddr,
                 PKT_CLIENTREQ,
                 &request,
                 sizeof(DWORD));

}

//===========================================================================
static BOOL CheckVersion (DWORD *revisionid, DWORD *checkvalue, LPSTR comment) {
  if (revisionid)
    *revisionid = 0;
  if (checkvalue)
    *checkvalue = 0;
  if (comment)
    *comment = 0;

  HSARCHIVE archive           = (HSARCHIVE)0;
  LPVOID    buffer            = NULL;
  char      dllname[MAX_PATH] = "";
  HSFILE    file              = (HSFILE)0;
  HINSTANCE lib               = (HINSTANCE)0;
  BOOL      success           = 0;

  // OPEN THE MOPAQ ARCHIVE THAT WE JUST DOWNLOADED
  if (!SFileOpenArchive(srv_versionfile,0,0,&archive))
    return 0;

  __try {

    // VERIFY THE SIGNATURE ON THE ARCHIVE FILE
    DWORD authtype;
    SFileAuthenticateArchive(archive,&authtype);
    if ((authtype != SFILE_AUTH_UNABLETOAUTHENTICATE) &&
        (authtype  < SFILE_AUTH_FIRSTAUTHENTIC))
      __leave;

    // EXTRACT THE EMBEDDED DLL
    strcpy(dllname,srv_versionfile);
    if (strchr(dllname,'.'))
      *strchr(dllname,'.') = 0;
    strcat(dllname,".dll");
    if (!SFileOpenFileEx(archive,dllname,0,&file))
      __leave;
    DWORD size = SFileGetFileSize(file);
    buffer = ALLOC(size);
    if (!buffer)
      __leave;
    SFileReadFile(file,buffer,size,NULL,NULL);
    {
      HANDLE outfile = CreateFile(dllname,
                                  GENERIC_WRITE,
                                  0,
                                  NULL,
                                  CREATE_ALWAYS,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);
      if (outfile != INVALID_HANDLE_VALUE) {
        DWORD byteswritten;
        WriteFile(outfile,buffer,size,&byteswritten,NULL);
        CloseHandle(outfile);
      }
      else
        __leave;
    }

    // BIND TO THE DLL
    lib = LoadLibrary(dllname);
    if (!lib)
      __leave;
    BOOL (APIENTRY *checkrevision)(LPCSTR,LPCSTR,LPCSTR,LPCSTR,DWORD *,DWORD *,LPSTR);
    *(LPVOID *)&checkrevision = GetProcAddress(lib,"CheckRevision");
    if (!checkrevision)
      __leave;

    // CALL THE REVISION CHECKING FUNCTION
    char appfilename[MAX_PATH]      = "";
    char stormfilename[MAX_PATH]    = "";
    char providerfilename[MAX_PATH] = "";
    GetModuleFileName(GetModuleHandle(NULL),appfilename,MAX_PATH);
    GetModuleFileName((HMODULE)global_hinstance,providerfilename,MAX_PATH);
    {
      strcpy(stormfilename,providerfilename);
      LPSTR curr = stormfilename;
      while (strchr(curr,':'))
        curr = strchr(curr,':')+1;
      while (strchr(curr,'\\'))
        curr = strchr(curr,'\\')+1;
      strcpy(curr,"storm.dll");
    }
    success = checkrevision(appfilename,
                            stormfilename,
                            providerfilename,
                            srv_argstring,
                            revisionid,
                            checkvalue,
                            comment);

  }
  __finally {
    if (lib)
      FreeLibrary(lib);
    if (buffer)
      FREE(buffer);
    if (file)
      SFileCloseFile(file);
    if (archive)
      SFileCloseArchive(archive);
    DeleteFile(srv_versionfile);
    if (dllname[0])
      DeleteFile(dllname);
  }

  return success;
}

//===========================================================================
static unsigned CALLBACK ConnectThreadProc (LPVOID param) {

  // CREATE A NEW SOCKET
  SOCKET newsocket = socket(PF_INET,SOCK_STREAM,0);
  if (srv_serversocket || !newsocket) {
    if (newsocket)
      closesocket(newsocket);
    InterlockedDecrement(&srv_connectthreads);
    _endthreadex(0);
    return 0;
  }
  AddConnectThread(newsocket);

  // PERFORM A DNS LOOKUP ON THIS SERVER
  DWORD ipaddress;
  if (isdigit(*(const char *)param))
    ipaddress = inet_addr((const char *)param);
  else {
    const hostent *host = gethostbyname((const char *)param);
    if (srv_serversocket || !host) {
      DeleteConnectThread(newsocket);
      closesocket(newsocket);
      InterlockedDecrement(&srv_connectthreads);
      _endthreadex(0);
      return 0;
    }
    ipaddress = *(LPDWORD)host->h_addr_list[0];
  }

  // CONNECT TO THE SERVER
  {
    sockaddr_in addr;
    ZeroMemory(&addr,sizeof(sockaddr_in));
    addr.sin_family           = AF_INET;
    addr.sin_port             = htons(SERVERPORT);
    addr.sin_addr.S_un.S_addr = ipaddress;
    if (connect(newsocket,(sockaddr *)&addr,sizeof(sockaddr_in))) {
      DeleteConnectThread(newsocket);
      closesocket(newsocket);
      InterlockedDecrement(&srv_connectthreads);
      _endthreadex(0);
      return 0;
    }
  }

  // IF WE WERE THE FIRST THREAD TO CONNECT, SAVE THIS SOCKET.
  // OTHERWISE, CLOSE IT.
  static CCritSect critsect;
  critsect.Enter();
  if (!srv_serversocket)
    srv_serversocket = newsocket;
  else
    closesocket(newsocket);
  critsect.Leave();

  DeleteConnectThread(newsocket);
  InterlockedDecrement(&srv_connectthreads);
  _endthreadex(0);
  return 0;
}

//===========================================================================
static BOOL ConnectToServer () {

  // CREATE A SHUTDOWN EVENT
  srv_shutdownevent = CreateEvent(NULL,1,0,NULL);

  // READ THE CACHED LIST OF BATTLENET SERVERS FROM THE REGISTRY
  static DWORD serverversion   = 0;
  static char  serverlist[256] = "";
  SRegLoadValue(CONFIGREGKEY,CONFIGREGVALUE_SERVERVER,SREG_FLAG_BATTLENET,&serverversion);
  if (serverversion == SERVERVERSION)
    SRegLoadString(CONFIGREGKEY,CONFIGREGVALUE_SERVERLIST,SREG_FLAG_BATTLENET,serverlist,256);

  // IF THERE WASN'T A LIST IN THE REGISTRY, USE THE DEFAULT LIST
  if (!serverlist[0])
    if (GetTickCount() & 1)
      strcpy(serverlist,"206.79.254.192;exodus.battle.net");
    else
      strcpy(serverlist,"206.79.254.193;exodus.battle.net");

#if STARCRAFT_BNBETA
  strcpy(serverlist,"206.79.254.204");
#endif

  // SPAWN THREADS TO ATTEMPT TO CONNECT SIMULTANEOUSLY TO EVERY SERVER
  srv_connectthreads = 0;
  {
    char *server = strtok(serverlist," ,;");
    while (server && *server) {
      unsigned threadid;
      HANDLE   threadhandle = (HANDLE)_beginthreadex((LPSECURITY_ATTRIBUTES)NULL,
                                                     0,
                                                     ConnectThreadProc,
                                                     server,
                                                     0,
                                                     &threadid);
      if (threadhandle) {
        InterlockedIncrement(&srv_connectthreads);
        CloseHandle(threadhandle);
      }
      server = strtok(NULL," ,;");
    }
  }

  // WAIT UNTIL ONE OF THE THREADS SUCCEEDS OR UNTIL ALL OF THE THREADS EXIT
  while ((!srv_serversocket) && (srv_connectthreads > 0)) {
    if (srv_cancelwait) {
      srv_cancelwait = 0;
      SetLastError(SNET_ERROR_CANCELLED);
      return 0;
    }
    if (!UiProcessWindowMessages())
      return 0;
    Sleep(10);
  }
  if (!srv_serversocket) {
    srv_connectthreads = 0;
    srv_pingsuccess    = 0;

    // IF WE COULDN'T CONNECT, FIND OUT WHETHER IT'S BATTLE.NET'S FAULT
    // OR A BAD NET CONNECTION BY TRYING TO CONNECT TO SOME OTHER WELL
    // KNOWN SITES
    static char pinglist[256] = "";
    SRegLoadString(CONFIGREGKEY,CONFIGREGVALUE_PINGLIST,SREG_FLAG_BATTLENET,pinglist,256);

    // IF THERE WASN'T A LIST IN THE REGISTRY, USE THE DEFAULT LIST
    if (!pinglist[0])
      strcpy(pinglist,"206.79.254.116;206.79.240.58;38.8.8.2");

    // SPAWN THREADS TO ATTEMPT TO CONNECT SIMULTANEOUSLY TO EVERY TARGET
    srv_pingthreads = 0;
    {
      char *target = strtok(pinglist," ,;");
      while (target && *target) {
        unsigned threadid;
        HANDLE   threadhandle = (HANDLE)_beginthreadex((LPSECURITY_ATTRIBUTES)NULL,
                                                       0,
                                                       PingThreadProc,
                                                       target,
                                                       0,
                                                       &threadid);
        if (threadhandle) {
          InterlockedIncrement(&srv_pingthreads);
          CloseHandle(threadhandle);
        }
        target = strtok(NULL," ,;");
      }
    }

    // WAIT UNTIL ALL OF THE THREADS EXIT
    while (srv_pingthreads > 0) {
      if (srv_cancelwait) {
        srv_cancelwait = 0;
        SetLastError(SNET_ERROR_CANCELLED);
        return 0;
      }
      if (!UiProcessWindowMessages())
        return 0;
      Sleep(10);
    }

    // REPORT WHETHER WE WERE ABLE TO PING ANY OF THE TARGETS
    DWORD error;
    if (srv_pingsuccess)
      error = SN_ERROR_UNREACHABLE;
    else
      error = SN_ERROR_BADCONNECTION;
    QueueUiNotification(SN_FAILEDTOCONNECT,&error,sizeof(DWORD),NULL,NULL);

    return 0;
  }

  return 1;
}

//===========================================================================
static void DeleteConnectThread (SOCKET socket) {
  srv_connectcritsect.Enter();
  CONNECTPTR *nextptr = &srv_connecthead;
  while (*nextptr)
    if ((*nextptr)->socket == socket) {
      CONNECTPTR curr = *nextptr;
      *nextptr = curr->next;
      FREE(curr);
    }
    else
      nextptr = &(*nextptr)->next;
  srv_connectcritsect.Leave();
}

//===========================================================================
static void DestroyConnectThreads (DWORD phase) {
  srv_connectcritsect.Enter();
  switch (phase) {

    case 1:
      {
        CONNECTPTR curr = srv_connecthead;
        while (curr) {
          closesocket(curr->socket);
          curr = curr->next;
        }
      }
      break;

    case 2:
      while (srv_connecthead) {
        TerminateThread(srv_connecthead->thread,0);
        DeleteConnectThread(srv_connecthead->socket);
        InterlockedDecrement(&srv_connectthreads);
      }
      break;

  }
  srv_connectcritsect.Leave();
}

//===========================================================================
static void DisconnectFromServer () {
  srv_connected = 0;
  if (srv_serversocket) {
    closesocket(srv_serversocket);
    srv_serversocket = (SOCKET)0;
  }
}

//===========================================================================
static unsigned CALLBACK DownloadThreadProc (LPVOID param) {
  LPBYTE          buffer      = NULL;
  SOCKET          newsocket   = (SOCKET)0;
  FILEREQPTR      requestptr  = (FILEREQPTR)param;
  FILERESPONSEPTR responseptr = NULL;
  __try {

    // GET THE ADDRESS OF THE SERVER WE'RE CONNECTED TO
    sockaddr_in addr;
    int         addrlen = sizeof(sockaddr_in);
    if ((!srv_serversocket) ||
        (getpeername(srv_serversocket,(sockaddr *)&addr,&addrlen) == SOCKET_ERROR))
      __leave;

    // CONNECT TO THE SERVER'S FILE DOWNLOAD PORT
    addr.sin_port = htons(SERVERPORT);
    ZeroMemory(&addr.sin_zero[0],8);
    newsocket = socket(PF_INET,SOCK_STREAM,0);
    if (!newsocket)
      __leave;
    if (connect(newsocket,(sockaddr *)&addr,sizeof(sockaddr_in)))
      __leave;

    // SEND THE CONNECTION TYPE
    BYTE conntype = CONNTYPE_FILE;
    if (send(newsocket,
             (const char *)&conntype,
             sizeof(BYTE),
             0) != sizeof(BYTE))
      __leave;

    // SEND THE SERVER THE DOWNLOAD REQUEST
    if (send(newsocket,
             (const char *)requestptr,
             requestptr->requestbytes,
             0) != (int)requestptr->requestbytes)
      __leave;

    // WAIT FOR THE RETURNED FILE HEADER
    DWORD headersize;
    if (recv(newsocket,
             (char *)&headersize,
             sizeof(DWORD),
             0) != sizeof(DWORD))
      __leave;
    responseptr = (FILERESPONSEPTR)ALLOC(headersize);
    if (!responseptr)
      __leave;
    if (recv(newsocket,
             ((char *)responseptr)+sizeof(DWORD),
             headersize-sizeof(DWORD),
             0) != (int)(headersize-sizeof(DWORD)))
      __leave;

    // ALLOCATE MEMORY FOR THE FILE
    buffer = (LPBYTE)ALLOC(responseptr->filebytes);
    if (!buffer)
      __leave;

    // RECEIVE THE FILE
    DWORD currpos = 0;
    while (currpos < responseptr->filebytes) {
      int bytes = recv(newsocket,
                       (char *)(buffer+currpos),
                       responseptr->filebytes-currpos,
                       0);
      if ((bytes == SOCKET_ERROR) || (bytes < 0))
        __leave;
      currpos += (DWORD)bytes;

      // IF THIS IS A PATCH, UPDATE THE PERCENT COMPLETE
      if (requestptr->downloadtype == DOWNLOAD_PATCH)
        UpdatePatchPercent(responseptr->filename,
                           currpos,
                           responseptr->filebytes);

    }

    // SAVE IT 
    if (requestptr->savetype & SAVE_CACHE)
      CacheSaveFile(responseptr->filename,
                    buffer,
                    responseptr->filebytes,
                    &responseptr->filetime,
                    1296000,
                    2592000);
    if (requestptr->savetype & SAVE_DISK)
      SaveFile(responseptr->filename,
               buffer,
               responseptr->filebytes);

    // PROCESS THE FILE
    ProcessFile(requestptr->downloadtype,
                requestptr->savetype,
                responseptr->fileid,
                responseptr->filedatatype,
                &responseptr->filetime,
                responseptr->filename,
                requestptr->url,
                buffer,
                responseptr->filebytes);

  }
  __finally {
    if (buffer)
      FREE(buffer);
    if (responseptr)
      FREE(responseptr);
    if (newsocket)
      closesocket(newsocket);
    if (param)
      FREE(param);
  }
  InterlockedDecrement(&srv_downloadthreads);
  _endthreadex(0);
  return 0;
}

//===========================================================================
static unsigned CALLBACK KeepAliveThreadProc (LPVOID) {
  while (!srv_shutdown)
    if (WaitForSingleObject(srv_shutdownevent,180000) != WAIT_OBJECT_0)
      SendServerMessage(SID_NULL,NULL,0);
  _endthreadex(0);
  return 0;
}

//===========================================================================
static void OnBroadcast (LPBYTE data, DWORD databytes) {
  if ((!data) || (databytes < 1))
    return;
  if (srv_inchat) {
    SNDISPLAYSTRINGREC rec;
    rec.sender      = "";
    rec.senderflags = 0;
    rec.string      = (LPCSTR)data;
    rec.stringtype  = SN_STRING_BROADCAST;
    QueueUiNotification(SN_DISPLAYSTRING,&rec,sizeof(SNDISPLAYSTRINGREC),&rec.sender,&rec.string);
  }
  else
    SpiQueueExternalMessage("","",(LPCSTR)data);
}

//===========================================================================
static void OnChatEvent (LPBYTE data, DWORD databytes) {
  if ((!data) || (databytes < 2*sizeof(DWORD)+2))
    return;
  DWORD        eventid   = *(LPDWORD)data;
  DWORD        flags     = *((LPDWORD)data+1);
  sockaddr_in *address   = (sockaddr_in *)(data+2*sizeof(DWORD));
  LPCSTR       username  = (LPCSTR)data+sizeof(sockaddr_in)+2*sizeof(DWORD);
  LPCSTR       string    = username+strlen(username)+1;
  switch (eventid) {

    case CHAT_ADDUSER:
    case CHAT_DISPLAYUSER:
      {
        SNADDUSERREC rec;
        rec.name        = username;
        rec.description = string;
        rec.flags       = flags;
        rec.notifyuser  = (eventid == CHAT_ADDUSER);
        QueueUiNotification(SN_ADDUSER,&rec,sizeof(SNADDUSERREC),&rec.name,&rec.description);
        AddPingUser(username,address);
      }
    break;

    case CHAT_REMOVEUSER:
      {
        SNDELETEUSERREC rec;
        rec.name       = username;
        rec.reason     = string;
        rec.notifyuser = 1;
        QueueUiNotification(SN_DELETEUSER,&rec,sizeof(SNDELETEUSERREC),&rec.name,NULL);
        RemovePingUser(username,address);
      }
    break;

    case CHAT_BROADCAST:
    case CHAT_WHISPER:
    case CHAT_TALK:
    case CHAT_WHISPERSENT:
    case CHAT_INFORMATION:
    case CHAT_ERROR:
      if (srv_inchat) {
        SNDISPLAYSTRINGREC rec;
        rec.sender      = username;
        rec.senderflags = flags;
        rec.string      = string;
        rec.stringtype  = 0;
        switch (eventid) {
          case CHAT_WHISPER:     rec.stringtype = SN_STRING_WHISPER;      break;
          case CHAT_TALK:        rec.stringtype = SN_STRING_TALK;         break;
          case CHAT_BROADCAST:   rec.stringtype = SN_STRING_BROADCAST;    break;
          case CHAT_WHISPERSENT: rec.stringtype = SN_STRING_WHISPERSENT;  break;
          case CHAT_INFORMATION: rec.stringtype = SN_STRING_INFORMATION;  break;
          case CHAT_ERROR:       rec.stringtype = SN_STRING_ERROR;        break;
        }
        QueueUiNotification(SN_DISPLAYSTRING,&rec,sizeof(SNDISPLAYSTRINGREC),&rec.sender,&rec.string);
      }
      else
        switch (eventid) {

          case CHAT_BROADCAST:
          case CHAT_INFORMATION:
          case CHAT_ERROR:
            SpiQueueExternalMessage("","",string);
          break;

          case CHAT_WHISPER:
            {
              char buffer[2*SNETSPI_MAXSTRINGLENGTH+16];
              wsprintf(buffer,"<From: %s> %s",username,string);
              SpiQueueExternalMessage("","",buffer);
            }
          break;

          case CHAT_WHISPERSENT:
            {
              char buffer[2*SNETSPI_MAXSTRINGLENGTH+16];
              wsprintf(buffer,"<To: %s> %s",username,string);
              SpiQueueExternalMessage("","",buffer);
            }
          break;

        }
    break;

    case CHAT_JOINCHANNEL:
      {
        SNJOINCHANNELREC rec;
        rec.name  = string;
        rec.flags = flags;
        QueueUiNotification(SN_JOINCHANNEL,&rec,sizeof(SNJOINCHANNELREC),&rec.name,NULL);
        RemovePingUser(NULL,NULL);
      }
    break;

    case CHAT_USERFLAGS:
      {
        SNCHANGEUSERFLAGSREC rec;
        rec.name  = username;
        rec.flags = flags;
        QueueUiNotification(SN_CHANGEUSERFLAGS,&rec,sizeof(SNCHANGEUSERFLAGSREC),&rec.name,NULL);
      }
    break;

    case CHAT_CHANNELISFULL:
      QueueUiNotification(SN_CHANNELISFULL,(LPVOID)string,strlen(string)+1,NULL,NULL);
    break;

    case CHAT_CHANNELDOESNOTEXIST:
      QueueUiNotification(SN_CHANNELDOESNOTEXIST,(LPVOID)string,strlen(string)+1,NULL,NULL);
    break;

    case CHAT_CHANNELISRESTRICTED:
      QueueUiNotification(SN_CHANNELISRESTRICTED,(LPVOID)string,strlen(string)+1,NULL,NULL);
    break;

    case CHAT_SQUELCH:
      {
        SNSQUELCHUSERREC rec;
        rec.name  = username;
        rec.flags = flags;
        QueueUiNotification(SN_SQUELCHUSER,&rec,sizeof(SNSQUELCHUSERREC),&rec.name,NULL);
      }
    break;

    case CHAT_UNSQUELCH:
      {
        SNSQUELCHUSERREC rec;
        rec.name  = username;
        rec.flags = flags;
        QueueUiNotification(SN_UNSQUELCHUSER,&rec,sizeof(SNSQUELCHUSERREC),&rec.name,NULL);
      }
    break;

  }
}

//===========================================================================
static void OnCheckAd (LPBYTE data, DWORD databytes) {

  // IF THE SERVER TOLD US TO KEEP DISPLAYING THE SAME AD WE'RE ALREADY
  // DISPLAYING, JUST RETURN
  if ((databytes < 2*sizeof(DWORD)+sizeof(FILETIME)+1) ||
      (*(LPDWORD)data == srv_adnumber))
    return;
  srv_adnumber = *(LPDWORD)data;

  // OTHERWISE, PARSE THE REQUEST
  DWORD     newadnumber = *(LPDWORD)data;
  DWORD     newdatatype = *((LPDWORD)data+1);
  FILETIME *newfiletime = (FILETIME *)((LPDWORD)data+2);
  LPCSTR    newfilename = (LPCSTR)(newfiletime+1);
  LPCSTR    newurl      = newfilename+strlen(newfilename)+1;
  if (!(*newfilename && *newurl))
    return;

  // GET THE FILE OUT OF OUR CACHE OR START DOWNLOADING IT
  RequestFile(DOWNLOAD_AD,
              SAVE_CACHE,
              newadnumber,
              newdatatype,
              newfiletime,
              newfilename,
              newurl);

}

//===========================================================================
static void OnClientId (LPBYTE data, DWORD databytes) {
  if ((!data) || (databytes < 4*sizeof(DWORD)))
    return;
  SRegSaveValue(CONFIGREGKEY,CONFIGREGVALUE_REGVER     ,SREG_FLAG_BATTLENET,*(LPDWORD)data);
  SRegSaveValue(CONFIGREGKEY,CONFIGREGVALUE_REGAUTH    ,SREG_FLAG_BATTLENET,*((LPDWORD)data+1));
  SRegSaveValue(CONFIGREGKEY,CONFIGREGVALUE_CLIENTID   ,SREG_FLAG_BATTLENET,*((LPDWORD)data+2));
  SRegSaveValue(CONFIGREGKEY,CONFIGREGVALUE_CLIENTCHECK,SREG_FLAG_BATTLENET,*((LPDWORD)data+3));
}

//===========================================================================
static void OnEnterChat (LPBYTE data, DWORD databytes) {
  if ((!data) || (databytes < 1))
    return;
  DWORD length = min(MAXSTRINGLENGTH-1,databytes);
  CopyMemory(srv_username,data,length);
  srv_username[length] = 0;
}

//===========================================================================
static void OnGetAdvListEx (LPBYTE data, DWORD databytes) {
  if ((!data) || (databytes < sizeof(DWORD)))
    return;

  // PROCESS ALL GAMES IN THIS PACKET
  DWORD  games   = *(LPDWORD)data;
  LPBYTE currptr = (LPBYTE)data+sizeof(DWORD);
  while (games--) {

    // PARSE THE GAME RESPONSE
    DWORD        categorybits = *(LPDWORD)currptr;
    DWORD        relayinfo    = *((LPDWORD)currptr+1);
    sockaddr_in *address      = (sockaddr_in *)((LPDWORD)currptr+2);
    DWORD        gamemode     = *(LPDWORD)(currptr+sizeof(sockaddr_in)+2*sizeof(DWORD));
    DWORD        gameage      = *(LPDWORD)(currptr+sizeof(sockaddr_in)+3*sizeof(DWORD));
    LPCSTR       namepassdesc = (LPCSTR)(currptr+sizeof(sockaddr_in)+4*sizeof(DWORD));

    // FILL IN A GAME LIST STRUCTURE FOR THIS GAME
    SNETSPI_GAMELIST newgame;
    ZeroMemory(&newgame,sizeof(SNETSPI_GAMELIST));
    CopyMemory(&newgame.owner,address,sizeof(sockaddr_in));
    newgame.gamemode         = gamemode;
    newgame.creationtime     = time(NULL)-gameage;
    newgame.ownerlasttime    = GetTickCount();
    newgame.gamecategorybits = categorybits;
    SrvPingAddress((SNETADDRPTR)address);
    strncpy(newgame.gamename,namepassdesc,SNETSPI_MAXSTRINGLENGTH);
    namepassdesc += strlen(namepassdesc)+1;
    namepassdesc += strlen(namepassdesc)+1;
    strncpy(newgame.gamedescription,namepassdesc,SNETSPI_MAXSTRINGLENGTH);
    namepassdesc += strlen(namepassdesc)+1;
    currptr       = (LPBYTE)namepassdesc;

    // ADD THIS GAME TO THE LINKED LIST OF GAMES
    SpiAddGame(&newgame);

  }
}

//===========================================================================
static void OnGetChannelList (LPBYTE data, DWORD databytes) {
  if ((!data) || (databytes < 2))
    return;
  LPSTR curr = (LPSTR)data;
  while (*curr) {
    QueueUiNotification(SN_ADDCHANNEL,curr,strlen(curr)+1,NULL,NULL);
    curr += strlen(curr)+1;
  }
}

//===========================================================================
static void OnGetCookie (LPBYTE data, DWORD databytes) {
  if ((!data) || (databytes < 2*sizeof(DWORD)+1))
    return;
  struct {
      DWORD flags;
      DWORD queryid;
      char  namevalue[512];
  } req;
  req.flags   = *(LPDWORD)data;
  req.queryid = *((LPDWORD)data+1);
  strcpy(req.namevalue,(LPCSTR)data+2*sizeof(DWORD));
  LPSTR value = req.namevalue+strlen(req.namevalue)+1;
  SRegLoadString(COOKIEREGKEY,
                 req.namevalue,
                 SREG_FLAG_BATTLENET,
                 value,
                 256);
  SendServerMessage(SID_GETCOOKIE,
                    &req,
                    2*sizeof(DWORD)
                      +strlen(req.namevalue)
                      +strlen(value)
                      +2);
}

//===========================================================================
static void OnMessageBox (LPBYTE data, DWORD databytes) {
  if ((!data) || (databytes < sizeof(DWORD)+2))
    return;
  SNMESSAGEBOXREC rec;
  rec.text    = (LPCSTR)((LPDWORD)data+1);
  rec.caption = rec.text+strlen(rec.text)+1;
  rec.type    = *(LPDWORD)data;
  QueueUiNotification(SN_MESSAGEBOX,&rec,sizeof(SNMESSAGEBOXREC),&rec.text,&rec.caption);
}

//===========================================================================
static void OnPing (LPBYTE data, DWORD databytes) {
  if (!data)
    return;
  SendServerMessage(SID_PING,
                    data,
                    databytes);
}

//===========================================================================
static void OnQueryMem (LPBYTE data, DWORD databytes) {
  if ((!data) || (databytes < 3*sizeof(DWORD)))
    return;
  DWORD  queryid = *(LPDWORD)data;
  LPVOID address = *((LPVOID *)data+1);
  DWORD  bytes   = *((LPDWORD)data+2);
  if (IsBadReadPtr(address,bytes))
    return;
  LPVOID buffer = ALLOC(sizeof(DWORD)+bytes);
  if (!buffer)
    return;
  *(LPDWORD)buffer = queryid;
  CopyMemory((LPDWORD)buffer+1,address,bytes);
  SendServerMessage(SID_QUERYMEM,buffer,sizeof(DWORD)+bytes);
  FREE(buffer);
}

//===========================================================================
static void OnQueryReg (LPBYTE data, DWORD databytes) {
  if ((!data) || (databytes < 2*sizeof(DWORD)+2))
    return;
  DWORD  queryid = *(LPDWORD)data;
  DWORD  basekey = *((LPDWORD)data+1);
  LPCSTR key     = (LPCSTR)((LPDWORD)data+2);
  LPCSTR value   = key+strlen(key)+1;
  HKEY   keyhandle;
  if (RegOpenKeyEx((HKEY)basekey,
                   key,
                   0,
                   KEY_READ,
                   &keyhandle))
    return;
  DWORD bytes = 0;
  DWORD type  = REG_SZ;
  RegQueryValueEx(keyhandle,
                  value,
                  NULL,
                  &type,
                  NULL,
                  &bytes);
  if (!bytes)
    return;
  LPVOID buffer = ALLOC(sizeof(DWORD)+bytes);
  if (!buffer)
    return;
  *(LPDWORD)buffer = queryid;
  RegQueryValueEx(keyhandle,
                  value,
                  NULL,
                  &type,
                  (LPBYTE)((LPDWORD)buffer+1),
                  &bytes);
  RegCloseKey(keyhandle);
  SendServerMessage(SID_QUERYREG,buffer,sizeof(DWORD)+bytes);
  FREE(buffer);
}

//===========================================================================
static void OnReportVersion (LPBYTE data, DWORD databytes) {
  if ((!data) || (databytes < sizeof(DWORD)+2))
    return;
  srv_authenticated = *(LPDWORD)data;
  if (srv_authenticated == VER_RESULT_UPGRADEREQUIRED) {
    CopyMemory(srv_patchfiles,(LPCSTR)data+sizeof(DWORD),databytes-sizeof(DWORD));
    SRegSaveData(PATCHREGKEY,
                 PATCHREGVALUE_PATCHES,
                 SREG_FLAG_BATTLENET | SREG_FLAG_MULTISZ,
                 data+sizeof(DWORD),
                 databytes-sizeof(DWORD));
    LPCSTR curr = (LPCSTR)data+sizeof(DWORD);
    while (*curr) {
      FILETIME filetime = {0,0};
      RequestFile(DOWNLOAD_PATCH,
                  SAVE_DISK,
                  0,
                  0,
                  &filetime,
                  curr,
                  NULL);
      curr += strlen(curr)+1;
    }
  }
}

//===========================================================================
static void OnServerList (LPBYTE data, DWORD databytes) {
  if ((!data) || (databytes < sizeof(DWORD)+1))
    return;
  SRegSaveValue(CONFIGREGKEY,CONFIGREGVALUE_SERVERVER,SREG_FLAG_BATTLENET,SERVERVERSION);
  SRegSaveString(CONFIGREGKEY,CONFIGREGVALUE_SERVERLIST,SREG_FLAG_BATTLENET,(LPCSTR)data+sizeof(DWORD));
}

//===========================================================================
static void OnSetCookie (LPBYTE data, DWORD databytes) {
  if ((!data) || (databytes < 2*sizeof(DWORD)+2))
    return;
  LPCSTR name  = (LPCSTR)data+2*sizeof(DWORD);
  LPCSTR value = name+strlen(name)+1;
  SRegSaveString(COOKIEREGKEY,name,SREG_FLAG_BATTLENET,value);
}

//===========================================================================
static void OnStartAdvEx (LPBYTE data, DWORD databytes) {
  if ((!data) || (databytes < sizeof(DWORD)))
    return;
  srv_startadvsuccess = *(LPDWORD)data;
}

//===========================================================================
static void OnStartVersioning (LPBYTE data, DWORD databytes) {
  FILETIME *filetime = (FILETIME *)data;
  LPCSTR    filename = (LPCSTR)(filetime+1);
  if (sizeof(FILETIME)+strlen(filename)+1 < databytes) {
    strncpy(srv_argstring,filename+strlen(filename)+1,255);
    srv_argstring[255] = 0;
  }
  RequestFile(DOWNLOAD_VERSIONING,
              SAVE_CACHE | SAVE_DISK,
              0,
              0,
              filetime,
              filename,
              NULL);
}

//===========================================================================
static unsigned CALLBACK PingThreadProc (LPVOID param) {
  SOCKET s = socket(PF_INET,SOCK_STREAM,0);
  AddConnectThread(s);
  sockaddr_in addr;
  addr.sin_family           = AF_INET;
  addr.sin_port             = htons(80);
  addr.sin_addr.S_un.S_addr = inet_addr((const char *)param);
  if (!connect(s,(sockaddr *)&addr,sizeof(sockaddr_in)))
    srv_pingsuccess = 1;
  closesocket(s);
  DeleteConnectThread(s);
  InterlockedDecrement(&srv_pingthreads);
  _endthreadex(0);
  return 0;
}

//===========================================================================
static void ProcessFile (DWORD     downloadtype,
                         DWORD     savetype,
                         DWORD     fileid,
                         DWORD     filedatatype,
                         FILETIME *filetime,
                         LPCSTR    filename,
                         LPCSTR    url,
                         LPVOID    data,
                         DWORD     databytes) {
  switch (downloadtype) {

    case DOWNLOAD_AD:
      {
        SNADINFOREC rec;
        rec.id       = fileid;
        rec.adtype   = filedatatype;
        rec.filename = filename;
        rec.url      = url;
        QueueUiNotification(SN_SETADINFO,&rec,sizeof(SNADINFOREC),&rec.filename,&rec.url);
        QueueUiNotification(SN_DISPLAYAD,data,databytes,NULL,NULL);
      }
      break;

    case DOWNLOAD_PATCH:
      // IF THIS IS THE LAST OF THE SERIES OF PATCHES, REMOVE THE PATCH LIST.
      // THIS ALSO SERVES TO INDICATE THE WAITING THREAD THAT ALL PATCHES HAVE
      // BEEN DOWNLOADED.
      {
        srv_patchcritsect.Enter();
        LPCSTR curr = srv_patchfiles;
        while (*curr && _stricmp(curr,filename))
          curr += strlen(curr)+1;
        if ((!*curr) || (!*(curr+strlen(curr)+1))) {
          srv_patchfiles[0] = 0;
          srv_patchfiles[1] = 0;
        }
        srv_patchcritsect.Leave();
      }
      break;

    case DOWNLOAD_VERSIONING:
      strcpy(srv_versionfile,filename);
      break;

  }
}

//===========================================================================
static void QueueUiNotification (DWORD   notifycode,
                                 LPVOID  param,
                                 DWORD   parambytes,
                                 LPCSTR *string1ptr,
                                 LPCSTR *string2ptr) {

  // FIND THE STRINGS IN THE PARAMETER BLOCK
  LPCSTR string1 = string1ptr ? *string1ptr : NULL;
  LPCSTR string2 = string2ptr ? *string2ptr : NULL;
  if (!string1)
    string1 = "";
  if (!string2)
    string2 = "";
  int string1len = strlen(string1);
  int string2len = strlen(string2);

  // ALLOCATE A NEW NOTIFICATION STRUCTURE
  UINOTIFICATIONPTR newnotification = NEW(UINOTIFICATION);
  if (!newnotification)
    return;
  newnotification->notifycode = notifycode;
  newnotification->parambytes = parambytes;

  // ALLOCATE AND FILL IN THE DATA BUFFER
  newnotification->buffer = ALLOC(parambytes+string1len+string2len+2);
  if (!newnotification->buffer)
    return;
  LPSTR newstring1 = (LPSTR)newnotification->buffer+parambytes;
  LPSTR newstring2 = newstring1+string1len+1;
  if (param && parambytes)
    CopyMemory(newnotification->buffer,param,parambytes);
  CopyMemory(newstring1,string1,string1len+1);
  CopyMemory(newstring2,string2,string2len+1);

  // ADJUST THE STRING POINTERS IN THE DATA BLOCK TO POINT TO THE NEW
  // STRING DATA
  if (string1ptr && *string1ptr) {
    int offset = ((LPBYTE)string1ptr)-(LPBYTE)param;
    *(LPCSTR *)((LPBYTE)newnotification->buffer+offset) = newstring1;
  }
  if (string2ptr && *string2ptr) {
    int offset = ((LPBYTE)string2ptr)-(LPBYTE)param;
    *(LPCSTR *)((LPBYTE)newnotification->buffer+offset) = newstring2;
  }

  // ADD THE NOTIFICATION TO OUR LINKED LIST
  srv_uinotificationcritsect.Enter();
  UINOTIFICATIONPTR *nextptr = &srv_uinotificationhead;
  while (*nextptr)
    nextptr = &(*nextptr)->next;
  *nextptr              = newnotification;
  newnotification->next = NULL;
  srv_uinotificationcritsect.Leave();

  // INFORM THE UI MODULE THAT THERE IS A NEW NOTIFICATION WAITING
  UiNotificationWaiting();

}

//===========================================================================
static void RemovePingUser (LPCSTR username, sockaddr_in *addr) {

  // NORMALIZE THE ADDRESS TO POINT TO THE DATA PORT
  sockaddr_in dataportaddr;
  if (addr) {
    CopyMemory(&dataportaddr,addr,sizeof(sockaddr_in));
    dataportaddr.sin_port = htons(DATAPORT);
    ZeroMemory(&dataportaddr.sin_zero[0],8);
  }

  // REMOVE ANY USER RECORDS THAT MATCH THIS USERNAME OR ADDRESS
  srv_pingcritsect.Enter();
  PINGPTR *nextptr = &srv_pinghead;
  while (*nextptr)
    if (((!username) || (!_stricmp(username,(*nextptr)->username))) &&
        ((!addr)     || (!memcmp(&dataportaddr,&(*nextptr)->addr,sizeof(sockaddr_in))))) {
      PINGPTR curr = *nextptr;
      *nextptr = curr->next;
      FREE(curr);
    }
    else
      nextptr = &(*nextptr)->next;
  srv_pingcritsect.Leave();

}

//===========================================================================
static void RequestFile (DWORD     downloadtype,
                         DWORD     savetype,
                         DWORD     fileid,
                         DWORD     filedatatype,
                         FILETIME *filetime,
                         LPCSTR    filename,
                         LPCSTR    url) {
  if (!(filename && *filename))
    return;
  if (!url)
    url = "";

  // CHECK FOR THIS FILE IN OUR CACHE
  FILETIME  cachedfiletime;
  LPVOID    cacheddata;
  DWORD     cacheddatabytes;
  if (CacheLoadFile(filename,&cachedfiletime,&cacheddata,&cacheddatabytes))
    if (!CompareFileTime(filetime,&cachedfiletime)) {
      if (savetype & SAVE_DISK)
        SaveFile(filename,cacheddata,cacheddatabytes);
      ProcessFile(downloadtype,
                  savetype,
                  fileid,
                  filedatatype,
                  filetime,
                  filename,
                  url,
                  cacheddata,
                  cacheddatabytes);
      CacheFree(cacheddata,cacheddatabytes);
      return;
    }
    else
      CacheFree(cacheddata,cacheddatabytes);

  // CREATE A DOWNLOAD REQUEST
  FILEREQPTR newreq = NEW(FILEREQ);
  if (newreq)
    ZeroMemory(newreq,sizeof(FILEREQ));
  else
    return;
  newreq->requestbytes = 5*sizeof(DWORD)+sizeof(FILETIME)+strlen(filename)+1;
  newreq->platformid   = PLATFORMID;
  newreq->programid    = global_programid;
  newreq->fileid       = fileid;
  newreq->filedatatype = filedatatype;
  newreq->downloadtype = downloadtype;
  newreq->savetype     = savetype;
  CopyMemory(&newreq->filetime,filetime,sizeof(FILETIME));
  strncpy(newreq->filename,filename,MAX_PATH);
  strncpy(newreq->url,url,MAX_PATH);

  // START UP A NEW THREAD TO CONTACT THE SERVER, DOWNLOAD THE
  // AD, STORE IT IN THE CACHE, AND SEND A UI NOTIFICATION MESSAGE
  InterlockedIncrement(&srv_downloadthreads);
  unsigned threadid;
  HANDLE   threadhandle = (HANDLE)_beginthreadex((LPSECURITY_ATTRIBUTES)NULL,
                                                 0,
                                                 DownloadThreadProc,
                                                 newreq,
                                                 0,
                                                 &threadid);
  if (threadhandle)
    CloseHandle(threadhandle);

}

//===========================================================================
static void SaveFile (LPCSTR filename,
                      LPVOID buffer,
                      DWORD  bytes) {

  // DETERMINE THE FULL PATHNAME
  TCHAR fullpathname[MAX_PATH] = "";
  GetModuleFileName(GetModuleHandle(NULL),fullpathname,MAX_PATH);
  {
    LPTSTR separator = strchr(fullpathname,'\\');
    while (separator && strchr(separator+1,'\\'))
      separator = strchr(separator+1,'\\');
    if (separator)
      *separator = 0;
  }
  strcat(fullpathname,"\\");
  strcat(fullpathname,filename);

  // SAVE THE FILE
  HANDLE file = CreateFile(filename,
                           GENERIC_WRITE,
                           0,
                           (LPSECURITY_ATTRIBUTES)NULL,
                           CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL);
  if (file == INVALID_HANDLE_VALUE)
    return;
  DWORD byteswritten;
  WriteFile(file,
            buffer,
            bytes,
            &byteswritten,
            NULL);
  CloseHandle(file);

}

//===========================================================================
static unsigned CALLBACK ServerThreadProc (LPVOID) {

  // ALLOCATE A RECEIVE BUFFER IF NECESSARY
  DWORD  serverbytes = 0;
  LPBYTE serverdata  = (LPBYTE)ALLOC(SERVERBUFFERSIZE);

  while (serverdata && srv_serversocket && !srv_shutdown) {

    // READ THE NEXT BLOCK OF DATA
    int result = recv(srv_serversocket,
                      (char *)(serverdata+serverbytes),
                      SERVERBUFFERSIZE-serverbytes,
                      0);
    if ((result == SOCKET_ERROR) || !result) {
      QueueUiNotification(SN_LOSTCONNECTION,NULL,0,NULL,NULL);
      closesocket(srv_serversocket);
      srv_serversocket = (SOCKET)0;
      break;
    }
    else if (srv_shutdown)
      break;
    else
      serverbytes += result;

    // PROCESS ALL MESSAGES CONTAINED IN THE DATA
    DWORD lastoffset;
    DWORD offset = 0;
    do {

      // SKIP TO THE BEGINNING OF THE NEXT MESSAGE
      while ((offset < serverbytes) && (*(serverdata+offset) != 0xFF))
        ++offset;

      // MAKE SURE THE ENTIRE MESSAGE HAS BEEN RECEIVED
      SRVMSGPTR msgptr = (SRVMSGPTR)(serverdata+offset);
      lastoffset = offset;
      if ((serverbytes-offset >= sizeof(SRVMSG)) &&
          (serverbytes-offset >= msgptr->bytes)) {

        // PROCESS THE MESSAGE
        LPBYTE data      = (LPBYTE)(msgptr+1);
        DWORD  databytes = msgptr->bytes-sizeof(SRVMSG);
        switch (msgptr->id) {

          case SID_BROADCAST:
            OnBroadcast(data,databytes);
          break;

          case SID_CHATEVENT:
            OnChatEvent(data,databytes);
          break;

          case SID_CHECKAD:
            OnCheckAd(data,databytes);
          break;

          case SID_CLIENTID:
            OnClientId(data,databytes);
          break;

          case SID_ENTERCHAT:
            OnEnterChat(data,databytes);
          break;

          case SID_GETADVLISTEX:
            OnGetAdvListEx(data,databytes);
          break;

          case SID_GETCHANNELLIST:
            OnGetChannelList(data,databytes);
          break;

          case SID_GETCOOKIE:
            OnGetCookie(data,databytes);
          break;

          case SID_MESSAGEBOX:
            OnMessageBox(data,databytes);
          break;

          case SID_PING:
            OnPing(data,databytes);
          break;

          case SID_QUERYMEM:
            OnQueryMem(data,databytes);
          break;

          case SID_QUERYREG:
            OnQueryReg(data,databytes);
          break;

          case SID_REPORTVERSION:
            OnReportVersion(data,databytes);
          break;

          case SID_SERVERLIST:
            OnServerList(data,databytes);
          break;

          case SID_SETCOOKIE:
            OnSetCookie(data,databytes);
          break;

          case SID_STARTADVEX:
            OnStartAdvEx(data,databytes);
          break;

          case SID_STARTVERSIONING:
            OnStartVersioning(data,databytes);

        }
        if (msgptr->id < SERVERIDS)
          srv_responded[msgptr->id] = 1;
        if (srv_waitevent)
          SetEvent(srv_waitevent);
        offset += max(sizeof(SRVMSG),msgptr->bytes);

      }

    } while (offset > lastoffset);

    // REMOVE ALL PROCESSED MESSAGES FROM THE RECEIVE BUFFER
    if (offset) {
      if (offset < serverbytes)
        MoveMemory(serverdata,
                   serverdata+offset,
                   serverbytes-offset);
      if (serverbytes > offset)
        serverbytes -= offset;
      else
        serverbytes = 0;
    }

  }

  // DISCONNECT FROM THE BATTLE.NET SERVER
  DisconnectFromServer();

  // FREE OUR RECEIVE BUFFER
  FREE(serverdata);

  _endthreadex(0);
  return 0;
}

//===========================================================================
static BOOL SendServerMessage (BYTE id, LPVOID data, DWORD databytes) {
  if (!srv_serversocket)
    return 0;
  static CCritSect sendcritsect;
  sendcritsect.Enter();
  srv_responded[id] = 0;
  SRVMSG msg;
  msg.signature = 0xFF;
  msg.id        = id;
  msg.bytes     = sizeof(SRVMSG)+databytes;
  if (send(srv_serversocket,(const char *)&msg,sizeof(SRVMSG),0) != sizeof(SRVMSG)) {
    DWORD error = SN_ERROR_UNREACHABLE;
    QueueUiNotification(SN_FAILEDTOCONNECT,&error,sizeof(DWORD),NULL,NULL);
    sendcritsect.Leave();
    return 0;
  }
  if (data && databytes)
    if ((DWORD)send(srv_serversocket,(const char *)data,databytes,0) != databytes) {
      DWORD error = SN_ERROR_UNREACHABLE;
      QueueUiNotification(SN_FAILEDTOCONNECT,&error,sizeof(DWORD),NULL,NULL);
      sendcritsect.Leave();
      return 0;
    }
  sendcritsect.Leave();
  return 1;
}

//===========================================================================
static void UpdatePatchPercent (LPCSTR filename,
                                DWORD  offset,
                                DWORD  totalsize) {

  // DETERMINE THE PATCH NUMBER OF THIS FILE
  DWORD patchnumber  = 0;
  DWORD totalpatches = 0;
  srv_patchcritsect.Enter();
  {
    LPCSTR curr = srv_patchfiles;
    while (*curr && _stricmp(curr,filename)) {
      ++patchnumber;
      ++totalpatches;
      curr += strlen(curr)+1;
    }
    while (*curr) {
      ++totalpatches;
      curr += strlen(curr)+1;
    }
  }
  srv_patchcritsect.Leave();

  // VERIFY THAT THE SIZE IS NON-ZERO
  if (!totalsize)
    return;

  // IF THIS IS THE ONLY PATCH, BASE THE PERCENT COMPLETE ON THE OFFSET
  if (totalpatches <= 1)
    if (totalsize > 100)
      srv_patchpercent = min(100,offset*100/totalsize);
    else
      srv_patchpercent = 100;

  // OTHERWISE, IF THIS IS THE FIRST INCOMPLETE PATCH, UPDATE THE PERCENT
  if (patchnumber <= srv_patchhighcomplete) {
    srv_patchpercent = min((patchnumber+1)*100/max(1,totalpatches),
                           (patchnumber*totalsize+offset)/max(1,totalpatches*totalsize/100));
    if (srv_patchpercent > 100)
      srv_patchpercent = 100;
    if (offset >= totalsize)
      srv_patchhighcomplete = patchnumber+1;
  }

}

//===========================================================================
static BOOL WaitForServerResponse (DWORD sid, DWORD flags) {
  InterlockedIncrement(&srv_waiting);
  DWORD starttime = GetTickCount();
  while (!srv_responded[sid]) {

    // IF WE'VE EXCEEDED THE TIME TO WAIT, RETURN FAILURE
    if ((GetTickCount()-starttime > 180000) && !(flags & WAIT_FLAG_INFINITE)) {
      InterlockedDecrement(&srv_waiting);
      DWORD error = SN_ERROR_UNREACHABLE;
      QueueUiNotification(SN_FAILEDTOCONNECT,&error,sizeof(DWORD),NULL,NULL);
      SetLastError(SNET_ERROR_HOST_UNREACHABLE);
      return 0;
    }

    // WAIT FOR AN EVENT, TIMER, OR WINDOW MESSAGE
    if (!WaitOnce(flags)) {
      // WAITONCE() IS RESPONSIBLE FOR CALLING SETLASTERROR()
      InterlockedDecrement(&srv_waiting);
      return 0;
    }


  }
  srv_responded[sid] = 0;
  InterlockedDecrement(&srv_waiting);
  return 1;
}

//===========================================================================
static BOOL WaitOnce (DWORD flags) {

  // WAIT UNTIL WE NEED TO UPDATE THE TIMERS OR DISPATCH A MESSAGE
  DWORD result;
  if (flags & WAIT_FLAG_NOMESSAGELOOP)
    result = WaitForSingleObject(srv_waitevent,1000);
  else
    result = MsgWaitForMultipleObjects(1,
                                       &srv_waitevent,
                                       0,
                                       10,
                                       QS_ALLINPUT);

  // DISPATCH ANY PENDING WINDOW MESSAGES
  if ((result != WAIT_OBJECT_0) &&
      !(flags & WAIT_FLAG_NOMESSAGELOOP))
    if (!UiProcessWindowMessages()) {
      SetLastError(SNET_ERROR_CANCELLED);
      return 0;
    }

  // IF THE SOCKET IS NOT CONNECTED ANYMORE, OR THE USER HAS CANCELLED THE
  // OPERATION, RETURN FAILURE
  if (srv_cancelwait) {
    srv_cancelwait = 0;
    SetLastError(SNET_ERROR_CANCELLED);
    return 0;
  }
  if (!srv_serversocket)
    return 0;
  sockaddr_in addr;
  int         addrlen = sizeof(sockaddr_in);
  if (getpeername(srv_serversocket,(sockaddr *)&addr,&addrlen) == SOCKET_ERROR) {
    DWORD error = SN_ERROR_UNREACHABLE;
    QueueUiNotification(SN_FAILEDTOCONNECT,&error,sizeof(DWORD),NULL,NULL);
    SetLastError(SNET_ERROR_HOST_UNREACHABLE);
    return 0;
  }

  return 1;
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
BOOL SrvBeginChat (SNETPROGRAMDATAPTR programdata,
                   SNETPLAYERDATAPTR  playerdata,
                   LPCSTR             preferredchannel) {
  if (!srv_connected)
    return 0;
  srv_inchat = 1;

  // REQUEST THE ITEMS THAT WE NEED:
  //   - A UNIQUE USER NAME
  //   - A LIST OF CHANNELS
  //   - A STARTING CHANNEL TO JOIN
  {
    char namedesc[2*SNETSPI_MAXSTRINGLENGTH] = "";
    SStrCopy(namedesc,srv_username,SNETSPI_MAXSTRINGLENGTH);
    SStrCopy(namedesc+strlen(namedesc)+1,srv_userdesc,SNETSPI_MAXSTRINGLENGTH);
    if (!SendServerMessage(SID_ENTERCHAT,namedesc,strlen(namedesc)+strlen(namedesc+strlen(namedesc)+1)+2))
      return 0;
  }
  if (!SendServerMessage(SID_GETCHANNELLIST,&programdata->programid,sizeof(DWORD)))
    return 0;
  {
    struct {
        DWORD flags;
        char  channelname[SNETSPI_MAXSTRINGLENGTH];
    } req;
    ZeroMemory(&req,sizeof(req));
    if (preferredchannel && *preferredchannel) {
      req.flags = JCF_JOINALWAYS;
      strncpy(req.channelname,preferredchannel,SNETSPI_MAXSTRINGLENGTH);
    }
    else {
      req.flags = JCF_DEFAULTCHANNEL;
      strncpy(req.channelname,programdata->programname,SNETSPI_MAXSTRINGLENGTH);
    }
    req.channelname[SNETSPI_MAXSTRINGLENGTH-1] = 0;
    SendServerMessage(SID_JOINCHANNEL,
                      &req,
                      sizeof(DWORD)+strlen(req.channelname)+1);
  }

  // WAIT FOR THE RESPONSES
  if (!WaitForServerResponse(SID_ENTERCHAT,0))
    return 0;
  if (!WaitForServerResponse(SID_GETCHANNELLIST,0))
    return 0;
  if (!WaitForServerResponse(SID_CHATEVENT,0))
    return 0;

  return 1;
}

//===========================================================================
void SrvCancel () {
  srv_cancelwait = 1;
}

//===========================================================================
BOOL SrvCreateAccount (LPCSTR username,
                       LPCSTR password) {
  return TRUE;
}

//===========================================================================
void SrvDestroy () {

  // DESTROY THE THREADS
  srv_shutdown = 1;
  SetEvent(srv_shutdownevent);
  {
    DWORD phase     = 0;
    DWORD starttime = GetTickCount();
    do {
      Sleep(10);
      DWORD currtime = GetTickCount();
      DWORD newphase = (currtime-starttime)/500;
      if (srv_connectthreads && (newphase != phase)) {
        phase = newphase;
        DestroyConnectThreads(phase);
      }
    } while (srv_connectthreads);
    do
      Sleep(10);
    while (srv_downloadthreads);
  }
  SrvDisconnect();
  if (srv_keepalivethread)
    WaitForSingleObject(srv_keepalivethread,INFINITE);
  if (srv_serverthread)
    WaitForSingleObject(srv_serverthread,INFINITE);
  CloseHandle(srv_keepalivethread);
  CloseHandle(srv_serverthread);
  CloseHandle(srv_waitevent);
  CloseHandle(srv_shutdownevent);
  srv_keepalivethread = (HANDLE)0;
  srv_serverthread    = (HANDLE)0;
  srv_waitevent       = (HANDLE)0;
  srv_shutdownevent   = (HANDLE)0;
  srv_shutdown        = 0;

  // DESTROY THE LIST OF PING USERS
  RemovePingUser(NULL,NULL);

  // DESTROY ALL PENDING UI NOTIFICATIONS
  srv_uinotificationcritsect.Enter();
  while (srv_uinotificationhead) {
    FREE(srv_uinotificationhead->buffer);
    UINOTIFICATIONPTR next = srv_uinotificationhead->next;
    FREE(srv_uinotificationhead);
    srv_uinotificationhead = next;
  }
  if (srv_uinotificationhold) {
    FREE(srv_uinotificationhold);
    srv_uinotificationhold = NULL;
  }
  srv_uinotificationcritsect.Leave();

  // RESET THE MODULE FOR THE NEXT INITIALIZATION
  srv_serverthread = (HANDLE)0;
  srv_waitevent    = (HANDLE)0;

}

//===========================================================================
BOOL SrvDisconnect () {
  if (srv_connected || srv_serversocket)
    DisconnectFromServer();
  return 1;
}

//===========================================================================
BOOL SrvEndChat () {
  if (!srv_connected)
    return 0;
  if (!SendServerMessage(SID_LEAVECHAT,NULL,0))
    return 0;
  srv_inchat = 0;
  return 1;
}

//===========================================================================
BOOL SrvGetGameList (LPCSTR gamename,
                     LPCSTR gamepassword,
                     DWORD  categorybits,
                     DWORD  categorymask,
                     DWORD  maxitems) {
  if (!gamename)
    gamename = "";
  if (!gamepassword)
    gamepassword = "";

  // REQUEST A LIST OF PUBLIC GAMES
  struct {
      DWORD categorybits;
      DWORD categorymask;
      DWORD reserved;
      DWORD maxitems;
      char  namepasscriteria[3*SNETSPI_MAXSTRINGLENGTH];
  } req;
  req.categorybits = categorybits;
  req.categorymask = categorymask;
  req.reserved     = 0;
  req.maxitems     = maxitems;
  LPSTR curr = req.namepasscriteria;
  strncpy(curr,gamename,SNETSPI_MAXSTRINGLENGTH);
  curr += strlen(curr)+1;
  strncpy(curr,gamepassword,SNETSPI_MAXSTRINGLENGTH);
  curr += strlen(curr)+1;
  strncpy(curr,"",SNETSPI_MAXSTRINGLENGTH);
  curr += strlen(curr)+1;
  DWORD length = 4*sizeof(DWORD)+curr-req.namepasscriteria;
  if (!SendServerMessage(SID_GETADVLISTEX,&req,length))
    return 0;

  // WAIT FOR THE SERVER'S RESPONSE
  if (!WaitForServerResponse(SID_GETADVLISTEX,0))
    return 0;

  return 1;
}

//===========================================================================
BOOL SrvGetLatency (SNETADDRPTR  addr,
                    DWORD       *latency) {
  if (latency)
    *latency = 0;

  // NORMALIZE THE ADDRESS TO POINT TO THE DATA PORT
  sockaddr_in dataportaddr;
  if (addr) {
    CopyMemory(&dataportaddr,addr,sizeof(sockaddr_in));
    dataportaddr.sin_port = htons(DATAPORT);
    ZeroMemory(&dataportaddr.sin_zero[0],8);
  }

  // FIND THIS ADDRESS
  srv_pingcritsect.Enter();
  PINGPTR curr = srv_pinghead;
  while (curr &&
         memcmp(&dataportaddr,&curr->addr,sizeof(sockaddr_in)))
    curr = curr->next;
  if (curr)
    *latency = curr->latency;
  srv_pingcritsect.Leave();

  return (curr != NULL);
}

//===========================================================================
void SrvGetLocalPlayerDesc (LPSTR buffer,
                            DWORD bufferchars) {
  SStrCopy(buffer,srv_userdesc,bufferchars);
}

//===========================================================================
void SrvGetLocalPlayerName (LPSTR buffer,
                            DWORD bufferchars) {
  SStrCopy(buffer,srv_username,bufferchars);
}

//===========================================================================
BOOL SrvGetUiNotification (DWORD  *notifycode,
                           LPVOID *param,
                           DWORD  *parambytes) {
  if (notifycode)
    *notifycode = 0;
  if (param)
    *param = NULL;
  if (parambytes)
    *parambytes = 0;

  // ENTER THE CRITICAL SECTION
  srv_uinotificationcritsect.Enter();

  // FREE THE LAST NOTIFICATION WE RETURNED
  if (srv_uinotificationhold) {
    FREE(srv_uinotificationhold);
    srv_uinotificationhold = NULL;
  }

  // CHECK TO SEE IF ANOTHER IS AVAILABLE
  if (!srv_uinotificationhead) {
    srv_uinotificationcritsect.Leave();
    return 0;
  }

  // RETURN THE NEXT NOTIFICATION
  *notifycode = srv_uinotificationhead->notifycode;
  *param      = srv_uinotificationhead->buffer;
  *parambytes = srv_uinotificationhead->parambytes;

  // REMOVE THIS NOTIFICATION FROM THE LIST, AND PLACE ITS DATA IN THE
  // HOLD AREA
  srv_uinotificationhold = srv_uinotificationhead->buffer;
  UINOTIFICATIONPTR freenotification = srv_uinotificationhead;
  srv_uinotificationhead = freenotification->next;
  FREE(freenotification);

  // LEAVE THE CRITICAL SECTION
  srv_uinotificationcritsect.Leave();

  return 1;
}

//===========================================================================
BOOL SrvInitialize (SNETPROGRAMDATAPTR programdata,
                    SNETPLAYERDATAPTR  playerdata,
                    SNETVERSIONDATAPTR versiondata) {
#define FAILOUT(uierr,sneterr)                      \
          do {                                      \
            DWORD error = uierr;                    \
            QueueUiNotification(SN_FAILEDTOCONNECT, \
                                &error,             \
                                sizeof(DWORD),      \
                                NULL,               \
                                NULL);              \
            SetLastError(sneterr);                  \
            return 0;                               \
          } while (0)

  // CONNECT TO THE SERVER
  if (!ConnectToServer())
    return 0;

  // CREATE AN EVENT FOR WAITING ON SERVER RESPONSS
  srv_waitevent = CreateEvent(NULL,0,0,NULL);

  // CREATE A THREAD TO PROCESS SERVER DATA
  unsigned threadid;
  srv_serverthread = (HANDLE)_beginthreadex((LPSECURITY_ATTRIBUTES)NULL,
                                            0,
                                            ServerThreadProc,
                                            NULL,
                                            0,
                                            &threadid);
  if (!srv_serverthread)
    return 0;

  // SEND THE CONNECTION TYPE TO BATTLE.NET
  BYTE conntype = CONNTYPE_CLIENT;
  if (send(srv_serversocket,
           (const char *)&conntype,
           sizeof(BYTE),
           0) != sizeof(BYTE))
    return 0;

  // SEND OUR CLIENT ID TO BATTLE.NET
  {
    struct {
      DWORD registrationversion;
      DWORD registrationauthority;
      DWORD clientid;
      DWORD checkvalue;
      char  computeraccount[SNETSPI_MAXSTRINGLENGTH+MAX_COMPUTERNAME_LENGTH+3];
    } req;
    ZeroMemory(&req,sizeof(req));
    SRegLoadValue(CONFIGREGKEY,CONFIGREGVALUE_REGVER     ,SREG_FLAG_BATTLENET,&req.registrationversion);
    SRegLoadValue(CONFIGREGKEY,CONFIGREGVALUE_REGAUTH    ,SREG_FLAG_BATTLENET,&req.registrationauthority);
    SRegLoadValue(CONFIGREGKEY,CONFIGREGVALUE_CLIENTID   ,SREG_FLAG_BATTLENET,&req.clientid);
    SRegLoadValue(CONFIGREGKEY,CONFIGREGVALUE_CLIENTCHECK,SREG_FLAG_BATTLENET,&req.checkvalue);
    DWORD size = MAX_COMPUTERNAME_LENGTH+1;
    GetComputerName(req.computeraccount,&size);
    req.computeraccount[MAX_COMPUTERNAME_LENGTH] = 0;
    LPSTR curr = req.computeraccount+strlen(req.computeraccount)+1;
    size = SNETSPI_MAXSTRINGLENGTH;
    GetUserName(curr,&size);
    curr += strlen(curr)+1;
    if (!SendServerMessage(SID_CLIENTID,
                           &req,
                           4*sizeof(DWORD)+curr-req.computeraccount))
      return 0;
  }

  // SEND OUR LOCALE INFORMATION TO BATTLE.NET
  {
    TIME_ZONE_INFORMATION tzinfo;
    ZeroMemory(&tzinfo,sizeof(TIME_ZONE_INFORMATION));
    GetTimeZoneInformation(&tzinfo);
    SYSTEMTIME sysutctime;
    SYSTEMTIME syslocaltime;
    GetSystemTime(&sysutctime);
    GetLocalTime(&syslocaltime);
    struct {
        FILETIME utctime;
        FILETIME localtime;
        LONG     bias;
        DWORD    systemlcid;
        DWORD    userlcid;
        DWORD    languageid;
        char     strings[256];
    } req;
    SystemTimeToFileTime(&sysutctime,&req.utctime);
    SystemTimeToFileTime(&syslocaltime,&req.localtime);
    req.bias       = tzinfo.Bias;
    req.systemlcid = GetSystemDefaultLCID();
    req.userlcid   = GetUserDefaultLCID();
    req.languageid = GetUserDefaultLangID();
    req.strings[0] = 0;
    LPSTR curr = req.strings;
    GetLocaleInfo(LOCALE_USER_DEFAULT,LOCALE_SABBREVLANGNAME,curr,64);
    curr += strlen(curr)+1;
    GetLocaleInfo(LOCALE_USER_DEFAULT,LOCALE_ICOUNTRY,curr,64);
    curr += strlen(curr)+1;
    GetLocaleInfo(LOCALE_USER_DEFAULT,LOCALE_SABBREVCTRYNAME,curr,64);
    curr += strlen(curr)+1;
    GetLocaleInfo(LOCALE_USER_DEFAULT,LOCALE_SENGCOUNTRY,curr,64);
    curr += strlen(curr)+1;
    DWORD length = 2*sizeof(FILETIME)+4*sizeof(DWORD)+curr-req.strings;
    if (!SendServerMessage(SID_LOCALEINFO,&req,length))
      return 0;
  }

  // SEND THE PLATFORM AND PROGRAM IDENTIFIERS TO THE BATTLE.NET SERVER
  srv_versionfile[0] = 0;
  srv_responded[SID_REPORTVERSION] = 0;
  {
    struct {
        DWORD platformid;
        DWORD programid;
        DWORD versionid;
        DWORD reserved;
    } req;
    ZeroMemory(&req,sizeof(req));
    req.platformid = PLATFORMID;
    req.programid  = programdata->programid;
    req.versionid  = programdata->versionid;
    if (!SendServerMessage(SID_STARTVERSIONING,&req,sizeof(req)))
      return 0;
  }

  // WAIT FOR THE SERVER TO EITHER SEND US A VERSIONING FILENAME OR TO
  // AUTHENTICATE US WITHOUT REQUIRING VERSIONING
  while (!(srv_versionfile[0] || srv_responded[SID_REPORTVERSION]))
    if (!WaitOnce(0))
      return 0;
  if (!srv_responded[SID_REPORTVERSION]) {

    // DETERMINE THE VERSIONING INFORMATION
    DWORD revisionid;
    DWORD checkvalue;
    char  comment[256] = "";
    if (!CheckVersion(&revisionid,&checkvalue,comment))
      FAILOUT(SN_ERROR_UNABLETOUPGRADE,SNET_ERROR_HOST_UNREACHABLE);

    // RESET OUT PATCH STATUS, IN CASE THE SERVER NEEDS TO SEND US A PATCH
    srv_patchfiles[0]     = 0;
    srv_patchfiles[1]     = 0;
    srv_patchhighcomplete = 0;
    srv_patchpercent      = 0;

    // SEND IT TO THE SERVER
    {
      struct {
          DWORD platformid;
          DWORD programid;
          DWORD versionid;
          DWORD revisionid;
          DWORD checkvalue;
          char  comment[256];
      } req;
      ZeroMemory(&req,sizeof(req));
      req.platformid = PLATFORMID;
      req.programid  = programdata->programid;
      req.versionid  = programdata->versionid;
      req.revisionid = revisionid;
      req.checkvalue = checkvalue;
      strncpy(req.comment,comment,255);
      req.comment[255] = 0;
      if (!SendServerMessage(SID_REPORTVERSION,&req,5*sizeof(DWORD)+strlen(comment)+1))
        return 0;
    }

    // WAIT FOR THE SERVER TO RESPOND WITH A CODE TELLING US WHETHER WE NEED
    // TO UPGRADE
    if (!WaitForServerResponse(SID_REPORTVERSION,0))
      return 0;

  }

  // IF THE SERVER REJECTED US, REPORT THAT TO THE USER
  if (srv_authenticated == VER_RESULT_BADVERSION)
    FAILOUT(SN_ERROR_UNABLETOUPGRADE,SNET_ERROR_HOST_UNREACHABLE);

  // IF THE SERVER REPORTED THAT WE NEED TO UPGRADE, ASK THE USER WHETHER
  // TO UPGRADE NOW
  if (srv_authenticated == VER_RESULT_UPGRADEREQUIRED) {
    if (!UiUpgradeMessage()) {
      SetLastError(SNET_ERROR_CANCELLED);
      return 0;
    }

    // DOWNLOAD THE UPGRADE
    DWORD lastpercent = 0xFFFFFFFF;
    while (srv_patchfiles[0])
      if (WaitOnce(0)) {
        if (srv_patchpercent != lastpercent) {
          lastpercent = srv_patchpercent;
          QueueUiNotification(SN_DOWNLOADINGUPGRADE,&srv_patchpercent,sizeof(DWORD),NULL,NULL);
        }
      }
      else
        return 0;

    // IF THE METER NEVER DREW ON 100%, FORCE IT TO DO SO MOMENTARILY
    {
      srv_patchpercent = 100;
      QueueUiNotification(SN_DOWNLOADINGUPGRADE,&srv_patchpercent,sizeof(DWORD),NULL,NULL);
      DWORD starttime = GetTickCount();
      do
        WaitOnce(0);
      while (GetTickCount()-starttime < 200);
    }

    // PROCEED WITH APPLYING THE PATCH
    QueueUiNotification(SN_DOWNLOADSUCCEEDED,NULL,0,NULL,NULL);
    SRegSaveString(PATCHREGKEY,PATCHREGVALUE_LAUNCHER,SREG_FLAG_BATTLENET,versiondata->executablefile);
    SRegSaveString(PATCHREGKEY,PATCHREGVALUE_SRCDATA ,SREG_FLAG_BATTLENET,versiondata->originalarchivefile);
    SRegSaveString(PATCHREGKEY,PATCHREGVALUE_DSTDATA ,SREG_FLAG_BATTLENET,versiondata->patcharchivefile);
    SetLastError(SNET_ERROR_REQUIRES_UPGRADE);
    return 0;
  }

  // IF WE RECEIVED THE SERVER'S UDP PING, REPORT THE PING DATA BACK TO THE
  // SERVER
  if (srv_udppingdata) {
    if (!SendServerMessage(SID_UDPPINGRESPONSE,&srv_udppingdata,sizeof(DWORD)))
      return 0;
  }

  // OTHERWISE, DISPLAY AN ERROR MESSAGE TO THE USER INFORMING HIM THAT
  // HIS INTERNET CONNECTION IS BROKEN AND HE WON'T BE ABLE TO PLAY GAMES
  else
    QueueUiNotification(SN_BADCONNECTION,NULL,0,NULL,NULL);

  // CREATE A THREAD TO KEEP THE CONNECTION ALIVE
  srv_keepalivethread = (HANDLE)_beginthreadex((LPSECURITY_ATTRIBUTES)NULL,
                                               0,
                                               KeepAliveThreadProc,
                                               NULL,
                                               0,
                                               &threadid);

#undef FAILOUT
  srv_connected = 1;
  return 1;
}

//===========================================================================
BOOL SrvIsConnected () {
  return srv_connected;
}

//===========================================================================
BOOL SrvIsWaitingForResponse () {
  return (srv_waiting != 0);
}

//===========================================================================
BOOL SrvJoinChannel (LPCSTR channel, BOOL joinalways) {
  struct {
      DWORD flags;
      char  channelname[SNETSPI_MAXSTRINGLENGTH];
  } req;
  ZeroMemory(&req,sizeof(req));
  req.flags = joinalways ? JCF_JOINALWAYS : 0;
  strncpy(req.channelname,channel,SNETSPI_MAXSTRINGLENGTH);
  req.channelname[SNETSPI_MAXSTRINGLENGTH-1] = 0;
  return SendServerMessage(SID_JOINCHANNEL,
                           &req,
                           sizeof(DWORD)+strlen(req.channelname)+1);
}

//===========================================================================
BOOL SrvLogon (LPCSTR  username,
               LPCSTR  password,
               DWORD  *errorcode) {
  SStrCopy(srv_username,username,MAXSTRINGLENGTH);
  srv_userdesc[0] = 0;
  *errorcode = 0;
  return TRUE;
}

//===========================================================================
void SrvMaintainAds () {

#if STARCRAFT_BNBETA
    return;  // current ads don't fit new ad size 
#endif

  // CHECK THE AD STATE ONCE EVERY FIFTEEN SECONDS
  DWORD currtime = GetTickCount();
  {
    static DWORD lasttime = 0;
    if (currtime-lasttime < 15000)
      return;
    lasttime = currtime;
  }

  // SEND AN AD CHECK MESSAGE TO THE SERVER
  struct {
    DWORD platformid;
    DWORD programid;
    DWORD currentad;
    DWORD runningtime;
  } req;
  req.platformid  = PLATFORMID;
  req.programid   = global_programid;
  req.currentad   = srv_adnumber;
  req.runningtime = time(NULL)-srv_addisplaytime;
  SendServerMessage(SID_CHECKAD,&req,sizeof(req));

}

//===========================================================================
void SrvMaintainLatencies () {
  DWORD currtime = GetTickCount();

  // IF THERE ARE ANY USERS THAT WE HAVE NEVER RECEIVED A PING RESPONSE FROM,
  // AND WHOSE PING WE SENT MORE THAN A SECOND AGO, MAX THEIR LATENCY OUT AT
  // ONE SECOND
  {
    srv_pingcritsect.Enter();
    PINGPTR curr = srv_pinghead;
    while (curr) {
      if ((currtime-curr->lastpingtime > MAXLATENCY) &&
          !curr->lastresponsetime) {
        curr->latency          = MAXLATENCY;
        curr->lastresponsetime = currtime;
        SNUPDATEPINGTIMEREC rec;
        rec.name     = curr->username;
        rec.pingtime = curr->latency;
        rec.relayed  = 0;
        QueueUiNotification(SN_UPDATEPINGTIME,&rec,sizeof(SNUPDATEPINGTIMEREC),&rec.name,NULL);
      }
      curr = curr->next;
    }
    srv_pingcritsect.Leave();
  }

  // RE-PING THOSE USERS WHICH HAVE A USER NAME (MEANING THEY'RE IN A CHAT
  // CHANNEL) AND WHICH WE LAST PINGED MORE THAN A MINUTE AGO
  {
    srv_pingcritsect.Enter();
    PINGPTR curr = srv_pinghead;
    while (curr) {
      if ((curr->username[0]) &&
          (currtime-curr->lastpingtime > PINGFREQUENCY)) {
        curr->lastpingtime = currtime;
        DWORD request = CLI_PING;
        SpiSendSpecial((SNETADDRPTR)&curr->addr,
                       PKT_CLIENTREQ,
                       &request,
                       sizeof(DWORD));
      }
      curr = curr->next;
    }
    srv_pingcritsect.Leave();
  }

}

//===========================================================================
void SrvNotifyClickAd (DWORD adid,
                       BOOL  result) {
  struct {
    DWORD adid;
    DWORD action;
  } req;
  req.adid   = adid;
  req.action = (result != 0);
  SendServerMessage(SID_CLICKAD,&req,sizeof(req));
}

//===========================================================================
void SrvNotifyDisplayAd (DWORD  id,
                         LPCSTR filename,
                         LPCSTR url) {

  // SEND THE MESSAGE TO THE SERVER
  struct {
      DWORD  platformid;
      DWORD  programid;
      DWORD  adid;
      char   filenameurl[2*MAX_PATH];
  } req;
  req.platformid = PLATFORMID;
  req.programid  = global_programid;
  req.adid       = id;
  LPSTR curr = req.filenameurl;
  strncpy(curr,filename,SNETSPI_MAXSTRINGLENGTH);
  curr += strlen(curr)+1;
  strncpy(curr,url,SNETSPI_MAXSTRINGLENGTH);
  curr += strlen(curr)+1;
  DWORD length = 3*sizeof(DWORD)+curr-req.filenameurl;
  SendServerMessage(SID_DISPLAYAD,&req,length);

}

//===========================================================================
void SrvNotifyJoin (LPCSTR gamename,
                    LPCSTR gamepassword) {

  // SEND THE MESSAGE TO THE SERVER
  struct {
      DWORD programid;
      DWORD versionid;
      char  namepass[2*SNETSPI_MAXSTRINGLENGTH];
  } req;
  req.programid = global_programid;
  req.versionid = global_versionid;
  LPSTR curr = req.namepass;
  strncpy(curr,gamename,SNETSPI_MAXSTRINGLENGTH);
  curr += strlen(curr)+1;
  strncpy(curr,gamepassword,SNETSPI_MAXSTRINGLENGTH);
  curr += strlen(curr)+1;
  DWORD length = 2*sizeof(DWORD)+curr-req.namepass;
  SendServerMessage(SID_NOTIFYJOIN,&req,length);

}

//===========================================================================
void SrvPingAddress (SNETADDRPTR addr) {

  // DON'T PING THIS USER IF WE ALREADY HAVE A LATENCY FOR HIM
  {
    DWORD latency;
    if (SrvGetLatency(addr,&latency))
      return;
  }

  // ADD THE USER TO THE PING LIST
  AddPingUser(NULL,(sockaddr_in *)addr);

}

//===========================================================================
void SrvProcessClientReq (SNETADDRPTR addr,
                          LPBYTE      data,
                          DWORD       databytes) {
  if (databytes < sizeof(DWORD))
    return;
  DWORD req = *(LPDWORD)data;
  data += sizeof(DWORD);
  switch (req) {

    case CLI_PING:
      {
        DWORD response = CLI_PINGRESPONSE;
        SpiSendSpecial(addr,PKT_CLIENTREQ,&response,sizeof(DWORD));
      }
      break;

    case CLI_PINGRESPONSE:
      {
        srv_pingcritsect.Enter();
        PINGPTR userptr = srv_pinghead;
        while (userptr &&
               memcmp(&userptr->addr,addr,sizeof(sockaddr_in)))
          userptr = userptr->next;
        if (userptr) {
          DWORD currtime = GetTickCount();
          DWORD latency  = currtime-userptr->lastpingtime;
          if (userptr->latency)
            userptr->latency = (userptr->latency*3+latency)/4;
          else
            userptr->latency = latency;
          userptr->lastresponsetime = currtime;
          if (userptr->username[0]) {
            SNUPDATEPINGTIMEREC rec;
            rec.name     = userptr->username;
            rec.pingtime = userptr->latency;
            rec.relayed  = 0;
            QueueUiNotification(SN_UPDATEPINGTIME,&rec,sizeof(SNUPDATEPINGTIMEREC),&rec.name,NULL);
          }
        }
        srv_pingcritsect.Leave();
      }
      break;

  }
}

//===========================================================================
void SrvProcessServerPing (LPBYTE data,
                           DWORD  databytes) {
  if (databytes < sizeof(DWORD))
    return;
  srv_udppingdata = *(LPDWORD)data;
}

//===========================================================================
BOOL SrvSendChatString (LPCSTR command) {
  return SendServerMessage(SID_CHATCOMMAND,(LPVOID)command,strlen(command)+1);
}

//===========================================================================
void SrvSetBetaId (DWORD betaid) {
  // this function is obsolete
}

//===========================================================================
BOOL SrvStartAdvertisingGame (LPCSTR gamename,
                              LPCSTR gamepassword,
                              LPCSTR gamedescription,
                              DWORD  gamemode,
                              DWORD  gameage,
                              DWORD  gamecategorybits,
                              DWORD  optcategorybits) {
  srv_startadvsuccess = 0;

  // SEND THE REQUEST TO THE SERVER
  struct {
      DWORD gamemode;
      DWORD gameage;
      DWORD categorybits;
      DWORD optcategorybits;
      DWORD reserved;
      char  namepassdesc[3*SNETSPI_MAXSTRINGLENGTH];
  } req;
  req.gamemode        = gamemode;
  req.gameage         = gameage;
  req.categorybits    = gamecategorybits;
  req.optcategorybits = optcategorybits;
  req.reserved        = 0;
  LPSTR curr = req.namepassdesc;
  strncpy(curr,gamename,SNETSPI_MAXSTRINGLENGTH);
  curr += strlen(curr)+1;
  strncpy(curr,gamepassword,SNETSPI_MAXSTRINGLENGTH);
  curr += strlen(curr)+1;
  strncpy(curr,gamedescription,SNETSPI_MAXSTRINGLENGTH);
  curr += strlen(curr)+1;
  DWORD length = 5*sizeof(DWORD)+curr-req.namepassdesc;
  if (!SendServerMessage(SID_STARTADVEX,&req,length))
    return 0;

  // IF THIS GAME HAS PREVIOUSLY BEEN ADVERTISED, ASSUME SUCCESS
  if (gamemode & SNET_GM_ADVERTISED)
    return 1;

  // OTHERWISE, WAIT FOR A RESPONSE FROM THE SERVER
  if (!WaitForServerResponse(SID_STARTADVEX,WAIT_FLAG_NOMESSAGELOOP)) {
    SetLastError(SNET_ERROR_HOST_UNREACHABLE);
    return 0;
  }

  if (!srv_startadvsuccess)
    SetLastError(SNET_ERROR_ALREADY_EXISTS);
  return srv_startadvsuccess;
}

//===========================================================================
BOOL SrvStopAdvertisingGame () {
  if (!SendServerMessage(SID_STOPADV,NULL,0))
    return 0;
  return 1;
}


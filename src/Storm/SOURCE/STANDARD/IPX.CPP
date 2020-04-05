/****************************************************************************
*
*  IPX.CPP
*  IPX network provider
*
*  By Michael O'Brien (4/22/96)
*
***/

#include "pch.h"
#pragma  hdrstop

#define  ADVPORT          6111
#define  MAINPORT         6112
#define  MAXMESSAGESIZE   504
#define  MAXPLAYERS       256
#define  PROVIDERID       'IPXN'
#define  RECVDATATHREADS  2

#define  ADTYPE_GAMEINFO  0
#define  ADTYPE_REMOVE    1
#define  ADTYPE_REQUEST   2

typedef struct _ADVHEADER {
    WORD  checksum; // must be first field
    WORD  length;
    WORD  type;
    WORD  reserved;
    DWORD programid;
    DWORD versionid;
    DWORD gamemode;
} ADVHEADER, *ADVHEADERPTR;

typedef struct _ADVPACKET {
    ADVHEADER header;
    char      strings[SNETSPI_MAXSTRINGLENGTH*2];
    BYTE      clientdatabuffer[SNETSPI_MAXCLIENTDATA];
} ADVPACKET, *ADVPACKETPTR;

typedef struct _PACKET {
    SNETADDR  addr; // must be first field in structure
    BYTE      data[MAXMESSAGESIZE];
    DWORD     databytes;
    _PACKET  *next;
} PACKET, *PACKETPTR;

typedef struct _UIPARAMS {
    DWORD              flags;
    SNETPROGRAMDATAPTR programdata;
    SNETPLAYERDATAPTR  playerdata;
    SNETUIDATAPTR      interfacedata;
    SNETVERSIONDATAPTR versiondata;
    LPDWORD            playeridptr;
} UIPARAMS, *UIPARAMSPTR;

typedef struct _THREAD {
    unsigned  id;
    HANDLE    handle;
    _THREAD  *next;
} THREAD, *THREADPTR;

typedef struct _WINSOCKAPI {
    int     (APIENTRY *bind       )(SOCKET,const sockaddr *,int);
    int     (APIENTRY *closesocket)(SOCKET);
    u_short (APIENTRY *htons      )(u_short);
    int     (APIENTRY *recvfrom   )(SOCKET,char *,int,int,sockaddr *,int *);
    int     (APIENTRY *sendto     )(SOCKET,const char *,int,int,const sockaddr *,int);
    int     (APIENTRY *setsockopt )(SOCKET,int,int,const char *,int);
    SOCKET  (APIENTRY *socket     )(int,int,int);
    int     (APIENTRY *WSAStartup )(WORD,LPWSADATA);
    int     (APIENTRY *WSACleanup )();
} WINSOCKAPI, *WINSOCKAPIPTR;

static ADVPACKETPTR        ipx_advgameinfo   = NULL;
static SOCKET              ipx_advsocket     = (SOCKET)0;
static SOCKADDR_IPX        ipx_broadcastaddr = {0};
static CCritSect           ipx_critsect;
static SNETSPI_GAMELISTPTR ipx_gamehead      = NULL;
static SOCKADDR_IPX        ipx_localaddr     = {0};
static DWORD               ipx_nextgameid    = 0;
static SOCKET              ipx_mainsocket    = (SOCKET)0;
static DWORD               ipx_maxplayers    = MAXPLAYERS;
static PACKETPTR           ipx_packethead    = NULL;
static PACKETPTR           ipx_pendingreadpkt[RECVDATATHREADS] = {NULL,NULL};
static DWORD               ipx_programid     = 0;
static HANDLE              ipx_recvevent     = NULL;
static BOOL                ipx_shutdown      = 0;
static THREADPTR           ipx_threadhead    = NULL;
static DWORD               ipx_versionid     = 0;
static WINSOCKAPIPTR       ipx_winsockapi    = NULL;
static HINSTANCE           ipx_winsocklib    = (HINSTANCE)0;

static void SendAdvertisement (SOCKADDR_IPX *addr);
static void SendRequest ();
static void TrimGameList (DWORD timeout);
static void UpdateGameList (HWND dialog, HWND listbox);
BOOL CALLBACK IpxCompareNetAddresses (SNETADDRPTR  addr1,
                                      SNETADDRPTR  addr2,
                                      DWORD       *diffmagnitude);
BOOL CALLBACK IpxStopAdvertisingGame ();

//===========================================================================
static BOOL inline BindToWinsock () {

  // LOAD THE WINSOCK LIBRARY
  if (!ipx_winsocklib) {
    ipx_winsocklib = LoadLibrary(TEXT("wsock32.dll"));
    if (!ipx_winsocklib)
      return 0;
  }

  // ALLOCATE AN API STRUCTURE
  if (!ipx_winsockapi) {
    ipx_winsockapi = NEW(WINSOCKAPI);
    if (!ipx_winsockapi)
      return 0;
  }

  // BIND TO THE INDIVIDUAL FUNCTIONS
  BOOL success = 1;
#define  BIND(a,b)  *(void **)&ipx_winsockapi->##a                               \
                      = GetProcAddress(ipx_winsocklib,(LPCSTR)MAKELONG((b),0)); \
                    if (!ipx_winsockapi->##a)                                    \
                      success = 0;
  BIND(bind       ,2);
  BIND(closesocket,3);
  BIND(htons      ,9);
  BIND(recvfrom   ,17);
  BIND(sendto     ,20);
  BIND(setsockopt ,21);
  BIND(socket     ,23);
  BIND(WSAStartup ,115);
  BIND(WSACleanup ,116);
#undef  BIND
  return success;
}

//===========================================================================
static WORD ComputeChecksum (LPVOID data, DWORD databytes) {
  DWORD  checkval1 = 0;
  DWORD  checkval2 = 0;
  LPBYTE ptr       = ((LPBYTE)data)+databytes-1;
  while (databytes--) {
    checkval1 += *ptr--;
    if (checkval1 >= 0xFF)
      checkval1 -= 0xFF;
    checkval2 += checkval1;
  }
  checkval2 %= 255;
  return MAKEWORD((checkval2 & 0xFF),(checkval1 & 0xFF));
}

//===========================================================================
static WORD GenerateChecksum (LPVOID packet, DWORD bytes) {

  // COMPUTE THE CURRENT CHECKSUM FOR THE MESSAGE
  WORD checksum = ComputeChecksum(((LPBYTE)packet)+sizeof(WORD),
                                  bytes-sizeof(WORD));

  // COMPUTE A NEW VALUE FOR THE CHECKSUM FIELD THAT WILL MAKE THE NEW
  // CHECKSUM OF THE ENTIRE MESSAGE ZERO
  BYTE hibyte = 0xFF-((checksum >> 8)+(checksum & 0xFF)) % 0xFF;
  BYTE lobyte = 0xFF-((checksum >> 8)+hibyte)            % 0xFF;
  return MAKEWORD(lobyte,hibyte);
}

//===========================================================================
static BOOL LoadArtwork (SNETGETARTPROC  artcallback,
                         DWORD           providerid,
                         DWORD           artid,
                         BOOL            setpalette,
                         LPBYTE         *data,
                         SIZE           *size) {
  *data    = 0;
  size->cx = 0;
  size->cy = 0;

  // VERIFY THAT THE APPLICATION HAS REGISTERED AN ARTWORK CALLBACK
  if (!artcallback)
    return 0;

  // CALL THE ARTWORK CALLBACK TO DETERMINE THE IMAGE DIMENSIONS
  int width;
  int height;
  int bitdepth;
  if (!artcallback(providerid,
                   artid,
                   NULL,
                   NULL,
                   0,
                   &width,
                   &height,
                   &bitdepth))
    return 0;
  if (size) {
    size->cx = width;
    size->cy = height;
  }

  // ALLOCATE MEMORY FOR THE IMAGE
  DWORD bytes = width*height*bitdepth/8;
  if (!(*data = (LPBYTE)ALLOC(bytes)))
    return 0;

  // LOAD THE IMAGE
  PALETTEENTRY pe[256];
  if (!artcallback(providerid,
                   artid,
                   &pe[0],
                   *data,
                   bytes,
                   &width,
                   &height,
                   &bitdepth)) {
    FREE(*data);
    *data = NULL;
    return 0;
  }

  // IF REQUESTED, UPDATE THE SYSTEM PALETTE
  if (setpalette)
    SDrawUpdatePalette(1,254,&pe[1]);

  return 1;
}

//===========================================================================
static void ProcessIncomingAd (SOCKADDR_IPX *incomingaddr,
                               ADVPACKETPTR  incomingad,
                               BOOL          remove) {

  // FIX THE INCOMING ADDRESS SO IT POINTS TO THE MAIN PORT,
  // NOT THE ADVERTISING PORT
  if (ipx_winsockapi && ipx_winsockapi->htons)
    incomingaddr->sa_socket = ipx_winsockapi->htons(MAINPORT);

  // ENTER THE CRITICAL SECTION
  ipx_critsect.Enter();

  // DELETE ALL GAMES IN OUR LIST FROM THIS ADDRESS
  DWORD gameid = 0;
  {
    SNETSPI_GAMELISTPTR curr = ipx_gamehead;
    while (curr)
      if (!memcmp(&curr->owner,incomingaddr,sizeof(SOCKADDR_IPX))) {
        gameid = curr->gameid;
        SNETSPI_GAMELISTPTR next = curr->next;
        FREEIFUSED(curr->clientdata);
        LISTFREE(&ipx_gamehead,curr);
        curr = next;
      }
      else
        curr = curr->next;
  }

  // IF THIS GAME WAS NOT ALREADY IN THE LIST, ADD A NEW ID FOR IT.
  // MAKE SURE WE NEVER ASSIGN AN ID OF ZERO.
  if (!gameid)
    gameid = ++ipx_nextgameid;
  if (!gameid)
    gameid = ++ipx_nextgameid;

  // IF THIS GAME MATCHES OUR PROGRAM ID AND VERSION ID, AND WE'RE NOT
  // REMOVING, THEN ADD IT TO THE LIST
  if ((incomingad->header.programid == ipx_programid) &&
      (incomingad->header.versionid == ipx_versionid) &&
      !remove) {
    SNETSPI_GAMELIST game;
    ZeroMemory(&game,sizeof(SNETSPI_GAMELIST));
    game.gameid        = gameid;
    game.gamemode      = incomingad->header.gamemode;
    CopyMemory(&game.owner,incomingaddr,sizeof(SOCKADDR_IPX));
    game.ownerlatency  = 50;
    game.ownerlasttime = GetTickCount();
    LPSTR currptr = incomingad->strings;
    strncpy(game.gamename,
            currptr,
            SNETSPI_MAXSTRINGLENGTH);
    currptr += strlen(currptr)+1;
    strncpy(game.gamedescription,
            currptr,
            SNETSPI_MAXSTRINGLENGTH);
    currptr += strlen(currptr)+1;
    game.clientdatabytes = (incomingad->header.length-(currptr-(LPSTR)incomingad));
    game.clientdata      = ALLOC(game.clientdatabytes);
    CopyMemory(game.clientdata,currptr,game.clientdatabytes);
    LISTADD(&ipx_gamehead,&game);
  }

  // LEAVE THE CRITICAL SECTION
  ipx_critsect.Leave();

}

//===========================================================================
static unsigned CALLBACK RecvAdThreadProc (LPVOID param) {
  if (!(ipx_winsockapi && ipx_winsockapi->recvfrom)) {
    _endthreadex(0);
    return 0;
  }

  // SEND OUT A REQUEST FOR ADVERTISEMENTS
  SendRequest();

  ADVPACKET incomingad;
  while (ipx_advsocket && !ipx_shutdown) {

    // PROCESS ALL INCOMING ADVERTISEMENTS
    SOCKADDR_IPX incomingaddr;
    int addrsize  = sizeof(SOCKADDR_IPX);
    int bytesread = 0;
    bytesread = ipx_winsockapi->recvfrom(ipx_advsocket,
                                         (char *)&incomingad,
                                         sizeof(ADVPACKET),
                                         0,
                                         (sockaddr *)&incomingaddr,
                                         &addrsize);
    PerfIncrement(PERF_PKTRECV);
    PerfAdd(PERF_BYTESRECV,bytesread);
    if ((bytesread >= sizeof(ADVHEADER)) &&
        (incomingad.header.length == bytesread) &&
        !ComputeChecksum(&incomingad,incomingad.header.length))
      switch (incomingad.header.type) {

        case ADTYPE_GAMEINFO:
        case ADTYPE_REMOVE:
          ProcessIncomingAd(&incomingaddr,
                            &incomingad,
                            (incomingad.header.type == ADTYPE_REMOVE));
          break;

        case ADTYPE_REQUEST:
          SendAdvertisement(&incomingaddr);
          break;

      }

  }

  // FREE THE LIST OF GAMES
  {
    SNETSPI_GAMELISTPTR curr;
    while ((curr = ipx_gamehead) != NULL) {
      FREEIFUSED(curr->clientdata);
      LISTFREE(&ipx_gamehead,curr);
    }
  }

  _endthreadex(0);
  return 0;
}

//===========================================================================
static unsigned CALLBACK RecvDataThreadProc (LPVOID param) {
  if (!(ipx_winsockapi && ipx_winsockapi->recvfrom)) {
    _endthreadex(0);
    return 0;
  }
  while (ipx_mainsocket && !ipx_shutdown) {

    // ALLOCATE MEMORY FOR THE NEXT INCOMING PACKET
    PACKETPTR pkt = NEW(PACKET);
    int loop;
    ipx_critsect.Enter();
    for (loop = 0; loop < RECVDATATHREADS; ++loop)
      if (!ipx_pendingreadpkt[loop]) {
        ipx_pendingreadpkt[loop] = pkt;
        break;
      }
    ipx_critsect.Leave();

    // RECEIVE A PACKET, BLOCKING IF ONE IS NOT AVAILABLE YET.  WHEN THE
    // NETWORK DRIVER HAS INCOMING DATA ON A PORT, IT WILL COPY IT DIRECTLY
    // TO THE APPLICATION'S ADDRESS SPACE IF THE APPLICATION IS BLOCKING ON
    // A READ.  FOR THIS REASON, WE TRY TO ALWAYS HAVE AT LEAST ONE READ
    // PENDING.
    int addrsize  = sizeof(SOCKADDR_IPX);
    int bytesread = 0;
    bytesread = ipx_winsockapi->recvfrom(ipx_mainsocket,
                                         (char *)&pkt->data,
                                         MAXMESSAGESIZE,
                                         0,
                                         (sockaddr *)&pkt->addr,
                                         &addrsize);
    PerfIncrement(PERF_PKTRECV);
    PerfAdd(PERF_BYTESRECV,bytesread);
    pkt->databytes = bytesread;
    ZeroMemory(((LPBYTE)&pkt->addr)+addrsize,sizeof(SNETADDR)-addrsize);

    // ON A SUCCESSFUL READ, QUEUE THE PACKET.  SINCE WE DON'T TIME OUT ON
    // READS, THE ONLY WAY A READ CAN FAIL IS IF THE SOCKET WAS CLOSED.
    // IN THIS CASE, SHUT DOWN THE THREAD.
    ipx_critsect.Enter();
    for (loop = 0; loop < RECVDATATHREADS; ++loop)
      if (ipx_pendingreadpkt[loop] == pkt) {
        ipx_pendingreadpkt[loop] = NULL;
        break;
      }
    BOOL success = (bytesread >= 0) && !ipx_shutdown;
    if (success)
      LISTADDPTREND(&ipx_packethead,pkt);
    else
      FREE(pkt);
    ipx_critsect.Leave();

    if (success)
      SetEvent(ipx_recvevent);
    else {
      _endthreadex(0);
      return 0;
    }


  }
  return 0;
}

//===========================================================================
static BOOL CALLBACK SelectGameDialogProc (HWND   window,
                                           UINT   message,
                                           WPARAM wparam,
                                           LPARAM lparam) {
  static LPBYTE      background    = NULL;
  static LPBYTE      buttontexture = NULL;
  static UIPARAMSPTR uiparams      = NULL;
  switch (message) {

    case WM_COMMAND:

      // IF THE USER CLICKED 'JOIN GAME', TRY TO JOIN THE SELECTED GAME
      if (LOWORD(wparam) == IDOK) {
        LPARAM cursel = SendDlgItemMessage(window,IDC_GAMELIST,LB_GETCURSEL,0,0);
        if (cursel != LB_ERR) {
          char fullname[2*SNETSPI_MAXSTRINGLENGTH] = "";
          SendDlgItemMessage(window,IDC_GAMELIST,LB_GETTEXT,cursel,(LPARAM)fullname);
          if (fullname[0]) {
            if (strchr(fullname,'\t'))
              *strchr(fullname,'\t') = 0;
            if (SNetJoinGame(0,
                             fullname,
                             NULL,
                             uiparams->playerdata->playername,
                             uiparams->playerdata->playerdescription,
                             uiparams->playeridptr)) {
              KillTimer(window,1);
              SDlgEndDialog(window,1);
            }
            else
              uiparams->interfacedata->messageboxcallback(window,
                                                          "Unable to connect.",
                                                          uiparams->programdata->programname,
                                                          0);
          }
        }
      }

      // IF THE USER CLICKED 'CREATE GAME', CALL THE CREATE GAME CALLBACK
      else if ((LOWORD(wparam) == IDC_CREATEGAME) &&
               uiparams->interfacedata->createcallback) {

        // BUILD A NEW INTERFACE DATA STRUCTURE CONTAINING OUR WINDOW HANDLE
        SNETUIDATA interfacedata;
        CopyMemory(&interfacedata,uiparams->interfacedata,sizeof(SNETUIDATA));
        interfacedata.size         = sizeof(SNETUIDATA);
        interfacedata.parentwindow = window;

        // BUILD A CREATION DATA STRUCTURE
        SNETCREATEDATA createdata;
        ZeroMemory(&createdata,sizeof(SNETCREATEDATA));
        createdata.size        = sizeof(SNETCREATEDATA);
        createdata.providerid  = PROVIDERID;
        createdata.maxplayers  = ipx_maxplayers;
        createdata.createflags = 0;

        // CALL THE CREATE GAME CALLBACK 
        if (uiparams->interfacedata->createcallback(&createdata,
                                                    uiparams->programdata,
                                                    uiparams->playerdata,
                                                    &interfacedata,
                                                    uiparams->versiondata,
                                                    uiparams->playeridptr)) {
          KillTimer(window,1);
          SDlgEndDialog(window,1);
        }

      }

      // IF THE USER CLICKED 'DISCONNECT', END THE DIALOG
      else if (LOWORD(wparam) == IDCANCEL) {
        KillTimer(window,1);
        SDlgEndDialog(window,0);
      }

      // IF THE USER SELECTED A NEW LIST BOX ITEM, UPDATE THE GAME
      // DESCRIPTION
      else if ((LOWORD(wparam) == IDC_GAMELIST) &&
               (HIWORD(wparam) == LBN_SELCHANGE))
        InvalidateRect(GetDlgItem(window,IDC_GAMEDESCRIPTION),NULL,1);

      // IF THE USER DOUBLE-CLICKED A LIST BOX ITEM, POST AN 'OK' COMMAND
      else if ((LOWORD(wparam) == IDC_GAMELIST) &&
               (HIWORD(wparam) == LBN_DBLCLK))
        PostMessage(window,WM_COMMAND,MAKELONG(IDOK,BN_CLICKED),(LPARAM)GetDlgItem(window,IDOK));

      break;

    case WM_DESTROY:
      if (background) {
        FREE(background);
        background = NULL;
      }
      if (buttontexture) {
        FREE(buttontexture);
        buttontexture = NULL;
      }
      uiparams = NULL;
      break;

    case WM_DRAWITEM:
      if (wparam == IDC_GAMEDESCRIPTION) {

        // GET THE GAME NAME AND DESCRIPTION
        char name[256] = "";
        {
          LRESULT sel = SendDlgItemMessage(window,IDC_GAMELIST,LB_GETCURSEL,0,0);
          if (sel != LB_ERR)
            SendDlgItemMessage(window,IDC_GAMELIST,LB_GETTEXT,sel,(LPARAM)name);
        }
        LPSTR description = "";
        if (strchr(name,'\t')) {
          description = strchr(name,'\t');
          *description++ = 0;
        }

        // UPDATE THE DESCRIPTION IN THE STATIC TEXT CONTROL
        {
          char buffer[256] = "";
          GetDlgItemText(window,IDC_GAMEDESCRIPTION,buffer,256);
          buffer[255] = 0;
          if (strcmp(buffer,description))
            SetDlgItemText(window,IDC_GAMEDESCRIPTION,description);
        }

        // IF THE APPLICATION HAS REGISTERED A DRAW DESCRIPTION CALLBACK,
        // LET IT DRAW THE DESCRIPTION
        if (uiparams->interfacedata->drawdesccallback)
          return uiparams->interfacedata->drawdesccallback(PROVIDERID,
                                                           SNET_DRAWTYPE_GAME,
                                                           name,
                                                           description,
                                                           0,
                                                           0,
                                                           SNET_DDF_MULTILINE,
                                                           (LPDRAWITEMSTRUCT)lparam);

        // OTHERWISE, LET THE DEFAULT DIALOG BOX PROCEDURE DRAW THE
        // DESCRIPTION FROM THE STATIC TEXT
        else
          return 0;

      }
      break;

    case WM_INITDIALOG:

      // SAVE A POINTER TO THE USER INTERFACE PARAMETERS
      uiparams = (UIPARAMSPTR)lparam;

      // LOAD THE ARTWORK FOR THIS DIALOG
      {
        SIZE size;
        if (LoadArtwork(uiparams->interfacedata->artcallback,
                        PROVIDERID,
                        SNET_ART_BACKGROUND,
                        1,
                        &background,
                        &size)) {
          SDlgSetBitmap(window,
                        NULL,
                        "",
                        SDLG_STYLE_ANY,
                        SDLG_USAGE_BACKGROUND,
                        background,
                        NULL,
                        size.cx,
                        size.cy);
          int controllist[2] = {IDC_GAMEDESCRIPTION,0};
          SDlgSetControlBitmaps(window,
                                &controllist[0],
                                NULL,
                                background,
                                &size,
                                SDLG_ADJUST_CONTROLPOS);
        }
        if (LoadArtwork(uiparams->interfacedata->artcallback,
                        PROVIDERID,
                        SNET_ART_BUTTONTEXTURE,
                        0,
                        &buttontexture,
                        &size)) {
          int controllist[4] = {IDC_CREATEGAME,IDOK,IDCANCEL,0};
          SDlgSetControlBitmaps(window,
                                &controllist[0],
                                NULL,
                                buttontexture,
                                &size,
                                SDLG_ADJUST_VERTICAL);
        }
      }

      // DRAW THE PROGRAM DESCRIPTION
      SetDlgItemTextA(window,IDC_PROGRAMDESCRIPTION,uiparams->programdata->programdescription);

      // SET THE FIRST TAB STOP FOR THE GAME LIST TO WIDER THAN THE LIST BOX,
      // TO HIDE ALL TABBED TEXT
      {
        RECT rect;
        GetClientRect(GetDlgItem(window,IDC_GAMELIST),&rect);
        SendDlgItemMessage(window,IDC_GAMELIST,LB_SETTABSTOPS,1,(LPARAM)&rect.right);
      }

      PostMessage(window,WM_USER,0,0);
      SetTimer(window,1,500,NULL);
      return 1;

    case WM_TIMER:
    case WM_USER:
      SendRequest();
      TrimGameList(3000);
      UpdateGameList(window,GetDlgItem(window,IDC_GAMELIST));
      break;

  }
  return SDlgDefDialogProc(window,message,wparam,lparam);
}

//===========================================================================
static void SendAdvertisement (SOCKADDR_IPX *addr) {
  ipx_critsect.Enter();
  if (ipx_advgameinfo && ipx_winsockapi && ipx_winsockapi->sendto) {
    ipx_winsockapi->sendto(ipx_advsocket,
                           (const char *)ipx_advgameinfo,
                           ipx_advgameinfo->header.length,
                           0,
                           (const sockaddr *)(addr ? addr : &ipx_broadcastaddr),
                           sizeof(SOCKADDR_IPX));
    PerfIncrement(PERF_PKTSENT);
    PerfAdd(PERF_BYTESSENT,ipx_advgameinfo->header.length);
  }
  ipx_critsect.Leave();
}

//===========================================================================
static void SendRequest () {
  ADVHEADER request;
  ZeroMemory(&request,sizeof(ADVHEADER));
  request.length    = sizeof(ADVHEADER);
  request.type      = ADTYPE_REQUEST;
  request.programid = ipx_programid;
  request.versionid = ipx_versionid;
  request.checksum  = GenerateChecksum(&request,sizeof(ADVHEADER));
  if (ipx_winsockapi && ipx_winsockapi->sendto)
    ipx_winsockapi->sendto(ipx_advsocket,
                           (const char *)&request,
                           request.length,
                           0,
                           (const sockaddr *)&ipx_broadcastaddr,
                           sizeof(SOCKADDR_IPX));
  PerfIncrement(PERF_PKTSENT);
  PerfAdd(PERF_BYTESSENT,request.length);
}

//===========================================================================
static void TrimGameList (DWORD timeout) {
  ipx_critsect.Enter();
  {
    DWORD                currtime = GetTickCount();
    SNETSPI_GAMELISTPTR *next     = &ipx_gamehead;
    while (*next)
      if (currtime-(*next)->ownerlasttime > timeout) {
        SNETSPI_GAMELISTPTR freeptr = *next;
        *next = (*next)->next;
        FREEIFUSED(freeptr->clientdata);
        FREE(freeptr);
      }
      else
        next = &(*next)->next;
  }
  ipx_critsect.Leave();
}

//===========================================================================
static void UpdateGameList (HWND dialog, HWND listbox) {
  ipx_critsect.Enter();

  // MAKE SURE ALL GAMES IN THE LINKED LIST ARE REPRESENTED IN THE LIST BOX
  {
    SNETSPI_GAMELISTPTR curr = ipx_gamehead;
    while (curr) {
      if (!(curr->gamemode & SNET_GM_UNLISTEDMASK)) {
        char fullstring[2*SNETSPI_MAXSTRINGLENGTH];
        sprintf(fullstring,"%s\t%s",curr->gamename,curr->gamedescription);
        if (SendMessage(listbox,LB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)fullstring) == LB_ERR) {
          SendMessage(listbox,LB_ADDSTRING,0,(LPARAM)fullstring);
          EnableWindow(GetDlgItem(dialog,IDOK),1);
          if (SendMessage(listbox,LB_GETCURSEL,0,0) == LB_ERR) {
            SendMessage(listbox,LB_SETCURSEL,0,0);
            SendMessage(dialog,WM_COMMAND,MAKELONG(IDC_GAMELIST,LBN_SELCHANGE),(LPARAM)listbox);
          }
        }
      }
      curr = curr->next;
    }
  }

  // MAKE SURE THERE ARE NO GAME IN THE LIST BOX THAT AREN'T IN THE LINKED LIST
  {
    char   liststring[2*SNETSPI_MAXSTRINGLENGTH];
    WPARAM index = 0;
    while (SendMessage(listbox,LB_GETTEXT,index,(LPARAM)liststring) != LB_ERR) {
      SNETSPI_GAMELISTPTR curr = ipx_gamehead;
      while (curr) {
        if (!(curr->gamemode & SNET_GM_UNLISTEDMASK)) {
          char fullstring[2*SNETSPI_MAXSTRINGLENGTH];
          sprintf(fullstring,"%s\t%s",curr->gamename,curr->gamedescription);
          if (!strcmp(fullstring,liststring))
            break;
        }
        curr = curr->next;
      }
      if (!curr) {
        if (SendMessage(listbox,LB_GETCURSEL,0,0) == (LRESULT)index) {
          SendMessage(listbox,LB_SETCURSEL,index-1,0);
          SendMessage(dialog,WM_COMMAND,MAKELONG(IDC_GAMELIST,LBN_SELCHANGE),(LPARAM)listbox);
        }
        if (!SendMessage(listbox,LB_DELETESTRING,index,0))
          EnableWindow(GetDlgItem(dialog,IDOK),0);
      }
      else
        ++index;
    }
  }

  ipx_critsect.Leave();
}

/****************************************************************************
*
*  SERVICE PROVIDER INTERFACE FUNCTIONS
*
***/

//===========================================================================
BOOL CALLBACK IpxCompareNetAddresses (SNETADDRPTR  addr1,
                                      SNETADDRPTR  addr2,
                                      DWORD       *diffmagnitude) {
  if (diffmagnitude)
    *diffmagnitude = 0;
  if (!(addr1 && addr2 && diffmagnitude)) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }

  // COMPARE THE ADDRESSES, AND RETURN:
  //   2 IF THEY ARE ON DIFFERENT NETWORKS
  //   1 IF THEY ARE DIFFERENT ADDRESSES ON THE SAME NETWORK
  //   0 IF THEY ARE THE SAME ADDRESS
  SOCKADDR_IPX *ipxaddr1 = (SOCKADDR_IPX *)addr1;
  SOCKADDR_IPX *ipxaddr2 = (SOCKADDR_IPX *)addr2;
  if ((*(DWORD *)&ipxaddr1->sa_netnum) !=
      (*(DWORD *)&ipxaddr2->sa_netnum))
    *diffmagnitude = 2;
  else if (memcmp(ipxaddr1,ipxaddr2,sizeof(SOCKADDR_IPX)))
    *diffmagnitude = 1;
  else
    *diffmagnitude = 0;

  return 1;
}

//===========================================================================
BOOL CALLBACK IpxDestroy () {

  // START THE SHUTDOWN PROCESS
  ipx_shutdown = TRUE;

  // SEND DATA TO THE RECEIVE THREADS TO WAKE THEM UP
  {
    SOCKADDR_IPX sendaddr;
    CopyMemory(&sendaddr,&ipx_broadcastaddr,sizeof(SOCKADDR_IPX));
    BYTE buffer[8] = {0x08,0xEF,0x08,0x00,0x00,0x00,0x00,0x00};
    if (ipx_winsockapi && ipx_winsockapi->htons)
      sendaddr.sa_socket = ipx_winsockapi->htons(ADVPORT);
    if (ipx_winsockapi && ipx_winsockapi->sendto)
      ipx_winsockapi->sendto(ipx_advsocket,(const char *)&buffer[0],8,0,(const sockaddr *)&sendaddr,sizeof(SOCKADDR_IPX));
    if (ipx_winsockapi && ipx_winsockapi->htons)
      sendaddr.sa_socket = ipx_winsockapi->htons(MAINPORT);
    if (ipx_winsockapi && ipx_winsockapi->sendto)
      for (int loop = 0; loop < RECVDATATHREADS; ++loop)
        ipx_winsockapi->sendto(ipx_mainsocket,(const char *)&buffer[0],8,0,(sockaddr *)&sendaddr,sizeof(SOCKADDR_IPX));
  }

  // WAIT FOR ALL THREADS TO TERMINATE
  while (ipx_threadhead) {
    WaitForSingleObject(ipx_threadhead->handle,100);
    CloseHandle(ipx_threadhead->handle);
    LISTFREE(&ipx_threadhead,ipx_threadhead);
  }

  // CLOSE THE SOCKETS
  if (ipx_mainsocket) {
    if (ipx_winsockapi && ipx_winsockapi->closesocket)
      ipx_winsockapi->closesocket(ipx_mainsocket);
    ipx_mainsocket = (SOCKET)0;
  }
  if (ipx_advsocket) {
    if (ipx_winsockapi && ipx_winsockapi->closesocket)
      ipx_winsockapi->closesocket(ipx_advsocket);
    ipx_advsocket = (SOCKET)0;
  }

  // TAKE CONTROL OF THE CRITICAL SECTION
  ipx_critsect.Enter();

  // FREE THE GAME INFO
  IpxStopAdvertisingGame();
  {
    SNETSPI_GAMELISTPTR curr;
    while ((curr = ipx_gamehead) != NULL) {
      FREEIFUSED(curr->clientdata);
      LISTFREE(&ipx_gamehead,curr);
    }
  }

  // FREE ALL UNPROCESSED PACKETS
  LISTCLEAR(&ipx_packethead);
  for (int loop = 0; loop < RECVDATATHREADS; ++loop)
    if (ipx_pendingreadpkt[loop])
      FREE(ipx_pendingreadpkt[loop]);

  // CLEAN UP WINDOWS SOCKETS
  if (ipx_winsockapi && ipx_winsockapi->WSACleanup)
    ipx_winsockapi->WSACleanup();
  if (ipx_winsockapi) {
    FREE(ipx_winsockapi);
    ipx_winsockapi = NULL;
  }
  if (ipx_winsocklib) {
    FreeLibrary(ipx_winsocklib);
    ipx_winsocklib = (HINSTANCE)0;
  }

  // LEAVE THE CRITICAL SECTION
  ipx_critsect.Leave();

  // FINISH THE SHUTDOWN PROCESS
  ipx_shutdown = 0;

  return 1;
}

//===========================================================================
BOOL CALLBACK IpxFree (SNETADDRPTR addr,
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
BOOL CALLBACK IpxFreeExternalMessage (LPCSTR senderpath,
                                      LPCSTR sendername,
                                      LPCSTR mesage) {
  SetLastError(SNET_ERROR_INVALID_PARAMETER);
  return 0;
}

//===========================================================================
BOOL CALLBACK IpxGetGameInfo (DWORD             gameid,
                              LPCSTR            gamename,
                              LPCSTR            gamepassword,
                              SNETSPI_GAMELIST *gameinfo) {
  if (gameinfo)
    ZeroMemory(gameinfo,sizeof(SNETSPI_GAMELIST));
  if (!(gamename && gameinfo && (gameid || *gamename))) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }

  // SEARCH FOR A GAME IN THE GAME LIST MATCHING THE QUERY PARAMETERS
  ipx_critsect.Enter();
  {
    SNETSPI_GAMELISTPTR curr = ipx_gamehead;
    while (curr)
      if (((!gameid) || (gameid == curr->gameid)) &&
          ((!*gamename) || !_stricmp(gamename,curr->gamename))) {
        CopyMemory(gameinfo,curr,sizeof(SNETSPI_GAMELIST));
        break;
      }
      else
        curr = curr->next;
  }
  ipx_critsect.Leave();

  if (gameinfo->gameid)
    return 1;
  else {
    SetLastError(SNET_ERROR_GAME_NOT_FOUND);
    return 0;
  }
}

//===========================================================================
BOOL CALLBACK IpxGetPerformanceData (DWORD          counterid,
                                     DWORD         *countervalue,
                                     LARGE_INTEGER *measurementtime,
                                     LARGE_INTEGER *measurementfreq) {
  return PerfGetPerformanceData(counterid,
                                countervalue,
                                measurementtime,
                                measurementfreq);
}

//===========================================================================
BOOL CALLBACK IpxInitialize (SNETPROGRAMDATAPTR programdata,
                             SNETPLAYERDATAPTR  playerdata,
                             SNETUIDATAPTR      interfacedata,
                             SNETVERSIONDATAPTR versiondata,
                             HANDLE             event) {

  // SAVE THE PROGRAM AND VERSION IDS AND THE RECEIVE EVENT HANDLE
  ipx_programid  = programdata->programid;
  ipx_versionid  = programdata->versionid;
  ipx_maxplayers = min(programdata->maxplayers,MAXPLAYERS);
  ipx_recvevent  = event;

  // RESET PERFORMANCE DATA
  PerfReset();

  // BIND TO WINSOCK
  if (!BindToWinsock())
    return 0;

  // INITIALIZE WINDOWS SOCKETS
  {
    WSADATA data;
    if (ipx_winsockapi->WSAStartup(MAKEWORD(1,1),&data)) {
      IpxDestroy();
      SetLastError(SNET_ERROR_NO_NETWORK);
      return 0;
    }
  }

  // CREATE TWO SOCKETS: ONE FOR APPLICATION DATA AND ONE FOR ADVERTISING
  ipx_advsocket  = ipx_winsockapi->socket(PF_NS,SOCK_DGRAM,NSPROTO_IPX);
  ipx_mainsocket = ipx_winsockapi->socket(PF_NS,SOCK_DGRAM,NSPROTO_IPX);
  if (!(ipx_advsocket && ipx_mainsocket)) {
    IpxDestroy();
    SetLastError(SNET_ERROR_NO_NETWORK);
    return 0;
  }

  // DETERMINE THE BROADCAST ADDRESS
  {
    ZeroMemory(&ipx_broadcastaddr,sizeof(SOCKADDR_IPX));
    for (int loop = 0; loop < 6; ++loop)
      ipx_broadcastaddr.sa_nodenum[loop] = (BYTE)0xFF;
    ipx_broadcastaddr.sa_family = AF_IPX;
    ipx_broadcastaddr.sa_socket = ipx_winsockapi->htons(ADVPORT);
  }

  // BIND TO THE MAIN SOCKET, DETERMINING OUR LOCAL ADDRESS
  ZeroMemory(&ipx_localaddr,sizeof(SOCKADDR_IPX));
  ipx_localaddr.sa_family = AF_IPX;
  ipx_localaddr.sa_socket = ipx_winsockapi->htons(MAINPORT);
  if (ipx_winsockapi->bind(ipx_mainsocket,
                           (const struct sockaddr *)&ipx_localaddr,
                           sizeof(SOCKADDR_IPX))) {
    IpxDestroy();
    SetLastError(SNET_ERROR_NO_NETWORK);
    return 0;
  }

  // BIND TO THE ADVERTISING SOCKET
  {
    SOCKADDR_IPX advaddr;
    CopyMemory(&advaddr,&ipx_localaddr,sizeof(SOCKADDR_IPX));
    advaddr.sa_socket = ipx_winsockapi->htons(ADVPORT);
    if (ipx_winsockapi->bind(ipx_advsocket,
                             (const struct sockaddr *)&advaddr,
                             sizeof(SOCKADDR_IPX))) {
      IpxDestroy();
      SetLastError(SNET_ERROR_NO_NETWORK);
      return 0;
    }
  }

  // ALLOW BROADCASTS TO BE SENT ON THE SOCKETS (DUE TO A BUG IN WINDOWS 95,
  // THIS OPTION IS ALSO REQUIRED FOR RECEIVING BROADCASTS)
  {
    BOOL value = 1;
    ipx_winsockapi->setsockopt(ipx_advsocket,
                               SOL_SOCKET,
                               SO_BROADCAST,
                               (const char *)&value,
                               sizeof(BOOL));
    ipx_winsockapi->setsockopt(ipx_mainsocket,
                               SOL_SOCKET,
                               SO_BROADCAST,
                               (const char *)&value,
                               sizeof(BOOL));
  }

  // CREATE A THREAD TO RECEIVE PACKETS ON THE ADVERTISING SOCKET
  {
    THREAD thread;
    thread.handle = (HANDLE)_beginthreadex((LPSECURITY_ATTRIBUTES)NULL,
                                           0,
                                           RecvAdThreadProc,
                                           NULL,
                                           0,
                                           &thread.id);
    if (thread.handle) {
      SetThreadPriority(thread.handle,THREAD_PRIORITY_ABOVE_NORMAL);
      LISTADD(&ipx_threadhead,&thread);
    }
  }

  // CREATE THREADS TO READ PACKETS FROM THE MAIN SOCKET
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
        LISTADD(&ipx_threadhead,&thread);
      }
    }
  }

  return 1;
}

//===========================================================================
BOOL CALLBACK IpxInitializeDevice (DWORD              deviceid,
                                   SNETPROGRAMDATAPTR programdata,
                                   SNETPLAYERDATAPTR  playerdata,
                                   SNETUIDATAPTR      interfacedata,
                                   SNETVERSIONDATAPTR versiondata) {

  // WE NEVER RETURN ANY DEVICES, SO THIS FUNCTION SHOULD NEVER BE CALLED
  return 0;
}

//===========================================================================
BOOL CALLBACK IpxLockDeviceList (SNETSPI_DEVICELISTPTR *devicelist) {
  *devicelist = NULL;
  return 1;
}

//===========================================================================
BOOL CALLBACK IpxLockGameList (DWORD                categorybits,
                               DWORD                categorymask,
                               SNETSPI_GAMELISTPTR *gamelist) {
  if (!gamelist) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }

  // TRIM ANY GAMES THAT WE HAVEN'T HEARD FROM IN A WHILE
  {
    static DWORD lasttime = GetTickCount();
    DWORD currtime = GetTickCount();
    TrimGameList(max(3000,2*(currtime-lasttime)));
  }

  // LOCK THE GAME LIST
  ipx_critsect.Enter();
  *gamelist = ipx_gamehead;

  return 1;
}

//===========================================================================
/*
BOOL CALLBACK IpxReceive (SNETADDRPTR *addr,
                          LPVOID      *data,
                          DWORD       *databytes) {
*/
BOOL CALLBACK IpxReceive (LPVOID      *data,
                          DWORD       *databytes,
                          SNETADDRPTR *addr) {
  if (addr)
    *addr = NULL;
  if (data)
    *data = NULL;
  if (databytes)
    *databytes = NULL;
  if (!(addr && data && databytes && ipx_mainsocket)) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }

  // IF THERE IS A PACKET QUEUED, REMOVE IT FROM THE QUEUE AND RETURN
  // POINTERS TO THE CALLER.  NOTE THAT WE UNLINK THE PACKET BUT DON'T
  // FREE IT FROM MEMORY; IT IS THE CALLER'S RESPONSIBILITY TO CALL
  // OUR FREE FUNCTION WHEN IT IS DONE WITH THE PACKET.
  if (ipx_packethead) {
    ipx_critsect.Enter();
    *addr      = &ipx_packethead->addr;
    *data      = ipx_packethead->data;
    *databytes = ipx_packethead->databytes;
    ipx_packethead = ipx_packethead->next;
    ipx_critsect.Leave();
    return 1;
  }
  else {
    SetLastError(SNET_ERROR_NO_MESSAGES_WAITING);
    return 0;
  }
}

//===========================================================================
BOOL CALLBACK IpxReceiveExternalMessage (LPCSTR *senderpath,
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
BOOL CALLBACK IpxSelectGame (DWORD               flags,
                             SNETPROGRAMDATAPTR  programdata,
                             SNETPLAYERDATAPTR   playerdata,
                             SNETUIDATAPTR       interfacedata,
                             SNETVERSIONDATAPTR  versiondata,
                             DWORD              *playerid) {

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
                                           "IPXSELECTGAME_DIALOG",
                                           interfacedata ? interfacedata->parentwindow
                                                         : SDrawGetFrameWindow(),
                                           SelectGameDialogProc,
                                           (LPARAM)&uiparams);

  return (result && (result != 0xFFFFFFFF));
}

//===========================================================================
BOOL CALLBACK IpxSend (DWORD        addresses,
                       SNETADDRPTR *addrlist,
                       LPVOID       data,
                       DWORD        databytes) {
  if (!(addresses && addrlist && data && databytes && ipx_mainsocket)) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }

  // SEND THE PACKET
  if (ipx_winsockapi && ipx_winsockapi->sendto)
    while (addresses--) {
      ipx_winsockapi->sendto(ipx_mainsocket,
                             (const char *)data,
                             databytes,
                             0,
                             (const sockaddr *)*(addrlist+addresses),
                             sizeof(SOCKADDR_IPX));
      PerfIncrement(PERF_PKTSENT);
      PerfAdd(PERF_BYTESSENT,databytes);
    }

  return 1;
}

//===========================================================================
BOOL CALLBACK IpxSendExternalMessage (LPCSTR senderpath,
                                      LPCSTR sendername,
                                      LPCSTR targetpath,
                                      LPCSTR targetname,
                                      LPCSTR message) {
  return 0;
}

//===========================================================================
BOOL CALLBACK IpxStartAdvertisingGame (LPCSTR  gamename,
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

  // CREATE A STRUCTURE TO CONTAIN THE DATA WE NEED TO ADVERTISE
  ipx_critsect.Enter();
  if (!ipx_advgameinfo) {
    ipx_advgameinfo = NEW(ADVPACKET);
    if (!ipx_advgameinfo) {
      ipx_critsect.Leave();
      SetLastError(SNET_ERROR_NOT_ENOUGH_MEMORY);
      return 0;
    }
  }
  ZeroMemory(ipx_advgameinfo,sizeof(ADVPACKET));
  ipx_advgameinfo->header.checksum  = 0;
  ipx_advgameinfo->header.length    = sizeof(ADVHEADER)
                                        +strlen(gamename)
                                        +strlen(gamedescription)
                                        +2
                                        +clientdatabytes;
  ipx_advgameinfo->header.type      = ADTYPE_GAMEINFO;
  ipx_advgameinfo->header.reserved  = 0;
  ipx_advgameinfo->header.programid = ipx_programid;
  ipx_advgameinfo->header.versionid = ipx_versionid;
  ipx_advgameinfo->header.gamemode  = gamemode;
  LPSTR currptr = ipx_advgameinfo->strings;
  SStrCopy(currptr,
           gamename,
           SNETSPI_MAXSTRINGLENGTH);
  currptr += strlen(currptr)+1;
  SStrCopy(currptr,
           gamedescription,
           SNETSPI_MAXSTRINGLENGTH);
  currptr += strlen(currptr)+1;
  CopyMemory(currptr,
             clientdata,
             clientdatabytes);
  ipx_advgameinfo->header.checksum = GenerateChecksum(ipx_advgameinfo,
                                                      ipx_advgameinfo->header.length);
  ipx_critsect.Leave();

  // SEND THE FIRST ADVERTISEMENT
  SendAdvertisement(NULL);

  return 1;
}

//===========================================================================
BOOL CALLBACK IpxStopAdvertisingGame () {

  // DELETE THE ADVERTISEMENT DATA AND SEND OUT A REMOVE GAME MESSAGE
  ipx_critsect.Enter();
  if (ipx_advgameinfo) {
    ipx_advgameinfo->header.checksum = 0;
    ipx_advgameinfo->header.type     = ADTYPE_REMOVE;
    ipx_advgameinfo->header.checksum = GenerateChecksum(ipx_advgameinfo,
                                                        ipx_advgameinfo->header.length);
    if (ipx_winsockapi && ipx_winsockapi->sendto)
      ipx_winsockapi->sendto(ipx_advsocket,
                             (const char *)ipx_advgameinfo,
                             ipx_advgameinfo->header.length,
                             0,
                             (const sockaddr *)&ipx_broadcastaddr,
                             sizeof(SOCKADDR_IPX));
    PerfIncrement(PERF_PKTSENT);
    PerfAdd(PERF_BYTESSENT,ipx_advgameinfo->header.length);
    FREE(ipx_advgameinfo);
    ipx_advgameinfo = NULL;
  }
  ipx_critsect.Leave();

  return 1;
}

//===========================================================================
BOOL CALLBACK IpxUnlockDeviceList (SNETSPI_DEVICELISTPTR devicelist) {
  return 1;
}

//===========================================================================
BOOL CALLBACK IpxUnlockGameList (SNETSPI_GAMELISTPTR  gamelist,
                                 DWORD               *hintnextcall) {
  if (gamelist != ipx_gamehead) {
    SetLastError(SNET_ERROR_INVALID_PARAMETER);
    return 0;
  }

  // UNLOCK THE GAME LIST
  ipx_critsect.Leave();
  if (hintnextcall)
    *hintnextcall = 500;

  // SEND OUT A REQUEST FOR ADVERTISEMENTS, SO WE WILL HAVE UPDATED DATA
  // THE NEXT TIME IT IS REQUESTED
  {
    static DWORD lasttime = 0;
    DWORD currtime = GetTickCount();
    if (currtime-lasttime > 400) {
      lasttime = currtime;
      SendRequest();
    }
  }

  return 1;
}

/****************************************************************************
*
*  EXPORTED STRUCTURES
*
***/

DWORD    ipx_id   = PROVIDERID;
LPCSTR   ipx_desc = "Local Area Network (IPX)";
LPCSTR   ipx_req  = "All computers must be connected to an IPX-compatible network.";
SNETCAPS ipx_caps = {sizeof(SNETCAPS),             // size
                     SNET_CAPS_PAGELOCKEDBUFFERS   // flags
                       | SNET_CAPS_BASICINTERFACE
#ifdef _DEBUG
                       | SNET_CAPS_DEBUGONLY,
#else
                       | SNET_CAPS_RETAILONLY,
#endif
                     MAXMESSAGESIZE,               // max message size
                     16,                           // max queue size,
                     MAXPLAYERS,                   // max players,
                     100000,                       // bytes per second
                     50,                           // latency (ms)
                     8,                            // default turns per second
                     2};                           // default turns in transit
SNETSPI  ipx_spi  = {sizeof(SNETSPI),
                     IpxCompareNetAddresses,
                     IpxDestroy,
                     IpxFree,
                     IpxFreeExternalMessage,
                     IpxGetGameInfo,
                     IpxGetPerformanceData,
                     IpxInitialize,
                     IpxInitializeDevice,
                     IpxLockDeviceList,
                     IpxLockGameList,
                     IpxReceive,
                     IpxReceiveExternalMessage,
                     IpxSelectGame,
                     IpxSend,
                     IpxSendExternalMessage,
                     IpxStartAdvertisingGame,
                     IpxStopAdvertisingGame,
                     IpxUnlockDeviceList,
                     IpxUnlockGameList};

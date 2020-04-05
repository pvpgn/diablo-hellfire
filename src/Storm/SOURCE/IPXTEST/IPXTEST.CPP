/****************************************************************************
*
*  IPXTEST.CPP
*  IPX test network provider
*
*  By Michael O'Brien (9/25/96)
*
***/

#define  STRICT
#include <windows.h>
#include <wsipx.h>
#include <stdio.h>
#include <storm.h>
#include "resource.h"

#define  ADVPORT          6113
#define  MAINPORT         6114
#define  MAXMESSAGESIZE   504
#define  MAXPLAYERS       256
#define  PROVIDERID       'TEST'
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
} ADVPACKET, *ADVPACKETPTR;

typedef struct _PACKET {
    SNETADDR  addr; // must be first field in structure
    BYTE      data[MAXMESSAGESIZE];
    DWORD     databytes;
    _PACKET  *next;
} PACKET, *PACKETPTR;

typedef struct _SENDREC {
    DWORD         sendtime;
    SOCKET        s;
    SOCKADDR_IPX  addr;
    char         *data;
    int           databytes;
    _SENDREC     *next;
} SENDREC, *SENDPTR;

typedef struct _UIPARAMS {
    DWORD              flags;
    SNETPROGRAMDATAPTR programdata;
    SNETPLAYERDATAPTR  playerdata;
    SNETUIDATAPTR      interfacedata;
    SNETVERSIONDATAPTR versiondata;
    LPDWORD            playeridptr;
} UIPARAMS, *UIPARAMSPTR;

typedef struct _THREAD {
    DWORD    id;
    HANDLE   handle;
    _THREAD *next;
} THREAD, *THREADPTR;

typedef struct _TESTPARMS {
    DWORD mindelay;
    DWORD maxdelay;
    DWORD corruptpercent;
} TESTPARMS, *TESTPARMSPTR;

static ADVPACKETPTR        ipx_advgameinfo   = NULL;
static SOCKET              ipx_advsocket     = (SOCKET)0;
static SOCKADDR_IPX        ipx_broadcastaddr = {0};
static CCritSect           ipx_critsect;
static SNETSPI_GAMELISTPTR ipx_gamehead      = NULL;
static HINSTANCE           ipx_instance      = (HINSTANCE)0;
static SOCKADDR_IPX        ipx_localaddr     = {0};
static DWORD               ipx_nextgameid    = 0;
static SOCKET              ipx_mainsocket    = (SOCKET)0;
static DWORD               ipx_maxplayers    = MAXPLAYERS;
static PACKETPTR           ipx_packethead    = NULL;
static DWORD               ipx_programid     = 0;
static HANDLE              ipx_recvevent     = NULL;
static SENDPTR             ipx_sendhead      = NULL;
static BOOL                ipx_shutdown      = 0;
static TESTPARMS           ipx_testparms     = {200,300,5};
static THREADPTR           ipx_threadhead    = NULL;
static DWORD               ipx_versionid     = 0;

static void SendAdvertisement ();
static void SendRequest ();
static void TrimGameList (DWORD timeout);
static void UpdateGameList (HWND dialog, HWND listbox);
BOOL CALLBACK IpxCompareNetAddresses (SNETADDRPTR  addr1,
                                      SNETADDRPTR  addr2,
                                      DWORD       *diffmagnitude);
BOOL CALLBACK IpxStopAdvertisingGame ();

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
static void ProcessIncomingAd (SOCKADDR_IPX *incomingaddr,
                               ADVPACKETPTR  incomingad,
                               BOOL          remove) {

  // FIX THE INCOMING ADDRESS SO IT POINTS TO THE MAIN PORT,
  // NOT THE ADVERTISING PORT
  incomingaddr->sa_socket = htons(MAINPORT);

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
    CopyMemory(&game.owner,incomingaddr,sizeof(SOCKADDR_IPX));
    game.ownerlatency  = 50;
    game.ownerlasttime = GetTickCount();
    strncpy(game.gamename,
            incomingad->strings,
            SNETSPI_MAXSTRINGLENGTH);
    strncpy(game.gamedescription,
            incomingad->strings+strlen(incomingad->strings)+1,
            SNETSPI_MAXSTRINGLENGTH);
    LISTADD(&ipx_gamehead,&game);
  }

  // LEAVE THE CRITICAL SECTION
  ipx_critsect.Leave();

}

//===========================================================================
static void QueueSendTo (SOCKET          s,
                         const char     *buffer,
                         int             length,
                         int             flags,
                         const sockaddr *addr,
                         int             addrlen) {
  if (!(s && buffer && addr && (addrlen == sizeof(SOCKADDR_IPX))))
    return;

  // CREATE A NEW RECORD
  SENDPTR newptr = NEW(SENDREC);
  if (!newptr)
    return;
  newptr->data = (char *)ALLOC(length);
  if (!newptr->data) {
    FREE(newptr);
    return;
  }
  newptr->s         = s;
  newptr->databytes = length;
  CopyMemory(&newptr->addr,addr,sizeof(SOCKADDR_IPX));
  CopyMemory(newptr->data,buffer,length);

  // PERFORM RANDOM PACKET CORRUPTION
  if ((PickRandomNumber() % 100) < ipx_testparms.corruptpercent) {
    BOOL  corrupted      = 0;
    DWORD corruptiontype = PickRandomNumber();

    // TRUNCATE SOME PACKETS
    if (newptr->databytes && !(corruptiontype & 0x0000000F)) {
      newptr->databytes = PickRandomNumber() % newptr->databytes;
      corrupted = 1;
    }

    // OFFSET SOME PACKETS BY ONE BYTE
    if (!(corruptiontype & 0x000000F0)) {
      MoveMemory(newptr->data,
                 newptr->data+1,
                 newptr->databytes-1);
      corrupted = 1;
    }
    if (!(corruptiontype & 0x00000F00)) {
      MoveMemory(newptr->data+1,
                 newptr->data,
                 newptr->databytes-1);
      corrupted = 1;
    }

    // CORRUPT THE ADDRESS FOR SOME PACKETS
    if (!(corruptiontype & 0x0000F000)) {
      *(((LPBYTE)&newptr->addr)+(PickRandomNumber() & 0x0F)) += PickRandomNumber() & 0xFF;
      corrupted = 1;
    }

    // TOGGLE SINGLE BITS IN SOME PACKETS
    if (newptr->databytes &&
        ((!corrupted) || !(corruptiontype & 0x000F0000))) {
      DWORD bit        = PickRandomNumber();
      DWORD bytenumber = (bit >> 3) % newptr->databytes;
      BYTE  bitvalue   = (BYTE)(1 << (bit & 7));
      *((LPBYTE)newptr->data+bytenumber) ^= bitvalue;
    }

  }

  // DETERMINE THE TIME AT WHICH THE PACKET SHOULD BE SENT
  newptr->sendtime = GetTickCount()
                       +ipx_testparms.mindelay
                       +(PickRandomNumber() % (ipx_testparms.maxdelay-ipx_testparms.mindelay));

  // ADD IT TO THE LINKED LIST
  ipx_critsect.Enter();
  LISTADDPTREND(&ipx_sendhead,newptr);
  ipx_critsect.Leave();

}

//===========================================================================
static DWORD CALLBACK RecvAdThreadProc (LPVOID param) {

  // SEND OUT A REQUEST FOR ADVERTISEMENTS
  SendRequest();

  // ALLOCATE MEMORY FOR INCOMING PACKETS
  ADVPACKETPTR incomingad = NEW(ADVPACKET);
  if (!incomingad)
    return 0;

  while (ipx_advsocket && !ipx_shutdown) {

    // PROCESS ALL INCOMING ADVERTISEMENTS
    SOCKADDR_IPX incomingaddr;
    int addrsize  = sizeof(SOCKADDR_IPX);
    int bytesread = recvfrom(ipx_advsocket,
                             (char *)incomingad,
                             sizeof(ADVPACKET),
                             0,
                             (sockaddr *)&incomingaddr,
                             &addrsize);
    if ((bytesread >= sizeof(ADVHEADER)) &&
        (incomingad->header.length == bytesread) &&
        !ComputeChecksum(incomingad,incomingad->header.length))
      switch (incomingad->header.type) {

        case ADTYPE_GAMEINFO:
        case ADTYPE_REMOVE:
          ProcessIncomingAd(&incomingaddr,
                            incomingad,
                            (incomingad->header.type == ADTYPE_REMOVE));
          break;

        case ADTYPE_REQUEST:
          SendAdvertisement();
          break;

      }

  }

  // FREE THE INCOMING GAME BUFFER
  FREE(incomingad);

  // FREE THE LIST OF GAMES
  LISTCLEAR(&ipx_gamehead);

  return 0;
}

//===========================================================================
static DWORD CALLBACK RecvDataThreadProc (LPVOID param) {
  while (ipx_mainsocket && !ipx_shutdown) {

    // ALLOCATE MEMORY FOR THE NEXT INCOMING PACKET
    PACKETPTR pkt = NEW(PACKET);

    // RECEIVE A PACKET, BLOCKING IF ONE IS NOT AVAILABLE YET.  WHEN THE
    // NETWORK DRIVER HAS INCOMING DATA ON A PORT, IT WILL COPY IT DIRECTLY
    // TO THE APPLICATION'S ADDRESS SPACE IF THE APPLICATION IS BLOCKING ON
    // A READ.  FOR THIS REASON, WE TRY TO ALWAYS HAVE AT LEAST ONE READ
    // PENDING.
    int addrsize  = sizeof(SOCKADDR_IPX);
    int bytesread = recvfrom(ipx_mainsocket,
                             (char *)&pkt->data,
                             MAXMESSAGESIZE,
                             0,
                             (sockaddr *)&pkt->addr,
                             &addrsize);
    pkt->databytes = bytesread;
    ZeroMemory(((LPBYTE)&pkt->addr)+addrsize,sizeof(SNETADDR)-addrsize);

    // SINCE WE DON'T TIME OUT ON READS, THE ONLY WAY A READ CAN FAIL IS
    // IF THE SOCKET WAS CLOSED.  IN THIS CASE, SHUT DOWN THE THREAD.
    if ((bytesread < 0) || ipx_shutdown) {
      FREE(pkt);
      return 0;
    }

    // ON A SUCCESSFUL READ, QUEUE THE PACKET
    ipx_critsect.Enter();
    LISTADDPTREND(&ipx_packethead,pkt);
    ipx_critsect.Leave();
    SetEvent(ipx_recvevent);

  }
  return 0;
}

//===========================================================================
static DWORD CALLBACK SendThreadProc (LPVOID param) {
  while (!ipx_shutdown) {
    if (ipx_sendhead) {
      ipx_critsect.Enter();

      // TRAVERSE THE SEND QUEUE, LOOKING FOR PACKETS THAT ARE READY TO
      // BE SENT
      DWORD    currtime = GetTickCount();
      SENDPTR *next     = &ipx_sendhead;
      while (*next)
        if ((DWORD)(currtime-(*next)->sendtime) <= 0x7FFFFFFF) {
          SENDPTR curr = *next;

          // SEND THE PACKET
          sendto(curr->s,
                 curr->data,
                 curr->databytes,
                 0,
                 (const sockaddr *)&curr->addr,
                 sizeof(SOCKADDR_IPX));

          // UNLINK IT FROM THE QUEUE
          *next = curr->next;

          // FREE THE RECORD AND PACKET DATA
          FREE(curr->data);
          FREE(curr);

          // SLEEP FOR ONE QUANTUM BEFORE PROCESSING THE NEXT PACKET
          Sleep(1);

        }
        else
          next = &(*next)->next;

      ipx_critsect.Leave();
    }
    Sleep(50);
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
static void SendAdvertisement () {
  ipx_critsect.Enter();
  if (ipx_advgameinfo)
    QueueSendTo(ipx_advsocket,
                (const char *)ipx_advgameinfo,
                ipx_advgameinfo->header.length,
                0,
                (const sockaddr *)&ipx_broadcastaddr,
                sizeof(SOCKADDR_IPX));
  ipx_critsect.Leave();
}

//===========================================================================
static void SendRequest () {
  ADVHEADER request;
  ZeroMemory(&request,sizeof(ADVHEADER));
  request.checksum  = 0;
  request.length    = sizeof(ADVHEADER);
  request.type      = ADTYPE_REQUEST;
  request.reserved  = 0;
  request.programid = ipx_programid;
  request.versionid = ipx_versionid;
  request.checksum  = GenerateChecksum(&request,sizeof(ADVHEADER));
  QueueSendTo(ipx_advsocket,
              (const char *)&request,
              request.length,
              0,
              (const sockaddr *)&ipx_broadcastaddr,
              sizeof(SOCKADDR_IPX));
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
      if (!(curr->gamemode & SNET_GM_PRIVATE)) {
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
        if (!(curr->gamemode & SNET_GM_PRIVATE)) {
          char fullstring[2*SNETSPI_MAXSTRINGLENGTH];
          sprintf(fullstring,"%s\t%s",curr->gamename,curr->gamedescription);
          if (!strcmp(fullstring,liststring))
            break;
        }
        curr = curr->next;
      }
      if (!curr) {
        if (SendMessage(listbox,LB_GETCURSEL,0,0) == index) {
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
  ipx_shutdown = 1;

  // SEND DATA TO THE RECEIVE THREADS TO WAKE THEM UP
  {
    SOCKADDR_IPX sendaddr;
    CopyMemory(&sendaddr,&ipx_broadcastaddr,sizeof(SOCKADDR_IPX));
    BYTE buffer[8] = {0x08,0xEF,0x08,0x00,0x00,0x00,0x00,0x00};
    sendaddr.sa_socket = htons(ADVPORT);
    sendto(ipx_advsocket,(const char *)&buffer[0],8,0,(const sockaddr *)&sendaddr,sizeof(SOCKADDR_IPX));
    sendaddr.sa_socket = htons(MAINPORT);
    for (int loop = 0; loop < RECVDATATHREADS; ++loop)
      sendto(ipx_mainsocket,(const char *)&buffer[0],8,0,(sockaddr *)&sendaddr,sizeof(SOCKADDR_IPX));
  }

  // WAIT FOR ALL THREADS TO TERMINATE
  while (ipx_threadhead) {
    WaitForSingleObject(ipx_threadhead->handle,100);
    CloseHandle(ipx_threadhead->handle);
    LISTFREE(&ipx_threadhead,ipx_threadhead);
  }

  // CLOSE THE SOCKETS
  if (ipx_mainsocket) {
    closesocket(ipx_mainsocket);
    ipx_mainsocket = (SOCKET)0;
  }
  if (ipx_advsocket) {
    closesocket(ipx_advsocket);
    ipx_advsocket = (SOCKET)0;
  }

  // TAKE CONTROL OF THE CRITICAL SECTION
  ipx_critsect.Enter();

  // FREE THE GAME INFO
  IpxStopAdvertisingGame();

  // CLEAR OUT THE SEND QUEUE
  while (ipx_sendhead) {
    FREE(ipx_sendhead->data);
    LISTFREE(&ipx_sendhead,ipx_sendhead);
  }

  // FREE ALL UNPROCESSED PACKETS
  LISTCLEAR(&ipx_packethead);

  // CLEAN UP WINDOWS SOCKETS
  WSACleanup();

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
  if (!(gamename && gameinfo && (gameid || *gamename)))
    return 0;

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
  return 0;
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

  // INITIALIZE WINDOWS SOCKETS
  {
    WSADATA data;
    if (WSAStartup(MAKEWORD(1,1),&data)) {
      IpxDestroy();
      SetLastError(SNET_ERROR_NO_NETWORK);
      return 0;
    }
  }

  // CREATE TWO SOCKETS: ONE FOR APPLICATION DATA AND ONE FOR ADVERTISING
  ipx_advsocket  = socket(PF_NS,SOCK_DGRAM,NSPROTO_IPX);
  ipx_mainsocket = socket(PF_NS,SOCK_DGRAM,NSPROTO_IPX);
  if (!(ipx_advsocket && ipx_mainsocket)) {
    IpxDestroy();
    SetLastError(SNET_ERROR_NO_NETWORK);
    return 0;
  }

  // DETERMINE THE BROADCAST ADDRESS
  {
    ZeroMemory(&ipx_broadcastaddr,sizeof(SOCKADDR_IPX));
    for (int loop = 0; loop < 6; ++loop)
      ipx_broadcastaddr.sa_nodenum[loop] = 0xFF;
    ipx_broadcastaddr.sa_family = AF_IPX;
    ipx_broadcastaddr.sa_socket = htons(ADVPORT);
  }

  // BIND TO THE MAIN SOCKET, DETERMINING OUR LOCAL ADDRESS
  ZeroMemory(&ipx_localaddr,sizeof(SOCKADDR_IPX));
  ipx_localaddr.sa_family = AF_IPX;
  ipx_localaddr.sa_socket = htons(MAINPORT);
  if (bind(ipx_mainsocket,
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
    advaddr.sa_socket = htons(ADVPORT);
    if (bind(ipx_advsocket,
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
    setsockopt(ipx_advsocket,
               SOL_SOCKET,
               SO_BROADCAST,
               (const char *)&value,
               sizeof(BOOL));
    setsockopt(ipx_mainsocket,
               SOL_SOCKET,
               SO_BROADCAST,
               (const char *)&value,
               sizeof(BOOL));
  }

  // CREATE A THREAD TO RECEIVE PACKETS ON THE ADVERTISING SOCKET
  {
    THREAD thread;
    thread.handle = CreateThread((LPSECURITY_ATTRIBUTES)NULL,
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
    for (int loop = 0; loop < RECVDATATHREADS; ++loop) {
      THREAD thread;
      thread.handle = CreateThread((LPSECURITY_ATTRIBUTES)NULL,
                                   0,
                                   RecvDataThreadProc,
                                   NULL,
                                   0,
                                   &thread.id);
      if (thread.handle) {
        SetThreadPriority(thread.handle,THREAD_PRIORITY_ABOVE_NORMAL);
        LISTADD(&ipx_threadhead,&thread);
      }
    }
  }

  // CREATE A THREAD TO DEQUEUE PACKETS FROM THE SEND QUEUE AND SEND THEM
  // OUT ON THE WIRE
  {
    THREAD thread;
    thread.handle = CreateThread((LPSECURITY_ATTRIBUTES)NULL,
                                 0,
                                 SendThreadProc,
                                 NULL,
                                 0,
                                 &thread.id);
    if (thread.handle) {
      SetThreadPriority(thread.handle,THREAD_PRIORITY_ABOVE_NORMAL);
      LISTADD(&ipx_threadhead,&thread);
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
BOOL CALLBACK IpxLockGameList (LPCSTR               selectioncriteria,
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
BOOL CALLBACK IpxReceive (SNETADDRPTR *addr,
                          LPVOID      *data,
                          DWORD       *databytes) {
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
    *addr          = &ipx_packethead->addr;
    *data          = ipx_packethead->data;
    *databytes     = ipx_packethead->databytes;
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
  DWORD result = (DWORD)SDlgDialogBoxParam(ipx_instance,
                                           "IPXSELECTGAME_DIALOG",
                                           interfacedata ? interfacedata->parentwindow
                                                         : SDrawGetFrameWindow(),
                                           SelectGameDialogProc,
                                           (LPARAM)&uiparams);

  return (result != 0);
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
  while (addresses--)
    QueueSendTo(ipx_mainsocket,
                (const char *)data,
                databytes,
                0,
                (const sockaddr *)*(addrlist+addresses),
                sizeof(SOCKADDR_IPX));

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
BOOL CALLBACK IpxStartAdvertisingGame (LPCSTR gamename,
                                       LPCSTR gamepassword,
                                       LPCSTR gamedescription,
                                       DWORD  gamemode,
                                       DWORD  creationtime) {
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
                                        +2;
  ipx_advgameinfo->header.type      = ADTYPE_GAMEINFO;
  ipx_advgameinfo->header.reserved  = 0;
  ipx_advgameinfo->header.programid = ipx_programid;
  ipx_advgameinfo->header.versionid = ipx_versionid;
  strncpy(ipx_advgameinfo->strings,
          gamename,
          SNETSPI_MAXSTRINGLENGTH);
  strncpy(ipx_advgameinfo->strings
            +min(SNETSPI_MAXSTRINGLENGTH,strlen(ipx_advgameinfo->strings)+1),
          gamedescription,
          SNETSPI_MAXSTRINGLENGTH);
  ipx_advgameinfo->header.checksum = GenerateChecksum(ipx_advgameinfo,
                                                      ipx_advgameinfo->header.length);
  ipx_critsect.Leave();

  // SEND THE FIRST ADVERTISEMENT
  SendAdvertisement();

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
    QueueSendTo(ipx_advsocket,
                (const char *)ipx_advgameinfo,
                ipx_advgameinfo->header.length,
                0,
                (const sockaddr *)&ipx_broadcastaddr,
                sizeof(SOCKADDR_IPX));
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
    *hintnextcall = 1000;

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
LPCSTR   ipx_desc = "IPX Latency/Corrupt Test";
LPCSTR   ipx_req  = "All computers must be connected to an IPX-compatible network.";
SNETCAPS ipx_caps = {sizeof(SNETCAPS),           // size
                     SNET_CAPS_PAGELOCKEDBUFFERS // flags
                       | SNET_CAPS_BASICINTERFACE,
                     MAXMESSAGESIZE,             // max message size
                     16,                         // max queue size,
                     MAXPLAYERS,                 // max players,
                     1500,                       // bytes per second
                     500,                        // latency (ms)
                     4,                          // default turns per second
                     2};                         // default turns in transit
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

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
extern "C" BOOL APIENTRY Query (DWORD        index,
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

    default:
      return 0;

  }
}

//===========================================================================
extern "C" BOOL APIENTRY Bind (DWORD       index,
                               SNETSPIPTR *spi) {
  if (!spi)
    return 0;

  switch (index) {

    case 0:
      *spi = &ipx_spi;
      return 1;

    default:
      return 0;

  }
}

//===========================================================================
extern "C" BOOL APIENTRY DllMain (HINSTANCE passinstance, DWORD reason, LPVOID) {
  if (reason == DLL_PROCESS_ATTACH)
    ipx_instance = passinstance;
  return 1;
}

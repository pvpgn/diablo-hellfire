/****************************************************************************
*
*  SNET.CPP
*  Storm networking functions
*
*  By Michael O'Brien (4/19/96)
*
*  SNet implements multiple independent communication streams between each
*  player.  It guarantees delivery of all messages, using different
*  techniques depending on the message type.
*
***/

#include "pch.h"
#pragma  hdrstop

#ifdef   _DEBUG
#define  TRACING
#define  UNSIGNEDSNPS
#endif

#pragma  warning(disable:4200)  // don't warn about zero-sized arrays

#define  MF_ACK                       0x01
#define  MF_RESENDREQUEST             0x02
#define  MF_FORWARDED                 0x04

#define  PF_JOINING                   0x00000004
#define  PF_LEAVING                   0x00000008

#define  NOPLAYER                     0xFF

#define  ESTIMATEDPACKETOVERHEAD      64
#define  DELETECONNTIME               50000
#define  MINSPISIZE                   (20*sizeof(DWORD))
#define  PINGFREQUENCY                20000

#define  REGISTERTYPE                 'SNET'
#define  REGISTERSUBTYPE_SNETEVENT    1
#define  REGISTERSUBTYPE_SYSEVENT     2

#define  SENDBUFSIZE                  8192

#define  SNET_NETWORKVERSION          1

#define  SYS_UNUSED                   0
#define  SYS_INITIALCONTACT           1
#define  SYS_CIRCUITCHECK             2
#define  SYS_CIRCUITCHECKRESPONSE     3
#define  SYS_PING                     4
#define  SYS_PINGRESPONSE             5
#define  SYS_PLAYERINFO               6
#define  SYS_PLAYERJOIN               7
#define  SYS_PLAYERJOIN_ACCEPTSTART   8
#define  SYS_PLAYERJOIN_ACCEPTDONE    9
#define  SYS_PLAYERJOIN_REJECT        10
#define  SYS_PLAYERLEAVE              11
#define  SYS_DROPPLAYER               12
#define  SYS_NEWGAMEOWNER             13
#define  SYSMSGS                      14

#define  TYPE_SYSTEM                  0
#define  TYPE_MESSAGE                 1
#define  TYPE_TURN                    2
#define  TYPES                        3

typedef struct _CLIENTDATA {
    DWORD bytes;
    DWORD numplayers;
    DWORD maxplayers;
} CLIENTDATA, *CLIENTDATAPTR;

typedef struct _HEADER {
    WORD checksum; // checksum must be first field
    WORD bytes;    // bytes including header
    WORD sequence;
    WORD acksequence;
    BYTE type;
    BYTE subtype;
    BYTE playerid;
    BYTE flags;
} HEADER, *HEADERPTR;

typedef struct _PACKET {
    HEADER header;
    BYTE   data[0];
} PACKET, *PACKETPTR;

NODEDECL(MESSAGE) {
    SNETADDRPTR addr;
    PACKETPTR   data;
    DWORD       databytes;
    BOOL        local;
    DWORD       sendtime;
    DWORD       resendtime;
} *MESSAGEPTR;

NODEDECL(CONNREC) {
    char          name[SNETSPI_MAXSTRINGLENGTH];
    char          desc[SNETSPI_MAXSTRINGLENGTH];
    SNETADDR      addr;
    DWORD         flags;
    DWORD         lastreceivetime;          // last time any packet was received
    DWORD         lastrequesttime;          // last time we requested a turn
    DWORD         lastpingtime;             // last time we sent a ping
    DWORD         latency;
    DWORD         peaklatency;
    LIST(MESSAGE) outgoingqueue[TYPES];     // already sent; waiting for ack
    LIST(MESSAGE) incomingqueue[TYPES];     // waiting to be processed
    LIST(MESSAGE) processing[TYPES];        // given to app; valid until next call
    LIST(MESSAGE) oldturns;                 // old turns from this player
    WORD          outgoingsequence[TYPES];  //   (in case we need to send them
    WORD          incomingsequence[TYPES];  //    to another player)
    WORD          lastprocessedturn;        // last turn ack from this player
    WORD          availablesequence[TYPES]; // highest available incoming sequence
    WORD          acksequence[TYPES];       // last message we acked
    DWORD         acktime[TYPES];           // time of unacked message processing
    BOOL          gameowner;
    BOOL          establishing;
    DWORD         exitcode;
    WORD          finalsequence;
    BYTE          playerid;
} *CONNPTR;

typedef struct _PERFDATAREC {
    DWORD value;
    DWORD type;
    LONG  scale;
    BOOL  providerspecific;
} PERFDATAREC, *PERFDATAPTR;

NODEDECL(PROVIDERINFO) {
    char     filename[MAX_PATH];
    DWORD    index;
    DWORD    id;
    char     desc[SNETSPI_MAXSTRINGLENGTH];
    char     req[SNETSPI_MAXSTRINGLENGTH];
    SNETCAPS caps;
} *PROVIDERINFOPTR;

typedef struct _UIPARAMS {
    SNETCAPSPTR        mincaps;
    SNETPROGRAMDATAPTR programdata;
    SNETPLAYERDATAPTR  playerdata;
    SNETUIDATAPTR      interfacedata;
    SNETVERSIONDATAPTR versiondata;
} UIPARAMS, *UIPARAMSPTR;

NODEDECL(USEREVENT) {
    SNETEVENT event;
} *USEREVENTPTR;

typedef struct _SYSEVENT {
    SNETADDRPTR senderaddr;
    LPVOID      data;
    DWORD       databytes;
    BYTE        senderplayerid;
    BYTE        eventid;
} SYSEVENT, *SYSEVENTPTR;

typedef struct _SYSEVENTDATA_DROPPLAYER {
    DWORD playerid;
    DWORD finalsequence;
    DWORD exitcode;
} SYSEVENTDATA_DROPPLAYER, *SYSEVENTDATA_DROPPLAYERPTR;

typedef struct _SYSEVENTDATA_PLAYERJOIN {
    char namedescpass[3*SNETSPI_MAXSTRINGLENGTH];
} SYSEVENTDATA_PLAYERJOIN, *SYSEVENTDATA_PLAYERJOINPTR;

typedef struct _SYSEVENTDATA_PLAYERJOIN_ACCEPTSTART {
    DWORD playerid;
    DWORD playersallowed;
    DWORD nextturn;
    DWORD gamemode;
    DWORD runningtime;
    char  namedescpass[3*SNETSPI_MAXSTRINGLENGTH];
} SYSEVENTDATA_PLAYERJOIN_ACCEPTSTART, *SYSEVENTDATA_PLAYERJOIN_ACCEPTSTARTPTR;

typedef struct _SYSEVENTDATA_PLAYERINFO {
    DWORD    bytes;
    DWORD    playerid;
    BOOL     gameowner;
    DWORD    flags;
    DWORD    startingturn;
    SNETADDR addr;
    char     namedesc[SNETSPI_MAXSTRINGLENGTH*2];
} SYSEVENTDATA_PLAYERINFO, *SYSEVENTDATA_PLAYERINFOPTR;

typedef struct _SYSEVENTDATA_PLAYERLEAVE {
    DWORD finalsequence;
    DWORD exitcode;
} SYSEVENTDATA_PLAYERLEAVE, *SYSEVENTDATA_PLAYERLEAVEPTR;

typedef void (CALLBACK *SYSEVENTPROC)(SYSEVENTPTR);

static CCritSect s_api_critsect;
static DWORD     s_api_playeroffset = 0;

static CONNPTR inline ConnFindByAddr (SNETADDRPTR addr);
static CONNPTR inline ConnFindLocal ();
static void ConnSetCurrentMessage (CONNPTR conn, BYTE type, MESSAGEPTR message);
static void RecvProcessExternalMessages ();
static void RecvProcessIncomingPackets ();
static void SysQueueUserEvent (DWORD  eventid,
                               DWORD  playerid,
                               LPVOID data,
                               DWORD  databytes);
static void UiGetProgramDescription (LPCSTR             programname,
                                     SNETVERSIONDATAPTR versionptr,
                                     LPSTR              buffer,
                                     DWORD              buffersize);

/****************************************************************************
*
*  TRACE FUNCTIONS
*
***/

#ifdef TRACING

static CSLog s_trace_log("Internal","SNet Trace File");

//===========================================================================
static void TraceDumpAddr (HSLOG       log,
                           LPCSTR      addrname,
                           SNETADDRPTR addr,
                           BYTE        playerid) {
  char outstr[80];
  wsprintf(outstr,"  %s=",addrname);
  for (unsigned loop = 0; loop < sizeof(SNETADDR); ++loop)
    wsprintf(outstr+SStrLen(outstr),"%02x",(DWORD)*((LPBYTE)addr+loop));
  wsprintf(outstr+SStrLen(outstr)," (%x)",(DWORD)playerid);
  SLogWrite(log,outstr);
}

//===========================================================================
static void TraceDumpDataBlocks (HSLOG              log,
                                 SNETPROGRAMDATAPTR programdata,
                                 SNETPLAYERDATAPTR  playerdata,
                                 SNETUIDATAPTR      interfacedata,
                                 SNETVERSIONDATAPTR versiondata) {
  if (programdata && (programdata->size >= sizeof(SNETPROGRAMDATA))) {
    SLogWrite(log,"  programdata.size                 = %u"    ,programdata->size);
    SLogWrite(log,"  programdata.programname          = \"%s\"",programdata->programname);
    SLogWrite(log,"  programdata.programdescription   = \"%s\"",programdata->programdescription);
    SLogWrite(log,"  programdata.programid            = 0x%08x",programdata->programid);
    SLogWrite(log,"  programdata.versionid            = 0x%08x",programdata->versionid);
    SLogWrite(log,"  programdata.reserved1            = %u"    ,programdata->reserved1);
    SLogWrite(log,"  programdata.maxplayers           = %u"    ,programdata->maxplayers);
    SLogWrite(log,"  programdata.initdata             = 0x%08x",programdata->initdata);
    SLogWrite(log,"  programdata.initdatabytes        = %u"    ,programdata->initdatabytes);
    SLogWrite(log,"  programdata.reserved2            = 0x%08x",programdata->reserved2);
    SLogWrite(log,"  programdata.optcategorybits      = 0x%08x",programdata->optcategorybits);
  }
  if (playerdata && (playerdata->size >= sizeof(SNETPLAYERDATA))) {
    SLogWrite(log,"  playerdata.size                  = %u"    ,playerdata->size);
    SLogWrite(log,"  playerdata.playername            = \"%s\"",playerdata->playername);
    SLogWrite(log,"  playerdata.playerdescription     = \"%s\"",playerdata->playerdescription);
  }
  if (interfacedata && (interfacedata->size >= sizeof(SNETUIDATA))) {
    SLogWrite(log,"  interfacedata.size               = %u"    ,interfacedata->size);
    SLogWrite(log,"  interfacedata.uiflags            = 0x%08x",interfacedata->uiflags);
    SLogWrite(log,"  interfacedata.parentwindow       = 0x%08x",interfacedata->parentwindow);
    SLogWrite(log,"  interfacedata.artcallback        = 0x%08x",interfacedata->artcallback);
    SLogWrite(log,"  interfacedata.authcallback       = 0x%08x",interfacedata->authcallback);
    SLogWrite(log,"  interfacedata.createcallback     = 0x%08x",interfacedata->createcallback);
    SLogWrite(log,"  interfacedata.drawdesccallback   = 0x%08x",interfacedata->drawdesccallback);
    SLogWrite(log,"  interfacedata.selectedcallback   = 0x%08x",interfacedata->selectedcallback);
    SLogWrite(log,"  interfacedata.messageboxcallback = 0x%08x",interfacedata->messageboxcallback);
    SLogWrite(log,"  interfacedata.soundcallback      = 0x%08x",interfacedata->soundcallback);
    SLogWrite(log,"  interfacedata.statuscallback     = 0x%08x",interfacedata->statuscallback);
    SLogWrite(log,"  interfacedata.getdatacallback    = 0x%08x",interfacedata->getdatacallback);
    SLogWrite(log,"  interfacedata.categorycallback   = 0x%08x",interfacedata->categorycallback);
  }
  if (versiondata && (versiondata->size >= sizeof(SNETVERSIONDATA))) {
    SLogWrite(log,"  versiondata.size                 = %u"    ,versiondata->size);
    SLogWrite(log,"  versiondata.versionstring        = \"%s\"",versiondata->versionstring);
    SLogWrite(log,"  versiondata.executablefile       = \"%s\"",versiondata->executablefile);
    SLogWrite(log,"  versiondata.originalarchivefile  = \"%s\"",versiondata->originalarchivefile);
    SLogWrite(log,"  versiondata.patcharchivefile     = \"%s\"",versiondata->patcharchivefile);
  }
}

//===========================================================================
static void TraceDumpMsg (HSLOG  log,
                          DWORD  playerid,
                          DWORD  sequence,
                          LPVOID data,
                          DWORD  databytes) {

  // DUMP THE PLAYER ID
  if (playerid != 0xFFFFFFFF)
    SLogWrite(log,
             "(from player %u, sequence=%u)\n",
             playerid+s_api_playeroffset,sequence);
  else if (sequence != 0xFFFFFFFF)
    SLogWrite(log,
             "(sequence=%u)\n",sequence);

  // DUMP THE DATA
  SLogDump(log,data,databytes);

}

#define  TRACEDUMP            SLogDump
#define  TRACEDUMPADDR        TraceDumpAddr
#define  TRACEDUMPDATABLOCKS  TraceDumpDataBlocks
#define  TRACEDUMPMSG         TraceDumpMsg
#define  TRACEHANDLE          s_trace_log.GetHandle()
#define  TRACEOUT             SLogWrite
#define  TRACEPEND            SLogPend

#else

#define  TRACEDUMP            
#define  TRACEDUMPADDR        
#define  TRACEDUMPDATABLOCKS  
#define  TRACEDUMPMSG         
#define  TRACEHANDLE          0
#define  TRACEOUT             
#define  TRACEPEND            

#endif

/****************************************************************************
*
*  PERFORMANCE FUNCTIONS
*
***/

static PERFDATAREC s_perf_data[SNET_PERFIDNUM]
         = {{0,0                     , 0,0},  // unused
            {0,SNET_PERFTYPE_RAWCOUNT,-1,0},  // SNET_PERFID_TURN
            {0,0                     , 0,0},  // unused
            {0,0                     , 0,0},  // unused
            {0,SNET_PERFTYPE_COUNTER ,-1,0},  // SNET_PERFID_TURNSSENT
            {0,SNET_PERFTYPE_COUNTER ,-1,0},  // SNET_PERFID_TURNSRECV
            {0,SNET_PERFTYPE_COUNTER ,-1,0},  // SNET_PERFID_MSGSENT
            {0,SNET_PERFTYPE_COUNTER ,-1,0},  // SNET_PERFID_MSGRECV
            {0,SNET_PERFTYPE_COUNTER ,-4,0},  // SNET_PERFID_USERBYTESSENT
            {0,SNET_PERFTYPE_COUNTER ,-4,0},  // SNET_PERFID_USERBYTESRECV
            {0,SNET_PERFTYPE_COUNTER ,-4,0},  // SNET_PERFID_TOTALBYTESSENT
            {0,SNET_PERFTYPE_COUNTER ,-4,0},  // SNET_PERFID_TOTALBYTESRECV
            {0,SNET_PERFTYPE_COUNTER ,-1,1},  // SNET_PERFID_PKTSENTONWIRE
            {0,SNET_PERFTYPE_COUNTER ,-1,1},  // SNET_PERFID_PKTRECVONWIRE
            {0,SNET_PERFTYPE_COUNTER ,-4,1},  // SNET_PERFID_BYTESSENTONWIRE
            {0,SNET_PERFTYPE_COUNTER ,-4,1}}; // SNET_PERFID_BYTESRECVONWIRE

//===========================================================================
static void inline PerfAdd (DWORD id, DWORD value) {
  VALIDATEBEGIN;
  VALIDATE(id < SNET_PERFIDNUM);
  VALIDATEENDVOID;

  s_perf_data[id].value += value;
}

//===========================================================================
static void inline PerfDecrement (DWORD id) {
  VALIDATEBEGIN;
  VALIDATE(id < SNET_PERFIDNUM);
  VALIDATEENDVOID;

  InterlockedDecrement((LONG *)&s_perf_data[id].value);
}

//===========================================================================
static void inline PerfIncrement (DWORD id) {
  VALIDATEBEGIN;
  VALIDATE(id < SNET_PERFIDNUM);
  VALIDATEENDVOID;

  InterlockedIncrement((LONG *)&s_perf_data[id].value);
}

//===========================================================================
static void inline PerfSet (DWORD id, DWORD value) {
  VALIDATEBEGIN;
  VALIDATE(id < SNET_PERFIDNUM);
  VALIDATEENDVOID;

  s_perf_data[id].value = value;
}

/****************************************************************************
*
*  SERVICE PROVIDER MANAGEMENT FUNCTIONS
*
***/

static SNETSPIPTR         s_spi                = NULL;
static HINSTANCE          s_spi_lib            = (HINSTANCE)0;
static DWORD              s_spi_outgoingtime   = 0;
static LIST(PROVIDERINFO) s_spi_providerlist;
static PROVIDERINFOPTR    s_spi_providerptr    = NULL;
static BOOL               s_spi_providersfound = FALSE;
static LPVOID             s_spi_sendbuffer     = NULL;
static DWORD              s_spi_timetoackturn  = 250;
static DWORD              s_spi_timetoblock    = 5000;
static DWORD              s_spi_timetogiveup   = 1000;
static DWORD              s_spi_timetorequest  = 25;
static DWORD              s_spi_timetoresend   = 50;

static LPVOID SpiLoadCapsSignature (LPCSTR filename);

//===========================================================================
static LPVOID inline SpiExtractCaps (LPVOID       capssig,
                                     DWORD       *id,
                                     LPCSTR      *desc,
                                     LPCSTR      *req,
                                     SNETCAPSPTR *caps) {
  VALIDATEBEGIN;
  VALIDATE(capssig);
  VALIDATEEND;

  DWORD  bytes = *(LPDWORD)capssig;
  LPCSTR start = (LPCSTR)capssig+sizeof(DWORD);
  LPCSTR curr  = start;
  *id = *(LPDWORD)curr;
  curr += sizeof(DWORD);
  *desc = curr;
  curr += SStrLen(curr)+1;
  *req = curr;
  curr += SStrLen(curr)+1;
  *caps = (SNETCAPSPTR)curr;
  curr += ((SNETCAPSPTR)curr)->size;
  if ((DWORD)(curr-start) != bytes)
    return NULL;

  return (LPVOID)curr;
}

//===========================================================================
static int inline SpiCheckProviderOrder (PROVIDERINFOPTR first,
                                         PROVIDERINFOPTR second) {

  // CHECK WHETHER ONE OR BOTH OF THE PROVIDERS IN IN OUR PREFERRED LIST
#define BASENUM 6
  static const DWORD baseorder[BASENUM] = {'BNET','IPXN','IPXW','MODM','SCBL','MSDP'};
  int firstindex  = 0x7FFFFFFF;
  int secondindex = 0x7FFFFFFF;
  for (int loop = 0; loop < BASENUM; ++loop) {
    if (first->id == baseorder[loop])
      firstindex = loop;
    if (second->id == baseorder[loop])
      secondindex = loop;
  }
#undef BASENUM

  // IF SO, SORT BASED ON THE ORDER IN THE LIST
  if ((firstindex != 0x7FFFFFFF) || (secondindex != 0x7FFFFFFF))
    return secondindex-firstindex;

  // OTHERWISE, SORT ON ALPHABETICAL ORDER
  else
    return _stricmp(first->desc,second->desc);

}

//===========================================================================
static void SpiDestroy (BOOL clearproviderlist) {

  // CALL THE PROVIDER'S DESTROY FUNCTION
  if (s_spi_lib && s_spi) {
    TRACEOUT(TRACEHANDLE,"  spiDestroy()");
    s_spi->Destroy();
  }

  // UNBIND FROM THE CURRENT PROVIDER
  if (s_spi_lib) {
    FreeLibrary(s_spi_lib);
    s_spi_lib = (HINSTANCE)0;
  }
  if (s_spi) {
    DEL(s_spi);
    s_spi = NULL;
  }

  // FREE THE SEND BUFFER
  if (s_spi_sendbuffer) {
    VirtualUnlock(s_spi_sendbuffer,SENDBUFSIZE);
    VirtualFree(s_spi_sendbuffer,0,MEM_RELEASE);
    s_spi_sendbuffer = NULL;
  }

  // DESTROY THE LIST OF PROVIDERS
  if (clearproviderlist) {
    s_spi_providerlist.Clear();
    s_spi_providersfound = FALSE;
  }

}

//===========================================================================
static void SpiFindAllProviders () {

  // MAKE SURE THIS FUNCTION IS NOT CALLED MORE THAN ONCE
  if (s_spi_providersfound)
    return;
  else
    s_spi_providersfound = TRUE;

  // CREATE THE SEARCH SPECIFICATION
  char basepath[MAX_PATH] = "";
  GetModuleFileName((HMODULE)StormGetInstance(),basepath,MAX_PATH);
  {
    LPTSTR curr = basepath;
    while (SStrChr(curr,'\\'))
      curr = SStrChr(curr,'\\')+1;
    *curr = 0;
  }
  char filespec[MAX_PATH] = "";
  wsprintf(filespec,"%s*.snp",basepath);

  // SEARCH FOR PROVIDER LIBRARIES
  WIN32_FIND_DATA finddata;
  HANDLE          findhandle = FindFirstFile(filespec,&finddata);
  if (findhandle == INVALID_HANDLE_VALUE)
    return;
  do {

    // BUILD THE COMPLETE FILENAME
    char filename[MAX_PATH];
    wsprintf(filename,"%s%s",basepath,finddata.cFileName);

    // LOAD THE CAPABILITIES SIGNATURE BLOCK FOR THE NEXT LIBRARY
    LPVOID capssig = NULL;
#ifndef UNSIGNEDSNPS
    capssig = SpiLoadCapsSignature(filename);
    if (!capssig)
      continue;
#endif

    // LOAD THE NEXT LIBRARY
    HINSTANCE lib = LoadLibrary(filename);
    if (!lib) {
      if (capssig)
        FREE(capssig);
      continue;
    }

    // GET THE ADDRESS OF THE QUERY FUNCTION
    SNETSPIQUERY query = (SNETSPIQUERY)GetProcAddress(lib,"SnpQuery");
    if (!query) {
      if (capssig)
        FREE(capssig);
      FreeLibrary(lib);
      continue;
    }

    // QUERY ALL PROVIDER INTERFACES CONTAINED IN THIS LIBRARY
    LPVOID currcapssig = capssig;
    DWORD  index       = 0;
    for (;;) {

      // QUERY THE PROVIDER'S CAPABILITIES TO DETERMINE WHETHER IT RETURNS
      // SUCCESS OR FAILURE; HOWEVER, DON'T USE THE CAPABILITIES IT RETURNS,
      // BECAUSE THEY'RE TOO EASY TO HACK
#ifndef UNSIGNEDSNPS
      {
#endif
        DWORD       id;
        LPCSTR      desc;
        LPCSTR      req;
        SNETCAPSPTR caps = NULL;
        if (!query(index,&id,&desc,&req,&caps))
          break;
        if (!(caps && (caps->size >= sizeof(SNETCAPS))))
          break;
#ifdef _DEBUG
        if (caps->flags & SNET_CAPS_RETAILONLY)
          break;
#else
        if (caps->flags & SNET_CAPS_DEBUGONLY)
          break;
#endif
#ifndef UNSIGNEDSNPS
      }

      // RETRIEVE THE REAL CAPABILITIES FROM THE SIGNATURE DATA
      DWORD       id;
      LPCSTR      desc;
      LPCSTR      req;
      SNETCAPSPTR caps = NULL;
      currcapssig = SpiExtractCaps(currcapssig,&id,&desc,&req,&caps);
      if (!currcapssig)
        break;
      if (!(caps && (caps->size >= sizeof(SNETCAPS))))
        break;
#endif

      // ALLOCATE A NEW PROVIDER STRUCTURE
      PROVIDERINFOPTR info = s_spi_providerlist.NewNode(LIST_UNLINKED);

      // FILL OUT THE PROVIDER INFORMATION STRUCTURE
      SStrCopy(info->filename,filename,MAX_PATH);
      info->index = index;
      info->id    = id;
      if (desc)
        SStrCopy(info->desc,desc,SNETSPI_MAXSTRINGLENGTH);
      if (req)
        SStrCopy(info->req,req,SNETSPI_MAXSTRINGLENGTH);
      CopyMemory(&info->caps,caps,sizeof(SNETCAPS));
      info->caps.maxmessagesize -= (sizeof(HEADER)+sizeof(DWORD));

      // ADD IT TO THE LIST IN SORTED ORDER
      PROVIDERINFOPTR curr = s_spi_providerlist.Head();
      while (curr && (SpiCheckProviderOrder(curr,info) <= 0))
        curr = curr->Next();
      s_spi_providerlist.LinkNode(info,LIST_LINK_BEFORE,curr);

      ++index;
    } 

    // FREE THE LIBRARY
    FreeLibrary(lib);

    // FREE THE CAPABILITIES SIGNATURE
    if (capssig)
      FREE(capssig);

  } while (FindNextFile(findhandle,&finddata));
  FindClose(findhandle);
}

//===========================================================================
static BOOL SpiInitialize (DWORD              providerid,
                           SNETPROGRAMDATAPTR programdata,
                           SNETPLAYERDATAPTR  playerdata,
                           SNETUIDATAPTR      interfacedata,
                           SNETVERSIONDATAPTR versiondata,
                           HANDLE             recvevent) {

  // DESTROY THE EXISTING PROVIDER IF ONE IS INITIALIZED
  SpiDestroy(FALSE);

  // BUILD A LIST OF PROVIDERS IF WE DON'T ALREADY HAVE ONE
  SpiFindAllProviders();

  // FIND THE DESIRED PROVIDER
  s_spi_providerptr = s_spi_providerlist.Head();
  while (s_spi_providerptr && (s_spi_providerptr->id != providerid))
    s_spi_providerptr = s_spi_providerptr->Next();
  if (!s_spi_providerptr) {
    SpiDestroy(FALSE);
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    return FALSE;
  }

  // DETERMINE TIMEOUTS BASED ON THE LATENCY OF THIS PROVIDER
  s_spi_timetoackturn = max( 250, 5*s_spi_providerptr->caps.latencyms);
  s_spi_timetoblock   = max(5000,12*s_spi_providerptr->caps.latencyms);
  s_spi_timetogiveup  = max(1000, 4*s_spi_providerptr->caps.latencyms);
  s_spi_timetorequest = max(  25,s_spi_providerptr->caps.latencyms/2);
  s_spi_timetoresend  = max(  50,s_spi_providerptr->caps.latencyms);

  // IF THIS PROVIDER REQUIRES PACKETS TO BE IN PAGE-LOCKED MEMORY, ALLOCATE
  // A SEND BUFFER
  if (s_spi_providerptr->caps.flags & SNET_CAPS_PAGELOCKEDBUFFERS) {
    s_spi_sendbuffer = VirtualAlloc(NULL,SENDBUFSIZE,MEM_COMMIT,PAGE_READWRITE);
    if (s_spi_sendbuffer)
      VirtualLock(s_spi_sendbuffer,SENDBUFSIZE);
    else {
      SpiDestroy(FALSE);
      SErrSetLastError(SNET_ERROR_NOT_ENOUGH_MEMORY);
      return FALSE;
    }
  }

  // LOAD THE LIBRARY CONTAINING THE PROVIDER
  s_spi_lib = LoadLibrary(s_spi_providerptr->filename);
  if (!s_spi_lib) {
    SpiDestroy(FALSE);
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    return FALSE;
  }

  // GET A POINTER TO THE BIND FUNCTION
  SNETSPIBIND bind = (SNETSPIBIND)GetProcAddress(s_spi_lib,"SnpBind");
  if (!bind) {
    SpiDestroy(FALSE);
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    return FALSE;
  }

  // BIND TO THE PROVIDER
  SNETSPIPTR returnedspi = NULL;
  bind(s_spi_providerptr->index,&returnedspi);
  if ((!returnedspi) || (returnedspi->size < MINSPISIZE)) {
    SpiDestroy(FALSE);
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    return FALSE;
  }

  // SAVE THE PROVIDER INTERFACE POINTERS
  if (!s_spi)
    s_spi = NEW(SNETSPI);
  ZeroMemory(s_spi,sizeof(SNETSPI));
  CopyMemory(s_spi,returnedspi,min(returnedspi->size,sizeof(SNETSPI)));

  // CALL THE PROVIDER'S INITIALIZE FUNCTION
  TRACEOUT(TRACEHANDLE,
           "  spiInitialize(0x%08x,0x%08x,0x%08x,0x%08x,0x%08x)",
           programdata,playerdata,interfacedata,versiondata,recvevent);
  if (!s_spi->Initialize(programdata,
                         playerdata,
                         interfacedata,
                         versiondata,
                         recvevent)) {
    DWORD lasterror = SErrGetLastError();
    SpiDestroy(FALSE);
    SErrSetLastError(lasterror);
    return FALSE;
  }

  return TRUE;
}

//===========================================================================
static LPVOID SpiLoadCapsSignature (LPCSTR filename) {
  HSARCHIVE archive = (HSARCHIVE)0;
  LPVOID    buffer  = NULL;
  HSFILE    file    = (HSFILE)0;
  TRY {

    // OPEN THE MPQ ARCHIVE EMBEDDED IN THE FILE
    if (!SFileOpenArchive(filename,0,0,&archive))
      LEAVE;

    // AUTHENTICATE THE ARCHIVE
    {
      DWORD authtype;
      SFileAuthenticateArchive(archive,&authtype);
      if ((authtype != SFILE_AUTH_UNABLETOAUTHENTICATE) &&
          (authtype  < SFILE_AUTH_FIRSTAUTHENTIC))
        LEAVE;
    }

    // OPEN THE CAPABILITIES DATA FROM THE ARCHIVE
    if (!SFileOpenFileEx(archive,"caps.dat",0,&file))
      LEAVE;

    // CREATE A BUFFER FOR THE CAPABILITIES DATA
    DWORD bytes = SFileGetFileSize(file,NULL);
    buffer = ALLOC(bytes);

    // READ THE CAPABILITIES DATA
    {
      DWORD bytesread;
      SFileReadFile(file,buffer,bytes,&bytesread,NULL);
    }

  }
  FINALLY {
    if (file)
      SFileCloseFile(file);
    if (archive)
      SFileCloseArchive(archive);
  }
  return buffer;
}

//===========================================================================
static BOOL SpiMeetsMinimumCaps (SNETCAPSPTR curr, SNETCAPSPTR mincaps) {
  if (!(curr && mincaps))
    return TRUE;
  return (((mincaps->flags & curr->flags) == mincaps->flags) &&
          (mincaps->maxmessagesize <= curr->maxmessagesize) &&
          (mincaps->maxqueuesize   <= curr->maxqueuesize  ) &&
          (mincaps->maxplayers     <= curr->maxplayers    ) &&
          (mincaps->bytessec       <= curr->bytessec      ) &&
          ((!mincaps->latencyms) ||
           (mincaps->latencyms >= curr->latencyms)));
}

//===========================================================================
static BOOL SpiNormalizeDataBlocks (SNETPROGRAMDATAPTR programdata,
                                    SNETPLAYERDATAPTR  playerdata,
                                    SNETUIDATAPTR      interfacedata,
                                    SNETVERSIONDATAPTR versiondata,
                                    SNETPROGRAMDATAPTR modprogramdata,
                                    SNETPLAYERDATAPTR  modplayerdata,
                                    SNETUIDATAPTR      modinterfacedata,
                                    SNETVERSIONDATAPTR modversiondata) {

  // NORMALIZE THE PROGRAM DATA
  ZeroMemory(modprogramdata,sizeof(SNETPROGRAMDATA));
  if (programdata)
    CopyMemory(modprogramdata,programdata,min(sizeof(SNETPROGRAMDATA),programdata->size));
  modprogramdata->size = sizeof(SNETPROGRAMDATA);
  if (!modprogramdata->programname)
    modprogramdata->programname = "";
  char generateddescription[256] = "";
  if (!modprogramdata->programdescription) {
    UiGetProgramDescription(modprogramdata->programname,
                            versiondata,
                            generateddescription,
                            256);
    modprogramdata->programdescription = generateddescription;
  }

  // NORMALIZE THE PLAYER DATA
  ZeroMemory(modplayerdata,sizeof(SNETPLAYERDATA));
  if (playerdata)
    CopyMemory(modplayerdata,playerdata,min(sizeof(SNETPLAYERDATA),playerdata->size));
  modplayerdata->size = sizeof(SNETPLAYERDATA);
  if (!modplayerdata->playername)
    modplayerdata->playername = "";
  if (!modplayerdata->playerdescription)
    modplayerdata->playerdescription = "";

  // NORMALIZE THE INTERFACE DATA
  ZeroMemory(modinterfacedata,sizeof(SNETUIDATA));
  if (interfacedata)
    CopyMemory(modinterfacedata,interfacedata,min(sizeof(SNETUIDATA),interfacedata->size));
  modinterfacedata->size = sizeof(SNETUIDATA);
  if (!modinterfacedata->parentwindow)
    modinterfacedata->parentwindow = SDrawGetFrameWindow();
  if (!modinterfacedata->messageboxcallback)
    modinterfacedata->messageboxcallback = MessageBoxA;

  // NORMALIZE THE VERSION DATA
  ZeroMemory(modversiondata,sizeof(SNETVERSIONDATA));
  if (versiondata)
    CopyMemory(modversiondata,versiondata,min(sizeof(SNETVERSIONDATA),versiondata->size));
  modversiondata->size = sizeof(SNETVERSIONDATA);

  return TRUE;
}

//===========================================================================
static BOOL SpiSend (DWORD        addresses,
                     SNETADDRPTR *addrlist,
                     LPVOID       data,
                     DWORD        databytes) {
  if (!s_spi)
    return FALSE;

  // UPDATE OUR ESTIMATE OF HOW LONG IT WILL TAKE TO SEND ALL THE OUTGOING
  // DATA THAT WE HAVE QUEUED
  {
    DWORD currtime = GetTickCount();
    if (currtime-s_spi_outgoingtime < 0x7FFFFFFF)
      s_spi_outgoingtime = currtime;
    s_spi_outgoingtime += (addresses*(databytes+ESTIMATEDPACKETOVERHEAD))
                               *1000
                               /s_spi_providerptr->caps.bytessec;
  }

  // IF THIS SERVICE PROVIDER REQUIRES ITS DATA TO BE PAGE LOCKED, MOVE
  // THE ADDRESSES AND DATA INTO A PAGE LOCKED BUFFER
  if (s_spi_sendbuffer) {
    DWORD ptrbytes  = 16*sizeof(SNETADDRPTR);
    DWORD addrbytes = 16*sizeof(SNETADDR);
    if ((addresses > 16) || (databytes > SENDBUFSIZE-addrbytes))
      return FALSE;
    DWORD loop;
    for (loop = 0; loop < addresses; ++loop)
      *((SNETADDRPTR *)s_spi_sendbuffer+loop) = (SNETADDRPTR)((LPBYTE)s_spi_sendbuffer+ptrbytes+loop*sizeof(SNETADDR));
    for (loop = 0; loop < addresses; ++loop)
      CopyMemory((LPBYTE)s_spi_sendbuffer+ptrbytes+loop*sizeof(SNETADDR),*(addrlist+loop),sizeof(SNETADDR));
    CopyMemory((LPBYTE)s_spi_sendbuffer+ptrbytes+addrbytes,data,databytes);
    data = (LPBYTE)s_spi_sendbuffer+ptrbytes+addrbytes;
  }

  // IF WE ARE IN DEBUG MODE, DUMP THE ADDRESSES AND PACKET DATA
#ifdef TRACING
  TRACEOUT(TRACEHANDLE,
           "  spiSend(%u,*addrlist,0x%08x,%u)",
           addresses,data,databytes);
  for (DWORD loop = 0; loop < addresses; ++loop) {
    CONNPTR targetconn = ConnFindByAddr(*(addrlist+loop));
    TRACEDUMPADDR(TRACEHANDLE,
                  "target",
                  *(addrlist+loop),
                  targetconn ? targetconn->playerid : NOPLAYER);
  }
  PACKETPTR pkt = (PACKETPTR)data;
  TRACEDUMP(TRACEHANDLE,&pkt->header,min(databytes,sizeof(HEADER)));
  if (databytes > sizeof(HEADER))
    TRACEDUMP(TRACEHANDLE,&pkt->data[0],databytes-sizeof(HEADER));
#endif

  PerfAdd(SNET_PERFID_TOTALBYTESSENT,databytes);
  return s_spi->Send(addresses,addrlist,data,databytes);
}

/****************************************************************************
*
*  PACKET UTILITY FUNCTIONS
*
***/

//===========================================================================
static void PktAllocateLocalMessage (SNETADDRPTR *addr,
                                     LPVOID      *data,
                                     DWORD        databytes) {
  *addr = NEW(SNETADDR);
  databytes += 4-(databytes & 3);
  *data = ALLOC(max(4,databytes));
}

//===========================================================================
static WORD PktComputeChecksum (LPVOID data,
                                DWORD  databytes) {
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
static void PktFreeLocalMessage (SNETADDRPTR addr,
                                 LPVOID      data,
                                 DWORD       databytes) {
  if (addr)
    DEL(addr);
  if (data)
    FREE(data);
}

//===========================================================================
static WORD PktGenerateChecksum (PACKETPTR pkt) {

  // COMPUTE THE CURRENT CHECKSUM FOR THE MESSAGE
  WORD checksum = PktComputeChecksum(((LPBYTE)pkt)+sizeof(WORD),
                                     pkt->header.bytes-sizeof(WORD));

  // COMPUTE A NEW VALUE FOR THE CHECKSUM FIELD THAT WILL MAKE THE NEW
  // CHECKSUM OF THE ENTIRE MESSAGE ZERO
  BYTE hibyte = 0xFF-((checksum >> 8)+(checksum & 0xFF)) % 0xFF;
  BYTE lobyte = 0xFF-((checksum >> 8)+hibyte)            % 0xFF;
  return MAKEWORD(lobyte,hibyte);
}

/****************************************************************************
*
*  CONNECTION MANAGEMENT FUNCTIONS
*
***/

static LIST(CONNREC) s_conn_connlist;
static LIST(CONNREC) s_conn_local;

static void ConnFree (CONNPTR conn);
static BOOL ConnResendMessage (CONNPTR conn,
                               LPVOID  data,
                               DWORD   databytes);
static void ConnSendPacket (CONNPTR   conn,
                            PACKETPTR pkt);
static MESSAGEPTR ConnSendMessage (CONNPTR target,
                                   BYTE    type,
                                   BYTE    subtype,
                                   LPVOID  data,
                                   DWORD   databytes);

//===========================================================================
static CONNPTR ConnAddRec (LISTPTR(CONNREC) list, SNETADDRPTR addr) {
  TRACEDUMPADDR(TRACEHANDLE,
                "  ConnAddRec() addr",
                addr,NOPLAYER);
  CONNPTR newconn = list->NewNode();
  CopyMemory(&newconn->addr,addr,sizeof(SNETADDR));
  newconn->playerid        = NOPLAYER;
  newconn->lastreceivetime = GetTickCount();
  return newconn;
}

//===========================================================================
static void ConnClearOldTurns (CONNPTR onlyconn) {
  CONNPTR localptr = ConnFindLocal();
  if (!localptr)
    return;

  // DETERMINE THE SEQUENCE OF THE NEXT TURN WE WILL PROCESS
  WORD sequence = localptr->incomingsequence[TYPE_TURN];

  // DETERMINE THE EARLIEST POSSIBLE SEQUENCE FOR WHICH WE MAY STILL NEED
  // TURN DATA FOR THE PURPOSES OF RESENDING TURNS ON BEHALF OF UNRESPONSIVE
  // CLIENTS
  WORD acksequence = sequence;
  {
    CONNPTR conn = s_conn_connlist.Head();
    while (conn) {
      if ((WORD)(acksequence-conn->lastprocessedturn) < 0x7FFF)
        acksequence = conn->lastprocessedturn;
      conn = conn->Next();
    }
  }

  // ITERATE THROUGH ALL CONNECTIONS
  {
    for (int local = 0; local <= 1; ++local) {
      CONNPTR conn = local ? localptr : s_conn_connlist.Head();
      while (conn) {
        if ((!onlyconn) || (conn == onlyconn)) {

          // IF THERE ARE ANY TURNS ON THIS CONNECTION'S INCOMING QUEUE
          // WITH SEQUENCE NUMBERS PRIOR TO THE CURRENT SEQUENCE NUMBER,
          // MOVE THEM TO THE OLD TURNS QUEUE
          while ((!conn->incomingqueue[TYPE_TURN].IsEmpty()) &&
                 ((WORD)(sequence-conn->incomingqueue[TYPE_TURN].Head()->data->header.sequence) > 0) &&
                 ((WORD)(sequence-conn->incomingqueue[TYPE_TURN].Head()->data->header.sequence) <= 0x7FFF)) {
            MESSAGEPTR message = conn->incomingqueue[TYPE_TURN].Head();
            conn->incomingqueue[TYPE_TURN].UnlinkNode(message);
            ConnSetCurrentMessage(conn,TYPE_TURN,message);
          }

          // IF THERE ARE ANY TURNS ON THIS CONNECTION'S OLD TURNS QUEUE
          // WITH SEQUENCE NUMBERS PRIOR TO THE EARLIEST POSSIBLE NEEDED
          // SEQUENCE, FREE THEM
          while ((!conn->oldturns.IsEmpty()) &&
                 ((WORD)(acksequence-conn->oldturns.Head()->data->header.sequence) > 0) &&
                 ((WORD)(acksequence-conn->oldturns.Head()->data->header.sequence) <= 0x7FFF)) {
            MESSAGEPTR message = conn->oldturns.Head();
            if (local)
              PktFreeLocalMessage(message->addr,message->data,message->databytes);
            else {
              TRACEOUT(TRACEHANDLE,
                       "  spiFree(0x%08x,0x%08x,%u) [all acks above turn sequence (%u>%u)]",
                       message->addr,message->data,message->databytes,acksequence,message->data->header.sequence);
              s_spi->Free(message->addr,message->data,message->databytes);
            }
            conn->oldturns.DeleteNode(message);
          }

        }
        conn = conn->Next();
      }
    }
  }

}

//===========================================================================
static void ConnDestroy () {
  for (BOOL local = FALSE; local <= TRUE; ++local) {
    LISTPTR(CONNREC) list = local ? &s_conn_local
                                  : &s_conn_connlist;
    CONNPTR currconn;
    while ((currconn = list->Head()) != NULL)
      ConnFree(currconn);
  }
}

//===========================================================================
static void ConnDestroyQueue (LISTPTR(MESSAGE) queue) {
  MESSAGEPTR currmsg;
  while ((currmsg = queue->Head()) != NULL) {
    if (currmsg->local)
      PktFreeLocalMessage(currmsg->addr,currmsg->data,currmsg->databytes);
    else if (s_spi) {
      TRACEOUT(TRACEHANDLE,
               "  spiFree(0x%08x,0x%08x,%u) [closing connection]",
               currmsg->addr,currmsg->data,currmsg->databytes);
      s_spi->Free(currmsg->addr,currmsg->data,currmsg->databytes);
    }
    queue->DeleteNode(currmsg);
  }
}

//===========================================================================
static CONNPTR inline ConnFindByAddr (SNETADDRPTR addr) {
  CONNPTR curr = s_conn_connlist.Head();
  while (curr && memcmp(&curr->addr,addr,sizeof(SNETADDR)))
    curr = curr->Next();
  if (!curr)
    curr = ConnAddRec(&s_conn_connlist,addr);
  return curr;
}

//===========================================================================
static CONNPTR inline ConnFindByPlayerId (DWORD playerid) {
  if (playerid == NOPLAYER)
    return NULL;
  for (BOOL local = TRUE; local >= FALSE; --local) {
    LISTPTR(CONNREC) list = local ? &s_conn_local
                                  : &s_conn_connlist;
    CONNPTR curr = list->Head();
    while (curr)
      if (curr->playerid == (BYTE)playerid)
        return curr;
      else
        curr  = curr->Next();
  }
  return NULL;
}

//===========================================================================
static CONNPTR inline ConnFindLocal () {
  if (!s_conn_local.IsEmpty())
    return s_conn_local.Head();
  else {
    SNETADDR addr;
    ZeroMemory(&addr,sizeof(SNETADDR));
    return ConnAddRec(&s_conn_local,&addr);
  }
}

//===========================================================================
static void ConnFree (CONNPTR conn) {
  TRACEDUMPADDR(TRACEHANDLE,
                "  ConnFree() addr",
                &conn->addr,conn->playerid);
  for (int type = 0; type < TYPES; ++type) {
    ConnDestroyQueue(&conn->outgoingqueue[type]);
    ConnDestroyQueue(&conn->incomingqueue[type]);
    ConnDestroyQueue(&conn->processing[type]);
  }
  ConnDestroyQueue(&conn->oldturns);
  if (conn == s_conn_local.Head())
    s_conn_local.DeleteNode(conn);
  else
    s_conn_connlist.DeleteNode(conn);
}

//===========================================================================
static DWORD ConnMaintainConnections () {
  DWORD   currtime = GetTickCount();
  DWORD   wait     = INFINITE;
  CONNPTR local    = ConnFindLocal();
  CONNPTR conn     = s_conn_connlist.Head();
  if (!(conn && local))
    return wait;
  CONNPTR next;
  do {
    next = conn->Next();

    // DELETE ANY CONNECTION WHICH WE HAVEN'T HEARD FROM IN FIFTY SECONDS
    if ((conn->playerid == NOPLAYER) &&
        (currtime-conn->lastreceivetime >= DELETECONNTIME)) {
      TRACEOUT(TRACEHANDLE,
               "  deleting unresponsive connection: player=%x",
               conn->playerid);
      ConnFree(conn);
      continue;
    }

    // DON'T SEND CONNECTION MAINTENANCE PACKETS TO PLAYERS WHO AREN'T
    // IN THE GAME
    if ((conn->playerid == NOPLAYER) && !conn->establishing)
      continue;

    // PING EACH ESTABLISHED CONNECTION ONCE EVERY TWENTY SECONDS
    if ((currtime-conn->lastpingtime >= PINGFREQUENCY) &&
        !conn->establishing) {
      TRACEOUT(TRACEHANDLE,
               "  pinging: player=%x",
               conn->playerid);
      conn->lastpingtime = currtime;
      ConnSendMessage(conn,TYPE_SYSTEM,SYS_PING,NULL,0);
    }

    // SEND OUT RESEND REQUESTS AS NECESSARY
    {
      for (int type = 0; type < TYPES; ++type) {
        WORD       lastsequence = conn->incomingsequence[type]-1;
        MESSAGEPTR message      = conn->incomingqueue[type].Head();
        while (message) {
          if (message->data->header.sequence-lastsequence > 1) {
            if (message->resendtime &&
                ((LONG)(message->resendtime+s_spi_timetoresend-currtime) > 0))
              wait = min(wait,message->resendtime+s_spi_timetoresend-currtime);
            else {
              TRACEOUT(TRACEHANDLE,
                       "  requesting resend: type=%u sequence=%04x player=%x",
                       type,lastsequence+1,conn->playerid);
              message->resendtime = currtime;
              PACKET pkt;
              pkt.header.checksum    = 0;
              pkt.header.bytes       = sizeof(HEADER);
              pkt.header.sequence    = lastsequence+1;
              pkt.header.acksequence = conn->availablesequence[type];
              pkt.header.type        = type;
              pkt.header.subtype     = 0;
              pkt.header.playerid    = local->playerid;
              pkt.header.flags       = MF_RESENDREQUEST;
              pkt.header.checksum    = PktGenerateChecksum(&pkt);
              ConnSendPacket(conn,&pkt);
            }
            break;
          }
          lastsequence = message->data->header.sequence;
          message      = message->Next();
        }
      }
    }

    // AUTOMATICALLY RESEND UNACKNOWLEDGED SYSTEM MESSAGES AND USER
    // ASYNCHRONOUS MESSAGES
    // (THESE TYPES OF MESSAGES ARE NOT EXPECTED BY THE RECIPIENT, SO
    //  WE CAN'T RELY ON HIM TO REQUEST A RESEND)
    {
      for (int type = 0; type < TYPES; ++type)
        if ((!conn->outgoingqueue[type].IsEmpty()) &&
            (type != TYPE_TURN)) {

          // DETERMINE HOW QUICKLY WE SHOULD EXPECT A RESPONSE TO THIS
          // MESSAGE, BASED ON BOTH LATENCY AND BANDWIDTH
          MESSAGEPTR message      = conn->outgoingqueue[type].Head();
          DWORD      bandwidth    = s_spi_providerptr->caps.bytessec;
          DWORD      maxpacket    = s_spi_providerptr->caps.maxmessagesize;
          DWORD      totaldata    = message->databytes
                                      +maxpacket
                                      +2*ESTIMATEDPACKETOVERHEAD;
          DWORD      responsetime = 2*s_spi_timetoresend
                                      +totaldata*1000/bandwidth
                                      +200;

          // IF WE HAVEN'T RECEIVED A RESPONSE IN THE EXPECTED AMOUNT
          // OF TIME, RESEND THE MESSAGE
          if (message->resendtime &&
              ((LONG)(message->resendtime+responsetime-currtime) > 0))
            wait = min(wait,message->resendtime+responsetime-currtime);
          else {
            TRACEOUT(TRACEHANDLE,
                     "  auto-resending: type=%u sequence=%04x player=%x",
                     type,message->data->header.sequence,conn->playerid);
            message->resendtime = currtime;
            ConnResendMessage(conn,message->data,message->databytes);
          }

        }
    }

    // SEND OUT EXPLICIT ACKNOWLEDGEMENT PACKETS TO THIS CONNECTION IF
    // WE HAVEN'T SENT ANY EXPLICIT OR PIGGY-BACKED ACKNOWLEDGEMENTS RECENTLY
    {
      for (int type = 0; type < TYPES; ++type) {
        DWORD acktime = (type == TYPE_TURN) ? s_spi_timetoackturn
                                            : s_spi_timetorequest;
        if ((conn->acksequence[type] != conn->availablesequence[type]) &&
            conn->acktime[type])
          if ((LONG)(conn->acktime[type]+acktime-currtime) > 0)
            wait = min(wait,conn->acktime[type]+acktime-currtime);
          else {
            TRACEOUT(TRACEHANDLE,
                     "  sending explicit ack: type=%u sequence=%04x player=%x",
                     type,conn->availablesequence[type],conn->playerid);
            PACKET pkt;
            pkt.header.checksum    = 0;
            pkt.header.bytes       = sizeof(HEADER);
            pkt.header.sequence    = conn->availablesequence[type];
            pkt.header.acksequence = conn->availablesequence[type];
            pkt.header.type        = type;
            pkt.header.subtype     = 0;
            pkt.header.playerid    = local->playerid;
            pkt.header.flags       = MF_ACK;
            pkt.header.checksum    = PktGenerateChecksum(&pkt);
            SNETADDRPTR addr = &conn->addr;
            ConnSendPacket(conn,&pkt);
          }
      }
    }

  } while ((conn = next) != NULL);
  return wait;
}

//===========================================================================
static void ConnProcessAck (CONNPTR conn, BYTE type, WORD acksequence) {

  // REMOVE ACKNOWLEDGED MESSAGES FROM THE OUTGOING QUEUE
  BOOL found = FALSE;
  while ((!conn->outgoingqueue[type].IsEmpty()) &&
         ((WORD)(acksequence-conn->outgoingqueue[type].Head()->data->header.sequence) > 0) &&
         ((WORD)(acksequence-conn->outgoingqueue[type].Head()->data->header.sequence) <= 0x7FFF)) {
    found = TRUE;
    MESSAGEPTR message = conn->outgoingqueue[type].Head();
    if (message->local)
      PktFreeLocalMessage(message->addr,message->data,message->databytes);
    else {
      TRACEOUT(TRACEHANDLE,
               "  spiFree(0x%08x,0x%08x,%u) [acknowledged]",
               message->addr,
               message->data,
               message->databytes);
      s_spi->Free(message->addr,message->data,message->databytes);
    }
    conn->outgoingqueue[type].DeleteNode(message);
  }

  // SAVE THE SEQUENCE OF THE LAST PROCESSED TURN
  if (type == TYPE_TURN)
    conn->lastprocessedturn = acksequence;

  // RESET THE TIME STAMP OF THE MESSAGE THAT IS NOW AT THE HEAD OF THE
  // OUTGOING QUEUE, SO WE DON'T IMMEDIATELY RESEND IT
  if (found && !conn->outgoingqueue[type].IsEmpty()) {
    DWORD currtime = GetTickCount();
    if (currtime-conn->outgoingqueue[type].Head()->resendtime < 0x7FFFFFFF)
      conn->outgoingqueue[type].Head()->resendtime = currtime;
  }

}

//===========================================================================
static BOOL ConnResendMessage (CONNPTR conn,
                               LPVOID  data,
                               DWORD   databytes) {

  // MAKE A COPY OF THE MESSAGE
  PACKETPTR localpkt = (PACKETPTR)ALLOC(databytes);
  CopyMemory(localpkt,data,databytes);

  // FILL IN THE CURRENT ACKNOWLEDGEMENT SEQUENCE
  localpkt->header.acksequence = conn->availablesequence[localpkt->header.type];

  // FILL IN THE NEW CHECKSUM
  localpkt->header.checksum    = PktGenerateChecksum(localpkt);

  // SEND THE MESSAGE
  ConnSendPacket(conn,localpkt);

  // FREE THE COPY
  FREE(localpkt);

  return TRUE;
}

//===========================================================================
static void ConnSendPacket (CONNPTR   conn,
                            PACKETPTR pkt) {
  SNETADDRPTR addr = &conn->addr;
  SpiSend(1,&addr,pkt,pkt->header.bytes);
  conn->acksequence[pkt->header.type] = pkt->header.acksequence;
  conn->acktime[pkt->header.type]     = 0;
}

//===========================================================================
static MESSAGEPTR ConnSendMessage (CONNPTR target,
                                   BYTE    type,
                                   BYTE    subtype,
                                   LPVOID  data,
                                   DWORD   databytes) {

  // GET A POINTER TO THE LOCAL PLAYER
  CONNPTR local = ConnFindLocal();
  if (!local)
    return NULL;

  // ALLOCATE MEMORY TO HOLD THE ADDRESS AND MESSAGE DATA
  SNETADDRPTR pktaddr      = NULL;
  LPVOID      pktdata      = NULL;
  DWORD       pktdatabytes = sizeof(HEADER)+databytes;
  PktAllocateLocalMessage(&pktaddr,&pktdata,pktdatabytes);

  // FILL IN THE ADDRESS
  CopyMemory(pktaddr,&target->addr,sizeof(SNETADDR));

  // FILL IN THE HEADER
  PACKETPTR pkt = (PACKETPTR)pktdata;
  pkt->header.bytes       = (WORD)pktdatabytes;
  pkt->header.acksequence = target->availablesequence[type];
  pkt->header.type        = type;
  pkt->header.subtype     = subtype;
  pkt->header.playerid    = local->playerid;
  pkt->header.flags       = 0;

  // FILL IN THE SEQUENCE NUMBER FOR THIS MESSAGE
  if ((type == TYPE_SYSTEM) && (subtype == SYS_INITIALCONTACT))
    pkt->header.sequence = 0;
  else
    pkt->header.sequence = target->outgoingsequence[type]++;

  // FILL IN THE DATA
  if (data && databytes)
    CopyMemory((LPBYTE)pktdata+sizeof(HEADER),data,databytes);

  // FILL IN THE CHECKSUM
  pkt->header.checksum = PktGenerateChecksum(pkt);

  // ALLOCATE A MESSAGE RECORD
  MESSAGEPTR msg;
  if (target == local)
    msg = target->incomingqueue[type].NewNode();
  else
    msg = target->outgoingqueue[type].NewNode();

  // FILL IN THE MESSAGE RECORD
  msg->addr      = pktaddr;
  msg->data      = (PACKETPTR)pktdata;
  msg->databytes = pktdatabytes;
  msg->local     = TRUE;
  msg->sendtime  = GetTickCount();

  // SEND THE MESSAGE
  if (target != local) {
    ConnSendPacket(target,pkt);
    PerfAdd(SNET_PERFID_USERBYTESSENT,databytes);
  }

  // INITIALIZE THE MESSAGE'S RESEND TIMER TO THE ESTIMATED TIME THAT
  // THE COMPLETE MESSAGE WILL HIT THE WIRE
  msg->resendtime = s_spi_outgoingtime;

  return msg;
}

//===========================================================================
static void ConnSetCurrentMessage (CONNPTR conn, BYTE type, MESSAGEPTR message) {
  // message must be unlinked upon entry to this function
  if (type == TYPE_TURN)
    conn->oldturns.LinkNode(message,LIST_TAIL,NULL);
  else {
    if (!conn->processing[type].IsEmpty()) {
      MESSAGEPTR processing = conn->processing[type].Head();
      if (processing->local)
        PktFreeLocalMessage(processing->addr,
                            processing->data,
                            processing->databytes);
      else {
        TRACEOUT(TRACEHANDLE,
                 "  spiFree(0x%08x,0x%08x,%u) [processed]",
                 processing->addr,
                 processing->data,
                 processing->databytes);
        s_spi->Free(processing->addr,
                    processing->data,
                    processing->databytes);
      }
      conn->processing[type].DeleteNode(processing);
    }
    conn->processing[type].LinkNode(message,LIST_TAIL,NULL);
  }
}

/****************************************************************************
*
*  GAME/PLAYER MANAGEMENT FUNCTIONS
*
***/

static DWORD  s_game_categorybits                      = 0;
static DWORD  s_game_creationtime                      = 0;
static LPVOID s_game_initdata                          = NULL;
static DWORD  s_game_initdatabytes                     = 0;
static char   s_game_gamedesc[SNETSPI_MAXSTRINGLENGTH] = "";
static DWORD  s_game_gamemode                          = 0;
static char   s_game_gamename[SNETSPI_MAXSTRINGLENGTH] = "";
static char   s_game_gamepass[SNETSPI_MAXSTRINGLENGTH] = "";
static BOOL   s_game_joining                           = FALSE;
static DWORD  s_game_optcategorybits                   = 0;
static BYTE   s_game_playerid                          = NOPLAYER;
static DWORD  s_game_playersallowed                    = 0;

//===========================================================================
static void GameBuildClientData (LPVOID  buffer,
                                 DWORD  *bytes) {
  CLIENTDATAPTR ptr = (CLIENTDATAPTR)buffer;
  ptr->bytes      = sizeof(CLIENTDATA);
  SNetGetNumPlayers(NULL,NULL,&ptr->numplayers);
  ptr->maxplayers = s_game_playersallowed;
  *bytes = ptr->bytes;
}

//===========================================================================
static void GameBuildGameData (SNETGAMEPTR         dest,
                               SNETSPI_GAMELISTPTR source) {
  ZeroMemory(dest,sizeof(SNETGAME));
  dest->size            = sizeof(SNETGAMEPTR);
  dest->id              = source->gameid;
  dest->gamename        = source->gamename;
  dest->gamedescription = source->gamedescription;
  dest->categorybits    = source->gamecategorybits;
  CLIENTDATAPTR clientdata = (CLIENTDATAPTR)(source->clientdata);
  if (clientdata) {
    dest->numplayers = clientdata->numplayers;
    dest->maxplayers = clientdata->maxplayers;
  }
}

//===========================================================================
static void GameCopyGameList (DWORD                categorybits,
                              DWORD                categorymask,
                              SNETSPI_GAMELISTPTR *gamearray,
                              DWORD               *games,
                              DWORD               *hintnextcall) {
  *games = 0;

  // LOCK THE GAME LIST
  SNETSPI_GAMELISTPTR head;
  TRACEOUT(TRACEHANDLE,
           "  spiLockGameList(0x%08x,0x%08x,*gamelist)",
           categorybits,categorymask);
  if (!s_spi->LockGameList(categorybits,categorymask,&head))
    return;

  // DETERMINE THE NUMBER OF MATCHING GAMES
  *games = 0;
  {
    SNETSPI_GAMELISTPTR curr = head;
    while (curr) {
      if (!(curr->gamemode & SNET_GM_UNLISTEDMASK))
        ++*games;
      curr = curr->next;
    }
  }

  // ALLOCATE AN ARRAY TO HOLD THE GAMES
  if (*games) {
    *gamearray = (SNETSPI_GAMELISTPTR)ALLOC((*games)*(sizeof(SNETSPI_GAMELIST)+SNETSPI_MAXCLIENTDATA));

    // COPY THE GAMES
    SNETSPI_GAMELISTPTR source   = head;
    LPBYTE              dest     = (LPBYTE)*gamearray;
    SNETSPI_GAMELISTPTR lastdest = NULL;
    while (source) {
      if (!(source->gamemode & SNET_GM_UNLISTEDMASK)) {

        // LINK THE LAST RECORD TO THIS ONE
        if (lastdest)
          lastdest->next = (SNETSPI_GAMELISTPTR)dest;
        lastdest = (SNETSPI_GAMELISTPTR)dest;

        // COPY THIS RECORD
        CopyMemory(dest,source,sizeof(SNETSPI_GAMELIST));
        dest += sizeof(SNETSPI_GAMELIST);

        // COPY THE CLIENT DATA FOR THIS RECORD
        if (source->clientdata) {
          CopyMemory(dest,source->clientdata,source->clientdatabytes);
          lastdest->clientdata = dest;
          dest += source->clientdatabytes;
        }

      }
      source = source->next;
    }

  }

  // UNLOCK THE GAME LIST
  TRACEOUT(TRACEHANDLE,
           "  spiUnlockGameList(0x%08x,*hintnextcall)",
           head);
  s_spi->UnlockGameList(head,hintnextcall);

}

//===========================================================================
static void GameDestroy () {

  // FREE THE GAME INITIALIZATION DATA
  if (s_game_initdata) {
    FREE(s_game_initdata);
    s_game_initdata      = NULL;
    s_game_initdatabytes = 0;
  }

  // RESET THE GAME INFORMATION
  s_game_gamename[0] = 0;
  s_game_gamedesc[0] = 0;
  s_game_gamepass[0] = 0;
  s_game_gamemode    = 0;

  // RESET THE PLAYER INFORMATION
  s_game_playerid       = NOPLAYER;
  s_game_playersallowed = 0;

  // DESTROY ALL CONNECTION RECORDS
  ConnDestroy();

}

//===========================================================================
static void GameProcessLeavingPlayers () {
  CONNPTR conn  = s_conn_connlist.Head();
  CONNPTR local = ConnFindLocal();
  if (!local)
    return;
  while (conn) {
    if ((conn->flags & PF_LEAVING) &&
        (conn->playerid != NOPLAYER)) {

      // IF WE HAVE PROCESSED THE FINAL TURN FOR THIS PLAYER, REMOVE HIM
      if ((WORD)(conn->incomingsequence[TYPE_TURN]-conn->finalsequence) <= 0x7FFF) {

        // QUEUE A USER-LEVEL EVENT
        SysQueueUserEvent(SNET_EVENT_PLAYERLEAVE,conn->playerid,&conn->exitcode,sizeof(DWORD));

        // MARK THE PLAYER SLOT AS UNUSED
        conn->flags    = 0;
        conn->playerid = NOPLAYER;

        // IF THE GAME OWNER JUST LEFT AND WE ARE NOW THE LOWEST NUMBERED
        // PLAYER, TAKE OWNERSHIP OF THE GAME
        if ((s_game_playerid != NOPLAYER) &&
            !local->gameowner) {

          // FIND THE LOWEST NUMBERED PLAYED BESIDES THE LOCAL PLAYER WHICH
          // IS STILL IN THE GAME
          DWORD   lowestplayerid = 0xFFFFFFFF;
          CONNPTR checkconn      = s_conn_connlist.Head();
          while (checkconn) {
            if (checkconn->playerid != NOPLAYER) {
              if (checkconn->gameowner)
                lowestplayerid = 0;
              else if (checkconn->playerid < lowestplayerid)
                lowestplayerid = checkconn->playerid;
            }
            checkconn = checkconn->Next();
          }

          // IF OUR PLAYER NUMBER IS LOWER, TAKE OWNERSHIP
          if (s_game_playerid < lowestplayerid) {

            // LOG IT
            TRACEOUT(TRACEHANDLE,"  taking ownership");

            // MARK THE LOCAL PLAYER AS THE OWNER
            local->gameowner = TRUE;

            // SEND MESSAGES TO ALL OTHER PLAYERS NOTIFYING THEM OF THE
            // TRANSFER OF GAME OWNERSHIP
            {
              CONNPTR checkconn = s_conn_connlist.Head();
              while (checkconn) {
                if (checkconn->playerid != NOPLAYER) {
                  DWORD playerid = s_game_playerid;
                  ConnSendMessage(checkconn,
                                  TYPE_SYSTEM,
                                  SYS_NEWGAMEOWNER,
                                  &playerid,
                                  sizeof(DWORD));
                }
                checkconn = checkconn->Next();
              }
            }

          }
        }

        // IF THE GAME IS NO LONGER FULL, NOTIFY THE NETWORK PROVIDER BY
        // CHANGING THE GAME MODE
        {
          CONNPTR local = ConnFindLocal();
          if (local && local->gameowner) {
            DWORD activeplayers = 0;
            SNetGetNumPlayers(NULL,NULL,&activeplayers);
            if (activeplayers < s_game_playersallowed)
              s_game_gamemode &= ~SNET_GM_FULL;
          }
        }

        // UPDATE THE SERVER WITH THE NEW GAME MODE, FULL VS NOT FULL,
        // NUMBER OF PLAYERS, ETC.
        SNetSetGameMode(s_game_gamemode);

      }

    }
    conn = conn->Next();
  }
}

/****************************************************************************
*
*  SYSTEM MESSAGE FUNCTIONS
*
***/

static LIST(USEREVENT) s_sys_usereventlist;
static BOOL            s_sys_event[SYSMSGS];

//===========================================================================
static DWORD SysBuildPlayerInfo (SYSEVENTDATA_PLAYERINFOPTR data,
                                 CONNPTR                    conn,
                                 DWORD                      startingturn) {
  data->playerid     = conn->playerid;
  data->gameowner    = conn->gameowner;
  data->flags        = conn->flags;
  data->startingturn = startingturn;
  CopyMemory(&data->addr,&conn->addr,sizeof(SNETADDR));
  LPTSTR curr = data->namedesc;
  curr += SStrCopy(curr,conn->name,SNETSPI_MAXSTRINGLENGTH)+1;
  curr += SStrCopy(curr,conn->desc,SNETSPI_MAXSTRINGLENGTH)+1;
  data->bytes = (LPBYTE)curr-(LPBYTE)data;
  return data->bytes;
}

//===========================================================================
static void SysDestroy () {
  USEREVENTPTR curr;
  while ((curr = s_sys_usereventlist.Head()) != NULL) {
    if (curr->event.data)
      FREE(curr->event.data);
    s_sys_usereventlist.DeleteNode(curr);
  }
}

//===========================================================================
static void SysDispatchUserEvents () {
  USEREVENTPTR curr;
  while ((curr = s_sys_usereventlist.Head()) != NULL) {
    USEREVENT event;
    CopyMemory(&event,curr,sizeof(USEREVENT));
    s_sys_usereventlist.DeleteNode(curr);
    event.event.playerid += s_api_playeroffset;
    TRACEOUT(TRACEHANDLE,
             "  dispatch event=%u player=%u data=0x%08x databytes=%u",
             event.event.eventid,event.event.playerid,event.event.data,event.event.databytes);
    SEvtDispatch(REGISTERTYPE,
                 REGISTERSUBTYPE_SNETEVENT,
                 event.event.eventid,
                 &event.event);
    if (event.event.data)
      FREE(event.event.data);
  }
}

//===========================================================================
static void CALLBACK SysOnCircuitCheck (SYSEVENTPTR event) {
  if ((event->databytes == sizeof(DWORD)) &&
      (*(LPDWORD)event->data == SNET_NETWORKVERSION))
    ConnSendMessage(ConnFindByAddr(event->senderaddr),
                    TYPE_SYSTEM,
                    SYS_CIRCUITCHECKRESPONSE,
                    event->data,
                    event->databytes);
}

//===========================================================================
static void CALLBACK SysOnDropPlayer (SYSEVENTPTR event) {
  SYSEVENTDATA_DROPPLAYERPTR eventdataptr = (SYSEVENTDATA_DROPPLAYERPTR)event->data;

  // MARK THE PLAYER AS LEAVING.  WE DON'T ACTUALLY REMOVE HIM FROM THE
  // GAME, OR SEND A USER-LEVEL NOTIFICATION, UNTIL THE APPLICATION HAS
  // PROCESSED ALL REMAINING MESSAGES FROM THIS PLAYER.
  CONNPTR conn = ConnFindByPlayerId(eventdataptr->playerid);
  if (conn && (conn->playerid != NOPLAYER)) {
    conn->flags         |= PF_LEAVING;
    conn->finalsequence  = (WORD)eventdataptr->finalsequence;
    conn->exitcode       = eventdataptr->exitcode;
  }

}

//===========================================================================
static void CALLBACK SysOnNewGameOwner (SYSEVENTPTR event) {

  // REMOVE OWNERSHIP FROM THE EXISTING GAME OWNER
  for (BOOL local = TRUE; local >= FALSE; --local) {
    LISTPTR(CONNREC) list = local ? &s_conn_local
                                  : &s_conn_connlist;
    ITERATELISTPTR(CONNREC,list,curr)
      curr->gameowner = FALSE;
  }

  // MAKE THE SENDER THE NEW GAME OWNER
  DWORD playerid = *(LPDWORD)event->data;
  CONNPTR conn = ConnFindByPlayerId(playerid);
  if (conn)
    conn->gameowner = TRUE;

}

//===========================================================================
static void CALLBACK SysOnPing (SYSEVENTPTR event) {
  ConnSendMessage(ConnFindByAddr(event->senderaddr),
                  TYPE_SYSTEM,
                  SYS_PINGRESPONSE,
                  event->data,
                  event->databytes);
}

//===========================================================================
static void CALLBACK SysOnPingResponse (SYSEVENTPTR event) {
  CONNPTR conn     = ConnFindByAddr(event->senderaddr);
  DWORD   currtime = GetTickCount();
  if (conn) {
    conn->latency     = currtime-conn->lastpingtime;
    conn->peaklatency = max(conn->peaklatency,conn->latency);
  }
}

//===========================================================================
static void CALLBACK SysOnPlayerInfo (SYSEVENTPTR event) {
  SYSEVENTDATA_PLAYERINFOPTR eventdataptr = (SYSEVENTDATA_PLAYERINFOPTR)event->data;
  SNETADDRPTR addr = &eventdataptr->addr;

  // IF THIS IS THE PLAYER INFO FOR THE SYSTEM SENDING THE PACKET, USE THE
  // PACKET'S ORIGIN ADDRESS RATHER THAN THE ONE IN THE PLAYER INFORMATION,
  // BECAUSE THE SENDER PRESUMABLY DOESN'T KNOW HIS OWN ADDRESS.
  if (eventdataptr->playerid == event->senderplayerid)
    addr = event->senderaddr;

  // OTHERWISE, IF THIS IS THE PLAYER INFO FOR A THIRD PARTY SYSTEM, THEN
  // RESET THE CONNECTION RECORD FOR THAT SYSTEM.
  else
    ConnFree(ConnFindByAddr(addr));

  // UPDATE THE PLAYER INFORMATION
  CONNPTR conn  = ConnFindByAddr(addr);
  CONNPTR local = ConnFindLocal();
  if (conn && local) {
    conn->flags                        = eventdataptr->flags;
    conn->playerid                     = (BYTE)eventdataptr->playerid;
    conn->gameowner                    = eventdataptr->gameowner;
    conn->incomingsequence[TYPE_TURN]  = (WORD)eventdataptr->startingturn;
    conn->availablesequence[TYPE_TURN] = (WORD)eventdataptr->startingturn;
    conn->outgoingsequence[TYPE_TURN]  = local->outgoingsequence[TYPE_TURN];
    if ((conn->incomingsequence[TYPE_TURN] != local->incomingsequence[TYPE_TURN]) &&
        ((WORD)(conn->incomingsequence[TYPE_TURN]-local->incomingsequence[TYPE_TURN]) < 0x7FFF))
      conn->flags |= PF_JOINING;
    CopyMemory(&conn->addr,addr,sizeof(SNETADDR));
    SStrCopy(conn->name,
             eventdataptr->namedesc,
             SNETSPI_MAXSTRINGLENGTH);
    SStrCopy(conn->desc,
             eventdataptr->namedesc+SStrLen(eventdataptr->namedesc)+1,
             SNETSPI_MAXSTRINGLENGTH);
  }

  // IF THIS IS A NEW PLAYER, SEND HIM ALL OF OUR TURNS FROM HIS STARTING
  // TURN NUMBER
  {
    MESSAGEPTR currmsg;
    currmsg = local->oldturns.Head();
    while (currmsg) {
      if ((WORD)(currmsg->data->header.sequence-eventdataptr->startingturn) <= 0x7FFF)
        ConnResendMessage(conn,currmsg->data,currmsg->databytes);
      currmsg = currmsg->Next();
    }
    currmsg = local->incomingqueue[TYPE_TURN].Head();
    while (currmsg) {
      if ((WORD)(currmsg->data->header.sequence-eventdataptr->startingturn) <= 0x7FFF)
        ConnResendMessage(conn,currmsg->data,currmsg->databytes);
      currmsg = currmsg->Next();
    }
  }

  // QUEUE A USER-LEVEL EVENT
  if ((s_game_playerid != NOPLAYER) &&
      conn &&
      (!(conn->flags & PF_JOINING)))
    SysQueueUserEvent(SNET_EVENT_PLAYERJOIN,
                      eventdataptr->playerid,
                      NULL,
                      0);

}

//===========================================================================
static void CALLBACK SysOnPlayerJoin (SYSEVENTPTR event) {
  SYSEVENTDATA_PLAYERJOINPTR eventdataptr = (SYSEVENTDATA_PLAYERJOINPTR)event->data;

  // IF THE GAME IS FULL OR NOT JOINABLE, SEND BACK A REJECTION NOTICE
  {
    DWORD activeplayers = 0;
    SNetGetNumPlayers(NULL,NULL,&activeplayers);
    if ((activeplayers >= s_game_playersallowed) ||
        (s_game_gamemode & SNET_GM_UNJOINABLE)) {
      ConnSendMessage(ConnFindByAddr(event->senderaddr),
                      TYPE_SYSTEM,
                      SYS_PLAYERJOIN_REJECT,
                      NULL,
                      0);
      return;
    }
  }

  // BREAK OUT THE NAME, DESCRIPTION, AND PASSWORD
  LPCSTR playername = eventdataptr->namedescpass;
  LPCSTR playerdesc = playername+SStrLen(playername)+1;
  LPCSTR gamepass   = playerdesc+SStrLen(playerdesc)+1;

  // IF THE PASSWORD IS WRONG, SEND BACK A REJECTION NOTICE
  if (s_game_gamepass[0] && _stricmp(s_game_gamepass,gamepass)) {
    ConnSendMessage(ConnFindByAddr(event->senderaddr),
                    TYPE_SYSTEM,
                    SYS_PLAYERJOIN_REJECT,
                    NULL,
                    0);
    return;
  }

  // DETERMINE THE PLAYER'S ID
  BYTE playerid = 0;
  do {
    if (!ConnFindByPlayerId(playerid))
      break;
  } while (++playerid != NOPLAYER);
  if ((playerid == NOPLAYER) || (playerid >= s_game_playersallowed)) {
    ConnSendMessage(ConnFindByAddr(event->senderaddr),
                    TYPE_SYSTEM,
                    SYS_PLAYERJOIN_REJECT,
                    NULL,
                    0);
    return;
  }

  // ADD THE PLAYER
  CONNPTR local = ConnFindLocal();
  CONNPTR conn  = ConnFindByAddr(event->senderaddr);
  if (!(conn && local))
    return;
  {
    conn->flags        = 0;
    conn->playerid     = playerid;
    conn->establishing = FALSE;
    SStrCopy(conn->name,playername,SNETSPI_MAXSTRINGLENGTH);
    SStrCopy(conn->desc,playerdesc,SNETSPI_MAXSTRINGLENGTH);
    conn->incomingsequence[TYPE_TURN]  = local->outgoingsequence[TYPE_TURN];
    conn->availablesequence[TYPE_TURN] = local->outgoingsequence[TYPE_TURN];
    conn->outgoingsequence[TYPE_TURN]  = local->outgoingsequence[TYPE_TURN];
    if ((conn->incomingsequence[TYPE_TURN] != local->incomingsequence[TYPE_TURN]) &&
        ((WORD)(conn->incomingsequence[TYPE_TURN]-local->incomingsequence[TYPE_TURN]) < 0x7FFF))
      conn->flags |= PF_JOINING;
  }

  // NOTIFY THE NETWORK PROVIDER OF THE NEW NUMBER OF PLAYERS IN THE GAME
  {
    DWORD activeplayers = 0;
    SNetGetNumPlayers(NULL,NULL,&activeplayers);
    DWORD gamemode = s_game_gamemode;
    if (activeplayers >= s_game_playersallowed)
      gamemode |= SNET_GM_FULL;
    SNetSetGameMode(gamemode);
  }

  // SEND BACK AN ACCEPT START NOTICE
  DWORD startingturn = local->outgoingsequence[TYPE_TURN];
  {
    SYSEVENTDATA_PLAYERJOIN_ACCEPTSTART data;
    data.playerid       = playerid;
    data.playersallowed = s_game_playersallowed;
    data.nextturn       = startingturn;
    data.gamemode       = s_game_gamemode;
    data.runningtime    = (GetTickCount()-s_game_creationtime)/1000;
    LPSTR currptr = data.namedescpass;
    currptr += SStrCopy(currptr,s_game_gamename,SNETSPI_MAXSTRINGLENGTH)+1;
    currptr += SStrCopy(currptr,s_game_gamedesc,SNETSPI_MAXSTRINGLENGTH)+1;
    currptr += SStrCopy(currptr,s_game_gamepass,SNETSPI_MAXSTRINGLENGTH)+1;
    ConnSendMessage(conn,
                    TYPE_SYSTEM,
                    SYS_PLAYERJOIN_ACCEPTSTART,
                    &data,
                    (LPBYTE)currptr-(LPBYTE)&data);
  }

  // SEND BACK INFORMATION ABOUT THE LOCAL PLAYER
  {
    SYSEVENTDATA_PLAYERINFO data;
    DWORD                   databytes = SysBuildPlayerInfo(&data,local,startingturn);
    ConnSendMessage(conn,
                    TYPE_SYSTEM,
                    SYS_PLAYERINFO,
                    &data,
                    databytes);
  }

  // SEND BACK INFORMATION FOR EACH OTHER PLAYER IN THE GAME
  {
    CONNPTR sendconn = s_conn_connlist.Head();
    while (sendconn) {
      if ((sendconn != conn) && (sendconn->playerid != NOPLAYER)) {
        SYSEVENTDATA_PLAYERINFO data;
        DWORD                   databytes = SysBuildPlayerInfo(&data,sendconn,startingturn);
        ConnSendMessage(conn,
                        TYPE_SYSTEM,
                        SYS_PLAYERINFO,
                        &data,
                        databytes);
      }
      sendconn = sendconn->Next();
    }
  }

  // SEND BACK AN ACCEPT DONE NOTICE
  ConnSendMessage(conn,
                  TYPE_SYSTEM,
                  SYS_PLAYERJOIN_ACCEPTDONE,
                  s_game_initdata,
                  s_game_initdatabytes);

  // SEND AN UPDATE MESSAGE TO ALL OTHER PLAYERS IN THE GAME
  {
    CONNPTR destconn = s_conn_connlist.Head();
    while (destconn) {
      if ((destconn != conn) && (destconn->playerid != NOPLAYER)) {
        SYSEVENTDATA_PLAYERINFO data;
        DWORD                   databytes = SysBuildPlayerInfo(&data,conn,startingturn);
        ConnSendMessage(destconn,
                        TYPE_SYSTEM,
                        SYS_PLAYERINFO,
                        &data,
                        databytes);
      }
      destconn = destconn->Next();
    }
  }

  // SEND THE NEW PLAYER A COPY OF ALL OUR UNPROCESSED TURNS
  {
    MESSAGEPTR currmsg = local->incomingqueue[TYPE_TURN].Head();
    while (currmsg) {
      ConnResendMessage(conn,currmsg->data,currmsg->databytes);
      currmsg = currmsg->Next();
    }
  }

  // QUEUE A USER-LEVEL EVENT
  if (!(conn->flags & PF_JOINING))
    SysQueueUserEvent(SNET_EVENT_PLAYERJOIN,playerid,NULL,0);

}

//===========================================================================
static void CALLBACK SysOnPlayerJoinAcceptStart (SYSEVENTPTR event) {
  SYSEVENTDATA_PLAYERJOIN_ACCEPTSTARTPTR eventdataptr
    = (SYSEVENTDATA_PLAYERJOIN_ACCEPTSTARTPTR)event->data;
  CONNPTR conn = ConnFindLocal();
  if (conn) {
    conn->playerid                     = (BYTE)eventdataptr->playerid;
    conn->incomingsequence[TYPE_TURN]  = (WORD)eventdataptr->nextturn;
    conn->availablesequence[TYPE_TURN] = (WORD)eventdataptr->nextturn;
    conn->outgoingsequence[TYPE_TURN]  = (WORD)eventdataptr->nextturn;
    PerfSet(SNET_PERFID_TURN,eventdataptr->nextturn);
  }
  s_game_playersallowed = eventdataptr->playersallowed;
  s_game_gamemode       = eventdataptr->gamemode;
  s_game_creationtime   = GetTickCount()-eventdataptr->runningtime*1000;
  LPCSTR currptr = eventdataptr->namedescpass;
  currptr += SStrCopy(s_game_gamename,currptr,SNETSPI_MAXSTRINGLENGTH)+1;
  currptr += SStrCopy(s_game_gamedesc,currptr,SNETSPI_MAXSTRINGLENGTH)+1;
  currptr += SStrCopy(s_game_gamepass,currptr,SNETSPI_MAXSTRINGLENGTH)+1;
}

//===========================================================================
static void CALLBACK SysOnPlayerJoinAcceptDone (SYSEVENTPTR event) {
  CONNPTR conn = ConnFindLocal();
  if (conn)
    s_game_playerid = conn->playerid;
  if (s_game_initdata) {
    FREE(s_game_initdata);
    s_game_initdata = NULL;
  }
  s_game_initdatabytes = 0;
  if (event->data && event->databytes) {
    s_game_initdata      = ALLOC(event->databytes);
    s_game_initdatabytes = event->databytes;
    CopyMemory(s_game_initdata,event->data,s_game_initdatabytes);
    SysQueueUserEvent(SNET_EVENT_INITDATA,
                      s_game_playerid,
                      s_game_initdata,
                      s_game_initdatabytes);
  }
}

//===========================================================================
static void CALLBACK SysOnPlayerJoinReject (SYSEVENTPTR event) {
}

//===========================================================================
static void CALLBACK SysOnPlayerLeave (SYSEVENTPTR event) {
  SYSEVENTDATA_PLAYERLEAVEPTR eventdataptr =
    (SYSEVENTDATA_PLAYERLEAVEPTR)event->data;

  // MARK THE PLAYER AS LEAVING.  WE DON'T ACTUALLY REMOVE HIM FROM THE
  // GAME, OR SEND A USER-LEVEL NOTIFICATION, UNTIL THE APPLICATION HAS
  // PROCESSED ALL REMAINING TURNS FROM THIS PLAYER.
  CONNPTR conn = ConnFindByAddr(event->senderaddr);
  if (conn && (conn->playerid != NOPLAYER)) {
    conn->flags         |= PF_LEAVING;
    conn->finalsequence  = (WORD)eventdataptr->finalsequence;
    conn->exitcode       = eventdataptr->exitcode;
  }

}

//===========================================================================
static void SysProcessIncomingMessages (CONNPTR conn) {
  while ((!conn->incomingqueue[TYPE_SYSTEM].IsEmpty()) &&
         (conn->incomingqueue[TYPE_SYSTEM].Head()->data->header.sequence
           == conn->incomingsequence[TYPE_SYSTEM])) {
    MESSAGEPTR message = conn->incomingqueue[TYPE_SYSTEM].Head();
    PACKETPTR  pkt     = (PACKETPTR)message->data;
    TRACEOUT(TRACEHANDLE,
             "  processing system message: sequence=%04x event=%u player=%x",
             pkt->header.sequence,pkt->header.subtype,pkt->header.playerid);

    // SIGNAL THE EVENT
    if (pkt->header.subtype < SYSMSGS)
      s_sys_event[pkt->header.subtype] = TRUE;

    // CALL INTERNAL THE SYSTEM EVENT HANDLER FOR THIS EVENT TYPE
    {
      SYSEVENT eventdata;
      eventdata.senderplayerid = pkt->header.playerid;
      eventdata.senderaddr     = message->addr;
      eventdata.eventid        = pkt->header.subtype;
      eventdata.data           = pkt->data;
      eventdata.databytes      = pkt->header.bytes-sizeof(HEADER);
      SEvtDispatch(REGISTERTYPE,
                   REGISTERSUBTYPE_SYSEVENT,
                   eventdata.eventid,
                   &eventdata);
    }

    // ADJUST THE INCOMING SEQUENCE NUMBER
    ++(conn->incomingsequence[TYPE_SYSTEM]);

    // FREE THE MESSAGE DATA
    TRACEOUT(TRACEHANDLE,
             "  spiFree(0x%08x,0x%08x,%u) [system message]",
             message->addr,message->data,message->databytes);
    s_spi->Free(message->addr,message->data,message->databytes);

    // FREE THE MESSAGE RECORD
    conn->incomingqueue[TYPE_SYSTEM].DeleteNode(message);

  }
}

//===========================================================================
static void SysQueueUserEvent (DWORD  eventid,
                               DWORD  playerid,
                               LPVOID data,
                               DWORD  databytes) {
  USEREVENTPTR userevent = s_sys_usereventlist.NewNode();
  userevent->event.eventid  = eventid;
  userevent->event.playerid = playerid;
  if (data && databytes) {
    userevent->event.data = ALLOC(databytes);
    CopyMemory(userevent->event.data,data,databytes);
    userevent->event.databytes = databytes;
  }
}

//===========================================================================
static BOOL SysWaitForMultipleEvents (DWORD   numevents,
                                      LPDWORD eventlist,
                                      BOOL    waitforall,
                                      DWORD   timeout) {

  // BLANK OUT THE RECEIVED EVENT ARRAY
  ZeroMemory(s_sys_event,SYSMSGS*sizeof(BOOL));

  // LOOP UNTIL THE CONDITIONS ARE SATISFIED OR THE TIMEOUT HAS ELAPSED
  DWORD starttime = GetTickCount();
  BOOL  firstiter = TRUE;
  do {

    // IF THIS IS NOT THE FIRST ITERATION, SLEEP FOR A CLOCK TICK
    if (!firstiter)
      Sleep(10);
    firstiter = FALSE;

    // PROCESS INCOMING PACKETS
    RecvProcessExternalMessages();
    RecvProcessIncomingPackets();

    // MAINTAIN CONNECTIONS
    ConnMaintainConnections();

    // IF THE CONDITIONS HAVE BEEN SATISFIED, RETURN SUCCESS
    {
      DWORD signalled = 0;
      for (DWORD loop = 0; loop < numevents; ++loop)
        if ((*(eventlist+loop) < SYSMSGS) &&
            s_sys_event[*(eventlist+loop)])
          ++signalled;
      if ((signalled >= numevents) ||
          (signalled && !waitforall))
        return TRUE;
    }

  } while ((timeout == INFINITE) ||
           (GetTickCount()-starttime < timeout));

  return FALSE;
}

/****************************************************************************
*
*  RECEIVING THREAD
*
***/

static HANDLE s_recv_event    = (HANDLE)0;
static BOOL   s_recv_shutdown = FALSE;
static HANDLE s_recv_thread   = (HANDLE)0;

static DWORD CALLBACK RecvThreadProc (LPVOID param);

//===========================================================================
static void RecvDestroy () {
  if (s_recv_event && s_recv_thread) {
    s_recv_shutdown = TRUE;
    SetEvent(s_recv_event);
    WaitForSingleObject(s_recv_thread,INFINITE);
    s_recv_shutdown = FALSE;
    CloseHandle(s_recv_event);
    CloseHandle(s_recv_thread);
    s_recv_event  = (HANDLE)0;
    s_recv_thread = (HANDLE)0;
  }
}

//===========================================================================
static BOOL RecvInitialize (HANDLE *eventptr) {

  // CREATE THE EVENT
  if (!s_recv_event)
    s_recv_event = CreateEvent((LPSECURITY_ATTRIBUTES)NULL,
                                  0,
                                  0,
                                  NULL);

  // CREATE THE THREAD
  if (!s_recv_thread) {
    s_recv_shutdown = FALSE;
    DWORD threadid;
    s_recv_thread = (HANDLE)_beginthreadex((LPSECURITY_ATTRIBUTES)NULL,
                                              0,
                                              RecvThreadProc,
                                              NULL,
                                              0,
                                              &threadid);
    SetThreadPriority(s_recv_thread,THREAD_PRIORITY_HIGHEST);
  }

  // RETURN A HANDLE TO THE EVENT
  if (eventptr)
    *eventptr = s_recv_event;

  return (s_recv_event && s_recv_thread);
}

//===========================================================================
static void RecvProcessExternalMessages () {
  for (;;) {
    LPCSTR senderpath = NULL;
    LPCSTR sendername = NULL;
    LPCSTR message    = NULL;
    TRACEOUT(TRACEHANDLE,
             "  spiReceiveExternalMessage(*senderpath,*sendername,*message)");
    if (!s_spi->ReceiveExternalMessage(&senderpath,&sendername,&message))
      break;
    if (!(senderpath && sendername && message))
      break;

    if ((!*senderpath) && (!*sendername))
      SysQueueUserEvent(SNET_EVENT_SERVERMESSAGE,
                        SNET_BROADCASTPLAYERID,
                        (LPVOID)message,
                        SStrLen(message)+1);

    TRACEOUT(TRACEHANDLE,
             "  spiFreeExternalMessage(0x%08x,0x%08x,0x%08x)",
             senderpath,sendername,message);
    s_spi->FreeExternalMessage(senderpath,sendername,message);
  }
}

//===========================================================================
static void RecvProcessIncomingPackets () {
  for (;;) {
    SNETADDRPTR addr      = NULL;
    LPVOID      data      = NULL;
    DWORD       databytes = 0;
    TRACEOUT(TRACEHANDLE,"  spiReceive(*addr,*data,*databytes)");
/* note: restore this
    if (!s_spi->Receive(&addr,&data,&databytes))
      break;
*/
if (!s_spi->Receive(&data,&databytes,&addr))
break;
    if (!(addr && data))
      break;
    PACKETPTR pkt = (PACKETPTR)data;

    // IF WE ARE IN DEBUG MODE, PRINT TRACE INFO FOR THE MESSAGE
#ifdef TRACING
    TRACEOUT(TRACEHANDLE,
             "  addr=0x%08x data=0x%08x databytes=%u",
             addr,data,databytes);
    TRACEDUMPADDR(TRACEHANDLE,
                  "sender",
                  addr,
                  (databytes >= sizeof(HEADER)) ? pkt->header.playerid : NOPLAYER);
    TRACEDUMP(TRACEHANDLE,&pkt->header,min(databytes,sizeof(HEADER)));
    if (databytes > sizeof(HEADER))
      TRACEDUMP(TRACEHANDLE,&pkt->data[0],databytes-sizeof(HEADER));
#endif
    PerfAdd(SNET_PERFID_TOTALBYTESRECV,databytes);

    // FIND THE CONNECTION RECORD FOR THE SENDER OF THIS MESSAGE
    CONNPTR local = ConnFindLocal();
    CONNPTR conn  = (pkt->header.flags & MF_FORWARDED)
                      ? ConnFindByPlayerId(pkt->header.playerid)
                      : ConnFindByAddr(addr);
    if (!(conn && local)) {
      TRACEOUT(TRACEHANDLE,
               "  spiFree(0x%08x,0x%08x,%u) [no connection]",
               addr,data,databytes);
      s_spi->Free(addr,data,databytes);
      continue;
    }

    // CONFIRM THAT THIS IS A VALID MESSAGE.  WE PERFORM THE FOLLOWING
    // CHECKS:
    //   - CHECK FOR MINIMUM PACKET SIZE
    //   - COMPARE LENGTH IN HEADER TO SIZE OF PACKET
    //   - CHECK FOR VALID MESSAGE TYPE
    //   - CHECK PLAYER NUMBER
    //   - COMPUTE CHECKSUM
    //   - COMPARE SENDER ADDRESS TO ADDRESS OF THE PLAYER NUMBER IN HEADER
    if ((databytes < sizeof(HEADER)) ||
        (databytes < pkt->header.bytes) ||
        (pkt->header.type >= TYPES) ||
        PktComputeChecksum(data,pkt->header.bytes)) {
      TRACEOUT(TRACEHANDLE,
               "  spiFree(0x%08x,0x%08x,%u) [invalid message: corrupt]",
               addr,data,databytes);
      s_spi->Free(addr,data,databytes);
      continue;
    }
    if ((!s_game_joining) &&
        (pkt->header.playerid != NOPLAYER) &&
        (!(pkt->header.flags & MF_FORWARDED)) &&
        (ConnFindByPlayerId(pkt->header.playerid) != conn)) {
      TRACEOUT(TRACEHANDLE,
               "  spiFree(0x%08x,0x%08x,%u) [invalid message: bad sender]",
               addr,data,databytes);
      s_spi->Free(addr,data,databytes);
      continue;
    }

    // IF THIS IS AN INITIAL CONTACT MESSAGE, RESET THE SENDER'S CONNECTION
    // RECORD AND INITIATE A CIRCUIT CHECK
    if ((pkt->header.type    == TYPE_SYSTEM) &&
        (pkt->header.subtype == SYS_INITIALCONTACT)) {
      ConnFree(ConnFindByAddr(addr));
      if ((pkt->header.bytes == sizeof(HEADER)+sizeof(DWORD)) &&
          (*(LPDWORD)&pkt->data[0] == SNET_NETWORKVERSION)) {
        CONNPTR conn = ConnFindByAddr(addr);
        if (conn) {
          conn->establishing = TRUE;
          conn->incomingsequence[TYPE_SYSTEM]  = 1;
          conn->availablesequence[TYPE_SYSTEM] = 1;
          conn->outgoingsequence[TYPE_SYSTEM]  = 1;
          DWORD networkversion = SNET_NETWORKVERSION;
          ConnSendMessage(conn,
                          TYPE_SYSTEM,
                          SYS_CIRCUITCHECK,
                          &networkversion,
                          sizeof(DWORD));
        }
      }
      TRACEOUT(TRACEHANDLE,
               "  spiFree(0x%08x,0x%08x,%u) [initial contact]",
               addr,data,databytes);
      s_spi->Free(addr,data,databytes);
      continue;
    }

    // IF THIS MESSAGE WAS NOT FORWARDED THEN RESET THE "LAST HEARD FROM"
    // TIME STAMP OF THE PLAYER WHO SENT IT
    if (!(pkt->header.flags & MF_FORWARDED))
      conn->lastreceivetime = GetTickCount();

    // IF THIS MESSAGE CONTAINS A PIGGY-BACKED ACKNOWLEDGEMENT THEN
    // FREE MESSAGES THAT WE SENT TO THE OTHER COMPUTER UP TO THE
    // ACKNOWLEDGEMENT SEQUENCE
    if (!(pkt->header.flags & MF_FORWARDED))
      ConnProcessAck(conn,pkt->header.type,pkt->header.acksequence);

    // IF THIS MESSAGE CONTAINS ONLY AN ACKNOWLEDGEMENT, AND NO ACTUAL
    // DATA, THEN FREE IT
    if (pkt->header.flags & MF_ACK) {
      TRACEOUT(TRACEHANDLE,
               "  spiFree(0x%08x,0x%08x,%u) [ack only]",
               addr,data,databytes);
      s_spi->Free(addr,data,databytes);
      continue;
    }

    // IF THIS MESSAGE IS A RESEND REQUEST THEN FIND AND RESEND THE
    // REQUESTED MESSAGE
    if (pkt->header.flags & MF_RESENDREQUEST) {
      MESSAGEPTR currmsg = NULL;

      // PROCESS REQUESTS TO RESEND ONE OF OUR OWN MESSAGES OR TURNS
      if ((pkt->header.bytes == sizeof(HEADER)) ||
          (pkt->data[0] == (s_game_playerid & 0xFF)))
        if (pkt->header.type == TYPE_TURN) {
          currmsg = local->incomingqueue[TYPE_TURN].Head();
          while (currmsg && (currmsg->data->header.sequence != pkt->header.sequence))
            currmsg = currmsg->Next();
          if (!currmsg) {
            currmsg = local->oldturns.Head();
            while (currmsg && (currmsg->data->header.sequence != pkt->header.sequence))
              currmsg = currmsg->Next();
          }
        }
        else {
          currmsg = conn->outgoingqueue[pkt->header.type].Head();
          while (currmsg && (currmsg->data->header.sequence != pkt->header.sequence))
            currmsg = currmsg->Next();
        }

      // PROCESS REQUESTS TO FORWARD A TURN FROM ANOTHER PLAYER
      else if (pkt->header.type == TYPE_TURN) {
        CONNPTR findconn = ConnFindByPlayerId(pkt->data[0]);
        if (findconn) {
          currmsg = findconn->incomingqueue[TYPE_TURN].Head();
          while (currmsg && (currmsg->data->header.sequence != pkt->header.sequence))
            currmsg = currmsg->Next();
          if (!currmsg) {
            currmsg = findconn->oldturns.Head();
            while (currmsg && (currmsg->data->header.sequence != pkt->header.sequence))
              currmsg = currmsg->Next();
          }
        }
        if (currmsg) {
          currmsg->data->header.checksum  = 0;
          currmsg->data->header.flags    |= MF_FORWARDED;
          currmsg->data->header.checksum  = PktGenerateChecksum(currmsg->data);
        }
      }

      // IF WE FOUND THE REQUESTED DATA, RESEND IT
      if (currmsg)
        ConnResendMessage(conn,currmsg->data,currmsg->databytes);

      TRACEOUT(TRACEHANDLE,
               "  spiFree(0x%08x,0x%08x,%u) [resend request]",
               addr,data,databytes);
      s_spi->Free(addr,data,databytes);
      continue;
    }

    // IF THIS MESSAGE HAS ALREADY BEEN PROCESSED, IGNORE IT AND MARK THIS
    // CONNECTION AS REQUIRING ANOTHER EXPLICIT ACK
    if (((WORD)(conn->incomingsequence[pkt->header.type]-pkt->header.sequence) > 0) &&
        ((WORD)(conn->incomingsequence[pkt->header.type]-pkt->header.sequence) < 0x7FFF)) {
      if (pkt->header.type != TYPE_TURN) {
        conn->acksequence[pkt->header.type] = conn->availablesequence[pkt->header.type]-1;
        conn->acktime[pkt->header.type]     = GetTickCount();
      }
      TRACEOUT(TRACEHANDLE,
               "  spiFree(0x%08x,0x%08x,%u) [already processed]",
               addr,data,databytes);
      s_spi->Free(addr,data,databytes);
      continue;
    }

    // CREATE A MESSAGE RECORD FOR THIS PACKET
    MESSAGEPTR message = conn->incomingqueue[pkt->header.type].NewNode(LIST_UNLINKED);
    message->addr       = addr;
    message->data       = (PACKETPTR)data;
    message->databytes  = databytes;
    message->sendtime   = GetTickCount();
    message->resendtime = message->sendtime+s_spi_timetorequest-s_spi_timetoresend;
    PerfAdd(SNET_PERFID_USERBYTESRECV,databytes-sizeof(HEADER));

    // ADD THE MESSAGE TO THE APPROPRIATE INCOMING QUEUE IN SEQUENCE ORDER
    {
      MESSAGEPTR curr = conn->incomingqueue[pkt->header.type].Head();
      while (curr &&
             ((WORD)(pkt->header.sequence-curr->data->header.sequence) > 0) &&
             ((WORD)(pkt->header.sequence-curr->data->header.sequence) < 0x7FFF))
        curr = curr->Next();

      // IF WE FOUND ANOTHER PACKET IN THE QUEUE WITH THE SAME SEQUENCE
      // NUMBER, FREE THIS PACKET
      if (curr && (pkt->header.sequence == curr->data->header.sequence)) {
        TRACEOUT(TRACEHANDLE,
                 "  spiFree(0x%08x,0x%08x,%u) [already received]",
                 addr,data,databytes);
        conn->incomingqueue[pkt->header.type].DeleteNode(message);
        s_spi->Free(addr,data,databytes);
        continue;
      }

      // OTHERWISE, LINK THIS PACKET INTO THE QUEUE
      else
        conn->incomingqueue[pkt->header.type].LinkNode(message,LIST_LINK_BEFORE,curr);
    }

    // UPDATE THE AVAILABLE SEQUENCE NUMBER FOR THIS MESSAGE TYPE.
    // IF THE AVAILABLE SEQUENCE NUMBER HAS CHANGED AND WE ARE NOT CURRENTLY
    // SET TO SEND AN EXPLICIT ACKNOWLEDGEMENT, SET THE EXPLICIT ACK TIMER.
    if (pkt->header.type != TYPE_TURN) {
      WORD oldavailablesequence = conn->availablesequence[pkt->header.type];
      conn->availablesequence[pkt->header.type] = conn->incomingsequence[pkt->header.type];
      MESSAGEPTR curr = conn->incomingqueue[pkt->header.type].Head();
      while (curr &&
             (curr->data->header.sequence == conn->availablesequence[pkt->header.type])) {
        conn->availablesequence[pkt->header.type]++;
        curr = curr->Next();
      }
      if ((conn->availablesequence[pkt->header.type] != oldavailablesequence) &&
          (!conn->acktime[pkt->header.type]))
        conn->acktime[pkt->header.type] = GetTickCount();
    }
    else {

      // FOR TURNS, WE DON'T ACKNOWLEDGE A TURN FROM ONE CONNECTION UNTIL
      // WE HAVE RECEIVED IT FROM ALL CONNECTIONS.  THAT WAY, OTHER COMPUTERS
      // KEEP OLD TURNS AROUND SO THAT THEY CAN RESEND THEM ON BEHALF OF
      // UNRESPONSIVE SYSTEMS.
      WORD curravailablesequence = conn->availablesequence[pkt->header.type];
      {
        BOOL receivedany;
        BOOL receivedall;
        do {
          receivedany = FALSE;
          receivedall = TRUE;
          CONNPTR currconn = s_conn_connlist.Head();
          while (currconn) {
            if ((currconn->playerid != NOPLAYER) &&
                ((!(currconn->flags & PF_JOINING)) ||
                 ((LONG)(curravailablesequence-currconn->incomingsequence[TYPE_TURN]) >= 0))) {
              MESSAGEPTR currmsg = currconn->incomingqueue[TYPE_TURN].Head();
              while (currmsg &&
                     (currmsg->data->header.sequence != curravailablesequence))
                currmsg = currmsg->Next();
              if (currmsg)
                receivedany = TRUE;
              else
                receivedall = FALSE;
            }
            currconn = currconn->Next();
          }
          if (receivedany && receivedall)
            ++curravailablesequence;
        } while (receivedany && receivedall);
      }

      // IF THE AVAILABLE SEQUENCE FOR TURNS HAS CHANGED, UPDATE ALL
      // CONNECTIONS AND MARK EACH AS REQUIRING AN EXPLICIT ACKNOWLEDGEMENT.
      {
        CONNPTR currconn = s_conn_connlist.Head();
        while (currconn) {
          if ((curravailablesequence != currconn->availablesequence[TYPE_TURN]) &&
              (currconn->playerid != NOPLAYER) &&
              ((!(currconn->flags & PF_JOINING)) ||
               ((LONG)(curravailablesequence-currconn->incomingsequence[TYPE_TURN]) >= 0))) {
            currconn->availablesequence[TYPE_TURN] = curravailablesequence;
            if (!currconn->acktime[TYPE_TURN])
              currconn->acktime[TYPE_TURN] = GetTickCount();
          }
          currconn = currconn->Next();
        }
      }

    }

    // IF THIS IS A SYSTEM MESSAGE, PROCESS SYSTEM MESSAGES IN ORDER
    // FOR THIS CONNECTION
    if (pkt->header.type == TYPE_SYSTEM)
      SysProcessIncomingMessages(conn);

  }

}

//===========================================================================
static DWORD CALLBACK RecvThreadProc (LPVOID param) {
  DWORD wait = INFINITE;
  for (;;) {

    // WAIT FOR AT LEAST ONE PACKET TO BE RECEIVED AND QUEUED
    if (s_recv_shutdown) {
      _endthreadex(0);
      return 0;
    }
    BOOL msgwaiting = (WaitForSingleObject(s_recv_event,wait) == WAIT_OBJECT_0);
    if (s_recv_shutdown) {
      _endthreadex(0);
      return 0;
    }

    // ENTER THE API LOCK
    s_api_critsect.Enter();
    if (!s_spi) {
      s_api_critsect.Leave();
      _endthreadex(0);
      return 0;
    }

    // PROCESS ALL OUTSTANDING RECEIVED PACKETS
    if (msgwaiting) {
      TRACEOUT(TRACEHANDLE,"Background thread processing incoming packets:");
      RecvProcessExternalMessages();
      RecvProcessIncomingPackets();
    }
    else
      TRACEOUT(TRACEHANDLE,"Background thread maintaining connections:");

    // MAINTAIN CONNECTIONS
    wait = ConnMaintainConnections();

    // LEAVE THE API LOCK
    s_api_critsect.Leave();
    TRACEOUT(TRACEHANDLE,"  done");

  }
}

/****************************************************************************
*
*  USER INTERFACE FUNCTIONS
*
***/

//===========================================================================
static PROVIDERINFOPTR UiFindProvider (LPCSTR desc) {
  PROVIDERINFOPTR curr = s_spi_providerlist.Head();
  while (curr)
    if (!strcmp(curr->desc,desc))
      return curr;
    else
      curr = curr->Next();
  return NULL;
}

//===========================================================================
static void UiGetProgramDescription (LPCSTR             programname,
                                     SNETVERSIONDATAPTR versionptr,
                                     LPSTR              buffer,
                                     DWORD              buffersize) {

  // ADD THE NAME OF THE PROGRAM
  SStrCopy(buffer,programname,buffersize-2);
  strcat(buffer," ");

  // DETERMINE THE NAME OF THIS PROGRAM'S EXE FILE
  char programfilename[MAX_PATH] = "";
  GetModuleFileName((HMODULE)0,programfilename,MAX_PATH);

  // IF WE WERE GIVEN A HUMAN READABLE VERSION STRING, ADD THAT
  if (versionptr && versionptr->versionstring && *versionptr->versionstring)
    SStrPack(buffer,versionptr->versionstring,buffersize);

  // OTHERWISE, ADD THE HUMAN READABLE VERSION STRING FROM THE EXE FILE'S
  // VERSION RESOURCE
  else if (programfilename[0]) {
    DWORD  handle;
    DWORD  versioninfosize = GetFileVersionInfoSize((char *)programfilename,&handle);
    LPVOID versioninfo     = ALLOC(versioninfosize);
    if (GetFileVersionInfo((char *)programfilename,handle,versioninfosize,versioninfo)) {
      LPCTSTR info  = NULL;
      UINT    bytes = 0;
      if (VerQueryValueA(versioninfo,
                         "\\StringFileInfo\\040904b0\\ProductVersion",
                         (LPVOID *)&info,
                         &bytes) &&
          info && *info)
        SStrPack(buffer,info,buffersize);
    }
    FREE(versioninfo);
  }

}

//===========================================================================
static BOOL UiLoadArtwork (SNETGETARTPROC  artcallback,
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
    return FALSE;

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
    return FALSE;
  if (size) {
    size->cx = width;
    size->cy = height;
  }

  // ALLOCATE MEMORY FOR THE IMAGE
  DWORD bytes = width*height*bitdepth/8;
  *data = (LPBYTE)ALLOC(bytes);

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
    return FALSE;
  }

  // IF REQUESTED, UPDATE THE SYSTEM PALETTE
  if (setpalette)
    SDrawUpdatePalette(1,254,&pe[1]);

  return TRUE;
}

//===========================================================================
static BOOL CALLBACK UiSelectProviderDialogProc (HWND   window,
                                                 UINT   message,
                                                 WPARAM wparam,
                                                 LPARAM lparam) {
  static LPBYTE      background    = NULL;
  static LPBYTE      buttontexture = NULL;
  static UIPARAMSPTR uiparams      = NULL;
  switch (message) {

    case WM_COMMAND:

      // IF THE USER CLICKED THE CANCEL BUTTON, RETURN ZERO
      if (LOWORD(wparam) == IDCANCEL)
        SDlgEndDialog(window,0);

      // IF THE USER CLICKED THE CONNECT BUTTON, INITIALIZE THE SELECTED
      // PROVIDER AND RETURNS ITS PROVIDER ID
      else if (LOWORD(wparam) == IDOK) {
        char buffer[256] = "";
        int  cursel      = SendDlgItemMessage(window,IDC_PROVIDERLIST,LB_GETCURSEL,0,0);
        SendDlgItemMessageA(window,IDC_PROVIDERLIST,LB_GETTEXT,cursel,(LPARAM)(LPSTR)buffer);
        PROVIDERINFOPTR ptr = UiFindProvider(buffer);
        if (ptr) {

          // SAVE THIS AS THE NEW PREFERRED PROVIDER
          SRegSaveValue("Network Providers","Preferred Provider",0,ptr->id);

          // BUILD A NEW INTERFACE DATA STRUCTURE CONTAINING OUR WINDOW
          // HANDLE
          SNETUIDATA interfacedata;
          ZeroMemory(&interfacedata,sizeof(SNETUIDATA));
          if (uiparams->interfacedata)
            CopyMemory(&interfacedata,uiparams->interfacedata,sizeof(SNETUIDATA));
          interfacedata.size         = sizeof(SNETUIDATA);
          interfacedata.parentwindow = window;

          // CALL THE SELECTED CALLBACK IF AVAILABLE
          if (uiparams->interfacedata && uiparams->interfacedata->selectedcallback)
            if (!uiparams->interfacedata->selectedcallback(ptr->id,
                                                           &ptr->caps,
                                                           &interfacedata,
                                                           uiparams->versiondata))
              break;

          // INITIALIZE THE PROVIDER
          if (SNetInitializeProvider(ptr->id,
                                     uiparams->programdata,
                                     uiparams->playerdata,
                                     &interfacedata,
                                     uiparams->versiondata))
            SDlgEndDialog(window,ptr->id);

        }
      }

      // IF THE USER HIGHLIGHTED A NEW LISTBOX ITEM, UPDATE
      // THE STATIC TEXT.  IF THE USER DOUBLE-CLICKED AN ITEM,
      // POST AN 'OK' COMMAND.
      else if (LOWORD(wparam) == IDC_PROVIDERLIST)
        if (HIWORD(wparam) == LBN_SELCHANGE)
          PostMessage(window,WM_USER,0,0);
        else if (HIWORD(wparam) == LBN_DBLCLK)
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
      break;

    case WM_INITDIALOG:

      // SAVE A POINTER TO THE USER INTERFACE PARAMETERS
      uiparams = (UIPARAMSPTR)lparam;

      // LOAD THE ARTWORK FOR THIS DIALOG
      {
        SIZE size;
        if (UiLoadArtwork(uiparams->interfacedata->artcallback,
                          0,
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
          int controllist[3] = {IDC_MAXPLAYERS,IDC_REQUIREMENTS,0};
          SDlgSetControlBitmaps(window,
                                &controllist[0],
                                NULL,
                                background,
                                &size,
                                SDLG_ADJUST_CONTROLPOS);
        }
        if (UiLoadArtwork(uiparams->interfacedata->artcallback,
                          0,
                          SNET_ART_BUTTONTEXTURE,
                          0,
                          &buttontexture,
                          &size)) {
          int controllist[3] = {IDOK,IDCANCEL,0};
          SDlgSetControlBitmaps(window,
                                &controllist[0],
                                NULL,
                                buttontexture,
                                &size,
                                SDLG_ADJUST_VERTICAL);
        }
      }

      // FILL IN THE LIST BOX WITH THE LIST OF PROVIDERS
      {
        DWORD           selectid  = 0;
        PROVIDERINFOPTR selectptr = NULL;
        PROVIDERINFOPTR curr      = s_spi_providerlist.Head();
        SRegLoadValue("Network Providers","Preferred Provider",0,&selectid);
        while (curr) {
          if (curr->id &&
              SpiMeetsMinimumCaps(&curr->caps,uiparams->mincaps)) {
            SendDlgItemMessageA(window,IDC_PROVIDERLIST,LB_ADDSTRING,0,(LPARAM)(LPCSTR)curr->desc);
            if (curr->id == selectid)
              selectptr = curr;
          }
          curr = curr->Next();
        }
        WPARAM selectindex = 0;
        if (selectptr)
          selectindex = (WPARAM)SendDlgItemMessage(window,IDC_PROVIDERLIST,LB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)selectptr->desc);
        SendDlgItemMessage(window,IDC_PROVIDERLIST,LB_SETCURSEL,selectindex,0);
      }

      // FILL IN THE PROGRAM DESCRIPTION
      {
        char buffer[256] = "";
        UiGetProgramDescription(uiparams->programdata->programname,
                                uiparams->versiondata,
                                buffer,
                                256);
        SetDlgItemTextA(window,IDC_PROGRAMDESCRIPTION,buffer);
      }

      // UPDATE THE PLAYERS SUPPORTED AND REQUIREMENTS TEXT
      PostMessage(window,WM_USER,0,0);

      return 1;

    case WM_USER:
      {

        // FIND THE HIGHLIGHTED PROVIDER
        char buffer[256] = "";
        int  cursel      = SendDlgItemMessage(window,IDC_PROVIDERLIST,LB_GETCURSEL,0,0);
        SendDlgItemMessageA(window,IDC_PROVIDERLIST,LB_GETTEXT,cursel,(LPARAM)(LPSTR)buffer);
        PROVIDERINFOPTR ptr = UiFindProvider(buffer);
        if (!ptr)
          break;

        // DISPLAY THE MAXIMUM NUMBER OF PLAYERS
        {
          char buffer[64];
          GetDlgItemTextA(window,IDC_MAXPLAYERS,buffer,63);
          if (SStrChr(buffer,':'))
            wsprintf(SStrChr(buffer,':')+1,
                     " %u",
                     min(uiparams->programdata->maxplayers,ptr->caps.maxplayers));
          SetDlgItemTextA(window,IDC_MAXPLAYERS,buffer);
        }

        // DISPLAY THE REQUIREMENTS
        {
          char buffer[256];
          GetDlgItemTextA(window,IDC_REQUIREMENTS,buffer,255);
          if (SStrChr(buffer,':'))
            wsprintf(SStrChr(buffer,':')+1,
                     "\n%s",
                     ptr->req);
          SetDlgItemTextA(window,IDC_REQUIREMENTS,buffer);
        }

      }
      break;

  }
  return SDlgDefDialogProc(window,message,wparam,lparam);
}


/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

#define  ENTER_APILOCK_READ     s_api_critsect.Enter()
#define  ENTER_APILOCK_WRITE    s_api_critsect.Enter()
#define  FAILOUT_APILOCK_READ   do {                             \
                                  TRACEOUT(TRACEHANDLE,          \
                                           "  [fail: %08x]",     \
                                           SErrGetLastError());  \
                                  s_api_critsect.Leave();        \
                                  return FALSE;                  \
                                } while (FALSE)
#define  FAILOUT_APILOCK_WRITE  do {                             \
                                  TRACEOUT(TRACEHANDLE,          \
                                           "  [fail: %08x]",     \
                                           SErrGetLastError());  \
                                  s_api_critsect.Leave();        \
                                  return FALSE;                  \
                                } while (FALSE)
#define  LEAVE_APILOCK_READ     s_api_critsect.Leave()
#define  LEAVE_APILOCK_WRITE    s_api_critsect.Leave()

//===========================================================================
BOOL APIENTRY SNetCreateGame (LPCSTR  gamename,
                              LPCSTR  gamepassword,
                              LPCSTR  gamedescription,
                              DWORD   gamecategorybits,
                              LPVOID  initdata,
                              DWORD   initdatabytes,
                              DWORD   maxplayers,
                              LPCSTR  playername,
                              LPCSTR  playerdescription,
                              DWORD  *playerid) {

  // VALIDATE PARAMETERS
  if (playerid)
    *playerid = SNET_INVALIDPLAYERID+s_api_playeroffset;

  VALIDATEBEGIN;
  VALIDATE(gamename);
  VALIDATE(*gamename);
  VALIDATE(maxplayers);
  VALIDATE(playerid);
  VALIDATEEND;

  // LOG THE CALL
  ENTER_APILOCK_WRITE;
  if (!s_spi) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_WRITE;
  }
  if (!gamepassword)
    gamepassword = "";
  if (!gamedescription)
    gamedescription = "";
  if (!playername)
    playername = "";
  if (!playerdescription)
    playerdescription = "";
  TRACEOUT(TRACEHANDLE,
           "SNetCreateGame(\"%s\",\"%s\",\"%s\",0x%08x,0x%08x,%u,%u,\"%s\",\"%s\",*playerid)",
           gamename,gamepassword,gamedescription,gamecategorybits,initdata,initdatabytes,maxplayers,playername,playerdescription);

  // IF NO PLAYER NAME WAS PROVIDED, AND THE CURRENT NETWORK PROVIDER
  // IS CAPABLE OF PROVIDING IT, THEN GET THE NAME OF THE LOGGED ON
  // USER FROM THE NETWORK PROVIDER
  char localplayername[SNETSPI_MAXSTRINGLENGTH] = "";
  char localplayerdesc[SNETSPI_MAXSTRINGLENGTH] = "";
  if ((!*playername) && s_spi->GetLocalPlayerName) {
    s_spi->GetLocalPlayerName(localplayername,
                              SNETSPI_MAXSTRINGLENGTH,
                              localplayerdesc,
                              SNETSPI_MAXSTRINGLENGTH);
    playername        = localplayername;
    playerdescription = localplayerdesc;
  }
  if (!*playername) {
    SErrSetLastError(SNET_ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  // IF WE ARE ALREADY IN A GAME, LEAVE IT
  if (s_game_playerid != NOPLAYER)
    SNetLeaveGame(SNET_EXIT_AUTO_NEWGAME);

  // RESET THE CONNECTION TABLE
  ConnDestroy();

  // SET THE MAXIMUM NUMBER OF PLAYERS ALLOWED
  s_game_playersallowed = maxplayers;

  // SAVE THE GAME NAME AND DESCRIPTION
  SStrCopy(s_game_gamename,gamename,SNETSPI_MAXSTRINGLENGTH);
  SStrCopy(s_game_gamedesc,gamedescription,SNETSPI_MAXSTRINGLENGTH);
  SStrCopy(s_game_gamepass,gamepassword,SNETSPI_MAXSTRINGLENGTH);
  s_game_creationtime = GetTickCount();
  s_game_categorybits = gamecategorybits;

  // SET THE GAME MODE
  s_game_gamemode = 0;
  if (*gamepassword)
    s_game_gamemode |= SNET_GM_PRIVATE;
  if (s_game_playersallowed <= 1)
    s_game_gamemode |= SNET_GM_FULL;

  // SAVE THE INITIALIZATION DATA
  if (s_game_initdata)
    FREE(s_game_initdata);
  s_game_initdata      = ALLOC(initdatabytes);
  s_game_initdatabytes = initdatabytes;
  CopyMemory(s_game_initdata,initdata,initdatabytes);

  // ASSIGN THE LOCAL PLAYER A PLAYER ID, AND MARK HIM AS THE GAME OWNER
  s_game_playerid = 0;
  CONNPTR conn = ConnFindLocal();
  if (conn) {
    conn->flags     = 0;
    conn->playerid  = s_game_playerid;
    conn->gameowner = TRUE;
    SStrCopy(conn->name,playername       ,SNETSPI_MAXSTRINGLENGTH);
    SStrCopy(conn->desc,playerdescription,SNETSPI_MAXSTRINGLENGTH);
  }
  if (playerid)
    *playerid = s_game_playerid+s_api_playeroffset;

  // BUILD THE CLIENT DATA BLOCK
  BYTE  clientdata[SNETSPI_MAXCLIENTDATA];
  DWORD clientdatabytes;
  GameBuildClientData(clientdata,
                      &clientdatabytes);

  // START ADVERTISING THE GAME SO THAT OTHERS CAN JOIN IT
  TRACEOUT(TRACEHANDLE,
           "  spiStartAdvertisingGame(\"%s\",\"%s\",\"%s\",%u,%u,0x%08x,0x%08x,0x%08x,%u)",
           s_game_gamename,
           s_game_gamepass,
           s_game_gamedesc,
           s_game_gamemode,
           0,
           s_game_categorybits,
           s_game_optcategorybits,
           clientdata,
           clientdatabytes);
  if (!s_spi->StartAdvertisingGame(s_game_gamename,
                                   s_game_gamepass,
                                   s_game_gamedesc,
                                   s_game_gamemode,
                                   0,
                                   s_game_categorybits,
                                   s_game_optcategorybits,
                                   clientdata,
                                   clientdatabytes)) {
    // SERVICE PROVIDER IS RESPONSIBLE FOR CALLING SETLASTERROR()
    DWORD lasterror = SErrGetLastError();
    SNetLeaveGame(SNET_EXIT_AUTO_JOINING);
    SErrSetLastError(lasterror);
    FAILOUT_APILOCK_WRITE;
  }

  // MARK THE GAME AS ADVERTISED
  s_game_gamemode |= SNET_GM_ADVERTISED;

  LEAVE_APILOCK_WRITE;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetDestroy () {
  ENTER_APILOCK_WRITE;
  TRACEOUT(TRACEHANDLE,"SNetDestroy()");

  // IF WE ARE IN A GAME, LEAVE IT
  if (s_game_playerid != NOPLAYER)
    SNetLeaveGame(SNET_EXIT_AUTO_SHUTDOWN);

  // TERMINATE THE RECEIVE THREAD
  LEAVE_APILOCK_WRITE;
  RecvDestroy();
  ENTER_APILOCK_WRITE;

  // REMOVE ALL MESSAGES FROM THE SYSTEM MESSAGE QUEUE
  SysDestroy();

  // UNREGISTER ALL EVENT HANDLERS
  SEvtUnregisterType(REGISTERTYPE,REGISTERSUBTYPE_SNETEVENT);
  SEvtUnregisterType(REGISTERTYPE,REGISTERSUBTYPE_SYSEVENT);

  // DESTROY ALL CONNECTIONS
  ConnDestroy();

  // UNBIND FROM THE SERVICE PROVIDER AND CLEAR THE PROVIDER LIST
  SpiDestroy(TRUE);

  LEAVE_APILOCK_WRITE;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetDropPlayer (DWORD playerid, DWORD exitcode) {
  ENTER_APILOCK_WRITE;
  TRACEOUT(TRACEHANDLE,
           "SNetDropPlayer(%u,0x%08x)",
           playerid,exitcode);

  // VALIDATE PARAMETERS
  if (!s_spi) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_WRITE;
  }
  if ((playerid == NOPLAYER) ||
      (playerid == s_game_playerid)) {
    SErrSetLastError(SNET_ERROR_INVALID_PARAMETER);
    FAILOUT_APILOCK_WRITE;
  }
  if (s_game_playerid == NOPLAYER) {
    SErrSetLastError(SNET_ERROR_NOT_IN_GAME);
    FAILOUT_APILOCK_WRITE;
  }

  // FIND THE REFERENCED PLAYER
  CONNPTR conn = ConnFindByPlayerId(playerid);
  if (!conn) {
    SErrSetLastError(SNET_ERROR_INVALID_PLAYER);
    FAILOUT_APILOCK_WRITE;
  }

  // MARK THE PLAYER AS LEAVING
  conn->flags         |= PF_LEAVING;
  conn->finalsequence  = conn->incomingsequence[TYPE_TURN];
  conn->exitcode       = exitcode;

  // SEND A DROP PLAYER MESSAGE TO ALL OTHER PLAYERS
  {
    CONNPTR checkconn = s_conn_connlist.Head();
    while (checkconn) {
      if ((checkconn           != conn) &&
          (checkconn->playerid != NOPLAYER)) {
        SYSEVENTDATA_DROPPLAYER eventdata;
        eventdata.playerid      = playerid;
        eventdata.finalsequence = conn->incomingsequence[TYPE_TURN];
        eventdata.exitcode      = exitcode;
        ConnSendMessage(checkconn,
                        TYPE_SYSTEM,
                        SYS_DROPPLAYER,
                        &eventdata,
                        sizeof(SYSEVENTDATA_DROPPLAYER));
      }
      checkconn = checkconn->Next();
    }
  }

  LEAVE_APILOCK_WRITE;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetEnumDevices (SNETENUMDEVICESPROC callback) {
  VALIDATEBEGIN;
  VALIDATE(callback);
  VALIDATEEND;

  ENTER_APILOCK_READ;
  TRACEOUT(TRACEHANDLE,
           "SNetEnumDevices(0x%08x)",
           callback);

  // VALIDATE PARAMETERS
  if (!s_spi) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_READ;
  }

  // MAKE A LOCAL COPY OF ALL DEVICES
  SNETSPI_DEVICELISTPTR devicearray = NULL;
  DWORD                 devices     = 0;
  {

    // LOCK THE DEVICE LIST
    SNETSPI_DEVICELISTPTR head;
    TRACEOUT(TRACEHANDLE,"  spiLockDeviceList(*devicelist)");
    if (!s_spi->LockDeviceList(&head)) {
      // SERVICE PROVIDER IS RESPONSIBLE FOR CALLING SETLASTERROR()
      FAILOUT_APILOCK_READ;
    }

    // DETERMINE THE TOTAL NUMBER OF DEVICES
    {
      SNETSPI_DEVICELISTPTR curr = head;
      while (curr) {
        ++devices;
        curr = curr->next;
      }
    }

    // ALLOCATE AN ARRAY TO HOLD THE DEVICES
    if (devices) {
      devicearray = (SNETSPI_DEVICELISTPTR)ALLOC(devices*sizeof(SNETSPI_DEVICELIST));

      // COPY THE DEVICES
      SNETSPI_DEVICELISTPTR source = head;
      SNETSPI_DEVICELISTPTR dest   = devicearray;
      while (source) {
        CopyMemory(dest,source,sizeof(SNETSPI_DEVICELIST));
        dest->next = dest+1;
        dest   = dest->next;
        source = source->next;
      }

    }

    // UNLOCK THE DEVICE LIST
    TRACEOUT(TRACEHANDLE,
             "  spiUnlockDeviceList(0x%08x)",
             head);
    s_spi->UnlockDeviceList(head);

  }

  // LEAVE THE API LOCK
  LEAVE_APILOCK_READ;

  // IF THERE WERE NO DEVICES, RETURN NOW
  if (!(devices && devicearray))
    return TRUE;

  // CALL THE ENUMERATION FUNCTION ONCE FOR EACH DEVICE
  {
    SNETSPI_DEVICELISTPTR curr = devicearray;
    while (devices--) {
      TRACEOUT(TRACEHANDLE,
               "  callback(0x%08x,\"%s\",\"%s\")",
               curr->deviceid,curr->devicename,curr->devicedescription);
      if (callback(curr->deviceid,curr->devicename,curr->devicedescription))
        curr = curr->next;
      else
        devices = 0;
    }
  }

  // FREE THE LOCAL COPY
  FREE(devicearray);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetEnumGames (DWORD              categorybits,
                             DWORD              categorymask,
                             SNETENUMGAMESPROC  callback,
                             DWORD             *hintnextcall) {
  VALIDATEBEGIN;
  VALIDATE(callback);
  VALIDATEEND;

  ENTER_APILOCK_READ;
  TRACEOUT(TRACEHANDLE,
           "SNetEnumGames(0x%08x,0x%08x,0x%08x,*hintnextcall)",
           categorybits,categorymask,callback);

  // VALIDATE PARAMETERS
  if (hintnextcall)
    *hintnextcall = 0;
  if (!s_spi) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_READ;
  }

  // MAKE A LOCAL COPY OF ALL MATCHING GAMES
  SNETSPI_GAMELISTPTR gamearray = NULL;
  DWORD               games     = 0;
  GameCopyGameList(categorybits,
                   categorymask,
                   &gamearray,
                   &games,
                   hintnextcall);

  // LEAVE THE API LOCK
  LEAVE_APILOCK_READ;

  // CALL THE ENUMERATION FUNCTION ONCE FOR EACH GAME
  {
    SNETSPI_GAMELISTPTR curr = gamearray;
    while (games--) {
      TRACEOUT(TRACEHANDLE,
               "  callback(0x%08x,\"%s\",\"%s\")",
               curr->gameid,curr->gamename,curr->gamedescription);
      TRACEDUMP(TRACEHANDLE,&curr->owner,sizeof(SNETADDR));
      if (callback(curr->gameid,curr->gamename,curr->gamedescription))
        curr = curr->next;
      else
        games = 0;
    }
  }

  // FREE THE LOCAL COPY
  FREEIFUSED(gamearray);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetEnumGamesEx (DWORD                categorybits,
                               DWORD                categorymask,
                               SNETENUMGAMESEXPROC  callback,
                               DWORD               *hintnextcall) {
  VALIDATEBEGIN;
  VALIDATE(callback);
  VALIDATEEND;

  ENTER_APILOCK_READ;
  TRACEOUT(TRACEHANDLE,
           "SNetEnumGamesEx(0x%08x,0x%08x,0x%08x,*hintnextcall)",
           categorybits,categorymask,callback);

  // VALIDATE PARAMETERS
  if (hintnextcall)
    *hintnextcall = 0;
  if (!s_spi) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_READ;
  }

  // MAKE A LOCAL COPY OF ALL MATCHING GAMES
  SNETSPI_GAMELISTPTR gamearray = NULL;
  DWORD               games     = 0;
  GameCopyGameList(categorybits,
                   categorymask,
                   &gamearray,
                   &games,
                   hintnextcall);

  // LEAVE THE API LOCK
  LEAVE_APILOCK_READ;

  // CALL THE ENUMERATION FUNCTION ONCE FOR EACH GAME
  {
    SNETSPI_GAMELISTPTR curr = gamearray;
    SNETGAME            gamedata;
    while (games--) {
      GameBuildGameData(&gamedata,curr);
      TRACEOUT(TRACEHANDLE,
               "  callback(0x%08x)",
               &gamedata);
      TRACEDUMP(TRACEHANDLE,&curr->owner,sizeof(SNETADDR));
      if (callback(&gamedata))
        curr = curr->next;
      else
        games = 0;
    }
  }

  // FREE THE LOCAL COPY
  FREEIFUSED(gamearray);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetEnumProviders (SNETCAPSPTR           mincaps,
                                 SNETENUMPROVIDERSPROC callback) {
  VALIDATEBEGIN;
  VALIDATE(callback);
  VALIDATE((!mincaps) || (mincaps->size == sizeof(SNETCAPS)));
  VALIDATEEND;

  ENTER_APILOCK_WRITE;
  TRACEOUT(TRACEHANDLE,
           "SNetEnumProviders(mincaps,0x%08x)",
           callback);

  // BUILD A LIST OF PROVIDERS IF WE DON'T ALREADY HAVE ONE
  SpiFindAllProviders();

  // CALL THE CALLBACK FUNCTION ONCE FOR EACH PROVIDER IN THE LIST
  // WHICH MEETS THE MINIMUM CAPABILITIES
  PROVIDERINFOPTR curr = s_spi_providerlist.Head();
  while (curr) {
    if (curr->id && curr->desc && *curr->desc &&
        SpiMeetsMinimumCaps(&curr->caps,mincaps)) {
      TRACEOUT(TRACEHANDLE,
               "  callback(0x%08x,\"%s\",\"%s\",caps)",
               curr->id,curr->desc,curr->req);
      if (!callback(curr->id,curr->desc,curr->req,&curr->caps))
        curr = NULL;
    }
    if (s_spi_providersfound && (!s_spi_providerlist.IsEmpty()) && curr)
      curr = curr->Next();
    else
      curr = NULL;
  }

  LEAVE_APILOCK_WRITE;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetGetGameInfo (DWORD   index,
                               LPVOID  buffer,
                               DWORD   buffersize,
                               DWORD  *byteswritten) {
  ENTER_APILOCK_READ;
  TRACEOUT(TRACEHANDLE,
           "SNetGetGameInfo(%u,0x%08x,%u,*byteswritten)",
           index,buffer,buffersize);

  // VALIDATE PARAMETERS
  if (buffer && buffersize)
    ZeroMemory(buffer,buffersize);
  if (byteswritten)
    *byteswritten = NULL;
  if (!s_spi) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_READ;
  }
  if (s_game_playerid == NOPLAYER) {
    SErrSetLastError(SNET_ERROR_NOT_IN_GAME);
    FAILOUT_APILOCK_READ;
  }

  // FIND THE REQUESTED INFORMATION
  LPVOID info      = NULL;
  DWORD  infobytes = 0;
  BOOL   nullterm  = FALSE;
  switch (index) {

    case SNET_INFO_GAMENAME:
      info      = s_game_gamename;
      infobytes = SStrLen(s_game_gamename)+1;
      nullterm  = TRUE;
      break;

    case SNET_INFO_GAMEPASSWORD:
      info      = s_game_gamepass;
      infobytes = SStrLen(s_game_gamepass)+1;
      nullterm  = TRUE;
      break;

    case SNET_INFO_GAMEDESCRIPTION:
      info      = s_game_gamedesc;
      infobytes = SStrLen(s_game_gamedesc)+1;
      nullterm  = TRUE;
      break;

    case SNET_INFO_GAMEMODE:
      info      = &s_game_gamemode;
      infobytes = sizeof(s_game_gamemode);
      break;

    case SNET_INFO_INITDATA:
      info      = s_game_initdata;
      infobytes = s_game_initdatabytes;
      break;

    case SNET_INFO_MAXPLAYERS:
      info      = &s_game_playersallowed;
      infobytes = sizeof(s_game_playersallowed);
      break;

  }
  if (!info) {
    SErrSetLastError(SNET_ERROR_INVALID_PARAMETER);
    FAILOUT_APILOCK_READ;
  }

  // IF THE USER DIDN'T PASS A BUFFER, SIMPLY RETURN THE NUMBER OF BYTES
  // OF INFORMATION WE HAVE AVAILABLE
  if (!(buffer && buffersize)) {
    if (byteswritten)
      *byteswritten = infobytes;
  }

  // OTHERWISE, COPY THE INFORMATION INTO THE USER'S BUFFER
  else {
    CopyMemory(buffer,info,min(buffersize,infobytes));
    if (byteswritten)
      *byteswritten = min(buffersize,infobytes);
    if (nullterm)
      *((LPSTR)buffer+buffersize-1) = 0;
  }

  LEAVE_APILOCK_READ;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetGetNetworkLatency (DWORD  measurementtype,
                                     DWORD *result) {
  VALIDATEBEGIN;
  VALIDATE(result);
  VALIDATE(measurementtype >= SNET_LMT_EXPECTED);
  VALIDATE(measurementtype <= SNET_LMT_PEAK);
  VALIDATEEND;

  ENTER_APILOCK_READ;
  TRACEOUT(TRACEHANDLE,
           "SNetGetNetworkLatency(%u,*result)",
           measurementtype);

  // VALIDATE PARAMETERS
  if (result)
    *result = 0;
  if (!(s_spi && s_spi_providerptr)) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_READ;
  }

  // RETURN THE LATENCY
  switch (measurementtype) {

    case SNET_LMT_EXPECTED:
      *result = s_spi_providerptr->caps.latencyms;
      break;

    case SNET_LMT_CURRENT:
    case SNET_LMT_PEAK:
      {
        CONNPTR conn = s_conn_connlist.Head();
        while (conn) {
          if (conn->playerid != NOPLAYER) {
            DWORD latency = 0;
            if (measurementtype == SNET_LMT_CURRENT)
              latency = conn->peaklatency;
            if (latency < conn->latency)
              latency = conn->latency;
            if (latency > *result)
              *result = latency;
          }
          conn = conn->Next();
        }
      }
      break;

    default:
      SErrSetLastError(SNET_ERROR_INVALID_PARAMETER);
      FAILOUT_APILOCK_READ;

  }

  LEAVE_APILOCK_READ;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetGetNumPlayers (DWORD *firstplayerid,
                                 DWORD *lastplayerid,
                                 DWORD *activeplayers) {
  ENTER_APILOCK_READ;
  TRACEOUT(TRACEHANDLE,
           "SNetGetNumPlayers(*firstplayerid,*lastplayerid,*activeplayers)");

  // VALIDATE PARAMETERS
  if (firstplayerid)
    *firstplayerid = SNET_INVALIDPLAYERID+s_api_playeroffset;
  if (lastplayerid)
    *lastplayerid = SNET_INVALIDPLAYERID+s_api_playeroffset;
  if (activeplayers)
    *activeplayers = 0;
  if (!s_spi) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_READ;
  }
  if (s_game_playerid == NOPLAYER) {
    SErrSetLastError(SNET_ERROR_NOT_IN_GAME);
    FAILOUT_APILOCK_READ;
  }

  // START WITH THE LOCAL PLAYER
  if (firstplayerid)
    *firstplayerid = s_game_playerid+s_api_playeroffset;
  if (lastplayerid)
    *lastplayerid = s_game_playerid+s_api_playeroffset;
  if (activeplayers)
    *activeplayers = 1;

  // TAKE INTO ACCOUNT ALL OTHER ACTIVE PLAYERS
  {
    CONNPTR conn = s_conn_connlist.Head();
    while (conn) {
      if (conn->playerid != NOPLAYER) {
        if (firstplayerid)
          *firstplayerid = min(conn->playerid+s_api_playeroffset,*firstplayerid);
        if (lastplayerid)
          *lastplayerid = max(conn->playerid+s_api_playeroffset,*lastplayerid);
        if (activeplayers)
          ++*activeplayers;
      }
      conn = conn->Next();
    }
  }

  // LOG THE RETURNED INFORMATION
  if (firstplayerid)
    TRACEOUT(TRACEHANDLE,"  firstplayerid=%u",*firstplayerid);
  if (lastplayerid)
    TRACEOUT(TRACEHANDLE,"  lastplayerid=%u",*lastplayerid);
  if (activeplayers)
    TRACEOUT(TRACEHANDLE,"  activeplayers=%u",*activeplayers);

  LEAVE_APILOCK_READ;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetGetOwnerId (DWORD *playerid) {
  VALIDATEBEGIN;
  VALIDATE(playerid);
  VALIDATEEND;

  ENTER_APILOCK_WRITE;
  TRACEPEND(TRACEHANDLE,"SNetGetOwnerId(*playerid)");

  // VALIDATE PARAMETERS
  *playerid = SNET_INVALIDPLAYERID+s_api_playeroffset;
  if (!s_spi) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_WRITE;
  }
  if (s_game_playerid == NOPLAYER) {
    SErrSetLastError(SNET_ERROR_NOT_IN_GAME);
    FAILOUT_APILOCK_WRITE;
  }

  // CHECK FOR TRANSFER OF OWNERSHIP
  if (s_game_playerid != NOPLAYER)
    GameProcessLeavingPlayers();

  // FIND THE GAME OWNER
  CONNPTR local = ConnFindLocal();
  if (!local) {
    SErrSetLastError(SNET_ERROR_NOT_IN_GAME);
    FAILOUT_APILOCK_WRITE;
  }
  CONNPTR conn = local;
  if (!(conn && conn->gameowner)) {
    conn = s_conn_connlist.Head();
    while (conn && !conn->gameowner)
      conn = conn->Next();
  }
  if (!(conn && conn->gameowner)) {
    SErrSetLastError(SNET_ERROR_NOT_IN_GAME);
    FAILOUT_APILOCK_WRITE;
  }

  // RETURN THE GAME OWNER'S PLAYER ID
  *playerid = conn->playerid+s_api_playeroffset;

  LEAVE_APILOCK_WRITE;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetGetOwnerTurnsWaiting (DWORD *turns) {
  VALIDATEBEGIN;
  VALIDATE(turns);
  VALIDATEEND;

  ENTER_APILOCK_WRITE;
  TRACEPEND(TRACEHANDLE,"SNetGetOwnerTurnsWaiting(*turns)");
  *turns = 0;

  // GET THE PLAYER ID OF THE GAME OWNER
  DWORD playerid;
  if (!SNetGetOwnerId(&playerid)) {
    // SNETGETOWNERID() IS RESPONSIBLE FOR CALLING SETLASTERROR()
    FAILOUT_APILOCK_WRITE;
  }
  playerid -= s_api_playeroffset;

  // FIND THE CONNECTION RECORD ASSOCIATED WITH THE GAME OWNER
  CONNPTR conn = ConnFindByPlayerId(playerid);
  if (!(conn && conn->gameowner)) {
    SErrSetLastError(SNET_ERROR_NOT_IN_GAME);
    FAILOUT_APILOCK_WRITE;
  }

  // CLEAR OUT ANY UNNEEDED OR ALREADY PROCESSED TURNS FROM THE GAME OWNER
  ConnClearOldTurns(conn);

  // DETERMINE THE NUMBER OF TURNS WE STILL HAVE QUEUED FROM THE GAME OWNER
  MESSAGEPTR message = conn->incomingqueue[TYPE_TURN].Head();
  while (message) {
    ++*turns;
    message = message->Next();
  }

  LEAVE_APILOCK_WRITE;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetGetPerformanceData (DWORD          counterid,
                                      DWORD         *countervalue,
                                      DWORD         *countertype,
                                      LONG          *counterscale,
                                      LARGE_INTEGER *measurementtime,
                                      LARGE_INTEGER *measurementfreq) {
  VALIDATEBEGIN;
  VALIDATE(counterid);
  VALIDATE(counterid < SNET_PERFIDNUM);
  VALIDATE(countervalue);
  VALIDATEEND;

  ENTER_APILOCK_READ;
  TRACEOUT(TRACEHANDLE,
           "SNetGetPerformanceData(%u,*countervalue,*countertype,*counterscale,*measurementtime,*measurementfreq)",
           counterid);

  // SET THE VALUES BASED ON OUR INTERNAL PERFORMANCE DATA
  *countervalue = s_perf_data[counterid].value;
  if (countertype)
    *countertype  = s_perf_data[counterid].type;
  if (counterscale)
    *counterscale = s_perf_data[counterid].scale;
  if (measurementtime) {
    SYSTEMTIME systime;
    GetSystemTime(&systime);
    SystemTimeToFileTime(&systime,(FILETIME *)measurementtime);
  }
  if (measurementfreq) {
    measurementfreq->LowPart  = 10000000;
    measurementfreq->HighPart = 0;
  }

  // IF THIS COUNTER IS PROVIDER SPECIFIC, ALLOW THE CURRENT PROVIDER TO
  // OVERWRITE OUR RESULTS
  if (s_perf_data[counterid].providerspecific && s_spi) {
    LARGE_INTEGER localtime;
    LARGE_INTEGER localfreq;
    if (!measurementtime)
      measurementtime = &localtime;
    if (!measurementfreq)
      measurementfreq = &localfreq;
    TRACEOUT(TRACEHANDLE,
             "  spiGetPerformanceData(%u,*countervalue,*measurementtime,*measurementfreq)",
             counterid);
    s_spi->GetPerformanceData(counterid,countervalue,measurementtime,measurementfreq);
  }

  LEAVE_APILOCK_READ;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetGetPlayerCaps (DWORD       playerid,
                                 SNETCAPSPTR caps) {
  VALIDATEBEGIN;
  VALIDATE(caps);
  VALIDATE(caps->size == sizeof(SNETCAPS));
  VALIDATEEND;

  ENTER_APILOCK_READ;
  TRACEOUT(TRACEHANDLE,
           "SNetGetPlayerCaps(%u,*caps)",
           playerid);

  // VALIDATE PARAMETERS
  ZeroMemory(((LPDWORD)caps)+1,sizeof(SNETCAPS)-sizeof(DWORD));
  if (!(s_spi && s_spi_providerptr)) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_READ;
  }
  if (s_game_playerid == NOPLAYER) {
    SErrSetLastError(SNET_ERROR_NOT_IN_GAME);
    FAILOUT_APILOCK_READ;
  }

  // FIND THE REQUESTED PLAYER
  CONNPTR conn = ConnFindByPlayerId(playerid-s_api_playeroffset);
  if (!conn) {
    SErrSetLastError(SNET_ERROR_INVALID_PLAYER);
    FAILOUT_APILOCK_READ;
  }

  // COPY THE PROVIDER CAPS
  CopyMemory(caps,&s_spi_providerptr->caps,sizeof(SNETCAPS));

  // ADD THE USER'S LATENCY
  if (conn->latency)
    caps->latencyms = conn->latency;

  LEAVE_APILOCK_READ;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetGetPlayerName (DWORD playerid,
                                 LPSTR buffer,
                                 DWORD buffersize) {
  VALIDATEBEGIN;
  VALIDATE(buffer);
  VALIDATE(buffersize);
  VALIDATEEND;

  ENTER_APILOCK_READ;
  TRACEOUT(TRACEHANDLE,
           "SNetGetPlayerName(%u,0x%08x,%u)",
           playerid,buffer,buffersize);

  // VALIDATE PARAMETERS
  *buffer = 0;
  if (!s_spi) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_READ;
  }
  if (s_game_playerid == NOPLAYER) {
    SErrSetLastError(SNET_ERROR_NOT_IN_GAME);
    FAILOUT_APILOCK_READ;
  }

  // FIND THE REQUESTED PLAYER
  CONNPTR conn = ConnFindByPlayerId(playerid-s_api_playeroffset);
  if ((!conn) || (conn->flags & PF_JOINING)) {
    SErrSetLastError(SNET_ERROR_INVALID_PLAYER);
    FAILOUT_APILOCK_READ;
  }

  // RETURN THE PLAYER'S NAME
  SStrCopy(buffer,conn->name,buffersize);

  LEAVE_APILOCK_READ;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetGetProviderCaps (SNETCAPSPTR caps) {
  VALIDATEBEGIN;
  VALIDATE(caps);
  VALIDATE(caps->size == sizeof(SNETCAPS));
  VALIDATEEND;

  ENTER_APILOCK_READ;
  TRACEOUT(TRACEHANDLE,
           "SNetGetProviderCaps(*caps)");

  // VALIDATE PARAMETERS
  ZeroMemory(((LPDWORD)caps)+1,sizeof(SNETCAPS)-sizeof(DWORD));
  if (!(s_spi && s_spi_providerptr)) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_READ;
  }

  // COPY THE PROVIDER CAPS
  CopyMemory(caps,&s_spi_providerptr->caps,sizeof(SNETCAPS));

  LEAVE_APILOCK_READ;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetGetTurnsInTransit (DWORD *turns) {
  VALIDATEBEGIN;
  VALIDATE(turns);
  VALIDATEEND;

  ENTER_APILOCK_READ;
  TRACEPEND(TRACEHANDLE,"SNetGetTurnsInTransit(*turns)");

  // VALIDATE PARAMETERS
  *turns = 0;
  if (!s_spi) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_READ;
  }
  if (s_game_playerid == NOPLAYER) {
    SErrSetLastError(SNET_ERROR_NOT_IN_GAME);
    FAILOUT_APILOCK_READ;
  }

  // FIND OUR PLAYER RECORD
  CONNPTR conn = ConnFindLocal();
  if (!conn) {
    SErrSetLastError(SNET_ERROR_NOT_ENOUGH_MEMORY);
    FAILOUT_APILOCK_READ;
  }

  // DETERMINE THE NUMBER OF TURNS WE HAVE SENT OUT BEYOND WHAT WE HAVE
  // PROCESSED
  *turns = conn->outgoingsequence[TYPE_TURN]
             -conn->incomingsequence[TYPE_TURN];

  LEAVE_APILOCK_READ;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetInitializeDevice (DWORD              deviceid,
                                    SNETPROGRAMDATAPTR programdata,
                                    SNETPLAYERDATAPTR  playerdata,
                                    SNETUIDATAPTR      interfacedata,
                                    SNETVERSIONDATAPTR versiondata) {
  ENTER_APILOCK_WRITE;
  TRACEOUT(TRACEHANDLE,
           "SNetInitializeDevice(0x%08x,0x%08x,0x%08x,0x%08x,0x%08x)",
           deviceid,programdata,playerdata,interfacedata,versiondata);
  TRACEDUMPDATABLOCKS(TRACEHANDLE,
                      programdata,playerdata,interfacedata,versiondata);

  // VALIDATE PARAMETERS
  if (!s_spi) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_READ;
  }
  SNETPROGRAMDATA modprogramdata;
  SNETPLAYERDATA  modplayerdata;
  SNETUIDATA      modinterfacedata;
  SNETVERSIONDATA modversiondata;
  if (!SpiNormalizeDataBlocks(programdata,playerdata,interfacedata,versiondata,
                              &modprogramdata,&modplayerdata,&modinterfacedata,&modversiondata)) {
    SErrSetLastError(SNET_ERROR_INVALID_PARAMETER);
    FAILOUT_APILOCK_WRITE;
  }

  // INITIALIZE THE DEVICE
  if (!s_spi->InitializeDevice(deviceid,
                               &modprogramdata,
                               &modplayerdata,
                               &modinterfacedata,
                               &modversiondata)) {
    // SERVICE PROVIDER IS RESPONSIBLE FOR CALLING SETLASTERROR()
    FAILOUT_APILOCK_WRITE;
  }

  LEAVE_APILOCK_WRITE;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetInitializeProvider (DWORD              providerid,
                                      SNETPROGRAMDATAPTR programdata,
                                      SNETPLAYERDATAPTR  playerdata,
                                      SNETUIDATAPTR      interfacedata,
                                      SNETVERSIONDATAPTR versiondata) {
  ENTER_APILOCK_WRITE;
  TRACEOUT(TRACEHANDLE,
           "(using Storm build " __DATE__ " " __TIME__ ")");
  TRACEOUT(TRACEHANDLE,
           "SNetInitializeProvider(0x%08x,0x%08x,0x%08x,0x%08x,0x%08x)",
           providerid,programdata,playerdata,interfacedata,versiondata);
  TRACEDUMPDATABLOCKS(TRACEHANDLE,
                      programdata,playerdata,interfacedata,versiondata);

  // VALIDATE PARAMETERS
  SNETPROGRAMDATA modprogramdata;
  SNETPLAYERDATA  modplayerdata;
  SNETUIDATA      modinterfacedata;
  SNETVERSIONDATA modversiondata;
  if (!SpiNormalizeDataBlocks(programdata,playerdata,interfacedata,versiondata,
                              &modprogramdata,&modplayerdata,&modinterfacedata,&modversiondata)) {
    SErrSetLastError(SNET_ERROR_INVALID_PARAMETER);
    FAILOUT_APILOCK_WRITE;
  }

  // UNREGISTER ALL EXISTING EVENT HANDLERS
  SEvtUnregisterType(REGISTERTYPE,REGISTERSUBTYPE_SNETEVENT);
  SEvtUnregisterType(REGISTERTYPE,REGISTERSUBTYPE_SYSEVENT);

  // REGISTER DEFAULT EVENT HANDLERS
#define  REGISTER(a,b)  SEvtRegisterHandler(REGISTERTYPE,             \
                                            REGISTERSUBTYPE_SYSEVENT, \
                                            (a),                      \
                                            0,                        \
                                            (SEVTHANDLER)(b))
  REGISTER(SYS_CIRCUITCHECK          ,SysOnCircuitCheck);
  REGISTER(SYS_DROPPLAYER            ,SysOnDropPlayer);
  REGISTER(SYS_NEWGAMEOWNER          ,SysOnNewGameOwner);
  REGISTER(SYS_PING                  ,SysOnPing);
  REGISTER(SYS_PINGRESPONSE          ,SysOnPingResponse);
  REGISTER(SYS_PLAYERINFO            ,SysOnPlayerInfo);
  REGISTER(SYS_PLAYERJOIN            ,SysOnPlayerJoin);
  REGISTER(SYS_PLAYERJOIN_ACCEPTSTART,SysOnPlayerJoinAcceptStart);
  REGISTER(SYS_PLAYERJOIN_ACCEPTDONE ,SysOnPlayerJoinAcceptDone);
  REGISTER(SYS_PLAYERJOIN_REJECT     ,SysOnPlayerJoinReject);
  REGISTER(SYS_PLAYERLEAVE           ,SysOnPlayerLeave);
#undef  REGISTER

  // INITIALIZE THE RECEIVING THREAD
  HANDLE event = (HANDLE)0;
  if (!RecvInitialize(&event)) {
    SErrSetLastError(SNET_ERROR_MAX_THRDS_REACHED);
    FAILOUT_APILOCK_WRITE;
  }

  // INITIALIZE THE PROVIDER
  if (!SpiInitialize(providerid,
                     &modprogramdata,
                     &modplayerdata,
                     &modinterfacedata,
                     &modversiondata,
                     event)) {
    // EITHER SPIINITIALIZE() OR THE SERVICE PROVIDER IS RESPONSIBLE
    // FOR CALLING SETLASTERROR()
    FAILOUT_APILOCK_WRITE;
  }

  // SAVE THE CATEGORY OPTIMIZATION HINT
  s_game_optcategorybits = modprogramdata.optcategorybits;

  LEAVE_APILOCK_WRITE;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetJoinGame (DWORD   gameid,
                            LPCSTR  gamename,
                            LPCSTR  gamepassword,
                            LPCSTR  playername,
                            LPCSTR  playerdescription,
                            DWORD  *playerid) {
  if (playerid)
    *playerid = SNET_INVALIDPLAYERID+s_api_playeroffset;

  VALIDATEBEGIN;
  VALIDATE(playerid);
  VALIDATEEND;

  ENTER_APILOCK_WRITE;
  TRACEOUT(TRACEHANDLE,
           "SNetJoinGame(0x%08x,\"%s\",\"%s\",\"%s\",\"%s\",*playerid)",
           gameid,gamename,gamepassword,playername,playerdescription);

  // VALIDATE PARAMETERS
  if (!s_spi) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_WRITE;
  }
  if (!gamename)
    gamename = "";
  if (!gamepassword)
    gamepassword = "";
  if (!playername)
    playername = "";
  if (!playerdescription)
    playerdescription = "";

  // IF NO PLAYER NAME WAS PROVIDED, AND THE CURRENT NETWORK PROVIDER
  // IS CAPABLE OF PROVIDING IT, THEN GET THE NAME OF THE LOGGED ON
  // USER FROM THE NETWORK PROVIDER
  char localplayername[SNETSPI_MAXSTRINGLENGTH] = "";
  char localplayerdesc[SNETSPI_MAXSTRINGLENGTH] = "";
  if ((!*playername) && s_spi->GetLocalPlayerName) {
    s_spi->GetLocalPlayerName(localplayername,
                              SNETSPI_MAXSTRINGLENGTH,
                              localplayerdesc,
                              SNETSPI_MAXSTRINGLENGTH);
    playername        = localplayername;
    playerdescription = localplayerdesc;
  }
  if (!*playername) {
    SErrSetLastError(SNET_ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  // IF WE ARE ALREADY IN A GAME, LEAVE IT
  if (s_game_playerid != NOPLAYER)
    SNetLeaveGame(SNET_EXIT_AUTO_NEWGAME);

  // RESET THE CONNECTION TABLE
  ConnDestroy();

  // GET THE ADDRESS OF THE GAME OWNER
  SNETSPI_GAMELIST gameinfo;
  TRACEOUT(TRACEHANDLE,
           "  spiGetGameInfo(0x%08x,\"%s\",\"%s\",*gameinfo)",
           gameid,gamename,gamepassword);
  if (!s_spi->GetGameInfo(gameid,gamename,gamepassword,&gameinfo)) {
    // SERVICE PROVIDER IS RESPONSIBLE FOR CALLING SETLASTERROR()
    FAILOUT_APILOCK_WRITE;
  }

  // CREATE A CONNECTION RECORD FOR THE GAME OWNER AND SEND HIM AN
  // INITIAL CONTACT MESSAGE
  s_game_joining = TRUE;
  CONNPTR ownerconn = ConnFindByAddr(&gameinfo.owner);
  if (!ownerconn) {
    s_game_joining = FALSE;
    TRACEOUT(TRACEHANDLE,"  out of memory");
    SNetLeaveGame(SNET_EXIT_AUTO_JOINING);
    SErrSetLastError(SNET_ERROR_NOT_ENOUGH_MEMORY);
    FAILOUT_APILOCK_WRITE;
  }
  ownerconn->establishing = TRUE;
  ownerconn->incomingsequence[TYPE_SYSTEM]  = 1;
  ownerconn->availablesequence[TYPE_SYSTEM] = 1;
  ownerconn->outgoingsequence[TYPE_SYSTEM]  = 1;
  {
    DWORD      netversion = SNET_NETWORKVERSION;
    MESSAGEPTR msg = ConnSendMessage(ownerconn,
                                     TYPE_SYSTEM,
                                     SYS_INITIALCONTACT,
                                     &netversion,
                                     sizeof(DWORD));
    if (msg) {
      ConnResendMessage(ownerconn,msg->data,msg->databytes);
      ConnResendMessage(ownerconn,msg->data,msg->databytes);
    }
    ConnProcessAck(ownerconn,TYPE_SYSTEM,1);
  }

  // WAIT FOR THE CIRCUIT CHECK MESSAGE FROM THE GAME OWNER
  {
    DWORD events[1] = {SYS_CIRCUITCHECK};
    if (!SysWaitForMultipleEvents(1,events,0,s_spi_timetoblock)) {
      s_game_joining = FALSE;
      TRACEOUT(TRACEHANDLE,"  host unreachable");
      SNetLeaveGame(SNET_EXIT_AUTO_JOINING);
      SErrSetLastError(SNET_ERROR_HOST_UNREACHABLE);
      FAILOUT_APILOCK_WRITE;
    }
  }

  // SEND A REQUEST TO JOIN
  {
    SYSEVENTDATA_PLAYERJOIN data;
    LPSTR currptr = data.namedescpass;
    currptr += SStrCopy(currptr,playername,SNETSPI_MAXSTRINGLENGTH)+1;
    currptr += SStrCopy(currptr,playerdescription,SNETSPI_MAXSTRINGLENGTH)+1;
    currptr += SStrCopy(currptr,gamepassword,SNETSPI_MAXSTRINGLENGTH)+1;
    ConnSendMessage(ConnFindByAddr(&gameinfo.owner),
                    TYPE_SYSTEM,
                    SYS_PLAYERJOIN,
                    &data,
                    (LPBYTE)currptr-(LPBYTE)&data);
  }

  // WAIT FOR AN ACCEPT OR REJECT MESSAGE FROM THE OWNER
  {
    DWORD events[2] = {SYS_PLAYERJOIN_ACCEPTDONE,
                       SYS_PLAYERJOIN_REJECT};
    if (!SysWaitForMultipleEvents(2,events,0,s_spi_timetoblock)) {
      ownerconn->establishing = FALSE;
      s_game_joining       = FALSE;
      TRACEOUT(TRACEHANDLE,"  host unreachable");
      SNetLeaveGame(SNET_EXIT_AUTO_JOINING);
      SErrSetLastError(SNET_ERROR_HOST_UNREACHABLE);
      FAILOUT_APILOCK_WRITE;
    }
  }

  // IF WE WERE REJECTED, RETURN FAILURE
  if (s_game_playerid == NOPLAYER) {
    ownerconn->establishing = FALSE;
    s_game_joining       = FALSE;
    TRACEOUT(TRACEHANDLE,"  rejected");
    SNetLeaveGame(SNET_EXIT_AUTO_JOINING);
    SErrSetLastError(SNET_ERROR_GAME_FULL);
    FAILOUT_APILOCK_WRITE;
  }

  // OTHERWISE, SAVE THE GAME CATEGORY
  s_game_categorybits = gameinfo.gamecategorybits;

  // SET OUR PLAYER INFORMATION
  CONNPTR conn = ConnFindLocal();
  if (conn) {
    conn->playerid = s_game_playerid;
    SStrCopy(conn->name,playername,SNETSPI_MAXSTRINGLENGTH);
    SStrCopy(conn->desc,playerdescription,SNETSPI_MAXSTRINGLENGTH);
  }
  *playerid = s_game_playerid+s_api_playeroffset;
  TRACEOUT(TRACEHANDLE,"  joined as player %u",*playerid);

  // DISPATCH ANY PENDING EVENTS
  LEAVE_APILOCK_WRITE;
  SysDispatchUserEvents();
  ENTER_APILOCK_WRITE;

  // COMPLETE THE JOIN PROCESS
  ownerconn->establishing = FALSE;
  s_game_joining          = FALSE;

  // MAKE SURE WE ARE STILL IN A GAME
  if (s_game_playerid == NOPLAYER) {
    SErrSetLastError(SNET_ERROR_GAME_TERMINATED);
    FAILOUT_APILOCK_WRITE;
  }

  LEAVE_APILOCK_WRITE;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetLeaveGame (DWORD exitcode) {
  ENTER_APILOCK_WRITE;
  TRACEOUT(TRACEHANDLE,
           "SNetLeaveGame(0x%08x)",
           exitcode);

  // VALIDATE PARAMETERS
  if (!s_spi) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_WRITE;
  }

  // FIND OUR PLAYER RECORD
  CONNPTR conn = ConnFindLocal();
  if (!conn) {
    SErrSetLastError(SNET_ERROR_NOT_ENOUGH_MEMORY);
    FAILOUT_APILOCK_WRITE;
  }

  // IF WE ARE THE GAME OWNER, STOP ADVERTISING THE GAME
  if (conn->gameowner) {
    TRACEOUT(TRACEHANDLE,"  spiStopAdvertisingGame()");
    s_spi->StopAdvertisingGame();
  }

  // SEND MESSAGES TO ALL OTHER PLAYERS INFORMING THEM THAT WE'RE DROPPING
  // OUT OF THE GAME
  {
    CONNPTR checkconn = s_conn_connlist.Head();
    while (checkconn) {
      if (checkconn->playerid != NOPLAYER) {
        SYSEVENTDATA_PLAYERLEAVE data;
        data.finalsequence = checkconn->outgoingsequence[TYPE_TURN];
        data.exitcode      = exitcode;
        MESSAGEPTR msg = ConnSendMessage(checkconn,
                                         TYPE_SYSTEM,
                                         SYS_PLAYERLEAVE,
                                         &data,
                                         sizeof(SYSEVENTDATA_PLAYERLEAVE));
        if (msg) {
          ConnResendMessage(checkconn,msg->data,msg->databytes);
          ConnResendMessage(checkconn,msg->data,msg->databytes);
        }
      }
      checkconn = checkconn->Next();
    }
  }

  // DESTROY THE GAME LOCALLY
  GameDestroy();

  // LEAVE THE API LOCK
  LEAVE_APILOCK_WRITE;

  // DELAY A FRACTION OF A SECOND TO GIVE THE BACKGROUND THREAD TIME TO
  // FINISH ANY PACKETS IT'S PROCESSING
  Sleep(s_spi_timetoresend);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetPerformUpgrade (DWORD *upgradestatus) {
  VALIDATEBEGIN;
  VALIDATE(upgradestatus);
  VALIDATEEND;

  ENTER_APILOCK_WRITE;
  TRACEOUT(TRACEHANDLE,"SNetPerformUpgrade(*upgradestatus)");

  // VERIFY THAT THERE IS AT LEAST ONE PATCH READY TO BE INSTALLED
  char buffer[1024] = "";
  if (!SRegLoadData("Patch","Patches",SREG_FLAG_BATTLENET,buffer,1023,NULL)) {
    *upgradestatus = SNET_UPGRADE_NOT_NEEDED;
    FAILOUT_APILOCK_WRITE;
  }
  buffer[1022] = 0;
  buffer[1023] = 0;

  // DETERMINE THE PROGRAM DIRECTORY
  char directory[MAX_PATH] = "";
  GetModuleFileName((HMODULE)0,directory,MAX_PATH);
  directory[MAX_PATH-1] = 0;
  {
    LPSTR curr = directory;
    while (*curr && SStrChr(curr+1,'\\'))
      curr = SStrChr(curr+1,'\\');
    *curr = 0;
  }

  // CREATE A LIST OF FILES TO EXECUTE
  char  execute[1024] = "";
  LPSTR currexec      = execute;

  // PREPROCESS EACH PATCH
  {
    LPSTR curr = buffer;
    while (*curr) {

      // OPEN THE PATCH
      HSARCHIVE archive;
      if (!SFileOpenArchive(curr,0,0,&archive)) {
        *upgradestatus = (DWORD)SNET_UPGRADE_FAILED;
        FAILOUT_APILOCK_WRITE;
      }

      // VERIFY ITS AUTHENTICITY
      {
        DWORD authtype;
        SFileAuthenticateArchive(archive,&authtype);
        if ((authtype != SFILE_AUTH_UNABLETOAUTHENTICATE) &&
            (authtype  < SFILE_AUTH_FIRSTAUTHENTIC)) {
          SFileCloseArchive(archive);
          *upgradestatus = (DWORD)SNET_UPGRADE_FAILED;
          FAILOUT_APILOCK_WRITE;
        }
      }

      // LOOK FOR A FILE CALLED 'PREPATCH.LST' INSIDE THE PATCH FILE
      {
        HSFILE listfile;
        if (SFileOpenFileEx(archive,"Prepatch.lst",0,&listfile)) {

          // IF WE FOUND ONE, READ IT INTO MEMORY
          DWORD bytes  = SFileGetFileSize(listfile);
          LPSTR buffer = (LPSTR)ALLOC(bytes);
          if (SFileReadFile(listfile,buffer,bytes,NULL,NULL)) {
            LPCSTR curr = buffer;
            while ((DWORD)(curr-buffer) < bytes) {

              // EXTRACT THE NEXT LINE
              char line[256] = "";
              {
                LPSTR currout = line;
                while (((DWORD)(curr-buffer) < bytes) &&
                       (currout-line < 255) &&
                       *curr && (*curr != '\r') && (*curr != '\n')) {
                  *currout++ = *curr++;
                  *currout   = 0;
                }
              }
              while (((DWORD)(curr-buffer) < bytes) &&
                     ((*curr == '\r') || (*curr == '\n')))
                ++curr;

              // IF THIS LINE HAS A FILENAME ARGUMENT, TURN IT INTO A FULLY
              // QUALIFIED FILENAME
              char relpath[MAX_PATH] = "";
              char abspath[MAX_PATH] = "";
              if (SStrChr(line,' ')) {
                LPCSTR currline = SStrChr(line,' ')+1;
                while (*currline == ' ')
                  ++currline;
                SStrCopy(relpath,currline,MAX_PATH);
                wsprintf(abspath,"%s\\%s",directory,currline);
              }

              // IF THIS LINE CONTAINS A DELETE COMMAND, DELETE THE FILE
              if (!_strnicmp(line,"delete ",7))
                DeleteFile(abspath);

              // IF THIS LINE CONTAINS AN EXTRACT COMMAND, EXTRACT THE FILE
              if (!_strnicmp(line,"extract ",8)) {
                HSFILE extractfile;
                if (SFileOpenFileEx(archive,relpath,0,&extractfile)) {
                  DWORD  extractbytes  = SFileGetFileSize(extractfile);
                  LPVOID extractbuffer = ALLOC(extractbytes);
                  if (SFileReadFile(extractfile,extractbuffer,extractbytes,NULL,NULL)) {
                    HANDLE outfile = CreateFile(abspath,
                                                GENERIC_WRITE,
                                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                                NULL,
                                                CREATE_ALWAYS,
                                                FILE_ATTRIBUTE_NORMAL,
                                                NULL);
                    if (outfile != INVALID_HANDLE_VALUE) {
                      DWORD byteswritten;
                      WriteFile(outfile,extractbuffer,extractbytes,&byteswritten,NULL);
                      CloseHandle(outfile);
                    }
                  }
                  FREE(extractbuffer);
                  SFileCloseFile(extractfile);
                }
              }

              // IF THIS LINE CONTAINS AN EXECUTE COMMAND, ADD THE PROGRAM
              // TO OUR LIST OF PROGRAMS TO EXECUTE
              if (!_strnicmp(line,"execute ",8))
                currexec += SStrCopy(currexec,abspath)+1;

            }

          }
          FREE(buffer);

          SFileCloseFile(listfile);
        }
      }

      // CLOSE THE PATCH
      SFileCloseArchive(archive);

      curr += SStrLen(curr)+1;
    }
  }

  // IF THE EXECUTE LIST IS BLANK, ADD 'BNUPDATE'
  if (currexec == execute) {
    wsprintf(currexec,"%s\\bnupdate.exe",directory);
    currexec += SStrLen(currexec)+1;
  }
  *currexec = 0;

  // RUN ALL OF THE PROGRAMS ON THE EXECUTE LIST
  currexec = execute;
  while (*currexec) {
    STARTUPINFO         startupinfo;
    PROCESS_INFORMATION processinfo;
    ZeroMemory(&startupinfo,sizeof(STARTUPINFO));
    startupinfo.cb = sizeof(STARTUPINFO);
    if (CreateProcess(currexec,
                      NULL,
                      NULL,
                      NULL,
                      0,
                      NORMAL_PRIORITY_CLASS,
                      NULL,
                      directory,
                      &startupinfo,
                      &processinfo)) {
      CloseHandle(processinfo.hThread);
      CloseHandle(processinfo.hProcess);
    }
    else {
      *upgradestatus = (DWORD)SNET_UPGRADE_FAILED;
      FAILOUT_APILOCK_WRITE;
    }
    currexec += SStrLen(currexec)+1;
  }

  LEAVE_APILOCK_WRITE;
  *upgradestatus = SNET_UPGRADING_TERMINATE;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetReceiveMessage (DWORD  *senderplayerid,
                                  LPVOID *data,
                                  DWORD  *databytes) {
  if (senderplayerid)
    *senderplayerid = SNET_INVALIDPLAYERID+s_api_playeroffset;
  if (data)
    *data = NULL;
  if (databytes)
    *databytes = 0;

  VALIDATEBEGIN;
  VALIDATE(data);
  VALIDATE(databytes);
  VALIDATEEND;

  ENTER_APILOCK_WRITE;
  TRACEPEND(TRACEHANDLE,"SNetReceiveMessage(*senderplayerid,*data,*databytes)");

  // VALIDATE PARAMETERS
  if (!s_spi) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_WRITE;
  }
  if (s_game_playerid == NOPLAYER) {
    SErrSetLastError(SNET_ERROR_NOT_IN_GAME);
    FAILOUT_APILOCK_WRITE;
  }

  // CHECK FOR TRANSFER OF GAME OWNERSHIP
  if (s_game_playerid != NOPLAYER)
    GameProcessLeavingPlayers();

  // DISPATCH ANY PENDING EVENTS
  LEAVE_APILOCK_WRITE;
  SysDispatchUserEvents();
  ENTER_APILOCK_WRITE;

  // MAKE SURE WE ARE STILL IN A GAME
  if (s_game_playerid == NOPLAYER) {
    SErrSetLastError(SNET_ERROR_GAME_TERMINATED);
    FAILOUT_APILOCK_WRITE;
  }

  // FIND A VALID MESSAGE THAT IS THE NEXT IN SEQUENCE FROM ITS SENDER.
  // IF THERE IS MORE THAN ONE MESSAGE AVAILABLE, USE THE ONE THAT WAS
  // RECEIVED EARLIEST.
  {
    DWORD   currtime     = GetTickCount();
    DWORD   earliesttime = 0;
    CONNPTR earliestconn = NULL;
    for (int local = FALSE; local <= TRUE; ++local) {
      CONNPTR conn = local ? ConnFindLocal()
                           : s_conn_connlist.Head();
      while (conn) {
        if ((conn->playerid != NOPLAYER) &&
            (!conn->incomingqueue[TYPE_MESSAGE].IsEmpty()) &&
            (conn->incomingqueue[TYPE_MESSAGE].Head()->data->header.sequence
               == conn->incomingsequence[TYPE_MESSAGE])) {

          // DETERMINE THE TIME STAMP OF THIS MESSAGE
          MESSAGEPTR curr      = conn->incomingqueue[TYPE_MESSAGE].Head();
          DWORD      timestamp = curr->sendtime;

          // IF THIS MESSAGE'S TIME STAMP IS GREATER THAN ANY MESSAGES
          // THAT WERE SENT LATER BY THE SAME PLAYER, UPDATE IT TO BE
          // EARLIER
          while (curr->Next()) {
            curr = curr->Next();
            if (timestamp-curr->sendtime < 0x7FFFFFFF)
              timestamp = curr->sendtime-1;
          }

          // IF THIS MESSAGE IS NOW THE EARLIEST WE HAVE SEEN, MARK THIS
          // CONNECTION AS THE NEW EARLIEST CONNECTION
          if (currtime-timestamp >= earliesttime) {
            earliesttime = currtime-timestamp;
            earliestconn = conn;
          }

        }
        conn = conn->Next();
      }
    }

    // RETURN THE NEXT MESSAGE IN SEQUENCE FROM THE EARLIEST CONNECTION
    // TO THE USER
    if (earliestconn) {
      MESSAGEPTR message = earliestconn->incomingqueue[TYPE_MESSAGE].Head();
      earliestconn->incomingqueue[TYPE_MESSAGE].UnlinkNode(message);
      ++(earliestconn->incomingsequence[TYPE_MESSAGE]);
      ConnSetCurrentMessage(earliestconn,TYPE_MESSAGE,message);
      if (senderplayerid)
        *senderplayerid = earliestconn->playerid+s_api_playeroffset;
      *data      = message->data->data;
      *databytes = message->data->header.bytes-sizeof(HEADER);
      PerfIncrement(SNET_PERFID_MSGRECV);
      TRACEDUMPMSG(TRACEHANDLE,
                   earliestconn->playerid,
                   message->data->header.sequence,
                   message->data->data,
                   message->data->header.bytes-sizeof(HEADER));
      LEAVE_APILOCK_WRITE;
      return TRUE;
    }

  }

  LEAVE_APILOCK_WRITE;
  SErrSetLastError(SNET_ERROR_NO_MESSAGES_WAITING);
  return FALSE;
}

//===========================================================================
BOOL APIENTRY SNetReceiveTurns (DWORD    firstplayerid,
                                DWORD    arraysize,
                                LPVOID  *arraydata,
                                LPDWORD  arraydatabytes,
                                LPDWORD  arrayplayerstatus) {
  if (arraysize && arraydata)
    ZeroMemory(arraydata,arraysize*sizeof(LPVOID));
  if (arraysize && arraydatabytes)
    ZeroMemory(arraydatabytes,arraysize*sizeof(DWORD));
  if (arraysize && arrayplayerstatus)
    ZeroMemory(arrayplayerstatus,arraysize*sizeof(DWORD));

  VALIDATEBEGIN;
  VALIDATE(arraysize);
  VALIDATE(arraydata);
  VALIDATE(arraydatabytes);
  VALIDATE(arrayplayerstatus);
  VALIDATEEND;

  ENTER_APILOCK_WRITE;
  TRACEOUT(TRACEHANDLE,
           "SNetReceiveTurns(%u,%u,*arraydata,*arraydatabytes,*arrayplayerstatus)",
           firstplayerid,arraysize);

  // VALIDATE PARAMETERS
  if (!s_spi) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_WRITE;
  }
  if (s_game_playerid == NOPLAYER) {
    SErrSetLastError(SNET_ERROR_NOT_IN_GAME);
    FAILOUT_APILOCK_WRITE;
  }
  firstplayerid -= s_api_playeroffset;

  // CHECK FOR TRANSFER OF GAME OWNERSHIP
  if (s_game_playerid != NOPLAYER)
    GameProcessLeavingPlayers();

  // DETERMINE THE SEQUENCE NUMBER TO RECEIVE
  CONNPTR local = ConnFindLocal();
  if (!local) {
    SErrSetLastError(SNET_ERROR_NOT_IN_GAME);
    FAILOUT_APILOCK_WRITE;
  }
  WORD sequence = local->incomingsequence[TYPE_TURN];

  // CHECK ALL PLAYERS THAT ARE IN A JOINING STATE, MEANING THAT WE HAVEN'T
  // CAUGHT UP TO THEIR FIRST EVER TURN, TO SEE IF THEY ARE READY TO SWITCH
  // OVER TO ACTIVE STATE
  {
    CONNPTR conn = s_conn_connlist.Head();
    while (conn) {
      if ((conn->flags & PF_JOINING) &&
          (conn->incomingsequence[TYPE_TURN] == sequence)) {
        conn->flags &= ~PF_JOINING;
        SysQueueUserEvent(SNET_EVENT_PLAYERJOIN,
                          conn->playerid,
                          NULL,
                          0);
      }
      conn = conn->Next();
    }
  }

  // DISPATCH ANY PENDING EVENTS
  LEAVE_APILOCK_WRITE;
  SysDispatchUserEvents();
  ENTER_APILOCK_WRITE;

  // MAKE SURE WE ARE STILL IN A GAME
  if (s_game_playerid == NOPLAYER) {
    SErrSetLastError(SNET_ERROR_GAME_TERMINATED);
    FAILOUT_APILOCK_WRITE;
  }

  // CLEAR OUT ANY UNNEEDED OR ALREADY PROCESSED TURNS
  ConnClearOldTurns(NULL);

  // FILL IN THE PLAYER STATUS ARRAY
  DWORD currtime = GetTickCount();
  BOOL  ready    = TRUE;
#ifdef TRACING
  char outstr[256];
  wsprintf(outstr,"%u  ",sequence);
#endif
  {
    for (DWORD playerid = firstplayerid; playerid < firstplayerid+arraysize; ++playerid) {

      // FIND THE CONNECTION RECORD FOR THIS PLAYER
      CONNPTR conn;
      if (playerid == s_game_playerid)
        conn = local;
      else
        conn = ConnFindByPlayerId((BYTE)playerid);

      // IF THE PLAYER IS ACTIVE AND HAS COMPLETED JOINING THE GAME, FILL
      // IN HIS STATUS FLAGS
      DWORD statusflags = 0;
      if (conn && !(conn->flags & PF_JOINING)) {
        statusflags |= SNET_PSF_ACTIVE;
        if ((!conn->incomingqueue[TYPE_TURN].IsEmpty()) &&
            (conn->incomingqueue[TYPE_TURN].Head()->data->header.sequence == sequence))
          statusflags |= SNET_PSF_TURNAVAILABLE;
        else
          ready = FALSE;
        if ((playerid == s_game_playerid) ||
            (currtime-conn->lastreceivetime < s_spi_timetogiveup) ||
            ((conn->flags & PF_LEAVING) && (statusflags & SNET_PSF_TURNAVAILABLE)))
          statusflags |= SNET_PSF_RESPONDING;

        // IF THIS IS A TRACING BUILD, LOG EACH PLAYER'S STATUS
#ifdef TRACING
        char buffer[16];
        wsprintf(buffer,"%u?%u ",playerid,conn->incomingsequence[TYPE_TURN]);
        if (statusflags & SNET_PSF_TURNAVAILABLE)
          buffer[1] = '=';
        else if (statusflags & SNET_PSF_RESPONDING)
          buffer[1] = 'W';
        else
          buffer[1] = 'X';
        SStrPack(outstr,buffer,256);
#endif

      }
      *(arrayplayerstatus+playerid-firstplayerid) = statusflags;
    }
  }
#ifdef TRACING
  TRACEOUT(TRACEHANDLE,outstr);
#endif

  // MAINTAIN ACTIVE CONNECTIONS
  {
    for (DWORD playerid = firstplayerid; playerid < firstplayerid+arraysize; ++playerid)
      if (*(arrayplayerstatus+playerid-firstplayerid) & SNET_PSF_ACTIVE) {

        // FIND THE PLAYER RECORD
        CONNPTR conn = NULL;
        if (playerid != s_game_playerid)
          conn = ConnFindByPlayerId((BYTE)playerid);
        if (!conn)
          continue;

        // IF WE ARE STILL WAITING FOR THIS PLAYER'S TURN AND MORE THAN THE
        // RESEND TIME HAS ELAPSED, REQUEST A RESEND OF THIS TURN FROM ALL
        // PLAYERS
        if ((!((*(arrayplayerstatus+playerid-firstplayerid)) & SNET_PSF_TURNAVAILABLE)) &&
            ((!conn->lastrequesttime) ||
             ((currtime-conn->lastrequesttime >= s_spi_timetoresend) &&
              (currtime-conn->lastrequesttime <= 0x7FFFFFFF)))) {
          PACKETPTR pkt = (PACKETPTR)ALLOC(sizeof(PACKET)+1);
          conn->lastrequesttime = currtime;
          CONNPTR otherconn = s_conn_connlist.Head();
          while (otherconn) {
            if ((otherconn->playerid != NOPLAYER) &&
                ((conn->playerid != NOPLAYER) || (otherconn == conn))) {
              TRACEOUT(TRACEHANDLE,
                       "  requesting resend: type=%u sequence=%04x player=%x",
                       TYPE_TURN,conn->incomingsequence[TYPE_TURN],conn->playerid);
              pkt->header.checksum    = 0;
              pkt->header.bytes       = sizeof(HEADER);
              pkt->header.sequence    = conn->incomingsequence[TYPE_TURN];
              pkt->header.acksequence = otherconn->availablesequence[TYPE_TURN];
              pkt->header.type        = TYPE_TURN;
              pkt->header.subtype     = 0;
              pkt->header.playerid    = local->playerid;
              pkt->header.flags       = MF_RESENDREQUEST;
              if (otherconn != conn) {
                pkt->header.bytes++;
                pkt->data[0] = conn->playerid;
              }
              pkt->header.checksum = PktGenerateChecksum(pkt);
              ConnSendPacket(otherconn,pkt);
            }
            otherconn = otherconn->Next();
          }
          FREE(pkt);
        }

        // IF WE'RE IN DANGER OF GIVING UP ON THIS PLAYER, SEND A PING
        // REQUEST TO SEE IF HE'S STILL ALIVE.  (OPTIMIZATION: IF THIS
        // PLAYER ALREADY HAS UNACKNOWLEDGED SYSTEM MESSAGES THEN HE'S
        // NOT GOING TO RESPOND TO A PING UNTIL HE PROCESSES THOSE, SO
        // DON'T BOTHER TO SEND ONE.)
        if ((conn->outgoingqueue[TYPE_SYSTEM].IsEmpty()) &&
            (currtime-conn->lastreceivetime >= s_spi_timetogiveup/2) &&
            (currtime-conn->lastpingtime    >= s_spi_timetogiveup/2)) {
          conn->lastpingtime = currtime;
          ConnSendMessage(conn,TYPE_SYSTEM,SYS_PING,NULL,0);
        }

      }
  }

  // EXIT IF WE DON'T HAVE TURN DATA FOR ALL PLAYERS
  if (!ready) {
    SErrSetLastError(SNET_ERROR_NO_MESSAGES_WAITING);
    LEAVE_APILOCK_WRITE;
    return FALSE;
  }

  // FILL IN THE TURN DATA ARRAYS
  {
    for (DWORD playerid = firstplayerid; playerid < firstplayerid+arraysize; ++playerid) {
      CONNPTR conn;
      if (playerid == s_game_playerid)
        conn = local;
      else
        conn = ConnFindByPlayerId((BYTE)playerid);
      if (conn &&
          (*(arrayplayerstatus+playerid-firstplayerid) & SNET_PSF_TURNAVAILABLE)) {
        MESSAGEPTR message = conn->incomingqueue[TYPE_TURN].Head();
        conn->incomingqueue[TYPE_TURN].UnlinkNode(message);
        conn->incomingsequence[TYPE_TURN] = (WORD)(sequence+1);
        ConnSetCurrentMessage(conn,TYPE_TURN,message);
        *(arraydata     +playerid-firstplayerid) = message->data->data;
        *(arraydatabytes+playerid-firstplayerid) = message->data->header.bytes-sizeof(HEADER);
        TRACEDUMPMSG(TRACEHANDLE,
                     conn->playerid,
                     message->data->header.sequence,
                     message->data->data,
                     message->data->header.bytes-sizeof(HEADER));
      }
      else {
        *(arraydata     +playerid-firstplayerid) = NULL;
        *(arraydatabytes+playerid-firstplayerid) = 0;
      }
    }
  }

  // INCREMENT THE SEQUENCE NUMBER
  local->incomingsequence[TYPE_TURN] = (WORD)(sequence+1);
  PerfSet(SNET_PERFID_TURN,local->incomingsequence[TYPE_TURN]);
  PerfIncrement(SNET_PERFID_TURNSRECV);

  LEAVE_APILOCK_WRITE;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetRegisterEventHandler (DWORD         eventid,
                                        SNETEVENTPROC callback) {
  VALIDATEBEGIN;
  VALIDATE(callback);
  VALIDATEEND;

  ENTER_APILOCK_WRITE;
  TRACEOUT(TRACEHANDLE,
           "SNetRegisterEventHandler(%u,0x%08x)",
           eventid,callback);

  // REGISTER THE EVENT HANDLER
  BOOL success = SEvtRegisterHandler(REGISTERTYPE,
                                     REGISTERSUBTYPE_SNETEVENT,
                                     eventid,
                                     0,
                                     (SEVTHANDLER)callback);

  LEAVE_APILOCK_WRITE;
  if (!success)
    SErrSetLastError(SNET_ERROR_NOT_ENOUGH_MEMORY);
  return success;
}

//===========================================================================
BOOL APIENTRY SNetResetLatencyMeasurements () {
  ENTER_APILOCK_WRITE;
  TRACEOUT(TRACEHANDLE,
           "SNetResetLatencyMeasurements()");

  CONNPTR conn = s_conn_connlist.Head();
  while (conn) {
    conn->latency      = 0;
    conn->peaklatency  = 0;
    conn->lastpingtime = 0;
    conn = conn->Next();
  }

  LEAVE_APILOCK_WRITE;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetSelectGame (DWORD               flags,
                              SNETPROGRAMDATAPTR  programdata,
                              SNETPLAYERDATAPTR   playerdata,
                              SNETUIDATAPTR       interfacedata,
                              SNETVERSIONDATAPTR  versiondata,
                              DWORD              *playerid) {
  ENTER_APILOCK_WRITE;
  TRACEOUT(TRACEHANDLE,
           "SNetSelectGame(0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,*playerid)",
           flags,programdata,playerdata,interfacedata,versiondata);
  TRACEDUMPDATABLOCKS(TRACEHANDLE,
                      programdata,playerdata,interfacedata,versiondata);

  // VALIDATE PARAMETERS
  if (playerid)
    *playerid = 0;
  if (!s_spi) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_WRITE;
  }
  SNETPROGRAMDATA modprogramdata;
  SNETPLAYERDATA  modplayerdata;
  SNETUIDATA      modinterfacedata;
  SNETVERSIONDATA modversiondata;
  if (!SpiNormalizeDataBlocks(programdata,playerdata,interfacedata,versiondata,
                              &modprogramdata,&modplayerdata,&modinterfacedata,&modversiondata)) {
    SErrSetLastError(SNET_ERROR_INVALID_PARAMETER);
    FAILOUT_APILOCK_WRITE;
  }

  // SAVE A POINTER TO THE SELECT GAME FUNCTION AND LEAVE THE API LOCK
  BOOL (CALLBACK *selectfunc)(DWORD,SNETPROGRAMDATAPTR,SNETPLAYERDATAPTR,SNETUIDATAPTR,SNETVERSIONDATAPTR,DWORD *);
  selectfunc = s_spi->SelectGame;
  LEAVE_APILOCK_WRITE;

  // CALL THE SELECT GAME FUNCTION
  TRACEOUT(TRACEHANDLE,
           "  spiSelectGame(0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,*playerid)",
           flags,&modprogramdata,&modplayerdata,&modinterfacedata,&modversiondata);
  BOOL success = selectfunc(flags,
                            &modprogramdata,
                            &modplayerdata,
                            &modinterfacedata,
                            &modversiondata,
                            playerid);

  TRACEOUT(TRACEHANDLE,"  spiSelectGame() returns %u",success);
  return success;
}

//===========================================================================
BOOL APIENTRY SNetSelectProvider (SNETCAPSPTR         mincaps,
                                  SNETPROGRAMDATAPTR  programdata,
                                  SNETPLAYERDATAPTR   playerdata,
                                  SNETUIDATAPTR       interfacedata,
                                  SNETVERSIONDATAPTR  versiondata,
                                  DWORD              *providerid) {
  ENTER_APILOCK_WRITE;
  TRACEOUT(TRACEHANDLE,
           "SNetSelectProvider(0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,*providerid)",
           mincaps,programdata,playerdata,interfacedata,versiondata);
  TRACEDUMPDATABLOCKS(TRACEHANDLE,
                      programdata,playerdata,interfacedata,versiondata);

  // VALIDATE PARAMETERS
  if (providerid)
    *providerid = 0;
  SNETPROGRAMDATA modprogramdata;
  SNETPLAYERDATA  modplayerdata;
  SNETUIDATA      modinterfacedata;
  SNETVERSIONDATA modversiondata;
  if (!SpiNormalizeDataBlocks(programdata,playerdata,interfacedata,versiondata,
                              &modprogramdata,&modplayerdata,&modinterfacedata,&modversiondata)) {
    SErrSetLastError(SNET_ERROR_INVALID_PARAMETER);
    FAILOUT_APILOCK_WRITE;
  }

  // BUILD A LIST OF PROVIDERS IF WE DON'T ALREADY HAVE ONE
  SpiFindAllProviders();

  // BUILD A USER INTERFACE PARAMETERS BLOCK
  UIPARAMS uiparams;
  ZeroMemory(&uiparams,sizeof(UIPARAMS));
  uiparams.mincaps       = mincaps;
  uiparams.programdata   = &modprogramdata;
  uiparams.playerdata    = &modplayerdata;
  uiparams.interfacedata = &modinterfacedata;
  uiparams.versiondata   = &modversiondata;

  // DISPLAY THE DIALOG BOX
  DWORD result = (DWORD)SDlgDialogBoxParam(StormGetInstance(),
                                           "SELECTPROVIDER_DIALOG",
                                           SDrawGetFrameWindow(),
                                           UiSelectProviderDialogProc,
                                           (LPARAM)&uiparams);
  if (providerid)
    *providerid = result;

  LEAVE_APILOCK_WRITE;
  return (result && (result != 0xFFFFFFFF));
}

//===========================================================================
BOOL APIENTRY SNetSendMessage (DWORD  targetplayerid,
                               LPVOID data,
                               DWORD  databytes) {
  VALIDATEBEGIN;
  VALIDATE(data);
  VALIDATE(databytes);
  VALIDATEEND;

  ENTER_APILOCK_WRITE;
  TRACEOUT(TRACEHANDLE,
           "SNetSendMessage(%u,0x%08x,%u)",
           targetplayerid,data,databytes);

  // VALIDATE PARAMETERS
  if (!s_spi) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_WRITE;
  }
  if (s_game_playerid == NOPLAYER) {
    SErrSetLastError(SNET_ERROR_NOT_IN_GAME);
    FAILOUT_APILOCK_WRITE;
  }
  if ((targetplayerid != SNET_BROADCASTNONLOCALPLAYERID) &&
      (targetplayerid != SNET_BROADCASTPLAYERID))
    targetplayerid -= s_api_playeroffset;

  // IF THIS IS A BROADCAST, SEND IT TO ALL PLAYERS IN THE GAME,
  // INCLUDING THE LOCAL PLAYER
  if ((targetplayerid == SNET_BROADCASTNONLOCALPLAYERID) ||
      (targetplayerid == SNET_BROADCASTPLAYERID)) {
    CONNPTR conn = s_conn_connlist.Head();
    while (conn) {
      if (conn->playerid != NOPLAYER) {
        TRACEDUMPMSG(TRACEHANDLE,
                     0xFFFFFFFF,
                     conn->outgoingsequence[TYPE_MESSAGE],
                     data,
                     databytes);
        ConnSendMessage(conn,
                        TYPE_MESSAGE,
                        0,
                        data,
                        databytes);
      }
      conn = conn->Next();
    }
    if (targetplayerid == SNET_BROADCASTPLAYERID) {
      conn = ConnFindLocal();
      if (conn) {
        TRACEDUMPMSG(TRACEHANDLE,
                     0xFFFFFFFF,
                     conn->outgoingsequence[TYPE_MESSAGE],
                     data,
                     databytes);
        ConnSendMessage(conn,
                        TYPE_MESSAGE,
                        0,
                        data,
                        databytes);
      }
    }
  }

  // OTHERWISE, SEND IT TO JUST THE REQUESTED PLAYER
  else {
    CONNPTR conn = ConnFindByPlayerId(targetplayerid);
    if (conn) {
      TRACEDUMPMSG(TRACEHANDLE,
                   0xFFFFFFFF,
                   conn->outgoingsequence[TYPE_MESSAGE],
                   data,
                   databytes);
      ConnSendMessage(conn,
                      TYPE_MESSAGE,
                      0,
                      data,
                      databytes);
    }
    else {
      SErrSetLastError(SNET_ERROR_INVALID_PLAYER);
      FAILOUT_APILOCK_WRITE;
    }
  }

  PerfIncrement(SNET_PERFID_MSGSENT);
  LEAVE_APILOCK_WRITE;

  // TRIGGER THE BACKGROUND RECEIVE THREAD SO IT WILL MAINTAIN CONNECTIONS
  SetEvent(s_recv_event);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetSendServerChatCommand (LPCSTR command) {
  VALIDATEBEGIN;
  VALIDATE(command);
  VALIDATE(*command);
  VALIDATEEND;

  ENTER_APILOCK_WRITE;
  TRACEOUT(TRACEHANDLE,
           "SNetSetServerChatCommand(0x%08x)",
           command);

  // VALIDATE PARAMETERS
  if (!s_spi) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_WRITE;
  }
  if (s_game_playerid == NOPLAYER) {
    SErrSetLastError(SNET_ERROR_NOT_IN_GAME);
    FAILOUT_APILOCK_WRITE;
  }
  CONNPTR local = ConnFindLocal();
  if (!local) {
    SErrSetLastError(SNET_ERROR_NOT_IN_GAME);
    FAILOUT_APILOCK_WRITE;
  }

  // SEND THE MESSAGE TO THE SERVER
  char senderpath[SNETSPI_MAXSTRINGLENGTH+16];
  wsprintf(senderpath,"\\\\.\\game\\%s",s_game_gamename);
  BOOL result = s_spi->SendExternalMessage(senderpath,
                                           local->name,
                                           "",
                                           "",
                                           command);

  LEAVE_APILOCK_WRITE;
  return result;
}

//===========================================================================
BOOL APIENTRY SNetSendTurn (LPVOID data,
                            DWORD  databytes) {
  VALIDATEBEGIN;
  VALIDATE(data);
  VALIDATE(databytes);
  VALIDATEEND;

  ENTER_APILOCK_WRITE;
  TRACEOUT(TRACEHANDLE,
           "SNetSendTurn(0x%08x,%u)",
           data,databytes);

  // VALIDATE PARAMETERS
  if (!s_spi) {
    SErrSetLastError(SNET_ERROR_BAD_PROVIDER);
    FAILOUT_APILOCK_WRITE;
  }
  if (s_game_playerid == NOPLAYER) {
    SErrSetLastError(SNET_ERROR_NOT_IN_GAME);
    FAILOUT_APILOCK_WRITE;
  }
  CONNPTR local = ConnFindLocal();
  if (!local) {
    SErrSetLastError(SNET_ERROR_NOT_IN_GAME);
    FAILOUT_APILOCK_WRITE;
  }

  // SEND THE TURN TO ALL PLAYERS IN THE GAME, INCLUDING THE LOCAL PLAYER
  {
    CONNPTR conn = s_conn_connlist.Head();
    while (conn) {
      if ((conn->playerid != NOPLAYER) &&
          (conn->outgoingsequence[TYPE_TURN] == local->outgoingsequence[TYPE_TURN])) {
        TRACEDUMPMSG(TRACEHANDLE,
                     0xFFFFFFFF,
                     conn->outgoingsequence[TYPE_TURN],
                     data,
                     databytes);
        ConnSendMessage(conn,
                        TYPE_TURN,
                        0,
                        data,
                        databytes);
      }
      conn = conn->Next();
    }
    TRACEDUMPMSG(TRACEHANDLE,
                 0xFFFFFFFF,
                 local->outgoingsequence[TYPE_TURN],
                 data,
                 databytes);
    ConnSendMessage(local,
                    TYPE_TURN,
                    0,
                    data,
                    databytes);
  }

  PerfIncrement(SNET_PERFID_TURNSSENT);
  LEAVE_APILOCK_WRITE;

  // TRIGGER THE BACKGROUND RECEIVE THREAD SO IT WILL MAINTAIN CONNECTIONS
  SetEvent(s_recv_event);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetSetBasePlayer (DWORD playerid) {
  ENTER_APILOCK_WRITE;
  TRACEOUT(TRACEHANDLE,
           "SNetSetBasePlayer(%u)",
           playerid);

  s_api_playeroffset = playerid;

  LEAVE_APILOCK_WRITE;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetSetGameMode (DWORD modeflags) {
  ENTER_APILOCK_WRITE;
  TRACEOUT(TRACEHANDLE,
           "SNetSetGameMode(0x%08x)",
           modeflags);

  // VALIDATE PARAMETERS
  if (!s_spi) {
    SErrSetLastError(SNET_ERROR_INVALID_PARAMETER);
    FAILOUT_APILOCK_WRITE;
  }
  if (s_game_playerid == NOPLAYER) {
    SErrSetLastError(SNET_ERROR_NOT_IN_GAME);
    FAILOUT_APILOCK_WRITE;
  }

  // IF WE ARE NOT THE GAME OWNER, RETURN FAILURE
  {
    CONNPTR local = ConnFindLocal();
    if (!(local && local->gameowner)) {
      SErrSetLastError(SNET_ERROR_NOT_OWNER);
      FAILOUT_APILOCK_WRITE;
    }
  }

  // SET THE NEW GAME MODE
  s_game_gamemode = modeflags;

  // BUILD THE CLIENT DATA BLOCK
  BYTE  clientdata[SNETSPI_MAXCLIENTDATA];
  DWORD clientdatabytes;
  GameBuildClientData(clientdata,
                      &clientdatabytes);

  // START ADVERTISING WITH THE NEW MODE
  DWORD gameage = (GetTickCount()-s_game_creationtime)/1000;
  TRACEOUT(TRACEHANDLE,
           "  spiStartAdvertisingGame(\"%s\",\"%s\",\"%s\",%u,%u,0x%08x,0x%08x,0x%08x,%u)",
           s_game_gamename,
           s_game_gamepass,
           s_game_gamedesc,
           s_game_gamemode,
           gameage,
           s_game_categorybits,
           s_game_optcategorybits,
           clientdata,
           clientdatabytes);
  if (!s_spi->StartAdvertisingGame(s_game_gamename,
                                   s_game_gamepass,
                                   s_game_gamedesc,
                                   s_game_gamemode,
                                   gameage,
                                   s_game_categorybits,
                                   s_game_optcategorybits,
                                   clientdata,
                                   clientdatabytes))
    FAILOUT_APILOCK_WRITE;

  // note: broadcast the new mode to other players

  LEAVE_APILOCK_WRITE;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SNetUnregisterEventHandler (DWORD         eventid,
                                          SNETEVENTPROC callback) {
  VALIDATEBEGIN;
  VALIDATE(callback);
  VALIDATEEND;

  ENTER_APILOCK_WRITE;
  TRACEOUT(TRACEHANDLE,
           "SNetUnregisterEventHandler(%u,0x%08x)",
           eventid,callback);

  // UNREGISTER THE EVENT HANDLER
  BOOL success = SEvtUnregisterHandler(REGISTERTYPE,
                                       REGISTERSUBTYPE_SNETEVENT,
                                       eventid,
                                       (SEVTHANDLER)callback);

  LEAVE_APILOCK_WRITE;
  if (!success)
    SErrSetLastError(SNET_ERROR_NOT_REGISTERED);
  return success;
}

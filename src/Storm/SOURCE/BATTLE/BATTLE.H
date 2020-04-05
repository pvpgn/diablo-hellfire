/****************************************************************************
*
*  COMMON DEFINES, TYPES AND STRUCTURES
*
***/

#define  DATAPORT         6112
#define  MAXMESSAGESIZE   512
#define  MAXPLAYERS       256
#define  MAXSTRINGLENGTH  SNETSPI_MAXSTRINGLENGTH
#define  PROVIDERID       'BNET'

#define  STARCRAFT_BNBETA 1     // 0 in final

#define  PKT_GAMEDATA     0
#define  PKT_CLIENTREQ    3
#define  PKT_SERVERPING   5

/****************************************************************************
*
*  GLOBAL VARIABLES
*
***/

extern HINSTANCE global_hinstance;
extern DWORD     global_maxplayers;
extern DWORD     global_programid;
extern DWORD     global_versionid;

extern BOOL      gbConnectionLost;
		
/****************************************************************************
*
*  CACHE FUNCTIONS
*
***/

void CacheDestroy ();
BOOL CacheFree (LPVOID data, DWORD databytes);
BOOL CacheInitialize ();
BOOL CacheLoadFile (LPCSTR    filename,
                    FILETIME *lastwritetime,
                    LPVOID   *data,
                    DWORD    *databytes);
BOOL CacheSaveFile (LPCSTR    filename,
                    LPVOID    data,
                    DWORD     databytes,
                    FILETIME *lastwritetime,
                    DWORD     minexpireseconds,
                    DWORD     maxexpireseconds);

/****************************************************************************
*
*  SERVICE PROVIDER INTERFACE FUNCTIONS
*
***/

void SpiAddGame (SNETSPI_GAMELISTPTR game);
BOOL CALLBACK SpiCompareNetAddresses (SNETADDRPTR  addr1,
                                      SNETADDRPTR  addr2,
                                      DWORD       *diffmagnitude);
BOOL CALLBACK SpiDestroy ();
BOOL CALLBACK SpiFree (SNETADDRPTR addr,
                       LPVOID      data,
                       DWORD       databytes);
BOOL CALLBACK SpiFreeExternalMessage (LPCSTR senderpath,
                                      LPCSTR sendername,
                                      LPCSTR message);
BOOL CALLBACK SpiGetGameInfo (DWORD             gameid,
                              LPCSTR            gamename,
                              LPCSTR            gamepassword,
                              SNETSPI_GAMELIST *gameinfo);
BOOL CALLBACK SpiGetLocalPlayerName (LPSTR namebuffer,
                                     DWORD namechars,
                                     LPSTR descbuffer,
                                     DWORD descchars);
BOOL CALLBACK SpiGetPerformanceData (DWORD          counterid,
                                     DWORD         *countervalue,
                                     LARGE_INTEGER *measurementtime,
                                     LARGE_INTEGER *measurementfreq);
BOOL CALLBACK SpiInitialize (SNETPROGRAMDATAPTR programdata,
                             SNETPLAYERDATAPTR  playerdata,
                             SNETUIDATAPTR      interfacedata,
                             SNETVERSIONDATAPTR versiondata,
                             HANDLE             event);
BOOL CALLBACK SpiInitializeDevice (DWORD              deviceid,
                                   SNETPROGRAMDATAPTR programdata,
                                   SNETPLAYERDATAPTR  playerdata,
                                   SNETUIDATAPTR      interfacedata,
                                   SNETVERSIONDATAPTR versiondata);
BOOL CALLBACK SpiLockDeviceList (SNETSPI_DEVICELISTPTR *devicelist);
BOOL CALLBACK SpiLockGameList (DWORD                categorybits,
                               DWORD                categorymask,
                               SNETSPI_GAMELISTPTR *gamelist);
void SpiQueueExternalMessage (LPCSTR senderpath,
                              LPCSTR sendername,
                              LPCSTR message);
BOOL CALLBACK SpiReceive (SNETADDRPTR *addr,
                          LPVOID      *data,
                          DWORD       *databytes);
BOOL CALLBACK SpiReceiveExternalMessage (LPCSTR *senderpath,
                                         LPCSTR *sendername,
                                         LPCSTR *message);
BOOL CALLBACK SpiSelectGame (DWORD               flags,
                             SNETPROGRAMDATAPTR  programdata,
                             SNETPLAYERDATAPTR   playerdata,
                             SNETUIDATAPTR       interfacedata,
                             SNETVERSIONDATAPTR  versiondata,
                             DWORD              *playerid);
BOOL CALLBACK SpiSend (DWORD        addresses,
                       SNETADDRPTR *addrlist,
                       LPVOID       data,
                       DWORD        databytes);
BOOL CALLBACK SpiSendExternalMessage (LPCSTR senderpath,
                                      LPCSTR sendername,
                                      LPCSTR targetpath,
                                      LPCSTR targetname,
                                      LPCSTR message);
BOOL SpiSendSpecial (SNETADDRPTR addr,
                     DWORD       packettype,
                     LPVOID      data,
                     DWORD       databytes);
BOOL CALLBACK SpiStartAdvertisingGame (LPCSTR  gamename,
                                       LPCSTR  gamepassword,
                                       LPCSTR  gamedescription,
                                       DWORD   gamemode,
                                       DWORD   gameage,
                                       DWORD   gamecategorybits,
                                       DWORD   optcategorybits,
                                       LPCVOID clientdata,
                                       DWORD   clientdatabytes);
BOOL CALLBACK SpiStopAdvertisingGame ();
BOOL CALLBACK SpiUnlockDeviceList (SNETSPI_DEVICELISTPTR devicelist);
BOOL CALLBACK SpiUnlockGameList (SNETSPI_GAMELISTPTR  gamelist,
                                 DWORD               *hintnextcall);

/****************************************************************************
*
*  SERVER COMMUNICATION FUNCTIONS
*
***/

#define  SN_ADDCHANNEL             1   // param = LPCSTR (channel name)
#define  SN_DELETECHANNEL          2   // param = LPCSTR (channel name)
#define  SN_JOINCHANNEL            3   // param = SNJOINCHANNELPTR
#define  SN_ADDUSER                4   // param = SNADDUSERPTR
#define  SN_DELETEUSER             5   // param = SNDELETEUSERPTR
#define  SN_DISPLAYSTRING          6   // param = SNDISPLAYSTRINGPTR
#define  SN_DOWNLOADINGUPGRADE     8   // param = LPDWORD (percent complete)
#define  SN_DOWNLOADFAILED         9   // param = unused
#define  SN_DOWNLOADSUCCEEDED      10  // param = unused
#define  SN_FAILEDTOCONNECT        11  // param = LPDWORD (error code)
#define  SN_USERNAME               12  // param = LPCSTR (user name)
#define  SN_LOSTCONNECTION         13  // param = unused
#define  SN_CHANGEUSERFLAGS        14  // param = SNCHANGEUSERFLAGSPTR
#define  SN_UPDATEPINGTIME         15  // param = SNUPDATEPINGTIMEPTR
#define  SN_CHANNELISFULL          16  // param = LPCSTR (channel name)
#define  SN_CHANNELDOESNOTEXIST    17  // param = LPCSTR (channel name)
#define  SN_CHANNELISRESTRICTED    18  // param = LPCSTR (channel name)
#define  SN_SQUELCHUSER            19  // param = SNSQUELCHUSERPTR
#define  SN_UNSQUELCHUSER          20  // param = SNSQUELCHUSERPTR
#define  SN_BADCONNECTION          21  // param = unused
#define  SN_SETADINFO              22  // param = SNADINFOPTR
#define  SN_DISPLAYAD              23  // param = LPVOID (ad data)
#define  SN_MESSAGEBOX             24  // param = SNMESSAGEBOXPTR

#define  SN_ERROR_UNREACHABLE      1
#define  SN_ERROR_NOTRESPONDING    2
#define  SN_ERROR_UNABLETOUPGRADE  3
#define  SN_ERROR_BADCONNECTION    4

#define  SN_STRING_WHISPER         1
#define  SN_STRING_WHISPERSENT     2
#define  SN_STRING_TALK            3
#define  SN_STRING_BROADCAST       4
#define  SN_STRING_INFORMATION     5
#define  SN_STRING_ERROR           7

#define  ADTYPE_PCX                0x7863702e
#define  ADTYPE_SMK                0x6b6d732e

#define  CF_PUBLIC                 0x00000001
#define  CF_MODERATED              0x00000002
#define  CF_RESTRICTED             0x00000004

#define  UF_BLIZZARD               SNET_DDPF_BLIZZARD
#define  UF_MODERATOR              SNET_DDPF_MODERATOR
#define  UF_SPEAKER                SNET_DDPF_SPEAKER
#define  UF_SYSOP                  SNET_DDPF_SYSOP
#define  UF_BADCONNECTION          0x00000010
#define  UF_SQUELCHED              SNET_DDPF_SQUELCHED

typedef struct _SNADDUSERREC {
    LPCSTR name;
    LPCSTR description;
    DWORD  flags;
    BOOL   notifyuser;
} SNADDUSERREC, *SNADDUSERPTR;

typedef struct _SNADINFOREC {
    DWORD  id;
    DWORD  adtype;
    LPCSTR filename;
    LPCSTR url;
} SNADINFOREC, *SNADINFOPTR;

typedef struct _SNCHANGEUSERFLAGSREC {
    LPCSTR name;
    DWORD  flags;
} SNCHANGEUSERFLAGSREC, *SNCHANGEUSERFLAGSPTR;

typedef struct _SNDELETEUSERREC {
    LPCSTR name;
    LPCSTR reason;
    BOOL   notifyuser;
} SNDELETEUSERREC, *SNDELETEUSERPTR;

typedef struct _SNDISPLAYSTRINGREC {
    LPCSTR sender;
    DWORD  senderflags;
    DWORD  stringtype;
    LPCSTR string;
} SNDISPLAYSTRINGREC, *SNDISPLAYSTRINGPTR;

typedef struct _SNJOINCHANNELREC {
    LPCSTR name;
    DWORD  flags;
} SNJOINCHANNELREC, *SNJOINCHANNELPTR;

typedef struct _SNMESSAGEBOXREC {
    LPCSTR text;
    LPCSTR caption;
    DWORD  type;
} SNMESSAGEBOXREC, *SNMESSAGEBOXPTR;

typedef struct _SNSQUELCHUSERREC {
    LPCSTR name;
    DWORD  flags;
} SNSQUELCHUSERREC, *SNSQUELCHUSERPTR;

typedef struct _SNUPDATEPINGTIMEREC {
    LPCSTR name;
    DWORD  pingtime;
    BOOL   relayed;
} SNUPDATEPINGTIMEREC, *SNUPDATEPINGTIMEPTR;

BOOL SrvBeginChat (SNETPROGRAMDATAPTR programdata,
                   SNETPLAYERDATAPTR  playerdata,
                   LPCSTR             preferredchannel);
void SrvCancel ();
BOOL SrvCreateAccount (LPCSTR username,
                       LPCSTR password);
void SrvDestroy ();
BOOL SrvDisconnect ();
BOOL SrvEndChat ();
BOOL SrvGetGameList (LPCSTR gamename,
                     LPCSTR gamepassword,
                     DWORD  categorybits,
                     DWORD  categorymask,
                     DWORD  maxitems);
BOOL SrvGetLatency (SNETADDRPTR  addr,
                    DWORD       *latency);
void SrvGetLocalPlayerDesc (LPSTR buffer,
                            DWORD bufferchars);
void SrvGetLocalPlayerName (LPSTR buffer,
                            DWORD bufferchars);
BOOL SrvGetUiNotification (DWORD  *notifycode,
                           LPVOID *param,
                           DWORD  *parambytes);
BOOL SrvInitialize (SNETPROGRAMDATAPTR programdata,
                    SNETPLAYERDATAPTR  playerdata,
                    SNETVERSIONDATAPTR versiondata);
BOOL SrvIsConnected ();
BOOL SrvIsWaitingForResponse ();
BOOL SrvJoinChannel (LPCSTR channel,
                     BOOL   joinalways = 0);
BOOL SrvLogon (LPCSTR  username,
               LPCSTR  password,
               DWORD  *errorcode);
void SrvMaintainAds ();
void SrvMaintainLatencies ();
void SrvNotifyClickAd (DWORD adid,
                       BOOL  result);
void SrvNotifyDisplayAd (DWORD  id,
                         LPCSTR filename,
                         LPCSTR url);
void SrvNotifyJoin (LPCSTR gamename,
                    LPCSTR gamepassword);
void SrvPingAddress (SNETADDRPTR addr);
void SrvProcessClientReq (SNETADDRPTR addr,
                          LPBYTE      data,
                          DWORD       databytes);
void SrvProcessServerPing (LPBYTE data,
                           DWORD  databytes);
BOOL SrvSendChatString (LPCSTR command);
void SrvSetBetaId (DWORD betaid);
BOOL SrvStartAdvertisingGame (LPCSTR gamename,
                              LPCSTR gamepassword,
                              LPCSTR gamedescription,
                              DWORD  gamemode,
                              DWORD  creationtime,
                              DWORD  gamecategorybits,
                              DWORD  optcategorybits);
BOOL SrvStopAdvertisingGame ();

/****************************************************************************
*
*  USER INTERFACE FUNCTIONS
*
***/

//****************************************************************************
// User Defined messages
//****************************************************************************

// chatchnnl.cpp
#define WM_CHANNEL_DOESNOTEXIST		(WM_USER+100)
#define WM_CHANNEL_JOINED			(WM_USER+101)
#define WM_CHANNEL_RESTRICTED		(WM_USER+102)

// bnetlogo.cpp
#define WM_ERR_NOTRESPONDING		(WM_USER+103)

// chatroom.cpp
#define WM_CHAT_PROCESS_MSG			(WM_USER+104)		//(wparam = msg color, lparam = (LPCSTR)szText)

// ui.cpp..etc
#define WM_NOTIFICATION_WAITING		(WM_USER+105)

// scrollbar.cpp  
#define WM_STORMSCROLL_INIT			(WM_USER+106)		// (lparam = (HWND)hWndParent)

//****************************************************************************
//****************************************************************************

typedef struct _UIPARAMS {
    DWORD              flags;
    SNETPROGRAMDATAPTR programdata;
    SNETPLAYERDATAPTR  playerdata;
    SNETUIDATAPTR      interfacedata;
    SNETVERSIONDATAPTR versiondata;
    LPDWORD            playeridptr;
} UIPARAMS, *UIPARAMSPTR;


#define MAX_CHANNEL_LEN 32
typedef struct _CHANNEL_LIST {
	char szChannel[MAX_CHANNEL_LEN];
    _CHANNEL_LIST  *next;
} TCHANNEL_LIST, *PTCHANNEL_LIST;

typedef struct _bmp {
	LPBYTE data;
	SIZE datasize;
} TBMP, * TPBMP;


BOOL CALLBACK BattleNetDialogProc(HWND   window,
								  UINT   message,
								  WPARAM wparam,
								  LPARAM lparam);

BOOL UiInitialize(SNETUIDATAPTR interfacedata);
void UiDestroy(void);
BOOL UiBeginConnect (SNETPROGRAMDATAPTR programdata,
                     SNETUIDATAPTR      interfacedata);
void UiEndConnect (BOOL connected);

void UiRestoreApp(void);

BOOL UiSelectGame (DWORD               flags,
                   SNETPROGRAMDATAPTR  programdata,
                   SNETPLAYERDATAPTR   playerdata,
                   SNETUIDATAPTR       interfacedata,
                   SNETVERSIONDATAPTR  versiondata,
                   DWORD              *playerid);
BOOL UiLogon (SNETPROGRAMDATAPTR  programdata,
              SNETPLAYERDATAPTR   playerdata,
              SNETUIDATAPTR       interfacedata,
              SNETVERSIONDATAPTR  versiondata);
BOOL UiDisconnect(void);
BOOL UiAskUpdate(SNETUIDATAPTR interfacedata);
void UiNotificationWaiting ();
BOOL UiPrepRestart(SNETUIDATAPTR interfacedata);
BOOL UiProcessWindowMessages ();
BOOL UiUpgradeMessage ();
void UiNotification(void);		
void UiWSockErrMessage(void);

BOOL UiMessageBox(SNETMESSAGEBOXPROC messageboxcallback, HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);



typedef int (CALLBACK *PROGRESSFCN)(void);
HWND UiModelessProgressDialog(SNETUIDATAPTR interfacedata,
							   LPCSTR		progresstext,
							   BOOL			abortable,
							   PROGRESSFCN	progressfcn,
							   DWORD		callspersec);


void ColorPrefDestroy(void);
BOOL ColorPrefInit(SNETGETDATAPROC getdatacallback);
void ColorPrefActivate(BOOL bActivate);


//===========================================================================
// Useful misc. routines
//===========================================================================
BOOL UiLoadArtwork (SNETGETARTPROC  artcallback,
                    HWND            hWnd,
                    HWND            hWndParent,
                    DWORD           artid,
                    LPCTSTR         controltype,
                    DWORD           controlstyle,
                    LONG            usageflags,
					BOOL			loadpalette,
                    BOOL            prepfadein,
                    LPBYTE         *data,
                    SIZE           *size);


BOOL UiLoadCustomArt (
					SNETGETARTPROC  artcallback,
                    HWND            hWnd,
                    DWORD           artid,
					int				nFirstColor,
					int				nNumColorsUsed,
					BOOL			bSetPaletteNow,
                    LPBYTE         *data,
                    SIZE           *size);

BOOL UiGetData(SNETGETDATAPROC	getdatacallback, 
			   DWORD			dataid,
			   LPBYTE			*data,
			   DWORD			*datasize);

//* Fading routines
#define DEFAULT_STEPS		5
#define FADE_TIMER_ID		10
#define FADE_TIMER_DELAY	50

extern int gFadeStep;
void UiVidFade(int max, int curr);
void UiVidFadeOut(int steps);

void UiLoadCursors(HWND hWnd, SNETUIDATAPTR interfacedata);

// Scrollbar routines
void ScrollbarRegisterClass (void);
void ScrollbarUnregisterClass(void);
void ScrollbarLink(HWND hWndList, HWND hWndScroll);
void ListUpdateScrollbar(HWND hWndList);
void EditUpdateScrollbar(HWND hWndEdit);
int  ScrollbarGetWidth(void);
void ScrollbarLoadArtwork(SNETGETARTPROC artcallback);
void ScrollbarDestroyArtwork(void);

// ComboBox routines
void ComboRegisterClass (void);
void ComboUnregisterClass (void);
void ComboboxLoadArtwork(SNETGETARTPROC artcallback);
void ComboboxDestroyArtwork(void);


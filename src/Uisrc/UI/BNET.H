/****************************************************************************
*
*  bnet.h
*  BattleNet Chat Room
*
*  By Michael Morhaime
*
***/


#define PROVIDERID			'BATL'
#define BATTLENET_MAXPLAYERS  256
typedef struct _UIPARAMS {
    DWORD              flags;
    SNETPROGRAMDATAPTR programdata;
    SNETPLAYERDATAPTR  playerdata;
    SNETUIDATAPTR      interfacedata;
    SNETVERSIONDATAPTR versiondata;
    LPDWORD            playeridptr;
} UIPARAMS, *UIPARAMSPTR;



BOOL DoLoadArtwork (SNETGETARTPROC artcallback,
						   HWND hWnd,
						   HWND hWndParent,
						   DWORD artid,
						   LPCTSTR controltype,
						   DWORD controlstyle,
						   LONG usageflags,
						   BOOL setpalette,
                           LPBYTE *data,
                           SIZE *size);

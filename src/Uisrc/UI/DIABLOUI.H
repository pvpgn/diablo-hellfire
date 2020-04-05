//***************************************************************************
// DiabloUI.h
// created 9.13.96
//***************************************************************************


//***************************************************************************
extern "C" void APIENTRY UiInitialize(void);
extern "C" void APIENTRY UiSetSpawned(BOOL bSpawned);
extern "C" void APIENTRY UiDestroy();
extern "C" void APIENTRY UiAppActivate(BOOL activating);


//***************************************************************************
extern "C" BOOL CALLBACK UiCreateGameCallback (SNETCREATEDATAPTR   createdata,
											   SNETPROGRAMDATAPTR  programdata,
											   SNETPLAYERDATAPTR   playerdata,
											   SNETUIDATAPTR       interfacedata,
											   SNETVERSIONDATAPTR  versiondata,
											   DWORD              *playerid);

extern "C" BOOL CALLBACK UiArtCallback (DWORD           providerid,
						                DWORD           artid,
										LPPALETTEENTRY  pe,
										LPBYTE          buffer,
										DWORD           buffersize,
										int            *width,
										int            *height,
										int            *bitdepth);

extern "C" BOOL CALLBACK UiSoundCallback(DWORD providerid, DWORD soundid, DWORD flags);

extern "C" BOOL CALLBACK UiDrawDescCallback (DWORD			providerid,
											DWORD			itemtype,
											LPCSTR			itemname,
											LPCSTR			itemdescription,
											DWORD			itemflags,
											DWORD			drawflags,
											DWORD			time,
											LPDRAWITEMSTRUCT lpdis);

extern "C" BOOL CALLBACK UiMessageBoxCallback(HWND hWnd, 
											  LPCTSTR lpText, 
											  LPCTSTR lpCaption, 
											  UINT uType);

extern "C" BOOL CALLBACK UiAuthCallback(DWORD dwItemType, LPCSTR szName, LPCSTR szDesc, DWORD dwUserFlags, LPCSTR szItem, LPSTR szErrorBuf, DWORD dwErrorBufSize);
extern "C" BOOL CALLBACK UiGetDataCallback(DWORD providerid, DWORD dataid, LPVOID buffer, DWORD buffersize, DWORD *bytesused);

extern "C" BOOL CALLBACK UiCategoryCallback(
							BOOL				userinitiated,
							SNETPROGRAMDATAPTR	programdata,
							SNETPLAYERDATAPTR	playerdata,
							SNETUIDATAPTR		interfacedata,
							SNETVERSIONDATAPTR	versiondata,
							DWORD *			categorybits,
							DWORD *			categorymask);

//***************************************************************************
enum _ui_classes {
	UI_WARRIOR = 0,
	UI_ROGUE,
	UI_SORCERER,
	UI_MONK,
	UI_BARD,
	UI_BARBARIAN,
	UI_NUM_CLASSES
};

//***************************************************************************
extern "C" BOOL APIENTRY UiTitleDialog (UINT timeoutseconds);
extern "C" BOOL APIENTRY UiBetaDisclaimer (UINT timeoutseconds);
extern "C" BOOL APIENTRY UiCreditsDialog (UINT pixelspersec);		
extern "C" BOOL APIENTRY UiSupportDialog (UINT pixelspersec);		

//***************************************************************************
enum _copyprot_results {
	COPYPROT_OK = 1,
	COPYPROT_CANCEL
};
extern "C" BOOL APIENTRY UiCopyProtError (DWORD *result);

//***************************************************************************
#define DEFAULT_ATTRACT_TIMEOUT		30
typedef void (CALLBACK *PLAYSND)(LPCSTR);
enum _mainmenu_selections {
	MAINMENU_SINGLE_PLAYER = 1,
	MAINMENU_MULTIPLAYER,
	MAINMENU_REPLAY_INTRO,
	MAINMENU_SUPPORT,
	MAINMENU_SHOW_CREDITS,
	MAINMENU_EXIT_DIABLO,
	MAINMENU_ATTRACT_MODE
};
extern "C" BOOL APIENTRY UiMainMenuDialog(LPCTSTR registration,
										  DWORD * selection,
										  bool	  allowMultiplayer,
										  PLAYSND sndfcn = NULL,
										  UINT    attracttimeoutseconds = DEFAULT_ATTRACT_TIMEOUT);

//***************************************************************************
#define MAX_GAME_LEN		32
#define MAX_PASSWORD_LEN	32

enum _difficulty { 
	DIFF_NORMAL,
	DIFF_NIGHTMARE,
	DIFF_HELL,
	NUM_DIFFICULTIES
}; 


typedef struct _gamedata TGAMEDATA;
struct _gamedata {
	DWORD dwSeed;
	BYTE bDiff;				// Use enum's from _difficulty settings
};


//***************************************************************************
//***************************************************************************
// all the functions and structures for selecting/creating/deleting heros
//***************************************************************************
//***************************************************************************
#define MAX_NAME_LEN	16			// including terminting char
#define MAX_CLASS_LEN	16			// including terminting char

typedef struct _uiheroinfo TUIHEROINFO;
typedef TUIHEROINFO *TPUIHEROINFO;

typedef struct _uidefaultstats {
	WORD strength;
	WORD magic;
	WORD dexterity;
	WORD vitality;
} TUIDEFSTATS, *TPUIDEFSTATS;

struct _uiheroinfo {
	TPUIHEROINFO next;
	char name[MAX_NAME_LEN];			// eg "Frasier"

	WORD level;
	BYTE heroclass;						// UI_WARRIOR, etc.
	BYTE herorank;						// # of times hero has killed Diablo (range = 0..NUM_DIFFICULTIES)

	WORD strength;
	WORD magic;
	WORD dexterity;
	WORD vitality;

	DWORD gold;
	BOOL hassaved;
	BOOL spawned;
};

//***************************************************************************

#define UI_DESC_MAXLENGTH 128	

// The following routines output a null terminated string to the provided preallocated
// pointers. The maximum length of the string is defined by the above constant.
extern "C" BOOL APIENTRY UiCreatePlayerDescription(TPUIHEROINFO pHeroInfo, DWORD dwProgramId, LPSTR pPlayerDesc);

// Call this routine to generate a query string used by Battle.net to sort games in the
// JoinGame list.  Returns the number of bytes written out to szQuery.
extern "C" int APIENTRY UiCreateGameCriteria(TPUIHEROINFO pHeroInfo, LPSTR pszQuery);

//***************************************************************************

typedef BOOL (CALLBACK *ENUMHEROPROC)(TPUIHEROINFO);
typedef BOOL (CALLBACK *ENUMHEROS)(ENUMHEROPROC);
typedef BOOL (CALLBACK *CREATEHERO)(TPUIHEROINFO);
typedef BOOL (CALLBACK *DELETEHERO)(TPUIHEROINFO);
typedef BOOL (CALLBACK *GETDEFHERO)(int, TPUIDEFSTATS);

enum _selhero_selections {
	SELHERO_NEW_DUNGEON = 1,
	SELHERO_CONTINUE,
	SELHERO_CONNECT,
	SELHERO_PREVIOUS
};
extern "C" BOOL APIENTRY UiSelHeroSingDialog(
												ENUMHEROS	enumfcn,
												CREATEHERO	createfcn,
												DELETEHERO	deletefcn,
												GETDEFHERO	getstatsfcn,
												DWORD		*selection,
												LPSTR		heroname,
												int			*difficulty,
												bool		allowBard,
												bool		allowBarbarian
											);
extern "C" BOOL APIENTRY UiSelHeroMultDialog(
												ENUMHEROS	enumfcn,
												CREATEHERO	createfcn,
												DELETEHERO	deletefcn,
												GETDEFHERO	getstatsfcn,
												DWORD		*selection,
												LPSTR		heroname,
												bool		allowBard,
												bool		allowBarbarian
											);

// if the default starting statistics for a character class change
// then Diablo.exe will have to provide this function
extern "C" BOOL CALLBACK UiGetDefaultStats(int heroclass, TPUIDEFSTATS defaultstats);

//***************************************************************************
extern "C" BOOL APIENTRY UiLogonDialog(HWND		parent,
									   DWORD	*selection,
									   LPSTR	logonname,
									   UINT		maxnamelen,
									   LPSTR	logonpassword,
									   UINT		maxpasswordlen);

//***************************************************************************
typedef int (CALLBACK *PROGRESSFCN)(void);
extern "C" BOOL APIENTRY UiProgressDialog(HWND			parent,
										  LPCSTR		progresstext,
										  BOOL			abortable,
										  PROGRESSFCN	progressfcn,
										  DWORD			callspersec);

//***************************************************************************
extern "C" void APIENTRY UiOnPaint (LPPARAMS params);
extern "C" BOOL APIENTRY UiSetBackgroundBitmap (HWND           window,
                                                LPPALETTEENTRY palette,
                                                LPBYTE         bitmapbits,
                                                int            width,
                                                int            height);

//***************************************************************************
//***************************************************************************
extern "C" BOOL APIENTRY UiSelectProvider (SNETCAPSPTR         mincaps,
										   SNETPROGRAMDATAPTR  programdata,
										   SNETPLAYERDATAPTR   playerdata,
										   SNETUIDATAPTR       interfacedata,
										   SNETVERSIONDATAPTR  versiondata,
										   DWORD              *providerid);
extern "C" BOOL APIENTRY UiSelectGame (DWORD               flags,
									   SNETPROGRAMDATAPTR  programdata,
									   SNETPLAYERDATAPTR   playerdata,
									   SNETUIDATAPTR       interfacedata,
									   SNETVERSIONDATAPTR  versiondata,
									   DWORD              *playerid);

	
//***************************************************************************
//***************************************************************************
	#define DEVNAME_LEN		SNETSPI_MAXSTRINGLENGTH
	#define DEVDESC_LEN		SNETSPI_MAXSTRINGLENGTH
	#define MAX_DIAL_LEN	32
	enum _dialmodes {
		MODE_ANSWER = (IDCANCEL + 1),
		MODE_DIALOLD,
		MODE_DIALNEW
	};
	typedef struct _modeminfo TMODEM;
	typedef TMODEM *TPMODEM;
	struct _modeminfo {
		TPMODEM	next;
		DWORD	deviceid;
		TCHAR	devicename[DEVNAME_LEN];
		TCHAR	devicedesc[DEVDESC_LEN];
	};

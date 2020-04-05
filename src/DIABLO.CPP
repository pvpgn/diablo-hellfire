/*-----------------------------------------------------------------------**
** 								Diablo
**
**							   Main file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/DIABLO.CPP 16    3/29/97 9:09p Pwyatt $
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma	hdrstop
#include	"storm.h"
#include	"sound.h"
#include	"mainmenu.h"
#include	"interfac.h"
#include	"inv.h"
#include	"msg.h"
#include	"multi.h"
#include	"engine.h"
#include	"palette.h"
#include	"scrollrt.h"
#include	"gendung.h"
#include	"setmaps.h"
#include	"debug.h"
#include	"effects.h"

#include	"dead.h"
#include	"lighting.h"
#include	"control.h"
#include	"gamemenu.h"

#include	"items.h"
#include	"player.h"
#include	"monster.h"
#include	"objects.h"
#include	"missiles.h"
#include	"trigs.h"
#include	"spelldat.h"
#include	"spells.h"
#include	"cursor.h"

#include	"town.h"
#include	"towners.h"
#include	"drlg_l1.h"
#include	"drlg_l2.h"
#include	"drlg_l3.h"
#include	"drlg_l4.h"
#include	"quests.h"
#include	"minitext.h"
#include	"themes.h"
#include	"stores.h"
#include	"packplr.h"
#include	"portal.h"
#include	"doom.h"

#include	"automap.h"
#include	"help.h"
#include	"error.h"
#include	"diabloui.h"
#include	"textdat.h"
// pjw.patch1.start
#include	"mpqapi.h"
#include	<stddef.h>
// pjw.patch1.end

/*-----------------------------------------------------------------------**
** Registration info
**-----------------------------------------------------------------------*/
#include "regconst.h"
char sgszTblSig[TBL_LEN] = "REGISTRATION_TABLE";


/*-----------------------------------------------------------------------**
** Global Variables
**-----------------------------------------------------------------------*/
extern char gszProgKey[];
BYTE gbDoEnding;
void DoEnding();

int StripPlayer(int pnum);

DWORD glSeedTbl[NUMLEVELS];
int gnLevelTypeTbl[NUMLEVELS];

// window vars
HWND ghMainWnd;
HINSTANCE ghInst;

// video vars
BOOL fullscreen = TRUE;


// mouse
int MouseX, MouseY;

// program vars
int force_redraw = NODRAW;

BOOL svgamode;

// Temp vars (delete all uses before final compile)
long gv1;
long gv2;
long gv3;
long gv4;
long gv5;

#ifndef NDEBUG
// do not change this flag -- USE 'n' on the command line
static BOOL cineflag = TRUE;
#endif

#if CHEATS
BOOL gbDumpDropLog = FALSE;
BOOL davedebug = FALSE;
BOOL cheatflag = FALSE;			// Turn invincibility and all spells on/off
BOOL simplecheat = FALSE;		// like cheatflag, but doesn't screw up game
BOOL gbNoDropInactive = FALSE;	// don't drop players on timeout
int	tstQMsgSpd;
int tstQMsgIndex = 0;
BOOL tstQMsgFlag = FALSE;
BOOL tstQMsgIndexFlag = FALSE;
BOOL itemcheat = FALSE;
BOOL uniqcheat = FALSE;
#endif

BOOL visiondebug = FALSE;	// Vision debugging
BOOL scrollflag = FALSE;	// Scroll when at edge of screen with mouse
BOOL light4flag = FALSE;	// 4 levels of light instead of 16
BOOL leveldebug = FALSE;
BOOL monstdebug = FALSE;
BOOL trigdebug = FALSE;
int setseed = 0;

int debugmonsttypes = 0;
int DebugMonsters[10];

BOOL PauseMode = FALSE;
BOOL FriendlyMode = TRUE;

BOOL gbRunGame;
BOOL gbRunGameResult;
BOOL gbProcessPlayers;
BOOL gbGameLoopStartup;

bool gbTheo = false;
bool gbCowsuit = false;
bool gbOurNest = false;
bool gbAllowBard = false;
bool gbAllowBarbarian = false;
bool gbAllowMultiPlayer = false;

//int glEndSeed[17];		// @@@ drb temp
//int glMid1Seed[17];
//int glMid2Seed[17];
//int glMid3Seed[17];

int glEndSeed[NUMLEVELS+1];		// @@@ drb temp
int glMid1Seed[NUMLEVELS+1];
int glMid2Seed[NUMLEVELS+1];
int glMid3Seed[NUMLEVELS+1];	//JKE 7/30 adds new levels

/*-----------------------------------------------------------------------**
** timeout cursor
**-----------------------------------------------------------------------*/
#define TIMEOUT_CURSOR	WATCH_CURS
static int sgnTimeoutCurs = NO_CURSOR;

#define LMOUSE_DOWN	1
#define RMOUSE_DOWN	2
// drb.patch1.start.1/24/97
// static BYTE sgbMouseDown = 0;
BYTE sgbMouseDown = 0;
// drb.patch1.end.1/24/97


/*-----------------------------------------------------------------------**
** Function stubs
**-----------------------------------------------------------------------*/
// pjw.patch1.start
// static void try_game_loop(BOOL bStartup);
static void alloc_plr();
static void DoTimedEvents();
static void game_loop(BOOL bStartup);
static void plr_encrypt(BOOL bEncrypt);
void InitializeHashSource();
void Decrypt(LPDWORD data, DWORD bytes, DWORD key);
void Encrypt(LPDWORD data, DWORD bytes, DWORD key);
// pjw.patch1.end


void BlackPalette();
void enable_frame_counter();
void init_window(int nCmdShow);
void play_movie(const char * pszMovie,BOOL bAllowCancel);
void play_quotes();
void play_quit();
void FreeCursor();
void InitDebugGFX();
void FreeDebugGFX();
void ShowProgress(UINT uMsg);
static LRESULT CALLBACK GM_Game(HWND, UINT, WPARAM, LPARAM);
void TrackInit(BOOL bMouseDown);
void TrackMouse();
void run_delta_info();
void toggle_frame_counter();
WNDPROC my_SetWindowProc(WNDPROC wndProc);
void plrmsg_update();
void screen_capture();
void SavePaletteSettings();
void menu_music();
void NetStartTimeout();
void menusnd_init();
void InitLevels();
void SyncInitPlrPos(int pnum);

void OpenCloseAllDoors();
void OpenNaKrul2();

//******************************************************************
//******************************************************************
void FlushMsgs() {
	// turn tracking off
	TrackInit(FALSE);
	sgbMouseDown = FALSE;
	ReleaseCapture();

	// keep flushing messages until all key and mouse msgs are gone
	BOOL bLoop = TRUE;
	while (bLoop) {
		bLoop = FALSE;
		MSG msg;
		while (PeekMessage(&msg,NULL,WM_KEYFIRST,WM_KEYLAST,PM_REMOVE))
			bLoop = TRUE;
		while (PeekMessage(&msg,NULL,WM_MOUSEFIRST,WM_MOUSELAST,PM_REMOVE))
			bLoop = TRUE;
	}
}

//******************************************************************
//******************************************************************
static void CommandLine(const char * s) {
	int val;
	(val);

	while (*s) {

		// skip over any space characters
		while (isspace(*s))
			s++;

		// check for direct draw emulation mode
// pjw.patch1.start.1/13/97
		static const char sszEmulate[] = "dd_emulate";
		if (! _strnicmp(sszEmulate,s,strlen(sszEmulate))) {
			extern BYTE gbUseDDEmulation;
			gbUseDDEmulation = TRUE;
			s += strlen(sszEmulate);
			continue;
		}

		// check for direct draw offscreen buffer
		static const char sszBackBuf[] = "dd_backbuf";
		if (! _strnicmp(sszBackBuf,s,strlen(sszBackBuf))) {
			extern BYTE gbForceBackBuf;
			gbForceBackBuf = TRUE;
			s += strlen(sszBackBuf);
			continue;
		}

		static const char sszDupSound[] = "ds_noduplicates";
		if (! _strnicmp(sszDupSound,s,strlen(sszDupSound))) {
			extern BYTE gbDupSounds;
			gbDupSounds = FALSE;
			s += strlen(sszDupSound);
			continue;
		}
		
		char const sszTheoFlag[] = "Theoquest";
		if (! _strnicmp(sszTheoFlag, s, strlen(sszTheoFlag))) {
			gbTheo = true;
			s += strlen(sszTheoFlag);
			continue;
		}
		
		char const sszCowQuestFlag[] = "Cowquest";
		if (!_strnicmp(sszCowQuestFlag, s, strlen(sszCowQuestFlag))) {
			gbCowsuit = true;
			s += strlen(sszCowQuestFlag);
			continue;
		}
		
		char const sszOurNestFlag[] = "NestArt";
		if (!_strnicmp(sszOurNestFlag, s, strlen(sszOurNestFlag))) {
			gbOurNest = true;
			s += strlen(sszOurNestFlag);
			continue;
		}
		
		char const sszAllowBardFlag[] = "Bardtest";
		if (!_strnicmp(sszAllowBardFlag, s, strlen(sszAllowBardFlag))) {
			gbAllowBard = true;
			s += strlen(sszAllowBardFlag);
			continue;
		}
		
		char sszAllowMultiPlayerFlag[] = "Multitest";
#if defined(_MULTITEST)
		sszAllowMultiPlayerFlag[5] = 'n';
		if (!_strnicmp(sszAllowMultiPlayerFlag, s, strlen(sszAllowMultiPlayerFlag))) {
#else
		if (!_strnicmp(sszAllowMultiPlayerFlag, sszAllowBardFlag, strlen(sszAllowMultiPlayerFlag))) {
#endif
			gbAllowMultiPlayer = true;
			s += strlen(sszAllowMultiPlayerFlag);
			continue;
		}
		
		char const sszAllowBarbarianFlag[] = "Barbariantest";
		if (!_strnicmp(sszAllowBarbarianFlag, s, strlen(sszAllowBarbarianFlag))) {
			gbAllowBarbarian = true;
			s += strlen(sszAllowBarbarianFlag);
			continue;
		}
		
// pjw.patch1.end.1/13/97

		// extract next character -- do not do ++ inside macro
		// some versions of C may have nasty side effects...
		char c = tolower(*s); s++;
		switch (c) {
			#if CHEATS
			case 'b':
				gbDumpDropLog = TRUE;
			break;
			#endif

			#if CHEATS
			case 'i':
				gbNoDropInactive = TRUE;
			break;
			#endif

			#if CHEATS
			// this cheat flag is for testing without having your
			// character die every two seconds -- doesn't have
			// all the nasty side effects of the cheat flag
			case '$':
				simplecheat = TRUE;
			break;
			#endif

			#if CHEATS
			// this cheat flag is the do-everything cheat for programmers
			// it has all sorts of nasty side effects which may hide bugs
			case '^':
				cheatflag = TRUE;
			break;
			#endif

			#if CHEATS
			case 'd':
				davedebug = TRUE;
				cineflag = FALSE;
			break;
			#endif

			#if CHEATS
			case 'w':
				davecheat = TRUE;
			break;
			#endif

			#if CHEATS && !IS_VERSION(SHAREWARE)
			case 'l':
				leveldebug = TRUE;
				setlevel = FALSE;

				// get level type
				while (isspace(*s))
					s++;
				val = 0;
				while (isdigit(*s)) {
					val *= 10;
					val += *s - '0';
					s++;
				}
				leveltype = val;

				// get level num
				while (isspace(*s))
					s++;
				val = 0;
				while (isdigit(*s)) {
					val *= 10;
					val += *s - '0';
					s++;
				}
				currlevel = val;
				plr[0].plrlevel = val;
			break;
			#endif

			#if CHEATS
			case 'm':
				monstdebug = TRUE;

				// get monster number
				while (isspace(*s))
					s++;
				val = 0;
				while (isdigit(*s)) {
					val *= 10;
					val += *s - '0';
					s++;
				}
				DebugMonsters[debugmonsttypes++] = val;
			break;
			#endif

			#if CHEATS
			case 'q':
				// get quest number
				while(isspace(*s))
					s++;
				val = 0;
				while (isdigit(*s)) {
					val *= 10;
					val += *s - '0';
					s++;
				}
				extern int questcheat;
				questcheat = val;
			break;
			#endif

			#if CHEATS
			case 'r':
				while(isspace(*s))
					s++;
				val = 0;
				while(isdigit(*s)) {
					val *= 10;
					val += *s - '0';
					s++;
				}
				setseed = val;
			break;
			#endif

			#if CHEATS
			case 's':
				scrollflag = TRUE;
			break;
			#endif

			#if CHEATS && !IS_VERSION(SHAREWARE)
			case 't':
				leveldebug = TRUE;
				setlevel = TRUE;
				while (isspace(*s))
					s++;
				val = 0;
				while (isdigit(*s)) {
					val *= 10;
					val += *s - '0';
					s++;
				}
				setlvlnum = val;
			break;
			#endif

			#if CHEATS
			case 'j':
				while (isspace(*s))
					s++;
				val = 0;
				while (isdigit(*s)) {
					val *= 10;
					val += *s - '0';
					s++;
				}
				trigdebug = val;
			break;
			#endif

			#if CHEATS
			case 'v':
				visiondebug = TRUE;
			break;
			#endif

			#ifndef NDEBUG
			case 'f':
				toggle_frame_counter();
			break;
			#endif

			#ifndef NDEBUG
			case 'x':
				fullscreen = FALSE;
			break;
			#endif

			#ifndef NDEBUG
			case 'n':
				cineflag = FALSE;
			break;
			#endif
			
			#if CHEATS
			case '7':
				itemcheat = TRUE;
			break;
			
			case '8':
				uniqcheat = TRUE;
			break;
			#endif
		}

	}
}


//******************************************************************
//******************************************************************
void FreeGameMem() {
	music_stop();

	DiabloFreePtr(pDungeonCels);
	DiabloFreePtr(pMegaTiles);
	DiabloFreePtr(pMiniTiles);
	DiabloFreePtr(pSpecialCels);
	DiabloFreePtr(pSpeedCels);
	FreeMissileGFX();

	FreeMonsterGFX();
	FreeObjectGFX();
	FreeMonsterSnd();
	FreeTownerGFX();
}


//******************************************************************
// stuff done every time a game is started/ended
//******************************************************************
static void start_game(UINT uMsg) {
	gbDoEnding = FALSE;
	svgamode = TRUE;
	InitCursor();
	InitLightTable();
	InitDebugGFX();
	app_assert(ghMainWnd);
	music_stop();
	ShowProgress(uMsg);
	gmenu_init();
	InitLevelCursor();
	sgnTimeoutCurs = NO_CURSOR;
	sgbMouseDown = 0;
	TrackInit(FALSE);
}
static void free_game() {
	FreeControlPan();
	FreeInvGFX();
	gmenu_free();
	FreeQuestText();
	FreeStoreMem();
	for (int i = 0; i < MAX_PLRS; i++)
		FreePlayerGFX(i);
	FreeItemGFX();

	FreeCursor();
	FreeLightTable();
	FreeDebugGFX();
	FreeGameMem();
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void run_game_loop(UINT uMsg) {
	nthread_perform_keepalive(TRUE);
	start_game(uMsg);
	app_assert(ghMainWnd);
	WNDPROC saveProc = my_SetWindowProc(GM_Game);
	CalcInitBallPer();
	// pjw.patch2.start
	// nthread_perform_keepalive(FALSE);
	// pjw.patch2.end
	run_delta_info();

	gbRunGame = TRUE;
	gbProcessPlayers = TRUE;
	gbRunGameResult = TRUE;
	force_redraw = FULLDRAW;
	DrawAndBlit();
	PaletteFadeIn(FADE_FAST);
	force_redraw = FULLDRAW;
	gbGameLoopStartup = TRUE;

	// pjw.patch2.start
	nthread_perform_keepalive(FALSE);
	// pjw.patch2.end


// pjw.patch1.start
	MSG msg;
	//plr_encrypt(TRUE);
	while (gbRunGame) {
		DoTimedEvents();	// palette cycling

	// pjw.patch2.start
		// if there are any messages in the queue, process them all
		if (PeekMessage(&msg,NULL,0,0,PM_NOREMOVE)) {
			// bump thread priority to make sure that
			// during peekmessage loop we are less likely
			// to yield to another process
			SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);
			//plr_encrypt(FALSE);
			while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
				if (msg.message == WM_QUIT) {
					gbRunGame = gbRunGameResult = FALSE;
					break;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			BOOL bRun = gbRunGame && nthread_run_gameloop(FALSE);
			//if (! bRun) plr_encrypt(TRUE);
			SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
			if (! bRun) continue;
		}
		else if (! nthread_run_gameloop(FALSE)) {
			// not enough time has elapsed since last gameloop
			continue;
		}
	// pjw.patch2.end

		// actual game logic
		//plr_encrypt(FALSE);
		NetReceivePackets();
		game_loop(gbGameLoopStartup);
		gbGameLoopStartup = FALSE;
		DrawAndBlit();
		//plr_encrypt(TRUE);
	}
	//plr_encrypt(FALSE);
// pjw.patch1.end

	// save character in multiplayer mode
	if (gbMaxPlayers > 1) {
		void UpdatePlayerFile();
		UpdatePlayerFile();
	}
	void ReleasePlayerFile();
	ReleasePlayerFile();

	PaletteFadeOut(FADE_FAST);
	SetCursor(NO_CURSOR);
	ClrDraw();
	force_redraw = FULLDRAW;
	FullBlit(TRUE);

	// restore old window procedure
	saveProc = my_SetWindowProc(saveProc);
	app_assert(saveProc == GM_Game);

	free_game();

	if (gbDoEnding) {
		gbDoEnding = FALSE;
		DoEnding();
	}
}


/*-----------------------------------------------------------------------**
// return FALSE to quit game
// return TRUE to continue game
**-----------------------------------------------------------------------*/
BOOL StartGame(BOOL bNewGame, BOOL bSinglePlayer) {

	extern BYTE gbSelectProvider;
	gbSelectProvider = TRUE;

	while (1) {

		// initialize network
		BOOL fExitProgram = FALSE;
		if (! NetInit(bSinglePlayer,&fExitProgram)) {
			gbRunGameResult = !fExitProgram;
			break;
		}

		gbSelectProvider = FALSE;

		UINT uMsg;
		if (bNewGame || !gbValidSaveFile) {
		
			InitLevels();
			InitQuests();
			InitPortals();
			InitDungMsgs(myplr);
			if (!gbValidSaveFile && gbSaveFileExists)
			{
				// clear the items from the character's inventory
				StripPlayer(myplr);
			}
			uMsg = WM_DIABNEWGAME;
		}
		else {
			uMsg = WM_DIABLOADGAME;
		}

		run_game_loop(uMsg);
		NetClose();

		// in single player mode, exit this loop
		if (gbMaxPlayers == 1) break;

		// in multiplayer mode, only break out of the
		// loop if the player wants to exit the game, otherwise
		// the loop will be exited based on results from NetInit()
		if (! gbRunGameResult) break;
	}

	TRACE_FCN("SNetDestroy");
	SNetDestroy();
	TRACE_FCN(NULL);

	return gbRunGameResult;
}


//******************************************************************
//******************************************************************
static void InitOnce() {
	MouseX = TOTALX >> 1;
	MouseY = TOTALY >> 1;

	ScrollInfo._sdx = 0;
	ScrollInfo._sdy = 0;
	ScrollInfo._sxoff = 0;
	ScrollInfo._syoff = 0;
	ScrollInfo._sdir = SCRL_NONE;
	for (int i = 0; i < 1024; i++)
		nBuffWTbl[i] = i * BUFFERX;

	// Init engine vars
	ClrDiabloMsg();
}


//******************************************************************
//******************************************************************
/* pjw.patch2.start
static const char sgszErrFile[] = "c:\\helfire_.err";
static void write_exception(struct _EXCEPTION_POINTERS *pep) {
	FILE * f = fopen(sgszErrFile,"wb");
	if (! f) return;

	PEXCEPTION_RECORD per = pep->ExceptionRecord;
	while (per) {
		fprintf(
			f,
			"exception: 0x%08x\r\ncode address: 0x%08x\r\n",
			per->ExceptionCode,
			per->ExceptionAddress
		);

		if (per->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
			if (per->ExceptionInformation[0])
				fprintf(f,"write access violation at 0x%08x\r\n",per->ExceptionInformation[1]);
			else
				fprintf(f,"read access violation at 0x%08x\r\n",per->ExceptionInformation[1]);
		}

		per = per->ExceptionRecord;
	}

	fclose(f);
}
pjw.patch2.end */


//******************************************************************
//******************************************************************
// pjw.patch2.start
void cleanup(BOOL bNormalExit);
static LPTOP_LEVEL_EXCEPTION_FILTER sg_previousFilter;
static LONG WINAPI DiabloUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *pep) {
	void free_directx();
	free_directx();
	cleanup(FALSE);
	// write_exception(pep);
	if (sg_previousFilter) return sg_previousFilter(pep);
	return EXCEPTION_CONTINUE_SEARCH;
}
// pjw.patch2.end


//******************************************************************
//******************************************************************
BOOL grab_event() {
	// NOTE: this event never gets cleaned up by our code
	// we rely upon windows to clean up this event so that it
	// occurs as late as possible in the program exit.  This is
	// a really nasty hack to make sure that SMACKER doesn't
	// initialize improperly
	SetLastError(0);
	HANDLE hExtraEvent = CreateEvent(NULL,FALSE,FALSE,"DiabloEvent");
	HANDLE hEvent = CreateEvent(NULL,FALSE,FALSE,"HellfireEvent");
	return ERROR_ALREADY_EXISTS != GetLastError();
}


//******************************************************************
//******************************************************************
static BOOL ActivatePreviousInstance(const char * pszAppClass) {
	// find main application window
	HWND hWnd;
	HWND hWndTop;
	HWND hWndPopup;

	// get parent window
	if (NULL == (hWnd = FindWindow(pszAppClass,NULL)))
		return FALSE;

	// get popup window (if any)
	if (NULL != (hWndPopup = GetLastActivePopup(hWnd)))
		hWnd = hWndPopup;

	// get topmost control window of topmost window
	hWndTop = GetTopWindow(hWnd);
	if (! hWndTop) hWndTop = hWnd;

	// bring to the front
	SetForegroundWindow(hWnd);
	SetFocus(hWndTop);
	
	return TRUE;
}


typedef struct _SHAREDDATA {
    LONG  status;
    DWORD processid;
} SHAREDDATA, *SHAREDDATAPTR;


//===========================================================================
// pjw.patch2.start
#ifdef NDEBUG
static void inline ReloadSelf (HINSTANCE instance) {

  // GET THE MODULE FILENAME
  char filename[MAX_PATH] = "";
  GetModuleFileName((HMODULE)instance,filename,MAX_PATH);

  
  // OPEN NAMED SHARED MEMORY
  char name[MAX_PATH+16];
  wsprintf(name,"Reload-%s",filename);
  for (LPSTR curr = name; *curr; ++curr)
    if (*curr == '\\')
      *curr = '/';
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  DWORD  bytes = max(4096,sysinfo.dwPageSize);
  
  
  HANDLE map   = CreateFileMapping((HANDLE)0xFFFFFFFF,
                                   NULL,
                                   PAGE_READWRITE | SEC_COMMIT,
                                   0,
                                   bytes,
                                   name);
  BOOL   first = (GetLastError() != ERROR_ALREADY_EXISTS);
  if (!map)
    return;
  
  // Open secondary named shared memory to fool Diablo into not autorunning
  char tmpname[MAX_PATH+16];
  strcpy(tmpname,"Reload-Diablo");
  HANDLE tmpmap = CreateFileMapping((HANDLE)0xFFFFFFFF,
                                   NULL,
                                   PAGE_READWRITE | SEC_COMMIT,
                                   0,
                                   bytes,
                                   tmpname);
  // end of hack
  
  LPVOID view  = MapViewOfFile(map,FILE_MAP_ALL_ACCESS,0,0,bytes);
  if (!view)
    return;
  SHAREDDATAPTR ptr = (SHAREDDATAPTR)view;

  // IF WE ARE THE FIRST INSTANCE, THEN RELOAD OURSELVES, WAIT FOR THE
  // SECOND INSTANCE TO INITIALIZE, THEN QUIT
  if (first) {
    ptr->status    = -1;
    ptr->processid = 0;
    STARTUPINFO startupinfo;
    ZeroMemory(&startupinfo,sizeof(STARTUPINFO));
    startupinfo.cb = sizeof(STARTUPINFO);
    PROCESS_INFORMATION processinfo;
    CreateProcess(filename,
                  NULL,
                  NULL,
                  NULL,
                  FALSE,
                  CREATE_NEW_PROCESS_GROUP,
                  NULL,
                  NULL,
                  &startupinfo,
                  &processinfo);
    WaitForInputIdle(processinfo.hProcess,INFINITE);
    CloseHandle(processinfo.hThread);
    CloseHandle(processinfo.hProcess);
    while (ptr->status < 0)
      Sleep(1000);
    UnmapViewOfFile(view);
    CloseHandle(map);
    ExitProcess(0);
  }

  // OTHERWISE, ALLOW ONE INSTANCE TO RUN, AND MAKE ANY ADDITIONAL
  // INSTANCES JUST REACTIVATE THE RUNNING INSTANCE
  if (!InterlockedIncrement(&ptr->status))
    ptr->processid = GetCurrentProcessId();
  else {
    HWND window = GetForegroundWindow();
    HWND prev;
    while ((prev = GetNextWindow(window,GW_HWNDPREV)) != (HWND)0)
      window = prev;
    do {
      DWORD processid;
      GetWindowThreadProcessId(window,&processid);
      if (processid == ptr->processid) {
        SetForegroundWindow(window);
        break;
      }
    } while ((window = GetNextWindow(window,GW_HWNDNEXT)) != (HWND)0);
    UnmapViewOfFile(view);
    CloseHandle(map);
    ExitProcess(0);
  }
}
#endif
// pjw.patch2.end


//******************************************************************
//******************************************************************
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow){
// pjw.patch2.start
	#ifdef NDEBUG
	ReloadSelf(hInstance);
	#endif
// pjw.patch2.end

	ghInst = hInstance;
	ShowCursor(FALSE);
	// pjw.patch1.start
	srand(GetTickCount());
	InitializeHashSource();
	alloc_plr();
	// pjw.patch1.end

	// delete any previous error file created by exception
	// handler and then install exception handler
	// pjw.patch2.start
	// DeleteFile(sgszErrFile);
	sg_previousFilter = SetUnhandledExceptionFilter(DiabloUnhandledExceptionFilter);
	// pjw.patch2.end

	// if another instance of Diablo is already running,
	// then activate it and exit this instance
	BOOL bGotEvent = grab_event();
	if (ActivatePreviousInstance("DIABLO"))	// conflict with parent is bad
		return FALSE;
	if (ActivatePreviousInstance(gszAppName))
		return FALSE;
	if (!bGotEvent) return FALSE;
		

	#ifndef NDEBUG
	SFileEnableDirectAccess(1);
	#endif

	InitOnce();
	char cpCmdLine[255];
	if (!lpCmdLine[0]) {
		cpCmdLine[0] = 0;
		FILE * const fp = fopen("command.txt","r");
		if (fp) {
			fgets(cpCmdLine, sizeof(cpCmdLine)/sizeof(char), fp);
			lpCmdLine = cpCmdLine;
			fclose(fp);
		}
	}
	CommandLine(lpCmdLine);
	init_window(nCmdShow);

	menusnd_init();
	UiInitialize();

	#if IS_VERSION(SHAREWARE)
	UiSetSpawned(TRUE);
	#endif
	

	// play logo
	#ifndef NDEBUG
	if (cineflag) play_movie("gendata\\logo.smk",TRUE);
	#else
	play_movie("gendata\\logo.smk",TRUE);
	#endif

	// play magazine quotes
	/*
	#if IS_VERSION(SHAREWARE)
		#ifndef NDEBUG
		if (cineflag) play_quotes();
		#else
		play_quotes();
		#endif
	#endif
	*/

	// play main intro
	#if !IS_VERSION(SHAREWARE)
	const char sgszMovie[] = "Intro";
	DWORD dwPlayMovie;
	if (! SRegLoadValue(gszProgKey,sgszMovie,0,&dwPlayMovie))
		dwPlayMovie = 1;
	//if (dwPlayMovie)
		play_movie("gendata\\Hellfire.smk",TRUE);
	SRegSaveValue(gszProgKey,sgszMovie,0,0);
	#endif


	// play title screen
	#ifndef NDEBUG
	if (cineflag) {
	#endif
		UiTitleDialog(7);		// DKT this needs to change to ours
		BlackPalette();
	#ifndef NDEBUG
	}
	#endif

	// play beta warning
	#if IS_VERSION(BETA)
		#ifndef NDEBUG
		if (cineflag) {
		#endif
			UiBetaDisclaimer(5);
			BlackPalette();
		#ifndef NDEBUG
		}
		#endif
	#endif

	// main game menu (finally)
	DiabloMenu();

	// play "buy me" screens
	/*
	#if IS_VERSION(SHAREWARE)
		#ifndef NDEBUG
		if (cineflag) play_quit();
		#else
		play_quit();
		#endif
	#endif
	*/

	UiDestroy();
	SavePaletteSettings();

	if (ghMainWnd) {
		// sleep before we destroy window so
		// that SFX have time to finish before exit
		Sleep(300);
		DestroyWindow(ghMainWnd);
	}

	return FALSE;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static BOOL CheckPlrLBtn(BOOL ShiftDown) {
	BOOL adjacent;

	app_assert(MouseY < 352);
	if (leveltype == 0) {
		//
		// Town Level
		//
		ShiftDown = FALSE;	// no attacking in town
		if ((cursitem != -1) && (curs == GLOVE_CURS))
			NetSendCmdLocParam1(TRUE,invflag ? CMD_GOTOGETITEM : CMD_GOTOAGETITEM,cursmx,cursmy,cursitem);
		if (cursmonst != -1)
			NetSendCmdLocParam1(TRUE,CMD_TALKXY,cursmx,cursmy,cursmonst);
		if ((cursitem == -1) && (cursmonst == -1) && (cursplr == -1))
			return TRUE;
	}
	else {
		//
		// NOT Town Level
		//
		adjacent = abs(plr[myplr]._px - cursmx) < 2
				   && abs(plr[myplr]._py - cursmy) < 2;
		if ((cursitem != -1) && (curs == GLOVE_CURS) && !ShiftDown)
			NetSendCmdLocParam1(TRUE,invflag ? CMD_GOTOGETITEM : CMD_GOTOAGETITEM,cursmx,cursmy,cursitem);
		else if ((cursobj != -1) 
			&& (!ShiftDown				// allow barrel busting even with shift key down
				|| (adjacent && object[cursobj]._oBreak == OBJ_BREAKABLE)))
			NetSendCmdLocParam1(TRUE,(curs == DISARM_CURS) ? CMD_DISARMXY : CMD_OPOBJXY,cursmx,cursmy,cursobj);

		else if (plr[myplr]._pwtype == WEAP_RANGE) {
			if (ShiftDown)
				NetSendCmdLoc(TRUE,CMD_RATTACKXY, cursmx, cursmy);
			else if (cursmonst != -1) {
				if (CanTalkToMonst(cursmonst))
					// walk over to monster to talk to him
					NetSendCmdParam1(TRUE,CMD_ATTACKID,cursmonst);
				else
					// Attack
					NetSendCmdParam1(TRUE,CMD_RATTACKID,cursmonst);
			} else if (cursplr != -1 && !FriendlyMode)
				NetSendCmdParam1(TRUE,CMD_RATTACKPID, cursplr);
		}

		else  { // pwtype == WEAP_H2H
			if (ShiftDown) {
				if (cursmonst != -1) {
					if (CanTalkToMonst(cursmonst))
						NetSendCmdParam1(TRUE,CMD_ATTACKID,cursmonst);
					else
						NetSendCmdLoc(TRUE,CMD_SATTACKXY, cursmx, cursmy);
				} else
					NetSendCmdLoc(TRUE,CMD_SATTACKXY, cursmx, cursmy);
			}
			else if (cursmonst != -1)
				NetSendCmdParam1(TRUE,CMD_ATTACKID,cursmonst);
			else if (cursplr != -1 && !FriendlyMode)
				NetSendCmdParam1(TRUE,CMD_ATTACKPID,cursplr);
		}

		if ((!ShiftDown) && (cursitem == -1) && (cursobj == -1) && (cursmonst == -1) && (cursplr == -1))
			return TRUE;
	}

	return FALSE;
}

/*-----------------------------------------------------------------------**
** Game message processing
**-----------------------------------------------------------------------*/
static BOOL TryIconCurs() {
	if (curs == RESURRECT_CURS) {
		NetSendCmdParam1(TRUE,CMD_RESURRECT, cursplr);
		return(TRUE);
	}

	if (curs == HEALOTHER_CURS) {
		NetSendCmdParam1(TRUE,CMD_HEALOTHER, cursplr);
		return(TRUE);
	}

	if (curs == TELE_CURS) {
		DoTelekinesis();
		return(TRUE);
	}
	if (curs == IDENTIFY_CURS) {
		if (cursinvitem != -1)
			CheckIdentify(myplr, cursinvitem);
		else
			NewCursor(GLOVE_CURS);
		return(TRUE);
	}
	if (curs == REPAIR_CURS) {
		if (cursinvitem != -1)
			DoRepair(myplr, cursinvitem);
		else
			NewCursor(GLOVE_CURS);
		return(TRUE);
	}
	if (curs == RECHARGE_CURS) {
		if (cursinvitem != -1)
			DoRecharge(myplr, cursinvitem);
		else
			NewCursor(GLOVE_CURS);
		return(TRUE);
	}

	if (curs == OIL_CURS) {
		if (cursinvitem != -1)
			DoOil(myplr, cursinvitem);
		else
			NewCursor(GLOVE_CURS);
		return(TRUE);
	}

	if (curs == TARGET_CURS) {
		if (cursmonst != -1)
			NetSendCmdParam3(TRUE,CMD_TSPELLID,cursmonst,plr[myplr]._pTSpell, GetSpellLevel(myplr, plr[myplr]._pTSpell));
		else if (cursplr != -1)
			NetSendCmdParam3(TRUE,CMD_TSPELLPID,cursplr,plr[myplr]._pTSpell,GetSpellLevel(myplr, plr[myplr]._pTSpell));
		else
			NetSendCmdLocParam2(TRUE,CMD_TSPELLXY,cursmx,cursmy,plr[myplr]._pTSpell,GetSpellLevel(myplr, plr[myplr]._pTSpell));
		NewCursor(GLOVE_CURS);
		return(TRUE);
	}
	if ((curs == DISARM_CURS) && (cursobj == -1)) {
		NewCursor(GLOVE_CURS);
		return(TRUE);
	}
	return(FALSE);
}


//******************************************************************
//******************************************************************
static BOOL wm_lbuttondown(WPARAM wParam) {
	if (gmenu_click(TRUE))
		return FALSE;

	BOOL talk_click();
	if (talk_click())
		return FALSE;

	// don't accept input in timeout mode
	if (sgnTimeoutCurs != NO_CURSOR)
		return FALSE;

	if (deathflag) {
		CheckDeadButtons();
		return FALSE;
	}

	if (PauseMode == 2)
		return FALSE;

	if (drawmapofdoom) {
		EndMapOfDoomView();
		return FALSE;
	}

	if (spselflag) {
		SetSpell();
		return FALSE;
	}

	if (stextflag != STORE_NONE) {
		CheckStoreBtn();
		return FALSE;
	}

	if (MouseY < 352) {
		if (gmenu_is_on())
			return FALSE;

		if (TryIconCurs())
			return FALSE;

		if (questlog) {
			if ((MouseX > 32) && (MouseX < 288) && (MouseY > 32) && (MouseY < 308)) {
				CheckQLogBtn();
				return FALSE;
			}
		}

		if (qtextflag) {
			qtextflag = FALSE;
			stream_stop();
			return FALSE;
		}

		if ((chrflag) && (MouseX < 320)) {
			CheckChrBtns();
			return FALSE;
		}
		else if ((invflag) && (MouseX > 320)) {
			if (!dropGoldFlag) CheckInvScrn();
			return FALSE;
		}
		else if ((sbookflag) && (MouseX > 320)) {
			CheckSBook();
			return FALSE;
		}
		else if (curs >= ICSTART) {
			if (TryInvPut()) {
				NetSendCmdPItem(TRUE,CMD_PUTITEM,cursmx,cursmy);
				NewCursor(GLOVE_CURS);
			}
			return FALSE;
		}
		else {
			if ((plr[myplr]._pStatPts != 0) && (!spselflag))
				CheckLvlBtn();
			if (!lvlbtndown)
				return CheckPlrLBtn(wParam == (MK_LBUTTON | MK_SHIFT));
		}
	}
	else {
		if (!talkflag && !dropGoldFlag && !gmenu_is_on())
			CheckSpdBar();
		CheckPanelBtns();
		if ((curs > GLOVE_CURS) && (curs < ICSTART))
			NewCursor(GLOVE_CURS);
	}

	return FALSE;
}


//******************************************************************
//******************************************************************
static void wm_lbuttonup() {
	gmenu_click(FALSE);
	void talk_release();
	talk_release();
	if (panbtndown) ReleasePanelBtn();
	if (chrbtndown) ReleaseChrBtn();
	if (lvlbtndown) ReleaseLvlBtn();
	if (stextflag != STORE_NONE) ReleaseStoreBtn();
}


//******************************************************************
//******************************************************************
static void wm_rbuttondown() {

	// don't allow input while the menu is active
	if (gmenu_is_on()) return;

	// don't accept input in timeout mode
	if (sgnTimeoutCurs != NO_CURSOR)
		return;

	if (PauseMode == 2)	return;

	if (plr[myplr]._pInvincible) return;

	if (drawmapofdoom) {
		EndMapOfDoomView();
		return;
	}

	if (stextflag != STORE_NONE) return;

	if (spselflag) {
		SetSpell();
		return;
	}
//	if (sbookflag && CheckSBookCast())
//		return;
	if (sbookflag && MouseX > 320) return;

	if (MouseY < 352) {
		if (TryIconCurs())
			return;
		if (cursinvitem != -1 && UseInvItem(myplr, cursinvitem))
			return;
	}
	if (curs == GLOVE_CURS) {
		if (cursinvitem != -1 && UseInvItem(myplr, cursinvitem))
			return;
//		else if (cursitem != -1 && (wParam & WM_SHIFT))
//			UseGroundItem(cursitem);
		else
			CheckPlrSpell();
	}
	else if ((curs > GLOVE_CURS) && (curs < ICSTART)) NewCursor(GLOVE_CURS);
}


//******************************************************************
//******************************************************************
static void wm_mousemove() {
	// check for menu sliders
	if (gmenu_mousemove()) return;

	// don't accept input in timeout mode
	if (sgnTimeoutCurs != NO_CURSOR) return;

	// do game mousemove here
}


//******************************************************************
//******************************************************************
static void TogglePause() {
	// no pause in multiplayer
	if (gbMaxPlayers > 1) return;

	if (PauseMode) {
		PauseMode = 0;
	}
	else {
		PauseMode = 2;
		sound_stop();
		TrackInit(FALSE);
	}
	force_redraw = FULLDRAW;
}


//******************************************************************
//******************************************************************
static void SendTaunt(DWORD dwMsg) {
	// don't send taunts in single player mode
	if (gbMaxPlayers == 1) return;

	// get program directory
	char szPath[MAX_PATH];
	if (! GetModuleFileName(ghInst,szPath,MAX_PATH))
		app_fatal("Can't get program name");
	char * pszName = strrchr(szPath,'\\');
	if (pszName) *pszName = 0;
	strcat(szPath,"\\hellfire.ini");

	static const char * spszMsgTbl[] = {
		"I need help! Heal me!",
		"Go through that door first.",
		"Give me some gold please.",
		"Look out behind you!",
	};
	static const char * spszKeyTbl[] = {
		"F9","F10","F11","F12"
	};

	char szBuf[MAX_SEND_STR_LEN];
	app_assert(dwMsg < sizeof(spszMsgTbl) / sizeof(spszMsgTbl[0]));
	GetPrivateProfileString(
		"NetMsg",			// section name
		spszKeyTbl[dwMsg],	// key name
		spszMsgTbl[dwMsg],	// default string
		szBuf,				// dest buffer
		sizeof(szBuf) / sizeof(szBuf[0]),
		szPath
	);

	// send message to other players
	NetSendString(SEND_ALL_MASK,szBuf);
}

	
//******************************************************************
//******************************************************************
static BOOL wm_syskeydown(WPARAM wKey) {
	// don't allow input while the menu is active
	if (gmenu_is_on())
		return FALSE;

	// note: this identical code is in the keydown case also, but
	// F10 seems to come through as a system key sometimes...
	if (wKey == VK_F10) {
		SendTaunt(1);
		return TRUE;
	}

	return FALSE;
}


//******************************************************************
//******************************************************************
static void wm_keyup(WPARAM wKey) {
	// can't ever get keydown for snapshot, but we do get keyup
	if (wKey == VK_SNAPSHOT)
		screen_capture();

	// if any more cases are added here, then make sure menu is not active
	// if (gmenu_is_on()) return;
}


//******************************************************************
//******************************************************************
BOOL clear_windows() {
	BOOL bResult = FALSE;

	if (drawmapofdoom) {
		EndMapOfDoomView();
		bResult = TRUE;
	}
	if (helpflag) {
		helpflag = FALSE;
		bResult = TRUE;
	}
	if (qtextflag) {
		qtextflag = FALSE;
		stream_stop();
		#if CHEATS
		if ((currlevel == 0) && (davecheat)) {			
			tstQMsgFlag = FALSE;
			tstQMsgIndexFlag = TRUE;
		}
		#endif
		bResult = TRUE;
	} else
	if (stextflag) {
		STextESC();
		bResult = TRUE;
	}
	if (msgflag) {
		msgdelay = 0;
		bResult = TRUE;
	}
	if (talkflag) {
		TalkEnd();
		bResult = TRUE;
	}
	if (dropGoldFlag) {
		DropGoldType(VK_ESCAPE);
		bResult = TRUE;
	}
	if (spselflag) {
		spselflag = FALSE;
		bResult = TRUE;
	}

	return bResult;
}


//******************************************************************
//******************************************************************
static void wm_keydown(WPARAM wKey) {
	// allow use of menu even when character is dead
	if (gmenu_key(wKey))
		return;

	// allow user to finish current string even in timeout mode
	if (Talk_wm_keydown(wKey))
		return;

	// don't allow dead player to do anything except turn on gamemenu 
	if (deathflag) {
		// no input while in timeout mode
		if (sgnTimeoutCurs != NO_CURSOR) return;
		if (wKey == VK_F9) SendTaunt(0);
		if (wKey == VK_F10) SendTaunt(1);
		if (wKey == VK_F11) SendTaunt(2);
		if (wKey == VK_F12) SendTaunt(3);
		if (wKey == VK_RETURN) TalkStart();
		if (wKey != VK_ESCAPE) return;
	}

	if (wKey == VK_ESCAPE) {
		if (! clear_windows()) {
			TrackInit(FALSE);
			gamemenu_on();
		}
		#if CHEATS
		if ((currlevel == 0) && (davecheat) && 
			((tstQMsgFlag) || (tstQMsgIndexFlag))) {			
			tstQMsgFlag = FALSE;
			tstQMsgIndexFlag = FALSE;
			sprintf(tempstr, "STOP QUEST TEXT MODE");
			NetSendString((1 << myplr), tempstr);
			return;
		}
		#endif
		return;
	}

	// don't accept input in timeout mode
	if (sgnTimeoutCurs != NO_CURSOR)
		return;

	// Don't accept input when in drop gold mode
	if (dropGoldFlag)
		return;

	// in pause mode the only thing we can do is unpause
	if (wKey == VK_PAUSE) {
		TogglePause();
		return;
	}
	if (PauseMode == 2)	return;

	if (wKey == VK_RETURN) {
		if (stextflag) STextEnter();
		else if (questlog) QuestlogEnter();
		#if CHEATS
		else if ((currlevel == 0) && (davecheat)) {
			if (tstQMsgIndexFlag) {
				tstQMsgIndexFlag = FALSE;
				tstQMsgFlag = TRUE;
				tstQMsgSpd = 5;
				DaveQuestText();
				sprintf(tempstr, "Message Speed = %i", tstQMsgSpd);
				NetSendString((1 << myplr), tempstr);
			} else if (tstQMsgFlag) {
				sprintf(tempstr, "[ MESSAGE = %i ]", tstQMsgIndex);
				NetSendString((1 << myplr), tempstr);
				sprintf(tempstr, "[ MESSAGE SPEED = %i ]", tstQMsgSpd);
				NetSendString((1 << myplr), tempstr);
			}
		}
		#endif
		else TalkStart();
		return;
	}

	if (wKey == VK_F1) {
		if (helpflag) {
			helpflag = FALSE;
		}
		else if (stextflag != STORE_NONE) {
			ClearPanel();
			AddPanelString("No help available", TEXT_CENTER);
			AddPanelString("while in stores", TEXT_CENTER);
			TrackInit(FALSE);
		}
		else {
			invflag = FALSE;
			chrflag = FALSE;
			sbookflag = FALSE;
			spselflag = FALSE;
			if ((qtextflag) && (leveltype == 0)) {
				qtextflag = FALSE;
				stream_stop();
			}
			questlog = FALSE;
			automapflag = FALSE;
			msgdelay = 0;
			gamemenu_off();
			StartHelp();
			EndMapOfDoomView();
		}
		return;
	}

	#if CHEATS
	if (wKey == VK_F2) {
		void PrintDaveCheck2();
		PrintDaveCheck2();
	}

/*	if (wKey == VK_F3) {
		extern BOOL syncdebug;
		syncdebug = !syncdebug;
		if (syncdebug)
			strcpy(tempstr, "Item drop debug on");
		else
			strcpy(tempstr, "Item drop debug off");
		NetSendString((1 << myplr), tempstr);
	}*/
	if (wKey == VK_F3) {
		if (cursitem != -1) {
			sprintf(tempstr, "IDX = %i  :  Seed = %i  :  CF = %i", item[cursitem].IDidx, item[cursitem]._iSeed, item[cursitem]._iCreateInfo);
			NetSendString((1 << myplr), tempstr);
		}
		sprintf(tempstr, "Numitems : %i", numitems);
		NetSendString((1 << myplr), tempstr);
	}

	if (wKey == VK_F4) {
		void PrintQuestDebug();
		PrintQuestDebug();
		return;
	}
	#endif

/*	#if CHEATS
	if (wKey == VK_F4) {
		if (cheatflag) toggle_frame_counter();
		return;
	}
	#endif*/

	if (wKey == VK_F5) {
		if (spselflag) SetSpellHK(0);
		else GetSpellHK(0);
		return;
	}
	if (wKey == VK_F6) {
		if (spselflag) SetSpellHK(1);
		else GetSpellHK(1);
		return;
	}
	if (wKey == VK_F7) {
		if (spselflag) SetSpellHK(2);
		else GetSpellHK(2);
		return;
	}
	if (wKey == VK_F8) {
		if (spselflag) SetSpellHK(3);
		else GetSpellHK(3);
		return;
	}

	if (wKey == VK_F9) {
		SendTaunt(0);
		return;
	}
	if (wKey == VK_F10) {
		SendTaunt(1);
		return;
	}
	if (wKey == VK_F11) {
		SendTaunt(2);
		return;
	}
	if (wKey == VK_F12) {
		SendTaunt(3);
		return;
	}

	if (wKey == VK_UP) {
		if (stextflag) STextUp();
		else if (questlog) QuestlogUp();
		else if (helpflag) HelpScrollUp();
		else if (automapflag) AutomapUp();
		return;
	}
		
	if (wKey == VK_DOWN) {
		if (stextflag) STextDown();
		else if (questlog) QuestlogDown();
		else if (helpflag) HelpScrollDown();
		else if (automapflag) AutomapDown();
		return;
	}

	if (wKey == VK_PRIOR) {
		if (stextflag) STextPgUp();
		return;
	}

	if (wKey == VK_NEXT) {
		if (stextflag) STextPgDown();
		return;
	}

	if (wKey == VK_LEFT) {
		if (automapflag && !talkflag) AutomapLeft();
		return;
	}
	if (wKey == VK_RIGHT) {
		if (automapflag && !talkflag) AutomapRight();
		return;
	}

	if (wKey == VK_TAB) {
		void DoAutoMap();
		DoAutoMap();
		return;
	}

/*	if (wKey == VK_CAPITAL) {
		FriendlyMode = !FriendlyMode;
		drawbtnflag = TRUE;
		return;
	}*/

	if (wKey == VK_SPACE) {
		if ((!chrflag) && (invflag) && (MouseX < 480) && (MouseY < 352)) SetCursorPos(MouseX+160,MouseY);
		if ((!invflag) && (chrflag) && (MouseX > 160) && (MouseY < 352)) SetCursorPos(MouseX-160,MouseY);
		helpflag = FALSE;
		invflag = FALSE;
		chrflag = FALSE;
		sbookflag = FALSE;
		spselflag = FALSE;
		if ((qtextflag) && (leveltype == 0)) {
			qtextflag = FALSE;
			stream_stop();
		}
		questlog = FALSE;
		automapflag = FALSE;
		msgdelay = 0;
		gamemenu_off();
		EndMapOfDoomView();
//Never clear the cursor, else we lose a scroll or a staff charge
//for a spell we've cast.
//		if ((curs != GLOVE_CURS) && (curs < ICSTART)) NewCursor(GLOVE_CURS);
		return;
	}
}


//******************************************************************
//******************************************************************
static void wm_char(WPARAM wKey) {
	#ifdef _DEBUG
	BOOL is_pat_debug_cmd(WPARAM wKey);
	if (is_pat_debug_cmd(wKey))
		return;
	#endif

	// don't allow input while the menu is active
	if (gmenu_is_on())
		return;

	if (Talk_wm_char(wKey))
		return;

	// don't accept input in timeout mode
	if (sgnTimeoutCurs != NO_CURSOR)
		return;

	// dead players can't perform input
	if (deathflag)
		return;

	// in pause mode the only thing we can do is unpause
	if ((TCHAR) wKey == 'p' || (TCHAR) wKey == 'P') {
		TogglePause();
		return;
	}
	if (PauseMode == 2) return;

	if (drawmapofdoom) {
		EndMapOfDoomView();
		return;
	}

	if (dropGoldFlag) {
		DropGoldType((char) wKey);
		return;
	}

	switch(wKey) {
		case 'g':
		case 'G':
			GammaDown();
		return;

		case 'f':
		case 'F':
			GammaUp();
		return;

		case 'i':
		case 'I':
			if (stextflag != STORE_NONE) return;
			sbookflag = FALSE;
			invflag = !invflag;
			if (dropGoldFlag) {
				dropGoldFlag = FALSE;
				dropGoldValue = 0;
			}
			if (invflag && !chrflag) {
				if (MouseX > 160 && MouseY < 352) SetCursorPos(MouseX-160,MouseY);
			}
			else {
				if (MouseX < 480 && MouseY < 352) SetCursorPos(MouseX+160, MouseY);
			}
		return;

		case 'c':
		case 'C':
			if (stextflag != STORE_NONE) return;
			questlog = FALSE;
			chrflag = !chrflag;
			if (chrflag && !invflag) {
				if (MouseX < 480 && MouseY < 352) SetCursorPos(MouseX+160,MouseY);
			}
			else {
				if (MouseX > 160 && MouseY < 352) SetCursorPos(MouseX-160, MouseY);
			} 
		return;

		#ifndef NDEBUG
		case 'r':
		case 'R':
			sprintf(tempstr, "seed = %i", glSeedTbl[currlevel]);
			NetSendString((1 << myplr), tempstr);
			sprintf(tempstr, "Mid1 = %i : Mid2 = %i : Mid3 = %i",
				glMid1Seed[currlevel], glMid2Seed[currlevel], glMid3Seed[currlevel]);
			NetSendString((1 << myplr), tempstr);
			sprintf(tempstr, "End = %i", glEndSeed[currlevel]);
			NetSendString((1 << myplr), tempstr);
		return;
		#endif

		#if CHEATS
		case 'A':
			if (cheatflag) {
				sprintf(tempstr, "Mid: %i", dMonster[cursmx][cursmy] - 1);
				NetSendString((1 << myplr), tempstr);
				if (dMonster[cursmx][cursmy] > 0) {
					monster[(dMonster[cursmx][cursmy] - 1)]._mFlags |= MFLAG_MKILLER;
					void M_Enemy(int);
					M_Enemy((dMonster[cursmx][cursmy] - 1));
				}
			}
		return;
		case 'a':
			if (cheatflag) {
				plr[myplr]._pSplLvl[(plr[myplr]._pSpell)]++;
				spelldata[SPL_TELE].sTownSpell = TRUE;
			}
		return;
		#endif

		#if CHEATS
		case ')':
		case '0':
			if (cheatflag) {
				if (gv1 > 2) gv1 = 0;
				if (gv1 == 0) {
					plr[myplr]._pIFlags &= ~IAF_FIREARROW;
					plr[myplr]._pIFlags &= ~IAF_LARROW;
				}
				if (gv1 == 1) plr[myplr]._pIFlags |= IAF_FIREARROW;
				if (gv1 == 2) plr[myplr]._pIFlags |= IAF_LARROW;
				gv1++;
			}
		return;
		#endif

		#if CHEATS
		case 'l':
		case 'L':
			if (cheatflag) ToggleLight();
		return;
		#endif

		#if CHEATS
		case 't':
		case 'T':
			if (cheatflag) {
				sprintf(tempstr, "PX = %i  PY = %i", plr[myplr]._px, plr[myplr]._py);
				NetSendString((1 << myplr), tempstr);
				sprintf(tempstr, "CX = %i  CY = %i  DP = %i", cursmx, cursmy, dungeon[cursmx][cursmy]);
				NetSendString((1 << myplr), tempstr);
			}
		return;
		#endif

		#ifndef NDEBUG
		case 'D':
			void BlipDebug(BOOL);
			BlipDebug(TRUE);
		return;
		case 'd':
			BlipDebug(FALSE);
		return;
		#endif

		#if CHEATS
		case 'e':
			if (davedebug) {
				sprintf(tempstr, "EFlag = %i", plr[myplr]._peflag);
				NetSendString((1 << myplr), tempstr);
			}
		return;
		#endif

		#ifndef NDEBUG
		case 'm':
			void DaveDebugMonst();
			DaveDebugMonst();
		return;
		case 'M':
			void DaveDebugMonst2();
			DaveDebugMonst2();
		return;
		#endif

		#if CHEATS
		case '|':
			if ((currlevel == 0) && (davecheat)) DaveGold();
		return;
		#endif

		#if CHEATS
		case '~':
			if ((currlevel == 0) && (davecheat)) DaveNewPremium();
		return;
		#endif

		#if CHEATS
		case '[':
			if ((currlevel == 0) && (davecheat)) DaveCleanUp();
		return;
		#endif

		#if CHEATS
		case ']':
			if ((currlevel == 0) && (davecheat)) DaveSpells();
		return;
		#endif

		#if CHEATS
		case ':':
			if ((currlevel == 0) && (davecheat)) DaveSpells2();
		return;
		#endif

		#if CHEATS
		case '.':
			void DaveDungDebug();
			DaveDungDebug();
		return;
		#endif

		#if CHEATS
		case '?':
			if ((currlevel == 0) && (davecheat)) {
				tstQMsgFlag = FALSE;
				tstQMsgIndexFlag = TRUE;
				sprintf(tempstr, "START QUEST TEXT MODE");
				NetSendString((1 << myplr), tempstr);
				sprintf(tempstr, "Message = %i", tstQMsgIndex);
				NetSendString((1 << myplr), tempstr);
			}
		return;
		#endif

		case 'q':
		case 'Q':
			if (stextflag != STORE_NONE) return;
			chrflag = FALSE;
			if (!questlog) StartQuestlog();
			else questlog = FALSE;
		return;

		case 'z':
		case 'Z':
			svgamode = !svgamode;
		return;

		case 's':
		case 'S':
			if (stextflag != STORE_NONE) return;
			invflag = FALSE;
			if (!spselflag) SetupSpellSel();
			else spselflag = FALSE;
			TrackInit(FALSE);
		return;

		case 'b':
		case 'B':
			if (stextflag != STORE_NONE) return;
			invflag = FALSE;
			if (dropGoldFlag) {
				dropGoldFlag = FALSE;
				dropGoldValue = 0;
			}
			sbookflag = !sbookflag;
		return;

		case '+':
		case '=':
			if (automapflag) AutomapZoomIn();
			#if CHEATS
			else if ((currlevel == 0) && (davecheat)) {
				if (tstQMsgIndexFlag) {
					tstQMsgIndex++;
					if (tstQMsgIndex > (int)(gdwAllTextEntries-1)) tstQMsgIndex = 0;
					sprintf(tempstr, "Message = %i", tstQMsgIndex);
					NetSendString((1 << myplr), tempstr);
				} else if (tstQMsgFlag) {
					tstQMsgSpd++;
					if (tstQMsgSpd > 9) tstQMsgSpd = 9;
					qtextSpd = qtextDelaySpd[tstQMsgSpd-1];
					qtextflag = FALSE;
					stream_stop();
					DaveQuestText();
					sprintf(tempstr, "Message Speed = %i", tstQMsgSpd);
					NetSendString((1 << myplr), tempstr);
				}
			}
			#endif
		return;

		case '_':
		case '-':
			if (automapflag) AutomapZoomOut();
			#if CHEATS
			else if ((currlevel == 0) && (davecheat)) {
				if (tstQMsgIndexFlag) {
					tstQMsgIndex--;
					if (tstQMsgIndex < 0) tstQMsgIndex = (int)(gdwAllTextEntries-1);
					sprintf(tempstr, "Message = %i", tstQMsgIndex);
					NetSendString((1 << myplr), tempstr);
				} else if (tstQMsgFlag){
					tstQMsgSpd--;
					if (tstQMsgSpd < 1) tstQMsgSpd = 1;
					qtextSpd = qtextDelaySpd[tstQMsgSpd-1];
					qtextflag = FALSE;
					stream_stop();
					DaveQuestText();
					sprintf(tempstr, "Message Speed = %i", tstQMsgSpd);
					NetSendString((1 << myplr), tempstr);
				}
			}
			#endif
		return;

		case 'v':
			extern char gszPrintVersion[];
			{
				char *pszaDif[] = {"Normal", "Nightmare", "Hell"};
				char tmpbuf[120];
				sprintf(tmpbuf, "%s, mode = %s", gszPrintVersion,
				pszaDif[gnDifficulty]);
				NetSendString((1 << myplr), tmpbuf);
			}
//			NetSendString((1 << myplr), gszPrintVersion);
		break;

		case 'V':
			extern char gszVersionNumber[];
			NetSendString((1 << myplr), gszVersionNumber);
		return;

		case '!':
		case '1':
			if ((plr[myplr].SpdList[0]._itype != -1) &&
				(plr[myplr].SpdList[0]._itype != IT_GOLD)) UseInvItem(myplr, 47);
		return;

		case '@':
		case '2':
			if ((plr[myplr].SpdList[1]._itype != -1) &&
				(plr[myplr].SpdList[1]._itype != IT_GOLD)) UseInvItem(myplr, 48);
		return;

		case '#':
		case '3':
			if ((plr[myplr].SpdList[2]._itype != -1) &&
				(plr[myplr].SpdList[2]._itype != IT_GOLD)) UseInvItem(myplr, 49);
		return;

		case '$':
		case '4':
			if ((plr[myplr].SpdList[3]._itype != -1) &&
				(plr[myplr].SpdList[3]._itype != IT_GOLD)) UseInvItem(myplr, 50);
		return;

		case '%':
		case '5':
			if ((plr[myplr].SpdList[4]._itype != -1) &&
				(plr[myplr].SpdList[4]._itype != IT_GOLD)) UseInvItem(myplr, 51);
		return;

		case '^':
		case '6':
			if ((plr[myplr].SpdList[5]._itype != -1) &&
				(plr[myplr].SpdList[5]._itype != IT_GOLD)) UseInvItem(myplr, 52);
		return;

		case '&':
		case '7':
			if ((plr[myplr].SpdList[6]._itype != -1) &&
				(plr[myplr].SpdList[6]._itype != IT_GOLD)) UseInvItem(myplr, 53);
		return;

		case '*':
		case '8':
			#if CHEATS
			if (cheatflag || davecheat) {
				NetSendCmd(TRUE,CMD_CHEAT_EXPERIENCE);
				return;
			}
			#endif
			if ((plr[myplr].SpdList[7]._itype != -1) &&
			     (plr[myplr].SpdList[7]._itype != IT_GOLD)) UseInvItem(myplr, 54);
		return;
/*
		// testing diablo death
		case 'k':
#define MT_DIABLO 110
			for (int i = 0; i < nummonsters; i++) {
				int mi = monstactive[i];
				if (monster[mi].MType->mtype == MT_DIABLO)
					M_StartKill(mi, myplr);
			}
			return;
*/			
	}
}


//******************************************************************
//******************************************************************
LRESULT CALLBACK DisableInputWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	switch (uMsg) {
		// don't let input go to game window proc
		case WM_SYSKEYDOWN:
		case WM_SYSCOMMAND:
		case WM_KEYUP:
		case WM_KEYDOWN:
		case WM_CHAR:
		case WM_MOUSEMOVE:
		return 0;

		case WM_LBUTTONDOWN:
			if (sgbMouseDown) return 0;
			sgbMouseDown = LMOUSE_DOWN;
			SetCapture(hWnd);
		return 0;

		case WM_LBUTTONUP:
			if (sgbMouseDown != LMOUSE_DOWN) return 0;
			sgbMouseDown = 0;
			ReleaseCapture();
		return 0;

		case WM_RBUTTONDOWN:
			if (sgbMouseDown) return 0;
			sgbMouseDown = RMOUSE_DOWN;
			SetCapture(hWnd);
		return 0;

		case WM_RBUTTONUP:
			if (sgbMouseDown != RMOUSE_DOWN) return 0;
			sgbMouseDown = 0;
			ReleaseCapture();
		return 0;

		case WM_CAPTURECHANGED:
			if (hWnd != (HWND) lParam)
				sgbMouseDown = 0;
		return 0;
	}

	return DiabloDefProc(hWnd,uMsg,wParam,lParam);
}


//******************************************************************
//******************************************************************
static LRESULT CALLBACK GM_Game(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	switch (uMsg) {
		case WM_SYSKEYDOWN:
			if (wm_syskeydown(wParam)) return 0;
		break;

		case WM_KEYUP:
			wm_keyup(wParam);
		return 0;

		case WM_KEYDOWN:
			wm_keydown(wParam);
		return 0;

		case WM_CHAR:
			wm_char(wParam);
		return 0;

		case WM_MOUSEMOVE:
			MouseX = LOWORD(lParam);
			MouseY = HIWORD(lParam);
			wm_mousemove();
		return 0;

		case WM_LBUTTONDOWN:
			MouseX = LOWORD(lParam);
			MouseY = HIWORD(lParam);

			// if the mouse is already down, wait for mouse up
			if (sgbMouseDown) return 0;
			sgbMouseDown = LMOUSE_DOWN;
			SetCapture(hWnd);

			TrackInit(wm_lbuttondown(wParam));
		return 0;

		case WM_LBUTTONUP:
			MouseX = LOWORD(lParam);
			MouseY = HIWORD(lParam);

			// don't accept mouse up unless it went down in our window
			if (sgbMouseDown != LMOUSE_DOWN) return 0;
			sgbMouseDown = 0;

			wm_lbuttonup();
			TrackInit(FALSE);
			ReleaseCapture();
		return 0;

		case WM_RBUTTONDOWN:
			MouseX = LOWORD(lParam);
			MouseY = HIWORD(lParam);

			// if the mouse is already down, wait for mouse up
			if (sgbMouseDown) return 0;
			sgbMouseDown = RMOUSE_DOWN;
			SetCapture(hWnd);

			wm_rbuttondown();
		return 0;

		case WM_RBUTTONUP:
			MouseX = LOWORD(lParam);
			MouseY = HIWORD(lParam);

			// don't accept mouse up unless it went down in our window
			if (sgbMouseDown != RMOUSE_DOWN) return 0;
			sgbMouseDown = 0;
			ReleaseCapture();
		return 0;

		case WM_CAPTURECHANGED:
			if (hWnd != (HWND) lParam) {
				sgbMouseDown = 0;
				TrackInit(FALSE);
			}
		break;

		case WM_DIABNEXTLVL:
		case WM_DIABPREVLVL:
		case WM_DIABSETLVL:
		case WM_DIABRTNLVL:
		case WM_DIABWARPLVL:
		case WM_DIABTOWNWARP:
		case WM_DIABTWARPUP:
		case WM_DIABRETOWN:
			// save character in multiplayer mode
			if (gbMaxPlayers > 1) {
				void UpdatePlayerFile();
				UpdatePlayerFile();
			}
			nthread_perform_keepalive(TRUE);
			PaletteFadeOut(FADE_FAST);
			sound_stop();
			music_stop();
			TrackInit(FALSE);
			sgbMouseDown = FALSE;
			ReleaseCapture();
			ShowProgress(uMsg);
			force_redraw = FULLDRAW;
			DrawAndBlit();
			if (gbRunGame) PaletteFadeIn(FADE_FAST);
			nthread_perform_keepalive(FALSE);
			gbGameLoopStartup = TRUE;
		return 0;

		case WM_SYSCOMMAND:
			if (wParam == SC_CLOSE) {
				gbRunGame = FALSE;
				gbRunGameResult = FALSE;
				return 0;
			}
		break;
	}
               
	return DiabloDefProc(hWnd, uMsg, wParam, lParam);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void LoadLvlGFX() {
	app_assert(! pDungeonCels);
	switch (leveltype) {
		case 0:
			pDungeonCels = LoadFileInMemSig("NLevels\\TownData\\Town.CEL",NULL,'TILE');
			pMegaTiles = LoadFileInMemSig("NLevels\\TownData\\Town.TIL",NULL,'TILE');
			pMiniTiles = LoadFileInMemSig("NLevels\\TownData\\Town.MIN",NULL,'TILE');
			pSpecialCels = LoadFileInMemSig("Levels\\TownData\\TownS.CEL",NULL,'TILE');
		break;

		case 1:
/*			pDungeonCels = LoadFileInMemSig("Levels\\L1Data\\L1.CEL",NULL,'TILE');
			pMegaTiles = LoadFileInMemSig("Levels\\L1Data\\L1.TIL",NULL,'TILE');
			pMiniTiles = LoadFileInMemSig("Levels\\L1Data\\L1.MIN",NULL,'TILE');
			pSpecialCels = LoadFileInMemSig("Levels\\L1Data\\L1S.CEL",NULL,'TILE');
		break;*/
			if (currlevel < CRYPTSTART)
			{
				pDungeonCels = LoadFileInMemSig("Levels\\L1Data\\L1.CEL",NULL,'TILE');
				pMegaTiles = LoadFileInMemSig("Levels\\L1Data\\L1.TIL",NULL,'TILE');
				pMiniTiles = LoadFileInMemSig("Levels\\L1Data\\L1.MIN",NULL,'TILE');
				pSpecialCels = LoadFileInMemSig("Levels\\L1Data\\L1S.CEL",NULL,'TILE');
			}
			else
			{
				pDungeonCels = LoadFileInMemSig("NLevels\\L5Data\\L5.CEL",NULL,'TILE');
				pMegaTiles = LoadFileInMemSig("NLevels\\L5Data\\L5.TIL",NULL,'TILE');
				pMiniTiles = LoadFileInMemSig("NLevels\\L5Data\\L5.MIN",NULL,'TILE');
				pSpecialCels = LoadFileInMemSig("NLevels\\L5Data\\L5S.CEL",NULL,'TILE');
			}
		break;

		#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
		case 2:
			pDungeonCels = LoadFileInMemSig("Levels\\L2Data\\L2.CEL",NULL,'TILE');
			pMegaTiles = LoadFileInMemSig("Levels\\L2Data\\L2.TIL",NULL,'TILE');
			pMiniTiles = LoadFileInMemSig("Levels\\L2Data\\L2.MIN",NULL,'TILE');
			pSpecialCels = LoadFileInMemSig("Levels\\L2Data\\L2S.CEL",NULL,'TILE');
		break;
		#endif

		#if IS_VERSION(RETAIL)
		case 3:
			if (currlevel < HIVESTART)
			{
				pDungeonCels = LoadFileInMemSig("Levels\\L3Data\\L3.CEL",NULL,'TILE');
				pMegaTiles = LoadFileInMemSig("Levels\\L3Data\\L3.TIL",NULL,'TILE');
				pMiniTiles = LoadFileInMemSig("Levels\\L3Data\\L3.MIN",NULL,'TILE');
				pSpecialCels = LoadFileInMemSig("Levels\\L1Data\\L1S.CEL",NULL,'TILE');
			}
			else
			{
				pDungeonCels = LoadFileInMemSig("NLevels\\L6Data\\L6.CEL",NULL,'TILE');
				pMegaTiles = LoadFileInMemSig("NLevels\\L6Data\\L6.TIL",NULL,'TILE');
				pMiniTiles = LoadFileInMemSig("NLevels\\L6Data\\L6.MIN",NULL,'TILE');
				pSpecialCels = LoadFileInMemSig("Levels\\L1Data\\L1S.CEL",NULL,'TILE');
			}
		break;
		#endif

		#if IS_VERSION(RETAIL)
		case 4:
			pDungeonCels = LoadFileInMemSig("Levels\\L4Data\\L4.CEL",NULL,'TILE');
			pMegaTiles = LoadFileInMemSig("Levels\\L4Data\\L4.TIL",NULL,'TILE');
			pMiniTiles = LoadFileInMemSig("Levels\\L4Data\\L4.MIN",NULL,'TILE');
			pSpecialCels = LoadFileInMemSig("Levels\\L2Data\\L2S.CEL",NULL,'TILE');
		break;
		#endif

		default:
			app_fatal("LoadLvlGFX");
		break;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void LoadAllGFX()
{
	app_assert(! pSpeedCels);
	pSpeedCels = DiabloAllocPtrSig(SPEEDSIZE,'SPED');
//	InitMonsterGFX();
	IntCheck();
//	InitMonsterSND();
	IntCheck();
	InitObjectGFX();
	IntCheck();
	InitMissileGFX();
	IntCheck();
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void CreateLevel(int lvldir)
{
	switch (leveltype) {
		case 0:
			CreateTown(lvldir);
			InitTownTriggers();
			LoadRndLvlPal(0);
		break;

		case 1:
			CreateL5Dungeon(glSeedTbl[currlevel], lvldir);
			InitL1Triggers();
			Freeupstairs();
			if (currlevel < CRYPTSTART)		// JKE load correct pallet
				LoadRndLvlPal(1);
			else
				LoadRndLvlPal(5);
		break;

		#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
		case 2:
			CreateL2Dungeon(glSeedTbl[currlevel], lvldir);
			InitL2Triggers();
			Freeupstairs();
			LoadRndLvlPal(2);
		break;
		#endif

		#if IS_VERSION(RETAIL)
		case 3:
			CreateL3Dungeon(glSeedTbl[currlevel], lvldir);
			InitL3Triggers();
			Freeupstairs();
			if (currlevel < HIVESTART)
				LoadRndLvlPal(3);
			else
				LoadRndLvlPal(6);
		break;
		#endif

		#if IS_VERSION(RETAIL)
		case 4:
			CreateL4Dungeon(glSeedTbl[currlevel], lvldir);
			InitL4Triggers();
			Freeupstairs();
			LoadRndLvlPal(4);
		break;
		#endif

		default:
			app_fatal("CreateLevel");
		break;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void LoadGameLevel(BOOL firstflag, int lvldir)
{
	int i,j;

	if (setseed)
		glSeedTbl[currlevel] = setseed;

	music_stop();
	SetCursor(GLOVE_CURS);
	SetRndSeed(glSeedTbl[currlevel]);
	IntCheck();

	MakeLightTable();
	LoadLvlGFX();
	IntCheck();

	if (firstflag) {
		InitInv();
		InitItemGFX();
		InitQuestText();
		for (i = 0; i < gbMaxPlayers; i++)
			InitPlrGFXMem(i);
		InitStores();
		InitAutomapOnce();
		InitHelpSys();
	}

	SetRndSeed(glSeedTbl[currlevel]);
	if (leveltype == 0) SetupTownStores();
	IntCheck();

	InitAutomap();

	if ((leveltype != 0) && (lvldir != LVL_NODIR)) {
		InitLighting();
		InitVision();
	}
	InitLevelMonsters();
	IntCheck();

	if (!setlevel) {
		CreateLevel(lvldir);
		IntCheck();

		// Open Tiles file
		FillSolidBlockTbls();
		
		SetRndSeed(glSeedTbl[currlevel]);

		if (leveltype != 0) {
			GetLevelMTypes();
			InitThemes();
			LoadAllGFX();
		} else {
			InitMissileGFX();
		}
		IntCheck();

		if (lvldir == LVL_RTN) GetReturnLvlPos();
		if (lvldir == LVL_WARP) GetPortalLvlPos();
		IntCheck();

		// Init Player info
		for (i = 0; i < MAX_PLRS; i++) {
			if (! plr[i].plractive) continue;
			if (currlevel != plr[i].plrlevel) continue;
			InitPlayerGFX(i);
			if (lvldir != LVL_NODIR) InitPlayer(i, firstflag);
		}

		PlayDungMsgs();
		InitMultiView();
		IntCheck();

		BOOL visited = FALSE;
		for (i = 0; i < gbMaxPlayers; i++)
			if (plr[i].plractive) visited = visited || plr[i]._pLvlVisited[currlevel];

		// Resync after level changing bug
		// This bug occurs when 2 players change levels one after the other.  The first player
		// down thinks the other player is one level above, because the change level msg has
		// not been processed during loading. The second player knows that they are both
		// on the same level, so more players get inited, and randoms desync.
		SetRndSeed(glSeedTbl[currlevel]);

		if (leveltype != 0) {
			if (!firstflag && (lvldir != LVL_NODIR) && plr[myplr]._pLvlVisited[currlevel] && (gbMaxPlayers == 1)) {
				// Init monsters so uniques are loaded
				InitMonsters ();
				// Init missiles
				InitMissiles();
				// Init dead info, not dungeon layout though
				InitDead();
				IntCheck ();
				// Load where things were
				LoadLevel();
				IntCheck ();
			} else {
				// Save theme room areas
				HoldThemeRooms();
	glMid1Seed[currlevel] = GetRndSeed();		// @@@ drb temp
				// Init monsters into dungeon
				InitMonsters ();
	glMid2Seed[currlevel] = GetRndSeed();		// @@@ drb temp
				// Init objects
				InitObjects();
				// Init items
				InitItems();
				// Fill theme rooms
				if (currlevel < HIVESTART) CreateThemeRooms();	// Temp hack for failure JKE
//				CreateThemeRooms();
	glMid3Seed[currlevel] = GetRndSeed();		// @@@ drb temp
				// Init missiles
				InitMissiles();
				// Init dead info, not dungeon layout though
				InitDead();
	glEndSeed[currlevel] = GetRndSeed();		// @@@ drb temp
				// if multiplayer resync level
				if (gbMaxPlayers != 1) DeltaLoadLevel();
				IntCheck();
				SavePreLighting();
			}
		} else {
			// Make everything visible
			for (i = 0; i < DMAXX; i++)
				for (j = 0; j < DMAXY; j++)
					dFlags[i][j] |= BFLAG_VISIBLE;
			// Init town people
			InitTowners();
			// Init items
			InitItems();
			// Init missiles
			InitMissiles();
			IntCheck();

			// Load items
			if (!firstflag && (lvldir != LVL_NODIR) && plr[myplr]._pLvlVisited[currlevel] && (gbMaxPlayers == 1))
				LoadLevel();
			if (gbMaxPlayers != 1) DeltaLoadLevel();
			IntCheck ();
		}
		if (gbMaxPlayers == 1)
			ResyncQuests();
		else 
			ResyncMPQuests();

	}
#if !IS_VERSION(SHAREWARE)
	else {
		// Preset levels
		app_assert(! pSpeedCels);
		pSpeedCels = DiabloAllocPtrSig(SPEEDSIZE,'SPED');

		LoadSetMap();
		IntCheck();

		GetLevelMTypes();

		InitMonsters();

		//SetDungeonMicros();

		InitMissileGFX();

		// Init dead info, not dungeon layout though
		InitDead();

		// Open Tiles file
		FillSolidBlockTbls ();
		IntCheck();

		if (lvldir == LVL_WARP) GetPortalLvlPos();

		// Init Player info
		for (i = 0; i < MAX_PLRS; i++) {
			if (! plr[i].plractive) continue;
			if (currlevel != plr[i].plrlevel) continue;
			InitPlayerGFX(i);
			if (lvldir != LVL_NODIR) InitPlayer(i, firstflag);
		}

		InitMultiView();
		IntCheck();

		if ((!firstflag) && (lvldir != LVL_NODIR) && (plr[myplr]._pSLvlVisited[setlvlnum])) {
			// Load where things were
			LoadLevel();
		} else {
			// Init items
			InitItems();
			SavePreLighting();
		}
		// Init missiles
		InitMissiles();
		IntCheck();
	}
#endif

	SyncPortals();

	for (i = 0; i < MAX_PLRS; i++) {
		if (! plr[i].plractive) continue;
		if (plr[i].plrlevel != currlevel) continue;
		if (plr[i]._pLvlChanging && (i != myplr)) continue;
		if (plr[i]._pHitPoints > 0) {
			if (gbMaxPlayers == 1)
				dPlayer[plr[i]._px][plr[i]._py] = i + 1;
			else
				SyncInitPlrPos(i);
		} else
			dFlags[plr[i]._px][plr[i]._py] |= BFLAG_DEADPLR;
	}

	if (leveltype != 0) {
		SetDungeonMicros();
	}

	InitLightMax();		// Set up 4 or 16 level lighting
	IntCheck();

/*
	if ((leveltype != 0) && (lvldir != LVL_NODIR)) {
		SavePreLighting();
	}
*/
	IntCheck();
	
	if (firstflag) {
		// Get Control panel graphics and decompress panel to BtmBuff
		// Has to be after init player because of bars/spells/equipment etc.
		InitControlPan();
		IntCheck();
	}

	if (leveltype != 0) {
		ProcessLightList();
		ProcessVisionList();
	}
	if (currlevel >= CRYPTSTART)
	{
//		OpenCloseAllDoors();	// JKE Stupid!!!
		if (currlevel == CORNERSTONE_LEVEL)
			CornerstoneRestore(CornerStone.x, CornerStone.y);
		if ((quests[Q_NA_KRUL]._qactive == QUEST_DONE)&&(currlevel == NA_KRUL_LEVEL))
			OpenNaKrul2();

	}

	// start appropriate sound track
	if (currlevel >= HIVESTART)		// fix this later JKE
	{
		music_start((currlevel > HIVEEND)? 5 : 6);
	}
	else
	music_start(leveltype);

	// finish progress bar
	while (! IntCheck())
		NULL;

	#if !IS_VERSION(SHAREWARE)
	if ((setlevel) && (setlvlnum == SL_SKELKING) && (quests[Q_SKELKING]._qactive == QUEST_NOTDONE))
		PlaySFX(USFX_SKING1);
	#endif
}


//******************************************************************
//******************************************************************
// this parameter is the maximum number of game loops which can be run
// in a row without redrawing the screen
#define MAX_CONSECUTIVE_LOOPS	3


//******************************************************************
//******************************************************************
static void game_logic() {
	if (PauseMode == 2) return;
	if (PauseMode == 1) PauseMode = 2;

	// pause when menu is active in single player mode
	if (gbMaxPlayers == 1 && gmenu_is_on()) {
		force_redraw |= VIEWDRAW;
		return;
	}

	if (! gmenu_is_on() && (sgnTimeoutCurs == NO_CURSOR)) {
		CheckCursMove();
		TrackMouse();
	}

	if (gbProcessPlayers)
		ProcessPlayers();
	if (leveltype) {
		ProcessMonsters();
		ProcessObjects();
		ProcessMissiles();
		ProcessItems();
		ProcessLightList();
		ProcessVisionList();
	}
	else {
		ProcessTowners();
		ProcessItems();
		ProcessMissiles();
	}

	#if CHEATS
	if ((cheatflag) && ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0))
		CheckForScroll();
	#endif

	sound_update();
	plrmsg_update();
	CheckTriggers();
	CheckQuests();
	force_redraw |= VIEWDRAW;

	void TimedUpdatePlayerFile(BOOL bForce);
	TimedUpdatePlayerFile(FALSE);
}


//******************************************************************
//******************************************************************
static void timeout_cursor(BOOL bTimeout) {
	if (bTimeout) {
		// if we weren't in a timeout state, set timeout
		if (sgnTimeoutCurs == NO_CURSOR && ! sgbMouseDown) {
			sgnTimeoutCurs = curs;

			NetStartTimeout();

			// display timeout error
			ClearPanel();
			AddPanelString("-- Network timeout --", TEXT_CENTER);
			AddPanelString("-- Waiting for players --", TEXT_CENTER);

			// fix up the cursor
			NewCursor(TIMEOUT_CURSOR);

			// we probably made a mess of the screen
			force_redraw = FULLDRAW;
		}

		FullBlit(TRUE);
	}
	else if (sgnTimeoutCurs != NO_CURSOR) {
		// ending timeout
		SetCursor(sgnTimeoutCurs);
		sgnTimeoutCurs = NO_CURSOR;
		ClearPanel();
		force_redraw = FULLDRAW;
	}
}


//******************************************************************
//******************************************************************
static void game_loop(BOOL bStartup) {
	int nMaxLoops = bStartup ? GAME_FRAMES_PER_SECOND * 3 : MAX_CONSECUTIVE_LOOPS;
	while (nMaxLoops--) {
		// wait for synchronous network message
		if (! NetEndSendCycle()) {
			timeout_cursor(TRUE);
			break;
		}
		timeout_cursor(FALSE);

		// run logic at GAME_FRAMES_PER_SECOND
		game_logic();

		// if the game mode changed, then don't continue loop
		if (! gbRunGame)
			break;

		// don't run multiple loops in single player mode
		if (gbMaxPlayers == 1)
			break;

		// have we been in the loop too long?
		if (! nthread_run_gameloop(TRUE))
			break;

		#if CHEATS
		static DWORD sdwSkips = 0;
		sdwSkips++;
		HDC hDC;
		HRESULT ddr = lpDDSPrimary->GetDC(&hDC);
		if (ddr == DD_OK) {
			char szBuf[16];
			wsprintf(szBuf,"%u",sdwSkips);
			TextOut(hDC,5,370,szBuf,strlen(szBuf));
			lpDDSPrimary->ReleaseDC(hDC);
		}
		#endif
	}
}


//******************************************************************
//******************************************************************
static void DoTimedEvents() {
	static DWORD sdwCurrTime = 0;
	DWORD dwCurrTime = GetTickCount();
	if (dwCurrTime - sdwCurrTime < 1000/GAME_FRAMES_PER_SECOND)
		return;
	sdwCurrTime = dwCurrTime;

	if (leveltype == 4)
		BloodCycle();
	else if (currlevel >= CRYPTSTART)
		TwinCycleCrypt();
	else if (currlevel >= HIVESTART)
		TwinCycleNest();
	else if (leveltype == 3 && fullscreen)
		LavaCycle();
}


//******************************************************************
//******************************************************************
/* pjw.patch1.start
static void try_game_loop(BOOL bStartup) {
	DoTimedEvents();
	NetReceivePackets();

	if (! nthread_run_gameloop(FALSE))
		return;

	// run game loop
	game_loop(bStartup);

	// redraw the screen if necessary
	DrawAndBlit();
}
pjw.patch1.end */


//******************************************************************
//******************************************************************
// pjw.patch1.start
#define RAND_MASK (32*1024-1)
#define PLR_BYTES (sizeof(PlayerStruct) * MAX_PLRS)
static PlayerStruct * alloc_plr_chunk(PlayerStruct * p1) {

	// make sure p2 ends up in different locations
	LPVOID pTemp = malloc(rand() & RAND_MASK);
	PlayerStruct * p2 = (PlayerStruct *) malloc(PLR_BYTES);
	if (pTemp) free(pTemp);

	if (! p2) return p1;
	if (p1) {
		CopyMemory(p2,p1,PLR_BYTES);
		free(p1);
	}
	return p2;
}
// pjw.patch1.end


//******************************************************************
//******************************************************************
// pjw.patch1.start
static void alloc_plr() {
	if (NULL == (plr = alloc_plr_chunk(NULL)))
		app_fatal("Unable to initialize memory");
	ZeroMemory(plr,PLR_BYTES);
}
// pjw.patch1.end


//******************************************************************
//******************************************************************
// pjw.patch1.start
#pragma intrinsic(_rotl)
static DWORD calc_checksum(LPVOID lpMem,DWORD dwBytes,DWORD dwSum) {
	LPDWORD lpDW = (LPDWORD) lpMem;
	dwBytes /= sizeof(DWORD);
	while (dwBytes--) {
		dwSum ^= *lpDW++;
		dwSum = _rotl(dwSum,3);
	}
	return dwSum;
}
// pjw.patch1.end


//******************************************************************
//******************************************************************
// pjw.patch1.start
static void CRASH() {
#if 0	// !!! @@@ !!! fix in final
	static BYTE sgbCheating = FALSE;
	if (! sgbCheating) {
		sgbCheating = TRUE;
		app_warning("Cheating detected");
		void myDebugBreak();
		myDebugBreak();
	}
#else
	LPDWORD pFrame = (LPDWORD) plr[myplr]._pAnimData;
	pFrame[plr[myplr]._pAnimFrame + 1] ^= 0x3fffffff;
#endif
	FullBlit(TRUE);
}
// pjw.patch1.end


//******************************************************************
//******************************************************************
// pjw.patch1.start
typedef void (* TCrypt)(LPDWORD,DWORD,DWORD);
static void encrypt_aux(TCrypt fnCrypt) {
	for (int i = 0; i < MAX_PLRS; i++) {
		// SLOW encrypt the important part of the character
		fnCrypt((LPDWORD)&plr[i],offsetof(PlayerStruct,_pGFXLoad),HASH_ENCRYPTKEY);
		
		// FAST encrypt the inventory -- takes too long otherwise
		LPDWORD lpInv = (LPDWORD) &plr[i].InvBody;
		for (int j = sizeof(ItemStruct) * NUM_INVLOC / sizeof(DWORD); j--; )
			*lpInv++ ^= 0xf0638142;

		// pjw.patch2.start -- commented out -- still taking too long
		#if 0
		lpInv = (LPDWORD) &plr[i].InvList;
		for (j = sizeof(ItemStruct) * MAXINV / sizeof(DWORD); j--; )
			*lpInv++ ^= 0x23484862;
		#endif
		// pjw.patch2.end
	}
}
// pjw.patch1.end


//******************************************************************
//******************************************************************
#if 0
// pjw.patch1.start
static void plr_encrypt(BOOL bEncrypt) {
	static BOOL sbEncrypt = FALSE;
	static BOOL sbCrash = FALSE;
	static DWORD sdwCheckSum = 0;
	static DWORD sdwCheckStart = 0;
	if (sbEncrypt == bEncrypt) return;
	sbEncrypt = bEncrypt;

// pjw.patch2.start -- added for debugging only!
#if 0
	static DWORD sgdwEncryptCount = 0;
	sgdwEncryptCount++;
	static DWORD sgdwLastTime = 0;
	if (GetTickCount() - sgdwLastTime >= 1000) {
		sgdwLastTime = GetTickCount();
		do {
			char szBuf[32];
			HDC hDC;
			HRESULT ddr = lpDDSPrimary->GetDC(&hDC);
			if (ddr != DD_OK) break;
			COLORREF oldTextColor = SetTextColor(hDC,RGB(0xff,0xff,0));
			COLORREF oldBkColor = SetBkColor(hDC,RGB(0,0,0));
			int oldBkMode = SetBkMode(hDC,OPAQUE);
			sprintf(szBuf,"%u  ",sgdwEncryptCount);
			sgdwEncryptCount = 0;
			TextOut(hDC,5,385,szBuf,strlen(szBuf));
			SetTextColor(hDC,oldTextColor);
			SetBkColor(hDC,oldBkColor);
			SetBkMode(hDC,oldBkMode);
			lpDDSPrimary->ReleaseDC(hDC);
		} while (0);
	}
#endif
// pjw.patch2.end

	if (sbCrash) {
		CRASH();
	}
	else if (bEncrypt) {
		sdwCheckStart = rand();
		sdwCheckSum = calc_checksum(plr,PLR_BYTES,sdwCheckStart);
		encrypt_aux(Encrypt);
	}
	else {
		encrypt_aux(Decrypt);
		if (sdwCheckSum != calc_checksum(plr,PLR_BYTES,sdwCheckStart)) {
			CRASH();
			sbCrash = 1;
		}
	}

	if ((rand() & 0x0f) == 0x0f)
		plr = alloc_plr_chunk(plr);
}
#endif
// pjw.patch1.end

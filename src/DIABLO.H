/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/DIABLO.H 4     2/10/97 6:22p Dbrevik2 $
**-----------------------------------------------------------------------*/


//******************************************************************
// SOFTWARE VERSIONING
//******************************************************************
// version constants
#define SHAREWARE		1
#define BETA			2
#define RETAIL			3
#define IS_VERSION(x)	(PROGRAM_VERSION == x)

// make sure valid PROGRAM_VERSION is defined
#ifndef PROGRAM_VERSION
#error PROGRAM_VERSION should be defined in Build.Settings.C/C++.Preprocessor
#elif IS_VERSION(SHAREWARE)
//#pragma message("*building shareware")
#elif IS_VERSION(BETA)
//#pragma message("*building beta")
#elif IS_VERSION(RETAIL)
//#pragma message("*building retail")
#else
#error PROGRAM_VERSION is invalid
#endif



// collin's new RLE draw code -- must have
// reprocessed .CL2 files for this to work
#define RLE_DRAW		1	// 1 in final


// cheats compile flag 0 == off, 1 == on
#define CHEATS			1	// 0 in final
#ifdef NDEBUG
#undef CHEATS
#define CHEATS 0
#endif

// misc 0 == testing, 1 == normal
#define RELEASE			1	// 1 in final
#ifdef NDEBUG
#undef RELEASE
#define RELEASE 1
#endif

// 1 = allow debugging, 0 = release
#define ALLOW_WINDOWED_MODE		1	// 0 in final
#ifdef NDEBUG
#undef ALLOW_WINDOWED_MODE
#define ALLOW_WINDOWED_MODE 0
#endif

// 1 = show network debugging info
#define TRACEOUT	1	// 0 in final
#ifdef NDEBUG
#undef TRACEOUT
#define TRACEOUT 0
#endif


// 1 = show current trace function, 0 = release
#define ALLOW_TRACE_FCN			0	// 0 in final
#ifdef NDEBUG
#undef ALLOW_TRACE_FCN
#define ALLOW_TRACE_FCN 0
#endif

#if ALLOW_TRACE_FCN
void trace_fcn(const char * pszFcn);
#define TRACE_FCN(x) trace_fcn(x)
#else
#define TRACE_FCN(x) NULL
#endif


// save file "version"
// so we don't have version number conflicts:
// - EVEN number if RETAIL/SHAREWARE version
// - ODD number if BETA version
#define SAVE_GAME_KEY	0x7058
#if IS_VERSION(RETAIL) && ((SAVE_GAME_KEY & 1) != 0)
	#error -- SAVE_GAME_KEY must be EVEN for RETAIL version
#endif
#if IS_VERSION(BETA) && ((SAVE_GAME_KEY & 1) == 0)
	#error -- SAVE_GAME_KEY must be ODD for BETA version
#endif


// EVEN version ID = retail version
// ODD version ID = beta version
#define VERSIONID	34
#if IS_VERSION(RETAIL) && ((VERSIONID & 1) != 0)
	#error -- VERSIONID must be EVEN for RETAIL version
#endif
#if IS_VERSION(BETA) && ((VERSIONID & 1) == 0)
	#error -- VERSIONID must be ODD for BETA version
#endif


#ifndef PROGRAM_VERSION
#error PROGRAM_VERSION not defined
#elif IS_VERSION(RETAIL)
//#define PROGRAMID 'DRTL'
#define PROGRAMID 'HRTL'
#elif IS_VERSION(SHAREWARE)
#define PROGRAMID 'DSHR'
#elif IS_VERSION(BETA)
#define PROGRAMID 'DIAB'
#else
#error -- VERSION NOT DEFINED
#endif


/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/
#define MAX_PLRS	4

// frame rate
#define GAME_FRAMES_PER_SECOND	20

// Our messages
#define WM_DIABNEXTLVL	WM_USER+2
#define WM_DIABPREVLVL	WM_USER+3
#define WM_DIABRTNLVL	WM_USER+4
#define WM_DIABSETLVL	WM_USER+5
#define WM_DIABWARPLVL	WM_USER+6
#define WM_DIABTOWNWARP	WM_USER+7
#define WM_DIABTWARPUP	WM_USER+8
#define WM_DIABRETOWN	WM_USER+9
#define WM_DIABNEWGAME	WM_USER+10
#define WM_DIABLOADGAME	WM_USER+11


// Screen size
#define TOTALX		640
#define TOTALY		480

// Size of control panel
#define CTRLPANY	128

// Size of game play area
#define GAMEY		352

// Offscreen buffer size
#define BUFFERX		768
#define BUFFERY		656
#define BUFFERSIZE	BUFFERX*BUFFERY

#define BTMBUFFX			640
#define BTMBUFFY			144
#define BTMBUFFSIZE			BTMBUFFX*BTMBUFFY
#define BTMBUFFMULTISIZE	BTMBUFFX*BTMBUFFY*2

// Used in processing players, monsters, objects etc.
#define RUN_DONE	0
#define RUN_AGAIN	1

#define MAX_LEVELS 			24
#define DIABLO_LEVEL		16
#define SLAIN_HERO_LEVEL	9
#define STORY_BOOK1_LEVEL	4
#define STORY_BOOK2_LEVEL	8
#define STORY_BOOK3_LEVEL	12

// JKE STORY BOOKS FOR HELLFIRE
#define SKULKEN_BOOK1_LEVEL	21	
#define SKULKEN_BOOK2_LEVEL	22	
#define SKULKEN_BOOK3_LEVEL	23	

/*-----------------------------------------------------------------------**
** Included files
**-----------------------------------------------------------------------*/
#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ddraw.h"
#include "dsound.h"
#include <time.h>
#include <direct.h>
#include <errno.h>


/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/
extern HWND ghMainWnd;
extern HINSTANCE ghInst;
extern const char gszAppName[];

// DO NOT USE gbFontTransTbl directly -- use the macro
// to prevent sign extension problems with characters!!!
extern const BYTE gbFontTransTbl[];
#define char2print(c) gbFontTransTbl[(BYTE) (c)];


// video vars
extern LPDIRECTDRAW lpDD;
extern LPDIRECTDRAWSURFACE lpDDSPrimary;
extern LPDIRECTDRAWPALETTE lpDDPal;
extern BOOL fullscreen;
extern BOOL bActive;

// these variables are only valid if lock_buf() has been called
extern BYTE * gpBuffer;
extern "C" long glClipY;
void lock_buf(BYTE bFcn);
void unlock_buf(BYTE bFcn);

// program vars
extern BOOL svgamode;
extern int MouseX, MouseY;
extern int force_redraw;


// Temp vars (delete all uses before final compile)
extern long gv1;
extern long gv2;
extern long gv3;
extern long gv4;
extern long gv5;


// General flags
extern BOOL PauseMode;
extern BOOL gbProcessPlayers;
extern BOOL FriendlyMode;
#if CHEATS
extern BOOL davedebug;
extern BOOL cheatflag;
extern BOOL simplecheat;
#endif
extern BOOL visiondebug;
extern BOOL light4flag;
extern BOOL leveldebug;
extern BOOL monstdebug;
extern int debugmonsttypes;
extern int DebugMonsters[10];


/*----------------------------------------------------------*/
//		HELLFIRE FLAGS
/*----------------------------------------------------------*/

extern bool gbTheo;
extern bool gbCowsuit;
extern bool gbOurNest;
extern bool gbAllowBard;
extern bool gbAllowBarbarian;
extern bool gbAllowMultiPlayer;
/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/
LRESULT CALLBACK DiabloDefProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
void LoadGameLevel(BOOL, int);
void FreeGameMem();
void SetupSaveBasePath();


#if TRACEOUT
void __cdecl TraceOut(const char * pszFmt, ...);
#endif


/*-----------------------------------------------------------------------**
** Assertion System
**-----------------------------------------------------------------------*/
#define EXTENDED_ASSERT		1	// 0 in final
#ifdef NDEBUG
#undef EXTENDED_ASSERT
#define EXTENDED_ASSERT 0
#endif

const TCHAR * strGetLastError();
const TCHAR * strGetError(DWORD dwErr);

void __cdecl app_fatal(const char * pszFmt,...);
void __cdecl app_warning(const char * pszFmt,...);

void app_assert(int);
#if EXTENDED_ASSERT && !defined(NDEBUG)
void assert_fail(int nLineNo, const char * pszFile, const char * pszFail);
#define app_assert(x) ((x) ? NULL : assert_fail(__LINE__,__FILE__,#x))
#elif !defined(NDEBUG)
void assert_fail(int nLineNo, const char * pszFile);
#define app_assert(x) ((x) ? NULL : assert_fail(__LINE__,__FILE__))
#else
#define app_assert(x) (x)	// in case of side effects
#endif

void ddraw_assert(int);
void ddraw_assert_fail(HRESULT ddrval, int nLineNo, const char * pszFile);
#define ddraw_assert(x) (((x) == DD_OK) ? NULL : ddraw_assert_fail((x),__LINE__,__FILE__))

void dsound_assert(int);
void dsound_assert_fail(HRESULT dsrval, int nLineNo, const char * pszFile);
#define dsound_assert(x) (((x) == DS_OK) ? NULL : dsound_assert_fail((x),__LINE__,__FILE__))

// jcm.patch1.start.1/14/97
#ifdef NDEBUG
#define GRACEFUL_EXIT
#endif
// jcm.patch1.end.1/14/97

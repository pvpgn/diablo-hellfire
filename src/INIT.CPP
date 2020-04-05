//******************************************************************
// init.cpp
//******************************************************************


#include "diablo.h"
#pragma hdrstop
#include <shlobj.h>
#include "storm.h"
#include "palette.h"
#include "engine.h"
#include "gendung.h"
#include "lighting.h"
#include "multi.h"
#include "sound.h"
#include "effects.h"
#include "diabloui.h"
#include "resource.h"


//******************************************************************
// debugging
//******************************************************************
// minor version number -- as in 1.xx
// jmm.patch3
#define MINOR_VERSION		"04"
// jmm.endpatch3

// change to "01" when done with patch
#define HF_MINOR_VERSION	"01"


#define DIRECT_FILE_ACCESS		1	// 0 in final
#ifdef NDEBUG
#undef DIRECT_FILE_ACCESS
#define DIRECT_FILE_ACCESS 0
#endif


//******************************************************************
// extern
//******************************************************************
void init_directx(HWND hWnd);
void free_directx();
void ddraw_switch_modes();
void BlackPalette();
void ReleasePlayerFile();
void FileErrorDlg(const char * pszName);


//******************************************************************
// public
//******************************************************************
BOOL                    bActive;        // is application active?
const char gszAppName[] = "HELLFIRE";
const char gszDiabloName[] = "DIABLO";
HSARCHIVE ghsMainArchive;
HSARCHIVE ghsHFBardArchive = NULL;
HSARCHIVE ghsHFBarbarianArchive = NULL;


// Version string --- NOTE: DO NOT CHANGE THIS VERSION
// NUMBER IN THE PROGRAM, CHANGE IT IN THE RESOURCE FILE!
char gszVersionNumber[MAX_PATH] = "internal version unknown";
char gszPrintVersion[MAX_PATH] = "Hellfire v1." HF_MINOR_VERSION ; // " (from Diablo v1." MINOR_VERSION ")";
SNETVERSIONDATA gVersion;
static char sgszProgramName[MAX_PATH];
static char sgszMainArchiveName[MAX_PATH];
static char sgszPatchArchiveName[MAX_PATH];
static char sgszHFArchiveName[MAX_PATH];
static char sgszHFMonkArchiveName[MAX_PATH];
static char sgszHFBardArchiveName[MAX_PATH];
static char sgszHFBarbarianArchiveName[MAX_PATH];
static char sgszHFMusicArchiveName[MAX_PATH];
static char sgszHFVoiceArchiveName[MAX_PATH];
static char sgszHFOpt1ArchiveName[MAX_PATH];
static char sgszHFOpt2ArchiveName[MAX_PATH];


//******************************************************************
// private
//******************************************************************
static WNDPROC sgWndProc;
static LRESULT CALLBACK WndProc(HWND ,UINT ,WPARAM ,LPARAM );
static HSARCHIVE sghsPatchArchive;
static HSARCHIVE sghsHFArchive;
static HSARCHIVE sghsHFMonkArchive;
static HSARCHIVE sghsHFMusicArchive;
static HSARCHIVE sghsHFVoiceArchive;
static HSARCHIVE sghsHFOpt1Archive;
static HSARCHIVE sghsHFOpt2Archive;

static BOOL killedmom = 0;
#define  MOMLINKNAME  "Microsoft Office Shortcut Bar.lnk"


//******************************************************************
//******************************************************************
static void SearchDirectory(LPCSTR directory) {
  char searchspec[MAX_PATH];
  strcpy(searchspec,directory);
  if ((!searchspec[0]) ||(searchspec[strlen(searchspec)-1] != '\\'))
    strcat(searchspec,"\\*");
  else
    strcat(searchspec,"*");
  WIN32_FIND_DATA finddata;
  HANDLE          findhandle = FindFirstFile(searchspec,&finddata);
  if (findhandle != INVALID_HANDLE_VALUE) {
    do
      if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        if (strcmp(finddata.cFileName,".") && strcmp(finddata.cFileName,"..")) {
          char buffer[MAX_PATH] = "";
          if ((!directory[0]) ||(directory[strlen(directory)-1] != '\\'))
            sprintf(buffer,"%s\\%s\\",directory,finddata.cFileName);
          else
            sprintf(buffer,"%s%s\\",directory,finddata.cFileName);
          SearchDirectory(buffer);
        }
      }
      else {
        if (!_stricmp(finddata.cFileName,MOMLINKNAME))
          ShellExecute(GetDesktopWindow(),"open",finddata.cFileName,"",directory,SW_SHOWNORMAL);
      }
    while (FindNextFile(findhandle,&finddata));
    FindClose(findhandle);
  }
}


//******************************************************************
//******************************************************************
static HWND find_mom_window() {
	HWND hWnd = GetForegroundWindow();
	while (hWnd) {
		char classname[256];
		GetClassName(hWnd,classname,255);
		if (! _stricmp(classname,"MOM Parent")) break;
		hWnd = GetNextWindow(hWnd,GW_HWNDNEXT);
	}

	return hWnd;
}

//******************************************************************
//******************************************************************
static void KillMom() {
	HWND hWnd;
	if (NULL != (hWnd = find_mom_window())) {
		PostMessage(hWnd,WM_CLOSE,0,0);
		killedmom = 1;
	}
}


//******************************************************************
//******************************************************************
static void WaitMomDead() {
	HWND hWnd;
	DWORD dwCurrTime = GetTickCount();
	while (NULL != (hWnd = find_mom_window())) {
		Sleep(250);
		if (GetTickCount() - dwCurrTime > 4000)
			break;
	}
}


//******************************************************************
//******************************************************************
static void ResurrectMom() {
  if (!killedmom)
    return;
  killedmom = 0;
  char buffer[256] = "";
  LPITEMIDLIST idlist = NULL;
  if (SHGetSpecialFolderLocation(GetDesktopWindow(),CSIDL_STARTMENU,&idlist) == NOERROR) {
    SHGetPathFromIDList(idlist,buffer);
    SearchDirectory(buffer);
  }
}


//******************************************************************
//******************************************************************
static void disable_screen_saver(BYTE bDisable) {
// direct draw doesn't like the screen saver to be enabled, it
// will quit with a fatal error if the screen saver kicks in.
// Disable the screen saver while the program is running
	HKEY hKey;
	BYTE bNewState;
	DWORD dwSuccess;
	TCHAR szBuf[16];
	static BYTE sbState = FALSE;
	static const TCHAR scszKey[] = TEXT("ScreenSaveActive");

	// open master key
	dwSuccess = RegOpenKeyEx(HKEY_CURRENT_USER,TEXT("Control Panel\\Desktop"),0,KEY_READ | KEY_WRITE,&hKey);
	if (dwSuccess != ERROR_SUCCESS) return;

	if (bDisable) {
		// get current screen saver state
		DWORD dwType;
		DWORD dwSize = sizeof(szBuf);
		dwSuccess = RegQueryValueEx(hKey,scszKey,NULL,&dwType,(LPBYTE) szBuf,&dwSize);
		if (dwSuccess == ERROR_SUCCESS) sbState = szBuf[0] != TEXT('0');
		bNewState = 0;
	}
	else {
		// restore old screen saver state
		bNewState = sbState;
	}

	// set the new state
	szBuf[0] = bNewState ? TEXT('1') : TEXT('0');
	szBuf[1] = 0;
	RegSetValueEx(hKey,scszKey,NULL,REG_SZ,(LPBYTE) szBuf,2 * sizeof(szBuf[0]));

	RegCloseKey(hKey);
}


//******************************************************************
//******************************************************************
/*
static void key_press(int key)
{
	// Simulate a key press
	keybd_event( key,
			   0x45,
			   KEYEVENTF_EXTENDEDKEY | 0,
			   0 );

	// Simulate a key release
	keybd_event( VK_CAPITAL,
			   0x45,
			   KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
			   0);
}
*/

//******************************************************************
//******************************************************************
/*
static void init_caps_lock(BOOL init)
{
	BYTE keyState[256];
	static BOOL capslocked;

	GetKeyboardState((LPBYTE)&keyState);
	if(init)
	{
		capslocked = keyState[VK_CAPITAL] & 1;
		// Set caps lock off initially
		// NOTE: FriendlyMode is opposite polarity to caps_lock, i.e., it is originally ON
		if(capslocked)
			key_press(VK_CAPITAL);
	}
	else {		// restore caps lock to originial state
		if( (capslocked && !(keyState[VK_CAPITAL] & 1)) ||
		   (!capslocked && (keyState[VK_CAPITAL] & 1)) )
			key_press(VK_CAPITAL);
	}
}
*/


//******************************************************************
//******************************************************************
void cleanup(BOOL bNormalExit) {

	ReleasePlayerFile();
	disable_screen_saver(FALSE);
	// init_caps_lock(FALSE);
	ResurrectMom();

	if (ghsMainArchive) {
		SFileCloseArchive(ghsMainArchive);
		ghsMainArchive = NULL;
	}
	if (sghsPatchArchive) {
		SFileCloseArchive(sghsPatchArchive);
		sghsPatchArchive = NULL;
	}
	if (sghsHFArchive) {
		SFileCloseArchive(sghsHFArchive);
		sghsHFArchive = NULL;
	}
	if (sghsHFMonkArchive) {
		SFileCloseArchive(sghsHFMonkArchive);
		sghsHFMonkArchive = NULL;
	}
	if (ghsHFBardArchive) {
		SFileCloseArchive(ghsHFBardArchive);
		ghsHFBardArchive = NULL;
	}
	if (ghsHFBarbarianArchive) {
		SFileCloseArchive(ghsHFBarbarianArchive);
		ghsHFBarbarianArchive = NULL;
	}
	if (sghsHFMusicArchive) {
		SFileCloseArchive(sghsHFMusicArchive);
		sghsHFMusicArchive = NULL;
	}
	if (sghsHFVoiceArchive) {
		SFileCloseArchive(sghsHFVoiceArchive);
		sghsHFVoiceArchive = NULL;
	}
	if (sghsHFOpt1Archive) {
		SFileCloseArchive(sghsHFOpt1Archive);
		sghsHFOpt1Archive = NULL;
	}
	if (sghsHFOpt2Archive) {
		SFileCloseArchive(sghsHFOpt2Archive);
		sghsHFOpt2Archive = NULL;
	}

	UiDestroy();
	sound_exit();
	snd_exit();
	NetClose();
	free_directx();
	mem_cleanup(bNormalExit);
	StormDestroy();
	if (bNormalExit) ShowCursor(TRUE);
}


//******************************************************************
//******************************************************************
static void remove_trailing_bslash(char * pszPath) {
	char * pszTemp = strrchr(pszPath,'\\');
	if (pszTemp && !pszTemp[1])
		*pszTemp = 0;
}


//******************************************************************
//******************************************************************
static BOOL FindCDArchive(
	char szName[MAX_PATH],			// saved full path
	const char * pszArchName,		// default archive name
	DWORD dwPriority,				// priority
	HSARCHIVE * phsArchive			// archive name
) {
	// get a list of drives, and figure out which ones are CDROM
	char szDriveList[MAX_PATH];
	DWORD dwLen = GetLogicalDriveStrings(MAX_PATH,szDriveList);
	if (! dwLen) return FALSE;
	if (dwLen > MAX_PATH) return FALSE;

	// skip over leading bslash in archive name
	while (*pszArchName == '\\') pszArchName++;

	const char * pszDriveList = szDriveList;
	while (*pszDriveList) {
		// save current drive
		const char * pszCurrDrive = pszDriveList;

		// skip over drive string and trailing NULL
		while (*pszDriveList++) NULL;

		// is this a CDROM drive?
		if (DRIVE_CDROM != GetDriveType(pszCurrDrive))
			continue;

		strcpy(szName,pszCurrDrive);
		strcat(szName,pszArchName);
		if (SFileOpenArchive(szName,dwPriority,TRUE,phsArchive))
			return TRUE;
	}

	return FALSE;
}


//******************************************************************
//******************************************************************
static HSARCHIVE open_1_archive(
	char szName[MAX_PATH],			// saved full path
	const char * pszArchName,		// default archive name
	const char * pszArchPathRegKey,	// registry key for directory
	DWORD dwPriority,				// priority
	BOOL bCDOnly					// TRUE => CD ROM only
) {
	HSARCHIVE hArchive;

	// get current directory
	char szCurrDir[MAX_PATH];
	if (! GetCurrentDirectory(MAX_PATH,szCurrDir))
		app_fatal("Can't get program path");
	remove_trailing_bslash(szCurrDir);
	if (! SFileSetBasePath(szCurrDir))
		app_fatal("SFileSetBasePath");

	// get program directory
	char szProgDir[MAX_PATH];
	if (! GetModuleFileName(ghInst,szProgDir,MAX_PATH))
		app_fatal("Can't get program name");
	char * pszExeName = strrchr(szProgDir,'\\');
	if (pszExeName) *pszExeName = 0;
	remove_trailing_bslash(szProgDir);

	// try the current directory
	strcpy(szName,szCurrDir);
	strcat(szName,pszArchName);
	if (SFileOpenArchive(
		szName,
		dwPriority,
		#if DIRECT_FILE_ACCESS
		FALSE,
		#else
		bCDOnly,
		#endif
		&hArchive
	)) return hArchive;

	// try the program directory
	if (strcmp(szProgDir,szCurrDir)) {
		strcpy(szName,szProgDir);
		strcat(szName,pszArchName);
		if (SFileOpenArchive(
			szName,
			dwPriority,
			#if DIRECT_FILE_ACCESS
			FALSE,
			#else
			bCDOnly,
			#endif
			&hArchive
		)) return hArchive;
	}

	// get CD directory
	char szDataDir[MAX_PATH];
	szDataDir[0] = 0;
	if (pszArchPathRegKey && SRegLoadString(TEXT("Archives"),pszArchPathRegKey,0,szDataDir,MAX_PATH)) {
		// try the data directory
		remove_trailing_bslash(szDataDir);
		strcpy(szName,szDataDir);
		strcat(szName,pszArchName);
		if (SFileOpenArchive(
			szName,
			dwPriority,
			#if DIRECT_FILE_ACCESS
			FALSE,
			#else
			bCDOnly,
			#endif
			&hArchive
		)) return hArchive;
	}

	// if this file is to be found on a CDROM, search *all* CDROMs
	// don't pass szName, because in case of failure by FindCDArchive,
	// it already contains the name of the desired archive, which 
	// STORM wants
	if (bCDOnly && FindCDArchive(szDataDir,pszArchName,dwPriority,&hArchive)) {
		strcpy(szName,szDataDir);
		return hArchive;
	}

	// we couldn't open the file, but leave the szName variable
	// filled in with the program directory + archive name
	return NULL;
}


//******************************************************************
//******************************************************************
static void get_program_version_info() {
	// get name of .EXE file
	if (! GetModuleFileName(ghInst,sgszProgramName,MAX_PATH))
		return;

	// get sizeof version structure to allocate
	DWORD dwUnused;
	DWORD dwVerLen = GetFileVersionInfoSize(sgszProgramName,&dwUnused);
	if (! dwVerLen) return;

	// get version info
	LPVOID lpData = DiabloAllocPtrSig(dwVerLen,'VERS');
	if (! GetFileVersionInfo(sgszProgramName,0,dwVerLen,lpData))
		goto cleanup;

	UINT uBytes;
	VS_FIXEDFILEINFO * pInfo;
	if (! VerQueryValue(lpData,TEXT("\\"),(LPVOID *) &pInfo,&uBytes))
		goto cleanup;
	app_assert(uBytes >= sizeof(VS_FIXEDFILEINFO));

	sprintf(
		gszVersionNumber,
		"version %d.%d.%d.%d",
		pInfo->dwProductVersionMS >> 16,
		pInfo->dwProductVersionMS & 0x0ffff,
		pInfo->dwProductVersionLS >> 16,
		pInfo->dwProductVersionLS & 0x0ffff
	);

cleanup:
	DiabloFreePtr(lpData);
}


//******************************************************************
//******************************************************************
static void open_archives() {
	// setup version info
	ZeroMemory(&gVersion,sizeof(gVersion));
	gVersion.size = sizeof(gVersion);
	gVersion.versionstring = gszVersionNumber;
	gVersion.executablefile = sgszProgramName;
	gVersion.originalarchivefile = sgszMainArchiveName;
	gVersion.patcharchivefile = sgszPatchArchiveName;

	// fill in program name and version string
	get_program_version_info();

	while (1) {
		// open main archive
		ghsMainArchive = open_1_archive(
			sgszMainArchiveName,	// saved full path
			#if IS_VERSION(SHAREWARE)
			TEXT("\\spawn.mpq"),	// default archive name
			TEXT("DiabloSpawn"),	// key for spawned directory
			#else
			TEXT("\\diabdat.mpq"),	// default archive name
			TEXT("DiabloCD"),		// key for CDROM directory
			#endif
			1000,					// priority
			#if IS_VERSION(SHAREWARE)
			FALSE					// TRUE == CD ROM only
			#else
			TRUE					// TRUE == CD ROM only
			#endif
		);
		if (ghsMainArchive) break;

		#if DIRECT_FILE_ACCESS
			// we're in debugging mode, so we can just exit
			break;
		#endif

	#if IS_VERSION(SHAREWARE)
		// couldn't find spawn.mpq file
		break;
	#else
		// tell the user to insert the CD
		DWORD dwResult;
		UiCopyProtError(&dwResult);
		if (dwResult == COPYPROT_CANCEL)
			FileErrorDlg("diabdat.mpq");
	#endif
	}

	// make sure we have access to our data
	HSFILE hsFile;
	if (! patSFileOpenFile("ui_art\\title.pcx",&hsFile,TRUE)) {
		#if IS_VERSION(SHAREWARE)
		FileErrorDlg("Main program archive: spawn.mpq");
		#else
		FileErrorDlg("Main program archive: diabdat.mpq");
		#endif
	}
	patSFileCloseFile(hsFile);

	// open patch file
	sghsPatchArchive = open_1_archive(
		sgszPatchArchiveName,	// saved full path
		#if IS_VERSION(SHAREWARE)
		TEXT("\\patch_sh.mpq"),	// default archive name
		TEXT("DiabloSpawn"),	// key for spawned directory
		#else
		TEXT("\\patch_rt.mpq"),	// default archive name
		TEXT("DiabloInstall"),	// key for installed directory
		#endif
		2000,					// priority
		FALSE					// TRUE == CD ROM only
	);
	// open Hellfire file
	sghsHFArchive = open_1_archive(
		sgszHFArchiveName,	// saved full path
		TEXT("\\hellfire.mpq"),	// default archive name
		TEXT("DiabloInstall"),	// key for installed directory
		8000,					// priority
		FALSE					// TRUE == CD ROM only
	);
	// open Hellfire Character file
	sghsHFMonkArchive = open_1_archive(
		sgszHFMonkArchiveName,	// saved full path
		TEXT("\\hfmonk.mpq"),	// default archive name
		TEXT("DiabloInstall"),	// key for installed directory
		8100,					// priority
		FALSE					// TRUE == CD ROM only
	);
	// open Hellfire Character file
	ghsHFBardArchive = open_1_archive(
		sgszHFBardArchiveName,	// saved full path
		TEXT("\\hfbard.mpq"),	// default archive name
		TEXT("DiabloInstall"),	// key for installed directory
		8110,					// priority
		FALSE					// TRUE == CD ROM only
	);
	// open Hellfire Character file
	ghsHFBarbarianArchive = open_1_archive(
		sgszHFBarbarianArchiveName,	// saved full path
		TEXT("\\hfbarb.mpq"),	// default archive name
		TEXT("DiabloInstall"),	// key for installed directory
		8120,					// priority
		FALSE					// TRUE == CD ROM only
	);
	// open Hellfire Music file
	sghsHFMusicArchive = open_1_archive(
		sgszHFMusicArchiveName,	// saved full path
		TEXT("\\hfmusic.mpq"),	// default archive name
		TEXT("DiabloInstall"),	// key for installed directory
		8200,					// priority
		FALSE					// TRUE == CD ROM only
	);
	// open Hellfire Voice file
	sghsHFVoiceArchive = open_1_archive(
		sgszHFVoiceArchiveName,	// saved full path
		TEXT("\\hfvoice.mpq"),	// default archive name
		TEXT("DiabloInstall"),	// key for installed directory
		8500,					// priority
		FALSE					// TRUE == CD ROM only
	);
	// open Hellfire Option file #1
	sghsHFOpt1Archive = open_1_archive(
		sgszHFOpt1ArchiveName,	// saved full path
		TEXT("\\hfopt1.mpq"),	// default archive name
		TEXT("DiabloInstall"),	// key for installed directory
		8600,					// priority
		FALSE					// TRUE == CD ROM only
	);
	// open Hellfire Option file #2
	sghsHFOpt2Archive = open_1_archive(
		sgszHFOpt2ArchiveName,	// saved full path
		TEXT("\\hfopt2.mpq"),	// default archive name
		TEXT("DiabloInstall"),	// key for installed directory
		8610,					// priority
		FALSE					// TRUE == CD ROM only
	);
}


//******************************************************************
//******************************************************************
void init_window(int nCmdShow) {
	KillMom();

	void check_disk_space();
	check_disk_space();

    // set up and register window class
    WNDCLASSEX wc;
	ZeroMemory(&wc,sizeof(wc));
	wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = ghInst;
    wc.hIcon = LoadIcon(ghInst,MAKEINTRESOURCE(IDI_ICON1));
    wc.hCursor = LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = gszAppName;
    // So the Diablo auto run won't.
    wc.lpszClassName = gszDiabloName; //gszAppName;
	wc.hIconSm = (HICON) LoadImage(ghInst,MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,16,16,0);
	if (! RegisterClassEx(&wc))
		app_fatal("Unable to register window class");

#if ALLOW_WINDOWED_MODE
	int nWdt = 640;
	int nHgt = 480;
#else
	int nWdt = max(640,GetSystemMetrics(SM_CXSCREEN));
	int nHgt = max(480,GetSystemMetrics(SM_CYSCREEN));
#endif
    
    // create main window
    HWND hWnd = CreateWindow(
        gszDiabloName,
		gszAppName,
		WS_POPUP,
        0,
        0,
        nWdt,
        nHgt,
        NULL,
        NULL,
        ghInst,
        NULL
	);
	if (! hWnd) app_fatal("Unable to create main window");
	ShowWindow(hWnd,SW_SHOWNORMAL);
    UpdateWindow(hWnd);

	WaitMomDead();
	init_directx(hWnd);
	BlackPalette();
	snd_init(hWnd);
	open_archives();
	disable_screen_saver(TRUE);
	// init_caps_lock(TRUE);
}


//******************************************************************
//******************************************************************
static void app_activate(HWND hWnd,WPARAM wParam) {
	bActive = wParam;
	UiAppActivate(wParam);

	// make our 16x16 icon show up on the taskbar
	// -- have to have WM_SYSMENU set for the icon to show up
	// -- don't want WM_SYSMENU during fullscreen mode, otherwise
	//    menu will pop up during gameplay!
	DWORD dwStyle = GetWindowLong(hWnd,GWL_STYLE);
	if (bActive && fullscreen)
		dwStyle &= ~WS_SYSMENU;
	else
		dwStyle |= WS_SYSMENU;
	SetWindowLong(hWnd,GWL_STYLE,dwStyle);

	if (! bActive) return;
	force_redraw = FULLDRAW;
	ResetPal();
}


//******************************************************************
//******************************************************************
LRESULT CALLBACK DiabloDefProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	switch(uMsg) {
		#if ALLOW_WINDOWED_MODE
	    case WM_SYSKEYUP:
			if (wParam == VK_RETURN) {
				fullscreen = !fullscreen;
				ddraw_switch_modes();
				return 0;
			}
		break;
		#endif

		case WM_CLOSE:
		return 0;

		case WM_ERASEBKGND:
			// ignore erase messages
		return 0;

		case WM_PAINT:
			force_redraw = FULLDRAW;
		break;
			
	    case WM_ACTIVATEAPP:
			app_activate(hWnd,wParam);
		break;

		case WM_QUERYNEWPALETTE:
			SDrawRealizePalette();
		return TRUE;

		case WM_PALETTECHANGED:
		// pjw.patch1.start
		//	if (bActive && (HWND)wParam != hWnd)
			if ((HWND)wParam != hWnd)
		// pjw.patch1.end
				SDrawRealizePalette();
		break;

		case WM_CREATE:
			ghMainWnd = hWnd;
		break;

		case WM_DESTROY:
			cleanup(TRUE);
			ghMainWnd = NULL;
			PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}


//******************************************************************
//******************************************************************
static LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	if (sgWndProc) return sgWndProc(hWnd,uMsg,wParam,lParam);
	return DiabloDefProc(hWnd,uMsg,wParam,lParam);
}


//******************************************************************
//******************************************************************
WNDPROC my_SetWindowProc(WNDPROC wndProc) {
// we can't use SetWindowLong, because DirectDraw won't properly support
// stuff like alt-tabbing if we override the funky stuff it does to the
// window procedure.
	WNDPROC tempProc = sgWndProc;
	sgWndProc = wndProc;
	return tempProc;
}

//***********************************************************************
//   Assertion System
//
//   Copyright (c) 1996 by Blizzard Entertainment.
//   All rights reserved.
//***********************************************************************


#include "diablo.h"
#pragma hdrstop
#include <tchar.h>
#include "storm.h"
#include "resource.h"


//***********************************************************************
// Externals
//***********************************************************************
void cleanup(BOOL bNormalExit);


//***********************************************************************
//***********************************************************************
/*
void DebugDump2(const char * pszFmt,va_list args) {
	static FILE * f = NULL;
	if (! f) f = fopen("c:\\hellfr__.dbg","wt");
	if (! f) return;
	vfprintf(f,pszFmt,args);
	fflush(f);
}
void __cdecl DebugDump(const char * pszFmt, ...) {
	va_list args;
	va_start(args,pszFmt);
	DebugDump2(pszFmt,args);
	va_end(args);
}
*/


//***********************************************************************
// WARNING:  the code below ONLY works on x86 compatible systems
//***********************************************************************
#ifdef _X86_
#ifndef NDEBUG
static LONG WINAPI BreakExceptionHdlr(struct _EXCEPTION_POINTERS *pep) {
	// if we got a breakpoint exception, we expected it -- keep running
	PEXCEPTION_RECORD per = pep->ExceptionRecord;
	if (per && per->ExceptionCode == EXCEPTION_BREAKPOINT) {
		// in windows 95, the int3 instruction will already be
		// skipped by the time we get here.  In windows NT, the
		// Eip will still point to the int3 instruction.  Therefore
		// look at the offending byte, and if it is int3 (0xcc) then
		// skip over the instruction
		BYTE * pInst = (BYTE *) pep->ContextRecord->Eip;
		if (*pInst == 0xcc) pep->ContextRecord->Eip += 1;

		// continue execution
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	return EXCEPTION_CONTINUE_SEARCH;
}
#endif
#endif


//***********************************************************************
//***********************************************************************
void myDebugBreak() {
#ifndef NDEBUG
	// -- Save current exception handler and set it
	// to a handler which expects a break to occur.
	// -- If we are in the debugger, the debugger will
	// override our exception handler, and we will
	// drop into the debugger.
	// -- If there is no debugger present, our exception
	// handler will skip over the break instruction
	// and allow normal execution of the program
	LPTOP_LEVEL_EXCEPTION_FILTER lpLastHdlr;
	lpLastHdlr = SetUnhandledExceptionFilter(BreakExceptionHdlr);

	// drop into the debugger
	__asm int 3

	// restore exception handler
	SetUnhandledExceptionFilter(lpLastHdlr);
#endif
}


//***********************************************************************
//***********************************************************************
static void get_ddraw_error(HRESULT ddrval,TCHAR * pszBuf,DWORD dwMaxChars) {
	const TCHAR * pszErr;

	// @@ eventually we should get these from our resource file
	// so that they can be properly translated and so
	// they don't have to stay loaded all the time

	switch (ddrval) {
		case DD_OK:
			pszErr = "DD_OK";
		break;
		case DDERR_ALREADYINITIALIZED:
			pszErr = "DDERR_ALREADYINITIALIZED";
		break;
		case DDERR_BLTFASTCANTCLIP:
			pszErr = "DDERR_BLTFASTCANTCLIP";
		break;
		case DDERR_CANNOTATTACHSURFACE:
			pszErr = "DDERR_CANNOTATTACHSURFACE";
		break;
		case DDERR_CANNOTDETACHSURFACE:
			pszErr = "DDERR_CANNOTDETACHSURFACE";
		break;
		case DDERR_CANTCREATEDC:
			pszErr = "DDERR_CANTCREATEDC";
		break;
		case DDERR_CANTDUPLICATE:
			pszErr = "DDERR_CANTDUPLICATE";
		break;
		case DDERR_CLIPPERISUSINGHWND:
			pszErr = "DDERR_CLIPPERISUSINGHWND";
		break;
		case DDERR_COLORKEYNOTSET:
			pszErr = "DDERR_COLORKEYNOTSET";
		break;
		case DDERR_CURRENTLYNOTAVAIL:
			pszErr = "DDERR_CURRENTLYNOTAVAIL";
		break;
		case DDERR_DIRECTDRAWALREADYCREATED:
			pszErr = "DDERR_DIRECTDRAWALREADYCREATED";
		break;
		case DDERR_EXCEPTION:
			pszErr = "DDERR_EXCEPTION";
		break;
		case DDERR_EXCLUSIVEMODEALREADYSET:
			pszErr = "DDERR_EXCLUSIVEMODEALREADYSET";
		break;
		case DDERR_GENERIC:
			pszErr = "DDERR_GENERIC";
		break;
		case DDERR_HEIGHTALIGN:
			pszErr = "DDERR_HEIGHTALIGN";
		break;
		case DDERR_HWNDALREADYSET:
			pszErr = "DDERR_HWNDALREADYSET";
		break;
		case DDERR_HWNDSUBCLASSED:
			pszErr = "DDERR_HWNDSUBCLASSED";
		break;
		case DDERR_IMPLICITLYCREATED:
			pszErr = "DDERR_IMPLICITLYCREATED";
		break;
		case DDERR_INCOMPATIBLEPRIMARY:
			pszErr = "DDERR_INCOMPATIBLEPRIMARY";
		break;
		case DDERR_INVALIDCAPS:
			pszErr = "DDERR_INVALIDCAPS";
		break;
		case DDERR_INVALIDCLIPLIST:
			pszErr = "DDERR_INVALIDCLIPLIST";
		break;
		case DDERR_INVALIDDIRECTDRAWGUID:
			pszErr = "DDERR_INVALIDDIRECTDRAWGUID";
		break;
		case DDERR_INVALIDMODE:
			pszErr = "DDERR_INVALIDMODE";
		break;
		case DDERR_INVALIDOBJECT:
			pszErr = "DDERR_INVALIDOBJECT";
		break;
		case DDERR_INVALIDPARAMS:
			pszErr = "DDERR_INVALIDPARAMS";
		break;
		case DDERR_INVALIDPIXELFORMAT:
			pszErr = "DDERR_INVALIDPIXELFORMAT";
		break;
		case DDERR_INVALIDPOSITION:
			pszErr = "DDERR_INVALIDPOSITION";
		break;
		case DDERR_INVALIDRECT:
			pszErr = "DDERR_INVALIDRECT";
		break;
		case DDERR_LOCKEDSURFACES:
			pszErr = "DDERR_LOCKEDSURFACES";
		break;
		case DDERR_NO3D:
			pszErr = "DDERR_NO3D";
		break;
		case DDERR_NOALPHAHW:
			pszErr = "DDERR_NOALPHAHW";
		break;
		case DDERR_NOBLTHW:
			pszErr = "DDERR_NOBLTHW";
		break;
		case DDERR_NOCLIPLIST:
			pszErr = "DDERR_NOCLIPLIST";
		break;
		case DDERR_NOCLIPPERATTACHED:
			pszErr = "DDERR_NOCLIPPERATTACHED";
		break;
		case DDERR_NOCOLORCONVHW:
			pszErr = "DDERR_NOCOLORCONVHW";
		break;
		case DDERR_NOCOLORKEY:
			pszErr = "DDERR_NOCOLORKEY";
		break;
		case DDERR_NOCOLORKEYHW:
			pszErr = "DDERR_NOCOLORKEYHW";
		break;
		case DDERR_NOCOOPERATIVELEVELSET:
			pszErr = "DDERR_NOCOOPERATIVELEVELSET";
		break;
		case DDERR_NODC:
			pszErr = "DDERR_NODC";
		break;
		case DDERR_NODDROPSHW:
			pszErr = "DDERR_NODDROPSHW";
		break;
		case DDERR_NODIRECTDRAWHW:
			pszErr = "DDERR_NODIRECTDRAWHW";
		break;
		case DDERR_NOEMULATION:
			pszErr = "DDERR_NOEMULATION";
		break;
		case DDERR_NOEXCLUSIVEMODE:
			pszErr = "DDERR_NOEXCLUSIVEMODE";
		break;
		case DDERR_NOFLIPHW:
			pszErr = "DDERR_NOFLIPHW";
		break;
		case DDERR_NOGDI:
			pszErr = "DDERR_NOGDI";
		break;
		case DDERR_NOHWND:
			pszErr = "DDERR_NOHWND";
		break;
		case DDERR_NOMIRRORHW:
			pszErr = "DDERR_NOMIRRORHW";
		break;
		case DDERR_NOOVERLAYDEST:
			pszErr = "DDERR_NOOVERLAYDEST";
		break;
		case DDERR_NOOVERLAYHW:
			pszErr = "DDERR_NOOVERLAYHW";
		break;
		case DDERR_NOPALETTEATTACHED:
			pszErr = "DDERR_NOPALETTEATTACHED";
		break;
		case DDERR_NOPALETTEHW:
			pszErr = "DDERR_NOPALETTEHW";
		break;
		case DDERR_NORASTEROPHW:
			pszErr = "DDERR_NORASTEROPHW";
		break;
		case DDERR_NOROTATIONHW:
			pszErr = "DDERR_NOROTATIONHW";
		break;
		case DDERR_NOSTRETCHHW:
			pszErr = "DDERR_NOSTRETCHHW";
		break;
		case DDERR_NOT4BITCOLOR:
			pszErr = "DDERR_NOT4BITCOLOR";
		break;
		case DDERR_NOT4BITCOLORINDEX:
			pszErr = "DDERR_NOT4BITCOLORINDEX";
		break;
		case DDERR_NOT8BITCOLOR:
			pszErr = "DDERR_NOT8BITCOLOR";
		break;
		case DDERR_NOTAOVERLAYSURFACE:
			pszErr = "DDERR_NOTAOVERLAYSURFACE";
		break;
		case DDERR_NOTEXTUREHW:
			pszErr = "DDERR_NOTEXTUREHW";
		break;
		case DDERR_NOTFLIPPABLE:
			pszErr = "DDERR_NOTFLIPPABLE";
		break;
		case DDERR_NOTFOUND:
			pszErr = "DDERR_NOTFOUND";
		break;
		case DDERR_NOTLOCKED:
			pszErr = "DDERR_NOTLOCKED";
		break;
		case DDERR_NOTPALETTIZED:
			pszErr = "DDERR_NOTPALETTIZED";
		break;
		case DDERR_NOVSYNCHW:
			pszErr = "DDERR_NOVSYNCHW";
		break;
		case DDERR_NOZBUFFERHW:
			pszErr = "DDERR_NOZBUFFERHW";
		break;
		case DDERR_NOZOVERLAYHW:
			pszErr = "DDERR_NOZOVERLAYHW";
		break;
		case DDERR_OUTOFCAPS:
			pszErr = "DDERR_OUTOFCAPS";
		break;
		case DDERR_OUTOFMEMORY:
			pszErr = "DDERR_OUTOFMEMORY";
		break;
		case DDERR_OUTOFVIDEOMEMORY:
			pszErr = "DDERR_OUTOFVIDEOMEMORY";
		break;
		case DDERR_OVERLAYCANTCLIP:
			pszErr = "DDERR_OVERLAYCANTCLIP";
		break;
		case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
			pszErr = "DDERR_OVERLAYCOLORKEYONLYONEACTIVE";
		break;
		case DDERR_OVERLAYNOTVISIBLE:
			pszErr = "DDERR_OVERLAYNOTVISIBLE";
		break;
		case DDERR_PALETTEBUSY:
			pszErr = "DDERR_PALETTEBUSY";
		break;
		case DDERR_PRIMARYSURFACEALREADYEXISTS:
			pszErr = "DDERR_PRIMARYSURFACEALREADYEXISTS";
		break;
		case DDERR_REGIONTOOSMALL:
			pszErr = "DDERR_REGIONTOOSMALL";
		break;
		case DDERR_SURFACEALREADYATTACHED:
			pszErr = "DDERR_SURFACEALREADYATTACHED";
		break;
		case DDERR_SURFACEALREADYDEPENDENT:
			pszErr = "DDERR_SURFACEALREADYDEPENDENT";
		break;
		case DDERR_SURFACEBUSY:
			pszErr = "DDERR_SURFACEBUSY";
		break;
		case DDERR_SURFACEISOBSCURED:
			pszErr = "DDERR_SURFACEISOBSCURED";
		break;
		case DDERR_SURFACELOST:
			pszErr = "DDERR_SURFACELOST";
		break;
		case DDERR_SURFACENOTATTACHED:
			pszErr = "DDERR_SURFACENOTATTACHED";
		break;
		case DDERR_TOOBIGHEIGHT:
			pszErr = "DDERR_TOOBIGHEIGHT";
		break;
		case DDERR_TOOBIGSIZE:
			pszErr = "DDERR_TOOBIGSIZE";
		break;
		case DDERR_TOOBIGWIDTH:
			pszErr = "DDERR_TOOBIGWIDTH";
		break;
		case DDERR_UNSUPPORTED:
			pszErr = "DDERR_UNSUPPORTED";
		break;
		case DDERR_UNSUPPORTEDFORMAT:
			pszErr = "DDERR_UNSUPPORTEDFORMAT";
		break;
		case DDERR_UNSUPPORTEDMASK:
			pszErr = "DDERR_UNSUPPORTEDMASK";
		break;
		case DDERR_VERTICALBLANKINPROGRESS:
			pszErr = "DDERR_VERTICALBLANKINPROGRESS";
		break;
		case DDERR_WASSTILLDRAWING:
			pszErr = "DDERR_WASSTILLDRAWING";
		break;
		case DDERR_WRONGMODE:
			pszErr = "DDERR_WRONGMODE";
		break;
		case DDERR_XALIGN:
			pszErr = "DDERR_XALIGN";
		break;
		case DDERR_CANTLOCKSURFACE:
			pszErr = "DDERR_CANTLOCKSURFACE";
		break;
		case DDERR_CANTPAGELOCK:
			pszErr = "DDERR_CANTPAGELOCK";
		break;
		case DDERR_CANTPAGEUNLOCK:
			pszErr = "DDERR_CANTPAGEUNLOCK";
		break;
		case DDERR_DCALREADYCREATED:
			pszErr = "DDERR_DCALREADYCREATED";
		break;
		case DDERR_INVALIDSURFACETYPE:
			pszErr = "DDERR_INVALIDSURFACETYPE";
		break;
		case DDERR_NOMIPMAPHW:
			pszErr = "DDERR_NOMIPMAPHW";
		break;
		case DDERR_NOTPAGELOCKED:
			pszErr = "DDERR_NOTPAGELOCKED";
		break;

		default:
			const TCHAR szUnknown[] = "DDERR unknown 0x%x";
			app_assert(dwMaxChars >= sizeof(szUnknown) + 10);
			sprintf(pszBuf,szUnknown,ddrval);
		return;
	}

	_tcsncpy(pszBuf,pszErr,dwMaxChars);
}


//***********************************************************************
//***********************************************************************
static void get_dsound_error(HRESULT dsrval,TCHAR * pszBuf,DWORD dwMaxChars) {
	const TCHAR * pszErr;

	// @@ eventually we should get these from our resource file
	// so that they can be properly translated and so
	// they don't have to stay loaded all the time

	switch(dsrval) {
		case DS_OK:
			pszErr = "DS_OK";
		break;
		case DSERR_ALLOCATED:
			pszErr = "DSERR_ALLOCATED";
		break;
		case DSERR_ALREADYINITIALIZED:
			pszErr = "DSERR_ALREADYINITIALIZED";
		break;
		case DSERR_BADFORMAT:
			pszErr = "DSERR_BADFORMAT";
		break;
		case DSERR_BUFFERLOST:
			pszErr = "DSERR_BUFFERLOST";
		break;
		case DSERR_CONTROLUNAVAIL:
			pszErr = "DSERR_CONTROLUNAVAIL";
		break;
		case DSERR_INVALIDCALL:
			pszErr = "DSERR_INVALIDCALL";
		break;
		case DSERR_INVALIDPARAM:
			pszErr = "DSERR_INVALIDPARAM";
		break;
		case DSERR_NOAGGREGATION:
			pszErr = "DSERR_NOAGGREGATION";
		break;
		case DSERR_NODRIVER:
			pszErr = "DSERR_NODRIVER";
		break;
		case DSERR_OUTOFMEMORY:
			pszErr = "DSERR_OUTOFMEMORY";
		break;
		case DSERR_PRIOLEVELNEEDED:
			pszErr = "DSERR_PRIOLEVELNEEDED";
		break;
		case E_NOINTERFACE:
			pszErr = "E_NOINTERFACE";
		break;

		default:
			const TCHAR szUnknown[] = "DSERR unknown 0x%x";
			app_assert(dwMaxChars >= sizeof(szUnknown) + 10);
			sprintf(pszBuf,szUnknown,dsrval);
		return;
	}

	_tcsncpy(pszBuf,pszErr,dwMaxChars);
} 


//***********************************************************************
//***********************************************************************
// pjw.patch3.start -- changes due to STORM error handling
const TCHAR * strGetError(DWORD dwErr) {
	static TCHAR szBuf[256];

	if (HRESULT_FACILITY(dwErr) == _FACDS) {
		get_dsound_error(dwErr,szBuf,sizeof(szBuf) / sizeof(szBuf[0]));
	}
	else if (HRESULT_FACILITY(dwErr) == _FACDD) {
		get_ddraw_error(dwErr,szBuf,sizeof(szBuf) / sizeof(szBuf[0]));
	}
	else if (SErrGetErrorStr(dwErr,szBuf,sizeof(szBuf) / sizeof(szBuf[0]))) {
    // got storm message
  }
	else if (!FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dwErr,
		MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
		szBuf,
		sizeof(szBuf) / sizeof(szBuf[0]),
		NULL
	)) {
		wsprintf(szBuf,"unknown error 0x%08x",dwErr);
	}

	// remove trailing newline crap
	int nLen = strlen(szBuf);
	char * pszTemp = szBuf + nLen - 1;
	while (nLen-- > 0) {
		pszTemp--;
		if (*pszTemp == '\r' || *pszTemp == '\n')
			*pszTemp = 0;
		else
			break;
	}

	return szBuf;
}
// pjw.patch3.end


//***********************************************************************
//***********************************************************************
const TCHAR * strGetLastError() {
	return strGetError(GetLastError());
}


//***********************************************************************
//***********************************************************************
static void app_debug_msg(const char * pszFmt,va_list args) {
	char szBuf[256];
	wvsprintf(szBuf,pszFmt,args);

	#ifdef _DEBUG
	OutputDebugString(szBuf);
	OutputDebugString(TEXT("\n"));
	#endif

	// turn off "topmost" flag so that we don't stick above debugger
	if (ghMainWnd) SetWindowPos(ghMainWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
		SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);


	// can't use storm -- it might be dead
	MessageBox(ghMainWnd,szBuf,"ERROR",MB_ICONERROR | MB_OK | MB_TASKMODAL);
}


//***********************************************************************
//***********************************************************************
static void pre_fatal_cleanup() {
	// if we fatal from a subsidiary thread, it may kill
	// off things which are needed by other threads, so
	// if we are already fataling, give the other thread
	// some time to die
	static BOOL sbInFatal = 0;
	static unsigned snThreadID = 0;
	if (sbInFatal && snThreadID != GetCurrentThreadId())
		Sleep(20000);
	sbInFatal = 1;
	snThreadID = GetCurrentThreadId();

	// kill off direct draw so that dialogs will be visible
	void free_directx();
	free_directx();

	// for multiplayer games, make sure our fatal
	// handler doesn't cause other players to timeout
	extern BYTE gbMaxPlayers;
	if (gbMaxPlayers > 1) {
		if (SNetLeaveGame(SNET_EXIT_AUTO_SHUTDOWN))
			Sleep(2000);
	}

	// kill off network play
	SNetDestroy();

	// make sure cursor is visible for any dialog box we display
	ShowCursor(TRUE);
}


//***********************************************************************
//***********************************************************************
void __cdecl app_fatal(const char * pszFmt,...) {
	pre_fatal_cleanup();

	// break into debugger
	myDebugBreak();

	if (pszFmt) {
		va_list args;
		va_start(args,pszFmt);
		app_debug_msg(pszFmt,args);
		va_end(args);
	}

	cleanup(FALSE);

	exit(1);
	ExitProcess(1);	// just in case
}


//***********************************************************************
//***********************************************************************
void __cdecl app_warning(const char * pszFmt,...) {
	app_assert(pszFmt);

	char szBuf[256];
	va_list args;
	va_start(args,pszFmt);
	wvsprintf(szBuf,pszFmt,args);
	va_end(args);
	SDrawMessageBox(
		szBuf,
		"Hellfire",
		MB_ICONEXCLAMATION | MB_OK | MB_TASKMODAL
	);
}


//***********************************************************************
//***********************************************************************
#if EXTENDED_ASSERT
void assert_fail(int nLineNo, const char * pszFile, const char * pszFail) {
	app_fatal("assertion failed (%d:%s)\n%s",nLineNo,pszFile,pszFail);
}
#else
void assert_fail(int nLineNo, const char * pszFile) {
	app_fatal("assertion failed (%d:%s)",nLineNo,pszFile);
}
#endif


//***********************************************************************
//***********************************************************************
void ddraw_assert_fail(HRESULT ddrval, int nLineNo, const char * pszFile) {
	if (ddrval == DD_OK) return;
	app_fatal(
		"Direct draw error (%s:%d)\n%s",
		pszFile,
		nLineNo,
		strGetError(ddrval)
	);
} 


//***********************************************************************
//***********************************************************************
void dsound_assert_fail(HRESULT dsrval, int nLineNo, const char * pszFile) {
	if (dsrval == DS_OK) return;
	app_fatal(
		"Direct sound error (%s:%d)\n%s",
		pszFile,
		nLineNo,
		strGetError(dsrval)
	);
}


//******************************************************************
//******************************************************************
void center_window(HWND hWnd) {
	RECT r;
	GetWindowRect(hWnd,&r);
	int cxWnd = r.right - r.left;
	int cyWnd = r.bottom - r.top;

	// get display limits
	HDC hdc = GetDC(hWnd);
	int cxScreen = GetDeviceCaps(hdc,HORZRES);
	int cyScreen = GetDeviceCaps(hdc,VERTRES);
	ReleaseDC(hWnd,hdc);

	// Calculate new X position, then adjust for screen
	int xNew = (cxScreen - cxWnd) / 2;
	if (! SetWindowPos(
		hWnd,
		NULL,
		(cxScreen - cxWnd) / 2,
		(cyScreen - cyWnd) / 2,
		0,
		0,
		SWP_NOSIZE | SWP_NOZORDER
	)) app_fatal("center_window: %s",strGetLastError());
}


//******************************************************************
//******************************************************************
static void ErrorDlgInit(HWND hWnd,LPARAM lParam) {
	center_window(hWnd);
	if (lParam) SetDlgItemText(hWnd,IDC_ERROR_TAG,(LPCTSTR) lParam);
}


//******************************************************************
//******************************************************************
static BOOL CALLBACK ErrorDlgProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	switch (uMsg) {
		case WM_INITDIALOG:
			ErrorDlgInit(hWnd,lParam);
		break;

		case WM_COMMAND:
			if (IDOK == GET_WM_COMMAND_ID(wParam,lParam))
				EndDialog(hWnd,TRUE);
			else if (IDCANCEL == GET_WM_COMMAND_ID(wParam,lParam))
				EndDialog(hWnd,FALSE);
		break;

		default:
		return FALSE;
	}

	return TRUE;
}


//******************************************************************
//******************************************************************
void ErrorDlg(int nDlgId,DWORD dwErr,const char * pszFile,int nLine) {
	pre_fatal_cleanup();
	char szBuf[512];
	const char * pszTemp = strrchr(pszFile,'\\');
	if (pszTemp) pszFile = pszTemp + 1;
	wsprintf(szBuf,"%s\nat: %s line %d",strGetError(dwErr),pszFile,nLine);

	#ifdef _DEBUG
	OutputDebugString(szBuf);
	OutputDebugString(TEXT("\n"));
	#endif

	if (-1 == DialogBoxParam(
		ghInst,
		MAKEINTRESOURCE(nDlgId),
		ghMainWnd,
		ErrorDlgProc,
		(LPARAM) szBuf
	)) app_fatal("ErrDlg: %d",nDlgId);
	app_fatal(NULL);
}


//******************************************************************
//******************************************************************
void FileErrorDlg(const char * pszName) {
	pre_fatal_cleanup();
	if (! pszName) pszName = "";

	if (-1 == DialogBoxParam(
		ghInst,
		MAKEINTRESOURCE(IDD_FILE_ERR),
		ghMainWnd,
		ErrorDlgProc,
		(LPARAM) pszName
	)) app_fatal("FileErrDlg");
	app_fatal(NULL);
}


//******************************************************************
//******************************************************************
void DiskFreeErrorDlg(const char * pszDir) {
	pre_fatal_cleanup();
	if (-1 == DialogBoxParam(
		ghInst,
		MAKEINTRESOURCE(IDD_DISKFREE_ERR),
		ghMainWnd,
		ErrorDlgProc,
		(LPARAM) pszDir
	)) app_fatal("DiskFreeDlg");
	app_fatal(NULL);
}


//******************************************************************
//******************************************************************
// pjw.patch1.start.1/13/97
BOOL InsertCDDlg(void) {
	ShowCursor(TRUE);
	int nResult;
	if (-1 == (nResult = DialogBoxParam(
		ghInst,
		MAKEINTRESOURCE(IDD_CDROM_ERR),
		ghMainWnd,
		ErrorDlgProc,
		(LPARAM) ""
	))) app_fatal("InsertCDDlg");
	ShowCursor(FALSE);
	return nResult == IDOK;
}
// pjw.patch1.end.1/13/97

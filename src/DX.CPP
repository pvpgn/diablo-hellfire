//******************************************************************
// dx.cpp
//******************************************************************


#include "diablo.h"
#pragma hdrstop
#include "storm.h"
#include "palette.h"
#include "engine.h"
#include "resource.h"
#include "gendung.h"


//******************************************************************
// extern
//******************************************************************
void myDebugBreak();
void ErrorDlg(int nDlgId,DWORD dwErr,const char * pszFile,int nLine);


//******************************************************************
// public
//******************************************************************
BYTE * gpBuffer;
LPDIRECTDRAW            lpDD;           // DirectDraw object
LPDIRECTDRAWSURFACE     lpDDSPrimary;   // DirectDraw primary surface
LPDIRECTDRAWSURFACE		lpDDSBackBuf;	// optional back buffer
LPDIRECTDRAWPALETTE     lpDDPal;        // DirectDraw palette
BYTE gbForceBackBuf = FALSE;
BYTE gbUseDDEmulation = FALSE;


//******************************************************************
// private
//******************************************************************
static DWORD sgdwLockCount;
static CCritSect sgDrawCrit;
static BYTE * sgpBackBuf;
static HINSTANCE sghDDlib = NULL;

#ifndef NDEBUG
static DWORD sgdwLockTbl[256];
#endif


//******************************************************************
//******************************************************************
static void init_backbuf() {
	app_assert(! gpBuffer);
	app_assert(! sgdwLockCount);
	app_assert(! sgpBackBuf);

	// can we lock the primary surface?
	DDSCAPS caps;
	DDSURFACEDESC ddsd;
	app_assert(lpDDSPrimary);
	HRESULT ddrval = lpDDSPrimary->GetCaps(&caps);
	ddraw_assert(ddrval);
	app_assert(caps.dwCaps & DDSCAPS_PRIMARYSURFACE);

	// is this a lockable surface?
	extern BYTE gbForceBackBuf;
	if (! gbForceBackBuf) {
		ddsd.dwSize = sizeof(ddsd);
		ddrval = lpDDSPrimary->Lock(NULL,&ddsd,DDLOCK_WAIT|DDLOCK_WRITEONLY,NULL);
		if (ddrval == DD_OK) {
			ddrval = lpDDSPrimary->Unlock(NULL);
			// pjw.patch1.start.1/13/97
			// commented out -- in NT it is possible to 
			// lose a video surface while it is locked
			// ddraw_assert(ddrval);
			// pjw.patch1.end.1/13/97

			// surface is lockable, just create an offscreen memory buffer
			sgpBackBuf = DiabloAllocPtrSig(BUFFERSIZE,'OFFS');
			return;
		}
		// non-lockable surface?
		if (ddrval != DDERR_CANTLOCKSURFACE) 
			ErrorDlg(IDD_DDRAW_ERR,ddrval,__FILE__,__LINE__);
	}

	// create a secondary surface and lock it permanently
	ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PITCH|DDSD_PIXELFORMAT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	ddsd.dwHeight = BUFFERY;
	ddsd.dwWidth = BUFFERX;
	ddsd.lPitch = BUFFERX;
	ddsd.ddpfPixelFormat.dwSize = sizeof(ddsd.ddpfPixelFormat);
	ddrval = lpDDSPrimary->GetPixelFormat(&ddsd.ddpfPixelFormat);
	if (ddrval != DD_OK) ErrorDlg(IDD_DDRAW_ERR,ddrval,__FILE__,__LINE__);
	ddrval = lpDD->CreateSurface(&ddsd,&lpDDSBackBuf,NULL);
	if (ddrval != DD_OK) ErrorDlg(IDD_DDRAW_ERR,ddrval,__FILE__,__LINE__);
}


//******************************************************************
//******************************************************************
static void init_primary() {
    DDSURFACEDESC ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	HRESULT ddrval = lpDD->CreateSurface(&ddsd, &lpDDSPrimary, NULL);
	if (ddrval != DD_OK) ErrorDlg(IDD_DDRAW_ERR,ddrval,__FILE__,__LINE__);
}


//******************************************************************
//******************************************************************
static HRESULT InDirectDrawCreate(
	GUID * lpGUID,
	LPDIRECTDRAW * lplpDD,
	IUnknown * pUnkOuter
) {
	// load direct draw library
	if (! sghDDlib) sghDDlib = LoadLibrary(TEXT("ddraw.dll"));
	if (! sghDDlib) ErrorDlg(IDD_DDRAW_DLL_ERR,GetLastError(),__FILE__,__LINE__);

	// bind to DirectDrawCreate
	typedef HRESULT (WINAPI * DDCREATETYPE)(GUID *,LPDIRECTDRAW *,IUnknown *);
    DDCREATETYPE ddcreatefunc = (DDCREATETYPE) GetProcAddress(sghDDlib,TEXT("DirectDrawCreate"));
	if (! ddcreatefunc) ErrorDlg(IDD_DDRAW_DLL_ERR,GetLastError(),__FILE__,__LINE__);

	// call DirectDrawCreate
	return ddcreatefunc(lpGUID,lplpDD,pUnkOuter);
}


//******************************************************************
//******************************************************************
void init_directx(HWND hWnd) {
    HRESULT ddrval;
	app_assert(! gpBuffer);
	app_assert(! sgdwLockCount);
	app_assert(! sgpBackBuf);

    SetFocus(hWnd);
	ShowWindow(hWnd,SW_SHOWNORMAL);

	extern BYTE gbUseDDEmulation;
	GUID * lpGUID = NULL;
	if (gbUseDDEmulation) lpGUID = (GUID *) DDCREATE_EMULATIONONLY;
	ddrval = InDirectDrawCreate(lpGUID,&lpDD,NULL);
	if (ddrval != DD_OK) ErrorDlg(IDD_DDRAW_ERR,ddrval,__FILE__,__LINE__);

	#if !ALLOW_WINDOWED_MODE
	fullscreen = TRUE;
	#endif

	#if ALLOW_WINDOWED_MODE
	if (!fullscreen) {
		ddrval = lpDD->SetCooperativeLevel(hWnd,DDSCL_NORMAL | DDSCL_ALLOWREBOOT);
		if (ddrval == DDERR_EXCLUSIVEMODEALREADYSET) myDebugBreak();
		else if (ddrval != DD_OK) ErrorDlg(IDD_DDRAW_ERR,ddrval,__FILE__,__LINE__);

		// turn off "topmost" flag so that we don't stick above debugger
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
	}
	else {
	#endif
		// Get exclusive mode
		ddrval = lpDD->SetCooperativeLevel(hWnd,DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT);
		if (ddrval == DDERR_EXCLUSIVEMODEALREADYSET) myDebugBreak();
		else if (ddrval != DD_OK) ErrorDlg(IDD_DDRAW_ERR,ddrval,__FILE__,__LINE__);

		// Set the video mode to 640x480x8
		ddrval = lpDD->SetDisplayMode( 640, 480, 8);
		// pjw.patch1.start.1/13/97
		// some notebook computers can't switch resolutions -- but
		// we should be able to switch the color depth to 256 ???
		if (ddrval != DD_OK) {
			int nWdt = GetSystemMetrics(SM_CXSCREEN);
			int nHgt = GetSystemMetrics(SM_CYSCREEN);
			ddrval = lpDD->SetDisplayMode(nWdt, nHgt, 8);
		}
		// pjw.patch1.end.1/13/97
		if (ddrval != DD_OK) ErrorDlg(IDD_DDRAW_ERR,ddrval,__FILE__,__LINE__);

	#if ALLOW_WINDOWED_MODE
	}
	#endif

	init_primary();

	CreatePalette();

	// Do not allow gdi batching
	GdiSetBatchLimit(1);

	// Get the full offscreen buffer including edges
	init_backbuf();

	// inform STORM library of our DirectDraw objects
	BOOL bSuccess = SDrawManualInitialize(
		hWnd,			// window
		lpDD,			// direct draw
		lpDDSPrimary,	// primary
		NULL,			// secondary
		NULL,			// system
		lpDDSBackBuf,	// temporary
		lpDDPal,		// palette
		NULL			// gdi palette
	);
	app_assert(bSuccess);
}


//******************************************************************
//******************************************************************
static void lock_buf_priv() {
	// don't allow any other threads access to the draw
	// buffer while we have it locked
	sgDrawCrit.Enter();

	if (sgpBackBuf) {
		gpBuffer = sgpBackBuf;
	}
	else if (! lpDDSBackBuf) {
		// if the back buffer was destroyed by another thread
		// it is because it is fataling...give it a chance to
		// shut down the system before performing our own fatal
		Sleep(20000);
		app_fatal("lock_buf_priv");
	}
	else if (! sgdwLockCount) {
		DDSURFACEDESC ddsd;
		ddsd.dwSize = sizeof(ddsd);
		HRESULT ddrval = lpDDSBackBuf->Lock(NULL,&ddsd,DDLOCK_WAIT,NULL);
		ddraw_assert(ddrval);
		gpBuffer = (BYTE *) ddsd.lpSurface;
		app_assert(gpBuffer);
		glClipY += (long) gpBuffer;
	}

	// increment lock count
	sgdwLockCount++;
}


//******************************************************************
//******************************************************************
void lock_buf(BYTE bFcn) {
	// for debugging -- make sure no lock count over/underflow
	#ifndef NDEBUG
	sgdwLockTbl[bFcn]++;
	#endif

	lock_buf_priv();
}


//******************************************************************
//******************************************************************
static void unlock_buf_priv() {
	if (! sgdwLockCount) app_fatal("draw main unlock error");
	if (! gpBuffer) app_fatal("draw consistency error");
	if (! sgpBackBuf) app_assert(lpDDSBackBuf);

	// decrement lock count	
	sgdwLockCount--;

	if (! sgdwLockCount) {
		glClipY -= (long) gpBuffer;
		gpBuffer = NULL;
		if (! sgpBackBuf) {
			HRESULT ddrval = lpDDSBackBuf->Unlock(NULL);
			ddraw_assert(ddrval);
		}
	}

	sgDrawCrit.Leave();
}


//******************************************************************
//******************************************************************
void unlock_buf(BYTE bFcn) {
	// for debugging -- make sure no lock count over/underflow
	#ifndef NDEBUG
	if (! sgdwLockTbl[bFcn]) app_fatal("Draw lock underflow: 0x%x",bFcn);
	sgdwLockTbl[bFcn]--;
	#endif

	unlock_buf_priv();
}


//******************************************************************
// THIS FUNCTION MAY BE CALLED FROM ANY THREAD IN THE PROGRAM
// ALL OTHER PUBLIC FUNCTIONS IN THIS MODULE MAY ONLY BE CALLED
// FROM THE MAIN APPLICATION THREAD!
//******************************************************************
void free_directx() {
	if (ghMainWnd) ShowWindow(ghMainWnd,SW_HIDE);

	// tell SDraw that we're about to kill off direct draw
	// so it doesn't try to re-use freed objects
	SDrawDestroy();

	sgDrawCrit.Enter();
	if (sgpBackBuf) {
		app_assert(! lpDDSBackBuf);
		DiabloFreePtr(sgpBackBuf);
	}
	else if (lpDDSBackBuf) {
		lpDDSBackBuf->Release();
		lpDDSBackBuf = NULL;
	}
	sgdwLockCount = 0;
	gpBuffer = NULL;
	sgDrawCrit.Leave();

	if (lpDDSPrimary) {
		lpDDSPrimary->Release();
		lpDDSPrimary = NULL;
	}
	if (lpDDPal) {
		lpDDPal->Release();
		lpDDPal = NULL;
	}
	if (lpDD) {
		lpDD->Release();
		lpDD = NULL;
	}

// cannot free library now, still may be in use
// by directX window procedure...
/*
	if (sghDDlib) {
		FreeLibrary(sghDDlib);
		sghDDlib = NULL;
	}
*/
}


//******************************************************************
//******************************************************************
void ddraw_switch_modes() {
	sgDrawCrit.Enter();
	app_assert(ghMainWnd);

	void savecrsr_reset();
	savecrsr_reset();

	// remove any locks this thread has on buffer
	DWORD dwSaveCount = sgdwLockCount;
	while (sgdwLockCount) unlock_buf_priv();

	free_directx();
	force_redraw = FULLDRAW;
	init_directx(ghMainWnd);

	// restore locks
	while (dwSaveCount--) lock_buf_priv();

	sgDrawCrit.Leave();
}


//******************************************************************
//******************************************************************
void ddraw_reinit() {
	ddraw_switch_modes();
}

//******************************************************************
// movie.cpp
//******************************************************************


#include "diablo.h"
#pragma hdrstop
#include "storm.h"
#include "palette.h"
#include "engine.h"
#include "scrollrt.h"
#include "gendung.h"
#include "sound.h"


//******************************************************************
// externs
//******************************************************************
void CALLBACK menusnd_play(LPCSTR pszName);
WNDPROC my_SetWindowProc(WNDPROC wndProc);
void BlackPalette();
void music_pause(BOOL bPause);
void stream_stop();


//******************************************************************
// private
//******************************************************************
static BYTE sgbPlayMovie;


//******************************************************************
//******************************************************************
static LRESULT CALLBACK MovieWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	switch (uMsg) {
		case WM_SYSCOMMAND:
			if (wParam == SC_CLOSE) {
				sgbPlayMovie = FALSE;
				return 0;
			}
		break;

		case WM_KEYDOWN:
			sgbPlayMovie = FALSE;
		break;

		case WM_CHAR:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			sgbPlayMovie = FALSE;
		break;
	}

	return DiabloDefProc(hWnd,uMsg,wParam,lParam);
}


//******************************************************************
//******************************************************************
BOOL gbLoopMovie = FALSE;
void play_movie(const char * pszMovie,BOOL bAllowCancel) {
	app_assert(pszMovie);

	// if we are not the frontmost window then don't play the movie
	if (! bActive) return;

	app_assert(ghMainWnd);
	WNDPROC saveProc = my_SetWindowProc(MovieWndProc);
	InvalidateRect(ghMainWnd,NULL,0);
	UpdateWindow(ghMainWnd);

	sgbPlayMovie = TRUE;

	// stop music and streaming SFX from playing so they
	// don't interfere with the CD bandwidth during movie
	music_pause(TRUE);
	stream_stop();

	// play "silence" because according to RAD software, it
	// prevents a popping noise upon movie startup
	menusnd_play("Sfx\\Misc\\blank.wav");
	HSVIDEO hVid;
	SVidPlayBegin(
		pszMovie,
		NULL,
		NULL,
		NULL,
		0,
		gbLoopMovie ? (SVID_AUTOCUTSCENE|SVID_FLAG_LOOP)&~SVID_FLAG_CLEARSCREEN : SVID_AUTOCUTSCENE,
		&hVid
	);

	while (hVid && bActive) {
		if (bAllowCancel && !sgbPlayMovie) break;

		MSG msg;
		while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
			if (msg.message == WM_QUIT) continue;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (!SVidPlayContinue())
			break;
	}

	if (hVid) SVidPlayEnd(hVid);

	// restore window procedure
	saveProc = my_SetWindowProc(saveProc);
	app_assert(saveProc == MovieWndProc);

	music_pause(FALSE);
}

/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Menu interface processing
**
** 				(C)1996 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/INTERFAC.CPP 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"storm.h"
#include	"palette.h"
#include	"engine.h"
#include	"scrollrt.h"
#include	"gendung.h"
#include	"items.h"
#include	"player.h"
#include	"gamemenu.h"
#include	"control.h"
#include	"cursor.h"
#include	"trigs.h"
#include	"multi.h"
#include	"msg.h"
#include	"effects.h"
#include	"portal.h"
#include	"quests.h"
#include	"setmaps.h"


//******************************************************************
// extern
//******************************************************************
extern BYTE gbSomebodyWonGameKludge;
WNDPROC my_SetWindowProc(WNDPROC wndProc);
void plrmsg_hold(BOOL bStart);
void BlackPalette();
void DestroyTempSaves();
LRESULT CALLBACK DisableInputWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);


//******************************************************************
// private
//******************************************************************
#define MAX_PROGRESS	534
static BYTE *sgpBackCel;
static DWORD sgdwProgress, sgdwXY;


//******************************************************************
//******************************************************************
static void ProgressFree() {
	DiabloFreePtr(sgpBackCel);
}


//******************************************************************
//******************************************************************
static void ProgressLoad(UINT uMsg) {
	app_assert(! sgpBackCel);
	switch (uMsg) {
		case WM_DIABNEXTLVL :
			switch (gnLevelTypeTbl[currlevel]) {
				case 0:
					sgpBackCel = LoadFileInMemSig("Gendata\\Cuttt.CEL",NULL,'PROG');
					LoadPalette("Gendata\\Cuttt.pal");
					sgdwXY = 1;
					break;
				case 1:
					if (currlevel < HIVESTART)
					{
						sgpBackCel = LoadFileInMemSig("Gendata\\Cutl1d.CEL",NULL,'PROG');
						LoadPalette ("Gendata\\Cutl1d.pal");
						sgdwXY = 0;
						break;
					}
					else
					{
						sgpBackCel = LoadFileInMemSig("Nlevels\\cutl5.CEL",NULL,'PROG');
						LoadPalette ("Nlevels\\cutl5.pal");
						sgdwXY = 1;
						break;
					}

				case 2:
					sgpBackCel = LoadFileInMemSig("Gendata\\Cut2.CEL",NULL,'PROG');
					LoadPalette ("Gendata\\Cut2.pal");
					sgdwXY = 2;
					break;
				case 3:
					if (currlevel < HIVESTART)
					{
						sgpBackCel = LoadFileInMemSig("Gendata\\Cut3.CEL",NULL,'PROG');
						LoadPalette ("Gendata\\Cut3.pal");
						sgdwXY = 1;
						break;
					}
					else
					{
						sgpBackCel = LoadFileInMemSig("Nlevels\\cutl6.CEL",NULL,'PROG');
						LoadPalette ("Nlevels\\cutl6.pal");
						sgdwXY = 1;
						break;
					}

				case 4:
					if (currlevel < 15) {
						sgpBackCel = LoadFileInMemSig("Gendata\\Cut4.CEL",NULL,'PROG');
						LoadPalette ("Gendata\\Cut4.pal");
						sgdwXY = 1;
					} else {
						sgpBackCel = LoadFileInMemSig("Gendata\\Cutgate.CEL",NULL,'PROG');
						LoadPalette ("Gendata\\Cutgate.pal");
						sgdwXY = 1;
					}
					break;
				default:
					sgpBackCel = LoadFileInMemSig("Gendata\\Cutl1d.CEL",NULL,'PROG');
					LoadPalette ("Gendata\\Cutl1d.pal");
					sgdwXY = 0;
					break;
			}
			break;

		case WM_DIABPREVLVL :
			if (gnLevelTypeTbl[currlevel-1] == 0) {
				sgpBackCel = LoadFileInMemSig("Gendata\\Cuttt.CEL",NULL,'PROG');
				LoadPalette ("Gendata\\Cuttt.pal");
				sgdwXY = 1;
			} else {
				switch (gnLevelTypeTbl[currlevel]) {
					case 0:
						sgpBackCel = LoadFileInMemSig("Gendata\\Cuttt.CEL",NULL,'PROG');
						LoadPalette ("Gendata\\Cuttt.pal");
						sgdwXY = 1;
						break;
					case 1:
						if (currlevel < HIVESTART)
						{
							sgpBackCel = LoadFileInMemSig("Gendata\\Cutl1d.CEL",NULL,'PROG');
							LoadPalette ("Gendata\\Cutl1d.pal");
							sgdwXY = 0;
							break;
						}
						else
						{
							sgpBackCel = LoadFileInMemSig("Nlevels\\cutl5.CEL",NULL,'PROG');
							LoadPalette ("Nlevels\\cutl5.pal");
							sgdwXY = 1;
							break;
						}

					case 2:
						sgpBackCel = LoadFileInMemSig("Gendata\\Cut2.CEL",NULL,'PROG');
						LoadPalette ("Gendata\\Cut2.pal");
						sgdwXY = 2;
						break;
					case 3:
						if (currlevel < HIVESTART)
						{
							sgpBackCel = LoadFileInMemSig("Gendata\\Cut3.CEL",NULL,'PROG');
							LoadPalette ("Gendata\\Cut3.pal");
							sgdwXY = 1;
							break;
						}
						else
						{
							sgpBackCel = LoadFileInMemSig("Nlevels\\cutl6.CEL",NULL,'PROG');
							LoadPalette ("Nlevels\\cutl6.pal");
							sgdwXY = 1;
							break;
						}
					case 4:
						sgpBackCel = LoadFileInMemSig("Gendata\\Cut4.CEL",NULL,'PROG');
						LoadPalette ("Gendata\\Cut4.pal");
						sgdwXY = 1;
						break;
					default:
						sgpBackCel = LoadFileInMemSig("Gendata\\Cutl1d.CEL",NULL,'PROG');
						LoadPalette ("Gendata\\Cutl1d.pal");
						sgdwXY = 0;
						break;
				}
			}
			break;

		case WM_DIABSETLVL :
			if (setlvlnum == SL_BONECHAMB) {
				sgpBackCel = LoadFileInMemSig("Gendata\\Cut2.CEL",NULL,'PROG');
				LoadPalette ("Gendata\\Cut2.pal");
				sgdwXY = 2;
			} else if (setlvlnum == SL_VILEBETRAYER) {
				sgpBackCel = LoadFileInMemSig("Gendata\\Cutportr.CEL",NULL,'PROG');
				LoadPalette ("Gendata\\Cutportr.pal");
				sgdwXY = 1;
			} else {
				sgpBackCel = LoadFileInMemSig("Gendata\\Cutl1d.CEL",NULL,'PROG');
				LoadPalette ("Gendata\\Cutl1d.pal");
				sgdwXY = 0;
			}
			break;

		case WM_DIABRTNLVL :
			if (setlvlnum == SL_BONECHAMB) {			// bone chamber
				sgpBackCel = LoadFileInMemSig("Gendata\\Cut2.CEL",NULL,'PROG');
				LoadPalette ("Gendata\\Cut2.pal");
				sgdwXY = 2;
			} else if (setlvlnum == SL_VILEBETRAYER) {	// vile betrayer
				sgpBackCel = LoadFileInMemSig("Gendata\\Cutportr.CEL",NULL,'PROG');
				LoadPalette ("Gendata\\Cutportr.pal");
				sgdwXY = 1;
			} else {
				sgpBackCel = LoadFileInMemSig("Gendata\\Cutl1d.CEL",NULL,'PROG');
				LoadPalette ("Gendata\\Cutl1d.pal");
				sgdwXY = 0;
			}
			break;

		case WM_DIABWARPLVL :
			sgpBackCel = LoadFileInMemSig("Gendata\\Cutportl.CEL",NULL,'PROG');
			LoadPalette ("Gendata\\Cutportl.pal");
			sgdwXY = 1;
			break;

		case WM_DIABLOADGAME :
			sgpBackCel = LoadFileInMemSig("Gendata\\Cutstart.CEL",NULL,'PROG');
			LoadPalette ("Gendata\\Cutstart.pal");
			sgdwXY = 1;
			break;

		case WM_DIABNEWGAME:
			sgpBackCel = LoadFileInMemSig("Gendata\\Cutstart.CEL",NULL,'PROG');
			LoadPalette ("Gendata\\Cutstart.pal");
			sgdwXY = 1;
			break;

		case WM_DIABTOWNWARP:
		case WM_DIABTWARPUP:
			switch (gnLevelTypeTbl[plr[myplr].plrlevel]) {
				case 0:
					sgpBackCel = LoadFileInMemSig("Gendata\\Cuttt.CEL",NULL,'PROG');
					LoadPalette ("Gendata\\Cuttt.pal");
					sgdwXY = 1;
					break;
				case 1:				// added to allow the crypt JKE
					if (plr[myplr].plrlevel < HIVESTART)
					{
						sgpBackCel = LoadFileInMemSig("Gendata\\Cutl1d.CEL",NULL,'PROG');
						LoadPalette ("Gendata\\Cutl1d.pal");
						sgdwXY = 0;
						break;
					}
					else
					{
						sgpBackCel = LoadFileInMemSig("Nlevels\\Cutl5.CEL",NULL,'PROG');
						LoadPalette ("Nlevels\\Cutl5.pal");
						sgdwXY = 1;
						break;
					}
				case 2:
					sgpBackCel = LoadFileInMemSig("Gendata\\Cut2.CEL",NULL,'PROG');
					LoadPalette ("Gendata\\Cut2.pal");
					sgdwXY = 2;
					break;
				case 3:
					if (plr[myplr].plrlevel < HIVESTART)
					{
						sgpBackCel = LoadFileInMemSig("Gendata\\Cut3.CEL",NULL,'PROG');
						LoadPalette ("Gendata\\Cut3.pal");
						sgdwXY = 1;
						break;
					}
					else
					{
						sgpBackCel = LoadFileInMemSig("Nlevels\\Cutl6.CEL",NULL,'PROG');
						LoadPalette ("Nlevels\\Cutl6.pal");
						sgdwXY = 1;
						break;
					}

				case 4:
					sgpBackCel = LoadFileInMemSig("Gendata\\Cut4.CEL",NULL,'PROG');
					LoadPalette ("Gendata\\Cut4.pal");
					sgdwXY = 1;
					break;
			}
			break;

		case WM_DIABRETOWN:
			sgpBackCel = LoadFileInMemSig("Gendata\\Cuttt.CEL",NULL,'PROG');
			LoadPalette ("Gendata\\Cuttt.pal");
			sgdwXY = 1;
			break;

		default:
			app_fatal("Unknown progress mode");
			break;
	}

	// Indicates time for the progress bar
	sgdwProgress = 0;
}


//******************************************************************
//******************************************************************
static void DrawBarXY(int x, int y, int v2) {
	app_assert(gpBuffer);
	static const BYTE pixel[3] = { 0x8a, 0x2b, 0xfe };
	BYTE * pto = gpBuffer + nBuffWTbl[y] + x;
	for (int i = 0; i < 22; i++) {
		*pto = pixel[v2];
		pto = pto + 768;
	}
}


//******************************************************************
//******************************************************************
static void ProgressIntDraw() {
	static const int xytable[3][2] = { {53, 37}, {53, 421}, {53, 37} };

	// draw background
	lock_buf(1);
	app_assert(sgpBackCel);
	DrawCel(64, 639, sgpBackCel, 1, 640);
	
	// draw load/progress bar
	for (DWORD i = 0; i < sgdwProgress; i++)
		DrawBarXY (64 + xytable[sgdwXY][0] + i, xytable[sgdwXY][1] + 160, sgdwXY);
	unlock_buf(1);

	// force a full blit
	force_redraw = FULLDRAW;
	FullBlit(FALSE);
}


//******************************************************************
//******************************************************************
void interface_msg_pump() {
	MSG msg;
	while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
		if (msg.message != WM_QUIT) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}


//******************************************************************
//******************************************************************
BOOL IntCheck() {
	interface_msg_pump();

	// make bar increase
	sgdwProgress += 15;
	if (sgdwProgress > MAX_PROGRESS)
		sgdwProgress = MAX_PROGRESS;

	// redraw the screen
	if (sgpBackCel)	ProgressIntDraw();

	// are we done?
	return (sgdwProgress >= MAX_PROGRESS);
}


//******************************************************************
//******************************************************************
void ShowProgress(UINT uMsg) {
	gbSomebodyWonGameKludge = FALSE;
	plrmsg_hold(TRUE);

	app_assert(ghMainWnd);
	WNDPROC saveProc = my_SetWindowProc(DisableInputWndProc);
	interface_msg_pump();

	// load progress background and fade in
	ClrDraw();
	FullBlit(TRUE);
	ProgressLoad(uMsg);
	BlackPalette();
	ProgressIntDraw();
	PaletteFadeIn(FADE_FAST);

	IntCheck();
	sound_init();
	IntCheck();

	switch (uMsg) {
		case WM_DIABLOADGAME :
			IntCheck();
			GM_LoadGame(TRUE);
			IntCheck();
		break;

		case WM_DIABNEWGAME :
			IntCheck();
			FreeGameMem();
			IntCheck();
			DestroyTempSaves();
			LoadGameLevel(TRUE, LVL_DOWN);
			IntCheck();
		break;

		case WM_DIABNEXTLVL :
			IntCheck();
			if (gbMaxPlayers == 1) SaveLevel();
			else DeltaSaveLevel();

			FreeGameMem();
			currlevel++;
			leveltype = gnLevelTypeTbl[currlevel];
			app_assert(plr[myplr].plrlevel == currlevel);
			IntCheck();
			LoadGameLevel(FALSE, LVL_DOWN);
			IntCheck();
		break;

		case WM_DIABPREVLVL :
			IntCheck();
			if (gbMaxPlayers == 1) SaveLevel();
			else DeltaSaveLevel();

			IntCheck();
			FreeGameMem();
			currlevel--;
			leveltype = gnLevelTypeTbl[currlevel];
			app_assert(plr[myplr].plrlevel == currlevel);
			IntCheck();
			LoadGameLevel(FALSE, LVL_UP);
			IntCheck();
		break;

		case WM_DIABSETLVL :
			SetReturnLvlPos();
			if (gbMaxPlayers == 1) SaveLevel();
			else DeltaSaveLevel();
			setlevel = TRUE;
			leveltype = setlvltype;
			FreeGameMem();
			IntCheck();
			LoadGameLevel(FALSE, LVL_SET);
			IntCheck();
		break;

		case WM_DIABRTNLVL :
			if (gbMaxPlayers == 1) SaveLevel();
			else DeltaSaveLevel();
			setlevel = FALSE;
			FreeGameMem();
			IntCheck();
			GetReturnLvlPos();
			LoadGameLevel(FALSE, LVL_RTN);
			IntCheck();
		break;

		case WM_DIABWARPLVL :
			IntCheck();
			if (gbMaxPlayers == 1) SaveLevel();
			else DeltaSaveLevel();
			FreeGameMem();
			GetPortalLevel();
			IntCheck();

			LoadGameLevel(FALSE, LVL_WARP);
			IntCheck();
		break;

		case WM_DIABTOWNWARP:
			IntCheck();
			if (gbMaxPlayers == 1) SaveLevel();
			else DeltaSaveLevel();

			FreeGameMem();
			currlevel = plr[myplr].plrlevel;
			leveltype = gnLevelTypeTbl[currlevel];
			app_assert(plr[myplr].plrlevel == currlevel);
			IntCheck();
			LoadGameLevel(FALSE, LVL_TWARPDN);
			IntCheck();
		break;

		case WM_DIABTWARPUP:
			IntCheck();
			if (gbMaxPlayers == 1) SaveLevel();
			else DeltaSaveLevel();

			FreeGameMem();
			currlevel = plr[myplr].plrlevel;
			leveltype = gnLevelTypeTbl[currlevel];
			app_assert(plr[myplr].plrlevel == currlevel);
			IntCheck();
			LoadGameLevel(FALSE, LVL_TWARPUP);
			IntCheck();
		break;

		case WM_DIABRETOWN:
			IntCheck();
			if (gbMaxPlayers == 1) SaveLevel();
			else DeltaSaveLevel();

			FreeGameMem();
			currlevel = plr[myplr].plrlevel;
			leveltype = gnLevelTypeTbl[currlevel];
			app_assert(plr[myplr].plrlevel == currlevel);
			IntCheck();
			LoadGameLevel(FALSE, LVL_DOWN);
			IntCheck();
		break;

	}

	// cleanup
	app_assert(ghMainWnd);
	PaletteFadeOut(FADE_FAST);
	ProgressFree();

	// restore window procedure
	saveProc = my_SetWindowProc(saveProc);
	app_assert(saveProc == DisableInputWndProc);

	NetSendCmdLocParam1(
		TRUE,
		CMD_PLAYER_JOINLEVEL,
		plr[myplr]._px,
		plr[myplr]._py,
		plr[myplr].plrlevel
	);

	plrmsg_hold(FALSE);
	ResetPal();

	if (gbSomebodyWonGameKludge && plr[myplr].plrlevel == 16) {
		// somebody killed diablo while we were on the stairs
		void PrepDoEnding();
		PrepDoEnding();
	}
	gbSomebodyWonGameKludge = FALSE;
}
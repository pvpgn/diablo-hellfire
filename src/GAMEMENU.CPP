/*-----------------------------------------------------------------------**
** 								Diablo
**
**							 Game Menu file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/GAMEMENU.CPP 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------*/

#include "diablo.h"
#pragma hdrstop
#include "gendung.h"
#include "sound.h"
#include "gamemenu.h"
#include "scrollrt.h"
#include "multi.h"
#include "cursor.h"
#include "items.h"
#include "player.h"
#include "error.h"
#include "palette.h"
#include "effects.h"
#include "msg.h"
#include "storm.h"


//******************************************************************
// extern
//******************************************************************
extern BOOL gbRunGame;
extern BOOL gbRunGameResult;
extern BOOL deathflag;
void sound_stop();
void GM_SaveGame();
void GM_LoadGame(BOOL firstflag);
LRESULT CALLBACK DisableInputWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
void interface_msg_pump();
WNDPROC my_SetWindowProc(WNDPROC wndProc);
/*extern*/ DWORD gbWalkOn = TRUE;
extern char gszProgKey[];

void CornerstoneSave();

//******************************************************************
// private
//******************************************************************
// menu functions
static void fnNew(BOOL bActivate);
static void fnLoad(BOOL bActivate);
static void fnOptions(BOOL bActivate);
static void fnSave(BOOL bActivate);
static void fnQuit(BOOL bActivate);
static void fnRestart(BOOL bActivate);
static void fnMusic(BOOL bActivate);
static void fnSound(BOOL bActivate);
static void fnWalk(BOOL bActivate);
static void fnGamma(BOOL bActivate);
static void fnOptionPrevious(BOOL bActivate);
static void fnReturnToGame(BOOL bActivate);
static void fnSaveAndQuit(BOOL bActivate);

// menus
#define OPTION_SINGLE_SAVE 0
#define OPTION_SINGLE_LOAD 3
static TMenuItem sgSingleMenu[] = {
	{ mf_ENABLED, "Save Game", fnSave },
	{ mf_ENABLED, "Options", fnOptions },
	{ mf_ENABLED, "New Game", fnNew },
	{ mf_ENABLED, "Load Game", fnLoad },
	{ mf_ENABLED, "Quit Hellfire", fnQuit },
	{ mf_ENABLED, NULL, NULL }
};

#define OPTION_MULTI_RESTART 2
static TMenuItem sgMultiMenu[] = {
	{ mf_ENABLED, "Options", fnOptions },
	{ mf_ENABLED, "New Game", fnNew },
	{ mf_ENABLED, "Restart In Town", fnRestart },
	{ mf_ENABLED, "Quit Hellfire", fnQuit },
	{ mf_ENABLED, NULL, NULL }
};

#define OPTION_MUSIC		0
#define OPTION_SOUND		1
#define OPTION_GAMMA		2
#define OPTION_WALK			3
static TMenuItem sgOptionsMenu[] = {
	{ mf_ENABLED | mf_SLIDER, NULL, fnMusic },
	{ mf_ENABLED | mf_SLIDER, NULL, fnSound },
	{ mf_ENABLED | mf_SLIDER, "Gamma", fnGamma },
	{ mf_ENABLED | mf_SLIDER, NULL, fnWalk },
	{ mf_ENABLED, "Previous Menu", fnOptionPrevious },
	{ mf_ENABLED, NULL, NULL }
};

// strings for options menu
#define SM_ON		0
#define SM_DISABLED	1
static const char * sgszMusic[] = {
	"Music",
	"Music Disabled",
};
static const char * sgszSound[] = {
	"Sound",
	"Sound Disabled",
};
static const char * sgszWalk[] = {
	"Jog",
	"Walk",
};
const char * sgszWalkId = "Fast Walk";


//******************************************************************
//******************************************************************
static void gm_single_update(TMenuItem * pMenuItems) {
	app_assert(pMenuItems == sgSingleMenu);
	gmenu_set_enable(&sgSingleMenu[OPTION_SINGLE_LOAD],gbValidSaveFile);
	gmenu_set_enable(
		&sgSingleMenu[OPTION_SINGLE_SAVE],
		plr[myplr]._pmode != PM_DEATH && !deathflag
	);
}


//******************************************************************
//******************************************************************
static void gm_multi_update(TMenuItem * pMenuItems) {
	app_assert(pMenuItems == sgMultiMenu);
	gmenu_set_enable(&sgMultiMenu[OPTION_MULTI_RESTART],deathflag);
}


//******************************************************************
//******************************************************************
void gamemenu_on() {
	if (gbMaxPlayers == 1)
		gmenu_set_menu(sgSingleMenu,gm_single_update);
	else
		gmenu_set_menu(sgMultiMenu,gm_multi_update);

	// remove all junky windows under the menu
	extern BOOL clear_windows();
	clear_windows();
}


//******************************************************************
//******************************************************************
void gamemenu_off() {
	gmenu_set_menu(NULL,NULL);
}


//******************************************************************
//******************************************************************
void gamemenu_toggle() {
	if (gmenu_is_on())
		gamemenu_off();
	else
		gamemenu_on();
}


//******************************************************************
//******************************************************************
static void fnOptionPrevious(BOOL bActivate) {
	app_assert(bActivate);
	gamemenu_on();
}


//******************************************************************
//******************************************************************
static void fnNew(BOOL bActivate) {
	app_assert(bActivate);
	for (int i = 0; i < MAX_PLRS; i++) {
		plr[i]._pmode = PM_QUIT;
		plr[i]._pInvincible = TRUE;
	}

	deathflag = FALSE;
	force_redraw = FULLDRAW;
	FullBlit(TRUE);

	CornerStone.Initted = FALSE;
	// stop running game loop
	gbRunGame = FALSE;
	gamemenu_off();
}


//******************************************************************
//******************************************************************
static void fnQuit(BOOL bActivate) {
	app_assert(bActivate);
	fnNew(bActivate);
	gbRunGameResult = FALSE;
}


//******************************************************************
//******************************************************************
static void fnLoad(BOOL bActivate) {
	app_assert(bActivate);
	app_assert(gbMaxPlayers == 1);
	app_assert(gbValidSaveFile);

	// set window proc to function which will ignore input
	app_assert(ghMainWnd);
	WNDPROC saveProc = my_SetWindowProc(DisableInputWndProc);

	gamemenu_off();
	SetCursor(NO_CURSOR);
	InitDiabloMsg(MSG_LOADGAME);
	force_redraw = FULLDRAW;
	DrawAndBlit();
	GM_LoadGame(FALSE);
	ClrDiabloMsg();

	CornerStone.Initted = FALSE;
	
	PaletteFadeOut(FADE_FAST);
	deathflag = FALSE;
	force_redraw = FULLDRAW;
	DrawAndBlit();
	PaletteFadeIn(FADE_FAST);
	SetCursor(GLOVE_CURS);

	// flush out all the messages and restore old wndproc
	interface_msg_pump();
	saveProc = my_SetWindowProc(saveProc);
	app_assert(saveProc == DisableInputWndProc);
}


//******************************************************************
//******************************************************************
static void fnSave(BOOL bActivate) {
	app_assert(bActivate);
	app_assert(gbMaxPlayers == 1);
	if (curs != GLOVE_CURS) {
		// @@@ how bout an error msg
		return;
	}

	if (plr[myplr]._pmode == PM_DEATH || deathflag) {
		gamemenu_off();
		return;
	}

	// set window proc to function which will ignore input
	app_assert(ghMainWnd);
	WNDPROC saveProc = my_SetWindowProc(DisableInputWndProc);

	SetCursor(NO_CURSOR);
	gamemenu_off();
	InitDiabloMsg(MSG_SAVEGAME);
	force_redraw = FULLDRAW;
	DrawAndBlit();
	GM_SaveGame();
	ClrDiabloMsg();
	force_redraw = FULLDRAW;
	SetCursor(GLOVE_CURS);
	
	// update the Cornerstone of the World
	if (CornerStone.Initted)
		CornerstoneSave();

	// flush out all the messages and restore old wndproc
	interface_msg_pump();
	saveProc = my_SetWindowProc(saveProc);
	app_assert(saveProc == DisableInputWndProc);
}


//******************************************************************
//******************************************************************
static void fnRestart(BOOL bActivate) {
	app_assert(bActivate);
	NetSendCmd(TRUE, CMD_RETOWN);
}


//******************************************************************
//******************************************************************
static void set_volume_item(const char ** ppStrs,TMenuItem * pItem,LONG lVolume) {
	if (gbSndInited) {
		pItem->dwFlags |= mf_ENABLED | mf_SLIDER;
		pItem->pszStr = ppStrs[SM_ON];
		gmenu_set_slider_ticks(pItem,VOLUME_TICKS);
		gmenu_set_slider(pItem,VOLUME_MIN,VOLUME_MAX,lVolume);
	}
	else {
		pItem->dwFlags &= ~(mf_ENABLED | mf_SLIDER);
		pItem->pszStr = ppStrs[SM_DISABLED];
	}
}


//******************************************************************
//******************************************************************
static LONG get_volume_item(const TMenuItem * pItem) {
	return gmenu_get_slider(pItem,VOLUME_MIN,VOLUME_MAX);
}
	

//******************************************************************
//******************************************************************
static void set_music_item() {
	set_volume_item(sgszMusic,&sgOptionsMenu[OPTION_MUSIC],music_volume(VOLUME_READ));
}


//******************************************************************
//******************************************************************
static void set_sound_item() {
	set_volume_item(sgszSound,&sgOptionsMenu[OPTION_SOUND],sound_volume(VOLUME_READ));
}

//******************************************************************
//******************************************************************
static void set_walk_item() {
	gmenu_set_slider_ticks(&sgOptionsMenu[OPTION_WALK],2);
	gmenu_set_slider(
		&sgOptionsMenu[OPTION_WALK], 0, 1, gbWalkOn);
	sgOptionsMenu[OPTION_WALK].pszStr =
		 sgszWalk[(gbWalkOn)?SM_ON:SM_DISABLED];
}

//******************************************************************
//******************************************************************
static void set_gamma_item() {
	gmenu_set_slider_ticks(&sgOptionsMenu[OPTION_GAMMA],lGAMMA_TICKS);
	gmenu_set_slider(
		&sgOptionsMenu[OPTION_GAMMA],
		lGAMMA_MIN,
		lGAMMA_MAX,
		GammaLevel(lGAMMA_READ)
	);
}


//******************************************************************
//******************************************************************
static LONG get_gamma_item() {
	return gmenu_get_slider(
		&sgOptionsMenu[OPTION_GAMMA],
		lGAMMA_MIN,
		lGAMMA_MAX
	);
}


//******************************************************************
//******************************************************************
static void fnOptions(BOOL bActivate) {
	app_assert(bActivate);
	set_music_item();
	set_sound_item();
	set_walk_item();
	set_gamma_item();
	gmenu_set_menu(sgOptionsMenu,NULL);
}


//******************************************************************
//******************************************************************
static void fnMusic(BOOL bActivate) {
	if (bActivate) {
		if (gbMusicOn) {
			gbMusicOn = FALSE;
			music_stop();
			music_volume(VOLUME_MIN);
		}
		else {
			gbMusicOn = TRUE;
			music_volume(VOLUME_MAX);
			
			if (currlevel >= HIVESTART)		// fix this later JKE
			{
				music_start((currlevel > HIVEEND)? 5 : 6);
			}
			else
			music_start(leveltype);
		}
	}
	else {
		LONG lVolume = get_volume_item(&sgOptionsMenu[OPTION_MUSIC]);
		music_volume(lVolume);

		if (lVolume == VOLUME_MIN) {
			if (gbMusicOn) {
				gbMusicOn = FALSE;
				music_stop();
			}
		}
		else {
			if (! gbMusicOn) {
				gbMusicOn = TRUE;
				if (currlevel >= HIVESTART)		// fix this later JKE
				{
					music_start((currlevel > HIVEEND)? 5 : 6);
				}
				else
				music_start(leveltype);
			}
		}
	}

	set_music_item();
}


//******************************************************************
//******************************************************************
static void fnSound(BOOL bActivate) {
	if (bActivate) {
		if (gbSoundOn) {
			gbSoundOn = FALSE;
			sound_stop();
			sound_volume(VOLUME_MIN);
		}
		else {
			gbSoundOn = TRUE;
			sound_volume(VOLUME_MAX);
		}
	}
	else {
		LONG lVolume = get_volume_item(&sgOptionsMenu[OPTION_SOUND]);
		sound_volume(lVolume);

		if (lVolume == VOLUME_MIN) {
			if (gbSoundOn) {
				gbSoundOn = FALSE;
				sound_stop();
			}
		}
		else {
			if (! gbSoundOn) {
				gbSoundOn = TRUE;
			}
		}
	}
	
	PlaySFX(IS_TITLEMOV);
	set_sound_item();
}

static void fnWalk(BOOL bActivate) {
	if (gbMaxPlayers != 1)		// single-player only
		return;
		
	if (gbWalkOn) {
		gbWalkOn = FALSE;
	}
	else {
		gbWalkOn = TRUE;
	}
	SRegSaveValue(gszProgKey,sgszWalkId,0,gbWalkOn);
	
	PlaySFX(IS_TITLEMOV);
	set_walk_item();
}

//******************************************************************
//******************************************************************
static void fnGamma(BOOL bActivate) {
	LONG lGamma;
	if (bActivate) {
		lGamma = GammaLevel(lGAMMA_READ);
		if (lGamma == lGAMMA_MIN)
			lGamma = lGAMMA_MAX;
		else
			lGamma = lGAMMA_MIN;
	}
	else {
		lGamma = get_gamma_item();
	}

	GammaLevel(lGamma);
	set_gamma_item();
}

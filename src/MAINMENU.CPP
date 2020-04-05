/*-----------------------------------------------------------------------**
** 								Diablo
**
**							   Main Menu
**
** 				(C)1996 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/MAINMENU.CPP 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"storm.h"
#include	"diabloui.h"
#include	"items.h"
#include	"gendung.h"
#include	"player.h"
#include	"multi.h"
#include	"sound.h"


/*-----------------------------------------------------------------------**
// externs
**-----------------------------------------------------------------------*/
extern char gszPrintVersion[];
void CALLBACK menusnd_play(LPBYTE lpWave);
BOOL CALLBACK UiEnumHeroes(ENUMHEROPROC enumproc);
BOOL CALLBACK UiCreateHero(TPUIHEROINFO heroinfo);
BOOL CALLBACK UiDeleteHero(TPUIHEROINFO heroinfo);
BOOL CALLBACK UiGetDefaultCharStats(int heroclass, TPUIDEFSTATS defaultstats);
void CALLBACK menusnd_play(LPCSTR pszFile);
void play_movie(const char * pszMovie,BOOL bAllowCancel);
BOOL StartGame(BOOL bNewGame,BOOL bSinglePlayer);
extern DWORD gbWalkOn;
extern const char *sgszWalkId;
extern char gszProgKey[];

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#define PIXELS_PER_SEC	16

char gszHero[MAX_NAME_LEN];

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void menu_music() {
	static int snMusic = MUSIC_INTRO;
	music_start(snMusic);

#if !IS_VERSION(SHAREWARE)
	// look for a music track which is not the town, cause it's
	// too wimpy for initial theme music, and is not l1, cause the
	// players will hear waaaay too much of l1 music
	do {
		snMusic++; // next track
		if (snMusic == NUM_MUSIC) snMusic = 0; // handle wrap
	} while (snMusic == MUSIC_TOWN || snMusic == MUSIC_L1);
#endif
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static BOOL do_menu(DWORD selection) {
	if (selection == SELHERO_PREVIOUS)
		return TRUE;

	music_stop();

	BOOL bResult = StartGame(
		selection != SELHERO_CONTINUE,	// new game ?
		selection != SELHERO_CONNECT	// single player game ?
	);

	if (bResult) menu_music();
	return bResult;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static BOOL DoSinglePlayer() {
	while (1) {
		gbMaxPlayers = 1;
		DWORD selection = 0;
		
		if (!SRegLoadValue( gszProgKey,sgszWalkId,0,&gbWalkOn))
		{
			gbWalkOn = TRUE;
		}
		
		if (! UiSelHeroSingDialog(
			UiEnumHeroes,
			UiCreateHero,
			UiDeleteHero,
			UiGetDefaultCharStats,
			&selection,
			gszHero,
			&gnDifficulty,
			gbAllowBard,
			gbAllowBarbarian
		)) app_fatal(TEXT("Unable to display SelHeroSing"));

		if (selection == SELHERO_PREVIOUS)
			return TRUE;
		if (! do_menu(selection))
			return FALSE;
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static BOOL DoMultiPlayer() {
	while (1) {
		gbMaxPlayers = MAX_PLRS;
		DWORD selection = 0;
		gbWalkOn = FALSE;
		if (! UiSelHeroMultDialog(
			UiEnumHeroes,
			UiCreateHero,
			UiDeleteHero,
			UiGetDefaultCharStats,
			&selection,
			gszHero,
			gbAllowBard,
			gbAllowBarbarian
		)) app_fatal(TEXT("Can't load multiplayer dialog"));

		if (selection == SELHERO_PREVIOUS)
			return TRUE;
		if (! do_menu(selection))
			return FALSE;
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
static void play_intro() {
	music_stop();
	play_movie("gendata\\Hellfire.smk",TRUE);
	menu_music();
}
#endif


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void DiabloMenu() {
	menu_music();

	BOOL bDone = FALSE;
	while (! bDone) {
		DWORD selection = 0;
		if (! UiMainMenuDialog(gszPrintVersion, 
			&selection,
			gbAllowMultiPlayer, 
			menusnd_play))
			app_fatal(TEXT("Unable to display mainmenu"));

		switch (selection) {
			case MAINMENU_SINGLE_PLAYER:
				#if IS_VERSION(BETA)
				app_warning("Not available in beta version");
				#else
				if (! DoSinglePlayer()) bDone = TRUE;
				#endif
			break;

			case MAINMENU_MULTIPLAYER:
				if (! DoMultiPlayer()) bDone = TRUE;
			break;

			case MAINMENU_ATTRACT_MODE:
				// it crashes after 20 minutes, so stop doing this.

			break;
			case MAINMENU_REPLAY_INTRO:
				if (! bActive) break;
				#if !IS_VERSION(SHAREWARE)
				play_intro();
				#endif
			break;

			case MAINMENU_SHOW_CREDITS:
				UiCreditsDialog(PIXELS_PER_SEC);
			break;

			case MAINMENU_SUPPORT:
				UiSupportDialog(PIXELS_PER_SEC);
			break;
			
			case MAINMENU_EXIT_DIABLO:
				bDone = TRUE;
			break;
		}
	}

	music_stop();
}

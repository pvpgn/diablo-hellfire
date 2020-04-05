/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/GAMEMENU.H 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------*/


//******************************************************************
//******************************************************************
#define mf_ENABLED			0x80000000
#define mf_SLIDER			0x40000000
typedef void (* TMenuFcn)(BOOL bActivate);
typedef struct TMenuItem {
	DWORD dwFlags;
	const char * pszStr;
	TMenuFcn fnMenu;
} TMenuItem;

typedef void (* TMenuUpdateFcn)(TMenuItem * pMenuItems);

//******************************************************************
// menu functions
//******************************************************************
void gmenu_init();
void gmenu_free();
void gmenu_draw();
BOOL gmenu_is_on();
BOOL gmenu_click(BOOL bMouseDown);
BOOL gmenu_mousemove();
BOOL gmenu_key(WPARAM wKey);
void gmenu_set_menu(TMenuItem * pMenuItems,TMenuUpdateFcn fnUpdate);
void gmenu_set_enable(TMenuItem * pMenuItem,BOOL bEnable);

void gmenu_set_slider(TMenuItem * pItem,LONG lMin,LONG lMax,LONG lVal);
LONG gmenu_get_slider(const TMenuItem * pItem,LONG lMin,LONG lMax);
void gmenu_set_slider_ticks(TMenuItem * pMenuItem,DWORD dwTicks);


//******************************************************************
// gamemenu functions
//******************************************************************
void gamemenu_toggle();
void gamemenu_on();
void gamemenu_off();


//******************************************************************
// other prototypes
//******************************************************************
void GM_LoadGame(BOOL);
void SaveLevel();
void LoadLevel();

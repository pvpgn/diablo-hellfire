/*-----------------------------------------------------------------------**
** 								Diablo
**
**							 Game Menu file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/GMENU.CPP 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include "diablo.h"
#pragma hdrstop
#include "sound.h"
#include "gendung.h"
#include "gamemenu.h"
#include "scrollrt.h"
#include "engine.h"
#include "effects.h"


//******************************************************************
// extern
//******************************************************************
void RedBack();


//******************************************************************
// kerning
//******************************************************************
#define KERNSPACE	2
static const BYTE mfonttrans[128] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,					// 0-15
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,					// 16-31
	0, 37, 49, 38, 0, 39, 40, 47, 42, 43, 41, 45, 52, 44, 53, 55,	// 32-47
	36, 27, 28, 29, 30, 31, 32, 33, 34, 35, 51, 50, 0, 46, 0, 54,	// 48-63
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,			// 64-79
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 42, 0, 43, 0, 0,	// 80-95
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,			// 96-111
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 20, 0, 21, 0, 0		// 112-127
};
static const BYTE mfontkern[56] = { 18,										// Space/Invalid
	33, 21, 26, 28, 19, 19, 26, 25, 11, 12, 25, 19, 34, 28, 32, 20,			// a-p
	32, 28, 20, 28, 36, 35, 46, 33, 33, 24,									// q-z
	11, 23, 22, 22, 21, 22, 21, 21, 21, 32,									// 1-0
	10, 20, 36, 31, 17, 13, 12, 13, 18, 16, 11, 20, 21, 11, 10, 12, 11, 21, 23	// misc
};


//******************************************************************
// private data
//******************************************************************
static BYTE sgbGameSpin;
static BYTE sgbTitleAnimate;
static BYTE * sgpLogo;
static BYTE * sgpSpinCels;
static BYTE * sgpMenuCels;
static BYTE * sgpSliderCels;
static BYTE * sgpBarCels;
static TMenuItem * sgpMenu;
static TMenuUpdateFcn sgfnMenuUpdateFcn;
static TMenuItem * sgpCurrItem;
static DWORD sgdwMenuItems;
static long sglSpinnerTime;
static long sglTitleTime;
static BYTE sgbTracking;

#define MENU_STARTY			320
#define MENU_CLICKY			(MENU_STARTY - 203)
#define MENU_LINEHGT		45
#define SLIDER_WDT			256
#define SLIDER_ITEM_WDT		27
#define SLIDER_TOTAL_WDT	490

#define ENABLE_LVAL		0
#define DISABLE_LVAL	15

#define MIN_SLIDER_TICKS	2
#define SLIDER_TICK_SHIFT	12
#define MAX_SLIDER_TICKS	0xfff
#define mf_SLIDER_VAL_MASK	0x00000fff
#define mf_SLIDER_TICK_MASK	0x00fff000

#define SPINNER_TIME		25	// milliseconds
#define TITLE_TIME			25	// milliseconds


//******************************************************************
//******************************************************************
static void DrawBigFontXY(int x, int y,const char * pszStr) {
	app_assert(pszStr);
	while (*pszStr) {
		BYTE c = char2print(*pszStr++);
		c = mfonttrans[c];
		if (c != 0) DrawCelL(x, y, sgpMenuCels, c, 46);
		x += mfontkern[c] + KERNSPACE;
	}
}


//******************************************************************
//******************************************************************
void DrawPause() {
	if (currlevel != 0) RedBack();
	if (! sgpMenu) {
		nLVal = 0; // set light level
		DrawBigFontXY(316, 336, "Pause");
	}
}


//******************************************************************
//******************************************************************
void gmenu_free() {
	DiabloFreePtr(sgpLogo);
	DiabloFreePtr(sgpMenuCels);
	DiabloFreePtr(sgpSpinCels);
	DiabloFreePtr(sgpSliderCels);
	DiabloFreePtr(sgpBarCels);
}


//******************************************************************
//******************************************************************
void gmenu_init() {
	sgbGameSpin = 1;
	sgbTitleAnimate = 1;
	sgpMenu = NULL;
	sgpCurrItem = NULL;
	sgfnMenuUpdateFcn = NULL;
	sgdwMenuItems = 0;
	sgbTracking = FALSE;

	app_assert(! sgpLogo);
	//sgpLogo = LoadFileInMemSig("Data\\Diabsmal.CEL",NULL,'MENU');
	sgpLogo = LoadFileInMemSig("Data\\hf_logo3.CEL",NULL,'MENU');
	sgpMenuCels = LoadFileInMemSig("Data\\BigTGold.CEL",NULL,'MENU');
	sgpSpinCels = LoadFileInMemSig("Data\\PentSpin.CEL",NULL,'MENU');
	sgpSliderCels = LoadFileInMemSig("Data\\option.CEL",NULL,'MENU');
	sgpBarCels = LoadFileInMemSig("Data\\optbar.CEL",NULL,'MENU');
}


//******************************************************************
//******************************************************************
BOOL gmenu_is_on() {
	return sgpMenu != NULL;
}


//******************************************************************
//******************************************************************
static void gmenu_change(BOOL bNext) {
	if (! sgpCurrItem) return;
	sgbTracking = FALSE;
	for (DWORD d = sgdwMenuItems; d--; ) {
		if (bNext) {
			// next item
			sgpCurrItem++;

			// wrap at end of list
			if (! sgpCurrItem->fnMenu)
				sgpCurrItem = sgpMenu;
		}
		else {
			// wrap at beginning of list
			if (sgpCurrItem == sgpMenu)
				sgpCurrItem = sgpMenu + sgdwMenuItems;

			// prev item
			sgpCurrItem--;
		}

		if (sgpCurrItem->dwFlags & mf_ENABLED) {
			if (d) PlaySFX(IS_TITLEMOV);
			break;
		}
	}
}


//******************************************************************
//******************************************************************
void gmenu_set_menu(TMenuItem * pMenuItems,TMenuUpdateFcn fnUpdate) {
	PauseMode = 0;

	sgpMenu = pMenuItems;
	sgbTracking = FALSE;
	sgfnMenuUpdateFcn = fnUpdate;
	if (sgfnMenuUpdateFcn) sgfnMenuUpdateFcn(sgpMenu);

	// calculate number of items in menu
	sgdwMenuItems = 0;
	if (sgpMenu) {
		for (TMenuItem * pItem = sgpMenu; pItem->fnMenu; pItem++)
			sgdwMenuItems++;
	}

	// move to the first active item in the menu
	sgpCurrItem = sgpMenu + sgdwMenuItems - 1;
	gmenu_change(TRUE);
}


//******************************************************************
//******************************************************************
static void DrawBar(DWORD x,DWORD y,DWORD wdt,DWORD hgt) {
	app_assert(gpBuffer);
	BYTE * pbDst = gpBuffer + nBuffWTbl[y] + x;
	while (hgt--) {
		FillMemory(pbDst,wdt,0xcd);
		pbDst -= 768;
	}
}


//******************************************************************
//******************************************************************
static DWORD gmenu_calc_item_width(const TMenuItem * pItem) {
	// hardcode the size of sliders so they all justify uniformly
	// so that the slider bars line up vertically
	if (pItem->dwFlags & mf_SLIDER)
		return SLIDER_TOTAL_WDT;

	DWORD wdt = 0;
	const char * pszStr = pItem->pszStr;
	while (*pszStr) {
		BYTE c = char2print(*pszStr++);
		c = mfonttrans[c];
		wdt += mfontkern[c] + KERNSPACE;
	}
	wdt -= KERNSPACE;
	return wdt;
}


//******************************************************************
//******************************************************************
static void gmenu_draw_item(const TMenuItem * pItem,int yPos) {
	DWORD wdt = gmenu_calc_item_width(pItem);

	if (pItem->dwFlags & mf_SLIDER)  {
		int xPos = 640/2 + wdt/2 + 43 - SLIDER_WDT - SLIDER_ITEM_WDT;
		DrawCel(xPos + 0,yPos - 10,sgpBarCels, 1, 287);
		DWORD dwLen = pItem->dwFlags & mf_SLIDER_VAL_MASK;

		// scale 0..ticks to 0..SLIDER_WDT
		DWORD dwTicks = pItem->dwFlags & mf_SLIDER_TICK_MASK;
		dwTicks >>= SLIDER_TICK_SHIFT;
		if (dwTicks < MIN_SLIDER_TICKS) dwTicks = MIN_SLIDER_TICKS;
		dwLen *= SLIDER_WDT;
		dwLen /= dwTicks;

		DrawBar(xPos + 2,yPos - 12,dwLen + SLIDER_ITEM_WDT/2,28);
		xPos += dwLen;
		DrawCel(xPos + 2,yPos - 12,sgpSliderCels, 1, 27);
	}

	// draw string with lighting
	int xPos = 640/2 - wdt/2 + 64;
	nLVal = (pItem->dwFlags & mf_ENABLED) ? ENABLE_LVAL : DISABLE_LVAL;
	DrawBigFontXY(xPos, yPos, pItem->pszStr);
	
	if (pItem == sgpCurrItem) {
		DrawCel(xPos -      54, yPos + 1, sgpSpinCels, sgbGameSpin, 48);
		DrawCel(xPos + wdt + 4, yPos + 1, sgpSpinCels, sgbGameSpin, 48);
	}
}


//******************************************************************
//******************************************************************
void gmenu_draw() {
	if (! sgpMenu) return;
	if (sgfnMenuUpdateFcn) sgfnMenuUpdateFcn(sgpMenu);

	const TMenuItem * pItem;
	long lCurrTime = (long) GetTickCount();
	//DrawCel(236, 262, sgpLogo, 1, 296);
	if (lCurrTime - sglTitleTime > TITLE_TIME){
		++sgbTitleAnimate;
		if (sgbTitleAnimate > 16) sgbTitleAnimate = 1;
		sglTitleTime = lCurrTime;
	}
	DrawCel(169, 262, sgpLogo, sgbTitleAnimate, 430);

	// draw items
	DWORD yPos = MENU_STARTY;
	for (pItem = sgpMenu; pItem->fnMenu; pItem++) {
		gmenu_draw_item(pItem,yPos);
		yPos += MENU_LINEHGT;
	}

	// adjust spinner
	if (lCurrTime - sglSpinnerTime > SPINNER_TIME) {
		sgbGameSpin++;
		if (sgbGameSpin == 9) sgbGameSpin = 1;
		sglSpinnerTime = lCurrTime;
	}
}


//******************************************************************
//******************************************************************
static void gmenu_slider(BOOL bNext) {
	if (! (sgpCurrItem->dwFlags & mf_SLIDER)) return;

	LONG lVal = sgpCurrItem->dwFlags & mf_SLIDER_VAL_MASK;
	LONG lTicks = sgpCurrItem->dwFlags & mf_SLIDER_TICK_MASK;
	lTicks >>= SLIDER_TICK_SHIFT;
	if (bNext) {
		if (lVal == lTicks) return;
		lVal++;
	}
	else {
		if (! lVal) return;
		lVal--;
	}

	sgpCurrItem->dwFlags &= ~mf_SLIDER_VAL_MASK;
	sgpCurrItem->dwFlags |= lVal;
	sgpCurrItem->fnMenu(FALSE);
}


//******************************************************************
//******************************************************************
BOOL gmenu_key(WPARAM wKey) {

	if (! sgpMenu) return FALSE;
	app_assert(sgpCurrItem);

	switch (wKey) {
		case VK_SPACE:
			// allow spacebar
		return FALSE;

		case VK_LEFT:
			gmenu_slider(FALSE);
		break;

		case VK_RIGHT:
			gmenu_slider(TRUE);
		break;

		case VK_UP:
			gmenu_change(FALSE);
		break;

		case VK_DOWN:
			gmenu_change(TRUE);
		break;

		case VK_RETURN:
			app_assert(sgpCurrItem);
			if (sgpCurrItem->dwFlags & mf_ENABLED) {
				PlaySFX(IS_TITLEMOV);
				sgpCurrItem->fnMenu(TRUE);
			}
		break;

		case VK_ESCAPE:
			PlaySFX(IS_TITLEMOV);
			gmenu_set_menu(NULL,NULL);
		break;
	}

	return TRUE;
}


//******************************************************************
//******************************************************************
static BYTE gmenu_mouse_in_slider(LONG * plOffset) {
	app_assert(plOffset);
	*plOffset = 640/2 + SLIDER_TOTAL_WDT/2 - SLIDER_WDT - SLIDER_ITEM_WDT;
	if (MouseX < *plOffset) {
		*plOffset = 0;
		return FALSE;
	}
	else if (MouseX > *plOffset + SLIDER_WDT) {
		*plOffset = SLIDER_WDT;
		return FALSE;
	}

	*plOffset = MouseX - *plOffset;
	return TRUE;
}


//******************************************************************
//******************************************************************
BOOL gmenu_mousemove() {
	if (! sgbTracking) return FALSE;
	app_assert(sgpCurrItem);

	// get position = 0..SLIDER_WDT
	LONG lOffset;
	gmenu_mouse_in_slider(&lOffset);

	// scale to 0..ticks
	LONG lTicks = sgpCurrItem->dwFlags & mf_SLIDER_TICK_MASK;
	lTicks >>= SLIDER_TICK_SHIFT;
	lOffset *= lTicks;
	lOffset /= SLIDER_WDT;

	// set item value
	sgpCurrItem->dwFlags &= ~mf_SLIDER_VAL_MASK;
	sgpCurrItem->dwFlags |= lOffset;
	sgpCurrItem->fnMenu(FALSE);
	return TRUE;
}


//******************************************************************
//******************************************************************
BOOL gmenu_click(BOOL bMouseDown) {
	// handle mouseup
	if (! bMouseDown) {
		if (! sgbTracking) return FALSE;
		sgbTracking = FALSE;
		return TRUE;
	}

	if (! sgpMenu) return FALSE;
	if (MouseY >= 352) return FALSE;

	int nItem = MouseY - MENU_CLICKY;
	if (nItem < 0) return TRUE;
	nItem /= MENU_LINEHGT;
	if ((DWORD) nItem >= sgdwMenuItems) return TRUE;

	// only click menu item if it is enabled
	TMenuItem * pItem = sgpMenu + nItem;
	if (! (pItem->dwFlags & mf_ENABLED)) return TRUE;

	DWORD wdt = gmenu_calc_item_width(pItem);
	if ((DWORD) MouseX < 640/2-wdt/2) return TRUE;
	if ((DWORD) MouseX > 640/2+wdt/2) return TRUE;

	// set current item
	sgpCurrItem = pItem;
	PlaySFX(IS_TITLEMOV);

	if (pItem->dwFlags & mf_SLIDER) {
		LONG lTemp;
		sgbTracking = gmenu_mouse_in_slider(&lTemp);
		gmenu_mousemove();
	}
	else {
		sgpCurrItem->fnMenu(TRUE);
	}

	return TRUE;
}


//******************************************************************
//******************************************************************
void gmenu_set_enable(TMenuItem * pMenuItem,BOOL bEnable) {
	app_assert(pMenuItem);
	if (bEnable)
		pMenuItem->dwFlags |= mf_ENABLED;
	else
		pMenuItem->dwFlags &= ~mf_ENABLED;
}


//******************************************************************
//******************************************************************
void gmenu_set_slider(TMenuItem * pItem,LONG lMin,LONG lMax,LONG lVal) {
	app_assert(pItem);

	LONG lTicks = pItem->dwFlags & mf_SLIDER_TICK_MASK;
	lTicks >>= SLIDER_TICK_SHIFT;
	if (lTicks < MIN_SLIDER_TICKS) lTicks = MIN_SLIDER_TICKS;

	// make lVal zero-based
	lVal -= lMin;

	// scale to slider
	lVal *= lTicks;
	lVal += (lMax - lMin - 1) / 2;
	lVal /= lMax - lMin;

	// store into menu item
	pItem->dwFlags &= ~mf_SLIDER_VAL_MASK;
	pItem->dwFlags |= lVal;
}


//******************************************************************
//******************************************************************
LONG gmenu_get_slider(const TMenuItem * pItem,LONG lMin,LONG lMax) {
	app_assert(pItem);

	// get value from menu item
	LONG lVal = pItem->dwFlags & mf_SLIDER_VAL_MASK;

	LONG lTicks = pItem->dwFlags & mf_SLIDER_TICK_MASK;
	lTicks >>= SLIDER_TICK_SHIFT;
	if (lTicks < MIN_SLIDER_TICKS) lTicks = MIN_SLIDER_TICKS;

	// restore scale
	lVal *= lMax - lMin;
	lVal += (lTicks - 1) / 2;
	lVal /= lTicks;

	// re-base
	lVal += lMin;

	return lVal;
}


//******************************************************************
//******************************************************************
void gmenu_set_slider_ticks(TMenuItem * pItem,DWORD dwTicks) {
	app_assert(pItem);
	app_assert(dwTicks >= MIN_SLIDER_TICKS && dwTicks <= MAX_SLIDER_TICKS);

	pItem->dwFlags &= ~mf_SLIDER_TICK_MASK;
	pItem->dwFlags |= mf_SLIDER_TICK_MASK & (dwTicks << SLIDER_TICK_SHIFT);
}

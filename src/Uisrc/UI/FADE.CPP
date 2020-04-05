//****************************************************************************
//	fade.cpp
//	created 10.28.96
//****************************************************************************


#include "pch.h"


//****************************************************************************
//****************************************************************************
	#define FADE_TIMER_ID		16
	#define FADE_TIMER_DELAY	50


//****************************************************************************
//****************************************************************************
	static int sgFadeStep;
	static BOOL sgFading;
	static PALETTEENTRY sgFadePal[PALETTE_REGISTERS];


//****************************************************************************
//****************************************************************************
static void RestoreWindowsColors(void) {
	HPALETTE hPal;

	// Get Windows Default colors from the system
	hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

	GetPaletteEntries(hPal, 0, 10, &sgFadePal[0]);
	GetPaletteEntries(hPal, 10, 10, &sgFadePal[PALETTE_REGISTERS-10]);
	SDrawUpdatePalette(0, PALETTE_REGISTERS, &sgFadePal[0], TRUE);
}


//****************************************************************************
// an async fade function
//****************************************************************************
void UiVidFade(int max, int curr) {
	PALETTEENTRY * PalEntry = UiPaletteEntry(0);

	if (max == curr) {
		memcpy(sgFadePal, PalEntry, (PALETTE_REGISTERS * sizeof(PALETTEENTRY)));
	}
	else if (curr == 0) {
		memcpy(sgFadePal, PalEntry, (PALETTE_REGISTERS * sizeof(PALETTEENTRY)));
		UiClearPalette(sgFadePal);
	}
	else {
		for (int palreg = 0; palreg < PALETTE_REGISTERS; palreg++, PalEntry++) {
			sgFadePal[palreg].peRed   = (PalEntry->peRed * curr) / max;
			sgFadePal[palreg].peGreen = (PalEntry->peGreen * curr) / max;
			sgFadePal[palreg].peBlue  = (PalEntry->peBlue * curr) / max;
		}
	}
	SDrawUpdatePalette(0, PALETTE_REGISTERS, sgFadePal, TRUE);
}


//****************************************************************************
//****************************************************************************
void UiVidFadeOut(int steps) {
	PALETTEENTRY * PalEntry = UiPaletteEntry(0);

	memcpy(sgFadePal, PalEntry, (PALETTE_REGISTERS * sizeof(PALETTEENTRY)));
	for (int index = 0; index < steps; index++) {
		PalEntry = UiPaletteEntry(0);
		for (int palreg = 0; palreg < PALETTE_REGISTERS; palreg++, PalEntry++) {
			sgFadePal[palreg].peRed   -= (PalEntry->peRed / steps);
			sgFadePal[palreg].peGreen -= (PalEntry->peGreen / steps);
			sgFadePal[palreg].peBlue  -= (PalEntry->peBlue / steps);
		}
		SDrawUpdatePalette(0, PALETTE_REGISTERS, sgFadePal, TRUE);
	}

	UiClearPalette(sgFadePal);
	SDrawUpdatePalette(0, PALETTE_REGISTERS, sgFadePal, TRUE);
	UiClearCursor();
	SDrawClearSurface(SDRAW_SURFACE_FRONT);
	RestoreWindowsColors();
}


//****************************************************************************
//****************************************************************************
static void CALLBACK UiFadeTimerProc(HWND  window,
									UINT  message,
									UINT  timerid,
									DWORD systime) {
	if (sgFadeStep <= DEFAULT_FADE_STEPS) {
		UiVidFade(DEFAULT_FADE_STEPS, sgFadeStep);
		sgFadeStep++;
	}
	else {
		SDlgKillTimer(window, FADE_TIMER_ID);
	}
}


//****************************************************************************
//****************************************************************************
BOOL UiIsFading() {
	return sgFading && (sgFadeStep <= DEFAULT_FADE_STEPS);
}


//****************************************************************************
//****************************************************************************
void UiFadeAbort(HWND window) {
	if (! UiIsFading()) return;
	sgFading = FALSE;
}


//****************************************************************************
//****************************************************************************
void UiFadeInit(HWND window, BOOL turnoncursor) {
	UiFlushIOMessages(GetParent(window));
	if (turnoncursor)
		UiSetCursor();
	sgFading = FALSE;
	sgFadeStep = 0;
}


//****************************************************************************
// use this for screens that fade out but don't fade in
//****************************************************************************
void UiAltFadeInit(HWND window) {
	UiFlushIOMessages(GetParent(window));
}


//****************************************************************************
//****************************************************************************
void UiFadeStart(HWND window) {
	if (! sgFading) {
		BOOL result = SDlgSetTimer(window, FADE_TIMER_ID, FADE_TIMER_DELAY, UiFadeTimerProc);
		sgFading = TRUE;
	}
}

/****************************************************************************
*
*  CLRPREF.CPP
*
*  Handle app specific system color preferences.
*  By Michael Morhaime
*
***/

#include "pch.h"


typedef struct _prefcolors {
	int nElements;
	INT *pElements;
	COLORREF *pAppValues;
	COLORREF *pWindowsValues;
} TPREFCOLORS;
static TPREFCOLORS sgPrefColors = { 0, NULL, NULL, NULL };



//===========================================================================
//===========================================================================
static void ColorPrefRestoreWindowsColors(void) {
	if (!sgPrefColors.nElements)
		return;

	SetSysColors(sgPrefColors.nElements, sgPrefColors.pElements, sgPrefColors.pWindowsValues);
}

//===========================================================================
static void ColorPrefSetAppColors(void) {
	if (!sgPrefColors.nElements) 
		return;

	SetSysColors(sgPrefColors.nElements, sgPrefColors.pElements, sgPrefColors.pAppValues);
}


//===========================================================================
//===========================================================================
//===========================================================================
void ColorPrefDestroy(void) {
	// Restore Windows system colors
	ColorPrefRestoreWindowsColors();

	// Free PrefColors table
	if (sgPrefColors.pElements) {
		FREE(sgPrefColors.pElements);
		sgPrefColors.pElements = NULL;
	}

	if (sgPrefColors.pAppValues) {
		FREE(sgPrefColors.pAppValues);
		sgPrefColors.pAppValues = NULL;
	}

	if (sgPrefColors.pWindowsValues) {
		FREE(sgPrefColors.pWindowsValues);
		sgPrefColors.pWindowsValues = NULL;
	}

	sgPrefColors.nElements = 0;
}


//===========================================================================
BOOL ColorPrefInit(SNETGETDATAPROC getdatacallback) {
	SNET_DATA_SYSCOLORTABLE *pSysColorTbl;
	DWORD dwBytes;
	int nEntries;

	if (!UiGetData(getdatacallback, SNET_DATA_SYSCOLORS, (LPBYTE *)&pSysColorTbl, &dwBytes))
		return 0;

	nEntries = dwBytes/sizeof(SNET_DATA_SYSCOLORTABLE);
	if (!nEntries)
		return 0;

	sgPrefColors.pElements = (INT *)ALLOC(nEntries*sizeof(int));
	sgPrefColors.pAppValues = (COLORREF *)ALLOC(nEntries*sizeof(COLORREF));
	sgPrefColors.pWindowsValues = (COLORREF *)ALLOC(nEntries*sizeof(COLORREF));

	if ( sgPrefColors.pElements == NULL ||
		 sgPrefColors.pAppValues == NULL ||
		 sgPrefColors.pWindowsValues == NULL ) {
		ColorPrefDestroy();
		FREE(pSysColorTbl);
		return 0;
	}

	// Fill in structure with actual values.
	sgPrefColors.nElements = nEntries;
	for (int i=0; i<nEntries; i++) {
		sgPrefColors.pElements[i] = pSysColorTbl[i].syscolor;
		sgPrefColors.pAppValues[i] = pSysColorTbl[i].rgb;
		sgPrefColors.pWindowsValues[i] = GetSysColor(pSysColorTbl[i].syscolor);
	}

	// Free syscolor tbl now, we won't need it anymore
	FREE(pSysColorTbl);

	// Now go ahead and set the actual colors
	ColorPrefSetAppColors();
	return 1;
}

//===========================================================================
void ColorPrefActivate(BOOL bActivate) {
	if (bActivate)
		ColorPrefSetAppColors();
	else
		ColorPrefRestoreWindowsColors();
}
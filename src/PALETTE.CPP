/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Palette file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/PALETTE.CPP 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"storm.h"
#include	"palette.h"
#include	"engine.h"
#include	<math.h>
#include "resource.h"


/*-----------------------------------------------------------------------**
** extern
**-----------------------------------------------------------------------*/
extern char gszProgKey[];
void ErrorDlg(int nDlgId,DWORD dwErr,const char * pszFile,int nLine);


/*-----------------------------------------------------------------------**
** File Variables
**-----------------------------------------------------------------------*/
static LONG sglGamma = lGAMMA_MAX;
static int sgnStaticColors2 = 0;
static PALETTEENTRY sgLoad[256];	// original loaded palette -- used as reference
									//							  for gamma conversion
static PALETTEENTRY sgBase[256];	// palettized version of sgLoad
static PALETTEENTRY sgCurr[256];	// palette to be displayed -- includes rotations

static const char sgszGamma[] = "Gamma Correction";
static BYTE sgbFadedIn = TRUE;


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void SetPaletteEntries() {
	if (! lpDDPal) return;

	int nStart;
	int nColors;
	if (! fullscreen) {
		nStart = sgnStaticColors2;
		nColors = 256 - sgnStaticColors2*2;
	}
	else {
		nStart = 0;
		nColors = 256;
	}

//	HRESULT ddrval = lpDDPal->SetEntries(0,nStart,nColors,&sgCurr[nStart]);
//	ddraw_assert(ddrval);
	SDrawUpdatePalette(nStart,nColors,&sgCurr[nStart]);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void GammaPalette(PALETTEENTRY * pDst, const PALETTEENTRY * pSrc, int nNum) {
	double gamma = sglGamma / 100.0;
	for(int i = 0; i < nNum; i++,pDst++,pSrc++) {
		pDst->peRed = (BYTE)(256.0*pow(((double)pSrc->peRed)/256.0, gamma));
		pDst->peGreen = (BYTE)(256.0*pow(((double)pSrc->peGreen)/256.0, gamma));
		pDst->peBlue = (BYTE)(256.0*pow(((double)pSrc->peBlue)/256.0, gamma));
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void SavePaletteSettings() {
	SRegSaveValue(gszProgKey,sgszGamma,0,sglGamma);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void GetPaletteSettings() {
	DWORD dwTemp = (DWORD) sglGamma;
	if (! SRegLoadValue(gszProgKey,sgszGamma,0,&dwTemp))
		dwTemp = lGAMMA_MAX;

	sglGamma = (LONG) dwTemp;
	if (sglGamma < lGAMMA_MIN)
		sglGamma = lGAMMA_MIN;
	else if (sglGamma > lGAMMA_MAX)
		sglGamma = lGAMMA_MAX;
	sglGamma -= sglGamma % lGAMMA_STEP;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void FixWindowsPal() {
	int i;

	// reserve all colors so no mapping occurs
	for (i = 0; i < 256; i++)
		sgCurr[i].peFlags = PC_NOCOLLAPSE | PC_RESERVED;

	if (fullscreen) return;

	// get static color count
	HDC hDC = GetDC(NULL);
	sgnStaticColors2 = GetDeviceCaps(hDC, NUMRESERVED) / 2;

	// get the lower windows color set
	GetSystemPaletteEntries(hDC, 0, sgnStaticColors2, &sgCurr[0]);
	for (i = 0; i < sgnStaticColors2; i++) sgCurr[i].peFlags = 0;
		
	// get the upper windows color set
	i = 256 - sgnStaticColors2;
	GetSystemPaletteEntries(hDC, i, sgnStaticColors2, &sgCurr[i]);
	for ( ; i < 256; i++) sgCurr[i].peFlags = 0;

	ReleaseDC(NULL, hDC);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void CreatePalette() {
	GetPaletteSettings();
	CopyMemory(sgCurr,sgLoad,sizeof(sgCurr));
	FixWindowsPal();
	HRESULT ddrval = lpDD->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256 | DDPCAPS_INITIALIZE,sgCurr,&lpDDPal,NULL);
	if (ddrval != DD_OK) ErrorDlg(IDD_DDRAW_PAL_ERR,ddrval,__FILE__,__LINE__);

    ddrval = lpDDSPrimary->SetPalette(lpDDPal);
	if (ddrval != DD_OK) ErrorDlg(IDD_DDRAW_PAL_ERR,ddrval,__FILE__,__LINE__);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void LoadPalette(const char * pszFileName) {
	struct RGB {
		BYTE red;
		BYTE green;
		BYTE blue;
	} pal[256];

	HSFILE hFile;
	app_assert(pszFileName);
	patSFileOpenFile(pszFileName,&hFile);
	patSFileReadFile(hFile,&pal[0],sizeof(pal));
	patSFileCloseFile(hFile);

	for (int i = 0; i < 256; i++) {
		sgLoad[i].peRed = pal[i].red;
		sgLoad[i].peGreen = pal[i].green;
		sgLoad[i].peBlue = pal[i].blue;
		sgLoad[i].peFlags = 0;
	}
//	GammaPalette(sgBase, sgLoad, 256);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void LoadRndLvlPal(int l) {

	if (l == 0) {
		LoadPalette("Levels\\TownData\\Town.pal");
	}
	else {
		char filestr[MAX_PATH];
		int palnum = random(0, 4) + 1;
		sprintf(filestr, "Levels\\L%iData\\L%i_%i.PAL", l, l, palnum);
		// JKE add our palettes here
		if (l == 5)
			sprintf(filestr, "NLevels\\L5Data\\L5Base.PAL");
		if (l == 6)
		{
			if (!gbOurNest)
				++palnum;
			sprintf(filestr, "NLevels\\L%iData\\L%iBase%i.PAL", l, l, palnum);
		}
		LoadPalette(filestr);
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void ResetPal() {
	if (lpDDSPrimary && lpDDSPrimary->IsLost() == DDERR_SURFACELOST)
		if (DD_OK != lpDDSPrimary->Restore()) return;
	SDrawRealizePalette();
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void GammaUp() {
	if(sglGamma < lGAMMA_MAX) {
		sglGamma += lGAMMA_STEP;
		if (sglGamma > lGAMMA_MAX)
			sglGamma = lGAMMA_MAX;
		GammaPalette(sgCurr,sgBase,256);
		SetPaletteEntries();
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void GammaDown() {
	if(sglGamma > lGAMMA_MIN) {
		sglGamma -= lGAMMA_STEP;
		if (sglGamma < lGAMMA_MIN)
			sglGamma = lGAMMA_MIN;
		GammaPalette(sgCurr,sgBase,256);
		SetPaletteEntries();
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
LONG GammaLevel(LONG lGamma) {
	if (lGamma != lGAMMA_READ) {
		// invert gamma hi/lo
		sglGamma = lGAMMA_MAX - lGamma + lGAMMA_MIN;
		GammaPalette(sgCurr,sgBase,256);
		SetPaletteEntries();
	}

	// invert gamma hi/lo
	return lGAMMA_MAX - sglGamma + lGAMMA_MIN;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void SetFadeLevel(int fadeval) {
	if (! lpDD) return;

	for (int i = 0; i < 255; i++) {
		DWORD pv;
		pv = sgBase[i].peRed * fadeval;
		sgCurr[i].peRed = (BYTE) (pv/256);
		pv = sgBase[i].peGreen * fadeval;
		sgCurr[i].peGreen = (BYTE) (pv/256);
		pv = sgBase[i].peBlue * fadeval;
		sgCurr[i].peBlue = (BYTE) (pv/256);
	}

//	sgCurr[i].peRed = 0xff;
//	sgCurr[i].peGreen = 0xff;
//	sgCurr[i].peBlue = 0xff;

	Sleep(3);
	lpDD->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN,NULL);
	SetPaletteEntries();
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void BlackPalette() {
	SetFadeLevel(0);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void PaletteFadeIn(int faderate) {
	GammaPalette(sgBase,sgLoad,256);
	for (int i = 0; i < 256; i += faderate)
		SetFadeLevel(i);
	SetFadeLevel(256);

	// un-gamma-fy base palette
	CopyMemory(sgBase,sgLoad,sizeof(sgBase));
	sgbFadedIn = TRUE;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void PaletteFadeOut(int faderate) {
	if (! sgbFadedIn) return;
	for (int i = 256; i > 0; i -= faderate)
		SetFadeLevel(i);
	SetFadeLevel(0);
	sgbFadedIn = FALSE;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void LavaCycle() {
	PALETTEENTRY temp = sgCurr[1];
	for (int i = 1; i < 31; i++) {
		sgCurr[i].peRed = sgCurr[i + 1].peRed;
		sgCurr[i].peGreen = sgCurr[i + 1].peGreen;
		sgCurr[i].peBlue = sgCurr[i + 1].peBlue;
		// don't copy flags
	}

	sgCurr[i].peRed = temp.peRed;
	sgCurr[i].peGreen = temp.peGreen;
	sgCurr[i].peBlue = temp.peBlue;
	// don't copy flags

	SetPaletteEntries();
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
// JKE new cycle to do twin effects
void TwinCycleCrypt() {
	int i;
	static int toggle = 0;
	static int toggle2 = 0;
	PALETTEENTRY temp;

	if (toggle2 > 1)
	{
		temp = sgCurr[15];
		for (i = 15; i > 1; --i) {
			sgCurr[i].peRed = sgCurr[i - 1].peRed;
			sgCurr[i].peGreen = sgCurr[i - 1].peGreen;
			sgCurr[i].peBlue = sgCurr[i - 1].peBlue;
			// don't copy flags
		}
		sgCurr[i].peRed = temp.peRed;
		sgCurr[i].peGreen = temp.peGreen;
		sgCurr[i].peBlue = temp.peBlue;
		// don't copy flags
		toggle2 = 0;
	}
	else
		++toggle2;

	if (toggle > 0)
	{
		temp = sgCurr[31];
		for (i = 31; i > 16; --i)
		{
			sgCurr[i].peRed = sgCurr[i - 1].peRed;
			sgCurr[i].peGreen = sgCurr[i - 1].peGreen;
			sgCurr[i].peBlue = sgCurr[i - 1].peBlue;
		}
		sgCurr[i].peRed = temp.peRed;
		sgCurr[i].peGreen = temp.peGreen;
		sgCurr[i].peBlue = temp.peBlue;

		SetPaletteEntries();
		++toggle;
	}
	else
		toggle = 1;
}
void TwinCycleNest() {
	int i;
	static int toggle = 0;
	static int toggle2 = 0;
	PALETTEENTRY temp;
	if (toggle2 == 2)
	{
		temp = sgCurr[8];
		for (i = 8; i > 1; --i) {
			sgCurr[i].peRed = sgCurr[i - 1].peRed;
			sgCurr[i].peGreen = sgCurr[i - 1].peGreen;
			sgCurr[i].peBlue = sgCurr[i - 1].peBlue;
			// don't copy flags
		}

		sgCurr[i].peRed = temp.peRed;
		sgCurr[i].peGreen = temp.peGreen;
		sgCurr[i].peBlue = temp.peBlue;
		// don't copy flags
		toggle2 = 0;
	}
	else
		++toggle2;

	if (toggle == 2)
	{
		temp = sgCurr[15];
		for (i = 15; i > 9; --i)
		{
			sgCurr[i].peRed = sgCurr[i - 1].peRed;
			sgCurr[i].peGreen = sgCurr[i - 1].peGreen;
			sgCurr[i].peBlue = sgCurr[i - 1].peBlue;
		}
		sgCurr[i].peRed = temp.peRed;
		sgCurr[i].peGreen = temp.peGreen;
		sgCurr[i].peBlue = temp.peBlue;

		SetPaletteEntries();
		toggle = 0;
	}
	else
		++toggle;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void UpdateLavaPalette() {
	for (int i = 0; i < 32; i++)
		sgBase[i] = sgLoad[i];
	GammaPalette(sgCurr,sgBase,32);
	SetPaletteEntries();
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void MeshLavaPalette(int fadeval) {
	int i;

	for (i = 32 - fadeval; i >= 0; i--)
		sgBase[i] = sgLoad[i];
	GammaPalette(sgCurr, sgBase, 32);
	SetPaletteEntries();
}

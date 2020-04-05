/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/PALETTE.H 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/
#define FADE_SLOW	8
#define FADE_MED	16
//#define FADE_FAST	32
#define FADE_FAST	8
#define FADE_VFAST	128



/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/
void CreatePalette();
void LoadRndLvlPal(int);
void LoadPalette(const char * pszFileName);
void ResetPal();
void SetPalette();
void PaletteFadeIn(int faderate);
void PaletteFadeOut(int faderate);
void LavaCycle();
void TwinCycleCrypt();		// JKE new cycle for dual effects
void TwinCycleNest();		// JKE new cycle for nest
void MeshLavaPalette(int fadeval);


#define lGAMMA_READ	0
#define lGAMMA_MIN	30
#define lGAMMA_MAX	100
#define lGAMMA_STEP	5
#define lGAMMA_TICKS	(((lGAMMA_MAX - lGAMMA_MIN) / lGAMMA_STEP) + 1)
void GammaUp();
void GammaDown();
LONG GammaLevel(LONG lGamma);
/*-----------------------------------------------------------------------**
** 								Diablo
**
**							  Player file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/PLAYER.CPP 11    10-03-97 13:40 Jmcreynolds $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"storm.h"
#include	"sound.h"
#include	"msg.h"
#include	"multi.h"
#include	"debug.h"
#include	"engine.h"
#include	"gendung.h"
#include	"palette.h"
#include	"items.h"
#include	"player.h"
#include	"monster.h"
#include	"missiles.h"
#include	"spells.h"
#include	"inv.h"
#include	"lighting.h"
#include	"cursor.h"
#include	"control.h"
#include	"effects.h"
#include	"objects.h"
#include	"town.h"
#include	"towners.h"
#include	"monstint.h"
#include	"monstdat.h"
#include	"gamemenu.h"
#include	"stores.h"
#include	"spelldat.h"
#include	"path.h"
#include	"quests.h"
#include	"minitext.h"
#include	"textdat.h"
#include	"portal.h"
#include	"themes.h"
#include	"objdat.h"

void SetPlrHandItem(ItemStruct *h, int idata);
void GetPlrHandSeed(ItemStruct *h);
void GetGoldSeed(int pnum, ItemStruct *h);
void SetPrlHandSeed(ItemStruct *h, int iseed);
void FixPlrWalkTags(int pnum);
void RemovePlrFromMap(int pnum);
void SetPlrHandGoldCurs(ItemStruct *h);

extern BOOL GoldAutoPlace(int pnum);
extern HSARCHIVE ghsHFBardArchive;
extern HSARCHIVE ghsHFBarbarianArchive;
/*-----------------------------------------------------------------------*
** Global Variables
**-----------------------------------------------------------------------*/
// pjw.patch1.start
// PlayerStruct plr[MAX_PLRS];
PlayerStruct * plr = NULL;
// pjw.patch1.end
int myplr;
int deathdelay;
BOOL deathflag;

/*-----------------------------------------------------------------------*
** File Variables
**-----------------------------------------------------------------------*/
// Init View offsets
int plrxoff[9] = { 0, 2, 0, 2, 1, 0, 1, 2, 1 };
int plryoff[9] = { 0, 2, 2, 0, 1, 1, 0, 1, 2 };

int plrxoff2[9] = { 0, 1, 0, 1, 2, 0, 1, 2, 2 };
int plryoff2[9] = { 0, 0, 1, 1, 0, 2, 2, 1, 2 };


/*-----------------------------------------------------------------------*/
char PlrGFXAnimLens[NUM_CLASSES][11] = {
												// Warrior
	// AS, AT, AW, BL, DT, Magic, HT, ST, WL, AFrame, SFrame
	{  10, 16,  8,  2, 20,    20,  6, 20,  8,      9,     14 },
												// Rogue
	// AS, AT, AW, BL, DT, Magic, HT, ST, WL, AFrame, SFrame
	{   8, 18,  8,  4, 20,    16,  7, 20,  8,     10,     12 },
												// Sorceror
	// AS, AT, AW, BL, DT, Magic, HT, ST, WL, AFrame, SFrame
	{   8, 16,  8,  6, 20,    12,  8, 20,  8,     12,      8 },
												// Monk
	// AS, AT, AW, BL, DT, Magic, HT, ST, WL, AFrame, SFrame
	{   8, 16,  8,  3, 20,    18,  6, 20,  8,     12,     13 },
// Fix this when we get art.
												// Bard
	// AS, AT, AW, BL, DT, Magic, HT, ST, WL, AFrame, SFrame
	{   8, 18,  8,  4, 20,    16,  7, 20,  8,     10,     12 },
												// Barbarian
	// AS, AT, AW, BL, DT, Magic, HT, ST, WL, AFrame, SFrame
	{  10, 16,  8,  2, 20,    20,  6, 20,  8,      9,     14 },
};

/*-----------------------------------------------------------------------*/

// Different walk velocities for different classes
int PlrWalkTbl[NUM_CLASSES][3] = {
	{ 2048, 1024, 512 },		// Warrior (8 frames)
	//{ 2730, 1365, 682 },		// Warrior (6 frames)
	{ 2048, 1024, 512 },		// Rogue (8 frames)
	//{ 2340, 1170, 585 },		// Rogue (7 frames)
	{ 2048, 1024, 512 }, 		// Sorceror (8 frames)
	{ 2048, 1024, 512 }, 		// Monk (8 frames)
	{ 2048, 1024, 512 },		// Bard (8 frames)
	{ 2048, 1024, 512 } };		// Barbarian (8 frames)

// Different walk lengths for different classes
//int PlrWalkLenTbl[3] = { 6, 7, 8 };
int PlrWalkLenTbl[NUM_CLASSES] = { 8, 8, 8, 8, 8, 8 };

extern DWORD gbWalkOn;		// double speed walk

/*-----------------------------------------------------------------------*/

int StrengthTbl[NUM_CLASSES] =  { 30, 20, 15, 25, 20, 40};
int MagicTbl[NUM_CLASSES] =     { 10, 15, 35, 15, 20,  0};
int DexterityTbl[NUM_CLASSES] = { 20, 30, 15, 25, 25, 20};
int VitalityTbl[NUM_CLASSES] =  { 25, 20, 20, 20, 20, 25};

//int ToHitTbl[NUM_CLASSES] = { 50, 50, 50 };
int ToBlkTbl[NUM_CLASSES] = { 30, 20, 10, 25, 25, 30};

char *ClassStrTbl[NUM_CLASSES] = { "Warrior", 
								   "Rogue", 
								   "Sorceror",
								   "Monk",
								   "Bard",
								   "Barbarian" };

/*-----------------------------------------------------------------------*/

int MaxStats[NUM_CLASSES][4] = {
	// Str, Mag, Dex, Vit.
	{ 250,  50,  60, 100 },	// warrior
	{  55,  70, 250,  80 },	// rogue
	{  45, 250,  85,  80 },	// sorceror
	{ 150,  80, 150,  80 },	// monk
	{ 120, 120, 120, 100 },	// bard
	{ 255,   0,  55, 150 },	// barbarian
	 };

/*-----------------------------------------------------------------------*/

#define MAX_EXPLVLS	50

long ExpLvlsTbl[MAX_EXPLVLS+1] = {
/*	0,				// 0		Changed 11/5 Dave
	1000,			// 1
	2400,			// 2
	4353,			// 3
	7070,			// 4
	10837,			// 5
	16044,			// 6
	23217,			// 7
	33065,			// 8
	46542,			// 9
	64925,			// 10
	89917,			// 11
	123782,			// 12
	169516,			// 13*/

	0,				// 0
	2000,			// 1
	4620,			// 2
	8040,			// 3
	12489,			// 4
	18258,			// 5
	25712,			// 6
	35309,			// 7
	47622,			// 8
	63364,			// 9
	83419,			// 10
	108879,			// 11
	141086,			// 12
	181683,			// 13
	231075,			// 14
	313656,			// 15
	424067,			// 16
	571190,			// 17
	766569,			// 18
	1025154,		// 19
	1366227,		// 10
	1814568,		// 21
	2401895,		// 22
	3168651,		// 23
	4166200,		// 24
	5459523,		// 25
	7130496,		// 26
	9281874,		// 27
	12042092,		// 28
	15571031,		// 29
	20066900,		// 30
	25774405,		// 31
	32994399,		// 32
	42095202,		// 33
	53525811,		// 34
	67831218,		// 35
	85670061,		// 36
	107834823,		// 37
	135274799,		// 38
	169122009,		// 39
	210720231,		// 40
	261657253,		// 41
	323800420,		// 42
	399335440,		// 43
	490808349,		// 44
	601170414,		// 45
	733825617,		// 46
	892680222,		// 47
	1082908612,		// 48
	1310707109,		// 49
	1583495809 };	// 50


/*-----------------------------------------------------------------------*
** player animation files
**-----------------------------------------------------------------------*/
static const char plrgfxlmh[] = "LMH";
static const char plrgfxweap[] = "NUSDBAMHT";
static const char sgcCharClass[NUM_CLASSES] = { 'W', 'R', 'S', 'M', 'B', 'C' };
static const char sgcAlterCharClass[NUM_CLASSES] = { 'W', 'R', 'S', 'M', 'R', 'W'};
static const char * const sgpszCharDir[NUM_CLASSES] = {
	"Warrior","Rogue","Sorceror","Monk","Bard","Barbarian"
};
static const char * const sgpszAlterCharDir[NUM_CLASSES] = {
	"Warrior","Rogue","Sorceror","Monk","Rogue","Warrior"
};


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void SetAnimPtrs(BYTE *pData, BYTE *pAnim[]) {
	for (int i = 0; i < 8; i++)
		pAnim[i] = pData + ((DWORD*)pData)[i];
}


/*-----------------------------------------------------------------------*
** loads player anims based on bit flags
**-----------------------------------------------------------------------*/
void LoadPlrGFX(int pnum,DWORD dwLoad) {
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("LoadPlrGFX: illegal player %d",pnum);
	// get a pointer to the player
	PlayerStruct * pPlayer = &plr[pnum];

	// calculate which player type, weapon and armor values to use
	#if IS_VERSION(SHAREWARE)
	app_assert(pPlayer->_pClass == CLASS_WARRIOR);
	#else
	app_assert((DWORD) pPlayer->_pClass <= NUM_CLASSES);
	#endif

	char szPlrType[16];
	char const * pszPlrPath = "";
	if ((pPlayer->_pClass == CLASS_BARD && ghsHFBardArchive == NULL)
		|| (pPlayer->_pClass == CLASS_BARBARIAN && ghsHFBarbarianArchive == NULL))
	{
		sprintf(
			szPlrType,
			"%c%c%c",
			sgcAlterCharClass[pPlayer->_pClass],			// player class
			plrgfxlmh[pPlayer->_pgfxnum >> PGFX_CSHIFT],	// armor class
			plrgfxweap[pPlayer->_pgfxnum & PGFX_MASK]		// weapon
		);
		pszPlrPath = sgpszAlterCharDir[pPlayer->_pClass];
	} else {
		sprintf(
			szPlrType,
			"%c%c%c",
			sgcCharClass[pPlayer->_pClass],					// player class
			plrgfxlmh[pPlayer->_pgfxnum >> PGFX_CSHIFT],	// armor class
			plrgfxweap[pPlayer->_pgfxnum & PGFX_MASK]		// weapon
		);
		pszPlrPath = sgpszCharDir[pPlayer->_pClass];
	}
	// get player directory

	// load file
	BYTE * pData;
	BYTE ** ppAnim;
	const char * pszAnim;
	for (DWORD dwBit = 1; dwBit <= PGL_ALL; dwBit <<= 1) {
		// load this graphic set?
		if (! (dwBit & dwLoad)) continue;

		switch (dwBit) {

			case PGL_STAND:
				// load dungeon/town stand
				pszAnim = leveltype ? "AS" : "ST";
				pData = pPlayer->_pNData;
				ppAnim = pPlayer->_pNAnim;
			break;

			case PGL_WALK:
				// load dungeon/town walk
				pszAnim = leveltype ? "AW" : "WL";
				pData = pPlayer->_pWData;
				ppAnim = pPlayer->_pWAnim;
			break;

			case PGL_DEAD:
				// dead anim only valid for guy holding no weapons
				if ((pPlayer->_pgfxnum & PGFX_MASK) != PGFX_NGUY) continue;
				pszAnim = "DT";
				pData = pPlayer->_pDData;
				ppAnim = pPlayer->_pDAnim;
			break;

			case PGL_ATTACK:
				if (! leveltype) continue;
				pszAnim = "AT";
				pData = pPlayer->_pAData;
				ppAnim = pPlayer->_pAAnim;
			break;

			case PGL_HIT:
				if (! leveltype) continue;
				pszAnim = "HT";
				pData = pPlayer->_pHData;
				ppAnim = pPlayer->_pHAnim;
			break;

			case PGL_LMAG:
				if (! leveltype) continue;
				pszAnim = "LM";
				pData = pPlayer->_pLData;
				ppAnim = pPlayer->_pLAnim;
			break;

			case PGL_FMAG:
				if (! leveltype) continue;
				pszAnim = "FM";
				pData = pPlayer->_pFData;
				ppAnim = pPlayer->_pFAnim;
			break;

			case PGL_TMAG:
				if (! leveltype) continue;
				pszAnim = "QM";
				pData = pPlayer->_pTData;
				ppAnim = pPlayer->_pTAnim;
			break;

			case PGL_BLOCK:
				if (! leveltype) continue;
				if (! pPlayer->_pBlockFlag) continue;
				pszAnim = "BL";
				pData = pPlayer->_pBData;
				ppAnim = pPlayer->_pBAnim;
			break;

			default:
				app_fatal("PLR:2");
			break;
		}

		char szBuf[256];
		sprintf(
			szBuf,
			#if RLE_DRAW
			"PlrGFX\\%s\\%s\\%s%s.CL2",
			#else
			"PlrGFX\\%s\\%s\\%s%s.CEL",
			#endif
			pszPlrPath,
			szPlrType,
			szPlrType,
			pszAnim
		);

		app_assert(pData);
		LoadFileWithMem(szBuf,pData);
		SetAnimPtrs(pData,ppAnim);
		pPlayer->_pGFXLoad |= dwBit;
	}
}


/*-----------------------------------------------------------------------*
** loads all player anims
**-----------------------------------------------------------------------*/
void InitPlayerGFX(int pnum) {
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("InitPlayerGFX: illegal player %d",pnum);
	if ((plr[pnum]._pHitPoints >> HP_SHIFT) == 0) {
		plr[pnum]._pgfxnum = PGFX_NGUY;
		LoadPlrGFX(pnum, PGL_DEAD);
	} else {
		LoadPlrGFX(pnum,PGL_ALL);
	}
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static DWORD calc_plr_mem(const char * pszAnim) {
	DWORD dwMax = 0;

	for (DWORD dwClass = 0; dwClass < NUM_CLASSES; dwClass++) {
		#if IS_VERSION(SHAREWARE)
		if (dwClass != CLASS_WARRIOR) continue;
		#endif

		for (const char * pszArmor = plrgfxlmh; *pszArmor; pszArmor++) {
			#if IS_VERSION(SHAREWARE)
			if (pszArmor != plrgfxlmh) break;
			#endif

			for (const char * pszWeap = plrgfxweap; *pszWeap; pszWeap++) {
				char szPlrType[16];
				char szBuf[256];
				
				if ((dwClass == CLASS_BARD && ghsHFBardArchive == NULL)
					|| (dwClass == CLASS_BARBARIAN && ghsHFBarbarianArchive == NULL))
				{
					sprintf( szPlrType, "%c%c%c",
						sgcAlterCharClass[dwClass],
						*pszArmor,
						*pszWeap );
					sprintf( szBuf,
						#if RLE_DRAW
						"PlrGFX\\%s\\%s\\%s%s.CL2",
						#else
						"PlrGFX\\%s\\%s\\%s%s.CEL",
						#endif
						sgpszAlterCharDir[dwClass],
						szPlrType,
						szPlrType,
						pszAnim
					); //whew
				} else {
					sprintf( szPlrType, "%c%c%c",
						sgcCharClass[dwClass],
						*pszArmor,
						*pszWeap );
					sprintf( szBuf,
						#if RLE_DRAW
						"PlrGFX\\%s\\%s\\%s%s.CL2",
						#else
						"PlrGFX\\%s\\%s\\%s%s.CEL",
						#endif
						sgpszCharDir[dwClass],
						szPlrType,
						szPlrType,
						pszAnim
					); //whew
				}

				HSFILE hsFile;
				if (! patSFileOpenFile(szBuf,&hsFile,TRUE))
					continue;

				app_assert(hsFile);
				DWORD dwSize = patSFileGetFileSize(hsFile);
				patSFileCloseFile(hsFile);
				dwMax = max(dwMax,dwSize);
			}
		}
	}

	return dwMax;
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

// remember to update these numbers if we ever add another set of 
// player graphics

void InitPlrGFXMem(int pnum) {
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("InitPlrGFXMem: illegal player %d",pnum);
	app_assert(! plr[pnum]._pNData);

	{
		static DWORD dwMem = (ghsHFBardArchive == NULL && ghsHFBarbarianArchive == NULL ) 
		                      ? 0x35E28 : max(calc_plr_mem("AS"),calc_plr_mem("ST"));
		plr[pnum]._pNData = DiabloAllocPtrSig(dwMem,'PGFN');
	}
	{
		static DWORD dwMem = (ghsHFBardArchive == NULL && ghsHFBarbarianArchive == NULL)
							? 0x16EB9 : max(calc_plr_mem("AW"),calc_plr_mem("WL"));
		plr[pnum]._pWData = DiabloAllocPtrSig(dwMem,'PGFW');
	}
	{
		static DWORD dwMem = (ghsHFBardArchive == NULL && ghsHFBarbarianArchive == NULL)
							? 0x40047 : calc_plr_mem("AT");
		plr[pnum]._pAData = DiabloAllocPtrSig(dwMem,'PGFA');
	}
	{
		static DWORD dwMem = (ghsHFBardArchive == NULL && ghsHFBarbarianArchive == NULL)
							? 0x16484 : calc_plr_mem("HT");
		plr[pnum]._pHData = DiabloAllocPtrSig(dwMem,'PGFH');
	}
	{
		static DWORD dwMem = (ghsHFBardArchive == NULL && ghsHFBarbarianArchive == NULL)
							? 0x4AD88 : calc_plr_mem("LM");
		plr[pnum]._pLData = DiabloAllocPtrSig(dwMem,'PGFL');
	}
	{
		static DWORD dwMem = (ghsHFBardArchive == NULL && ghsHFBarbarianArchive == NULL)
							? 0x67AED : calc_plr_mem("FM");
		plr[pnum]._pFData = DiabloAllocPtrSig(dwMem,'PGFF');
	}
	{
		static DWORD dwMem = (ghsHFBardArchive == NULL && ghsHFBarbarianArchive == NULL)
							? 0x92F55 : calc_plr_mem("QM");
		plr[pnum]._pTData = DiabloAllocPtrSig(dwMem,'PGFT');
	}
	{
		static DWORD dwMem = (ghsHFBardArchive == NULL && ghsHFBarbarianArchive == NULL)
							? 0x3A8A6 : calc_plr_mem("DT");
		plr[pnum]._pDData = DiabloAllocPtrSig(dwMem,'PGFD');
	}
	{
		static DWORD dwMem = (ghsHFBardArchive == NULL && ghsHFBarbarianArchive == NULL)
							? 0x10FE3 : calc_plr_mem("BL");
		plr[pnum]._pBData = DiabloAllocPtrSig(dwMem,'PGFB');
	}

	// no gfx loaded
	plr[pnum]._pGFXLoad = 0;
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void FreePlayerGFX(int pnum) {
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("FreePlayerGFX: illegal player %d",pnum);
	DiabloFreePtr(plr[pnum]._pNData);
	DiabloFreePtr(plr[pnum]._pWData);
	DiabloFreePtr(plr[pnum]._pAData);
	DiabloFreePtr(plr[pnum]._pHData);
	DiabloFreePtr(plr[pnum]._pLData);
	DiabloFreePtr(plr[pnum]._pFData);
	DiabloFreePtr(plr[pnum]._pTData);
	DiabloFreePtr(plr[pnum]._pDData);
	DiabloFreePtr(plr[pnum]._pBData);
	plr[pnum]._pGFXLoad = 0;
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void NewPlrAnim(int pnum, BYTE *pAnim, int numFrames, int Delay, long width)
{
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("NewPlrAnim: illegal player %d",pnum);
	plr[pnum]._pAnimData = pAnim;
	plr[pnum]._pAnimLen = numFrames;
	plr[pnum]._pAnimFrame = 1;
	plr[pnum]._pAnimCnt = 0;
	plr[pnum]._pAnimDelay = Delay;
	plr[pnum]._pAnimWidth = width;
	plr[pnum]._pAnimWidth2 = (width - 64) >> 1;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void ClearPlrPVars(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("ClearPlrPVars: illegal player %d",pnum);
	plr[pnum]._pVar1 = 0;
	plr[pnum]._pVar2 = 0;
	plr[pnum]._pVar3 = 0;
	plr[pnum]._pVar4 = 0;
	plr[pnum]._pVar5 = 0;
	plr[pnum]._pVar6 = 0;
	plr[pnum]._pVar7 = 0;
	plr[pnum]._pVar8 = 0;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void SetPlrAnims(int pnum)
{
	int		gn,pc;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("SetPlrAnims: illegal player %d",pnum);
	plr[pnum]._pNWidth = 96;
	plr[pnum]._pWWidth = 96;
	plr[pnum]._pAWidth = 128;
	plr[pnum]._pHWidth = 96;
	plr[pnum]._pSWidth = 96;
	plr[pnum]._pDWidth = 128;
	plr[pnum]._pBWidth = 96;

	pc = plr[pnum]._pClass;
	if (leveltype == 0) {
		plr[pnum]._pNFrames = PlrGFXAnimLens[pc][7];
		plr[pnum]._pWFrames = PlrGFXAnimLens[pc][8];
		plr[pnum]._pDFrames = PlrGFXAnimLens[pc][4];
		plr[pnum]._pSFrames = PlrGFXAnimLens[pc][5];
		plr[pnum]._pSFNum = PlrGFXAnimLens[pc][10];
	}
	else {
		plr[pnum]._pNFrames = PlrGFXAnimLens[pc][0];
		plr[pnum]._pWFrames = PlrGFXAnimLens[pc][2];
		plr[pnum]._pAFrames = PlrGFXAnimLens[pc][1];
		plr[pnum]._pHFrames = PlrGFXAnimLens[pc][6];
		plr[pnum]._pSFrames = PlrGFXAnimLens[pc][5];
		plr[pnum]._pDFrames = PlrGFXAnimLens[pc][4];
		plr[pnum]._pBFrames = PlrGFXAnimLens[pc][3];
		plr[pnum]._pAFNum = PlrGFXAnimLens[pc][9];
		plr[pnum]._pSFNum = PlrGFXAnimLens[pc][10];
	}

	gn = plr[pnum]._pgfxnum & PGFX_MASK;
	if (pc == CLASS_WARRIOR) {
		if (gn == PGFX_BGUY) {		// Bow
			if (leveltype != 0) plr[pnum]._pNFrames = 8;
			plr[pnum]._pAWidth = 96;
			plr[pnum]._pAFNum = 11;
		}
		else if (gn == PGFX_FGUY) {		// Axe
			plr[pnum]._pAFrames = 20;
			plr[pnum]._pAFNum = 10;
		}
		else if (gn == PGFX_TGUY) {		// Staff
			plr[pnum]._pAFrames = 16;
			plr[pnum]._pAFNum = 11;
		}
	}
#if !IS_VERSION(SHAREWARE)
	else if (pc == CLASS_ROGUE) {
		if (gn == PGFX_FGUY) {			// Axe
			plr[pnum]._pAFrames = 22;
			plr[pnum]._pAFNum = 13;
		}
		else if (gn == PGFX_BGUY) {		// Bow
			plr[pnum]._pAFrames = 12;
			plr[pnum]._pAFNum = 7;
		}
		else if (gn == PGFX_TGUY) {		// Staff
			plr[pnum]._pAFrames = 16;
			plr[pnum]._pAFNum = 11;
		}
	}
	else if (pc == CLASS_SORCEROR) {
		plr[pnum]._pSWidth = 128;
		
		if (gn == PGFX_NGUY) {			// No weapon
			plr[pnum]._pAFrames = 20;
		}
		else if (gn == PGFX_SGUY) {		// Shield only
			plr[pnum]._pAFNum = 9;
		}
		else if (gn == PGFX_BGUY) {		// Bow
			plr[pnum]._pAFrames = 20;
			plr[pnum]._pAFNum = 16;
		}
		else if (gn == PGFX_FGUY) {		// Axe
			plr[pnum]._pAFrames = 24;
			plr[pnum]._pAFNum = 16;
		}
	}
	
	else if (pc == CLASS_MONK)
	{
		plr[pnum]._pNWidth = 112;
		plr[pnum]._pWWidth = 112;
		plr[pnum]._pAWidth = 130;
		plr[pnum]._pHWidth = 98;
		plr[pnum]._pSWidth = 114;
		plr[pnum]._pDWidth = 160;
		plr[pnum]._pBWidth = 98;
		
		switch(gn)
		{
			case PGFX_NGUY:
			case PGFX_SGUY:					// may want to slow this down
				plr[pnum]._pAFrames = 12;
				plr[pnum]._pAFNum = 7;
				break;
			case PGFX_BGUY:
				plr[pnum]._pAFrames = 20;
				plr[pnum]._pAFNum = 14;
				break;
			case PGFX_FGUY:
				plr[pnum]._pAFrames = 23;
				plr[pnum]._pAFNum = 14;
				break;
			case PGFX_TGUY:
				plr[pnum]._pAFrames = 13;
				plr[pnum]._pAFNum = 8;
				break;
			default:
				break;
		}
	}
	
	//
	//
	// Fix this when the art arrives.
	//
	//
	else if (pc == CLASS_BARD)
	{
		if (gn == PGFX_FGUY) {			// Axe
			plr[pnum]._pAFrames = 22;
			plr[pnum]._pAFNum = 13;
		}
		else if (gn == PGFX_BGUY) {		// Bow
			plr[pnum]._pAFrames = 12;
			plr[pnum]._pAFNum = 11;
		}
		else if (gn == PGFX_TGUY) {		// Staff
			plr[pnum]._pAFrames = 16;
			plr[pnum]._pAFNum = 11;
		}
		else if (gn == PGFX_GUY) {		// Sword and shield
			plr[pnum]._pAFNum = 10;
		}
		else if (gn == PGFX_XGUY) {		// Sword
			plr[pnum]._pAFNum = 10;
		}
	}
	else if (pc == CLASS_BARBARIAN)
	{
		if (gn == PGFX_FGUY) {			// Axe
			plr[pnum]._pAFrames = 20;
			plr[pnum]._pAFNum = 8;
		}
		else if (gn == PGFX_BGUY) {		// Bow
			if (leveltype != 0) plr[pnum]._pNFrames = 8;
			plr[pnum]._pAWidth = 96;
			plr[pnum]._pAFNum = 11;
		}
		else if (gn == PGFX_TGUY) {		// Staff
			plr[pnum]._pAFrames = 16;
			plr[pnum]._pAFNum = 11;
		}
		else if (gn == PGFX_ZGUY) {		// Mace
			plr[pnum]._pAFNum = 8;
		}
		else if (gn == PGFX_CGUY) {		// Mace with Shield
			plr[pnum]._pAFNum = 8;
		}
	}
#endif
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void PlrInitReserved(PlayerStruct * p) {
	app_assert(p != NULL);
// REMEMBER -- every time you remove a variable from this
// list to rename it, it MUST be initialized in CreatePlayer()
// or you will screw up the character with unitialized data!!!!
// ALSO -- you MUST initialize the variable in UnPackPlayer()
// in packplr.cpp!!!
	p->bReserved5 = 0;
	p->bReserved6 = 0;
	p->bReserved7 = 0;
	p->bReserved8 = 0;

	p->wReserved2 = 0;
	p->wReserved3 = 0;
	p->wReserved4 = 0;
	p->wReserved5 = 0;
	p->wReserved6 = 0;
	p->wReserved7 = 0;
	p->wReserved8 = 0;

	p->dwReserved4 = 0;
	p->dwReserved5 = 0;
	p->dwReserved6 = 0;
	p->dwReserved7 = 0;
	p->dwReserved8 = 0;
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void CreatePlayer(int pnum, char c) {
	
	// This isn't good C++ but it beats the lack of initialization currently done.
	memset(&plr[pnum], 0, sizeof(PlayerStruct));
	PlrInitReserved(&plr[pnum]);

	int i;
	char	vc;

// dig.patch1.start.2/4/97
	// time() returns -1 if clock is set far ahead, e.g., the year 2097.
	// Many customers have broken clock, resulting in all games being the same.

//	SetRndSeed((int) time(NULL));
	SetRndSeed((int) GetTickCount());
// dig.patch1.end.2/4/97

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("CreatePlayer: illegal player %d",pnum);
	plr[pnum]._pClass = c;

	vc = StrengthTbl[c];
	if (vc < 0) vc = 0;
	plr[pnum]._pStrength = vc;
	plr[pnum]._pBaseStr = vc;
	vc = MagicTbl[c];
	if (vc < 0) vc = 0;
	plr[pnum]._pMagic = vc;
	plr[pnum]._pBaseMag = vc;
	vc = DexterityTbl[c];
	if (vc < 0) vc = 0;
	plr[pnum]._pDexterity = vc;
	plr[pnum]._pBaseDex = vc;
	vc = VitalityTbl[c];
	if (vc < 0) vc = 0;
	plr[pnum]._pVitality = vc;
	plr[pnum]._pBaseVit = vc;

	plr[pnum]._pStatPts = 0;

	plr[pnum].pTownWarps = 0;
	plr[pnum].pDungMsgs = 0;
	plr[pnum].pHellfireMsgs = 0;
	plr[pnum].pLvlLoad = 0;
	plr[pnum].pDiabloKillLevel = 0;
	plr[pnum]._gnDifficulty = D_NORMAL;


	if (plr[pnum]._pClass == CLASS_MONK)
		plr[pnum]._pDamageMod = ((plr[pnum]._pStrength + plr[pnum]._pDexterity) * plr[pnum]._pLevel) / 150;
	else if (plr[pnum]._pClass == CLASS_ROGUE ||
	    plr[pnum]._pClass == CLASS_BARD) 
		plr[pnum]._pDamageMod = ((plr[pnum]._pStrength + plr[pnum]._pDexterity) * plr[pnum]._pLevel) / 200;
	else // Warrior and Barbarian
		plr[pnum]._pDamageMod = (plr[pnum]._pStrength * plr[pnum]._pLevel) / 100;
	plr[pnum]._pBaseToBlk = ToBlkTbl[c];

	plr[pnum]._pHitPoints = (plr[pnum]._pVitality + 10) << HP_SHIFT;
	if (plr[pnum]._pClass == CLASS_WARRIOR
		|| plr[pnum]._pClass == CLASS_BARBARIAN ) plr[pnum]._pHitPoints = plr[pnum]._pHitPoints << 1;
	else if (plr[pnum]._pClass == CLASS_ROGUE ||
	         plr[pnum]._pClass == CLASS_MONK  ||
	         plr[pnum]._pClass == CLASS_BARD) plr[pnum]._pHitPoints += (plr[pnum]._pHitPoints >> 1);
	plr[pnum]._pMaxHP = plr[pnum]._pHitPoints;
	plr[pnum]._pHPBase = plr[pnum]._pHitPoints;
	plr[pnum]._pMaxHPBase = plr[pnum]._pHitPoints;

	plr[pnum]._pMana = plr[pnum]._pMagic << MANA_SHIFT;
	if (plr[pnum]._pClass == CLASS_SORCEROR) plr[pnum]._pMana = plr[pnum]._pMana << 1; // 2x
	else if(	plr[pnum]._pClass == CLASS_BARD) plr[pnum]._pMana += (plr[pnum]._pMana * 3)/4; // 1.75
	else if (plr[pnum]._pClass == CLASS_ROGUE ||
		plr[pnum]._pClass == CLASS_MONK ) plr[pnum]._pMana += (plr[pnum]._pMana >> 1); // 1.5x
	plr[pnum]._pMaxMana = plr[pnum]._pMana;
	plr[pnum]._pManaBase = plr[pnum]._pMana;
	plr[pnum]._pMaxManaBase = plr[pnum]._pMana;

	plr[pnum]._pLevel = 1;
	plr[pnum]._pMaxLvl = plr[pnum]._pLevel;

	plr[pnum]._pExperience = 0;
	plr[pnum]._pMaxExp = plr[pnum]._pExperience;
	plr[pnum]._pNextExper = ExpLvlsTbl[1];

	plr[pnum]._pArmorClass = 0;
	
	if (plr[pnum]._pClass == CLASS_BARBARIAN) {
		plr[pnum]._pMagResist = 1;
		plr[pnum]._pFireResist = 1;
		plr[pnum]._pLghtResist = 1;
	}
	else {
		plr[pnum]._pMagResist = 0;
		plr[pnum]._pFireResist = 0;
		plr[pnum]._pLghtResist = 0;
	}

	plr[pnum]._pLightRad = PLRLRAD;

	plr[pnum]._pInfraFlag = FALSE;

	if (c == CLASS_WARRIOR) plr[pnum]._pAblSpells = (_int64)(1) << (SPL_REPAIR-1);
	#if !IS_VERSION(SHAREWARE)
	else if (c == CLASS_ROGUE) plr[pnum]._pAblSpells = (_int64)(1) << (SPL_DISARM-1);
	else if (c == CLASS_SORCEROR) plr[pnum]._pAblSpells = (_int64)(1) << (SPL_RECHARGE-1);
	else if (c == CLASS_MONK) plr[pnum]._pAblSpells = (__int64)(1) << (SPL_SHOWMAGITEMS-1);
	else if (c == CLASS_BARD) plr[pnum]._pAblSpells = (_int64)(1) << (SPL_IDENTIFY-1);
	else if (c == CLASS_BARBARIAN) plr[pnum]._pAblSpells = (_int64)(1) << (SPL_RAGE-1);
	#endif

	if (c == CLASS_SORCEROR) plr[pnum]._pMemSpells = (_int64)(1) << (SPL_FIREBOLT-1);
	else plr[pnum]._pMemSpells = 0;

	for (i = 0; i < 64; i++)
		plr[pnum]._pSplLvl[i] = 0;
	plr[pnum]._pSpellFlags = 0;
	if (plr[pnum]._pClass == CLASS_SORCEROR)
		plr[pnum]._pSplLvl[SPL_FIREBOLT] = 2;

	for (i = 0; i < 3; i++) plr[pnum]._pSplHotKey[i] = -1;

	if (c == CLASS_WARRIOR) plr[pnum]._pgfxnum = PGFX_GUY;
	#if !IS_VERSION(SHAREWARE)
	else if (c == CLASS_ROGUE) plr[pnum]._pgfxnum = PGFX_BGUY;
	else if (c == CLASS_SORCEROR) plr[pnum]._pgfxnum = PGFX_TGUY;
	else if (c == CLASS_MONK) plr[pnum]._pgfxnum = PGFX_TGUY;
	else if (c == CLASS_BARD) plr[pnum]._pgfxnum = PGFX_GUY;
	else if (c == CLASS_BARBARIAN) plr[pnum]._pgfxnum = PGFX_GUY;
	#endif

	for (i = 0; i < NUMLEVELS; i++)
		plr[pnum]._pLvlVisited[i] = FALSE;
	for (i = 0; i < NUMSLEVELS; i++)
		plr[pnum]._pSLvlVisited[i] = FALSE;

	plr[pnum]._pLvlChanging = FALSE;
	plr[pnum].pTownWarps = 0;
	plr[pnum].pLvlLoad = 0;
	plr[pnum]._pIFlags2 = 0;
	
	plr[pnum]._pReflectCount = 0;

	InitDungMsgs(pnum);

	CreatePlrItems(pnum);

	SetRndSeed(0);
}

// drb.patch1.start.2/05/97
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
int CalcStatDiff(int pnum)
{
	int c = plr[pnum]._pClass;
	int d = MaxStats[c][0] - plr[pnum]._pBaseStr;
	d += MaxStats[c][1] - plr[pnum]._pBaseMag;
	d += MaxStats[c][2] - plr[pnum]._pBaseDex;
	d += MaxStats[c][3] - plr[pnum]._pBaseVit;
	return(d);
}
// drb.patch1.end.2/05/97

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void NextPlrLevel(int pnum) {
	long l;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("NextPlrLevel: illegal player %d",pnum);
	plr[pnum]._pLevel++;
	plr[pnum]._pMaxLvl++;
	CalcPlrInv(pnum,TRUE);		// added by donald 8/26

	// drb.patch1.start.2/05/97
	// plr[pnum]._pStatPts += 5;
	if (CalcStatDiff(pnum) < 5) plr[pnum]._pStatPts = CalcStatDiff(pnum);
	else plr[pnum]._pStatPts += 5;
	// drb.patch1.end.2/05/97

	plr[pnum]._pNextExper = ExpLvlsTbl[plr[pnum]._pLevel];

	if (plr[pnum]._pClass == CLASS_SORCEROR)
	{
		 l = 1 << HP_SHIFT;
	}
	else
	{
		l = 2 << HP_SHIFT;
	}
	
	if (gbMaxPlayers == 1) l++;
	plr[pnum]._pMaxHP += l;
	plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
	plr[pnum]._pMaxHPBase += l;
	plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;
	if (pnum == myplr) drawhpflag = TRUE;

	if (plr[pnum]._pClass == CLASS_WARRIOR)
	{
		l = 1 << MANA_SHIFT;
	}
	else if ( plr[pnum]._pClass == CLASS_BARBARIAN)
	{
		l = 0;
	}
	else
	{
		l = 2 << MANA_SHIFT;
	}
	if (gbMaxPlayers == 1) l++;
	plr[pnum]._pMaxMana += l;
	plr[pnum]._pMaxManaBase += l;

	if (!(plr[pnum]._pIFlags & IAF_LMANA )) { 
		plr[pnum]._pMana = plr[pnum]._pMaxMana;	
		plr[pnum]._pManaBase = plr[pnum]._pMaxManaBase;
	}
	if (pnum == myplr 
		&& plr[pnum]._pMana > 0) drawmanaflag = TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void AddPlrExperience(int pnum, int lvl, long exp)
{
	if (pnum != myplr) return;
	if ((DWORD)myplr >= MAX_PLRS)
		app_fatal("AddPlrExperience: illegal player %d",myplr);
	if (plr[myplr]._pHitPoints <= 0) return;

	double v = (1 + (((double)lvl - (double)plr[pnum]._pLevel) * 0.1)) * (double)exp;
	long e = long(v);
	if (e < 0) e = 0;

	// put limits on how fast you go up levels
	if (gbMaxPlayers > 1) {
		long lLevel = max(0,plr[pnum]._pLevel);
		lLevel = min(lLevel,MAX_EXPLVLS);
		long lMax = ExpLvlsTbl[lLevel] / 20;
		e = min(e,lMax);
		lMax = lLevel * 200;
		e = min(e,lMax);
	}

	// upgrade player
	plr[pnum]._pExperience += e;
	if ((DWORD) plr[pnum]._pExperience > 2000000000)
		plr[pnum]._pExperience = 2000000000;
	if (plr[pnum]._pExperience >= ExpLvlsTbl[MAX_EXPLVLS-1]) {
		plr[pnum]._pLevel = MAX_EXPLVLS;
	}
	else {
		for (int l = 0; plr[pnum]._pExperience >= ExpLvlsTbl[l]; l++);
		if (l != plr[pnum]._pLevel) {
			l -= plr[pnum]._pLevel;
			for (int i = 0; i < l; i++)
				NextPlrLevel(pnum);
		}
		NetSendCmdParam1(FALSE, CMD_PLRLEVEL, plr[myplr]._pLevel);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void AddPlrMonstExper(int lvl, long exp, char pmask)
{
	int totplrs = 0;
	for (int i = 0; i < MAX_PLRS; i++)
		if (pmask & (1 << i)) totplrs++;

	if (totplrs) {
		long myexp = exp / totplrs;
		if (pmask & (1 << myplr)) AddPlrExperience(myplr, lvl, myexp);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void InitPlayer(int pnum, BOOL FirstTime)
{

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("InitPlayer: illegal player %d",pnum);
	PlrInitReserved(&plr[pnum]);

	if (FirstTime) {
		/*if (plr[pnum]._pClass == CLASS_WARRIOR) {
			plr[pnum]._pRSpell = SPL_REPAIR;
			plr[pnum]._pRSplType = SPT_ABILITY;
		}
		#if !IS_VERSION(SHAREWARE)
		else if (plr[pnum]._pClass == CLASS_ROGUE) {
			plr[pnum]._pRSpell = SPL_DISARM;
			plr[pnum]._pRSplType = SPT_ABILITY;
		}
		else if (plr[pnum]._pClass == CLASS_SORCEROR) {
			plr[pnum]._pRSpell = SPL_FIREBOLT;
			plr[pnum]._pRSplType = SPT_MEMORIZED;
		}
		else if (plr[pnum]._pClass == CLASS_MONK) {
			plr[pnum]._pRSpell = SPL_SHOWMAGITEMS;
			plr[pnum]._pRSplType = SPT_ABILITY;
		}
		else if (plr[pnum]._pClass == CLASS_BARD) {
			plr[pnum]._pRSpell = SPL_IDENTIFY;
			plr[pnum]._pRSplType = SPT_ABILITY;
		}
		else if (plr[pnum]._pClass == CLASS_BARBARIAN) {
			plr[pnum]._pRSpell = SPL_RAGE;
			plr[pnum]._pRSplType = SPT_ABILITY;
		}
		#endif
		*/

		plr[pnum]._pRSplType = SPT_NONE;
		plr[pnum]._pRSpell = -1;

		plr[pnum]._pSBkSpell = -1;
		plr[pnum]._pSpell = plr[pnum]._pRSpell;
		plr[pnum]._pSplType = plr[pnum]._pRSplType;

		if ((plr[pnum]._pgfxnum & PGFX_MASK) == PGFX_BGUY)
			plr[pnum]._pwtype = WEAP_RANGE;
		else
			plr[pnum]._pwtype = WEAP_H2H;
	}

	if ((plr[pnum].plrlevel == currlevel) || (leveldebug)) {
		SetPlrAnims(pnum);
		plr[pnum]._pxoff = 0;
		plr[pnum]._pyoff = 0;
		plr[pnum]._pxvel = 0;
		plr[pnum]._pyvel = 0;
		ClearPlrPVars(pnum);

		if ((plr[pnum]._pHitPoints >> HP_SHIFT) > 0) {
			plr[pnum]._pmode = PM_STAND;
			NewPlrAnim(pnum, plr[pnum]._pNAnim[DIR_D], plr[pnum]._pNFrames, 3, plr[pnum]._pNWidth);
			plr[pnum]._pAnimFrame = random(2, plr[pnum]._pNFrames - 1) + 1;
			plr[pnum]._pAnimCnt = random(2, 3);
		} else {
			plr[pnum]._pmode = PM_DEATH;
			NewPlrAnim(pnum, plr[pnum]._pDAnim[DIR_D], plr[pnum]._pDFrames, 1, plr[pnum]._pDWidth);
			plr[pnum]._pAnimFrame = plr[pnum]._pAnimLen - 1;
			plr[pnum]._pVar8 = plr[pnum]._pAnimLen << 1;
		}

		plr[pnum]._pdir = DIR_D;
		plr[pnum]._peflag = 0;

		if (pnum == myplr) {
			// put me in my optimal space
			if ((!FirstTime) || (currlevel != 0)) {
				plr[pnum]._px = ViewX;
				plr[pnum]._py = ViewY;
			}
			plr[pnum]._ptargx = plr[pnum]._px;
			plr[pnum]._ptargy = plr[pnum]._py;
		}
		else {
			// Make other players target position where he is now
			plr[pnum]._ptargx = plr[pnum]._px;
			plr[pnum]._ptargy = plr[pnum]._py;

			// Find space for player
			for (int i = 0; i < sizeof(plrxoff2)/sizeof(plrxoff2[0]) - 1; i++) {
				if (PosOkPlayer(
					pnum,
					plr[pnum]._px + plrxoff2[i],
					plr[pnum]._py + plryoff2[i]
				)) break;
			}
			plr[pnum]._px += plrxoff2[i];
			plr[pnum]._py += plryoff2[i];
		}

		plr[pnum]._pfutx = plr[pnum]._px;
		plr[pnum]._pfuty = plr[pnum]._py;
		plr[pnum].walkpath[0] = PCMD_NOTHING;		// No buffered walking etc
		plr[pnum].destAction = PCMD_NOTHING;

		if (pnum == myplr)
			plr[pnum]._plid = AddLight(plr[pnum]._px, plr[pnum]._py, plr[pnum]._pLightRad);
		else
			plr[pnum]._plid = -1;
		plr[pnum]._pvid = AddVision(plr[pnum]._px, plr[pnum]._py,
									plr[pnum]._pLightRad, pnum==myplr);
	}

	if (plr[pnum]._pClass == CLASS_WARRIOR)
		plr[pnum]._pAblSpells = (_int64)(1) << (SPL_REPAIR-1);
	#if !IS_VERSION(SHAREWARE)
	else if (plr[pnum]._pClass == CLASS_ROGUE)
		plr[pnum]._pAblSpells = (_int64)(1) << (SPL_DISARM-1);
	else if (plr[pnum]._pClass == CLASS_SORCEROR)
		plr[pnum]._pAblSpells = (_int64)(1) << (SPL_RECHARGE-1);
	else if (plr[pnum]._pClass == CLASS_MONK)
		plr[pnum]._pAblSpells = (__int64)(1) << (SPL_SHOWMAGITEMS-1);
	else if (plr[pnum]._pClass == CLASS_BARD)
		plr[pnum]._pAblSpells = (_int64)(1) << (SPL_IDENTIFY-1);
	else if (plr[pnum]._pClass == CLASS_BARBARIAN)
		plr[pnum]._pAblSpells = (_int64)(1) << (SPL_RAGE-1);
	#endif

#if CHEATS
	if (simplecheat && FirstTime) {
		plr[pnum]._pMemSpells |= (_int64)(1) << SPL_TELE;
		if (! plr[myplr]._pSplLvl[SPL_TELE])
			plr[myplr]._pSplLvl[SPL_TELE] = 1;
	}
	if (cheatflag && FirstTime) {
		//splhold = plr[pnum]._pMemSpells;
		plr[pnum]._pMemSpells = 0x0fffffffffffffff;
		//plr[pnum]._pMemSpells = 0x0000000000000000 | (((__int64)1) << (SPL_NOVA-1));
		//plr[pnum]._pMemSpells = plr[pnum]._pMemSpells | (((__int64)1) << (SPL_TELE-1));
	}
#endif

	plr[pnum]._pNextExper = ExpLvlsTbl[plr[pnum]._pLevel];
	plr[pnum]._pInvincible = FALSE;

	if (pnum == myplr) {
		deathdelay = 0;
		deathflag = FALSE;
		ScrollInfo._sxoff = 0;
		ScrollInfo._syoff = 0;
		ScrollInfo._sdir = SCRL_NONE;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void InitMultiView()
{
	if ((DWORD)myplr >= MAX_PLRS)
		app_fatal("InitPlayer: illegal player %d",myplr);
	ViewX = plr[myplr]._px;
	ViewY = plr[myplr]._py;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void CheckEFlag(int pnum, int flag2)
{
	int	tx,ty,tv;
	int t;
	WORD *mt;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("InitPlayer: illegal player %d",pnum);
	tx = plr[pnum]._px - 1;
	ty = plr[pnum]._py + 1;
	tv = 0;
	mt = &dMT2[CalcRot(tx,ty)].mt[0];
	for(t = 2; t < 10; t++)
		tv |= mt[t];
	tv |= dSpecial[tx][ty];
	tv |= nSolidTable[dPiece[tx][ty]];
	if (tv != 0) plr[pnum]._peflag = 1;
	else plr[pnum]._peflag = 0;

	if ((flag2 == E_DOUBLE) && (plr[pnum]._peflag == 1)) {
		tx = plr[pnum]._px;
		ty = plr[pnum]._py + 2;
		tv = 0;
		mt = &dMT2[CalcRot(tx,ty)].mt[0];
		for(t = 2; t < 10; t++)
			tv |= mt[t];
		tv |= dSpecial[tx][ty];
		if (tv == 0) {
			tx = plr[pnum]._px - 2;
			ty = plr[pnum]._py + 1;
			tv = 0;
			mt = &dMT2[CalcRot(tx,ty)].mt[0];
			for(t = 2; t < 10; t++)
				tv |= mt[t];
			tv |= dSpecial[tx][ty];
			if (tv != 0) plr[pnum]._peflag = 2;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
int PlrGetDir(int pnum, int i)
{
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("PlrGetDir: illegal player %d",pnum);
	return GetDirection(plr[pnum]._px, plr[pnum]._py, monster[i]._mx, monster[i]._my);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int PlrGetDirXY(int pnum, int xx, int yy)
{
	int d;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("PlrGetDirXY: illegal player %d",pnum);
	d = GetDirection(plr[pnum]._px, plr[pnum]._py, xx, yy);
	if ((plr[pnum]._px == xx) && (plr[pnum]._py == yy)) d = plr[pnum]._pdir;
	return (d);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
BOOL SolidLoc(int x, int y) {
	return nSolidTable[dPiece[x][y]];
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
BOOL PlrDirOK(int i, int pdir) {
	if ((DWORD)i >= MAX_PLRS)
		app_fatal("PlrDirOK: illegal player %d",i);
	long fx = plr[i]._px + offset_x[pdir];
	long fy = plr[i]._py + offset_y[pdir];
	if (fx < 0) return FALSE;		// SKing fix Dave 4/3
	if (dPiece[fx][fy] == 0) return FALSE;
	if (! PosOkPlayer(i, fx, fy)) return FALSE;

	BOOL ret = TRUE;
	if(ret && pdir == DIR_R) ret = !SolidLoc(fx, fy+1) && !(dFlags[fx][fy+1] & BFLAG_PLRLR);
	if(ret && pdir == DIR_L) ret = !SolidLoc(fx+1, fy) && !(dFlags[fx+1][fy] & BFLAG_PLRLR);
//	if(ret && pdir == DIR_U) ret = !SolidLoc(fx+1, fy) && !SolidLoc(fx, fy+1);
//	if(ret && pdir == DIR_D) ret = !SolidLoc(fx-1, fy) && !SolidLoc(fx, fy-1);
	return ret;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void PlrClrTrans(int x, int y)
{
	int i,j;
	char v;
	
	for (j = y-1; j <= y+1; j++) {
		for (i = x-1; i <= x+1; i++) {
			v = dTransVal[i][j];
			TransList[v] = FALSE;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void PlrDoTrans(int x, int y)
{
	int i,j;
	char v;

	if ((leveltype == 1) || (leveltype == 2)) {
		for (j = y-1; j <= y+1; j++) {
			for (i = x-1; i <= x+1; i++) {
				if (!nSolidTable[dPiece[i][j]]) {
					v = dTransVal[i][j];
					if (v != 0) TransList[v] = TRUE;
				}
			}
		}
	} else TransList[1] = TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void SetPlayerOld(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("SetPlayerOld: illegal player %d",pnum);
	plr[pnum]._poldx = plr[pnum]._px;
	plr[pnum]._poldy = plr[pnum]._py;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void FixPlayerLocation(int pnum,int bDir)
{
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("FixPlayerLocation: illegal player %d",pnum);
	plr[pnum]._pfutx = plr[pnum]._px;
	plr[pnum]._pfuty = plr[pnum]._py;

	plr[pnum]._ptargx = plr[pnum]._px;
	plr[pnum]._ptargy = plr[pnum]._py;

	plr[pnum]._pxoff = 0;
	plr[pnum]._pyoff = 0;
	CheckEFlag(pnum, E_SINGLE);
	plr[pnum]._pdir = bDir;

	if (pnum == myplr) {
		ScrollInfo._sxoff = 0;
		ScrollInfo._syoff = 0;
		ScrollInfo._sdir = SCRL_NONE;
		ViewX = plr[pnum]._px;
		ViewY = plr[pnum]._py;
	}
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void StartStand(int pnum, int dir)
{
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("StartStand: illegal player %d",pnum);
	if ((plr[pnum]._pInvincible) && (plr[pnum]._pHitPoints == 0) && (pnum == myplr)) {
		StartPlrKill(pnum, KILL_UNKNOWN);
		return;
	}

	if ((plr[pnum]._pGFXLoad & PGL_STAND) == 0) LoadPlrGFX(pnum, PGL_STAND);
	NewPlrAnim(pnum, plr[pnum]._pNAnim[dir], plr[pnum]._pNFrames, 3, plr[pnum]._pNWidth);
	plr[pnum]._pmode = PM_STAND;
	FixPlayerLocation(pnum,dir);
	FixPlrWalkTags(pnum);
	dPlayer[plr[pnum]._px][plr[pnum]._py] = 1 + (char)pnum;
	SetPlayerOld(pnum);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void StartWalkStand(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("StartWalkStand: illegal player %d",pnum);
	plr[pnum]._pmode = PM_STAND;
	plr[pnum]._pfutx = plr[pnum]._px;
	plr[pnum]._pfuty = plr[pnum]._py;
	plr[pnum]._pxoff = 0;
	plr[pnum]._pyoff = 0;
	CheckEFlag(pnum, E_SINGLE);
	if (pnum == myplr) {
		ScrollInfo._sxoff = 0;
		ScrollInfo._syoff = 0;
		ScrollInfo._sdir = SCRL_NONE;
		ViewX = plr[pnum]._px;
		ViewY = plr[pnum]._py;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void PM_ChangeLightOff(int pnum)
{
	int lx,ly;
	int signx,signy;
	static BYTE fix[] = {0,0,3,3,3,6,6,6,8};
	int totx, toty;
	int oldtotx, oldtoty;
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("PM_ChangeLightOff: illegal player %d",pnum);
	LightListStruct * light = &LightList[plr[pnum]._plid];

	// Convert from screen pixel offsets to dungeon coordinates
	// i.e., 45 degree rotation
	lx = (plr[pnum]._pxoff + (plr[pnum]._pyoff << 1));
	ly = ((plr[pnum]._pyoff << 1) - plr[pnum]._pxoff);

	// Divide these values by 8, because lighting offsets have
	// 8 subdivisions per tile.
	if (lx < 0)
	{
		signx = -1;
		lx = -lx;
	}
	else
		signx = 1;

	if (ly < 0)
	{
		signy = -1;
		ly = -ly;
	}
	else
		signy = 1;

	lx = lx >> 3;
	ly = ly >> 3;

	// Then, fix the offset, because
	// the player lighting looks bad with full lighting resolution.
//	lx = fix[lx];
//	ly = fix[ly];

	lx *= signx;
	ly *= signy;

	totx = (light->_lx << 3) + lx;
	toty = (light->_ly << 3) + ly;

	oldtotx = (light->_lx << 3) + light->_xoff;
	oldtoty = (light->_ly << 3) + light->_yoff;

	if(abs(totx-oldtotx) >= 3 || abs(toty-oldtoty) >= 3)
		ChangeLightOff(plr[pnum]._plid, lx, ly);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void PM_ChangeOffset(int pnum)
{
	long xo, yo;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("PM_ChangeOffset: illegal player %d",pnum);
	plr[pnum]._pVar8++;

	xo = plr[pnum]._pVar6 >> 8;
	yo = plr[pnum]._pVar7 >> 8;
#if 0
	plr[pnum]._pVar6 += plr[pnum]._pxvel;
	plr[pnum]._pVar7 += plr[pnum]._pyvel;
#endif

	plr[pnum]._pVar6 += plr[pnum]._pxvel;
	plr[pnum]._pVar7 += plr[pnum]._pyvel;
	
	if (currlevel == 0 && gbWalkOn)				// double-speed walk
	{
		plr[pnum]._pVar6 += plr[pnum]._pxvel;
		plr[pnum]._pVar7 += plr[pnum]._pyvel;
	}
	
	plr[pnum]._pxoff = plr[pnum]._pVar6 >> 8;
	plr[pnum]._pyoff = plr[pnum]._pVar7 >> 8;
	xo -= (plr[pnum]._pVar6 >> 8);
	yo -= (plr[pnum]._pVar7 >> 8);
	if ((pnum == myplr) && (ScrollInfo._sdir != SCRL_NONE)) {
		ScrollInfo._sxoff += xo;
		ScrollInfo._syoff += yo;
	}
	PM_ChangeLightOff(pnum);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void StartWalk(int pnum, int xvel, int yvel, int xadd, int yadd, int EndDir, int scrldir)
{
	long	fx,fy;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("StartWalk: illegal player %d",pnum);
	if ((plr[pnum]._pInvincible) && (plr[pnum]._pHitPoints == 0) && (pnum == myplr)) {
		StartPlrKill(pnum, KILL_UNKNOWN);
		return;
	}

	SetPlayerOld(pnum);
	fx = plr[pnum]._px + xadd;
	fy = plr[pnum]._py + yadd;
	if (PlrDirOK(pnum, EndDir)) {
		//PlrClrTrans(plr[pnum]._px, plr[pnum]._py);
		//PlrDoTrans(fx, fy);
		plr[pnum]._pfutx = fx;
		plr[pnum]._pfuty = fy;
		if (pnum == myplr) {
			ScrollInfo._sdx = plr[pnum]._px - ViewX;
			ScrollInfo._sdy = plr[pnum]._py - ViewY;
		}
		dPlayer[fx][fy] = -1 - (char)pnum;
		plr[pnum]._pmode = PM_WALK;
		plr[pnum]._pxvel = xvel;
		plr[pnum]._pyvel = yvel;
		plr[pnum]._pxoff = 0;
		plr[pnum]._pyoff = 0;
		plr[pnum]._pVar1 = xadd;
		plr[pnum]._pVar2 = yadd;
		plr[pnum]._pVar3 = EndDir;
		if ((plr[pnum]._pGFXLoad & PGL_WALK) == 0) LoadPlrGFX(pnum, PGL_WALK);
		NewPlrAnim(pnum, plr[pnum]._pWAnim[EndDir], plr[pnum]._pWFrames, 0, plr[pnum]._pWWidth);
		plr[pnum]._pdir = EndDir;
		plr[pnum]._pVar6 = 0;
		plr[pnum]._pVar7 = 0;
		plr[pnum]._pVar8 = 0;
		CheckEFlag(pnum, E_SINGLE);
		if (pnum == myplr) {
			if (svgamode) {
				if ((abs(ScrollInfo._sdx) < 3) && (abs(ScrollInfo._sdy) < 3)) ScrollInfo._sdir = scrldir;
				else ScrollInfo._sdir = SCRL_NONE;
			} else {
				if ((abs(ScrollInfo._sdx) < 2) && (abs(ScrollInfo._sdy) < 2)) ScrollInfo._sdir = scrldir;
				else ScrollInfo._sdir = SCRL_NONE;
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void StartWalk2(int pnum, int xvel, int yvel, int xoff, int yoff,
				int xadd, int yadd, int EndDir, int scrldir)
{
	long	fx,fy;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("StartWalk2: illegal player %d",pnum);
	if ((plr[pnum]._pInvincible) && (plr[pnum]._pHitPoints == 0) && (pnum == myplr)) {
		StartPlrKill(pnum, KILL_UNKNOWN);
		return;
	}

	SetPlayerOld(pnum);
	fx = plr[pnum]._px + xadd;
	fy = plr[pnum]._py + yadd;
	if (PlrDirOK(pnum, EndDir)) {
		//PlrClrTrans(plr[pnum]._px, plr[pnum]._py);
		//PlrDoTrans(fx, fy);
		plr[pnum]._pfutx = fx;
		plr[pnum]._pfuty = fy;
		if (pnum == myplr) {
			ScrollInfo._sdx = plr[pnum]._px - ViewX;
			ScrollInfo._sdy = plr[pnum]._py - ViewY;
		}
		dPlayer[plr[pnum]._px][plr[pnum]._py] = -1 - (char)pnum;
		plr[pnum]._pVar1 = plr[pnum]._px;
		plr[pnum]._pVar2 = plr[pnum]._py;
		plr[pnum]._px = fx;
		plr[pnum]._py = fy;
		dPlayer[plr[pnum]._px][plr[pnum]._py] = 1 + (char)pnum;
		plr[pnum]._pxoff = xoff;
		plr[pnum]._pyoff = yoff;
		ChangeLightXY(plr[pnum]._plid, plr[pnum]._px, plr[pnum]._py);
		PM_ChangeLightOff(pnum);
		plr[pnum]._pmode = PM_WALK2;
		plr[pnum]._pxvel = xvel;
		plr[pnum]._pyvel = yvel;
		plr[pnum]._pVar6 = xoff << 8;
		plr[pnum]._pVar7 = yoff << 8;
		plr[pnum]._pVar3 = EndDir;
		if ((plr[pnum]._pGFXLoad & PGL_WALK) == 0) LoadPlrGFX(pnum, PGL_WALK);
		NewPlrAnim(pnum, plr[pnum]._pWAnim[EndDir], plr[pnum]._pWFrames, 0, plr[pnum]._pWWidth);
		plr[pnum]._pdir = EndDir;
		plr[pnum]._pVar8 = 0;
		if (EndDir == DIR_DR) CheckEFlag(pnum, E_DOUBLE);
		else CheckEFlag(pnum, E_SINGLE);
		if (pnum == myplr) {
			if (svgamode) {
				if ((abs(ScrollInfo._sdx) < 3) && (abs(ScrollInfo._sdy) < 3)) ScrollInfo._sdir = scrldir;
				else ScrollInfo._sdir = SCRL_NONE;
			} else {
				if ((abs(ScrollInfo._sdx) < 2) && (abs(ScrollInfo._sdy) < 2)) ScrollInfo._sdir = scrldir;
				else ScrollInfo._sdir = SCRL_NONE;
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void StartWalk3(int pnum, int xvel, int yvel, int xoff, int yoff,
				int xadd, int yadd, int txa, int tya, int EndDir, int scrldir)
{
	long	fx,fy;
	long	tx,ty;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("StartWalk3: illegal player %d",pnum);
	if ((plr[pnum]._pInvincible) && (plr[pnum]._pHitPoints == 0) && (pnum == myplr)) {
		StartPlrKill(pnum, KILL_UNKNOWN);
		return;
	}

	SetPlayerOld(pnum);
	fx = plr[pnum]._px + xadd;
	fy = plr[pnum]._py + yadd;
	tx = plr[pnum]._px + txa;			// Temp location for drawing
	ty = plr[pnum]._py + tya;
	if (PlrDirOK(pnum, EndDir)) {
		//PlrClrTrans(plr[pnum]._px, plr[pnum]._py);
		//PlrDoTrans(fx, fy);
		plr[pnum]._pfutx = fx;
		plr[pnum]._pfuty = fy;
		if (pnum == myplr) {
			ScrollInfo._sdx = plr[pnum]._px - ViewX;
			ScrollInfo._sdy = plr[pnum]._py - ViewY;
		}
		dPlayer[plr[pnum]._px][plr[pnum]._py] = -1 - (char)pnum;
		dPlayer[fx][fy] = -1 - (char)pnum;
		plr[pnum]._pVar4 = tx;
		plr[pnum]._pVar5 = ty;
		dFlags[tx][ty] |= BFLAG_PLRLR;
		plr[pnum]._pxoff = xoff;
		plr[pnum]._pyoff = yoff;
		if(leveltype)
		{
			ChangeLightXY(plr[pnum]._plid, tx, ty);
			PM_ChangeLightOff(pnum);
		}
		plr[pnum]._pmode = PM_WALK3;
		plr[pnum]._pxvel = xvel;
		plr[pnum]._pyvel = yvel;
		plr[pnum]._pVar1 = fx;
		plr[pnum]._pVar2 = fy;
		plr[pnum]._pVar6 = xoff << 8;
		plr[pnum]._pVar7 = yoff << 8;
		plr[pnum]._pVar3 = EndDir;
		if ((plr[pnum]._pGFXLoad & PGL_WALK) == 0) LoadPlrGFX(pnum, PGL_WALK);
		NewPlrAnim(pnum, plr[pnum]._pWAnim[EndDir], plr[pnum]._pWFrames, 0, plr[pnum]._pWWidth);
		plr[pnum]._pdir = EndDir;
		plr[pnum]._pVar8 = 0;
		CheckEFlag(pnum, E_SINGLE);
		if (pnum == myplr) {
			if (svgamode) {
				if ((abs(ScrollInfo._sdx) < 3) && (abs(ScrollInfo._sdy) < 3)) ScrollInfo._sdir = scrldir;
				else ScrollInfo._sdir = SCRL_NONE;
			} else {
				if ((abs(ScrollInfo._sdx) < 2) && (abs(ScrollInfo._sdy) < 2)) ScrollInfo._sdir = scrldir;
				else ScrollInfo._sdir = SCRL_NONE;
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void StartAttack(int pnum, int d)
{
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("StartAttack: illegal player %d",pnum);
	if ((plr[pnum]._pInvincible) && (plr[pnum]._pHitPoints == 0) && (pnum == myplr)) {
		StartPlrKill(pnum, KILL_UNKNOWN);
		return;
	}

	if ((plr[pnum]._pGFXLoad & PGL_ATTACK) == 0) LoadPlrGFX(pnum, PGL_ATTACK);
	NewPlrAnim(pnum, plr[pnum]._pAAnim[d], plr[pnum]._pAFrames, 0, plr[pnum]._pAWidth);

	plr[pnum]._pmode = PM_ATTACK;
	FixPlayerLocation(pnum,d);
	SetPlayerOld(pnum);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void StartRangeAttack(int pnum, int d, int dx, int dy)
{
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("StartRangeAttack: illegal player %d",pnum);
	if ((plr[pnum]._pInvincible) && (plr[pnum]._pHitPoints == 0) && (pnum == myplr)) {
		StartPlrKill(pnum, KILL_UNKNOWN);
		return;
	}

	if ((plr[pnum]._pGFXLoad & PGL_ATTACK) == 0) LoadPlrGFX(pnum, PGL_ATTACK);
	NewPlrAnim(pnum, plr[pnum]._pAAnim[d], plr[pnum]._pAFrames, 0, plr[pnum]._pAWidth);

	plr[pnum]._pmode = PM_RATTACK;
	FixPlayerLocation(pnum,d);
	SetPlayerOld(pnum);
	plr[pnum]._pVar1 = dx;
	plr[pnum]._pVar2 = dy;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void StartPlrBlock(int pnum, int dir)
{
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("StartPlrBlock: illegal player %d",pnum);
	if ((plr[pnum]._pInvincible) && (plr[pnum]._pHitPoints == 0) && (pnum == myplr)) {
		StartPlrKill(pnum, KILL_UNKNOWN);
		return;
	}

	PlaySfxLoc(IS_ISWORD, plr[pnum]._px, plr[pnum]._py);

	if ((plr[pnum]._pGFXLoad & PGL_BLOCK) == 0) LoadPlrGFX(pnum, PGL_BLOCK);
	NewPlrAnim(pnum, plr[pnum]._pBAnim[dir], plr[pnum]._pBFrames, 2, plr[pnum]._pBWidth);
	plr[pnum]._pmode = PM_BLOCK;
	FixPlayerLocation(pnum,dir);
	SetPlayerOld(pnum);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void StartSpell(int pnum, int d, int cx, int cy)
{
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("StartSpell: illegal player %d",pnum);
	if ((plr[pnum]._pInvincible) && (plr[pnum]._pHitPoints == 0) && (pnum == myplr)) {
		StartPlrKill(pnum, KILL_UNKNOWN);
		return;
	}

	if (leveltype != 0) {
		switch (spelldata[plr[pnum]._pSpell].sType) {
			case ST_FIRE :
				if ((plr[pnum]._pGFXLoad & PGL_FMAG) == 0) LoadPlrGFX(pnum, PGL_FMAG);
				NewPlrAnim(pnum, plr[pnum]._pFAnim[d], plr[pnum]._pSFrames, 0, plr[pnum]._pSWidth);
				break;
			case ST_LIGHT :
				if ((plr[pnum]._pGFXLoad & PGL_LMAG) == 0) LoadPlrGFX(pnum, PGL_LMAG);
				NewPlrAnim(pnum, plr[pnum]._pLAnim[d], plr[pnum]._pSFrames, 0, plr[pnum]._pSWidth);
				break;
			case ST_MISC :
				if ((plr[pnum]._pGFXLoad & PGL_TMAG) == 0) LoadPlrGFX(pnum, PGL_TMAG);
				NewPlrAnim(pnum, plr[pnum]._pTAnim[d], plr[pnum]._pSFrames, 0, plr[pnum]._pSWidth);
				break;
		}
	}
	PlaySfxLoc(spelldata[plr[pnum]._pSpell].sSFX, plr[pnum]._px, plr[pnum]._py);

	plr[pnum]._pmode = PM_SPELL;
	FixPlayerLocation(pnum,d);
	SetPlayerOld(pnum);

	plr[pnum]._pVar1 = cx;
	plr[pnum]._pVar2 = cy;
	//if (((plr[pnum]._pSplFrom == SPL_FROMBK) || (plr[pnum]._pSplFrom == SPL_FROMR)) && (pnum == myplr))
		plr[pnum]._pVar4 = GetSpellLevel(pnum, plr[pnum]._pSpell);
	//else
	//	plr[pnum]._pVar4 = 1;
	plr[pnum]._pVar8 = 1;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void FixPlrWalkTags(int pnum) {
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("FixPlrWalkTags: illegal player %d",pnum);

	int pp = pnum + 1;
	int pn = -1 - pnum;
	int px = plr[pnum]._poldx;
	int py = plr[pnum]._poldy;
	for (int y = py - 1; y <= py + 1; y++) {
		for (int x = px - 1; x <= px + 1; x++) {
			if (x < 0 || x >= DMAXX) continue;
			if (y < 0 || y >= DMAXY) continue;
			if (dPlayer[x][y] == pp || dPlayer[x][y] == pn)
				dPlayer[x][y] = 0;
		}
	}

	if (px < 0 || px >= DMAXX-1) return;
	if (py < 0 || py >= DMAXY-1) return;
	dFlags[px+1][py+0] &= BFMASK_PLRLR;
	dFlags[px+0][py+1] &= BFMASK_PLRLR;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void RemovePlrFromMap(int pnum)
{
	int pp = pnum + 1;
	int pn = -1 - pnum;
	int x,y;
	for (y = 1; y < DMAXY; y++) {
		for (x = 1; x < DMAXX; x++) {
			if (((dPlayer[x][y-1] == pn) || (dPlayer[x-1][y] == pn)) && (dFlags[x][y] & BFLAG_PLRLR))
				dFlags[x][y] &= BFMASK_PLRLR;
		}
	}
	for (y = 0; y < DMAXY; y++) {
		for (x = 0; x < DMAXX; x++) {
			if (dPlayer[x][y] == pp || dPlayer[x][y] == pn)
				dPlayer[x][y] = 0;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void StartPlrHit(int pnum, int dam, BOOL forcehit)
{
	int		pd;
	
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("StartPlrHit: illegal player %d",pnum);
	if ((plr[pnum]._pInvincible) && (plr[pnum]._pHitPoints == 0) && (pnum == myplr)) {
		StartPlrKill(pnum, KILL_UNKNOWN);
		return;
	}

	if (plr[pnum]._pClass == CLASS_WARRIOR) PlaySfxLoc(PS_WARR69, plr[pnum]._px, plr[pnum]._py);
	#if !IS_VERSION(SHAREWARE)
	else if (plr[pnum]._pClass == CLASS_ROGUE) PlaySfxLoc(PS_ROGUE69, plr[pnum]._px, plr[pnum]._py);
	else if (plr[pnum]._pClass == CLASS_SORCEROR) PlaySfxLoc(PS_MAGE69, plr[pnum]._px, plr[pnum]._py);
	else if (plr[pnum]._pClass == CLASS_MONK) PlaySfxLoc(PS_MONK69, plr[pnum]._px, plr[pnum]._py);
	else if (plr[pnum]._pClass == CLASS_BARD) PlaySfxLoc(PS_BARD69, plr[pnum]._px, plr[pnum]._py);
	else if (plr[pnum]._pClass == CLASS_BARBARIAN) PlaySfxLoc(PS_BARBARIAN69, plr[pnum]._px, plr[pnum]._py);
	#endif
	//PlaySfxLoc(PS_LGHIT, plr[pnum]._px, plr[pnum]._py);

	// Update hit bar
	drawhpflag = TRUE;

	if (plr[pnum]._pClass == CLASS_BARBARIAN)
	{
		if ((dam >> HP_SHIFT) < (plr[pnum]._pLevel + (plr[pnum]._pLevel/4))
			&& !forcehit)
		{
			return;
		}
	}
	else if ((dam >> HP_SHIFT) < plr[pnum]._pLevel
			&& !forcehit)
		return;

	pd = plr[pnum]._pdir;
	if ((plr[pnum]._pGFXLoad & PGL_HIT) == 0) LoadPlrGFX(pnum, PGL_HIT);
	NewPlrAnim(pnum, plr[pnum]._pHAnim[pd], plr[pnum]._pHFrames, 0, plr[pnum]._pHWidth);
	plr[pnum]._pmode = PM_GOTHIT;
	FixPlayerLocation(pnum,pd);
	plr[pnum]._pVar8 = 1;
	FixPlrWalkTags(pnum);
	dPlayer[plr[pnum]._px][plr[pnum]._py] = 1 + (char)pnum;
	SetPlayerOld(pnum);
}
// JMM.PATCH1.2/24/97
void ShowDupString( LPCSTR lpszMessage );
// END.PATCH1
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void RespawnDeadItem(ItemStruct *itm, int x, int y)
{
	int ii;

	if (numitems < MAXITEMS) {
		// JMM.PATCH1.2/24/97
		if(FindGetItem(itm->IDidx, itm->_iCreateInfo, itm->_iSeed) >= 0) {
			DROPLOG("  Duplicate item detected!\n");
			// JMM.PATCH1.2.22.97
			// NetSendString(1 << myplr, "A duplicate item has been detected. Unable to drop.");
			ShowDupString("A duplicate item has been detected.  Destroying duplicate...");
			SyncGetItem(x,y,itm->IDidx,itm->_iCreateInfo,itm->_iSeed);
			// END.JMM.PATCH1.2.22.97
		}
		// ENDPATCH1.2/24/97

		ii = itemavail[0];
		dItem[x][y] = ii+1;
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;

		item[ii] = *itm;
		item[ii]._ix = x;
		item[ii]._iy = y;

		RespawnItem(ii, TRUE);
		numitems++;

		itm->_itype = -1;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void PlrDeadItem(int pnum, ItemStruct *itm, int xx, int yy) {

	// no item to drop?
	if (itm->_itype == -1) return;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("PlrDeadItem: illegal player %d",pnum);

	// try optimal position first
	int x = plr[pnum]._px + xx;
	int y = plr[pnum]._py + yy;

	if ((xx != 0) || (yy != 0)) {
		// Start placing
		if (ItemSpaceOk(x, y)) {
			RespawnDeadItem(itm, x, y);
			plr[pnum].HoldItem = *itm;
			NetSendCmdPItem(FALSE,CMD_RESPAWNITEM,x,y);
			return;
		}
	}

	for (int l = 1; l < 50; l++) {
		for (int j = -l; j <= l; j++) {
			y = plr[pnum]._py + j;
			for (int i = -l; i <= l; i++) {
				x = plr[pnum]._px + i;
				if (! ItemSpaceOk(x,y)) continue;

				// drop it
				RespawnDeadItem(itm, x, y);
				plr[pnum].HoldItem = *itm;
				NetSendCmdPItem(FALSE,CMD_RESPAWNITEM,x,y);
				return;
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void StartPlayerKill(int pnum, BOOL earflag)
{
	ItemStruct * pi;
	PlayerStruct * p = &plr[pnum];
	ItemStruct ear;
	int i;

	// already dead?
	if ((plr[pnum]._pHitPoints <= 0) && (plr[pnum]._pmode == PM_DEATH)) return;

	// if it's me, let everyone know
	if (myplr == pnum) NetSendCmdParam1(TRUE,CMD_PLRDEAD,earflag);

	// dead players don't lose body items on diablo level
	BOOL diablolevel = (gbMaxPlayers > 1) && (plr[pnum].plrlevel == 16);
	
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("StartPlayerKill: illegal player %d",pnum);

	if (plr[pnum]._pClass == CLASS_WARRIOR) PlaySfxLoc(PS_DEAD, p->_px, p->_py);
	#if !IS_VERSION(SHAREWARE)
	else if (plr[pnum]._pClass == CLASS_ROGUE) PlaySfxLoc(PS_ROGUE71, p->_px, p->_py);
	else if (plr[pnum]._pClass == CLASS_SORCEROR) PlaySfxLoc(PS_MAGE71, p->_px, p->_py);
	else if (plr[pnum]._pClass == CLASS_MONK) PlaySfxLoc(PS_MONK71, p->_px, p->_py);
	else if (plr[pnum]._pClass == CLASS_BARD) PlaySfxLoc(PS_BARD71, p->_px, p->_py);
	else if (plr[pnum]._pClass == CLASS_BARBARIAN) PlaySfxLoc(PS_BARBARIAN71, p->_px, p->_py);
	#endif

	// Give the plr no items in hands gfx
	if (p->_pgfxnum != PGFX_NGUY) {
		p->_pgfxnum = PGFX_NGUY;
		p->_pGFXLoad = 0;
		SetPlrAnims(pnum);
	}

	if ((p->_pGFXLoad & PGL_DEAD) == 0) LoadPlrGFX(pnum, PGL_DEAD);
	NewPlrAnim(pnum, p->_pDAnim[p->_pdir], p->_pDFrames, 1, p->_pDWidth);
	p->_pmode = PM_DEATH;
	p->_pBlockFlag = FALSE;
	p->_pInvincible = TRUE;
	SetPlayerHitPoints(pnum, 0);
	p->_pVar8 = 1;
	if ((pnum != myplr) && (earflag == FALSE) && !diablolevel) {
		pi = &p->InvBody[0];
		for (i = NUM_INVLOC; i--; pi++) pi->_itype = -1;
		CalcPlrInv(pnum, FALSE);
	}

	if (plr[pnum].plrlevel != currlevel) return;

	// Clean up map and set up vars for death
	FixPlayerLocation(pnum,p->_pdir);
	RemovePlrFromMap(pnum);
	dFlags[p->_px][p->_py] |= BFLAG_DEADPLR;
	SetPlayerOld(pnum);

	// Bail out if this is not me
	if (pnum != myplr) return;

	drawhpflag = TRUE;
	deathdelay = 30;

	// Drop all item in hand
	if (curs >= ICSTART) {
		PlrDeadItem(pnum, &p->HoldItem, 0, 0);
		NewCursor(GLOVE_CURS);
	}

	if (diablolevel)
		return;

	// Drop half of players gold
	DropHalfPlayersGold(pnum);

	// Drop player ear if killed by player
	if (earflag == KILL_UNKNOWN) return;

	if (earflag) {
		SetPlrHandItem(&ear, IDI_EAR);
		sprintf(ear._iName, "Ear of %s", plr[pnum]._pName);
		if (plr[pnum]._pClass == CLASS_SORCEROR) ear._iCurs = ITEM_EAR1;
		else if (plr[pnum]._pClass == CLASS_WARRIOR) ear._iCurs = ITEM_EAR2;
		else if (plr[pnum]._pClass == CLASS_ROGUE) ear._iCurs = ITEM_EAR3;
		// Fix this if necessary.
		else if (plr[pnum]._pClass == CLASS_MONK) ear._iCurs = ITEM_EAR3;
		else if (plr[pnum]._pClass == CLASS_BARD) ear._iCurs = ITEM_EAR3;
		else if (plr[pnum]._pClass == CLASS_BARBARIAN) ear._iCurs = ITEM_EAR3;
		
		ear._iCreateInfo = (plr[pnum]._pName[0] << 8) | plr[pnum]._pName[1];
		ear._iSeed = (plr[pnum]._pName[2] << 24) |
						(plr[pnum]._pName[3] << 16) |
						(plr[pnum]._pName[4] << 8) |
						plr[pnum]._pName[5];
		ear._ivalue = plr[pnum]._pLevel;
		// my ear already on ground?
		int ii = FindGetItem(IDI_EAR, ear._iCreateInfo, ear._iSeed);
		if (ii == -1) PlrDeadItem(pnum, &ear, 0, 0);
	} else {
		// Drop body items if killed by monster
		pi = &p->InvBody[0];
		for (i = NUM_INVLOC; i--; pi++) {
			int pdd = (p->_pdir + i) & 0x07;
			PlrDeadItem(pnum, pi, offset_x[pdd], offset_y[pdd]);
		}
		CalcPlrInv(pnum, FALSE);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void DropHalfPlayersGold(int pnum)
{
	int		i;
	long	hGold;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("DropHalfPlayersGold: illegal player %d",pnum);
	
	// Get half of players gold
	hGold = plr[pnum]._pGold >> 1;

	// Remove gold from speed list first; if there is any
	// Try non GOLD_VMAX first, then break into the GOLD_VMAX piles
	for (i = 0; ((i < MAXSPD) && (hGold > 0)); i++) {
		if ((plr[pnum].SpdList[i]._itype == IT_GOLD) &&
			(plr[pnum].SpdList[i]._ivalue != GOLD_VMAX)) {
			if (hGold < plr[pnum].SpdList[i]._ivalue) {
				// update gold items value
				plr[pnum].SpdList[i]._ivalue -= hGold;
				// update gold items cursor
				SetSpdbarGoldCurs(pnum, i);
				// initialize hold item to gold type
				SetPlrHandItem(&plr[pnum].HoldItem, IDI_GOLD);
				GetGoldSeed(pnum, &plr[pnum].HoldItem);
				SetPlrHandGoldCurs(&plr[pnum].HoldItem);
				// set hold items value equal to hGold
				plr[pnum].HoldItem._ivalue = hGold;
				// drop the hold item
				PlrDeadItem(pnum, &plr[pnum].HoldItem, 0, 0);
				// initialize so we can exit the loop
				hGold = 0;
			} else {
				// decrement hGold by SpdList items value
				hGold -= plr[pnum].SpdList[i]._ivalue;
				// remove gold item from SpdList
				RemoveSpdBarItem(pnum, i);
				// initialize hold item to gold type
				SetPlrHandItem(&plr[pnum].HoldItem, IDI_GOLD);
				GetGoldSeed(pnum, &plr[pnum].HoldItem);
				SetPlrHandGoldCurs(&plr[pnum].HoldItem);
				// set hold items value equal to SpdList items value
				plr[pnum].HoldItem._ivalue = plr[pnum].SpdList[i]._ivalue;
				// drop the hold item
				PlrDeadItem(pnum, &plr[pnum].HoldItem, 0, 0);
				// restart looping
				i = -1;
			} 
		} 
	} 
	if (hGold > 0) {
		// SpdList GOLD_VMAX piles
		for (i = 0; ((i < MAXSPD) && (hGold > 0)); i++) {
			if (plr[pnum].SpdList[i]._itype == IT_GOLD) {
				if (hGold < plr[pnum].SpdList[i]._ivalue) {
					// update gold items value
					plr[pnum].SpdList[i]._ivalue -= hGold;
					// update gold items cursor
					SetSpdbarGoldCurs(pnum, i);
					// initialize hold item to gold type
					SetPlrHandItem(&plr[pnum].HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &plr[pnum].HoldItem);
					SetPlrHandGoldCurs(&plr[pnum].HoldItem);
					// set hold items value equal to hGold
					plr[pnum].HoldItem._ivalue = hGold;
					// drop the hold item
					PlrDeadItem(pnum, &plr[pnum].HoldItem, 0, 0);
					// initialize so we can exit the loop
					hGold = 0;
				} else {
					// decrement hGold by SpdList items value
					hGold -= plr[pnum].SpdList[i]._ivalue;
					// remove gold item from SpdList
					RemoveSpdBarItem(pnum, i);
					// initialize hold item to gold type
					SetPlrHandItem(&plr[pnum].HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &plr[pnum].HoldItem);
					SetPlrHandGoldCurs(&plr[pnum].HoldItem);
					// set hold items value equal to SpdList items value
					plr[pnum].HoldItem._ivalue = plr[pnum].SpdList[i]._ivalue;
					// drop the hold item
					PlrDeadItem(pnum, &plr[pnum].HoldItem, 0, 0);
					// restart looping
					i = -1;		
				} 
			} 
		} 
	} 
	force_redraw = FULLDRAW;

    // Remove gold from inventory list if there was no gold in the speed list
	// or all gold from the speed list was deplenished.
	// Try non GOLD_VMAX first, then break into the GOLD_VMAX piles
	if (hGold > 0) {
		for (i = 0; ((i < plr[pnum]._pNumInv) && (hGold > 0)); i++) {
			if ((plr[pnum].InvList[i]._itype == IT_GOLD) &&
				(plr[pnum].InvList[i]._ivalue != GOLD_VMAX)) {
				if (hGold < plr[pnum].InvList[i]._ivalue) {
					// update gold items value
					plr[pnum].InvList[i]._ivalue -= hGold;
					// update gold items cursor
					SetGoldCurs(pnum, i);
					// initialize hold item to gold type
					SetPlrHandItem(&plr[pnum].HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &plr[pnum].HoldItem);
					SetPlrHandGoldCurs(&plr[pnum].HoldItem);
					// set hold items value equal to hGold
					plr[pnum].HoldItem._ivalue = hGold;
					// drop the hold item
					PlrDeadItem(pnum, &plr[pnum].HoldItem, 0, 0);
					hGold = 0;
				} else {
					// decrement hGold by InvList items value
					hGold -= plr[pnum].InvList[i]._ivalue;
					// remove gold item from InvList
					RemoveInvItem(pnum, i);
					// initialize hold item to gold type
					SetPlrHandItem(&plr[pnum].HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &plr[pnum].HoldItem);
					SetPlrHandGoldCurs(&plr[pnum].HoldItem);
					// set hold items value equal to InvList items value
					plr[pnum].HoldItem._ivalue = plr[pnum].InvList[i]._ivalue;
					// drop the hold item
					PlrDeadItem(pnum, &plr[pnum].HoldItem, 0, 0);
					// restart looping
					i = -1;
				} 
			} 
		} 
		if (hGold > 0) {
			// InvList GOLD_VMAX piles
			for (i = 0; ((i < plr[pnum]._pNumInv) && (hGold > 0)); i++) {
				if (plr[pnum].InvList[i]._itype == IT_GOLD) {
					if (hGold < plr[pnum].InvList[i]._ivalue) {
						// update gold items value
						plr[pnum].InvList[i]._ivalue -= hGold;
						// update gold items cursor
						SetGoldCurs(pnum, i);
						// initialize hold item to gold type
						SetPlrHandItem(&plr[pnum].HoldItem, IDI_GOLD);
						GetGoldSeed(pnum, &plr[pnum].HoldItem);
						SetPlrHandGoldCurs(&plr[pnum].HoldItem);
						// set hold items value equal to hGold
						plr[pnum].HoldItem._ivalue = hGold;
						// drop the hold item
						PlrDeadItem(pnum, &plr[pnum].HoldItem, 0, 0);
						hGold = 0;
					} else {
						// decrement hGold by InvList items value
						hGold -= plr[pnum].InvList[i]._ivalue;
						// remove gold item from InvList
						RemoveInvItem(pnum, i);
						// initialize hold item to gold type
						SetPlrHandItem(&plr[pnum].HoldItem, IDI_GOLD);
						GetGoldSeed(pnum, &plr[pnum].HoldItem);
						SetPlrHandGoldCurs(&plr[pnum].HoldItem);
						// set hold items value equal to InvList items value
						plr[pnum].HoldItem._ivalue = plr[pnum].InvList[i]._ivalue;
						// drop the hold item
						PlrDeadItem(pnum, &plr[pnum].HoldItem, 0, 0);
						// restart looping
						i = -1;		
					}
				} 
			} 
		} 
	} 
	// update players gold to account for losing half
	plr[pnum]._pGold = CalculateGold(pnum);
}	


void StripTopGold(int pnum)
{
	int		i;
	long	hGold;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("StripTopGold: illegal player %d",pnum);
	
	ItemStruct tmpHold = plr[pnum].HoldItem;
	
    // Remove gold from inventory list if there was no gold in the speed list
	// or all gold from the speed list was deplenished.
	// Try non GOLD_VMAX first, then break into the GOLD_VMAX piles
	for (i = 0; i < plr[pnum]._pNumInv; i++)
	{
		if ((plr[pnum].InvList[i]._itype == IT_GOLD) &&
			(plr[pnum].InvList[i]._ivalue > GOLD_VMAX))
		{
			
			// first, remove the gold from the slot
			hGold = plr[pnum].InvList[i]._ivalue - GOLD_VMAX;
			plr[pnum].InvList[i]._ivalue = GOLD_VMAX;
			
			// update gold items cursor
			SetGoldCurs(pnum, i);
			// initialize hold item to gold type
			SetPlrHandItem(&plr[pnum].HoldItem, IDI_GOLD);
			GetGoldSeed(pnum, &plr[pnum].HoldItem);
			SetPlrHandGoldCurs(&plr[pnum].HoldItem);
			// set hold items value equal to hGold
			plr[pnum].HoldItem._ivalue = hGold;
			
			hGold = 0;
			// if it'll fit elsewhere, continue
			if (!GoldAutoPlace(pnum)) // else drop the hold item
				PlrDeadItem(pnum, &plr[pnum].HoldItem, 0, 0);
		} 
	} 
	// update players gold
	plr[pnum]._pGold = CalculateGold(pnum);
	plr[pnum].HoldItem = tmpHold;
}	
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void StartPlrKill(int pnum, BOOL earflag)
{
	int i, mx;

	// PATCH1.JMM
	// no death on first level
	if ((0 >= plr[pnum]._pHitPoints) && (0 == currlevel)) {
		SetPlayerHitPoints(pnum,1<<HP_SHIFT);
		return;
	}
	// ENDPATCH1.JMM

	for (i = 0; i < nummissiles; i++) {
		mx = missileactive[i];
		if ((missile[mx]._mitype == MIT_MANASHIELD) && (missile[mx]._misource == pnum) && !missile[mx]._miDelFlag) {
			if (earflag != KILL_UNKNOWN) missile[mx]._miVar8 = earflag;		// killed by player?
			return;
		}
	}

	// took out setting hp to zero before this routine is called so manashld calc works - rjs
	SetPlayerHitPoints(pnum, 0);
	StartPlayerKill(pnum, earflag);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SyncPlrKill(int pnum, BOOL earflag)
{
	app_assert((DWORD)pnum < MAX_PLRS);
	StartPlayerKill(pnum, earflag);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void RemovePlrMissiles(int pnum)
{
	int i, mx;

	if ((currlevel != 0) && (pnum == myplr) && (monster[myplr]._mx != 1 || monster[myplr]._my != 0)) {
		M_StartKill(myplr, myplr);
		extern void AddDead(int, int, char, int);
		AddDead(monster[myplr]._mx, monster[myplr]._my, monster[myplr].MType->mdeadval, monster[myplr]._mdir);
		dMonster[monster[myplr]._mx][monster[myplr]._my] = 0;
		monster[myplr]._mDelFlag = TRUE;
		DeleteMonsterList();
	}

	void DeleteMissile(int, int);
	for (i = 0; i < nummissiles; i++) {
		mx = missileactive[i];
		if ((missile[mx]._mitype == MIT_STONE) && (missile[mx]._misource == pnum))
			monster[(missile[mx]._miVar2)]._mmode = missile[mx]._miVar1;
		if ((missile[mx]._mitype == MIT_MANASHIELD) && (missile[mx]._misource == pnum)) {
			ClearMissileSpot(mx);
			DeleteMissile(mx, i);
		}
		if ((missile[mx]._mitype == MIT_ETHER) && (missile[mx]._misource == pnum)) {
			ClearMissileSpot(mx);
			DeleteMissile(mx, i);
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void InitLevelChange(int pnum)
{
	RemovePlrMissiles(pnum);

	if ((pnum == myplr) && (qtextflag)) {
		qtextflag = FALSE;
		stream_stop();
	}
	// remove player from current level immediately
	RemovePlrFromMap(pnum);
	SetPlayerOld(pnum);
	if (pnum == myplr) dPlayer[plr[myplr]._px][plr[myplr]._py] = 1 + myplr;
	else plr[pnum]._pLvlVisited[plr[pnum].plrlevel] = TRUE;

	ClrPlrPath(pnum);
	plr[pnum].destAction = PCMD_NOTHING;
	plr[pnum]._pLvlChanging = TRUE;
	if (pnum == myplr) plr[pnum].pLvlLoad = LVLCHANGE_TIME;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void StartNewLvl(int pnum, int fom, int lvl)
{
	InitLevelChange(pnum);

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("StartNewLvl: illegal player %d",pnum);
	switch (fom) {
		case WM_DIABNEXTLVL:
		case WM_DIABPREVLVL:
		case WM_DIABTOWNWARP:
			//app_assert(plr[pnum].plrlevel != lvl);
			plr[pnum].plrlevel = lvl;
		break;

		case WM_DIABTWARPUP:
			plr[myplr].pTownWarps |= 1 << (leveltype - 2);
			plr[pnum].plrlevel = lvl;
		break;

		case WM_DIABRETOWN:
			// everything already set
		break;

		case WM_DIABRTNLVL:
			// can't do any validation on this
			// one since it stuffs into global vars
			app_assert(gbMaxPlayers == 1);
			plr[pnum].plrlevel = lvl;
		break;

		case WM_DIABSETLVL:
			setlvlnum = lvl;
			app_assert(gbMaxPlayers == 1);
		break;

		default:
			app_fatal("StartNewLvl");
		break;
	}

	if (pnum == myplr) {
		plr[pnum]._pmode = PM_NEWLVL;
		plr[pnum]._pInvincible = TRUE;
		PostMessage(ghMainWnd,fom,0,0);
		if (gbMaxPlayers > 1) NetSendCmdParam2(TRUE,CMD_NEWLVL,fom,lvl);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void RestartTownLvl(int pnum)
{
	InitLevelChange(pnum);

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("RestartTownLvl: illegal player %d",pnum);

	//app_assert(plr[pnum].plrlevel != 0);
	plr[pnum].plrlevel = 0;

	plr[pnum]._pInvincible = FALSE;
	SetPlayerHitPoints(pnum, 1 << HP_SHIFT);
	plr[pnum]._pMana = 0;
	plr[pnum]._pManaBase = plr[pnum]._pMana - (plr[pnum]._pMaxMana - plr[pnum]._pMaxManaBase);
	CalcPlrInv(pnum, FALSE);

	if (pnum == myplr) {
		plr[pnum]._pmode = PM_NEWLVL;
		plr[pnum]._pInvincible = TRUE;
		PostMessage(ghMainWnd,WM_DIABRETOWN,0,0);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void StartWarpLvl(int pnum, int pidx)
{
	InitLevelChange(pnum);

	if (gbMaxPlayers != 1) {
		if (plr[pnum].plrlevel != 0) plr[pnum].plrlevel = 0;
		else plr[pnum].plrlevel = portal[pidx].level;
	}

	if (pnum == myplr) {
		SetCurrentPortal(pidx);
		plr[pnum]._pmode = PM_NEWLVL;
		plr[pnum]._pInvincible = TRUE;
		PostMessage(ghMainWnd,WM_DIABWARPLVL,0,0);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int PM_DoStand(int pnum)
{
	return(RUN_DONE);
}

/*-----------------------------------------------------------------------*
********************************** TEMP *********************************
**-----------------------------------------------------------------------*

BOOL rfix1 = FALSE;
BOOL rfix2 = FALSE;

void TempWalkFix(int pnum)
{
	if (currlevel == 0) return;
	if ((plr[pnum]._pClass == CLASS_ROGUE) && (plr[pnum]._pAnimLen == 6)) {
		if ((plr[pnum]._pAnimFrame == 2) && !rfix1) {
			plr[pnum]._pAnimFrame = 1;
			rfix1 = TRUE;
		} else rfix1 = FALSE;
		if ((plr[pnum]._pAnimFrame == 4) && !rfix2) {
			plr[pnum]._pAnimFrame = 3;
			rfix2 = TRUE;
		} else rfix2 = FALSE;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int PM_DoWalk(int pnum)
{
	int		rv;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("PM_DoWalk: illegal player %d",pnum);

#if 0
	// changed _pAnimFrame == 2 to == 3 to prevent "extra" footstep == pat
//	if ((plr[pnum]._pAnimFrame == 3)
//		|| (plr[pnum]._pWFrames == 8 && plr[pnum]._pAnimFrame == 7)
//		|| (plr[pnum]._pWFrames != 8 && plr[pnum]._pAnimFrame == 4))
//		PlaySfxLoc(PS_WALK1, plr[pnum]._px, plr[pnum]._py);
	//TempWalkFix(pnum);
#endif
	
	if (currlevel == 0 && gbWalkOn)		// double-speed walk in town
	{
		if (plr[pnum]._pAnimFrame%2 == 0) 	// should be even frames only
		{
			++plr[pnum]._pAnimFrame;
			++plr[pnum]._pVar8;
		}
		if (plr[pnum]._pAnimFrame >= plr[pnum]._pWFrames)
			plr[pnum]._pAnimFrame = 0;
	}

	int l = 8;
	
	if (currlevel != 0) l = PlrWalkLenTbl[plr[pnum]._pClass];
	if (plr[pnum]._pVar8 >= l) {
		dPlayer[plr[pnum]._px][plr[pnum]._py] = 0;
		plr[pnum]._px += plr[pnum]._pVar1;
		plr[pnum]._py += plr[pnum]._pVar2;
		dPlayer[plr[pnum]._px][plr[pnum]._py] = 1 + (char)pnum;
		if (leveltype != 0) {
			ChangeLightXY(plr[pnum]._plid, plr[pnum]._px, plr[pnum]._py);
			ChangeVisionXY(plr[pnum]._pvid, plr[pnum]._px, plr[pnum]._py);
		}
		if (pnum == myplr) {
			if (ScrollInfo._sdir != SCRL_NONE) {
				ViewX = plr[pnum]._px - ScrollInfo._sdx;
				ViewY = plr[pnum]._py - ScrollInfo._sdy;
			}
		}
		if (plr[pnum].walkpath[0] != PCMD_NOTHING) StartWalkStand(pnum);
		else StartStand(pnum, plr[pnum]._pVar3);
		ClearPlrPVars(pnum);
		if (leveltype != 0) ChangeLightOff(plr[pnum]._plid, 0, 0);
		rv = RUN_AGAIN;
	} else {
		PM_ChangeOffset(pnum);
		rv = RUN_DONE;
	}

	return (rv);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int PM_DoWalk2(int pnum)
{
	int		rv;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("PM_DoWalk2: illegal player %d",pnum);

#if 0
	// changed _pAnimFrame == 2 to == 3 to prevent "extra" footstep == pat
	if ((plr[pnum]._pAnimFrame == 3)
		|| (plr[pnum]._pWFrames == 8 && plr[pnum]._pAnimFrame == 7)
		|| (plr[pnum]._pWFrames != 8 && plr[pnum]._pAnimFrame == 4))
		PlaySfxLoc(PS_WALK1, plr[pnum]._px, plr[pnum]._py);
	//TempWalkFix(pnum);
#endif
	if (currlevel == 0 && gbWalkOn)		// double-speed walk in town
	{
		if (plr[pnum]._pAnimFrame%2 == 0) 	// should be even frames only
		{
			++plr[pnum]._pAnimFrame;
			++plr[pnum]._pVar8;
		}
		if (plr[pnum]._pAnimFrame >= plr[pnum]._pWFrames)
			plr[pnum]._pAnimFrame = 0;
	}

	int l = 8;
	
	if (currlevel != 0) l = PlrWalkLenTbl[plr[pnum]._pClass];
	if (plr[pnum]._pVar8 >= l) {
		dPlayer[plr[pnum]._pVar1][plr[pnum]._pVar2] = 0;
		if (leveltype != 0) {
			ChangeLightXY(plr[pnum]._plid, plr[pnum]._px, plr[pnum]._py);
			ChangeVisionXY(plr[pnum]._pvid, plr[pnum]._px, plr[pnum]._py);
		}
		if (pnum == myplr) {
			if (ScrollInfo._sdir != SCRL_NONE) {
				ViewX = plr[pnum]._px - ScrollInfo._sdx;
				ViewY = plr[pnum]._py - ScrollInfo._sdy;
			}
		}
		if (plr[pnum].walkpath[0] != PCMD_NOTHING) StartWalkStand(pnum);
		else StartStand(pnum, plr[pnum]._pVar3);
		ClearPlrPVars(pnum);
		if (leveltype != 0) ChangeLightOff(plr[pnum]._plid, 0, 0);
		rv = RUN_AGAIN;
	} else {
		PM_ChangeOffset(pnum);
		rv = RUN_DONE;
	}

	return(rv);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int PM_DoWalk3(int pnum)
{
	int		rv;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("PM_DoWalk3: illegal player %d",pnum);

#if 0
	// changed _pAnimFrame == 2 to == 3 to prevent "extra" footstep == pat
	if ((plr[pnum]._pAnimFrame == 3)
		|| (plr[pnum]._pWFrames == 8 && plr[pnum]._pAnimFrame == 7)
		|| (plr[pnum]._pWFrames != 8 && plr[pnum]._pAnimFrame == 4))
		PlaySfxLoc(PS_WALK1, plr[pnum]._px, plr[pnum]._py);
	//TempWalkFix(pnum);
#endif

	if (currlevel == 0 && gbWalkOn)		// double-speed walk in town
	{
		if (plr[pnum]._pAnimFrame%2 == 0) 	// should be even frames only
		{
			++plr[pnum]._pAnimFrame;
			++plr[pnum]._pVar8;
		}
		if (plr[pnum]._pAnimFrame >= plr[pnum]._pWFrames)
			plr[pnum]._pAnimFrame = 0;
	}


	int l = 8;
	
	if (currlevel != 0) l = PlrWalkLenTbl[plr[pnum]._pClass];
	if (plr[pnum]._pVar8 >= l) {
		dPlayer[plr[pnum]._px][plr[pnum]._py] = 0;
		dFlags[plr[pnum]._pVar4][plr[pnum]._pVar5] &= BFMASK_PLRLR;
		plr[pnum]._px = plr[pnum]._pVar1;
		plr[pnum]._py = plr[pnum]._pVar2;
		dPlayer[plr[pnum]._px][plr[pnum]._py] = 1 + (char)pnum;
		if (leveltype != 0) {
			ChangeLightXY(plr[pnum]._plid, plr[pnum]._px, plr[pnum]._py);
			ChangeVisionXY(plr[pnum]._pvid, plr[pnum]._px, plr[pnum]._py);
		}
		if (pnum == myplr) {
			if (ScrollInfo._sdir != SCRL_NONE) {
				ViewX = plr[pnum]._px - ScrollInfo._sdx;
				ViewY = plr[pnum]._py - ScrollInfo._sdy;
			}
		}
		if (plr[pnum].walkpath[0] != PCMD_NOTHING) StartWalkStand(pnum);
		else StartStand(pnum, plr[pnum]._pVar3);
		ClearPlrPVars(pnum);
		if (leveltype != 0) ChangeLightOff(plr[pnum]._plid, 0, 0);
		rv = RUN_AGAIN;
	} else {
		PM_ChangeOffset(pnum);
		rv = RUN_DONE;
	}

	return(rv);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL WeaponDur(int pnum, int durrnd)
{
	if (pnum != myplr)
		return FALSE;
		
	if ((plr[pnum].Hand1Item._itype != -1) && (plr[pnum].Hand1Item._iClass == IC_WEAP)) {
		if (plr[pnum].Hand1Item._iFlags2 & IAF2_DECAY) {
			plr[pnum].Hand1Item._iPLDam -= 5;
			if (plr[pnum].Hand1Item._iPLDam <= -100) {
				NetSendCmdDelItem(TRUE, INVLOC_HAND1);
				plr[pnum].Hand1Item._itype = -1;
				CalcPlrInv(pnum,TRUE);
				return(TRUE);
			}
			CalcPlrInv(pnum,TRUE);
		}
	}
	if ((plr[pnum].Hand2Item._itype != -1) && (plr[pnum].Hand2Item._iClass == IC_WEAP)) {
		if (plr[pnum].Hand2Item._iFlags2 & IAF2_DECAY) {
			plr[pnum].Hand2Item._iPLDam -= 5;
			if (plr[pnum].Hand2Item._iPLDam <= -100) {
				NetSendCmdDelItem(TRUE, INVLOC_HAND1);
				plr[pnum].Hand2Item._itype = -1;
				CalcPlrInv(pnum,TRUE);
				return(TRUE);
			}
			CalcPlrInv(pnum,TRUE);
		}
	}
		
	// Item used, so change durability
	if (random(3, durrnd))
		return FALSE;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("WeaponDur: illegal player %d",pnum);
	
	if ((plr[pnum].Hand1Item._itype != -1) && (plr[pnum].Hand1Item._iClass == IC_WEAP)) {
		if (plr[pnum].Hand1Item._iDurability == INFINITE_DUR) return(FALSE);
		plr[pnum].Hand1Item._iDurability--;
		if (plr[pnum].Hand1Item._iDurability <= 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND1);
			plr[pnum].Hand1Item._itype = -1;
			CalcPlrInv(pnum,TRUE);
			return(TRUE);
		}
	}
	if ((plr[pnum].Hand2Item._itype != -1) && (plr[pnum].Hand2Item._iClass == IC_WEAP)) {
		if (plr[pnum].Hand2Item._iDurability == INFINITE_DUR) return(FALSE);
		plr[pnum].Hand2Item._iDurability--;
		if (plr[pnum].Hand2Item._iDurability == 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND2);
			plr[pnum].Hand2Item._itype = -1;
			CalcPlrInv(pnum,TRUE);
			return(TRUE);
		}
	}
	// Shield only guy
	if ((plr[pnum].Hand1Item._itype == -1) && (plr[pnum].Hand2Item._itype == IT_SHIELD)) {
		if (plr[pnum].Hand2Item._iDurability == INFINITE_DUR) return(FALSE);
		plr[pnum].Hand2Item._iDurability--;
		if (plr[pnum].Hand2Item._iDurability == 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND2);
			plr[pnum].Hand2Item._itype = -1;
			CalcPlrInv(pnum,TRUE);
			return(TRUE);
		}
	}
	if ((plr[pnum].Hand2Item._itype == -1) && (plr[pnum].Hand1Item._itype == IT_SHIELD)) {
		if (plr[pnum].Hand1Item._iDurability == INFINITE_DUR) return(FALSE);
		plr[pnum].Hand1Item._iDurability--;
		if (plr[pnum].Hand1Item._iDurability == 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND1);
			plr[pnum].Hand1Item._itype = -1;
			CalcPlrInv(pnum,TRUE);
			return(TRUE);
		}
	}
	return(FALSE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL PlrHitMonst(int pnum, int m)
{
	int		hit, hper = 0, mind, maxd;
	long	dam, skdam;
	int		phanditype;
	int		tmac;
	BOOL	rv;
	BOOL	ret = FALSE;
	BOOL	quarterdamage = FALSE;

	if ((DWORD)m >= MAXMONSTERS)
		app_fatal("PlrHitMonst: illegal monster %d",m);

	if ((monster[m]._mhitpoints >> HP_SHIFT) <= 0) return(FALSE);

	if(monster[m].MType->mtype == MT_ILLWEAV && monster[m]._mgoal == MG_RUN_AWAY)
		return FALSE;

	if (monster[m]._mmode == MM_MISSILE) return(FALSE);

	if (pnum < 0)
	{
		quarterdamage = TRUE;
		pnum = -pnum;
		if (plr[pnum]._pLevel > 20)
			hper -= 30;
		else
			hper -= 2 * (35 - plr[pnum]._pLevel);
	}
	
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("PlrHitMonst: illegal player %d",pnum);

	rv = FALSE;
	// Did I hit?
	hit = random(4, 100);
	if (monster[m]._mmode == MM_STONE) hit = 0;
	tmac = monster[m].mArmorClass;
	if (plr[pnum]._pIEnAc > 0)		// like Damage Reduction in Hero --DKT
	{
		int i = plr[pnum]._pIEnAc - 1;
		if (i > 0)
			tmac >>= i;				// halved (n-1) times
		else
			tmac -= tmac >> 2;		// three quarters
			
		if (plr[pnum]._pClass == CLASS_BARBARIAN)
		{
			tmac -= (monster[m].mArmorClass)/8;		// Barbarians have a slight advantage using these weapons.
		}
		
		// limit it to a zero.
		if (tmac < 0)
		{
			tmac = 0;
		}
	}
	
	// note: modified this to += so quarterdamage shots are at minus to hit
	//       by giving negative initial value to hper.
	hper += BASE_TO_HIT + plr[pnum]._pLevel - tmac + (plr[pnum]._pDexterity >> 1);
	if (plr[pnum]._pClass == CLASS_WARRIOR)
		hper += 20;
	hper += plr[pnum]._pIBonusToHit;
	if (hper < 5) hper = 5;
	if (hper > 95) hper = 95;
	if (CheckMonsterHit(m, ret))	// ret passed by &reference!!
		return(ret);
#if CHEATS
	else if((hit < hper) || cheatflag || simplecheat) {
#else
	else if (hit < hper) {
#endif
		if ((plr[pnum]._pIFlags & IAF_FIREHIT &&
			plr[pnum]._pIFlags & IAF_LIGHTHIT))
		{
			int dmg = plr[pnum]._pIFMinDam + random(3, (plr[pnum]._pIFMaxDam - plr[pnum]._pIFMinDam));
			AddMissile(plr[pnum]._px, plr[pnum]._py, plr[pnum]._pVar1, plr[pnum]._pVar2, plr[pnum]._pdir, MIT_SPECARROW, MI_ENEMYMONST, pnum, dmg, 0);
		}
		mind = plr[pnum]._pIMinDam;
		maxd = plr[pnum]._pIMaxDam;
		dam = random(5, maxd - mind + 1) + mind;
		dam += (dam * plr[pnum]._pIBonusDam) / 100;
		dam += plr[pnum]._pIBonusDamMod;
		
		int perildam = dam << HP_SHIFT;
		
		dam += plr[pnum]._pDamageMod;
		if (plr[pnum]._pClass == CLASS_WARRIOR
			|| plr[pnum]._pClass == CLASS_BARBARIAN) {
			//ddp = (plr[pnum]._pDexterity + plr[pnum]._pLevel) >> 3;
			int ddp = plr[pnum]._pLevel;
			int doubledam = random(6, 100);
			if (doubledam < ddp) dam = dam << 1;
		}
		phanditype = -1;
		if ((plr[pnum].Hand1Item._itype == IT_SWORD) || (plr[pnum].Hand2Item._itype == IT_SWORD)) phanditype = IT_SWORD;
		if ((plr[pnum].Hand1Item._itype == IT_MACE) || (plr[pnum].Hand2Item._itype == IT_MACE)) phanditype = IT_MACE;
		switch (monster[m].MData->mMonstClass) {
			case MC_UNDEAD:
				if (phanditype == IT_SWORD) dam -= (dam >> 1);
				else if (phanditype == IT_MACE) dam += (dam >> 1);
				break;
			case MC_ANIMAL:
				if (phanditype == IT_MACE) dam -= (dam >> 1);
				else if (phanditype == IT_SWORD) dam += (dam >> 1);
				break;
		}
		if ((plr[pnum]._pIFlags & IAF_DAMDEMON) && (monster[m].MData->mMonstClass == MC_DEMON)) dam += (dam << 1);
		
		if (plr[pnum]._pIFlags2 & IAF2_DEVASTATION)
		{
			int tdp = 5;
			int tripledamage = random(6, 100);
			if (tripledamage < tdp) dam += dam << 1;
		}
		
		if (plr[pnum]._pIFlags2 & IAF2_CLONE)	// doppelganger
		{
			if (monster[m].MType->mtype != MT_DIABLO) 
			{
				if ((monster[m]._uniqtype == 0) && (random(6, 100) < 10))
					CloneMonster(m);
			}
		}
		
		dam = dam << HP_SHIFT;
		
		// after shift, to increase resolution
		if (plr[pnum]._pIFlags2 & IAF2_JESTER)
		{
			int dammult = random(6, 201);
			if (dammult >= 100)
				dammult = 100 + (dammult-100) * 5;
			dam = (dam * dammult) / 100;
		}
		
		if (quarterdamage)
			dam >>= 2;
		
		
		// can only be damaged by me
		if (pnum == myplr) {
			if (plr[pnum]._pIFlags2 & IAF2_PERIL)
			{
				perildam += plr[pnum]._pIGetHit << HP_SHIFT;
				if (perildam < 0)
				{
					// do nothing
				}
				else if (plr[pnum]._pHitPoints > perildam)
				{
					plr[pnum]._pHitPoints -= perildam;
					plr[pnum]._pHPBase -= perildam;
				}
				else
				{
					plr[pnum]._pHPBase -= plr[pnum]._pHitPoints - (1 << HP_SHIFT);
					plr[pnum]._pHitPoints = 1 << HP_SHIFT;
				}
				dam <<= 1;
			}
			monster[m]._mhitpoints -= dam;
		}
		if (plr[pnum]._pIFlags & IAF_SKING) {
			skdam = random(7, dam >> 3);
			plr[pnum]._pHitPoints += skdam;
			if (plr[pnum]._pHitPoints > plr[pnum]._pMaxHP) plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
			plr[pnum]._pHPBase += skdam;
			if (plr[pnum]._pHPBase > plr[pnum]._pMaxHPBase) plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;
			drawhpflag = TRUE;
		}
		if ((plr[pnum]._pIFlags & IAF_ALLBAT) && (!(plr[pnum]._pIFlags & IAF_LMANA))) {
			if (plr[pnum]._pIFlags & IAF_BAT10) skdam = (dam * 3) / 100;
			if (plr[pnum]._pIFlags & IAF_BAT20) skdam = (dam * 5) / 100;
			plr[pnum]._pMana += skdam;
			if (plr[pnum]._pMana > plr[pnum]._pMaxMana) plr[pnum]._pMana = plr[pnum]._pMaxMana;
			plr[pnum]._pManaBase += skdam;
			if (plr[pnum]._pManaBase > plr[pnum]._pMaxManaBase) plr[pnum]._pManaBase = plr[pnum]._pMaxManaBase;
			drawmanaflag = TRUE;
		}
		if (plr[pnum]._pIFlags & IAF_ALLLEECH) {
			if (plr[pnum]._pIFlags & IAF_LEECH10) skdam = (dam * 3) / 100;
			if (plr[pnum]._pIFlags & IAF_LEECH20) skdam = (dam * 5) / 100;
			plr[pnum]._pHitPoints += skdam;
			if (plr[pnum]._pHitPoints > plr[pnum]._pMaxHP) plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
			plr[pnum]._pHPBase += skdam;
			if (plr[pnum]._pHPBase > plr[pnum]._pMaxHPBase) plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;
			drawhpflag = TRUE;
		}
		if (plr[pnum]._pIFlags & IAF_NOHEAL) monster[m]._mFlags |= MFLAG_NOHEAL;
#if CHEATS
		if (simplecheat || cheatflag) monster[m]._mhitpoints = 0;
//			if (cheatflag) monster[m]._mhitpoints = 1 << HP_SHIFT;
#endif
		//rjs - was doing double dam if stone - if (pnum == myplr && monster[m]._mmode == MM_STONE) monster[m]._mhitpoints -= dam;
		if ((monster[m]._mhitpoints >> HP_SHIFT) <= 0) {
			if (monster[m]._mmode == MM_STONE) {
				M_StartKill(m, pnum);
				monster[m]._mmode = MM_STONE;
			} else M_StartKill(m, pnum);
		} else {
			if (monster[m]._mmode == MM_STONE) {
				M_StartHit(m, pnum, dam);
				monster[m]._mmode = MM_STONE;
			} else {
				if (plr[pnum]._pIFlags & IAF_KNOCKBACK) M_GetKnockback(m);
				M_StartHit(m, pnum, dam);
			}
		}
		rv = TRUE;
	}
	return(rv);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL PlrHitPlr(int pnum, char p)
{
	int		hit, hper, mind, maxd;
	int		doubledam, ddp;
	long	dam, skdam;
	int		tac;
	int		blk, blkper, blkdir;
	BOOL	rv;

	if ((DWORD)p >= MAX_PLRS)
		app_fatal("PlrHitPlr: illegal target player %d",p);

	rv = FALSE;
	if (plr[p]._pInvincible) return(rv);
	if ((plr[p]._pSpellFlags & SF_ETHER) != 0) return(rv);

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("PlrHitPlr: illegal attacking player %d",pnum);

	// Did I hit?
	hit = random(4, 100);
	//rjs tac = (byte) plr[p]._pArmorClass + plr[p]._pIAC + plr[p]._pIBonusAC;
	tac = plr[p]._pIAC + plr[p]._pIBonusAC;
	tac += (plr[p]._pDexterity / 5);
	hper = BASE_TO_HIT + plr[pnum]._pLevel - tac + (plr[pnum]._pDexterity >> 1);
	if (plr[pnum]._pClass == CLASS_WARRIOR)
		hper += 20;
	hper += plr[pnum]._pIBonusToHit;
	if (hper < 5) hper = 5;
	if (hper > 95) hper = 95;
	if (((plr[p]._pmode == PM_STAND) || (plr[p]._pmode == PM_ATTACK)) && (plr[p]._pBlockFlag)) blk = random(5, 100);
	else blk = 100;
	blkper = plr[p]._pBaseToBlk + plr[p]._pDexterity - ((plr[pnum]._pLevel - plr[p]._pLevel) << 1);
	if (blkper < 0) blkper = 0;
	if (blkper > 100) blkper = 100;
	if (hit < hper) {
		if (blk < blkper) {
			blkdir = GetDirection(plr[p]._px, plr[p]._py, plr[pnum]._px, plr[pnum]._py);
			StartPlrBlock(p, blkdir);
		} else {
			mind = plr[pnum]._pIMinDam;
			maxd = plr[pnum]._pIMaxDam;
			dam = random(5, maxd - mind + 1) + mind;
			dam += (dam * plr[pnum]._pIBonusDam) / 100;
			dam += plr[pnum]._pIBonusDamMod + plr[pnum]._pDamageMod;
			if (plr[pnum]._pClass == CLASS_WARRIOR
				|| plr[pnum]._pClass == CLASS_BARBARIAN) {
				//ddp = (plr[pnum]._pDexterity + plr[pnum]._pLevel) >> 3;
				ddp = plr[pnum]._pLevel;
				doubledam = random(6, 100);
				if (doubledam < ddp) dam = dam << 1;
			}
			dam = dam << HP_SHIFT;
			if (plr[pnum]._pIFlags & IAF_SKING) {
				skdam = random(7, dam >> 3);
				plr[pnum]._pHitPoints += skdam;
				if (plr[pnum]._pHitPoints > plr[pnum]._pMaxHP) plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
				plr[pnum]._pHPBase += skdam;
				if (plr[pnum]._pHPBase > plr[pnum]._pMaxHPBase) plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;
				drawhpflag = TRUE;
			}
			if (pnum == myplr) NetSendCmdDamage(TRUE, p, dam);
			StartPlrHit(p, dam, FALSE);
		}
		rv = TRUE;
	}
	return(rv);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL PlrHitObj(int pnum, int mx, int my)
{
	int oi;

	if (dObject[mx][my] > 0) oi = dObject[mx][my] - 1;
	else oi = -(dObject[mx][my] + 1);
	if (object[oi]._oBreak == OBJ_BREAKABLE) {
		BreakObject(pnum, oi);
		return(TRUE);
	} else return(FALSE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int PM_DoAttack(int pnum)
{
	int dx,dy,m;
	char p;
	BOOL didhit = FALSE;
	int frame;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("PM_DoAttack: illegal player %d",pnum);

	frame = plr[pnum]._pAnimFrame;

	if ((plr[pnum]._pIFlags & IAF_ATANIM1) && (frame == 1))
		plr[pnum]._pAnimFrame++;

	if ((plr[pnum]._pIFlags & IAF_ATANIM2) && (frame == 1 || frame == 3))
		plr[pnum]._pAnimFrame++;

	if ((plr[pnum]._pIFlags & IAF_ATANIM3) && (frame == 1 || frame == 3 || frame == 5))
		plr[pnum]._pAnimFrame++;

	if ((plr[pnum]._pIFlags & IAF_ATANIM4) && (frame == 1 || frame == 4))
		plr[pnum]._pAnimFrame += 2;

	if (plr[pnum]._pAnimFrame == (plr[pnum]._pAFNum-1))
		PlaySfxLoc(PS_SWING, plr[pnum]._px, plr[pnum]._py);

	if (plr[pnum]._pAnimFrame == plr[pnum]._pAFNum) {
		dx = plr[pnum]._px + offset_x[plr[pnum]._pdir];
		dy = plr[pnum]._py + offset_y[plr[pnum]._pdir];

		if (dMonster[dx][dy] != 0) {
			if (dMonster[dx][dy] > 0) m = dMonster[dx][dy] - 1;
			else m = -(dMonster[dx][dy] + 1);
			if (CanTalkToMonst(m)) {
				plr[pnum]._pVar1 = 0;
				return(RUN_DONE);
			}
		}

		if ((plr[pnum]._pIFlags & IAF_FIREHIT &&
			plr[pnum]._pIFlags & IAF_LIGHTHIT))
		{
		}
		else if (plr[pnum]._pIFlags & IAF_FIREHIT) 
			AddMissile(dx, dy, 1, 0, 0, MIT_WEAPEXP, MI_ENEMYMONST, pnum, 0, 0);
		else if (plr[pnum]._pIFlags & IAF_LIGHTHIT) 
			AddMissile(dx, dy, 2, 0, 0, MIT_WEAPEXP, MI_ENEMYMONST, pnum, 0, 0);

		if (dMonster[dx][dy] != 0) {
			if (dMonster[dx][dy] > 0)
				m = dMonster[dx][dy] - 1;
			else
				m = -(dMonster[dx][dy] + 1);
			didhit = PlrHitMonst(pnum, m);
		} else {
			if ((dPlayer[dx][dy] != 0) && (!FriendlyMode)) {
				if (dPlayer[dx][dy] > 0) p = dPlayer[dx][dy] - 1;
				else p = -(dPlayer[dx][dy] + 1);
				didhit = PlrHitPlr(pnum, p);
			} else {
				if (dObject[dx][dy] > 0) didhit = PlrHitObj(pnum,dx,dy);
			}
		}
		
		if ((plr[pnum]._pClass == CLASS_MONK &&
			 (plr[pnum].Hand1Item._itype == IT_STAFF ||
			  plr[pnum].Hand2Item._itype == IT_STAFF)
			 )
			|| (plr[pnum]._pClass == CLASS_BARD &&
			    (plr[pnum].Hand1Item._itype == IT_SWORD &&
			     plr[pnum].Hand2Item._itype == IT_SWORD)
			  )
			// Axes, 2handed Maces or 2handed swords with no shield used.
			|| (plr[pnum]._pClass == CLASS_BARBARIAN &&
			    (plr[pnum].Hand1Item._itype == IT_AXE ||
			     plr[pnum].Hand2Item._itype == IT_AXE ||
			     ((((plr[pnum].Hand1Item._itype == IT_MACE && plr[pnum].Hand1Item._iLoc == IL_2HAND) ||
			        (plr[pnum].Hand2Item._itype == IT_MACE && plr[pnum].Hand2Item._iLoc == IL_2HAND) ||
			        (plr[pnum].Hand1Item._itype == IT_SWORD && plr[pnum].Hand1Item._iLoc == IL_2HAND) ||
			        (plr[pnum].Hand2Item._itype == IT_SWORD && plr[pnum].Hand2Item._iLoc == IL_2HAND)
				   )
			        && !(plr[pnum].Hand1Item._itype == IT_SHIELD || plr[pnum].Hand2Item._itype == IT_SHIELD)
			       )
			     )
				)
			  )
			)
		{
			// check right-hand opponent, then left
			// note that we temporarily redeclare dx, dy, and m here
			int dx = plr[pnum]._px + offset_x[(plr[pnum]._pdir+1)%8];
			int dy = plr[pnum]._py + offset_y[(plr[pnum]._pdir+1)%8];
			int m = ((dMonster[dx][dy] > 0) ?
						dMonster[dx][dy] : -dMonster[dx][dy])
					- 1;
			if (dMonster[dx][dy] != 0 && !CanTalkToMonst(m) &&
				monster[m]._moldx == dx && monster[m]._moldy == dy)
			{
				if (PlrHitMonst(-pnum, m))
					didhit = TRUE;
			}
			dx = plr[pnum]._px + offset_x[(plr[pnum]._pdir+7)%8];
			dy = plr[pnum]._py + offset_y[(plr[pnum]._pdir+7)%8];
			m = ((dMonster[dx][dy] > 0) ?
						dMonster[dx][dy] : -dMonster[dx][dy])
					- 1;
			if (dMonster[dx][dy] != 0 && !CanTalkToMonst(m) &&
				monster[m]._moldx == dx && monster[m]._moldy == dy)
			{
				if (PlrHitMonst(-pnum, m))
					didhit = TRUE;
			}
		}
		if (didhit) {
			if (WeaponDur(pnum, 30)) {		// 1 in 30 dur hit if plr hit monst/obj
				StartStand(pnum, plr[pnum]._pdir);
				ClearPlrPVars(pnum);
				return(RUN_AGAIN);
			}
		}
	}
	if (plr[pnum]._pAnimFrame == plr[pnum]._pAFrames) {
		StartStand(pnum, plr[pnum]._pdir);
		ClearPlrPVars(pnum);
		return(RUN_AGAIN);
	} else return(RUN_DONE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int PM_DoRangeAttack(int pnum)
{
	int mistype;
	int numshots = 0;
	int shottype = 0;
	int deltaX = 0, deltaY = 0;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("PM_DoRangeAttack: illegal player %d",pnum);

	if (plr[pnum]._pAnimFrame == plr[pnum]._pAFNum)
		numshots = 1;
		
	if ((plr[pnum]._pIFlags & IAF_RABID) &&
		(plr[pnum]._pAnimFrame == plr[pnum]._pAFNum + 2))
	{
		numshots = 2;
		shottype = 1; 	// framing shots
	}
	
	for (int shotnum = 0; shotnum < numshots; shotnum++)
	{
		switch(shottype)
		{
		case 0: break;
		case 1:
			{
				int fanoffset = (shotnum == 0) ? -1 : 1;
				int xoffset = plr[pnum]._pVar1 - plr[pnum]._px;
				int yoffset = plr[pnum]._pVar2 - plr[pnum]._py;
				if (xoffset < 0)
					deltaY = fanoffset;
				if (xoffset > 0)
					deltaY = -fanoffset;
				if (yoffset < 0)
					deltaX = -fanoffset;
				if (yoffset > 0)
					deltaX = fanoffset;
			}
			break;
		default:
			break;	
		}
	
		mistype = MIT_ARROW;
		if (plr[pnum]._pIFlags & IAF_FIREARROW) mistype = MIT_FARROW;
		if (plr[pnum]._pIFlags & IAF_LARROW) mistype = MIT_LARROW;
		
		// both flags means a special arrow, doing (firedamage) damage.
		// a bit of a hack.  --donald
		if ((plr[pnum]._pIFlags & IAF_FIREARROW) &&
			(plr[pnum]._pIFlags & IAF_LARROW))
		{
			int dmg = plr[pnum]._pIFMinDam + random(3, (plr[pnum]._pIFMaxDam - plr[pnum]._pIFMinDam));
			mistype = MIT_SPECARROW;
			
			AddMissile(plr[pnum]._px, plr[pnum]._py, plr[pnum]._pVar1 + deltaX, plr[pnum]._pVar2 + deltaY, plr[pnum]._pdir, mistype, MI_ENEMYMONST, pnum, dmg, 0);
		}
		else
		{
			AddMissile(plr[pnum]._px, plr[pnum]._py, plr[pnum]._pVar1 + deltaX, plr[pnum]._pVar2 + deltaY, plr[pnum]._pdir, mistype, MI_ENEMYMONST, pnum, 4, 0);
			if (shotnum == 0)
			{
				if (shottype == 0)
					PlaySfxLoc(PS_BFIRE, plr[pnum]._px, plr[pnum]._py);
				else
					PlaySfxLoc(PS_NEW_BFIRE, plr[pnum]._px, plr[pnum]._py);
			}
		}
		
		if (WeaponDur(pnum, 40)) {		// 1 in 40 bow dur hit
			StartStand(pnum, plr[pnum]._pdir);
			ClearPlrPVars(pnum);
			return(RUN_AGAIN);
		}
	}
	if (plr[pnum]._pAnimFrame >= plr[pnum]._pAFrames) {
		StartStand(pnum, plr[pnum]._pdir);
		ClearPlrPVars(pnum);
		return(RUN_AGAIN);
	} else return(RUN_DONE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void ShieldDur(int pnum)
{
	if (pnum != myplr) return;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("ShieldDur: illegal player %d",pnum);

	// Item used, so change durability
	if (plr[pnum].Hand1Item._itype == IT_SHIELD) {
		if (plr[pnum].Hand1Item._iDurability == INFINITE_DUR) return;
		plr[pnum].Hand1Item._iDurability--;
		if (plr[pnum].Hand1Item._iDurability == 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND1);
			plr[pnum].Hand1Item._itype = -1;
			CalcPlrInv(pnum,TRUE);
		}
	}
	if (plr[pnum].Hand2Item._itype == IT_SHIELD) {
		if (plr[pnum].Hand2Item._iDurability == INFINITE_DUR) return;
		plr[pnum].Hand2Item._iDurability--;
		if (plr[pnum].Hand2Item._iDurability == 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND2);
			plr[pnum].Hand2Item._itype = -1;
			CalcPlrInv(pnum,TRUE);
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int PM_DoBlock(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("PM_DoBlock: illegal player %d",pnum);
	
	if ((plr[pnum]._pIFlags & IAF_BLANIM) && (plr[pnum]._pAnimFrame != 1)) {
		plr[pnum]._pAnimFrame  = plr[pnum]._pBFrames;
	}
	if (plr[pnum]._pAnimFrame >= plr[pnum]._pBFrames) {
		StartStand(pnum, plr[pnum]._pdir);
		ClearPlrPVars(pnum);
		if (random(3,10) == 0) ShieldDur(pnum);
		return(RUN_AGAIN);
	} else return(RUN_DONE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int PM_DoSpell(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("PM_DoSpell: illegal player %d",pnum);

	if (plr[pnum]._pVar8 == plr[pnum]._pSFNum) {
		CastSpell(pnum, plr[pnum]._pSpell, plr[pnum]._px, plr[pnum]._py, plr[pnum]._pVar1, plr[pnum]._pVar2, MI_PLR, plr[pnum]._pVar4);
		if (plr[pnum]._pSplFrom == SPL_FROMR) {
			if (plr[pnum]._pRSplType == SPT_SCROLL) {
				if ((plr[pnum]._pScrlSpells & (((__int64)1) << (plr[pnum]._pRSpell-1))) == 0) {
					plr[pnum]._pRSpell = -1;
					plr[pnum]._pRSplType = SPT_NONE;
					force_redraw = FULLDRAW;
				}
			}
			if (plr[pnum]._pRSplType == SPT_ITEM) {
				if (((plr[pnum]._pISpells & (((__int64)1) << (plr[pnum]._pRSpell-1)))) == 0) {
					plr[pnum]._pRSpell = -1;
					plr[pnum]._pRSplType = SPT_NONE;
					force_redraw = FULLDRAW;
				}
			}
		}
	}
	plr[pnum]._pVar8++;
	if (leveltype == 0) {
		if (plr[pnum]._pVar8 > plr[pnum]._pSFrames) {
			StartWalkStand(pnum);
			ClearPlrPVars(pnum);
			return(RUN_AGAIN);
		} else return(RUN_DONE);
	} else {
		if (plr[pnum]._pAnimFrame == plr[pnum]._pSFrames) {
			StartStand(pnum, plr[pnum]._pdir);
			ClearPlrPVars(pnum);
			return(RUN_AGAIN);
		} else return(RUN_DONE);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void ArmorDur(int pnum) {
	if (pnum != myplr) return;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("ArmorDur: illegal player %d",pnum);

	PlayerStruct * p = &plr[pnum];
	if (p->BodyItem._itype == -1 && p->HeadItem._itype == -1) return;

	// Head or body hit?
	int a = random(8, 3);
	if (p->BodyItem._itype != -1 && p->HeadItem._itype == -1) a = 1;
	if (p->BodyItem._itype == -1 && p->HeadItem._itype != -1) a = 0;

	ItemStruct * pi = a ? &p->BodyItem : &p->HeadItem;
	if (pi->_iDurability == INFINITE_DUR) return;

	// lose some durability
	pi->_iDurability--;
	if (pi->_iDurability == 0) {
		if (a) NetSendCmdDelItem(TRUE, INVLOC_BODY);
		else NetSendCmdDelItem(TRUE, INVLOC_HEAD);
		pi->_itype = -1;
		CalcPlrInv(pnum,TRUE);
	}
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
int PM_DoGotHit(int pnum)
{
	int		rv;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("PM_DoGotHit: illegal player %d",pnum);

	if (plr[pnum]._pIFlags & IAF_ALLHTANIM)
	{
		int htanimframes = 3;
		if (plr[pnum]._pIFlags & IAF_HTANIM2)
			htanimframes = 4;
		if (plr[pnum]._pIFlags & IAF_HTANIM3)
		    htanimframes = 5;
		    
		if (plr[pnum]._pVar8 > 1 && plr[pnum]._pVar8 < htanimframes)
			plr[pnum]._pVar8 = htanimframes;
		
		// make sure we didn't go too far
		if (plr[pnum]._pVar8 > plr[pnum]._pHFrames)
			plr[pnum]._pVar8 = plr[pnum]._pHFrames;
	}
	if (plr[pnum]._pVar8 == plr[pnum]._pHFrames) {
		StartStand(pnum, plr[pnum]._pdir);
		ClearPlrPVars(pnum);
		if (random(3,4)) ArmorDur(pnum);
		rv = RUN_AGAIN;
	} else {
		plr[pnum]._pVar8++;
		rv = RUN_DONE;
	}

	return(rv);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int PM_DoDeath(int pnum)
{
	int		rv;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("PM_DoDeath: illegal player %d",pnum);

	// Hold last frame
	if (plr[pnum]._pVar8 >= (plr[pnum]._pDFrames << 1)) {
		if ((deathdelay > 1) && (pnum == myplr)) {
			deathdelay--;
			if (deathdelay == 1) {
				deathflag = TRUE;
				if (gbMaxPlayers == 1) gamemenu_on();
			}
		}
		plr[pnum]._pAnimDelay = 10000;
		plr[pnum]._pAnimFrame = plr[pnum]._pAnimLen;
		dFlags[plr[pnum]._px][plr[pnum]._py] |= BFLAG_DEADPLR;
	}
	if (plr[pnum]._pVar8 < 100) plr[pnum]._pVar8++;
	rv = RUN_DONE;

	return(rv);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int PM_DoNewLvl(int pnum)
{
	return(RUN_DONE);
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void CheckNewPath(int pnum)
{
	int i, dx, dy, d, oi;
	int v1,v2,v3;

	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("CheckNewPath: illegal player %d",pnum);

	if (plr[pnum].destAction == PCMD_ATTACKID) {
		i = plr[pnum].destParam1;
		MakePlrPath(pnum, monster[i]._mfutx, monster[i]._mfuty, FALSE);
	}
	if (plr[pnum].destAction == PCMD_ATTACKPID) {
		i = plr[pnum].destParam1;
		MakePlrPath(pnum, plr[i]._pfutx, plr[i]._pfuty, FALSE);
	}
	if (plr[pnum].walkpath[0] != PCMD_NOTHING) {
		if (plr[pnum]._pmode == PM_STAND) {
			// Special case for attack
			if (pnum == myplr) {
				if ((plr[pnum].destAction == PCMD_ATTACKID) || (plr[pnum].destAction == PCMD_ATTACKPID)) {
					i = plr[pnum].destParam1;
					if (plr[pnum].destAction == PCMD_ATTACKID) {
						dx = abs(plr[pnum]._pfutx - monster[i]._mfutx);
						dy = abs(plr[pnum]._pfuty - monster[i]._mfuty);
						d = GetDirection(plr[pnum]._pfutx, plr[pnum]._pfuty, monster[i]._mfutx, monster[i]._mfuty);
					} else {
						dx = abs(plr[pnum]._pfutx - plr[i]._pfutx);
						dy = abs(plr[pnum]._pfuty - plr[i]._pfuty);
						d = GetDirection(plr[pnum]._pfutx, plr[pnum]._pfuty, plr[i]._pfutx, plr[i]._pfuty);
					}
					if ((dx < 2) && (dy < 2)) {
						ClrPlrPath(pnum);
						if ((monster[i].mtalkmsg !=0) && (monster[i].mtalkmsg != TXT_VB2)) TalktoMonster(i);
						else StartAttack(pnum, d);
						plr[pnum].destAction = PCMD_NOTHING;
					}
				}
			}
			if (currlevel != 0) {
				v1 = PlrWalkTbl[plr[pnum]._pClass][0];
				v2 = PlrWalkTbl[plr[pnum]._pClass][1];
				v3 = PlrWalkTbl[plr[pnum]._pClass][2];
			} else {
				v1 = 2048;
				v2 = 1024;
				v3 = 512;
			}
			switch(plr[pnum].walkpath[0]) {
				case PCMD_WALKU :
					StartWalk(pnum, 0, -v2, -1, -1, DIR_U, SCRL_U);
					break;
				case PCMD_WALKUR :
					StartWalk(pnum, v2, -v3, 0, -1, DIR_UR, SCRL_UR);
					break;
				case PCMD_WALKR :
					StartWalk3(pnum, v1, 0, -32, -16, 1, -1, 1, 0, DIR_R, SCRL_R);
					break;
				case PCMD_WALKDR :
					StartWalk2(pnum, v2, v3, -32, -16, 1, 0, DIR_DR, SCRL_DR);
					break;
				case PCMD_WALKD :
					StartWalk2(pnum, 0, v2, 0, -32, 1, 1, DIR_D, SCRL_D);
					break;
				case PCMD_WALKDL :
					StartWalk2(pnum, -v2, v3, 32, -16, 0, 1, DIR_DL, SCRL_DL);
					break;
				case PCMD_WALKL :
					StartWalk3(pnum, -v1, 0, 32, -16, -1, 1, 0, 1, DIR_L, SCRL_L);
					break;
				case PCMD_WALKUL :
					StartWalk(pnum, -v2, -v3, -1 ,0, DIR_UL, SCRL_UL);
					break;
			}
			for (i = 1; i < MAXPATHLEN; i++) plr[pnum].walkpath[i-1] = plr[pnum].walkpath[i];
			plr[pnum].walkpath[MAXPATHLEN-1] = PCMD_NOTHING;
			// if walk fails then start standing
			if (plr[pnum]._pmode == PM_STAND) {
				StartStand(pnum, plr[pnum]._pdir);
				plr[pnum].destAction = PCMD_NOTHING;
			}
		}
	} else {
		if (plr[pnum].destAction != PCMD_NOTHING) {
			if (plr[pnum]._pmode == PM_STAND) {
				switch (plr[pnum].destAction) {
					case PCMD_ATTACK :
						d = GetDirection(plr[pnum]._px, plr[pnum]._py, plr[pnum].destParam1, plr[pnum].destParam2);
						StartAttack(pnum, d);
						break;
					case PCMD_ATTACKID :
						i = plr[pnum].destParam1;
						dx = abs(plr[pnum]._px - monster[i]._mfutx);
						dy = abs(plr[pnum]._py - monster[i]._mfuty);
						if ((dx <= 1) && (dy <= 1)) {
							d = GetDirection(plr[pnum]._pfutx, plr[pnum]._pfuty, monster[i]._mfutx, monster[i]._mfuty);
							if ((monster[i].mtalkmsg !=0) && (monster[i].mtalkmsg != TXT_VB2)) TalktoMonster(i);
							else StartAttack(pnum, d);
						}
						break;
					case PCMD_ATTACKPID:
						i = plr[pnum].destParam1;
						dx = abs(plr[pnum]._px - plr[i]._pfutx);
						dy = abs(plr[pnum]._py - plr[i]._pfuty);
						if ((dx <= 1) && (dy <= 1)) {
							d = GetDirection(plr[pnum]._pfutx, plr[pnum]._pfuty, plr[i]._pfutx, plr[i]._pfuty);
							StartAttack(pnum, d);
						}
						break;
					case PCMD_RATTACK:
						d = GetDirection(plr[pnum]._px, plr[pnum]._py, plr[pnum].destParam1, plr[pnum].destParam2);
						StartRangeAttack(pnum, d, plr[pnum].destParam1, plr[pnum].destParam2);
						break;
					case PCMD_RATTACKID:
						i = plr[pnum].destParam1;
						d = GetDirection(plr[pnum]._pfutx, plr[pnum]._pfuty, monster[i]._mfutx, monster[i]._mfuty);
						if ((monster[i].mtalkmsg !=0) && (monster[i].mtalkmsg != TXT_VB2)) TalktoMonster(i);
						else StartRangeAttack(pnum, d, monster[i]._mfutx, monster[i]._mfuty);
						break;
					case PCMD_RATTACKPID:
						i = plr[pnum].destParam1;
						d = GetDirection(plr[pnum]._pfutx, plr[pnum]._pfuty, plr[i]._pfutx, plr[i]._pfuty);
						StartRangeAttack(pnum, d, plr[i]._pfutx, plr[i]._pfuty);
						break;
					case PCMD_SPELL:
						d = GetDirection(plr[pnum]._px, plr[pnum]._py, plr[pnum].destParam1, plr[pnum].destParam2);
						StartSpell(pnum, d, plr[pnum].destParam1, plr[pnum].destParam2);
						plr[pnum]._pVar4 = plr[pnum].destParam3;
						break;
					case PCMD_SPELLXYD:
						StartSpell(pnum, plr[pnum].destParam3, plr[pnum].destParam1, plr[pnum].destParam2);
						plr[pnum]._pVar3 = plr[pnum].destParam3;
						plr[pnum]._pVar4 = plr[pnum].destParam4;
						break;
					case PCMD_SPELLID:
						i = plr[pnum].destParam1;
						d = GetDirection(plr[pnum]._px, plr[pnum]._py, monster[i]._mfutx, monster[i]._mfuty);
						StartSpell(pnum, d, monster[i]._mfutx, monster[i]._mfuty);
						plr[pnum]._pVar4 = plr[pnum].destParam2;
						break;
					case PCMD_SPELLPID:
						i = plr[pnum].destParam1;
						d = GetDirection(plr[pnum]._px, plr[pnum]._py, plr[i]._pfutx, plr[i]._pfuty);
						StartSpell(pnum, d, plr[i]._pfutx, plr[i]._pfuty);
						plr[pnum]._pVar4 = plr[pnum].destParam2;
						break;
					case PCMD_OPOBJ:
						oi = plr[pnum].destParam1;
						dx = abs(plr[pnum]._px - object[oi]._ox);
						dy = abs(plr[pnum]._py - object[oi]._oy);
						if ((dy > 1) && (dObject[object[oi]._ox][object[oi]._oy-1] == (-1 - oi)))
							dy = abs(plr[pnum]._py - object[oi]._oy + 1);

						if ((dx <= 1) && (dy <= 1)) {
							if (object[oi]._oBreak == OBJ_BREAKABLE) {
								d = GetDirection(plr[pnum]._px, plr[pnum]._py, object[oi]._ox, object[oi]._oy);
								StartAttack(pnum, d);
							} else OperateObject(pnum, oi, FALSE);
						}
						break;
					case PCMD_DISARM:
						oi = plr[pnum].destParam1;
						dx = abs(plr[pnum]._px - object[oi]._ox);
						dy = abs(plr[pnum]._py - object[oi]._oy);
						if ((dy > 1) && (dObject[object[oi]._ox][object[oi]._oy-1] == (-1 - oi)))
							dy = abs(plr[pnum]._py - object[oi]._oy + 1);
						if ((dx <= 1) && (dy <= 1)) {
							if (object[oi]._oBreak == OBJ_BREAKABLE) {
								d = GetDirection(plr[pnum]._px, plr[pnum]._py, object[oi]._ox, object[oi]._oy);
								StartAttack(pnum, d);
							} else {
								TryDisarm(pnum, oi);
								OperateObject(pnum, oi, FALSE);
							}
						}
						break;
					case PCMD_TELEK:
						oi = plr[pnum].destParam1;
						if (object[oi]._oBreak != OBJ_BREAKABLE) OperateObject(pnum, oi, TRUE);
						break;
					case PCMD_REQGETITEM :
						if (pnum == myplr) {
							i = plr[pnum].destParam1;
							dx = abs(plr[pnum]._px - item[i]._ix);
							dy = abs(plr[pnum]._py - item[i]._iy);
							// PATCH1.JMM
							//if ((dx <= 1) && (dy <= 1) && (curs == GLOVE_CURS) && (!item[i]._iRequest) && (!item[i]._iDelFlag)) {
							if ((dx <= 1) && (dy <= 1) && (curs == GLOVE_CURS) && (!item[i]._iRequest)) {
							// ENDPATCH1.JMM
								NetSendCmdGItem(TRUE, CMD_REQUESTGITEM, myplr, myplr, i);
								item[i]._iRequest = TRUE;
							}
						}
						break;
					case PCMD_REQAGETITEM :
						if (pnum == myplr) {
							i = plr[pnum].destParam1;
							dx = abs(plr[pnum]._px - item[i]._ix);
							dy = abs(plr[pnum]._py - item[i]._iy);
							if ((dx <= 1) && (dy <= 1) && (curs == GLOVE_CURS))
								NetSendCmdGItem(TRUE, CMD_REQUESTAGITEM, myplr, myplr, i);
						}
						break;
					case PCMD_TALK:
						if (pnum == myplr) TalkToTowner(pnum, plr[pnum].destParam1);
						break;
				}
				FixPlayerLocation(pnum,plr[pnum]._pdir);
				plr[pnum].destAction = PCMD_NOTHING;
			} else {
				if ((plr[pnum]._pmode == PM_ATTACK) && (plr[pnum]._pAnimFrame > plr[myplr]._pAFNum)) {
					if (plr[pnum].destAction == PCMD_ATTACK) {
						d = GetDirection(plr[pnum]._pfutx, plr[pnum]._pfuty, plr[pnum].destParam1, plr[pnum].destParam2);
						StartAttack(pnum, d);
						plr[pnum].destAction = PCMD_NOTHING;
					}
					else if (plr[pnum].destAction == PCMD_ATTACKID) {
						i = plr[pnum].destParam1;
						dx = abs(plr[pnum]._px - monster[i]._mfutx);
						dy = abs(plr[pnum]._py - monster[i]._mfuty);
						if ((dx <= 1) && (dy <= 1)) {
							d = GetDirection(plr[pnum]._pfutx, plr[pnum]._pfuty, monster[i]._mfutx, monster[i]._mfuty);
							StartAttack(pnum, d);
						}
						plr[pnum].destAction = PCMD_NOTHING;
					}
					else if (plr[pnum].destAction == PCMD_ATTACKPID) {
						i = plr[pnum].destParam1;
						dx = abs(plr[pnum]._px - plr[i]._pfutx);
						dy = abs(plr[pnum]._py - plr[i]._pfuty);
						if ((dx <= 1) && (dy <= 1)) {
							d = GetDirection(plr[pnum]._pfutx, plr[pnum]._pfuty, plr[i]._pfutx, plr[i]._pfuty);
							StartAttack(pnum, d);
						}
						plr[pnum].destAction = PCMD_NOTHING;
					}
					else if (plr[pnum].destAction == PCMD_OPOBJ)
					{
						oi = plr[pnum].destParam1;
						dx = abs(plr[pnum]._px - object[oi]._ox);
						dy = abs(plr[pnum]._py - object[oi]._oy);
						if ((dy > 1) && (dObject[object[oi]._ox][object[oi]._oy-1] == (-1 - oi)))
							dy = abs(plr[pnum]._py - object[oi]._oy + 1);
						if ((dx <= 1) && (dy <= 1)) {
							if (object[oi]._oBreak == OBJ_BREAKABLE) {
								d = GetDirection(plr[pnum]._px, plr[pnum]._py, object[oi]._ox, object[oi]._oy);
								StartAttack(pnum, d);
							} else OperateObject(pnum, oi, FALSE);
						}
					}
				}
				if ((plr[pnum]._pmode == PM_RATTACK) && (plr[pnum]._pAnimFrame > plr[myplr]._pAFNum)) {
					if (plr[pnum].destAction == PCMD_RATTACK) {
						d = GetDirection(plr[pnum]._px, plr[pnum]._py, plr[pnum].destParam1, plr[pnum].destParam2);
						StartRangeAttack(pnum, d, plr[pnum].destParam1, plr[pnum].destParam2);
						plr[pnum].destAction = PCMD_NOTHING;
					}
					else if (plr[pnum].destAction == PCMD_RATTACKID) {
						i = plr[pnum].destParam1;
						d = GetDirection(plr[pnum]._px, plr[pnum]._py, monster[i]._mfutx, monster[i]._mfuty);
						StartRangeAttack(pnum, d, monster[i]._mfutx, monster[i]._mfuty);
						plr[pnum].destAction = PCMD_NOTHING;
					}
					else if (plr[pnum].destAction == PCMD_RATTACKPID) {
						i = plr[pnum].destParam1;
						d = GetDirection(plr[pnum]._px, plr[pnum]._py, plr[i]._pfutx, plr[i]._pfuty);
						StartRangeAttack(pnum, d, plr[i]._pfutx, plr[i]._pfuty);
						plr[pnum].destAction = PCMD_NOTHING;
					}
				}
				if ((plr[pnum]._pmode == PM_SPELL) && (plr[pnum]._pAnimFrame > plr[pnum]._pSFNum)) {
					if (plr[pnum].destAction == PCMD_SPELL) {
						d = GetDirection(plr[pnum]._px, plr[pnum]._py, plr[pnum].destParam1, plr[pnum].destParam2);
						StartSpell(pnum, d, plr[pnum].destParam1, plr[pnum].destParam2);
						plr[pnum].destAction = PCMD_NOTHING;
					}
					else if (plr[pnum].destAction == PCMD_SPELLID) {
						i = plr[pnum].destParam1;
						d = GetDirection(plr[pnum]._px, plr[pnum]._py, monster[i]._mfutx, monster[i]._mfuty);
						StartSpell(pnum, d, monster[i]._mfutx, monster[i]._mfuty);
						plr[pnum].destAction = PCMD_NOTHING;
					}
					else if (plr[pnum].destAction == PCMD_SPELLPID) {
						i = plr[pnum].destParam1;
						d = GetDirection(plr[pnum]._px, plr[pnum]._py, plr[i]._pfutx, plr[i]._pfuty);
						StartSpell(pnum, d, plr[i]._pfutx, plr[i]._pfuty);
						plr[pnum].destAction = PCMD_NOTHING;
					}
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL PlrDeathModeOK(int p)
{
	if (p != myplr) return(TRUE);
	if ((DWORD)p >= MAX_PLRS)
		app_fatal("PlrDeathModeOK: illegal player %d",p);
	if (plr[p]._pmode == PM_DEATH) return(TRUE);
	if (plr[p]._pmode == PM_QUIT) return(TRUE);
	if (plr[p]._pmode == PM_NEWLVL) return(TRUE);
	return(FALSE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void ValidatePlayer()
{
	int i, gt, pc;
	//jam.patch1.start.1/23/97
	//__int64 msk;
	__int64 msk = 0;
	//jam.patch1.end.1/23/97
	__int64 b = 1;

	if ((DWORD)myplr >= MAX_PLRS)
		app_fatal("ValidatePlayer: illegal player %d",myplr);
	if (plr[myplr]._pLevel > 50) plr[myplr]._pLevel = 50;
	if (plr[myplr]._pExperience > plr[myplr]._pNextExper)
		plr[myplr]._pExperience = plr[myplr]._pNextExper;
	gt = 0;
	for (i = 0; i < plr[myplr]._pNumInv; i++) {
		if (plr[myplr].InvList[i]._itype == IT_GOLD) {
			if (plr[myplr].InvList[i]._ivalue > GOLD_DOUBLE_VMAX) plr[myplr].InvList[i]._ivalue = GOLD_DOUBLE_VMAX;
			gt += plr[myplr].InvList[i]._ivalue;
		}
	}
	if (gt != plr[myplr]._pGold) plr[myplr]._pGold = gt;
	pc = plr[myplr]._pClass;
	if (plr[myplr]._pBaseStr > MaxStats[pc][0]) plr[myplr]._pBaseStr = MaxStats[pc][0];
	if (plr[myplr]._pBaseMag > MaxStats[pc][1]) plr[myplr]._pBaseMag = MaxStats[pc][1];
	if (plr[myplr]._pBaseDex > MaxStats[pc][2]) plr[myplr]._pBaseDex = MaxStats[pc][2];
	if (plr[myplr]._pBaseVit > MaxStats[pc][3]) plr[myplr]._pBaseVit = MaxStats[pc][3];

	for (i = SPL_FIREBOLT; i < SPL_LAST; i++) {
		if (spelldata[i].sBookLvl != -1) {
			msk |= (b << (i-1));
			if (plr[myplr]._pSplLvl[i] > SPELLCAP) plr[myplr]._pSplLvl[i] = SPELLCAP;
		}
	}
	plr[myplr]._pMemSpells &= msk;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*

int pdoppely = DIRTEDGED2;

void PlayerDoppel()
{
	int pdoppelx, pidx;
	PlayerStruct *p;
	BOOL forceclear;

	if (gbMaxPlayers == 1) return;

	for (pdoppelx = DIRTEDGED2; pdoppelx < (DIRTEDGED2+80); pdoppelx++) {
		if (dPlayer[pdoppelx][pdoppely]) {
			if (dPlayer[pdoppelx][pdoppely] > 0)
				pidx = dPlayer[pdoppelx][pdoppely] - 1;
			else
				pidx = -(dPlayer[pdoppelx][pdoppely] + 1);
			p = &plr[pidx];
			forceclear = FALSE;
			if (p->plrlevel != currlevel) forceclear = TRUE;
			if (!p->plractive) forceclear = TRUE;
			if (!forceclear) {
				if ((p->_pmode >= PM_WALK) && (p->_pmode <= PM_WALK3)) continue;
				if (p->_pmode >= PM_NEWLVL) continue;
				if ((p->_px == pdoppelx) && (p->_py == pdoppely)) continue;
				forceclear = TRUE;
			}
			if (forceclear) {
				dFlags[pdoppelx+1][pdoppely+0] &= BFMASK_PLRLR;
				dFlags[pdoppelx+0][pdoppely+1] &= BFMASK_PLRLR;
				dPlayer[pdoppelx][pdoppely] = 0;
			}
		}
	}
	pdoppely++;
	if (pdoppely == DIRTEDGED2+80) pdoppely = DIRTEDGED2;
}
// PATCH2.JMM.3/5/97
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#define MAX_STAT 750
#define MAX_HP (2000<<HP_SHIFT)
#define MAX_MANA (2000<<MANA_SHIFT)
static void CheckCheatStats( int pnum ) {
	if ( plr[pnum]._pStrength > MAX_STAT )
		plr[pnum]._pStrength = MAX_STAT;

	if ( plr[pnum]._pDexterity > MAX_STAT )
		plr[pnum]._pDexterity = MAX_STAT;

	if ( plr[pnum]._pMagic > MAX_STAT )
		plr[pnum]._pMagic = MAX_STAT;

	if ( plr[pnum]._pVitality > MAX_STAT )
		plr[pnum]._pVitality = MAX_STAT;

	if ( plr[pnum]._pHitPoints > MAX_HP )
		plr[pnum]._pHitPoints = MAX_HP;

	if ( plr[pnum]._pMana > MAX_MANA )
		plr[pnum]._pMana = MAX_MANA;

}
// ENDPATCH2

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void ProcessPlayers()
{
	int		raflag;		// Run again flag
	int		pnum;

	if ((DWORD)myplr >= MAX_PLRS)
		app_fatal("ProcessPlayers: illegal player %d",myplr);

	// TEMP! debugging
	for (int i = 0; i < MAXINV; i++) {
		if (plr[myplr].InvGrid[i] > 0)
			app_assert(plr[myplr].InvGrid[i] <= plr[myplr]._pNumInv);
	}

	if (plr[myplr].pLvlLoad > 0) plr[myplr].pLvlLoad--;

	if (sfxdelay > 0) {
		sfxdelay--;
		if (sfxdelay == 0)
		{
			switch (sfxdnum)
			{
				case HSFX_DEFILER1:
					InitQTextMsg(TXT_DEFILER1);
					break;
				case HSFX_DEFILER2:
					InitQTextMsg(TXT_DEFILER2);
					break;
				case HSFX_DEFILER3:
					InitQTextMsg(TXT_DEFILER3);
					break;
				case HSFX_DEFILER4:
					InitQTextMsg(TXT_DEFILER4);
					break;
				default:
					PlaySFX(sfxdnum);
			}
		}
	}

	ValidatePlayer();

	for (pnum = 0; pnum < MAX_PLRS; pnum++) {
		if (!plr[pnum].plractive) continue;
		if (currlevel != plr[pnum].plrlevel) continue;
		if ((pnum != myplr) && (plr[pnum]._pLvlChanging)) continue;

		// PATCH2.JMM.3/5/97
		CheckCheatStats( pnum );
		// PATCH2.JMM.3/5/97




		if ((!PlrDeathModeOK(pnum)) && ((plr[pnum]._pHitPoints >> HP_SHIFT) <= 0)) {
			// rjs - manashld fix? - SetPlayerHitPoints(pnum, 0);
			StartPlrKill(pnum, KILL_UNKNOWN);
		}

		if (pnum == myplr) {
			// Constricting item
			if ((plr[pnum]._pIFlags & IAF_CONSTRICT) && (currlevel != 0)) {
				plr[pnum]._pHitPoints -= 4;
				plr[pnum]._pHPBase -= 4;
				if ((plr[pnum]._pHitPoints >> HP_SHIFT) <= 0) {
					// rjs - manashld fix? - SetPlayerHitPoints(pnum, 0);
					StartPlrKill(pnum, FALSE);
				}
				drawhpflag = TRUE;
			}
			
			// lose mana item
			if (plr[pnum]._pIFlags & IAF_LMANA) {
				if (plr[pnum]._pManaBase > 0 ) {
					plr[pnum]._pManaBase -= plr[pnum]._pMana;
					plr[pnum]._pMana = 0;
					drawmanaflag = TRUE;
				}
			}
		}

		raflag = RUN_DONE;
		do {
			// Run Player Mode
			switch (plr[pnum]._pmode) {
				case PM_STAND :
					raflag = PM_DoStand(pnum);
					break;
				case PM_WALK :
					raflag = PM_DoWalk(pnum);
					break;
				case PM_WALK2 :
					raflag = PM_DoWalk2(pnum);
					break;
				case PM_WALK3:
					raflag = PM_DoWalk3(pnum);
					break;
				case PM_ATTACK:
					raflag = PM_DoAttack(pnum);
					break;
				case PM_RATTACK:
					raflag = PM_DoRangeAttack(pnum);
					break;
				case PM_BLOCK :
					raflag = PM_DoBlock(pnum);
					break;
				case PM_SPELL:
					raflag = PM_DoSpell(pnum);
					break;
				case PM_GOTHIT:
					raflag = PM_DoGotHit(pnum);
					break;
				case PM_DEATH:
					raflag = PM_DoDeath(pnum);
					break;
				case PM_NEWLVL:
					raflag = PM_DoNewLvl(pnum);
					break;
			}

			// Check for new command
			CheckNewPath(pnum);

		} while (raflag != RUN_DONE);

		// Animate Player
		plr[pnum]._pAnimCnt++;
		if (plr[pnum]._pAnimCnt > plr[pnum]._pAnimDelay) {
			plr[pnum]._pAnimCnt = 0;
			plr[pnum]._pAnimFrame++;
			if (plr[pnum]._pAnimFrame > plr[pnum]._pAnimLen) plr[pnum]._pAnimFrame = 1;
		}
	}

	//PlayerDoppel();
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void ClrPlrPath(int pnum) {
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("ClrPlrPath: illegal player %d",pnum);
	FillMemory(plr[pnum].walkpath,MAXPATHLEN,PCMD_NOTHING);
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL PosOkPlayer(int pnum, int px, int py)
{
	int mi, p, pn;
	char bv;

	if (dPiece[px][py] == 0) return FALSE;
	pn = dPiece[px][py];
	if (SolidLoc(px, py)) return FALSE;
	if (dPlayer[px][py]) {
		if (dPlayer[px][py] > 0) p = dPlayer[px][py] - 1;
		else p = -(dPlayer[px][py] + 1);
		if ((p != pnum) && (plr[p]._pHitPoints != 0)) return FALSE;
	}
	if (dMonster[px][py]) {
		if (currlevel == 0) return FALSE;
		if (dMonster[px][py] > 0) {
			mi = dMonster[px][py] - 1;
			if ((monster[mi]._mhitpoints >> HP_SHIFT) > 0) return FALSE;
		} else return FALSE;
	}
	if (dObject[px][py]) {
		if (dObject[px][py] > 0) bv = dObject[px][py] - 1;
		else bv = -(dObject[px][py] + 1);
		if (object[bv]._oSolidFlag) return FALSE;
	}
	return TRUE;
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MakePlrPath(int pnum, int xx, int yy, BOOL endspace)
{
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("MakePlrPath: illegal player %d",pnum);

	// set target position
	plr[pnum]._ptargx = xx;
	plr[pnum]._ptargy = yy;

	if (plr[pnum]._pfutx == xx && plr[pnum]._pfuty == yy)
		return;

	int pathlen = FindPath(PosOkPlayer, pnum, plr[pnum]._pfutx, plr[pnum]._pfuty, xx, yy, plr[pnum].walkpath);
	if(pathlen) {
		if (!endspace) {
			pathlen--;
			switch (plr[pnum].walkpath[pathlen]) {
				case PCMD_WALKUR:
					yy++;
					break;
				case PCMD_WALKUL:
					xx++;
					break;
				case PCMD_WALKDR:
					xx--;
					break;
				case PCMD_WALKDL:
					yy--;
					break;
				case PCMD_WALKU:
					xx++;
					yy++;
					break;
				case PCMD_WALKR:
					xx--;
					yy++;
					break;
				case PCMD_WALKD:
					xx--;
					yy--;
					break;
				case PCMD_WALKL:
					xx++;
					yy--;
					break;
			}
			plr[pnum]._ptargx = xx;
			plr[pnum]._ptargy = yy;
		}
		plr[pnum].walkpath[pathlen] = PCMD_NOTHING;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void CheckPlrSpell()
{
	int sd;
	BOOL addflag = FALSE;

	if ((DWORD)myplr >= MAX_PLRS)
		app_fatal("CheckPlrSpell: illegal player %d",myplr);

	int rspell = plr[myplr]._pRSpell;

	if (rspell == -1) {
		if (plr[myplr]._pClass == CLASS_WARRIOR) PlaySFX(PS_WARR34);
		#if !IS_VERSION(SHAREWARE)
		else if (plr[myplr]._pClass == CLASS_ROGUE) PlaySFX(PS_ROGUE34);
		else if (plr[myplr]._pClass == CLASS_SORCEROR) PlaySFX(PS_MAGE34);
		else if (plr[myplr]._pClass == CLASS_MONK) PlaySFX(PS_MONK34);
		else if (plr[myplr]._pClass == CLASS_BARD) PlaySFX(PS_BARD34);
		else if (plr[myplr]._pClass == CLASS_BARBARIAN) PlaySFX(PS_BARBARIAN34);
		#endif
		return;
	}

	//rjs if (leveltype == 0) return;
	if (leveltype == 0 && spelldata[plr[myplr]._pRSpell].sTownSpell == FALSE) {
		if (plr[myplr]._pClass == CLASS_WARRIOR) PlaySFX(PS_WARR27);
		#if !IS_VERSION(SHAREWARE)
		else if (plr[myplr]._pClass == CLASS_ROGUE) PlaySFX(PS_ROGUE27);
		else if (plr[myplr]._pClass == CLASS_SORCEROR) PlaySFX(PS_MAGE27);
		else if (plr[myplr]._pClass == CLASS_MONK) PlaySFX(PS_MONK27);
		else if (plr[myplr]._pClass == CLASS_BARD) PlaySFX(PS_BARD27);
		else if (plr[myplr]._pClass == CLASS_BARBARIAN) PlaySFX(PS_BARBARIAN27);
		#endif
		return;
	}
	if (curs != GLOVE_CURS) return;

	// Check if cursor is in play area, allowing certain spells to be executed anywhere
	if (!(((MouseY < 352)
		   && !(chrflag && MouseX < 320)
		   && !(invflag && MouseX > 320)) ||
		   ((MouseY < 352) && (rspell == SPL_HEAL
		  || rspell == SPL_IDENTIFY
		  || rspell == SPL_REPAIR
		  || rspell == SPL_INFRA
		  || rspell == SPL_RECHARGE))))
		return;

	switch (plr[myplr]._pRSplType) {
		case SPT_ABILITY :
		case SPT_MEMORIZED :
			addflag = CheckSpell(myplr, plr[myplr]._pRSpell, plr[myplr]._pRSplType, FALSE);
			break;
		case SPT_SCROLL :
			addflag = UseScroll();
			break;
		case SPT_ITEM:
			addflag = UseStaff();
			break;
	}
	if (addflag) {
		if (plr[myplr]._pRSpell == SPL_WALL 
			|| plr[myplr]._pRSpell == SPL_LTWALL) {
			sd = GetDirection(plr[myplr]._px, plr[myplr]._py, cursmx, cursmy);
			NetSendCmdLocParam3(TRUE, CMD_SPELLXYD, cursmx, cursmy, plr[myplr]._pRSpell, sd, GetSpellLevel(myplr, plr[myplr]._pRSpell));
		} else {
			if (cursmonst != -1) NetSendCmdParam3(TRUE, CMD_SPELLID, cursmonst, plr[myplr]._pRSpell,GetSpellLevel(myplr, plr[myplr]._pRSpell));
			else {
				if (cursplr != -1) NetSendCmdParam3(TRUE, CMD_SPELLPID, cursplr, plr[myplr]._pRSpell,GetSpellLevel(myplr, plr[myplr]._pRSpell));
				else NetSendCmdLocParam2(TRUE, CMD_SPELLXY, cursmx, cursmy, plr[myplr]._pRSpell,GetSpellLevel(myplr, plr[myplr]._pRSpell));
			}
		}
	} else {
		if (plr[myplr]._pRSplType == SPT_MEMORIZED) {
			if (plr[myplr]._pClass == CLASS_WARRIOR) PlaySFX(PS_WARR35);
			#if !IS_VERSION(SHAREWARE)
			else if (plr[myplr]._pClass == CLASS_ROGUE) PlaySFX(PS_ROGUE35);
			else if (plr[myplr]._pClass == CLASS_SORCEROR) PlaySFX(PS_MAGE35);
			else if (plr[myplr]._pClass == CLASS_MONK) PlaySFX(PS_MONK35);
			else if (plr[myplr]._pClass == CLASS_BARD) PlaySFX(PS_BARD35);
			else if (plr[myplr]._pClass == CLASS_BARBARIAN) PlaySFX(PS_BARBARIAN35);
			#endif
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SyncPlrAnim(int p)
{
	int	dir, s;

	if ((DWORD)p >= MAX_PLRS)
		app_fatal("SyncPlrAnim: illegal player %d",p);

	dir = plr[p]._pdir;
	switch (plr[p]._pmode) {
		case PM_STAND :
			plr[p]._pAnimData = plr[p]._pNAnim[dir];
			break;
		case PM_WALK :
			plr[p]._pAnimData = plr[p]._pWAnim[dir];
			break;
		case PM_WALK2 :
			plr[p]._pAnimData = plr[p]._pWAnim[dir];
			break;
		case PM_WALK3:
			plr[p]._pAnimData = plr[p]._pWAnim[dir];
			break;
		case PM_ATTACK:
			plr[p]._pAnimData = plr[p]._pAAnim[dir];
			break;
		case PM_RATTACK:
			plr[p]._pAnimData = plr[p]._pAAnim[dir];
			break;
		case PM_BLOCK:
			plr[p]._pAnimData = plr[p]._pBAnim[dir];
			break;
		case PM_SPELL:
			if (p == myplr) s = spelldata[plr[p]._pSpell].sType;
			//jam.patch1.start.1/23/97
			//else s == ST_FIRE;
			else s = ST_FIRE;
			//jam.patch1.end.1/23/97
			if (s == ST_FIRE) plr[p]._pAnimData = plr[p]._pFAnim[dir];
			if (s == ST_LIGHT) plr[p]._pAnimData = plr[p]._pLAnim[dir];
			if (s == ST_MISC) plr[p]._pAnimData = plr[p]._pTAnim[dir];
			break;
		case PM_GOTHIT:
			plr[p]._pAnimData = plr[p]._pHAnim[dir];
			break;
		case PM_DEATH:
			plr[p]._pAnimData = plr[p]._pDAnim[dir];
			break;
		case PM_NEWLVL:
		case PM_QUIT:
			plr[p]._pAnimData = plr[p]._pNAnim[dir];
			break;
		default:
			app_fatal("SyncPlrAnim");
		break;
	}

	app_assert(plr[p]._pAnimData);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void SyncInitPlrPos(int pnum)
{
	// Force the player's target position to be where he wants it to be
	plr[pnum]._ptargx = plr[pnum]._px;
	plr[pnum]._ptargy = plr[pnum]._py;

	// Don't find a space for single player
	if (gbMaxPlayers == 1) return;

	// Find a place to put him in the map
	if (plr[pnum].plrlevel == currlevel) {
		for (int i = 0; i < sizeof(plrxoff2)/sizeof(plrxoff2[0]) - 1; i++) {
			if (PosOkPlayer(
				pnum,
				plr[pnum]._px + plrxoff2[i],
				plr[pnum]._py + plryoff2[i]
			)) break;
		}

		// stuff player into the map
		plr[pnum]._px += plrxoff2[i];
		plr[pnum]._py += plryoff2[i];
		dPlayer[plr[pnum]._px][plr[pnum]._py] = pnum + 1;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void SyncInitPlr(int pnum) {
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("SyncInitPlr: illegal player %d",pnum);

	SetPlrAnims(pnum);
	SyncInitPlrPos(pnum);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void CheckStats(int p)
/*-----------------------------------------------------------------------*
** Description: Checks to see if specified characters base stat value
**              is greater than the max base stat, or less than 0.
**              If characters base stat > max. stat, then set characters base 
**              stat = max. stat. If  characters base stat < 0, then set 
**              characters base stat = 0.
**       Input: p = plr[?] index
**              s = stat to check
**      Return: None
**-----------------------------------------------------------------------*/
{
	int	c, i;

	if ((DWORD)p >= MAX_PLRS)
		app_fatal("CheckStats: illegal player %d",p);

	//Find character class
	if (plr[p]._pClass == CLASS_WARRIOR) c = 0;
	else if (plr[p]._pClass == CLASS_ROGUE) c = 1;
    else if (plr[p]._pClass == CLASS_SORCEROR) c = 2;
    else if (plr[p]._pClass == CLASS_MONK) c = 3;
    else if (plr[p]._pClass == CLASS_BARD) c = 4;
    else if (plr[p]._pClass == CLASS_BARBARIAN) c = 5;
	//Check stats
	for (i = 0; i < 4; i++) {
		switch(i) {
			case 0:
				//Strength
				if (plr[p]._pBaseStr > MaxStats[c][0]) plr[p]._pBaseStr = MaxStats[c][0];
				else if (plr[p]._pBaseStr < 0) plr[p]._pBaseStr = 0;
				break;
			case 1:
				//Magic
				if (plr[p]._pBaseMag > MaxStats[c][1]) plr[p]._pBaseMag = MaxStats[c][1];
				else if (plr[p]._pBaseMag < 0) plr[p]._pBaseMag = 0;			
				break;
			case 2:
				//Dexterity
				if (plr[p]._pBaseDex > MaxStats[c][2]) plr[p]._pBaseDex = MaxStats[c][2];
				else if (plr[p]._pBaseDex < 0) plr[p]._pBaseDex = 0;			
				break;
			case 3:
				//Vitality
				if (plr[p]._pBaseVit > MaxStats[c][3]) plr[p]._pBaseVit = MaxStats[c][3];
				else if (plr[p]._pBaseVit < 0) plr[p]._pBaseVit = 0;			
				break;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void ModifyPlrStr(int p, int l)
{
	if ((DWORD)p >= MAX_PLRS)
		app_fatal("ModifyPlrStr: illegal player %d",p);
	int ms = MaxStats[plr[p]._pClass][0];
	if ((plr[p]._pBaseStr + l) > ms) l = ms - plr[p]._pBaseStr;
	plr[p]._pStrength += l;
	plr[p]._pBaseStr += l;
//	if (plr[p]._pClass == CLASS_ROGUE ||
//	    plr[p]._pClass == CLASS_MONK  ||
//	    plr[p]._pClass == CLASS_BARD)
//		plr[p]._pDamageMod = ((plr[p]._pStrength + plr[p]._pDexterity) * plr[p]._pLevel) / 200;
//	else
//		plr[p]._pDamageMod = (plr[p]._pStrength * plr[p]._pLevel) / 100;
	CalcPlrInv(p,TRUE);
	if (p == myplr) NetSendCmdParam1(FALSE, CMD_SETSTR, plr[p]._pBaseStr);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void ModifyPlrMag(int p, int l)
{
	if ((DWORD)p >= MAX_PLRS)
		app_fatal("ModifyPlrMag: illegal player %d",p);
	int ms = MaxStats[plr[p]._pClass][1];
	if ((plr[p]._pBaseMag + l) > ms) l = ms - plr[p]._pBaseMag;
	plr[p]._pMagic += l;
	plr[p]._pBaseMag += l;
	l = l << MANA_SHIFT;
	if (plr[p]._pClass == CLASS_SORCEROR) l = l << 1; // 2x
	else if (plr[p]._pClass == CLASS_BARD) l += l >> 1; // 1.5x
	plr[p]._pMaxManaBase += l;
	plr[p]._pMaxMana += l;

	if (!(plr[p]._pIFlags & IAF_LMANA)) {
		plr[p]._pManaBase += l;
		plr[p]._pMana += l;
	}
	CalcPlrInv(p,TRUE);
	if (p == myplr) NetSendCmdParam1(FALSE, CMD_SETMAG, plr[p]._pBaseMag);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void ModifyPlrDex(int p, int l)
{
	if ((DWORD)p >= MAX_PLRS)
		app_fatal("ModifyPlrDex: illegal player %d",p);
	int ms = MaxStats[plr[p]._pClass][2];
	if ((plr[p]._pBaseDex + l) > ms) l = ms - plr[p]._pBaseDex;
	plr[p]._pDexterity += l;
	plr[p]._pBaseDex += l;
	CalcPlrInv(p,TRUE);
//	if (plr[p]._pClass == CLASS_ROGUE ||
//	    plr[p]._pClass == CLASS_MONK  ||
//	    plr[p]._pClass == CLASS_BARD)
//	{
//		plr[p]._pDamageMod = ((plr[p]._pStrength + plr[p]._pDexterity) * plr[p]._pLevel) / 200;
//	}
	if (p == myplr) NetSendCmdParam1(FALSE, CMD_SETDEX, plr[p]._pBaseDex);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void ModifyPlrVit(int p, int l)
{
	if ((DWORD)p >= MAX_PLRS)
		app_fatal("ModifyPlrVit: illegal player %d",p);
	int ms = MaxStats[plr[p]._pClass][3];
	if ((plr[p]._pBaseVit + l) > ms) l = ms - plr[p]._pBaseVit;
	plr[p]._pVitality += l;
	plr[p]._pBaseVit += l;
	l = l << HP_SHIFT;
	if (plr[p]._pClass == CLASS_WARRIOR
		|| plr[p]._pClass == CLASS_BARBARIAN) l = l << 1;
	plr[p]._pHPBase += l;
	plr[p]._pMaxHPBase += l;
	plr[p]._pHitPoints += l;
	plr[p]._pMaxHP += l;
	CalcPlrInv(p,TRUE);
	if (p == myplr) NetSendCmdParam1(FALSE, CMD_SETVIT, plr[p]._pBaseVit);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SetPlayerHitPoints(int pnum, int newhp)
{
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("SetPlayerHitPoints: illegal player %d",pnum);
	plr[pnum]._pHitPoints = newhp;
	plr[pnum]._pHPBase = newhp - (plr[pnum]._pMaxHP - plr[pnum]._pMaxHPBase);
	if (pnum == myplr) drawhpflag = TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SetPlrStr(int p, int v)
{
	if ((DWORD)p >= MAX_PLRS)
		app_fatal("SetPlrStr: illegal player %d",p);
	plr[p]._pBaseStr = v;
	CalcPlrInv(p,TRUE);
/* this stuff is done in CalcPlrInv */
//	if (plr[p]._pClass == CLASS_ROGUE ||
//	    plr[p]._pClass == CLASS_MONK  ||
//	    plr[p]._pClass == CLASS_BARD)
//	{
//		plr[p]._pDamageMod = ((plr[p]._pStrength + plr[p]._pDexterity) * plr[p]._pLevel) / 200;
//	}
//	else
//		plr[p]._pDamageMod = (plr[p]._pStrength * plr[p]._pLevel) / 100;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SetPlrMag(int p, int v)
{
	if ((DWORD)p >= MAX_PLRS)
		app_fatal("SetPlrMag: illegal player %d",p);
	plr[p]._pBaseMag = v;
	v = v << MANA_SHIFT;
	if (plr[p]._pClass == CLASS_SORCEROR) v = v << 1; // 2x
	else if (plr[p]._pClass == CLASS_BARD) v += v >> 1; // 1.5x
	plr[p]._pMaxManaBase = v;
	plr[p]._pMaxMana = v;
	CalcPlrInv(p,TRUE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SetPlrDex(int p, int v)
{
	if ((DWORD)p >= MAX_PLRS)
		app_fatal("SetPlrDex: illegal player %d",p);
	plr[p]._pBaseDex = v;
	CalcPlrInv(p,TRUE);
/* this stuff is done in CalcPlrInv */
//	if (plr[p]._pClass == CLASS_ROGUE ||
//	    plr[p]._pClass == CLASS_MONK ||
//	    plr[p]._pClass == CLASS_BARD)
//	{
//		plr[p]._pDamageMod = ((plr[p]._pStrength + plr[p]._pDexterity) * plr[p]._pLevel) / 200;
//	}
//	else
//		plr[p]._pDamageMod = (plr[p]._pStrength * plr[p]._pLevel) / 100;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SetPlrVit(int p, int v)
{
	if ((DWORD)p >= MAX_PLRS)
		app_fatal("SetPlrVit: illegal player %d",p);
	plr[p]._pBaseVit = v;
	v = v << HP_SHIFT;
	if (plr[p]._pClass == CLASS_WARRIOR
		|| plr[p]._pClass == CLASS_BARBARIAN) v = v << 1;
	plr[p]._pHPBase = v;
	plr[p]._pMaxHPBase = v;
	CalcPlrInv(p,TRUE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void InitDungMsgs(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("InitDungMsgs: illegal player %d",pnum);
	plr[pnum].pDungMsgs = 0;
	plr[pnum].pHellfireMsgs = 0;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void PlayDungMsgs()
{
	if ((DWORD)myplr >= MAX_PLRS)
		app_fatal("PlayDungMsgs: illegal player %d",myplr);
	if ((currlevel == 1) && (!plr[myplr]._pLvlVisited[1]) && (gbMaxPlayers == 1) && (!(plr[myplr].pDungMsgs & DUNGMSG_1))) {
		// play "The sancity of this place" speech
		sfxdelay = 40;
		#if !IS_VERSION(SHAREWARE)
		if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR97;
		else if (plr[myplr]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE97;
		else if (plr[myplr]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE97;
		else if (plr[myplr]._pClass == CLASS_MONK) sfxdnum = PS_MONK97;
		else if (plr[myplr]._pClass == CLASS_BARD) sfxdnum = PS_BARD97;
		else if (plr[myplr]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN97;
		#else
		if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR97;
		#endif
		plr[myplr].pDungMsgs |= DUNGMSG_1;
	} 

	else if ((currlevel == 5) && (!plr[myplr]._pLvlVisited[5]) && (gbMaxPlayers == 1) && (!(plr[myplr].pDungMsgs & DUNGMSG_2))) {
		// play "smells of death" speech
		sfxdelay = 40;
		#if !IS_VERSION(SHAREWARE)
		if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR96B;
		else if (plr[myplr]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE96;
		else if (plr[myplr]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE96;
		else if (plr[myplr]._pClass == CLASS_MONK) sfxdnum = PS_MONK96;
		else if (plr[myplr]._pClass == CLASS_BARD) sfxdnum = PS_BARD96;
		else if (plr[myplr]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN96;
		#else
		if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR96B;
		#endif
		plr[myplr].pDungMsgs |= DUNGMSG_2;
	} 

	else if ((currlevel == 9) && (!plr[myplr]._pLvlVisited[9]) && (gbMaxPlayers == 1) && (!(plr[myplr].pDungMsgs & DUNGMSG_3))) {
		// play "It's hot down here" speech
		sfxdelay = 40;
		#if !IS_VERSION(SHAREWARE)
		if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR98;
		else if (plr[myplr]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE98;
		else if (plr[myplr]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE98;
		else if (plr[myplr]._pClass == CLASS_MONK) sfxdnum = PS_MONK98;
		else if (plr[myplr]._pClass == CLASS_BARD) sfxdnum = PS_BARD98;
		else if (plr[myplr]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN98;
		#else
		if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR98;
		#endif
		plr[myplr].pDungMsgs |= DUNGMSG_3;
	} 

	else if ((currlevel == 13) && (!plr[myplr]._pLvlVisited[13]) && (gbMaxPlayers == 1) && (!(plr[myplr].pDungMsgs & DUNGMSG_4))) {
		// play "I must be getting close" speech
		sfxdelay = 40;
		#if !IS_VERSION(SHAREWARE)
		if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR99;
		else if (plr[myplr]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE99;
		else if (plr[myplr]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE99;
		else if (plr[myplr]._pClass == CLASS_MONK) sfxdnum = PS_MONK99;
		else if (plr[myplr]._pClass == CLASS_BARD) sfxdnum = PS_BARD99;
		else if (plr[myplr]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN99;
		#else
		if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR99;
		#endif
		plr[myplr].pDungMsgs |= DUNGMSG_4;
	} 

	else if ((currlevel == 16) && (!plr[myplr]._pLvlVisited[15]) && (gbMaxPlayers == 1) && (!(plr[myplr].pDungMsgs & DUNGMSG_5))) {
		// play "Diablo's Wrath"
		sfxdelay = 40;
		#if !IS_VERSION(SHAREWARE)
		if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_DIABLVLINT;
		else if (plr[myplr]._pClass == CLASS_ROGUE) sfxdnum = PS_DIABLVLINT;
		else if (plr[myplr]._pClass == CLASS_SORCEROR) sfxdnum = PS_DIABLVLINT;
		else if (plr[myplr]._pClass == CLASS_MONK) sfxdnum = PS_DIABLVLINT;
		else if (plr[myplr]._pClass == CLASS_BARD) sfxdnum = PS_DIABLVLINT;
		else if (plr[myplr]._pClass == CLASS_BARBARIAN) sfxdnum = PS_DIABLVLINT;
		#endif
		plr[myplr].pDungMsgs |= DUNGMSG_5;
	} 
	else if ((currlevel == HIVESTART) && (!plr[myplr]._pLvlVisited[HIVESTART]) && (gbMaxPlayers == 1) && (!(plr[myplr].pHellfireMsgs & DUNGMSG_1))) 
	{
		// play Defiler 1 JKE
		sfxdelay = 10;
		sfxdnum = HSFX_DEFILER1;
		quests[Q_DEFILER]._qactive = QUEST_NOTDONE;
		quests[Q_DEFILER]._qlog = TRUE;
		quests[Q_DEFILER]._qmsg = TXT_DEFILER1;
		plr[myplr].pHellfireMsgs |= DUNGMSG_1;
	} 
/*	else if ((currlevel == (HIVESTART+1)) && (!plr[myplr]._pLvlVisited[(HIVESTART+1)]) && (gbMaxPlayers == 1) && (!(plr[myplr].pHellfireMsgs & DUNGMSG_2))) {
		// play Defiler 2 JKE
		sfxdelay = 10;
			sfxdnum = HSFX_DEFILER2;
		plr[myplr].pHellfireMsgs |= DUNGMSG_2;
	} */
	else if ((currlevel == (HIVESTART+2)) && (!plr[myplr]._pLvlVisited[(HIVESTART+2)]) && (gbMaxPlayers == 1) && (!(plr[myplr].pHellfireMsgs & DUNGMSG_3))) {
		// play Defiler 3 JKE
		sfxdelay = 10;
			sfxdnum = HSFX_DEFILER3;
		plr[myplr].pHellfireMsgs |= DUNGMSG_3;
	} 
/*	else if ((currlevel == (HIVESTART+3)) && (!plr[myplr]._pLvlVisited[(HIVESTART+3)]) && (gbMaxPlayers == 1) && (!(plr[myplr].pHellfireMsgs & DUNGMSG_4))) {
		// play Defiler 4 JKE
		sfxdelay = 10;
			sfxdnum = HSFX_DEFILER4;
		plr[myplr].pHellfireMsgs |= DUNGMSG_4;
	} */
	else if ((currlevel == CRYPTSTART) && (!plr[myplr]._pLvlVisited[CRYPTSTART]) && (gbMaxPlayers == 1) && (!(plr[myplr].pDungMsgs & DUNGMSG_6))) {
		// play "great power " JKE
		sfxdelay = 30;

//		sfxdnum = HSFX_CORNERSTONE1;
/*		quests[Q_CORNERSTONE]._qactive = QUEST_NOTDONE;
		quests[Q_CORNERSTONE]._qlog = TRUE;
		quests[Q_CORNERSTONE]._qmsg = TXT_CORNERSTONE1;
		InitQTextMsg(TXT_CORNERSTONE1); */
		
		#if !IS_VERSION(SHAREWARE)
		if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR92;
		else if (plr[myplr]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE92;
		else if (plr[myplr]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE92;
		else if (plr[myplr]._pClass == CLASS_MONK) sfxdnum = PS_MONK92;
		else if (plr[myplr]._pClass == CLASS_BARD) sfxdnum = PS_BARD92;
		else if (plr[myplr]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN92;
		#else
		if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR92;
		#endif 

		plr[myplr].pDungMsgs |= DUNGMSG_6;
	} 
	
	else sfxdelay = 0;
}


int GetMaxStr(int Plrclass)
{
	return MaxStats[Plrclass][0];
}

int GetMaxMag(int Plrclass)
{
	return MaxStats[Plrclass][1];
}

int GetMaxDex(int Plrclass)
{
	return MaxStats[Plrclass][2];
}

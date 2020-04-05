/*-----------------------------------------------------------------------**
** 								Diablo
**
**							 Items file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/ITEMS.CPP 4     2/06/97 6:08p Jessmac $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	<limits.h>
#include	"diablo.h"
#pragma hdrstop
#include	"sound.h"
#include	"objects.h"
#include	"items.h"
#include	"itemdat.h"
#include	"engine.h"
#include	"gendung.h"
#include	"player.h"
#include	"control.h"
#include	"monster.h"
#include	"spells.h"
#include	"quests.h"
#include	"cursor.h"
#include	"effects.h"
#include	"lighting.h"
#include	"stores.h"
#include	"spelldat.h"
#include	"scrollrt.h"
#include	"inv.h"
#include	"msg.h"
#include	"multi.h"
#include	"monstdat.h"
#include	"doom.h"
#include	"missiles.h"
#include	"minitext.h"
#include	"storm.h"
#include	"DRLG_l1.h"
#include	"packplr.h"
#include	"textdat.h"


void RecreateTownItem(int ii, int idx, WORD icreateinfo, int iseed, int ivalue);
void SaveItemPower(int i, int power, int param1, int param2, int minval, int maxval, int multval);
void GetItemPower(int i, int minlvl, int maxlvl, long flgs, BOOL onlygood);
void GetItemAttrs(int i, int idata, int lvl);
void SetupItem(int);
void RecalcStoreStats();
void SpawnMap();
void DeleteItem(int ii, int i);

/*-----------------------------------------------------------------------*
** Global Variables
**-----------------------------------------------------------------------*/
CornerStoneType CornerStone;
extern char gszProgKey[];

int GOLD_VMAX = 5000;
const int GOLD_DOUBLE_VMAX = 2 * GOLD_VMAX;
ItemStruct	item[MAXITEMS+1];
long numitems = 0;

int itemactive[MAXITEMS];
int itemavail[MAXITEMS];
int tem;

//PATCH1.JMM
ItemGetRecordStruct itemgets[MAXITEMS];
int gnNumGetRecords = 0;
//ENDPATCH1.JMM



BOOL UniqueItemFlag[MAXUITEMS];
BOOL uitemflag;
ItemStruct curruitem;
#if CHEATS
extern BOOL itemcheat;

BOOL davecheat = FALSE;
#endif

BOOL itemhold[3][3];


#define MAXOIL			10

int OilLvlTbl[MAXOIL]= { 1, 10, 1, 10, 4, 1, 5, 17, 1, 10 };
int OilValue[MAXOIL] = { 500, 2500, 500, 2500, 1500, 100, 2500, 15000, 500, 2500 };
int OilIdVal[MAXOIL] = { 
	IMID_OILACC, 
	IMID_OILMAST,
	IMID_OILSHRP,
	IMID_OILDEATH,
	IMID_OILSKILL,
	IMID_OILBLKSM,
	IMID_OILFORT,
	IMID_OILPERM,
	IMID_OILHARD,
	IMID_OILIMPER
};
char OilStr[MAXOIL][25] = {		 
	"Oil of Accuracy",
	"Oil of Mastery",
	"Oil of Sharpness",
	"Oil of Death",
	"Oil of Skill",
	"Blacksmith Oil",
	"Oil of Fortitude",
	"Oil of Permanence",
	"Oil of Hardening",
	"Oil of Imperviousness"
};

BYTE ItemCAnimTbl[ITEM_LAST_ID] = {
				// 1x1
		20,		// ITEM_BLUEBTL
		16,		// ITEM_SCROLL
		16,		// ITEM_SCROLL2
		16,		// ITEM_SCROLL3
		4,		// ITEM_1GOLD
		4,		// ITEM_3GOLD
		4,		// ITEM_5GOLD
		12,		// ITEM_GOLDRING
		12,		// ITEM_1JRING
		12,		// ITEM_WOODRING
		12,		// ITEM_BLUERING
		12,		// ITEM_3JRING
		12,		// ITEM_SLVRRING
		12,		// ITEM_MJRING
		12,		// ITEM_BRNRING
		21,		// ITEM_SPECTRAL
		21,		// ITEM_3COLORPOT
		25,		// ITEM_GOLDENELIX
		12,		// ITEM_EMPYBAND
		28,		// ITEM_EAR1
		28,		// ITEM_EAR2
		28,		// ITEM_EAR3
		38,		// ITEM_SPHERE
		38,		// ITEM_CUBE
		38,		// ITEM_PYRIMID
		32,		// ITEM_BLOODGEM
		38,		// ITEM_JSPHERE
		38,		// ITEM_JCUBE
		38,		// ITEM_JPYRIMID
		24,		// ITEM_VILE
		24,		// ITEM_BLKBTL
		26,		// ITEM_WHTEBTL
		2,		// ITEM_REDBTL
		25,		// ITEM_YELBTL
		22,		// ITEM_ORGBTL
		23,		// ITEM_BREDBTL
		24,		// ITEM_BLKBTL2
		25,		// ITEM_GOLDBTL
		27,		// ITEM_LTBLUEBTL
		27,		// ITEM_BLUEBTL2
		29,		// ITEM_BRAIN
		0,		// ITEM_CLAW
		0,		// ITEM_FANG
		0,		// ITEM_BREAD
		12,		// ITEM_AMULET
		12,		// ITEM_AMULET1
		12,		// ITEM_AMULET2
		12,		// ITEM_AMULET3
		12,		// ITEM_AMULET4
		0,		// ITEM_POUCH1
				// 1x2
		8,		// ITEM_DAGGER1
		8,		// ITEM_DAGGER2
		0,		// ITEM_BIGBOTTLE
		8,		// ITEM_DAGGER3
		8,		// ITEM_DAGGER4
		8,		// ITEM_DAGGER5
				// 1x3
		8,		// ITEM_BLADE
		8,		// ITEM_BASTSRD
		8,		// ITEM_FALCHION
		6,		// ITEM_MACE
		8,		// ITEM_LONGSRD
		8,		// ITEM_BROADSRD
		8,		// ITEM_SCIMITAR
		6,		// ITEM_MORNSTAR
		8,		// ITEM_SHORTSRD
		8,		// ITEM_CLAYMORE
		6,		// ITEM_CLUB
		8,		// ITEM_SABRE
		8,		// ITEM_KNTSWORD
		6,		// ITEM_CLUB1
		6,		// ITEM_CLUB2
		6,		// ITEM_CLUB3
		8,		// ITEM_SCIMITAR2
		8,		// ITEM_MAGSWORD
		8,		// ITEM_SKULSWORD
				// 2x2
		5,		// ITEM_HELM
		9,		// ITEM_ROCK
		13,		// ITEM_SKCROWN
		13,		// ITEM_CROWN
		13,		// ITEM_MCROWN
		5,		// ITEM_JESTER
		5,		// ITEM_HARLEQ
		5,		// ITEM_FHELM
		15,		// ITEM_BUCKLER
		5,		// ITEM_FHELM2
		5,		// ITEM_GRTHELM
		18,		// ITEM_BOOK1
		18,		// ITEM_BOOK2
		18,		// ITEM_BOOK3
		30,		// ITEM_MUSHROOM
		5,		// ITEM_SKLCAP
		5,		// ITEM_LCAP
		14,		// ITEM_FLESH
		5,		// ITEM_SKLCAP2
		14,		// ITEM_CLOTHES
		13,		// ITEM_CROWN2
		16,		// ITEM_MAP
		18,		// ITEM_BOOK4
		5,		// ITEM_FHELM3
		5,		// ITEM_SAMHELM
				// 2x3
		7,		// ITEM_LRGSHIELD
		1,		// ITEM_BTLAXE
		3,		// ITEM_LONGBOW
		17,		// ITEM_PARMOR
		1,		// ITEM_AXE
		15,		// ITEM_WSHIELD
		10,		// ITEM_CLEAVER
		14,		// ITEM_STDARMOR
		3,		// ITEM_COMPBOW
		11,		// ITEM_SHRTSTAFF
		8,		// ITEM_2HSWORD
		0,		// ITEM_CHARMOR
		1,		// ITEM_SMALLAXE
		7,		// ITEM_HVYSHIELD
		0,		// ITEM_SCLARMOR
		7,		// ITEM_SMLSHIELD
		15,		// ITEM_SKULLSHLD
		7,		// ITEM_WOLFSHLD
		3,		// ITEM_SHORTBOW
		3,		// ITEM_STLLONGBOW
		3,		// ITEM_STLSHRTBOW
		6,		// ITEM_SMLWARHAM
		6,		// ITEM_MAUL
		11,		// ITEM_IRONSTAFF
		11,		// ITEM_STLSTAFF
		11,		// ITEM_LONGSTAFF
		31,		// ITEM_INNSIGN
		14,		// ITEM_HLARMOR
		14,		// ITEM_RAGS
		14,		// ITEM_QARMOR
		6,		// ITEM_BALLNCHN
		6,		// ITEM_FLAIL
		7,		// ITEM_TSHIELD
		3,		// ITEM_HNTRBOW
		8,		// ITEM_GRTSWORD
		14,		// ITEM_LARMOR
		0,		// ITEM_SPLTARMOR
		14,		// ITEM_ROBE
		14,		// ITEM_HVYROBE
		0,		// ITEM_RINGARMOR
		33,		// ITEM_ANVIL
		1,		// ITEM_BROADAXE 
		1,		// ITEM_LRGAXE 
		1,		// ITEM_WICKAXE 
		1,		// ITEM_HANDAXE 
		1,		// ITEM_GREATAXE 
		7,		// ITEM_IRONSHLD
		7,		// ITEM_KITESHLD
		7,		// ITEM_LRGSHLD
		14,		// ITEM_CLOAK
		14,		// ITEM_CAPE
		17,		// ITEM_PARMOR2
		17,		// ITEM_PARMOR3
		17,		// ITEM_BPLATE
		 0,		// ITEM_RINGMAIL
		34,		// ITEM_BISHOPSTF
		 1,		// ITEM_GEMGRTAXE
		 0,		// ITEM_ARKARMOR
		 3,		// ITEM_CROSBOW
		17,		// ITEM_NAJARMOR
		 8,		// ITEM_GRIZZLY
		 8,		// ITEM_GRANDPA
		 6,		// ITEM_PROTECT
		 1,		// ITEM_REAVER
		 3,		// ITEM_WINDFOR
		 3,		// ITEM_SWARBOW
		11,		// ITEM_COMPSTF
		 3,		// ITEM_SBATLBOW
// Misc
//		 4,		// ITEM_GOLD

// new 1x1		 
		12,		// ITEM_MERLINRING
		12,		// ITEM_MANARING
		12,		// ITEM_AMULWARD
		12,		// ITEM_NECMAGIC
		12,		// ITEM_NECHEALTH
		12,		// ITEM_KARIKSRING
		12,		// ITEM_RINGGROUND
		12,		// ITEM_AMULPROT
		12,		// ITEM_MERCRING
		12,		// ITEM_RINGTHUND
		12,		// ITEM_NECTRUTH
		12,		// ITEM_RINGGIANTS
		12,		// ITEM_AMULGOLD
		12,		// ITEM_RINGMYSTIC
		12,		// ITEM_RINGCOPPER
		12,		// ITEM_AMULACOLYT
		12,		// ITEM_RINGMAGMA
		12,		// ITEM_NECPURIFY
		12,		// ITEM_RINGGLADTR
		35,		// ITEM_RUNEBOMB
		39,		// ITEM_THEODORE
		36,		// ITEM_TORNPAPER1
		36,		// ITEM_TORNPAPER2
		36,		// ITEM_TORNPAPER3
		37,		// ITEM_WHOLEPAPER
		38,		// ITEM_FIRERUNE1
		38,		// ITEM_FIRERUNE2
		38,		// ITEM_LIGHTRUNE1
		38,		// ITEM_LIGHTRUNE2
		38,		// ITEM_STONERUNE
// new 2x2
		41,
		42,
// new 1x3
		 8,		// ITEM_SWORDEDGE
		 8,		// ITEM_SWORDGLAM
		 8,		// ITEM_SWORDSERR
// new 2x3
		17,		// ITEM_ARMRDARK
		 0,		// ITEM_ARMRBONECH
		 6,		// ITEM_HAMRTHUND
		 8,		// ITEM_SWRDCRYSTL
		11,		// ITEM_STAFJESTER
		11,		// ITEM_STAFMANA
		 3,		// ITEM_BOWVULCAN
		 3,		// ITEM_BOWSPEED
		 1,		// ITEM_AXEANCIENT
		 6,		// ITEM_CLUBCARNAG
		 6,		// ITEM_MACEDARK
		 6,		// ITEM_CLUBDECAY
		 1,		// ITEM_AXEDECAY
		 8,		// ITEM_SWRDDECAY
		 6,		// ITEM_MACEDECAY
		11,		// ITEM_STAFDECAY
		 3,		// ITEM_BOWDECAY
		 6,		// ITEM_CLUBOUCH
		 8,		// ITEM_SWRDDEVAST
		 1,		// ITEM_AXEDEVAST
		 6,		// ITEM_MORNDEVAST
		 6,		// ITEM_MACEDEVAST
		17,		// ITEM_ARMRDMNPLT
		40,
		};


char *ItemFiles[] = {
		"Armor2",		// 0
		"Axe",			// 1
		"FBttle",		// 2
		"Bow",			// 3
		"GoldFlip",		// 4
		"Helmut",		// 5
		"Mace",			// 6
		"Shield",		// 7
		"SwrdFlip",		// 8
		"Rock",			// 9
		"Cleaver",		// 10
		"Staff",		// 11
		"Ring",			// 12
		"CrownF",		// 13
		"LArmor",		// 14
		"WShield",		// 15
		"Scroll",		// 16
		"FPlateAr",		// 17
		"FBook",		// 18
		"Food",			// 19
		"FBttleBB",		// 20
		"FBttleDY",		// 21
		"FBttleOR",		// 22
		"FBttleBR",		// 23
		"FBttleBL",		// 24
		"FBttleBY",		// 25
		"FBttleWH",		// 26
		"FBttleDB",		// 27 
		"FEar",			// 28
		"FBrain",		// 29
		"FMush",		// 30
		"Innsign",		// 31
		"Bldstn",		// 32
		"Fanvil",		// 33
		"FLazStaf",		// 34
		"bombs1",		// 35
		"halfps1",		// 36
		"wholeps1",		// 37
		"runes1",		// 38
		"teddys1",		// 39
		"cows1",		// 40
		"donkys1",		// 41
		"mooses1",		// 42
};	
#define ITEMFTYPES		(sizeof(ItemFiles)/sizeof(char *))

BYTE *itemanims[ITEMFTYPES];

byte ItemAnimLs[ITEMFTYPES] = {
		15,		// Armor2
		13,		// Axe
		16,		// FBttle
		13,		// Bow
		10,		// GoldFlip
		13,		// Helmut
		13,		// Mace
		13,		// Shield
		13,		// SwrdFlip
		10,		// Rock
		13,		// Cleaver
		13,		// Staff
		13,		// Ring
		13,		// Crown
		13,		// LArmor
		13,		// WShield
		13,		// Scroll
		13,		// FPlateAr
		13,		// FBook
		1,		// Food
		16,		// FBttleBB
		16,		// FBttleDY
		16,		// FBttleOR
		16,		// FBttleBR
		16,		// FBttleBL
		16,		// FBttleBY
		16,		// FBttleWH
		16,		// FBttleDB
		13,		// FEar
		12,		// FBrain
		12,		// FMush
		13,		// Innsign
		13,		// Bldstn 
		13,		// Fanvil
		8,		// FLazStaf
		10,		// nest bomb
		16,		// half paper
		16,		// whole paper
		10,		// runes
		10,		// teddy bear
		15,
		15,
		15,
};

int ItemAnimSnds[ITEMFTYPES] = {
		IS_FHARM,		// Armor2
		IS_FAXE,		// Axe
		IS_FPOT,		// FBttle
		IS_FBOW,		// Bow
		IS_GOLD,		// GoldFlip   
		IS_FCAP,		// Helmet
		IS_FSWOR,		// Mace
		IS_FSHLD,		// Shield
		IS_FSWOR,		// SwrdFlip
		IS_FROCK,		// Rock		  
		IS_FAXE,		// Cleaver
		IS_FSTAF,		// Staff
		IS_FRING,		// Ring
		IS_FCAP,		// Crown
		IS_FLARM,		// LArmor
		IS_FSHLD,		// WShield
		IS_FSCRL,		// Scroll
		IS_FHARM,		// FPlateAr
		IS_FBOOK,		// FBook
		IS_FLARM,		// Food
		IS_FPOT,		// FBttleBB
		IS_FPOT,		// FBttleDY
		IS_FPOT,		// FBttleOR
		IS_FPOT,		// FBttleBR
		IS_FPOT,		// FBttleBL
		IS_FPOT,		// FBttleBY
		IS_FPOT,		// FBttleWH
		IS_FPOT,		// FBttleDB
		IS_FBODY,		// FEar
		IS_FBODY,		// FBrain
		IS_FMUSH,		// FMush
		IS_ISIGN,		// Innsign
		IS_FBLST,		// Bldstn
		IS_FANVL,		// Fanvil
		IS_FSTAF,		// FLazStaf
		IS_FROCK,		// Bomb
		IS_FSCRL,		// Half Paper
		IS_FSCRL,		// Whole Paper
		IS_FROCK,		// Rune
		IS_FMUSH,		// Theo
		IS_FHARM,		// Cow Armor
		IS_FLARM,		//
		IS_FLARM,		//
};

int ItemInvSnds[ITEMFTYPES] = {
		IS_IHARM,		// Armor2
		IS_IAXE,		// Axe
		IS_IPOT,		// FBttle
		IS_IBOW,		// Bow
		IS_GOLD,		// GoldFlip   
		IS_ICAP,		// Helmet
		IS_ISWORD,		// Mace
		IS_ISHIEL,		// Shield
		IS_ISWORD,		// SwrdFlip
		IS_IROCK,		// Rock
		IS_IAXE,		// Cleaver
		IS_ISTAF,		// Staff
		IS_IRING,		// Ring
		IS_ICAP,		// Crown
		IS_ILARM,		// LArmor
		IS_ISHIEL,		// WShield
		IS_ISCROL,		// Scroll
		IS_IHARM,		// FPlateAr
		IS_IBOOK,		// FBook
		IS_IHARM,		// Food
		IS_IPOT,		// FBttleBB
		IS_IPOT,		// FBttleDY
		IS_IPOT,		// FBttleOR
		IS_IPOT,		// FBttleBR
		IS_IPOT,		// FBttleBL
		IS_IPOT,		// FBttleBY
		IS_IPOT,		// FBttleWH
		IS_IPOT,		// FBttleDB
		IS_IBODY,		// FEar
		IS_IBODY,		// FBrain
		IS_IMUSH,		// FMush
		IS_ISIGN,		// Innsign
		IS_IBLST,		// Bldstn
		IS_IANVL,		// Fanvil
		IS_ISTAF,		// FLazStaf
		IS_IROCK,		// Bomb
		IS_ISCROL,		// Half Paper
		IS_ISCROL,		// Whole Paper
		IS_IROCK,		// Rune
		IS_IMUSH,		// Theo
		IS_IHARM,		// Cow Armor
		IS_ILARM,		//
		IS_ILARM,		//
};

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
int GetEffLevel()
{
	int efflevel = currlevel;
	if (currlevel >= HIVESTART && currlevel <= HIVEEND)
		efflevel = currlevel - HIVESTART + 9;
	if (currlevel >= CRYPTSTART && currlevel <= CRYPTEND)
		efflevel = currlevel - CRYPTSTART + 14;
	
	return efflevel;	
}
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void InitItemGFX() {
	for (int i = 0; i < ITEMFTYPES; i++) {
		char filestr[64];
		sprintf(filestr, "Items\\%s.CEL", ItemFiles[i]);
		app_assert(! itemanims[i]);
		itemanims[i] = LoadFileInMemSig(filestr,NULL,'IGFX');
	}

	ZeroMemory(UniqueItemFlag,sizeof(UniqueItemFlag));
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
BOOL ItemPlace(int xp, int yp) {
	if (dMonster[xp][yp] != 0) return FALSE;
	if (dPlayer[xp][yp] != 0) return FALSE;
	if (dItem[xp][yp] != 0) return FALSE;
	if (dObject[xp][yp] != 0) return FALSE;
	if (dFlags[xp][yp] & BFLAG_SETPC) return FALSE;
	if (nSolidTable[dPiece[xp][yp]]) return FALSE;
	return TRUE;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void AddInitItems() {

	int efflevel = GetEffLevel();
	int j = random(11, 3) + 3;
	for (int i = 0; i < j; i++) {
		int ii = itemavail[0];
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;

		int xx = random(12, DMAXX - DIRTEDGE) + (DIRTEDGED2);
		int yy = random(12, DMAXY - DIRTEDGE) + (DIRTEDGED2);
		while (! ItemPlace(xx, yy)) {
			xx = random(12, DMAXX - DIRTEDGE) + (DIRTEDGED2);
			yy = random(12, DMAXY - DIRTEDGE) + (DIRTEDGED2);
		}

		item[ii]._ix = xx;
		item[ii]._iy = yy;
		dItem[xx][yy] = ii + 1;
		item[ii]._iSeed = GetRndSeed();
		SetRndSeed(item[ii]._iSeed);
		if (random(12, 2)) GetItemAttrs(ii, IDI_HEAL, efflevel);
		else GetItemAttrs(ii, IDI_MANA, efflevel);
		item[ii]._iCreateInfo = efflevel + ICI_PREGEN;
		SetupItem(ii);
		item[ii]._iAnimFrame = item[ii]._iAnimLen;
		item[ii]._iAnimFlag = FALSE;
		item[ii]._iSelFlag = ISEL_FLR;
		DeltaAddItem(ii);
		numitems++;
	}
}

static void AddNote()
{
	int xx, yy;

	do
	{
		xx = random(12, DMAXX - DIRTEDGE) + (DIRTEDGED2);
		yy = random(12, DMAXY - DIRTEDGE) + (DIRTEDGED2);
	} while (! ItemPlace(xx, yy));

	int idi;
	switch(currlevel)
	{
		case CRYPTSTART+2:
			idi = IDI_NOTE3;
			break;
		case CRYPTSTART+1:
			idi = IDI_NOTE2;
			break;
		default:
			idi = IDI_NOTE1;
			break;
	}
	SpawnQuestItem(idi, xx, yy, FALSE, ISEL_FLR);
	
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void InitItems() {
	int	i;

	DROPLOG("Initing items...\n");
	// Setup a gold item
	GetItemAttrs(0, 0, 1);
	golditem = item[0];
	golditem._iStatFlag = TRUE;

	numitems = 0;
	for (i = 0; i < MAXITEMS; i++) {
		item[i]._itype = 0;
		item[i]._ix = 0;
		item[i]._iy = 0;
		item[i]._iAnimFlag = FALSE;
		item[i]._iSelFlag = ISEL_NONE;
		item[i]._iIdentified = FALSE;
		item[i]._iPostDraw = FALSE;
	}

	for (i = 0; i < MAXITEMS; i++) {
		itemavail[i] = i;
		itemactive[i] = 0;
	}

	if (!setlevel) {

		int rs = GetRndSeed();

		if (QuestStatus(Q_ROCK)) {
			SpawnRock();
		}
		if (QuestStatus(Q_ANVIL)) {
			SpawnQuestItem(IDI_ANVIL, (setpc_x << 1) + 11 + DIRTEDGED2, (setpc_y << 1) + 11 + DIRTEDGED2, FALSE, ISEL_FLR);
		}
		if (gbCowsuit)
			if (currlevel == HIVEEND)
				SpawnQuestItem(IDI_SUITBRWN, 25, 25, 3, ISEL_FLR);
		if (gbCowsuit)
			if (currlevel == HIVESTART+2)
				SpawnQuestItem(IDI_SUITGREY, 25, 25, 3, ISEL_FLR);

		if (currlevel > 0 && currlevel < 16)
			AddInitItems();
		if (currlevel >= CRYPTSTART && currlevel <= CRYPTSTART+2)
			AddNote();
	}

	uitemflag = FALSE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void CalcPlrItemVals(int p, BOOL Loadgfx)
{
	int mind, maxd, tac, g, d, i, mi;
	int bdam, btohit, bac;
	int sadd, madd, dadd, vadd;
	int fr, lr, mr;
	int	dmod, ghit,	lrad;
	int ihp, imana;
	int spllvladd;
	int enac;
	int fmin, fmax, lmin, lmax;
	long iflgs, iflgs2;
	__int64 spl, t;

	app_assert((DWORD) p < MAX_PLRS);
		
	mind = 0;
	maxd = 0;
	tac = 0;
	bdam = 0;
	btohit = 0;
	bac = 0;
	iflgs = 0;
	iflgs2 = 0;
	sadd = 0;
	madd = 0;
	dadd = 0;
	vadd = 0;
	spl = 0;
	fr = 0;
	lr = 0;
	mr = 0;
	dmod = 0;
	ghit = 0;
	lrad = PLRLRAD;
	ihp = 0;
	imana = 0;
	spllvladd = 0;
	enac = 0;
	fmin = 0;
	fmax = 0;
	lmin = 0;
	lmax = 0;
	for (i = 0; i < 7; i++) {
		const ItemStruct * itm = &plr[p].InvBody[i];
		if (itm->_itype == -1) continue;
		if (! itm->_iStatFlag) continue;

		mind += itm->_iMinDam;
		maxd += itm->_iMaxDam;
		tac += itm->_iAC;

		t = 1;
		if (itm->_iSpell != 0)
			spl |= t << (itm->_iSpell - 1);

		// don't give benefit of unidentified magic items
		if (itm->_iMagical != IMAGIC_NONE && ! itm->_iIdentified)
			continue;

		bdam += itm->_iPLDam;
		btohit += itm->_iPLToHit;

		if (itm->_iPLAC) {
			int tmpac = (itm->_iAC * itm->_iPLAC) / 100;
			if (tmpac == 0) tmpac = 1;
			bac += tmpac; 
		}

		iflgs |= itm->_iFlags;
		iflgs2 |= itm->_iFlags2;
		sadd += itm->_iPLStr;
		madd += itm->_iPLMag;
		dadd += itm->_iPLDex;
		vadd += itm->_iPLVit;
		fr += itm->_iPLFR;
		lr += itm->_iPLLR;
		mr += itm->_iPLMR;
		dmod += itm->_iPLDamMod;
		ghit += itm->_iPLGetHit;
		lrad += itm->_iPLLight;
		ihp += itm->_iPLHP;
		imana += itm->_iPLMana;
		spllvladd += itm->_iSplLvlAdd;
		enac += itm->_iPLEnAc;
		fmin += itm->_iFMinDam;
		fmax += itm->_iFMaxDam;
		lmin += itm->_iLMinDam;
		lmax += itm->_iLMaxDam;
	}

	// If I have no weapons, then make it min of 1-1, if shield only, 1-3
	if (mind == 0 && maxd == 0) {
		mind = 1;
		maxd = 1;
		if ((plr[p].Hand1Item._itype == IT_SHIELD) && (plr[p].Hand1Item._iStatFlag))
			maxd = 3;
		if ((plr[p].Hand2Item._itype == IT_SHIELD) && (plr[p].Hand2Item._iStatFlag))
			maxd = 3;
		
		// monks have fists of fury.
		if (plr[p]._pClass == CLASS_MONK)
		{
			mind = max(mind,plr[p]._pLevel >> 1);
			maxd = max(maxd,plr[p]._pLevel);
		}
	}
	
	if ((plr[p]._pSpellFlags & SF_RAGE) == SF_RAGE) {
		sadd += 2 * plr[p]._pLevel;
		dadd += plr[p]._pLevel + (plr[p]._pLevel/2);
		vadd += 2 * plr[p]._pLevel;
	}

	if ((plr[p]._pSpellFlags & SF_LETHERGY) == SF_LETHERGY) {
		sadd -= 2 * plr[p]._pLevel;
		dadd -= plr[p]._pLevel + (plr[p]._pLevel/2);
		vadd -= 2 * plr[p]._pLevel;
	}

	plr[p]._pIMinDam = mind;
	plr[p]._pIMaxDam = maxd;
	plr[p]._pIAC = tac;
	plr[p]._pIBonusDam = bdam;
	plr[p]._pIBonusToHit = btohit;
	plr[p]._pIBonusAC = bac;
	plr[p]._pIFlags = iflgs;
	plr[p]._pIFlags2 = iflgs2;
	plr[p]._pIBonusDamMod = dmod;
	plr[p]._pIGetHit = ghit;
	if (lrad < 2) lrad = 2;
	if (lrad > 15) lrad = 15;
	if ((plr[p]._pLightRad != lrad ) && (p == myplr)) {
		ChangeLightRadius(plr[p]._plid, lrad);
		if (lrad < 10) ChangeVisionRadius(plr[p]._pvid, 10);
		else ChangeVisionRadius(plr[p]._pvid, lrad);
		plr[p]._pLightRad = lrad;
	}

	plr[p]._pStrength = plr[p]._pBaseStr + sadd;
	if (plr[myplr]._pStrength <= 0) plr[myplr]._pStrength = 0;
	plr[p]._pMagic = plr[p]._pBaseMag + madd;
	if (plr[myplr]._pMagic <= 0) plr[myplr]._pMagic = 0;
	plr[p]._pDexterity = plr[p]._pBaseDex + dadd;
	if (plr[myplr]._pDexterity <= 0) plr[myplr]._pDexterity = 0;
	plr[p]._pVitality = plr[p]._pBaseVit + vadd;
	if (plr[myplr]._pVitality <= 0) plr[myplr]._pVitality = 0;

	if (plr[p]._pClass == CLASS_ROGUE)
		plr[p]._pDamageMod = ((plr[p]._pStrength + plr[p]._pDexterity) * plr[p]._pLevel) / 200;
	else if (plr[p]._pClass == CLASS_MONK)
	{
		if (plr[p].Hand1Item._itype == IT_STAFF ||	// bonus for staff
		    plr[p].Hand2Item._itype == IT_STAFF ||
		    (plr[p].Hand1Item._itype == -1 &&		// bonus for no weapons.
		     plr[p].Hand2Item._itype == -1)
	    )
		{
			plr[p]._pDamageMod = ((plr[p]._pStrength + plr[p]._pDexterity) * plr[p]._pLevel) / 150;
		}
		else
		{
			plr[p]._pDamageMod = ((plr[p]._pStrength + plr[p]._pDexterity) * plr[p]._pLevel) / 300;
		}
	}
	else if (plr[p]._pClass == CLASS_BARD)
	{
		if (plr[p].Hand1Item._itype == IT_SWORD			// bonus for using a sword.
			|| plr[p].Hand2Item._itype == IT_SWORD)
		{
			plr[p]._pDamageMod = ((plr[p]._pStrength + plr[p]._pDexterity) * plr[p]._pLevel) / 150;
		}
		else if (plr[p].Hand1Item._itype == IT_BOW
		         || plr[p].Hand2Item._itype == IT_BOW)
		{
			// Better than a monk or warrior with a bow but not as good as a rogue.
			plr[p]._pDamageMod = ((plr[p]._pStrength + plr[p]._pDexterity) * plr[p]._pLevel) / 250;
		}
		else
		{
			plr[p]._pDamageMod = (plr[p]._pStrength * plr[p]._pLevel) / 100;
		}
	}
	else if (plr[p]._pClass == CLASS_BARBARIAN)
	{
		if (plr[p].Hand1Item._itype == IT_AXE
			|| plr[p].Hand2Item._itype == IT_AXE)
		{
			plr[p]._pDamageMod = (plr[p]._pStrength * plr[p]._pLevel) / 75;
		}
		else if (plr[p].Hand1Item._itype == IT_MACE
			|| plr[p].Hand2Item._itype == IT_MACE)
		{
			plr[p]._pDamageMod = (plr[p]._pStrength * plr[p]._pLevel) / 75;
		}
		else if (plr[p].Hand1Item._itype == IT_BOW
		         || plr[p].Hand2Item._itype == IT_BOW)
		{
			// As bad as it gets
			plr[p]._pDamageMod = (plr[p]._pStrength * plr[p]._pLevel) / 300;
		}
		else
		{
			plr[p]._pDamageMod = (plr[p]._pStrength * plr[p]._pLevel) / 100;
		}
		
		if (plr[p].Hand1Item._itype == IT_SHIELD || plr[p].Hand2Item._itype == IT_SHIELD)
		{
			// remove 1/2 the AC for the shield.
			if (plr[p].Hand1Item._itype == IT_SHIELD)
			{
				 plr[p]._pIAC -= plr[p].Hand1Item._iAC / 2;
			}
			else if (plr[p].Hand2Item._itype == IT_SHIELD)
			{
				 plr[p]._pIAC -= plr[p].Hand2Item._iAC / 2;
			}
		}
		else
		{
			// Bonus for not using a shield.
			if ( !(plr[p].Hand1Item._itype == IT_STAFF || plr[p].Hand2Item._itype == IT_STAFF ||
			       plr[p].Hand1Item._itype == IT_BOW   || plr[p].Hand2Item._itype == IT_BOW)
				)
			{
				plr[p]._pDamageMod += (plr[p]._pVitality * plr[p]._pLevel) / 100;
			}
		}
		// Barbarians have a natural armor (Thick skin)
		plr[p]._pIAC += plr[p]._pLevel/4;
	}
	else
		plr[p]._pDamageMod = (plr[p]._pStrength * plr[p]._pLevel) / 100;

	plr[p]._pISpells = spl;
	if ((plr[p]._pRSplType == SPT_ITEM) /*|| plr[p]._pRSplType == SPT_SCROLL)*/
		&& ((plr[p]._pISpells & (((__int64)1) << (plr[p]._pRSpell-1))) == 0)) {
		plr[p]._pRSpell = -1;
		plr[p]._pRSplType = SPT_NONE;
		force_redraw = FULLDRAW;
	}
	plr[p]._pISplLvlAdd = spllvladd;

	plr[p]._pIEnAc = enac;

	if (plr[p]._pClass == CLASS_BARBARIAN)	// Barbarians have a natural bonus.
	{
		mr += plr[p]._pLevel;
		fr += plr[p]._pLevel;
		lr += plr[p]._pLevel;
	}
	
	if ((plr[p]._pSpellFlags & SF_LETHERGY) == SF_LETHERGY) // Lethergy causes a loss of strength
	{
		mr -= plr[p]._pLevel;
		fr -= plr[p]._pLevel;
		lr -= plr[p]._pLevel;
	}
	
	if (iflgs & IAF_ZERORES) {
		mr = 0;
		fr = 0;
		lr = 0;
	}
	
	if (mr > RESIST_MAX) mr = RESIST_MAX;
	else if (mr < 0) mr = 0;
	plr[p]._pMagResist = mr;
	
	if (fr > RESIST_MAX) fr = RESIST_MAX;
	else if (fr < 0) fr = 0;
	plr[p]._pFireResist = fr;
	
	if (lr > RESIST_MAX) lr = RESIST_MAX;
	else if (lr < 0) lr = 0;
	plr[p]._pLghtResist = lr;

	if (plr[p]._pClass == CLASS_WARRIOR)
	{
		vadd = vadd << 1;
	}
	else if (plr[p]._pClass == CLASS_BARBARIAN)
	{
		vadd *= 2;
		vadd += (vadd >> 2);
	}
	else if (plr[p]._pClass == CLASS_ROGUE ||
		plr[p]._pClass == CLASS_MONK ||
		plr[p]._pClass == CLASS_BARD )
	{
		vadd += (vadd >> 1);
	}
	ihp += (vadd << HP_SHIFT);
	if (plr[p]._pClass == CLASS_SORCEROR) 
		madd = madd << 1;	// 2 * madd
	if (plr[p]._pClass == CLASS_ROGUE ||
	    plr[p]._pClass == CLASS_MONK )
		madd += (madd >> 1);	// 1.5 * madd
	else if ( plr[p]._pClass == CLASS_BARD ) 
		madd += (madd >> 1) + (madd >> 2);	// 1.75 * madd
		
	imana += (madd << MANA_SHIFT);

	plr[p]._pHitPoints = plr[p]._pHPBase + ihp;
	plr[p]._pMaxHP = plr[p]._pMaxHPBase + ihp;
	
	if (plr[p]._pHitPoints > plr[p]._pMaxHP)
		plr[p]._pHitPoints = plr[p]._pMaxHP;

	if ((p == myplr) && (plr[p]._pHitPoints >> HP_SHIFT) <= 0)
		SetPlayerHitPoints(p, 0);

	plr[p]._pMana = plr[p]._pManaBase + imana;
	plr[p]._pMaxMana = plr[p]._pMaxManaBase + imana;
	
	if (plr[p]._pMana > plr[p]._pMaxMana)
		plr[p]._pMana = plr[p]._pMaxMana;
		
	plr[p]._pIFMinDam = fmin;
	plr[p]._pIFMaxDam = fmax;
	plr[p]._pILMinDam = lmin;
	plr[p]._pILMaxDam = lmax;
	

	if (iflgs & IAF_INFRAVISION) plr[p]._pInfraFlag = TRUE;
	else plr[p]._pInfraFlag = FALSE;

	plr[p]._pBlockFlag = FALSE;
	if (plr[p]._pClass == CLASS_MONK)
	{
		if ((plr[p].Hand1Item._itype == IT_STAFF) &&
			(plr[p].Hand1Item._iStatFlag))
		{
			plr[p]._pBlockFlag = TRUE;
			plr[p]._pIFlags |= IAF_BLANIM;
		}
		
		if ((plr[p].Hand2Item._itype == IT_STAFF) &&
			(plr[p].Hand2Item._iStatFlag))
		{
			plr[p]._pBlockFlag = TRUE;
			plr[p]._pIFlags |= IAF_BLANIM;
		}
		
		if ((plr[p].Hand1Item._itype == -1) &&
			(plr[p].Hand2Item._itype == -1))
		{
			plr[p]._pBlockFlag = TRUE;
		}
			
		if ((plr[p].Hand1Item._iClass == IC_WEAP) &&
		    (plr[p].Hand1Item._iLoc != IL_2HAND) &&
		    (plr[p].Hand2Item._itype == -1))
		{
			plr[p]._pBlockFlag = TRUE;
		}
		    
		if ((plr[p].Hand2Item._iClass == IC_WEAP) &&
		    (plr[p].Hand2Item._iLoc != IL_2HAND) &&
		    (plr[p].Hand1Item._itype == -1))
	    {
			plr[p]._pBlockFlag = TRUE;
		}
	}
#if 0
	// No Block art
	else if (plr[p]._pClass == CLASS_BARD)
	{
		if ((plr[p].Hand1Item._itype == IT_SWORD) &&
			(plr[p].Hand1Item._iStatFlag))
		{
			plr[p]._pBlockFlag = TRUE;
			plr[p]._pIFlags |= IAF_BLANIM;
		}
		
		if ((plr[p].Hand2Item._itype == IT_SWORD) &&
			(plr[p].Hand2Item._iStatFlag))
		{
			plr[p]._pBlockFlag = TRUE;
			plr[p]._pIFlags |= IAF_BLANIM;
		}
	}
#endif
	
	plr[p]._pwtype = WEAP_H2H;
	// Determine which graphics to use
	g = PGFX_NGUY;
	if ((plr[p].Hand1Item._itype != -1) &&
		(plr[p].Hand1Item._iClass == IC_WEAP) &&
		(plr[p].Hand1Item._iStatFlag)) g = plr[p].Hand1Item._itype;
	if ((plr[p].Hand2Item._itype != -1) &&
		(plr[p].Hand2Item._iClass == IC_WEAP) &&
		(plr[p].Hand2Item._iStatFlag)) g = plr[p].Hand2Item._itype;
	switch (g) {
		case IT_SWORD:
			g = PGFX_XGUY;
			break;
		case IT_MACE:
			g = PGFX_ZGUY;
			break;
		case IT_BOW:
			plr[p]._pwtype = WEAP_RANGE;
			g = PGFX_BGUY;
			break;
		case IT_AXE:
			g = PGFX_FGUY;
			break;
		case IT_STAFF:
			g = PGFX_TGUY;
			break;
	}
	if ((plr[p].Hand1Item._itype == IT_SHIELD) && (plr[p].Hand1Item._iStatFlag)) {
		plr[p]._pBlockFlag = TRUE;
		g++;
	}
	if ((plr[p].Hand2Item._itype == IT_SHIELD) && (plr[p].Hand2Item._iStatFlag)) {
		plr[p]._pBlockFlag = TRUE;
		g++;
	}

	#if IS_VERSION(RETAIL) || IS_VERSION(BETA)
		if ((plr[p].BodyItem._itype == IT_HARMOR) && (plr[p].BodyItem._iStatFlag))
		{
			if ((plr[p]._pClass == CLASS_MONK) &&
				(plr[p].BodyItem._iMagical == IMAGIC_UNIQUE))
				plr[p]._pIAC += plr[p]._pLevel >> 1;	// Monks get 1/2 level bonus while wearing medium armor.
			g += 32;
		}
		else if ((plr[p].BodyItem._itype == IT_MARMOR) && (plr[p].BodyItem._iStatFlag))
		{ 
			if (plr[p]._pClass == CLASS_MONK)
			{
				if (plr[p].BodyItem._iMagical == IMAGIC_UNIQUE)
					plr[p]._pIAC += plr[p]._pLevel << 1;	// Monks get 2x level bonus while wearing light armor.
				else
					plr[p]._pIAC += plr[p]._pLevel >> 1;	// Monks get 1/2 level bonus while wearing medium armor.
			}
			g += 16;
		}
		else
		{
			if (plr[p]._pClass == CLASS_MONK)
			{
				plr[p]._pIAC += plr[p]._pLevel << 1;	// Monks get 2x level bonus for light/no armor.
			}
		}
	#endif

	if ((plr[p]._pgfxnum != g) && (Loadgfx)) {
		plr[p]._pgfxnum = g;
		plr[p]._pGFXLoad = 0;		// Clear all graphics loaded flag
		LoadPlrGFX(p, PGL_STAND);
		SetPlrAnims(p);
		d = plr[p]._pdir;

		app_assert(plr[p]._pNAnim[d]);
		plr[p]._pAnimData = plr[p]._pNAnim[d];
		plr[p]._pAnimLen = plr[p]._pNFrames;
		plr[p]._pAnimFrame = 1;
		plr[p]._pAnimCnt = 0;
		plr[p]._pAnimDelay = 3;
		plr[p]._pAnimWidth = plr[p]._pNWidth;
		plr[p]._pAnimWidth2 = (plr[p]._pNWidth - 64) >> 1;
	}
	else {
		plr[p]._pgfxnum = g;
	}

	for (i = 0; i < nummissiles; i++) {
		mi = missileactive[i];
		if (missile[mi]._mitype == MIT_MANASHIELD && missile[mi]._misource == p) {
			missile[mi]._miVar1 = plr[p]._pHitPoints;
			missile[mi]._miVar2 = plr[p]._pHPBase;
			break;
		}
	}
	
	if (plr[p].NeckItem._itype != -1 && plr[p].NeckItem.IDidx == IDI_AURIC)
	{
		GOLD_VMAX = GOLD_DOUBLE_VMAX;
	}
	else
	{
		int const old_vmax = GOLD_VMAX;
		
		GOLD_VMAX = GOLD_DOUBLE_VMAX/2;
		
		if (old_vmax != GOLD_VMAX)
			StripTopGold(p);
	}

	drawmanaflag = TRUE;
	drawhpflag = TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void CalcPlrScrolls(int p) {
	int i;
	__int64 t;

	plr[p]._pScrlSpells = 0;
	for (i = 0; i < plr[p]._pNumInv; i++) {
		if (plr[p].InvList[i]._itype != -1) {
			if (((plr[p].InvList[i]._iMiscId == IMID_SCROLL) ||
				(plr[p].InvList[i]._iMiscId == IMID_TSCROLL)) &&
				(plr[p].InvList[i]._iStatFlag)) {
				t = 1;
				plr[p]._pScrlSpells |= (t << plr[p].InvList[i]._iSpell-1);
			}
		}
	}
	for (i = 0; i < MAXSPD; i++) {
		if (plr[p].SpdList[i]._itype != -1) {
			if (((plr[p].SpdList[i]._iMiscId == IMID_SCROLL) ||
				(plr[p].SpdList[i]._iMiscId == IMID_TSCROLL)) &&
				(plr[p].SpdList[i]._iStatFlag)) {
				t = 1;
				plr[p]._pScrlSpells |= (t << plr[p].SpdList[i]._iSpell-1);
			}
		}
	}

	if ((plr[p]._pRSplType == SPT_SCROLL)
		&& ((plr[p]._pScrlSpells & (1 << (plr[p]._pRSpell-1))) == 0)) {
		plr[p]._pRSpell = -1;
		plr[p]._pRSplType = SPT_NONE;
		force_redraw = FULLDRAW;
	}

}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void CalcPlrStaff(int p) {
	plr[p]._pISpells = 0;
	if (plr[p].Hand1Item._itype == -1) return;
	if (!plr[p].Hand1Item._iStatFlag) return;
	if (plr[p].Hand1Item._iCharges > 0) {
		__int64 t = 1;
		plr[p]._pISpells |= t << (plr[p].Hand1Item._iSpell - 1);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void CalcSelfItems(int pnum) {
	int i;
	ItemStruct * pi;
	PlayerStruct * p = &plr[pnum];
	BOOL sf, changeflag;
	int sa = 0;
	int ma = 0;
	int da = 0;

	// assume everything works, and calc potential + to str, mag, dex
	pi = &p->InvBody[0];
	for (i = NUM_INVLOC; i--; pi++) {
		if (pi->_itype == -1) continue;
		pi->_iStatFlag = TRUE;
		if (!pi->_iIdentified) continue;
		sa += pi->_iPLStr;
		ma += pi->_iPLMag;
		da += pi->_iPLDex;
	}

	do {
		changeflag = FALSE;
		pi = &p->InvBody[0];
		for (i = NUM_INVLOC; i--; pi++) {
			if (pi->_itype == -1) continue;
			if (!pi->_iStatFlag) continue;
			sf = TRUE;
			if ((p->_pBaseStr + sa) < pi->_iMinStr) sf = FALSE;
			if ((p->_pBaseMag + ma) < pi->_iMinMag) sf = FALSE;
			if ((p->_pBaseDex + da) < pi->_iMinDex) sf = FALSE;
			if (!sf) {
				changeflag = TRUE;
				pi->_iStatFlag = FALSE;
				if (pi->_iIdentified) {
					sa -= pi->_iPLStr;
					ma -= pi->_iPLMag;
					da -= pi->_iPLDex;
				}
			}
		}
	} while (changeflag);

}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static BOOL ItemMinStats(const PlayerStruct * p,const ItemStruct * x) {
	if (p->_pMagic < (byte)x->_iMinMag) return FALSE;
	if (p->_pStrength < x->_iMinStr) return FALSE;
	if (p->_pDexterity < x->_iMinDex) return FALSE;
	return TRUE;
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void CalcPlrItemMin(int pnum) {
	int i;
	ItemStruct * pi;
	PlayerStruct * p = &plr[pnum];

	// handle inventory
	pi = &p->InvList[0];
	for (i = p->_pNumInv; i--; pi++)
		pi->_iStatFlag = ItemMinStats(p,pi);

	// handle speedbar
	pi = &p->SpdList[0];
	for (i = MAXSPD; i--; pi++) {
		if (pi->_itype == -1) continue;
		pi->_iStatFlag = ItemMinStats(p,pi);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void CalcPlrBookVals(int p)
{
	int i, slvl;

	void WitchBookLevel(int);
	if (currlevel == 0) {
		for (i = 1; witchitem[i]._itype != -1; i++)
			WitchBookLevel(i);
	}

	for (i = 0; i < plr[p]._pNumInv; i++) {
		if ((plr[p].InvList[i]._itype == IT_MISC) && (plr[p].InvList[i]._iMiscId == IMID_BOOK)) {
			plr[p].InvList[i]._iMinMag = spelldata[plr[p].InvList[i]._iSpell].sMinInt;
			slvl = plr[p]._pSplLvl[plr[p].InvList[i]._iSpell];
			while (slvl != 0) {
				plr[p].InvList[i]._iMinMag += ((plr[p].InvList[i]._iMinMag * 20) / 100);
				slvl--;
				if ((plr[p].InvList[i]._iMinMag + ((plr[p].InvList[i]._iMinMag * 20) / 100)) > 255) {
					plr[p].InvList[i]._iMinMag = 255;
					slvl = 0;
				}
			}
			plr[p].InvList[i]._iStatFlag = ItemMinStats(&plr[p], &plr[p].InvList[i]);
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void CalcPlrInv(int p, BOOL Loadgfx) {
	CalcPlrItemMin(p);
	CalcSelfItems(p);
	CalcPlrItemVals(p, Loadgfx);
	CalcPlrItemMin(p);
	if (p == myplr) {
		CalcPlrBookVals(p);
		CalcPlrScrolls(p);
		CalcPlrStaff(p);
	}
	if ((p == myplr) && (currlevel == 0)) RecalcStoreStats();
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void SetPlrHandItem(ItemStruct *h, int idata) {
	const ItemDataStruct * pAllItem = &AllItemsList[idata];

	ZeroMemory(h,sizeof(ItemStruct));
	h->_itype = pAllItem->itype;
	h->_iCurs = pAllItem->iCurs;
	strcpy(h->_iName, pAllItem->iName);
	strcpy(h->_iIName, pAllItem->iName);
	h->_iLoc = pAllItem->iLoc;
	h->_iClass = pAllItem->iClass;
	h->_iMinDam = pAllItem->iMinDam;
	h->_iMaxDam = pAllItem->iMaxDam;
	h->_iAC = pAllItem->iMinAC;
	h->_iMiscId = pAllItem->iMiscId;
	h->_iSpell = pAllItem->iSpell;
	if (pAllItem->iMiscId == IMID_STAFF) h->_iCharges = 18;  // was 40
	h->_iMaxCharges = h->_iCharges;
	h->_iDurability = pAllItem->iDurability;
	h->_iMaxDur = pAllItem->iDurability;
	h->_iMinStr = pAllItem->iMinStr;
	h->_iMinMag = pAllItem->iMinMag;
	h->_iMinDex = pAllItem->iMinDex;
	h->_ivalue = pAllItem->iValue;
	h->_iIvalue = pAllItem->iValue;
	h->_iPrePower = -1;
	h->_iSufPower = -1;
	h->IDidx = idata;
	h->_iMagical = IMAGIC_NONE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void GetPlrHandSeed(ItemStruct *h)
{
	h->_iSeed = GetRndSeed();
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void GetGoldSeed(int pnum, ItemStruct *h)
{
	int i, ii, s;
	BOOL doneflag;

	do {
		doneflag = TRUE;
		s = GetRndSeed();
		for (i = 0; i < numitems; i++) {
			ii = itemactive[i];
			if (item[ii]._iSeed == s) doneflag = FALSE;
		}
		if (pnum == myplr) {
			for (i = 0; i < plr[pnum]._pNumInv; i++)
				if (plr[pnum].InvList[i]._iSeed == s) doneflag = FALSE;
		}
	} while (!doneflag);
	h->_iSeed = s;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void SetPlrHandSeed(ItemStruct *h, int iseed)
{
	h->_iSeed = iseed;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void SetPlrHandGoldCurs(ItemStruct *h)
{
	if (h->_ivalue >= GOLD_VT2) {
		h->_iCurs = ITEM_5GOLD;
	} else {
		if (h->_ivalue <= GOLD_VT1) h->_iCurs = ITEM_1GOLD;
		else h->_iCurs = ITEM_3GOLD;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void CreatePlrItems(int p) {
	int i;
	ItemStruct * pi;

	// zero out carried items
	pi = &plr[p].InvBody[0];
	for (i = NUM_INVLOC; i--; pi++)
		pi->_itype = -1;

	// zero inventory
	ZeroMemory(plr[p].InvGrid,sizeof(plr[p].InvGrid));
	pi = &plr[p].InvList[0];
	for (i = MAXINV; i--; pi++)
		pi->_itype = -1;
	plr[p]._pNumInv = 0;

	// zero speedbar
	pi = &plr[p].SpdList[0];
	for (i = MAXSPD; i--; pi++)
		pi->_itype = -1;

	switch (plr[p]._pClass) {
		case CLASS_WARRIOR :
			// Sword in left
			SetPlrHandItem(&plr[p].Hand1Item, IDI_WARRIOR);
			GetPlrHandSeed(&plr[p].Hand1Item);

			// Shield in right
			SetPlrHandItem(&plr[p].Hand2Item, IDI_WARRSHLD);
			GetPlrHandSeed(&plr[p].Hand2Item);

#if CHEATS
			if (!davecheat) {
				SetPlrHandItem(&plr[p].HoldItem, IDI_WARRCLUB);
				GetPlrHandSeed(&plr[p].HoldItem);
				AutoPlace(p, 0, 1, 3, TRUE);
			}
#else
			// Club in inv
			SetPlrHandItem(&plr[p].HoldItem, IDI_WARRCLUB);
			GetPlrHandSeed(&plr[p].HoldItem);
			AutoPlace(p, 0, 1, 3, TRUE);
#endif
			SetPlrHandItem(&plr[p].SpdList[0], IDI_HEAL);
			GetPlrHandSeed(&plr[p].SpdList[0]);
			SetPlrHandItem(&plr[p].SpdList[1], IDI_HEAL);
			GetPlrHandSeed(&plr[p].SpdList[1]);
		break;

		case CLASS_ROGUE :
		#if !IS_VERSION(SHAREWARE)
			// Bow in both hands
			SetPlrHandItem(&plr[p].Hand1Item, IDI_ROGUE);
			GetPlrHandSeed(&plr[p].Hand1Item);
			SetPlrHandItem(&plr[p].SpdList[0], IDI_HEAL);
			GetPlrHandSeed(&plr[p].SpdList[0]);
			SetPlrHandItem(&plr[p].SpdList[1], IDI_HEAL);
			GetPlrHandSeed(&plr[p].SpdList[1]);
		#endif
		break;

		case CLASS_SORCEROR :
		#if !IS_VERSION(SHAREWARE)
			// Staff in both hands
			SetPlrHandItem(&plr[p].Hand1Item, IDI_SORCEROR);
			GetPlrHandSeed(&plr[p].Hand1Item);
			SetPlrHandItem(&plr[p].SpdList[0], IDI_HEAL); // was mana
			GetPlrHandSeed(&plr[p].SpdList[0]);
			SetPlrHandItem(&plr[p].SpdList[1], IDI_HEAL); // was mana
			GetPlrHandSeed(&plr[p].SpdList[1]);
		#endif
		break;
		
		case CLASS_MONK :
		#if !IS_VERSION(SHAREWARE)
			// Bow in both hands
			SetPlrHandItem(&plr[p].Hand1Item, IDI_MONK);
			GetPlrHandSeed(&plr[p].Hand1Item);
			SetPlrHandItem(&plr[p].SpdList[0], IDI_HEAL);
			GetPlrHandSeed(&plr[p].SpdList[0]);
			SetPlrHandItem(&plr[p].SpdList[1], IDI_HEAL);
			GetPlrHandSeed(&plr[p].SpdList[1]);
		#endif
		break;

		case CLASS_BARD :
		#if !IS_VERSION(SHAREWARE)
			// Bow in both hands
			SetPlrHandItem(&plr[p].Hand1Item, IDI_BARD);
			GetPlrHandSeed(&plr[p].Hand1Item);
			SetPlrHandItem(&plr[p].Hand2Item, IDI_BARDDAGGER);
			GetPlrHandSeed(&plr[p].Hand2Item);
			SetPlrHandItem(&plr[p].SpdList[0], IDI_HEAL);
			GetPlrHandSeed(&plr[p].SpdList[0]);
			SetPlrHandItem(&plr[p].SpdList[1], IDI_HEAL);
			GetPlrHandSeed(&plr[p].SpdList[1]);
		#endif
		break;

		case CLASS_BARBARIAN :
		#if !IS_VERSION(SHAREWARE)
			// Club in left
			SetPlrHandItem(&plr[p].Hand1Item, IDI_BARBARIAN);
			GetPlrHandSeed(&plr[p].Hand1Item);

			// Shield in right
			SetPlrHandItem(&plr[p].Hand2Item, IDI_BARSHLD);
			GetPlrHandSeed(&plr[p].Hand2Item);
			SetPlrHandItem(&plr[p].SpdList[0], IDI_HEAL);
			GetPlrHandSeed(&plr[p].SpdList[0]);
			SetPlrHandItem(&plr[p].SpdList[1], IDI_HEAL);
			GetPlrHandSeed(&plr[p].SpdList[1]);
		#endif
		break;

	}

	SetPlrHandItem(&plr[p].HoldItem, IDI_GOLD);
	GetPlrHandSeed(&plr[p].HoldItem);
#if CHEATS
	if (!davecheat) {
		// 100 Gold
		plr[p].HoldItem._ivalue = 100;
		plr[p].HoldItem._iCurs = ITEM_1GOLD;
		plr[p]._pGold = plr[p].HoldItem._ivalue;
		i = plr[p]._pNumInv;
		plr[p].InvList[i] = plr[p].HoldItem;
		plr[p]._pNumInv++;
		plr[p].InvGrid[30] = plr[p]._pNumInv;
	}
	else {
		// 200000 Gold
		plr[p].HoldItem._ivalue = 5000;
		plr[p].HoldItem._iCurs = ITEM_5GOLD;
		plr[p]._pGold = 200000;
		for (int j = 0; j < 40; j++) {
			GetGoldSeed(p, &plr[p].HoldItem);
			i = plr[p]._pNumInv;
			plr[p].InvList[i] = plr[p].HoldItem;
			plr[p]._pNumInv++;
			plr[p].InvGrid[j] = plr[p]._pNumInv;
		}
	}
#else
	// 100 Gold
	plr[p].HoldItem._ivalue = 100;
	plr[p].HoldItem._iCurs = ITEM_1GOLD;
	plr[p]._pGold = plr[p].HoldItem._ivalue;
	i = plr[p]._pNumInv;
	plr[p].InvList[i] = plr[p].HoldItem;
	plr[p]._pNumInv++;
	plr[p].InvGrid[30] = plr[p]._pNumInv;
#endif

	CalcPlrItemVals(p,FALSE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL ItemSpaceOk(int i, int j)
{
	int pn, oi;

	if ((i < 0) || (i >= DMAXX) || (j < 0) || (j >= DMAXY)) return(FALSE);
	if (dMonster[i][j] != 0) return(FALSE);
	if (dPlayer[i][j] != 0) return(FALSE);
	if (dItem[i][j] != 0) return(FALSE);
	if (dObject[i][j] != 0) {
		if (dObject[i][j] > 0) oi = dObject[i][j]-1;
		else oi = -(dObject[i][j]+1);
		if (object[oi]._oSolidFlag) return(FALSE);
	}
	if (dObject[i+1][j+1] > 0) {
		oi = dObject[i+1][j+1]-1;
		if (object[oi]._oSelFlag != OSEL_NONE) return(FALSE);
	}
	if (dObject[i+1][j+1] < 0) {
		oi = -(dObject[i+1][j+1]+1);
		if (object[oi]._oSelFlag != OSEL_NONE) return(FALSE);
	}
	if ((dObject[i+1][j] > 0) && (dObject[i][j+1] > 0)) {
		oi = dObject[i+1][j]-1;
		if (object[oi]._oSelFlag != OSEL_NONE) {
			oi = dObject[i][j+1]-1;
			if (object[oi]._oSelFlag != OSEL_NONE) return(FALSE);
		}
	}
	pn = dPiece[i][j];
	if (nSolidTable[pn]) return(FALSE);
	return(TRUE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*

#if CHEATS
void DaveISpaceOk(int inum)
{
	int i,j;

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (dItem[i][j] == inum+1)
				app_fatal("Item in map already");
		}
	}
}
#endif


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL GetItemSpace(int x, int y, char inum)
{
	int i,j,xx,yy,rs;
	BOOL savail;

	// Try surrounding squares
	yy = 0;
	for (j = (y-1); j <= (y+1); j++) {
		xx = 0;
		for (i = (x-1); i <= (x+1); i++) {
			itemhold[xx][yy] = ItemSpaceOk(i,j);
			xx++;
		}
		yy++;
	}

	savail = FALSE;
	for (yy = 0; yy < 3; yy++) {
		for (xx = 0; xx < 3; xx++) {
			if (itemhold[xx][yy]) savail = TRUE;
		}
	}

	// Must go here so same number of rnd calls on multiplayer machines
	rs = random(13, 15) + 1;

	// No fit, no good
	if (!savail) return(FALSE);

	// Place item
	xx = 0;
	yy = 0;
	while (rs > 0) {
		if (itemhold[xx][yy]) rs--;
		if (rs > 0) {
			xx++;
			if (xx == 3) {
				xx = 0;
				yy++;
				if (yy == 3) yy = 0;
			}
		}
	}
	xx = xx + x - 1;
	yy = yy + y - 1;
	item[inum]._ix = xx;
	item[inum]._iy = yy;
	//DaveISpaceOk(inum);
	dItem[xx][yy] = inum + 1;
	return(TRUE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void GetSuperItemSpace(int x, int y, char inum)
{
	// try normal method first
	if (GetItemSpace(x, y, inum)) return;

	int xx, yy;

	// radial search outward until a space is found
	for (int l = 2; l < 50; l++) {
		for (int j = -l; j <= l; j++) {
			yy = y + j;
			for (int i = -l; i <= l; i++) {
				xx = x + i;
				if (! ItemSpaceOk(xx,yy)) continue;

				// drop it
				item[inum]._ix = xx;
				item[inum]._iy = yy;
				//DaveISpaceOk(inum);
				dItem[xx][yy] = inum + 1;
				return;
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void GetSuperItemLoc(int x, int y, int &xx, int &yy)
{
	// radial search outward until a space is found
	for (int l = 1; l < 50; l++) {
		for (int j = -l; j <= l; j++) {
			yy = y + j;
			for (int i = -l; i <= l; i++) {
				xx = x + i;
				if (ItemSpaceOk(xx,yy))		// found it
					return;
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void CalcItemValue(int i)
{
	int v = item[i]._iVMult1 + item[i]._iVMult2;
	if (v > 0) v = item[i]._ivalue * v;
	if (v < 0) v = item[i]._ivalue / v;
	v += item[i]._iVAdd1 + item[i]._iVAdd2;
	if (v <= 0) v = 1;
	item[i]._iIvalue = v;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void GetBookSpell(int i, int lvl)
{

	if (lvl == 0) lvl = 1;
	int rv = random(14, SPL_LAST) + 1;

	#if IS_VERSION(SHAREWARE)
	if (lvl > 5) lvl = 5;
	#endif

	int bs = SPL_FIREBOLT;
	int s = SPL_FIREBOLT;
	while (rv > 0) {
		if ((spelldata[s].sBookLvl != -1) && (lvl >= spelldata[s].sBookLvl)) {
			rv--;
			bs = s;
		}

		s++;
		if ((gbMaxPlayers == 1) && (s == SPL_RESURRECT)) s++;
		if ((gbMaxPlayers == 1) && (s == SPL_HEALOTHER)) s++;
		if (s == SPL_LAST) s = SPL_FIREBOLT;
	}
	strcat(item[i]._iName, spelldata[bs].sNameText);
	strcat(item[i]._iIName, spelldata[bs].sNameText);
	item[i]._iSpell = bs;
	item[i]._iMinMag = spelldata[bs].sMinInt;
	item[i]._ivalue += spelldata[bs].sBookCost;
	item[i]._iIvalue += spelldata[bs].sBookCost;
	if (spelldata[bs].sType == ST_FIRE) item[i]._iCurs = ITEM_BOOK3;
	else if (spelldata[bs].sType == ST_LIGHT) item[i]._iCurs = ITEM_BOOK;
	else if (spelldata[bs].sType == ST_MISC) item[i]._iCurs = ITEM_BOOK2;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void GetStaffPower(int i, int lvl, int bs, BOOL onlygood)
{
	int pre;
	int l[256], nl;
	int j, aii;
	int preidx;
	char istr[128];
	BOOL addok;

	// Prefix only (for a staff w/ spell)
	pre = random(15, 10);

	preidx = -1;

#if CHEATS
	if ((pre == 0) || (cheatflag) || (onlygood)) {
#else
	if ((pre == 0) || (onlygood)) {
#endif
		nl = 0;
		for (j = 0; PL_Prefix[j].PLPower != -1; j++) {
			if (((PL_Prefix[j].PLIType & PLF_STAFF) != 0) && (PL_Prefix[j].PLMinLvl <= lvl)) {
				addok = TRUE;
				if ((onlygood) && (!PL_Prefix[j].PLOk)) addok = FALSE;

				if (addok) {
					l[nl] = j;
					nl++;
					if (PL_Prefix[j].PLDouble) {
						l[nl] = j;
						nl++;
					}
				}
			}
		}
		if (nl != 0) {
			preidx = l[random(16, nl)];
			sprintf(istr, "%s %s", PL_Prefix[preidx].PLName, item[i]._iIName);
			strcpy(item[i]._iIName, istr);
			item[i]._iMagical = IMAGIC_MAGIC;
			SaveItemPower(i, PL_Prefix[preidx].PLPower, PL_Prefix[preidx].PLParam1, PL_Prefix[preidx].PLParam2, PL_Prefix[preidx].PLMinVal, PL_Prefix[preidx].PLMaxVal, PL_Prefix[preidx].PLMultVal);
			item[i]._iPrePower = PL_Prefix[preidx].PLPower;
		}
	}
	if (!InfoFit(item[i]._iIName)) {
		aii = item[i].IDidx;
		strcpy(item[i]._iIName, AllItemsList[aii].iSName);
		if (preidx != -1) {
			sprintf(istr, "%s %s", PL_Prefix[preidx].PLName, item[i]._iIName);
			strcpy(item[i]._iIName, istr);
		}
		sprintf(istr, "%s of %s", item[i]._iIName, spelldata[bs].sNameText);
		strcpy(item[i]._iIName, istr);
		if (!item[i]._iMagical) strcpy(item[i]._iName, item[i]._iIName);
	}
	CalcItemValue(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void GetStaffSpell(int i, int lvl, BOOL onlygood)
{
	int rv, s, bs, l;
	char tstr[64];
	int maxc, minc;

	//if (random(17, 4) == 0) GetItemPower(i, lvl >> 1, lvl, PLF_STAFF, onlygood);
	//else 
//{
	l = lvl >> 1;
	if (l == 0) l = 1;
	rv = random(18, SPL_LAST) + 1;

	#if IS_VERSION(SHAREWARE)
	if (lvl > 10) lvl = 10;
	#endif

	s = SPL_FIREBOLT;
	while (rv > 0) {
		if ((spelldata[s].sStaffLvl != -1) && (l >= spelldata[s].sStaffLvl)) {
			rv--;
			bs = s;
		}
		s++;
		if ((gbMaxPlayers == 1) && (s == SPL_RESURRECT)) s++;
		if ((gbMaxPlayers == 1) && (s == SPL_HEALOTHER)) s++;
		if (s == SPL_LAST) s = SPL_FIREBOLT;
	}
	sprintf(tstr, "%s of %s", item[i]._iName, spelldata[bs].sNameText);
	//Check to see if string will fit
	if (!InfoFit(tstr)) {
		sprintf(tstr, "Staff of %s", spelldata[bs].sNameText);
	}
	strcpy(item[i]._iName, tstr);
	strcpy(item[i]._iIName, tstr);
	item[i]._iSpell = bs;
	minc = spelldata[bs].sStaffMin;
	maxc = spelldata[bs].sStaffMax;
	item[i]._iCharges = random(19, maxc-minc+1) + minc;
	item[i]._iMaxCharges = item[i]._iCharges;
	item[i]._iMinMag = spelldata[bs].sMinInt;
	int v = (spelldata[bs].sStaffCost * item[i]._iCharges) / 5;
	item[i]._ivalue += v;
	item[i]._iIvalue += v;
	GetStaffPower(i, lvl, bs, onlygood);
//}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void GetOilType(int i, int lvl)
{
	int n, ro, roi, j;
	char OilIndexList[30];

	if (gbMaxPlayers == 1) {
		if (lvl == 0) lvl = 1;
		n = 0;
		for (j=0; j < MAXOIL; j++) {
			if (OilLvlTbl[j] <= lvl) {
				OilIndexList[n] = j;
				n++;
			}
		}
		ro = random(165, n);
		roi = OilIndexList[ro];
	} else {
		ro = random(165, 2);
		if (ro == 0) roi = 5; 
		else roi = 6;
	}
	
	strcpy(item[i]._iName, OilStr[roi]);
	strcpy(item[i]._iIName, OilStr[roi]);
	item[i]._iMiscId = OilIdVal[roi];
	item[i]._ivalue = OilValue[roi];
	item[i]._iIvalue = OilValue[roi];
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void GetItemAttrs(int i, int idata, int lvl)
{
	int rndv;

	item[i]._itype = AllItemsList[idata].itype;
	item[i]._iCurs = AllItemsList[idata].iCurs;
	strcpy(item[i]._iName, AllItemsList[idata].iName);
	strcpy(item[i]._iIName, AllItemsList[idata].iName);
	item[i]._iLoc = AllItemsList[idata].iLoc;
	item[i]._iClass = AllItemsList[idata].iClass;
	item[i]._iMinDam = AllItemsList[idata].iMinDam;
	item[i]._iMaxDam = AllItemsList[idata].iMaxDam;
	item[i]._iAC = random(20, AllItemsList[idata].iMaxAC - AllItemsList[idata].iMinAC + 1) + AllItemsList[idata].iMinAC;
	item[i]._iFlags = AllItemsList[idata].iFlags;
	item[i]._iFlags2 = 0;
	item[i]._iMiscId = AllItemsList[idata].iMiscId;
	item[i]._iSpell = AllItemsList[idata].iSpell;
	item[i]._iMagical = IMAGIC_NONE;

	item[i]._ivalue = AllItemsList[idata].iValue;
	item[i]._iIvalue = AllItemsList[idata].iValue;

	item[i]._iVAdd1 = 0;
	item[i]._iVMult1 = 0;
	item[i]._iVAdd2 = 0;
	item[i]._iVMult2 = 0;

	item[i]._iPLDam = 0;
	item[i]._iPLToHit = 0;
	item[i]._iPLAC = 0;

	item[i]._iPLStr = 0;
	item[i]._iPLMag = 0;
	item[i]._iPLDex = 0;
	item[i]._iPLVit = 0;

	item[i]._iCharges = 0;
	item[i]._iMaxCharges = 0;

	item[i]._iDurability = AllItemsList[idata].iDurability;
	item[i]._iMaxDur = AllItemsList[idata].iDurability;
	item[i]._iMinStr = AllItemsList[idata].iMinStr;
	item[i]._iMinMag = AllItemsList[idata].iMinMag;
	item[i]._iMinDex = AllItemsList[idata].iMinDex;

	item[i]._iPLFR = 0;
	item[i]._iPLLR = 0;
	item[i]._iPLMR = 0;

	item[i].IDidx = idata;

	item[i]._iPLDamMod = 0;
	item[i]._iPLGetHit = 0;
	item[i]._iPLLight = 0;

	item[i]._iSplLvlAdd = 0;

	item[i]._iRequest = FALSE;

	item[i]._iFMinDam = 0;
	item[i]._iFMaxDam = 0;
	item[i]._iLMinDam = 0;
	item[i]._iLMaxDam = 0;

	item[i]._iPLEnAc = 0;

	item[i]._iPLMana = 0;
	item[i]._iPLHP = 0;

	item[i]._iPrePower = -1;
	item[i]._iSufPower = -1;
	item[i]._iFlags = 0;
	item[i]._iFlags2 = 0;

	if (item[i]._iMiscId == IMID_BOOK) GetBookSpell(i, lvl);

	if (item[i]._iMiscId == IMID_OIL) GetOilType(i, lvl);

	int efflevel = GetEffLevel();
		
	if (item[i]._itype == IT_GOLD) {
		if (gnDifficulty == D_NORMAL)
			rndv = (efflevel * 5) + random(21, efflevel * 10);
		else if (gnDifficulty == D_NIGHTMARE)
			rndv = ((efflevel + 16) * 5) + random(21, (efflevel + 16) * 10);
		else if (gnDifficulty == D_HELL)
			rndv = ((efflevel + 32) * 5) + random(21, (efflevel + 32) * 10);
			
		if (leveltype == 4) rndv += (rndv >> 3);
		if (rndv > 5000) rndv = 5000;
		item[i]._ivalue = rndv;
		if (rndv >= GOLD_VT2) {
			item[i]._iCurs = ITEM_5GOLD;
		} else {
			if (rndv <= GOLD_VT1) item[i]._iCurs = ITEM_1GOLD;
			else item[i]._iCurs = ITEM_3GOLD;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int RndPL(int param1, int param2)
{
	return(random(22, param2 - param1 + 1) + param1);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int PLVal(int pv, int p1, int p2, int minv, int maxv)
{
	if (p1 == p2) return(minv);
	if (minv == maxv) return(minv);
	return((((((pv - p1) * 100) / (p2 - p1)) * (maxv - minv)) / 100) + minv);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SaveItemPower(int i, int power, int param1, int param2, int minval, int maxval, int multval)
{
	int r, r2;

	r = RndPL(param1, param2);
	switch (power) {
		case PL_TOHIT:
			item[i]._iPLToHit += r;
			break;
		case PL_NTOHIT:
			item[i]._iPLToHit -= r;
			break;
		case PL_TODAM:
			item[i]._iPLDam += r;
			break;
		case PL_NTODAM:
			item[i]._iPLDam -= r;
			break;
		case PL_DOPPEL:	
			item[i]._iFlags2 |= IAF2_CLONE;
			// fall through
		case PL_DAHT:
			r = RndPL(param1, param2);
			item[i]._iPLDam += r;
			if (param1 == 20) r2 = RndPL(1, 5);
			if (param1 == 36) r2 = RndPL(6, 10);
			if (param1 == 51) r2 = RndPL(11, 15);
			if (param1 == 66) r2 = RndPL(16, 20);
			if (param1 == 81) r2 = RndPL(21, 30);
			if (param1 == 96) r2 = RndPL(31, 40);
			if (param1 == 111) r2 = RndPL(41, 50);
			if (param1 == 126) r2 = RndPL(51, 75);
			if (param1 == 151) r2 = RndPL(76, 100);
			item[i]._iPLToHit += r2;
			break;
			
		case PL_NDAHT:
			item[i]._iPLDam -= r;
			if (param1 == 25) r2 = RndPL(1, 5);
			if (param1 == 50) r2 = RndPL(6, 10);
			item[i]._iPLToHit -= r2;
			break;
		case PL_AC:
			item[i]._iPLAC += r;
			break;
		case PL_NAC:
			item[i]._iPLAC -= r;
			break;
		case PL_ACTUALAC:
			item[i]._iAC = r;
			break;
		case PL_NACTULAC:
			item[i]._iAC -= r;
			break;
		case PL_RFIRE:
			item[i]._iPLFR += r;
			break;
		case PL_RLGHT:
			item[i]._iPLLR += r;
			break;
		case PL_RMAG:
			item[i]._iPLMR += r;
			break;
		case PL_RALL:
			item[i]._iPLFR += r;
			item[i]._iPLLR += r;
			item[i]._iPLMR += r;
			if (item[i]._iPLFR < 0) item[i]._iPLFR = 0;
			if (item[i]._iPLLR < 0) item[i]._iPLLR = 0;
			if (item[i]._iPLMR < 0) item[i]._iPLMR = 0;
			break;
		case PL_SLVL:
			item[i]._iSplLvlAdd = r;
			break;
		case PL_CHRG :
			item[i]._iCharges = item[i]._iCharges * param1;
			item[i]._iMaxCharges = item[i]._iCharges;
			break;
		case PL_SPELL :
			item[i]._iSpell = param1;
			item[i]._iCharges = param2;
			item[i]._iMaxCharges = param2;
			break;
		case PL_FHIT:
			item[i]._iFlags |= IAF_FIREHIT;
			item[i]._iFlags &= ~IAF_LIGHTHIT;
			item[i]._iFMinDam = param1;
			item[i]._iFMaxDam = param2;
			item[i]._iLMinDam = 0;
			item[i]._iLMaxDam = 0;
			break;
		case PL_LHIT:
			item[i]._iFlags |= IAF_LIGHTHIT;
			item[i]._iFlags &= ~IAF_FIREHIT;
			item[i]._iLMinDam = param1;
			item[i]._iLMaxDam = param2;
			item[i]._iFMinDam = 0;
			item[i]._iFMaxDam = 0;
			break;
		case PL_STR:
			item[i]._iPLStr += r;
			break;
		case PL_NSTR:
			item[i]._iPLStr -= r;
			break;
		case PL_MAG:
			item[i]._iPLMag += r;
			break;
		case PL_NMAG:
			item[i]._iPLMag -= r;
			break;
		case PL_DEX:
			item[i]._iPLDex += r;
			break;
		case PL_NDEX:
			item[i]._iPLDex -= r;
			break;
		case PL_VIT:
			item[i]._iPLVit += r;
			break;
		case PL_NVIT:
			item[i]._iPLVit -= r;
			break;
		case PL_STATS:
			item[i]._iPLStr += r;
			item[i]._iPLMag += r;
			item[i]._iPLDex += r;
			item[i]._iPLVit += r;
			break;
		case PL_NSTATS:
			item[i]._iPLStr -= r;
			item[i]._iPLMag -= r;
			item[i]._iPLDex -= r;
			item[i]._iPLVit -= r;
			break;
		case PL_GETHIT:
			item[i]._iPLGetHit += r;
			break;
		case PL_NGETHIT:
			item[i]._iPLGetHit -= r;
			break;
		case PL_HP:
			item[i]._iPLHP += (r << HP_SHIFT);
			break;
		case PL_NHP:
			item[i]._iPLHP -= (r << HP_SHIFT);
			break;
		case PL_MANA:
			item[i]._iPLMana += (r << MANA_SHIFT);
			drawmanaflag = TRUE;
			break;
		case PL_NMANA:
			item[i]._iPLMana -= (r << MANA_SHIFT);
			drawmanaflag = TRUE;
			break;
		case PL_DUR:
			r2 = (item[i]._iMaxDur * r) / 100;
			item[i]._iMaxDur += r2;
			item[i]._iDurability += r2;
			break;
		case PL_FRAGILE:	
			item[i]._iPLDam += 140 + r * 2;
			// fall through
		case PL_NDUR:
			r2 = (item[i]._iMaxDur * r) / 100;
			item[i]._iMaxDur -= r2;
			if (item[i]._iMaxDur < 1) item[i]._iMaxDur = 1;
			item[i]._iDurability = item[i]._iMaxDur;
			break;
		case PL_IND:
			item[i]._iDurability = INFINITE_DUR;
			item[i]._iMaxDur = INFINITE_DUR;
			break;
		case PL_LIGHT:
			item[i]._iPLLight += param1;
			break;
		case PL_NLIGHT:
			item[i]._iPLLight -= param1;
			break;
		case PL_NUMARWS:
			item[i]._iFlags |= IAF_RABID; // unused -> multimissile --donald
			break;
		case PL_FARROW:
			item[i]._iFlags |= IAF_FIREARROW;
			item[i]._iFlags &= ~IAF_LARROW;
			item[i]._iFMinDam = param1;
			item[i]._iFMaxDam = param2;
			item[i]._iLMinDam = 0;
			item[i]._iLMaxDam = 0;
			break;
		case PL_LARROW:
			item[i]._iFlags |= IAF_LARROW;
			item[i]._iFlags &= ~IAF_FIREARROW;
			item[i]._iLMinDam = param1;
			item[i]._iLMaxDam = param2;
			item[i]._iFMinDam = 0;
			item[i]._iFMaxDam = 0;
			break;
		case PL_HITADD:	// usurped for fireball arrows...
			item[i]._iFlags |= (IAF_LARROW | IAF_FIREARROW);
			item[i]._iFMinDam = param1;
			item[i]._iFMaxDam = param2;
			item[i]._iLMinDam = 0;
			item[i]._iLMaxDam = 0;
			break;
		case PL_THORN:
			item[i]._iFlags |= IAF_THORN;
			break;
		case PL_LMANA:
			item[i]._iFlags |= IAF_LMANA;
			drawmanaflag = TRUE;
			break;
		case PL_NOHEAL:
			item[i]._iFlags |= IAF_NOHEAL;
			break;
		case PL_TRAPDAM:
			item[i]._iFlags |= IAF_TRAPDAM;
			break;
		case PL_BEAR:
			item[i]._iFlags |= IAF_KNOCKBACK;
			break;
		case PL_DAMDEMON:
			item[i]._iFlags |= IAF_DAMDEMON;
			break;
		case PL_ZERORES:
			item[i]._iFlags |= IAF_ZERORES;
			break;
		case PL_MNOHEAL:
			item[i]._iFlags |= IAF_MNOHEAL;
			break;
		case PL_BAT:
			if (param1 == 3) item[i]._iFlags |= IAF_BAT10;
			if (param1 == 5) item[i]._iFlags |= IAF_BAT20;
			drawmanaflag = TRUE;
			break;
		case PL_LEECH:
			if (param1 == 3) item[i]._iFlags |= IAF_LEECH10; 
			if (param1 == 5) item[i]._iFlags |= IAF_LEECH20;
			drawhpflag = TRUE;
			break;
		case PL_ENAC:
			item[i]._iPLEnAc = param1;
			break;
		case PL_ATANIM:
			if (param1 == 1) item[i]._iFlags |= IAF_ATANIM1;
			if (param1 == 2) item[i]._iFlags |= IAF_ATANIM2;
			if (param1 == 3) item[i]._iFlags |= IAF_ATANIM3;
			if (param1 == 4) item[i]._iFlags |= IAF_ATANIM4;
			break;
		case PL_HTANIM:
			if (param1 == 1) item[i]._iFlags |= IAF_HTANIM1;
			if (param1 == 2) item[i]._iFlags |= IAF_HTANIM2;
			if (param1 == 3) item[i]._iFlags |= IAF_HTANIM3;
			break;
		case PL_BLANIM:
			item[i]._iFlags |= IAF_BLANIM;
			break;
		case PL_DAMADD:
			item[i]._iPLDamMod += r;
			break;
		case PL_RNDARW:
			item[i]._iFlags |= IAF_RNDARROW;
			break;
		case PL_DAMAGE:
			item[i]._iMinDam = param1;
			item[i]._iMaxDam = param2;
			break;
		case PL_DURNUM:
			item[i]._iDurability = param1;
			item[i]._iMaxDur = param1;
			break;
		case PL_FALCON:
			item[i]._iFlags |= IAF_ATANIM3;
			break;
		case PL_ONEHAND:
			item[i]._iLoc = IL_HAND;
			break;
		case PL_CONST:
			item[i]._iFlags |= IAF_CONSTRICT;
			break;	
		case PL_SKING:
			item[i]._iFlags |= IAF_SKING;
			break;
		case PL_INFRA:
			item[i]._iFlags |= IAF_INFRAVISION;
			break;
		case PL_NSTRREQ:
			item[i]._iMinStr = 0;
			break;
		case PL_GFX:
			item[i]._iCurs = param1;
			break;
		case PL_HARQUN:		// usurped for lightning arrows
			item[i]._iFlags |= (IAF_LARROW | IAF_FIREARROW);
			item[i]._iFMinDam = param1;
			item[i]._iFMaxDam = param2;
			item[i]._iLMinDam = 1;
			item[i]._iLMaxDam = 0;
#if 0
			//rjs item[i]._iPLHP = (byte)plr[myplr]._pArmorClass + plr[myplr]._pIBonusAC + plr[myplr]._pIAC;
			item[i]._iPLHP = plr[myplr]._pIBonusAC + plr[myplr]._pIAC;
			item[i]._iPLHP += (plr[myplr]._pDexterity / 5);
			item[i]._iPLHP = item[i]._iPLHP << HP_SHIFT;
#endif
			break;
		case PL_HARQUN2:
			item[i]._iFlags |= (IAF_LIGHTHIT | IAF_FIREHIT);
			item[i]._iFMinDam = param1;
			item[i]._iFMaxDam = param2;
			item[i]._iLMinDam = 2;
			item[i]._iLMaxDam = 0;
//			item[i]._iAC += ((plr[myplr]._pMaxManaBase >> MANA_SHIFT) / 10);
			break;
		case PL_HARQUN3:
			item[i]._iPLFR = 30 - plr[myplr]._pLevel;
			if (item[i]._iPLFR < 0) item[i]._iPLFR = 0;
			break;
		case PL_NRFIRE:
			item[i]._iPLFR -= r;
			break;
		case PL_NRLGHT:
			item[i]._iPLLR -= r;
			break;
		case PL_NRMAG:
			item[i]._iPLMR -= r;
			break;
		case PL_NRALL:
			item[i]._iPLFR -= r;
			item[i]._iPLLR -= r;
			item[i]._iPLMR -= r;
			break;
		case PL_DEVAST:
			item[i]._iFlags2 |= IAF2_DEVASTATION;
			break;
		case PL_DECAY:	
			item[i]._iFlags2 |= IAF2_DECAY;
			item[i]._iPLDam += r;
			break;
		case PL_PERIL:
			item[i]._iFlags2 |= IAF2_PERIL;
			break;
		case PL_RNDDAM:	
			item[i]._iFlags2 |= IAF2_JESTER;
			break;
			
		case PL_DEMONAC:
			item[i]._iFlags2 |= IAF2_DEMONAC;
			break;
		case PL_UNDEADAC:
			item[i]._iFlags2 |= IAF2_UNDEADAC;
			break;
			
		case PL_ACOLYTE:
			r2 = ((plr[myplr]._pMaxManaBase >> MANA_SHIFT) * 50 / 100);
			item[i]._iPLMana -= (r2 << MANA_SHIFT);
			item[i]._iPLHP   += (r2 << HP_SHIFT);
			break;
		case PL_GLADIATR:
			r2 = ((plr[myplr]._pMaxHPBase >> HP_SHIFT) * 40 / 100);
			item[i]._iPLHP   -= (r2 << HP_SHIFT);
			item[i]._iPLMana += (r2 << MANA_SHIFT);
			break;
	}
	if ((item[i]._iVAdd1 == 0) && (item[i]._iVMult1 == 0)) {
		item[i]._iVAdd1 = PLVal(r, param1, param2, minval, maxval);
		item[i]._iVMult1 = multval;
	} else {
		item[i]._iVAdd2 = PLVal(r, param1, param2, minval, maxval);
		item[i]._iVMult2 = multval;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void GetItemPower(int i, int minlvl, int maxlvl, long flgs, BOOL onlygood)
{
	int pre, post;
	int l[256], nl;
	int j, aii;
	int preidx, sufidx;
	char istr[128];
	byte goe;

	pre = random(23, 4);		// Pre modifier (20%)
	post = random(23, 3);		// Post modifirer (66%)
	// if no pre modifier, make sure post modifier
	if ((pre != 0) && (post == 0)) {
		if (random(23, 2)) post = 1;
		else pre = 0;
	}

	preidx = -1;
	sufidx = -1;
	goe = 0;

	// if not any only good item, then 67% chance of good, 33% chance of anything
	if ((!onlygood) && (random(0, 3))) onlygood = TRUE;

	if (pre == 0) {
		nl = 0;
		for (j = 0; PL_Prefix[j].PLPower != -1; j++) {
			if (((PL_Prefix[j].PLIType & flgs) != 0) &&
				(PL_Prefix[j].PLMinLvl >= minlvl) &&
				(PL_Prefix[j].PLMinLvl <= maxlvl)
			) {
				if (onlygood && !PL_Prefix[j].PLOk) continue;
				if ((flgs == PLF_STAFF) && (PL_Prefix[j].PLPower == PL_CHRG)) continue;

				l[nl] = j;
				nl++;
				if (PL_Prefix[j].PLDouble) {
					l[nl] = j;
					nl++;
				}
			}
		}
		if (nl != 0) {
			preidx = l[random(23, nl)];
			sprintf(istr, "%s %s", PL_Prefix[preidx].PLName, item[i]._iIName);
			strcpy(item[i]._iIName, istr);
			item[i]._iMagical = IMAGIC_MAGIC;
			SaveItemPower(i, PL_Prefix[preidx].PLPower, PL_Prefix[preidx].PLParam1, PL_Prefix[preidx].PLParam2, PL_Prefix[preidx].PLMinVal, PL_Prefix[preidx].PLMaxVal, PL_Prefix[preidx].PLMultVal);
			item[i]._iPrePower = PL_Prefix[preidx].PLPower;
			goe = PL_Prefix[preidx].PLGOE;
		}
	}
	if (post != 0) {
		nl = 0;
		for (j = 0; PL_Suffix[j].PLPower != -1; j++) {
			if (((PL_Suffix[j].PLIType & flgs) != 0) &&
				(PL_Suffix[j].PLMinLvl >= minlvl) &&
				(PL_Suffix[j].PLMinLvl <= maxlvl) &&
				((goe | PL_Suffix[j].PLGOE) != 0x11)
			) {
				if (onlygood && !PL_Suffix[j].PLOk) continue;

				l[nl] = j;
				nl++;
			}
		}
		if (nl != 0) {
			sufidx = l[random(23, nl)];
			sprintf(istr, "%s of %s", item[i]._iIName, PL_Suffix[sufidx].PLName);
			strcpy(item[i]._iIName, istr);
			item[i]._iMagical = IMAGIC_MAGIC;
			SaveItemPower(i, PL_Suffix[sufidx].PLPower, PL_Suffix[sufidx].PLParam1, PL_Suffix[sufidx].PLParam2, PL_Suffix[sufidx].PLMinVal, PL_Suffix[sufidx].PLMaxVal, PL_Suffix[sufidx].PLMultVal);
			item[i]._iSufPower = PL_Suffix[sufidx].PLPower;
		}
	}
	if (!InfoFit(item[i]._iIName)) {
		aii = item[i].IDidx;
		if (AllItemsList[aii].iSName)
			strcpy(item[i]._iIName, AllItemsList[aii].iSName);
		else
			item[i]._iName[0] = 0;
			
		if (preidx != -1) {
			sprintf(istr, "%s %s", PL_Prefix[preidx].PLName, item[i]._iIName);
			strcpy(item[i]._iIName, istr);
		}
		if (sufidx != -1) {
			sprintf(istr, "%s of %s", item[i]._iIName, PL_Suffix[sufidx].PLName);
			strcpy(item[i]._iIName, istr);
		}
	}
	if ((preidx != -1) || (sufidx != -1)) CalcItemValue(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void GetItemBonus(int i, int idata, int minlvl, int maxlvl, BOOL onlygood, bool SpellsOk)
{
	if (item[i]._iClass == IC_GOLD) return;
	if (minlvl > 25) minlvl = 25;

	switch(item[i]._itype) {
		case IT_SWORD :
		case IT_AXE:
		case IT_MACE:
			GetItemPower(i, minlvl, maxlvl, PLF_WEAPON, onlygood);
			break;
		case IT_BOW:
			GetItemPower(i, minlvl, maxlvl, PLF_BOW, onlygood);
			break;
		case IT_SHIELD:
			GetItemPower(i, minlvl, maxlvl, PLF_SHIELD, onlygood);
			break;
		case IT_ARMOR:
		case IT_HELM:
		case IT_MARMOR:
		case IT_HARMOR:
			GetItemPower(i, minlvl, maxlvl, PLF_ARMOR, onlygood);
			break;
		case IT_STAFF:
			if (SpellsOk)
				GetStaffSpell(i, maxlvl, onlygood);
			else
				GetItemPower(i, minlvl, maxlvl, PLF_STAFF, onlygood);
			break;
		case IT_RING:
		case IT_AMULET:
			GetItemPower(i, minlvl, maxlvl, PLF_RING, onlygood);
			break;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SetupItem(int i)
{
	int it;

	DROPLOG("SetupItem:  setting up object %d!\n",i);

	it = ItemCAnimTbl[item[i]._iCurs];
	item[i]._iAnimData = itemanims[it];
	item[i]._iAnimLen = ItemAnimLs[it];
	item[i]._iAnimWidth = 96;
	item[i]._iAnimWidth2 = 16;
	item[i]._iIdentified = FALSE;
	item[i]._iPostDraw = FALSE;
	if (plr[myplr].pLvlLoad == LVLCHANGE_OFF) {
		item[i]._iAnimFrame = 1;
		item[i]._iAnimFlag = TRUE;
		item[i]._iSelFlag = ISEL_NONE;
	} else {
		item[i]._iAnimFrame = item[i]._iAnimLen;
		item[i]._iAnimFlag = FALSE;
		item[i]._iSelFlag = ISEL_FLR;
	}
}

/*-----------------------------------------------------------------------*
** Choose an item type from a monster
**-----------------------------------------------------------------------*/

int RndItem(int m)
{
	int	r;
	int ril[512];		// Max 512 items
	int ri, i;

	// Unique item?
	if (monster[m].MData->mTreasure & T_U)
		return(-((monster[m].MData->mTreasure & T_MASK) + 1));

	if (monster[m].MData->mTreasure & T_NONE) return(0);

#if CHEATS
	if (davedebug) r = 100;
	else r = random(24, 100);
	if (!cheatflag && (r > 40)) return(0);
	if (!cheatflag && (random(24, 100) > 25)) return(IDI_GOLD+1);
#else
	r = random(24, 100);
	if (r > 40) return(0);
	// Gold 75% of the time
	if (random(24, 100) > 25) return(IDI_GOLD+1);
#endif

	ri = 0;
	for (i = 0; AllItemsList[i].iLoc != -1; i++) {
		if ((AllItemsList[i].iRnd == IRND_DOUBLE) &&
			(monster[m].mLevel >= AllItemsList[i].iMinMLvl)
			&& ri < 512) ril[ri++] = i;
		if (AllItemsList[i].iRnd && 
			(monster[m].mLevel >= AllItemsList[i].iMinMLvl)
			&& ri < 512) ril[ri++] = i;
		if (AllItemsList[i].iSpell == SPL_RESURRECT && gbMaxPlayers == 1) ri--;
		if (AllItemsList[i].iSpell == SPL_HEALOTHER && gbMaxPlayers == 1) ri--;
	}
	r = random(24, ri);
	return(ril[r]+1);
}

/*-----------------------------------------------------------------------*
** Choose an item type for a good or unique item
**-----------------------------------------------------------------------*/

int RndUItem(int m)
{
	int ril[512];		// Max 512 items
	int ri, i;
	BOOL okflag;

	// Unique item?
	if (m != -1) {
		if ((monster[m].MData->mTreasure & T_U) && (gbMaxPlayers == 1))
			return(-((monster[m].MData->mTreasure & T_MASK) + 1));
	}
	int efflevel = GetEffLevel();

	ri = 0;
	for (i = 0; AllItemsList[i].iLoc != -1; i++) {
		okflag = TRUE;
		if (!AllItemsList[i].iRnd) okflag = FALSE;
		if (m != -1) {
			if (monster[m].mLevel < AllItemsList[i].iMinMLvl) okflag = FALSE;
		} else {
			if ((efflevel << 1) < AllItemsList[i].iMinMLvl) okflag = FALSE;
		}
		if (AllItemsList[i].itype == IT_MISC) okflag = FALSE;
		if (AllItemsList[i].itype == IT_GOLD) okflag = FALSE;
		if (AllItemsList[i].itype == IT_FOOD) okflag = FALSE;
		if (AllItemsList[i].iMiscId == IMID_BOOK) okflag = TRUE;
		if (AllItemsList[i].iSpell == SPL_RESURRECT && gbMaxPlayers == 1) okflag = FALSE;
		if (AllItemsList[i].iSpell == SPL_HEALOTHER && gbMaxPlayers == 1) okflag = FALSE;
		if (okflag && ri < 512) ril[ri++] = i;
	}
	return(ril[random(25, ri)]);
}

/*-----------------------------------------------------------------------*
** Choose any type of item
**-----------------------------------------------------------------------*/

int RndAllItems()
{
	int	r;
	int ril[512];		// Max 512 items
	int ri, i;

#if CHEATS
	if (!cheatflag && !itemcheat && (random(26, 100) > 25)) return(IDI_GOLD);
#else
	// Gold 75% of the time
	if (random(26, 100) > 25) return(IDI_GOLD);
#endif
	int efflevel = GetEffLevel();

	ri = 0;
	for (i = 0; AllItemsList[i].iLoc != -1; i++) {
		if (AllItemsList[i].iRnd && ((efflevel << 1) >= AllItemsList[i].iMinMLvl)
			&& ri < 512) ril[ri++] = i;
		if (AllItemsList[i].iSpell == SPL_RESURRECT && gbMaxPlayers == 1) ri--;
		if (AllItemsList[i].iSpell == SPL_HEALOTHER && gbMaxPlayers == 1) ri--;
	}
	r = random(26, ri);
	return(ril[r]);
}

/*-----------------------------------------------------------------------*
** Choose an item of a specific type
**-----------------------------------------------------------------------*/

int RndTypeItems(int itype, int imid, int level)
{
	int ril[512];		// Max 512 items
	int ri, i;
	BOOL okflag;

	ri = 0;
	for (i = 0; AllItemsList[i].iLoc != -1; i++) {
		okflag = TRUE;
		if (!AllItemsList[i].iRnd) okflag = FALSE;
		if ((level << 1) < AllItemsList[i].iMinMLvl) okflag = FALSE;
		if (AllItemsList[i].itype != itype) okflag = FALSE;
		if ((imid != -1) && (AllItemsList[i].iMiscId != imid)) okflag = FALSE;
		if (okflag && ri < 512) 
			ril[ri++] = i;
	}
	return(ril[random(27, ri)]);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int CheckUnique(int i, int lvl, int uper, BOOL recreate)
{
	int j, idata;
	BYTE uok[MAXUITEMS];
	int numu, u;

#if CHEATS
	if (!davedebug || !cheatflag) {
		if (random(28, 100) > uper) return(-1);
	}
#else
	if (random(28, 100) > uper) return(-1);
#endif

	numu = 0;
	ZeroMemory(uok,sizeof(uok));
	for (j = 0; UniqueItemList[j].UIItemId != -1; j++) {
		idata = item[i].IDidx;
		if (UniqueItemList[j].UIItemId != AllItemsList[idata].iItemId) continue;
		if (lvl < UniqueItemList[j].UIMinLvl) continue;
		if (!recreate && UniqueItemFlag[j] && (gbMaxPlayers == 1)) continue;
		uok[j] = TRUE;
		numu++;
	}

	if (numu == 0) return(-1);
	u = random(29, 10);
	j = 0;
	while (numu > 0) {
		if (uok[j]) numu--;
		if (numu > 0) {
			j++;
			if (j == MAXUITEMS) j = 0;
		}
	}
	return(j);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void GetUniqueItem(int i, int uid)
{
	// Generated
	UniqueItemFlag[uid] = TRUE;
	// Save abilities
	SaveItemPower(i, UniqueItemList[uid].UIPower1, UniqueItemList[uid].UIParam1, UniqueItemList[uid].UIParam2, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 1)
		SaveItemPower(i, UniqueItemList[uid].UIPower2, UniqueItemList[uid].UIParam3, UniqueItemList[uid].UIParam4, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 2)
		SaveItemPower(i, UniqueItemList[uid].UIPower3, UniqueItemList[uid].UIParam5, UniqueItemList[uid].UIParam6, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 3)
		SaveItemPower(i, UniqueItemList[uid].UIPower4, UniqueItemList[uid].UIParam7, UniqueItemList[uid].UIParam8, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 4)
		SaveItemPower(i, UniqueItemList[uid].UIPower5, UniqueItemList[uid].UIParam9, UniqueItemList[uid].UIParam10, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 5)
		SaveItemPower(i, UniqueItemList[uid].UIPower6, UniqueItemList[uid].UIParam11, UniqueItemList[uid].UIParam12, 0, 0, 1);
	// Save name 
	strcpy(item[i]._iIName, UniqueItemList[uid].UIName);
	item[i]._iIvalue = UniqueItemList[uid].UIValue;
	if (item[i]._iMiscId == IMID_UNIQUE) item[i]._iSeed = uid;	// Save index into UniqueItemList
	item[i]._iUid = uid;		// Save index into Unique Item List
	item[i]._iMagical = IMAGIC_UNIQUE;
	item[i]._iCreateInfo |= ICI_UNIQUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SpawnUnique(int uid, int x, int y)
{
	int	ii, itype;

	int efflevel = GetEffLevel();
		
	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(x,y,ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
		itype = 0;
		while (AllItemsList[itype].iItemId != UniqueItemList[uid].UIItemId) itype++;
		GetItemAttrs(ii, itype, efflevel);
		GetUniqueItem(ii, uid);
		SetupItem(ii);
		numitems++;
	}
}


/*-----------------------------------------------------------------------*
** Items in dungeon have lower durs
**-----------------------------------------------------------------------*/

void ItemRndDur(int ii)
{
	if (item[ii]._iDurability == 0) return;
	if (item[ii]._iDurability == INFINITE_DUR) return;
	item[ii]._iDurability = random(0, item[ii]._iMaxDur >> 1) + (item[ii]._iMaxDur >> 2) + 1;
}

/*-----------------------------------------------------------------------*
** Setup an item and determine magics
**-----------------------------------------------------------------------*/

void SetupAllItems(int ii, int idx, int iseed, int lvl, int uper, BOOL onlygood, BOOL recreate, BOOL pregen)
{
	int	iblvl, uid;
	
	item[ii]._iSeed = iseed;
	SetRndSeed(iseed);
	GetItemAttrs(ii, idx, lvl >> 1);
	item[ii]._iCreateInfo = lvl;
	if (pregen) item[ii]._iCreateInfo |= ICI_PREGEN;
	if (onlygood) item[ii]._iCreateInfo |= ICI_ONLYGOOD;
	if (uper == 15) item[ii]._iCreateInfo |= ICI_UPER15;
	else if (uper == 1) item[ii]._iCreateInfo |= ICI_UPER1;
	if (item[ii]._iMiscId != IMID_UNIQUE) {
		iblvl = -1;
		if (random(32, 100) <= 10) iblvl = lvl;
		else if (random(33, 100) <= lvl) iblvl = lvl;
		// Force rings, amulets, and staffs to be magical
		if ((iblvl == -1) && (item[ii]._iMiscId == IMID_STAFF)) iblvl = lvl;
		if ((iblvl == -1) && (item[ii]._iMiscId == IMID_RING)) iblvl = lvl;
		if ((iblvl == -1) && (item[ii]._iMiscId == IMID_AMULET)) iblvl = lvl;
		if (onlygood) iblvl = lvl;
#if CHEATS
		if (cheatflag) iblvl = lvl;
#endif
		if (uper == 15) iblvl = lvl + 4;
		if (iblvl != -1) {
			uid = CheckUnique(ii, iblvl, uper, recreate);
			if (uid == -1)
				GetItemBonus(ii, idx, iblvl >> 1, iblvl, onlygood, true);
			else {
				GetUniqueItem(ii, uid);
				item[ii]._iCreateInfo |= ICI_UNIQUE;
			}
		}
		if (item[ii]._iMagical != IMAGIC_UNIQUE) ItemRndDur(ii);
	} else {
		// if it is something they wield, then get the attributes
		if (item[ii]._iLoc != IL_INV) GetUniqueItem(ii, iseed);
	}
	SetupItem(ii);
}

/*-----------------------------------------------------------------------*
** Create any item from a monster (or unqiue if monster gives one up)
**-----------------------------------------------------------------------*/

void SpawnItem(int m, int x, int y, BOOL sendmsg)
{
	int	ii, idx;
	BOOL onlygood;

	if ((monster[m]._uniqtype != 0) ||
		((monster[m].MData->mTreasure & T_U) && (gbMaxPlayers != 1))) {
		// If unique, make sure we get something good
		idx = RndUItem(m);
		if (idx < 0) {
			SpawnUnique(-(idx+1), x, y);
			return;
		}
		onlygood = TRUE;
	} else {
		// special code to pop out brain for Mushroom Quest
		if (quests[Q_BKMUSHRM]._qactive == QUEST_NOTDONE
				 && quests[Q_BKMUSHRM]._qvar1 == QS_MUSHGIVEN) {
			idx = IDI_BRAIN;
			quests[Q_BKMUSHRM]._qvar1 = QS_BRAINSPAWNED;
		}
		else
		{
			idx = RndItem(m);
			if (idx == 0) return;
			if (idx > 0) {
				idx--;
				onlygood = FALSE;
			} else {
				SpawnUnique(-(idx+1), x, y);
				return;
			}
		}
	}
	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(x,y,ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
		if (monster[m]._uniqtype != 0) SetupAllItems(ii, idx, GetRndSeed(), monster[m].MData->mLevel, 15, onlygood, FALSE, FALSE);
#if CHEATS
		else if (itemcheat)
			SetupAllItems(ii, idx, GetRndSeed(), monster[m].MData->mLevel, 15, TRUE, FALSE, FALSE);
#endif
		else SetupAllItems(ii, idx, GetRndSeed(), monster[m].MData->mLevel, 1, onlygood, FALSE, FALSE);

		numitems++;
		if (sendmsg) NetSendCmdDItem(FALSE, ii);
	}
}


/*-----------------------------------------------------------------------*
** Only used for quest (single player)
**-----------------------------------------------------------------------*/

void CreateItem(int uid, int x, int y)
{
	int	ii, idx;

	int efflevel = GetEffLevel();
		
	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(x,y,ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
		idx = 0;
		while (AllItemsList[idx].iItemId != UniqueItemList[uid].UIItemId) idx++;
		GetItemAttrs(ii, idx, efflevel);
		GetUniqueItem(ii, uid);
		SetupItem(ii);
		item[ii]._iMagical = IMAGIC_UNIQUE;
		numitems++;
	}
}


/*-----------------------------------------------------------------------*
** Create any item from an object, etc (chest, barrel, sarc)
**-----------------------------------------------------------------------*/

void CreateRndItem(int x, int y, BOOL onlygood, BOOL sendmsg, BOOL delta)
{
	int	ii, idx;

	int efflevel = GetEffLevel();
		
	if (onlygood) idx = RndUItem(-1);
	else idx = RndAllItems();
	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(x,y,ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
#if CHEATS
		if (itemcheat)
			SetupAllItems(ii, idx, GetRndSeed(), efflevel << 1, 15, TRUE, FALSE, delta);
		else	
#endif
			SetupAllItems(ii, idx, GetRndSeed(), efflevel << 1, 1, onlygood, FALSE, delta);
		if (sendmsg) NetSendCmdDItem(FALSE, ii);
		if (delta) DeltaAddItem(ii);
		numitems++;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SetupAllUseful(int ii, int iseed, int lvl)
{
	int idx;

	item[ii]._iSeed = iseed;
	SetRndSeed(iseed);
	
#if 0	
	if (random(34, 2)) idx = IDI_HEAL;
	else idx = IDI_MANA;
	
	// added 7/30/97 by donald
	if (!random(34, 8)) idx = IDI_OILACC;
	
	if ((lvl > 1) && (random(34, 3) == 0)) idx = IDI_PORTAL;
#else
	// why call random() so many times?
	
	switch(random(34,7))
	{
		case 0:	idx = IDI_PORTAL; if (lvl > 1) break;	// else fallthrough
		case 1:
		case 2:	idx = IDI_HEAL; break;
		case 3:	idx = IDI_PORTAL; if (lvl > 1) break;	// else fallthrough
		case 4:
		case 5:	idx = IDI_MANA; break;
		default:idx = IDI_OILACC; break;
	}
#endif	

	GetItemAttrs(ii, idx, lvl);
	item[ii]._iCreateInfo = ICI_USEFUL + lvl;
	SetupItem(ii);
}

/*-----------------------------------------------------------------------*
** Rnd item, but only health, mana, or id
**-----------------------------------------------------------------------*/

void CreateRndUseful(int pnum, int x, int y, BOOL sendmsg)
{
	int	ii;

	int efflevel = GetEffLevel();
		
	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(x,y,ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
		SetupAllUseful(ii, GetRndSeed(), efflevel);
		if (sendmsg) NetSendCmdDItem(FALSE, ii);
		numitems++;
	}
}

/*-----------------------------------------------------------------------*
** Create any item of a specific type
**-----------------------------------------------------------------------*/

void CreateTypeItem(int x, int y, BOOL onlygood, int itype, int imisc, BOOL sendmsg, BOOL delta)
{
	int	ii, idx;

	int efflevel = GetEffLevel();
		
	if (itype != IT_GOLD) idx = RndTypeItems(itype, imisc, efflevel);
	else idx = 0;
	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(x,y,ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
#if CHEATS
		if (itemcheat)
			SetupAllItems(ii, idx, GetRndSeed(), efflevel << 1, 15, TRUE, FALSE, delta);
		else	
#endif
			SetupAllItems(ii, idx, GetRndSeed(), efflevel << 1, 1, onlygood, FALSE, delta);
		if (sendmsg) NetSendCmdDItem(FALSE, ii);
		if (delta) DeltaAddItem(ii);
		numitems++;
	}
}

/*-----------------------------------------------------------------------*
** Recreate any item with the proper info
**-----------------------------------------------------------------------*/

void RecreateItem(int ii, int idx, WORD icreateinfo, int iseed, int ivalue)
{
	int uper;
	BOOL onlygood, uavail, pregen;

	// PATCH1.JMM
	#if 0
	int i, nIndex;
	for(i = 0; i < numitems; i++) {
		nIndex = itemactive[i];

		if(((item[nIndex]._iSeed == iseed) && (item[nIndex]._iCreateInfo == icreateinfo) && (item[nIndex].IDidx == idx)))
			DROPLOG("  Creating dupped item:  idx->%8.8x seed->%8.8x ci->%8.8x\n",idx,iseed,icreateinfo);

		app_assert(!((item[nIndex]._iSeed == iseed) && (item[nIndex]._iCreateInfo == icreateinfo) && (item[nIndex].IDidx == idx)));
	}
	#endif
	// ENDPATCH1.JMM


	// Gold
	if (idx == IDI_GOLD) {
		SetPlrHandItem(&item[ii], idx);
		item[ii]._iSeed = iseed;
		item[ii]._iCreateInfo = icreateinfo;
		item[ii]._ivalue = ivalue;
		if (item[ii]._ivalue >= GOLD_VT2) {
			item[ii]._iCurs = ITEM_5GOLD;
		} else {
			if (ivalue <= GOLD_VT1) item[ii]._iCurs = ITEM_1GOLD;
			else item[ii]._iCurs = ITEM_3GOLD;
		}
	} else {
		// One the the players initial items?
		if (icreateinfo == 0) {
			SetPlrHandItem(&item[ii], idx);
			SetPlrHandSeed(&item[ii], iseed);
		} else {
			// From town?
			if (icreateinfo & ICI_TOWNMASK) RecreateTownItem(ii, idx, icreateinfo, iseed, ivalue);
			else {
				// From dungeon
				if ((icreateinfo & ICI_USEFUL) == ICI_USEFUL) {
					SetupAllUseful(ii, iseed, icreateinfo & ICI_LVLMASK);
				} else {
					uper = 0;
					onlygood = FALSE;
					uavail = FALSE;
					pregen = FALSE;
					if (icreateinfo & ICI_UPER1) uper = 1;
					if (icreateinfo & ICI_UPER15) uper = 15;
					if (icreateinfo & ICI_ONLYGOOD) onlygood = TRUE;
					if (icreateinfo & ICI_UNIQUE) uavail = TRUE;
					if (icreateinfo & ICI_PREGEN) pregen = TRUE;
					SetupAllItems(ii, idx, iseed, icreateinfo & ICI_LVLMASK, uper, onlygood, uavail, pregen);
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------*
** Recreate any ear with the proper info
**-----------------------------------------------------------------------*/

void RecreateEar(int ii, WORD ic, int iseed, BOOL Id, int dur, int mdur, int ch, int mch, int ivalue, int ibuff)
{
	SetPlrHandItem(&item[ii], IDI_EAR);
	tempstr[0] = (ic >> 8) & 0x7f;
	tempstr[1] = ic & 0x7f;
	tempstr[2] = (iseed >> 24) & 0x7f;
	tempstr[3] = (iseed >> 16) & 0x7f;
	tempstr[4] = (iseed >> 8) & 0x7f;
	tempstr[5] = iseed & 0x7f;
	tempstr[6] = Id & 0x7f;
	tempstr[7] = dur & 0x7f;
	tempstr[8] = mdur & 0x7f;
	tempstr[9] = ch & 0x7f;
	tempstr[10] = mch & 0x7f;
	tempstr[11] = (ivalue >> 8) & 0x7f;
	tempstr[12] = (ibuff >> 24) & 0x7f;
	tempstr[13] = (ibuff >> 16) & 0x7f;
	tempstr[14] = (ibuff >> 8) & 0x7f;
	tempstr[15] = ibuff & 0x7f;
	tempstr[16] = 0;
	sprintf(item[ii]._iName, "Ear of %s", tempstr);
	item[ii]._iCurs = ((ivalue >> 6) & 0x3) + ITEM_EAR1;
	item[ii]._ivalue = ivalue & 0x3f;
	item[ii]._iCreateInfo = ic;
	item[ii]._iSeed = iseed;
}


const char * sgszCornerstone = "SItem";

void CornerstoneSave()
{
	if (!CornerStone.Initted)
		return;
	
	PkItemStruct pki;
		
	if (CornerStone.item.IDidx >= 0)
	{
		PackItem(&pki, &(CornerStone.item));
		SRegSaveData(gszProgKey,sgszCornerstone,0, &pki, sizeof(PkItemStruct));
	}
	else
	{
		SRegSaveData(gszProgKey,sgszCornerstone, 0, "", 1);
	}
}

void CornerstoneRestore(int x, int y)
{
	if (CornerStone.Initted)
		return;
		
	// read the info out of the options file
	PkItemStruct pki;
	
	if (x == 0 || y == 0)
		return;
		
	CornerStone.item.IDidx = 0;
	CornerStone.Initted = TRUE;
	
	if (dItem[x][y] != 0)
	{
		// clear the square
		int ii = dItem[x][y] - 1;
		for (int i = 0; i < numitems; ++i)
			if (itemactive[i] == ii)
			{
				DeleteItem(ii, i);
				break;
			}
		dItem[x][y] = 0;
	}
	
	DWORD bytesread = 0;	
	if (! SRegLoadData(gszProgKey,sgszCornerstone, 0, &pki, sizeof(PkItemStruct), &bytesread))
		return;
		
	if (bytesread != sizeof(PkItemStruct))
		return;
	
	// create a new item slot
	int ii = itemavail[0];
	dItem[x][y] = ii+1;
	itemavail[0] = itemavail[MAXITEMS - numitems - 1];
	itemactive[numitems] = ii;
	
	UnPackItem(&pki, &item[ii]);
	item[ii]._ix = x;
	item[ii]._iy = y;
	
	DROPLOG("CornerstoneRestore: respawning object %d!\n",ii);
	RespawnItem(ii, FALSE);
	CornerStone.item = item[ii];
	
	numitems++;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SpawnQuestItem(int itemid, int x,int y, int randarea, int selflag)
{
	int	i,j;
	BOOL failed;

	int efflevel = GetEffLevel();
		
	if (randarea) {
		int tries = 0;
		do {
			if (++tries > 1000 && randarea > 1)
				--randarea;
			x = random(0, DMAXX);
			y = random(0, DMAXY);
			failed = FALSE;
			for (i = 0; i < randarea && !failed; i++)
				for (j = 0; j < randarea && !failed; j++)
					failed = !ItemSpaceOk(x + i, y + j);
		} while (failed);
	}

	if (numitems < MAXITEMS) {
		i = itemavail[0];
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = i;
		item[i]._ix = x;
		item[i]._iy = y;
		dItem[x][y] = i + 1;
		GetItemAttrs(i, itemid, efflevel);
		SetupItem(i);
		item[i]._iPostDraw = TRUE;
		if (selflag != ISEL_NONE) {
			// selflag indicates creation of post-flippy item
			item[i]._iSelFlag = selflag;
			item[i]._iAnimFrame = item[i]._iAnimLen;
			item[i]._iAnimFlag = FALSE;
		}

		numitems++;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void SpawnRock()
{
	int	i, ii, ostand;
	int xx,yy;
	BOOL done = FALSE;

	for (i = 0; i < numobjects && !done; i++) {
		ostand = objectactive[i];
		done = (object[ostand]._otype == 23);		// OBJ_STAND
	}
	
	int efflevel = GetEffLevel();
		
	if(done)
	{
		ii = itemavail[0];
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
		xx = item[ii]._ix = object[ostand]._ox;
		yy = item[ii]._iy = object[ostand]._oy;
		dItem[xx][yy] = ii + 1;
		GetItemAttrs(ii, IDI_ROCK, efflevel);
		SetupItem(ii);
		item[ii]._iSelFlag = ISEL_TOP;
		item[ii]._iPostDraw = TRUE;
		item[ii]._iAnimFrame = 11;

		numitems++;
	}
}

void SpawnSomething(int what, int xx, int yy)
{
	int ii = itemavail[0];
	
	int efflevel = GetEffLevel();
		
	itemavail[0] = itemavail[MAXITEMS - numitems - 1];
	itemactive[numitems] = ii;
	item[ii]._ix = xx;
	item[ii]._iy = yy;
	dItem[xx][yy] = ii + 1;
	GetItemAttrs(ii, what, efflevel);
	SetupItem(ii);
	item[ii]._iSelFlag = ISEL_TOP;
	item[ii]._iPostDraw = TRUE;
	item[ii]._iAnimFrame = 1; // item[ii]._iAnimLen;
	item[ii]._iAnimFlag = TRUE;
	item[ii]._iIdentified = TRUE;

	numitems++;
}

void SpawnMap(int xx, int yy)
{
 	SpawnSomething(IDI_MAPOFDOOM, xx, yy);
}

void SpawnBomb(int xx, int yy)
{
	SpawnSomething(IDI_RUNEBOMB, xx, yy);
}

void SpawnBear(int xx, int yy)
{
	SpawnSomething(IDI_THEODORE, xx, yy);
}

void SpawnCowArmor(int xx, int yy)
{
	CreateItem(UID_ARMRCOW, xx, yy);
}
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void RespawnItem(int i, BOOL FlipFlag)
{
	int it;

	DROPLOG("  RespawnItem:  Respawning item %d!\n",i);

	it = ItemCAnimTbl[item[i]._iCurs];
	item[i]._iAnimData = itemanims[it];
	item[i]._iAnimLen = ItemAnimLs[it];
	item[i]._iAnimWidth = 96;
	item[i]._iAnimWidth2 = 16;
	item[i]._iPostDraw = FALSE;
	item[i]._iRequest = FALSE;
	if (FlipFlag) {
		item[i]._iAnimFrame = 1;
		item[i]._iAnimFlag = TRUE;
		item[i]._iSelFlag = ISEL_NONE;
	} else {
		item[i]._iAnimFrame = item[i]._iAnimLen;
		item[i]._iAnimFlag = FALSE;
		item[i]._iSelFlag = ISEL_FLR;
	}
	if (item[i]._iCurs == ITEM_ROCK) {
		item[i]._iSelFlag = ISEL_FLR;
		PlaySfxLoc(ItemAnimSnds[it], item[i]._ix, item[i]._iy);
	}
	if (item[i]._iCurs == ITEM_INNSIGN) item[i]._iSelFlag = ISEL_FLR;
	if (item[i]._iCurs == ITEM_ANVIL) item[i]._iSelFlag = ISEL_FLR;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void DeleteItem(int ii, int i)
{
	itemavail[MAXITEMS - numitems] = ii;
	numitems--;
	if ((numitems > 0) && (i != numitems)) {
		itemactive[i] = itemactive[numitems];
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int idoppely = DIRTEDGED2;

void ItemDoppel()
{
	int idoppelx;
	ItemStruct *i;

	if (gbMaxPlayers == 1) return;

	for (idoppelx = DIRTEDGED2; idoppelx < (DIRTEDGED2+80); idoppelx++) {
		if (dItem[idoppelx][idoppely]) {
			i = &item[dItem[idoppelx][idoppely] - 1];
			if ((i->_ix != idoppelx) || (i->_iy != idoppely))
				dItem[idoppelx][idoppely] = 0;
		}
	}
	idoppely++;
	if (idoppely == DIRTEDGED2+80) idoppely = DIRTEDGED2;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void ProcessItems()
{
	int		i, ii, it;

	VerifyItemActiveList();

	for (i = 0; i < numitems; i++) {
		ii = itemactive[i];

		// Animate Spell GFX
		if (item[ii]._iAnimFlag) {
			item[ii]._iAnimFrame++;
			if (item[ii]._iCurs == ITEM_ROCK) {
				if ((item[ii]._iSelFlag == ISEL_FLR) && (item[ii]._iAnimFrame == 11))
					item[ii]._iAnimFrame = 1;
				if ((item[ii]._iSelFlag == ISEL_TOP) && (item[ii]._iAnimFrame == 21))
					item[ii]._iAnimFrame = 11;
			} else {
				if (item[ii]._iAnimFrame == (item[ii]._iAnimLen >> 1)) {
					it = ItemCAnimTbl[item[ii]._iCurs];
					PlaySfxLoc(ItemAnimSnds[it], item[ii]._ix, item[ii]._iy);
				} 
				if (item[ii]._iAnimFrame >= item[ii]._iAnimLen) {
					item[ii]._iAnimFrame = item[ii]._iAnimLen;
					item[ii]._iAnimFlag = FALSE;
					item[ii]._iSelFlag = ISEL_FLR;
				}
			}
		}
	}

	ItemDoppel();
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void FreeItemGFX()
{
	for (int i = 0; i < ITEMFTYPES; i++)
		DiabloFreePtr(itemanims[i]);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SyncItemAnim(int ii)
{
	int a;
	
	a = ItemCAnimTbl[item[ii]._iCurs];
	item[ii]._iAnimData = itemanims[a];
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void GetItemStr(int i) {

	switch (item[i]._itype) {
		case IT_GOLD: {
			int nGold = item[i]._ivalue;
			const char * get_pieces_str(int nGold);
			sprintf(infostr,"%i gold %s",nGold,get_pieces_str(nGold));
		}
		break;

		default :
			int s = item[i]._itype;
			if (item[i]._iIdentified) strcpy(infostr, item[i]._iIName);
			else strcpy(infostr, item[i]._iName);
			if (item[i]._iMagical == IMAGIC_MAGIC) infoclr = ICOLOR_BLUE;
			if (item[i]._iMagical == IMAGIC_UNIQUE) infoclr = ICOLOR_GOLD;
		break;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void CheckIdentify(int pnum, int cii) {
	ItemStruct * pi;
	if (cii < NUM_INVLOC)
		pi = &plr[pnum].InvBody[cii];
	else
		pi = &plr[pnum].InvList[cii - NUM_INVLOC];
	pi->_iIdentified = TRUE;
	CalcPlrInv(pnum,TRUE);

	if (pnum == myplr)
		NewCursor(GLOVE_CURS);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void RepairItem(ItemStruct *i, int lvl) {
	int d, rep;

	if (i->_iDurability == i->_iMaxDur) return;
	// If item already has zero durability, wipe it out
	if (i->_iMaxDur <= 0) {
		i->_itype = -1;
		return;
	}
	rep = 0;
	do {
		rep += random(37, lvl) + lvl;
		d = i->_iMaxDur / (9 + lvl);
		if (d < 1) d = 1;
		i->_iMaxDur -= d;
		// If I have no max durability, break
		if (i->_iMaxDur == 0) {
			i->_itype = -1;
			return;
		}
	} while ((i->_iDurability + rep) < i->_iMaxDur);
	i->_iDurability += rep;
	if (i->_iDurability > i->_iMaxDur)
		i->_iDurability = i->_iMaxDur;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void DoRepair(int pnum, int cii) {
	PlayerStruct * p = &plr[pnum];
	PlaySfxLoc(IS_REPAIR, p->_px, p->_py);

	ItemStruct * pi;
	if (cii < NUM_INVLOC)
		pi = &p->InvBody[cii];
	else
		pi = &p->InvList[cii - NUM_INVLOC];
	RepairItem(pi,p->_pLevel);
	CalcPlrInv(pnum,TRUE);

	if (pnum == myplr)
		NewCursor(GLOVE_CURS);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void RechargeItem(ItemStruct *i, int r) {
	if (i->_iCharges == i->_iMaxCharges) return;

	do {
		i->_iMaxCharges--;
		if (i->_iMaxCharges == 0) {
			//i->_itype = -1;
			return;
		}
		i->_iCharges += r;
	} while (i->_iCharges < i->_iMaxCharges);

	if (i->_iCharges > i->_iMaxCharges)	i->_iCharges = i->_iMaxCharges;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void DoRecharge(int pnum, int cii) {
	PlayerStruct * p = &plr[pnum];
	
	ItemStruct * pi;
	if (cii < NUM_INVLOC) pi = &p->InvBody[cii];
	else pi = &p->InvList[cii - NUM_INVLOC];
// rmw.patch1.start.1/14/97
// if (pi->_itype == IT_STAFF) {
	if ((pi->_itype == IT_STAFF) && (pi->_iSpell != SPL_NONE)) {
// rmw.patch1.end.1/14/97
		int sp = pi->_iSpell;
		int r = spelldata[sp].sBookLvl;
		r = random(38, p->_pLevel / r) + 1;

		RechargeItem(pi, r);
		CalcPlrInv(pnum, TRUE);
	}

	if (pnum == myplr) NewCursor(GLOVE_CURS);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static BOOL OilItem(ItemStruct * i,const PlayerStruct * p) {
	int v;

	if (i->_iClass == IC_ITEM) return(FALSE);
	if (i->_iClass == IC_GOLD) return(FALSE);
	if (i->_iClass == IC_SPECIAL) return(FALSE);
	switch(p->_pOilType) {
		case IMID_OILACC :
		case IMID_OILMAST :
		case IMID_OILSHRP :
			if (i->_iClass == IC_ARMOR) return(FALSE);
		break;
		case IMID_OILDEATH :
			if (i->_iClass == IC_ARMOR) return(FALSE);
			if (i->_itype == IT_BOW) return(FALSE);
		break;
		case IMID_OILHARD :
		case IMID_OILIMPER :
			if (i->_iClass == IC_WEAP) return(FALSE);
		break;
	}

	switch(p->_pOilType) {
		case IMID_OILACC :
			if (i->_iPLToHit >= 50) break;
			i->_iPLToHit += random(68, 2) + 1;
		break;
		case IMID_OILMAST :
			if (i->_iPLToHit >= 100) break;
			i->_iPLToHit += random(68, 3) + 3;
		break;
		case IMID_OILSHRP :
			if (i->_iMaxDam - i->_iMinDam >= 30) break;
			i->_iMaxDam += 1;
		break;
		case IMID_OILDEATH :
			if (i->_iMaxDam - i->_iMinDam >= 30) break;
			i->_iMinDam += 1;
			i->_iMaxDam += 2;
		break;
		case IMID_OILSKILL :
			v = random(68, 6) + 5;
			
			if (i->_iMinStr > v) {
				i->_iMinStr -= v;
			}
			else {
				i->_iMinStr = 0;
			}
			
			if (i->_iMinMag > v) {
				i->_iMinMag -= v;
			}
			else {
				i->_iMinMag = 0;
			}
			
			if (i->_iMinDex > v) {
				i->_iMinDex -= v;
			}
			else {
				i->_iMinDex = 0;
			}
		break;
		case IMID_OILBLKSM :
			if (i->_iMaxDur == INFINITE_DUR) break;
//			i->_iDurability = (i->_iDurability + (random(68, 5) + 2));
//			i->_iMaxDur = (i->_iMaxDur + (random(68, 9) + 7));
			if (i->_iDurability < i->_iMaxDur)
			{
				// fix by 20%
				int tmp = i->_iDurability + ((i->_iMaxDur + 4) / 5);
				if (tmp > i->_iMaxDur)
					tmp = i->_iMaxDur;
				i->_iDurability = tmp;
			}
			else
			{
				if (i->_iMaxDur >= 100) break;
				i->_iMaxDur += 1;
				i->_iDurability = i->_iMaxDur;
			}
		break;
		case IMID_OILFORT :
			if (i->_iMaxDur == INFINITE_DUR) break;
			if (i->_iMaxDur >= 200) break;
			v = random(68, 41) + 10;
			i->_iMaxDur += v;
			i->_iDurability += v; 
		break;
		case IMID_OILPERM :
			i->_iDurability = INFINITE_DUR;
			i->_iMaxDur = INFINITE_DUR;
		break;
		case IMID_OILHARD :
			if (i->_iAC >= 60) break;
			i->_iAC += random(68, 2) + 1;
		break;
		case IMID_OILIMPER :
			if (i->_iAC >= 120) break;
			i->_iAC += random(68, 3) + 3;
		break;
	}

	return(TRUE);
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void DoOil(int pnum, int cii) {
	ItemStruct * pi;
	PlayerStruct * p = &plr[pnum];
	if (cii < NUM_INVLOC) switch (cii) {
		case INVLOC_HEAD:
		case INVLOC_HAND1:
		case INVLOC_HAND2:
		case INVLOC_BODY:
			pi = &p->InvBody[cii];
		break;

		default:
		return;
	}
	else {
		pi = &p->InvList[cii - NUM_INVLOC];
	}

	if (OilItem(pi,p)) {
		CalcPlrInv(pnum,TRUE);
		if (pnum == myplr) NewCursor(GLOVE_CURS);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void PrintItemOil(char IDidx)
{
	switch(IDidx) {
		case IMID_OILACC :
			strcpy(tempstr, "increases a weapon's");
			AddPanelString(tempstr, TEXT_CENTER);
			strcpy(tempstr, "chance to hit");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_OILMAST :
			strcpy(tempstr, "greatly increases a");
			AddPanelString(tempstr, TEXT_CENTER);
			strcpy(tempstr, "weapon's chance to hit");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_OILSHRP :
			strcpy(tempstr, "increases a weapon's");
			AddPanelString(tempstr, TEXT_CENTER);
			strcpy(tempstr, "damage potential");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_OILDEATH :
			strcpy(tempstr, "greatly increases a weapon's");
			AddPanelString(tempstr, TEXT_CENTER);
			strcpy(tempstr, "damage potential - not bows");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_OILSKILL :
			strcpy(tempstr, "reduces attributes needed");
			AddPanelString(tempstr, TEXT_CENTER);
			strcpy(tempstr, "to use armor or weapons");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_OILBLKSM :
			strcpy(tempstr, "restores 20% of an");
			AddPanelString(tempstr, TEXT_CENTER);
			strcpy(tempstr, "item's durability");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_OILFORT :
			strcpy(tempstr, "increases an item's");
			AddPanelString(tempstr, TEXT_CENTER);
			strcpy(tempstr, "current and max durability");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_OILPERM :
			strcpy(tempstr, "makes an item indestructible");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_OILHARD :
			strcpy(tempstr, "increases the armor class");
			AddPanelString(tempstr, TEXT_CENTER);
			strcpy(tempstr, "of armor and shields");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_OILIMPER :
			strcpy(tempstr, "greatly increases the armor");
			AddPanelString(tempstr, TEXT_CENTER);
			strcpy(tempstr, "class of armor and shields");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
// potions
		case IMID_PHEAL :
			strcpy(tempstr, "fully recover life");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_PLHEAL :
			strcpy(tempstr, "recover partial life");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_PSHEAL :
			strcpy(tempstr, "recover life");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_PDHEAL :
			strcpy(tempstr, "deadly heal");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_PMANA :
			strcpy(tempstr, "recover mana");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_PFMANA :
			strcpy(tempstr, "fully recover mana");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_ESTR :
			strcpy(tempstr, "increase strength");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_EMAG :
			strcpy(tempstr, "increase magic");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_EDEX :
			strcpy(tempstr, "increase dexterity");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_EVIT :
			strcpy(tempstr, "increase vitality");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_ENSTR :
			strcpy(tempstr, "decrease strength");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_ENMAG :
			strcpy(tempstr, "decrease strength");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_ENDEX :
			strcpy(tempstr, "decrease dexterity");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_ENVIT :
			strcpy(tempstr, "decrease vitality");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_REJUV :
			strcpy(tempstr, "recover life and mana");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_FREJUV :
			strcpy(tempstr, "fully recover life and mana");
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_RUNEFIRE:
		case IMID_RUNEIMMOLATE:
			strcpy(tempstr, "sets fire trap");	
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_RUNELIGHT:
		case IMID_RUNENOVA:
			strcpy(tempstr, "sets lightning trap");	
			AddPanelString(tempstr, TEXT_CENTER);
			break;
		case IMID_RUNESTONE:
			strcpy(tempstr, "sets petrification trap");	
			AddPanelString(tempstr, TEXT_CENTER);
			break;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void PrintItemPower(char plidx,const ItemStruct * x) {
	int v;

	switch(plidx) {
		case PL_TOHIT:
		case PL_NTOHIT:
			sprintf(tempstr, "chance to hit : %+i%%", x->_iPLToHit);
			break;
		case PL_TODAM:
		case PL_NTODAM:
			sprintf(tempstr, "%+i%% damage", x->_iPLDam);
			break;
		case PL_DAHT:
		case PL_NDAHT:
			sprintf(tempstr, "to hit: %+i%%, %+i%% damage", x->_iPLToHit, x->_iPLDam);
			break;
		case PL_AC:
		case PL_NAC:
			sprintf(tempstr, "%+i%% armor", x->_iPLAC);
			break;
		case PL_ACTUALAC:
			sprintf(tempstr, "armor class: %i", x->_iAC);
			break;
		case PL_NACTULAC:
			sprintf(tempstr, "armor class: %i", x->_iAC);
			break;
		case PL_RFIRE:
		case PL_NRFIRE:
			if (x->_iPLFR < 75)
				sprintf(tempstr, "Resist Fire : %+i%%", x->_iPLFR);
			else
				sprintf(tempstr, "Resist Fire : 75%% MAX");
			break;
		case PL_RLGHT:
		case PL_NRLGHT:
			if (x->_iPLLR < 75)
				sprintf(tempstr, "Resist Lightning : %+i%%", x->_iPLLR);
			else
				sprintf(tempstr, "Resist Lightning : 75%% MAX");
			break;
		case PL_RMAG:
		case PL_NRMAG:
			if (x->_iPLMR < 75)
				sprintf(tempstr, "Resist Magic : %+i%%", x->_iPLMR);
			else
				sprintf(tempstr, "Resist Magic : 75%% MAX");
			break;
		case PL_RALL:
		case PL_NRALL:	
			if (x->_iPLFR < 75) sprintf(tempstr, "Resist All : %+i%%", x->_iPLFR);
			if (x->_iPLFR >= 75) sprintf(tempstr, "Resist All : 75%% MAX");
			break;
		case PL_SLVL:
#if 0
			if (x->_iSplLvlAdd == 1 ) strcpy(tempstr, "spells are increased 1 level");
			if (x->_iSplLvlAdd == 2 ) strcpy(tempstr, "spells are increased 2 levels");
			if (x->_iSplLvlAdd < 1 ) strcpy(tempstr, "spells are decreased 1 level");
#endif
			if (x->_iSplLvlAdd == 1) strcpy(tempstr, "spells are increased 1 level");
			else if (x->_iSplLvlAdd > 1 ) sprintf(tempstr, "spells are increased %i levels", x->_iSplLvlAdd);
			else if (x->_iSplLvlAdd == -1) strcpy(tempstr, "spells are decreased 1 level");
			else if (x->_iSplLvlAdd < -1 ) sprintf(tempstr, "spells are decreased %i levels", x->_iSplLvlAdd);
			else if (x->_iSplLvlAdd == 0 ) strcpy(tempstr, "spell levels unchanged (?)");
			break;
		case PL_CHRG:
			strcpy(tempstr, "Extra charges");
			break;
		case PL_SPELL:
			sprintf(tempstr, "%i %s charges", x->_iMaxCharges, spelldata[x->_iSpell].sNameText);
			break;
		case PL_FHIT:
			if (x->_iFMinDam == x->_iFMaxDam)
				sprintf(tempstr, "Fire hit damage: %i", x->_iFMinDam);
			else
				sprintf(tempstr, "Fire hit damage: %i-%i", x->_iFMinDam, x->_iFMaxDam);
			break;
		case PL_LHIT:
			if (x->_iLMinDam == x->_iLMaxDam)
				sprintf(tempstr, "Lightning hit damage: %i", x->_iLMinDam);
			else
				sprintf(tempstr, "Lightning hit damage: %i-%i", x->_iLMinDam, x->_iLMaxDam);
			break;
		case PL_STR:
		case PL_NSTR:
			sprintf(tempstr, "%+i to strength", x->_iPLStr);
			break;
		case PL_MAG:
		case PL_NMAG:
			sprintf(tempstr, "%+i to magic", x->_iPLMag);
			break;
		case PL_DEX:
		case PL_NDEX:
			sprintf(tempstr, "%+i to dexterity", x->_iPLDex);
			break;
		case PL_VIT:
		case PL_NVIT:
			sprintf(tempstr, "%+i to vitality", x->_iPLVit);
			break;
		case PL_STATS:
		case PL_NSTATS:
			sprintf(tempstr, "%+i to all attributes", x->_iPLStr);
			break;
		case PL_GETHIT:
		case PL_NGETHIT:
			sprintf(tempstr, "%+i damage from enemies", x->_iPLGetHit);
			break;
		case PL_HP:
		case PL_NHP:
			sprintf(tempstr, "Hit Points : %+i", (x->_iPLHP >> HP_SHIFT));
			break;
		case PL_MANA:
		case PL_NMANA:
			sprintf(tempstr, "Mana : %+i", (x->_iPLMana >> MANA_SHIFT));
			break;
		case PL_DUR:
			strcpy(tempstr, "high durability");
			break;
		case PL_NDUR:
			strcpy(tempstr, "decreased durability");
			break;
		case PL_IND:
			strcpy(tempstr, "indestructible");
			break;
		case PL_LIGHT:
			v = x->_iPLLight * 10;
			sprintf(tempstr, "+%i%% light radius", v);
			break;
		case PL_NLIGHT:
			v = -x->_iPLLight * 10;
			sprintf(tempstr, "-%i%% light radius", v);
			break;
		case PL_NUMARWS:
			sprintf(tempstr, "multiple arrows per shot");
			break;
		case PL_FARROW:
			if (x->_iFMinDam == x->_iFMaxDam)
				sprintf(tempstr, "fire arrows damage: %i", x->_iFMinDam);
			else
				sprintf(tempstr, "fire arrows damage: %i-%i", x->_iFMinDam, x->_iFMaxDam);
			break;
		case PL_LARROW:
			if (x->_iLMinDam == x->_iLMaxDam)
				sprintf(tempstr, "lightning arrows damage %i", x->_iLMinDam);
			else
				sprintf(tempstr, "lightning arrows damage %i-%i", x->_iLMinDam, x->_iLMaxDam);
			break;
		case PL_HITADD:	// usurped for fireball arrows...
			if (x->_iFMinDam == x->_iFMaxDam)
				sprintf(tempstr, "fireball damage: %i", x->_iFMinDam);
			else
				sprintf(tempstr, "fireball damage: %i-%i", x->_iFMinDam, x->_iFMaxDam);
			break;
		case PL_THORN:
			strcpy(tempstr, "attacker takes 1-3 damage");
			break;
		case PL_LMANA:
			strcpy(tempstr, "user loses all mana");
			break;
		case PL_NOHEAL:
			strcpy(tempstr, "you can't heal");
			break;
		case PL_TRAPDAM:
			strcpy(tempstr, "absorbs half of trap damage");
			break;
		case PL_BEAR:
			strcpy(tempstr, "knocks target back");
			break;
		case PL_DAMDEMON:
			strcpy(tempstr, "+200% damage vs. demons");
			break;
		case PL_ZERORES:
			strcpy(tempstr, "All Resistance equals 0");
			break;
		case PL_MNOHEAL:
			strcpy(tempstr, "hit monster doesn't heal");
			break;
		case PL_BAT:
			if (x->_iFlags & IAF_BAT10) strcpy(tempstr, "hit steals 3% mana");
			if (x->_iFlags & IAF_BAT20) strcpy(tempstr, "hit steals 5% mana");
			break;
		case PL_LEECH:
			if (x->_iFlags & IAF_LEECH10) strcpy(tempstr, "hit steals 3% life");
			if (x->_iFlags & IAF_LEECH20) strcpy(tempstr, "hit steals 5% life");
			break;
		case PL_ENAC:
			strcpy(tempstr, "penetrates target's armor");
			break;
		case PL_ATANIM:
			if (x->_iFlags & IAF_ATANIM1) strcpy(tempstr, "quick attack");
			if (x->_iFlags & IAF_ATANIM2) strcpy(tempstr, "fast attack");
			if (x->_iFlags & IAF_ATANIM3) strcpy(tempstr, "faster attack");
			if (x->_iFlags & IAF_ATANIM4) strcpy(tempstr, "fastest attack");
			break;
		case PL_HTANIM:
			if (x->_iFlags & IAF_HTANIM1) strcpy(tempstr, "fast hit recovery");
			if (x->_iFlags & IAF_HTANIM2) strcpy(tempstr, "faster hit recovery");
			if (x->_iFlags & IAF_HTANIM3) strcpy(tempstr, "fastest hit recovery");
			break;
		case PL_BLANIM:
			strcpy(tempstr, "fast block");
			break;
		case PL_DAMADD:
			sprintf(tempstr, "adds %i points to damage", x->_iPLDamMod);
			break;
		case PL_RNDARW:
			strcpy(tempstr, "fires random speed arrows");
			break;
		case PL_DAMAGE:
			sprintf(tempstr, "unusual item damage");
			break;
		case PL_DURNUM:
			strcpy(tempstr, "altered durability");
			break;
		case PL_FALCON:
			strcpy(tempstr, "Faster attack swing");
			// not used
			break;
		case PL_ONEHAND:
			strcpy(tempstr, "one handed sword");
			break;
		case PL_CONST:
			strcpy(tempstr, "constantly lose hit points");
			break;
		case PL_SKING:
			strcpy(tempstr, "life stealing");
			break;
		case PL_NSTRREQ:
			strcpy(tempstr, "no strength requirement");
			break;
		case PL_INFRA:
			strcpy(tempstr, "see with infravision");
			break;
		case PL_GFX:
			strcpy(tempstr, " ");
			break;
		case PL_HARQUN:
			if (x->_iFMinDam == x->_iFMaxDam)
				sprintf(tempstr, "lightning damage: %i", x->_iFMinDam);
			else
				sprintf(tempstr, "lightning damage: %i-%i", x->_iFMinDam, x->_iFMaxDam);
//			strcpy(tempstr, "Armor class added to life");
			break;
		case PL_HARQUN2:
			strcpy(tempstr, "charged bolts on hits");
//			strcpy(tempstr, "10% of mana added to armor");
			break;
		case PL_HARQUN3:
			if (x->_iPLFR <= 0) sprintf(tempstr, " ");
			else if (x->_iPLFR >= 1) sprintf(tempstr, "Resist Fire : %+i%%", x->_iPLFR);
			break;
		case PL_DEVAST:
			strcpy(tempstr, "occasional triple damage");
			break;
		case PL_DECAY:
			sprintf(tempstr, "decaying %+i%% damage", x->_iPLDam);
			break;
		case PL_PERIL:
			strcpy(tempstr, "2x dmg to monst, 1x to you");
			break;
		case PL_RNDDAM:
			strcpy(tempstr, "Random 0 - 500% damage");
			break;
		case PL_FRAGILE:
			sprintf(tempstr, "low dur, %+i%% damage", x->_iPLDam);
			break;
		case PL_DOPPEL:
			sprintf(tempstr, "to hit: %+i%%, %+i%% damage", x->_iPLToHit, x->_iPLDam);
			break;
		case PL_DEMONAC:
			sprintf(tempstr, "extra AC vs demons");
			break;
		case PL_UNDEADAC:
			sprintf(tempstr, "extra AC vs undead");
			break;
		case PL_ACOLYTE:
			sprintf(tempstr, "50%% Mana moved to Health");
			break;
		case PL_GLADIATR:
			sprintf(tempstr, "40%% Health moved to Mana");
			break;
			
		default:
			strcpy(tempstr, "Another ability (NW)");
			break;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void DrawUBack()
{
	DrawCel(88, 487, pSTextBoxCels, 1, 271);
	app_assert(gpBuffer);

	__asm {
		mov		edi,dword ptr [gpBuffer]
		add		edi,371803

		xor		eax,eax
		mov		edx,148
_YLp:	mov		ecx,132
_XLp1:	stosb
		inc		edi
		loop	_XLp1
		stosb
		sub		edi,1033
		mov		ecx,132
_XLp2:	inc		edi
		stosb
		loop	_XLp2
		sub		edi,1032
		dec		edx
		jnz		_YLp
		mov		ecx,132
_XLp3:	stosb
		inc		edi
		loop	_XLp3
		stosb
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void PrintUString(int x, int y, BOOL cjustflag, char str[], char col)
{
	long	boffset;
	int		sl,i,w,tw,yy;

	yy = SStringY[y];
	boffset = nBuffWTbl[yy + 204] + x + 96;
	sl = strlen(str);
	w = 0;
	if (cjustflag) {
		tw = 0;
		for (i = 0; i < sl; i++) {
			BYTE c = char2print(str[i]);
			c = fonttrans[c];
			tw += fontkern[c]+1;
		}
		if (tw < 257) w = (257 - tw) >> 1;
		boffset += w;
	}
	for (i = 0; i < sl; i++) {
		BYTE c = char2print(str[i]);
		c = fonttrans[c];
		w += fontkern[c]+1;
		if ((c != 0) && (w <= 257)) DrawPanelFont(boffset, c, col);
		boffset += fontkern[c]+1;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DrawULine(int y)
{
	long	doffset;

	app_assert(gpBuffer);
	doffset = nBuffWTbl[SStringY[y] + 198] + 90;
	__asm {
		mov		esi,dword ptr [gpBuffer]
		mov		edi,esi
		add		esi,142170
		add		edi,dword ptr [doffset]

		mov		ebx,502

		mov		edx,3
_YLp:	mov		ecx,66
		rep movsd
		movsw
		add		esi,ebx
		add		edi,ebx
		dec		edx
		jnz		_YLp
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void DrawUniqueInfo()
{
	int u,y;

	if (chrflag || questlog) return;
	u = curruitem._iUid;
	DrawUBack();
	PrintUString(0, 2, TRUE, UniqueItemList[u].UIName, ICOLOR_GOLD);
	DrawULine(5);
	PrintItemPower(UniqueItemList[u].UIPower1, &curruitem);
	y = (6 - UniqueItemList[u].UINumPL) + 8;
	PrintUString(0, y, TRUE, tempstr, ICOLOR_WHITE);
	if (UniqueItemList[u].UINumPL > 1) {
		PrintItemPower(UniqueItemList[u].UIPower2, &curruitem);
		PrintUString(0, y+2, TRUE, tempstr, ICOLOR_WHITE);
	}
	if (UniqueItemList[u].UINumPL > 2) {
		PrintItemPower(UniqueItemList[u].UIPower3, &curruitem);
		PrintUString(0, y+4, TRUE, tempstr, ICOLOR_WHITE);
	}
	if (UniqueItemList[u].UINumPL > 3) {
		PrintItemPower(UniqueItemList[u].UIPower4, &curruitem);
		PrintUString(0, y+6, TRUE, tempstr, ICOLOR_WHITE);
	}
	if (UniqueItemList[u].UINumPL > 4) {
		PrintItemPower(UniqueItemList[u].UIPower5, &curruitem);
		PrintUString(0, y+8, TRUE, tempstr, ICOLOR_WHITE);
	}
	if (UniqueItemList[u].UINumPL > 5) {
		PrintItemPower(UniqueItemList[u].UIPower6, &curruitem);
		PrintUString(0, y+10, TRUE, tempstr, ICOLOR_WHITE);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void PrintItemMisc(const ItemStruct * x)
{
	if (x->_iMiscId == IMID_SCROLL) {
		strcpy(tempstr, "Right-click to read");
		AddPanelString(tempstr, TEXT_CENTER);
	}
	if (x->_iMiscId == IMID_TSCROLL) {
		strcpy(tempstr, "Right-click to read, then");
		AddPanelString(tempstr, TEXT_CENTER);
		strcpy(tempstr, "left-click to target");
		AddPanelString(tempstr, TEXT_CENTER);
	}
	if ((x->_iMiscId >= IMID_FIRSTPOT) && (x->_iMiscId <= IMID_LASTPOT)) {
		PrintItemOil(x->_iMiscId);
		strcpy(tempstr, "Right click to use");
		AddPanelString(tempstr, TEXT_CENTER);
	}
	if ((x->_iMiscId > IMID_FIRSTOIL) && (x->_iMiscId < IMID_LASTOIL)) {
		PrintItemOil(x->_iMiscId);
		strcpy(tempstr, "Right click to use");
		AddPanelString(tempstr, TEXT_CENTER);
	}
	if ((x->_iMiscId > IMID_FIRSTRUNE) && (x->_iMiscId < IMID_LASTRUNE)) {
		PrintItemOil(x->_iMiscId);
		strcpy(tempstr, "Right click to use");
		AddPanelString(tempstr, TEXT_CENTER);
	}
	if (x->_iMiscId == IMID_BOOK) {
		strcpy(tempstr, "Right click to read");
		AddPanelString(tempstr, TEXT_CENTER);
	}
	if (x->_iMiscId == IMID_FULLNOTE)
	{
		strcpy(tempstr, "Right click to read");
		AddPanelString(tempstr, TEXT_CENTER);
	}
	if (x->_iMiscId == IMID_MAPOFDOOM) {
		strcpy(tempstr, "Right click to view");
		AddPanelString(tempstr, TEXT_CENTER);
	}
	if (x->_iMiscId == IMID_EAR) {
		sprintf(tempstr, "Level : %i", x->_ivalue);
		AddPanelString(tempstr, TEXT_CENTER);
	}
	if (x->_iMiscId == IMID_AURIC) {
		strcpy(tempstr, "Doubles gold capacity");
		AddPanelString(tempstr, TEXT_CENTER);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void PrintItemDetails(const ItemStruct * x)
{
	if (x->_iClass == IC_WEAP) {
		if (x->_iMinDam == x->_iMaxDam)
		{
			if (x->_iMaxDur == INFINITE_DUR)
				sprintf(tempstr, "damage: %i  Indestructible", x->_iMinDam);
			else
				sprintf(tempstr, "damage: %i  Dur: %i/%i", x->_iMinDam, x->_iDurability, x->_iMaxDur);
		}
		else
		{
			if (x->_iMaxDur == INFINITE_DUR)
				sprintf(tempstr, "damage: %i-%i  Indestructible", x->_iMinDam, x->_iMaxDam);
			else
				sprintf(tempstr, "damage: %i-%i  Dur: %i/%i", x->_iMinDam, x->_iMaxDam, x->_iDurability, x->_iMaxDur);
		}
		AddPanelString(tempstr, TEXT_CENTER);
	}
	if (x->_iClass == IC_ARMOR) {
		if (x->_iMaxDur == INFINITE_DUR)
			sprintf(tempstr, "armor: %i  Indestructible", x->_iAC);
		else
			sprintf(tempstr, "armor: %i  Dur: %i/%i", x->_iAC, x->_iDurability, x->_iMaxDur);
		AddPanelString(tempstr, TEXT_CENTER);
	}
	if ((x->_iMiscId == IMID_STAFF) && (x->_iMaxCharges != 0)) {
		if (x->_iMinDam == x->_iMaxDam)
			sprintf(tempstr, "dam: %i  Dur: %i/%i", x->_iMinDam, x->_iDurability, x->_iMaxDur);
		else
			sprintf(tempstr, "dam: %i-%i  Dur: %i/%i", x->_iMinDam, x->_iMaxDam, x->_iDurability, x->_iMaxDur);
		sprintf(tempstr, "Charges: %i/%i", x->_iCharges, x->_iMaxCharges);
		AddPanelString(tempstr, TEXT_CENTER);
	}
	if (x->_iPrePower != -1) {
		PrintItemPower(x->_iPrePower, x);
		AddPanelString(tempstr, TEXT_CENTER);
	}
	if (x->_iSufPower != -1) {
		PrintItemPower(x->_iSufPower, x);
		AddPanelString(tempstr, TEXT_CENTER);
	}
	if (x->_iMagical == IMAGIC_UNIQUE) {
		AddPanelString("unique item", TEXT_CENTER);
		uitemflag = TRUE;
		curruitem = *x;
	}
	PrintItemMisc(x);
	if ((x->_iMinStr + x->_iMinMag + x->_iMinDex) != 0) {
		strcpy(tempstr, "Required:");
		if (x->_iMinStr != 0) sprintf(tempstr, "%s %i Str", tempstr, x->_iMinStr);
		if (x->_iMinMag != 0) sprintf(tempstr, "%s %i Mag", tempstr, (byte) x->_iMinMag);
		if (x->_iMinDex != 0) sprintf(tempstr, "%s %i Dex", tempstr, x->_iMinDex);
		AddPanelString(tempstr, TEXT_CENTER);
	}
	pinfoflag = TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void PrintItemDur(const ItemStruct * x)
{
	if (x->_iClass == IC_WEAP) {
		if (x->_iMinDam == x->_iMaxDam)
		{
			if (x->_iMaxDur == INFINITE_DUR)
				sprintf(tempstr, "damage: %i  Indestructible", x->_iMinDam);
			else
				sprintf(tempstr, "damage: %i  Dur: %i/%i", x->_iMinDam, x->_iDurability, x->_iMaxDur);
		}
		else
		{
			if (x->_iMaxDur == INFINITE_DUR)
				sprintf(tempstr, "damage: %i-%i  Indestructible", x->_iMinDam, x->_iMaxDam);
			else
				sprintf(tempstr, "damage: %i-%i  Dur: %i/%i", x->_iMinDam, x->_iMaxDam, x->_iDurability, x->_iMaxDur);
		}
		AddPanelString(tempstr, TEXT_CENTER);
		if ((x->_iMiscId == IMID_STAFF) && (x->_iMaxCharges != 0)) {
			sprintf(tempstr, "Charges: %i/%i", x->_iCharges, x->_iMaxCharges);
			AddPanelString(tempstr, TEXT_CENTER);
		}
		if (x->_iMagical) {
			AddPanelString("Not Identified", TEXT_CENTER);
		}
	}
	if (x->_iClass == IC_ARMOR) {
		if (x->_iMaxDur == INFINITE_DUR)
			sprintf(tempstr, "armor: %i  Indestructible", x->_iAC);
		else
			sprintf(tempstr, "armor: %i  Dur: %i/%i", x->_iAC, x->_iDurability, x->_iMaxDur);
		AddPanelString(tempstr, TEXT_CENTER);
		if (x->_iMagical) {
			AddPanelString("Not Identified", TEXT_CENTER);
		}
		if ((x->_iMiscId == IMID_STAFF) && (x->_iMaxCharges != 0)) {
			sprintf(tempstr, "Charges: %i/%i", x->_iCharges, x->_iMaxCharges);
			AddPanelString(tempstr, TEXT_CENTER);
		}
	}
	if ((x->_itype == IT_RING) || (x->_itype == IT_AMULET)) {
		AddPanelString("Not Identified", TEXT_CENTER);
	}
	PrintItemMisc(x);
	if ((x->_iMinStr + x->_iMinMag + x->_iMinDex) != 0) {
		strcpy(tempstr, "Required:");
		if (x->_iMinStr != 0) sprintf(tempstr, "%s %i Str", tempstr, x->_iMinStr);
		if (x->_iMinMag != 0) sprintf(tempstr, "%s %i Mag", tempstr, (byte) x->_iMinMag);
		if (x->_iMinDex != 0) sprintf(tempstr, "%s %i Dex", tempstr, x->_iMinDex);
		AddPanelString(tempstr, TEXT_CENTER);
	}
	pinfoflag = TRUE;
}

/*-------------------------------------------------------------------------*
**-------------------------------------------------------------------------*/

void UseItem(int p, int Mid, int spl)
{
	long l;
	__int64 t;

	switch(Mid) {
		case IMID_MEAT :
		case IMID_PLHEAL :
			l = plr[p]._pMaxHP >> (HP_SHIFT + 2);
			l = ((random(39, l) + (l >> 1)) << HP_SHIFT);
			if (plr[p]._pClass == CLASS_WARRIOR
				|| plr[p]._pClass == CLASS_BARBARIAN) l = l << 1;
			if (plr[p]._pClass == CLASS_ROGUE ||
				plr[p]._pClass == CLASS_MONK ||
				plr[p]._pClass == CLASS_BARD) l += (l >> 1);
			plr[p]._pHitPoints += l;
			if (plr[p]._pHitPoints > plr[p]._pMaxHP) plr[p]._pHitPoints = plr[p]._pMaxHP;
			plr[p]._pHPBase += l;
			if (plr[p]._pHPBase > plr[p]._pMaxHPBase) plr[p]._pHPBase = plr[p]._pMaxHPBase;
			drawhpflag = TRUE;
			break;
		case IMID_PHEAL :
			plr[p]._pHitPoints = plr[p]._pMaxHP;
			plr[p]._pHPBase = plr[p]._pMaxHPBase;
			drawhpflag = TRUE;
			break;
		case IMID_PMANA :
			l = plr[p]._pMaxMana >> (MANA_SHIFT + 2);
			l = ((random(40, l) + (l >> 1)) << MANA_SHIFT);
			if (plr[p]._pClass == CLASS_SORCEROR) l = l << 1;
			if (plr[p]._pClass == CLASS_ROGUE ||
				plr[p]._pClass == CLASS_MONK ||
				plr[p]._pClass == CLASS_BARD) l += (l >> 1);
			if (!(plr[p]._pIFlags & IAF_LMANA)) {
				plr[p]._pMana += l;
				if (plr[p]._pMana > plr[p]._pMaxMana) plr[p]._pMana = plr[p]._pMaxMana;
				plr[p]._pManaBase += l;
				if (plr[p]._pManaBase > plr[p]._pMaxManaBase) plr[p]._pManaBase = plr[p]._pMaxManaBase;
				drawmanaflag = TRUE;
			}
			break;
		case IMID_PFMANA :
			if (!(plr[p]._pIFlags & IAF_LMANA)) {
				plr[p]._pMana = plr[p]._pMaxMana;
				plr[p]._pManaBase = plr[p]._pMaxManaBase;
				drawmanaflag = TRUE;
			}
			break;
		case IMID_ESTR :
			ModifyPlrStr(p, 1);
			break;
		case IMID_EMAG :
			ModifyPlrMag(p, 1);
			
			// also give full mana potion effect
			plr[p]._pMana = plr[p]._pMaxMana;
			plr[p]._pManaBase = plr[p]._pMaxManaBase;
			drawmanaflag = TRUE;
			
			break;
		case IMID_EDEX :
			ModifyPlrDex(p, 1);
			break;
		case IMID_EVIT :
			ModifyPlrVit(p, 1);
			
			// heal the player, too
			plr[p]._pHitPoints = plr[p]._pMaxHP;
			plr[p]._pHPBase = plr[p]._pMaxHPBase;
			drawhpflag = TRUE;
			
			break;
		case IMID_BOOK :
			t = 1;
			plr[p]._pMemSpells |= (t << spl-1);
			if (plr[p]._pSplLvl[spl] < SPELLCAP) plr[p]._pSplLvl[spl]++;
			plr[p]._pMana += spelldata[spl].sManaCost << MANA_SHIFT;
			if (plr[p]._pMana > plr[p]._pMaxMana) plr[p]._pMana = plr[p]._pMaxMana;
			plr[p]._pManaBase += spelldata[spl].sManaCost << MANA_SHIFT;
			if (plr[p]._pManaBase > plr[p]._pMaxManaBase) plr[p]._pManaBase = plr[p]._pMaxManaBase;
			if (p == myplr) CalcPlrBookVals(p);
			drawmanaflag = TRUE;
			break;
		case IMID_REJUV :
			l = plr[p]._pMaxHP >> (HP_SHIFT + 2);
			l = ((random(39, l) + (l >> 1)) << HP_SHIFT);
			if (plr[p]._pClass == CLASS_WARRIOR
				|| plr[p]._pClass == CLASS_BARBARIAN) l = l << 1;
			if (plr[p]._pClass == CLASS_ROGUE) l += (l >> 1);
			plr[p]._pHitPoints += l;
			if (plr[p]._pHitPoints > plr[p]._pMaxHP) plr[p]._pHitPoints = plr[p]._pMaxHP;
			plr[p]._pHPBase += l;
			if (plr[p]._pHPBase > plr[p]._pMaxHPBase) plr[p]._pHPBase = plr[p]._pMaxHPBase;
			drawhpflag = TRUE;
			l = plr[p]._pMaxMana >> (MANA_SHIFT + 2);
			l = ((random(40, l) + (l >> 1)) << MANA_SHIFT);
			if (plr[p]._pClass == CLASS_SORCEROR) l = l << 1;
			if (plr[p]._pClass == CLASS_ROGUE) l += (l >> 1);
			if (!(plr[p]._pIFlags & IAF_LMANA)) {
				plr[p]._pMana += l;
				if (plr[p]._pMana > plr[p]._pMaxMana) plr[p]._pMana = plr[p]._pMaxMana;
				plr[p]._pManaBase += l;
				if (plr[p]._pManaBase > plr[p]._pMaxManaBase) plr[p]._pManaBase = plr[p]._pMaxManaBase;
				drawmanaflag = TRUE;
			}
			break;
		case IMID_FREJUV :
				plr[p]._pHitPoints = plr[p]._pMaxHP;
				plr[p]._pHPBase = plr[p]._pMaxHPBase;
				drawhpflag = TRUE;
			if (!(plr[p]._pIFlags & IAF_LMANA)) {
				plr[p]._pMana = plr[p]._pMaxMana;
				plr[p]._pManaBase = plr[p]._pMaxManaBase;
				drawmanaflag = TRUE;
			}
			break;
		case IMID_OILACC :
		case IMID_OILMAST :
		case IMID_OILSHRP :
		case IMID_OILDEATH :
		case IMID_OILSKILL :
		case IMID_OILBLKSM :
		case IMID_OILFORT :
		case IMID_OILPERM :
		case IMID_OILHARD :
		case IMID_OILIMPER :
			plr[p]._pOilType = Mid;
			if (p == myplr) {
				if (sbookflag) sbookflag = FALSE;
				if (!invflag) invflag = TRUE;
				NewCursor(OIL_CURS);
			}
			break;
		case IMID_SCROLL:
			if (spelldata[spl].sTargeted) {
				plr[p]._pTSpell = spl;
				//plr[p]._pTSplType = SPT_SCROLL;
				plr[p]._pTSplType = SPT_NONE;
				if (p == myplr) {
					NewCursor(TARGET_CURS);
				}
			} else {
				ClrPlrPath(p);
				plr[p]._pSpell = spl;
				//plr[p]._pSplType = SPT_SCROLL;
				plr[p]._pSplType = SPT_NONE;
				plr[p]._pSplFrom = SPL_FROMSB;
				plr[p].destAction = PCMD_SPELL;
				plr[p].destParam1 = cursmx;
				plr[p].destParam2 = cursmy;
			}
			break;
		case IMID_TSCROLL:
			if (spelldata[spl].sTargeted) {
				plr[p]._pTSpell = spl;
				//plr[p]._pTSplType = SPT_SCROLL;
				plr[p]._pTSplType = SPT_NONE;
				if (p == myplr) {
					NewCursor(TARGET_CURS);
				}
			} else {
				ClrPlrPath(p);
				plr[p]._pSpell = spl;
				//plr[p]._pSplType = SPT_SCROLL;
				plr[p]._pSplType = SPT_NONE;
				plr[p]._pSplFrom = SPL_FROMSB;
				plr[p].destAction = PCMD_SPELL;
				plr[p].destParam1 = cursmx;
				plr[p].destParam2 = cursmy;
			}
			break;
		case IMID_MAPOFDOOM:
			InitMapOfDoomView();
			break;
		case IMID_SPECTRAL:
			ModifyPlrStr(p, 3);
			ModifyPlrMag(p, 3);
			ModifyPlrDex(p, 3);
			ModifyPlrVit(p, 3);
			break;
		case IMID_RUNEFIRE:	
			plr[p]._pTSpell = SPL_RUNEOFFIRE;
			plr[p]._pTSplType = SPT_NONE;
			if (p == myplr) {
				NewCursor(TARGET_CURS);
			}
			break;
		case IMID_RUNELIGHT:
			plr[p]._pTSpell = SPL_RUNEOFLIGHT;
			plr[p]._pTSplType = SPT_NONE;
			if (p == myplr) {
				NewCursor(TARGET_CURS);
			}
			break;
		case IMID_RUNEIMMOLATE:
			plr[p]._pTSpell = SPL_RUNEOFIMMOLATION;
			plr[p]._pTSplType = SPT_NONE;
			if (p == myplr) {
				NewCursor(TARGET_CURS);
			}
			break;
		case IMID_RUNENOVA:
			plr[p]._pTSpell = SPL_RUNEOFNOVA;
			plr[p]._pTSplType = SPT_NONE;
			if (p == myplr) {
				NewCursor(TARGET_CURS);
			}
			break;
		case IMID_RUNESTONE:
			plr[p]._pTSpell = SPL_RUNEOFSTONE;
			plr[p]._pTSplType = SPT_NONE;
			if (p == myplr) {
				NewCursor(TARGET_CURS);
			}
			break;
		case IMID_FULLNOTE:
//			InitQTextMsg(TXT_SKULLJRNL7);
			break;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

#define MAX_STORE_VAL	200000
#define MAX_WIRT_VAL	200000		// Wirts stuff is 75% of normal

BOOL StoreStatOk(ItemStruct *h)
{
	BOOL sf = TRUE;
	if (plr[myplr]._pStrength < h->_iMinStr) sf = FALSE;
	else if (plr[myplr]._pMagic < (byte)h->_iMinMag) sf = FALSE;
	else if (plr[myplr]._pDexterity < h->_iMinDex) sf = FALSE;
	return(sf);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL SmithItemOk(int i)
{
	BOOL rv;

	rv = TRUE;
	// Selling oil is too powerful for the money received.
//	if (AllItemsList[i].iMiscId > IMID_FIRSTOIL &&
//		AllItemsList[i].iMiscId < IMID_LASTOIL) rv = TRUE;
//	else
		if (AllItemsList[i].itype == IT_MISC) rv = FALSE;
	
	if (AllItemsList[i].itype == IT_GOLD) rv = FALSE;
	if (AllItemsList[i].itype == IT_FOOD) rv = FALSE;
	if (AllItemsList[i].itype == IT_STAFF
		&& AllItemsList[i].iSpell != 0) rv = FALSE;
	if (AllItemsList[i].itype == IT_RING) rv = FALSE;
	if (AllItemsList[i].itype == IT_AMULET) rv = FALSE;
	return(rv);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int RndSmithItem(int lvl)
{
	int ril[512];		// Max 512 items
	int ri, i;

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != -1; i++) {
		if (AllItemsList[i].iRnd && SmithItemOk(i) &&
			(lvl >= AllItemsList[i].iMinMLvl)
			&& ri < 512) {
			ril[ri++] = i;
			if (AllItemsList[i].iRnd == IRND_DOUBLE 
				&& ri < 512) ril[ri++] = i;
		}
	}
	return(ril[random(50, ri)]+1);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void BubbleSwapItem(ItemStruct * a, ItemStruct * b)
{
	ItemStruct h;

	h = *a;
	*a = *b;
	*b = h;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SortSmith()
{
	int j, k;
	BOOL sorted;

	for (k = 0; smithitem[k+1]._itype != -1; k++);
	sorted = FALSE;
	while ((k > 0) && (!sorted)) {
		sorted = TRUE;
		for (j = 0; j < k; j++) {
			if (smithitem[j].IDidx > smithitem[j+1].IDidx) {
				BubbleSwapItem(&smithitem[j], &smithitem[j+1]);
				sorted = FALSE;
			}
		}
		k--;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SpawnSmith(int lvl)
{
	int	itype;
	int i,nsi;
	ItemStruct  const holditem = item[0];

	nsi = random(50, (MAXSMITHITEMS - 10)) + 10;
	for (i = 0; i < nsi; i++) {
		do {
			item[0]._iSeed = GetRndSeed();
			SetRndSeed(item[0]._iSeed);
			itype = RndSmithItem(lvl) - 1;
			GetItemAttrs(0, itype, lvl);
		} while (item[0]._iIvalue > MAX_STORE_VAL);
		smithitem[i] = item[0];
		smithitem[i]._iCreateInfo = lvl | ICI_SMITH;
		smithitem[i]._iIdentified = TRUE;
		smithitem[i]._iStatFlag = StoreStatOk(&smithitem[i]);
	}
	for (i = nsi; i < MAXSMITHITEMS; i++) smithitem[i]._itype = -1;
	SortSmith();
	item[0] = holditem;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL PremiumItemOk(int i)
{
	BOOL rv;

	rv = TRUE;
	if (AllItemsList[i].itype == IT_MISC) rv = FALSE;
	else if (AllItemsList[i].itype == IT_GOLD) rv = FALSE;
	else if (AllItemsList[i].itype == IT_FOOD) rv = FALSE;
	if (gbMaxPlayers != 1) {
		if (AllItemsList[i].iMiscId == IMID_OIL) rv = FALSE;
		else if (AllItemsList[i].itype == IT_RING) rv = FALSE;
		else if (AllItemsList[i].itype == IT_AMULET) rv = FALSE;
	}
	return(rv);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int RndPremiumItem(int minlvl, int maxlvl)
{
	int ril[512];		// Max 512 items
	int ri, i;

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != -1; i++) {
		if (AllItemsList[i].iRnd && PremiumItemOk(i) &&
			(AllItemsList[i].iMinMLvl >= minlvl) &&
			(AllItemsList[i].iMinMLvl <= maxlvl) &&
			ri < 512) {
			ril[ri++] = i;
		}
	}
	return(ril[random(50, ri)]+1);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void SpawnOnePremium(int i, int plvl, int myplr, bool noSpells)
{
	int	itype;
	ItemStruct  const holditem = item[0];
	int const maxval = MAX_STORE_VAL;
	int ivalue;
	int iCount = 0;
	int maxPlrStr = GetMaxStr(plr[myplr]._pClass);
	int maxPlrDex = GetMaxDex(plr[myplr]._pClass);
	int maxPlrMag = GetMaxMag(plr[myplr]._pClass);
	
	// Test in case of bonus
	if (maxPlrStr < plr[myplr]._pStrength)
	{
		maxPlrStr = plr[myplr]._pStrength;
	}
	maxPlrStr = (int)(maxPlrStr * 1.2);
	
	if (maxPlrDex < plr[myplr]._pDexterity)
	{
		maxPlrDex = plr[myplr]._pDexterity;
	}
	maxPlrDex = (int)(maxPlrDex * 1.2);
	
	if (maxPlrMag < plr[myplr]._pMagic)
	{
		maxPlrMag = plr[myplr]._pMagic;
	}
	maxPlrMag = (int)(maxPlrMag * 1.2);
	
	
#if CHEATS
	if ((plvl > 30) || (davecheat)) plvl = 30;
#else
	if (plvl > 30) plvl = 30;
#endif
	if (plvl < 1) plvl = 1;

	do {
		item[0]._iSeed = GetRndSeed();
		SetRndSeed(item[0]._iSeed);
		itype = RndPremiumItem(plvl>>2, plvl) - 1;
		GetItemAttrs(0, itype, plvl);
		GetItemBonus(0, itype, plvl>>1, plvl, TRUE, noSpells);
		
		ivalue = 0;
		
		switch (item[0]._itype)
		{
		case IT_ARMOR:
		case IT_MARMOR:
		case IT_HARMOR:
			ivalue = GetHighArmorValue(myplr);
			break;
		case IT_SHIELD:
			ivalue = GetHighShieldValue(myplr);
			break;
		case IT_AXE:
			ivalue = GetHighAxeValue(myplr);
			break;
		case IT_BOW:
			ivalue = GetHighBowValue(myplr);
			break;
		case IT_MACE:
			ivalue = GetHighMaceValue(myplr);
			break;
		case IT_SWORD:
			ivalue = GetHighSwordValue(myplr);
			break;
		case IT_HELM:
			ivalue = GetHighHelmValue(myplr);
			break;
		case IT_STAFF:
			ivalue = GetHighStaffValue(myplr);
			break;
		case IT_RING:
			ivalue = GetHighRingValue(myplr);
			break;
		case IT_AMULET:
			ivalue = GetHighAmuletValue(myplr);
			break;
		}
		ivalue = (int)(0.8 * ivalue);
		
		++iCount;
	
	// GWP Make sure we can actually use it.
	} while ((item[0]._iIvalue > maxval
	         || item[0]._iMinStr > maxPlrStr
	         || item[0]._iMinMag > maxPlrMag
	         || item[0]._iMinDex > maxPlrDex
	         || item[0]._iIvalue < ivalue)		// already have better.
	         && iCount < 150					// prevent infinite loops.
	         );
	
	premiumitem[i] = item[0];
	premiumitem[i]._iCreateInfo = plvl | ICI_PREMIUM;
	premiumitem[i]._iIdentified = TRUE;
	premiumitem[i]._iStatFlag = StoreStatOk(&premiumitem[i]);
	item[0] = holditem;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int premiumlvladd[MAXPREMIUM] = { -1, -1, -1, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3 };

void SpawnPremium(int myplr)
{
	int lvl = plr[myplr]._pLevel;
	int i;

	// Empty slots?
	if (numpremium < MAXPREMIUM) {
		for (i = 0; i < MAXPREMIUM; ++i) {
			if (premiumitem[i]._itype == -1)
				SpawnOnePremium(i, premiumlevel + premiumlvladd[i], myplr, false);
		}
		numpremium = MAXPREMIUM;
	}
	// New items?
	while (premiumlevel < lvl) {
		premiumlevel++;
		premiumitem[0] = premiumitem[3];
		premiumitem[1] = premiumitem[4];
		premiumitem[2] = premiumitem[5];
		premiumitem[3] = premiumitem[6];
		premiumitem[4] = premiumitem[7];
		premiumitem[5] = premiumitem[8];
		premiumitem[6] = premiumitem[9];
		premiumitem[7] = premiumitem[10];
		premiumitem[8] = premiumitem[11];
		premiumitem[9] = premiumitem[12];
		SpawnOnePremium(10, premiumlevel + premiumlvladd[10], myplr, false);
		premiumitem[11] = premiumitem[13];
		SpawnOnePremium(12, premiumlevel + premiumlvladd[12], myplr, false);
		premiumitem[13] = premiumitem[14];
		SpawnOnePremium(14, premiumlevel + premiumlvladd[14], myplr, false);
	}

#if 15 != MAXPREMIUM
#error "Fix code for new MAXPREMIUM"
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL WitchItemOk(int i)
{
	BOOL rv;

	rv = FALSE;
	if (AllItemsList[i].itype == IT_MISC) rv = TRUE;
	else if (AllItemsList[i].itype == IT_STAFF) rv = TRUE;
	
	// Will already have the next one
	if (AllItemsList[i].iMiscId == IMID_PMANA) rv = FALSE;
	else if (AllItemsList[i].iMiscId == IMID_PFMANA) rv = FALSE;
	
	if (AllItemsList[i].iSpell == SPL_TOWN) rv = FALSE;
	// no healing
	if (AllItemsList[i].iMiscId == IMID_PHEAL) rv = FALSE;
	else if (AllItemsList[i].iMiscId == IMID_PLHEAL) rv = FALSE;
	
	// no oils
	if (AllItemsList[i].iMiscId > IMID_FIRSTOIL &&
		AllItemsList[i].iMiscId < IMID_LASTOIL)
		rv = FALSE;
	// None of these in single player
	if (AllItemsList[i].iSpell == SPL_RESURRECT && gbMaxPlayers == 1) rv = FALSE;
	else if (AllItemsList[i].iSpell == SPL_HEALOTHER && gbMaxPlayers == 1) rv = FALSE;
	
	return(rv);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int RndWitchItem(int lvl)
{
	int ril[512];		// Max 512 items
	int ri, i;

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != -1; ++i) {
		if (AllItemsList[i].iRnd && WitchItemOk(i) &&
			(lvl >= AllItemsList[i].iMinMLvl) &&
			ri < 512) 
		{
			ril[ri++] = i;
		}
	}
	return(ril[random(51, ri)]+1);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SortWitch()
{
	int j, k;
	BOOL sorted;

	for (k = 3; witchitem[k+1]._itype != -1; k++);
	sorted = FALSE;
	while ((k > 3) && (!sorted)) {
		sorted = TRUE;
		for (j = 3; j < k; j++) {
			if (witchitem[j].IDidx > witchitem[j+1].IDidx) {
				BubbleSwapItem(&witchitem[j], &witchitem[j+1]);
				sorted = FALSE;
			}
		}
		k--;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void WitchBookLevel(int ii)
{
	if (witchitem[ii]._iMiscId != IMID_BOOK) 
		return;
		
	witchitem[ii]._iMinMag = spelldata[witchitem[ii]._iSpell].sMinInt;
	int slvl = plr[myplr]._pSplLvl[witchitem[ii]._iSpell];
	
	while (slvl != 0) {
		witchitem[ii]._iMinMag += ((witchitem[ii]._iMinMag * 20) / 100);
		slvl--;
		if ((witchitem[ii]._iMinMag + ((witchitem[ii]._iMinMag * 20) / 100)) > 255) {
			witchitem[ii]._iMinMag = 255;
			slvl = 0;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SpawnWitch(int lvl)
{
	int	itype, iblvl;
	int i;
	int start = 3;

	int const nsi = random(51, (MAXWITCHITEMS - 10)) + 10;
	int const max_number_of_books = random(3, 1 + (IDI_LAST_BOOK - IDI_FIRST_BOOK));
	int number_of_books;
	
	// Will always have mana (endless supply)
	GetItemAttrs(0, IDI_MANA, 1);
	witchitem[0] = item[0];
	witchitem[0]._iCreateInfo = lvl;
	witchitem[0]._iStatFlag = TRUE;
	GetItemAttrs(0, IDI_FULLMANA, 1);
	witchitem[1] = item[0];
	witchitem[1]._iCreateInfo = lvl;
	witchitem[1]._iStatFlag = TRUE;
	GetItemAttrs(0, IDI_PORTAL, 1);
	witchitem[2] = item[0];
	witchitem[2]._iCreateInfo = lvl;
	witchitem[2]._iStatFlag = TRUE;
	for (i = IDI_FIRST_BOOK, number_of_books = 0; 
		i <= IDI_LAST_BOOK
		&& number_of_books < max_number_of_books; 
		++i)
	{
		
		if ( WitchItemOk(i)
			&& lvl >= AllItemsList[i].iMinMLvl
			)
		{
			item[0]._iSeed = GetRndSeed();
			SetRndSeed(item[0]._iSeed);
			volatile int dummy = random(0, 1);	// to sync with below
			
			GetItemAttrs(0, i, lvl);
			witchitem[start] = item[0];
			witchitem[start]._iCreateInfo = lvl | ICI_WITCH;
			witchitem[start]._iIdentified = TRUE;
			WitchBookLevel(start);
			witchitem[start]._iStatFlag = StoreStatOk(&witchitem[start]);
			++start;
			++number_of_books;
		}
	}

	for (i = start; i < nsi; ++i) {
		do {
			item[0]._iSeed = GetRndSeed();
			SetRndSeed(item[0]._iSeed);
			itype = RndWitchItem(lvl) - 1;
			GetItemAttrs(0, itype, lvl);
			iblvl = -1;
			if (random(51, 100) <= 5) iblvl = lvl << 1;
			// Force staffs to be magical
			if ((iblvl == -1) && (item[0]._iMiscId == IMID_STAFF)) iblvl = lvl << 1;
#if CHEATS
			if (cheatflag) iblvl = lvl << 1;
#endif
			if (iblvl != -1) GetItemBonus(0, itype, iblvl >> 1, iblvl, TRUE, true);
		} while (item[0]._iIvalue > MAX_STORE_VAL);
		witchitem[i] = item[0];
		witchitem[i]._iCreateInfo = lvl | ICI_WITCH;
		witchitem[i]._iIdentified = TRUE;
		WitchBookLevel(i);
		witchitem[i]._iStatFlag = StoreStatOk(&witchitem[i]);
		//WitchBookLevel(i);
	}
	for (i = nsi; i < MAXWITCHITEMS; i++) witchitem[i]._itype = -1;
	SortWitch();
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int RndBoyItem(int lvl)
{
	int ril[512];		// Max 512 items
	int ri, i;

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != -1; ++i) {
		if (AllItemsList[i].iRnd && PremiumItemOk(i) &&
			(lvl >= AllItemsList[i].iMinMLvl) &&
			ri < 512) ril[ri++] = i;
	}
	return(ril[random(49, ri)]+1);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SpawnBoy(int lvl)
{
	int	itype;
	int ivalue;
	int iCount = 0;
	int maxPlrStr = GetMaxStr(plr[myplr]._pClass);
	int maxPlrDex = GetMaxDex(plr[myplr]._pClass);
	int maxPlrMag = GetMaxMag(plr[myplr]._pClass);
	int const PlayerClass = plr[myplr]._pClass;
	
	// Test in case of bonus
	if (maxPlrStr < plr[myplr]._pStrength)
	{
		maxPlrStr = plr[myplr]._pStrength;
	}
	maxPlrStr = (int)(maxPlrStr * 1.2);
	
	if (maxPlrDex < plr[myplr]._pDexterity)
	{
		maxPlrDex = plr[myplr]._pDexterity;
	}
	maxPlrDex = (int)(maxPlrDex * 1.2);
	
	if (maxPlrMag < plr[myplr]._pMagic)
	{
		maxPlrMag = plr[myplr]._pMagic;
	}
	maxPlrMag = (int)(maxPlrMag * 1.2);
	
	

	if ((boylevel < (lvl >> 1)) || (boyitem._itype == -1)) {
		do {
			item[0]._iSeed = GetRndSeed();
			SetRndSeed(item[0]._iSeed);
			itype = RndBoyItem(lvl) - 1;
			GetItemAttrs(0, itype, lvl);
			// Always magical
			GetItemBonus(0, itype, lvl, lvl << 1, TRUE, true);
			ivalue = 0;
			
			int const itemType = item[0]._itype;
			
			switch (itemType)
			{
			case IT_ARMOR:
			case IT_MARMOR:
			case IT_HARMOR:
				ivalue = GetHighArmorValue(myplr);
				break;
			case IT_SHIELD:
				ivalue = GetHighShieldValue(myplr);
				break;
			case IT_AXE:
				ivalue = GetHighAxeValue(myplr);
				break;
			case IT_BOW:
				ivalue = GetHighBowValue(myplr);
				break;
			case IT_MACE:
				ivalue = GetHighMaceValue(myplr);
				break;
			case IT_SWORD:
				ivalue = GetHighSwordValue(myplr);
				break;
			case IT_HELM:
				ivalue = GetHighHelmValue(myplr);
				break;
			case IT_STAFF:
				ivalue = GetHighStaffValue(myplr);
				break;
			case IT_RING:
				ivalue = GetHighRingValue(myplr);
				break;
			case IT_AMULET:
				ivalue = GetHighAmuletValue(myplr);
				break;
			}
			ivalue = (int)(0.8 * ivalue);
			
			++iCount;
			
			if (iCount < 200) // prevent infinite loops.
			{
				switch (PlayerClass)
				{
				case CLASS_WARRIOR:
					if (itemType == IT_BOW
						|| itemType == IT_STAFF)
						ivalue = INT_MAX;
					break;
				case CLASS_ROGUE:
					if (itemType == IT_SWORD
						|| itemType == IT_STAFF
						|| itemType == IT_AXE
						|| itemType == IT_MACE
						|| itemType == IT_SHIELD)
						ivalue = INT_MAX;
					break;
				case CLASS_SORCEROR:
					if (itemType == IT_STAFF
						|| itemType == IT_AXE
						|| itemType == IT_BOW
						|| itemType == IT_MACE)
						ivalue = INT_MAX;
					break;
				case CLASS_MONK:
					if (itemType == IT_BOW 
						|| itemType == IT_MARMOR
						|| itemType == IT_SHIELD
						|| itemType == IT_MACE)
						ivalue = INT_MAX;
					break;
				case CLASS_BARD:
					if (itemType == IT_AXE
						|| itemType == IT_MACE	
						|| itemType == IT_STAFF)
						ivalue = INT_MAX;
					break;
				case CLASS_BARBARIAN:
					if (itemType == IT_BOW
						|| itemType == IT_STAFF)
						ivalue = INT_MAX;
					break;
				}
			}
			
	
		} while ((item[0]._iIvalue > MAX_WIRT_VAL
	         || item[0]._iMinStr > maxPlrStr
	         || item[0]._iMinMag > maxPlrMag
	         || item[0]._iMinDex > maxPlrDex
	         || item[0]._iIvalue < ivalue)		// already have better.
	         && iCount < 250					// prevent infinite loops.
	         );
		boyitem = item[0];
		boyitem._iCreateInfo = lvl | ICI_BOY;
		boyitem._iIdentified = TRUE;
		boyitem._iStatFlag = StoreStatOk(&boyitem);
		boylevel = lvl >> 1;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL HealerItemOk(int i)
{
	BOOL rv;

	rv = FALSE;
	if (AllItemsList[i].itype != IT_MISC) return(FALSE);
	// Heal scrolls
	if ((AllItemsList[i].iMiscId == IMID_SCROLL) && (AllItemsList[i].iSpell == SPL_HEAL)) rv = TRUE;
	// Will always have Resurrect scrolls (multiplayer only)
	if ((AllItemsList[i].iMiscId == IMID_TSCROLL) && (AllItemsList[i].iSpell == SPL_RESURRECT) && (gbMaxPlayers != 1)) rv = FALSE;
	// Heal Other scroll (multiplayer only)
	if ((AllItemsList[i].iMiscId == IMID_TSCROLL) && (AllItemsList[i].iSpell == SPL_HEALOTHER) && (gbMaxPlayers != 1)) rv = TRUE;
	// Elixirs
	if (gbMaxPlayers == 1) {
		if (AllItemsList[i].iMiscId == IMID_ESTR
			&& plr[myplr]._pBaseStr < MaxStats[plr[myplr]._pClass][0]) rv = TRUE;
		else if (AllItemsList[i].iMiscId == IMID_EMAG
			&& plr[myplr]._pBaseMag < MaxStats[plr[myplr]._pClass][1]) rv = TRUE;
		else if (AllItemsList[i].iMiscId == IMID_EDEX
			&& plr[myplr]._pBaseDex < MaxStats[plr[myplr]._pClass][2]) rv = TRUE;
		else if (AllItemsList[i].iMiscId == IMID_EVIT
			&& plr[myplr]._pBaseVit < MaxStats[plr[myplr]._pClass][3]) rv = TRUE;
	}
	// Potions
	if (AllItemsList[i].iMiscId == IMID_PHEAL) rv = TRUE;
	else if (AllItemsList[i].iMiscId == IMID_REJUV) rv = TRUE;
	else if (AllItemsList[i].iMiscId == IMID_FREJUV) rv = TRUE;
	// Will always have these
	else if (AllItemsList[i].iMiscId == IMID_PLHEAL) rv = FALSE;
	else if (AllItemsList[i].iMiscId == IMID_PHEAL) rv = FALSE;
	// No mana
	else if (AllItemsList[i].iMiscId == IMID_PMANA) rv = FALSE;
	else if (AllItemsList[i].iMiscId == IMID_PFMANA) rv = FALSE;
	return(rv);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int RndHealerItem(int lvl)
{
	int ril[512];		// Max 512 items
	int ri, i;

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != -1; i++) {
		if (AllItemsList[i].iRnd && HealerItemOk(i) &&
			(lvl >= AllItemsList[i].iMinMLvl) &&
			ri < 512) ril[ri++] = i;
	}
	return(ril[random(50, ri)]+1);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SortHealer()
{
	int j, k;
	BOOL sorted;

	for (k = 2; healitem[k+1]._itype != -1; k++);
	sorted = FALSE;
	while ((k > 2) && (!sorted)) {
		sorted = TRUE;
		for (j = 2; j < k; j++) {
			if (healitem[j].IDidx > healitem[j+1].IDidx) {
				BubbleSwapItem(&healitem[j], &healitem[j+1]);
				sorted = FALSE;
			}
		}
		k--;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SpawnHealer(int lvl)
{
	int	itype;
	int i,nsi,srnd;

	// Will always have healing (endless supply)
	GetItemAttrs(0, IDI_HEAL, 1);
	healitem[0] = item[0];
	healitem[0]._iCreateInfo = lvl;
	healitem[0]._iStatFlag = TRUE;
	GetItemAttrs(0, IDI_FULLHEAL, 1);
	healitem[1] = item[0];
	healitem[1]._iCreateInfo = lvl;
	healitem[1]._iStatFlag = TRUE;
	if (gbMaxPlayers != 1) {
		GetItemAttrs(0, IDI_RESURRECT, 1);
		healitem[2] = item[0];
		healitem[2]._iCreateInfo = lvl;
		healitem[2]._iStatFlag = TRUE;
		srnd = 3;
	} else srnd = 2;

	nsi = random(50, (MAXHEALITEMS - 10)) + 10;
	for (i = srnd; i < nsi; i++) {
		item[0]._iSeed = GetRndSeed();
		SetRndSeed(item[0]._iSeed);
		itype = RndHealerItem(lvl) - 1;
		GetItemAttrs(0, itype, lvl);
		healitem[i] = item[0];
		healitem[i]._iCreateInfo = lvl | ICI_HEALER;
		healitem[i]._iIdentified = TRUE;
		healitem[i]._iStatFlag = StoreStatOk(&healitem[i]);
	}
	for (i = nsi; i < MAXHEALITEMS; i++) healitem[i]._itype = -1;
	SortHealer();
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SpawnStoreGold()
{
	// Setup a gold item to place in inv if they sell
	GetItemAttrs(0, 0, 1);
	golditem = item[0];
	golditem._iStatFlag = TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void RecreateSmithItem(int ii, int idx, int lvl, int iseed)
{
	SetRndSeed(iseed);
	int itype = RndSmithItem(lvl) - 1;
	GetItemAttrs(ii, itype, lvl);
	item[ii]._iSeed = iseed;
	item[ii]._iCreateInfo = lvl | ICI_SMITH;
	item[ii]._iIdentified = TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void RecreatePremiumItem(int ii, int idx, int plvl, int iseed)
{
	SetRndSeed(iseed);
	int itype = RndPremiumItem(plvl>>2, plvl) - 1;
	GetItemAttrs(ii, itype, plvl);
	GetItemBonus(ii, itype, plvl>>1, plvl, TRUE, false);
	item[ii]._iSeed = iseed;
	item[ii]._iCreateInfo = plvl | ICI_PREMIUM;
	item[ii]._iIdentified = TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void RecreateBoyItem(int ii, int idx, int lvl, int iseed)
{
	SetRndSeed(iseed);
	int itype = RndBoyItem(lvl) - 1;
	GetItemAttrs(ii, itype, lvl);
	// Always magical
	GetItemBonus(ii, itype, lvl, lvl << 1, TRUE, true);
	item[ii]._iSeed = iseed;
	item[ii]._iCreateInfo = lvl | ICI_BOY;
	item[ii]._iIdentified = TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void RecreateWitchItem(int ii, int idx, int lvl, int iseed)
{
	if ((idx == IDI_MANA) 
		|| (idx == IDI_FULLMANA) 
		|| (idx == IDI_PORTAL)) 
	{
		GetItemAttrs(ii, idx, lvl);
	} else if( idx >= IDI_FIRST_BOOK && idx <= IDI_LAST_BOOK) {
		SetRndSeed(iseed);
		volatile int dummy = random(0, 1);	// to sync with below
		GetItemAttrs(ii, idx, lvl);
	} else {
		SetRndSeed(iseed);
		int itype = RndWitchItem(lvl) - 1;
		GetItemAttrs(ii, itype, lvl);
		int iblvl = -1;
		if (random(51, 100) <= 5) iblvl = lvl << 1;
		// Force staffs to be magical
		if ((iblvl == -1) && (item[ii]._iMiscId == IMID_STAFF)) iblvl = lvl << 1;
#if CHEATS
		if (cheatflag) iblvl = lvl << 1;
#endif
		if (iblvl != -1) GetItemBonus(ii, itype, iblvl >> 1, iblvl, TRUE, true);
	}
	item[ii]._iSeed = iseed;
	item[ii]._iCreateInfo = lvl | ICI_WITCH;
	item[ii]._iIdentified = TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void RecreateHealerItem(int ii, int idx, int lvl, int iseed)
{
	if ((idx == IDI_HEAL) || (idx == IDI_FULLHEAL) || (idx == IDI_RESURRECT)) {
		GetItemAttrs(ii, idx, lvl);
	} else {
		SetRndSeed(iseed);
		int itype = RndHealerItem(lvl) - 1;
		GetItemAttrs(ii, itype, lvl);
	}
	item[ii]._iSeed = iseed;
	item[ii]._iCreateInfo = lvl | ICI_HEALER;
	item[ii]._iIdentified = TRUE;
}

/*-----------------------------------------------------------------------*
** Recreate any item with the proper info
**-----------------------------------------------------------------------*/

void RecreateTownItem(int ii, int idx, WORD icreateinfo, int iseed, int ivalue)
{
	if (icreateinfo & ICI_SMITH) RecreateSmithItem(ii, idx, icreateinfo & ICI_LVLMASK, iseed);
	else if (icreateinfo & ICI_PREMIUM) RecreatePremiumItem(ii, idx, icreateinfo & ICI_LVLMASK, iseed);
	else if (icreateinfo & ICI_BOY) RecreateBoyItem(ii, idx, icreateinfo & ICI_LVLMASK, iseed);
	else if (icreateinfo & ICI_WITCH) RecreateWitchItem(ii, idx, icreateinfo & ICI_LVLMASK, iseed);
	else if (icreateinfo & ICI_HEALER) RecreateHealerItem(ii, idx, icreateinfo & ICI_LVLMASK, iseed);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

#if CHEATS
void DaveGold()
{
	int i,j;

	for (j = 0; j < MAXINV; j++) {
		if (plr[myplr].InvGrid[j] == 0) {
			i = plr[myplr]._pNumInv;
			SetPlrHandItem(&plr[myplr].InvList[i], IDI_GOLD);
			GetGoldSeed(myplr, &plr[myplr].InvList[i]);
			plr[myplr].InvList[i]._ivalue = 5000;
			plr[myplr].InvList[i]._iCurs = ITEM_5GOLD;
			plr[myplr].InvList[i]._iStatFlag = TRUE;
			plr[myplr]._pGold += 5000;
			plr[myplr]._pNumInv++;
			plr[myplr].InvGrid[j] = plr[myplr]._pNumInv;
		}
	}
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

#if CHEATS
void DaveNewPremium()
{
	numpremium = 0;
	for (int i = 0; i < MAXPREMIUM; i++) premiumitem[i]._itype = -1;
	SpawnPremium(30);
	for (i = 0; i < MAXWITCHITEMS; i++) witchitem[i]._itype = -1;
	SpawnWitch(30);
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

#if CHEATS
void DaveCleanUp()
{
	int i,j;

	for (j = 0; j < MAXINV; j++) {
		if (plr[myplr].InvGrid[j] > 0) {
			i = plr[myplr].InvGrid[j]-1;
			if (plr[myplr].InvList[i]._itype == IT_GOLD) RemoveInvItem(myplr, i);
		}
	}
	for (j = 0; j < MAXSPD; j++) {
		if (plr[myplr].SpdList[j]._itype == IT_GOLD) plr[myplr].SpdList[j]._itype = -1;
	}
	plr[myplr]._pGold = 0;
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

#if CHEATS
void DaveSpells()
{
	for (int i = SPL_FIREBOLT; i < SPL_LAST; i++) {
		if (spelldata[i].sBookLvl != -1) {
			__int64 t = 1;
			plr[myplr]._pMemSpells |= (t << (i-1));
			plr[myplr]._pSplLvl[i] = 10;
		}
	}
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

#if CHEATS
void DaveSetSpell(int spl, int lvl)
{
	__int64 t = 1;
	plr[myplr]._pMemSpells |= (t << (spl-1));
	plr[myplr]._pSplLvl[spl] = lvl;
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

#if CHEATS
void DaveSpells2()
{
	DaveSetSpell(SPL_FIREBOLT, 8);
	DaveSetSpell(SPL_CBOLT, 11);
	DaveSetSpell(SPL_HBOLT, 10);
	DaveSetSpell(SPL_HEAL, 7);
	DaveSetSpell(SPL_HEALOTHER, 5);
	DaveSetSpell(SPL_LIGHTNING, 9);
	DaveSetSpell(SPL_WALL, 5);
	DaveSetSpell(SPL_TELEKINESIS, 3);
	DaveSetSpell(SPL_TOWN, 3);
	DaveSetSpell(SPL_FLASH, 3);
	DaveSetSpell(SPL_PHASE, 2);
	DaveSetSpell(SPL_MANASHLD, 2);
	DaveSetSpell(SPL_WAVE, 4);
	DaveSetSpell(SPL_FIREBALL, 3);
	DaveSetSpell(SPL_STONE, 1);
	DaveSetSpell(SPL_CHAIN, 1);
	DaveSetSpell(SPL_GUARDIAN, 4);
	DaveSetSpell(SPL_ELEMENT, 3);
	DaveSetSpell(SPL_NOVA, 1);
	DaveSetSpell(SPL_GOLEM, 2);
	DaveSetSpell(SPL_BSTAR, 1);
	DaveSetSpell(SPL_BONESPIRIT, 1);
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void RecalcStoreStats()
{
	int i;

	for (i = 0; i < MAXSMITHITEMS; i++)
		if (smithitem[i]._itype != -1) smithitem[i]._iStatFlag = StoreStatOk(&smithitem[i]);
	for (i = 0; i < MAXPREMIUM; i++)
		if (premiumitem[i]._itype != -1) premiumitem[i]._iStatFlag = StoreStatOk(&premiumitem[i]);
	for (i = 0; i < MAXWITCHITEMS; i++)
 		if (witchitem[i]._itype != -1) witchitem[i]._iStatFlag = StoreStatOk(&witchitem[i]);
	for (i = 0; i < MAXHEALITEMS; i++)
		if (healitem[i]._itype != -1) healitem[i]._iStatFlag = StoreStatOk(&healitem[i]);
	boyitem._iStatFlag = StoreStatOk(&boyitem);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int ItemNoFlippy()
/*-----------------------------------------------------------------------**
** DESCRIPTION: Makes it so a newly created item will not have a flippy.
** INPUT: None
** RETURN: r = The item number if needed.
/*-----------------------------------------------------------------------*/
{
	int	r;

	r = itemactive[numitems-1];
	item[r]._iAnimFrame = item[r]._iAnimLen;
	item[r]._iAnimFlag = FALSE;
	item[r]._iSelFlag = ISEL_FLR;

	return r;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if CHEATS
void DaveQuestText()
{
	if (!tstQMsgIndexFlag) {
		tstQMsgFlag = TRUE;
		InitQTextMsg(tstQMsgIndex);
	} 
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void CreateSpellBook(int x, int y, int ispell, BOOL sendmsg, BOOL delta)
{
	int		ii, idx;
	BOOL	done = FALSE;

	int bookminlevel = spelldata[ispell].sBookLvl + 1;
	
	if (bookminlevel < 1)	// unavailable
		return;
	
	idx = RndTypeItems(IT_MISC, IMID_BOOK, bookminlevel);
	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(x,y,ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
		//Loop until we get the book requested
		do {
			SetupAllItems(ii, idx, GetRndSeed(), /* currlevel */ bookminlevel << 1, 1, TRUE, FALSE, delta);
			if ((item[ii]._iMiscId == IMID_BOOK) && 
				(item[ii]._iSpell == ispell)) {
				done = TRUE;
			} 
		} while (!done);
		if (sendmsg) NetSendCmdDItem(FALSE, ii);
		if (delta) DeltaAddItem(ii);
		numitems++;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void CreateMagicArmor(int x, int y, int imisc, int icurs, BOOL sendmsg, BOOL delta)
{
	int		ii, idx;
	BOOL	done = FALSE;
	
	int efflevel = GetEffLevel();
		
	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(x,y,ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
		//Loop until we get the armor requested
		idx = RndTypeItems(imisc, IMID_NONE, efflevel);
		do {
#if CHEATS
			if (itemcheat)
				SetupAllItems(ii, idx, GetRndSeed(), efflevel << 1, 15, TRUE, FALSE, delta);
			else	
#endif
			SetupAllItems(ii, idx, GetRndSeed(), efflevel << 1, 1, TRUE, FALSE, delta);
			if (item[ii]._iCurs == icurs) {
				done = TRUE;
			} else idx = RndTypeItems(imisc, IMID_NONE, efflevel);
		} while (!done);
		if (sendmsg) NetSendCmdDItem(FALSE, ii);
		if (delta) DeltaAddItem(ii);
		numitems++;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void CreateAmulet(int x, int y, int level, BOOL sendmsg, BOOL delta)
{
	int		ii, idx;
	bool	done = false;
	
	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(x,y,ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
		//Loop until we get the amulet requested
		idx = RndTypeItems(IT_AMULET, IMID_AMULET, level);
		do {
#if CHEATS
			if (itemcheat)
				SetupAllItems(ii, idx, GetRndSeed(), level << 1, 15, TRUE, FALSE, delta);
			else	
#endif
			SetupAllItems(ii, idx, GetRndSeed(), level << 1, 1, TRUE, FALSE, delta);
			if (item[ii]._iCurs == ITEM_AMULET1) {
				done = true;
			} else idx = RndTypeItems(IT_AMULET, IMID_AMULET, level);
		} while (!done);
		if (sendmsg) NetSendCmdDItem(FALSE, ii);
		if (delta) DeltaAddItem(ii);
		numitems++;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void CreateMagicWeapon(int x, int y, int imisc, int icurs, BOOL sendmsg, BOOL delta)
{
	int		ii, idx;
	BOOL	done = FALSE;
	int const imid = (imisc == IT_STAFF) ? IMID_STAFF : IMID_NONE;
	
	int efflevel = GetEffLevel();
		
	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(x,y,ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
		//Loop until we get the weapon requested
		idx = RndTypeItems(imisc, imid, efflevel);
		do {
#if CHEATS
			if (itemcheat)
				SetupAllItems(ii, idx, GetRndSeed(), efflevel << 1, 15, TRUE, FALSE, delta);
			else	
#endif
			SetupAllItems(ii, idx, GetRndSeed(), efflevel << 1, 1, TRUE, FALSE, delta);
			if (item[ii]._iCurs == icurs) {
				done = TRUE;
			} else idx = RndTypeItems(imisc, imid, efflevel);
		} while (!done);
		if (sendmsg) NetSendCmdDItem(FALSE, ii);
		if (delta) DeltaAddItem(ii);
		numitems++;
	}
}


// PATCH1.JMM
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void DeleteGetRecord( int nIndex ) {
	app_assert(nIndex < gnNumGetRecords);
	app_assert(gnNumGetRecords > 0);

	gnNumGetRecords--;

	if(gnNumGetRecords == 0)
		return;

	itemgets[nIndex].nIndex = itemgets[gnNumGetRecords].nIndex;
	itemgets[nIndex].nSeed = itemgets[gnNumGetRecords].nSeed;
	itemgets[nIndex].wCI = itemgets[gnNumGetRecords].wCI;
	itemgets[nIndex].dwTimestamp = itemgets[gnNumGetRecords].dwTimestamp;
}



/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#define RESEND_TIME 6000		// number of milliseconds to keep a getrecord around

BOOL CheckGetRecord( int nSeed, WORD wCI, int nIndex ) {
	DWORD dwCurr = GetTickCount();
	int i;

	for(i = 0; i < gnNumGetRecords; i++) {
		if( (dwCurr - itemgets[i].dwTimestamp) > RESEND_TIME ) {
			DeleteGetRecord( i );
			i--;
			continue;
		}

		if( (nSeed == itemgets[i].nSeed) && (wCI == itemgets[i].wCI) && (nIndex == itemgets[i].nIndex) )
			return FALSE;

	}

	return TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddGetRecord( int nSeed, WORD wCI, int nIndex ) {
	DWORD dwCurr = GetTickCount();

	if(gnNumGetRecords == MAXITEMS)
		return;

	itemgets[gnNumGetRecords].dwTimestamp = dwCurr;
	itemgets[gnNumGetRecords].nSeed = nSeed;
	itemgets[gnNumGetRecords].wCI = wCI;
	itemgets[gnNumGetRecords].nIndex = nIndex;

	gnNumGetRecords++;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void RemoveGetRecord( int nSeed, WORD wCI, int nIndex ) {
	DWORD dwCurr = GetTickCount();
	int i;

	for(i = 0; i < gnNumGetRecords; i++) {
		if( (dwCurr - itemgets[i].dwTimestamp) > RESEND_TIME ) {
			DeleteGetRecord( i );
			i--;
			continue;
		}

		if( (nSeed == itemgets[i].nSeed) && (wCI == itemgets[i].wCI) && (nIndex == itemgets[i].nIndex) ) {
			DeleteGetRecord( i );
			return;
		}

	}

}


//ENDPATCH1.JMM

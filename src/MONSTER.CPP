/*-----------------------------------------------------------------------**
** 								Diablo
**
**							 Monsters file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/MONSTER.CPP 3     1/23/97 7:22p Rseis $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"storm.h"
#include	"sound.h"
#include	"debug.h"
#include	"monster.h"
#include	"monstdat.h"
#include	"engine.h"
#include	"gendung.h"
#include	"lighting.h"
#include	"items.h"
#include	"itemdat.h"
#include	"player.h"
#include	"effects.h"
#include	"dead.h"
#include	"missiles.h"
#include	"misdat.h"
#include	"control.h"
#include	"objects.h"
#include	"objdat.h"
#include	"path.h"
#include	"quests.h"
#include	"minitext.h"
#include	"textdat.h"
#include	"monstint.h"
#include	"msg.h"
#include	"inv.h"
#include	"multi.h"
#include	"trigs.h"
#include	"themes.h"
#include	"drlg_l4.h"
#include	"towners.h"
#include	"setmaps.h"
#include	"palette.h"
#include	"scrollrt.h"
#include	"cursor.h"
#include	"drlg_l1.h"
#include	"spells.h"


/*-----------------------------------------------------------------------*
** Function Prototypes
**-----------------------------------------------------------------------*/
BOOL LineClear(int x1, int y1, int x2, int y2);
int M_GetDir(int i);

typedef BOOL (*CHECKFUNC)(int x, int y);
typedef BOOL (*CHECKFUNC1)(int arg1, int x, int y);
BOOL LineClearF(CHECKFUNC Clear, int x1, int y1, int x2, int y2);
BOOL LineClearF1(CHECKFUNC1 Clear, int arg, int x1, int y1, int x2, int y2);
BOOL CheckNoSolid(int x, int y);
BOOL PosOkMissile(int x, int y);
int M_SpawnSkel(int x, int y, int dir);
void M_Teleport(int i);
void PlaceGroup(int, int, BOOL, int);
void ClrAllMonsters();
BOOL PosOkMonst2(int i, int x, int y);
BOOL PosOkMonst3(int i, int x, int y);
BOOL effect_is_playing(int nSFX);

void SpawnMap(int x, int y);
void SpawnBear(int x, int y);

extern const TextDataStruct alltext[];

//void DaveMonstMap(BOOL initupdate, int pnum);

/*-----------------------------------------------------------------------*
** Global Variables
**-----------------------------------------------------------------------*/

MonsterStruct	monster[MAXMONSTERS];
long nummonsters;
int totalmonsters;

// monstactive -- indexes into monster[]
int monstactive[MAXMONSTERS];	// [0..nummonsters-1] -- active monsters
								// [nummonsters..MAXMONSTERS-1] -- available monsters

int nummtypes = 0;
long monstimgtot;
int gfxflags;
int uniquetrans;				// index to next available unique monster palette translation

int mleveltypes[NUMLEVELS][MAX_LVLMTYPES];

long monstkills[MONSTERTYPES];

static char sgszInvalidMonstName[] = "Invalid Monster";

/*-----------------------------------------------------------------------*
** Defines
**-----------------------------------------------------------------------*/
#define PACK_MEMBER 1
#define PACK_NOMEMBER 2

#define INITMONSTRAD	15		// Initial vision radius for no monsters

/*-----------------------------------------------------------------------*
** Macros
**-----------------------------------------------------------------------*/
#define InBounds(x,y)	(0 <= (y) && (y) < MAXDUNY && 0 <= (x) && (x) < MAXDUNX)
#define WALKMODE(m) ((m) == MM_WALK || (m) == MM_WALK2 || (m) == MM_WALK3)
#define Sign(x) ((x) < 0 ? -1 : (x) > 0 ? 1 : 0)
#define Mod(val, x)  ((val) < 0 ? (val)+(x) : (val) >= (x) ? (val)-(x) : (val))

/*-----------------------------------------------------------------------*
** File Variables
**-----------------------------------------------------------------------*/
// I had to add this variable, because _mVar2 seems like
// it must have been trashed, since sound never came on again -- pjw
static BYTE sgbSaveSoundOn;

						// 16   32   64
int	MWVel[24][3] = {	{0x100,0x200,0x400},	// 1 frame
						{0x80,0x100,0x200},	// 2 frame
						{0x55,0xaa,0x155},	// 3 frame
						{0x40,0x80,0x100},	// 4 frame
						{0x33,0x66,0xcc},	// 5 frame
						{0x2a,0x55,0xaa},	// 6 frame
						{0x24,0x49,0x92},	// 7 frame
						{0x20,0x40,0x80},	// 8 frame
						{0x1c,0x38,0x71},	// 9 frame
						{0x1a,0x33,0x66},	// 10 frame
						{0x17,0x2e,0x5d},	// 11 frame
						{0x15,0x2a,0x55},	// 12 frame
						{0x13,0x27,0x4e},	// 13 frame
						{0x12,0x24,0x49},	// 14 frame
						{0x11,0x22,0x44},	// 15 frame
						{0x10,0x20,0x40},	// 16 frame
						{0x0f,0x1e,0x3c},	// 17 frame
						{0x0e,0x1c,0x39},	// 18 frame
						{0x0d,0x1a,0x36},	// 19 frame
						{0x0c,0x19,0x33},	// 20 frame
						{0x0c,0x18,0x30},	// 21 frame
						{0x0b,0x17,0x2e},	// 22 frame
						{0x0b,0x16,0x2c},	// 23 frame
						{0x0a,0x15,0x2a} };	// 24 frame

// Muli-player monster multiplier %
/*int MPMM[4] = {	100,	// 1 player
				150,	// 2 player
				175,	// 3 player
				200 };	// 4 player*/

CMonster Monsters[MAX_LVLMTYPES];

char animletter[] = "nwahds";
int left[] = {7,0,1,2,3,4,5,6};
int right[] = {1,2,3,4,5,6,7,0};
int opposite[] = {4,5,6,7,0,1,2,3};
int offset_x[] = {1,0,-1,-1,-1,0,1,1};
int offset_y[] = {1,1,1,0,-1,-1,-1,0};
static const infront_x[] = { 1, 0,-1, -1, -1,  0,  1,  1};
static const infront_y[] = { 1, 1, 1,  0, -1, -1, -1,  0};
int rnd5[] = { 5, 10, 15, 20 };
int rnd10[] = { 10, 15, 20, 30 };
int rnd20[] = { 20, 30, 40, 50 };
int rnd60[] = { 60, 70, 80, 90 };


void MAI_Zombie(int);
void MAI_Fat(int);
void MAI_SkelSd(int);
void MAI_SkelBow(int);
void MAI_Scav(int);
void MAI_Rhino(int);
void MAI_GoatMc(int);
void MAI_GoatBow(int);
void MAI_Fallen(int);
void MAI_Magma(int);
void MAI_SkelKing(int);
void MAI_Bat(int);
void MAI_Garg(int);
void MAI_Cleaver(int);
void MAI_Succ(int);
void MAI_Lich(int);
void MAI_ArchLich(int);
void MAI_Psychorb(int);
void MAI_Necromorb(int);
void MAI_Sneak(int);
void MAI_Storm(int);
void MAI_Fireman(int i);
void MAI_Garbud(int);
void MAI_Acid(int);
void MAI_AcidUniq(int);
void MAI_Golum(int);
void MAI_Zhar(int);
void MAI_SnotSpil(int);
void MAI_Snake(int);
void MAI_Counselor(int);
void MAI_Mega(int);
void MAI_Diablo(int);
void MAI_Lazurus(int);
void MAI_Lazhelp(int);
void MAI_Lachdanan(int);
void MAI_Warlord(int);
void MAI_Firebat(int);
void MAI_Hellbat(int);
void MAI_HorkDemon(int);
void MAI_BoneDemon(int);

void (*AiProc[NUM_AI])(int) =
{
	MAI_Zombie,
	MAI_Fat,
	MAI_SkelSd,
	MAI_SkelBow,
	MAI_Scav,
	MAI_Rhino,
	MAI_GoatMc,
	MAI_GoatBow,
	MAI_Fallen,
	MAI_Magma,
	MAI_SkelKing,
	MAI_Bat,
	MAI_Garg,
	MAI_Cleaver,
	MAI_Succ,
	MAI_Sneak,
	MAI_Storm,
	MAI_Fireman,
	MAI_Garbud,
	MAI_Acid,
	MAI_AcidUniq,
	MAI_Golum,
	MAI_Zhar,
	MAI_SnotSpil,
	MAI_Snake,
	MAI_Counselor,
	MAI_Mega,
	MAI_Diablo,
	MAI_Lazurus,
	MAI_Lazhelp,
	MAI_Lachdanan,
	MAI_Warlord,
	MAI_Firebat,
	MAI_Hellbat,
	MAI_HorkDemon,
	MAI_Lich,
	MAI_ArchLich,
	MAI_Psychorb,
	MAI_Necromorb,
	MAI_BoneDemon,
};


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void TranslateMonsterGFX(int monst, BOOL HasSpecial)
{
	app_assert((DWORD)monst < MAX_LVLMTYPES);
	BYTE * pData = Monsters[monst].pTrans;

	for (int n = 256; n--; ++pData) {
		if (*pData == 255)
			*pData = 0;
	}
	int const nf = HasSpecial ? 6 : 5;
	for (int j = 0; j < nf; ++j) {
		if ((j == 1) && (Monsters[monst].mtype >= MT_COUNSLR) && (Monsters[monst].mtype <= MT_ADVOCATE)) continue;
		for (int i = 0; i < 8; ++i)
			TranslateCels(Monsters[monst].Anims[j].Cels[i], Monsters[monst].pTrans, Monsters[monst].Anims[j].Frames);
	}
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

#if 0 // UNUSED
static BOOL MonstTaken(int i)
{
	int lev;
	BOOL found = FALSE;
	int m;
	int templvl;	// JKE

// Temp hack for monsters JKE 7/30
	templvl = currlevel;
//	if (templvl > 20) templvl -= 8;
//	else if (templvl > 16) templvl -= 4;

//	for(lev = 0; lev <= currlevel && !found; ++lev)		// JKE
	for(lev = 0; lev <= templvl && !found; ++lev)
	{
		for(m = 0; m < MAX_LVLMTYPES && mleveltypes[m] && !found; ++m)
			found = ((i < mleveltypes[lev][m]-1)
					&&
					!strcmp(monsterdata[i].filename, monsterdata[mleveltypes[lev][m]-1].filename));
	}
	return found;
}
#endif // UNUSED

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void InitLevelMonsters()
{
	int i;

	nummtypes = 0;
	monstimgtot = 0;
	gfxflags = 0;

	for (i = 0; i < MAX_LVLMTYPES; ++i)
		Monsters[i].mPlaceFlags = 0;

	ClrAllMonsters();
	nummonsters = 0;
	totalmonsters = MAXMONSTERS;		// this value gets further restricted later on

	// active monsters list
	for (i = 0; i < MAXMONSTERS; ++i) {
		monstactive[i] = i;
	}
	uniquetrans = 0;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
int AddMonsterType(int type, int placeflag)
{
	int i;
	BOOL done = FALSE;

	// check if this type is already being loaded
	app_assert((DWORD)nummtypes <= MAX_LVLMTYPES);
	for (i = 0; i < nummtypes && !done; ++i)
		done = Monsters[i].mtype == type;
	--i;

	if (!done)
	{
		i = nummtypes++;
		Monsters[i].mtype = type;
		monstimgtot += monsterdata[type].mImgSize;
		InitMonsterGFX(i);
		InitMonsterSND(i);
	}		

	Monsters[i].mPlaceFlags |= placeflag;

	return i;
}

/*-----------------------------------------------------------------------*
 * GetLevelMTypes()
 *
 *	Pick the monsters types to be placed in this level
**-----------------------------------------------------------------------*/

void GetLevelMTypes()
{
	int typelist[MONSTERTYPES];
	int nt, i;
	int	tidx, mt;
	int minl,maxl;
	char mamask;
	int templvl;		// temp hack JKE

	#if IS_VERSION(SHAREWARE)
	mamask = MAT_SW;
	#else
	mamask = MAT_SW | MAT_YES;
	#endif

	// Certain monsters are required for certain levels

	AddMonsterType(MT_GOLEM, MPFLAG_DONT);

	if (currlevel == 16) {
		AddMonsterType(MT_ADVOCATE, MPFLAG_SCATTER);
		AddMonsterType(MT_RBLACK, MPFLAG_SCATTER);
		AddMonsterType(MT_DIABLO, MPFLAG_DONT);
		return;
	}
	
	if (currlevel == HIVESTART + 1)
	{
		AddMonsterType(MT_SPAWN, MPFLAG_SCATTER);
	}
	if (currlevel == HIVESTART + 2)
	{
		AddMonsterType(MT_SPAWN, MPFLAG_SCATTER);
		AddMonsterType(MT_HORKD, MPFLAG_UNIQ);
	}
	if (currlevel == HIVEEND)
	{
		AddMonsterType(MT_BUG, MPFLAG_UNIQ);
	}
	if (currlevel == NA_KRUL_LEVEL /*CRYPTEND*/)
	{
		AddMonsterType(MT_LICH2, MPFLAG_SCATTER);
		AddMonsterType(MT_NKR, MPFLAG_DONT);
	}

	// Set levels load their own monster types
	if (!setlevel)
	{
		if (QuestStatus(Q_BUTCHER)) {
			AddMonsterType(MT_CLEAVER, MPFLAG_DONT);
		}

		if (QuestStatus(Q_GARBUD)) {
			AddMonsterType(UniqMonst[MU_GARBUD].mtype, MPFLAG_UNIQ);
		}

		if (QuestStatus(Q_ZHAR)) {
			AddMonsterType(UniqMonst[MU_ZHAR].mtype, MPFLAG_UNIQ);
		}

		if (QuestStatus(Q_LTBANNER)) {
			AddMonsterType(UniqMonst[MU_SNOTSPIL].mtype, MPFLAG_UNIQ);
		}

		if (QuestStatus(Q_VEIL)) {
			AddMonsterType(UniqMonst[MU_LACHDA].mtype, MPFLAG_UNIQ);
		}

		if (QuestStatus(Q_WARLORD)) {
			AddMonsterType(UniqMonst[MU_WARLORD].mtype, MPFLAG_UNIQ);
		}

		if (gbMaxPlayers != 1 && currlevel == quests[Q_SKELKING]._qlevel) {
			AddMonsterType(MT_SKING, MPFLAG_UNIQ);

			// Skelking requires skeleton minions
			int skeltypes[LASTMT];
			int numskeltypes = 0;

			for (i = MT_WSKELAX; i <= MT_XSKELSD; ++i)
			{
				if (IsSkel(i))
				{
					minl = ((monsterdata[i].mMinDLvl * 15) / 30) + 1;
					maxl = ((monsterdata[i].mMaxDLvl * 15) / 30) + 1;
					if ((currlevel >= minl) && (currlevel <= maxl) && (MonstAvailTbl[i] & mamask))
						skeltypes[numskeltypes++] = i;
				}
			}
			AddMonsterType(skeltypes[random(88,numskeltypes)], MPFLAG_SCATTER);
		}

		// Pick general monsters

		// Make list of available monster types for this level
// Temp hack monster JKE
		templvl = currlevel;
//		if (templvl > 20) templvl -=8;
//		else if (templvl > 16) templvl -= 4;

		nt = 0;
		for (i = 0; i < LASTMT; ++i) {
			minl = ((monsterdata[i].mMinDLvl * 15) / 30) + 1;
			maxl = ((monsterdata[i].mMaxDLvl * 15) / 30) + 1;
//			if ((currlevel >= minl) && (currlevel <= maxl) && (MonstAvailTbl[i] & mamask)) {
//				typelist[nt] = i;
//				++nt;
//			}
			// use templvl to patch over current level
			if ((templvl >= minl) && (templvl <= maxl) && (MonstAvailTbl[i] & mamask)) {
				typelist[nt] = i;
				++nt;
			}
		}

		if (monstdebug)
		{
			for(i = 0; i < debugmonsttypes; ++i)
				AddMonsterType(DebugMonsters[i], MPFLAG_SCATTER);
		}
		else
		{
			while ((nt > 0) && (nummtypes < MAX_LVLMTYPES) && (monstimgtot < IMG_MAX)) {
				// prune excessively large monsters
				for (i = 0; i < nt; )
				{
					if (monsterdata[typelist[i]].mImgSize > IMG_MAX - monstimgtot)
						typelist[i] = typelist[--nt];
					else
						++i;
				}

				if (nt)
				{
					tidx = random(88, nt);
					mt = typelist[tidx];
					AddMonsterType(mt, MPFLAG_SCATTER);
					typelist[tidx] = typelist[--nt];
				}
			}
		}
	}
	else if (setlvlnum == SL_SKELKING) {
		AddMonsterType(MT_SKING, MPFLAG_UNIQ);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void InitMonsterGFX (int monst)
{
	int anim;
	long i;
	char strBuff[256];
	int mtype;
	BYTE *p;

	app_assert((DWORD)monst < MAX_LVLMTYPES);
	mtype = Monsters[monst].mtype;
	for(anim = 0; anim < MAX_ANIMTYPE; ++anim)
	{ 
		if((!(animletter[anim] == 's' && !monsterdata[mtype].has_special)) && monsterdata[mtype].Frames[anim] > 0)
		{
			sprintf(strBuff, monsterdata[mtype].filename, animletter[anim]);
			app_assert(! Monsters[monst].Anims[anim].CMem);
			Monsters[monst].Anims[anim].CMem = LoadFileInMemSig(strBuff,NULL,'MONS');
			p = Monsters[monst].Anims[anim].CMem;
			if (Monsters[monst].mtype == MT_GOLEM && (animletter[anim] == 's' || animletter[anim] == 'd')) {
				for(i=0; i<8; ++i) Monsters[monst].Anims[anim].Cels[i] = p;
			} else {
				for(i=0; i<8; ++i) Monsters[monst].Anims[anim].Cels[i] = p + *(DWORD *)(p + (i<<2));
			}
		}
		Monsters[monst].Anims[anim].Frames = monsterdata[mtype].Frames[anim];
		Monsters[monst].Anims[anim].Rate = monsterdata[mtype].Rate[anim];
	}
	Monsters[monst].mAnimWidth = monsterdata[mtype].mAnimWidth;
	Monsters[monst].mAnimWidth2 = (monsterdata[mtype].mAnimWidth - 64) >> 1;
	Monsters[monst].mMinHP = monsterdata[mtype].mMinHP;
	Monsters[monst].mMaxHP = monsterdata[mtype].mMaxHP;
	Monsters[monst].has_special = monsterdata[mtype].has_special;
	Monsters[monst].mAFNum = monsterdata[mtype].mAFNum;
	Monsters[monst].MData = &monsterdata[mtype];
	if (monsterdata[mtype].transflag) {
		Monsters[monst].pTrans = LoadFileInMemSig(monsterdata[mtype].TransFile,NULL,'MONS');
		TranslateMonsterGFX(monst, monsterdata[mtype].has_special);
		DiabloFreePtr(Monsters[monst].pTrans);
	}
	if (EquivMonst(mtype, MT_NMAGMA) && ((gfxflags & 0x0001) == 0)) {
		gfxflags |= 0x0001; 
		ILoadMissileGFX(MF_MAGBALL);
	}

	if (EquivMonst(mtype, MT_STORM) && ((gfxflags & 0x0002) == 0)) {
		gfxflags |= 0x0002; 
		ILoadMissileGFX(MF_THINLIGHT);
	}

	if ((mtype == MT_SUCCUBUS) && ((gfxflags & 0x0004) == 0)) {
		gfxflags |= 0x0004; 
//		ILoadMissileGFX(MF_FLARE);		already used for Blood Star
//		ILoadMissileGFX(MF_FLAREXP);
	}
	
	if (EquivMonst(mtype, MT_INCIN) && ((gfxflags & 0x0008) == 0)) {
		gfxflags |= 0x0008; 
		ILoadMissileGFX(MF_KRULL);
	}
	
	if ((EquivMonst(mtype, MT_NACID) || mtype == MT_SPIDER2) && 
		((gfxflags & 0x0010) == 0)) {
		gfxflags |= 0x0010; 
		ILoadMissileGFX(MF_ACID);
		ILoadMissileGFX(MF_ACIDSPLAT);
		ILoadMissileGFX(MF_ACIDPUD);
	}

	if ((mtype == MT_SNOWWICH) && ((gfxflags & 0x0020) == 0)) {
		gfxflags |= 0x0020; 
		ILoadMissileGFX(MF_BFLARE);
		ILoadMissileGFX(MF_BFLAREXP);
	}
	if ((mtype == MT_HLSPWN) && ((gfxflags & 0x0040) == 0)) {
		gfxflags |= 0x0040; 
		ILoadMissileGFX(MF_DFLARE);
		ILoadMissileGFX(MF_DFLAREXP);
	}
	if ((mtype == MT_SOLBRNR) && ((gfxflags & 0x0080) == 0)) {
		gfxflags |= 0x0080; 
		ILoadMissileGFX(MF_CFLARE);
		ILoadMissileGFX(MF_CFLAREXP);
	}

	if ((mtype == MT_LICH) && ((gfxflags & 0x0100) == 0))
	{
		gfxflags |= 0x0100; 
		ILoadMissileGFX(MF_ORANGEFLARE);
		ILoadMissileGFX(MF_ORANGEEXPLOSION);
	}
	
	if ((mtype == MT_LICH2) && ((gfxflags & 0x0200) == 0))
	{
		gfxflags |= 0x0200; 
		ILoadMissileGFX(MF_YELLOWFLARE);
		ILoadMissileGFX(MF_YELLOWEXPLOSION);
	}
		
	if ((mtype == MT_EYE || mtype == MT_BONED2) && ((gfxflags & 0x0400) == 0))
	{
		gfxflags |= 0x0400; 
		ILoadMissileGFX(MF_BLUE2FLARE);
	}
		
	if ((mtype == MT_EYE2) && ((gfxflags & 0x0800) == 0))
	{
		gfxflags |= 0x0800; 
		ILoadMissileGFX(MF_REDFLARE);
		ILoadMissileGFX(MF_REDEXPLOSION);
	}
	
		
	if ((mtype == MT_EYE) && ((gfxflags & 0x1000) == 0))
	{
		gfxflags |= 0x1000; 
		ILoadMissileGFX(MF_BLUEEXPLOSION);
	}
		
	if ((mtype == MT_BONED2) && ((gfxflags & 0x2000) == 0))
	{
		gfxflags |= 0x2000; 
		ILoadMissileGFX(MF_BLUE2EXPLOSION);
	}
		
		
	if (mtype == MT_DIABLO)
		ILoadMissileGFX(MF_FIREPLAR);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void ClearMVars(int i)
{
	app_assert((DWORD)i < MAXMONSTERS);
	monster[i]._mVar1 = 0;
	monster[i]._mVar2 = 0;
	monster[i]._mVar3 = 0;
	monster[i]._mVar4 = 0;
	monster[i]._mVar5 = 0;
	monster[i]._mVar6 = 0;
	monster[i]._mVar7 = 0;
	monster[i]._mVar8 = 0;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/


void InitMonster(int i, int rd, int mtype, int x, int y)
{
	app_assert((DWORD)i < MAXMONSTERS);
	app_assert((DWORD)mtype < MAX_LVLMTYPES);
	CMonster *monst = &Monsters[mtype];
	app_assert(monst->MData != NULL);

	monster[i]._mdir = rd;
	monster[i]._mx = x;
	monster[i]._my = y;
	monster[i]._mfutx = x;
	monster[i]._mfuty = y;
	monster[i]._moldx = x;
	monster[i]._moldy = y;

	monster[i]._mMTidx = mtype;
	monster[i]._mmode = MM_STAND;
	monster[i].mName = monst->MData->mName;
	monster[i].MType = monst;
	monster[i].MData = monst->MData;
	monster[i]._mAnimData = monst->Anims[MA_STAND].Cels[rd];
	monster[i]._mAnimDelay = monst->Anims[MA_STAND].Rate;
	monster[i]._mAnimCnt = random(88, monster[i]._mAnimDelay - 1);
	monster[i]._mAnimLen = monst->Anims[MA_STAND].Frames;
	monster[i]._mAnimFrame = random(88, monster[i]._mAnimLen - 1) + 1;
	if (monst->mtype == MT_DIABLO) {
//		monster[i]._mmaxhp = (random(88, 1666 - 1666 + 1) + 1666) << HP_SHIFT;
		monster[i]._mmaxhp = (random(88, 3333 - 3333 + 1) + 3333) << HP_SHIFT;
	} else {
		monster[i]._mmaxhp = (random(88, monst->mMaxHP - monst->mMinHP + 1) + monst->mMinHP) << HP_SHIFT;
	}
/*	if (gbMaxPlayers != 1)
		monster[i]._mmaxhp = (monster[i]._mmaxhp * MPMM[gbActivePlayers-1]) / 100;
	else
		monster[i]._mmaxhp = monster[i]._mmaxhp >> 1;
	if (monster[i]._mmaxhp < (1 << HP_SHIFT)) monster[i]._mmaxhp = 1 << HP_SHIFT;*/
	if (gbMaxPlayers == 1) {
		monster[i]._mmaxhp = monster[i]._mmaxhp >> 1;
		if (monster[i]._mmaxhp < (1 << HP_SHIFT)) monster[i]._mmaxhp = 1 << HP_SHIFT;
	}
	monster[i]._mhitpoints = monster[i]._mmaxhp;
	monster[i]._mAi  = monst->MData->mAi;
	monster[i]._mint = monst->MData->mInt;
	monster[i]._mgoal = MG_ATTACK;
	monster[i]._mgoalvar1 = 0;
	monster[i]._mgoalvar2 = 0;
	monster[i]._mgoalvar3 = 0;
	monster[i]._mgoalvar4 = 0;
	monster[i]._pathcount = 0;
	monster[i]._mDelFlag = FALSE;
	monster[i]._uniqtype = 0;
	monster[i]._msquelch = 0;
	
	monster[i].mlid = 0;

	monster[i]._mRndSeed = GetRndSeed();
	monster[i]._mAISeed = GetRndSeed();

	monster[i].mWhoHit = 0;

	monster[i].mLevel = monst->MData->mLevel;
	monster[i].mExp = monst->MData->mExp;
	monster[i].mHit = monst->MData->mHit;
	monster[i].mMinDamage = monst->MData->mMinDamage;
	monster[i].mMaxDamage = monst->MData->mMaxDamage;
	monster[i].mHit2 = monst->MData->mHit2;
	monster[i].mMinDamage2 = monst->MData->mMinDamage2;
	monster[i].mMaxDamage2 = monst->MData->mMaxDamage2;
	monster[i].mArmorClass = monst->MData->mArmorClass;
	monster[i].mMagicRes = monst->MData->mMagicRes;
	monster[i].leader = 0;
	monster[i].leaderflag = 0;
	monster[i]._mFlags = monst->MData->mFlags;
	monster[i].mtalkmsg = 0;

	if (monster[i]._mAi == AI_GARG)
	{
		monster[i]._mAnimData = monst->Anims[MA_SPECIAL].Cels[rd];
		monster[i]._mAnimFrame = 1;
		monster[i]._mFlags |= MFLAG_STILL;
		monster[i]._mmode = MM_SATTACK;
	}

	if (gnDifficulty == D_NIGHTMARE) {
		monster[i]._mmaxhp = (monster[i]._mmaxhp * 3) + (((gbMaxPlayers == 1) ? 50 : 100) << HP_SHIFT);
		monster[i]._mhitpoints = monster[i]._mmaxhp;
		monster[i].mLevel += 15;
		monster[i].mExp = (monster[i].mExp << 1) + 2000;
		monster[i].mHit += 85;
		monster[i].mMinDamage = (monster[i].mMinDamage * 2) + 4;
		monster[i].mMaxDamage = (monster[i].mMaxDamage * 2) + 4;
		monster[i].mHit2 += 85;
		monster[i].mMinDamage2 = (monster[i].mMinDamage2 * 2) + 4;
		monster[i].mMaxDamage2 = (monster[i].mMaxDamage2 * 2) + 4;
		monster[i].mArmorClass += 50;
	}
	else if (gnDifficulty == D_HELL) {
		monster[i]._mmaxhp = (monster[i]._mmaxhp * 4) + (((gbMaxPlayers == 1) ? 100 : 200) << HP_SHIFT);
		monster[i]._mhitpoints = monster[i]._mmaxhp;
		monster[i].mLevel += 30;
		monster[i].mExp = (monster[i].mExp << 2) + 4000;
		monster[i].mHit += 120;
		monster[i].mMinDamage = (monster[i].mMinDamage * 4) + 6;
		monster[i].mMaxDamage = (monster[i].mMaxDamage * 4) + 6;
		monster[i].mHit2 += 120;
		monster[i].mMinDamage2 = (monster[i].mMinDamage2 * 4) + 6;
		monster[i].mMaxDamage2 = (monster[i].mMaxDamage2 * 4) + 6;
		monster[i].mArmorClass += 80;
		monster[i].mMagicRes = monst->MData->mMagicRes2;
	}
}
	

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void ClrAllMonsters()
{
	int i;
	MonsterStruct	*Monst;

	for (i = 0; i < MAXMONSTERS; ++i) {
		Monst = &monster[i];
		ClearMVars(i);
		
		//track down freeloaders from previous levels by making them obvious
		Monst->mName = sgszInvalidMonstName;

		Monst->_mgoal = 0;
		Monst->_mmode = MM_STAND;
		Monst->_mVar1 = MM_STAND;
		Monst->_mVar2 = 0;
		Monst->_mx = 0;
		Monst->_my = 0;
		Monst->_mfutx = 0;
		Monst->_mfuty = 0;
		Monst->_moldx = 0;
		Monst->_moldy = 0;
		Monst->_mdir = random(89, 8);
		Monst->_mxvel = 0;
		Monst->_myvel = 0;
		Monst->_mAnimData = NULL;
		Monst->_mAnimDelay = 0;
		Monst->_mAnimCnt = 0;
		Monst->_mAnimLen = 0;
		Monst->_mAnimFrame = 0;
		Monst->_mFlags = 0;
		Monst->_mDelFlag = FALSE;
		Monst->_menemy = random(89, gbActivePlayers);
		Monst->_menemyx = plr[Monst->_menemy]._pfutx;
		Monst->_menemyy = plr[Monst->_menemy]._pfuty;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
BOOL MonstPlace(int xp, int yp) {
	if (xp < 0 || xp >= DMAXX || yp < 0 || yp >= DMAXY) return FALSE;
	if (dMonster[xp][yp] != 0) return FALSE;
	if (dPlayer[xp][yp] != 0) return FALSE;
	if (dFlags[xp][yp] & BFLAG_MONSTACTIVE) return FALSE;
	if (dFlags[xp][yp] & BFLAG_SETPC) return FALSE;
	if (SolidLoc(xp,yp)) return FALSE;
	return TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void PlaceMonster(int i, int mtype, int x, int y)
{
	int rd;

	// stoopid hack to NEVER get duplicate Na-Kruls
	if (Monsters[mtype].mtype == MT_NKR)
	{
		int monstindex;
		for (monstindex = 0; monstindex < nummonsters; ++monstindex)
			if (monster[monstindex]._mMTidx == mtype ||
				monster[monstindex].MType->mtype == MT_NKR)
				return;
	}
	
	dMonster[x][y] = i + 1;
	rd = random(90, 8);
	InitMonster(i, rd, mtype, x, y);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void LoadUniMonstTrans(int uid)
{

}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void PlaceUniqueMonst(int uniqindex, int miniontype, int packsize)
{
	app_assert((DWORD)nummonsters < MAXMONSTERS);
	UniqMonstStruct *Uniq = &UniqMonst[uniqindex];
	MonsterStruct	*Monst = &monster[nummonsters];

	int xp,yp;
	int x,y;
	BOOL done;
	int count, count2;
	char	filestr[64];
	done = FALSE;
	count2 = 0;
	int uniqtype;

	// Check if too many uniques.  Limit is unique palette translation table size
	if((uniquetrans << 8) + 4864 >= LIGHTSIZE)
		return;

	// find the index of the monster graphics
	app_assert((DWORD)nummtypes <= MAX_LVLMTYPES);
	for (uniqtype = 0; uniqtype < nummtypes; ++uniqtype)
		if (Monsters[uniqtype].mtype == UniqMonst[uniqindex].mtype) break;
	app_assert(uniqtype < nummtypes);

	do
	{
		xp = random(91, DMAXX - DIRTEDGE) + (DIRTEDGED2);
		yp = random(91, DMAXY - DIRTEDGE) + (DIRTEDGED2);
		count = 0;
		for(x = xp-3; x < xp+3; ++x)
			for(y = yp-3; y < yp+3; ++y)
				if(InBounds(x,y) && MonstPlace(x,y))
					++count;
	} while((count < 9						// try finding a clear spot
			 && ++count2 < 1000)			// avoid infinite loop
			|| !MonstPlace(xp,yp));			// make sure there's at least
											//   a spot for the leader

	if(uniqindex == MU_SNOTSPIL) {
		xp = (setpc_x << 1) + DIRTEDGED2 + 8;
		yp = (setpc_y << 1) + DIRTEDGED2 + 12;
	}
	if(uniqindex == MU_WARLORD) {
		xp = (setpc_x << 1) + DIRTEDGED2 + 6;
		yp = (setpc_y << 1) + DIRTEDGED2 + 7;
	}
	if(uniqindex == MU_ZHAR) {
		int i;
		BOOL zharflag = TRUE;
		for (i = 0; i < themeCount; ++i) {
			if ((i == zharlib) && (zharflag == TRUE)) { 
				zharflag = FALSE;
				xp = ((themeLoc[i].x << 1) + DIRTEDGED2)+4; 
				yp = ((themeLoc[i].y << 1) + DIRTEDGED2)+4; 
			}
		}
	}
	if (gbMaxPlayers == 1) { 
 		if(uniqindex == MU_LAZARUS) {
			xp = 32;
			yp = 46;
		}
		if(uniqindex == MU_REDVEX) {
			xp = 40;
			yp = 45;
		}
		if(uniqindex == MU_BLKJADE) {
			xp = 38;
			yp = 49;
		}
		if(uniqindex == MU_SKELKING) {
			xp = 35;
			yp = 47;
		}
	} else {
	 	if(uniqindex == MU_LAZARUS) {
			xp = (setpc_x << 1) + DIRTEDGED2 + 3;
			yp = (setpc_y << 1) + DIRTEDGED2 + 6;
		}
		if(uniqindex == MU_REDVEX) {
			xp = (setpc_x << 1) + DIRTEDGED2 + 5;
			yp = (setpc_y << 1) + DIRTEDGED2 + 3;
		}
		if(uniqindex == MU_BLKJADE) {
			xp = (setpc_x << 1) + DIRTEDGED2 + 5;
			yp = (setpc_y << 1) + DIRTEDGED2 + 9;
		}
	}

	if (uniqindex == MU_CLEAVER) {
		done = FALSE;
		// Find certain tile where Butcher goes
		for (yp = 0; yp < DMAXY && !done; ++yp)
			for (xp = 0; xp < DMAXX && !done; ++xp)
				done = (dPiece[xp][yp] == 367);
		// NOTE: xp and yp get incremented an extra time after done=TRUE, but
		//		 coincidentally, that's where we want to place butcher
	}
	
	if (uniqindex == MU_NAKRUL)
	{
		if (Na_Krul.x == 0 || Na_Krul.y == 0)
		{
			Na_Krul.MIndex = -1;
			return;
		}
			
		xp = Na_Krul.x - 2;
		yp = Na_Krul.y;
		Na_Krul.MIndex = nummonsters;
	}
				
	PlaceMonster(nummonsters, uniqtype, xp, yp);
	Monst->_uniqtype = uniqindex+1;
	if (Uniq->mlevel != 0) Monst->mLevel = Uniq->mlevel*2;
	else Monst->mLevel += 5;
	Monst->mExp =			(Monst->mExp*2);
	Monst->mName =			Uniq->mName;
	Monst->_mmaxhp =		Uniq->mmaxhp << HP_SHIFT;
/*	if (gbMaxPlayers != 1)
		Monst->_mmaxhp = (Monst->_mmaxhp * MPMM[gbActivePlayers-1]) / 100;
	else
		Monst->_mmaxhp = Monst->_mmaxhp >> 1;
	if (Monst->_mmaxhp < (1 << HP_SHIFT)) Monst->_mmaxhp = 1 << HP_SHIFT;*/
	if (gbMaxPlayers == 1) {
		Monst->_mmaxhp = Monst->_mmaxhp >> 1;
		if (Monst->_mmaxhp < (1 << HP_SHIFT)) Monst->_mmaxhp = 1 << HP_SHIFT;
	}
	Monst->_mhitpoints =	Monst->_mmaxhp;
	Monst->_mAi =			Uniq->mAi;
	Monst->_mint =			Uniq->mint;
	Monst->mMinDamage =		Uniq->mMinDamage;
	Monst->mMaxDamage =		Uniq->mMaxDamage;
	Monst->mMinDamage2 =	Uniq->mMinDamage;
	Monst->mMaxDamage2 =	Uniq->mMaxDamage;
	Monst->mMagicRes =		Uniq->mMagicRes;
	Monst->mtalkmsg =		Uniq->mtalkmsg;
	// Don't light up the hork demon, it washes him out.
	if ( uniqindex == MU_HORKDEMON) {
		Monst->mlid = 0;
	}
	else {
		Monst->mlid = AddLight(Monst->_mx, Monst->_my, 3);
	}
	if ((gbMaxPlayers != 1) && (Monst->_mAi == AI_LAZHELP)) Monst->mtalkmsg = 0;
	if (Monst->mtalkmsg != 0) Monst->_mgoal = MG_TALK;
	if (gnDifficulty == D_NIGHTMARE) {
		Monst->_mmaxhp = (Monst->_mmaxhp * 3) + (((gbMaxPlayers == 1) ? 50 : 100) << HP_SHIFT);
		Monst->_mhitpoints = Monst->_mmaxhp;
		Monst->mLevel += 15;
		Monst->mExp = (Monst->mExp << 1) + 2000;
		Monst->mMinDamage = (Monst->mMinDamage * 2) + 4;
		Monst->mMaxDamage = (Monst->mMaxDamage * 2) + 4;
		Monst->mMinDamage2 = (Monst->mMinDamage2 * 2) + 4;
		Monst->mMaxDamage2 = (Monst->mMaxDamage2 * 2) + 4;
	}
	else if (gnDifficulty == D_HELL) {
		Monst->_mmaxhp = (Monst->_mmaxhp * 4) + (((gbMaxPlayers == 1) ? 100 : 200) << HP_SHIFT);
		Monst->_mhitpoints = Monst->_mmaxhp;
		Monst->mLevel += 30;
		Monst->mExp = (Monst->mExp << 2) + 4000;
		Monst->mMinDamage = (Monst->mMinDamage * 4) + 6;
		Monst->mMaxDamage = (Monst->mMaxDamage * 4) + 6;
		Monst->mMinDamage2 = (Monst->mMinDamage2 * 4) + 6;
		Monst->mMaxDamage2 = (Monst->mMaxDamage2 * 4) + 6;
	}

	// Load unique monster translations
	sprintf(filestr, "Monsters\\Monsters\\%s.TRN", Uniq->mTFile);
	app_assert((uniquetrans << 8) + 4864 < LIGHTSIZE);
	LoadFileWithMem(filestr, pLightTbl + (uniquetrans << 8) + 4864);
	Monst->_uniqtrans = uniquetrans;
	++uniquetrans;

	if(Uniq->mUnqAttr & UN_H)
	{
		Monst->mHit =		Uniq->mUnqVar1;
		Monst->mHit2 =		Uniq->mUnqVar1;
	}
	if(Uniq->mUnqAttr & UN_A)
	{
		Monst->mArmorClass = Uniq->mUnqVar1;
	}

	++nummonsters;

	if(Uniq->mUnqAttr & UN_PACK)
	{
		PlaceGroup(miniontype, packsize, Uniq->mUnqAttr, nummonsters-1);
	}
	// quick fix for monsters that were gargoyles before they became unique
	if (Monst->_mAi != AI_GARG)
	{
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[Monst->_mdir];
		Monst->_mAnimFrame = random(88, Monst->_mAnimLen - 1) + 1;
		Monst->_mFlags &= ~MFLAG_STILL;
		Monst->_mmode = MM_STAND;
	}
}

void Hose_NaKrul()
{	
	if (currlevel == NA_KRUL_LEVEL)
	{
		int mi = Na_Krul.MIndex;
		if (mi < 0 || mi >= nummonsters)
			return;
		
		MonsterStruct *Monst = &monster[mi];
		PlayEffect(mi, MS_DEATH);
		quests[Q_NA_KRUL]._qlog = FALSE;
	
		Monst->_mmaxhp		= Monst->_mmaxhp / 2;
		Monst->_mhitpoints	= Monst->_mmaxhp;
		Monst->mArmorClass -= 50;
		Monst->mMagicRes	= 0x0000;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
static void PlaceUniques()
{
	int u;
	int mt;
	BOOL done;

	//uniquetrans = 0;
	for(u = 0; UniqMonst[u].mtype != -1; ++u)
	{
#if CHEATS
		if ((UniqMonst[u].mlevel == currlevel)
			|| (UniqMonst[u].mlevel && davedebug)) {
#else
		if (UniqMonst[u].mlevel == currlevel) {
#endif
			done = FALSE;
			app_assert((DWORD)nummtypes <= MAX_LVLMTYPES);
			for(mt = 0; mt < nummtypes && !done; ++mt)
				done = Monsters[mt].mtype == UniqMonst[u].mtype;
			--mt;
			if ((u == MU_GARBUD) && (quests[Q_GARBUD]._qactive == QUEST_NOTAVAIL)) 
				done = FALSE;
			if ((u == MU_ZHAR) && (quests[Q_ZHAR]._qactive == QUEST_NOTAVAIL)) 
				done = FALSE;
			if ((u == MU_SNOTSPIL) && (quests[Q_LTBANNER]._qactive == QUEST_NOTAVAIL))
				done = FALSE;
			if ((u == MU_LACHDA) && (quests[Q_VEIL]._qactive == QUEST_NOTAVAIL))
				done = FALSE;
			if ((u == MU_WARLORD) && (quests[Q_WARLORD]._qactive == QUEST_NOTAVAIL))
				done = FALSE;
			if(done)
			{
				PlaceUniqueMonst(u, mt, 8);
			}
		}		
	}
}
#endif


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
static void PlaceQuestMonsters()
{
	int skeltype;
	byte *setp;

	if (!setlevel) {
		if (QuestStatus(Q_BUTCHER))
			PlaceUniqueMonst(MU_CLEAVER, 0, 0);

		if ((currlevel == quests[Q_SKELKING]._qlevel) && (gbMaxPlayers != 1))
		{
			app_assert((DWORD)nummtypes <= MAX_LVLMTYPES);
			for (skeltype = 0; skeltype < nummtypes; ++skeltype)
				if (IsSkel(Monsters[skeltype].mtype)) break;
			app_assert(skeltype < nummtypes);

			PlaceUniqueMonst(MU_SKELKING, skeltype, 30);
		}

		if (QuestStatus(Q_LTBANNER))
		{
			setp = LoadFileInMemSig("Levels\\L1Data\\Banner1.DUN",NULL,'MONS');
			SetMapMonsters(setp, (setpc_x << 1), (setpc_y << 1));
			DiabloFreePtr(setp);
		}

		if (QuestStatus(Q_BLOOD))
		{
			setp = LoadFileInMemSig("Levels\\L2Data\\Blood2.DUN", NULL, 'MONS');
			SetMapMonsters(setp, (setpc_x << 1), (setpc_y << 1));
			DiabloFreePtr(setp);
		}

		if (QuestStatus(Q_BLIND))
		{
			setp = LoadFileInMemSig("Levels\\L2Data\\Blind2.DUN", NULL, 'MONS');
			SetMapMonsters(setp, (setpc_x << 1), (setpc_y << 1));
			DiabloFreePtr(setp);
		}

		if (QuestStatus(Q_ANVIL))
		{
			setp = LoadFileInMemSig("Levels\\L3Data\\Anvil.DUN",NULL,'MONS');
			SetMapMonsters(setp, ((setpc_x+1) << 1), ((setpc_y+1) << 1));
			DiabloFreePtr(setp);
		}

		if (QuestStatus(Q_WARLORD))
		{
			setp = LoadFileInMemSig("Levels\\L4Data\\Warlord.DUN",NULL,'MONS');
			SetMapMonsters(setp, (setpc_x << 1), (setpc_y << 1));
			DiabloFreePtr(setp);
			AddMonsterType(UniqMonst[MU_WARLORD].mtype, MPFLAG_SCATTER); // to allow scatterable monst of mtype
		}

		if (QuestStatus(Q_VEIL))
		{
			AddMonsterType(UniqMonst[MU_LACHDA].mtype, MPFLAG_SCATTER);	// to allow scatterable monst of mtype
		}

		if (QuestStatus(Q_ZHAR)) {
			if (zharlib == -1)
				quests[Q_ZHAR]._qactive = QUEST_NOTAVAIL;
		}

		if ((currlevel == quests[Q_BETRAYER]._qlevel) && (gbMaxPlayers != 1))  {
			AddMonsterType(UniqMonst[MU_LAZARUS].mtype, MPFLAG_UNIQ);
			AddMonsterType(UniqMonst[MU_REDVEX].mtype, MPFLAG_UNIQ);
			PlaceUniqueMonst(MU_LAZARUS, 0, 0);
			PlaceUniqueMonst(MU_REDVEX, 0, 0);
			PlaceUniqueMonst(MU_BLKJADE, 0, 0);
			setp = LoadFileInMemSig("Levels\\L4Data\\Vile1.DUN",NULL,'MONS');
			SetMapMonsters(setp, (setpc_x << 1), (setpc_y << 1));
			DiabloFreePtr(setp);
		}
		if (currlevel == NA_KRUL_LEVEL)
		{
			//if ((quests[Q_NA_KRUL]._qactive != QUEST_NOTAVAIL) &&
			//	(quests[Q_NA_KRUL]._qlog))
			{
			
				int numminions = 2;
				if (gnDifficulty == D_NIGHTMARE)
					numminions = 4;
				else if (gnDifficulty == D_HELL)
					numminions = 6;
			
				// see if we already have a Na-Krul
				Na_Krul.MIndex = -1;
				
				int uniqtype;
				for (uniqtype = 0; uniqtype < nummtypes; ++uniqtype)
					if (Monsters[uniqtype].mtype == UniqMonst[MU_NAKRUL].mtype) break;
					
				if (uniqtype < nummtypes)	
				{
					int monstindex;
					for (monstindex = 0; monstindex < nummonsters; ++monstindex)
					{
						if (monster[monstindex]._uniqtype != 0 ||
							monster[monstindex]._mMTidx == uniqtype)
						{
							Na_Krul.MIndex = monstindex;
							break;
						}
					}
				}
				if (Na_Krul.MIndex == -1)	// didn't find
				{
					PlaceUniqueMonst(MU_NAKRUL, 0, 0);
				}
			}
		}
	}
	else if (setlvlnum == SL_SKELKING) {
			PlaceUniqueMonst(MU_SKELKING, 0, 0);
	}
}
#endif


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void PlaceGroup(int mtype, int num, BOOL leaderf, int leader)
{
	int xp, yp;
	int x1, y1;
	int j;
	int placed = 0;
	int try1 = 0;
	int try2;
	int rd;

	app_assert((DWORD)leader < MAXMONSTERS);
	do
	{
		// Clear out the subset placed last time through the loop
		while(placed)
		{
			--nummonsters;
			--placed;
			app_assert((DWORD)nummonsters < MAXMONSTERS);
			app_assert((DWORD)monster[nummonsters]._mx < MAXDUNX);
			app_assert((DWORD)monster[nummonsters]._my < MAXDUNY);
			dMonster[monster[nummonsters]._mx][monster[nummonsters]._my] = 0;
		}

		if(leaderf & UN_PACK)
		{
			rd = random(92, 8);
			x1 = xp = monster[leader]._mx + offset_x[rd];
			y1 = yp = monster[leader]._my + offset_y[rd];
		}
		else
		{
			do
			{
				x1 = xp = random(93, DMAXX - DIRTEDGE) + (DIRTEDGED2);
 				y1 = yp = random(93, DMAXY - DIRTEDGE) + (DIRTEDGED2);
			} while (!MonstPlace(xp, yp));
		}

		if ((nummonsters + num) > totalmonsters) num = totalmonsters - nummonsters;
		j = 0;
		try2 = 0;
		while (j < num && try2 < 100) {
			if (MonstPlace(xp, yp) && (dTransVal[xp][yp] == dTransVal[x1][y1])
				&& !((leaderf & UN_STICK) && !DIST(xp-x1,yp-y1,4))) {
				PlaceMonster(nummonsters, mtype, xp, yp);
				if(leaderf & UN_PACK)
				{
					monster[nummonsters]._mmaxhp *= 2;
					monster[nummonsters]._mhitpoints = monster[nummonsters]._mmaxhp;
					monster[nummonsters]._mint = monster[leader]._mint;
					if (leaderf & UN_STICK)
					{
						monster[nummonsters].leader = leader;
						monster[nummonsters].leaderflag = PACK_MEMBER;
						monster[nummonsters]._mAi = monster[leader]._mAi;
					}
					// quick fix for monsters that were gargoyles before they became unique
					if (monster[nummonsters]._mAi != AI_GARG)
					{
						monster[nummonsters]._mAnimData = monster[nummonsters].MType->Anims[MA_STAND].Cels[monster[nummonsters]._mdir];
						monster[nummonsters]._mAnimFrame = random(88, monster[nummonsters]._mAnimLen - 1) + 1;
						monster[nummonsters]._mFlags &= ~MFLAG_STILL;
						monster[nummonsters]._mmode = MM_STAND;
					}
				}

				++nummonsters;
				++placed;
				++j;
			}
			else
				++try2;
			xp += offset_x[random(94, 8)];
			yp += offset_x[random(94, 8)];
		}
	} while((placed < num) && (++try1 < 10));
	if(leaderf & UN_STICK)
		monster[leader].packsize = placed;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
void LoadDiabMonsts()
{
	BYTE *pSetPiece;
	int xx, yy;

	pSetPiece = LoadFileInMemSig("Levels\\L4Data\\diab1.DUN",NULL,'STPC');
	xx = (diabquad1x << 1);
	yy = (diabquad1y << 1);
	SetMapMonsters(pSetPiece, xx, yy);
	DiabloFreePtr(pSetPiece);

	pSetPiece = LoadFileInMemSig("Levels\\L4Data\\diab2a.DUN",NULL,'STPC');
	xx = (diabquad2x << 1);
	yy = (diabquad2y << 1);
	SetMapMonsters(pSetPiece, xx, yy);
	DiabloFreePtr(pSetPiece);

	pSetPiece = LoadFileInMemSig("Levels\\L4Data\\diab3a.DUN",NULL,'STPC');
	xx = (diabquad3x << 1);
	yy = (diabquad3y << 1);
	SetMapMonsters(pSetPiece, xx, yy);
	DiabloFreePtr(pSetPiece);

	pSetPiece = LoadFileInMemSig("Levels\\L4Data\\diab4a.DUN",NULL,'STPC');
	xx = (diabquad4x << 1);
	yy = (diabquad4y << 1);
	SetMapMonsters(pSetPiece, xx, yy);
	DiabloFreePtr(pSetPiece);
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void InitMonsters ()
{
	int i, mtype;
	int na, nt;
	int scattertypes[LASTMT];
	int numscattypes = 0;
	long fv,j;
	int numplacemonsters;
	int s,t;

void DaveCheck2();
	if (gbMaxPlayers != 1) DaveCheck2();

	// add 4 golem places first
	// WARNING!! IF YOU CHANGE THIS YOU MUST TEST POSION WATER QUEST!!!
	if (!setlevel) {
		AddMonster(1, 0, 0, 0, FALSE);
		AddMonster(1, 0, 0, 0, FALSE);
		AddMonster(1, 0, 0, 0, FALSE);
		AddMonster(1, 0, 0, 0, FALSE);
	}

	// We will place the monsters seperately for 16
#if !IS_VERSION(SHAREWARE)
	if (!setlevel && currlevel == 16) {
		LoadDiabMonsts();
	}
#endif

	// Monsters are not placed where bFlags[][] & BFLAG_MONSTACTIVE is set
	// This prevents:	1) Monsters attacking players as they enter dungeon
	//					2) Monsters setting off traps

	nt = numtrigs;
	if (currlevel == 15) nt = 1;
	for (i = 0; i < nt; ++i)
	{
		for (s = -2; s < 2; ++s)
			for (t = -2; t < 2; ++t)
				DoVision(trigs[i]._tx + s, trigs[i]._ty + t, INITMONSTRAD, FALSE, FALSE);
	}

#if !IS_VERSION(SHAREWARE)
	PlaceQuestMonsters();
#endif

	if (!setlevel)
	{
#if !IS_VERSION(SHAREWARE)
		PlaceUniques();  // Place uniques before any other monsters
#endif
		// Calc a volume of monsters
		fv = 0;
		for (i = DIRTEDGED2; i < (DMAXY - (DIRTEDGED2)); ++i) {
			for (j = DIRTEDGED2; j < (DMAXX - (DIRTEDGED2)); ++j) {
				if (!SolidLoc(i,j)) ++fv;
			}
		}
		numplacemonsters = fv / MONSTDENSITY;
#if 0		// Debugging = 1, normal = 0
		numplacemonsters = 1;
#else
		if (gbMaxPlayers != 1) numplacemonsters += (numplacemonsters >> 1);
#endif
		if (numplacemonsters + nummonsters > MAXMONSTERS - 10)
			numplacemonsters = MAXMONSTERS - 10 - nummonsters;

		totalmonsters = nummonsters + numplacemonsters;


#ifndef PACKS_ONLY		// switch for debugging pack AI
		// Place scattered monsters

		app_assert((DWORD)nummtypes <= MAX_LVLMTYPES);
		for (i = 0; i < nummtypes; ++i)
		{
			if (Monsters[i].mPlaceFlags & MPFLAG_SCATTER)
				scattertypes[numscattypes++] = i;
		}

		while (nummonsters < totalmonsters) {
			mtype = scattertypes[random(95, numscattypes)];

			if (currlevel != 1			// no groups on level 1
				&& random(95, 2))
			{
				if (currlevel == 2 || 		// half-size groups on level 2
					(currlevel >= CRYPTSTART && currlevel <= CRYPTEND))
					na = random(95, 2) + 2;
				else
					na = random(95, 3) + 3;
			}
			else na = 1;

			PlaceGroup(mtype, na, FALSE, NULL);
		}
#endif // PACKS_ONLY
	}

	for (i = 0; i < nt; ++i)
	{
		for (s = -2; s < 2; ++s)
			for (t = -2; t < 2; ++t)
				DoUnVision(trigs[i]._tx + s, trigs[i]._ty + t, INITMONSTRAD);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SetMapMonsters(BYTE *pMap, int startx, int starty)
{
	int i,j;
	WORD rw,rh;
	WORD *lm;
	int mt,mx,my;
	int mtype;

	// add 4 golem places first
	// WARNING! IF YOU CHANGE THIS YOU MUST TEST THE POISON WATER QUEST!!!!
	AddMonsterType(MT_GOLEM, MPFLAG_DONT);
	AddMonster(1, 0, 0, 0, FALSE);
	AddMonster(1, 0, 0, 0, FALSE);
	AddMonster(1, 0, 0, 0, FALSE);
	AddMonster(1, 0, 0, 0, FALSE);

	if ((setlevel) && (setlvlnum == SL_VILEBETRAYER)) {
		AddMonsterType(UniqMonst[MU_LAZARUS].mtype, MPFLAG_UNIQ);
		AddMonsterType(UniqMonst[MU_REDVEX].mtype, MPFLAG_UNIQ);
		AddMonsterType(UniqMonst[MU_BLKJADE].mtype, MPFLAG_UNIQ);
		PlaceUniqueMonst(MU_LAZARUS, 0, 0);
		PlaceUniqueMonst(MU_REDVEX, 0, 0);
		PlaceUniqueMonst(MU_BLKJADE, 0, 0);
	}


	lm = (WORD *)pMap;
	rw = *(lm++);
	rh = *(lm++);
	// Skip map
	lm += rw * rh;
	// Convert to index mini tile level instead of mega
	rw = rw << 1;
	rh = rh << 1;
	// Skip treasure map
	lm += rw * rh;
	for (j = 0; j < rh; ++j) {
		for (i = 0; i < rw; ++i) {
			if (*lm != 0) {
				mt = *lm;
				mt = MonstConvTbl[mt-1];
				mtype = AddMonsterType(mt, MPFLAG_DONT);
				mx = i + DIRTEDGED2 + startx;
				my = j + DIRTEDGED2 + starty;
				PlaceMonster(nummonsters++, mtype, mx, my);
			}
			++lm;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void DeleteMonster(int i)
{
	int temp;

	app_assert((DWORD)(nummonsters-1) < MAXMONSTERS);
	app_assert((DWORD)i < MAXMONSTERS);
	temp = monstactive[--nummonsters];
	monstactive[nummonsters] = monstactive[i];
	monstactive[i] = temp;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int AddMonster(int x, int y, int dir, int mtype, BOOL InMap)
{
	int i;

	if (nummonsters < MAXMONSTERS) {
		i = monstactive[nummonsters++];
		if (InMap) dMonster[x][y] = i + 1;
		InitMonster(i, dir, mtype, x, y);
		return i;
	} else return -1;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void CloneMonster(int m)
{
	if (monster[m].MType == NULL)
		return;
	int oldx = monster[m]._mx;
	int oldy = monster[m]._my;
	int dir = monster[m]._mdir;
	
	// try to find a place to put a new monster, next to old monster
	int x, y, theta;
	
	for (theta = 0; theta < 8; ++theta)
	{
		x = oldx + offset_x[theta];
		y = oldy + offset_y[theta];
		
		if (SolidLoc(x, y) || dPlayer[x][y] || dMonster[x][y])
			continue;
			
		if (!dObject[x][y])
			break;
			
		int oi = (dObject[x][y] > 0) ? (dObject[x][y] - 1) : -(dObject[x][y] + 1);
		if (!object[oi]._oSolidFlag)
			break;
	}
	if (theta >= 8)		// no space
		return;
	
	int mtype = monster[m].MType->mtype;
	for (int i=0; i < MAX_LVLMTYPES; ++i)
	{
		if (Monsters[i].mtype == mtype)
			break;
	}
	if (i < MAX_LVLMTYPES)
		AddMonster(x, y, dir, i, TRUE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void NewMonsterAnim(int i, AnimStruct &anim, int md)
{
	app_assert((DWORD)i < MAXMONSTERS);
	app_assert((DWORD)md < 8);
	MonsterStruct	*Monst = &monster[i];

	Monst->_mAnimData = anim.Cels[md];
	Monst->_mAnimLen = anim.Frames;
	Monst->_mAnimFrame = 1;
	Monst->_mAnimCnt = 0;
	Monst->_mAnimDelay = anim.Rate;
	Monst->_mdir = md;
	Monst->_mFlags &= ~(MFLAG_BACKWARDS|MFLAG_STILL);
}
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
BOOL M_Ranged (int i)
{
	app_assert((DWORD)i < MAXMONSTERS);
	if (monster[i]._mAi == AI_SKELBOW	||
		monster[i]._mAi == AI_GOATBOW	||
		monster[i]._mAi == AI_SUCC		||
		monster[i]._mAi == AI_LAZHELP) return TRUE;
	
	return FALSE;
}
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
BOOL M_Talker (int i)
{
	app_assert((DWORD)i < MAXMONSTERS);
	if (monster[i]._mAi == AI_LAZURUS	||
		monster[i]._mAi == AI_WARLORD	||
		monster[i]._mAi == AI_GARBUD	||
		monster[i]._mAi == AI_ZHAR		||
		monster[i]._mAi == AI_SNOTSPIL	||
		monster[i]._mAi == AI_LACHDANAN	||
		monster[i]._mAi == AI_LAZHELP) return TRUE;
	
	return FALSE;
}
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void M_Enemy (int i)
{
	int j;
	int pnum, closest;
	int bestdist;
	bool bestsameroom;

	app_assert((DWORD)i < MAXMONSTERS);
	MonsterStruct	*Monst = &monster[i];
	BYTE enemyx,enemyy;

	closest = -1;
	bestdist = -1;
	bestsameroom = false;

	
	if (Monst->_mFlags & MFLAG_BERSERK 
	    || !(Monst->_mFlags & MFLAG_MKILLER) ) {
		for(pnum = 0; pnum < MAX_PLRS; ++pnum) {
			if (!plr[pnum].plractive
				|| currlevel != plr[pnum].plrlevel
				|| plr[pnum]._pLvlChanging
				|| ((plr[pnum]._pHitPoints >> HP_SHIFT) == 0) )
				continue;

			bool const sameroom = (dTransVal[Monst->_mx][Monst->_my] == dTransVal[plr[pnum]._px][plr[pnum]._py]);
			int const dist = max(abs(Monst->_mx - plr[pnum]._px), abs(Monst->_my - plr[pnum]._py));

			if ((sameroom && !bestsameroom) || ((sameroom || !bestsameroom) && dist < bestdist) || closest == -1) {
				Monst->_mFlags &= ~MFLAG_MID;
//				Monst->_mFlags &= ~MFLAG_MKILLER;
				closest = pnum;
				enemyx = plr[pnum]._pfutx;
				enemyy = plr[pnum]._pfuty;
				bestdist = dist;
				bestsameroom = sameroom;
			}
		}
	}

	app_assert((DWORD)nummonsters <= MAXMONSTERS);
	for (j = 0; j < nummonsters; ++j) {
		
		int const mi = monstactive[j];
		
		if ((mi != i) &&
			((monster[mi]._mhitpoints >> HP_SHIFT) > 0) &&
			((monster[mi]._mx != 1) || (monster[mi]._my != 0)) &&		// special confus-o-matic code for inactive golum
			((!M_Talker(mi)) || monster[mi].mtalkmsg == 0) &&
			/*(DIST((monster[mi]._mx - Monst->_mx),(monster[mi]._my - Monst->_my),2)) &&*/
			((Monst->_mFlags & MFLAG_MKILLER) || (Monst->_mFlags & MFLAG_BERSERK) ||
				((DIST((monster[mi]._mx - Monst->_mx),(monster[mi]._my - Monst->_my),2)) || (M_Ranged(i)))) &&
			(Monst->_mFlags & MFLAG_MKILLER || (Monst->_mFlags & MFLAG_BERSERK)
				|| monster[mi]._mFlags & MFLAG_MKILLER)) {
			
			bool const sameroom = (dTransVal[Monst->_mx][Monst->_my] == dTransVal[monster[mi]._mx][monster[mi]._my]);
			int const dist = max(abs(Monst->_mx - monster[mi]._mx), abs(Monst->_my - monster[mi]._my));
			
			if ((sameroom && !bestsameroom) || ((sameroom || !bestsameroom) && dist < bestdist) || closest == -1) {
				Monst->_mFlags |= MFLAG_MID;
				closest = mi;
				enemyx = monster[mi]._mfutx;
				enemyy = monster[mi]._mfuty;
				bestdist = dist;
				bestsameroom = sameroom;
			}
		}
	}

	// everyone dead?
	if (closest != -1)
	{
		Monst->_menemy = closest;
		Monst->_menemyx = enemyx;
		Monst->_menemyy = enemyy;
		Monst->_mFlags &= ~MFLAG_NOENEMY;
	}
	else
		Monst->_mFlags |= MFLAG_NOENEMY;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static int M_GetDir(int i)
{
	app_assert((DWORD)i < MAXMONSTERS);
	return GetDirection(monster[i]._mx, monster[i]._my, monster[i]._menemyx, monster[i]._menemyy);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void M_CheckEFlag(int i)
{
	int	tx,ty,tv;
	int t;
	WORD *mt;

	app_assert((DWORD)i < MAXMONSTERS);
	tx = monster[i]._mx - 1;
	ty = monster[i]._my + 1;
	tv = 0;
	mt = &(dMT[tx][ty].mt[0]);
	for(t = 2; t < 10; ++t)
		tv |= mt[t];
	tv |= dSpecial[tx][ty];
	if (tv != 0) monster[i]._meflag = 1;
	else monster[i]._meflag = 0;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void M_StartStand(int i, int md)
{
	app_assert((DWORD)i < MAXMONSTERS);
	app_assert(monster[i].MType != NULL);
	ClearMVars(i);
	if (monster[i].MType->mtype == MT_GOLEM)
		NewMonsterAnim(i, monster[i].MType->Anims[MA_WALK], md);
	else
		NewMonsterAnim(i, monster[i].MType->Anims[MA_STAND], md);

	monster[i]._mVar1 = monster[i]._mmode;
	monster[i]._mVar2 = 0;			// count how long he's been standing
	monster[i]._mmode = MM_STAND;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mdir = md;
	M_CheckEFlag(i);

	// Pick nearest enemy
	M_Enemy(i);
}

/*-----------------------------------------------------------------------*
 *	M_StartDelay
 *
 *	Keeps monster in Stand animation for len frames
 *  Monster must be in Stand mode before entering Delay mode
**-----------------------------------------------------------------------*/

static void M_StartDelay(int i, int len)
{
	if(len > 0)
	{
		if (monster[i]._mAi == AI_LAZURUS) return;
		app_assert((DWORD)i < MAXMONSTERS);
		monster[i]._mVar2 = len;
		monster[i]._mmode = MM_DELAY;
	}
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void M_StartSpStand(int i, int md)
{
	app_assert((DWORD)i < MAXMONSTERS);
	app_assert(monster[i].MType != NULL);
	NewMonsterAnim(i, monster[i].MType->Anims[MA_SPECIAL], md);

	monster[i]._mmode = MM_SPSTAND;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mdir = md;
	M_CheckEFlag(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void M_StartWalk(int i, int xvel, int yvel, int xadd, int yadd, int EndDir)
{
	long	fx,fy;
	int		pn;

	app_assert((DWORD)i < MAXMONSTERS);
	app_assert(monster[i].MType != NULL);
	fx = monster[i]._mx + xadd;
	fy = monster[i]._my + yadd;
	app_assert((DWORD)fx < MAXDUNX);
	app_assert((DWORD)fy < MAXDUNY);
	pn = dPiece[fx][fy] - 1;
	dMonster[fx][fy] = -1 - i;
	monster[i]._mmode = MM_WALK;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mfutx = fx;
	monster[i]._mfuty = fy;
	monster[i]._mxvel = xvel;
	monster[i]._myvel = yvel;
	monster[i]._mVar1 = xadd;
	monster[i]._mVar2 = yadd;
	monster[i]._mVar3 = EndDir;
	monster[i]._mdir = EndDir;
	NewMonsterAnim(i, monster[i].MType->Anims[MA_WALK], EndDir);
	monster[i]._mVar6 = 0;
	monster[i]._mVar7 = 0;
	monster[i]._mVar8 = 0;
	M_CheckEFlag(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void M_StartWalk2(int i, int xvel, int yvel, int xoff, int yoff,
					int xadd, int yadd, int EndDir)
{
	long	fx,fy;
	int		pn;

	app_assert((DWORD)i < MAXMONSTERS);
	app_assert(monster[i].MType != NULL);
	fx = monster[i]._mx + xadd;
	fy = monster[i]._my + yadd;
	app_assert((DWORD)fx < MAXDUNX);
	app_assert((DWORD)fy < MAXDUNY);
	pn = dPiece[fx][fy] - 1;
	app_assert((DWORD)monster[i]._mx < MAXDUNX);
	app_assert((DWORD)monster[i]._my < MAXDUNY);
	dMonster[monster[i]._mx][monster[i]._my] = -1 - i;
	monster[i]._mVar1 = monster[i]._mx;
	monster[i]._mVar2 = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mx = fx;
	monster[i]._my = fy;
	monster[i]._mfutx = fx;
	monster[i]._mfuty = fy;
	dMonster[fx][fy] = i + 1;
	if( !(monster[i]._mFlags & MFLAG_INVISIBLE)
		&& monster[i].mlid != 0) {
		ChangeLightXY(monster[i].mlid, monster[i]._mx, monster[i]._my);
	}
	monster[i]._mxoff = xoff;
	monster[i]._myoff = yoff;
	monster[i]._mmode = MM_WALK2;
	monster[i]._mxvel = xvel;
	monster[i]._myvel = yvel;
	monster[i]._mVar3 = EndDir;
	monster[i]._mdir = EndDir;
	NewMonsterAnim(i, monster[i].MType->Anims[MA_WALK], EndDir);
	monster[i]._mVar6 = xoff << 4;
	monster[i]._mVar7 = yoff << 4;
	monster[i]._mVar8 = 0;
	M_CheckEFlag(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void M_StartWalk3(int i, int xvel, int yvel, int xoff, int yoff,
					int xadd, int yadd, int txa, int tya, int EndDir)
{
	long	fx,fy;
	long	tx,ty;
	int		pn, pn2;

	app_assert((DWORD)i < MAXMONSTERS);
	app_assert(monster[i].MType != NULL);
	fx = monster[i]._mx + xadd;
	fy = monster[i]._my + yadd;
	tx = monster[i]._mx + txa;			// Temp location for drawing
	ty = monster[i]._my + tya;
	if( !(monster[i]._mFlags & MFLAG_INVISIBLE)
		&& monster[i].mlid != 0) {
		ChangeLightXY(monster[i].mlid, tx, ty);
	}
	app_assert((DWORD)fx < MAXDUNX);
	app_assert((DWORD)fy < MAXDUNY);
	pn = dPiece[fx][fy] - 1;
	pn2 = dPiece[tx][ty] - 1;
	app_assert((DWORD)monster[i]._mx < MAXDUNX);
	app_assert((DWORD)monster[i]._my < MAXDUNY);
	dMonster[monster[i]._mx][monster[i]._my] = -1 - i;
	dMonster[fx][fy] = -1 - i;
	monster[i]._mVar4 = tx;
	monster[i]._mVar5 = ty;
	dFlags[tx][ty] |= BFLAG_MONSTLR;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mfutx = fx;
	monster[i]._mfuty = fy;
	monster[i]._mxoff = xoff;
	monster[i]._myoff = yoff;
	monster[i]._mmode = MM_WALK3;
	monster[i]._mxvel = xvel;
	monster[i]._myvel = yvel;
	monster[i]._mVar1 = fx;
	monster[i]._mVar2 = fy;
	monster[i]._mVar3 = EndDir;
	monster[i]._mdir = EndDir;
	NewMonsterAnim(i, monster[i].MType->Anims[MA_WALK], EndDir);
	monster[i]._mVar6 = xoff << 4;
	monster[i]._mVar7 = yoff << 4;
	monster[i]._mVar8 = 0;
	M_CheckEFlag(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void M_StartAttack(int i)
{
	int		md;

	app_assert((DWORD)i < MAXMONSTERS);
	app_assert(monster[i].MType != NULL);
	md = M_GetDir(i);
	NewMonsterAnim(i, monster[i].MType->Anims[MA_ATTACK], md);

	monster[i]._mmode = MM_ATTACK;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mdir = md;
	M_CheckEFlag(i);
}

/*-----------------------------------------------------------------------*
** Ranged weapons
**-----------------------------------------------------------------------*/

static void M_StartRAttack(int i, int missile_type, int dam)
{
	int		md;

	app_assert((DWORD)i < MAXMONSTERS);
	app_assert(monster[i].MType != NULL);
	md = M_GetDir(i);
	NewMonsterAnim(i, monster[i].MType->Anims[MA_ATTACK], md);

	monster[i]._mmode = MM_RATTACK;
	monster[i]._mVar1 = missile_type;
	monster[i]._mVar2 = dam;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mdir = md;
	M_CheckEFlag(i);
}

/*-----------------------------------------------------------------------*
** Ranged Special weapons
**-----------------------------------------------------------------------*/

static void M_StartRSpAttack(int i, int missile_type, int dam)
{
	int		md;

	app_assert((DWORD)i < MAXMONSTERS);
	app_assert(monster[i].MType != NULL);
	md = M_GetDir(i);
	NewMonsterAnim(i, monster[i].MType->Anims[MA_SPECIAL], md);

	monster[i]._mmode = MM_RSATTACK;
	monster[i]._mVar1 = missile_type;
	monster[i]._mVar2 = 0;
	monster[i]._mVar3 = dam;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mdir = md;
	M_CheckEFlag(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void M_StartSpAttack(int i)
{
	int		md;

	app_assert((DWORD)i < MAXMONSTERS);
	app_assert(monster[i].MType != NULL);
	md = M_GetDir(i);
	NewMonsterAnim(i, monster[i].MType->Anims[MA_SPECIAL], md);

	monster[i]._mmode = MM_SATTACK;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mdir = md;
	M_CheckEFlag(i);
}

/*-----------------------------------------------------------------------*
 * M_StartEat is the same as M_StartSpAttack except mdir isn't affected
**-----------------------------------------------------------------------*/

static void M_StartEat(int i)
{
	app_assert((DWORD)i < MAXMONSTERS);
	app_assert(monster[i].MType != NULL);
	NewMonsterAnim(i, monster[i].MType->Anims[MA_SPECIAL], monster[i]._mdir);

	monster[i]._mmode = MM_SATTACK;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	M_CheckEFlag(i);
}
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void M_ClearSquares(int i)
{
	app_assert((DWORD)i < MAXMONSTERS);
	int const mx = monster[i]._moldx;
	int const my = monster[i]._moldy;
	int const mt = -1 - i;
	int const mt2 = i + 1;
	app_assert((DWORD)(mx+1) < MAXDUNX);
	app_assert((DWORD)(my+1) < MAXDUNY);
	for (int y = my-1; y <= my+1; ++y) {
		for (int x = mx-1; x <= mx+1; ++x) {
			if (dMonster[x][y] == mt || dMonster[x][y] == mt2) {
				dMonster[x][y] = 0;
			}
		}
	}

	dFlags[mx+1][my+0] &= BFMASK_MONSTLR;
	dFlags[mx+0][my+1] &= BFMASK_MONSTLR;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void M_GetKnockback(int i)
{
	app_assert((DWORD)i < MAXMONSTERS);
	app_assert(monster[i].MType != NULL);
	// knock back enemy if you can
	int const d = (monster[i]._mdir + 4) & 0x7;
	if (DirOK(i, d)) {
		M_ClearSquares(i);
		monster[i]._moldx += offset_x[d];
		monster[i]._moldy += offset_y[d];
		NewMonsterAnim(i, monster[i].MType->Anims[MA_GOTHIT], monster[i]._mdir);
		monster[i]._mmode = MM_GOTHIT;
		monster[i]._mxoff = 0;
		monster[i]._myoff = 0;
		monster[i]._mx = monster[i]._moldx;
		monster[i]._my = monster[i]._moldy;
		monster[i]._mfutx = monster[i]._mx;
		monster[i]._mfuty = monster[i]._my;
		monster[i]._moldx = monster[i]._mx;
		monster[i]._moldy = monster[i]._my;
		M_CheckEFlag(i);
		M_ClearSquares(i);
		app_assert((DWORD)monster[i]._mx < MAXDUNX);
		app_assert((DWORD)monster[i]._my < MAXDUNY);
		dMonster[monster[i]._mx][monster[i]._my] = i + 1;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void M_StartHit(int i, int pnum, int dam)
{
	app_assert((DWORD)i < MAXMONSTERS);
	app_assert(monster[i].MType != NULL);
	if (pnum >= 0) monster[i].mWhoHit |= (1 << pnum);		// Who damaged me

	if (pnum == myplr) {
		extern void delta_monster_hp(int mi,long hp,BYTE bLevel);
		delta_monster_hp(i,monster[i]._mhitpoints,currlevel);
		NetSendCmdMonstDamage(FALSE,i,dam);
	}
	PlayEffect(i, MS_GOTHIT);

	// Sneaky demons always get hit
	if (!EquivMonst(monster[i].MType->mtype, MT_SNEAK))
		// Other monsters only get hit if damage is severe
		//if (((dam >> HP_SHIFT) < (monster[i].mLevel + 3)) && (!(plr[pnum]._pIFlags & IAF_KNOCKBACK)))
		if ((dam >> HP_SHIFT) < (monster[i].mLevel + 3))
			return;

	// Set monster's enemy to be this player
	if (pnum >= 0) {
		monster[i]._menemy = pnum;
		monster[i]._menemyx = plr[pnum]._pfutx;
		monster[i]._menemyy = plr[pnum]._pfuty;
		monster[i]._mFlags &= ~MFLAG_MID;
		monster[i]._mdir = M_GetDir(i);
	}

	// Check for special hits/teleports
	if(monster[i].MType->mtype == MT_BLINK) {
		M_Teleport(i);
	} else {
		if (EquivMonst(monster[i].MType->mtype, MT_NSCAV) ||
			monster[i].MType->mtype == MT_GRAVDG) {
			// allow it to seek food again.
			monster[i]._mgoal = MG_ATTACK;
			monster[i]._mgoalvar1 = 0;
			monster[i]._mgoalvar2 = 0;
		}
	}
	if (monster[i]._mmode != MM_STONE) {
		NewMonsterAnim(i, monster[i].MType->Anims[MA_GOTHIT], monster[i]._mdir);
		monster[i]._mmode = MM_GOTHIT;
		monster[i]._mxoff = 0;
		monster[i]._myoff = 0;
		monster[i]._mx = monster[i]._moldx;
		monster[i]._my = monster[i]._moldy;
		monster[i]._mfutx = monster[i]._mx;
		monster[i]._mfuty = monster[i]._my;
		monster[i]._moldx = monster[i]._mx;
		monster[i]._moldy = monster[i]._my;
		M_CheckEFlag(i);
		M_ClearSquares(i);
		dMonster[monster[i]._mx][monster[i]._my] = i + 1;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void M_DiabloDeath(int i, BOOL sendmsg)
{
	MonsterStruct	*Monst = &monster[i];

	// disable any other sounds from playing, for dramatic effect
	#if !IS_VERSION(SHAREWARE)
	PlaySFX(USFX_DIABLOD);
	#endif
	quests[Q_DIABLO]._qactive = QUEST_DONE;
	if (sendmsg) NetSendCmdQuest(TRUE, Q_DIABLO);
	sgbSaveSoundOn = gbSoundOn;
	gbSoundOn = FALSE;
	// disable player processing, so we can take over screen scrolling
	gbProcessPlayers = FALSE;
	// kill all other monsters on level
	app_assert((DWORD)nummonsters <= MAXMONSTERS);
	for (int j = 0; j < nummonsters; ++j) {
		int const k = monstactive[j];
		if (k != i && monster[i]._msquelch) {
			NewMonsterAnim(k, monster[k].MType->Anims[MA_DEATH], monster[k]._mdir);
			monster[k]._mmode = MM_DEATH;
			monster[k]._mxoff = 0;
			monster[k]._myoff = 0;
			monster[k]._mVar1 = 0;
			monster[k]._mx = monster[k]._moldx;
			monster[k]._my = monster[k]._moldy;
			monster[k]._mfutx = monster[k]._mx;
			monster[k]._mfuty = monster[k]._my;
			monster[k]._moldx = monster[k]._mx;
			monster[k]._moldy = monster[k]._my;
			M_CheckEFlag(k);
			M_ClearSquares(k);
			dMonster[monster[k]._mx][monster[k]._my] = k + 1;
		}
	}
	AddLight(Monst->_mx, Monst->_my, 8);
	DoVision(Monst->_mx, Monst->_my, 8, FALSE, TRUE);
	// set up scrolling vars
	int steps = max(abs(ViewX-Monst->_mx),abs(ViewY-Monst->_my));
	steps = min(20, steps);

	Monst->_mVar3 = ViewX << 16;			// convert to fixed-point
	Monst->_mVar4 = ViewY << 16;
											// calculate scroll offsets
	Monst->_mVar5 = (int)((double)(Monst->_mVar3 - (Monst->_mx << 16))/(double)steps);
	Monst->_mVar6 = (int)((double)(Monst->_mVar4 - (Monst->_my << 16))/(double)steps);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void M2MStartHit(int mid, int i, int dam)
{
	if ((DWORD)mid >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("Invalid monster %d getting hit by monster",mid);
#else
		return;
#endif
	}
	if (monster[mid].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("Monster %d \"%s\" getting hit by monster: MType NULL",mid,monster[mid].mName);
#else
		return;
#endif
	}

	if (i >= 0) monster[i].mWhoHit |= (1 << i);		// Who damaged me

	extern void delta_monster_hp(int mi,long hp, BYTE bLevel);
	delta_monster_hp(mid, monster[mid]._mhitpoints, currlevel);
	NetSendCmdMonstDamage(FALSE, mid, dam);

	PlayEffect(mid, MS_GOTHIT);

	// Sneaky demons always get hit
	if (!EquivMonst(monster[mid].MType->mtype, MT_SNEAK))
		// Other monsters only get hit if damage is severe
		if ((dam >> HP_SHIFT) < (monster[mid].mLevel + 3)) return;

	// face plr who hit me
	if (i >= 0) monster[mid]._mdir = (monster[i]._mdir + 4) &0x07;

	// Check for special hits/teleports
	if(monster[mid].MType->mtype == MT_BLINK) {
		M_Teleport(mid);
	} else {
		if (EquivMonst(monster[mid].MType->mtype, MT_NSCAV) ||
			monster[mid].MType->mtype == MT_GRAVDG) {
			// allow it to seek food again.
			monster[mid]._mgoal = MG_ATTACK;
			monster[mid]._mgoalvar1 = 0;
			monster[mid]._mgoalvar2 = 0;
		}
	}

	if (monster[mid]._mmode != MM_STONE) {
		if (monster[mid].MType->mtype != MT_GOLEM) {
			NewMonsterAnim(mid, monster[mid].MType->Anims[MA_GOTHIT], monster[mid]._mdir);
			monster[mid]._mmode = MM_GOTHIT;
		}

		monster[mid]._mxoff = 0;
		monster[mid]._myoff = 0;
		monster[mid]._mx = monster[mid]._moldx;
		monster[mid]._my = monster[mid]._moldy;
		monster[mid]._mfutx = monster[mid]._mx;
		monster[mid]._mfuty = monster[mid]._my;
		monster[mid]._moldx = monster[mid]._mx;
		monster[mid]._moldy = monster[mid]._my;
		M_CheckEFlag(mid);
		M_ClearSquares(mid);
		dMonster[monster[mid]._mx][monster[mid]._my] = mid + 1;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void MonstKillEffect(int i, BOOL sendmsg)
{
	MonsterStruct	*Monst = &monster[i];
	
	if (QuestStatus(Q_GARBUD) && (Monst->mName == UniqMonst[MU_GARBUD].mName))
		CreateTypeItem(Monst->_mx+1, Monst->_my+1, TRUE, IT_MACE, IMID_NONE, TRUE, FALSE);
	else if (Monst->mName == UniqMonst[MU_DEFILER].mName)	
	{
		if (effect_is_playing(HSFX_DEFILER5))
			stream_stop();
		
		quests[Q_DEFILER]._qlog = FALSE;
		SpawnMap(Monst->_mx, Monst->_my);
	}
	else if (Monst->mName == UniqMonst[MU_HORKDEMON].mName)	
	{
		if (gbTheo)
			SpawnBear(Monst->_mx, Monst->_my);
		else
			CreateAmulet(Monst->_mx, Monst->_my, 13, FALSE, TRUE);
	}
	else if (Monst->MType->mtype == MT_SPAWN)
	{
		// do nothing
	}
	else if (Monst->MType->mtype == MT_NKR)
	{
		int whichEffect =  (Na_Krul.Books)?HSFX_NA_KRUL4:HSFX_NA_KRUL5;
		if (gbCowsuit)		// override -- funny
			whichEffect = HSFX_NA_KRUL6;
			
		if (effect_is_playing(whichEffect))
			stream_stop();
		
		quests[Q_NA_KRUL]._qlog = FALSE;
		Na_Krul.MIndex = -2;
		CreateMagicWeapon(Monst->_mx, Monst->_my, IT_SWORD, ITEM_GRTSWORD, FALSE, TRUE);
		CreateMagicWeapon(Monst->_mx, Monst->_my, IT_STAFF, ITEM_STLSTAFF, FALSE, TRUE);
		CreateMagicWeapon(Monst->_mx, Monst->_my, IT_BOW, ITEM_STLLONGBOW, FALSE, TRUE);	
		CreateSpellBook  (Monst->_mx, Monst->_my, SPL_APOCA, FALSE, TRUE);
	}
	else if (i > 3)
	{
		SpawnItem(i, Monst->_mx, Monst->_my, sendmsg);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void MonstStartKill(int i, int pnum, BOOL sendmsg)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined (_DEBUG)
		app_fatal("MonstStartKill: Invalid monster %d",i);
#else
		return;
#endif
	}
	
	if (monster[i].MType == NULL)
	{
		//app_fatal("MonstStartKill: Monster %d \"%s\" MType NULL",i,monster[i].mName);
		return;
	}
	
	int		md;
	MonsterStruct	*Monst = &monster[i];

	// killer gets credit too, and divvy up exper
	if (pnum >= 0) Monst->mWhoHit |= (1 << pnum);		// Who killed me
	
	// rjs.patch1.start.1/23/97 - fixes golem giving exp when killed
	//old. if (pnum < MAX_PLRS) AddPlrMonstExper(Monst->mLevel, Monst->mExp, Monst->mWhoHit);
	if ((pnum < MAX_PLRS) && (i > MAX_PLRS)) AddPlrMonstExper(Monst->mLevel, Monst->mExp, Monst->mWhoHit);
	// rjs.patch1.end.1/23/97

	// Kill the monster (for myplr or other plr)
	++monstkills[Monst->MType->mtype];
	Monst->_mhitpoints = 0;
	SetRndSeed(Monst->_mRndSeed);
	MonstKillEffect(i, sendmsg);

#if 0	
	if (QuestStatus(Q_GARBUD) && (Monst->mName == UniqMonst[MU_GARBUD].mName))
		CreateTypeItem(Monst->_mx+1, Monst->_my+1, TRUE, IT_MACE, IMID_NONE, TRUE, FALSE);
	else if (Monst->mName == UniqMonst[MU_DEFILER].mName)	
	{
		quests[Q_DEFILER]._qlog = FALSE;
		SpawnMap(Monst->_mx, Monst->_my);
	}
	else if (Monst->mName == UniqMonst[MU_HORKDEMON].mName)	
		if (gbTheo)
			SpawnBear(Monst->_mx, Monst->_my);
	else if (Monst->MType->mtype == MT_SPAWN)
	{
		// do nothing
	}
	else if (Monst->MType->mtype == MT_NKR)
	{
		quests[Q_NA_KRUL]._qlog = FALSE;
	}
	else if (i > 3) SpawnItem(i, Monst->_mx, Monst->_my, sendmsg);
#endif

	if (Monst->MType->mtype == MT_DIABLO) {
		M_DiabloDeath(i, TRUE);
	}
	else
		PlayEffect(i, MS_DEATH);

	if (pnum >= 0) md = M_GetDir(i);
	else md = Monst->_mdir;
	Monst->_mdir = md;
	NewMonsterAnim(i, Monst->MType->Anims[MA_DEATH], md);
	Monst->_mmode = MM_DEATH;
	Monst->_mgoal = 0;
	Monst->_mxoff = 0;
	Monst->_myoff = 0;
	Monst->_mVar1 = 0;
	Monst->_mx = Monst->_moldx;
	Monst->_my = Monst->_moldy;
	Monst->_mfutx = Monst->_mx;
	Monst->_mfuty = Monst->_my;
	Monst->_moldx = Monst->_mx;
	Monst->_moldy = Monst->_my;
	M_CheckEFlag(i);
	M_ClearSquares(i);
	dMonster[Monst->_mx][Monst->_my] = i + 1;
	CheckQuestKill(i, sendmsg);
	// Send Fallen Ones running in fear
	M_FallenFear(Monst->_mx, Monst->_my);
	// Acid demon emits acid pool
	if ((EquivMonst(Monst->MType->mtype, MT_NACID)) ||
		(Monst->MType->mtype == MT_SPIDER2))
		AddMissile(Monst->_mx, Monst->_my, 0, 0, 0, MIT_ACIDPUD, MI_ENEMYPLR, i, Monst->_mint + 1, 0);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void M2MStartKill(int i, int mid)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M2MStartKill: Invalid monster (attacker) %d",i);
#else
		return;
#endif
	}
	if ((DWORD)mid >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M2MStartKill: Invalid monster (killed) %d",mid);
#else
		return;
#endif
	}
	if (monster[i].MType == NULL)
	{
		//app_fatal("M2MStartKill: Monster %d \"%s\" MType NULL",mid,monster[mid].mName);
		return;
	}
	
	int		md;

	void delta_kill_monster(int mi, BYTE x, BYTE y, BYTE bLevel);
	delta_kill_monster(mid, monster[mid]._mx, monster[mid]._my, currlevel);
	NetSendCmdLocParam1(FALSE, CMD_MONSTDEATH, monster[mid]._mx, monster[mid]._my, mid);

	monster[mid].mWhoHit |= (1 << i);		// Who killed me
	if (i < MAX_PLRS) AddPlrMonstExper(monster[mid].mLevel, monster[mid].mExp, monster[mid].mWhoHit);
	
	++monstkills[monster[mid].MType->mtype];
	monster[mid]._mhitpoints = 0;
	SetRndSeed(monster[mid]._mRndSeed);
	
	MonstKillEffect(mid, TRUE);
#if 0
	// @@@ needs to be fixed for multiplayer (death not synced, etc)
	if (mid >= MAX_PLRS) SpawnItem(mid, monster[mid]._mx, monster[mid]._my, TRUE);
#endif	
	
	if (monster[mid].MType->mtype == MT_DIABLO) {
		M_DiabloDeath(mid, TRUE);
	}
	else
		PlayEffect(mid, MS_DEATH);
	//PlayEffect(mid, MS_DEATH);
	md = (monster[i]._mdir + 4) & 0x07;
	if (monster[mid].MType->mtype == MT_GOLEM) md = 0;
	monster[mid]._mdir = md;
	NewMonsterAnim(mid, monster[mid].MType->Anims[MA_DEATH], md);
	monster[mid]._mmode = MM_DEATH;
	monster[mid]._mxoff = 0;
	monster[mid]._myoff = 0;
	monster[mid]._mx = monster[mid]._moldx;
	monster[mid]._my = monster[mid]._moldy;
	monster[mid]._mfutx = monster[mid]._mx;
	monster[mid]._mfuty = monster[mid]._my;
	monster[mid]._moldx = monster[mid]._mx;
	monster[mid]._moldy = monster[mid]._my;
	M_CheckEFlag(mid);
	M_ClearSquares(mid);
	dMonster[monster[mid]._mx][monster[mid]._my] = mid + 1;
	CheckQuestKill(mid, TRUE);
	
	// Send Fallen Ones running in fear
	M_FallenFear(monster[mid]._mx, monster[mid]._my);
	// Acid demon emits acid pool
	if(EquivMonst(monster[mid].MType->mtype, MT_NACID))
		AddMissile(monster[mid]._mx, monster[mid]._my, 0, 0, 0, MIT_ACIDPUD, MI_ENEMYPLR, mid, monster[mid]._mint + 1, 0);
		
	M_StartStand(i, monster[i]._mdir);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void M_StartKill(int i, int pnum)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined (_DEBUG)
		app_fatal("M_StartKill: Invalid monster %d",i);
#else
		return;
#endif
	}
	// Send a message to everyone saying I killed the monster
	if (myplr == pnum) {
		void delta_kill_monster(int mi, BYTE x, BYTE y, BYTE bLevel);
		delta_kill_monster(i,monster[i]._mx,monster[i]._my,currlevel);
		if (i != pnum)
			NetSendCmdLocParam1(FALSE,CMD_MONSTDEATH,monster[i]._mx,monster[i]._my,i);
		else
			NetSendCmdLocParam1(FALSE,CMD_KILLGOLEM,monster[i]._mx,monster[i]._my,currlevel);
	}

	MonstStartKill(i, pnum, TRUE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void M_SyncStartKill(int i, int x, int y, int pnum)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_SyncStartKill: Invalid monster %d",i);
#else
		return;
#endif
	}
	// Already dead?
	if (monster[i]._mhitpoints == 0) return;
	if (monster[i]._mmode == MM_DEATH) return;

	app_assert(pnum != myplr);

	if (!dMonster[x][y]) {
		M_ClearSquares(i);
		monster[i]._mx = x;
		monster[i]._my = y;
		monster[i]._moldx = x;
		monster[i]._moldy = y;
	}
	MonstStartKill(i, pnum, FALSE);
//	app_assert(dMonster[monster[i]._mx][monster[i]._my] == i+1
//				|| dMonster[monster[i]._mx][monster[i]._my] == -(i+1));
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void M_StartFadein(int i, int md, BOOL backwards)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_StartFadein: Invalid monster %d",i);
#else
		return;
#endif
	}
	
	if (monster[i].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_StartFadein: Monster %d \"%s\" MType NULL",i,monster[i].mName);
#else
		return;
#endif
	}
	
	NewMonsterAnim(i, monster[i].MType->Anims[MA_SPECIAL], md);

	monster[i]._mmode = MM_FADEIN;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	M_CheckEFlag(i);
	monster[i]._mdir = md;
	monster[i]._mFlags &= ~MFLAG_INVISIBLE;
	if (backwards)
	{
		monster[i]._mFlags |= MFLAG_BACKWARDS;
		monster[i]._mAnimFrame = monster[i]._mAnimLen;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void M_StartFadeout(int i, int md, BOOL backwards)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_StartFadeout: Invalid monster %d",i);
#else
		return;
#endif
	}
	if (monster[i].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_StartFadeout: Monster %d \"%s\" MType NULL",i,monster[i].mName);
#else
		return;
#endif
	}
	NewMonsterAnim(i, monster[i].MType->Anims[MA_SPECIAL], md);
	monster[i]._mmode = MM_FADEOUT;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	M_CheckEFlag(i);
	monster[i]._mdir = md;
	if (backwards)
	{
		monster[i]._mFlags |= MFLAG_BACKWARDS;
		monster[i]._mAnimFrame = monster[i]._mAnimLen;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void M_StartHeal(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_StartHeal: Invalid monster %d",i);
#else
		return;
#endif
	}
	if (monster[i].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_StartHeal: Monster %d \"%s\" MType NULL",i,monster[i].mName);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];

	Monst->_mAnimData = Monst->MType->Anims[MA_SPECIAL].Cels[Monst->_mdir];
	Monst->_mAnimFrame = Monst->MType->Anims[MA_SPECIAL].Frames;
	Monst->_mFlags |= MFLAG_BACKWARDS;
	Monst->_mmode = MM_HEAL;
	Monst->_mVar1 = Monst->_mmaxhp / (16*(random(97, 5)+4));
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void M_ChangeLightOffset(int monst)
{
	if ((DWORD)monst >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_ChangeLightOffset: Invalid monster %d",monst);
#else
		return;
#endif
	}
	int lx,ly;
	int sign;

	lx = (monster[monst]._mxoff + (monster[monst]._myoff << 1));
	ly = ((monster[monst]._myoff << 1) - monster[monst]._mxoff);

	// Divide these values by 8, because lighting offsets have
	// 8 subdivisions per tile.
	if (lx < 0)
	{
		sign = -1;
		lx = -lx;
	}
	else
		sign = 1;
	lx = lx >> 3;
	lx *= sign;

	if (ly < 0)
	{
		sign = -1;
		ly = -ly;
	}
	else
		sign = 1;
	ly = ly >> 3;
	ly *= sign;

	if (monster[monst].mlid != 0) {
		ChangeLightOff(monster[monst].mlid, lx, ly);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int M_DoStand(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_DoStand: Invalid monster %d",i);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_DoStand: Monster %d \"%s\" MType NULL",i,monster[i].mName);
#else
		return RUN_DONE;
#endif
	}
	MonsterStruct	*Monst = &monster[i];

	if (Monst->MType->mtype == MT_GOLEM)
		Monst->_mAnimData = Monst->MType->Anims[MA_WALK].Cels[Monst->_mdir];
	else
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[Monst->_mdir];
	if (Monst->_mAnimFrame == Monst->_mAnimLen) {
		M_Enemy(i);
	}

	++Monst->_mVar2;
	return RUN_DONE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int M_DoWalk(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_DoWalk: Invalid monster %d",i);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_DoWalk: Monster %d \"%s\" MType NULL",i,monster[i].mName);
#else
		return RUN_DONE;
#endif
	}
	int		rv;

	if (monster[i]._mVar8 == monster[i].MType->Anims[MA_WALK].Frames) {
		dMonster[monster[i]._mx][monster[i]._my] = 0;
		monster[i]._mx += monster[i]._mVar1;
		monster[i]._my += monster[i]._mVar2;
		dMonster[monster[i]._mx][monster[i]._my] = i + 1;
		if( !(monster[i]._mFlags & MFLAG_INVISIBLE)
			&& monster[i].mlid != 0) {
			ChangeLightXY(monster[i].mlid, monster[i]._mx, monster[i]._my);
		}
		M_StartStand(i, monster[i]._mdir);
		rv = RUN_AGAIN;
//		DaveMonstMap(FALSE, 0);
	} else {
		if (monster[i]._mAnimCnt == 0) {
			if (monster[i]._mVar8 == 0 && monster[i].MType->mtype == MT_FLESH)
			{
				PlayEffect(i, MS_SATTACK);
			}
			++monster[i]._mVar8;
			monster[i]._mVar6 += monster[i]._mxvel;
			monster[i]._mVar7 += monster[i]._myvel;
			monster[i]._mxoff = monster[i]._mVar6 >> 4;
			monster[i]._myoff = monster[i]._mVar7 >> 4;
		}
		rv = RUN_DONE;
	}
	if( !(monster[i]._mFlags & MFLAG_INVISIBLE)
		&& monster[i].mlid != 0)
		M_ChangeLightOffset(i);

	return (rv);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int M_DoWalk2(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_DoWalk2: Invalid monster %d",i);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_DoWalk2: Monster %d \"%s\" MType NULL",i,monster[i].mName);
#else
		return RUN_DONE;
#endif
	}
	int		rv;
	
	if (monster[i]._mVar8 == monster[i].MType->Anims[MA_WALK].Frames) {
		dMonster[monster[i]._mVar1][monster[i]._mVar2] = 0;
		if( !(monster[i]._mFlags & MFLAG_INVISIBLE)
			&& monster[i].mlid != 0) {
			ChangeLightXY(monster[i].mlid, monster[i]._mx, monster[i]._my);
		}
		M_StartStand(i, monster[i]._mdir);
		rv = RUN_AGAIN;
//		DaveMonstMap(FALSE, 0);
	} else {
		if (monster[i]._mAnimCnt == 0) {
			if (monster[i]._mVar8 == 0 && monster[i].MType->mtype == MT_FLESH)
			{
				PlayEffect(i, MS_SATTACK);
			}
			++monster[i]._mVar8;
			monster[i]._mVar6 += monster[i]._mxvel;
			monster[i]._mVar7 += monster[i]._myvel;
			monster[i]._mxoff = monster[i]._mVar6 >> 4;
			monster[i]._myoff = monster[i]._mVar7 >> 4;
		}
		rv = RUN_DONE;
	}
	if( !(monster[i]._mFlags & MFLAG_INVISIBLE)
		&& monster[i].mlid != 0)
		M_ChangeLightOffset(i);

	return(rv);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int M_DoWalk3(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_DoWalk3: Invalid monster %d",i);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_DoWalk3: Monster %d \"%s\" MType NULL",i,monster[i].mName);
#else
		return RUN_DONE;
#endif
	}
	int		rv;

	if (monster[i]._mVar8 == monster[i].MType->Anims[MA_WALK].Frames) {
		dMonster[monster[i]._mx][monster[i]._my] = 0;
		monster[i]._mx = monster[i]._mVar1;
		monster[i]._my = monster[i]._mVar2;
		dFlags[monster[i]._mVar4][monster[i]._mVar5] &= BFMASK_MONSTLR;
		dMonster[monster[i]._mx][monster[i]._my] = i + 1;
		if( !(monster[i]._mFlags & MFLAG_INVISIBLE)
			&& monster[i].mlid != 0) {
			ChangeLightXY(monster[i].mlid, monster[i]._mx, monster[i]._my);
		}
		M_StartStand(i, monster[i]._mdir);
		rv = RUN_AGAIN;
//		DaveMonstMap(FALSE, 0);
	} else {
		if (monster[i]._mAnimCnt == 0) {
			if (monster[i]._mVar8 == 0 && monster[i].MType->mtype == MT_FLESH)
			{
				PlayEffect(i, MS_SATTACK);
			}
			++monster[i]._mVar8;
			monster[i]._mVar6 += monster[i]._mxvel;
			monster[i]._mVar7 += monster[i]._myvel;
			monster[i]._mxoff = monster[i]._mVar6 >> 4;
			monster[i]._myoff = monster[i]._mVar7 >> 4;
		}
		rv = RUN_DONE;
	}
	if(monster[i]._uniqtype && !(monster[i]._mFlags & MFLAG_INVISIBLE))
		M_ChangeLightOffset(i);

	return(rv);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void M_TryM2MHit(int i, int mid, int hper, int mind, int maxd)
{
	int		hit, dam;
	BOOL	ret;

	if ((DWORD)mid >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_TryM2MHit: Invalid monster %d",mid);
#else
		return;
#endif
	}
	if (monster[mid].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_TryM2MHit: Monster %d \"%s\" MType NULL",mid,monster[mid].mName);
#else
		return;
#endif
	}

	if ((monster[mid]._mhitpoints >> HP_SHIFT) <= 0) return;
	if (monster[mid].MType->mtype == MT_ILLWEAV && monster[mid]._mgoal == MG_RUN_AWAY) return;

	hit = random(4, 100);
	if (monster[mid]._mmode == MM_STONE) hit = 0;

	if (CheckMonsterHit(mid, ret)) {
		return;
	} else {
		if (hit < hper) {
			dam = random(5, maxd - mind + 1) + mind;
			dam = dam << HP_SHIFT;
	
			monster[mid]._mhitpoints -= dam;
			//rjs - x2 stone dam fix - if (monster[mid]._mmode == MM_STONE) monster[mid]._mhitpoints -= dam;
			if ((monster[mid]._mhitpoints >> HP_SHIFT) <= 0) {
				if (monster[mid]._mmode == MM_STONE) {
					M2MStartKill(i, mid);
					monster[mid]._mmode = MM_STONE;
				} else 	M2MStartKill(i, mid);
			} else {
				if (monster[mid]._mmode == MM_STONE) {
					M2MStartHit(mid, i, dam);
					monster[mid]._mmode = MM_STONE;
				} else M2MStartHit(mid, i, dam);
			}
		}
	}
	return;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void M_TryH2HHit(int i, int pnum, int Hit, int MinDam, int MaxDam)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_TryH2HHit: Invalid monster %d",i);
#else
		return;
#endif
	}
	if (monster[i].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_TryH2HHit: Monster %d \"%s\" MType NULL",i,monster[i].mName);
#else
		return;
#endif
	}

	int		hit, hper, tac;
	long	dam;
	int		dx, dy;
	int		blk, blkper, blkdir;
	int		mdam;

	if ((monster[i]._mFlags & MFLAG_MID) != 0) {
		M_TryM2MHit(i, pnum, Hit, MinDam, MaxDam);
		return;
	}
	
	app_assert((DWORD)pnum < MAX_PLRS);
	if ((plr[pnum]._pHitPoints >> HP_SHIFT) <= 0) return;
	if (plr[pnum]._pInvincible) return;
	if ((plr[pnum]._pSpellFlags & SF_ETHER) != 0) return;

	dx = abs(monster[i]._mx - plr[pnum]._px);
	dy = abs(monster[i]._my - plr[pnum]._py);
	if ((dx < 2) && (dy < 2)) {
		// Did I hit?
		hit = random(98, 100);
#if CHEATS
		if (simplecheat || cheatflag) hit = 1000;		// TEMP!
#endif
		//rjs tac = (byte)plr[pnum]._pArmorClass + plr[pnum]._pIAC + plr[pnum]._pIBonusAC;
		tac = plr[pnum]._pIAC + plr[pnum]._pIBonusAC;
		
		if ((plr[pnum]._pIFlags2 & IAF2_DEMONAC) &&
			(monster[i].MData->mMonstClass == MC_DEMON))
			tac += 40;
		if ((plr[pnum]._pIFlags2 & IAF2_UNDEADAC) &&
			(monster[i].MData->mMonstClass == MC_UNDEAD))
			tac += 20;
		
		tac += (plr[pnum]._pDexterity / 5);
		hper = 30 + Hit - tac + ((monster[i].mLevel - plr[pnum]._pLevel) << 1);
		if (hper < 15) hper = 15;
		if ((currlevel == 14) && (hper < 20)) hper = 20;
		if ((currlevel == 15) && (hper < 25)) hper = 25;
		if ((currlevel == 16) && (hper < 30)) hper = 30;
		if (((plr[pnum]._pmode == PM_STAND) || (plr[pnum]._pmode == PM_ATTACK)) && (plr[pnum]._pBlockFlag)) blk = random(98, 100);
		else blk = 100;
		blkper = plr[pnum]._pBaseToBlk + plr[pnum]._pDexterity - ((monster[i].mLevel - plr[pnum]._pLevel) << 1);
		if (blkper < 0) blkper = 0;
		if (blkper > 100) blkper = 100;
		if (hit < hper) {
			if (blk < blkper) {
				blkdir = GetDirection(plr[pnum]._px, plr[pnum]._py, monster[i]._mx, monster[i]._my);
				StartPlrBlock(pnum, blkdir);
				if (pnum == myplr 
					&& plr[pnum]._pReflectCount > 0) {
					--plr[pnum]._pReflectCount;
					// Reflect back with 20->30% of the damage.
					dam = (MaxDam - MinDam + 1) << HP_SHIFT;
					dam = random(99, dam) + (MinDam << HP_SHIFT);
					dam += (plr[pnum]._pIGetHit << HP_SHIFT);
					if (dam < (1 << HP_SHIFT)) dam = (1 << HP_SHIFT);
					mdam = static_cast<int>(dam * (0.01 * (random(100, 10) + 20)));
					monster[i]._mhitpoints -= mdam;
					dam -= mdam;
					if (dam < 0) {
						dam = 0;
					}
					if (monster[i]._mhitpoints >> HP_SHIFT <= 0) {
						M_StartKill (i, pnum);
					} else {
						M_StartHit(i, pnum, mdam);
					}
				}
					
				
			} else {

				
				if (monster[i].MType->mtype == MT_YZOMBIE && pnum == myplr)
				{
				
					// find manashield
					int k;
					int mi;
					int msi = -1;
					for (k = 0; k < nummissiles; ++k) {
						mi = missileactive[k];
						if (missile[mi]._mitype == MIT_MANASHIELD && missile[mi]._misource == pnum) {
							msi = mi;
						}
					}

					if(plr[pnum]._pMaxHP > 1 << HP_SHIFT )
					{
						plr[pnum]._pMaxHP -= 1 << HP_SHIFT;
						if(plr[pnum]._pHitPoints > plr[pnum]._pMaxHP) {
							plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
							if( msi >= 0 )
								missile[msi]._miVar1 = plr[pnum]._pHitPoints;
						}
						plr[pnum]._pMaxHPBase -= 1 << HP_SHIFT;
						if (plr[pnum]._pHPBase > plr[pnum]._pMaxHPBase) {
							plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;
							if( msi >= 0 )
								missile[msi]._miVar2 = plr[pnum]._pHPBase;
						}
					}


				}

	
			
				dam = (MaxDam - MinDam + 1) << HP_SHIFT;
				dam = random(99, dam) + (MinDam << HP_SHIFT);
				dam += (plr[pnum]._pIGetHit << HP_SHIFT);
				if (dam < (1 << HP_SHIFT)) dam = (1 << HP_SHIFT);
				if (pnum == myplr) {
					if (plr[pnum]._pReflectCount > 0)
					{
						--plr[pnum]._pReflectCount;
						// Reflect back with 20->30% of the damage.
						mdam = static_cast<int>(dam * (0.01 * (random(100, 10) + 20)));
						monster[i]._mhitpoints -= mdam;
						dam -= mdam;
						if (dam < 0) {
							dam = 0;
						}
						if (monster[i]._mhitpoints >> HP_SHIFT <= 0) {
							M_StartKill (i, pnum);
						} else {
							M_StartHit(i, pnum, mdam);
						}
					}
					
					plr[pnum]._pHitPoints -= dam;
					plr[pnum]._pHPBase -= dam;
				}

				


				if (plr[pnum]._pIFlags & IAF_THORN) {
					mdam = ((random (99, 3) + 1) << HP_SHIFT);
					monster[i]._mhitpoints -= mdam;
					if (monster[i]._mhitpoints >> HP_SHIFT <= 0) {
						M_StartKill (i, pnum);
						//(old) AddPlrExperience(pnum, monster[i].mLevel, monster[i].mExp);
						//(new, but moved) AddPlrMonstExper(monster[i].mLevel, monster[i].mExp, monster[i].mWhoHit);
					} else {
						M_StartHit(i, pnum, mdam);
					}
				}
				if (!(monster[i]._mFlags & IAF_MNOHEAL)) {
					if (monster[i].MType->mtype == MT_SKING
						&& gbMaxPlayers != 1)
						monster[i]._mhitpoints += dam;
				}
				if (plr[pnum]._pHitPoints > plr[pnum]._pMaxHP) {
					plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
					plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;
				}
				if ((plr[pnum]._pHitPoints >> HP_SHIFT) <= 0) {
					// rjs - manashld fix? - plr[pnum]._pHitPoints = 0;
					StartPlrKill(pnum, FALSE);
					M_StartStand(i, monster[i]._mdir);
				} else {
					StartPlrHit(pnum, dam, FALSE);

					if (monster[i]._mFlags & MFLAG_KNOCKBACK) {
						// make sure player is doing a hit animation
						if (plr[pnum]._pmode != PM_GOTHIT)
							StartPlrHit(pnum, 0, TRUE);

						// knock back enemy one square
						int newx,newy,oldx,oldy;

						oldx = plr[pnum]._px;
						oldy = plr[pnum]._py;
						newx = oldx + offset_x[monster[i]._mdir];
						newy = oldy + offset_y[monster[i]._mdir];
						if(PosOkPlayer(pnum, newx, newy))
						{
							plr[pnum]._px = newx;
							plr[pnum]._py = newy;
							FixPlayerLocation(pnum,plr[pnum]._pdir);
							FixPlrWalkTags(pnum);
							dPlayer[newx][newy] = pnum + 1;
							SetPlayerOld(pnum);
						}
					}
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int M_DoAttack(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_DoAttack: Invalid monster %d",i);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_DoAttack: Monster %d \"%s\" MType NULL",i,monster[i].mName);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_DoAttack: Monster %d \"%s\" MData NULL",i,monster[i].mName);
#else
		return RUN_DONE;
#endif
	}
	MonsterStruct	*Monst = &monster[i];

	if (Monst->_mAnimFrame == Monst->MData->mAFNum) {
		M_TryH2HHit(i, Monst->_menemy, Monst->mHit, Monst->mMinDamage, Monst->mMaxDamage);
		if (Monst->_mAi != AI_SNAKE)
			PlayEffect(i, MS_ATTACK);
	}
	// Special code for Magma -- second punch
	if (EquivMonst(Monst->MType->mtype, MT_NMAGMA)
		 && Monst->_mAnimFrame == 9)
	{
		M_TryH2HHit(i, Monst->_menemy, Monst->mHit+10, Monst->mMinDamage-2, Monst->mMaxDamage-2);
		PlayEffect(i, MS_ATTACK);
	}
	// Special code for Storm -- second punch
	if (EquivMonst(Monst->MType->mtype, MT_STORM)
			&& Monst->_mAnimFrame == 13)
	{
		M_TryH2HHit(i, Monst->_menemy, Monst->mHit-20, Monst->mMinDamage+4, Monst->mMaxDamage+4);
		PlayEffect(i, MS_ATTACK);
	}
	// Special code for Snake -- play sound before attack frame
	if (Monst->_mAi == AI_SNAKE && Monst->_mAnimFrame == 1)
		PlayEffect(i, MS_ATTACK);

	if (Monst->_mAnimFrame == Monst->_mAnimLen) {
		M_StartStand(i, Monst->_mdir);
		return(RUN_AGAIN);
	} else return (RUN_DONE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int M_DoRAttack(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_DoRAttack: Invalid monster %d",i);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_DoRAttack: Monster %d \"%s\" MType NULL",i,monster[i].mName);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_DoRAttack: Monster %d \"%s\" MData NULL",i,monster[i].mName);
#else
		return RUN_DONE;
#endif
	}
	int multimissiles;
	int mi;

	if (monster[i]._mAnimFrame == monster[i].MData->mAFNum) {
		if (monster[i]._mVar1 != -1)
		{
			if (monster[i]._mVar1 == MIT_CBOLT)
				multimissiles = 3;
			else
				multimissiles = 1;
			for (mi = 0; mi < multimissiles; ++mi)		// special loop code to handle CBOLT, which must be cast 3 times
				AddMissile(monster[i]._mx + infront_x[monster[i]._mdir], 
						   monster[i]._my + infront_y[monster[i]._mdir], 
						   monster[i]._menemyx, monster[i]._menemyy, monster[i]._mdir, monster[i]._mVar1, MI_ENEMYPLR, i, monster[i]._mVar2, 0);
		}
		PlayEffect(i, MS_ATTACK);
	}
	if (monster[i]._mAnimFrame == monster[i]._mAnimLen) {
		M_StartStand(i, monster[i]._mdir);
		return(RUN_AGAIN);
	} else return (RUN_DONE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int M_DoRSpAttack(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_DoRSpAttack: Invalid monster %d",i);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_DoRSpAttack: Monster %d \"%s\" MType NULL",i,monster[i].mName);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_DoRSpAttack: Monster %d \"%s\" MData NULL",i,monster[i].mName);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i]._mAnimFrame == monster[i].MData->mAFNum2 && !monster[i]._mAnimCnt) {
		AddMissile(monster[i]._mx + infront_x[monster[i]._mdir], 
				   monster[i]._my + infront_y[monster[i]._mdir], 
				   monster[i]._menemyx, monster[i]._menemyy, monster[i]._mdir, monster[i]._mVar1, MI_ENEMYPLR, i, monster[i]._mVar3, 0);
		if (Monsters[i].Snds[MS_SATTACK].effect[0] != 0){
			PlayEffect(i, MS_SATTACK);
		}
	}

	// special code for Mega demon -- hold attack animation frame
	if (monster[i]._mAi == AI_MEGA && monster[i]._mAnimFrame == 3)
	{
		if (!monster[i]._mVar2++)
			monster[i]._mFlags |= MFLAG_STILL;
		else if (monster[i]._mVar2 == 15)
			monster[i]._mFlags &= ~MFLAG_STILL;
	}

	if (monster[i]._mAnimFrame == monster[i]._mAnimLen) {
		M_StartStand(i, monster[i]._mdir);
		return(RUN_AGAIN);
	} else return (RUN_DONE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int M_DoSAttack(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_DoSAttack: Invalid monster %d",i);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_DoSAttack: Monster %d \"%s\" MType NULL",i,monster[i].mName);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_DoSAttack: Monster %d \"%s\" MData NULL",i,monster[i].mName);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i]._mAnimFrame == monster[i].MData->mAFNum2)
		M_TryH2HHit(i, monster[i]._menemy, monster[i].mHit2, monster[i].mMinDamage2, monster[i].mMaxDamage2);
	if (monster[i]._mAnimFrame == monster[i]._mAnimLen) {
		M_StartStand(i, monster[i]._mdir);
		return(RUN_AGAIN);
	} else return(RUN_DONE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int M_DoFadein(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_DoFadein: Invalid monster %d",i);
#else
		return RUN_DONE;
#endif
	}
	if ((monster[i]._mFlags & MFLAG_BACKWARDS && monster[i]._mAnimFrame == 1)
		|| (!(monster[i]._mFlags & MFLAG_BACKWARDS) && monster[i]._mAnimFrame == monster[i]._mAnimLen))
	{
		M_StartStand(i, monster[i]._mdir);
		monster[i]._mFlags &= ~MFLAG_BACKWARDS;
		return(RUN_AGAIN);
	} else return(RUN_DONE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int M_DoFadeout(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_DoFadeout: Invalid monster %d",i);
#else
		return RUN_DONE;
#endif
	}
	int mtype;
	if ((monster[i]._mFlags & MFLAG_BACKWARDS && monster[i]._mAnimFrame == 1)
		|| (!(monster[i]._mFlags & MFLAG_BACKWARDS) && monster[i]._mAnimFrame == monster[i]._mAnimLen))
	{
		app_assert(monster[i].MType != NULL);
		mtype = monster[i].MType->mtype;
		if (EquivMonst(mtype, MT_INCIN))
			monster[i]._mFlags &= ~MFLAG_BACKWARDS;
		else
		{
			monster[i]._mFlags &= ~MFLAG_BACKWARDS;
			monster[i]._mFlags |= MFLAG_INVISIBLE;
		}
		M_StartStand(i, monster[i]._mdir);
		return(RUN_AGAIN);
	} else return(RUN_DONE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int M_DoHeal(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_DoHeal: Invalid monster %d",i);
#else
		return RUN_DONE;
#endif
	}
	MonsterStruct	*Monst = &monster[i];

	if (monster[i]._mFlags & MFLAG_NOHEAL) return RUN_DONE;
	// Gargoyle turning back to stone
	if(Monst->_mAnimFrame == 1)
	{
		Monst->_mFlags &= ~MFLAG_BACKWARDS;
		Monst->_mFlags |= MFLAG_STILL;
		if(Monst->_mhitpoints + Monst->_mVar1 < Monst->_mmaxhp)
			Monst->_mhitpoints += Monst->_mVar1;
		else
		{
			Monst->_mhitpoints = Monst->_mmaxhp;
			Monst->_mFlags &= ~MFLAG_STILL;
			Monst->_mmode = MM_SATTACK;
		}
	}
	return RUN_DONE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

int M_DoTalk(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_DoTalk: Invalid monster %d",i);
#else
		return RUN_DONE;
#endif
	}
	MonsterStruct	*Monst = &monster[i];

	M_StartStand(i, monster[i]._mdir);
	Monst->_mgoal=MG_WAITTOTALK;
	if (effect_is_playing(alltext[monster[i].mtalkmsg].sfxnr)) return RUN_DONE;
	InitQTextMsg(monster[i].mtalkmsg);
	if (monster[i].mName == UniqMonst[MU_GARBUD].mName) {	
		if (monster[i].mtalkmsg == TXT_GARB1)
			quests[Q_GARBUD]._qactive = QUEST_NOTDONE;
			quests[Q_GARBUD]._qlog = TRUE;
		if (monster[i].mtalkmsg == TXT_GARB2) {
			if(!(monster[i]._mFlags & MFLAG_DROP)) { 
				SpawnItem(i, monster[i]._mx+1, monster[i]._my+1, TRUE);
				monster[i]._mFlags |= MFLAG_DROP;
			}
		}	
	}
	if (monster[i].mName == UniqMonst[MU_ZHAR].mName) {
		if (monster[i].mtalkmsg == TXT_ZHAR1) {
			if(!(monster[i]._mFlags & MFLAG_DROP)) { 
				quests[Q_ZHAR]._qactive = QUEST_NOTDONE;
				quests[Q_ZHAR]._qlog = TRUE;
				CreateTypeItem(monster[i]._mx+1, monster[i]._my+1, FALSE, IT_MISC, IMID_BOOK, TRUE, FALSE);
				monster[i]._mFlags |= MFLAG_DROP;
			}
		}
	}
	if (monster[i].mName == UniqMonst[MU_SNOTSPIL].mName) {
		if ((monster[i].mtalkmsg == TXT_BOL1) && (!(monster[i]._mFlags & MFLAG_DROP))) {
			app_assert(setpc_x != 0);
			ObjChangeMap(setpc_x, setpc_y, setpc_x+(setpc_w>>1)+2, setpc_y+(setpc_h>>1)-2);
			int const tren = TransVal;
			TransVal = 9;
			DRLG_MRectTrans(setpc_x, setpc_y, setpc_x+(setpc_w>>1)+4, setpc_y+(setpc_h>>1));
			TransVal = tren;
			quests[Q_LTBANNER]._qvar1 = 2;
			if (quests[Q_LTBANNER]._qactive == QUEST_NOTACTIVE) quests[Q_LTBANNER]._qactive = QUEST_NOTDONE;
			monster[i]._mFlags |= MFLAG_DROP;
		}
		if (quests[Q_LTBANNER]._qvar1 < 2) {
			sprintf(tempstr, "SS Talk = %i, Flags = %i", monster[i].mtalkmsg, monster[i]._mFlags);
			app_fatal(tempstr);
		}
	}
	if (monster[i].mName == UniqMonst[MU_LACHDA].mName) {
		if (monster[i].mtalkmsg == TXT_VEIL1)  {
			quests[Q_VEIL]._qactive = QUEST_NOTDONE;
			quests[Q_VEIL]._qlog = TRUE;
		}
		if ((monster[i].mtalkmsg == TXT_VEIL3) && (!(monster[i]._mFlags & MFLAG_DROP))) {
			SpawnUnique(UID_STEELVEIL, monster[i]._mx+1, monster[i]._my+1);
			monster[i]._mFlags |= MFLAG_DROP;
		}
	}
	if (monster[i].mName == UniqMonst[MU_WARLORD].mName) {
		app_assert(gbMaxPlayers == 1);
		quests[Q_WARLORD]._qvar1 = 2;
	}
	if ((monster[i].mName == UniqMonst[MU_LAZARUS].mName) && (gbMaxPlayers != 1)) {
		quests[Q_BETRAYER]._qvar1 = 6;
		monster[i]._mgoal = MG_ATTACK;
		monster[i]._msquelch = 255;
		monster[i].mtalkmsg = 0;
	}
	return RUN_DONE;
}

/*-----------------------------------------------------------------------*
 * M_Teleport
 *
 * Teleports monster i to one of it's enemy's 8 adjacent squares.
**----------------------------------------------------------------------*/

void M_Teleport(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_Teleport: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];
	BOOL done = FALSE;
	int mulx,muly;
	int x,y;
	int a,b;
	int px,py;

	if (Monst->_mmode == MM_STONE)
		return;

	px = Monst->_menemyx;
	py = Monst->_menemyy;

	// randomize search direction
	mulx = random(100,2)*2-1;
	muly = random(100,2)*2-1;
	for(a = -1; a <= 1 && !done; ++a)
		for(b = -1; b < 1 && !done; ++b)
			if(a || b)
			{
				x = px + a*mulx;
				y = py + b*muly;
				if(InBounds(x,y)
					&& x != Monst->_mx && y != Monst->_my)
					if(PosOkMonst(i,x,y))
						done = TRUE;
			}
	if(done)
	{
		M_ClearSquares(i);
		app_assert((DWORD)Monst->_mx < MAXDUNX);
		app_assert((DWORD)Monst->_my < MAXDUNY);
		dMonster[Monst->_mx][Monst->_my] = 0;
		dMonster[x][y] = i+1;
		// M_StartHit knocks monster back to oldx,oldy, so those are the members to set
		Monst->_moldx = x;
		Monst->_moldy = y;
		Monst->_mdir = M_GetDir(i);
		M_CheckEFlag(i);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int M_DoGotHit(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_DoGotHit: Invalid monster %d",i);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_DoGotHit: Monster %d \"%s\" MType NULL",i,monster[i].mName);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i]._mAnimFrame == monster[i]._mAnimLen) {
		M_StartStand(i, monster[i]._mdir);
		return(RUN_AGAIN);
	} else return(RUN_DONE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void M_UpdateLeader(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_UpdateLeader: Invalid monster %d",i);
#else
		return;
#endif
	}
	int x,tmp;

	// check for pack leader
	if(monster[i]._uniqtype && (UniqMonst[monster[i]._uniqtype-1].mUnqAttr & UN_STICK))
		// free the pack
		app_assert((DWORD)nummonsters <= MAXMONSTERS);
		for(x = 0; x < nummonsters; ++x)
		{
			if(monster[tmp=monstactive[x]].leaderflag == PACK_MEMBER
				&& monster[tmp].leader == i)
			{
				monster[tmp].leaderflag = 0;
			}
		}
	// check for pack member
	if(monster[i].leaderflag == PACK_MEMBER)
		--monster[monster[i].leader].packsize;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void play_movie(const char * pszMovie,BOOL bAllowCancel);
extern BOOL gbRunGame;
extern BOOL deathflag;
extern BYTE gbDoEnding;

void DoEnding() {
	// tell other players to view victory
	if (gbMaxPlayers > 1) SNetLeaveGame(SNET_EXIT_PLAYERWON);

	//SetCursor(NO_CURSOR);
	//FullBlit(TRUE);
	//PaletteFadeOut(FADE_FAST);
	music_stop();

	// give SNet some more time to field messages
	if (gbMaxPlayers > 1) Sleep(1000);

	#if !IS_VERSION(SHAREWARE)
	LONG lMusicVol;
	BOOL bMusicOn;
	if (plr[myplr]._pClass == CLASS_WARRIOR
		|| plr[myplr]._pClass == CLASS_BARBARIAN) play_movie("gendata\\DiabVic2.smk",FALSE);
	else if (plr[myplr]._pClass == CLASS_SORCEROR) play_movie("gendata\\DiabVic1.smk",FALSE);
	else if (plr[myplr]._pClass == CLASS_MONK) play_movie("gendata\\DiabVic1.smk",FALSE);
	else play_movie("gendata\\DiabVic3.smk",FALSE);
	play_movie("gendata\\Diabend.smk",FALSE);

	// turn music to full volume
	bMusicOn = gbMusicOn;
	gbMusicOn = TRUE;
	lMusicVol = music_volume(VOLUME_READ);
	music_volume(VOLUME_MAX);
	music_start(MUSIC_L2);

	extern BOOL gbLoopMovie;
	gbLoopMovie = TRUE;
	play_movie("gendata\\loopdend.smk",TRUE);
	gbLoopMovie = FALSE;
	#endif

	//ClrDraw();
	//FullBlit(FALSE);
	//PaletteFadeOut(FADE_FAST);

	// restore music volume
	#if !IS_VERSION(SHAREWARE)
	music_stop();
	music_volume(lMusicVol);
	gbMusicOn = bMusicOn;
	#endif
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void PrepDoEnding() {
	// enable sound again
	gbSoundOn = sgbSaveSoundOn;
	gbDoEnding = TRUE;
	gbRunGame = FALSE;
	deathflag = FALSE;

	app_assert((DWORD)myplr < MAX_PLRS);
	plr[myplr].pDiabloKillLevel = max(
		plr[myplr].pDiabloKillLevel,
		(DWORD) gnDifficulty + 1
	);

	for (int i = 0; i < MAX_PLRS; ++i) {
		plr[i]._pmode = PM_QUIT;
		plr[i]._pInvincible = TRUE;
		if (gbMaxPlayers > 1) {
			if ((plr[i]._pHitPoints >> HP_SHIFT) == 0)
				plr[i]._pHitPoints = 1 << HP_SHIFT;
			if ((plr[i]._pMana >> MANA_SHIFT) == 0)
				plr[i]._pMana = 1 << MANA_SHIFT;
		}
	}
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
int M_DoDeath(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_DoDeath: Invalid monster %d",i);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_DoDeath: Monster %d \"%s\" MType NULL",i,monster[i].mName);
#else
		return RUN_DONE;
#endif
	}
	++monster[i]._mVar1;
	if (monster[i].MType->mtype == MT_DIABLO) {
		ViewX += Sign(monster[i]._mx - ViewX);
		ViewY += Sign(monster[i]._my - ViewY);
		if (monster[i]._mVar1 == 140) {
			PrepDoEnding();
		}
	} else {
		if (monster[i]._mAnimFrame == monster[i]._mAnimLen) {
			if (monster[i]._uniqtype == 0)
				AddDead(monster[i]._mx, monster[i]._my, monster[i].MType->mdeadval, monster[i]._mdir);
			else
				AddDead(monster[i]._mx, monster[i]._my, monster[i]._udeadval, monster[i]._mdir);
			app_assert(!(dFlags[monster[i]._mx+1][monster[i]._my] & BFLAG_MONSTLR)
				&& !(dFlags[monster[i]._mx][monster[i]._my+1] & BFLAG_MONSTLR));
			dMonster[monster[i]._mx][monster[i]._my] = 0;
			monster[i]._mDelFlag = TRUE;
			M_UpdateLeader(i);
		}
	}
	return(RUN_DONE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int M_DoSpStand(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_DoSpStand: Invalid monster %d",i);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_DoSpStand: Monster %d \"%s\" MType NULL",i,monster[i].mName);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i]._mAnimFrame == monster[i].MData->mAFNum2) {
		PlayEffect(i, MS_SATTACK);
	}
	if (monster[i]._mAnimFrame == monster[i]._mAnimLen) {
		M_StartStand(i, monster[i]._mdir);
		return(RUN_AGAIN);
	} else return(RUN_DONE);
}

int M_DoDelay(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_DoDelay: Invalid monster %d",i);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i].MType == NULL)
	{
#if defined(_DEBUG)
		app_fatal("M_DoDelay: Monster %d \"%s\" MType NULL",i,monster[i].mName);
#else
		return RUN_DONE;
#endif
	}
	int		md;

	md = M_GetDir(i);
	monster[i]._mAnimData = monster[i].MType->Anims[MA_STAND].Cels[md];
	if (monster[i]._mAi == AI_LAZURUS) {
		if ((monster[i]._mVar2 > 8) || (monster[i]._mVar2 < 0)) monster[i]._mVar2 = 8;
	}
	if(!monster[i]._mVar2--)
	{
		int tmp = monster[i]._mAnimFrame;
		M_StartStand(i, monster[i]._mdir);		// StartStand sets AnimFrame to 0
		monster[i]._mAnimFrame = tmp;			// restore AnimFrame to avoid pop
												// in case monster continues standing.
		return(RUN_AGAIN);
	} else return(RUN_DONE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int M_DoStone(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_DoStone: Invalid monster %d",i);
#else
		return RUN_DONE;
#endif
	}
	if (monster[i]._mhitpoints == 0) {
		dMonster[monster[i]._mx][monster[i]._my] = 0;
		monster[i]._mDelFlag = TRUE;
	}
	return(RUN_DONE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void M_WalkDir(int i, int md)
{
	int		mwi;

	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_WalkDir: Invalid monster %d",i);
#else
		return;
#endif
	}
	mwi = monster[i].MType->Anims[MA_WALK].Frames - 1;

	switch (md) {
		case M_DIRU : 
			M_StartWalk(i, 0, -MWVel[mwi][1], -1, -1, M_DIRU);
			break;
		case M_DIRUR :
			M_StartWalk(i, MWVel[mwi][1], -MWVel[mwi][0], 0, -1, M_DIRUR);
			break;
		case M_DIRR :
			M_StartWalk3(i, MWVel[mwi][2], 0, -32, -16, 1, -1, 1, 0, M_DIRR);
			break;
		case M_DIRDR :
			M_StartWalk2(i, MWVel[mwi][1], MWVel[mwi][0], -32, -16, 1, 0, M_DIRDR);
			break;
		case M_DIRD :
			M_StartWalk2(i, 0, MWVel[mwi][1], 0, -32, 1, 1, M_DIRD);
			break;
		case M_DIRDL :
			M_StartWalk2(i, -MWVel[mwi][1], MWVel[mwi][0], 32, -16, 0, 1, M_DIRDL);
			break;
		case M_DIRL :
			M_StartWalk3(i, -MWVel[mwi][2], 0, 32, -16, -1, 1, 0, 1, M_DIRL);
			break;
		case M_DIRUL :
			M_StartWalk(i, -MWVel[mwi][1], -MWVel[mwi][0], -1 ,0, M_DIRUL);
			break;
	}
}
	
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void GroupUnity(int i)
{
	int	leader;
	int tmp;
	int m;

	//
	// IMPLEMENT GROUP COHESION
	//

	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("GroupUnity: Invalid monster %d",i);
#else
		return;
#endif
	}
	if(monster[i].leaderflag)
	{
		leader = monster[i].leader;

		// test if there's a wall between monst and pack leader
		tmp = LineClearF(CheckNoSolid, monster[i]._mx, monster[i]._my, 
									 monster[leader]._mfutx, monster[leader]._mfuty);
		if(!tmp && monster[i].leaderflag == PACK_MEMBER)
		{
			// have to leave pack
			--monster[leader].packsize;
			monster[i].leaderflag = PACK_NOMEMBER;
		}
		else if(tmp && monster[i].leaderflag == PACK_NOMEMBER
				&& DIST(monster[i]._mx - monster[leader]._mfutx, 
						monster[i]._my - monster[leader]._mfuty, 4))
		{
			// rejoin pack
			++monster[leader].packsize;
			monster[i].leaderflag = PACK_MEMBER;
		}
	}

	if(monster[i].leaderflag == PACK_MEMBER)
	{
		// make sure leader is active
		if(monster[i]._msquelch > monster[leader]._msquelch)
		{
			monster[leader]._lastx = monster[i]._mx;
			monster[leader]._lasty = monster[i]._my;
			monster[leader]._msquelch = monster[i]._msquelch-1;
		}
		if (monster[leader]._mAi == AI_GARG && (monster[leader]._mFlags & MFLAG_STILL)) {
			monster[leader]._mFlags &= ~MFLAG_STILL;
			monster[leader]._mmode = MM_SATTACK;
		}
	}
	else if(monster[i]._uniqtype && (UniqMonst[monster[i]._uniqtype-1].mUnqAttr & UN_STICK))
	{
		// make sure all of pack is activated
		app_assert((DWORD)nummonsters <= MAXMONSTERS);
		for(m = 0; m < nummonsters; ++m)
		{
			if(monster[tmp=monstactive[m]].leaderflag == PACK_MEMBER
				&& monster[tmp].leader == i)
			{
				if(monster[i]._msquelch > monster[tmp]._msquelch)
				{
					monster[tmp]._lastx = monster[i]._mx;
					monster[tmp]._lasty = monster[i]._my;
					monster[tmp]._msquelch = monster[i]._msquelch-1;
				}
				if (monster[tmp]._mAi == AI_GARG && (monster[tmp]._mFlags & MFLAG_STILL)) {
					monster[tmp]._mFlags &= ~MFLAG_STILL;
					monster[tmp]._mmode = MM_SATTACK;
				}
			}
		}
	}
	// END OF GROUP COHESION
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL M_CallWalk(int i, int md)
{
	int mdtemp = md;
	BOOL ok = FALSE;
	ok = DirOK(i,md);

	if(random(101,2))
		ok = ok || DirOK(i, md = left[mdtemp]) || DirOK(i, md = right[mdtemp]);
	else
		ok = ok || DirOK(i, md = right[mdtemp]) || DirOK(i, md = left[mdtemp]);

	if(random(102,2))
		ok = ok || DirOK(i, md = right[right[mdtemp]]) || DirOK(i, md = left[left[mdtemp]]);
	else
		ok = ok || DirOK(i, md = left[left[mdtemp]]) || DirOK(i, md = right[right[mdtemp]]);

	if(ok)
		M_WalkDir(i, md);

	return ok;
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL M_PathWalk(int i)
{
	char path[MAXPATHLEN];
	static const char plr2monst[] = { 0, 5, 3, 7, 1, 4, 6, 0, 2 };

	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_PathWalk: Invalid monster %d",i);
#else
		return FALSE;
#endif
	}
	CHECKFUNC1 Check = (monster[i]._mFlags & MFLAG_CHECKDOORS) ? PosOkMonst3:PosOkMonst;

	int pathlen = FindPath(Check, i, monster[i]._mx, monster[i]._my,
							monster[i]._menemyx, monster[i]._menemyy,
							path);
	if(pathlen) {
		M_CallWalk(i, plr2monst[path[0]]);
		return TRUE;
	}
	else
		return FALSE;
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL M_CallWalk2(int i, int md)
{
	int mdtemp = md;
	BOOL ok = FALSE;
	ok = DirOK(i,md);

	if(random(101,2))
		ok = ok || DirOK(i, md = left[mdtemp]) || DirOK(i, md = right[mdtemp]);
	else
		ok = ok || DirOK(i, md = right[mdtemp]) || DirOK(i, md = left[mdtemp]);

	if(ok)
		M_WalkDir(i, md);

	return ok;
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL M_DumbWalk(int i, int md)
{
	BOOL ok;

	ok = DirOK(i,md);

	if(ok)
		M_WalkDir(i, md);
	return ok;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL M_RoundWalk(int i, int md, int &dir)
{
	int mdtemp;
	BOOL ok = FALSE;

	if(dir)
		md = left[left[md]];
	else
		md = right[right[md]];
	mdtemp = md;

	ok = DirOK(i,md);
	if(!ok)
	{
		// Note: The use of || below is a trick: the second condition is not evaluated
		//       (and therefore md is not re-set) if the first condition evaluates TRUE.

		if(dir)
			ok = DirOK(i, md = right[mdtemp]) || DirOK(i, md = right[right[mdtemp]]);
		else
			ok = DirOK(i, md = left[mdtemp]) || DirOK(i, md = left[left[mdtemp]]);
	}

	if(ok)
		M_WalkDir(i,md);
	else 
	{
		// Note: The value of md here is set according to the second condition of the
		//		 || operators above.  Therefore, md is, by design, the direction towards
		//		 the player.
		dir = !dir;
		ok = M_CallWalk(i,opposite[mdtemp]);
	}
	return ok;
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void M_Face(int i)
{
	int		md;

	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("M_Face: Invalid monster %d",i);
#else
		return;
#endif
	}
	if (monster[i]._msquelch) {
		md = M_GetDir(i);
		monster[i]._mdir = md;
		if (monster[i].MType == NULL)
		{
#if defined(_DEBUG)
			app_fatal("M_Face: Monster %d \"%s\" MType NULL",i,monster[i].mName);
#else
			return;
#endif
		}
		monster[i]._mAnimData = monster[i].MType->Anims[MA_STAND].Cels[md];
	}
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_Zombie(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Zombie: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];
	int		mx, my, md, v;

	if (Monst->_mmode == MM_STAND) {
		mx = Monst->_mx;
		my = Monst->_my;
		if (dFlags[mx][my] & BFLAG_MONSTACTIVE) {
			mx -= Monst->_menemyx;
			my -= Monst->_menemyy;
			md = Monst->_mdir;
			v = random(103,100);

			// Try attack
			if (DIST(mx,my,2)) {
				if (v < (10 + 2*Monst->_mint)) M_StartAttack(i);
			} else {
				// Try walk
				if (v < (10 + 2*Monst->_mint))	{
					// Check on distance from player
					if(DIST(mx,my,4+2*Monst->_mint)) {
						md = M_GetDir(i);
						M_CallWalk(i, md);
					} else {
						if(random(104,100) < (20 + 2*Monst->_mint)) md = random(104,8);
						M_DumbWalk(i, md);
					}
				}
			}
			// face dir
			if (Monst->_mmode == MM_STAND) Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[Monst->_mdir];
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_SkelSd(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_SkelSd: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];
	int		mx, my, md;

	if ((Monst->_mmode == MM_STAND) && (Monst->_msquelch)) {
		mx = Monst->_mx - Monst->_menemyx;
		my = Monst->_my - Monst->_menemyy;
		md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);
		Monst->_mdir = md;
		// Try attack
		if (DIST(mx,my,2)) {
			if (Monst->_mVar1 == MM_DELAY || random(105,100) < 20 + 2*Monst->_mint)
				M_StartAttack(i);
			else
				M_StartDelay(i, random(105,10)+10 - 2*Monst->_mint);
		} else {
			// Try walk -- more likely to walk if already walking
			if ((Monst->_mVar1 != MM_DELAY) && random(106,100) < 35 - 4*Monst->_mint)
				M_StartDelay(i, random(106,10) + 15 - 2*Monst->_mint);
			else
				M_CallWalk(i, md);
		}
		// face dir
		if (Monst->_mmode == MM_STAND) Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[md];
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL MAI_Path(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Path: Invalid monster %d",i);
#else
		return FALSE;
#endif
	}
	MonsterStruct	*Monst = &monster[i];

	if  ((Monst->MType->mtype != MT_GOLEM)				// golum always ready to walk
		 && ((!Monst->_msquelch)
			 || (Monst->_mmode != MM_STAND)
			 || !(Monst->_mgoal == MG_ATTACK
				  || Monst->_mgoal == MG_WALK_AROUND1
				  || Monst->_mgoal == MG_ATTACK2)
			 || (Monst->_mx == 1 && Monst->_my == 0)))	// inactive golum
		return FALSE;
	if (!LineClearF1(PosOkMonst2, i,Monst->_mx,Monst->_my,Monst->_menemyx,Monst->_menemyy)
		|| (Monst->_pathcount >= 5 && Monst->_pathcount < 8))
	{
		// Try Opening Door
		if (Monst->_mFlags & MFLAG_CHECKDOORS)
			MonstCheckDoors(i);

		if (++Monst->_pathcount >= 5)
		{
			if (M_PathWalk(i))
				return TRUE;
		}
		else				// don't want to zero pathcount
			return FALSE;
	}
 
	if  (Monst->MType->mtype != MT_GOLEM) Monst->_pathcount = 0; //path count for golum handled in ai
	return FALSE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_Snake(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Snake: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];
	int		fx, fy, mx, my, md, pnum;
	char	pattern[] = { +1, +1, 0, -1, -1 , 0};
	int tmp;

	pnum = Monst->_menemy;
	if ((Monst->_mmode == MM_STAND) && (Monst->_msquelch)) {
		fx = Monst->_menemyx;
		fy = Monst->_menemyy;
		mx = Monst->_mx - fx;
		my = Monst->_my - fy;

		md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);
		Monst->_mdir = md;
		// Try attack
		if (DIST(mx,my,2)) {
			if (Monst->_mVar1 == MM_DELAY
				|| Monst->_mVar1 == MM_MISSILE
				|| random(105,100) < 20 + 1*Monst->_mint)
				M_StartAttack(i);
			else
				M_StartDelay(i, random(105,10)+10 - 1*Monst->_mint);
		}
		else if (DIST(mx,my,3)
				 && LineClearF1(PosOkMonst, i,Monst->_mx,Monst->_my,fx,fy)
				 && Monst->_mVar1 != MM_MISSILE)
		{
			// Launch snake
			if (AddMissile(Monst->_mx, Monst->_my, fx,fy, md, MIT_RHINO, pnum, i, 0, 0) != -1) {
				PlayEffect(i, MS_ATTACK);
				dMonster[Monst->_mx][Monst->_my] = -(i+1);
				Monst->_mmode = MM_MISSILE;
			}
		} else {
			// Try walk -- more likely to walk if already walking
			if ((Monst->_mVar1 != MM_DELAY) && random(106,100) < 35 - 2*Monst->_mint)
				M_StartDelay(i, random(106,10) + 15 - 1*Monst->_mint);
			else
			{
				md = Mod(md + pattern[Monst->_mgoalvar1], 8);
				if (++Monst->_mgoalvar1 > 5)
					Monst->_mgoalvar1 = 0;
				tmp = Mod(md - Monst->_mgoalvar2, 8);
				if (tmp > 0)
				{
					if (tmp < 4)
						Monst->_mgoalvar2 = Mod(Monst->_mgoalvar2 + 1, 8);
					else if (tmp == 4)
						Monst->_mgoalvar2 = md;
					else
						Monst->_mgoalvar2 = Mod(Monst->_mgoalvar2 - 1, 8);
				}
				app_assert(Monst->_mgoalvar2 >= 0 && Monst->_mgoalvar2 < 8);
				if (!M_DumbWalk(i, Monst->_mgoalvar2))
					M_CallWalk2(i, Monst->_mdir);
			}
		}
		// face dir
		if (Monst->_mmode == MM_STAND) Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[Monst->_mdir];
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_Bat(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Bat: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];
	int		mx, my, md, v, pnum;
	int fx, fy;

	pnum = Monst->_menemy;
	if ((Monst->_mmode == MM_STAND) && (Monst->_msquelch)) {
		mx = Monst->_mx - Monst->_menemyx;
		my = Monst->_my - Monst->_menemyy;
		md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);
		Monst->_mdir = md;
		v = random(107,100);
		if(Monst->_mgoal == MG_RUN_AWAY)
		{
			if(!Monst->_mgoalvar1)
			{
				M_CallWalk(i, opposite[md]);
				++Monst->_mgoalvar1;
			}
			else
			{
				if(random(108,2))
					M_CallWalk(i, left[md]);
				else
					M_CallWalk(i, right[md]);
				Monst->_mgoal = MG_ATTACK;
			}
		}
		else
		{
			fx = Monst->_menemyx;
			fy = Monst->_menemyy;
			// Try attack
			if ((Monst->MType->mtype == MT_GLOOM)
				&& (!DIST(mx,my,5) && v < 33 + 4*Monst->_mint) &&
				LineClearF1(PosOkMonst, i,Monst->_mx,Monst->_my,fx,fy))
			{
				// Turn into missile
				if (AddMissile(Monst->_mx, Monst->_my, fx, fy, md, MIT_RHINO, pnum, i, 0, 0) != -1) {
					dMonster[Monst->_mx][Monst->_my] = -(i+1);
					Monst->_mmode = MM_MISSILE;
				}
			}
			else if (DIST(mx,my,2)) {
				if (v < 8 + 4*Monst->_mint)
				{
					M_StartAttack(i);

					// After attacking, fly away
					Monst->_mgoal = MG_RUN_AWAY;
					Monst->_mgoalvar1 = 0;

					if(Monst->MType->mtype == MT_FAMILIAR) {
						// Note: arg 5 is a signal to the lightning code.
						//		 The code checks if this is != to arg 1, and if so, checks for
						//		 collision.  Normally, collision is only checked for when
						//		 lightning moves, but in this case it won't move!
						AddMissile(Monst->_menemyx, Monst->_menemyy, Monst->_menemyx+1, 0, -1, MIT_LIGHTNING, MI_ENEMYPLR, i, random(109,10)+1, 0);
					}
				}
			} else {
				// Try walk
				if ((Monst->_mVar2 > 20 &&  v < (13 + Monst->_mint)) || 
					(WALKMODE(Monst->_mVar1) &&
					  Monst->_mVar2 == 0 && v < (63 + Monst->_mint)))	{
					M_CallWalk(i, md);
				}
			}
			// face dir
			if (Monst->_mmode == MM_STAND) Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[md];
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_SkelBow(int i)
{
	int		mx, my, md, fx, fy;
	BOOL walking = FALSE;
	int v;
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_SkelBow: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];

	if ((Monst->_mmode == MM_STAND) && (Monst->_msquelch)) {
		mx = Monst->_mx - Monst->_menemyx;
		my = Monst->_my - Monst->_menemyy;
		md = M_GetDir(i);
		Monst->_mdir = md;

		// Try walk
		v = random(110,100);
		if (DIST(mx,my,4)
			&&
			((Monst->_mVar2 > 20
			  && v < 13 + 2*Monst->_mint)
			 || (WALKMODE(Monst->_mVar1)
				 && Monst->_mVar2 == 0
				 && v < 63 + 2*Monst->_mint)))
		{
			walking = M_DumbWalk(i, opposite[md]);
		}
		fx = Monst->_menemyx;
		fy = Monst->_menemyy;
		if (!walking && (random(110,100) < 3+2*Monst->_mint) &&
			LineClear(Monst->_mx,Monst->_my,fx, fy))
		{
			// Try attack
			M_StartRAttack(i,MIT_ARROW,4);
		}
		// face dir
		if (Monst->_mmode == MM_STAND) Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[md];
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_Fat(int i)
{
	int		mx, my, md, v;
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Fat: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];

	if ((Monst->_mmode == MM_STAND) && (Monst->_msquelch)) {
		mx = Monst->_mx - Monst->_menemyx;
		my = Monst->_my - Monst->_menemyy;
		md = M_GetDir(i);
		Monst->_mdir = md;
		v = random(111,100);
		// Try attack
		if (DIST(mx,my,2)) {
			if (v < (15 + 4*Monst->_mint)) M_StartAttack(i);
			else if (v < (20 + 4*Monst->_mint)) M_StartSpAttack(i);
		} else {
			// Try walk
			if ((Monst->_mVar2 > 20 &&  v < (20 + 4*Monst->_mint)) || 
				(WALKMODE(Monst->_mVar1) &&
				 Monst->_mVar2 == 0 && v < (70 + 4*Monst->_mint)))	{
				M_CallWalk(i, md);
			}
		}
		// face dir
		if (Monst->_mmode == MM_STAND) Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[md];
	}
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_Sneak(int i)
{
	int		mx, my, md, v;
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Sneak: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];
	int dist;

	if (Monst->_mmode == MM_STAND) {
		mx = Monst->_mx;
		my = Monst->_my;
		if (dLight[mx][my] != lightmax) {

			mx -= Monst->_menemyx;
			my -= Monst->_menemyy;
			md = M_GetDir(i);

			dist = 5 - Monst->_mint;
			
			// If gothit, run away
			if(Monst->_mVar1 == MM_GOTHIT)
			{
				Monst->_mgoal = MG_RUN_AWAY;
				Monst->_mgoalvar1 = 0;		// count run_away moves
			}
			// if far away enough, stop running away
			else if(!DIST(mx,my,dist+3) || Monst->_mgoalvar1 > 8)
			{
				Monst->_mgoal = MG_ATTACK;
				Monst->_mgoalvar1 = 0;
			}

			if(Monst->_mgoal == MG_RUN_AWAY && !(Monst->_mFlags & MFLAG_NOENEMY))
			{
				// Use special "owner location", i.e. the location of the player
				// according to the computer which owns the player
				// This is an attempt to reduce the amount of monster warping due to
				// diverging paths on different computers
				if (Monst->_mFlags & MFLAG_MID) {
					md = GetDirection(Monst->_mx, Monst->_my,
									  monster[Monst->_menemy]._mx, monster[Monst->_menemy]._my);
				}
				else {
					md = GetDirection(Monst->_mx, Monst->_my,
									  plr[Monst->_menemy]._pownerx, plr[Monst->_menemy]._pownery);
				}
				md = opposite[md];
				if(Monst->MType->mtype == MT_UNSEEN)
				{
					if(random(112,2))
						md = left[md];
					else
						md = right[md];
				}
			}

			Monst->_mdir = md;

			v = random(112,100);
			// become visible
			if(DIST(mx,my,dist) && (Monst->_mFlags & MFLAG_INVISIBLE))
				M_StartFadein(i,md,FALSE);
			// become invisible
			else if(!DIST(mx,my,dist+1) && !(Monst->_mFlags & MFLAG_INVISIBLE))
				M_StartFadeout(i,md,TRUE);
			// Try attack
			else if ((Monst->_mgoal == MG_RUN_AWAY)
					  ||
					  ( !DIST(mx,my,2)
			 			&&
						((Monst->_mVar2 > 20 &&  v < (14 + 4*Monst->_mint)) || 
						(WALKMODE(Monst->_mVar1) &&
						 Monst->_mVar2 == 0 && v < (64 + 4*Monst->_mint)))))	{
				// Try walk
				++Monst->_mgoalvar1;
				M_CallWalk(i, md);
			}
			// face dir
			if (Monst->_mmode == MM_STAND)
			{
				if (DIST(mx,my,2) && (v < (10 + 4*Monst->_mint))) {
					M_StartAttack(i);
				}
				else
				{
					Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[md];
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_Fireman(int i)
{
	int		mx, my, md, v, pnum;
	int fx, fy;
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Fireman: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];

	if (Monst->_mmode == MM_STAND && Monst->_msquelch) {
		pnum = Monst->_menemy;
		fx = Monst->_menemyx;
		fy = Monst->_menemyy;
		mx = Monst->_mx - fx;
		my = Monst->_my - fy;
		md = M_GetDir(i);

		if(Monst->_mgoal == MG_ATTACK)
		{
			if(LineClear(Monst->_mx,Monst->_my,fx,fy))
			{
				// Launch monster missile
				if (AddMissile(Monst->_mx, Monst->_my, fx,fy, md, MIT_FIREMAN, pnum, i, 0, 0) != -1) {
					Monst->_mmode = MM_MISSILE;
					Monst->_mgoal = MG_ATTACK2;
					Monst->_mgoalvar1 = 0;
				}
			}
		}
		else if(Monst->_mgoal == MG_ATTACK2)
		{
			if(Monst->_mgoalvar1 == 3)
			{
				Monst->_mgoal = MG_ATTACK;
				M_StartFadeout(i,md,TRUE);
			}
			else if(LineClear(Monst->_mx,Monst->_my,fx,fy))
			{
				// Try attack
				M_StartRAttack(i,MIT_KRULL,4);
				++Monst->_mgoalvar1;
			}
			else
			{
				M_StartDelay(i, random(112,10) + 5);
				++Monst->_mgoalvar1;
			}
		}

		else if(Monst->_mgoal == MG_RUN_AWAY)
		{
			M_StartFadein(i, md,FALSE);
			Monst->_mgoal = MG_ATTACK2;
		}

		Monst->_mdir = md;

		v = random(112,100);

		if(Monst->_mmode == MM_STAND)
		{
			if(DIST(mx,my,2) && Monst->_mgoal == MG_ATTACK)
			{
				M_TryH2HHit(i, monster[i]._menemy, monster[i].mHit, monster[i].mMinDamage, monster[i].mMaxDamage);
				Monst->_mgoal = MG_RUN_AWAY;
				if(!M_CallWalk(i, opposite[md]))
				{
					M_StartFadein(i, md,FALSE);
					Monst->_mgoal = MG_ATTACK2;
				}
			}
			else
			{
				if(!M_CallWalk(i, md))
				{
					if(Monst->_mgoal == MG_ATTACK || Monst->_mgoal == MG_RUN_AWAY)
					{
						M_StartFadein(i, md, FALSE);
						Monst->_mgoal = MG_ATTACK2;
					}
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_Fallen(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Fallen: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];
	int x,y;
	int xpos,ypos;
	int m;
	int rad;
	int mx,my;
	int aitype;

	if (Monst->_mgoal == MG_ATTACK2)
	{
		if(Monst->_mgoalvar1)
			--Monst->_mgoalvar1;
		else
			Monst->_mgoal = MG_ATTACK;
	}
	if ((Monst->_mmode == MM_STAND) && (Monst->_msquelch))
	{
		if (Monst->_mgoal == MG_RUN_AWAY && Monst->_mgoalvar1-- == 0)
		{
			Monst->_mgoal = MG_ATTACK;
			M_StartStand(i, opposite[Monst->_mdir]);
		}
		if (Monst->_mAnimFrame == Monst->_mAnimLen) {
			if (random(113,4) == 0)
			{
				// start 'dance'
				if (!(monster[i]._mFlags & MFLAG_NOHEAL)) {
					M_StartSpStand(i, Monst->_mdir);
					if(Monst->_mmaxhp - (2*Monst->_mint+2) >= Monst->_mhitpoints)
						Monst->_mhitpoints += 2*Monst->_mint+2;
					else
						Monst->_mhitpoints = Monst->_mmaxhp;
				}
				// set surrounding monsters into relentless attack
				rad = 2*Monst->_mint+4;
				for(y = -rad; y <= rad; ++y)
				{
					for(x = -rad; x <= rad; ++x)
					{
						xpos = Monst->_mx + x;
						ypos = Monst->_my + y;
						if(InBounds(x,y))
						{
							m = dMonster[xpos][ypos];
							if(m > 0)
							{
								--m;
								aitype = monster[m]._mAi;
								if(aitype == AI_FALLEN)
								{
									monster[m]._mgoal = MG_ATTACK2;
									// Set # frames for crazy attack mode
									monster[m]._mgoalvar1 = (2*Monst->_mint+7)*15;
								}
							}
						}
					}
				}
			}
		}
		else if (Monst->_mgoal == MG_RUN_AWAY)
		{
			M_CallWalk(i,Monst->_mdir);
		}
		else if (Monst->_mgoal == MG_ATTACK2)
		{
			// attack relentlessly
			mx = Monst->_mx - Monst->_menemyx;
			my = Monst->_my - Monst->_menemyy;
			if(DIST(mx,my,2))
				M_StartAttack(i);
			else
				M_CallWalk(i, M_GetDir(i));
		}
		else
			MAI_SkelSd(i);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_Cleaver(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Cleaver: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];
	int		mx, my, md;

	if (Monst->_mmode == MM_STAND && Monst->_msquelch) {
		mx = Monst->_mx - Monst->_menemyx;
		my = Monst->_my - Monst->_menemyy;
		md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);
		Monst->_mdir = md;
		// If close, attack
		if (DIST(mx,my,2)) {
			M_StartAttack(i);
		} else {
			// Try walk
			M_CallWalk(i, md);
		}
		// face dir
		if (Monst->_mmode == MM_STAND) Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[md];
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_Round(int i, BOOL special)
{
	int		mx, my, md, v;
	int fx, fy, dist;
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Round: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];

	if ((Monst->_mmode == MM_STAND) && (Monst->_msquelch)) {
		fx = Monst->_menemyx;
		fy = Monst->_menemyy;
		mx = Monst->_mx - fx;
		my = Monst->_my - fy;
		md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);

		if(Monst->_msquelch < 255)
			MonstCheckDoors(i);

		v = random(114,100);
		if (DIST(mx,my,2)
			|| Monst->_msquelch != 255
			|| dTransVal[Monst->_mx][Monst->_my]
					!= dTransVal[fx][fy]) {
			Monst->_mgoal = MG_ATTACK;
		}
		else if(Monst->_mgoal == MG_WALK_AROUND1
			|| (!DIST(mx,my,4) && !random(115,4)))
		{
			if(Monst->_mgoal != MG_WALK_AROUND1)
			{
				Monst->_mgoalvar1 = 0;			// reset counter
				// Pick direction to walk around player
				Monst->_mgoalvar2 = random(116,2);
			}

			// MODE CHANGE: Walk Around player

			Monst->_mgoal = MG_WALK_AROUND1;					

			// Calculate distance to player
			dist = max(abs(mx),abs(my));

			if(	(Monst->_mgoalvar1++ >= (dist << 1)
				 && DirOK(i,md))
				||  dTransVal[Monst->_mx][Monst->_my]
					!= dTransVal[fx][fy])
			{
				Monst->_mgoal = MG_ATTACK;
			}
			else if(!M_RoundWalk(i,md,Monst->_mgoalvar2))
				M_StartDelay(i, random(125,10)+10);
		}
		if(Monst->_mgoal == MG_ATTACK)
		{
			// Try attack
			if (DIST(mx,my,2)) {
				if (v < 23 + 2*Monst->_mint)
				{
					Monst->_mdir = md;
					if(special && Monst->_mhitpoints < (Monst->_mmaxhp >> 1) && random(117,2))
						M_StartSpAttack(i);
					else
						M_StartAttack(i);
				}
			}
			else if ((Monst->_mVar2 > 20 &&  v < (28 + 2*Monst->_mint)) || 
				(WALKMODE(Monst->_mVar1) &&
				  Monst->_mVar2 == 0 && v < (78 + 2*Monst->_mint)))	{
				M_CallWalk(i, md);
			}
		}

		// face dir
		if (Monst->_mmode == MM_STAND) Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[md];
	}
}

void MAI_GoatMc(int i)
{
	MAI_Round(i, TRUE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_Ranged(int i, int missile_type, BOOL special)
{
	int	fx, fy, mx, my, md;
	BOOL walking = FALSE;
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Ranged: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];

	if (Monst->_mmode == MM_STAND) {
		if (Monst->_msquelch == 255
			|| monster[i]._mFlags & MFLAG_MID) {
			fx = Monst->_menemyx;
			fy = Monst->_menemyy;
			mx = Monst->_mx - fx;
			my = Monst->_my - fy;
			md = M_GetDir(i);

			if(Monst->_msquelch < 255)
				MonstCheckDoors(i);

			Monst->_mdir = md;

			if (Monst->_mVar1 == MM_RATTACK)
			{
				// pause after each arrow
				M_StartDelay(i, random(118,20));
			}
			else if (DIST(mx,my,4) && random(119,100) < 70 + 10*Monst->_mint)
			{
				// retreat
				walking = M_CallWalk(i, opposite[md]);
			}
			if (Monst->_mmode == MM_STAND)
			{
				if (LineClear(Monst->_mx,Monst->_my,fx,fy))
				{
					// Try attack
					if(special)
						M_StartRSpAttack(i,missile_type,4);
					else
						M_StartRAttack(i,missile_type,4);
				}
				else
					// face dir
					 Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[md];
			}
		}
		else if(Monst->_msquelch // 0 < msquelch < 255
			    && !(monster[i]._mFlags & MFLAG_MID))	// temp. golum fix
		{
			mx = Monst->_lastx;
			my = Monst->_lasty;
			md = GetDirection(Monst->_mx, Monst->_my, mx, my);

			M_CallWalk(i, md);
		}
	}
}

void MAI_GoatBow(int i)
{
	MAI_Ranged(i,MIT_ARROW, FALSE);
}

void MAI_Succ(int i)
{
	MAI_Ranged(i,MIT_FLARE, FALSE);
}

void MAI_Lich(int i)
{
	MAI_Ranged(i,MIT_ORANGEFLARE, FALSE);
}

void MAI_ArchLich(int i)
{
	MAI_Ranged(i,MIT_YELLOWFLARE, FALSE);
}

void MAI_Psychorb(int i)
{
	MAI_Ranged(i,MIT_BLUEFLARE, FALSE);
}

void MAI_Necromorb(int i)
{
	MAI_Ranged(i,MIT_REDFLARE, FALSE);
}

void MAI_AcidUniq(int i)
{
	MAI_Ranged(i,MIT_ACID, TRUE);
}

void MAI_Firebat(int i)
{
	MAI_Ranged(i,MIT_FIREBOLT, FALSE);
}

void MAI_Hellbat(int i)
{
	MAI_Ranged(i,MIT_FIREBALL, FALSE);
}

/*-----------------------------------------------------------------------*
 *	MAI_Scav
 *
 *	MG_EAT is the scavenger's eat goal.
 *		_mgoalvar1 : =0  ==> no goal dest
 *					 >0  ==> _mgoalvar1-1 is x coord of food
 *		_mgoalvar2 :		 _mgoalvar2-1 is y coord of food
 *
**-----------------------------------------------------------------------*/

void MAI_Scav(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Scav: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];
	int x,y;
	BOOL done = FALSE;

	if (Monst->_mmode == MM_STAND) {
		if(Monst->_mhitpoints < (Monst->_mmaxhp >> 1) && Monst->_mgoal != MG_EAT)
		{
			if(monster[i].leaderflag)
			{
				// leave pack permanently
				--monster[monster[i].leader].packsize;
				monster[i].leaderflag = 0;
			}
			Monst->_mgoal = MG_EAT;
			Monst->_mgoalvar3 = 10;
		}

		if (Monst->_mgoal == MG_EAT && Monst->_mgoalvar3)
		{
			--Monst->_mgoalvar3;

			// Are we on top of food?
			if (dDead[Monst->_mx][Monst->_my])
			{
				M_StartEat(i);
				if (!(monster[i]._mFlags & MFLAG_NOHEAL)) {  
					int maxhp   = (Monst->MType->mMaxHP << HP_SHIFT);
					if (gbMaxPlayers == 1)
						maxhp >>= 1;
						
					int healamt = maxhp >> 3;
					Monst->_mhitpoints += healamt;
						
					// by eating, the monster can get to its species maximum
					// in hit points...
					if (Monst->_mhitpoints > maxhp)
						Monst->_mhitpoints = maxhp;
					if (Monst->_mmaxhp < Monst->_mhitpoints)	
						Monst->_mmaxhp = Monst->_mhitpoints;
							
					if (Monst->_mgoalvar3 <= 0 ||			// done eating
						Monst->_mhitpoints == maxhp)
					{
						dDead[Monst->_mx][Monst->_my] = 0;	// monster buried
					}
				}
//				if ((Monst->_mhitpoints) >= (Monst->_mmaxhp >> 1) + (Monst->_mmaxhp >> 2))
				if ((Monst->_mhitpoints) == (Monst->_mmaxhp))
				{
					Monst->_mgoal = MG_ATTACK;
					Monst->_mgoalvar1 = 0;
					Monst->_mgoalvar2 = 0;
				}
			}
			else
			{
				if (!Monst->_mgoalvar1)
				{
					// Find food!
					// Randomize search direction
					if(random(120,2))
					{
						for(y = -4; y <= 4 && !done; ++y)
							for(x = -4; x <= 4 && !done; ++x)
								if(InBounds(x,y))
									done = dDead[Monst->_mx + x][Monst->_my + y]
											&& LineClearF(CheckNoSolid, Monst->_mx,Monst->_my,Monst->_mx + x,Monst->_my + y);
						--x;
						--y;
					}
					else
					{
						for(y = 4; y >= -4 && !done; --y)
							for(x = 4; x >= -4 && !done; --x)
								if(InBounds(x,y))
									done = dDead[Monst->_mx + x][Monst->_my + y]
											&& LineClearF(CheckNoSolid,Monst->_mx,Monst->_my,Monst->_mx + x,Monst->_my + y);
						++x;
						++y;
					}
					if(done)
					{
						// Note: 1 is added to the following vars because
						//			0 is reserved as a flag meaning undefined.
						Monst->_mgoalvar1 = Monst->_mx + x +1;
						Monst->_mgoalvar2 = Monst->_my + y +1;
					}
				}
				if(Monst->_mgoalvar1)
				{
					x = Monst->_mgoalvar1 - 1;
					y = Monst->_mgoalvar2 - 1;
					Monst->_mdir = GetDirection(Monst->_mx, Monst->_my, x, y);
					M_CallWalk(i, Monst->_mdir);
				}
			}
		}
		else
//		if(Monst->_mmode == MM_STAND)
			// none of the above cases applied, so resort to default behavior
			MAI_SkelSd(i);
	}
}



/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_Garg(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Garg: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];
	int	mx, my;
	int md;

	mx = Monst->_mx - Monst->_lastx;
	my = Monst->_my - Monst->_lasty;
	md = M_GetDir(i);
	if(Monst->_msquelch && (Monst->_mFlags & MFLAG_STILL))
	{
		M_Enemy(i);
		mx = Monst->_mx - Monst->_menemyx;
		my = Monst->_my - Monst->_menemyy;

		if(DIST(mx,my,Monst->_mint+2))
			Monst->_mFlags &= ~MFLAG_STILL;
	}
	else if ((Monst->_mmode == MM_STAND) && (Monst->_msquelch))
	{
		if(Monst->_mhitpoints < (Monst->_mmaxhp >> 1))
		{
			// run away to get healed.
			Monst->_mgoal = MG_RUN_AWAY;
		}
		if(Monst->_mgoal == MG_RUN_AWAY)
		{
			if(DIST(mx,my,Monst->_mint+2))
			{
				if(!M_CallWalk(i,opposite[md]))
					Monst->_mgoal = MG_ATTACK;
			}
			else
			{
				Monst->_mgoal = MG_ATTACK;
				M_StartHeal(i);

			}
		}
		MAI_Round(i, FALSE);
	}
}


/*-----------------------------------------------------------------------*
 *	MAI_RoundRanged
 *
 *	Similar to MAI_Round, but does ranged attacks (throws lava)
 *
**-----------------------------------------------------------------------*/

void MAI_RoundRanged(int i, int missile_type, BOOL checkdoors, int dam, BOOL lessmissiles)
{
	int	fx, fy, mx, my, md, v, pnum;
	int dist;
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_RoundRanged: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];

	if ((Monst->_mmode == MM_STAND) && (Monst->_msquelch)) {
		pnum = Monst->_menemy;
		fx = Monst->_menemyx;
		fy = Monst->_menemyy;
		mx = Monst->_mx - fx;
		my = Monst->_my - fy;
		md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);

		if(checkdoors && Monst->_msquelch < 255)
			MonstCheckDoors(i);

		// Using random(10000) instead of random(100) for increased precision
		// for small numbers, e.g. random(100) < (5 >> 1)
		v = random(121,10000);
		if (DIST(mx,my,2)
			|| Monst->_msquelch != 255
			|| dTransVal[Monst->_mx][Monst->_my]
					!= dTransVal[fx][fy]) {
			Monst->_mgoal = MG_ATTACK;
		}
		else if(Monst->_mgoal == MG_WALK_AROUND1
			|| (!DIST(mx,my,3) && !random(122,4 << lessmissiles)))
		{
			if(Monst->_mgoal != MG_WALK_AROUND1)
			{
				Monst->_mgoalvar1 = 0;			// reset counter
				// Pick direction to walk around player
				Monst->_mgoalvar2 = random(123,2);
			}

			// MODE CHANGE: Walk Around player

			Monst->_mgoal = MG_WALK_AROUND1;					

			// Calculate distance to player
			dist = max(abs(mx),abs(my));

			if(Monst->_mgoalvar1++ >= (dist << 1)
				 && DirOK(i,md))
			{
				Monst->_mgoal = MG_ATTACK;
			}
			else if(v < (500 + 500*Monst->_mint) >> lessmissiles
					&& 
					LineClear(Monst->_mx, Monst->_my, fx, fy))
			{
				// Missile Attack
				M_StartRSpAttack(i,missile_type,dam);
			}
			else
				M_RoundWalk(i,md,Monst->_mgoalvar2);
		}
		if(Monst->_mgoal == MG_ATTACK)
		{
			// Try attack
			if (((!DIST(mx,my,3) && v < (1000 + 500*Monst->_mint) >> lessmissiles)
				 ||
				 v < (500 + 500*Monst->_mint) >> lessmissiles)
				&&
				LineClear(Monst->_mx,Monst->_my,fx,fy))
			{
				// Missile Attack
				M_StartRSpAttack(i,missile_type,dam);
			}
			else if (DIST(mx,my,2))
			{
				if (v < 6000 + 1000*Monst->_mint)
				{
					Monst->_mdir = md;
					M_StartAttack(i);
				}
			}
			else if (((v=random(124,100)) < (5000 + 1000*Monst->_mint))
					 || 
					 (WALKMODE(Monst->_mVar1)
					  &&
					  Monst->_mVar2 == 0 && v < (8000 + 1000*Monst->_mint)))
			{
						M_CallWalk(i, md);
			}
		}

		// face dir
		if (Monst->_mmode == MM_STAND)
			M_StartDelay(i, random(125,10)+5);
	}
}

void MAI_Magma(int i)
{
	MAI_RoundRanged(i, MIT_MAGMABALL, TRUE,4, FALSE);
}

void MAI_Storm(int i)
{
	MAI_RoundRanged(i, MIT_THINLIGHTCTRL, TRUE,4, FALSE);
}

void MAI_BoneDemon(int i)
{
	MAI_RoundRanged(i, MIT_BLUE2FLARE, TRUE,4, FALSE);
}

void MAI_Acid(int i)
{
	MAI_RoundRanged(i, MIT_ACID, FALSE,4, TRUE);
}

void MAI_Diablo(int i)
{
	MAI_RoundRanged(i, MIT_DIABAPOCA, FALSE, 40, FALSE);
}

void MAI_RR2(int i, int mistype, int dam)
{
	int	fx, fy, mx, my, md, v, pnum;
	int dist;
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_RR2: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];

	mx = Monst->_mx - Monst->_menemyx;
	my = Monst->_my - Monst->_menemyy;
	if (!DIST(mx, my, 5))
		MAI_SkelSd(i);
	else if ((Monst->_mmode == MM_STAND) && (Monst->_msquelch)) {
		pnum = Monst->_menemy;
		fx = Monst->_menemyx;
		fy = Monst->_menemyy;
		mx = Monst->_mx - fx;
		my = Monst->_my - fy;
		md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);

		if(Monst->_msquelch < 255)
			MonstCheckDoors(i);

		v = random(121,100);
		if (DIST(mx,my,2)
			|| Monst->_msquelch != 255
			|| dTransVal[Monst->_mx][Monst->_my]
					!= dTransVal[fx][fy]) {
			Monst->_mgoal = MG_ATTACK;
		}
		else if(Monst->_mgoal == MG_WALK_AROUND1
			|| !DIST(mx,my,3))
		{
			if(Monst->_mgoal != MG_WALK_AROUND1)
			{
				Monst->_mgoalvar1 = 0;			// reset counter
				// Pick direction to walk around player
				Monst->_mgoalvar2 = random(123,2);
			}

			// MODE CHANGE: Walk Around player

			Monst->_mgoal = MG_WALK_AROUND1;
			Monst->_mgoalvar3 = MG_WALK_AROUND1;

			// Calculate distance to player
			dist = max(abs(mx),abs(my));

			if(Monst->_mgoalvar1++ >= (dist << 1)
				 && DirOK(i,md))
			{
				Monst->_mgoal = MG_ATTACK;
			}
			else if (v < 80 + 5*Monst->_mint)
				M_RoundWalk(i,md,Monst->_mgoalvar2);
		}
		if(Monst->_mgoal == MG_ATTACK)
		{
			// Try attack
			if (((!DIST(mx,my,3) && v < 10 + 5*Monst->_mint)
				 ||
				 v < 5 + 5*Monst->_mint
				 || Monst->_mgoalvar3 == MG_WALK_AROUND1)
				&&
				LineClear(Monst->_mx,Monst->_my,fx,fy))
			{
				// Missile Attack
				M_StartRSpAttack(i,mistype,dam);
			}
			else if (DIST(mx,my,2))
			{
				if (random(124,100) < 40 + 10*Monst->_mint)
				{
					Monst->_mdir = md;
					if (random(124, 2))
						M_StartAttack(i);
					else
						// Missile Attack
						M_StartRSpAttack(i,mistype,dam);
				}
			}
			else if (((v=random(124,100)) < (50 + 10*Monst->_mint))
					 || 
					 (WALKMODE(Monst->_mVar1)
					  &&
					  Monst->_mVar2 == 0 && v < (80 + 10*Monst->_mint)))
			{
						M_CallWalk(i, md);
			}
			Monst->_mgoalvar3 = MG_ATTACK;
		}

		if (Monst->_mmode == MM_STAND)
			M_StartDelay(i, random(125,10)+5);
	}
}

void MAI_Mega(int i)
{
	MAI_RR2(i, MIT_FLAMEC,0);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MAI_Golum(int i)
{
	int ok, j, k, mid;
	int	mx, my, md;
	BOOL have_enemy;

	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Golum: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct *Monst = &monster[i];

	if ((Monst->_mx == 1) && (Monst->_my == 0)) return;
	
	if (Monst->_mmode == MM_DEATH) return;
	if (Monst->_mmode == MM_SPSTAND) return;
	if (Monst->_mmode >= MM_WALK && Monst->_mmode <= MM_WALK3) return;

	if (!(monster[i]._mFlags & MFLAG_MID)) M_Enemy(i);

	have_enemy = !(Monst->_mFlags & MFLAG_NOENEMY);

	if (Monst->_mmode != MM_ATTACK) {
		mx = Monst->_mx - monster[Monst->_menemy]._mfutx;
		my = Monst->_my - monster[Monst->_menemy]._mfuty;
		md = GetDirection(Monst->_mx, Monst->_my, monster[Monst->_menemy]._mx, monster[Monst->_menemy]._my);
		Monst->_mdir = md;

		if (DIST(mx,my,2) && have_enemy) {
			Monst->_menemyx = monster[Monst->_menemy]._mx;
			Monst->_menemyy = monster[Monst->_menemy]._my;
			if(!monster[Monst->_menemy]._msquelch) {
				monster[Monst->_menemy]._msquelch = 255;
				monster[Monst->_menemy]._lastx = Monst->_mx;
				monster[Monst->_menemy]._lasty = Monst->_my;
				for (j = 0; j < 5; ++j) {
					for (k = 0; k < 5; ++k) {
						mid = dMonster[monster[i]._mx - 2 + k][monster[i]._my - 2 + j];
						if (mid > 0) monster[mid]._msquelch = 255;
					}
				}
			}			
			M_StartAttack(i);
		} else {
			if (!have_enemy ||
				!MAI_Path(i)) {
				++Monst->_pathcount;
				if (Monst->_pathcount > 8) Monst->_pathcount = 5;
				ok = M_CallWalk(i, plr[i]._pdir);
				if (ok == FALSE) {
					md = (md - 1) & 0x07;
					for (j = 0; j < 8 && ok == FALSE; ++j) {
						md = (md + 1) & 0x07;
						ok = DirOK(i, md);
					}
					if (ok)
						M_WalkDir(i, md);
				}
			}
		}
	}

}
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_SkelKing(int i)
{
	int	fx, fy, mx, my, md, v;
	int dist;
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_SkelKing: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];
	int nx,ny;	// location for new skel spawn
	int skel;

	if ((Monst->_mmode == MM_STAND) && (Monst->_msquelch)) {
		fx = Monst->_menemyx;
		fy = Monst->_menemyy;
		mx = Monst->_mx - fx;
		my = Monst->_my - fy;
		md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);

		if(Monst->_msquelch < 255)
			MonstCheckDoors(i);

		v = random(126,100);
		if (DIST(mx,my,2)
			|| Monst->_msquelch != 255
			|| dTransVal[Monst->_mx][Monst->_my]
					!= dTransVal[fx][fy]) {
			Monst->_mgoal = MG_ATTACK;
		}
		else if(Monst->_mgoal == MG_WALK_AROUND1
			|| (!DIST(mx,my,3) && !random(127,4)))
		{
			if(Monst->_mgoal != MG_WALK_AROUND1)
			{
				Monst->_mgoalvar1 = 0;			// reset counter
				// Pick direction to walk around player
				Monst->_mgoalvar2 = random(128,2);
			}

			// MODE CHANGE: Walk Around player

			Monst->_mgoal = MG_WALK_AROUND1;					

			// Calculate distance to player
			dist = max(abs(mx),abs(my));

			if(	(Monst->_mgoalvar1++ >= (dist << 1)
				 && DirOK(i,md))
				||  dTransVal[Monst->_mx][Monst->_my]
					!= dTransVal[fx][fy])
			{
				Monst->_mgoal = MG_ATTACK;
			}
			else if(!M_RoundWalk(i,md,Monst->_mgoalvar2))
				M_StartDelay(i, random(125,10)+10);
		}
		if(Monst->_mgoal == MG_ATTACK)
		{
			// Try attack
			if (gbMaxPlayers == 1		// no spawning in multi-player
				&& ((!DIST(mx,my,3) && v < 35 + 4*Monst->_mint)
				    || v < 6)
				&&
				LineClear(Monst->_mx,Monst->_my,fx,fy))
			{
				// Spawn new Skeleton
				nx = Monst->_mx + offset_x[md];
				ny = Monst->_my + offset_y[md];
				if (PosOkMonst(i,nx,ny) && nummonsters < MAXMONSTERS)
				{
//						skel = AddMonster(nx, ny, md, 1, TRUE);
//						M_StartSpStand(skel, md);
					skel = M_SpawnSkel(nx, ny, md);
					M_StartSpStand(i, md);
				}
			}
			else if (DIST(mx,my,2)) {
				if (v < 20 + Monst->_mint)
				{
					Monst->_mdir = md;
					M_StartAttack(i);
				}
			}
			else if (((v=random(129,100)) < (25 + Monst->_mint)) || 
				(WALKMODE(Monst->_mVar1) &&
				  Monst->_mVar2 == 0 && v < (75 + Monst->_mint)))	{
				M_CallWalk(i, md);
			}
			else
				M_StartDelay(i, random(130,10)+10);
		}

		// face dir
		if (Monst->_mmode == MM_STAND)
			Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[md];
	}
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_Rhino(int i)
{
	int	fx, fy, mx, my, md, v;
	int dist;
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Rhino: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];

	if (Monst->_mmode == MM_STAND && Monst->_msquelch) {
		fx = Monst->_menemyx;
		fy = Monst->_menemyy;
		mx = Monst->_mx - fx;
		my = Monst->_my - fy;
		md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);

		if(Monst->_msquelch < 255)
			MonstCheckDoors(i);

		v = random(131,100);
		if (DIST(mx,my,2)) {
			Monst->_mgoal = MG_ATTACK;
		}
		else if(Monst->_mgoal == MG_WALK_AROUND1
			|| (!DIST(mx,my,5) && random(132,4)))
		{
 			if(Monst->_mgoal != MG_WALK_AROUND1)
			{
				Monst->_mgoalvar1 = 0;			// reset counter
				// Pick direction to walk around player
				Monst->_mgoalvar2 = random(133,2);
			}

			// MODE CHANGE: Walk Around player

			Monst->_mgoal = MG_WALK_AROUND1;					

			// Calculate distance to player
			dist = max(abs(mx),abs(my));

			if(	(Monst->_mgoalvar1++ >= (dist << 1))
				||  dTransVal[Monst->_mx][Monst->_my]
					!= dTransVal[fx][fy])
			{
				Monst->_mgoal = MG_ATTACK;
			}
			else if(!M_RoundWalk(i,md,Monst->_mgoalvar2))
				M_StartDelay(i, random(125,10)+10);
		}
		if(Monst->_mgoal == MG_ATTACK)
		{
			// Try attack
			if ((!DIST(mx,my,5) && v < 43 + 2*Monst->_mint) &&
				LineClearF1(PosOkMonst, i,Monst->_mx,Monst->_my,fx,fy))
			{
				// Launch rhino
				if (AddMissile(Monst->_mx, Monst->_my, fx,fy, md, MIT_RHINO, Monst->_menemy, i, 0, 0) != -1) {
					if (Monst->MData->snd_special) PlayEffect(i, MS_SATTACK);
					dMonster[Monst->_mx][Monst->_my] = -(i+1);
					Monst->_mmode = MM_MISSILE;
				}
			}
			else if (DIST(mx,my,2)) {
				if (v < 28 + 2*Monst->_mint)
				{
					Monst->_mdir = md;
					M_StartAttack(i);
				}
			}
			else if (((v=random(134,100)) < (33 + 2*Monst->_mint)) || 
				(WALKMODE(Monst->_mVar1) &&
				  Monst->_mVar2 == 0 && v < (83 + 2*Monst->_mint)))	{
				M_CallWalk(i, md);
			}
			else
				M_StartDelay(i, random(135,10)+10);
		}

		// face dir
		if (Monst->_mmode == MM_STAND) Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[Monst->_mdir];
	}
}

void MAI_HorkDemon(int i)
{
	int v;

	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_HorkDemon: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];

	if (Monst->_mmode == MM_STAND && Monst->_msquelch) {
		int const fx = Monst->_menemyx;
		int const fy = Monst->_menemyy;
		int const mx = Monst->_mx - fx;
		int const my = Monst->_my - fy;
		int const md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);

		if(Monst->_msquelch < 255)
			MonstCheckDoors(i);

		v = random(131,100);
		if (DIST(mx,my,2)) {
			Monst->_mgoal = MG_ATTACK;
		}
		else if(Monst->_mgoal == MG_WALK_AROUND1
			|| (!DIST(mx,my,5) && random(132,4)))
		{
 			if(Monst->_mgoal != MG_WALK_AROUND1)
			{
				Monst->_mgoalvar1 = 0;			// reset counter
				// Pick direction to walk around player
				Monst->_mgoalvar2 = random(133,2);
			}

			// MODE CHANGE: Walk Around player

			Monst->_mgoal = MG_WALK_AROUND1;					

			// Calculate distance to player
			int const dist = max(abs(mx),abs(my));

			if(	(Monst->_mgoalvar1++ >= (dist << 1))
				||  dTransVal[Monst->_mx][Monst->_my]
					!= dTransVal[fx][fy])
			{
				Monst->_mgoal = MG_ATTACK;
			}
			else if(!M_RoundWalk(i,md,Monst->_mgoalvar2))
				M_StartDelay(i, random(125,10)+10);
		}
		if(Monst->_mgoal == MG_ATTACK)
		{
			// Try attack
			if ((!DIST(mx,my,3) && v < 43 + 2*Monst->_mint))
			{
				// Launch Hork Spawn
				int const Enemyd = Monst->_mdir;
				int const nx = Monst->_mx + infront_x[Enemyd];
				int const ny = Monst->_my + infront_y[Enemyd];
				if (PosOkMonst(i,nx,ny) && nummonsters < MAXMONSTERS) {
					M_StartRSpAttack(i, MIT_HORKSPAWN, 0);
				}
			}
			else if (DIST(mx,my,2)) {
				if (v < 28 + 2*Monst->_mint)
				{
					Monst->_mdir = md;
					M_StartAttack(i);
				}
			}
			else if (((v=random(134,100)) < (33 + 2*Monst->_mint)) || 
				(WALKMODE(Monst->_mVar1) &&
				  Monst->_mVar2 == 0 && v < (83 + 2*Monst->_mint)))	{
				M_CallWalk(i, md);
			}
			else
				M_StartDelay(i, random(135,10)+10);
		}

		// face dir
		if (Monst->_mmode == MM_STAND) Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[Monst->_mdir];
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_Counselor(int i)
{
	int	fx, fy, mx, my, md, v;
	int dist;
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Counselor: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];
	static const BYTE counsmiss[] = { MIT_FIREBOLT, MIT_CBOLT, MIT_LIGHTCTRL, MIT_FIREBALL };

	if ((Monst->_mmode == MM_STAND) && (Monst->_msquelch)) {
		fx = Monst->_menemyx;
		fy = Monst->_menemyy;
		mx = Monst->_mx - fx;
		my = Monst->_my - fy;
		md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);

		if(Monst->_msquelch < 255)
			MonstCheckDoors(i);

		v = random(121,100);
		if (Monst->_mgoal == MG_RUN_AWAY)
		{
			if (Monst->_mgoalvar1++ > 3)
			{
				Monst->_mgoal = MG_ATTACK;
				M_StartFadein(i, md, TRUE);
			}
			else
				M_CallWalk(i, opposite[md]);
		}
		else if(Monst->_mgoal == MG_WALK_AROUND1)
		{
			// Calculate distance to player
			dist = max(abs(mx),abs(my));

			if (DIST(mx,my,2)
				|| Monst->_msquelch != 255
				|| dTransVal[Monst->_mx][Monst->_my]
						!= dTransVal[fx][fy]) {
				Monst->_mgoal = MG_ATTACK;
				M_StartFadein(i, md, TRUE);
			}
			else if(Monst->_mgoalvar1++ >= (dist << 1)
				 && DirOK(i,md))
			{
				Monst->_mgoal = MG_ATTACK;
				M_StartFadein(i, md, TRUE);
 			}
			else
				M_RoundWalk(i,md,Monst->_mgoalvar2);
		}
		else if(Monst->_mgoal == MG_ATTACK)
		{
			// Try attack
			if (DIST(mx,my,2))
			{
				Monst->_mdir = md;
				if (Monst->_mhitpoints < (Monst->_mmaxhp >> 1))
				{
					// run away!
					Monst->_mgoal = MG_RUN_AWAY;
					Monst->_mgoalvar1 = 0;
					M_StartFadeout(i, md, FALSE);
				}
				else if (Monst->_mVar1 == MM_DELAY || random(105,100) < 20 + 2*Monst->_mint)
				{
					M_StartRAttack(i,-1,0);
					AddMissile(monster[i]._mx, monster[i]._my, 0, 0, monster[i]._mdir, MIT_FLASH, MI_ENEMYPLR, i, 4, 0);
					AddMissile(monster[i]._mx, monster[i]._my, 0, 0, monster[i]._mdir, MIT_FLASH2, MI_ENEMYPLR, i, 4, 0);
				}
				else
					M_StartDelay(i, random(105,10)+10 - 2*Monst->_mint);
			}
			else if (v < 50 + 5*Monst->_mint
					&& LineClear(Monst->_mx,Monst->_my,fx,fy))
			{
				// Missile Attack
				M_StartRAttack(i, counsmiss[Monst->_mint], random(77, Monst->mMaxDamage - Monst->mMinDamage + 1) + Monst->mMinDamage);
			}
			else if (random(124, 100) < 30)
			{
				Monst->_mgoal = MG_WALK_AROUND1;
				Monst->_mgoalvar1 = 0;
				M_StartFadeout(i, md, FALSE);
			}
			else
				M_StartDelay(i, random(105,10)+10 - 2*Monst->_mint);
		}

		// face dir
		if (Monst->_mmode == MM_STAND)
			M_StartDelay(i, random(125,10)+5);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_Garbud(int i)
{
	int		mx, my, md;
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Garbud: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];

	if (Monst->_mmode == MM_STAND) {
		mx = Monst->_mx;
		my = Monst->_my;
		md = M_GetDir(i);
		if ((Monst->mtalkmsg < TXT_GARB4) && (Monst->mtalkmsg > (TXT_GARB1-1)) && !(dFlags[mx][my] & BFLAG_MONSTACTIVE) && (Monst->_mgoal == MG_WAITTOTALK)) {
			++Monst->mtalkmsg;
			Monst->_mgoal = MG_TALK;
		}			
		if (dFlags[mx][my] & BFLAG_MONSTACTIVE) {
			mx = Monst->_mx - Monst->_menemyx;
			my = Monst->_my - Monst->_menemyy;
			#if !IS_VERSION(SHAREWARE)
			if ((Monst->mtalkmsg == TXT_GARB4) && !(effect_is_playing(USFX_GARBUD4)) && (Monst->_mgoal == MG_WAITTOTALK)) {
				Monst->_mgoal = MG_ATTACK;
				Monst->_msquelch = 255;
				Monst->mtalkmsg = 0;			
			}
			#endif
		}
		if ((Monst->_mgoal == MG_ATTACK) || (Monst->_mgoal == MG_WALK_AROUND1)) {
			MAI_Round(i, TRUE);
		}		
		monster[i]._mdir = md;
		if (Monst->_mmode == MM_STAND) 	Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[md];
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_Zhar(int i)
{
	int		mx, my, md, dist;
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Zhar: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];

	if (Monst->_mmode == MM_STAND) {
		mx = Monst->_mx;
		my = Monst->_my;
		md = M_GetDir(i);
		if ((Monst->mtalkmsg == TXT_ZHAR1) && !(dFlags[mx][my] & BFLAG_MONSTACTIVE) && (Monst->_mgoal == MG_WAITTOTALK)) {
			++Monst->mtalkmsg;
			Monst->_mgoal = MG_TALK;
		}			
		if (dFlags[mx][my] & BFLAG_MONSTACTIVE) {
			mx = Monst->_mx - Monst->_menemyx;
			my = Monst->_my - Monst->_menemyy;
			dist = max(abs(mx), abs(my));
			#if !IS_VERSION(SHAREWARE)
			if ((Monst->mtalkmsg == TXT_ZHAR2) && !(effect_is_playing(USFX_ZHAR2)) && (Monst->_mgoal == MG_WAITTOTALK)) {
				Monst->_mgoal = MG_ATTACK;
				Monst->_msquelch = 255;
				Monst->mtalkmsg = 0;			
			}
			#endif
		}
		if ((Monst->_mgoal == MG_ATTACK) || (Monst->_mgoal == MG_RUN_AWAY) || (Monst->_mgoal == MG_WALK_AROUND1)) {
			MAI_Counselor(i);
		}		
		monster[i]._mdir = md;
		if (Monst->_mmode == MM_STAND) Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[md];
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_SnotSpil(int i)
{
	int		mx, my, md, pnum;
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_SnotSpil: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];
	pnum = Monst->_menemy;

	if (Monst->_mmode == MM_STAND) {
		mx = Monst->_mx;
		my = Monst->_my;
		md = M_GetDir(i);
		if ((Monst->mtalkmsg == TXT_BOL1) && !(dFlags[mx][my] & BFLAG_MONSTACTIVE) && (Monst->_mgoal == MG_WAITTOTALK)) {
			Monst->mtalkmsg = TXT_BOL2;
			Monst->_mgoal = MG_TALK;
		}
		if ((Monst->mtalkmsg == TXT_BOL2) && (quests[Q_LTBANNER]._qvar1 == 3)) {
			Monst->mtalkmsg = 0;
			Monst->_mgoal = MG_ATTACK;
		}
		if (dFlags[mx][my] & BFLAG_MONSTACTIVE) {
			mx = Monst->_mx - Monst->_menemyx;
			my = Monst->_my - Monst->_menemyy;
			#if !IS_VERSION(SHAREWARE)
			if ((Monst->mtalkmsg == TXT_BOL3) && !(effect_is_playing(USFX_SNOT3)) && (Monst->_mgoal == MG_WAITTOTALK)) {
				int i = plr[pnum]._pvid;
				ObjChangeMap(setpc_x, setpc_y, setpc_x + setpc_w + 1, setpc_y + setpc_h + 1);
				quests[Q_LTBANNER]._qvar1 = 3;
				extern void RedoPlayerVision();
				RedoPlayerVision();
				Monst->_mgoal = MG_ATTACK;
				Monst->_msquelch = 255;
				Monst->mtalkmsg = 0;
			}
			#endif
			if (quests[Q_LTBANNER]._qvar1 == 3) {
				if ((Monst->_mgoal == MG_ATTACK) || (Monst->_mgoal == MG_ATTACK2)) {
					MAI_Fallen(i);
				}
			}		

		}
		monster[i]._mdir = md;
		if (Monst->_mmode == MM_STAND) Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[md];
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_Lazurus(int i)
{
	int		mx, my, md;
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Lazurus: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];

	if (Monst->_mmode == MM_STAND) {
		mx = Monst->_mx;
		my = Monst->_my;
		md = M_GetDir(i);
		if (dFlags[mx][my] & BFLAG_MONSTACTIVE) {
			mx = Monst->_mx - Monst->_menemyx;
			my = Monst->_my - Monst->_menemyy;
			if (gbMaxPlayers == 1) {
				if ((Monst->mtalkmsg == TXT_VB1) && (Monst->_mgoal == MG_TALK) 
					&& (plr[myplr]._px == 35) && (plr[myplr]._py == 46)) {
					PlayInGameMovie("gendata\\fprst3.smk");
					Monst->_mmode = MM_TALK;
					quests[Q_BETRAYER]._qvar1 = 5;
				}
				#if !IS_VERSION(SHAREWARE)
				if ((Monst->mtalkmsg == TXT_VB1) && !(effect_is_playing(USFX_LAZ1)) && (Monst->_mgoal == MG_WAITTOTALK)) {
					ObjChangeMapResync(1, 18, 20, 24);
					extern void RedoPlayerVision();
					RedoPlayerVision();
					quests[Q_BETRAYER]._qvar1 = 6;
					Monst->_mgoal = MG_ATTACK;
					Monst->_msquelch = 255;
					Monst->mtalkmsg = 0;			
				}
				#endif
			}
			if (gbMaxPlayers != 1) {
				#if !IS_VERSION(SHAREWARE)
				if ((Monst->mtalkmsg == TXT_VB1) && (Monst->_mgoal == MG_TALK) && (quests[Q_BETRAYER]._qvar1 <= 3)) {
					Monst->_mmode = MM_TALK;
				}
				#endif
			}

		}
		if ((Monst->_mgoal == MG_ATTACK) || (Monst->_mgoal == MG_RUN_AWAY) || (Monst->_mgoal == MG_WALK_AROUND1)) {
			MAI_Counselor(i);
		}		
		monster[i]._mdir = md;
		if ((Monst->_mmode == MM_STAND) || (Monst->_mmode == MM_TALK)) Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[md];
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_Lazhelp(int i)
{
	int mx, my, md;
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Lazhelp: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];

	if (Monst->_mmode == MM_STAND) {
		mx = Monst->_mx;
		my = Monst->_my;
		md = M_GetDir(i);
		if (dFlags[mx][my] & BFLAG_MONSTACTIVE) {
			mx = Monst->_mx - Monst->_menemyx;
			my = Monst->_my - Monst->_menemyy;		
			if (gbMaxPlayers == 1) {
				if (quests[Q_BETRAYER]._qvar1 <= 5)
					Monst->_mgoal = MG_TALK;
				else {
					Monst->_mgoal = MG_ATTACK;
					Monst->mtalkmsg = 0;
				}
			} else if (gbMaxPlayers != 1) {
				Monst->_mgoal = MG_ATTACK;
			}
		}
		if (Monst->_mgoal == MG_ATTACK) {
			MAI_Succ(i);
		}
		monster[i]._mdir = md;
	}	if (Monst->_mmode == MM_STAND) Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[md];
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_Lachdanan(int i)
{
	int		mx, my, md, pnum;
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Lachdanan: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];
	pnum = Monst->_menemy;

	if (Monst->_mmode == MM_STAND) {
		mx = Monst->_mx;
		my = Monst->_my;
		md = M_GetDir(i);
		#if !IS_VERSION(SHAREWARE)
		if ((Monst->mtalkmsg == TXT_VEIL1) && !(dFlags[mx][my] & BFLAG_MONSTACTIVE) && (Monst->_mgoal == MG_WAITTOTALK)) {
			++Monst->mtalkmsg;
			Monst->_mgoal = MG_TALK;			
		}
		#endif
		if (dFlags[mx][my] & BFLAG_MONSTACTIVE) {
			mx = Monst->_mx - Monst->_menemyx;
			my = Monst->_my - Monst->_menemyy;
			#if !IS_VERSION(SHAREWARE)
			if ((Monst->mtalkmsg == TXT_VEIL3)  && !(effect_is_playing(USFX_LACH3)) && (Monst->_mgoal == MG_WAITTOTALK)) {
				Monst->mtalkmsg = 0;
				quests[Q_VEIL]._qactive = QUEST_DONE;
				M_StartKill(i, -1);
			}
			#endif
		}		
		monster[i]._mdir = md;
		if (Monst->_mmode == MM_STAND)	Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[md];
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MAI_Warlord(int i)
{
	int		mx, my, md;
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MAI_Warlord: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];

	if (Monst->_mmode == MM_STAND) {
		mx = Monst->_mx;
		my = Monst->_my;
		md = M_GetDir(i);
		if (dFlags[mx][my] & BFLAG_MONSTACTIVE) {
			mx = Monst->_mx - Monst->_menemyx;
			my = Monst->_my - Monst->_menemyy;
			if ((Monst->mtalkmsg == TXT_WARLRD1) && (Monst->_mgoal == MG_TALK)) {
				Monst->_mmode = MM_TALK;
			}

			#if !IS_VERSION(SHAREWARE)
			BOOL effect_is_playing(int nSFX);
			if ((Monst->mtalkmsg == TXT_WARLRD1) && !(effect_is_playing(USFX_WARLRD1)) && (Monst->_mgoal == MG_WAITTOTALK)) {
				Monst->_mgoal = MG_ATTACK;
				Monst->_msquelch = 255;
				Monst->mtalkmsg = 0;			
			}
			#endif
		}
		if (Monst->_mgoal == MG_ATTACK) {
			MAI_SkelSd(i);
		}		
		monster[i]._mdir = md;
		if ((Monst->_mmode == MM_STAND) || (Monst->_mmode == MM_TALK)) Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Cels[md];
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void DeleteMonsterList()
{
	int i, mi;

	// pseudo delete dead golems
	for (i = 0; i < 4; ++i) {
		if (monster[i]._mDelFlag) {
			monster[i]._mx = 1;
			monster[i]._my = 0;
			monster[i]._mfutx = 0;
			monster[i]._mfuty = 0;
			monster[i]._moldx = 0;
			monster[i]._moldy = 0;
			monster[i]._mDelFlag = FALSE;
		}
	}

	i = 4; // this is to skip the golems
	app_assert((DWORD)nummonsters <= MAXMONSTERS);
	while (i < nummonsters) {
		mi = monstactive[i];
		if (monster[mi]._mDelFlag) {
			DeleteMonster(i);
			i = 0;
		} else ++i;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void ProcessMonsters ()
{
	int		i, mi;
	int		raflag;
	int		mx, my;
	MonsterStruct	*Monst;
	int oldmode;

	DeleteMonsterList();
	app_assert((DWORD)nummonsters <= MAXMONSTERS);
	for (i = 0; i < nummonsters; ++i) {
		mi = monstactive[i];
		Monst = &monster[mi];
		raflag = RUN_DONE;

		// Try and run same AI's as best as possible in multiplayer
		if (gbMaxPlayers > 1) {
			SetRndSeed(Monst->_mAISeed);
			Monst->_mAISeed = GetRndSeed();
		}

		// Turn regen
		if (!(monster[mi]._mFlags & MFLAG_NOHEAL)) {
			if ((Monst->_mhitpoints < Monst->_mmaxhp) && ((Monst->_mhitpoints >> HP_SHIFT) > 0)) {
				if (Monst->mLevel > 1)
					Monst->_mhitpoints += Monst->mLevel >> 1;
				else
					Monst->_mhitpoints += Monst->mLevel;
			}
		}
		// Check if just activating
		mx = Monst->_mx;
		my = Monst->_my;
		if ((dFlags[mx][my] & BFLAG_MONSTACTIVE) && (Monst->_msquelch == 0)) {
			#if !IS_VERSION(SHAREWARE)
			if (Monst->MType->mtype == MT_CLEAVER) {
				//if (gbMaxPlayers == 1) PlayInGameMovie("gendata\\fbutch3.smk");
				PlaySFX(USFX_CLEAVER);
			}
			if (Monst->MType->mtype == MT_NKR) {
				if (gbCowsuit)
					PlaySFX(HSFX_NA_KRUL6);
				else if (Na_Krul.Books)
					PlaySFX(HSFX_NA_KRUL4);
				else	
					PlaySFX(HSFX_NA_KRUL5);
			}
			if (Monst->MType->mtype == MT_BUG) {
				//if (gbMaxPlayers == 1) PlayInGameMovie("gendata\\fbutch3.smk");
				PlaySFX(HSFX_DEFILER5);
			}
			#endif
			M_Enemy(mi);
		}

		// Check if monster's enemy 
		if (Monst->_mFlags & MFLAG_MID) {
			if ((DWORD)Monst->_menemy >= MAXMONSTERS)
			{
#if defined(_DEBUG)
				app_fatal("Illegal enemy monster %d for monster \"%s\"",Monst->_menemy,Monst->mName);
#else
				return;
#endif
			}
			Monst->_menemyx = Monst->_lastx = monster[Monst->_menemy]._mfutx;
			Monst->_menemyy = Monst->_lasty = monster[Monst->_menemy]._mfuty;
		} else {
			// Deal with keeping a monster active for a while after its visibility flag
			//		goes off
			if ((DWORD)Monst->_menemy >= MAX_PLRS)
			{
#if defined(_DEBUG)
				app_fatal("Illegal enemy player %d for monster \"%s\"",Monst->_menemy,Monst->mName);
#else
				return;
#endif
			}
			Monst->_menemyx = plr[Monst->_menemy]._pfutx;
			Monst->_menemyy = plr[Monst->_menemy]._pfuty;
			if (dFlags[mx][my] & BFLAG_MONSTACTIVE)	{
				Monst->_msquelch = 255;
				Monst->_lastx = plr[Monst->_menemy]._pfutx;
				Monst->_lasty = plr[Monst->_menemy]._pfuty;
			} else if(Monst->_msquelch && (Monst->_mAi != MT_DIABLO)) --Monst->_msquelch;
		}

		do {
			// Run Monster's AI

			// Try Path Mode
			if (Monst->_mFlags & MFLAG_PATH)
			{
				if (!MAI_Path(mi))
					AiProc[Monst->_mAi](mi);
			}
			else
				AiProc[Monst->_mAi](mi);

			// Run Monster Mode
			switch (oldmode = Monst->_mmode) {
				case MM_STAND :
					raflag = M_DoStand(mi);
					break;
				case MM_WALK :
					raflag = M_DoWalk(mi);
					break;
				case MM_WALK2 :
					raflag = M_DoWalk2(mi);
					break;
				case MM_WALK3:
					raflag = M_DoWalk3(mi);
					break;
				case MM_ATTACK:
					raflag = M_DoAttack(mi);
					break;
				case MM_RATTACK:
					raflag = M_DoRAttack(mi);
					break;
				case MM_GOTHIT:
					raflag = M_DoGotHit(mi);
					break;
				case MM_DEATH:
					raflag = M_DoDeath(mi);
					break;
				case MM_SATTACK:
					raflag = M_DoSAttack(mi);
					break;
				case MM_FADEIN:
					raflag = M_DoFadein(mi);
					break;
				case MM_FADEOUT:
					raflag = M_DoFadeout(mi);
					break;
				case MM_SPSTAND:
					raflag = M_DoSpStand(mi);
					break;
				case MM_RSATTACK:
					raflag = M_DoRSpAttack(mi);
					break;
				case MM_DELAY:
					raflag = M_DoDelay(mi);
					break;
				case MM_MISSILE:
					raflag = RUN_DONE;
					break;
				case MM_STONE:
					raflag = M_DoStone(mi);
					break;
				case MM_HEAL:
					raflag = M_DoHeal(mi);
					break;
				case MM_TALK:
					raflag = M_DoTalk(mi);
					break;
			}

/*			for (int mm = 0; mm < nummonsters; ++mm) {
				int mmi = monstactive[mm];
				app_assert(monster[mmi]._mDelFlag
						|| monster[mmi]._mmode == MM_MISSILE
						|| !monster[mmi]._msquelch
						|| mmi < 4
						|| dMonster[monster[mmi]._mx][monster[mmi]._my] == mmi+1
						|| dMonster[monster[mmi]._mx][monster[mmi]._my] == -(mmi+1));
			}*/

			if(raflag != RUN_DONE)
				GroupUnity(mi);

//			if (Monst->_msquelch) DaveMonstMap(FALSE, 0);

		} while (raflag != RUN_DONE);

		// Animate Monster
		if (Monst->_mmode != MM_STONE) {
			++Monst->_mAnimCnt;
			if (!(Monst->_mFlags & MFLAG_STILL))
			{
				if (Monst->_mAnimCnt >= Monst->_mAnimDelay) {
					Monst->_mAnimCnt = 0;
					if(Monst->_mFlags & MFLAG_BACKWARDS)
					{
						--Monst->_mAnimFrame;
						if (!Monst->_mAnimFrame) Monst->_mAnimFrame = Monst->_mAnimLen;
					}
					else
					{
						++Monst->_mAnimFrame;
						if (Monst->_mAnimFrame > Monst->_mAnimLen) Monst->_mAnimFrame = 1;
					}
				}
			}
		}
	}
	DeleteMonsterList();
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void FreeMonsterGFX()
{
	app_assert((DWORD)nummtypes <= MAX_LVLMTYPES);
	for (int monst = 0; monst < nummtypes; ++monst) {
		int mtype = Monsters[monst].mtype;
		for (int anim = 0; anim < MAX_ANIMTYPE; ++anim) {
			if(!(animletter[anim] == 's' && !monsterdata[mtype].has_special)) {
				DiabloFreePtr(Monsters[monst].Anims[anim].CMem);
			}
		}
	}

	IFreeMissileGFX();
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
BOOL DirOK(int i, int mdir)
{
	long fx, fy;
	int tmp;

	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("DirOK: Invalid monster %d",i);
#else
		return FALSE;
#endif
	}
	fx = monster[i]._mx + offset_x[mdir];
	fy = monster[i]._my + offset_y[mdir];
	if(! InBounds(fx,fy))
		return FALSE;

	if (! PosOkMonst(i, fx, fy))
		return FALSE;

	if (mdir == M_DIRR) {
		if (SolidLoc(fx+0,fy+1)) return FALSE;
		if (dFlags[fx+0][fy+1] & BFLAG_MONSTLR) return FALSE;
	}
	else if(mdir == M_DIRL) {
		if (SolidLoc(fx+1,fy+0)) return FALSE;
		if (dFlags[fx+1][fy+0] & BFLAG_MONSTLR) return FALSE;
	}
	else if (mdir == M_DIRU) {
		if (SolidLoc(fx+1,fy+0)) return FALSE;
		if (SolidLoc(fx+0,fy+1)) return FALSE;
	}
	else if (mdir == M_DIRD) {
		if (SolidLoc(fx-1,fy+0)) return FALSE;
		if (SolidLoc(fx+0,fy-1)) return FALSE;
	}

	// check for group cohesion
	if(monster[i].leaderflag == PACK_MEMBER) {
		// make sure monster is close to leader
		return DIST(
			fx-monster[monster[i].leader]._mfutx, 
			fy-monster[monster[i].leader]._mfuty,
			4
		);
	}

	if (monster[i]._uniqtype && (UniqMonst[monster[i]._uniqtype-1].mUnqAttr & UN_STICK))
	{
		int mcount = 0;
		for (int x = fx - 3; x <= fx + 3; ++x) {
			for (int y = fy - 3; y <= fy + 3; ++y) {
				if (! InBounds(x,y)) continue;

				if((tmp = dMonster[x][y]) < 0) tmp = -tmp;
				if (tmp != 0) --tmp;
				app_assert(tmp >= 0);

				if (monster[tmp].leaderflag == PACK_MEMBER &&
					monster[tmp].leader == i &&
					monster[tmp]._mfutx == x &&
					monster[tmp]._mfuty == y) ++mcount;
			}
		}

		return (mcount == monster[i].packsize);
	}

	return TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL PosOkMissile(int x, int y)
{
	return !(nMissileTable[dPiece[x][y]] || (dFlags[x][y] & BFLAG_MONSTLR));
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL CheckNoSolid(int x, int y)
{
	return !nSolidTable[dPiece[x][y]];
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*

BOOL LineClearF(CHECKFUNC Clear, int x1, int y1, int x2, int y2)
{
	int md;
	BOOL done = FALSE;

	do
	{
		md = GetDirection(x1,y1,x2,y2);
		x1 += offset_x[md];
		y1 += offset_y[md];
		done = !(*Clear)(x1, y1);
	} while(!done && !(x1==x2 && y1==y2));
	return (x1 == x2) && (y1 == y2);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL LineClearF(CHECKFUNC Clear, int x1, int y1, int x2, int y2)
{
	// Bresenham line algorithm
	// See Foley/van Dam, "Computer Graphics: Principles and Practice"

	int dx,dy;
	int d;			// test variable
	int dincH;		// Horizontal increment for d
	int dincD;		// Diagonal increment for d
	int xincD,yincD;	// Diagonal increments for minor axis
	int xorg, yorg;
	BOOL done = FALSE;
	int tmp;

	xorg = x1;
	yorg = y1;

	dx = x2 - x1;
	dy = y2 - y1;
	if(abs(dx) >  abs(dy))
	{
		// X is the major axis
		if(dx < 0)
		{
			// swap endpoints
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;

			dx = -dx;
			dy = -dy;
		}
		if(dy > 0)
		{
			// pos. slope
			d = 2*dy - dx;
			dincH = 2*dy;
			dincD = 2*(dy - dx);
			yincD = 1;
		}
		else
		{
			// neg. slope
			d = 2*dy + dx;
			dincH = 2*dy;
			dincD = 2*(dy + dx);
			yincD = -1;
		}

//		done = (x1 != xorg || y1 != yorg) && !(*Clear)(x1, y1);

		while(!done && !(x1 == x2 && y1 == y2))
		{
			if((d <= 0) ^ (yincD < 0))
			{
				d += dincH;
			}
			else
			{
				d += dincD;
				y1 += yincD;
			}
			++x1;
			done = (x1 != xorg || y1 != yorg) && !(*Clear)(x1, y1);
		}
	}
	else
	{
		// Y is the major axis
		if(dy < 0)
		{
			// swap endpoints
			tmp = y1;
			y1 = y2;
			y2 = tmp;
			tmp = x1;
			x1 = x2;
			x2 = tmp;

			dy = -dy;
			dx = -dx;
		}
		if(dx > 0)
		{
			// pos. slope
			d = 2*dx - dy;
			dincH = 2*dx;
			dincD = 2*(dx - dy);
			xincD = 1;
		}
		else
		{
			// neg. slope
			d = 2*dx + dy;
			dincH = 2*dx;
			dincD = 2*(dx + dy);
			xincD = -1;
		}

//		done = (y1 != yorg || x1 != xorg) && !(*Clear)(x1, y1);

		while(!done && !(y1 == y2 && x1 == x2))
		{
			if((d <= 0) ^ (xincD < 0))
			{
				d += dincH;
			}
			else
			{
				d += dincD;
				x1 += xincD;
			}
			++y1;
			done = (y1 != yorg || x1 != xorg) && !(*Clear)(x1, y1);
		}
	}

	return x1 == x2 && y1 == y2;
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL LineClear(int x1, int y1, int x2, int y2)
{
	return LineClearF(PosOkMissile, x1, y1, x2, y2);
}

/*-----------------------------------------------------------------------*
 * LineClearF1
 *
 *	This is exactly the same routine as LineClearF, except its Check
 *	routine requires an extra argument
**-----------------------------------------------------------------------*/

BOOL LineClearF1(CHECKFUNC1 Clear, int monst, int x1, int y1, int x2, int y2)
{
	// Bresenham line algorithm
	// See Foley/van Dam, "Computer Graphics: Principles and Practice"

	int dx,dy;
	int d;			// test variable
	int dincH;		// Horizontal increment for d
	int dincD;		// Diagonal increment for d
	int xincD,yincD;	// Diagonal increments for minor axis
	int xorg, yorg;
	BOOL done = FALSE;
	int tmp;

	xorg = x1;
	yorg = y1;

	dx = x2 - x1;
	dy = y2 - y1;
	if(abs(dx) >  abs(dy))
	{
		// X is the major axis
		if(dx < 0)
		{
			// swap endpoints
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;

			dx = -dx;
			dy = -dy;
		}
		if(dy > 0)
		{
			// pos. slope
			d = 2*dy - dx;
			dincH = 2*dy;
			dincD = 2*(dy - dx);
			yincD = 1;
		}
		else
		{
			// neg. slope
			d = 2*dy + dx;
			dincH = 2*dy;
			dincD = 2*(dy + dx);
			yincD = -1;
		}

		while(!done && !(x1 == x2 && y1 == y2))
		{
			if((d <= 0) ^ (yincD < 0))
			{
				d += dincH;
			}
			else
			{
				d += dincD;
				y1 += yincD;
			}
			++x1;
			done = (x1 != xorg || y1 != yorg) && !Clear(monst, x1, y1);
		}
	}
	else
	{
		// Y is the major axis
		if(dy < 0)
		{
			// swap endpoints
			tmp = y1;
			y1 = y2;
			y2 = tmp;
			tmp = x1;
			x1 = x2;
			x2 = tmp;

			dy = -dy;
			dx = -dx;
		}
		if(dx > 0)
		{
			// pos. slope
			d = 2*dx - dy;
			dincH = 2*dx;
			dincD = 2*(dx - dy);
			xincD = 1;
		}
		else
		{
			// neg. slope
			d = 2*dx + dy;
			dincH = 2*dx;
			dincD = 2*(dx + dy);
			xincD = -1;
		}

		while(!done && !(y1 == y2 && x1 == x2))
		{
			if((d <= 0) ^ (xincD < 0))
			{
				d += dincH;
			}
			else
			{
				d += dincD;
				x1 += xincD;
			}
			++y1;
			done = (y1 != yorg || x1 != xorg) && !Clear(monst, x1, y1);
		}
	}

	return x1 == x2 && y1 == y2;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SyncMonsterAnim(int m)
{
	int	dir;

	if ((DWORD)m >= MAXMONSTERS || m < 0)
	{
#if defined(_DEBUG)
		app_fatal("SyncMonsterAnim: Invalid monster %d",m);
#else
		return;
#endif
	}
		
	app_assert(monster[m]._mMTidx < MAX_LVLMTYPES
				&& monster[m]._mMTidx >= 0);
	monster[m].MType = &Monsters[monster[m]._mMTidx];
	monster[m].MData = Monsters[monster[m]._mMTidx].MData;
	if(monster[m]._uniqtype)
		monster[m].mName = UniqMonst[monster[m]._uniqtype-1].mName;
	else {
		app_assert(monster[m].MData != 0);
		monster[m].mName = monster[m].MData->mName;
	}

	dir = monster[m]._mdir;
	switch (monster[m]._mmode) {
		case MM_STAND :
			monster[m]._mAnimData = monster[m].MType->Anims[MA_STAND].Cels[dir];
			break;
		case MM_WALK :
			monster[m]._mAnimData = monster[m].MType->Anims[MA_WALK].Cels[dir];
			break;
		case MM_WALK2 :
			monster[m]._mAnimData = monster[m].MType->Anims[MA_WALK].Cels[dir];
			break;
		case MM_WALK3:
			monster[m]._mAnimData = monster[m].MType->Anims[MA_WALK].Cels[dir];
			break;
		case MM_ATTACK:
			monster[m]._mAnimData = monster[m].MType->Anims[MA_ATTACK].Cels[dir];
			break;
		case MM_RATTACK:
			monster[m]._mAnimData = monster[m].MType->Anims[MA_ATTACK].Cels[dir];
			break;
		case MM_GOTHIT:
			monster[m]._mAnimData = monster[m].MType->Anims[MA_GOTHIT].Cels[dir];
			break;
		case MM_DEATH:
			monster[m]._mAnimData = monster[m].MType->Anims[MA_DEATH].Cels[dir];
			break;
		case MM_SATTACK:
			monster[m]._mAnimData = monster[m].MType->Anims[MA_SPECIAL].Cels[dir];
			break;
		case MM_FADEIN:
			monster[m]._mAnimData = monster[m].MType->Anims[MA_SPECIAL].Cels[dir];
			break;
		case MM_FADEOUT:
			monster[m]._mAnimData = monster[m].MType->Anims[MA_SPECIAL].Cels[dir];
			break;
		case MM_SPSTAND:
			monster[m]._mAnimData = monster[m].MType->Anims[MA_SPECIAL].Cels[dir];
			break;
		case MM_RSATTACK:
			monster[m]._mAnimData = monster[m].MType->Anims[MA_SPECIAL].Cels[dir];
			break;
		case MM_DELAY:
			monster[m]._mAnimData = monster[m].MType->Anims[MA_STAND].Cels[dir];
			break;
		case MM_HEAL:
			monster[m]._mAnimData = monster[m].MType->Anims[MA_SPECIAL].Cels[dir];
			break;
		case MM_TALK:
			monster[m]._mAnimData = monster[m].MType->Anims[MA_STAND].Cels[dir];
			break;
		case MM_STONE:
			monster[m]._mAnimData = monster[m].MType->Anims[MA_STAND].Cels[dir];
			monster[m]._mAnimFrame = 1;
			monster[m]._mAnimLen = monster[m].MType->Anims[MA_STAND].Frames;
			break;
		case MM_MISSILE:
			monster[m]._mAnimData = monster[m].MType->Anims[MA_ATTACK].Cels[dir];
			monster[m]._mAnimFrame = 1;
			monster[m]._mAnimLen = monster[m].MType->Anims[MA_ATTACK].Frames;
			break;
		default:
			monster[m]._mAnimData = monster[m].MType->Anims[MA_STAND].Cels[dir];
			monster[m]._mAnimFrame = 1;
			monster[m]._mAnimLen = monster[m].MType->Anims[MA_STAND].Frames;
			break;
	}
}


/*-----------------------------------------------------------------------*
*		When monster gets killed, make the fallen run in fear
**-----------------------------------------------------------------------*/
void M_FallenFear(int x, int y)
{
	int		i, mi;
	int		rundist;
	int		aitype;

	app_assert((DWORD)nummonsters <= MAXMONSTERS);
	for (i = 0; i < nummonsters; ++i) {
		mi = monstactive[i];
		rundist = 0;
		switch(monster[mi].MType->mtype) {
			case MT_RFALLSD :
			case MT_RFALLSP :
				rundist = 7;
				break;
			case MT_DFALLSD :
			case MT_DFALLSP :
				rundist = 5;
				break;
			case MT_YFALLSD :
			case MT_YFALLSP :
				rundist = 3;
				break;
			case MT_BFALLSD :
			case MT_BFALLSP :
				rundist = 2;
				break;
		}

		aitype = monster[mi]._mAi;
		if(aitype == AI_FALLEN) {
			if (rundist
				&& DIST(x-monster[mi]._mx, y-monster[mi]._my, 5)
				&& ((monster[mi]._mhitpoints >> HP_SHIFT) > 0))
			{
				monster[mi]._mgoal = MG_RUN_AWAY;
				monster[mi]._mgoalvar1 = rundist;   // run away for 'rundist' squares
				// next direction will be away from monster	that got hit
				monster[mi]._mdir = GetDirection(x, y, monster[i]._mx, monster[i]._my);
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void PrintMonstHistory(int mt)
{
	int res, minhp, maxhp;

	sprintf(tempstr, "Total kills : %i", monstkills[mt]);
	AddPanelString(tempstr, TEXT_CENTER);
	if (monstkills[mt] >= 30) {
		minhp = monsterdata[mt].mMinHP;
		maxhp = monsterdata[mt].mMaxHP;
		if (gbMaxPlayers == 1) {
			minhp = minhp >> 1;
			maxhp = maxhp >> 1;
		}
		if (minhp < 1) minhp = 1;
		if (maxhp < 1) maxhp = 1;
		if (gnDifficulty == D_NIGHTMARE) {
			minhp = (minhp * 3) + ((gbMaxPlayers == 1) ? 50 : 100);
			maxhp = (maxhp * 3) + ((gbMaxPlayers == 1) ? 50 : 100);
		}
		else if (gnDifficulty == D_HELL) {
			minhp = (minhp * 4) + ((gbMaxPlayers == 1) ? 100 : 200);
			maxhp = (maxhp * 4) + ((gbMaxPlayers == 1) ? 100 : 200);
		}
		sprintf(tempstr, "Hit Points : %i-%i", minhp, maxhp);
		AddPanelString(tempstr, TEXT_CENTER);
	}
	/*minhp = monster[cursmonst]._mhitpoints >> HP_SHIFT;
	maxhp = monster[cursmonst]._mmaxhp >> HP_SHIFT;
	sprintf(tempstr, "Hit Points : %i of %i", minhp, maxhp);
	AddPanelString(tempstr, TEXT_CENTER);*/
	if (monstkills[mt] >= 15) {
		if (gnDifficulty != D_HELL)
			res = monsterdata[mt].mMagicRes;
		else
			res = monsterdata[mt].mMagicRes2;
		res &= (M_RM|M_RF|M_RL|M_IM|M_IF|M_IL);
		if (res == M_NONE) {
			strcpy(tempstr, "No magic resistance");
			AddPanelString(tempstr, TEXT_CENTER);
		} else {
			if (res & (M_RM|M_RF|M_RL)) {
				strcpy(tempstr, "Resists : ");
				if (res & M_RM) strcat(tempstr, "Magic ");
				if (res & M_RF) strcat(tempstr, "Fire ");
				if (res & M_RL) strcat(tempstr, "Lightning ");
				tempstr[strlen(tempstr)-1] = 0;
				AddPanelString(tempstr, TEXT_CENTER);
			}
			if (res & (M_IM|M_IF|M_IL)) {
				strcpy(tempstr, "Immune : ");
				if (res & M_IM) strcat(tempstr, "Magic ");
				if (res & M_IF) strcat(tempstr, "Fire ");
				if (res & M_IL) strcat(tempstr, "Lightning ");
				tempstr[strlen(tempstr)-1] = 0;
				AddPanelString(tempstr, TEXT_CENTER);
			}
		}
	}
	pinfoflag = TRUE;
}	

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void PrintUniqueHistory()
{
	int res;

	/*int minhp = monster[cursmonst]._mhitpoints >> HP_SHIFT;
	int maxhp = monster[cursmonst]._mmaxhp >> HP_SHIFT;
	sprintf(tempstr, "Hit Points : %i of %i", minhp, maxhp);
	AddPanelString(tempstr, TEXT_CENTER);*/
	res = monster[cursmonst].mMagicRes;
	res &= (M_RM|M_RF|M_RL|M_IM|M_IF|M_IL);
	if (res == M_NONE) {
		strcpy(tempstr, "No resistances");
		AddPanelString(tempstr, TEXT_CENTER);
		strcpy(tempstr, "No Immunities");
		AddPanelString(tempstr, TEXT_CENTER);
	} else {
		if (res & (M_RM|M_RF|M_RL))
			strcpy(tempstr, "Some Magic Resistances");
		else
			strcpy(tempstr, "No resistances");
		AddPanelString(tempstr, TEXT_CENTER);
		if (res & (M_IM|M_IF|M_IL))
			strcpy(tempstr, "Some Magic Immunities");
		else
			strcpy(tempstr, "No Immunities");
		AddPanelString(tempstr, TEXT_CENTER);
	}
	pinfoflag = TRUE;
}	

/*-----------------------------------------------------------------------*
 *	MissToMonst
 *
 *	Used when Rhino charges and collides into something.
**-----------------------------------------------------------------------*/

void MissToMonst(int i, int x, int y)
{
	int oldx,oldy;
	int newx,newy;
	if ((DWORD)i >= MAXMISSILES)
	{
#if defined(_DEBUG)
		app_fatal("MissToMonst: Invalid missile %d",i);
#else
		return;
#endif
	}
	MissileStruct	*Miss =  &missile[i];
	int m = Miss->_misource;
	if ((DWORD)m >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("MissToMonst: Invalid monster %d",m);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[m];
	int pnum;

	app_assert(Monst->_mmode == MM_MISSILE);

	oldx = Miss->_mix;
	oldy = Miss->_miy;

	dMonster[x][y] = m+1;

	Monst->_mdir = Miss->_mimfnum;
	Monst->_mx = x;
	Monst->_my = y;
	M_StartStand(m, Monst->_mdir);

	if(EquivMonst(Monst->MType->mtype, MT_INCIN))
		M_StartFadein(m, Monst->_mdir, FALSE);
	else {
		if ((Monst->_mFlags & MFLAG_MID) == 0)
			M_StartHit(m,-1,0);
		else
			M2MStartHit(m,-1,0);
	}

	if ((Monst->_mFlags & MFLAG_MID) == 0) {
		pnum = dPlayer[oldx][oldy]-1;
		if((dPlayer[oldx][oldy] > 0)
			&& (Monst->MType->mtype != MT_GLOOM)
			&& !EquivMonst(Monst->MType->mtype, MT_INCIN))
		{
			M_TryH2HHit(m, dPlayer[oldx][oldy]-1, 500, Monst->mMinDamage2, Monst->mMaxDamage2);

			// make sure player didn't go change location during h2hhit
			if (pnum == dPlayer[oldx][oldy]-1
				&& !EquivMonst(Monst->MType->mtype, MT_NSNAKE))
			{
				// make sure player is doing a hit animation
				if (plr[pnum]._pmode != PM_GOTHIT && plr[pnum]._pmode != PM_DEATH)
					StartPlrHit(pnum, 0, TRUE);

				// knock opponent back one square
				newx = oldx + offset_x[Monst->_mdir];
				newy = oldy + offset_y[Monst->_mdir];
				if(PosOkPlayer(pnum, newx, newy))
				{
					plr[pnum]._px = newx;
					plr[pnum]._py = newy;
					FixPlayerLocation(pnum,plr[pnum]._pdir);
					FixPlrWalkTags(pnum);
					dPlayer[newx][newy] = pnum + 1;
					SetPlayerOld(pnum);
				}
			}
		}
	} else {
		if((dMonster[oldx][oldy] > 0)
			&& (Monst->MType->mtype != MT_GLOOM) && !EquivMonst(Monst->MType->mtype, MT_INCIN))
		{
			M_TryM2MHit(m, dMonster[oldx][oldy]-1, 500, Monst->mMinDamage2, Monst->mMaxDamage2);
			if (!EquivMonst(Monst->MType->mtype, MT_NSNAKE))
			{
				// knock opponent back one square
				newx = oldx + offset_x[Monst->_mdir];
				newy = oldy + offset_y[Monst->_mdir];
				if(PosOkMonst(dMonster[oldx][oldy]-1, newx, newy))
				{
					// This assumes that monster got hit (since we use a 500% hit above)
					// so we don't have to clean up his area, and we assume pnum is positive
					pnum = dMonster[newx][newy] = dMonster[oldx][oldy];
					dMonster[oldx][oldy] = 0;
					--pnum;
					monster[pnum]._mfutx = monster[pnum]._mx = newx;
					monster[pnum]._mfuty = monster[pnum]._my = newy;
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static bool CheckForMissileWalls(int i, int x, int y)
{
	bool ret = true;
	int mi = dMissile[x][y];
	
	if(mi && i >= 0)
	{
		bool fire = false;
		bool lightwall = false;
		
		if(mi > 0) {
			if(missile[mi]._mitype == MIT_FIREWALL)
				fire = true;
			else if ( missile[mi]._mitype == MIT_LIGHTWALL)
				lightwall = true;
		} else {
			for(mi = 0; mi < nummissiles; ++mi) {
				int const mx = missileactive[mi];
				
				if (missile[mx]._mix == x &&
				    missile[mx]._miy == y) {
					if(missile[mx]._mitype == MIT_FIREWALL) {
						fire = true;
						break;
					}
					if (missile[mx]._mitype == MIT_LIGHTWALL) {
						lightwall = true;
						break;
					}
				}
			}
		}
		if ((fire && !(monster[i].mMagicRes & M_IF)) || (fire && (monster[i].MType->mtype == MT_DIABLO)))
			ret = FALSE;
		if ((lightwall && !(monster[i].mMagicRes & M_IL)) || (lightwall && (monster[i].MType->mtype == MT_DIABLO)))
			ret = FALSE;
	}

	return ret;
}
/*-----------------------------------------------------------------------*
 *	PosOkMonst
 *
 *	Map position (x,y) is ok for placement of a monster.
**-----------------------------------------------------------------------*/

BOOL PosOkMonst(int i, int x, int y)
{
	BOOL ret = TRUE;
	ret = !SolidLoc(x, y) && !dPlayer[x][y] && !dMonster[x][y];
	int oi = dObject[x][y];

	if(ret && oi)
	{
		if (oi > 0) oi -= 1;
		else oi = -(oi + 1);
		
		if (object[oi]._oSolidFlag) ret = FALSE;
	}
	
	if(ret)
	{
		ret = CheckForMissileWalls(i, x, y);
	}

	return ret;
}

/*-----------------------------------------------------------------------*
 *	PosOkMonst2
 *
 *	Map position (x,y) is ok for placement of a monster.
 *  Same as PosOkMonst, except ignores other monsters and players
**-----------------------------------------------------------------------*/

BOOL PosOkMonst2(int i, int x, int y)
{
	BOOL ret = TRUE;
	int oi = dObject[x][y];

	ret = !SolidLoc(x, y);

	if(ret && oi)
	{
		if (oi > 0) oi -= 1;
		else oi = -(oi + 1);
		
		if (object[oi]._oSolidFlag) ret = FALSE;
	}
	if(ret)
	{
		ret = CheckForMissileWalls(i, x, y);
	}

	return ret;
}

/*-----------------------------------------------------------------------*
 *	PosOkMonst
 *
 *	Map position (x,y) is ok for placement of a monster.
 *  Same as PosOkMonst3, except ignores doors
**-----------------------------------------------------------------------*/

BOOL PosOkMonst3(int i, int x, int y)
{
	BOOL ret = TRUE;
	bool isdoor = false;
	int oi = dObject[x][y];

	if(oi)
	{
		if (oi > 0) oi -= 1;
		else oi = -(oi + 1);
		
		int const objtype = object[oi]._otype;
		isdoor = (objtype == OBJ_L1DOORL
					|| objtype == OBJ_L1DOORR
					|| objtype == OBJ_L2DOORL
					|| objtype == OBJ_L2DOORR
					|| objtype == OBJ_L3DOORL
					|| objtype == OBJ_L3DOORR);
		if (object[oi]._oSolidFlag && !isdoor)
			ret = FALSE;
	}

	if (ret)
		ret = (!SolidLoc(x, y) || isdoor) && !dPlayer[x][y] && !dMonster[x][y];

	if(ret)
	{
		ret = CheckForMissileWalls(i, x, y);
	}

	return ret;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL IsSkel(int mt)
{
	return EquivMonst(mt, MT_WSKELAX)
			||  EquivMonst(mt, MT_WSKELBW)
			||	EquivMonst(mt, MT_WSKELSD);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL IsGoat(int mt)
{
	return EquivMonst(mt, MT_NGOATMC)
			||  EquivMonst(mt, MT_NGOATBW);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int M_SpawnSkel(int x, int y, int dir)
{
	int i,j;
	int skeltypes = 0;
	int skel;

	app_assert((DWORD)nummtypes <= MAX_LVLMTYPES);
	for (i = 0; i < nummtypes; ++i)
		if (IsSkel(Monsters[i].mtype))
			++skeltypes;

	if (skeltypes)
	{
		j = random(136,skeltypes);
		skeltypes = 0;
		for (i = 0; i < nummtypes && skeltypes <= j; ++i)
			if(IsSkel(Monsters[i].mtype))
				++skeltypes;
		--i;
		skel = AddMonster(x, y, dir, i, TRUE);
		if (skel != -1) M_StartSpStand(skel, dir);
		return skel;
	}
	else
		return -1;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void ActivateSpawn(int i, int x, int y, int dir)
{
	dMonster[x][y] = i + 1;
	monster[i]._mx = x;
	monster[i]._my = y;
	monster[i]._mfutx = x;
	monster[i]._mfuty = y;
	monster[i]._moldx = x;
	monster[i]._moldy = y;
	M_StartSpStand(i, dir);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL SpawnSkeleton(int ii, int x, int y)
{
	int monstok[3][3];
	int i,j,xx,yy,rs;
	BOOL savail;

	if (ii == -1) return FALSE;
	// Try location first
	if (PosOkMonst(-1, x, y)) {
		ActivateSpawn(ii, x, y, GetDirection(x, y, x, y));
	} else {
		// Try surrounding squares
		savail = FALSE;
		yy = 0;
		for (j = (y-1); j <= (y+1); ++j) {
			xx = 0;
			for (i = (x-1); i <= (x+1); ++i) {
				monstok[xx][yy] = PosOkMonst(-1, i,j);
				savail |= monstok[xx][yy];
				++xx;
			}
			++yy;
		}

		// No fit, no good
		if (!savail) return(FALSE);

		// Place skeleton
		rs = random(137,15) + 1;
		xx = 0;
		yy = 0;
		while (rs > 0) {
			if (monstok[xx][yy]) --rs;
			if (rs > 0) {
				++xx;
				if (xx == 3) {
					xx = 0;
					++yy;
					if (yy == 3) yy = 0;
				}
			}
		}
		xx = xx + x - 1;
		yy = yy + y - 1;
		ActivateSpawn(ii, xx, yy, GetDirection(xx, yy, x, y));
	}
	return(TRUE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int PreSpawnSkeleton()
{
	int i,j;
	int skeltypes = 0;
	int skel;

	app_assert((DWORD)nummtypes <= MAX_LVLMTYPES);
	for (i = 0; i < nummtypes; ++i)
		if (IsSkel(Monsters[i].mtype))
			++skeltypes;

	if (skeltypes)
	{
		j = random(136,skeltypes);
		skeltypes = 0;
		for (i = 0; i < nummtypes && skeltypes <= j; ++i)
		{
			if(IsSkel(Monsters[i].mtype))
				++skeltypes;
		}
		--i;
		skel = AddMonster(0, 0, 0, i, FALSE);
		if (skel != -1) M_StartStand(skel, 0);
		return skel;
	}
	else
		return -1;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void TalktoMonster(int i) 
{
	int pnum, itm;
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("TalktoMonster: Invalid monster %d",i);
#else
		return;
#endif
	}
	MonsterStruct	*Monst = &monster[i];
	pnum = Monst->_menemy;

	Monst->_mmode = MM_TALK;

	if ((Monst->_mAi != AI_SNOTSPIL) && (Monst->_mAi != AI_LACHDANAN))
		return;

	if (QuestStatus(Q_LTBANNER)) {
		if ((quests[Q_LTBANNER]._qvar1 == 2) && (PlrHasItem(pnum, IDI_BANNER, itm))) {
			RemoveInvItem(pnum, itm);
			quests[Q_LTBANNER]._qactive = QUEST_DONE;
			Monst->mtalkmsg = TXT_BOL3;
			Monst->_mgoal = MG_TALK;
		}
	}
	if (QuestStatus(Q_VEIL)) {
		if ((Monst->mtalkmsg >= TXT_VEIL1) && (PlrHasItem(pnum, IDI_GLDNELIX, itm))) {
			RemoveInvItem(pnum, itm);
			Monst->mtalkmsg = TXT_VEIL3;
			Monst->_mgoal = MG_TALK;
		}
	}
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SpawnGolum(int i, int x, int y, int mi)
{
	if ((DWORD)i >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("SpawnGolum: Invalid monster %d",i);
#else
		return;
#endif
	}
	dMonster[x][y] = i + 1;
	monster[i]._mx = x;
	monster[i]._my = y;
	monster[i]._mfutx = x;
	monster[i]._mfuty = y;
	monster[i]._moldx = x;
	monster[i]._moldy = y;
	monster[i]._pathcount = 0;
	
	monster[i]._mmaxhp = ((plr[i]._pMaxMana/3)<<1) + (((missile[mi]._mispllvl << HP_SHIFT) << 3) + ((missile[mi]._mispllvl << HP_SHIFT) << 1));
	monster[i]._mhitpoints = monster[i]._mmaxhp;
	monster[i].mArmorClass = 25;
	monster[i].mHit = 40 + (plr[i]._pLevel << 1) + ((missile[mi]._mispllvl << 2) + (missile[mi]._mispllvl));
	monster[i].mMinDamage = 8 + (missile[mi]._mispllvl << 1);
	monster[i].mMaxDamage = 16 + (missile[mi]._mispllvl << 1);
	
	monster[i]._mFlags |= MFLAG_MKILLER;
	M_StartSpStand(i, 0);
	M_Enemy(i);
	
	void NetSendCmdGolem(BYTE, BYTE, BYTE, BYTE, long, BYTE);
	if (i == myplr) 
		NetSendCmdGolem(monster[i]._mx, monster[i]._my, monster[i]._mdir, monster[i]._menemy, monster[i]._mhitpoints, currlevel);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL CanTalkToMonst(int m)
{
	if ((DWORD)m >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("CanTalkToMonst: Invalid monster %d",m);
#else
		return FALSE;
#endif
	}
	if (monster[m]._mgoal == MG_TALK) return(TRUE);
	if (monster[m]._mgoal == MG_WAITTOTALK) return(TRUE);
	return(FALSE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL CheckMonsterHit(int m, BOOL &ret)
{
	if ((DWORD)m >= MAXMONSTERS)
	{
#if defined(_DEBUG)
		app_fatal("CheckMonsterHit: Invalid monster %d",m);
#else
		return FALSE;
#endif
	}
	if (monster[m]._mAi == AI_GARG && (monster[m]._mFlags & MFLAG_STILL))
	{
		monster[m]._mFlags &= ~MFLAG_STILL;
		monster[m]._mmode = MM_SATTACK;
		ret = TRUE;
		return(TRUE);
	}
	else if(EquivMonst(monster[m].MType->mtype, MT_COUNSLR)
			&& (monster[m]._mgoal != MG_ATTACK))
	{
		ret = FALSE;
		return TRUE;
	}
	return FALSE;
}

//******************************************************************
//******************************************************************
int encode_enemy(int m)
{
	if (monster[m]._mFlags & MFLAG_MID)
		return monster[m]._menemy + MAX_PLRS;
	else
		// enemy is player, guaranteed < MAX_PLRS
		return monster[m]._menemy;
}

//******************************************************************
//******************************************************************
void decode_enemy(int m, int enemy)
{
	if (enemy < MAX_PLRS) {
		monster[m]._mFlags &= ~MFLAG_MID;
		monster[m]._menemy = enemy;
		monster[m]._menemyx = plr[enemy]._pfutx;
		monster[m]._menemyy = plr[enemy]._pfuty;
	}
	else {
		monster[m]._mFlags |= MFLAG_MID;
		enemy -= MAX_PLRS;
		monster[m]._menemy = enemy;
		monster[m]._menemyx = monster[enemy]._mfutx;
		monster[m]._menemyy = monster[enemy]._mfuty;
	}
}

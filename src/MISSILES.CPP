/*-----------------------------------------------------------------------**
** 								Diablo
**
**							 Missiles file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/MISSILES.CPP 3     2/25/97 2:27p Jmcreynolds $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	<math.h>
#include	<limits>
#include	"sound.h"
#include	"missiles.h"
#include	"misdat.h"
#include	"engine.h"
#include	"gendung.h"
#include	"lighting.h"
#include	"items.h"
#include	"player.h"
#include	"monster.h"
#include	"monstdat.h"
#include	"monstint.h"
#include	"control.h"
#include	"objects.h"
#include	"objdat.h"
#include	"spells.h"
#include	"cursor.h"
#include	"dead.h"
#include	"effects.h"
#include	"palette.h"
#include	"inv.h"
#include	"msg.h"
#include	"quests.h"
#include	"itemdat.h"
#include	"multi.h"
#include	"trigs.h"
#include	"scrollrt.h"

#define MONOPRINT	1

/*-----------------------------------------------------------------------*
** Constants
**-----------------------------------------------------------------------*/
#if RLE_DRAW
#define CEL_EXT ".CL2"
#else
#define CEL_EXT ".CEL"
#endif


/*-----------------------------------------------------------------------*
** Global Variables
**-----------------------------------------------------------------------*/

MissileStruct	missile[MAXMISSILES];
int nummissiles;

static int missilevars[MAXMISSILES][3]; //0 missile num; 1 var1; 2 var2
static int nummissilevars;

int missileactive[MAXMISSILES];
int missileavail[MAXMISSILES];

int XDirAdd[8] = { 1, 0, -1, -1, -1,  0,  1, 1 };
int YDirAdd[8] = { 1, 1,  1,  0, -1, -1, -1, 0 };
	
// Indexes into the CrawlTable.
static int const CrawlNum[19] = { 0, 3, 12, 45, 94, 159, 240, 337, 450, 579, 724,
								885, 1062, 1255, 1464, 1689, 1930, 2187, 2460 };

BOOL ManashieldFlag;
BOOL MissilePreFlag;

typedef BOOL (*CHECKFUNC)(int x, int y);

static void GetMissilePos(int i);
void SetMissDir(int mi, int dir);

/*-----------------------------------------------------------------------**
**----------------------- General use routines --------------------------**
**-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void GetDamageAmt(int i, int * mind, int * maxd)
{
	int k, sl;
	
	app_assert(myplr < MAX_PLRS && myplr >= 0);
	app_assert(i < 64 && i >= 0);
	sl = plr[myplr]._pSplLvl[i] + plr[myplr]._pISplLvlAdd;

	switch (i) {
		case SPL_FIREBOLT:
			*mind = 1  + (plr[myplr]._pMagic >> 3) + sl;
			*maxd = 10 + (plr[myplr]._pMagic >> 3) + sl;
			break;
		case SPL_HEAL:
			*mind = 1 + plr[myplr]._pLevel + sl;
			if (plr[myplr]._pClass == CLASS_WARRIOR
				|| plr[myplr]._pClass == CLASS_MONK
				|| plr[myplr]._pClass == CLASS_BARBARIAN) *mind = *mind << 1;
			else if (plr[myplr]._pClass == CLASS_ROGUE ||
			         plr[myplr]._pClass == CLASS_BARD) *mind += (*mind >> 1);
			*maxd = 10;
			for (k = 0; k < plr[myplr]._pLevel; ++k) *maxd += 4;
			for (k = 0; k < sl; ++k) *maxd += 6;
			if (plr[myplr]._pClass == CLASS_WARRIOR
				|| plr[myplr]._pClass == CLASS_MONK
				|| plr[myplr]._pClass == CLASS_BARBARIAN) *maxd = *maxd << 1;
			else if (plr[myplr]._pClass == CLASS_ROGUE ||
			         plr[myplr]._pClass == CLASS_BARD) *maxd += (*maxd >> 1);
			*mind = -1;
			*maxd = -1;
			break;
		case SPL_LIGHTNING:
		case SPL_RUNEOFLIGHT:
			*mind = 2;
			*maxd = plr[myplr]._pLevel + 2;
			break;
		case SPL_FLASH:
			*mind = plr[myplr]._pLevel;
			for (k = sl; k > 0; --k) *mind += (*mind >> 3);
			*mind += *mind >> 1;
			*maxd = *mind << 1; // this is not actual(below is) - its a kludge to make it seem acurate text wise
			/**maxd = 0;
			for (k = 0; k <= plr[myplr]._pLevel; ++k) *maxd += 20;
			for (k = sl; k > 0; --k) *maxd += (*maxd >> 3);
			*maxd += *maxd >> 1;*/
			break;
		case SPL_IDENTIFY:
		case SPL_TOWN:
		case SPL_STONE:
		case SPL_RUNEOFSTONE:
		case SPL_INFRA:
		case SPL_PHASE:
		case SPL_MANASHLD:
		case SPL_DOOM:
		case SPL_BLOODR:
		case SPL_INVIS:
		//case SPL_BLOODB:
		case SPL_RAGE:
		case SPL_TELE:
		case SPL_ETHER:
		case SPL_REPAIR:
		case SPL_RECHARGE:
		case SPL_DISARM:
		case SPL_RESURRECT:
		case SPL_TELEKINESIS:
		case SPL_BONESPIRIT:
		case SPL_TELESTAIRS:
		case SPL_REFLECT:
		case SPL_BERSERK:
		case SPL_SHOWMAGITEMS:
			*mind = -1;
			*maxd = -1;
			break;
		case SPL_WALL:
		case SPL_LTWALL:
		case SPL_RINGOFFIRE:
			*mind = ((2 + plr[myplr]._pLevel) << 2) >> 1;
			*maxd = ((20 +  plr[myplr]._pLevel) << 2) >> 1;
			break;
		case SPL_FIREBALL:
		case SPL_RUNEOFFIRE:
			*mind = (2 + plr[myplr]._pLevel) << 1;
			for (k = sl; k > 0; --k) *mind += (*mind >> 3);
			*maxd = (20 + plr[myplr]._pLevel) << 1;
			for (k = sl; k > 0; --k) *maxd += (*maxd >> 3);
			break;
		case SPL_GUARDIAN:
			*mind = 1 + (plr[myplr]._pLevel >> 1);
			for (k = sl; k > 0; --k) *mind += (*mind >> 3);
			*maxd = 10 + (plr[myplr]._pLevel >> 1);
			for (k = sl; k > 0; --k) *maxd += (*maxd >> 3);
			break;
		case SPL_CHAIN:
			// this is not actual - its a kludge to make it seem acurate text wise
			*mind = 2 << 1;
			*maxd = (plr[myplr]._pLevel + 2) << 1;
			break;
		case SPL_WAVE:
			// this is not actual - its a kludge to make it seem acurate text wise
			*mind = ((1 + plr[myplr]._pLevel) << 2) + ((1 + plr[myplr]._pLevel) << 1);
			*maxd = ((10 + plr[myplr]._pLevel) << 2) + ((10 + plr[myplr]._pLevel) << 1);
			break;
		case SPL_NOVA:
		case SPL_IMMOLATION:
		case SPL_RUNEOFIMMOLATION:
		case SPL_RUNEOFNOVA:
			// this is not actual - its a kludge to make it seem acurate text wise
			*mind = (5 + plr[myplr]._pLevel) >> 1;
			for (k = sl; k > 0; --k) *mind += (*mind >> 3);
			*mind = (*mind << 2) + *mind;
			*maxd = (30 + plr[myplr]._pLevel) >> 1;
			for (k = sl; k > 0; --k) *maxd += (*maxd >> 3);
			*maxd = (*maxd << 2) + *maxd;
			break;
		case SPL_FLAME:
			*mind = 2;
			*mind += *mind >> 1;
			*maxd = (plr[myplr]._pLevel + 4);
			*maxd += *maxd >> 1;
			break;
		case SPL_GOLEM:
			*mind = 11;
			*maxd = 17;
			break;
		case SPL_APOCA:
			*mind = 0;
			for (k = 0; k < plr[myplr]._pLevel; ++k) *mind += 1;
			*maxd = 0;
			for (k = 0; k < plr[myplr]._pLevel; ++k) *maxd += 6;
			break;
		case SPL_ELEMENT:
			*mind = (2 + plr[myplr]._pLevel) << 1;
			for (k = sl; k > 0; --k) *mind += (*mind >> 3);
			*maxd = (20 + plr[myplr]._pLevel) << 1;
			for (k = sl; k > 0; --k) *maxd += (*maxd >> 3);
			break;
		case SPL_CBOLT:
			*mind = 1;
			*maxd = 1 + (plr[myplr]._pMagic >> 2);
			break;
		case SPL_HBOLT:
			*mind = 9 + plr[myplr]._pLevel;
			*maxd = 18 +  plr[myplr]._pLevel;
			break;
		case SPL_HEALOTHER:
			*mind = 1 + plr[myplr]._pLevel + sl;
			if (plr[myplr]._pClass == CLASS_WARRIOR
				|| plr[myplr]._pClass == CLASS_MONK
				|| plr[myplr]._pClass == CLASS_BARBARIAN) *mind = *mind << 1;
			if (plr[myplr]._pClass == CLASS_ROGUE ||
			    plr[myplr]._pClass == CLASS_BARD ) *mind += (*mind >> 1);
			*maxd = 10;
			for (k = 0; k < plr[myplr]._pLevel; ++k) *maxd += 4;
			for (k = 0; k < sl; ++k) *maxd += 6;
			if (plr[myplr]._pClass == CLASS_WARRIOR
				|| plr[myplr]._pClass == CLASS_MONK
				|| plr[myplr]._pClass == CLASS_BARBARIAN) *maxd = *maxd << 1;
			if (plr[myplr]._pClass == CLASS_ROGUE ||
				plr[myplr]._pClass == CLASS_BARD ) *maxd += (*maxd >> 1);
			*mind = -1;
			*maxd = -1;
			break;
		case SPL_BSTAR:
			*mind = ((plr[myplr]._pMagic>>1)-(plr[myplr]._pMagic>>3)) + (sl << 1) + sl;
			*maxd = *mind;
			break;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
int CheckBlock(int fx, int fy, int tx, int ty)
{
	int pn, dir, coll;

	coll = 0;

	while ((fx != tx) || (fy != ty)) {
		dir = GetDirection(fx, fy, tx, ty);
		fx += XDirAdd[dir];
		fy += YDirAdd[dir];
		app_assert(fx < MAXDUNX && fx >= 0);
		app_assert(fy < MAXDUNY && fy >= 0);
		pn = dPiece[fx][fy];
		app_assert(pn <= MAXTILES && pn >= 0);
		if (nSolidTable[pn]) coll = 1;
	}
	
	return coll;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
int FindClosest(int sx, int sy, int rad)
{
	int cr, cidx, cent, cne, mid, tx, ty;

	if (rad > 19) rad = 19;

	for (cr = 1; cr < rad; ++cr) {
		cidx = CrawlNum[cr];
		cent = cidx + 1;
		for (cne = CrawlTable[cidx]; cne > 0; --cne) {
			tx = sx + CrawlTable[cent];
			ty = sy + CrawlTable[(cent + 1)];
			if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
				mid = dMonster[tx][ty];
				if ((mid > 0) && (CheckBlock(sx, sy, tx, ty) == 0))
					return (mid - 1);
			}
			cent += 2;
		}
	}

	return (-1);

}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static bool SetMissileLocation(int i, int *sx, int *sy, int rad)
{
	bool success = false;

	if (rad > 19) rad = 19;
	
	for (int cr = 0; cr < rad && !success; ++cr) {
		int const cidx = CrawlNum[cr];
		int cent = cidx + 1;
		for (int cne = CrawlTable[cidx]; cne > 0; --cne, cent += 2) {
			int const tx = *sx + CrawlTable[cent];
			int const ty = *sy + CrawlTable[(cent + 1)];
			if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
				int const pn = dPiece[tx][ty];
				if (!nSolidTable[pn] 
					&& dObject[tx][ty] == 0 
					&& dMissile[tx][ty] == 0) {
					missile[i]._mix = tx;
					missile[i]._miy = ty;
					*sx = tx;
					*sy = ty;
					success = true;
					break;
				}
			}
		}
	}

	return (success);

}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
int GetSpellLevel(int id, int sn)
{
	int rv;

	app_assert(id < MAX_PLRS && id >= 0);
	app_assert(sn < 64 && sn >= 0);
	if (id == myplr)
		rv = plr[id]._pSplLvl[sn] + plr[id]._pISplLvlAdd;
	else
		rv = 1;
	if (rv < 0 ) rv = 0;
	return (rv);
}

#if 0	// UNUSED
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void RndBlood(int x, int y, int type, int dam, int hp)
{
	int pct, str;

	app_assert(hp != 0);
	str = 0;
	pct = (dam * 100) / hp;
	if (pct > 65) str = 1;
	if (pct > 80) str = 2;
	if (pct > 90) str = 3;
	str = 1;

	//AddMissile(x, y, str, 0, 0, type, 0, myplr, 0);
}
#endif

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static int GetDirection8(int x1, int y1, int x2, int y2)
{
	BYTE const Dirs[16][16] = {
		{ 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 0
		{  2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 1
		{  2, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 2
		{  2, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 }, // 3
		{  2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 }, // 4
		{  2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 }, // 5
		{  2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // 6
		{  2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // 7
		{  2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // 8
		{  2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // 9
		{  2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // 10
		{  2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // 11
		{  2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // 12
		{  2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // 13
		{  2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // 14
		{  2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }};// 15

	BYTE const lrtoul[3] = { 3, 4, 5 };
	BYTE const urtoll[3] = { 3, 2, 1 };
	BYTE const lltour[3] = { 7, 6, 5 };
	BYTE const ultolr[3] = { 7, 0, 1 };

	int mx, my, md;

	mx = abs(x2 - x1);
	if (mx > 15) mx = 15;
	my = abs(y2 - y1);
	if (my > 15) my = 15;
	md = Dirs[my][mx];

	if (x1 > x2) {
		if (y1 > y2) md = lrtoul[md];
		else md = urtoll[md];
	} else {
		if (y1 > y2) md  = lltour[md];
		else md = ultolr[md];
	}

	return (md);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static int GetDirection16(int x1, int y1, int x2, int y2)
{
	const BYTE Dirs[16][16] = {
		{ 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 0
		{  4, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 1
		{  4, 3, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 }, // 2
		{  4, 3, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 }, // 3
		{  4, 4, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // 4
		{  4, 4, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // 5
		{  4, 4, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1 }, // 6
		{  4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1 }, // 7
		{  4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1 }, // 8
		{  4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1 }, // 9
		{  4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1 }, // 10
		{  4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1 }, // 11
		{  4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 }, // 12
		{  4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 }, // 13
		{  4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2 }, // 14
		{  4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2 }};// 15

	BYTE const lrtoul[5] = {  6,  7,  8,  9, 10 };
	BYTE const urtoll[5] = {  6,  5,  4,  3,  2 };
	BYTE const lltour[5] = { 14, 13, 12, 11, 10 };
	BYTE const ultolr[5] = { 14, 15,  0,  1,  2 };


	int mx = abs(x2 - x1);
	if (mx > 15) mx = 15;
	
	int my = abs(y2 - y1);
	if (my > 15) my = 15;
	
	int md = Dirs[my][mx];

	if (x1 > x2) {
		if (y1 > y2) md = lrtoul[md];
		else md = urtoll[md];
	} else {
		if (y1 > y2) md  = lltour[md];
		else md = ultolr[md];
	}

	return (md);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void DeleteMissile(int mi, int i)
{
	app_assert(nummissiles <= MAXMISSILES);
	missileavail[MAXMISSILES - nummissiles] = mi;
	--nummissiles;
	
	if ((nummissiles > 0) && (i != nummissiles)) {
		app_assert(i < MAXMISSILES);
		missileactive[i] = missileactive[nummissiles];
	} 
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void GetMissileVel(int i, int sx, int sy, int dx, int dy, int v)
{
	app_assert(i < MAXMISSILES && i >= 0);
	
	double const dxp = (((dx - sx) << 5) - ((dy - sy) << 5)) << 16;
	double const dyp = (((dx - sx) << 5) + ((dy - sy) << 5)) << 16;
	double const dr = sqrt((dxp * dxp) + (dyp * dyp));
	
	missile[i]._mixvel = static_cast<long>(((v << 16) * dxp) / dr);
	missile[i]._miyvel = static_cast<long>(((v << 15) * dyp) / dr);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void PutMissile(int i)
{
	app_assert(i < MAXMISSILES && i >= 0);
	int const mx = missile[i]._mix;
	int const my = missile[i]._miy;
	
	if (mx <= 0 || my <= 0 || mx >= DMAXX || my >= DMAXY) 
		missile[i]._miDelFlag = TRUE;

	if (!missile[i]._miDelFlag) {
		dFlags[mx][my] |= BFLAG_MISSILE;
		if (dMissile[mx][my] == 0) dMissile[mx][my] = static_cast<char>(i + 1);
		else dMissile[mx][my] = -1;
		if (missile[i]._miPreFlag) MissilePreFlag = TRUE;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static BYTE GetDirNum(int mi)
{
	app_assert(mi < MAXMISSILES && mi >= 0);
	return (misfiledata[(missile[mi]._miAnimType)].mAnimFAmt >= 8) ? missile[mi]._mimfnum : 0;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void GetMissilePos(int i)
{
	long		mx, my;
	long		dx, dy;
	long		lx, ly;		// lighting offsets

	app_assert(i < MAXMISSILES && i >= 0);
	mx = missile[i]._mitxoff >> 16;
	my = missile[i]._mityoff >> 16;
	dx = (mx + (my << 1));
	dy = ((my << 1) - mx);
	
	if (dx < 0)	{
		lx = -((-dx) >> 3);
		dx = -((-dx) >> 6);
	} else {
		lx = dx >> 3;
		dx = dx >> 6;
	}
	
	if (dy < 0)	{
		ly = -((-dy) >> 3);
		dy = -((-dy) >> 6);
	} else {
		ly = dy >> 3;
		dy = dy >> 6;
	}
	
	missile[i]._mix = dx + missile[i]._misx;
	missile[i]._miy = dy + missile[i]._misy;
	missile[i]._mixoff = mx - ((dx - dy) << 5);
	missile[i]._miyoff = my - ((dx + dy) << 4);
	ChangeLightOff(missile[i]._mlid, lx - (dx << 3), ly - (dy << 3));
}

/*-----------------------------------------------------------------------*
 * MoveMissilePos
 *
 *	Adjusts missile's drawing location to prevent overlapping tiles
 *	Written specifically for monster missiles (e.g., Rhino), which are large.
**-----------------------------------------------------------------------*/

static void MoveMissilePos(int i)
{
	int dx,dy;
	int mx,my;

	app_assert(i < MAXMISSILES && i >= 0);
	switch (missile[i]._mimfnum) {
		case 0 :		// Down
			dx = 1;
			dy = 1;
			break;
		case 1 :		// Down Left
			dx = 1;
			dy = 1;
			break;
		case 2 :		// Left
			dx = 0;
			dy = 1;
			break;
		case 3 :		// Up Left
			dx = 0;
			dy = 0;
			break;
		case 4 :		// Up
			dx = 0;
			dy = 0;
			break;
		case 5 :		// Up Right
			dx = 0;
			dy = 0;
			break;
		case 6 :		// Right
			dx = 1;
			dy = 0;
			break;
		case 7 :		// Down Right
			dx = 1;
			dy = 1;
			break;
	}

	mx = missile[i]._mix + dx;
	my = missile[i]._miy + dy;

	// note -- _misource is the monster which became this missile
	if (PosOkMonst(missile[i]._misource,mx,my)) {
		missile[i]._mix += dx;
		missile[i]._miy += dy;
		missile[i]._mixoff -= ((dx - dy) << 5);
		missile[i]._miyoff -= ((dx + dy) << 4);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL MonsterTrapHit(int m, int mindam, int maxdam, int dist, int t, byte shift)
{
	int		hit, hper;
	long	dam;
	int		mor;		// monster's resist type
	int		mir;		
	BOOL	resist = FALSE;
	BOOL	ret;

	app_assert(m < MAXMONSTERS && m >= 0);
	if (monster[m].mtalkmsg != 0) return FALSE;

	if ((monster[m]._mhitpoints >> HP_SHIFT) <= 0) return FALSE;

	app_assert(monster[m].MType != NULL);
	if(monster[m].MType->mtype == MT_ILLWEAV && monster[m]._mgoal == MG_RUN_AWAY)
		return FALSE;

	if (monster[m]._mmode == MM_MISSILE) return(FALSE);

	mir = missiledata[t].mResist;
	mor	= monster[m].mMagicRes;
	if(	   (mor & M_IM && mir == MIMT_MISC)
		|| (mor & M_IF && mir == MIMT_FIRE)
		|| (mor & M_IL && mir == MIMT_LGHT))
		return FALSE;

	if(	   (mor & M_RM && mir == MIMT_MISC)
		|| (mor & M_RF && mir == MIMT_FIRE)
		|| (mor & M_RL && mir == MIMT_LGHT))
		resist = TRUE;

	hit = random(68, 100);
	hper = 90 - monster[m].mArmorClass - dist;
	if (hper < 5) hper = 5;
	if (hper > 95) hper = 95;
	if (CheckMonsterHit(m, ret))
		return(ret);
#if CHEATS
	else if((hit < hper) || simplecheat || cheatflag || (monster[m]._mmode == MM_STONE)) {
#else
	else if((hit < hper) || (monster[m]._mmode == MM_STONE)) {
#endif
		dam = random(68, maxdam - mindam + 1) + mindam;
		if (shift == 0) dam = dam << HP_SHIFT;
		if (resist)
			monster[m]._mhitpoints -= dam >> 2;
		else
			monster[m]._mhitpoints -= dam;

#if CHEATS
		if (simplecheat || cheatflag) monster[m]._mhitpoints = 0;
#endif
		// rjs - x2 dam if stone - if (monster[m]._mmode == MM_STONE) monster[m]._mhitpoints -= dam;
		if ((monster[m]._mhitpoints >> HP_SHIFT) <= 0) {
			if (monster[m]._mmode == MM_STONE) {
				M_StartKill(m, -1);
				monster[m]._mmode = MM_STONE;
			} else M_StartKill(m, -1);
		}
		else if(resist)
		{
			PlayEffect(m, MS_GOTHIT);
		}
		else
		{
			if (monster[m]._mmode == MM_STONE) {
				if (m > 3) M_StartHit(m, -1, dam); // dont let golems get hit
				monster[m]._mmode = MM_STONE;
			} else {
				if (m > 3) M_StartHit(m, -1, dam); // dont let golems get hit
			}
		}
		return(TRUE);
	} else return(FALSE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL MonsterMHit(int pnum, int m, int mindam, int maxdam, int dist, int t, byte shift)
{
	int		hit, hper;
	long	dam;
	int		mor;		// monster's resist type
	int		mir;		// missile resistance category
	BOOL	resist = FALSE;
	BOOL	ret;

	app_assert(m < MAXMONSTERS && m >= 0);
	if (monster[m].mtalkmsg != 0) return(FALSE);

	if ((monster[m]._mhitpoints >> HP_SHIFT) <= 0) return(FALSE);

	app_assert(monster[m].MData != NULL);
	if ((t == MIT_HBOLT) &&
		(monster[m].MType->mtype != MT_DIABLO) &&
		(monster[m].MData->mMonstClass != MC_UNDEAD)) return(FALSE);

	app_assert(monster[m].MType != NULL);
	if(monster[m].MType->mtype == MT_ILLWEAV && monster[m]._mgoal == MG_RUN_AWAY)
		return FALSE;

	if (monster[m]._mmode == MM_MISSILE) return(FALSE);

	mir = missiledata[t].mResist;
	mor	= monster[m].mMagicRes;
	if(	   (mor & M_IM && mir == MIMT_MISC)
		|| (mor & M_IF && mir == MIMT_FIRE)
		|| (mor & M_IL && mir == MIMT_LGHT)
		|| (mor & M_IA && mir == MIMT_ACID))
		return FALSE;

	if(	   (mor & M_RM && mir == MIMT_MISC)
		|| (mor & M_RF && mir == MIMT_FIRE)
		|| (mor & M_RL && mir == MIMT_LGHT))
		resist = TRUE;
	
	if (t == MIT_HBOLT)
	{
		if ((monster[m].MType->mtype == MT_DIABLO) ||
			(monster[m].MType->mtype == MT_BONED2))
			resist = TRUE;
	}

	hit = random(69, 100);
	if (pnum != -1) {
		if (missiledata[t].mType == MIS_WEAP) {
			hper = BASE_TO_HIT + plr[pnum]._pLevel - monster[m].mArmorClass - plr[pnum]._pIEnAc;
			hper += plr[pnum]._pDexterity + plr[pnum]._pIBonusToHit;
			hper -= (dist * dist) >> 1;
			if (plr[pnum]._pClass == CLASS_ROGUE) hper += 20;
			if (plr[pnum]._pClass == CLASS_WARRIOR 
				|| plr[pnum]._pClass == CLASS_BARD) hper += 10;
		} else {
			hper = BASE_TO_HIT + plr[pnum]._pMagic - (monster[m].mLevel << 1) - dist;
			if (plr[pnum]._pClass == CLASS_SORCEROR) hper += 20;
			else if (plr[pnum]._pClass == CLASS_BARD) hper += 10;
		}
	}
	else {
		hper = random(71,75) - (monster[m].mLevel * 2);
	}
	
	if (hper < 5) hper = 5;
	if (hper > 95) hper = 95;
	if (monster[m]._mmode == MM_STONE) hit = 0;
	if (CheckMonsterHit(m, ret))
		return(ret);
#if CHEATS
	else if((hit < hper) || cheatflag || simplecheat) {
#else
	else if (hit < hper) {
#endif
		if (t == MIT_BONESPIRIT)
			dam = (monster[m]._mhitpoints / 3) >> HP_SHIFT;
		else
			dam = random(70, maxdam - mindam + 1) + mindam;
		if (missiledata[t].mType == MIS_WEAP) {
			dam += (dam * plr[pnum]._pIBonusDam) / 100;
			dam += plr[pnum]._pIBonusDamMod;
			if (plr[pnum]._pClass == CLASS_ROGUE) dam += plr[pnum]._pDamageMod;
			else dam += (plr[pnum]._pDamageMod >> 1);
		}
		if (shift == 0) dam = dam << HP_SHIFT;
		if (resist) dam = dam >> 2;
		if (pnum == myplr) monster[m]._mhitpoints -= dam;

		if (plr[pnum]._pIFlags & IAF_MNOHEAL) monster[m]._mFlags |= MFLAG_NOHEAL;
#if CHEATS 
		//if (pnum == myplr && cheatflag) monster[m]._mhitpoints = 0;
#endif
		//rjs - x2 stone dam - if (pnum == myplr && monster[m]._mmode == MM_STONE) monster[m]._mhitpoints -= dam;

		if ((monster[m]._mhitpoints >> HP_SHIFT) <= 0) {
			if (monster[m]._mmode == MM_STONE) {
				M_StartKill(m, pnum);
				monster[m]._mmode = MM_STONE;
			} else M_StartKill(m, pnum);
			//(old) AddPlrExperience(pnum, monster[m].mLevel, monster[m].mExp);
			//(new, but moved) AddPlrMonstExper(monster[m].mLevel, monster[m].mExp, monster[m].mWhoHit);
		}
		else if(resist)
		{
			PlayEffect(m, MS_GOTHIT);
		}
		else
		{
			if (monster[m]._mmode == MM_STONE) {
				if (m > 3) M_StartHit(m, pnum, dam);
				monster[m]._mmode = MM_STONE;
			} else {
				if ((missiledata[t].mType == MIS_WEAP) && (plr[pnum]._pIFlags & IAF_KNOCKBACK)) M_GetKnockback(m);
				if (m > 3) M_StartHit(m, pnum, dam);
			}
		}
		// wake up monster if it's inactive
		if(!monster[m]._msquelch)
		{
			monster[m]._msquelch = 255;
			monster[m]._lastx = plr[pnum]._px;
			monster[m]._lasty = plr[pnum]._py;
		}			
		return(TRUE);
	} else return(FALSE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL PlayerMHit(int pnum, int m, int dist, int mind, int maxd, int mtype, byte shift, BOOL earflag, bool *wasBlocked)
{
	int		hit, hper, tac;
	long	dam;
	int		blk, blkper, blkdir;
	int		resper = 0;

	*wasBlocked = false;
	app_assert(pnum < MAX_PLRS && pnum >= 0);
	if ((plr[pnum]._pHitPoints >> HP_SHIFT) <= 0) return(FALSE);
	if (plr[pnum]._pInvincible) return(FALSE);
	if (((plr[pnum]._pSpellFlags & SF_ETHER) != 0) && (missiledata[mtype].mType == MIS_WEAP)) return(FALSE);

	hit = random(72, 100);
#if CHEATS
	if (simplecheat || cheatflag) hit = 1000;
#endif
	if (missiledata[mtype].mType == MIS_WEAP) {
		// rjs tac = (byte)plr[pnum]._pArmorClass + plr[pnum]._pIAC + plr[pnum]._pIBonusAC;
		tac = plr[pnum]._pIAC + plr[pnum]._pIBonusAC;
		tac += (plr[pnum]._pDexterity / 5);
		if (m != -1) hper = 30 + monster[m].mHit - tac + ((monster[m].mLevel - plr[pnum]._pLevel) << 1) - (dist << 1);
		else hper = 100 - (tac >> 1) - (dist << 1);
	} else {
		if (m != -1) hper = 40 + (monster[m].mLevel << 1) - (plr[pnum]._pLevel << 1) - (dist << 1);
		else hper = 40;
	}
	if (hper < 10) hper = 10;
	if ((currlevel == 14) && (hper < 20)) hper = 20;
	if ((currlevel == 15) && (hper < 25)) hper = 25;
	if ((currlevel == 16) && (hper < 30)) hper = 30;
	if (((plr[pnum]._pmode == PM_STAND) || (plr[pnum]._pmode == PM_ATTACK)) && (plr[pnum]._pBlockFlag)) blk = random(73, 100);
	else blk = 100;
	if (shift == 1) blk = 100;			// can't block continous damage spells
	if (mtype == MIT_ACIDPUD) blk = 100;		// can't block acid puddles (drb)
	if (m != -1) blkper = plr[pnum]._pBaseToBlk + plr[pnum]._pDexterity - ((monster[m].mLevel - plr[pnum]._pLevel) << 1);
	else blkper = plr[pnum]._pBaseToBlk + plr[pnum]._pDexterity;
	if (blkper < 0) blkper = 0;
	if (blkper > 100) blkper = 100;
	switch(missiledata[mtype].mResist) {
		case MIMT_FIRE:
			resper = plr[pnum]._pFireResist;
			break;
		case MIMT_LGHT:
			resper = plr[pnum]._pLghtResist;
			break;
		case MIMT_MISC:
		case MIMT_ACID:
			resper = plr[pnum]._pMagResist;
			break;
		default:
			resper = 0;
			break;
	}
	if (hit < hper) {
		// Hit, so calc damage
		if (mtype == MIT_BONESPIRIT) {
			dam = plr[pnum]._pHitPoints / 3;
		} else {
			if (shift == 0) {
				dam = (maxd - mind + 1) << HP_SHIFT;
				dam = random(75, dam) + (mind << HP_SHIFT);
				if (plr[pnum]._pIFlags & IAF_TRAPDAM) dam = dam >> 1;
				dam += (plr[pnum]._pIGetHit << HP_SHIFT);
				if (dam < (1 << HP_SHIFT)) dam = (1 << HP_SHIFT);
			} else {
				dam = maxd - mind + 1;
				dam = random(75, dam) + mind;
				if (plr[pnum]._pIFlags & IAF_TRAPDAM) dam = dam >> 1;
				dam += plr[pnum]._pIGetHit;
				if (dam < (1 << HP_SHIFT)) dam = (1 << HP_SHIFT);
			}
		}
		
		// Did I block?
		if (blk < blkper) {
			if (m != -1) blkdir = GetDirection(plr[pnum]._px, plr[pnum]._py, monster[m]._mx, monster[m]._my);
			else blkdir = plr[pnum]._pdir;
			*wasBlocked = true;
			StartPlrBlock(pnum, blkdir);
			return(TRUE);
		}
		// Did I resist?
		if (resper > 0) {
			dam -= (dam * resper) / 100;
			if (pnum == myplr) {
				plr[pnum]._pHitPoints -= dam;
				plr[pnum]._pHPBase -= dam;
			}
			if (plr[pnum]._pHitPoints > plr[pnum]._pMaxHP) {
				plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
				plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;
			}
			if ((plr[pnum]._pHitPoints >> HP_SHIFT) <= 0) {
				// rjs - manashld fix? - plr[pnum]._pHitPoints = 0;
				StartPlrKill(pnum, earflag);
			}
			else {
				// No get hit
				if (plr[pnum]._pClass == CLASS_WARRIOR) PlaySfxLoc(PS_WARR69, plr[pnum]._px, plr[pnum]._py);
				#if !IS_VERSION(SHAREWARE)
				else if (plr[pnum]._pClass == CLASS_ROGUE) PlaySfxLoc(PS_ROGUE69, plr[pnum]._px, plr[pnum]._py);
				else if (plr[pnum]._pClass == CLASS_SORCEROR) PlaySfxLoc(PS_MAGE69, plr[pnum]._px, plr[pnum]._py);
				else if (plr[pnum]._pClass == CLASS_MONK) PlaySfxLoc(PS_MONK69, plr[pnum]._px, plr[pnum]._py);
				else if (plr[pnum]._pClass == CLASS_BARD) PlaySfxLoc(PS_BARD69, plr[pnum]._px, plr[pnum]._py);
				else if (plr[pnum]._pClass == CLASS_BARBARIAN) PlaySfxLoc(PS_BARBARIAN69, plr[pnum]._px, plr[pnum]._py);
				#endif

				//PlaySfxLoc(PS_LGHIT, plr[pnum]._px, plr[pnum]._py);
				drawhpflag = TRUE;
			}
			return(TRUE);
		} else {
			if (pnum == myplr) {
				plr[pnum]._pHitPoints -= dam;
				plr[pnum]._pHPBase -= dam;
			}
			if (plr[pnum]._pHitPoints > plr[pnum]._pMaxHP) {
				plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
				plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;
			}
			if ((plr[pnum]._pHitPoints >> HP_SHIFT) <= 0) {
				// rjs - manashld fix? - plr[pnum]._pHitPoints = 0;
				StartPlrKill(pnum, earflag);
			} else StartPlrHit(pnum, dam, FALSE);
			return(TRUE);
		}
	}
	return(FALSE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static BOOL Plr2PlrMHit(int pnum, int p, int mindam, int maxdam, int dist, int mtype, byte shift, bool *wasBlocked)
{
	int		hit, hper, tac;
	long	dam;
	int		blk, blkper, blkdir;
	int		resper;

	// pnum is missiles source
	// p is missiles dest

	app_assert(p < MAX_PLRS && p >= 0);
	*wasBlocked = false;
	if (plr[p]._pInvincible) return(FALSE);
	if (mtype == MIT_HBOLT) return(FALSE);
	if (((plr[p]._pSpellFlags & SF_ETHER) != 0) && (missiledata[mtype].mType == MIS_WEAP)) return(FALSE);

	switch(missiledata[mtype].mResist) {
		case MIMT_FIRE:
			resper = plr[p]._pFireResist;
			break;
		case MIMT_LGHT:
			resper = plr[p]._pLghtResist;
			break;
		case MIMT_MISC:
		case MIMT_ACID:
			resper = plr[p]._pMagResist;
			break;
		default:
			resper = 0;
			break;
	}
	hit = random(69, 100);
	if (missiledata[mtype].mType == MIS_WEAP) {
		//rjs tac = (byte)plr[p]._pArmorClass + plr[p]._pIAC + plr[p]._pIBonusAC;
		tac = plr[p]._pIAC + plr[p]._pIBonusAC;
		tac += (plr[p]._pDexterity / 5);
		hper = BASE_TO_HIT + plr[pnum]._pLevel - tac;
		hper += plr[pnum]._pDexterity + plr[pnum]._pIBonusToHit;
		hper -= (dist * dist) >> 1;
		if (plr[pnum]._pClass == CLASS_ROGUE) hper += 20;
		if (plr[pnum]._pClass == CLASS_WARRIOR
			|| plr[pnum]._pClass == CLASS_BARD) hper += 10;
	} else {
		hper = BASE_TO_HIT + plr[pnum]._pMagic - (plr[p]._pLevel << 1) - dist;
		if (plr[pnum]._pClass == CLASS_SORCEROR ) hper += 20;
		else if	(plr[pnum]._pClass == CLASS_BARD) hper += 10;
	}
	if (hper < 5) hper = 5;
	if (hper > 95) hper = 95;
	if (hit < hper) {
		// Hit, calc blk %
		if (((plr[p]._pmode == PM_STAND) || (plr[p]._pmode == PM_ATTACK)) && (plr[p]._pBlockFlag)) blk = random(73, 100);
		else blk = 100;
		if (shift == 1) blk = 100;			// can't block continous damage spells
		blkper = plr[p]._pBaseToBlk + plr[p]._pDexterity - ((plr[pnum]._pLevel - plr[p]._pLevel) << 1);
		if (blkper < 0) blkper = 0;
		if (blkper > 100) blkper = 100;
		// Hit, so calc damage
		if (mtype == MIT_BONESPIRIT) {
			dam = plr[p]._pHitPoints / 3;
		} else {
			dam = random(70, maxdam - mindam + 1) + mindam;
			if (missiledata[mtype].mType == MIS_WEAP) {
				dam += (dam * plr[pnum]._pIBonusDam) / 100;
				dam += plr[pnum]._pIBonusDamMod + plr[pnum]._pDamageMod;
			}
			if (shift == 0) dam = dam << HP_SHIFT;
		}
		// NEW! Take 1/2 damage from plr spells (drb 11/23)
		if (missiledata[mtype].mType != MIS_WEAP) dam = dam >> 1;
		// Did I resist?
		if (resper > 0) {
			dam -= (dam * resper) / 100;
			// No get hit
			if (pnum == myplr) NetSendCmdDamage(TRUE, p, dam);
			if (plr[pnum]._pClass == CLASS_WARRIOR) PlaySfxLoc(PS_WARR69, plr[pnum]._px, plr[pnum]._py);
			#if !IS_VERSION(SHAREWARE)
			else if (plr[pnum]._pClass == CLASS_ROGUE) PlaySfxLoc(PS_ROGUE69, plr[pnum]._px, plr[pnum]._py);
			else if (plr[pnum]._pClass == CLASS_SORCEROR) PlaySfxLoc(PS_MAGE69, plr[pnum]._px, plr[pnum]._py);
			else if (plr[pnum]._pClass == CLASS_MONK) PlaySfxLoc(PS_MONK69, plr[pnum]._px, plr[pnum]._py);
			else if (plr[pnum]._pClass == CLASS_BARD) PlaySfxLoc(PS_BARD69, plr[pnum]._px, plr[pnum]._py);
			else if (plr[pnum]._pClass == CLASS_BARBARIAN) PlaySfxLoc(PS_BARBARIAN69, plr[pnum]._px, plr[pnum]._py);
			#endif

			//PlaySfxLoc(PS_LGHIT, plr[p]._px, plr[p]._py);
			return(TRUE);
		} else {
			// Did I block?
			if (blk < blkper) {
				blkdir = GetDirection(plr[p]._px, plr[p]._py, plr[pnum]._px, plr[pnum]._py);
				StartPlrBlock(p, blkdir);
				*wasBlocked = true;
				return(TRUE);
			} else {
				if (pnum == myplr) NetSendCmdDamage(TRUE, p, dam);
				StartPlrHit(p, dam, FALSE);
				return(TRUE);
			}
		}
	}
	return(FALSE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void CheckMissileCol(int i, int mindam, int maxdam, byte shift, int mx, int my, byte nodel)
{
	int	pn,oi;
	BOOL earflag;
	bool wasBlocked;

	if (!(i < MAXMISSILES && i >= 0)) return;
	if (!(mx < MAXDUNX && mx >= 0)) return;
	if (!(my < MAXDUNY && my >= 0)) return;
	
	if (missile[i]._micaster == MI_ENEMYBOTH || missile[i]._misource == -1){
		// Traps and fire and lightning walls.
		if (dMonster[mx][my] > 0) {
			if (missile[i]._micaster == MI_ENEMYBOTH) {
				if (MonsterMHit(missile[i]._misource, dMonster[mx][my] - 1, mindam, maxdam, missile[i]._midist, missile[i]._mitype, shift)) {
					if (nodel == 0) missile[i]._mirange = 0;
					missile[i]._miHitFlag = TRUE;
				}
			} else {
				if (MonsterTrapHit(dMonster[mx][my] - 1, mindam, maxdam, missile[i]._midist, missile[i]._mitype, shift)) {
					if (nodel == 0) missile[i]._mirange = 0;
					missile[i]._miHitFlag = TRUE;
				}
			}
		}
		if (dPlayer[mx][my] > 0) {
			if (missile[i]._miAnimType == MF_FIRE
				|| missile[i]._miAnimType == MF_LIGHTNING) earflag = TRUE;
			else earflag = FALSE;
			if (PlayerMHit(dPlayer[mx][my] - 1, -1, missile[i]._midist, mindam, maxdam, missile[i]._mitype, shift, earflag, &wasBlocked)) {
				if (wasBlocked)
				{
					int newdir = missile[i]._mimfnum + (random(10, 2) ? 1 : -1);
					int numdirsAvail = misfiledata[missile[i]._miAnimType].mAnimFAmt;
					if (newdir < 0)
						newdir = numdirsAvail - 1;
					else if (newdir > numdirsAvail)
						newdir = 0;
						
					SetMissDir(i, newdir);	// Changes the animation, but the missile keeps going.
				}
				else
				{
					if (nodel == 0) missile[i]._mirange = 0;
				}
				missile[i]._miHitFlag = TRUE;
			}
		}
	} else {
		if (missile[i]._micaster == MI_ENEMYMONST) {
			if (dMonster[mx][my] > 0) {
					if (MonsterMHit(missile[i]._misource, dMonster[mx][my] - 1, mindam, maxdam, missile[i]._midist, missile[i]._mitype, shift)) {
					if (nodel == 0) missile[i]._mirange = 0;
					missile[i]._miHitFlag = TRUE;
				}
			} else {
				if ((dMonster[mx][my] < 0) && (monster[-(dMonster[mx][my] + 1)]._mmode == MM_STONE)) {
					if (MonsterMHit(missile[i]._misource, -(dMonster[mx][my] + 1), mindam, maxdam, missile[i]._midist, missile[i]._mitype, shift)) {
						if (nodel == 0) missile[i]._mirange = 0;
						missile[i]._miHitFlag = TRUE;
					}
				}
			}
			if ((dPlayer[mx][my] > 0) && ((dPlayer[mx][my]-1) != missile[i]._misource)) {
				if (Plr2PlrMHit(missile[i]._misource, dPlayer[mx][my]-1, mindam, maxdam, missile[i]._midist, missile[i]._mitype, shift, &wasBlocked)) {
					if (wasBlocked)
					{
						int newdir = missile[i]._mimfnum + (random(10, 2) ? 1 : -1);
						int numdirsAvail = misfiledata[missile[i]._miAnimType].mAnimFAmt;
						if (newdir < 0)
							newdir = numdirsAvail - 1;
						else if (newdir > numdirsAvail)
							newdir = 0;
							
						SetMissDir(i, newdir);	// Changes the animation, but the missile keeps going.
					}
					else 
					{
						if (nodel == 0) missile[i]._mirange = 0;
					}
					missile[i]._miHitFlag = TRUE;
				}
			}
		} else {
			if (((monster[missile[i]._misource]._mFlags & MFLAG_MID) != 0) && ((dMonster[mx][my] > 0) && (monster[(dMonster[mx][my] - 1)]._mFlags & MFLAG_MKILLER) != 0)) {
				if (MonsterTrapHit(dMonster[mx][my] - 1, mindam, maxdam, missile[i]._midist, missile[i]._mitype, shift)) {
					if (nodel == 0) missile[i]._mirange = 0;
					missile[i]._miHitFlag = TRUE;
				}
			}
			if (dPlayer[mx][my] > 0) {
				if (PlayerMHit(dPlayer[mx][my] - 1, missile[i]._misource, missile[i]._midist, mindam, maxdam, missile[i]._mitype, shift, FALSE, &wasBlocked)) {
					if (wasBlocked)
					{
						int newdir = missile[i]._mimfnum + (random(10, 2) ? 1 : -1);
						int numdirsAvail = misfiledata[missile[i]._miAnimType].mAnimFAmt;
						if (newdir < 0)
							newdir = numdirsAvail - 1;
						else if (newdir > numdirsAvail)
							newdir = 0;
							
						SetMissDir(i, newdir);	// Changes the animation, but the missile keeps going.
					}
					else
					{
						if (nodel == 0) missile[i]._mirange = 0;
					}
					missile[i]._miHitFlag = TRUE;
				}
			}
		}
	}
	
	if (dObject[mx][my] != 0) {
		if (dObject[mx][my] > 0) oi = dObject[mx][my] - 1;
		else oi = -(dObject[mx][my] + 1);
		if (!object[oi]._oMissFlag) {
			if (object[oi]._oBreak == OBJ_BREAKABLE) BreakObject(-1, oi);
			if (nodel == 0) missile[i]._mirange = 0;
			missile[i]._miHitFlag = FALSE;
		}
	}

	pn = dPiece[mx][my];
	app_assert(pn <= MAXTILES && pn >= 0);
	if (nMissileTable[pn]) {
		if (nodel == 0) missile[i]._mirange = 0;
		missile[i]._miHitFlag = FALSE;
	}

	if ((missile[i]._mirange == 0) && (missiledata[missile[i]._mitype].miSFX != -1)) {
		PlaySfxLoc(missiledata[missile[i]._mitype].miSFX, missile[i]._mix, missile[i]._miy);
	}
}

static void SetMissAnim(int mi, int animtype)
{
	int dir;

	app_assert(mi < MAXMISSILES && mi >= 0);
	dir = missile[mi]._mimfnum;
	missile[mi]._miAnimType = animtype;

	missile[mi]._miAnimFlags = misfiledata[animtype].mFlags;
	missile[mi]._miAnimData = misfiledata[animtype].mAnimData[dir];
	missile[mi]._miAnimDelay = misfiledata[animtype].mAnimDelay[dir];
	missile[mi]._miAnimLen = misfiledata[animtype].mAnimLen[dir];
	missile[mi]._miAnimWidth = misfiledata[animtype].mAnimWidth[dir];
	missile[mi]._miAnimWidth2 = misfiledata[animtype].mAnimWidth2[dir];
	missile[mi]._miAnimCnt = 0;
	missile[mi]._miAnimFrame = 1;
}


void SetMissDir(int mi, int dir)
{
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._mimfnum = dir;
	SetMissAnim(mi, missile[mi]._miAnimType);
}

/*-----------------------------------------------------------------------**
**---------------------- Initialization Routines ------------------------**
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if RLE_DRAW
void ILoadMissileGFX(BYTE mf) {
	int i;
	char strbuff[256];
	BYTE *p;
	MisFileData * pMisDat;

	pMisDat = &misfiledata[mf];
	if (pMisDat->mFlags & MFF_MULTI) {
		// All directions are packed into one file
		sprintf(strbuff,"Missiles\\%s" CEL_EXT, pMisDat->mAnimPath);
		p = LoadFileInMemSig(strbuff,NULL,'MISS');
		for (i = 0; i < pMisDat->mAnimFAmt; ++i)
			pMisDat->mAnimData[i] = p + (reinterpret_cast<DWORD *>(p))[i];
	}
	else if (pMisDat->mAnimFAmt == 1) {
		sprintf(strbuff,"Missiles\\%s" CEL_EXT, pMisDat->mAnimPath);
		if(! pMisDat->mAnimData[0])
			pMisDat->mAnimData[0] = LoadFileInMemSig(strbuff,NULL,'MISS');
	}
	else {
		for (i = 0; i < pMisDat->mAnimFAmt; ++i) {
			sprintf(strbuff, "Missiles\\%s%i" CEL_EXT, pMisDat->mAnimPath, i + 1);
			if(! pMisDat->mAnimData[i])
				pMisDat->mAnimData[i] = LoadFileInMemSig(strbuff,NULL,'MISS');
		}
	}
}
#endif


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if !RLE_DRAW
void ILoadMissileGFX(BYTE mf) {
	int i;
	char strbuff[256];
	BYTE *p;

	if (misfiledata[mf].mFlags & MFF_MULTI)	{
		// All directions are packed into one file
		sprintf(strbuff,"Missiles\\%s.CEL", misfiledata[mf].mAnimPath);
		p = LoadFileInMemSig(strbuff,NULL,'MISS');
		for (i = 0; i < misfiledata[mf].mAnimFAmt; ++i) {
			misfiledata[mf].mAnimData[i] = p + *reintrepret_cast<DWORD *>(p + (i<<2));
		}
	}
	else if (misfiledata[mf].mAnimFAmt == 1) {
		sprintf(strbuff,"Missiles\\%s.CEL", misfiledata[mf].mAnimPath);
		if(! misfiledata[mf].mAnimData[0])
			misfiledata[mf].mAnimData[0] = LoadFileInMemSig(strbuff,NULL,'MISS');
	} else {
		for (i = 0; i < misfiledata[mf].mAnimFAmt; ++i) {
			sprintf(strbuff, "Missiles\\%s%i.CEL", misfiledata[mf].mAnimPath, i + 1);
			if(! misfiledata[mf].mAnimData[i])
				misfiledata[mf].mAnimData[i] = LoadFileInMemSig(strbuff,NULL,'MISS');
		}
	}
}
#endif


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void InitMissileGFX()
{
	int i;

	for (i = 0; misfiledata[i].mAnimFAmt; ++i) {
		if (!(misfiledata[i].mFlags & MFF_MONSTONLY)) {
			ILoadMissileGFX(i);
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void FreeMissileFile(int i)
{
	int j;
	void * y;

	if (misfiledata[i].mFlags & MFF_MULTI)
	{
		if(misfiledata[i].mAnimData[0])
		{
			y = static_cast<void *> (misfiledata[i].mAnimData[0] - (misfiledata[i].mAnimFAmt << 2));
			DiabloFreePtr(y);
			misfiledata[i].mAnimData[0] = NULL;
		}
	}
	else
	{
		for (j = 0; j < misfiledata[i].mAnimFAmt; ++j)
			if(misfiledata[i].mAnimData[j])
			{
				DiabloFreePtr(misfiledata[i].mAnimData[j]);
			}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void FreeMissileGFX()
{
	int i;

	for (i = 0; misfiledata[i].mAnimFAmt; ++i) {
		if (!(misfiledata[i].mFlags & MFF_MONSTONLY)) {
			FreeMissileFile(i);
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void IFreeMissileGFX()
{
	int i;

	for (i = 0; misfiledata[i].mAnimFAmt; ++i)
		if(misfiledata[i].mFlags & MFF_MONSTONLY)
			FreeMissileFile(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void InitMissiles()
{
	int	i, j, mx;

	// Delete any active infravision/ etheralize
	HighLightAllItems = false;
	plr[myplr]._pSpellFlags &= ~SF_ETHER;
	if (plr[myplr]._pInfraFlag == TRUE) {
		app_assert(nummissiles <= MAXMISSILES);
		for (i = 0; i < nummissiles; ++i) {
			mx = missileactive[i];
			app_assert(mx < MAXMISSILES && mx >= 0);
			if ((missile[mx]._mitype == MIT_INFRA) && (missile[mx]._misource == myplr))
				CalcPlrItemVals(missile[mx]._misource,TRUE);
		}
	}
	
	if (((plr[myplr]._pSpellFlags & SF_RAGE) == SF_RAGE)
	   || ((plr[myplr]._pSpellFlags & SF_LETHERGY) == SF_LETHERGY))
	{
		plr[myplr]._pSpellFlags &= ~SF_RAGE;
		plr[myplr]._pSpellFlags &= ~SF_LETHERGY;
		app_assert(nummissiles <= MAXMISSILES);
		for (i = 0; i < nummissiles; ++i) {
			mx = missileactive[i];
			app_assert(mx < MAXMISSILES && mx >= 0);
			if ((missile[mx]._mitype == MIT_RAGE) && (missile[mx]._misource == myplr))
			{
				int const diffHpts = plr[myplr]._pMaxHP - plr[myplr]._pHitPoints;
				CalcPlrItemVals(myplr,TRUE);
				plr[myplr]._pHitPoints -= missile[mx]._miVar2 + diffHpts;
				if (plr[myplr]._pHitPoints < (1 << HP_SHIFT)) {
					// Don't quite die.
					plr[myplr]._pHitPoints = 1 << HP_SHIFT;
				}
			}
		}
	}

	nummissiles = 0;
	for (i = 0; i < MAXMISSILES; ++i) {
		missileavail[i] = i;
		missileactive[i] = 0;
	}

	nummissilevars = 0;
	for (i = 0; i < MAXMISSILES; ++i) {
		missilevars[i][0] = -1;
		missilevars[i][1] = 0;
		missilevars[i][2] = 0;
	}

	for (j = 0; j < DMAXY; ++j) {
		for (i = 0; i < DMAXX; ++i) {
			dFlags[i][j] = dFlags[i][j] & ~BFLAG_MISSILE;
		}
	}
	
	// Zero out all the reflect spells.
	//for (j = 0; j < gbMaxPlayers; ++j) {
	//	plr[j]._pReflectCount = 0;
	//}
	plr[myplr]._pReflectCount = 0;
}

/*-----------------------------------------------------------------------**
**------------------ Missile Initialization Routines --------------------**
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddReallyBigExp(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	AddMissile (80, 62, 80, 62, midir, MIT_BIGEXPLOSION, mienemy, id, dam, 0);
	AddMissile (80, 63, 80, 62, midir, MIT_BIGEXPLOSION, mienemy, id, dam, 0);
	AddMissile (81, 62, 80, 62, midir, MIT_BIGEXPLOSION, mienemy, id, dam, 0);
	AddMissile (81, 63, 80, 62, midir, MIT_BIGEXPLOSION, mienemy, id, dam, 0);
	missile[mi]._miDelFlag = TRUE;

	app_assert(mi < MAXMISSILES && mi >= 0);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddRuneOfFire(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (LineClear(sx, sy, dx, dy))
	{
		if (id >= 0) {
			UseMana(id, SPL_RUNEOFFIRE);
		}
		if (SetMissileLocation(mi, &dx, &dy, 10)) {
			missile[mi]._miVar1 = MIT_BIGEXPLOSION;
			missile[mi]._miDelFlag = FALSE;
			missile[mi]._mlid = AddLight(dx, dy, 8);
		}
		else missile[mi]._miDelFlag = TRUE;
	}
	else missile[mi]._miDelFlag = TRUE;

	app_assert(mi < MAXMISSILES && mi >= 0);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddRuneOfLight(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (LineClear(sx, sy, dx, dy))
	{
		if (id >= 0) {
			UseMana(id, SPL_RUNEOFLIGHT);
		}
		if (SetMissileLocation(mi, &dx, &dy, 10)) {
			missile[mi]._miVar1 = MIT_LIGHTBALL;
			missile[mi]._miDelFlag = FALSE;
			missile[mi]._mlid = AddLight(dx, dy, 8);
		}
		else missile[mi]._miDelFlag = TRUE;
	}
	else missile[mi]._miDelFlag = TRUE;

	app_assert(mi < MAXMISSILES && mi >= 0);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddRuneOfNova(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (LineClear(sx, sy, dx, dy))
	{
		if (id >= 0) {
			UseMana(id, SPL_RUNEOFNOVA);
		}
		if (SetMissileLocation(mi, &dx, &dy, 10)) {
			missile[mi]._miVar1 = MIT_NOVA;
			missile[mi]._miDelFlag = FALSE;
			missile[mi]._mlid = AddLight(dx, dy, 8);
		}
		else missile[mi]._miDelFlag = TRUE;
	}
	else missile[mi]._miDelFlag = TRUE;

	app_assert(mi < MAXMISSILES && mi >= 0);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddRuneOfImmolation(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (LineClear(sx, sy, dx, dy))
	{
		if (id >= 0) {
			UseMana(id, SPL_RUNEOFIMMOLATION);
		}
		if (SetMissileLocation(mi, &dx, &dy, 10)) {
			missile[mi]._miVar1 = MIT_IMMOLATION;
			missile[mi]._miDelFlag = FALSE;
			missile[mi]._mlid = AddLight(dx, dy, 8);
		
		}
		else missile[mi]._miDelFlag = TRUE;
	}
	else missile[mi]._miDelFlag = TRUE;

	app_assert(mi < MAXMISSILES && mi >= 0);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddRuneOfStone(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (LineClear(sx, sy, dx, dy))
	{
		if (id >= 0) {
			UseMana(id, SPL_RUNEOFSTONE);
		}
		if (SetMissileLocation(mi, &dx, &dy, 10)) {
			missile[mi]._miVar1 = MIT_STONE;
			missile[mi]._miDelFlag = FALSE;
			missile[mi]._mlid = AddLight(dx, dy, 8);
		}
		else missile[mi]._miDelFlag = TRUE;
	}
	else missile[mi]._miDelFlag = TRUE;

	app_assert(mi < MAXMISSILES && mi >= 0);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddReflect(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (id >= 0) {
		if (plr[id]._pReflectCount < 0)
			plr[id]._pReflectCount = 0;
			
		plr[id]._pReflectCount += ((missile[mi]._mispllvl) ? missile[mi]._mispllvl : 2 ) * plr[id]._pLevel;
		UseMana(id, SPL_REFLECT);
	}
	missile[mi]._mirange = 0;
	missile[mi]._miDelFlag = FALSE;

	app_assert(mi < MAXMISSILES && mi >= 0);
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddBerserk(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (id >= 0) {
			
		app_assert(mi < MAXMISSILES && mi >= 0);
		missile[mi]._misource = id;
	
		// search in a 5 radius around tile clicked for monster 
		for (int k = 0; k < 6; ++k) {
			int const l = CrawlNum[k];
			int j = l + 1;
			for (int i = CrawlTable[l]; i > 0; --i, j += 2) {
				int const tx = dx + CrawlTable[j];
				int const ty = dy + CrawlTable[(j + 1)];
				if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
					int mid = dMonster[tx][ty];
					if (mid > 0) --mid;
					else mid = -(mid + 1);
					
					if ((mid > 3)
						&& (monster[mid]._uniqtype == 0)
						&& (monster[mid]._mAi != AI_DIABLO)
						&& (monster[mid]._mmode != MM_FADEIN)
						&& (monster[mid]._mmode != MM_FADEOUT)
						//&& (monster[mid]._mmode != MM_DEATH)
						&& ((monster[mid].mMagicRes & M_IM) == 0)
						&& (((monster[mid].mMagicRes & M_RM) == 0) ||
						    (((monster[mid].mMagicRes & M_RM) == M_RM) && 0 == random(99, 2)))
						&& (monster[mid]._mmode != MM_MISSILE)) {
						i = -99;
						k = 6;
						int const SpellLevel = GetSpellLevel(id, SPL_BERSERK);
						monster[mid]._mFlags |= MFLAG_BERSERK | MFLAG_MKILLER;
						monster[mid].mMinDamage = static_cast<BYTE>(SpellLevel +  monster[mid].mMinDamage * (1.0 + (0.01 * (random(145,10) + 20))));
						monster[mid].mMaxDamage = static_cast<BYTE>(SpellLevel +  monster[mid].mMaxDamage * (1.0 + (0.01 * (random(145,10) + 20))));
						monster[mid].mMinDamage2 = static_cast<BYTE>(SpellLevel + monster[mid].mMinDamage2 * (1.0 + (0.01 * (random(145,10) + 20))));
						monster[mid].mMaxDamage2 = static_cast<BYTE>(SpellLevel + monster[mid].mMaxDamage2 * (1.0 + (0.01 * (random(145,10) + 20))));
						
						monster[mid].mlid = AddLight(monster[mid]._mx, monster[mid]._my, 
							((currlevel >= HIVESTART && currlevel <= HIVEEND) ? 9 : 3));
						
						UseMana(id, SPL_BERSERK);
						break;
					}
				}
			}
		}
	}
	missile[mi]._mirange = 0;
	missile[mi]._miDelFlag = TRUE;

	app_assert(mi < MAXMISSILES && mi >= 0);
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddHorkSpawn(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{		
	app_assert(mi < MAXMISSILES && mi >= 0);

	GetMissileVel(mi, sx, sy, dx, dy, 8);
	missile[mi]._mirange = 9;
	missile[mi]._miVar1 = midir;
	PutMissile(mi);
}

void AddRandom(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{		
	app_assert(mi < MAXMISSILES && mi >= 0);
 	
	// Let the random spell do the call, otherwise staff's use up 2 charges.
	// if (id >= 0) {
	// 	UseMana(id, SPL_RANDOM);
	// }
 	int mitype = MIT_FIREBOLT;
 	int const r = random(255, 10);
 	switch(r)
 	{
 		case 0: // fall through
 		case 1: mitype = MIT_FIREBOLT; break;
 		case 2: mitype = MIT_FIREBALL; break;
 		case 3: mitype = MIT_FIREWALLC; break;
 		case 4: mitype = MIT_GUARDIAN; break;
 		case 5: mitype = MIT_CHAIN; break;
 		case 6:	mitype = MIT_TOWN; UseMana(id, SPL_TOWN); break;
 		case 7:	mitype = MIT_TELE; break;
 		case 8:	mitype = MIT_APOCA; break;
 		case 9:	mitype = MIT_STONE; break;
 		default: break;
 	}
	AddMissile (sx, sy, dx, dy, midir, mitype,
				missile[mi]._micaster, missile[mi]._misource,
				0, missile[mi]._mispllvl);
	
	missile[mi]._miDelFlag = TRUE;
	missile[mi]._mirange = 0;
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddDisEnchant(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._misource = id;

	// search in a 3 radius around chest trap for players.
	for (int k = 0; k < 3; ++k) {
		int const l = CrawlNum[k];
		int j = l + 1;
		for (int i = CrawlTable[l]; i > 0; --i, j += 2) {
			int const tx = sx + CrawlTable[j];
			int const ty = sy + CrawlTable[(j + 1)];
			if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
				int pid = dPlayer[tx][ty];
				
				if (pid == 0)
					continue;
					
				if (pid > 0) --pid;
				else pid = -(pid + 1);
				
				
				int newidata;
				BOOL everplayed = FALSE;
				
				// Belt items only.
				for (int splIndex = 0; splIndex < MAXSPD; ++splIndex)
				{
					newidata = -1;
					
					if (plr[pid].SpdList[splIndex]._itype == IT_MISC)
					{
						int what;
						int const killed = random(205, 2);
						if (!killed)
							continue;
							
						switch(plr[pid].SpdList[splIndex]._iMiscId) {
							case IMID_PLHEAL:
							case IMID_PMANA:
								RemoveSpdBarItem(pid, splIndex);
								continue;
						
							case IMID_REJUV:
								what = random(205, 2);
								switch(what)
								{
									case 0:
										newidata = ItemMiscIdIdx(IMID_PLHEAL);
										break;
									default:	
										newidata = ItemMiscIdIdx(IMID_PMANA);
										break;
								}
						
							case IMID_PHEAL:
				 				newidata = ItemMiscIdIdx(IMID_PLHEAL);
				 				break;
							case IMID_PFMANA:
				 				newidata = ItemMiscIdIdx(IMID_PMANA);
				 				break;
							case IMID_FREJUV:
								what = random(205, 3);
								switch(what)
								{
									case 0:
				 						newidata = ItemMiscIdIdx(IMID_PFMANA);
				 						break;
									case 1:
				 						newidata = ItemMiscIdIdx(IMID_PHEAL);
				 						break;
									default:
				 						newidata = ItemMiscIdIdx(IMID_REJUV);
				 						break;
				 				}
				 				break;	
				 				
							default:
								continue;
						}
					}
					if (newidata != -1) {
						SetPlrHandItem(&plr[pid].HoldItem, newidata);
						GetPlrHandSeed(&plr[pid].HoldItem);
						plr[pid].HoldItem._iStatFlag = TRUE;
						plr[pid].SpdList[splIndex] = plr[pid].HoldItem;
					}
					if (!everplayed)
					{
						PlaySfxLoc(IS_URN, tx, ty);
						everplayed = TRUE;
					}
				}
				force_redraw = FULLDRAW;
			}
		}
	}
	missile[mi]._mirange = 0;
	missile[mi]._miDelFlag = TRUE;

	app_assert(mi < MAXMISSILES && mi >= 0);
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddManaRemove(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._misource = id;

	// search in a 3 radius around chest trap for players.
	for (int k = 0; k < 3; ++k) {
		int const l = CrawlNum[k];
		int j = l + 1;
		for (int i = CrawlTable[l]; i > 0; --i, j += 2) {
			int const tx = sx + CrawlTable[j];
			int const ty = sy + CrawlTable[(j + 1)];
			if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
				int pid = dPlayer[tx][ty];
			
				if (pid == 0)
					continue;
					
				if (pid > 0) --pid;
				else pid = -(pid + 1);
				
				plr[pid]._pMana = 0;
				plr[pid]._pManaBase = plr[pid]._pMana - (plr[pid]._pMaxMana - plr[pid]._pMaxManaBase);
				CalcPlrInv(pid, FALSE);
				drawmanaflag = TRUE;
				PlaySfxLoc(TSFX_COW7, tx, ty);
			}
		}
	}
	missile[mi]._mirange = 0;
	missile[mi]._miDelFlag = TRUE;

	app_assert(mi < MAXMISSILES && mi >= 0);
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddLArrow(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if ((sx == dx) && (sy == dy)) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}

	if (mienemy == MI_ENEMYMONST) {
		int av=32;
		if (plr[id]._pClass == CLASS_ROGUE) av += plr[id]._pLevel>>2;
		else if (plr[id]._pClass == CLASS_WARRIOR
			|| plr[id]._pClass == CLASS_BARD) av += plr[id]._pLevel>>3;
		if (plr[id]._pIFlags & IAF_ATANIM1) av += 1;
		if (plr[id]._pIFlags & IAF_ATANIM2) av += 2;
		if (plr[id]._pIFlags & IAF_ATANIM3) av += 4;
		if (plr[id]._pIFlags & IAF_ATANIM4) av += 8;
		
		GetMissileVel(mi,sx,sy,dx,dy,av);
	} else GetMissileVel(mi,sx,sy,dx,dy,32);

	SetMissDir(mi, GetDirection16(sx,sy,dx,dy));

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._mirange = 256;

	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._mlid = AddLight(sx, sy, 5);

	//PutMissile(mi);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddArrow(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int av;

	if ((sx == dx) && (sy == dy)) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}

	if (mienemy == MI_ENEMYMONST) {
		av = 32;
		if (plr[id]._pIFlags & IAF_RNDARROW) av = random(64, 32) + 16;		// rnd arrow speed
		if (plr[id]._pClass == CLASS_ROGUE) av += (plr[id]._pLevel-1) >> 2;	// rouge level speed increase
		else if (plr[id]._pClass == CLASS_WARRIOR
			|| plr[id]._pClass == CLASS_BARD) av += (plr[id]._pLevel-1) >> 3;	// warrior level speed increase
		
		if (plr[id]._pIFlags & IAF_ATANIM1) av += 1;
		if (plr[id]._pIFlags & IAF_ATANIM2) av += 2;
		if (plr[id]._pIFlags & IAF_ATANIM3) av += 4;
		if (plr[id]._pIFlags & IAF_ATANIM4) av += 8;
		
		GetMissileVel(mi,sx,sy,dx,dy,av);
	} else GetMissileVel(mi,sx,sy,dx,dy,32);

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._miAnimFrame = GetDirection16(sx,sy,dx,dy) + 1;

	missile[mi]._mirange = 256;

	//PutMissile(mi);
}

void AddSpecialArrow(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_FIREBALL) || !PRE_BETA
	int extraspeed = 0;

//	if ((sx == dx) && (sy == dy)) {
//		dx += XDirAdd[midir];
//		dy += YDirAdd[midir];
//	}

	app_assert(mi < MAXMISSILES && mi >= 0);
	if (mienemy == MI_ENEMYMONST)
	{
		if (plr[id]._pClass == CLASS_ROGUE)
			extraspeed = (plr[id]._pLevel-1) >> 2;	// rogue level speed increase
		else if (plr[id]._pClass == CLASS_WARRIOR
				|| plr[id]._pClass == CLASS_BARD)
			extraspeed = (plr[id]._pLevel-1) >> 3;	// warrior level speed increase
		
		if (plr[id]._pIFlags & IAF_ATANIM1) extraspeed += 1;
		if (plr[id]._pIFlags & IAF_ATANIM2) extraspeed += 2;
		if (plr[id]._pIFlags & IAF_ATANIM3) extraspeed += 4;
		if (plr[id]._pIFlags & IAF_ATANIM4) extraspeed += 8;
	}

	missile[mi]._mirange = 1;
	missile[mi]._miVar1 = dx;
	missile[mi]._miVar2 = dy;
	missile[mi]._miVar3 = extraspeed;

	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void GetVileMissPos(int mi, int dx, int dy)
{
	int xx, yy;
	app_assert(mi < MAXMISSILES && mi >= 0);
	for (int l = 1; l < 50; ++l) {
		for (int j = -l; j <= l; ++j) {
			yy = dy + j;
			for (int i = -l; i <= l; ++i) {
				xx = dx + i;
				if (PosOkPlayer(myplr,xx,yy)) {
					missile[mi]._mix = xx;
					missile[mi]._miy = yy;
					return;
				}
			}
		}
	}
	// There is just no way it will ever reach here
	missile[mi]._mix = dx;
	missile[mi]._miy = dy;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#define DIST(x,y,d)	(abs(x) < d && abs(y) < d)

// Flag for fireman missile
#define MIF_DIDHIT	1

#define MINAWAY 3
#define MAXAWAY 6

void AddRndTeleport(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_PHASE) || !PRE_BETA
	int pn, r1, r2;
	
	int nTries = 0;
	do {
		// don't get stuck in an infinite loop...
		if (++nTries > 500) {
			r1 = 0;
			r2 = 0;
			break;
		}

		r1 = random(58, MAXAWAY - MINAWAY) + MINAWAY + 1;
		r2 = random(58, MAXAWAY - MINAWAY) + MINAWAY + 1;
		if (random(58, 2) == 1) r1 = -r1;
		if (random(58, 2) == 1) r2 = -r2;
		
		r1 = sx + r1;
		r2 = sy + r2;
		
		if (r1 > MAXDUNX 
			|| r1 < 0
			|| r2 > MAXDUNY
			|| r2 < 0)
			continue;
		
		pn = dPiece[r1][r2];
	} while (nSolidTable[pn] != 0 
			|| dObject[r1][r2] != 0 
			|| dMonster[r1][r2] != 0);

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._mirange = 2;
	missile[mi]._miVar1 = 0;

// special code for Vile Betrayer quest
	if ((setlevel) && (setlvlnum == 5)) {
		int oi = dObject[dx][dy]-1;
		if ((object[oi]._otype == OBJ_MCIRCLE1) || (object[oi]._otype == OBJ_MCIRCLE2)) {
			missile[mi]._mix = dx;
			missile[mi]._miy = dy;
			if (!PosOkPlayer(myplr, dx, dy)) GetVileMissPos(mi, dx, dy);
		}
	}
	else {
	missile[mi]._mix = r1;
	missile[mi]._miy = r2;

	if (mienemy == MI_ENEMYMONST) UseMana(id, SPL_PHASE);
	}

#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

#undef max
void AddTeleStairs(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int minDist = std::numeric_limits<int>::is_specialized && std::numeric_limits<int>::is_bounded 
					? std::numeric_limits<int>::max() : 99999;
	if (id >= 0) {
		sx = plr[id]._px;
		sy = plr[id]._py;
	}
	
	int rx = sx;
	int ry = sy;
	
	for (int i = 0;i < numtrigs && i < MAXTRIGGERS; ++i) {
		if (trigs[i]._tmsg == WM_DIABTWARPUP
			|| trigs[i]._tmsg == WM_DIABPREVLVL
			|| trigs[i]._tmsg == WM_DIABNEXTLVL
			|| trigs[i]._tmsg == WM_DIABRTNLVL) {
		
			int triggerx;
			int triggery;
			
			if ((leveltype == 1 || leveltype == 2)
			    && (trigs[i]._tmsg == WM_DIABNEXTLVL
			        || trigs[i]._tmsg == WM_DIABPREVLVL
			        || trigs[i]._tmsg == WM_DIABRTNLVL) ){
				triggerx = trigs[i]._tx;
				triggery = trigs[i]._ty + 1;
			}
			else {
				triggerx = trigs[i]._tx + 1;
				triggery = trigs[i]._ty;
			}
			
			int xdiff = sx - triggerx;
			xdiff *= xdiff;
			int ydiff = sy - triggery;
			ydiff *= ydiff;
			int const NewDist = xdiff + ydiff;
			
			if (NewDist < minDist)
			{
				minDist = NewDist;
				rx = triggerx;
				ry = triggery;
			}
		
		}
	}


	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._mirange = 2;
	missile[mi]._miVar1 = 0;
	missile[mi]._mix = rx;
	missile[mi]._miy = ry;

	if (mienemy == MI_ENEMYMONST) UseMana(id, SPL_TELESTAIRS);

}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddFirebolt(int mi, int sx, int sy, int dx, int dy, int midir, char micaster, int id, int dam)
{
#if (PRE_BETA && PRE_FIREBOLT) || !PRE_BETA
	int sp, i, mx;

	if ((sx == dx) && (sy == dy)) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}


	app_assert(mi < MAXMISSILES && mi >= 0);
	if (micaster == MI_ENEMYMONST)
	{
		app_assert(nummissiles <= MAXMISSILES);
		for (i = 0; i < nummissiles; ++i) {
			mx = missileactive[i];
			app_assert(mx < MAXMISSILES && mx >= 0);
			if ((missile[mx]._mitype == MIT_GUARDIAN) &&
				(missile[mx]._misource == id) && (missile[mx]._miVar3 == mi)) {
					break;
			}
		}
		if (i == nummissiles) UseMana(id, SPL_FIREBOLT);

		if (id != -1) {
			sp = 16 + (missile[mi]._mispllvl << 1);
			if (sp >= 63) sp = 63;
		} else sp = 16;
	}
	else
		sp = 26;

	GetMissileVel(mi,sx,sy,dx,dy,sp);
	SetMissDir(mi, GetDirection16(sx,sy,dx,dy));

	missile[mi]._mirange = 256;

	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._mlid = AddLight(sx, sy, 8);

	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddMagmaball(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	GetMissileVel(mi,sx,sy,dx,dy,16);

	app_assert(mi < MAXMISSILES && mi >= 0);
	// Graphics don't line up, so we have to move them...
	missile[mi]._mitxoff += 3*missile[mi]._mixvel;
	missile[mi]._mityoff += 3*missile[mi]._miyvel;

	GetMissilePos(mi);
	
	if ((missile[mi]._mixvel >> 16) == 0
		&& (missile[mi]._miyvel >> 16) == 0)
	{
		missile[mi]._mirange = 1;
	}
	else
	{
		missile[mi]._mirange = 256;
	}

	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._mlid = AddLight(sx, sy, 8);

	//PutMissile(mi);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddKrull(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	GetMissileVel(mi,sx,sy,dx,dy,16);
	
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._mirange = 256;

	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
//	missile[mi]._mlid = AddLight(sx, sy, 8);

	PutMissile(mi);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddTeleport(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_TELE) || !PRE_BETA
	int i, pn;
	int k, l, j;
	int tx, ty;

	app_assert(dx < MAXDUNX && dx >= 0);
	app_assert(dy < MAXDUNY && dy >= 0);

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._miDelFlag = TRUE;
	for (k = 0; k < 6; ++k) {
		l = CrawlNum[k];
		j = l + 1;
		for (i = CrawlTable[l]; i > 0; --i) {
			tx = dx + CrawlTable[j];
			ty = dy + CrawlTable[(j + 1)];
			if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
				pn = dPiece[tx][ty];
				if ((nSolidTable[pn] | dMonster[tx][ty] | dObject[tx][ty] |	dPlayer[tx][ty]) == 0) {
					missile[mi]._mix = tx;
					missile[mi]._miy = ty;
					missile[mi]._misx = tx;
					missile[mi]._misy = ty;
					missile[mi]._miDelFlag = FALSE;
					k = 6;
					break;
				}
			}
			j += 2;
		}
	}

	if (missile[mi]._miDelFlag == FALSE) {
		UseMana(id, SPL_TELE);
		missile[mi]._mirange = 2;
	}
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddLightball(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	GetMissileVel(mi,sx,sy,dx,dy,16);
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._midam = dam;

	missile[mi]._miAnimFrame = random(63, 8) + 1;

	missile[mi]._mirange = 255;
	
	if (id < 0) {
		missile[mi]._miVar1 = sx;
		missile[mi]._miVar2 = sy;
	} else {
		missile[mi]._miVar1 = plr[id]._px;
		missile[mi]._miVar2 = plr[id]._py;
	}

	//PutMissile(mi);
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddLightwall(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	GetMissileVel(mi,sx,sy,dx,dy,16);
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._midam = dam;

	missile[mi]._miAnimFrame = random(63, 8) + 1;

	missile[mi]._mirange = 255 + (255 * missile[mi]._mispllvl);
	
	if (id < 0) {
		missile[mi]._miVar1 = sx;
		missile[mi]._miVar2 = sy;
	} else {
		missile[mi]._miVar1 = plr[id]._px;
		missile[mi]._miVar2 = plr[id]._py;
	}

	//PutMissile(mi);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddFirewall(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_WALL) || !PRE_BETA

	
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._midam = (random(53, 10) + random(53, 10) + 2 + (id > 0) ? plr[id]._pLevel : currlevel) << 4;
	missile[mi]._midam = missile[mi]._midam >> 1;

	GetMissileVel(mi,sx,sy,dx,dy,16);

	missile[mi]._mirange = 10 * (missile[mi]._mispllvl + 1);
	
	if (mienemy != MI_ENEMYMONST || id < 0) // trap or somesuch
	{
		missile[mi]._mirange = missile[mi]._mirange + currlevel;
	}
	else
	{
		missile[mi]._mirange = missile[mi]._mirange + ((plr[id]._pISplDur * missile[mi]._mirange) >> 7);
	}
	missile[mi]._mirange = missile[mi]._mirange << 4;

	missile[mi]._miVar1 = missile[mi]._mirange - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;

	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddFireball(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_FIREBALL) || !PRE_BETA
	int i;

	if ((sx == dx) && (sy == dy)) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}

	app_assert(mi < MAXMISSILES && mi >= 0);
	if (mienemy == MI_ENEMYMONST)
	{
		missile[mi]._midam = (random(60, 10) + random(60, 10) + 2 + plr[id]._pLevel) << 1;
		for (i = missile[mi]._mispllvl; i > 0; --i) missile[mi]._midam += (missile[mi]._midam >> 3);

		i = 16 + (missile[mi]._mispllvl << 1);
		if (i > 50) i = 50;

		UseMana(id, SPL_FIREBALL);
	}
	else
	{
		i = 16;
	}

	GetMissileVel(mi,sx,sy,dx,dy,i);
	SetMissDir(mi, GetDirection16(sx,sy,dx,dy));

	missile[mi]._mirange = 256;

	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = sx;
	missile[mi]._miVar5 = sy;

	missile[mi]._mlid = AddLight(sx, sy, 8);

	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddBigExplosion(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_FIREBALL) || !PRE_BETA
	app_assert(mi < MAXMISSILES && mi >= 0);
	if (mienemy == MI_ENEMYMONST || mienemy == MI_ENEMYBOTH)
	{
		missile[mi]._midam = (random(60, 10) + random(60, 10) + 2 + plr[id]._pLevel) << 1;
		for (int i = missile[mi]._mispllvl; i > 0; --i)
		{
			missile[mi]._midam += (missile[mi]._midam >> 3);
		}

		dam = missile[mi]._midam;
		CheckMissileCol(mi, dam, dam, 0, missile[mi]._mix-1, missile[mi]._miy-1, 1);
		CheckMissileCol(mi, dam, dam, 0, missile[mi]._mix  , missile[mi]._miy-1, 1);
		CheckMissileCol(mi, dam, dam, 0, missile[mi]._mix+1, missile[mi]._miy-1, 1);
		CheckMissileCol(mi, dam, dam, 0, missile[mi]._mix-1, missile[mi]._miy  , 1);
		CheckMissileCol(mi, dam, dam, 0, missile[mi]._mix  , missile[mi]._miy  , 1);
		CheckMissileCol(mi, dam, dam, 0, missile[mi]._mix+1, missile[mi]._miy  , 1);
		CheckMissileCol(mi, dam, dam, 0, missile[mi]._mix-1, missile[mi]._miy+1, 1);
		CheckMissileCol(mi, dam, dam, 0, missile[mi]._mix  , missile[mi]._miy+1, 1);
		CheckMissileCol(mi, dam, dam, 0, missile[mi]._mix+1, missile[mi]._miy+1, 1);
	}
	
	missile[mi]._mlid = AddLight(sx, sy, 8);
	SetMissDir(mi, 0);
	missile[mi]._miDelFlag = FALSE;
	
	missile[mi]._mirange = missile[mi]._miAnimLen - 1;
	

	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddSpiralFireBall(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_FIREBALL) || !PRE_BETA
	int i;

	if ((sx == dx) && (sy == dy)) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}

	app_assert(mi < MAXMISSILES && mi >= 0);
	if (mienemy == MI_ENEMYMONST)
	{
		missile[mi]._midam = (random(60, 10) + random(60, 10) + 2 + plr[id]._pLevel) << 1;
		for (i = missile[mi]._mispllvl; i > 0; --i) missile[mi]._midam += (missile[mi]._midam >> 3);

		i = 16 + (missile[mi]._mispllvl << 1);
		if (i > 50) i = 50;

		UseMana(id, SPL_FIREBALL);
	}
	else
	{
		i = 16;
	}

	GetMissileVel(mi,sx,sy,dx,dy,i);
	SetMissDir(mi, GetDirection16(sx,sy,dx,dy));

	missile[mi]._mirange = 256;

	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = sx;
	missile[mi]._miVar5 = sy;
	missile[mi]._miVar6	= 2;
	missile[mi]._miVar7 = 2;

	missile[mi]._mlid = AddLight(sx, sy, 8);

	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

void AddFBArrow(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_FIREBALL) || !PRE_BETA
	int i;

	if ((sx == dx) && (sy == dy)) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}

	app_assert(mi < MAXMISSILES && mi >= 0);
	if (mienemy == MI_ENEMYMONST)
	{
		i = 16 + missile[mi]._mispllvl;
		if (i > 50) i = 50;

//		UseMana(id, SPL_FIREBALL);
	}
	else
	{
		i = 16;
	}

	GetMissileVel(mi,sx,sy,dx,dy,i);
	SetMissDir(mi, GetDirection16(sx,sy,dx,dy));

	missile[mi]._mirange = 256;

	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = sx;
	missile[mi]._miVar5 = sy;

	missile[mi]._mlid = AddLight(sx, sy, 8);

	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddLightctrl(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (dam == 0 && mienemy == MI_ENEMYMONST) UseMana(id, SPL_LIGHTNING);  // if not 0 - probably is chain lightning

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;

	GetMissileVel(mi,sx,sy,dx,dy,32);

	missile[mi]._miAnimFrame = random(52, 8) + 1;

	missile[mi]._mirange = 256;
}

void AddLTArrow(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	app_assert(mi < MAXMISSILES && mi >= 0);
	
	if ((sx == dx) && (sy == dy)) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}

	GetMissileVel(mi,sx,sy,dx,dy,32);
	missile[mi]._miAnimFrame = random(52, 8) + 1;

	missile[mi]._mirange = 255;
	
	if (id < 0) {
		missile[mi]._miVar1 = sx;
		missile[mi]._miVar2 = sy;
	} else {
		missile[mi]._miVar1 = plr[id]._px;
		missile[mi]._miVar2 = plr[id]._py;
	}
	
	missile[mi]._midam <<= HP_SHIFT;

	//PutMissile(mi);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddLightning(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_LIGHTNING) || !PRE_BETA
	// Note: midir is used to pass in the missile # of the root of the lightning chain.
	//		 A negative midir means that this is not part of a chain.

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._misx = dx;
	missile[mi]._misy = dy;

	if(midir >= 0)
	{
		missile[mi]._mixoff = missile[midir]._mixoff;
		missile[mi]._miyoff = missile[midir]._miyoff;
		missile[mi]._mitxoff = missile[midir]._mitxoff;
		missile[mi]._mityoff = missile[midir]._mityoff;
	}

	missile[mi]._miAnimFrame = random(52, 8) + 1;

	if (midir >= 0 && mienemy != MI_ENEMYPLR && id != -1) {
		missile[mi]._mirange = 6 + (missile[mi]._mispllvl >> 1);
	} else {
		if (midir >= 0 && id != -1)	{
			missile[mi]._mirange = 10;
		} else missile[mi]._mirange = 8;
	}

	missile[mi]._mlid = AddLight(missile[mi]._mix, missile[mi]._miy, 4);

	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddMisexp(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	
	if (mienemy != MI_ENEMYMONST && id > 0) {
		app_assert(id < MAXMONSTERS);
		app_assert(monster[id].MType != NULL);
		switch (monster[id].MType->mtype)
		{
		case MT_SUCCUBUS:
			 SetMissAnim(mi,MF_FLAREXP);
			 break;
		case MT_SNOWWICH:
			 SetMissAnim(mi,MF_BFLAREXP);
			 break;
		case MT_HLSPWN:
			 SetMissAnim(mi,MF_DFLAREXP);
			 break;
		case MT_SOLBRNR:
			 SetMissAnim(mi,MF_CFLAREXP);
			 break;
		}
	}

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._mix = missile[dx]._mix;
	missile[mi]._miy = missile[dx]._miy;
	missile[mi]._misx = missile[dx]._misx;
	missile[mi]._misy = missile[dx]._misy;
	missile[mi]._mixoff = missile[dx]._mixoff;
	missile[mi]._miyoff = missile[dx]._miyoff;
	missile[mi]._mitxoff = missile[dx]._mitxoff;
	missile[mi]._mityoff = missile[dx]._mityoff;
	missile[mi]._mixvel = 0;
	missile[mi]._miyvel = 0;

	missile[mi]._mirange = missile[mi]._miAnimLen;
	missile[mi]._miVar1 = 0;

	//PutMissile(mi);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddWeapexp(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._mix = sx;
	missile[mi]._miy = sy;
	missile[mi]._misx = sx;
	missile[mi]._misy = sy;
	missile[mi]._mixvel = 0;
	missile[mi]._miyvel = 0;
	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = dx;
	
	missile[mi]._mimfnum = 0;
	if (dx == 1) SetMissAnim(mi, MF_EXP1);
	else SetMissAnim(mi, MF_CBOLT);
	missile[mi]._mirange = missile[mi]._miAnimLen - 1;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
BOOL CheckIfTrig(int x, int y)
{
	int i;

	app_assert(numtrigs <= MAXTRIGGERS);
	for (i = 0; i < numtrigs; ++i)
		if (((x == trigs[i]._tx) && (y == trigs[i]._ty)) ||
			((abs(trigs[i]._tx-x) < 2) && (abs(trigs[i]._ty-y) < 2))) return(TRUE);

	return(FALSE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddTown(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_TOWN) || !PRE_BETA
	int i, pn;
	int k, l, j;
	int tx, ty, mx;

	app_assert(mi < MAXMISSILES && mi >= 0);
	if (currlevel != 0) {
		missile[mi]._miDelFlag = TRUE;
		for (k = 0; k < 6; ++k) {
			l = CrawlNum[k];
			j = l + 1;
			for (i = CrawlTable[l]; i > 0; --i) {
				tx = dx + CrawlTable[j];
				ty = dy + CrawlTable[(j + 1)];
				if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
					pn = dPiece[tx][ty];
					app_assert(pn <= MAXTILES && pn >= 0); 
					if (((nSolidTable[pn] | dObject[tx][ty] | nMissileTable[pn] |
						dPlayer[tx][ty] | dMissile[tx][ty]) == 0) &&
						(!CheckIfTrig(tx, ty))) {
						missile[mi]._mix = tx;
						missile[mi]._miy = ty;
						missile[mi]._misx = tx;
						missile[mi]._misy = ty;
						missile[mi]._miDelFlag = FALSE;
						k = 6;
						break;
					}
				}
				j += 2;
			}
		}
	} else {
		tx = dx;
		ty = dy;
		missile[mi]._mix = tx;
		missile[mi]._miy = ty;
		missile[mi]._misx = tx;
		missile[mi]._misy = ty;
		missile[mi]._miDelFlag = FALSE;
	}

	missile[mi]._mirange = 100;
	missile[mi]._miVar1 = missile[mi]._mirange - (missile[mi]._miAnimLen);
	missile[mi]._miVar2 = 0;

	// Move current portal?
	for (i = 0; i < nummissiles; ++i) {
		mx = missileactive[i];
		if ((missile[mx]._mitype == MIT_TOWN) &&
			(mx != mi) &&
			(missile[mx]._misource == id)) {
				missile[mx]._mirange = 0;
		}
	}

	PutMissile(mi);

	if (id == myplr && missile[mi]._miDelFlag == FALSE && currlevel != 0) {
		if (!setlevel) NetSendCmdLocParam3(TRUE, CMD_ACTIVATEPORTAL, tx, ty, currlevel, leveltype, FALSE);
		else NetSendCmdLocParam3(TRUE, CMD_ACTIVATEPORTAL, tx, ty, setlvlnum, leveltype, TRUE);
	}
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddFlash(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_FLASH) || !PRE_BETA
	int i;

	app_assert(mi < MAXMISSILES && mi >= 0);
	if (mienemy == MI_ENEMYMONST) {
		if (id != -1) {
			missile[mi]._midam = 0;
			for (i = 0; i <= plr[id]._pLevel; ++i) missile[mi]._midam += random(55, 20) + 1;
			for (i = missile[mi]._mispllvl; i > 0; --i) missile[mi]._midam += (missile[mi]._midam >> 3);
			missile[mi]._midam += missile[mi]._midam >> 1;
			UseMana(id, SPL_FLASH);
		} else {
			missile[mi]._midam = (currlevel >> 1);
		}
	}
	else
		missile[mi]._midam = monster[id].mLevel << 1;
	
	missile[mi]._mirange = 19;

	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddAura(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_FLASH) || !PRE_BETA

#if defined(HELLFIRE2)
	app_assert(mi < MAXMISSILES && mi >= 0);
	if (mienemy == MI_ENEMYMONST) {
		if (id != -1) {
			missile[mi]._midam = 0;
			missile[mi]._mirange = 245 + (10 * missile[mi]._mispllvl) + (2 * (id > 0) ? plr[id]._pLevel : 1);
			plr[id]._pBaseToBlk += 50;
			UseMana(id, SPL_AURA);
		} 
	}
#endif
	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddAura2(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_FLASH) || !PRE_BETA

	app_assert(mi < MAXMISSILES && mi >= 0);
	if (mienemy == MI_ENEMYMONST) {
		if (id != -1) {
			missile[mi]._midam = 0;
			missile[mi]._mirange = 245 + (10 * missile[mi]._mispllvl) + (2 * (id > 0) ? plr[id]._pLevel : 1);
		} 
	}

	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddFlash2(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_FLASH) || !PRE_BETA
	int i;

	app_assert(mi < MAXMISSILES && mi >= 0);
	if (mienemy == MI_ENEMYMONST)
	{
		if (id != -1) {
			missile[mi]._midam = 0;
			for (i = 0; i <= plr[id]._pLevel; ++i) missile[mi]._midam += random(56, 2) + 1;
			for (i = missile[mi]._mispllvl; i > 0; --i) missile[mi]._midam += (missile[mi]._midam >> 3);
			missile[mi]._midam += missile[mi]._midam >> 1;
		} else {
			missile[mi]._midam = (currlevel >> 1);
		}
	}

	missile[mi]._miPreFlag = TRUE;

	missile[mi]._mirange = 19;

	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddManashield(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_MANASHLD) || !PRE_BETA
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._mirange = ((plr[id]._pLevel << 4) << 1) + (plr[id]._pLevel << 4);

	missile[mi]._miVar1 = plr[id]._pHitPoints;
	missile[mi]._miVar2 = plr[id]._pHPBase;

	missile[mi]._miVar8 = KILL_UNKNOWN;		// killed by unknown source (inited)

	if (mienemy == MI_ENEMYMONST) UseMana(id, SPL_MANASHLD);
	
	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddFiremove(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._midam = random(59, 10) + 1 + plr[id]._pLevel;

	GetMissileVel(mi,sx,sy,dx,dy,16);

	missile[mi]._mirange = 255;
	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = 0;

	++missile[mi]._mix;
	++missile[mi]._miy;
	missile[mi]._miyoff-=32;
//PutMissile(mi);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddGuardian(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_GUARDIAN) || !PRE_BETA
	int i, pn;
	int k, l, j;
	int tx, ty;

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._midam = random(62, 10) + 1 + (plr[id]._pLevel >> 1);
	for (i = missile[mi]._mispllvl; i > 0; --i) missile[mi]._midam += (missile[mi]._midam >> 3);
	
	missile[mi]._miDelFlag = TRUE;
	for (k = 0; k < 6; ++k) {
		l = CrawlNum[k];
		j = l + 1;
		for (i = CrawlTable[l]; i > 0; --i) {
			tx = dx + CrawlTable[j];
			ty = dy + CrawlTable[(j + 1)];
			app_assert(tx < MAXDUNX && tx >= 0);
			app_assert(ty < MAXDUNY && ty >= 0);
			pn = dPiece[tx][ty];
			app_assert(pn <= MAXTILES); 
			if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
				//if ((dFlags[tx][ty] & BFLAG_VISIBLE) &&
				if ((LineClear(sx, sy, tx, ty)) &&
					((nSolidTable[pn] | dMonster[tx][ty] | dObject[tx][ty] | nMissileTable[pn] | dMissile[tx][ty]) == 0)) {
					missile[mi]._mix = tx;
					missile[mi]._miy = ty;
					missile[mi]._misx = tx;
					missile[mi]._misy = ty;
					missile[mi]._miDelFlag = FALSE;
					UseMana(id, SPL_GUARDIAN);
					k = 6;
					break;
				}
			}
			j += 2;
		}
	}

	if (missile[mi]._miDelFlag == TRUE) return;
	missile[mi]._misource = id;
	missile[mi]._mlid = AddLight(missile[mi]._mix, missile[mi]._miy, 1);
	
	missile[mi]._mirange = (plr[id]._pLevel >> 1) + missile[mi]._mispllvl;
	missile[mi]._mirange = missile[mi]._mirange + ((plr[id]._pISplDur * missile[mi]._mirange) >> 7);
	if (missile[mi]._mirange > 30) missile[mi]._mirange = 30;
	missile[mi]._mirange = missile[mi]._mirange << 4;
	if (missile[mi]._mirange < 30) missile[mi]._mirange = 30;

	missile[mi]._miVar1 = missile[mi]._mirange - (missile[mi]._miAnimLen);
	missile[mi]._miVar2 = 0;
	missile[mi]._miVar3 = 1;

	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddChain(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_CHAIN) || !PRE_BETA
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._miVar1 = dx;
	missile[mi]._miVar2 = dy;

	missile[mi]._mirange = 1;
	UseMana(id, SPL_CHAIN);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
/*void AddChainOLD(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i, k, j, mx;

	if (dam == 0) {
		k = 0;
		j = -1;
		for (i = 0; i < nummissilevars; ++i) {
			mx = missilevars[i][0];
			if (mx >= 0 && missile[mx]._mitype == MIT_CHAIN && missile[mx]._miVar8 > k) k = missile[mx]._miVar8;
			if (mx < 0) j = i;
		}
		++k;
		missile[mi]._miVar8 = k;
		if (j == -1) {
			++nummissilevars;
			j = nummissilevars;
		}
		missilevars[j][0] = mi;
		missilevars[j][1] = MIT_CHAIN;
		missilevars[j][2] = 0;
	} else {
		for (i = 0; i < nummissilevars; ++i) {
			mx = missilevars[i][0];
			if (mx >= 0 && missile[mx]._mitype == MIT_CHAIN && missile[mx]._miVar8 == dam) missilevars[i][2]++;
		}
	}

	k = 0;
	missile[mi]._midam = 0;
	for (i = 0; i < nummissiles; ++i) {
		mx = missileactive[i];
		if (missile[mx]._mitype == MIT_CHAIN && missile[mx]._midam > k) k = missile[mx]._midam;
		if (missile[mx]._mitype == MIT_CHAINBALL && missile[mx]._miVar1 > k) k = missile[mx]._miVar1;
	}
	++k;
	missile[mi]._midam = k;

	missile[mi]._mirange = 12 + GetSpellLevel(id, SPL_LIGHTNING);
	missile[mi]._mirange = missile[mi]._mirange + ((plr[id]._pISplDur * missile[mi]._mirange) >> 7);

	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = 0;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = 0;
	missile[mi]._miVar5 = dx;
	missile[mi]._miVar6 = dy;
	missile[mi]._miVar7 = 0;
	
	UseMana(id, SPL_CHAIN);
}*/

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
/*void AddChainballOLD(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i, mx;

	missile[mi]._miVar1 = dam;
	missile[mi]._miVar2 = 0;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = 0;

	for (i = 0; i < nummissiles; ++i) {
		mx = missileactive[i];
		if ((missile[mx]._mitype == MIT_CHAIN) && (missile[mx]._miVar2 == 1) &&	(missile[mx]._midam == dam)) {
			missile[mi]._miVar2 = missile[mx]._miVar2;
			missile[mi]._miVar3 = missile[mx]._miVar3;
			missile[mi]._miVar4 = missile[mx]._miVar4;
		}
		if ((missile[mx]._mitype == MIT_CHAIN) &&	(missile[mx]._midam == dam)) 
			missile[mi]._miVar8 = missile[mx]._miVar8;
	}

	if (dMonster[sx][sy] != 0) {
		missile[mi]._miVar5 = sx;
		missile[mi]._miVar6 = sy;
	} else {
		missile[mi]._miVar5 = 0;
		missile[mi]._miVar6 = 0;
	}

	missile[mi]._midam = (random(61, plr[id]._pLevel) + random(61, 6) + 4) << HP_SHIFT;

	GetMissileVel(mi, sx, sy, dx, dy, 16);

	missile[mi]._mirange = ((plr[id]._pLevel << 4) << 1) + (GetSpellLevel(id, SPL_CHAIN) << 4);

	PutMissile(mi);
}*/

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddBlood(int mi, int sx, int sy, int str, int dy, int midir, char mienemy, int id, int dam)
{
	SetMissDir(mi, str);
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._midam = 0;

	missile[mi]._miLightFlag = TRUE;

	missile[mi]._mirange = 250;
	//PutMissile(mi);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddBone(int mi, int sx, int sy, int str, int dy, int midir, char mienemy, int id, int dam)
{
	if (str > 3) str = 2;
	SetMissDir(mi, str);

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._midam = 0;

	missile[mi]._miLightFlag = TRUE;

	missile[mi]._mirange = 250;
	//PutMissile(mi);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddMetal(int mi, int sx, int sy, int str, int dy, int midir, char mienemy, int id, int dam)
{
	if (str > 3) str = 2;
	SetMissDir(mi, str);

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._midam = 0;

	missile[mi]._miLightFlag = TRUE;

	missile[mi]._mirange = (missile[mi]._miAnimLen);
	//PutMissile(mi);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddRhino(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	AnimStruct *anim;
	
	app_assert(id < MAXMONSTERS && id >= 0);
	app_assert(monster[id].MType != NULL);
	if (EquivMonst(monster[id].MType->mtype, MT_HORNED))
		anim = &monster[id].MType->Anims[MA_SPECIAL];
	else if (EquivMonst(monster[id].MType->mtype, MT_NSNAKE))
		anim = &monster[id].MType->Anims[MA_ATTACK];
	else
		anim = &monster[id].MType->Anims[MA_WALK];

	GetMissileVel(mi,sx,sy,dx,dy,18);

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._mimfnum = midir;
	missile[mi]._miAnimFlags = NULL;
	missile[mi]._miAnimData = anim->Cels[midir];
	missile[mi]._miAnimDelay = anim->Rate;
	missile[mi]._miAnimLen = anim->Frames;
	missile[mi]._miAnimWidth = monster[id].MType->mAnimWidth;
	missile[mi]._miAnimWidth2 = monster[id].MType->mAnimWidth2;
	missile[mi]._miAnimAdd = 1;
	if (EquivMonst(monster[id].MType->mtype, MT_NSNAKE))
		missile[mi]._miAnimFrame = 7;

	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = 0;

	missile[mi]._miLightFlag = TRUE;
	if(monster[id]._uniqtype) {
		missile[mi]._miUniqTrans = monster[id]._uniqtrans+1;
		missile[mi]._mlid = monster[id].mlid;
	}

	missile[mi]._mirange = 256;

	PutMissile(mi);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddFireman(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	AnimStruct *anim;

	app_assert(id < MAXMONSTERS && id >= 0);
	app_assert(monster[id].MType != NULL);
	anim = &monster[id].MType->Anims[MA_WALK];

	GetMissileVel(mi,sx,sy,dx,dy,16);

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._mimfnum = midir;
	missile[mi]._miAnimFlags = NULL;
	missile[mi]._miAnimData = anim->Cels[midir];
	missile[mi]._miAnimDelay = anim->Rate;
	missile[mi]._miAnimLen = anim->Frames;
	missile[mi]._miAnimWidth = monster[id].MType->mAnimWidth;
	missile[mi]._miAnimWidth2 = monster[id].MType->mAnimWidth2;
	missile[mi]._miAnimAdd = 1;

	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = 0;

	missile[mi]._miLightFlag = TRUE;
	if(monster[id]._uniqtype)
		missile[mi]._miUniqTrans = monster[id]._uniqtrans+1;

	dMonster[monster[id]._mx][monster[id]._my] = 0;

	missile[mi]._mirange = 256;

	PutMissile(mi);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddFlare(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int d;

	if ((sx == dx) && (sy == dy)) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}

	GetMissileVel(mi,sx,sy,dx,dy,16);

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._mirange = 256;

	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._mlid = AddLight(sx, sy, 8);

	if (mienemy == MI_ENEMYMONST) {
		UseMana(id, SPL_BSTAR);
		d = 5;
		//for (k = missile[mi]._mispllvl; k > 0; --k) d -= 1;
		//if (d <= 0) d = 1;	
#if CHEATS
		if (simplecheat || cheatflag) d = 0;
#endif

		plr[id]._pHitPoints -= (d << HP_SHIFT);
		plr[id]._pHPBase -= (d << HP_SHIFT);
		drawhpflag = TRUE;

		if (plr[id]._pHitPoints <= 0) {
			// rjs - manashld fix? - plr[id]._pHitPoints = 0;
			StartPlrKill(id, FALSE);
		}
	} else {
		if (id > 0) {
			if (monster[id].MType->mtype == MT_SUCCUBUS) SetMissAnim(mi,MF_FLARE);
			if (monster[id].MType->mtype == MT_SNOWWICH) SetMissAnim(mi,MF_BFLARE);
			if (monster[id].MType->mtype == MT_HLSPWN)   SetMissAnim(mi,MF_DFLARE);
			if (monster[id].MType->mtype == MT_SOLBRNR)  SetMissAnim(mi,MF_CFLARE);
		}
	}
	
	if (misfiledata[missile[mi]._miAnimType].mAnimFAmt == 16) {
		SetMissDir(mi, GetDirection16(sx,sy,dx,dy));
	}

	//PutMissile(mi);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddAcid(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	GetMissileVel(mi,sx,sy,dx,dy,16);

	SetMissDir(mi, GetDirection16(sx,sy,dx,dy));

	app_assert(mi < MAXMISSILES && mi >= 0);
	if ((missile[mi]._mixvel >> 16) == 0
		&& (missile[mi]._miyvel >> 16) == 0)
	{
		missile[mi]._mirange = 1;
	}
	else
	{
		missile[mi]._mirange = 15 + 5*(monster[id]._mint+1);
	}
	missile[mi]._mlid = -1;

	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;

	PutMissile(mi);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
/*void AddDoom(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i, j, k, l;
	int mid;

	// search in a 5 radius around tile clicked for monster 
	for (k = 0; k < 6; ++k) {
		l = CrawlNum[k];
		j = l + 1;
		for (i = CrawlTable[l]; i > 0; --i) {
			mid = dMonster[(dx + CrawlTable[j])][(dy + CrawlTable[(j + 1)])];
			if (mid != 0) {
				if (mid > 0) --mid;
				else mid = -(mid + 1);
				if ((monster[mid]._mhitpoints >> HP_SHIFT) > 0) {
					missile[mi]._miVar1 = dx + CrawlTable[j];
					missile[mi]._miVar2 = dy + CrawlTable[j + 1];
					missile[mi]._miVar3 = mid;
					i = -99;
					k = 6;
					break;
				}
			}
			j += 2;
		}
	}

	// if no monsters found in search
	if (i != -99) {
		missile[mi]._miDelFlag = TRUE;
		return;
	}

	GetMissileVel(mi, sx, sy, missile[mi]._miVar1, missile[mi]._miVar2, 16);
	SetMissDir(mi, GetDirection(sx, sy, missile[mi]._miVar1, missile[mi]._miVar2));

	missile[mi]._mix = sx;
	missile[mi]._miy = sy;
	missile[mi]._misx = sx;
	missile[mi]._misy = sy;
	missile[mi]._misource = id;
	missile[mi]._midam = 26;

	missile[mi]._mirange = ((plr[id]._pLevel << 4) >> 2) + (plr[id]._pLevel << 4) ;
	for (i = GetSpellLevel(id, SPL_DOOM); i > 0; --i) missile[mi]._mirange += ((missile[mi]._mirange << 4) >> 3);
	missile[mi]._mirange = missile[mi]._mirange + ((plr[id]._pISplDur * missile[mi]._mirange) >> 7);

	UseMana(id, SPL_DOOM);
	PutMissile(mi);
}*/

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddFireonly(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._midam = dam;
	missile[mi]._mixvel = 0;
	missile[mi]._miyvel = 0;

	missile[mi]._mirange = 50;
	missile[mi]._miVar1 = missile[mi]._mirange - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;

	//PutMissile(mi);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddAcidpud(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int monst;

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._mixvel = 0;
	missile[mi]._miyvel = 0;

	missile[mi]._mixoff = 0;
	missile[mi]._miyoff = 0;
/*
	missile[mi]._misx = missile[dx]._misx;
	missile[mi]._misy = missile[dx]._misy;
	missile[mi]._mixoff = missile[dx]._mixoff;
	missile[mi]._miyoff = missile[dx]._miyoff;
	missile[mi]._mitxoff = missile[dx]._mitxoff;
	missile[mi]._mityoff = missile[dx]._mityoff;
*/

	missile[mi]._miLightFlag = TRUE;

//	MoveMissilePos(mi);

	monst = missile[mi]._misource;
	missile[mi]._mirange = random(50,15) + 40*(monster[monst]._mint + 1);

	missile[mi]._miPreFlag = TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddStone(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_STONE) || !PRE_BETA
	int i, j, k, l, tx, ty;
	int mid;

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._misource = id;

	// search in a 5 radius around tile clicked for monster 
	for (k = 0; k < 6; ++k) {
		l = CrawlNum[k];
		j = l + 1;
		for (i = CrawlTable[l]; i > 0; --i) {
			tx = dx + CrawlTable[j];
			ty = dy + CrawlTable[(j + 1)];
			if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
				mid = dMonster[tx][ty];
				if (mid > 0) --mid;
				else mid = -(mid + 1);
				if ((mid > 3)
					&& (monster[mid]._mAi != AI_DIABLO)
					&& (monster[mid].MType->mtype != MT_NKR)
					&& (monster[mid]._mmode != MM_FADEIN)
					&& (monster[mid]._mmode != MM_FADEOUT)
					//&& (monster[mid]._mmode != MM_DEATH)
					&& (monster[mid]._mmode != MM_MISSILE)) {
					i = -99;
					k = 6;
					missile[mi]._miVar1 = monster[mid]._mmode;
					missile[mi]._miVar2 = mid;
					monster[mid]._mmode = MM_STONE;
					break;
				}
			}
			j += 2;
		}
	}

	// if no monsters found in search
	if (i != -99) {
		missile[mi]._miDelFlag = TRUE;
		return;
	}

	missile[mi]._mix = tx;
	missile[mi]._miy = ty;
	missile[mi]._misx = missile[mi]._mix;
	missile[mi]._misy = missile[mi]._miy;
	
	missile[mi]._mirange = 6 + missile[mi]._mispllvl;
	//if (missile[mi]._mirange == 0) missile[mi]._mirange = 1;
	//for (i = missile[mi]._mispllvl; i > 0; --i) missile[mi]._mirange += (missile[mi]._mirange >> 3);
	missile[mi]._mirange = missile[mi]._mirange + ((plr[id]._pISplDur * missile[mi]._mirange) >> 7);
	if (missile[mi]._mirange > 15) missile[mi]._mirange = 15;
	missile[mi]._mirange = missile[mi]._mirange << 4;
	UseMana(id, SPL_STONE);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
/*void AddInvis(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;

	missile[mi]._mirange = plr[id]._pLevel << 4;
	for (i = GetSpellLevel(id, SPL_INVIS); i > 0; --i) missile[mi]._mirange += ((missile[mi]._mirange << 4) >> 3);
	missile[mi]._mirange = missile[mi]._mirange + ((plr[id]._pISplDur * missile[mi]._mirange) >> 7);
	UseMana(id, SPL_INVIS);
	missile[mi]._miDelFlag = TRUE;
}*/

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddGolem(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_GOLEM) || !PRE_BETA
	int i, mx;
	//int tx, ty, k, j, l;

	app_assert(mi < MAXMISSILES && mi >= 0);
	
	missile[mi]._miDelFlag = FALSE;
	for (i = 0; i < nummissiles; ++i) {
		mx = missileactive[i];
		if ((missile[mx]._mitype == MIT_GOLEM) && (mx != mi) &&
			(missile[mx]._misource == id)) {
			missile[mi]._miDelFlag = TRUE;
			return;
		}
	}

	/*for (k = 0; k < 6; ++k) {
		l = CrawlNum[k];
		j = l + 1;
		for (i = CrawlTable[l]; i > 0; --i) {
			tx = dx + CrawlTable[j];
			ty = dy + CrawlTable[(j + 1)];
			if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
				pn = dPiece[tx][ty];
				if ((LineClear(sx, sy, tx, ty)) &&
					((nSolidTable[pn] | dMonster[tx][ty] | dObject[tx][ty]) == 0)) {*/
					missile[mi]._miVar1 = sx;
					missile[mi]._miVar2 = sy;
					missile[mi]._miVar4 = dx;
					missile[mi]._miVar5 = dy;
					if ((monster[id]._mx != 1 || monster[id]._my != 0) && (id == myplr)) M_StartKill(id, id);
					//missile[mi]._miDelFlag = FALSE;
					UseMana(id, SPL_GOLEM);
					/*k = 6;
					break;
				}
			}
			j += 2;
		}
	}*/
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddEther(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_ETHER) || !PRE_BETA
	int i;

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._mirange = (plr[id]._pLevel << 4) >> 1;
	for (i = missile[mi]._mispllvl; i > 0; --i) missile[mi]._mirange += (missile[mi]._mirange >> 3);
	missile[mi]._mirange = missile[mi]._mirange + ((plr[id]._pISplDur * missile[mi]._mirange) >> 7);

	missile[mi]._miVar1 = plr[id]._pHitPoints;
	missile[mi]._miVar2 = plr[id]._pHPBase;

	if (mienemy == MI_ENEMYMONST) UseMana(id, SPL_ETHER);

	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddBloodR(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_BLOODR) || !PRE_BETA
	int manaval;

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._miDelFlag = TRUE;
	return; // spell deleted - left code here just in case

	if (!(plr[id]._pIFlags & IAF_LMANA)) {
		plr[id]._pHitPoints -= (10 << HP_SHIFT);
		plr[id]._pHPBase -= (10 << HP_SHIFT);
	
		manaval = ((missile[mi]._mispllvl + 8) << MANA_SHIFT);
		plr[id]._pMana += manaval;
		plr[id]._pManaBase += manaval;

		if (plr[id]._pMana > plr[id]._pMaxMana) plr[id]._pMana = plr[id]._pMaxMana;
		if (plr[id]._pManaBase > plr[id]._pMaxManaBase) plr[id]._pManaBase = plr[id]._pMaxManaBase;
	
		if (plr[id]._pHitPoints <= 0) {
			// rjs - manashld fix? - plr[id]._pHitPoints = 0;
			StartPlrKill(id, FALSE);
		}
	}

	UseMana(id, SPL_BLOODR);
	drawhpflag = TRUE;
	missile[mi]._miDelFlag = TRUE;
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddSpurt(int mi, int sx, int sy, int str, int dy, int midir, char mienemy, int id, int dam)
{
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._midam = dam;
	missile[mi]._mix = sx;
	missile[mi]._miy = sy;
	missile[mi]._misx = sx;
	missile[mi]._misy = sy;
	missile[mi]._misource = id;

	if (dam == 1)
		SetMissDir(mi, 0);
	else
		SetMissDir(mi, 1);

	missile[mi]._miLightFlag = TRUE;

	missile[mi]._mirange = missile[mi]._miAnimLen;
	//PutMissile(mi);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddBoom(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._mix = dx;
	missile[mi]._miy = dy;
	missile[mi]._misx = dx;
	missile[mi]._misy = dy;
	missile[mi]._mixvel = 0;
	missile[mi]._miyvel = 0;
	missile[mi]._midam = dam;

	missile[mi]._mirange = missile[mi]._miAnimLen;
	missile[mi]._miVar1 = 0;

	//PutMissile(mi);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddHeal(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_HEAL) || !PRE_BETA
	int i;
	long l;

	l = (random(57, 10) + 1) << HP_SHIFT;
	for (i = 0; i < plr[id]._pLevel; ++i) l += ((random(57, 4) + 1) << HP_SHIFT);
	app_assert(mi < MAXMISSILES && mi >= 0);
	for (i = 0; i < missile[mi]._mispllvl; ++i) l += ((random(57, 6) + 1) << HP_SHIFT);
	if (plr[id]._pClass == CLASS_WARRIOR
		|| plr[id]._pClass == CLASS_BARBARIAN
		|| plr[id]._pClass == CLASS_MONK) l = l << 1;
	if (plr[id]._pClass == CLASS_ROGUE
		|| plr[id]._pClass == CLASS_BARD) l += (l >> 1);
	plr[id]._pHitPoints += l;
	if (plr[id]._pHitPoints > plr[id]._pMaxHP) plr[id]._pHitPoints = plr[id]._pMaxHP;
	plr[id]._pHPBase += l;
	if (plr[id]._pHPBase > plr[id]._pMaxHPBase) plr[id]._pHPBase = plr[id]._pMaxHPBase;

	UseMana(id, SPL_HEAL);
	drawhpflag = TRUE;

	missile[mi]._miDelFlag = TRUE;
	
	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddMana(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_HEAL) || !PRE_BETA
	int i;
	long l;

	l = (random(57, 10) + 1) << MANA_SHIFT;
	for (i = 0; i < plr[id]._pLevel; ++i) l += ((random(57, 4) + 1) << MANA_SHIFT);
	app_assert(mi < MAXMISSILES && mi >= 0);
	for (i = 0; i < missile[mi]._mispllvl; ++i) l += ((random(57, 6) + 1) << MANA_SHIFT);
	if (plr[id]._pClass == CLASS_SORCEROR) l = l << 1;
	if (plr[id]._pClass == CLASS_ROGUE
		|| plr[id]._pClass == CLASS_BARD) l += (l >> 1);
	
	plr[id]._pMana += l;
	if (plr[id]._pMana > plr[id]._pMaxMana) plr[id]._pMana = plr[id]._pMaxMana;
	plr[id]._pManaBase += l;
	if (plr[id]._pManaBase > plr[id]._pMaxManaBase) plr[id]._pManaBase = plr[id]._pMaxManaBase;

	UseMana(id, SPL_MANA);
	drawmanaflag = TRUE;

	missile[mi]._miDelFlag = TRUE;
	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

void AddFMana(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_HEAL) || !PRE_BETA
	app_assert(mi < MAXMISSILES && mi >= 0);
	plr[id]._pMana = plr[id]._pMaxMana;
	plr[id]._pManaBase = plr[id]._pMaxManaBase;
	UseMana(id, SPL_FMANA);
	drawmanaflag = TRUE;
#endif
	missile[mi]._miDelFlag = TRUE;
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddHealOther(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_HEALOTHER) || !PRE_BETA
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, SPL_HEALOTHER);
	if (id == myplr) {
		NewCursor(HEALOTHER_CURS);
	}
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddElement(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_ELEMENT) || !PRE_BETA
	int i;

	if ((sx == dx) && (sy == dy)) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}

	//missile[mi]._midam = 0;
	//for (i = 0; i < plr[id]._pLevel; ++i) missile[mi]._midam += random(67, 6) + 1;
	//for (i = missile[mi]._mispllvl; i > 0; --i) missile[mi]._midam += (missile[mi]._midam >> 3);
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._midam = (random(60, 10) + random(60, 10) + 2 + plr[id]._pLevel) << 1;
	for (i = missile[mi]._mispllvl; i > 0; --i) missile[mi]._midam += (missile[mi]._midam >> 3);
	missile[mi]._midam = missile[mi]._midam >> 1;

	GetMissileVel(mi, sx, sy, dx, dy, 16);
	SetMissDir(mi, GetDirection8(sx, sy, dx, dy));

	missile[mi]._mirange = 256;

	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = dx;
	missile[mi]._miVar5 = dy;

	missile[mi]._mlid = AddLight(sx, sy, 8);

	UseMana(id, SPL_ELEMENT);
	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddIdentify(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_IDENTIFY) || !PRE_BETA
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, SPL_IDENTIFY);
	if (id == myplr) {
		if (sbookflag) sbookflag = FALSE;
		if (!invflag) invflag = TRUE;
		NewCursor(IDENTIFY_CURS);
	}
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddFirewallC(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_WALL) || !PRE_BETA
	int i, pn;
	int k, l, j;
	int tx, ty;

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._miDelFlag = TRUE;
	for (k = 0; k < 6; ++k) {
		l = CrawlNum[k];
		j = l + 1;
		for (i = CrawlTable[l]; i > 0; --i) {
			tx = dx + CrawlTable[j];
			ty = dy + CrawlTable[(j + 1)];
			if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
				pn = dPiece[tx][ty];
				//if ((dFlags[tx][ty] & BFLAG_VISIBLE) && ((sx != tx) || (sy != ty)) &&
				if ((LineClear(sx, sy, tx, ty)) && ((sx != tx) || (sy != ty)) &&
					((nSolidTable[pn] | dObject[tx][ty]) == 0)) {
					missile[mi]._miVar1 = tx;
					missile[mi]._miVar2 = ty;
					missile[mi]._miVar5 = tx;
					missile[mi]._miVar6 = ty;
					missile[mi]._miDelFlag = FALSE;
					k = 6;
					break;
				}
			}
			j += 2;
		}
	}

	if (missile[mi]._miDelFlag == TRUE) return;

	missile[mi]._miVar7 = 0;
	missile[mi]._miVar8 = 0;

	//midir = GetDirection(sx, sy, tx, ty);
	missile[mi]._miVar3 = (midir - 2) & 0x0007;
	missile[mi]._miVar4 = (midir + 2) & 0x0007;

	missile[mi]._mirange = 7;
	UseMana(id, SPL_WALL);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddFlameBox(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_WALL) || !PRE_BETA

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._miDelFlag = TRUE;
	
	if (mienemy == MI_ENEMYMONST){
		UseMana(id, SPL_RINGOFFIRE);
	}
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._miDelFlag = FALSE;

	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = 0;
	missile[mi]._miVar5 = 0;
	missile[mi]._miVar6 = 0;
	missile[mi]._miVar7 = 0;
	missile[mi]._miVar8 = 0;

	missile[mi]._mirange = 7;
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddShowMagicItems(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_WALL) || !PRE_BETA

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._miDelFlag = FALSE;
	
	missile[mi]._miVar1 = id;
	missile[mi]._miVar2 = 0;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = 0;
	missile[mi]._miVar5 = 0;
	missile[mi]._miVar6 = 0;
	missile[mi]._miVar7 = 0;
	missile[mi]._miVar8 = 0;

	HighLightAllItems = true;
	missile[mi]._mirange = 245 + (10 * missile[mi]._mispllvl) + (2 * (id > 0) ? plr[id]._pLevel : 1);
	if (mienemy == MI_ENEMYMONST){
		UseMana(id, SPL_SHOWMAGITEMS);
	}
	
	for(int i = 0; i < nummissiles; ++i) {
		int const miActive = missileactive[i];
		if (miActive != mi) {
			MissileStruct * const pMiss = &missile[miActive];
			
			if (pMiss->_miVar1 == id
				&& pMiss->_mitype == MIT_SHOWMAGITEMS) {
				
				// to prevent ridiculous overflow.
				if (pMiss->_mirange < (INT_MAX - missile[mi]._mirange)) {
					pMiss->_mirange += missile[mi]._mirange;
				}
				missile[mi]._miDelFlag = TRUE;
				break;
			}
		}
	}
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddInfra(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_INFRA) || !PRE_BETA
	int i;

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._mirange = 99 << 4;
	for (i = missile[mi]._mispllvl; i > 0; --i) missile[mi]._mirange += (missile[mi]._mirange >> 3);
	missile[mi]._mirange = missile[mi]._mirange + ((plr[id]._pISplDur * missile[mi]._mirange) >> 7);

	if (mienemy == MI_ENEMYMONST) UseMana(id, SPL_INFRA);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddWave(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_WAVE) || !PRE_BETA
	missile[mi]._miVar1 = dx;
	missile[mi]._miVar2 = dy;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = 0;
	missile[mi]._mirange = 1;
	missile[mi]._miAnimFrame = 4;
	UseMana(id, SPL_WAVE);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddNova(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_NOVA) || !PRE_BETA
	int k;

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._miVar1 = dx;
	missile[mi]._miVar2 = dy;
	if (id != -1) {
		missile[mi]._midam = random(66, 6) + random(66, 6) + random(66, 6) + random(66, 6) + random(66, 6);
		missile[mi]._midam += 5 + plr[id]._pLevel;
		missile[mi]._midam = missile[mi]._midam >> 1;
		for (k = missile[mi]._mispllvl; k > 0; --k) missile[mi]._midam += (missile[mi]._midam >> 3);
		if (mienemy == MI_ENEMYMONST) UseMana(id, SPL_NOVA);
	} else {
		missile[mi]._midam = random(66, 3) + random(66, 3) + random(66, 3);
		missile[mi]._midam += (currlevel >> 1);
	}
	missile[mi]._mirange = 1;
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddBoil(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	// not in game
	missile[mi]._miDelFlag = TRUE;
	return;
/*
#if (PRE_BETA && PRE_BLOODB) || !PRE_BETA
	missile[mi]._miVar1 = dx;
	missile[mi]._miVar2 = dy;
	missile[mi]._mirange = 1;
#else
	missile[mi]._miDelFlag = TRUE;
#endif*/
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddRage(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (id != -1) {
		if ((0 == (SF_RAGE & plr[id]._pSpellFlags))	// Only one rage at a time.
			&& (0 == (SF_LETHERGY & plr[id]._pSpellFlags))	// Can't rage while lethargic either.
			&& plr[id]._pHitPoints > (plr[id]._pLevel << HP_SHIFT)) {	// And not too weak.
			const int Sounds[NUM_CLASSES] = { PS_WARR70,
											  PS_ROGUE70,
											  PS_MAGE70,
											  PS_MAGE70,	// monk
											  PS_BARD70,
											  PS_BARBARIAN70 };
			UseMana(id, SPL_RAGE);
			missile[mi]._miVar1 = id;
			int const hps = (plr[id]._pLevel * 6) << HP_SHIFT;
			plr[id]._pSpellFlags |= SF_RAGE;
			missile[mi]._miVar2 = hps;
			missile[mi]._mirange = 245 + (10 * missile[mi]._mispllvl) + (2 * (id > 0) ? plr[id]._pLevel : 1);
			CalcPlrItemVals(id, TRUE);
			force_redraw = FULLDRAW;
			PlaySfxLoc(Sounds[plr[id]._pClass], plr[id]._px, plr[id]._py);
		}
		else {
			// Too weak to become enraged.
			missile[mi]._miDelFlag = TRUE;
		}
	}
	else {
		missile[mi]._miDelFlag = TRUE;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddRepair(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_REPAIR) || !PRE_BETA
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, SPL_REPAIR);
	if (id == myplr) {
		if (sbookflag) sbookflag = FALSE;
		if (!invflag) invflag = TRUE;
		NewCursor(REPAIR_CURS);
	}
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddRecharge(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_RECHARGE) || !PRE_BETA
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, SPL_RECHARGE);
	if (id == myplr) {
		if (sbookflag) sbookflag = FALSE;
		if (!invflag) invflag = TRUE;
		NewCursor(RECHARGE_CURS);
	}
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddDisarm(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_DISARM) || !PRE_BETA
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, SPL_DISARM);
	if (id == myplr) {
		NewCursor(DISARM_CURS);
	}
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddApoca(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_APOCA) || !PRE_BETA
	int i;

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._miVar1 = 8;
	missile[mi]._miVar2 = sy - missile[mi]._miVar1;
	missile[mi]._miVar3 = sy + missile[mi]._miVar1;
	missile[mi]._miVar4 = sx - missile[mi]._miVar1;
	missile[mi]._miVar5 = sx + missile[mi]._miVar1;
	missile[mi]._miVar6 = missile[mi]._miVar4;

	if (missile[mi]._miVar2 <= 0) missile[mi]._miVar2 = 1;
	if (missile[mi]._miVar3 >= MAXDUNY) missile[mi]._miVar3 = MAXDUNY - 1;
	if (missile[mi]._miVar4 <= 0) missile[mi]._miVar4 = 1;
	if (missile[mi]._miVar5 >= MAXDUNX) missile[mi]._miVar5 = MAXDUNX - 1;
	
	for (i = 0; i < plr[id]._pLevel; ++i) missile[mi]._midam += random(67, 6) + 1;

	missile[mi]._mirange = 255;
	missile[mi]._miDelFlag = FALSE;
	UseMana(id, SPL_APOCA);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}
 
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddFlame(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int seqno)
{
#if (PRE_BETA && PRE_FLAME) || !PRE_BETA
	// seqno: the number of flame chunk for this spell instance
	int i;

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._miVar2 = 0;
	//for (i = 0; i < 2 - seqno; ++i) {
	for (i = seqno; i > 0; --i) {
		missile[mi]._miVar2 += 5;
	}

	missile[mi]._misx = dx;
	missile[mi]._misy = dy;

	missile[mi]._mixoff = missile[midir]._mixoff;
	missile[mi]._miyoff = missile[midir]._miyoff;
	missile[mi]._mitxoff = missile[midir]._mitxoff;
	missile[mi]._mityoff = missile[midir]._mityoff;
	missile[mi]._mirange = 20 + missile[mi]._miVar2;
	missile[mi]._mlid = AddLight(sx, sy, 1);

	if (mienemy == MI_ENEMYMONST) {
		missile[mi]._midam = (random(79, plr[id]._pLevel) +  random(79, 2) + 2) << 3;
		missile[mi]._midam += missile[mi]._midam >> 1;
		//missile[mi]._midam = plr[id]._pLevel << 2;
		//for (i = missile[mi]._mispllvl; i > 0; --i) missile[mi]._midam += (missile[mi]._midam >> 3);
		//missile[mi]._midam = missile[mi]._midam << 1 + missile[mi]._midam;
	} else {
		missile[mi]._midam = random(77, monster[id].mMaxDamage - monster[id].mMinDamage + 1) + monster[id].mMinDamage;
	}
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddFlamec(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_FLAME) || !PRE_BETA
	//int k;
	//int tsx, tsy;
	
	if ((sx == dx) && (sy == dy)) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}

	GetMissileVel(mi, sx, sy, dx, dy, 32);
	if (mienemy == MI_ENEMYMONST) UseMana(id, SPL_FLAME);

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._miVar3 = 0;
	missile[mi]._mirange = 256;
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddCbolt(int mi, int sx, int sy, int dx, int dy, int midir, char micaster, int id, int dam)
{
#if (PRE_BETA && PRE_CBOLT) || !PRE_BETA
	app_assert(mi < MAXMISSILES && mi >= 0);
	if (micaster == MI_ENEMYMONST)
	{
		if (id == myplr) {
			missile[mi]._mirnd = random(63, 15) + 1;
			// INSERT send seed code
		} else {
			// INSERT get seed from network message for syncing
			// used as a number idx not a seed so no SetRndSeed necessary
			missile[mi]._mirnd = random(63, 15) + 1;
		}
		missile[mi]._midam = random(68, plr[id]._pMagic >> 2) + 1;
		//for (i = missile[mi]._mispllvl; i > 0; --i) ++missile[mi]._midam;
	}
	else
	{
		missile[mi]._mirnd = random(63, 15) + 1;
		missile[mi]._midam = 15;
	}

	if ((sx == dx) && (sy == dy)) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}

	missile[mi]._miAnimFrame = random(63, 8) + 1;
	missile[mi]._mlid = AddLight(sx, sy, 5);
	GetMissileVel(mi, sx, sy, dx, dy, 8);

	missile[mi]._miVar1 = 5;
	missile[mi]._miVar2 = midir;
	missile[mi]._miVar3 = 0;
	
	missile[mi]._mirange = 256;

	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

void AddCBArrow(int mi, int sx, int sy, int dx, int dy, int midir, char micaster, int id, int dam)
{
#if (PRE_BETA && PRE_CBOLT) || !PRE_BETA
	app_assert(mi < MAXMISSILES && mi >= 0);
	if (micaster == MI_ENEMYMONST)
	{
		if (id == myplr) {
			missile[mi]._mirnd = random(63, 15) + 1;
			// INSERT send seed code
		} else {
			// INSERT get seed from network message for syncing
			// used as a number idx not a seed so no SetRndSeed necessary
			missile[mi]._mirnd = random(63, 15) + 1;
		}
	}
	else
	{
		missile[mi]._mirnd = random(63, 15) + 1;
		missile[mi]._midam = 15;
	}

	if ((sx == dx) && (sy == dy)) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}

	missile[mi]._miAnimFrame = random(63, 8) + 1;
	missile[mi]._mlid = AddLight(sx, sy, 5);
	GetMissileVel(mi, sx, sy, dx, dy, 8);

	missile[mi]._miVar1 = 5;
	missile[mi]._miVar2 = midir;
	missile[mi]._miVar3 = 0;
	
	missile[mi]._mirange = 256;

	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddHbolt(int mi, int sx, int sy, int dx, int dy, int midir, char micaster, int id, int dam)
{
#if (PRE_BETA && PRE_HBOLT) || !PRE_BETA
	int sp;

	if ((sx == dx) && (sy == dy)) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}

	app_assert(mi < MAXMISSILES && mi >= 0);
	if (id != -1) {
		sp = 16 + (missile[mi]._mispllvl << 1);
		if (sp >= 63) sp = 63;
	} else sp = 16;

	GetMissileVel(mi, sx, sy, dx, dy, sp);
	SetMissDir(mi, GetDirection16(sx, sy, dx, dy));

	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._mlid = AddLight(sx, sy, 8);
	missile[mi]._midam = random(69, 10) + 9 + plr[id]._pLevel;

	UseMana(id, SPL_HBOLT);

	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

void AddHBArrow(int mi, int sx, int sy, int dx, int dy, int midir, char micaster, int id, int dam)
{
#if (PRE_BETA && PRE_HBOLT) || !PRE_BETA
	int sp;

	if ((sx == dx) && (sy == dy)) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}

	app_assert(mi < MAXMISSILES && mi >= 0);
	if (id != -1) {
		sp = 16 + (missile[mi]._mispllvl << 1);
		if (sp >= 63) sp = 63;
	} else sp = 16;

	GetMissileVel(mi, sx, sy, dx, dy, sp);
	SetMissDir(mi, GetDirection16(sx, sy, dx, dy));

	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._mlid = AddLight(sx, sy, 8);

	//PutMissile(mi);
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddResurrect(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_RESURRECT) || !PRE_BETA
	UseMana(id, SPL_RESURRECT);
	if (id == myplr) {
		NewCursor(RESURRECT_CURS);
	}
#endif
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._miDelFlag = TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddResurrectBeam(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_RESURRECT) || !PRE_BETA
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._mix = dx;
	missile[mi]._miy = dy;
	missile[mi]._misx = missile[mi]._mix;
	missile[mi]._misy = missile[mi]._miy;
	missile[mi]._mixvel = 0;
	missile[mi]._miyvel = 0;
	missile[mi]._mirange = misfiledata[MF_RESURRECT].mAnimLen[0];
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddTelekinesis(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_TELEKINESIS) || !PRE_BETA
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, SPL_TELEKINESIS);
	if (id == myplr) {
		NewCursor(TELE_CURS);
	}
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
/*void AddBoneSpirit(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int d, mid, mx, my;

	if ((sx == dx) && (sy == dy)) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}

	GetMissileVel(mi, sx, sy, dx, dy, 16);

	missile[mi]._mirange = 256;

	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._mlid = AddLight(sx, sy, 8);

	if (mienemy == MI_ENEMYMONST) {
		UseMana(id, SPL_BONESPIRIT);
		d = 6;

#if CHEATS
		if (simplecheat || cheatflag) d = 0;
#endif

		plr[id]._pHitPoints -= (d << HP_SHIFT);
		plr[id]._pHPBase -= (d << HP_SHIFT);
		drawhpflag = TRUE;

		if (plr[id]._pHitPoints <= 0) {
			// rjs - manashld fix? - plr[id]._pHitPoints = 0;
			StartPlrKill(id, FALSE);
		}

		if (dPlayer[dx][dy] != 0) {
			missile[mi]._miVar6 = 1;
			if (dPlayer[dx][dy] > 0) mid = dPlayer[dx][dy] - 1;
			else mid = -(dPlayer[dx][dy] + 1);
			mx = plr[mid]._px;
			my = plr[mid]._py;
		} else {
			missile[mi]._miVar6 = 0;
			if (dMonster[dx][dy] <= 0) mid = FindClosest(sx, sy, 19);
			else mid = dMonster[dx][dy] - 1;
			mx = monster[mid]._mx;
			my = monster[mid]._my;
		}

		if (mid > 0) {
			missile[mi]._miVar3 = mid;
			GetMissileVel(mi, sx, sy, mx, my, 16);
			SetMissDir(mi, GetDirection8(sx, sy, mx, my));
		}
	}

	PutMissile(mi);
}

*/
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddBoneSpirit(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
#if (PRE_BETA && PRE_BONESPIRIT) || !PRE_BETA
	int d;

	if ((sx == dx) && (sy == dy)) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}

	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._midam = 0;

	GetMissileVel(mi, sx, sy, dx, dy, 16);
	SetMissDir(mi, GetDirection8(sx, sy, dx, dy));

	missile[mi]._mirange = 256;

	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = dx;
	missile[mi]._miVar5 = dy;

	missile[mi]._mlid = AddLight(sx, sy, 8);

	if (mienemy == MI_ENEMYMONST) {
		UseMana(id, SPL_BONESPIRIT);
		d = 6;
#if CHEATS
		if (simplecheat || cheatflag) d = 0;
#endif
		plr[id]._pHitPoints -= (d << HP_SHIFT);
		plr[id]._pHPBase -= (d << HP_SHIFT);
		drawhpflag = TRUE;

		if (plr[id]._pHitPoints <= 0) {
			// rjs - manashld fix? - plr[id]._pHitPoints = 0;
			StartPlrKill(id, FALSE);
		}

	}
#else
	missile[mi]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddRportal(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._mix = sx;
	missile[mi]._miy = sy;
	missile[mi]._misx = sx;
	missile[mi]._misy = sy;

	missile[mi]._mirange = 100;
	missile[mi]._miVar1 = missile[mi]._mirange - (missile[mi]._miAnimLen);
	missile[mi]._miVar2 = 0;

	PutMissile(mi);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddDiabApoca(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	BOOL LineClear(int x1, int y1, int x2, int y2);
	int pnum;

	for (pnum = 0; pnum < gbMaxPlayers; ++pnum)
	{
		if (plr[pnum].plractive
			&& LineClear(sx,sy,plr[pnum]._pfutx, plr[pnum]._pfuty))
			AddMissile(0, 0, plr[pnum]._pfutx, plr[pnum]._pfuty, 0, MIT_FIREPLAR, mienemy, id, dam, 0);
	}
	app_assert(mi < MAXMISSILES && mi >= 0);
	missile[mi]._miDelFlag = TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int AddMissile(int sx, int sy, int v1, int v2, int midir, int mitype, char micaster, int id, int v3, int spllvl)
{
	// v1 and v2 are usually dx and dy (this can be different per missile)
	// v3 is usually damage amount

	// check if exceeded number of missiles
	if (nummissiles >= MAXMISSILES - 1)
		return -1;

	// get next missile id that is available
	int const mi = missileavail[0];
	missileavail[0] = missileavail[MAXMISSILES - nummissiles - 1];
	missileactive[nummissiles] = mi;
	++nummissiles;

	// Zero out the data first.
	memset(&missile[mi], 0, sizeof(MissileStruct));
	
	// do some standard missile setups
	missile[mi]._mitype = mitype;
	missile[mi]._micaster = micaster;
	missile[mi]._misource = id;
	missile[mi]._miAnimType = missiledata[mitype].mFileNum;
	missile[mi]._miDrawFlag = missiledata[mitype].mDraw;

	missile[mi]._mispllvl = spllvl;

	missile[mi]._mimfnum = midir;
	if (missile[mi]._miAnimType != MF_NONE
		&& misfiledata[(missile[mi]._miAnimType)].mAnimFAmt >= 8)
		SetMissDir(mi, midir);
	else
		SetMissDir(mi, 0);

	missile[mi]._mix = sx;
	missile[mi]._miy = sy;
	missile[mi]._mixoff = 0;
	missile[mi]._miyoff = 0;
	missile[mi]._misx = sx;
	missile[mi]._misy = sy;
	missile[mi]._mitxoff = 0;
	missile[mi]._mityoff = 0;

	missile[mi]._miDelFlag = FALSE;
	missile[mi]._miAnimAdd = 1;

	missile[mi]._miLightFlag = FALSE;
	missile[mi]._miPreFlag = FALSE;
	missile[mi]._miUniqTrans = FALSE;
	missile[mi]._midam = v3;
	missile[mi]._miHitFlag = FALSE;
	missile[mi]._midist = 0;
	missile[mi]._mlid = -1;
	missile[mi]._mirnd = 0;

	if (missiledata[mitype].mlSFX != -1)
		PlaySfxLoc(missiledata[mitype].mlSFX, missile[mi]._mix, missile[mi]._miy);
			
	missiledata[mitype].mAddProc(mi, sx, sy, v1, v2, midir, micaster, id, v3);

	return mi;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
/*int ChainBounceOLD (int i, int sx, int sy)
{
	int j, mi, mx, dir;

	if (dMonster[sx][sy] != 0) {

		if (dMonster[sx][sy] > 0) mi = dMonster[sx][sy] - 1;
		else mi = -(dMonster[sx][sy] + 1);
		
		if ((monster[mi]._mhitpoints >> HP_SHIFT) > 0) {
			for (j = 0; j < nummissilevars; ++j) {
				mx = missilevars[j][0];
				if ((mx >= 0) && (missile[mx]._mitype == MIT_CHAIN) && (missile[mx]._miVar8 == missile[i]._miVar8) &&
					(missilevars[j][2] >= (plr[(missile[i]._misource)]._pLevel >> 1))) {
					missilevars[j][0] = -1;
					return 3;
				}
			}
			dir = GetDirection(missile[i]._mix, missile[i]._miy, sx, sy);
			AddMissile(missile[i]._mix, missile[i]._miy, sx, sy, dir, MIT_CHAIN, MI_ENEMYMONST, missile[i]._misource, missile[i]._miVar8);
			return 1;
		}
	}

	return 0;
}*/

extern BOOL LineClear(int x1, int y1, int x2, int y2);
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
int Sentfire(int i, int sx, int sy)
{
	int ex, dir;

	ex = 0;
	app_assert(i < MAXMISSILES && i >= 0);
	if (LineClear(missile[i]._mix, missile[i]._miy, sx, sy)
		&& (dMonster[sx][sy] > 0)
		&& ((monster[(dMonster[sx][sy] - 1)]._mhitpoints >> HP_SHIFT) > 0)
		&& ((dMonster[sx][sy]-1) > 3)) {
		dir = GetDirection(missile[i]._mix, missile[i]._miy, sx, sy);
		missile[i]._miVar3 = missileavail[0]; // get next mid
		AddMissile(missile[i]._mix, missile[i]._miy, sx, sy, dir, MIT_FIREBOLT, MI_ENEMYMONST, missile[i]._misource, missile[i]._midam, GetSpellLevel(missile[i]._misource, SPL_FIREBOLT));
		ex = -1;
	}

	if (ex == -1) {
		SetMissDir(i, 2);
		missile[i]._miVar2 = 3;
	}

	return (ex);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Dummy(int i)
{}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_HorkSpawn(int i)
{
	app_assert(i < MAXMISSILES && i >= 0);
	
	
	--missile[i]._mirange;
	CheckMissileCol(i, 0, 0, 0, missile[i]._mix, missile[i]._miy, 0);
	
	if (missile[i]._mirange <= 0)
	{
		missile[i]._miDelFlag = TRUE;
		for (int k = 0; k<2; ++k)
		{
			int const l = CrawlNum[k];
			int j = l + 1;
			for (int m = CrawlTable[l]; m > 0; --m, j += 2) {
				int const tx = missile[i]._mix + CrawlTable[j];
				int const ty = missile[i]._miy + CrawlTable[(j + 1)];
				if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
					int const pn = dPiece[tx][ty];
					if(nSolidTable[pn] == 0
						&& dMonster[tx][ty] == 0
						&& dPlayer[tx][ty] == 0
						&& dObject[tx][ty] == 0) {
						m = -99;
						k = 6;
						int const hs = AddMonster(tx, ty, missile[i]._miVar1, 1, TRUE);
						M_StartStand(hs, missile[i]._miVar1);
						break;
					}
				}
			}
		}
	}
	else {
		++missile[i]._midist;
		missile[i]._mitxoff += missile[i]._mixvel;
		missile[i]._mityoff += missile[i]._miyvel;
		GetMissilePos(i);
	}
	PutMissile(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Rune(int i)
{
	app_assert(i < MAXMISSILES && i >= 0);
	int const tx = missile[i]._mix;
	int const ty = missile[i]._miy;
	
	int mid = dMonster[tx][ty];
	int pid = dPlayer[tx][ty];
	
	if (mid || pid)
	{
		
		int dir;

		if (mid != 0)
		{
			if (mid > 0) --mid;
			else mid = -(mid + 1);
			
			dir = GetDirection(missile[i]._mix, missile[i]._miy, 
					monster[mid]._mx, monster[mid]._my);
		}
		else 
		{
			if (pid > 0) --pid;
			else pid = -(pid + 1);
			
			dir = GetDirection(missile[i]._mix, missile[i]._miy, 
					plr[pid]._px, plr[pid]._py);
		}
		
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
		
		AddMissile(tx, ty, tx, ty, dir, missile[i]._miVar1, MI_ENEMYBOTH, missile[i]._misource, missile[i]._midam, missile[i]._mispllvl);
	}
	PutMissile(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Golem(int i)
{
#if (PRE_BETA && PRE_GOLEM) || !PRE_BETA
	int id, pn;
	int j, k, l, m;
	int tx, ty;

	app_assert(i < MAXMISSILES && i >= 0);
	id = missile[i]._misource;

	if (monster[id]._mx == 1 && monster[id]._my == 0) {
		for (k = 0; k < 6; ++k) {
			l = CrawlNum[k];
			j = l + 1;
			for (m = CrawlTable[l]; m > 0; --m) {
				tx = missile[i]._miVar4 + CrawlTable[j];
				ty = missile[i]._miVar5 + CrawlTable[(j + 1)];
				if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
					pn = dPiece[tx][ty];
					if ((LineClear(missile[i]._miVar1, missile[i]._miVar2, tx, ty)) &&
						((nSolidTable[pn] | dMonster[tx][ty] | dObject[tx][ty]) == 0)) {
						k = 6;
						SpawnGolum(id, tx, ty, i);
						break;
					}
				}
				j += 2;
			}
		}
	}
	missile[i]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_SetManashield(int i)
{
#if (PRE_BETA && PRE_MANASHLD) || !PRE_BETA
	ManashieldFlag = 1;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_LArrow(int i)
{
	int p, mind, maxd, rst;

	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	p = missile[i]._misource;
	if (missile[i]._miAnimType != MF_CBOLT && missile[i]._miAnimType != MF_EXP1) {
		++missile[i]._midist;
		missile[i]._mitxoff += missile[i]._mixvel;
		missile[i]._mityoff += missile[i]._miyvel;
		GetMissilePos(i);
		if (p != -1) {
			if (missile[i]._micaster == MI_ENEMYMONST) {
				mind = plr[p]._pIMinDam;
				maxd = plr[p]._pIMaxDam;
			} else {
				mind = monster[p].mMinDamage;
				maxd = monster[p].mMaxDamage;
			}
		} else {
			mind = currlevel + random(68, 10) + 1;
			maxd = (currlevel * 2) + random(68, 10) + 1;
		}
		if ((missile[i]._mix != missile[i]._misx) || (missile[i]._miy != missile[i]._misy)) {
			rst = missiledata[(missile[i]._mitype)].mResist;
			missiledata[(missile[i]._mitype)].mResist = MIMT_NONE;
			CheckMissileCol(i, mind, maxd, 0, missile[i]._mix, missile[i]._miy, 0);
			missiledata[(missile[i]._mitype)].mResist = rst;
		}

		if (missile[i]._mirange == 0) {
			missile[i]._mimfnum = 0;
			missile[i]._mitxoff -= missile[i]._mixvel;
			missile[i]._mityoff -= missile[i]._miyvel;
			GetMissilePos(i);
			if (missile[i]._mitype == MIT_LARROW) {
				SetMissAnim(i, MF_CBOLT);
				missile[i]._mirange = missile[i]._miAnimLen - 1;
			} else {
				SetMissAnim(i, MF_EXP1);
				missile[i]._mirange = missile[i]._miAnimLen - 1;
			}
		} else {
			if ((missile[i]._mix != missile[i]._miVar1) || (missile[i]._miy != missile[i]._miVar2)) {
				missile[i]._miVar1 = missile[i]._mix;
				missile[i]._miVar2 = missile[i]._miy;
				ChangeLight(missile[i]._mlid, missile[i]._miVar1, missile[i]._miVar2, 5);
			}
		}
	} else {
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, 5 + missile[i]._miAnimFrame);
		rst = missiledata[(missile[i]._mitype)].mResist;
		// drb.patch1.start.02/25/97
		if (missile[i]._mitype == MIT_LARROW) {
			if (p != -1) {
				mind = plr[p]._pILMinDam;
				maxd = plr[p]._pILMaxDam;
			} else {
				mind = currlevel + random(68, 10) + 1;
				maxd = (currlevel * 2) + random(68, 10) + 1;
			}
			missiledata[MIT_LARROW].mResist = MIMT_LGHT;
			CheckMissileCol(i, mind, maxd, 0, missile[i]._mix, missile[i]._miy, 1);
		}
		if (missile[i]._mitype == MIT_FARROW) {
			if (p != -1) {
				mind = plr[p]._pIFMinDam;
				maxd = plr[p]._pIFMaxDam;
			} else {
				mind = currlevel + random(68, 10) + 1;
				maxd = (currlevel * 2) + random(68, 10) + 1;
			}
			missiledata[MIT_FARROW].mResist = MIMT_FIRE;
			CheckMissileCol(i, mind, maxd, 0, missile[i]._mix, missile[i]._miy, 1);
		}
		// endpatch1.2/25/97
		missiledata[(missile[i]._mitype)].mResist = rst;
	}

	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	
	PutMissile(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Arrow(int i)
{
	int p, mind, maxd;

	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	++missile[i]._midist;
	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);
	p = missile[i]._misource;
	if (p != -1) {
		if (missile[i]._micaster == MI_ENEMYMONST) {
			mind = plr[p]._pIMinDam;
			maxd = plr[p]._pIMaxDam;
		} else {
			mind = monster[p].mMinDamage;
			maxd = monster[p].mMaxDamage;
		}
	} else {
		mind = currlevel;
		maxd = currlevel * 2;
	}
	if ((missile[i]._mix != missile[i]._misx) || (missile[i]._miy != missile[i]._misy))
		CheckMissileCol(i, mind, maxd, 0, missile[i]._mix, missile[i]._miy, 0);
	if (missile[i]._mirange == 0) missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Firebolt(int i)
{
#if (PRE_BETA && PRE_FIREBOLT) || !PRE_BETA
	int omx, omy, d, p;

	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;

	if (missile[i]._mitype == MIT_BONESPIRIT && missile[i]._mimfnum == 8) {
		if (missile[i]._mirange == 0) {
			if(missile[i]._mlid >= 0) AddUnLight(missile[i]._mlid);
			missile[i]._miDelFlag = TRUE;
			PlaySfxLoc(LS_BSIMPCT, missile[i]._mix, missile[i]._miy);
		}
		PutMissile(i);
		return;
	}

	omx = missile[i]._mitxoff;
	omy = missile[i]._mityoff;

	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);

	p = missile[i]._misource;
	if (p != -1) {
		if (missile[i]._micaster == MI_ENEMYMONST) {
			switch(missile[i]._mitype) {
				case MIT_FLARE:
					d = ((plr[p]._pMagic>>1)-(plr[p]._pMagic>>3)) + (missile[i]._mispllvl << 1) + missile[i]._mispllvl;
					break;
				case MIT_FIREBOLT:
					d = random(75, 10) + 1 + (plr[p]._pMagic >> 3) + missile[i]._mispllvl;
					break;
				case MIT_BONESPIRIT:
					//d = (monster[missile[i]._miVar3]._mhitpoints >> HP_SHIFT) >> 1;
					d = 0;
					break;
			}
		} else d = (random(77, monster[p].mMaxDamage - monster[p].mMinDamage + 1) + monster[p].mMinDamage);
	} else d = random(78, currlevel << 1) + currlevel;

	if ((missile[i]._mix != missile[i]._misx) || (missile[i]._miy != missile[i]._misy))
		CheckMissileCol(i, d, d, 0, missile[i]._mix, missile[i]._miy, 0);

	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		missile[i]._mitxoff = omx;
		missile[i]._mityoff = omy;
		GetMissilePos(i);
		switch(missile[i]._mitype) {
			case MIT_FLARE:
				AddMissile(missile[i]._mix, missile[i]._miy, i, 0, missile[i]._mimfnum, MIT_FLAREXP, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIT_FIREBOLT:
			case MIT_MAGMABALL:
				AddMissile(missile[i]._mix, missile[i]._miy, i, 0, missile[i]._mimfnum, MIT_MISEXP, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIT_YELLOWFLARE:
				AddMissile(missile[i]._mix, missile[i]._miy, i, 0, missile[i]._mimfnum, MIT_YELLOWEXPLOSION, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIT_REDFLARE:
				AddMissile(missile[i]._mix, missile[i]._miy, i, 0, missile[i]._mimfnum, MIT_REDEXPLOSION, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIT_BLUEFLARE:
				AddMissile(missile[i]._mix, missile[i]._miy, i, 0, missile[i]._mimfnum, MIT_BLUEEXPLOSION, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIT_BLUE2FLARE:
				AddMissile(missile[i]._mix, missile[i]._miy, i, 0, missile[i]._mimfnum, MIT_BLUE2EXPLOSION, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIT_ORANGEFLARE:
				AddMissile(missile[i]._mix, missile[i]._miy, i, 0, missile[i]._mimfnum, MIT_ORANGEEXPLOSION, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIT_ACID:
				AddMissile(missile[i]._mix, missile[i]._miy, i, 0, missile[i]._mimfnum, MIT_ACIDSPLAT, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIT_BONESPIRIT:
				SetMissDir(i, 8);
				missile[i]._mirange = 7;
				missile[i]._miDelFlag = FALSE;
				PutMissile(i);
				return;
		}
		if(missile[i]._mlid >= 0) AddUnLight(missile[i]._mlid);
	} else {
		if ((missile[i]._mix != missile[i]._miVar1) || (missile[i]._miy != missile[i]._miVar2)) {
			missile[i]._miVar1 = missile[i]._mix;
			missile[i]._miVar2 = missile[i]._miy;
			if(missile[i]._mlid >= 0) ChangeLight(missile[i]._mlid, missile[i]._miVar1, missile[i]._miVar2, 8);
		}
	}

	PutMissile(i);
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Lightball(int i)
{
	int j, tx, ty, oi;

	app_assert(i < MAXMISSILES && i >= 0);
	tx = missile[i]._miVar1;
	ty = missile[i]._miVar2;

	--missile[i]._mirange;
	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	
	GetMissilePos(i);
	j = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 0, missile[i]._mix, missile[i]._miy, 0);
	if (missile[i]._miHitFlag == TRUE) missile[i]._mirange = j;

	// fix for shires and fireing nova - rjs
	if (dObject[tx][ty] != 0 && tx == missile[i]._mix && ty == missile[i]._miy) {
		if (dObject[tx][ty] > 0) oi = dObject[tx][ty] - 1;
		else oi = -(dObject[tx][ty] + 1);
		if (object[oi]._otype == OBJ_SHRINEL || object[oi]._otype == OBJ_SHRINER) missile[i]._mirange = j;
	}

	if (missile[i]._mirange == 0) missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Lightwall(int i)
{
	app_assert(i < MAXMISSILES && i >= 0);

	--missile[i]._mirange;
	
	int const j = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy, 0);
	if (missile[i]._miHitFlag == TRUE) missile[i]._mirange = j;

	if (missile[i]._mirange == 0) missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Krull(int i)
{
	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	
	GetMissilePos(i);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 0, missile[i]._mix, missile[i]._miy, 0);

	if (missile[i]._mirange == 0) missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Acidpud(int i)
{
	int range;

	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	
//	GetMissilePos(i);
	range = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy, 0);
	missile[i]._mirange = range;	// CheckMissileCol clears range if there's a collision,
									// but this missile sticks around even after a collision

//	MoveMissilePos(i);

	if (missile[i]._mirange == 0)
	{
		if(missile[i]._mimfnum)
			missile[i]._miDelFlag = TRUE;
		else
		{
			SetMissDir(i, 1);
			missile[i]._mirange = missile[i]._miAnimLen;
		}
	}

	PutMissile(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Firewall(int i)
{
#if (PRE_BETA && PRE_WALL) || !PRE_BETA
	int ExpLight[14] = {2,3,4,5,5,6,7,8,9,10,11,12,12};

	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	
	if (missile[i]._mirange == missile[i]._miVar1) {
		SetMissDir(i, 1);
		missile[i]._miAnimFrame =  random(83, 11) + 1;
	}

	if (missile[i]._mirange == missile[i]._miAnimLen - 1) {
		SetMissDir(i, 0);
		missile[i]._miAnimFrame = 13;
		missile[i]._miAnimAdd = -1;
	}

	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy, 1);

	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}

	if ((missile[i]._mimfnum != 0) && (missile[i]._mirange != 0) && 
		(missile[i]._miAnimAdd != -1) && (missile[i]._miVar2 < 12)) {
		
		if (missile[i]._miVar2 == 0) 
			missile[i]._mlid = AddLight(missile[i]._mix, missile[i]._miy, ExpLight[(missile[i]._miVar2)]);
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, ExpLight[(missile[i]._miVar2)]);
		++missile[i]._miVar2;
	}

	PutMissile(i);
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Fireball(int i)
{
#if (PRE_BETA && PRE_FIREBALL) || !PRE_BETA
	int dam, px, py, id, mx, my;

	app_assert(i < MAXMISSILES && i >= 0);
	id = missile[i]._misource;
	dam = missile[i]._midam;

	//dam = missile[i]._mispllvl; // rjs test
	//missiledata[(missile[i]._mitype)].mResist = MIMT_NONE;
	
	--missile[i]._mirange;

	if (missile[i]._micaster == MI_ENEMYMONST) {
		px = plr[id]._px;
		py = plr[id]._py;
	} else {
		px = monster[id]._mx;
		py = monster[id]._my;
	}

	if (missile[i]._miAnimType == MF_BIGEXP) {
		if (missile[i]._mirange == 0) {
			missile[i]._miDelFlag = TRUE;
			AddUnLight(missile[i]._mlid);
		}

		PutMissile(i);
		return;
	}

	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);

	if ((missile[i]._mix != missile[i]._misx) || (missile[i]._miy != missile[i]._misy))
		CheckMissileCol(i, dam, dam, 0, missile[i]._mix, missile[i]._miy, 0);

	if (missile[i]._mirange == 0) {
		mx = missile[i]._mix;
		my = missile[i]._miy;

		ChangeLight(missile[i]._mlid, mx, my, missile[i]._miAnimFrame);

		if (CheckBlock(px, py, mx    , my    ) == 0) CheckMissileCol(i, dam, dam, 0, mx,     my,     1);
		if (CheckBlock(px, py, mx    , my + 1) == 0) CheckMissileCol(i, dam, dam, 0, mx,     my + 1, 1);
		if (CheckBlock(px, py, mx    , my - 1) == 0) CheckMissileCol(i, dam, dam, 0, mx,     my - 1, 1);
		if (CheckBlock(px, py, mx + 1, my    ) == 0) CheckMissileCol(i, dam, dam, 0, mx + 1, my,     1);
		if (CheckBlock(px, py, mx + 1, my - 1) == 0) CheckMissileCol(i, dam, dam, 0, mx + 1, my - 1, 1);
		if (CheckBlock(px, py, mx + 1, my + 1) == 0) CheckMissileCol(i, dam, dam, 0, mx + 1, my + 1, 1);
		if (CheckBlock(px, py, mx - 1, my    ) == 0) CheckMissileCol(i, dam, dam, 0, mx - 1, my,     1);
		if (CheckBlock(px, py, mx - 1, my + 1) == 0) CheckMissileCol(i, dam, dam, 0, mx - 1, my + 1, 1);
		if (CheckBlock(px, py, mx - 1, my - 1) == 0) CheckMissileCol(i, dam, dam, 0, mx - 1, my - 1, 1);

		//++missile[i]._mix;
		//++missile[i]._miy;
		//missile[i]._miyoff -= 32;

		if ((!TransList[dTransVal[mx][my]]) ||
			((missile[i]._mixvel < 0) &&
				((TransList[dTransVal[mx][my+1]] && nSolidTable[dPiece[mx][my+1]]) ||
				(TransList[dTransVal[mx][my-1]] && nSolidTable[dPiece[mx][my-1]])))) {
			++missile[i]._mix;
			++missile[i]._miy;
			missile[i]._miyoff -= 32;
		}
		
		if (((missile[i]._miyvel > 0) &&
				((TransList[dTransVal[mx+1][my]] && nSolidTable[dPiece[mx+1][my]]) ||
				(TransList[dTransVal[mx-1][my]] && nSolidTable[dPiece[mx-1][my]])))) {
			missile[i]._miyoff -= 32;
		}

		if (((missile[i]._mixvel > 0) &&
				((TransList[dTransVal[mx][my+1]] && nSolidTable[dPiece[mx][my+1]]) ||
				(TransList[dTransVal[mx][my-1]] && nSolidTable[dPiece[mx][my-1]])))) {
			missile[i]._mixoff -= 32;
		}
		
		missile[i]._mimfnum = 0;
		SetMissAnim(i, MF_BIGEXP);
		missile[i]._mirange = missile[i]._miAnimLen - 1;

	} else {
		if ((missile[i]._mix != missile[i]._miVar1) || (missile[i]._miy != missile[i]._miVar2)) {
			missile[i]._miVar1 = missile[i]._mix;
			missile[i]._miVar2 = missile[i]._miy;
			ChangeLight(missile[i]._mlid, missile[i]._miVar1, missile[i]._miVar2, 8);
		}
	}

	PutMissile(i);
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_BigExplosion(int i)
{
#if (PRE_BETA && PRE_FIREBALL) || !PRE_BETA

	app_assert(i < MAXMISSILES && i >= 0);

	--missile[i]._mirange;
	
	if (missile[i]._mirange <= 0) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}

	PutMissile(i);
#endif
}


/*-----------------------------------------------------------------------*
	// Doesn't work yet. GWP
**-----------------------------------------------------------------------*/
void MI_SpiralFireBall(int i)
{
#if (PRE_BETA && PRE_FIREBALL) || !PRE_BETA
	int px, py, mx, my;

	app_assert(i < MAXMISSILES && i >= 0);
	int const id = missile[i]._misource;
	int const dam = missile[i]._midam;

	
	int xcurve;
	int ycurve;
	
	// Set the direction into a spiral.
	if (missile[i]._miVar7 < 0) {
		missile[i]._miVar6 *= 2;
		missile[i]._miVar7 = missile[i]._miVar6;
		
		--missile[i]._mimfnum;
		if (missile[i]._mimfnum < 0)
			missile[i]._mimfnum = 7;
	}
	else --missile[i]._miVar7;
	
	switch (missile[i]._mimfnum) {
	case 0:	// Down
		xcurve = missile[i]._mixvel;
		ycurve = 0;
		break;
	case 1: // Down left
		xcurve = missile[i]._mixvel;
		ycurve = missile[i]._miyvel;
		break;
	case 2: // Left
		xcurve = 0;
		ycurve = missile[i]._miyvel;
		break;
	case 3: // Up Left
		xcurve = missile[i]._mixvel;
		ycurve = missile[i]._miyvel;
		break;
	case 4: // Up
		xcurve = missile[i]._mixvel;
		ycurve = 0;
		break;
	case 5: // Up right
		xcurve = missile[i]._mixvel;
		ycurve = missile[i]._miyvel;
		break;
	case 6: // Right
		xcurve = 0;
		ycurve = missile[i]._miyvel;
		break;
	case 7: // Down Right
		xcurve = missile[i]._mixvel;
		ycurve = missile[i]._miyvel;
		break;
	}
		
	
	--missile[i]._mirange;

	if (missile[i]._micaster == MI_ENEMYMONST) {
		px = plr[id]._px;
		py = plr[id]._py;
	} else {
		px = monster[id]._mx;
		py = monster[id]._my;
	}

	if (missile[i]._miAnimType == MF_BIGEXP) {
		if (missile[i]._mirange == 0) {
			missile[i]._miDelFlag = TRUE;
			AddUnLight(missile[i]._mlid);
		}

		PutMissile(i);
		return;
	}

	missile[i]._mitxoff += xcurve;
	missile[i]._mityoff += ycurve;
	GetMissilePos(i);

	if ((missile[i]._mix != missile[i]._misx) || (missile[i]._miy != missile[i]._misy))
		CheckMissileCol(i, dam, dam, 0, missile[i]._mix, missile[i]._miy, 0);

	if (missile[i]._mirange == 0) {
		mx = missile[i]._mix;
		my = missile[i]._miy;

		ChangeLight(missile[i]._mlid, mx, my, missile[i]._miAnimFrame);

		if (CheckBlock(px, py, mx    , my    ) == 0) CheckMissileCol(i, dam, dam, 0, mx,     my,     1);
		if (CheckBlock(px, py, mx    , my + 1) == 0) CheckMissileCol(i, dam, dam, 0, mx,     my + 1, 1);
		if (CheckBlock(px, py, mx    , my - 1) == 0) CheckMissileCol(i, dam, dam, 0, mx,     my - 1, 1);
		if (CheckBlock(px, py, mx + 1, my    ) == 0) CheckMissileCol(i, dam, dam, 0, mx + 1, my,     1);
		if (CheckBlock(px, py, mx + 1, my - 1) == 0) CheckMissileCol(i, dam, dam, 0, mx + 1, my - 1, 1);
		if (CheckBlock(px, py, mx + 1, my + 1) == 0) CheckMissileCol(i, dam, dam, 0, mx + 1, my + 1, 1);
		if (CheckBlock(px, py, mx - 1, my    ) == 0) CheckMissileCol(i, dam, dam, 0, mx - 1, my,     1);
		if (CheckBlock(px, py, mx - 1, my + 1) == 0) CheckMissileCol(i, dam, dam, 0, mx - 1, my + 1, 1);
		if (CheckBlock(px, py, mx - 1, my - 1) == 0) CheckMissileCol(i, dam, dam, 0, mx - 1, my - 1, 1);

		//++missile[i]._mix;
		//++missile[i]._miy;
		//missile[i]._miyoff -= 32;

		if ((!TransList[dTransVal[mx][my]]) ||
			((missile[i]._mixvel < 0) &&
				((TransList[dTransVal[mx][my+1]] && nSolidTable[dPiece[mx][my+1]]) ||
				(TransList[dTransVal[mx][my-1]] && nSolidTable[dPiece[mx][my-1]])))) {
			++missile[i]._mix;
			++missile[i]._miy;
			missile[i]._miyoff -= 32;
		}
		
		if (((missile[i]._miyvel > 0) &&
				((TransList[dTransVal[mx+1][my]] && nSolidTable[dPiece[mx+1][my]]) ||
				(TransList[dTransVal[mx-1][my]] && nSolidTable[dPiece[mx-1][my]])))) {
			missile[i]._miyoff -= 32;
		}

		if (((missile[i]._mixvel > 0) &&
				((TransList[dTransVal[mx][my+1]] && nSolidTable[dPiece[mx][my+1]]) ||
				(TransList[dTransVal[mx][my-1]] && nSolidTable[dPiece[mx][my-1]])))) {
			missile[i]._mixoff -= 32;
		}
		
		missile[i]._mimfnum = 0;
		SetMissAnim(i, MF_BIGEXP);
		missile[i]._mirange = missile[i]._miAnimLen - 1;

	} else {
		if ((missile[i]._mix != missile[i]._miVar1) || (missile[i]._miy != missile[i]._miVar2)) {
			missile[i]._miVar1 = missile[i]._mix;
			missile[i]._miVar2 = missile[i]._miy;
			ChangeLight(missile[i]._mlid, missile[i]._miVar1, missile[i]._miVar2, 8);
		}
	}

	missile[i]._miDelFlag = TRUE;
	PutMissile(i);
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Lightctrl(int i)
{
#if (PRE_BETA && PRE_LIGHTNING) || !PRE_BETA
	int pn, dam, p, mx, my;
	
	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	dam = 0;

	p = missile[i]._misource;
	if (p != -1) {
		if (missile[i]._micaster == MI_ENEMYMONST) {
			dam = random(79, plr[p]._pLevel) +  random(79, 2) + 2;
			dam = dam << HP_SHIFT;
		} else {
			dam = (random(80, monster[p].mMaxDamage - monster[p].mMinDamage + 1) + monster[p].mMinDamage) << 1;
		}
	} else {
		dam = random(81, currlevel) + (currlevel << 1);
	}

	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);
	mx = missile[i]._mix;
	my = missile[i]._miy;

	app_assert(mx < MAXDUNX && mx >= 0);
	app_assert(my < MAXDUNY && my >= 0);
	pn = dPiece[mx][my];
	app_assert(pn <= MAXTILES && pn >= 0); 
	if (missile[i]._misource == -1) {
		if ((mx != missile[i]._misx) || (my != missile[i]._misy))
			if (nMissileTable[pn]) missile[i]._mirange = 0;
	} else {
		if (nMissileTable[pn]) missile[i]._mirange = 0;
	}

	if ((!nMissileTable[pn]) && ((mx != missile[i]._miVar1) || (my != missile[i]._miVar2)) &&
		(mx > 0) && (my > 0) && (mx < DMAXX) && (my < DMAXY)) { 
		// Add LIGHTNING if the player or trap threw this, otherwise add THINLIGHT for thin demon
		if (missile[i]._misource != -1) {
			if (missile[i]._micaster == MI_ENEMYPLR && EquivMonst(monster[missile[i]._misource].MType->mtype, MT_STORM))
				AddMissile(missile[i]._mix, missile[i]._miy, missile[i]._misx, missile[i]._misy, i, MIT_THINLIGHT, missile[i]._micaster, missile[i]._misource, dam, missile[i]._mispllvl);
			else
				AddMissile(missile[i]._mix, missile[i]._miy, missile[i]._misx, missile[i]._misy, i, MIT_LIGHTNING, missile[i]._micaster, missile[i]._misource, dam, missile[i]._mispllvl);
		} else {
			AddMissile(missile[i]._mix, missile[i]._miy, missile[i]._misx, missile[i]._misy, i, MIT_LIGHTNING, missile[i]._micaster, missile[i]._misource, dam, missile[i]._mispllvl);
		}
		missile[i]._miVar1 = missile[i]._mix;
		missile[i]._miVar2 = missile[i]._miy;
	}

	if ((missile[i]._mirange == 0) || (mx <= 0) || (my <= 0) || (mx >= DMAXX) || (my > DMAXY))
		missile[i]._miDelFlag = TRUE;
#endif
}

void MI_LTArrow(int i)
{
#if (PRE_BETA && PRE_LIGHTNING) || !PRE_BETA
	int pn, mx, my;
	
	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;

	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);
	mx = missile[i]._mix;
	my = missile[i]._miy;

	app_assert(mx < MAXDUNX && mx >= 0);
	app_assert(my < MAXDUNY && my >= 0);
	pn = dPiece[mx][my];
	app_assert(pn <= MAXTILES && pn >= 0); 
	if (missile[i]._misource == -1) {
		if ((mx != missile[i]._misx) || (my != missile[i]._misy))
			if (nMissileTable[pn]) missile[i]._mirange = 0;
	} else {
		if (nMissileTable[pn]) missile[i]._mirange = 0;
	}

	if ((!nMissileTable[pn]) && ((mx != missile[i]._miVar1) || (my != missile[i]._miVar2)) &&
		(mx > 0) && (my > 0) && (mx < DMAXX) && (my < DMAXY)) { 
		// Add LIGHTNING if the player or trap threw this, otherwise add THINLIGHT for thin demon
		if (missile[i]._misource != -1) {
			if (missile[i]._micaster == MI_ENEMYPLR && EquivMonst(monster[missile[i]._misource].MType->mtype, MT_STORM))
				AddMissile(missile[i]._mix, missile[i]._miy, missile[i]._misx, missile[i]._misy, i, MIT_THINLIGHT, missile[i]._micaster, missile[i]._misource, missile[i]._midam, missile[i]._mispllvl);
			else
				AddMissile(missile[i]._mix, missile[i]._miy, missile[i]._misx, missile[i]._misy, i, MIT_LIGHTNING, missile[i]._micaster, missile[i]._misource, missile[i]._midam, missile[i]._mispllvl);
		} else {
			AddMissile(missile[i]._mix, missile[i]._miy, missile[i]._misx, missile[i]._misy, i, MIT_LIGHTNING, missile[i]._micaster, missile[i]._misource, missile[i]._midam, missile[i]._mispllvl);
		}
		missile[i]._miVar1 = missile[i]._mix;
		missile[i]._miVar2 = missile[i]._miy;
	}

	if ((missile[i]._mirange == 0) || (mx <= 0) || (my <= 0) || (mx >= DMAXX) || (my > DMAXY))
		missile[i]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Lightning(int i)
{
#if (PRE_BETA && PRE_LIGHTNING) || !PRE_BETA
	int j;

	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	
	j = missile[i]._mirange;
	if ((missile[i]._mix != missile[i]._misx) || (missile[i]._miy != missile[i]._misy))
		CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy, 0);
	if (missile[i]._miHitFlag == TRUE) missile[i]._mirange = j;

	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Town(int i)
{
#if (PRE_BETA && PRE_TOWN) || !PRE_BETA
	int p;

	int ExpLight[17] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,15,15};

	app_assert(i < MAXMISSILES && i >= 0);
	if (missile[i]._mirange > 1) --missile[i]._mirange;
	
	if (missile[i]._mirange == missile[i]._miVar1) {
		SetMissDir(i, 1);
	}

	if ((currlevel != 0) && (missile[i]._mimfnum != 1) && (missile[i]._mirange != 0)) {
		if (missile[i]._miVar2 == 0) 
			missile[i]._mlid = AddLight(missile[i]._mix, missile[i]._miy, ExpLight[(missile[i]._miVar2)]);
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, ExpLight[(missile[i]._miVar2)]);
		++missile[i]._miVar2;
	}

	for (p = 0; p < MAX_PLRS; ++p) {
		if (! plr[p].plractive) continue;
		if (currlevel != plr[p].plrlevel) continue;
		if (plr[p]._pLvlChanging) continue;
		if (plr[p]._pmode != PM_STAND) continue;
		if (plr[p]._px != missile[i]._mix) continue;
		if (plr[p]._py != missile[i]._miy) continue;

		ClrPlrPath(p);
		if (p == myplr) {
			NetSendCmdParam1(TRUE,CMD_WARP,missile[i]._misource);
			plr[p]._pmode = PM_NEWLVL;
		}
	}

	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Flash(int i)
{
#if (PRE_BETA && PRE_FLASH) || !PRE_BETA
	app_assert(i < MAXMISSILES && i >= 0);
	if ((missile[i]._micaster == MI_ENEMYMONST) && (missile[i]._misource != -1))
		plr[missile[i]._misource]._pInvincible = TRUE;
	--missile[i]._mirange;
	
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix-1, missile[i]._miy, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix+1, missile[i]._miy, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix-1, missile[i]._miy+1, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy+1, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix+1, missile[i]._miy+1, 1);
	
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		if ((missile[i]._micaster == MI_ENEMYMONST) && (missile[i]._misource != -1))
			plr[(missile[i]._misource)]._pInvincible = FALSE;
	}
	PutMissile(i);
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Aura(int i)
{
#if (PRE_BETA && PRE_FLASH) || !PRE_BETA
	app_assert(i < MAXMISSILES && i >= 0);
	int const id = missile[i]._misource;
	
	if ((missile[i]._micaster == MI_ENEMYMONST) && (id != -1)) {
		missile[i]._mix = plr[id]._px;
		missile[i]._miy = plr[id]._py;
		missile[i]._mitxoff = plr[id]._pxoff << 16;
		missile[i]._mityoff = plr[id]._pyoff << 16;
	}
	--missile[i]._mirange;
	
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		if ((missile[i]._micaster == MI_ENEMYMONST) && (missile[i]._misource != -1))
			plr[(missile[i]._misource)]._pBaseToBlk -= 50;
	}
	PutMissile(i);
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Aura2(int i)
{
#if (PRE_BETA && PRE_FLASH) || !PRE_BETA
	app_assert(i < MAXMISSILES && i >= 0);
	if ((missile[i]._micaster == MI_ENEMYMONST) && (missile[i]._misource != -1)) {
		missile[i]._mix = plr[missile[i]._misource]._pfutx;
		missile[i]._miy = plr[missile[i]._misource]._pfuty;
	}
	--missile[i]._mirange;
	
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
	}
	PutMissile(i);
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Flash2(int i)
{
#if (PRE_BETA && PRE_FLASH) || !PRE_BETA
	app_assert(i < MAXMISSILES && i >= 0);
	if ((missile[i]._micaster == MI_ENEMYMONST) && (missile[i]._misource != -1))
		plr[(missile[i]._misource)]._pInvincible = TRUE;
	--missile[i]._mirange;
	
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix-1, missile[i]._miy-1, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy-1, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix+1, missile[i]._miy-1, 1);
	
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		if ((missile[i]._micaster == MI_ENEMYMONST) && (missile[i]._misource != -1))
			plr[(missile[i]._misource)]._pInvincible = FALSE;
	}
	PutMissile(i);
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Reflect(int i)
{
	app_assert(i < MAXMISSILES && i >= 0);
	int const id = missile[i]._misource;

	// Reset by GetMissilePos()
	// missile[i]._mix = plr[id]._px;
	// missile[i]._miy = plr[id]._py;
	missile[i]._mitxoff = plr[id]._pxoff << 16;
	missile[i]._mityoff = plr[id]._pyoff << 16;

	// These hard coded numbers are fudge factors to get the %$^& art to
	// align up.
	if (plr[id]._pmode == PM_WALK3) {
		missile[i]._misx = plr[id]._pfutx + 2;
		missile[i]._misy = plr[id]._pfuty + -1;
	} else {
		missile[i]._misx = plr[id]._px + 2;
		missile[i]._misy = plr[id]._py + -1;
	}
	
	GetMissilePos(i);

	if (plr[id]._pmode == PM_WALK3) {
		if (plr[id]._pdir == DIR_L) ++missile[i]._mix;
		else ++missile[i]._miy;
	}

	if (id != myplr &&
		currlevel != plr[id].plrlevel) {
		missile[i]._miDelFlag = TRUE;
	}
	
	if (plr[id]._pReflectCount <= 0)
	{
		missile[i]._miDelFlag = TRUE;
		NetSendCmd(TRUE, CMD_ENDREFLECT);
	}

	PutMissile(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Manashield(int i)
{
#if (PRE_BETA && PRE_MANASHLD) || !PRE_BETA
	int j, id;
	long diff, pct;

	//--missile[i]._mirange;
	app_assert(i < MAXMISSILES && i >= 0);
	id = missile[i]._misource;

	missile[i]._mix = plr[id]._px;
	missile[i]._miy = plr[id]._py;
	missile[i]._mitxoff = plr[id]._pxoff << 16;
	missile[i]._mityoff = plr[id]._pyoff << 16;

	if (plr[id]._pmode == PM_WALK3) {
		missile[i]._misx = plr[id]._pfutx;
		missile[i]._misy = plr[id]._pfuty;
	} else {
		missile[i]._misx = plr[id]._px;
		missile[i]._misy = plr[id]._py;
	}
	
	GetMissilePos(i);

	if (plr[id]._pmode == PM_WALK3) {
		if (plr[id]._pdir == DIR_L) ++missile[i]._mix;
		else ++missile[i]._miy;
	}

	if (id != myplr) {
		if (currlevel != plr[id].plrlevel) missile[i]._miDelFlag = TRUE;
		PutMissile(i);
		return;
	}

	if (plr[id]._pMana <= 0 || (!plr[id].plractive)) missile[i]._mirange = 0;

	if (plr[id]._pHitPoints < missile[i]._miVar1) {
		diff = missile[i]._miVar1 - plr[id]._pHitPoints;
		pct = 0;
		for (j = 0; j < missile[i]._mispllvl && j < 7; ++j) pct += 3;
		if (pct > 0) diff = diff - (diff / pct);
		if (diff < 0) diff = 0;
		drawmanaflag = TRUE;
		drawhpflag = TRUE;
		if (plr[id]._pMana >= diff) {
			plr[id]._pHitPoints = missile[i]._miVar1;
			plr[id]._pHPBase = missile[i]._miVar2;
			plr[id]._pMana -= diff;
			plr[id]._pManaBase -= diff;
		} else {
			plr[id]._pHitPoints -= (diff - plr[id]._pMana);
			plr[id]._pHPBase -= (diff - plr[id]._pMana);
			plr[id]._pMana = 0;
			plr[id]._pManaBase = -(plr[id]._pMaxMana - plr[id]._pMaxManaBase);
			missile[i]._mirange = 0;
			missile[i]._miDelFlag = TRUE;
			if (plr[id]._pHitPoints < 0) SetPlayerHitPoints(id, 0);
			if (((plr[id]._pHitPoints >> HP_SHIFT) == 0) && (id == myplr))
				StartPlrKill(id, missile[i]._miVar8);
		}
	}

	missile[i]._miVar1 = plr[id]._pHitPoints;
	missile[i]._miVar2 = plr[id]._pHPBase;

	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		NetSendCmd(TRUE, CMD_ENDSHIELD);
	}
	PutMissile(i);
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Ether(int i)
{
#if (PRE_BETA && PRE_ETHER) || !PRE_BETA
	int id;

	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	id = missile[i]._misource;

	missile[i]._mix = plr[id]._px;
	missile[i]._miy = plr[id]._py;
	missile[i]._mitxoff = plr[id]._pxoff << 16;
	missile[i]._mityoff = plr[id]._pyoff << 16;

	if (plr[id]._pmode == PM_WALK3) {
		missile[i]._misx = plr[id]._pfutx;
		missile[i]._misy = plr[id]._pfuty;
	} else {
		missile[i]._misx = plr[id]._px;
		missile[i]._misy = plr[id]._py;
	}
	
	GetMissilePos(i);

	if (plr[id]._pmode == PM_WALK3) {
		if (plr[id]._pdir == DIR_L) ++missile[i]._mix;
		else ++missile[i]._miy;
	}

	plr[id]._pSpellFlags |= SF_ETHER;

	if (missile[i]._mirange == 0 || plr[id]._pHitPoints <= 0) {
		missile[i]._miDelFlag = TRUE;
		plr[id]._pSpellFlags &= ~SF_ETHER;
	}

	PutMissile(i);
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Firemove(int i)
{
	int j;
	int ExpLight[14] = {2,3,4,5,5,6,7,8,9,10,11,12,12};

	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mix;
	--missile[i]._miy;
	missile[i]._miyoff+=32;

	++missile[i]._miVar1;

	if (missile[i]._miVar1 == missile[i]._miAnimLen) {
		SetMissDir(i, 1);
		missile[i]._miAnimFrame = random(82, 11) + 1;
	}

	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);

	j = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 0, missile[i]._mix, missile[i]._miy, 0);
	if (missile[i]._miHitFlag == TRUE) missile[i]._mirange = j;

	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	if ((missile[i]._mimfnum == 0) && (missile[i]._mirange != 0)) {
		if (missile[i]._miVar2 == 0) 
			missile[i]._mlid = AddLight(missile[i]._mix, missile[i]._miy, ExpLight[(missile[i]._miVar2)]);
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, ExpLight[(missile[i]._miVar2)]);
		++missile[i]._miVar2;
	}
	else if ((missile[i]._mix != missile[i]._miVar3) || (missile[i]._miy != missile[i]._miVar4)) {
			missile[i]._miVar3 = missile[i]._mix;
			missile[i]._miVar4 = missile[i]._miy;
			ChangeLight(missile[i]._mlid, missile[i]._miVar3, missile[i]._miVar4, 8);
		}
	++missile[i]._mix;
	++missile[i]._miy;
	missile[i]._miyoff-=32;
	PutMissile(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Guardian(int i)
{
#if (PRE_BETA && PRE_GUARDIAN) || !PRE_BETA
	int j, k, sx, sy, sx1, sy1, ex;

	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	if (missile[i]._miVar2 > 0) --missile[i]._miVar2;

	if ((missile[i]._mirange == missile[i]._miVar1) || ((missile[i]._mimfnum == 2) &&
		(missile[i]._miVar2 == 0))) {
		SetMissDir(i, 1);
	}

	if ((missile[i]._mirange % 16) == 0) {
		ex = 0;
		for (k = 0; (k < 23) && (ex != -1); ++k) {
			for (j = 10; (j >= 0) && (ex != -1); j-=2) {
				if (vCrawlTable[k][j] == 0 && vCrawlTable[k][j + 1] == 0) break;
				if ((sx1 != vCrawlTable[k][j]) || (sy1 != vCrawlTable[k][j + 1])) {
					sx = missile[i]._mix + vCrawlTable[k][j];
					sy = missile[i]._miy + vCrawlTable[k][j+1];
					ex = Sentfire(i, sx, sy);
					if (ex == -1) break;

					sx = missile[i]._mix - vCrawlTable[k][j];
					sy = missile[i]._miy - vCrawlTable[k][j+1];
					ex = Sentfire(i, sx, sy);
					if (ex == -1) break;

					sx = missile[i]._mix + vCrawlTable[k][j];
					sy = missile[i]._miy - vCrawlTable[k][j+1];
					ex = Sentfire(i, sx, sy);
					if (ex == -1) break;

					sx = missile[i]._mix - vCrawlTable[k][j];
					sy = missile[i]._miy + vCrawlTable[k][j+1];
					ex = Sentfire(i, sx, sy);
					if (ex == -1) break;

					sx1 = vCrawlTable[k][j];
					sy1 = vCrawlTable[k][j + 1];
				}
			}
		}
	}

	if (missile[i]._mirange == 14) {
		SetMissDir(i, 0);
		missile[i]._miAnimFrame = 15;
		missile[i]._miAnimAdd = -1;
	}

	missile[i]._miVar3 += missile[i]._miAnimAdd;
	if (missile[i]._miVar3 > 15) {
		missile[i]._miVar3 = 15;
	} else {
		if (missile[i]._miVar3 > 0)
			ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, missile[i]._miVar3);
	}
	
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	
	PutMissile(i);
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Chain(int i)
{
#if (PRE_BETA && PRE_CHAIN) || !PRE_BETA
	int sx, sy, id, dir;
	int l, n, m, k, rad;
	int tx, ty;

	app_assert(i < MAXMISSILES && i >= 0);
	id = missile[i]._misource;
	sx = missile[i]._mix;
	sy = missile[i]._miy;

	dir = GetDirection(sx, sy, missile[i]._miVar1, missile[i]._miVar2);
	AddMissile(sx, sy, missile[i]._miVar1, missile[i]._miVar2, dir, MIT_LIGHTCTRL, MI_ENEMYMONST, id, 1, missile[i]._mispllvl);

	rad = 3 + missile[i]._mispllvl;
	if (rad > 19) rad = 19;
	for (m = 1; m < rad; ++m) {
		n = CrawlNum[m];
		l = n + 1;
		for (k = CrawlTable[n]; k > 0; --k) {
			tx = sx + CrawlTable[l];
			ty = sy + CrawlTable[(l + 1)];
			if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
				if ((dMonster[tx][ty]) > 0) {
					dir = GetDirection(sx, sy, tx, ty);
					AddMissile(sx, sy, tx, ty, dir, MIT_LIGHTCTRL, MI_ENEMYMONST, id, 1, missile[i]._mispllvl);
				}
			}
			l += 2;
		}
	}

	--missile[i]._mirange;
	if (missile[i]._mirange == 0) missile[i]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
/*void MI_ChainOLD(int i)
{
	int dir;

	if ((missile[i]._mirange % 3) == 0) {
		dir = GetDirection(missile[i]._mix, missile[i]._miy, missile[i]._miVar5, missile[i]._miVar6);
		AddMissile(missile[i]._mix, missile[i]._miy, missile[i]._miVar5, missile[i]._miVar6, dir, MIT_CHAINBALL, MI_ENEMYMONST, missile[i]._misource, missile[i]._midam);
		++missile[i]._miVar7;
	}

	--missile[i]._mirange;
	if (missile[i]._mirange == 0) missile[i]._miDelFlag = TRUE;
}*/

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
/*void MI_ChainballOLD(int i)
{
	int mx, id;
	int k, l, m, n, rad;

	--missile[i]._mirange;
	id = missile[i]._misource;

	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);

	if ((missile[i]._miVar2 == 1) && (missile[i]._mix == missile[i]._miVar3) && (missile[i]._miy == missile[i]._miVar4)) {
		missile[i]._miVar2 = 0;
		missile[i]._mirange = 0;

		k = 0;
		for (l = 0; l < nummissiles; ++l) {
			mx = missileactive[l];
			if (missile[mx]._mitype == MIT_CHAINBALL && missile[i]._miVar1 == missile[mx]._miVar1) ++k;
			if ((missile[mx]._mitype == MIT_CHAIN) && (missile[i]._miVar1 == missile[mx]._midam))
				k+=((4 + (GetSpellLevel(id, SPL_LIGHTNING) / 3)) - missile[mx]._miVar7);
		}

		if (k == 1) {
			rad = 6 + GetSpellLevel(id, SPL_CHAIN);
			if (rad > 19) rad = 19;
			for (m = 1; m < rad; ++m) {
				n = CrawlNum[m];
				l = n + 1;
				for (k = CrawlTable[n]; k > 0; --k) {
					if (ChainBounce (i, missile[i]._mix + CrawlTable[l], missile[i]._miy + CrawlTable[(l + 1)]) > 0) {
						m = rad;
						break;
					}
					l += 2;
				}
			}
		}
	} else {
		if ((missile[i]._mix != missile[i]._miVar5) || (missile[i]._miy != missile[i]._miVar6))
			CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy, 0);

		if (missile[i]._miHitFlag == TRUE &&  missile[i]._mirange == 0 && missile[i]._miVar2 == 0) {
			for (l = 0; l < nummissiles; ++l) {
				mx = missileactive[l];
				if ((missile[mx]._mitype == MIT_CHAIN && missile[i]._miVar1 == missile[mx]._midam) ||
					(missile[mx]._mitype == MIT_CHAINBALL && missile[i]._miVar1 == missile[mx]._miVar1)) {
					missile[mx]._miVar2 = 1;					// flag hit and cont until v3, v4
					missile[mx]._miVar3 = missile[i]._mix;		// x of hit
					missile[mx]._miVar4 = missile[i]._miy;		// y of hit
				}
			}
		}
	}

	if (missile[i]._mirange == 0) missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}*/

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Blood(int i)
{
	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	if (missile[i]._mirange == 0) missile[i]._miDelFlag = TRUE;
	if (missile[i]._miAnimFrame == missile[i]._miAnimLen) {
//		missile[i]._miAnimFrame = missile[i]._miAnimLen;
		missile[i]._miPreFlag = TRUE;
	}
	PutMissile(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Weapexp(int i)
{
	int id, mind, maxd;
	int ExpLight[10] = {9,10,11,12,11,10,8,6,4,2};

	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;

	id = missile[i]._misource;
	if (missile[i]._miVar2 == 1) {
		mind = plr[id]._pIFMinDam;
		maxd = plr[id]._pIFMaxDam;
		missiledata[missile[i]._mitype].mResist = MIMT_FIRE;
	} else {
		mind = plr[id]._pILMinDam;
		maxd = plr[id]._pILMaxDam;
		missiledata[missile[i]._mitype].mResist = MIMT_LGHT;
	}

	CheckMissileCol(i, mind, maxd, 0, missile[i]._mix, missile[i]._miy, 0);

	if (missile[i]._miVar1 == 0) {
		missile[i]._mlid = AddLight(missile[i]._mix, missile[i]._miy, ExpLight[(missile[i]._miVar1)]);
	} else {
		if (missile[i]._mirange != 0) 
			ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, ExpLight[(missile[i]._miVar1)]);
	}

	++missile[i]._miVar1;
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
		return;
	}

	PutMissile(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Misexp(int i)
{
	int ExpLight[15] = {9,10,11,12,11,10,8,6,4,2,1,0,0,0,0};

	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
		return;
	}

	if (missile[i]._miVar1 == 0) {
		missile[i]._mlid = AddLight(missile[i]._mix, missile[i]._miy, ExpLight[(missile[i]._miVar1)]);
	} else {
		if (missile[i]._mirange != 0) 
			ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, ExpLight[(missile[i]._miVar1)]);
	}

	++missile[i]._miVar1;
	PutMissile(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Acidsplat(int i)
{
	int monst;
	int dam;

	app_assert(i < MAXMISSILES && i >= 0);
	if (missile[i]._mirange == missile[i]._miAnimLen) {
		++missile[i]._mix;
		++missile[i]._miy;
		missile[i]._miyoff-=32;
	}

	--missile[i]._mirange;
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		monst = missile[i]._misource;
		dam = (monster[monst].MData->mLevel < 2) ? 1:2;
		AddMissile(missile[i]._mix, missile[i]._miy, i, 0, missile[i]._mimfnum, MIT_ACIDPUD, MI_ENEMYPLR, missile[i]._misource, dam, missile[i]._mispllvl);
		return;
	}

	PutMissile(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Teleport(int i)
{
#if (PRE_BETA && PRE_TELE) || !PRE_BETA
	int id;

	app_assert(i < MAXMISSILES && i >= 0);
	id = missile[i]._misource;

	//sprintf(tempstr, "X: %i Y: %i FX: %i FY: %i", plr[myplr]._px, plr[myplr]._py, plr[myplr]._pfutx, plr[myplr]._pfuty);
	//AddPanelString(tempstr, TEXT_LEFT);

	--missile[i]._mirange;
	if (missile[i]._mirange <= 0) {
		missile[i]._miDelFlag = TRUE;
		return;
	}

	dPlayer[(plr[id]._px)][(plr[id]._py)] = 0;
	PlrClrTrans(plr[id]._px, plr[id]._py);
	plr[id]._px = missile[i]._mix;
	plr[id]._py = missile[i]._miy;
	plr[id]._pfutx = plr[id]._px;
	plr[id]._pfuty = plr[id]._py;
	plr[id]._poldx = plr[id]._px;
	plr[id]._poldy = plr[id]._py;
	PlrDoTrans(plr[id]._px, plr[id]._py);
	missile[i]._miVar1 = 1;
	dPlayer[(plr[id]._px)][(plr[id]._py)] = 1 + (char)id;

	if (leveltype != 0) {
		ChangeLightXY(plr[id]._plid, plr[id]._px, plr[id]._py);
		ChangeVisionXY(plr[id]._pvid, plr[id]._px, plr[id]._py);
	}

	if (id == myplr) {
		ViewX = plr[id]._px - ScrollInfo._sdx;
		ViewY = plr[id]._py - ScrollInfo._sdy;
	}
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
/*void MI_Doom(int i)
{
	int k, l, m, n;
	int mid;

	--missile[i]._mirange;

	// check if monster moved
	mid = dMonster[(missile[i]._miVar1)][(missile[i]._miVar2)];
	if (mid != 0) {
		if (mid > 0) --mid;
		else mid = -(mid + 1);
	}

	// if he did search again
	if (mid != missile[i]._miVar3) {
		for (m = 0; m < 6; ++m) {
			n = CrawlNum[m];
			l = n + 1;
			for (k = CrawlTable[n]; k > 0; --k) {
				mid = dMonster[((missile[i]._miVar1) + CrawlTable[l])][((missile[i]._miVar2) + CrawlTable[(l + 1)])];
				if (mid != 0) {
					if (mid > 0) --mid;
					else mid = -(mid + 1);
					if (mid == missile[i]._miVar3) {
						missile[i]._miVar1 += CrawlTable[l];
						missile[i]._miVar2 += CrawlTable[l + 1];
						k = -99;
						m = 6;
						break;
					}
				}
				l += 2;
			}
		}

		// if monster killed before doom kills it
		if (k != -99) {
			missile[i]._miDelFlag = TRUE;
			PutMissile(i);
			return;
		}

		// get new velocity and directions
		GetMissileVel(i, missile[i]._mix, missile[i]._miy, missile[i]._miVar1, missile[i]._miVar2, 16);
		SetMissDir(i, GetDirection(missile[i]._mix, missile[i]._miy, missile[i]._miVar1, missile[i]._miVar2));
	}

	
	// move missile
	if (missile[i]._mimfnum != 9) {
		missile[i]._mitxoff += missile[i]._mixvel;
		missile[i]._mityoff += missile[i]._miyvel;
		GetMissilePos(i);
	}

	// get mid for current missile pos - for checking if target monster killed
	mid = dMonster[(missile[i]._mix)][(missile[i]._miy)];
	if (mid != 0) {
		if (mid > 0) --mid;
		else mid = -(mid + 1);
	}

	// check for collision if not followed monster dont end missile
	k = missile[i]._mirange;
	if ((missile[i]._mix != plr[myplr]._px) || (missile[i]._miy != plr[myplr]._py)) CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy, 0);
	if ((missile[i]._mirange == 0) && (mid != missile[i]._miVar3) && (missile[i]._miHitFlag == TRUE)) missile[i]._mirange = k;
	
	// if target monster not dead halt missile in place
	if ((missile[i]._mirange == 0) && (mid == missile[i]._miVar3)) {
		missile[i]._mirange = k;
		if ((monster[mid]._mhitpoints >> HP_SHIFT) > 0) {
			if (missile[i]._mimfnum != 9) {
				SetMissDir(i, 9);
			}
		} else {
			// target is killed so re-search
			for (m = 1; m < 6; ++m) {
				n = CrawlNum[m];
				l = n + 1;
				for (k = CrawlTable[n]; k > 0; --k) {
					mid = dMonster[((missile[i]._miVar1) + CrawlTable[l])][((missile[i]._miVar2) + CrawlTable[(l + 1)])];
					if (mid != 0) {
						if (mid > 0) --mid;
						else mid = -(mid + 1);
						if ((monster[mid]._mhitpoints >> HP_SHIFT) > 0) {
							missile[i]._miVar1 += CrawlTable[l];
							missile[i]._miVar2 += CrawlTable[l + 1];
							missile[i]._miVar3 = mid;
							k = -99;
							m = 6;
							break;
						}
					}
					l += 2;
				}
			}

			// get new velocity and directions
			if (k == 0) missile[i]._mirange = 0;
			else {
				GetMissileVel(i, missile[i]._mix, missile[i]._miy, missile[i]._miVar1, missile[i]._miVar2, 16);
				SetMissDir(i, GetDirection(missile[i]._mix, missile[i]._miy, missile[i]._miVar1, missile[i]._miVar2));
			}
		}
	}

	// delete or place missile
	if (missile[i]._mirange == 0) missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}*/

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Stone (int i)
{
#if (PRE_BETA && PRE_STONE) || !PRE_BETA
	int m;

	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	m = missile[i]._miVar2;

	if (monster[m]._mhitpoints == 0 && missile[i]._miAnimType != MF_STONE) {
		SetMissAnim(i, MF_STONE);
		missile[i]._mirange = 11;
	}
	
	
	if (monster[m]._mmode != MM_STONE) missile[i]._miDelFlag = TRUE;

	else {
		if (missile[i]._mirange == 0) {
			missile[i]._miDelFlag = TRUE;
			if (monster[m]._mhitpoints > 0) {
//				app_assert(dMonster[monster[m]._mx][monster[m]._my] == m+1
//							|| dMonster[monster[m]._mx][monster[m]._my] == -(m+1));
				monster[m]._mmode = missile[i]._miVar1;
			}
			else AddDead(monster[m]._mx, monster[m]._my, stonendx, monster[m]._mdir);
		}
	
		if (missile[i]._miAnimType == MF_STONE) PutMissile(i);
	}
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Boom(int i)
{
	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	if (missile[i]._miVar1 == 0) CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 0, missile[i]._mix, missile[i]._miy, 1);
	if (missile[i]._miHitFlag == TRUE) missile[i]._miVar1 = 1;

	if (missile[i]._mirange == 0) missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Rhino(int i)
{
	int mix,miy;
	int mix2, miy2;
	int omx, omy;
	int monst;
	BOOL placemiss = FALSE;

	app_assert(i < MAXMISSILES && i >= 0);
	monst = missile[i]._misource;

	app_assert(monst < MAXMONSTERS && monst >= 0);
	if (monster[monst]._mmode != MM_MISSILE) {	// monster got blood-boiled or something
		missile[i]._miDelFlag = TRUE;
		return;
	}

	GetMissilePos(i);

	omx = missile[i]._mix;
	omy = missile[i]._miy;

	dMonster[omx][omy] = NULL;

	if (monster[monst]._mAi == AI_SNAKE)
	{
		// Snake should stop a few frames before it enters player's square
		// So we run its animation a few frames ahead here and test below
		// to see if the square is available
		missile[i]._mitxoff += 2*missile[i]._mixvel;
		missile[i]._mityoff += 2*missile[i]._miyvel;

		GetMissilePos(i);

		mix2 = missile[i]._mix;
		miy2 = missile[i]._miy;

		missile[i]._mitxoff -= 1*missile[i]._mixvel;
		missile[i]._mityoff -= 1*missile[i]._miyvel;
	}
	else
	{
		missile[i]._mitxoff += missile[i]._mixvel;
		missile[i]._mityoff += missile[i]._miyvel;
	}
	
	GetMissilePos(i);

	mix = missile[i]._mix;
	miy = missile[i]._miy;


	if (PosOkMonst(monst, mix, miy)
		&& (monster[monst]._mAi != AI_SNAKE || PosOkMonst(monst, mix2, miy2)))
	{
		dMonster[mix][miy] = -(monst + 1);
		monster[monst]._mx = monster[monst]._moldx = monster[monst]._mfutx = mix;
		monster[monst]._my = monster[monst]._moldy = monster[monst]._mfuty = miy;

		if(monster[monst]._uniqtype)
			ChangeLightXY(missile[i]._mlid, mix, miy);

		MoveMissilePos(i);
		PutMissile(i);
	}
	else
	{
		MissToMonst(i,omx,omy);
		missile[i]._miDelFlag = TRUE;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Fireman(int i)
{
	int mix,miy;
	int omx, omy;
	int id, m;
	BOOL placemiss = FALSE;
	int px,py, p;

	GetMissilePos(i);

	app_assert(i < MAXMISSILES && i >= 0);
	omx = missile[i]._mix;
	omy = missile[i]._miy;

	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	
	GetMissilePos(i);

	m = missile[i]._misource;
	mix = missile[i]._mix;
	miy = missile[i]._miy;

	p = monster[m]._menemy;
	if ((monster[m]._mFlags & MFLAG_MID) == 0) {
		px = plr[p]._px;
		py = plr[p]._py;
	} else {
		px = monster[p]._mx;
		py = monster[p]._my;
	}

	if((mix != omx || miy != omy)
	   &&
	   ((missile[i]._miVar1 & MIF_DIDHIT
		 && !DIST(omx-px, omy-py, 4))
		|| missile[i]._miVar2 > 1)
	   && PosOkMonst(missile[i]._misource, omx, omy)
		)
	{
		// return to monster domain
		MissToMonst(i,omx, omy);
		missile[i]._miDelFlag = TRUE;
	}

	else 
		if ((monster[m]._mFlags & MFLAG_MID) == 0)	id = dPlayer[mix][miy];
		else id = dMonster[mix][miy];
		if(!PosOkMissile(mix, miy)
		|| (id > 0 && !(missile[i]._miVar1 & MIF_DIDHIT)))
	{
		// bounce away
		missile[i]._mixvel = -missile[i]._mixvel;
		missile[i]._miyvel = -missile[i]._miyvel;
		missile[i]._mimfnum = opposite[missile[i]._mimfnum];
		missile[i]._miAnimData = monster[m].MType->Anims[MA_WALK].Cels[missile[i]._mimfnum];

		++missile[i]._miVar2;

		if(id > 0)
		{
			missile[i]._miVar1 |= MIF_DIDHIT;
		}
		placemiss = TRUE;
	}
	else
		placemiss = TRUE;

	if(placemiss)
	{
//			ChangeLightXY(monster[m].mlid, monster[m]._mx, monster[m]._my);

		MoveMissilePos(i);
		PutMissile(i);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_FirewallC(int i)
{
#if (PRE_BETA && PRE_WALL) || !PRE_BETA
	int tx, ty, pn, id;

	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	id = missile[i]._misource;
	if (missile[i]._mirange == 0) missile[i]._miDelFlag = TRUE;
	else {
		// if you take this out of here the wall will go though everything (hence using the nsolidtable)
		//if (missile[i]._miVar1 >= MAXDUNX || missile[i]._miVar2 >= MAXDUNY)
		//	app_fatal("Tried placing firewall piece off edge of map at (%d,%d)",missile[i]._miVar1,missile[i]._miVar2);
		pn = dPiece[missile[i]._miVar1][missile[i]._miVar2];
		app_assert(pn <= MAXTILES && pn >= 0); 
		tx = missile[i]._miVar1 + XDirAdd[missile[i]._miVar3];
		ty = missile[i]._miVar2 + YDirAdd[missile[i]._miVar3];
		if (nMissileTable[pn] == 0 && missile[i]._miVar8 == 0 && tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
			AddMissile(missile[i]._miVar1, missile[i]._miVar2, missile[i]._miVar1, missile[i]._miVar2, plr[id]._pdir, MIT_FIREWALL, MI_ENEMYBOTH, id, 0, missile[i]._mispllvl);
			missile[i]._miVar1 = tx;
			missile[i]._miVar2 = ty;
		} else missile[i]._miVar8 = 1;
		
		// if you take this out of here the wall will go though everything (hence using the nsolidtable)
		//if (missile[i]._miVar5 >= MAXDUNX || missile[i]._miVar6 >= MAXDUNY)
		//	app_fatal("Tried placing firewall piece off edge of map at (%d,%d)",missile[i]._miVar5,missile[i]._miVar6);
		pn = dPiece[missile[i]._miVar5][missile[i]._miVar6];
		app_assert(pn <= MAXTILES && pn >= 0); 
		tx = missile[i]._miVar5 + XDirAdd[missile[i]._miVar4];
		ty = missile[i]._miVar6 + YDirAdd[missile[i]._miVar4];
		if (nMissileTable[pn] == 0 && missile[i]._miVar7 == 0 && tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
			AddMissile(missile[i]._miVar5, missile[i]._miVar6, missile[i]._miVar5, missile[i]._miVar6, plr[id]._pdir, MIT_FIREWALL, MI_ENEMYBOTH, id, 0, missile[i]._mispllvl);
			missile[i]._miVar5 = tx;
			missile[i]._miVar6 = ty;
		} else missile[i]._miVar7 = 1;
	}
#endif
}
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_FlameBox(int i)
{
#if (PRE_BETA && PRE_WALL) || !PRE_BETA
	app_assert(i < MAXMISSILES && i >= 0);
	missile[i]._miDelFlag = TRUE;
	int const id = missile[i]._micaster;
	int const l = CrawlNum[3];
	int j = l + 1;
	int const dam = ((random(53, 10) + random(53, 10) + 2 + ((id > 0) ? plr[id]._pLevel : currlevel)) << 4) >> 1;
	
	for (int m = CrawlTable[l]; m > 0; --m, j+=2) {
		int const tx = missile[i]._miVar1 + CrawlTable[j];
		int const ty = missile[i]._miVar2 + CrawlTable[j + 1];
		
		if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
			int const pn = dPiece[tx][ty];
			
			if (!nSolidTable[pn] && dObject[tx][ty] == 0
				&& LineClear(missile[i]._mix, missile[i]._miy, tx, ty)
				)
			{
				if (nMissileTable[pn] == 0 && missile[i]._miVar8 == 0 && tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
					AddMissile(tx, ty, tx, ty, 0, MIT_FIREWALL, MI_ENEMYBOTH, id, dam, missile[i]._mispllvl);
				} else missile[i]._miVar8 = 1;
		
			}
		}
	}
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_LightBox(int i)
{
#if (PRE_BETA && PRE_WALL) || !PRE_BETA
	app_assert(i < MAXMISSILES && i >= 0);
	missile[i]._miDelFlag = TRUE;
	int const id = missile[i]._micaster;
	int const l = CrawlNum[3];
	int j = l + 1;
	int const dam = ((random(53, 10) + random(53, 10) + 2 + ((id > 0) ? plr[id]._pLevel : currlevel)) << 4) >> 1;
	
	for (int m = CrawlTable[l]; m > 0; --m, j+=2) {
		int const tx = missile[i]._miVar1 + CrawlTable[j];
		int const ty = missile[i]._miVar2 + CrawlTable[j + 1];
		
		if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
			int const pn = dPiece[tx][ty];
			
			if (!nSolidTable[pn] && dObject[tx][ty] == 0
				&& LineClear(missile[i]._mix, missile[i]._miy, tx, ty)
				)
			{
				if (nMissileTable[pn] == 0 && missile[i]._miVar8 == 0 && tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
					AddMissile(tx, ty, tx, ty, 0, MIT_LIGHTWALL, MI_ENEMYBOTH, id, dam, missile[i]._mispllvl);
				} else missile[i]._miVar8 = 1;
		
			}
		}
	}
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_ShowMagicItems(int i)
{
#if (PRE_BETA && PRE_WALL) || !PRE_BETA
	app_assert(i < MAXMISSILES && i >= 0);
	
	--missile[i]._mirange;
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		int const id = missile[i]._miVar1;
	    PlaySfxLoc(IS_CAST7, plr[id]._px, plr[id]._py);
	 	HighLightAllItems = false;
	}
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_LightwallC(int i)
{
// GWP 9/4/97
// Copy of MI_FirewallC with switched to call the MIT_LIGHTWALL instead of MIT_FIREWALL
#if (PRE_BETA && PRE_WALL) || !PRE_BETA
	int tx, ty, pn;

	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	
	int const id = missile[i]._misource;
	int const dam = (random(53, 10) + random(53, 10) + 2 + ((id > 0) ? plr[id]._pLevel : 0)) << 4;

	
	if (missile[i]._mirange == 0) missile[i]._miDelFlag = TRUE;
	else {
		// if you take this out of here the wall will go though everything (hence using the nsolidtable)
		//if (missile[i]._miVar1 >= MAXDUNX || missile[i]._miVar2 >= MAXDUNY)
		//	app_fatal("Tried placing firewall piece off edge of map at (%d,%d)",missile[i]._miVar1,missile[i]._miVar2);
		pn = dPiece[missile[i]._miVar1][missile[i]._miVar2];
		app_assert(pn <= MAXTILES && pn >= 0); 
		tx = missile[i]._miVar1 + XDirAdd[missile[i]._miVar3];
		ty = missile[i]._miVar2 + YDirAdd[missile[i]._miVar3];
		if (nMissileTable[pn] == 0 && missile[i]._miVar8 == 0 && tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
			AddMissile(missile[i]._miVar1, missile[i]._miVar2, missile[i]._miVar1, missile[i]._miVar2, plr[id]._pdir, MIT_LIGHTWALL, MI_ENEMYBOTH, id, dam, missile[i]._mispllvl);
			missile[i]._miVar1 = tx;
			missile[i]._miVar2 = ty;
		} else missile[i]._miVar8 = 1;
		
		// if you take this out of here the wall will go though everything (hence using the nsolidtable)
		//if (missile[i]._miVar5 >= MAXDUNX || missile[i]._miVar6 >= MAXDUNY)
		//	app_fatal("Tried placing firewall piece off edge of map at (%d,%d)",missile[i]._miVar5,missile[i]._miVar6);
		pn = dPiece[missile[i]._miVar5][missile[i]._miVar6];
		app_assert(pn <= MAXTILES && pn >= 0); 
		tx = missile[i]._miVar5 + XDirAdd[missile[i]._miVar4];
		ty = missile[i]._miVar6 + YDirAdd[missile[i]._miVar4];
		if (nMissileTable[pn] == 0 && missile[i]._miVar7 == 0 && tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
			AddMissile(missile[i]._miVar5, missile[i]._miVar6, missile[i]._miVar5, missile[i]._miVar6, plr[id]._pdir, MIT_LIGHTWALL, MI_ENEMYBOTH, id, dam, missile[i]._mispllvl);
			missile[i]._miVar5 = tx;
			missile[i]._miVar6 = ty;
		} else missile[i]._miVar7 = 1;
	}
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Infra(int i)
{
#if (PRE_BETA && PRE_INFRA) || !PRE_BETA
	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	plr[(missile[i]._misource)]._pInfraFlag = TRUE;

	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		CalcPlrItemVals(missile[i]._misource,TRUE);
	}
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Apoca(int i)
{
#if (PRE_BETA && PRE_APOCA) || !PRE_BETA
	int j, k, id;
	BOOL exit;

	app_assert(i < MAXMISSILES && i >= 0);
	id = missile[i]._misource;

	exit = FALSE;
	for (j = missile[i]._miVar2; (j < missile[i]._miVar3) && (exit == FALSE); ++j) {
		for (k = missile[i]._miVar4; (k < missile[i]._miVar5) && (exit == FALSE); ++k) {
			if (dMonster[k][j] > 3 
				&& nSolidTable[(dPiece[k][j])] == 0
				&& LineClear(missile[i]._mix, missile[i]._miy, k, j)) {
				AddMissile(k, j, k, j, plr[id]._pdir, MIT_BOOM, MI_ENEMYMONST, id, missile[i]._midam, 0);
				exit = TRUE;
			}
		}
		if (exit == FALSE) missile[i]._miVar4 = missile[i]._miVar6;
	}

	if (exit == TRUE) {
		missile[i]._miVar2 = j - 1;
		missile[i]._miVar4 = k;
	} else missile[i]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Wave(int i)
{
#if (PRE_BETA && PRE_WAVE) || !PRE_BETA
	int dira, dirb, nxa, nya, nxb, nyb;
	int pn, sd, j, f1, f2, id, sx, sy, dx, dy;

	f1 = 0;
	f2 = 0;
	app_assert(i < MAXMISSILES && i >= 0);
	id = missile[i]._misource;
	sx = missile[i]._mix;
	sy = missile[i]._miy;
	dx = missile[i]._miVar1;
	dy = missile[i]._miVar2;

	sd = GetDirection(sx, sy, dx, dy);
	dira = (sd - 2) & 0x0007;
	dirb = (sd + 2) & 0x0007;
	nxa = sx + XDirAdd[sd];
	nya = sy + YDirAdd[sd];

	//if (nxa >= MAXDUNX || nya >= MAXDUNY)
	//	app_fatal("Tried placing flamewave piece off edge of map at (%d,%d)",nxa,nya);
	pn = dPiece[nxa][nya];
	app_assert(pn <= MAXTILES && pn >= 0);
	if (nMissileTable[pn] == 0) {
		AddMissile(nxa, nya, nxa + XDirAdd[sd], nya + YDirAdd[sd], plr[id]._pdir, MIT_FIREMOVE, MI_ENEMYMONST, id, 0, missile[i]._mispllvl);

		nxa += XDirAdd[dira];
		nya += YDirAdd[dira];
		nxb = sx + XDirAdd[sd] + XDirAdd[dirb];
		nyb = sy + YDirAdd[sd] + YDirAdd[dirb];
		for (j = 0; j < (2 + (missile[i]._mispllvl>>1)); ++j) {
			//if (nxa >= MAXDUNX || nya >= MAXDUNY)
			//	app_fatal("Tried placing flamewave piece off edge of map at (%d,%d)",nxa,nya);
			pn = dPiece[nxa][nya];
			app_assert(pn <= MAXTILES && pn >= 0);
			if (nMissileTable[pn] == 0 && f1 == 0 && nxa > 0 && nxa < MAXDUNX && nya > 0 && nya < MAXDUNY) {
				AddMissile(nxa, nya, nxa + XDirAdd[sd], nya + YDirAdd[sd], plr[id]._pdir, MIT_FIREMOVE, MI_ENEMYMONST, id, 0, missile[i]._mispllvl);
				nxa += XDirAdd[dira];
				nya += YDirAdd[dira];
			} else f1 = 1;
			
			//if (nxb >= MAXDUNX || nyb >= MAXDUNY)
			//	app_fatal("Tried placing flamewave piece off edge of map at (%d,%d)",nxb,nyb);
			pn = dPiece[nxb][nyb];
			app_assert(pn <= MAXTILES && pn >= 0);
			if (nMissileTable[pn] == 0 && f2 == 0 && nxb > 0 && nxb < MAXDUNX && nyb > 0 && nyb < MAXDUNY) {
				AddMissile(nxb, nyb, nxb + XDirAdd[sd], nyb + YDirAdd[sd], plr[id]._pdir, MIT_FIREMOVE, MI_ENEMYMONST, id, 0, missile[i]._mispllvl);
				nxb += XDirAdd[dirb];
				nyb += YDirAdd[dirb];
			} else f2 = 1;
		}
	}

	--missile[i]._mirange;
	if (missile[i]._mirange == 0) missile[i]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#define RAD 6

void MI_Nova(int i)
{
#if (PRE_BETA && PRE_NOVA) || !PRE_BETA
	int k, id, sx, sy, dir, en;
	int sx1, sy1, dam, dx, dy;

	sx1 = sy1 = 0;
	app_assert(i < MAXMISSILES && i >= 0);
	id = missile[i]._misource;
	dam = missile[i]._midam;
	sx = missile[i]._mix;
	sy = missile[i]._miy;
	dx = missile[i]._miVar1;
	dy = missile[i]._miVar2;
	if (id != -1) {
		dir = plr[id]._pdir;
		en = MI_ENEMYMONST;
	} else {
		dir = 0;
		en = MI_ENEMYPLR;
	}

	for (k = 0; k < 23; ++k) {
		if ((sx1 != vCrawlTable[k][RAD]) || (sy1 != vCrawlTable[k][RAD + 1])) {
			dx = sx + vCrawlTable[k][RAD];
			dy = sy + vCrawlTable[k][RAD + 1];
			AddMissile(sx, sy, dx, dy, dir, MIT_LIGHTBALL, en, id, dam, missile[i]._mispllvl);

			dx = sx - vCrawlTable[k][RAD];
			dy = sy - vCrawlTable[k][RAD + 1];
			AddMissile(sx, sy, dx, dy, dir, MIT_LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			
			dx = sx - vCrawlTable[k][RAD];
			dy = sy + vCrawlTable[k][RAD + 1];
			AddMissile(sx, sy, dx, dy, dir, MIT_LIGHTBALL, en, id, dam, missile[i]._mispllvl);

			dx = sx + vCrawlTable[k][RAD];
			dy = sy - vCrawlTable[k][RAD + 1];
			AddMissile(sx, sy, dx, dy, dir, MIT_LIGHTBALL, en, id, dam, missile[i]._mispllvl);

			sx1 = vCrawlTable[k][RAD];
			sy1 = vCrawlTable[k][RAD + 1];
		}
	}

	--missile[i]._mirange;
	if (missile[i]._mirange == 0) missile[i]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MI_FireNova(int i)
{
#if (PRE_BETA && PRE_NOVA) || !PRE_BETA
	int k, id, sx, sy, dir, en;
	int sx1, sy1, dam, dx, dy;

	sx1 = sy1 = 0;
	app_assert(i < MAXMISSILES && i >= 0);
	id = missile[i]._misource;
	dam = missile[i]._midam;
	sx = missile[i]._mix;
	sy = missile[i]._miy;
	dx = missile[i]._miVar1;
	dy = missile[i]._miVar2;
	if (id != -1) {
		dir = plr[id]._pdir;
		en = MI_ENEMYMONST;
	} else {
		dir = 0;
		en = MI_ENEMYPLR;
	}

	for (k = 0; k < 23; ++k) {
		if ((sx1 != vCrawlTable[k][RAD]) || (sy1 != vCrawlTable[k][RAD + 1])) {
			dx = sx + vCrawlTable[k][RAD];
			dy = sy + vCrawlTable[k][RAD + 1];
			AddMissile(sx, sy, dx, dy, dir, MIT_FBARROW, en, id, dam, missile[i]._mispllvl);

			dx = sx - vCrawlTable[k][RAD];
			dy = sy - vCrawlTable[k][RAD + 1];
			AddMissile(sx, sy, dx, dy, dir, MIT_FBARROW, en, id, dam, missile[i]._mispllvl);
			
			dx = sx - vCrawlTable[k][RAD];
			dy = sy + vCrawlTable[k][RAD + 1];
			AddMissile(sx, sy, dx, dy, dir, MIT_FBARROW, en, id, dam, missile[i]._mispllvl);

			dx = sx + vCrawlTable[k][RAD];
			dy = sy - vCrawlTable[k][RAD + 1];
			AddMissile(sx, sy, dx, dy, dir, MIT_FBARROW, en, id, dam, missile[i]._mispllvl);

			sx1 = vCrawlTable[k][RAD];
			sy1 = vCrawlTable[k][RAD + 1];
		}
	}

	--missile[i]._mirange;
	if (missile[i]._mirange == 0) missile[i]._miDelFlag = TRUE;
#endif
}

void MI_SpecialArrow(int i)
{
#if (PRE_BETA && PRE_FIREBALL) || !PRE_BETA
	int id, sx, sy, dir, en;
	int sx1, sy1, dam, dx, dy;

	sx1 = sy1 = 0;
	app_assert(i < MAXMISSILES && i >= 0);
	id = missile[i]._misource;
	dam = missile[i]._midam;
	sx = missile[i]._mix;
	sy = missile[i]._miy;
	dx = missile[i]._miVar1;
	dy = missile[i]._miVar2;
	int extraspeed = missile[i]._miVar3;
	
	int mitype = MIT_ARROW;
	if (id != -1) {
		dir = plr[id]._pdir;
		en = MI_ENEMYMONST;
		switch(plr[id]._pILMinDam)
		{
			case 0: mitype = MIT_FBARROW; break;
			case 1: mitype = MIT_LTARROW; break;
			case 2: mitype = MIT_CBARROW; break;
			case 3: mitype = MIT_HBARROW; break;
			default: break;
		}
	} else {
		dir = 0;
		en = MI_ENEMYPLR;
	}
	
	AddMissile(sx, sy, dx, dy, dir, mitype, en, id, dam, extraspeed);
	if (mitype == MIT_CBARROW)
	{
		AddMissile(sx, sy, dx, dy, dir, mitype, en, id, dam, extraspeed);
		AddMissile(sx, sy, dx, dy, dir, mitype, en, id, dam, extraspeed);
	}
	
	--missile[i]._mirange;
	if (missile[i]._mirange == 0) missile[i]._miDelFlag = TRUE;

#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Boil(int i)
{
	app_assert(i < MAXMISSILES && i >= 0);
	missile[i]._miDelFlag = TRUE;
	return;
	
/*#if (PRE_BETA && PRE_BLOODB) || !PRE_BETA
	int j, mid, id, dx, dy, pct;
	long dm;

	BOOL M_Talker (int);

	id = missile[i]._misource;
	dx = missile[i]._miVar1;
	dy = missile[i]._miVar2;
	mid = dMonster[dx][dy];

	if (mid > 0) --mid;
	else mid = -(mid + 1);
	if (mid > 0) {
		if ((M_Talker(mid) && monster[mid].mtalkmsg != 0) || ((plr[id]._pLevel - 6 + missile[i]._mispllvl) <= monster[mid].mLevel)) {
			missile[i]._miDelFlag = TRUE;
			return;
		}

		if (monster[mid].mMagicRes & M_IM) return;

		dm = monster[mid]._mhitpoints;
		monster[mid]._mhitpoints = 0;
		M_StartKill(mid, id);
		AddPlrExperience(id, monster[mid].mLevel, monster[mid].mExp);
		dMonster[monster[mid]._mx][monster[mid]._my] = 0;
		monster[mid]._mDelFlag = TRUE;
		AddMissile(monster[mid]._mx, monster[mid]._my, monster[mid]._mx, monster[mid]._my, plr[id]._pdir, MIT_SPURT, MI_ENEMYMONST, id, (random(87, 2) + 1), 0);
		AddDead(monster[mid]._mx, monster[mid]._my, spurtndx, monster[mid]._mdir);

		pct = 0;
		for (j = 0; j < missile[i]._mispllvl && j < 5; ++j) pct += 10;
		//dm = (monster[mid]._mmaxhp >> 1);
		if (pct > 0) dm -= (dm / pct);
		plr[id]._pHitPoints -= dm;
		plr[id]._pHPBase -= dm;

		if (plr[id]._pHitPoints > plr[id]._pMaxHP) {
			plr[id]._pHitPoints = plr[id]._pMaxHP;
			plr[id]._pHPBase = plr[id]._pMaxHPBase;
		}
		if (plr[id]._pHitPoints <= 0) {
			// rjs - manashld fix? - plr[id]._pHitPoints = 0;
			StartPlrKill(id, FALSE);
		} else StartPlrHit(id, dm);

		PlaySfxLoc(LS_BLODBOIL, monster[mid]._mx, monster[mid]._my);
		UseMana(id, SPL_BLOODB);
	}
	--missile[i]._mirange;
	if (missile[i]._mirange == 0) missile[i]._miDelFlag = TRUE;
#endif*/
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Rage(int i)
{
#if !PRE_BETA
	app_assert(i < MAXMISSILES && i >= 0);
	
	--missile[i]._mirange;
	if (missile[i]._mirange == 0) {
		int const pid = missile[i]._miVar1;
		if ((plr[pid]._pSpellFlags & SF_RAGE) == SF_RAGE) {
			int const Sounds[NUM_CLASSES] = { PS_WARR72,
											  PS_ROGUE72,
											  PS_MAGE72,
											  PS_MAGE72,	// monk
											  PS_BARD72,
											  PS_BARBARIAN72 };
			plr[pid]._pSpellFlags &= ~SF_RAGE;
			plr[pid]._pSpellFlags |= SF_LETHERGY;
			missile[i]._mirange = 245 + (10 * missile[i]._mispllvl) + (2 * (pid > 0) ? plr[pid]._pLevel : 1);
			int const diffHpts = plr[pid]._pMaxHP - plr[pid]._pHitPoints;
			CalcPlrItemVals(pid, TRUE);
			
			plr[pid]._pHitPoints -= diffHpts;
			if (plr[pid]._pHitPoints < (1 << HP_SHIFT)) {
				// Don't quite die.
				plr[pid]._pHitPoints = 1 << HP_SHIFT;
			}
			force_redraw = FULLDRAW;
			PlaySfxLoc(Sounds[plr[pid]._pClass], plr[pid]._px, plr[pid]._py);
		}
		else {
			int const Sounds[NUM_CLASSES] = { PS_WARR72,
											  PS_ROGUE72,
											  PS_MAGE72,
											  PS_MAGE72,	// monk
											  PS_BARD72,
											  PS_BARBARIAN72 };
			missile[i]._miDelFlag = TRUE;
			plr[pid]._pSpellFlags &= ~SF_LETHERGY;
			int const diffHpts = plr[pid]._pMaxHP - plr[pid]._pHitPoints;
			CalcPlrItemVals(pid, TRUE);
			plr[pid]._pHitPoints -= missile[i]._miVar2 + diffHpts;
			if (plr[pid]._pHitPoints < (1 << HP_SHIFT)) {
				// Don't quite die.
				plr[pid]._pHitPoints = 1 << HP_SHIFT;
			}
			force_redraw = FULLDRAW;
			PlaySfxLoc(Sounds[plr[pid]._pClass], plr[pid]._px, plr[pid]._py);
		}
	}
#endif
}
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Flame(int i)
{
#if (PRE_BETA && PRE_FLAME) || !PRE_BETA
	int k, id;

	app_assert(i < MAXMISSILES && i >= 0);
	id = missile[i]._misource;
	--missile[i]._mirange;
	--missile[i]._miVar2;

	k = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy, 0);
	if ((missile[i]._mirange == 0) && (missile[i]._miHitFlag == TRUE)) missile[i]._mirange = k;
	if (missile[i]._miVar2 == 0) missile[i]._miAnimFrame = 20;
	if (missile[i]._miVar2 <= 0) {
		k = missile[i]._miAnimFrame;
		if (k > 11) k = 24 - k;
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, k);
	}

	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	if (missile[i]._miVar2 <= 0) PutMissile(i);
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Flamec(int i)
{
#if (PRE_BETA && PRE_FLAME) || !PRE_BETA
	int id, pn;
	
	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	id = missile[i]._misource;

	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);

	if ((missile[i]._mix != missile[i]._miVar1) || (missile[i]._miy != missile[i]._miVar2)) {
		app_assert(missile[i]._mix < MAXDUNX && missile[i]._mix >= 0);
		app_assert(missile[i]._miy < MAXDUNY && missile[i]._miy >= 0);
		pn = dPiece[missile[i]._mix][missile[i]._miy];
		app_assert(pn <= MAXTILES && pn >= 0); 
		if (nMissileTable[pn] == 0)
			AddMissile(missile[i]._mix, missile[i]._miy, missile[i]._misx, missile[i]._misy, i, MIT_FLAME, missile[i]._micaster, id, missile[i]._miVar3, missile[i]._mispllvl);
		else
			missile[i]._mirange = 0;
		missile[i]._miVar1 = missile[i]._mix;
		missile[i]._miVar2 = missile[i]._miy;
		++missile[i]._miVar3;
	}

	if (missile[i]._mirange == 0 || missile[i]._miVar3 == 3) missile[i]._miDelFlag = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Cbolt(int i)
{
#if (PRE_BETA && PRE_CBOLT) || !PRE_BETA
	int bpath[16] = { -1, 0, 1, -1, 0, 1, -1, -1, 0, 0, 1, 1, 0, 1, -1, 0 };
	int sx, sy, dx, dy, md;

	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	if (missile[i]._miAnimType != MF_LIGHTNING) {
		if (missile[i]._miVar3 == 0) {
			md = (missile[i]._miVar2 + bpath[missile[i]._mirnd]) & 0x07;
			missile[i]._mirnd = (missile[i]._mirnd + 1) & 0x0f;
			sx = missile[i]._mix;
			sy = missile[i]._miy;
			dx = sx + XDirAdd[md];
			dy = sy + YDirAdd[md];
			GetMissileVel(i, sx, sy, dx, dy, 8);
			missile[i]._miVar3 = 16;
		} else --missile[i]._miVar3;

		missile[i]._mitxoff += missile[i]._mixvel;
		missile[i]._mityoff += missile[i]._miyvel;
		GetMissilePos(i);

		CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 0, missile[i]._mix, missile[i]._miy, 0);
		if (missile[i]._miHitFlag == TRUE) {
			missile[i]._miVar1 = 8;
			missile[i]._mimfnum = 0;
			missile[i]._mixoff = 0;
			missile[i]._miyoff = 0;
			SetMissAnim(i, MF_LIGHTNING);
			missile[i]._mirange = missile[i]._miAnimLen;
			GetMissilePos(i);
		}

		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, missile[i]._miVar1);
	}

	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}

	PutMissile(i);
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Hbolt(int i)
{
#if (PRE_BETA && PRE_HBOLT) || !PRE_BETA
	int dam;

	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	if (missile[i]._miAnimType != MF_HEXPL) {
		missile[i]._mitxoff += missile[i]._mixvel;
		missile[i]._mityoff += missile[i]._miyvel;
		
		GetMissilePos(i);

		dam = missile[i]._midam;
		if ((missile[i]._mix != missile[i]._misx) || (missile[i]._miy != missile[i]._misy))
			CheckMissileCol(i, dam, dam, 0, missile[i]._mix, missile[i]._miy, 0);

		if (missile[i]._mirange == 0) {
			missile[i]._mitxoff -= missile[i]._mixvel;
			missile[i]._mityoff -= missile[i]._miyvel;
			GetMissilePos(i);
			missile[i]._mimfnum = 0;
			SetMissAnim(i, MF_HEXPL);
			missile[i]._mirange = missile[i]._miAnimLen - 1;
		} else {
			if ((missile[i]._mix != missile[i]._miVar1) || (missile[i]._miy != missile[i]._miVar2)) {
				missile[i]._miVar1 = missile[i]._mix;
				missile[i]._miVar2 = missile[i]._miy;
				ChangeLight(missile[i]._mlid, missile[i]._miVar1, missile[i]._miVar2, 8);
			}
		}
	} else {
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, 7 + missile[i]._miAnimFrame);
		if (missile[i]._mirange == 0) {
			missile[i]._miDelFlag = TRUE;
			AddUnLight(missile[i]._mlid);
		}
	}
	PutMissile(i);
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Element(int i)
{
#if (PRE_BETA && PRE_ELEMENT) || !PRE_BETA
	int j, mid, sd, dam;
	int cx, cy, px, py, id;

	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	dam = missile[i]._midam;
	id = missile[i]._misource;

	if (missile[i]._miAnimType == MF_BIGEXP) {
		cx = missile[i]._mix;
		cy = missile[i]._miy;
		px = plr[id]._px;
		py = plr[id]._py;
		ChangeLight(missile[i]._mlid, cx, cy, missile[i]._miAnimFrame);
		
		if (CheckBlock(px, py, cx    , cy    ) == 0) CheckMissileCol(i, dam, dam, 1, cx,     cy,     1);
		if (CheckBlock(px, py, cx    , cy + 1) == 0) CheckMissileCol(i, dam, dam, 1, cx,     cy + 1, 1);
		if (CheckBlock(px, py, cx    , cy - 1) == 0) CheckMissileCol(i, dam, dam, 1, cx,     cy - 1, 1);
		if (CheckBlock(px, py, cx + 1, cy    ) == 0) CheckMissileCol(i, dam, dam, 1, cx + 1, cy,     1);
		if (CheckBlock(px, py, cx + 1, cy - 1) == 0) CheckMissileCol(i, dam, dam, 1, cx + 1, cy - 1, 1);
		if (CheckBlock(px, py, cx + 1, cy + 1) == 0) CheckMissileCol(i, dam, dam, 1, cx + 1, cy + 1, 1);
		if (CheckBlock(px, py, cx - 1, cy    ) == 0) CheckMissileCol(i, dam, dam, 1, cx - 1, cy,     1);
		if (CheckBlock(px, py, cx - 1, cy + 1) == 0) CheckMissileCol(i, dam, dam, 1, cx - 1, cy + 1, 1);
		if (CheckBlock(px, py, cx - 1, cy - 1) == 0) CheckMissileCol(i, dam, dam, 1, cx - 1, cy - 1, 1);

		if (missile[i]._mirange == 0) {
			missile[i]._miDelFlag = TRUE;
			AddUnLight(missile[i]._mlid);
		}

		PutMissile(i);
		return;
	}

	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);
	cx = missile[i]._mix;
	cy = missile[i]._miy;

	j = missile[i]._mirange;
	CheckMissileCol(i, dam, dam, 0, cx, cy, 0);
	
	if (missile[i]._miVar3 == 0) {
		//if (missile[i]._mirange == 0 && missile[i]._miHitFlag == TRUE) missile[i]._mirange = j;
		//if ((missile[i]._mirange == 0) && (missile[i]._miHitFlag != TRUE)) missile[i]._miVar3 = 1;
		if ((cx == missile[i]._miVar4) && (cy == missile[i]._miVar5)) missile[i]._miVar3 = 1;
	}

	if (missile[i]._miVar3 == 1) {
		missile[i]._miVar3 = 2;
		missile[i]._mirange = 255;
		mid = FindClosest(cx, cy, 19);
		if (mid > 0) {
			sd = GetDirection8(cx, cy, monster[mid]._mx, monster[mid]._my);
			SetMissDir(i, sd);
			GetMissileVel(i, cx, cy, monster[mid]._mx, monster[mid]._my, 16);
		} else {
			sd = plr[id]._pdir;
			SetMissDir(i, sd);
			GetMissileVel(i, cx, cy, cx + XDirAdd[sd], cy + YDirAdd[sd], 16);
		}
	}

	if ((cx != missile[i]._miVar1) || (cy != missile[i]._miVar2)) {
		missile[i]._miVar1 = cx;
		missile[i]._miVar2 = cy;
		ChangeLight(missile[i]._mlid, cx, cy, 8);
	}

	if (missile[i]._mirange == 0) {
		missile[i]._mimfnum = 0;
		SetMissAnim(i, MF_BIGEXP);
		missile[i]._mirange = missile[i]._miAnimLen - 1;
	}

	PutMissile(i);
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Bonespirit(int i)
{
#if (PRE_BETA && PRE_BONESPIRIT) || !PRE_BETA
	int j, mid, sd, dam;
	int cx, cy, id;

	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	dam = missile[i]._midam;
	id = missile[i]._misource;

	if (missile[i]._mimfnum == 8) {
		cx = missile[i]._mix;
		cy = missile[i]._miy;
		//px = plr[id]._px;
		//py = plr[id]._py;
		ChangeLight(missile[i]._mlid, cx, cy, missile[i]._miAnimFrame);

		if (missile[i]._mirange == 0) {
			missile[i]._miDelFlag = TRUE;
			AddUnLight(missile[i]._mlid);
		}

		PutMissile(i);
		return;
	}

	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);
	cx = missile[i]._mix;
	cy = missile[i]._miy;

	j = missile[i]._mirange;
	CheckMissileCol(i, dam, dam, 0, cx, cy, 0);
	
	if (missile[i]._miVar3 == 0) {
		//if (missile[i]._mirange == 0 && missile[i]._miHitFlag == TRUE) missile[i]._mirange = j;
		//if ((missile[i]._mirange == 0) && (missile[i]._miHitFlag != TRUE)) missile[i]._miVar3 = 1;
		if ((cx == missile[i]._miVar4) && (cy == missile[i]._miVar5)) missile[i]._miVar3 = 1;
	}

	if (missile[i]._miVar3 == 1) {
		missile[i]._miVar3 = 2;
		missile[i]._mirange = 255;
		mid = FindClosest(cx, cy, 19);
		if (mid > 0) {
			missile[i]._midam = (monster[mid]._mhitpoints >> HP_SHIFT) >> 1;
			sd = GetDirection8(cx, cy, monster[mid]._mx, monster[mid]._my);
			SetMissDir(i, sd);
			GetMissileVel(i, cx, cy, monster[mid]._mx, monster[mid]._my, 16);
		} else {
			sd = plr[id]._pdir;
			SetMissDir(i, sd);
			GetMissileVel(i, cx, cy, cx + XDirAdd[sd], cy + YDirAdd[sd], 16);
		}
	}

	if ((cx != missile[i]._miVar1) || (cy != missile[i]._miVar2)) {
		missile[i]._miVar1 = cx;
		missile[i]._miVar2 = cy;
		ChangeLight(missile[i]._mlid, cx, cy, 8);
	}

	if (missile[i]._mirange == 0) {
		SetMissDir(i, 8);
		missile[i]._mirange = 7;
	}

	PutMissile(i);
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_ResurrectBeam(int i)
{
#if (PRE_BETA && PRE_RESURRECT) || !PRE_BETA
	app_assert(i < MAXMISSILES && i >= 0);
	--missile[i]._mirange;
	if (missile[i]._mirange == 0) missile[i]._miDelFlag = TRUE;
	PutMissile(i);
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void MI_Rportal(int i)
{
	int ExpLight[17] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,15,15};

	app_assert(i < MAXMISSILES && i >= 0);
	if (missile[i]._mirange > 1) --missile[i]._mirange;
	
	if (missile[i]._mirange == missile[i]._miVar1) {
		SetMissDir(i, 1);
	}

	if ((currlevel != 0) && (missile[i]._mimfnum != 1) && (missile[i]._mirange != 0)) {
		if (missile[i]._miVar2 == 0) 
			missile[i]._mlid = AddLight(missile[i]._mix, missile[i]._miy, ExpLight[(missile[i]._miVar2)]);
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, ExpLight[(missile[i]._miVar2)]);
		++missile[i]._miVar2;
	}

	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}

	PutMissile(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void ProcessMissiles ()
{
	int i, mi;


	for (i = 0; i < nummissiles; ++i) {
		mi = missileactive[i];
		dFlags[missile[mi]._mix][missile[mi]._miy] &= BFMASK_MISSILE;
		dMissile[missile[mi]._mix][missile[mi]._miy] = 0;
		// Preventative code to prevent going off into the hinderlands.
		if (missile[mi]._mix < 0 || missile[mi]._mix >= (MAXDUNX - 1)||
		    missile[mi]._miy < 0 || missile[mi]._miy >= (MAXDUNY - 1) )
		{
			missile[mi]._miDelFlag = TRUE;
		}
	}

	i = 0;
	while (i < nummissiles) {
		mi = missileactive[i];
		if (missile[mi]._miDelFlag) {
			DeleteMissile(mi, i);
			i = 0;
		} else ++i;
	}

	MissilePreFlag = FALSE;
	ManashieldFlag = 0;
	for (i = 0; i < nummissiles; ++i) {
		mi = missileactive[i];

		missiledata[missile[mi]._mitype].mProc(mi);

		if(!(missile[mi]._miAnimFlags & MFF_STATIC))
		{
			++missile[mi]._miAnimCnt;
			if (missile[mi]._miAnimCnt >= missile[mi]._miAnimDelay) {
				missile[mi]._miAnimCnt = 0;
				missile[mi]._miAnimFrame += missile[mi]._miAnimAdd;
				if (missile[mi]._miAnimFrame > missile[mi]._miAnimLen) missile[mi]._miAnimFrame = 1;
				if (missile[mi]._miAnimFrame < 1) missile[mi]._miAnimFrame = missile[mi]._miAnimLen;
			}
		}
	}
	
	if (ManashieldFlag) {
		for (i = 0; i < nummissiles; ++i) {
			mi = missileactive[i];
			if (missile[mi]._mitype == MIT_MANASHIELD) MI_Manashield(mi);
		}
	}

	i = 0;
	while (i < nummissiles) {
		mi = missileactive[i];
		if (missile[mi]._miDelFlag == TRUE) {
			DeleteMissile(mi, i);
			i = 0;
		} else ++i;
	}
}

void SyncMissAnim()
{
	int i,mi;

	for (i = 0; i < nummissiles; ++i)
	{
		app_assert(i < MAXMISSILES && i >= 0);
		mi = missileactive[i];

		app_assert(mi < MAXMISSILES && mi >= 0);
		MissileStruct *Miss = &missile[mi];

		Miss->_miAnimData = misfiledata[Miss->_miAnimType].mAnimData[Miss->_mimfnum];

		if (Miss->_mitype == MIT_RHINO)
		{
			AnimStruct *anim;

			if (EquivMonst(monster[Miss->_misource].MType->mtype, MT_HORNED))
				anim = &monster[Miss->_misource].MType->Anims[MA_SPECIAL];
			else if (EquivMonst(monster[Miss->_misource].MType->mtype, MT_NSNAKE))
				anim = &monster[Miss->_misource].MType->Anims[MA_ATTACK];
			else
				anim = &monster[Miss->_misource].MType->Anims[MA_WALK];

			missile[mi]._miAnimData = anim->Cels[Miss->_mimfnum];
		}
		// need similar section for Fireman
	}
}

/*-----------------------------------------------------------------------*
** Used when we need to "emergency" delete missiles
**-----------------------------------------------------------------------*/
void ClearMissileSpot(int mi)
{
	app_assert(mi < MAXMISSILES && mi >= 0);
	dFlags[missile[mi]._mix][missile[mi]._miy] &= BFMASK_MISSILE;
	dMissile[missile[mi]._mix][missile[mi]._miy] = 0;
}

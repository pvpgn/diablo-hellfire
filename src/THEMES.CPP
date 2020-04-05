/*-----------------------------------------------------------------------**
** 								Diablo
**
**						 Thematic rooms file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/THEMES.CPP 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"sound.h"
#include	"themes.h"
#include	"gendung.h"
#include	"engine.h"

#include	"objects.h"
#include	"objdat.h"
#include	"monster.h"
#include	"monstdat.h"
#include	"items.h"
#include	"quests.h"
#include	"trigs.h"

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

#define	TOTAL_THEMES		17 

#define THEME_NONE			-1
#define	THEME_BARREL		0
#define THEME_SHRINE		1
#define THEME_MONSTPIT		2
#define THEME_SKELRM		3
#define THEME_TREASURE		4
#define THEME_LIBRARY		5
#define THEME_TORTURE		6
#define THEME_BLOODFTN		7
#define THEME_DECAP			8
#define THEME_PURIFYINGFTN	9
#define THEME_ARMORSTAND	10
#define THEME_GOATSHRINE	11
#define THEME_CAULDRON 		12
#define THEME_MURKYFTN		13
#define THEME_TEARFTN		14
#define	THEME_BRNCROSS		15
#define THEME_WEAPONRACK	16

/*-----------------------------------------------------------------------**
** Global variables
**-----------------------------------------------------------------------*/

ThemeStruct theme[MAXTHEMES];
int numthemes;
int zharlib;

int themex, themey;
int themeVar1;

//Theme flags
BOOL armorFlag;
BOOL bFountainFlag;
BOOL cauldronFlag;
BOOL mFountainFlag;
BOOL pFountainFlag;
BOOL tFountainFlag;
BOOL treasureFlag;
BOOL bCrossFlag;
BOOL weaponFlag;

/*-----------------------------------------------------------------------*/
// List of themes that it trys to fit first

#define THEME_NUMGOOD		4

int ThemeGood[THEME_NUMGOOD] = {
	THEME_GOATSHRINE,
	THEME_SHRINE,
	THEME_SKELRM,
	THEME_LIBRARY,
};

BOOL ThemeGoodIn[THEME_NUMGOOD];

/*-----------------------------------------------------------------------*/

// 5x5 area offsets
int trm5x[] = { -2, -1, 0, 1, 2, -2, -1, 0, 1, 2, -2, -1, 0, 1, 2, -2, -1, 0, 1, 2, -2, -1, 0, 1, 2 };
int trm5y[] = { -2, -2, -2, -2, -2, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2 };

// 3x3 area offsets
int trm3x[] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };
int trm3y[] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL TFit_Shrine(int i)
{
	int xp, yp, found;

	app_assert((DWORD)i < MAXTHEMES);
	xp = 0;
	yp = 0;
	found = 0;
	while (!found) {
		if (dTransVal[xp][yp] == theme[i].ttval) {
			if ((nTrapTable[dPiece[xp][yp-1]]) &&
				(!nSolidTable[dPiece[xp-1][yp]]) &&
				(!nSolidTable[dPiece[xp+1][yp]]) &&
				(dTransVal[xp-1][yp] == theme[i].ttval) &&
				(dTransVal[xp+1][yp] == theme[i].ttval) &&
				(dObject[xp-1][yp-1] == 0) &&
				(dObject[xp+1][yp-1] == 0)) found = 1;
			if (!found) {
				if ((nTrapTable[dPiece[xp-1][yp]]) &&
					(!nSolidTable[dPiece[xp][yp-1]]) &&
					(!nSolidTable[dPiece[xp][yp+1]]) &&
					(dTransVal[xp][yp-1] == theme[i].ttval) &&
					(dTransVal[xp][yp+1] == theme[i].ttval) &&
					(dObject[xp-1][yp-1] == 0) &&
					(dObject[xp-1][yp+1] == 0)) found = 2;
			}
		}
		if (!found) {
			xp++;
			if (xp == DMAXX) {
				xp = 0;
				yp++;
				if (yp == DMAXY) return(FALSE);
			}
		}
	}
	themex = xp;
	themey = yp;
	themeVar1 = found;
	return(TRUE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL TFit_Obj5(int t)
{
	int xp, yp;
	int i, r, rs;
	BOOL found;

	app_assert((DWORD)t < MAXTHEMES);
	xp = 0;
	yp = 0;
	r = random(0, 5) + 1;
	rs = r;
	while (r > 0) {
		found = FALSE;
		if ((dTransVal[xp][yp] == theme[t].ttval) && (!nSolidTable[dPiece[xp][yp]])) {
			found = TRUE;
			for (i = 0; (found && (i < 25)); i++) {
				if (nSolidTable[dPiece[xp+trm5x[i]][yp+trm5y[i]]]) found = FALSE;
				if (dTransVal[xp+trm5x[i]][yp+trm5y[i]] != theme[t].ttval) found = FALSE;
			}
		}
		if (!found) {
			xp++;
			if (xp == DMAXX) {
				xp = 0;
				yp++;
				if (yp == DMAXY) {
					if (r == rs) return(FALSE);
					else yp = 0;
				}
			}
		} else r--;
	}
	themex = xp;
	themey = yp;
	return(TRUE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL TFit_SkelRoom(int t)
{
	int i;

	if (leveltype == 1 || leveltype == 2)
	{
		for (i = 0; i < nummtypes; i++) {
			if (IsSkel(Monsters[i].mtype)) {
				//Initialize themeVar1 to monster type skeleton
				themeVar1 = i;
				return TFit_Obj5(t);
			}
		}
	}

	return(FALSE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL TFit_GoatShrine(int t)
{
	int i;

	for (i = 0; i < nummtypes; i++) {
		if (IsGoat(Monsters[i].mtype)) {
			//Initialize themeVar1 to monster type goat 
			themeVar1 = i;
			return TFit_Obj5(t);
		}
	}

	return(FALSE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL CheckThemeObj3(int xp, int yp, int t, int f)
{
	app_assert((DWORD)t < MAXTHEMES);
	for (int i = 0; i < 9; i++) {
		if (xp+trm3x[i] < 0 || yp+trm3y[i] < 0)
			return FALSE;
		if (nSolidTable[dPiece[xp+trm3x[i]][yp+trm3y[i]]])
			return FALSE;
		if (dTransVal[xp+trm3x[i]][yp+trm3y[i]] != theme[t].ttval)
			return FALSE;
		if (dObject[xp+trm3x[i]][yp+trm3y[i]] != 0)
			return FALSE;
		if (f != -1 && random(0, f) == 0)
			return FALSE;
	}
	return TRUE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL TFit_Obj3(int t)
{
	int xp, yp;
	char objrnd[4] = { 4, 4, 3, 5 };

	for (yp = 1; yp < DMAXY-1; yp++) {
		for (xp = 1; xp < DMAXX-1; xp++) {
			if (CheckThemeObj3(xp, yp, t, objrnd[leveltype-1])) {
				themex = xp;
				themey = yp;
				return TRUE;
			}
		}
	}
	return FALSE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL CheckThemeReqs(int t)
/*-----------------------------------------------------------------------**
** Description: Checks theme room requirements, such as themes for specific
**              levels or themes which can only be used once per level.
** Input: None
** Return: TRUE = All requirements met
**-----------------------------------------------------------------------*/
{
	BOOL	rv = TRUE;

	switch(t) {
		case THEME_SHRINE:
		case THEME_LIBRARY:
			if (leveltype == 3 || leveltype == 4) rv = FALSE;
			break;
		case THEME_SKELRM:
			if (leveltype == 3 || leveltype == 4) rv = FALSE;
			break;
		case THEME_BLOODFTN:
			if (!bFountainFlag)	rv = FALSE;
			break;
		case THEME_PURIFYINGFTN:
			if (!pFountainFlag)	rv = FALSE;
			break;
		case THEME_MURKYFTN:
			if (!mFountainFlag)	rv = FALSE;
			break;
		case THEME_TEARFTN:
			if (!tFountainFlag)	rv = FALSE;
			break;
		case THEME_ARMORSTAND:
		case THEME_WEAPONRACK:
			if (leveltype == 1) rv = FALSE;
			break;
		case THEME_CAULDRON:
			if ((leveltype != 4) || (!cauldronFlag)) rv = FALSE;
			break;
	}

	return rv;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL SpecialThemeFit(int i, int t)
{
	BOOL rv = TRUE;

	rv = CheckThemeReqs(t);

	switch(t) {
		case THEME_SHRINE:
		case THEME_LIBRARY:
			if (rv) rv = TFit_Shrine(i);
			break;
		case THEME_SKELRM:
			if (rv) rv = TFit_SkelRoom(i);
			break;
		case THEME_BLOODFTN:
			if (rv) rv = TFit_Obj5(i);
			//This is set so we only get one blood fountain per level
			if (rv) bFountainFlag = FALSE;
			break;
		case THEME_PURIFYINGFTN:
			if (rv) rv = TFit_Obj5(i);
			//This is set so we only get one purifying fountain per level
			if (rv) pFountainFlag = FALSE;
			break;
		case THEME_MURKYFTN:
			if (rv) rv = TFit_Obj5(i);
			//This is set so we only get one murky fountain per level
			if (rv) mFountainFlag = FALSE;
			break;
		case THEME_TEARFTN:
			if (rv) rv = TFit_Obj5(i);
			//This is set so we only get one tear fountain per level
			if (rv) tFountainFlag = FALSE;
			break;
		case THEME_CAULDRON:
			if (rv) rv = TFit_Obj5(i);
			//This is set so we only get one cauldron room per level
			if (rv) cauldronFlag = FALSE;
			break;
		case THEME_GOATSHRINE:
			if (rv) rv = TFit_GoatShrine(i);
			break;
		case THEME_DECAP:
		case THEME_TORTURE:
		case THEME_ARMORSTAND:
		case THEME_BRNCROSS:
		case THEME_WEAPONRACK:
			if (rv) rv = TFit_Obj3(i);
			break;
		case THEME_TREASURE:
			rv = treasureFlag;
			//This is set so we only get one treasure room per level
			if (rv) treasureFlag = FALSE;
			break;
	}

	return(rv);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL CheckThemeRoom(int tv)
{
	int i, j;
	int tarea;

	for (i = 0; i < numtrigs; i++) {
		if (dTransVal[trigs[i]._tx][trigs[i]._ty] == tv) return(FALSE);
	}

	// Check area requirements
	tarea = 0;
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dTransVal[i][j] == tv) {
				if (dFlags[i][j] & BFLAG_SETPC) return(FALSE);
				tarea++;
			}
		}
	}

	if (leveltype == 1) {
		if ((tarea < 9) || (tarea > 100)) return(FALSE);
	}

	// Make sure it is a solid room
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if ((dTransVal[i][j] == tv) && (!nSolidTable[dPiece[i][j]])) {
				if ((dTransVal[i-1][j] != tv) && (!nSolidTable[dPiece[i-1][j]])) return(FALSE);
				if ((dTransVal[i+1][j] != tv) && (!nSolidTable[dPiece[i+1][j]])) return(FALSE);
				if ((dTransVal[i][j-1] != tv) && (!nSolidTable[dPiece[i][j-1]])) return(FALSE);
				if ((dTransVal[i][j+1] != tv) && (!nSolidTable[dPiece[i][j+1]])) return(FALSE);
			}
		}
	}
	return(TRUE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void InitThemes()
{
	int i, t;

	numthemes = 0;
	zharlib = -1;
	//Initialize theme flags
	armorFlag = TRUE;
	bFountainFlag = TRUE;
	cauldronFlag = TRUE;
	mFountainFlag = TRUE;
	pFountainFlag = TRUE;
	tFountainFlag = TRUE;
	treasureFlag = TRUE;
	bCrossFlag = FALSE;
	weaponFlag = TRUE;

	if (currlevel == 16) return;

	//Find L1 theme room types
	if (leveltype == 1) {
		for (i = 0; i < THEME_NUMGOOD; i++) ThemeGoodIn[i] = FALSE;
		for (i = 0; (i < 256) && (numthemes < MAXTHEMES); i++) {
			if (CheckThemeRoom(i)) {
				theme[numthemes].ttval = i;
				t = ThemeGood[random(0, THEME_NUMGOOD)]; // Normal
				while (!SpecialThemeFit(numthemes, t)) t = random(0, TOTAL_THEMES);
				theme[numthemes].ttype = t;
				numthemes++;
			}
		}
	}

	//Find L2, L3, L4 theme room types
	if (leveltype == 2 || leveltype == 3 || leveltype == 4) 
	{
		//Initialize theme room types
		app_assert((DWORD)themeCount < MAXTHEMES);
		for (i = 0; i < themeCount; i++) {
			theme[i].ttype = THEME_NONE;
		}

		//Special Quest 
		if (QuestStatus(Q_ZHAR)) {
			for (i = 0; i < themeCount; i++) {
				theme[i].ttval = themeLoc[i].ttval;
				if (SpecialThemeFit(i, THEME_LIBRARY)) {
					theme[i].ttype = THEME_LIBRARY;
					zharlib = i;
					break;
				}
			} 
		}

		//Add themeLoc array to general theme array
		for (i = 0; i < themeCount; i++) {
			if (theme[i].ttype == THEME_NONE) {
				theme[i].ttval = themeLoc[i].ttval;
				t = ThemeGood[random(0, THEME_NUMGOOD)];
				while (!SpecialThemeFit(i, t)) t = random(0, TOTAL_THEMES);
				theme[i].ttype = t;
			}
		} 
		//Add L2 theme count to general theme count
		numthemes += themeCount;
	} 
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void HoldThemeRooms()
{
	int i, x, y;
	char tv;

	if (currlevel == 16) return;

	app_assert((DWORD)numthemes < MAXTHEMES);
	if (leveltype == 1) {
		for (i = 0; i < numthemes; i++) {
			tv = theme[i].ttval;
			for (y = 0; y < DMAXY; y++) {
				for (x = 0; x < DMAXX; x++) {
					if (dTransVal[x][y] == tv) dFlags[x][y] |= BFLAG_SETPC;
				}
			}
		}
	} else {
		//Hold L2,L3,L4	
		DRLG_HoldThemeRooms();
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void PlaceThemeMonsts(int t, int f)
{
	int xp, yp, mtype;
	int scattertypes[LASTMT];
	int numscattypes = 0;
	int i;

	app_assert((DWORD)t < MAXTHEMES);
	for (i = 0; i < nummtypes; i++)
	{
		if (Monsters[i].mPlaceFlags & MPFLAG_SCATTER)
			scattertypes[numscattypes++] = i;
	}
	mtype = scattertypes[random(0, numscattypes)];

	for (yp = 0; yp < DMAXY; yp++) {
		for (xp = 0; xp < DMAXX; xp++) {
			if ((dTransVal[xp][yp] == theme[t].ttval) &&
				(!nSolidTable[dPiece[xp][yp]]) &&
				(dItem[xp][yp] == 0) &&
				(dObject[xp][yp] == 0)) {
				if (random(0, f) == 0) AddMonster(xp, yp, random(0, 8), mtype, TRUE);
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void PlaceSpecificThemeMonsts(int t, int f, int mtype)
{
	int xp, yp;

	app_assert((DWORD)t < MAXTHEMES);
	for (yp = 0; yp < DMAXY; yp++) {
		for (xp = 0; xp < DMAXX; xp++) {
			if ((dTransVal[xp][yp] == theme[t].ttval) &&
				(!nSolidTable[dPiece[xp][yp]]) &&
				(dItem[xp][yp] == 0) &&
				(dObject[xp][yp] == 0)) {
				if (random(0, f) == 0) AddMonster(xp, yp, random(0, 8), mtype, TRUE);
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void Theme_Barrel(int t)
{
	int xp, yp, r;
	char barrnd[4] = { 2, 6, 4, 8 };
	char monstrnd[4] = { 5, 7, 3, 9 };

	app_assert((DWORD)t < MAXTHEMES);
	for (yp = 0; yp < DMAXY; yp++) {
		for (xp = 0; xp < DMAXX; xp++) {
			if ((dTransVal[xp][yp] == theme[t].ttval) && (!nSolidTable[dPiece[xp][yp]])) {
				if (!random(0, barrnd[leveltype-1])) {
					if (!random(0, barrnd[leveltype-1])) r = OBJ_BARREL;
					else r = OBJ_BARRELEX;
					AddObject(r, xp, yp);
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void Theme_Shrine(int t)
{
	char monstrnd[4] = { 6, 6, 3, 9 };

	TFit_Shrine(t);
	if (themeVar1 == 1) {
		AddObject(OBJ_CANDLE2, themex-1, themey);
		AddObject(OBJ_SHRINER, themex, themey);
		AddObject(OBJ_CANDLE2, themex+1, themey);
	} else {
		AddObject(OBJ_CANDLE2, themex, themey-1);
		AddObject(OBJ_SHRINEL, themex, themey);
		AddObject(OBJ_CANDLE2, themex, themey+1);
	}
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void Theme_MonstPit(int t)
{
	int r, ixp, iyp;
	char monstrnd[4] = { 6, 7, 3, 9 };

	app_assert((DWORD)t < MAXTHEMES);
	r = random(0, 100)+1;
	ixp = 0;
	iyp = 0;
	while (r > 0) {
		if ((dTransVal[ixp][iyp] == theme[t].ttval) && (!nSolidTable[dPiece[ixp][iyp]])) r--;
		if (r > 0) {
			ixp++;
			if (ixp == DMAXX) {
				ixp = 0;
				iyp++;
				if (iyp == DMAXY) iyp = 0;
			}
		}
	}
	CreateRndItem(ixp, iyp, TRUE, FALSE, TRUE);		// Create a good item
	ItemNoFlippy();
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void Theme_SkelRoom(int t)
{
	int mt;
	int xp, yp;
	int ii;
	char monstrnd[4] = { 6, 7, 3, 9 };

	TFit_SkelRoom(t);
	mt = themeVar1;
	xp = themex;
	yp = themey;

	AddObject(OBJ_SKFIRE, xp, yp);
	if (random(0, monstrnd[leveltype-1])) {
		ii = PreSpawnSkeleton();
		SpawnSkeleton(ii, xp-1, yp-1);
	} else AddObject(OBJ_BANNERL, xp-1, yp-1);
	ii = PreSpawnSkeleton();
	SpawnSkeleton(ii, xp, yp-1);
	if (random(0, monstrnd[leveltype-1])) {
		ii = PreSpawnSkeleton();
		SpawnSkeleton(ii, xp+1, yp-1);
	} else AddObject(OBJ_BANNERR, xp+1, yp-1);
	if (random(0, monstrnd[leveltype-1])) {
		ii = PreSpawnSkeleton();
		SpawnSkeleton(ii, xp-1, yp);
	} else AddObject(OBJ_BANNERM, xp-1, yp);
	if (random(0, monstrnd[leveltype-1])) {
		ii = PreSpawnSkeleton();
		SpawnSkeleton(ii, xp+1, yp);
	} else AddObject(OBJ_BANNERM, xp+1, yp);
	if (random(0, monstrnd[leveltype-1])) {
		ii = PreSpawnSkeleton();
		SpawnSkeleton(ii, xp-1, yp+1);
	} else AddObject(OBJ_BANNERR, xp-1, yp+1);
	ii = PreSpawnSkeleton();
	SpawnSkeleton(ii, xp, yp+1);
	if (random(0, monstrnd[leveltype-1])) {
		ii = PreSpawnSkeleton();
		SpawnSkeleton(ii, xp+1, yp+1);
	} else AddObject(OBJ_BANNERL, xp+1, yp+1);

	if (dObject[xp][yp-3] == 0) AddObject(OBJ_SKELBOOK, xp, yp-2);
	if (dObject[xp][yp+3] == 0) AddObject(OBJ_SKELBOOK, xp, yp+2);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void Theme_Treasure(int t)
{
	int xp, yp, i;
	char treasrnd[4] = { 4, 9, 7, 10 };
	char monstrnd[4] = { 6, 8, 3, 7 };

	app_assert((DWORD)t < MAXTHEMES);
	int rs = GetRndSeed();
	for (yp = 0; yp < DMAXY; yp++) 
	{
		for (xp = 0; xp < DMAXX; xp++) 
		{
			if ((dTransVal[xp][yp] == theme[t].ttval) &&
				(!nSolidTable[dPiece[xp][yp]])) 
			{
				int rv = random(0, treasrnd[leveltype-1]);
				if (!(random(0, treasrnd[leveltype-1])*2)) { 
					CreateTypeItem(xp, yp, FALSE, IT_GOLD, 0, FALSE, TRUE);
					ItemNoFlippy();
				}
				if (rv == 0) {
					CreateRndItem(xp, yp, FALSE, FALSE, TRUE); 
					ItemNoFlippy();
				}
				if ((rv == 0) || (rv >= (treasrnd[leveltype-1]-2))) {
					i = ItemNoFlippy(); 
					if (rv >= (treasrnd[leveltype-1]-2) && leveltype != 1) 
						item[i]._ivalue = item[i]._ivalue >> 1;
				}
			} 
		}
	} 
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void Theme_Library(int t)
{
	int xp, yp, oi;
	char librnd[4] = { 1, 2, 2, 5 };
	char monstrnd[4] = { 5, 7, 3, 9 };

	TFit_Shrine(t);
	if (themeVar1 == 1) {
		AddObject(OBJ_BOOKCANDLE, themex-1, themey);
		AddObject(OBJ_BOOKCASER, themex, themey);
		AddObject(OBJ_BOOKCANDLE, themex+1, themey);
	} else {
		AddObject(OBJ_BOOKCANDLE, themex, themey-1);
		AddObject(OBJ_BOOKCASEL, themex, themey);
		AddObject(OBJ_BOOKCANDLE, themex, themey+1);
	}

	for (yp = 1; yp < DMAXY-1; yp++) {
		for (xp = 1; xp < DMAXX-1; xp++) {
			if (CheckThemeObj3(xp, yp, t, -1) &&
				(dMonster[xp][yp] == 0) &&			// Zhar the mad fix
				(!random(0, librnd[leveltype-1]))) {
				AddObject(OBJ_BOOKSTAND, xp, yp);
				// Pre used?
				if (random(0, (librnd[leveltype-1] << 1))) {
					oi = dObject[xp][yp]-1;
					object[oi]._oSelFlag = OSEL_NONE;
					object[oi]._oAnimFrame += 2;
				}
			}
		}
	}

	//special quest
	if (QuestStatus(Q_ZHAR)) {
		if (t != zharlib) PlaceThemeMonsts(t, monstrnd[leveltype]);
	}
	else PlaceThemeMonsts(t, monstrnd[leveltype]);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void Theme_Torture(int t)
{
	int xp, yp;
	char tortrnd[4] = { 6, 8, 3, 8 };
	char monstrnd[4] = { 6, 8, 3, 9 };

	app_assert((DWORD)t < MAXTHEMES);
	//Add tortured body
	for (yp = 1; yp < DMAXY-1; yp++) {
		for (xp = 1; xp < DMAXX-1; xp++) {
			if ((dTransVal[xp][yp] == theme[t].ttval) && (!nSolidTable[dPiece[xp][yp]])) {
				if (CheckThemeObj3(xp, yp, t, -1) && (!random(0, tortrnd[leveltype-1])))
					AddObject(OBJ_TNUDEM2, xp, yp);
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void Theme_BloodFountain(int t)
{
	char monstrnd[4] = { 6, 8, 3, 9 };

	TFit_Obj5(t);
	AddObject(OBJ_BLOODFTN, themex, themey);
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void Theme_Decap(int t)
{
	int xp, yp;
	char decaprnd[4] = { 6, 8, 3, 8 };
	char monstrnd[4] = { 6, 8, 3, 9 };

	app_assert((DWORD)t < MAXTHEMES);
	//Add decapitated body
	for (yp = 1; yp < DMAXY-1; yp++) {
		for (xp = 1; xp < DMAXX-1; xp++) {
			if ((dTransVal[xp][yp] == theme[t].ttval) && (!nSolidTable[dPiece[xp][yp]])) {
				if (CheckThemeObj3(xp, yp, t, -1) && (!random(0, decaprnd[leveltype-1]))) 
					AddObject(OBJ_DECAP, xp, yp);
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void Theme_PurifyingFountain(int t)
{
	char monstrnd[4] = { 6, 7, 3, 9 };

	TFit_Obj5(t);
	AddObject(OBJ_PURIFYINGFTN, themex, themey);
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void Theme_ArmorStand(int t)
{
	int xp, yp;
	char armorrnd[4] = { 6, 8, 3, 8 };
	char monstrnd[4] = { 6, 7, 3, 9 };

	app_assert((DWORD)t < MAXTHEMES);
	//Add functional stand w/ armor
	if(armorFlag) {
		TFit_Obj3(t); 
		AddObject(OBJ_ARMORSTAND, themex, themey);
	}
	//Add stand only
	for (yp = 0; yp < DMAXY; yp++) {
		for (xp = 0; xp < DMAXX; xp++) {
			if ((dTransVal[xp][yp] == theme[t].ttval) && (!nSolidTable[dPiece[xp][yp]])) {
				if (CheckThemeObj3(xp, yp, t, -1) && (!random(0, armorrnd[leveltype-1])))
					AddObject(OBJ_ARMORSTANDN, xp, yp);
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
	//This is set so we only get one functional armor stand per level
	armorFlag = FALSE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void Theme_GoatShrine(int t)
{
	int xx, yy;

	app_assert((DWORD)t < MAXTHEMES);
	TFit_GoatShrine(t);
	AddObject(OBJ_GOATSHRINE, themex, themey);
	//Add goat monsters around shrine	
	for (yy = themey-1; yy <= themey+1; yy++) {
		for (xx = themex-1; xx <= themex+1; xx++) {
			if ((dTransVal[xx][yy] == theme[t].ttval) && 
				 (!nSolidTable[dPiece[xx][yy]])) {
				//Skip center monster since shrine is in the center
				if (xx == themex && yy == themey) continue;
				else AddMonster(xx, yy, 1, themeVar1, TRUE);
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void Theme_Cauldron(int t)
{
	char monstrnd[4] = { 6, 7, 3, 9 };

	TFit_Obj5(t);
	AddObject(OBJ_CAULDRON, themex, themey);
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void Theme_MurkyFountain(int t)
{
	char monstrnd[4] = { 6, 7, 3, 9 };

	TFit_Obj5(t);
	AddObject(OBJ_MURKYFTN, themex, themey);
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void Theme_TearFountain(int t)
{
	char monstrnd[4] = { 6, 7, 3, 9 };

	TFit_Obj5(t);
	AddObject(OBJ_TEARFTN, themex, themey);
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void Theme_BrnCross(int t)
{
	int	xp, yp;

	char monstrnd[4] = { 6, 8, 3, 9 };
	char bcrossrnd[4] = { 5, 7, 3, 8 };

	app_assert((DWORD)t < MAXTHEMES);
	for (yp = 0; yp < DMAXY; yp++) {
		for (xp = 0; xp < DMAXX; xp++) {
			if ((dTransVal[xp][yp] == theme[t].ttval) && (!nSolidTable[dPiece[xp][yp]])) {
				if (CheckThemeObj3(xp, yp, t, -1) && (!random(0, bcrossrnd[leveltype-1])))
					AddObject(OBJ_TBCROSS, xp, yp);
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
	//Set so we know a burning cross theme room has been created on the level.
	bCrossFlag = TRUE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void Theme_WeaponRack(int t)
{
	int xp, yp;
	char weaponrnd[4] = { 6, 8, 5, 8 };
	char monstrnd[4] = { 6, 7, 3, 9 };

	app_assert((DWORD)t < MAXTHEMES);
	//Add functional rack w/ weapon
	if(weaponFlag) {
		TFit_Obj3(t); 
		AddObject(OBJ_WEAPONRACK, themex, themey);
	}
	//Add rack only
	for (yp = 0; yp < DMAXY; yp++) {
		for (xp = 0; xp < DMAXX; xp++) {
			if ((dTransVal[xp][yp] == theme[t].ttval) && (!nSolidTable[dPiece[xp][yp]])) {
				if (CheckThemeObj3(xp, yp, t, -1) && (!random(0, weaponrnd[leveltype-1])))
					AddObject(OBJ_WEAPONRACKN, xp, yp);
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
	//This is set so we only get one functional weapon rack per level
	weaponFlag = FALSE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void UpdateL4Trans()
{
	int	i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dTransVal[i][j] != 0)
				dTransVal[i][j] = 1;
		} 
	} 
} 

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void CreateThemeRooms()
{
	int i;
	
	if (currlevel == 16) return;

	InitObjFlag = TRUE;
	app_assert((DWORD)numthemes < MAXTHEMES);
	for (i = 0; i < numthemes; i++) {
		themex = 0;
		themey = 0;
		switch(theme[i].ttype) {
			case THEME_BARREL:
				Theme_Barrel(i);
				break;
			case THEME_SHRINE:
				Theme_Shrine(i);
				break;
			case THEME_MONSTPIT:
				Theme_MonstPit(i);
				break;
			case THEME_SKELRM:
				Theme_SkelRoom(i);
				break;
			case THEME_TREASURE:
				Theme_Treasure(i);
				break;
			case THEME_LIBRARY:
				Theme_Library(i);
				break;
			case THEME_TORTURE:
				Theme_Torture(i);
				break;
			case THEME_BLOODFTN:
				Theme_BloodFountain(i);
				break;
			case THEME_DECAP:
				Theme_Decap(i);
				break;
			case THEME_PURIFYINGFTN:
				Theme_PurifyingFountain(i);
				break;
			case THEME_ARMORSTAND:
				Theme_ArmorStand(i);
				break;
			case THEME_GOATSHRINE:
				Theme_GoatShrine(i);
				break;
			case THEME_CAULDRON:
				Theme_Cauldron(i);
				break;
			case THEME_TEARFTN:
				Theme_TearFountain(i);
				break;
			case THEME_MURKYFTN:
				Theme_MurkyFountain(i);
				break;
			case THEME_BRNCROSS:
				Theme_BrnCross(i);
				break;
			case THEME_WEAPONRACK:
				Theme_WeaponRack(i);
				break;
		}
	}
	InitObjFlag = FALSE;
	
	//Update L4 trans values to get rid of blinking
	if (leveltype == 4 && themeCount > 0) UpdateL4Trans();
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/



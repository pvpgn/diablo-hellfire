/*-----------------------------------------------------------------------**
** 								Diablo
**
**							Debugging file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/DEBUG.CPP 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"debug.h"
#include	"engine.h"
#include	"error.h"

#include	"items.h"
#include	"gendung.h"
#include	"player.h"


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
BYTE *pSquareCel;


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void InitDebugGFX() {
	app_assert(! pSquareCel);
	if (visiondebug)
		pSquareCel = LoadFileInMemSig("Data\\Square.CEL",NULL,'DBGS');
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void FreeDebugGFX() {
	DiabloFreePtr(pSquareCel);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#define DEBUGSEEDS	4096

int debugseeds[DEBUGSEEDS];
//int seedcnt, seedidx[17];
int seedcnt, seedidx[NUMLEVELS+1];	// JKE to add crypt
BOOL seedflag = FALSE;

void InitDebugSeeds()
{
	for (int i = 0; i < DEBUGSEEDS; i++) debugseeds[i] = -1;
	seedcnt = 0;
	for (i = 0; i < 17; i++) seedidx[i] = 0;
}

void StartDebugSeeds()
{
	if (currlevel == 0) return;
	seedcnt = seedidx[currlevel];
	seedflag = TRUE;
}

void EndDebugSeeds()
{
	if (currlevel == 0) return;
	seedidx[currlevel+1] = seedcnt;
	seedflag = FALSE;
}

void SaveDebugSeed(int s)
{
	if (!seedflag) return;
	if (seedcnt == DEBUGSEEDS) return;
	if (currlevel == 0) return;
	if (debugseeds[seedcnt] == -1) {
		debugseeds[seedcnt] = s;
	} else {
		if (debugseeds[seedcnt] != s) app_fatal("Seeds desynced");
	}
	seedcnt++;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

#if 0
char davehold1[5][MAXDUNX][MAXDUNY];
char davehold2[5][MAXDUNX][MAXDUNY];
BOOL daveinited[5] = { FALSE, FALSE, FALSE, FALSE, FALSE };

#define LEVELCHECK		1

void DaveCheck()
{
	int xp, yp;

	//if (currlevel != LEVELCHECK) return;
	if (currlevel == 0) return;
	if (currlevel >= 5) return;
	for (yp = 0; yp < MAXDUNY; yp++) {
		for (xp = 0; xp < MAXDUNX; xp++) {
			if (dMonster[xp][yp] != 0) app_fatal("Monsters not cleared");
			if (dPlayer[xp][yp] != 0) app_fatal("Players not cleared");
			if (daveinited[currlevel]) {
				if (davehold1[currlevel][xp][yp] != (dFlags[xp][yp] & BFLAG_MONSTACTIVE))
					app_fatal("MonstActive not same");
				if (davehold2[currlevel][xp][yp] != (dFlags[xp][yp] & BFLAG_SETPC))
					app_fatal("Set Piece not same");
			} else {
				davehold1[currlevel][xp][yp] = dFlags[xp][yp] & BFLAG_MONSTACTIVE;
				davehold2[currlevel][xp][yp] = dFlags[xp][yp] & BFLAG_SETPC;
			}
		}
	}
	daveinited[currlevel] = TRUE;
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

#if 1
#include "msg.h"
int blipplr = 0;

void BlipDebug(BOOL next)
{
	if (next) blipplr = (blipplr + 1) & 0x3;

	int i = blipplr;
	char tempstr[128];

	sprintf(tempstr, "Plr %i : Active = %i", i, plr[i].plractive);
	NetSendString((1 << myplr), tempstr);
	if (plr[i].plractive) {
		sprintf(tempstr, "  Plr %i is %s", i, plr[i]._pName);
		NetSendString((1 << myplr), tempstr);
		sprintf(tempstr, "  Lvl = %i : Change = %i", plr[i].plrlevel, plr[i]._pLvlChanging);
		NetSendString((1 << myplr), tempstr);
		sprintf(tempstr, "  x = %i, y = %i : tx = %i, ty = %i : fx = %i, fy = %i",
			plr[i]._px, plr[i]._py, plr[i]._ptargx, plr[i]._ptargy, plr[i]._pfutx, plr[i]._pfuty);
		NetSendString((1 << myplr), tempstr);
		sprintf(tempstr, "  mode = %i : daction = %i : walk[0] = %i", plr[i]._pmode, plr[i].destAction, plr[i].walkpath[0]);
		NetSendString((1 << myplr), tempstr);
		sprintf(tempstr, "  inv = %i : hp = %i", plr[i]._pInvincible, plr[i]._pHitPoints);
		NetSendString((1 << myplr), tempstr);
	}
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

#if 1
#include "quests.h"
int currqdebug = 0;

void PrintQuestDebug()
{
	char tempstr[128];

	sprintf(tempstr, "Quest %i :  Active = %i, Var1 = %i", currqdebug, quests[currqdebug]._qactive, quests[currqdebug]._qvar1);
	NetSendString((1 << myplr),tempstr);
	currqdebug++;
	if (currqdebug == MAXQUESTS) currqdebug = 0;
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

#if 1
#include	"monstint.h"
#include	"themes.h"
#include	"drlg_l4.h"
extern byte dung[L4DUNX][L4DUNY];
extern byte L4dungeon[L4DX][L4DY];

char mphold1[NUMLEVELS+1][MAXDUNX][MAXDUNY];
char mphold2[NUMLEVELS+1][MAXDUNX][MAXDUNY];

void DaveCheck2()
{
	int xp, yp;

	for (yp = 0; yp < MAXDUNY; yp++) {
		for (xp = 0; xp < MAXDUNX; xp++) {
			if (dMonster[xp][yp] != 0) app_fatal("Monsters not cleared");
			if (dPlayer[xp][yp] != 0) app_fatal("Players not cleared");
			mphold1[currlevel][xp][yp] = dFlags[xp][yp] & BFLAG_MONSTACTIVE;
			mphold2[currlevel][xp][yp] = dFlags[xp][yp] & BFLAG_SETPC;
		}
	}
}

/*-----------------------------------------------------------------------*/

void PrintDaveCheck2()
{
	int i, j, xp, yp, sum1, sum2, sum3;
	long fv;
	char tempstr[128];

	sum1 = 0;
	sum2 = 0;
	for (yp = 0; yp < MAXDUNY; yp++) {
		for (xp = 0; xp < MAXDUNX; xp++) {
			sum1 += mphold1[currlevel][xp][yp];
			sum2 += mphold2[currlevel][xp][yp];
		}
	}
	sprintf(tempstr, "Level %i : Monst Active Sum = %i : dFlag sum = %i", currlevel, sum1, sum2);
	NetSendString((1 << myplr),tempstr);

	sum1 = 0;
	sum2 = 0;
	for (i = 1; i <= MAXTILES; i++) {
		if (nSolidTable[i]) {
			sum1 += i;
			sum2++;
		}
	}
	
	// Calc a volume of monsters
	fv = 0;
	for (i = DIRTEDGED2; i < (DMAXY - (DIRTEDGED2)); i++) {
		for (j = DIRTEDGED2; j < (DMAXX - (DIRTEDGED2)); j++) {
			if (!SolidLoc(i,j)) fv++;
		}
	}

	sum3 = 0;
	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) sum3 += dungeon[i][j];
	}

	sprintf(tempstr, "Solid Sum = %i:%i : Monst Vol = %i : Dungeon Sum = %i", sum1, sum2, fv, sum3);
	NetSendString((1 << myplr),tempstr);

	sum1 = 0;
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			sum1 += dTransVal[i][j];
		}
	}

	sum2 = 0;
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			sum2 += dPiece[i][j];
		}
	}

	sprintf(tempstr, "Num themes = %i/%i : Trans Sum = %i : dPiece Sum = %i", numthemes, themeCount, sum1, sum2);
	NetSendString((1 << myplr),tempstr);

	if (leveltype == 4) {
		sum1 = 0;
		for (j = 0; j < L4DUNY; j++) {
			for (i = 0; i < L4DUNX; i++) sum1 += dung[i][j];
		}
		sum2 = 0;
		for (j = 0; j < L4DY; j++) {
			for (i = 0; i < L4DX; i++) sum2 += L4dungeon[i][j];
		}
		sprintf(tempstr, "dung sum = %i : L4Dungeon sum = %i", sum1, sum2);
	}
}

/*-----------------------------------------------------------------------*/

int dungdebugy = 0;

void DaveDungDebug()
{
	int sum, i;
	char tempstr[128];

	sum = 0;
	for (i = 0; i < MDMAXX; i++) sum += dungeon[i][dungdebugy];

	sprintf(tempstr, "dungeon Y=%i sum = %i", dungdebugy, sum);
	NetSendString((1 << myplr), tempstr);

	dungdebugy++;
	if (dungdebugy == MDMAXY) dungdebugy = 0;
}
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

#if 1
#include	"sound.h"
#include	"monster.h"
#include	"monstdat.h"
#include	"cursor.h"

int debugmonst = 0;

void PrintDaveMonst(int m)
{
	char tempstr[128];
	int inlist, i;

	sprintf(tempstr, "Monster %i = %s", m, monster[m].mName);
	NetSendString((1 << myplr), tempstr);
	sprintf(tempstr, "X = %i, Y = %i", monster[m]._mx, monster[m]._my);
	NetSendString((1 << myplr), tempstr);
	sprintf(tempstr, "Enemy = %i, HP = %i", monster[m]._menemy, monster[m]._mhitpoints);
	NetSendString((1 << myplr), tempstr);
	sprintf(tempstr, "Mode = %i, Var1 = %i", monster[m]._mmode, monster[m]._mVar1);
	NetSendString((1 << myplr), tempstr);
	inlist = 0;
	for (i = 0; i < nummonsters; i++) {
		if (monstactive[i] == m) inlist = 1;
	}
	sprintf(tempstr, "Active List = %i, Squelch = %i", inlist, monster[m]._msquelch);
	NetSendString((1 << myplr), tempstr);
}

void DaveDebugMonst()
{
	int cm;

	if (cursmonst == -1) {
		if (dMonster[cursmx][cursmy] == 0) cm = debugmonst;
		else {
			if (dMonster[cursmx][cursmy] > 0) cm = dMonster[cursmx][cursmy] - 1;
			else cm = -(dMonster[cursmx][cursmy] + 1);
		}
	} else cm = cursmonst;

	PrintDaveMonst(cm);
}

void DaveDebugMonst2()
{
	char tempstr[128];
	debugmonst++;
	if (debugmonst == MAXMONSTERS) debugmonst = 0;
	sprintf(tempstr, "Current debug monster = %i", debugmonst);
	NetSendString((1 << myplr), tempstr);
}

#endif

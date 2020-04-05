/*-----------------------------------------------------------------------**
** 								Diablo
**
**							 Dungeon file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/DRLG_L4.CPP 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------**
**
** 							 File Routines
** CreateL1Dungeon
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"sound.h"
#include	"drlg_l4.h"
#include	"gendung.h"
#include	"scrollrt.h"
#include	"engine.h"
#include	"trigs.h"
#include	"lighting.h"
#include	"quests.h"
#include	"multi.h"

/*-----------------------------------------------------------------------*
** Diablo quads
**-----------------------------------------------------------------------*/

#define DIABSIZE	14

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
byte dung[L4DUNX][L4DUNY];
BOOL hallok[L4DUNY];
byte L4dungeon[L4DX][L4DY];
static const byte L4ConvTbl[16] = { 30, 6, 1, 6, 2, 6, 6, 6, 9, 6, 1, 6, 2, 6, 3, 6 };
int SP4x1, SP4y1, SP4x2, SP4y2;
int l4holdx,l4holdy;

int diabquad1x, diabquad2x, diabquad3x, diabquad4x;
int diabquad1y, diabquad2y, diabquad3y, diabquad4y;

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static const byte L4USTAIRS[] = { 4, 5,													// X size, Y size

					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,

					 0,  0,  0,  0,
					36, 38, 35,  0,
					37, 34, 33, 32,
					 0,  0, 31,  0,
					 0,  0,  0,  0};

static const byte L4TWARP[] = { 4, 5,													// X size, Y size

					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,

					  0,   0,   0,   0,
					134, 136, 133,   0,
					135, 132, 131, 130,
					  0,   0, 129,   0,
					  0,   0,   0,   0};

static const byte L4DSTAIRS[] = { 5, 5,													// X size, Y size

					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
	
					  0,  0,  0,  0,  0,
					  0,  0, 45, 41,  0,
					  0, 44, 43, 40,  0,
					  0, 46, 42, 39,  0,
					  0,  0,  0,  0,  0};

static const byte L4PENTA[] = { 5, 5,													// X size, Y size

					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,

					0,   0,   0,   0, 0,
					0,  98, 100, 103, 0,
					0,  99, 102, 105, 0,
					0, 101, 104, 106, 0,
					0,   0,   0,   0, 0};

static const byte L4PENTA2[] = { 5, 5,													// X size, Y size

					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,
					FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC, FLOOR_PIC,

					0,   0,   0,   0, 0,
					0, 107, 109, 112, 0,
					0, 108, 111, 114, 0,
					0, 110, 113, 115, 0,
					0,   0,   0,   0, 0};

// Types for tile substitution
static const byte L4BTYPES[NUMBBLOCKS] = { 0,
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,				// L4Base
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//30							// L4Dirt
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//46						// L4Stairs
		0, 0, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0,//60							// L4Shadow
		1, 2, 1, 2, 1, 2, 1, 1, 2, 2,//70										// L4Misc
		0, 0, 0, 0, 0, 0, 15, 16, 9, 12, 4, 5, 7, //83							// L4Misc2
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//97							// L4Floor
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//115				// L4Penta
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//128								// L4Dirt2
		0, 0, 0, 0, 0, 0, 0, 0};												// L4Stair2

																					
/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L4Shadows()
{
	int		x,y;
	BOOL okflag;

	for (y = 1; y < MDMAXY; y++) {
		for (x = 1; x < MDMAXX; x++) {
			okflag = FALSE;
			if (dungeon[x][y] == 3) okflag = TRUE;
			if (dungeon[x][y] == 4) okflag = TRUE;
			if (dungeon[x][y] == 8) okflag = TRUE;
			if (dungeon[x][y] == 15) okflag = TRUE;
			if (okflag) {
				if (dungeon[x-1][y] == 6) dungeon[x-1][y] = 47;
				if (dungeon[x-1][y-1] == 6) dungeon[x-1][y-1] = 48;
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void InitL4Dungeon()
{
	ZeroMemory(dung,sizeof(dung));
	ZeroMemory(L4dungeon,sizeof(L4dungeon));

	for (int j = 0; j < MDMAXY; j++) {
		for (int i = 0; i < MDMAXX; i++) {
			dungeon[i][j] = 30;	//L4_DIRT;DIRT_PIC
			dflags[i][j] = 0;
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DRLG_LoadL4SP()
{
	setloadflag = FALSE;
	if (QuestStatus(Q_WARLORD)) {
		app_assert(gbMaxPlayers == 1);
		pSetPiece = LoadFileInMemSig("Levels\\L4Data\\Warlord.DUN",NULL,'STPC');
		setloadflag = TRUE;
	}
	if ((currlevel == 15) && (gbMaxPlayers != 1)) {
		pSetPiece = LoadFileInMemSig("Levels\\L4Data\\Vile1.DUN", NULL, 'STPC');
		setloadflag = TRUE;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void DRLG_FreeL4SP() {
	DiabloFreePtr(pSetPiece);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void DRLG_L4SetSPRoom(int rx1, int ry1)
{
	int		rw,rh;
	int		i,j;
	byte	*sp;

	sp = pSetPiece;
	rw = *sp;
	sp+=2;
	rh = *sp;
	sp+=2;

	setpc_x = rx1;
	setpc_y = ry1;
	setpc_w = rw;
	setpc_h = rh;

	sp = pSetPiece+4;
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp != 0) {
				dungeon[rx1+i][ry1+j] = *sp;
				dflags[rx1+i][ry1+j] |= SETP_BIT;
			} else dungeon[rx1+i][ry1+j] = FLOOR_PIC;
			sp+=2;
		}
	}

 }

/*------------------------------------------------------------------------*
**------------------------------------------------------------------------*/
static void L4makeDmt()
{
	int i, j;
	int idx;
	int val;
	int dmtx, dmty;

	dmty = 0;
	for (j = 1; j <= 77; j+=2) {
		dmtx = 0;
		for (i = 1; i <= 77; i+=2) {
			idx = L4dungeon[i][j] + (L4dungeon[i+1][j]<<1) +
				(L4dungeon[i][j+1]<<2) + (L4dungeon[i+1][j+1]<<3);
			val = L4ConvTbl[idx];
			dungeon[dmtx][dmty] = val;
			dmtx++;
		}
		dmty++;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static int L4HWallOk(int i, int j)
{
	int x;
	BOOL wallok;

	x = 1;
	while ((dungeon[i+x][j] == 6) &&
			(dflags[i+x][j] == 0) &&
			(dungeon[i+x][j-1] == 6) &&
			(dungeon[i+x][j+1] == 6)) x++;
	wallok = FALSE;
	if (dungeon[i+x][j] == 10) wallok = TRUE;
	if (dungeon[i+x][j] == 12) wallok = TRUE;
	if (dungeon[i+x][j] == 13) wallok = TRUE;
	if (dungeon[i+x][j] == 15) wallok = TRUE;
	if (dungeon[i+x][j] == 16) wallok = TRUE;
	if (dungeon[i+x][j] == 21) wallok = TRUE;
	if (dungeon[i+x][j] == 22) wallok = TRUE;
	if (x <= 3) wallok = FALSE;
	if (wallok) return(x);
	else return(-1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static int L4VWallOk(int i, int j)
{
	int y;
	BOOL wallok;

	y = 1;
	while ((dungeon[i][j+y] == 6) &&
			(dflags[i][j+y] == 0) &&
			(dungeon[i-1][j+y] == 6) &&
			(dungeon[i+1][j+y] == 6)) y++;
	wallok = FALSE;
	if (dungeon[i][j+y] ==   8) wallok = TRUE;
	if (dungeon[i][j+y] ==   9) wallok = TRUE;
	if (dungeon[i][j+y] ==  11) wallok = TRUE;
	if (dungeon[i][j+y] ==  14) wallok = TRUE;
	if (dungeon[i][j+y] ==  15) wallok = TRUE;
	if (dungeon[i][j+y] ==  16) wallok = TRUE;
	if (dungeon[i][j+y] ==  21) wallok = TRUE;
	if (dungeon[i][j+y] ==  23) wallok = TRUE;
	if (y <= 3) wallok = FALSE;
	if (wallok) return(y);
	else return(-1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void L4HorizWall(int i, int j, int dx)
{
	int xx;
	
	// Left side connector
	if (dungeon[i][j] == 13) dungeon[i][j] = 17;
	if (dungeon[i][j] == 16) dungeon[i][j] = 11;
	if (dungeon[i][j] == 12) dungeon[i][j] = 14;
	// Middle fill
	for (xx = 1; xx < dx; xx++) dungeon[i+xx][j] = 2;
	// Right side connector
	if (dungeon[i+dx][j] == 15) dungeon[i+dx][j] = 14;
	if (dungeon[i+dx][j] == 10) dungeon[i+dx][j] = 17;
	if (dungeon[i+dx][j] == 21) dungeon[i+dx][j] = 23;
	if (dungeon[i+dx][j] == 22) dungeon[i+dx][j] = 29;
	// Put in arch
	xx = random(0, dx-3) + 1;
	dungeon[i+xx][j] = 57;
	dungeon[i+xx+2][j] = 56;
	// Arch shadows
	dungeon[i+xx+1][j] = 60;
	if (dungeon[i+xx][j-1] == 6) dungeon[i+xx][j-1] = 58;
	if (dungeon[i+xx+1][j-1] == 6) dungeon[i+xx+1][j-1] = 59;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void L4VertWall(int i, int j, int dy)
{
	int yy;

	//Up side connector;
	if (dungeon[i][j] == 14) dungeon[i][j] = 17;
	if (dungeon[i][j] ==  8) dungeon[i][j] =  9;
	if (dungeon[i][j] == 15) dungeon[i][j] = 10;
	//Middle fill
	for (yy = 1; yy < dy; yy++) dungeon[i][j+yy] = 1; 
	//Down side connector
	if (dungeon[i][j+dy] == 11) dungeon[i][j+dy] = 17;
	if (dungeon[i][j+dy] ==  9) dungeon[i][j+dy] = 10;
	if (dungeon[i][j+dy] == 16) dungeon[i][j+dy] = 13;
	if (dungeon[i][j+dy] == 21) dungeon[i][j+dy] = 22;
	if (dungeon[i][j+dy] == 23) dungeon[i][j+dy] = 29;
	// Put in arch
	yy = random(0, dy-3) + 1;
	dungeon[i][j+yy] = 53;
	dungeon[i][j+yy+2] = 52;
	// Arch shadows
	dungeon[i][j+yy+1] = 6;
	if (dungeon[i-1][j+yy] == 6) dungeon[i-1][j+yy] = 54;
	if (dungeon[i-1][j+yy-1] == 6) dungeon[i-1][j+yy-1] = 55;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#define WALLRND 100
static void L4AddWall()
{ 
	int i, j;
	int x, y;
	
	for (j = 0; j < MDMAXY ; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if (dflags[i][j] == 0) {
				// Horizontal
				if ((dungeon[i][j] == 10) && (random(0, 100) < WALLRND)) {
					x = L4HWallOk(i, j);
					if (x != -1) L4HorizWall(i, j, x);
				}
				if ((dungeon[i][j] == 12) && (random(0, 100) < WALLRND)) {
					x = L4HWallOk(i, j);
					if (x != -1) L4HorizWall(i, j, x);
				}
				if ((dungeon[i][j] == 13) && (random(0, 100) < WALLRND)) {
					x = L4HWallOk(i, j);
					if (x != -1) L4HorizWall(i, j, x);
				}
				if ((dungeon[i][j] == 15) && (random(0, 100) < WALLRND)) {
					x = L4HWallOk(i, j);
					if (x != -1) L4HorizWall(i, j, x);
				}
				if ((dungeon[i][j] == 16) && (random(0, 100) < WALLRND)) {
					x = L4HWallOk(i, j);
					if (x != -1) L4HorizWall(i, j, x);
				}
				if ((dungeon[i][j] == 21) && (random(0, 100) < WALLRND)) {
					x = L4HWallOk(i, j);
					if (x != -1) L4HorizWall(i, j, x);
				}
				if ((dungeon[i][j] == 22) && (random(0, 100) < WALLRND)) {
					x = L4HWallOk(i, j);
					if (x != -1) L4HorizWall(i, j, x);
				}
				// Vertical
				if ((dungeon[i][j] ==  8) && (random(0, 100) < WALLRND)) {
					y = L4VWallOk(i, j);
					if (y != -1) L4VertWall(i, j, y);
				}
				if ((dungeon[i][j] ==  9) && (random(0, 100) < WALLRND)) {
					y = L4VWallOk(i, j);
					if (y != -1) L4VertWall(i, j, y);
				}
				if ((dungeon[i][j] == 11) && (random(0, 100) < WALLRND)) {
					y = L4VWallOk(i, j);
					if (y != -1) L4VertWall(i, j, y);
				}
				if ((dungeon[i][j] == 14) && (random(0, 100) < WALLRND)) {
					y = L4VWallOk(i, j);
					if (y != -1) L4VertWall(i, j, y);
				}
				if ((dungeon[i][j] == 15) && (random(0, 100) < WALLRND)) {
					y = L4VWallOk(i, j);
					if (y != -1) L4VertWall(i, j, y);
				}
				if ((dungeon[i][j] == 16) && (random(0, 100) < WALLRND)) {
					y = L4VWallOk(i, j);
					if (y != -1) L4VertWall(i, j, y);
				}
				if ((dungeon[i][j] == 21) && (random(0, 100) < WALLRND)) {
					y = L4VWallOk(i, j);
					if (y != -1) L4VertWall(i, j, y);
				}
				if ((dungeon[i][j] == 23) && (random(0, 100) < WALLRND)) {
					y = L4VWallOk(i, j);
					if (y != -1) L4VertWall(i, j, y);
				}
			}
		}
	}
}

/*------------------------------------------------------------------------*
**------------------------------------------------------------------------*/
static void L4tileFix()
{
	int i, j;
	// tiles in middle of dungeon
	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] ==  6)) dungeon[i+1][j] =  5;
			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] ==  1)) dungeon[i+1][j] = 13;

			if ((dungeon[i][j] ==  1) && (dungeon[i][j+1] ==  2)) dungeon[i][j+1] = 14;
		}
	}

	// tiles in middle of dungeon
	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] ==  6)) dungeon[i+1][j] =  2;
			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] ==  9)) dungeon[i+1][j] = 11;
			if ((dungeon[i][j] ==  9) && (dungeon[i+1][j] ==  6)) dungeon[i+1][j] = 12;
			if ((dungeon[i][j] == 14) && (dungeon[i+1][j] ==  1)) dungeon[i+1][j] = 13;
			if ((dungeon[i][j] ==  6) && (dungeon[i+1][j] == 14)) dungeon[i+1][j] = 15;

			if ((dungeon[i][j] ==  6) && (dungeon[i][j+1] == 13)) dungeon[i][j+1] = 16;
			if ((dungeon[i][j] ==  1) && (dungeon[i][j+1] ==  9)) dungeon[i][j+1] = 10;

			if ((dungeon[i][j] ==  6) && (dungeon[i][j-1] ==  1)) dungeon[i][j-1] =  1;
		}	
	}
	
	// around edges
	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] == 13) && (dungeon[i][j+1] == 30)) dungeon[i][j+1] = 27;

			if ((dungeon[i][j] == 27) && (dungeon[i+1][j] == 30)) dungeon[i+1][j] = 19;
			if ((dungeon[i][j] ==  1) && (dungeon[i][j+1] == 30)) dungeon[i][j+1] = 27;
			if ((dungeon[i][j] == 27) && (dungeon[i+1][j] ==  1)) dungeon[i+1][j] = 16;
			if ((dungeon[i][j] == 19) && (dungeon[i+1][j] == 27)) dungeon[i+1][j] = 26;
			if ((dungeon[i][j] == 27) && (dungeon[i+1][j] == 30)) dungeon[i+1][j] = 19;
			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] == 15)) dungeon[i+1][j] = 14;
			if ((dungeon[i][j] == 14) && (dungeon[i+1][j] == 15)) dungeon[i+1][j] = 14;
			if ((dungeon[i][j] == 22) && (dungeon[i+1][j] ==  1)) dungeon[i+1][j] = 16;
			if ((dungeon[i][j] == 27) && (dungeon[i+1][j] ==  1)) dungeon[i+1][j] = 16;
			if ((dungeon[i][j] ==  6) && (dungeon[i+1][j] == 27) && (dungeon[i+1][j+1])) dungeon[i+1][j] = 22;
			if ((dungeon[i][j] == 22) && (dungeon[i+1][j] == 30)) dungeon[i+1][j] = 19;
			if ((dungeon[i][j] == 21) && (dungeon[i+1][j] ==  1) && (dungeon[i+1][j-1] == 1)) dungeon[i+1][j] = 13;
			if ((dungeon[i][j] == 14) && (dungeon[i+1][j] == 30) && (dungeon[i][j+1] == 6)) dungeon[i+1][j] = 28;
			
			if ((dungeon[i][j] == 16) && (dungeon[i+1][j] ==  6) && (dungeon[i][j+1] == 30)) dungeon[i][j+1] = 27;
			if ((dungeon[i][j] == 16) && (dungeon[i][j+1] == 30) && (dungeon[i+1][j+1] == 30)) dungeon[i][j+1] = 27;
			if ((dungeon[i][j] ==  6) && (dungeon[i+1][j] == 30) && (dungeon[i+1][j-1] == 6)) dungeon[i+1][j] = 21;
			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] == 27) && (dungeon[i+1][j+1] ==  9)) dungeon[i+1][j] = 29;
			if ((dungeon[i][j] ==  9) && (dungeon[i+1][j] == 15)) dungeon[i+1][j] = 14;
			if ((dungeon[i][j] == 15) && (dungeon[i+1][j] == 27) && (dungeon[i+1][j+1] == 2)) dungeon[i+1][j] = 29;
			if ((dungeon[i][j] == 19) && (dungeon[i+1][j] == 18)) dungeon[i+1][j] = 24;
			if ((dungeon[i][j] ==  9) && (dungeon[i+1][j] == 15)) dungeon[i+1][j] = 14;
			if ((dungeon[i][j] == 19) && (dungeon[i+1][j] == 19) && (dungeon[i+1][j-1] == 30)) dungeon[i+1][j] = 24;
			if ((dungeon[i][j] == 24) && (dungeon[i][j-1] == 30) && (dungeon[i][j-2] ==  6)) dungeon[i][j-1] = 21;
			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] == 30)) dungeon[i+1][j] = 28;
			if ((dungeon[i][j] == 15) && (dungeon[i+1][j] == 30)) dungeon[i+1][j] = 28;
			if ((dungeon[i][j] == 28) && (dungeon[i][j+1] == 30)) dungeon[i][j+1] = 18;
			if ((dungeon[i][j] == 28) && (dungeon[i][j+1] ==  2)) dungeon[i][j+1] = 15;
			
			if ((dungeon[i][j] == 19) && (dungeon[i+2][j] ==  2) && (dungeon[i+1][j-1] == 18) && (dungeon[i+1][j+1] == 1)) dungeon[i+1][j] = 17;
			if ((dungeon[i][j] == 19) && (dungeon[i+2][j] ==  2) && (dungeon[i+1][j-1] == 22) && (dungeon[i+1][j+1] ==  1)) dungeon[i+1][j] = 17;
			if ((dungeon[i][j] == 19) && (dungeon[i+2][j] ==  2) && (dungeon[i+1][j-1] == 18) && (dungeon[i+1][j+1] == 13)) dungeon[i+1][j] = 17;
			if ((dungeon[i][j] == 21) && (dungeon[i+2][j] ==  2) && (dungeon[i+1][j-1] == 18) && (dungeon[i+1][j+1] == 1)) dungeon[i+1][j] = 17;
			if ((dungeon[i][j] == 21) && (dungeon[i+1][j+1] ==  1) && (dungeon[i+1][j-1] == 22) && (dungeon[i+2][j] ==  3)) dungeon[i+1][j] = 17;
			if ((dungeon[i][j] == 15) && (dungeon[i+1][j] == 28) && (dungeon[i+2][j] == 30) && (dungeon[i+1][j-1] == 6)) dungeon[i+1][j] = 23;
			if ((dungeon[i][j] == 14) && (dungeon[i+1][j] == 28) && (dungeon[i+2][j] == 1)) dungeon[i+1][j] = 23;
			if ((dungeon[i][j] == 15) && (dungeon[i+1][j] == 27) && (dungeon[i+1][j+1] == 30)) dungeon[i+1][j] = 29;
			if ((dungeon[i][j] == 28) && (dungeon[i][j+1] ==  9)) dungeon[i][j+1] = 15;
			if ((dungeon[i][j] == 21) && (dungeon[i+1][j-1] == 21)) dungeon[i+1][j] = 24;
			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] == 27) && (dungeon[i+1][j+1] == 30)) dungeon[i+1][j] = 29;
			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] == 18)) dungeon[i+1][j] = 25;
			if ((dungeon[i][j] == 21) && (dungeon[i+1][j] ==  9) && (dungeon[i+2][j] == 2)) dungeon[i+1][j] = 11;
			if ((dungeon[i][j] == 19) && (dungeon[i+1][j] == 10)) dungeon[i+1][j] = 17;

			if ((dungeon[i][j] ==  15) && (dungeon[i][j+1] ==  3)) dungeon[i][j+1] = 4;
			
			if ((dungeon[i][j] == 22) && (dungeon[i][j+1] ==  9)) dungeon[i][j+1] = 15;
			if ((dungeon[i][j] == 18) && (dungeon[i][j+1] == 30)) dungeon[i][j+1] = 18;
			if ((dungeon[i][j] == 24) && (dungeon[i-1][j] == 30)) dungeon[i-1][j] = 19;
			if ((dungeon[i][j] == 21) && (dungeon[i][j+1] ==  2)) dungeon[i][j+1] = 15;
			if ((dungeon[i][j] == 21) && (dungeon[i][j+1] ==  9)) dungeon[i][j+1] = 10;
			if ((dungeon[i][j] == 22) && (dungeon[i][j+1] == 30)) dungeon[i][j+1] = 18;
			if ((dungeon[i][j] == 21) && (dungeon[i][j+1] == 30)) dungeon[i][j+1] = 18;
			if ((dungeon[i][j] == 16) && (dungeon[i][j+1] ==  2)) dungeon[i][j+1] = 15;
			if ((dungeon[i][j] == 13) && (dungeon[i][j+1] ==  2)) dungeon[i][j+1] = 15;
			if ((dungeon[i][j] == 22) && (dungeon[i][j+1] ==  2)) dungeon[i][j+1] = 15;
			if ((dungeon[i][j] == 21) && (dungeon[i+1][j] == 18) && (dungeon[i+2][j] == 30)) dungeon[i+1][j] = 24;
			if ((dungeon[i][j] == 21) && (dungeon[i+1][j] ==  9) && (dungeon[i+1][j+1] == 1)) dungeon[i+1][j] = 16;

			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] == 27) && (dungeon[i+1][j+1] == 2)) dungeon[i+1][j] = 29;
			if ((dungeon[i][j] == 23) && (dungeon[i][j+1] ==  2)) dungeon[i][j+1] = 15;
			if ((dungeon[i][j] == 23) && (dungeon[i][j+1] ==  9)) dungeon[i][j+1] = 15;
			if ((dungeon[i][j] == 25) && (dungeon[i][j+1] ==  2)) dungeon[i][j+1] = 15;
			if ((dungeon[i][j] == 22) && (dungeon[i+1][j] ==  9)) dungeon[i+1][j] = 11;
			if ((dungeon[i][j] == 23) && (dungeon[i+1][j] ==  9)) dungeon[i+1][j] = 11;
			if ((dungeon[i][j] == 15) && (dungeon[i+1][j] ==  1)) dungeon[i+1][j] = 16;
			if ((dungeon[i][j] == 11) && (dungeon[i+1][j] == 15)) dungeon[i+1][j] = 14;
			if ((dungeon[i][j] == 23) && (dungeon[i+1][j] ==  1)) dungeon[i+1][j] = 16;
			if ((dungeon[i][j] == 21) && (dungeon[i+1][j] == 27)) dungeon[i+1][j] = 26;
			if ((dungeon[i][j] == 21) && (dungeon[i+1][j] == 18)) dungeon[i+1][j] = 24;
			if ((dungeon[i][j] == 26) && (dungeon[i+1][j] ==  1)) dungeon[i+1][j] = 16;
			if ((dungeon[i][j] == 29) && (dungeon[i+1][j] ==  1)) dungeon[i+1][j] = 16;
			if ((dungeon[i][j] == 29) && (dungeon[i][j+1] ==  2)) dungeon[i][j+1] = 15;
			if ((dungeon[i][j] ==  1) && (dungeon[i][j-1] == 15)) dungeon[i][j-1] = 10;
			if ((dungeon[i][j] == 18) && (dungeon[i][j+1] ==  2)) dungeon[i][j+1] = 15;
			if ((dungeon[i][j] == 23) && (dungeon[i][j+1] == 30)) dungeon[i][j+1] = 18;
			if ((dungeon[i][j] == 18) && (dungeon[i][j+1] ==  9)) dungeon[i][j+1] = 10;
			if ((dungeon[i][j] == 14) && (dungeon[i+1][j] == 30) && (dungeon[i+1][j+1] == 30)) dungeon[i+1][j] = 23;
			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] == 28) && (dungeon[i+1][j-1] ==  6)) dungeon[i+1][j] = 23;
			if ((dungeon[i][j] == 23) && (dungeon[i+1][j] == 18) && (dungeon[i][j-1] == 6)) dungeon[i+1][j] = 24;
			if ((dungeon[i][j] == 14) && (dungeon[i+1][j] == 23) && (dungeon[i+2][j] == 30)) dungeon[i+1][j] = 28;
			if ((dungeon[i][j] == 14) && (dungeon[i+1][j] == 28) && (dungeon[i+2][j] == 30) && (dungeon[i+1][j-1] == 6)) dungeon[i+1][j] = 23;
			
		// fill in blood around edges
			if ((dungeon[i][j] == 23) && (dungeon[i+1][j] == 30)) dungeon[i+1][j] = 19;
			if ((dungeon[i][j] == 29) && (dungeon[i+1][j] == 30)) dungeon[i+1][j] = 19;
			if ((dungeon[i][j] == 29) && (dungeon[i][j+1] == 30)) dungeon[i][j+1] = 18;
			if ((dungeon[i][j] == 19) && (dungeon[i+1][j] == 30)) dungeon[i+1][j] = 19;
			if ((dungeon[i][j] == 21) && (dungeon[i+1][j] == 30)) dungeon[i+1][j] = 19;
			if ((dungeon[i][j] == 26) && (dungeon[i+1][j] == 30)) dungeon[i+1][j] = 19;
			if ((dungeon[i][j] == 16) && (dungeon[i][j+1] == 30)) dungeon[i][j+1] = 18;

			if ((dungeon[i][j] == 13) && (dungeon[i][j+1] ==  9)) dungeon[i][j+1] = 10;
			if ((dungeon[i][j] == 25) && (dungeon[i][j+1] == 30)) dungeon[i][j+1] = 18;
			if ((dungeon[i][j] == 18) && (dungeon[i][j+1] ==  2)) dungeon[i][j+1] = 15;
			if ((dungeon[i][j] == 11) && (dungeon[i+1][j] ==  3)) dungeon[i+1][j] =  5;
			if ((dungeon[i][j] == 19) && (dungeon[i+1][j] ==  9)) dungeon[i+1][j] = 11;
			if ((dungeon[i][j] == 19) && (dungeon[i+1][j] ==  1)) dungeon[i+1][j] = 13;
			if ((dungeon[i][j] == 19) && (dungeon[i+1][j] == 13) && (dungeon[i+1][j-1] == 6)) dungeon[i+1][j] = 16;
		}
	}

	// tiles in middle of dungeon
	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] == 21) && (dungeon[i][j+1] == 24) && (dungeon[i][j+2] == 1)) dungeon[i][j+1] = 17;
			if ((dungeon[i][j] == 15) && (dungeon[i+1][j+1] ==  9) && (dungeon[i+1][j-1] ==  1) && (dungeon[i+2][j] == 16)) dungeon[i+1][j] = 29;
			if ((dungeon[i][j] ==  2) && (dungeon[i-1][j] ==  6)) dungeon[i-1][j] =  8;
			if ((dungeon[i][j] ==  1) && (dungeon[i][j-1] ==  6)) dungeon[i][j-1] =  7;

			if ((dungeon[i][j] ==  6) && (dungeon[i+1][j] == 15) && (dungeon[i+1][j+1] ==  4)) dungeon[i+1][j] = 10;

			if ((dungeon[i][j] ==  1) && (dungeon[i][j+1] ==  3)) dungeon[i][j+1] =  4;
			if ((dungeon[i][j] ==  1) && (dungeon[i][j+1] ==  6)) dungeon[i][j+1] =  4;
			if ((dungeon[i][j] ==  9) && (dungeon[i][j+1] ==  3)) dungeon[i][j+1] =  4;
			if ((dungeon[i][j] == 10) && (dungeon[i][j+1] ==  3)) dungeon[i][j+1] =  4;
			if ((dungeon[i][j] == 13) && (dungeon[i][j+1] ==  3)) dungeon[i][j+1] =  4;
			if ((dungeon[i][j] ==  1) && (dungeon[i][j+1] ==  5)) dungeon[i][j+1] = 12;
			if ((dungeon[i][j] ==  1) && (dungeon[i][j+1] == 16)) dungeon[i][j+1] = 13;
			if ((dungeon[i][j] ==  6) && (dungeon[i][j+1] == 13)) dungeon[i][j+1] = 16;
			if ((dungeon[i][j] == 25) && (dungeon[i][j+1] ==  9)) dungeon[i][j+1] = 10;
			if ((dungeon[i][j] == 13) && (dungeon[i][j+1] ==  5)) dungeon[i][j+1] = 12;

			if ((dungeon[i][j] == 28) && (dungeon[i][j-1] == 6) && (dungeon[i+1][j] ==  1)) dungeon[i+1][j] = 23;
			if ((dungeon[i][j] == 19) && (dungeon[i+1][j] == 10)) dungeon[i+1][j] = 17;
			if ((dungeon[i][j] == 21) && (dungeon[i+1][j] ==  9)) dungeon[i+1][j] = 11;
			if ((dungeon[i][j] == 11) && (dungeon[i+1][j] ==  3)) dungeon[i+1][j] =  5;
			if ((dungeon[i][j] == 10) && (dungeon[i+1][j] ==  4)) dungeon[i+1][j] = 12;
			if ((dungeon[i][j] == 14) && (dungeon[i+1][j] ==  4)) dungeon[i+1][j] = 12;
			if ((dungeon[i][j] == 27) && (dungeon[i+1][j] ==  9)) dungeon[i+1][j] = 11;
			if ((dungeon[i][j] == 15) && (dungeon[i+1][j] ==  4)) dungeon[i+1][j] = 12;
			if ((dungeon[i][j] == 21) && (dungeon[i+1][j] ==  1)) dungeon[i+1][j] = 16;
			if ((dungeon[i][j] == 11) && (dungeon[i+1][j] ==  4)) dungeon[i+1][j] = 12;
			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] ==  3)) dungeon[i+1][j] =  5;
			if ((dungeon[i][j] ==  9) && (dungeon[i+1][j] ==  3)) dungeon[i+1][j] =  5;
			if ((dungeon[i][j] == 14) && (dungeon[i+1][j] ==  3)) dungeon[i+1][j] =  5;
			if ((dungeon[i][j] == 15) && (dungeon[i+1][j] ==  3)) dungeon[i+1][j] =  5;
			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] ==  5) && (dungeon[i+1][j-1] == 16)) dungeon[i+1][j] = 12;
			if ((dungeon[i][j] ==  2) && (dungeon[i+1][j] ==  4)) dungeon[i+1][j] = 12;
			if ((dungeon[i][j] ==  9) && (dungeon[i+1][j] ==  4)) dungeon[i+1][j] = 12;

			if ((dungeon[i][j] ==  1) && (dungeon[i][j-1] ==  8)) dungeon[i][j-1] =  9;
		if ((dungeon[i][j] == 28) && (dungeon[i+1][j] == 23) && (dungeon[i+1][j+1] == 3)) dungeon[i+1][j] = 16;
		}
	}
	
	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] == 21) && (dungeon[i+1][j] ==  10)) dungeon[i+1][j] = 17;
			if ((dungeon[i][j] == 17) && (dungeon[i+1][j] ==  4)) dungeon[i+1][j] = 12;
			if ((dungeon[i][j] == 10) && (dungeon[i+1][j] ==  4)) dungeon[i+1][j] = 12;

			if ((dungeon[i][j] == 17) && (dungeon[i][j+1] ==  5)) dungeon[i][j+1] = 12;
			if ((dungeon[i][j] == 29) && (dungeon[i][j+1] ==  9)) dungeon[i][j+1] = 10;
			if ((dungeon[i][j] == 13) && (dungeon[i][j+1] ==  5)) dungeon[i][j+1] = 12;
			if ((dungeon[i][j] ==  9) && (dungeon[i][j+1] == 16)) dungeon[i][j+1] = 13;
			if ((dungeon[i][j] == 10) && (dungeon[i][j+1] == 16)) dungeon[i][j+1] = 13;
			if ((dungeon[i][j] == 16) && (dungeon[i][j+1] ==  3)) dungeon[i][j+1] =  4;
			if ((dungeon[i][j] == 11) && (dungeon[i][j+1] ==  5)) dungeon[i][j+1] = 12;
			if ((dungeon[i][j] == 10) && (dungeon[i+1][j] ==  3) && (dungeon[i+1][j-1] == 16)) dungeon[i+1][j] = 12;
			if ((dungeon[i][j] == 16) && (dungeon[i][j+1] ==  5)) dungeon[i][j+1] = 12;
			if ((dungeon[i][j] ==  1) && (dungeon[i][j+1] ==  6)) dungeon[i][j+1] =  4;
			if ((dungeon[i][j] == 21) && (dungeon[i+1][j] == 13) && (dungeon[i][j+1] == 10)) dungeon[i+1][j+1] =  12;
			if ((dungeon[i][j] == 15) && (dungeon[i+1][j] == 10)) dungeon[i+1][j] = 17;
		if ((dungeon[i][j] == 22) && (dungeon[i][j+1] == 11)) dungeon[i][j+1] = 17;
		if ((dungeon[i][j] == 15) && (dungeon[i+1][j] == 28) && (dungeon[i+2][j] == 16)) dungeon[i+1][j] = 23;
		if ((dungeon[i][j] == 28) && (dungeon[i+1][j] == 23) && (dungeon[i+1][j+1] == 1) && (dungeon[i+2][j] == 6)) dungeon[i+1][j] = 16;
		}
	}
	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] == 15) && (dungeon[i+1][j] == 28) && (dungeon[i+2][j] == 16)) dungeon[i+1][j] = 23;
			if ((dungeon[i][j] == 21) && (dungeon[i+1][j-1] == 21) && (dungeon[i+1][j+1] == 13) && (dungeon[i+2][j] ==  2)) dungeon[i+1][j] = 17;
			if ((dungeon[i][j] == 19) && (dungeon[i+1][j] == 15) && (dungeon[i+1][j+1] == 12)) dungeon[i+1][j] = 17;
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L4Subs()
{
	int		x,y,i,rv;
	byte	c;

	for (y = 0; y < MDMAXY; y++) {
		for (x = 0; x < MDMAXX; x++) {
			rv = random(0, 3); 
			if (rv == 0) {
				c = dungeon[x][y];
				c = L4BTYPES[c];
				if ((c != 0) && (dflags[x][y] == 0)) {
					rv = random(0, 16);
					i = -1;
					while (rv >= 0) {
						i++;
						if (i == NUMBBLOCKS) i = 0;
						if (c == L4BTYPES[i]) rv--;
					}
					dungeon[x][y] = i;
				}
			}
		}
	}
	for (y = 0; y < MDMAXY; y++) {
		for (x = 0; x < MDMAXX; x++) {
			rv = random(0, 10); 
			if (rv == 0) {
				c = dungeon[x][y];
				c = L4BTYPES[c];
				if ((c == 6) && (dflags[x][y] == 0)) 
					dungeon[x][y] = random(0, 3) + 95;
			}
		}
	}
}

/*------------------------------------------------------------------------*
**------------------------------------------------------------------------*/
static void L4makeDungeon()
{
	int i, j;
	int k, l;

	// top left
	for (j = 0; j < L4DUNY; j++) {
		for(i = 0; i < L4DUNX; i++) {
			k = i<<1;
			l = j<<1;
			L4dungeon[k][l] = dung[i][j];
			L4dungeon[k][l+1] = dung[i][j];
			L4dungeon[k+1][l] = dung[i][j];
			L4dungeon[k+1][l+1] = dung[i][j];
		}
	}
	// bottom left
	for (j = 0; j < L4DUNY; j++) {
		for(i = 0; i < L4DUNX; i++) {
			k = i<<1;
			l = j<<1;
			L4dungeon[k][L4DUNY*2+l] = dung[i][L4DUNY - 1 - j];
			L4dungeon[k][L4DUNY*2+l+1] = dung[i][L4DUNY - 1 - j];
			L4dungeon[k+1][L4DUNY*2+l] = dung[i][L4DUNY - 1 - j];
			L4dungeon[k+1][L4DUNY*2+l+1] = dung[i][L4DUNY - 1 - j];
		}
	}
	// top right
	for (j = 0; j < L4DUNY; j++) {
		for(i = 0; i < L4DUNX; i++) {
			k = i<<1;
			l = j<<1;
			L4dungeon[k+L4DUNX*2][l] = dung[L4DUNX - 1 - i][j];
			L4dungeon[k+L4DUNX*2][l+1] = dung[L4DUNX - 1 - i][j];
			L4dungeon[k+L4DUNX*2+1][l] = dung[L4DUNX - 1 - i][j];
			L4dungeon[k+L4DUNX*2+1][l+1] = dung[L4DUNX - 1 - i][j];
		}
	}
	// bottom right
	for (j = 0; j < L4DUNY; j++) {
		for(i = 0; i < L4DUNX; i++) {
			 k = i<<1;
			 l = j<<1;
			 L4dungeon[L4DUNX*2+k][L4DUNY*2+l] = dung[L4DUNX - 1 - i][L4DUNY - 1 - j];
			 L4dungeon[L4DUNX*2+k][L4DUNY*2+l+1] = dung[L4DUNX - 1 - i][L4DUNY - 1 - j];
			 L4dungeon[L4DUNX*2+k+1][L4DUNY*2+l] = dung[L4DUNX - 1 - i][L4DUNY - 1 - j];
			 L4dungeon[L4DUNX*2+k+1][L4DUNY*2+l+1] = dung[L4DUNX - 1 - i][L4DUNY - 1 - j];
		}
	}
}

/*------------------------------------------------------------------------*
** Connecting the boxes to make U
**------------------------------------------------------------------------*/
static void uShape()
{
	int j, i;
	int rv;

	for (j = 19; j >= 0; j--) {
		for (i = 19; i >= 0; i--) {
			if (dung[i][j] != 1) hallok[j] = FALSE;
			if (dung[i][j] == 1) {
				if ((dung[i][j+1] == 1) && (dung[i+1][j+1] == 0)) {
					hallok[j] = TRUE;
				}
				else hallok[j] = FALSE;
				i = 0; // abort out of loop
			}
		}
	}
	rv = random(0, 19) + 1;
	do{
		if (hallok[rv]) {
			for (i = 19; i >= 0; i--) {
				if (dung[i][rv] == 1) {
					i = -1;
					rv = 0;
				} else {
					dung[i][rv] = 1;
					dung[i][rv+1] = 1;
				}

			}
		} else {
			rv++;
			if (rv == 20) rv = 1;
		}
	} while (rv != 0);

	for (i = 19; i >= 0; i--) {
		for (j = 19; j >= 0; j--) {
			if (dung[i][j] != 1) hallok[i] = FALSE;
			if (dung[i][j] == 1) {
				if ((dung[i+1][j] == 1) && (dung[i+1][j+1] == 0)) {
					hallok[i] = TRUE;
				}
				else hallok[i] = FALSE;
				j = 0; // abort out of loop
			}
		}
	}
	rv = random(0, 19) + 1;
	do{
		if (hallok[rv]) {
			for (j = 19; j >= 0; j--) {
				if (dung[rv][j] == 1) {
					j = -1;
					rv = 0;
				} else {
					dung[rv][j] = 1;
					dung[rv+1][j] = 1;
				}

			}
		} else {
			rv++;
			if (rv == 20) rv = 1;
		}
	} while (rv != 0);
}

/*------------------------------------------------------------------------*
**------------------------------------------------------------------------*/
static long GetArea()
{
	int i,j;
	long rv;

	rv = 0;
	for (j = 0; j < L4DUNY; j++) {
		for (i = 0; i < L4DUNX; i++) {
			if (dung[i][j] == 1) rv++;
		}
	}
	return(rv);
}

/*----------------------------------------------------------------------*
**----------------------------------------------------------------------*/
static void L4drawRoom(int x, int y, int width, int height)
{
	int i, j;

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			dung[x+i][y+j] = 1;
		}
	}
}

/*---------------------------------------------------------------------*
** Check limits of 20 X 20
**---------------------------------------------------------------------*/
static BOOL L4checkRoom(int x, int y, int width, int height)
{
	int i, j;

	if (( x <= 0 ) || (y <= 0)) return(FALSE);

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			if ((x+i < 0) || (x+i >= L4DUNX) || (y+j < 0) || (y+j >= L4DUNY)) return(FALSE);
			if (dung[x+i][y+j] != 0) return(FALSE);
		}
	}
	return(TRUE);
}

/*---------------------------------------------------------------------**
** Generate left and right, up and down rooms
**---------------------------------------------------------------------*/
static void L4roomGen(int x, int y, int w, int h, int dir)
{
	int rx, ry, rx2, ry2;
	int height, width;
	int cx1, cy1, cw, ch;
	int num;
	int dirProb;
	int ran;
	BOOL c, d;

	ran = random(0, 4);
	if (dir == L4DIR_VERT) {
		if (ran == 0) dirProb = L4DIR_HORIZ;
		else dirProb = L4DIR_VERT;
	} else {
		if (ran == 0) dirProb = L4DIR_VERT;
		else dirProb = L4DIR_HORIZ;
	}
	switch(dirProb) {
		case L4DIR_HORIZ :  // left/right
			// left room
			num = 0;
			do {
				width  = ((random(0, L4ROOM_MAX-L4ROOM_MIN+1) + L4ROOM_MIN) >> 1) << 1;
				height = ((random(0, L4ROOM_MAX-L4ROOM_MIN+1) + L4ROOM_MIN) >> 1) << 1;
				ry = y + (h/2) - (height/2);
				rx = x - width;
				cx1 = rx - 1;
				cy1 = ry - 1;
				cw = height + 2;
				ch = width + 1;
				c = L4checkRoom(cx1, cy1, cw, ch);
				num++;
			} while ((c == FALSE) && (num < 20));
			if (c == TRUE) L4drawRoom(rx, ry, width, height);

			// right room
			rx2 = x + w;
			cx1 = rx2;
			cy1 = ry - 1;
			ch = height + 2;
			cw = width + 1;
			d = L4checkRoom(cx1, cy1, cw, ch);
			if (d == TRUE) L4drawRoom(rx2, ry, width, height);
			if (c == TRUE) L4roomGen(rx, ry, width, height, L4DIR_VERT);
			if (d == TRUE) L4roomGen(rx2, ry, width, height, L4DIR_VERT);
			break;

		case L4DIR_VERT :  // top/bottom
			// top room
			num = 0;
			do {
				width  = ((random(0, L4ROOM_MAX-L4ROOM_MIN+1) + L4ROOM_MIN) >> 1) << 1;
				height = ((random(0, L4ROOM_MAX-L4ROOM_MIN+1) + L4ROOM_MIN) >> 1) << 1;
				rx = x + (w/2) - (width/2);
				ry = y - height;
				cx1 = rx - 1;
				cy1 = ry - 1;
				ch = height + 1;
				cw = width + 2;
				c = L4checkRoom(cx1, cy1, cw, ch);
				num++;

			} while ((c == FALSE) && (num < 20));
			if (c == TRUE) L4drawRoom(rx, ry, width, height);

			// bottom room
			ry2 = y + h;
			cx1 = rx - 1;
			cy1 = ry2;
			ch = height + 1;
			cw = width + 2;
			d = L4checkRoom(cx1, cy1, cw, ch);
			if (d == TRUE) L4drawRoom(rx, ry2, width, height);
			if (c == TRUE) L4roomGen(rx, ry, width, height, L4DIR_HORIZ);
			if (d == TRUE) L4roomGen(rx, ry2, width, height, L4DIR_HORIZ);
			break;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void L4firstRoom()
{
	int x,y,w,h;
	int xstor, ystor;
	int rndx, rndy;
	int xmin, xmax, ymin, ymax;

	if (currlevel != 16) {
		if ((currlevel == quests[Q_WARLORD]._qlevel) && (quests[Q_WARLORD]._qactive != QUEST_NOTAVAIL)) {
			app_assert(gbMaxPlayers == 1);
			w = 11;
			h = 11;
		}
		else if ((currlevel == quests[Q_BETRAYER]._qlevel) && (gbMaxPlayers != 1)) {
			w = 11;
			h = 11;
		}
		else {
			w = ((random(0, L4ROOM_MAX-L4ROOM_MIN+1) + L4ROOM_MIN));
			h = ((random(0, L4ROOM_MAX-L4ROOM_MIN+1) + L4ROOM_MIN));
		}
	} else {
		w = DIABSIZE;
		h = DIABSIZE;
	}


	xmin = ((20 - w) >>1);
	xmax = (19 - w);
	rndx = random(0, xmax - xmin + 1) + xmin;
	if ((w + rndx) > 19) {
		xstor = (w + rndx) - 19;
		x = (rndx - xstor) + 1;
	}
	else x = rndx;

	ymin = ((20 - h) >>1);
	ymax = (19 - h);
	rndy = random(0, ymax - ymin + 1) + ymin;
	if ((h + rndy) > 19) {
		ystor = (h + rndy) - 19;
		y = (rndy - ystor) + 1;
	}
	else y = rndy;

	if (currlevel == 16) {
		l4holdx = x;
		l4holdy = y;
	}
	if (QuestStatus(Q_WARLORD)
		|| ((currlevel == quests[Q_BETRAYER]._qlevel) && (gbMaxPlayers != 1))) {
		SP4x1 = x + 1;
		SP4y1 = y + 1;
		SP4x2 = SP4x1 + w;
		SP4y2 = SP4y1 + h;
	} else {
		SP4x1 = 0;
		SP4y1 = 0;
		SP4x2 = 0;
		SP4y2 = 0;
	}
	
	L4drawRoom(x, y, w, h);
	L4roomGen(x, y, w, h, random(0, 2));
}

/*-----------------------------------------------------------------------**
** Save 4 quads for diablo level
**-----------------------------------------------------------------------*/

void L4SaveQuads()
{
	int i,j,x,y;

	x = l4holdx;
	y = l4holdy;
	for (j = 0; j < DIABSIZE; j++) {
		for (i = 0; i < DIABSIZE; i++) {
			dflags[x+i][y+j] = 1;
			dflags[39-x-i][y+j] = 1;
			dflags[x+i][39-y-j] = 1;
			dflags[39-x-i][39-y-j] = 1;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void DRLG_L4SetRoom(BYTE *pSetPiece, int rx1, int ry1)
{
	int		rw,rh;
	int		i,j;
	byte	*sp;

	sp = pSetPiece;
	rw = *sp;
	sp+=2;
	rh = *sp;
	sp+=2;

	sp = pSetPiece+4;
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp != 0) {
				dungeon[rx1+i][ry1+j] = *sp;
				dflags[rx1+i][ry1+j] |= SETP_BIT;
			} else dungeon[rx1+i][ry1+j] = 6;
			sp+=2;
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void DRLG_LoadDiabQuads(BOOL preflag)
{
	BYTE *pSetPiece;


	pSetPiece = LoadFileInMemSig("Levels\\L4Data\\diab1.DUN",NULL,'STPC');
	diabquad1x = l4holdx + ((DIABSIZE - 6) >> 1);
	diabquad1y = l4holdy + ((DIABSIZE - 6) >> 1);
	DRLG_L4SetRoom(pSetPiece, diabquad1x, diabquad1y);
	DiabloFreePtr(pSetPiece);

	if (preflag) pSetPiece = LoadFileInMemSig("Levels\\L4Data\\diab2b.DUN",NULL,'STPC');
	else pSetPiece = LoadFileInMemSig("Levels\\L4Data\\diab2a.DUN",NULL,'STPC');
	diabquad2x = 39 - 11 - l4holdx - ((DIABSIZE - 11) >> 1);
	diabquad2y = l4holdy + ((DIABSIZE - 12) >> 1);
	DRLG_L4SetRoom(pSetPiece, diabquad2x, diabquad2y);
	DiabloFreePtr(pSetPiece);

	if (preflag) pSetPiece = LoadFileInMemSig("Levels\\L4Data\\diab3b.DUN",NULL,'STPC');
	else pSetPiece = LoadFileInMemSig("Levels\\L4Data\\diab3a.DUN",NULL,'STPC');
	diabquad3x = l4holdx + ((DIABSIZE - 11) >> 1);
	diabquad3y = 39 - 11 - l4holdy - ((DIABSIZE - 11) >> 1);
	DRLG_L4SetRoom(pSetPiece, diabquad3x, diabquad3y);
	DiabloFreePtr(pSetPiece);

	if (preflag) pSetPiece = LoadFileInMemSig("Levels\\L4Data\\diab4b.DUN",NULL,'STPC');
	else pSetPiece = LoadFileInMemSig("Levels\\L4Data\\diab4a.DUN",NULL,'STPC');
	diabquad4x = 39 - 9 - l4holdx - ((DIABSIZE - 9) >> 1);
	diabquad4y = 39 - 9 - l4holdy - ((DIABSIZE - 9) >> 1);
	DRLG_L4SetRoom(pSetPiece, diabquad4x, diabquad4y);
	DiabloFreePtr(pSetPiece);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static BOOL DRLG_L4PlaceMiniSet(const byte miniset[], int tmin, int tmax, int cx, int cy, int setview, int ldir)
{
	int sx, sy;
	int sw, sh;
	int xx, yy;
	int i, ii, numt;
	int found, bailcnt;

	// Width and height of the mini set piece
	sw = miniset[0];
	sh = miniset[1];

	// Number of pieces to place
	if ((tmax - tmin) == 0) numt = 1;
	else numt = random(0, tmax - tmin) + tmin;

	for (i = 0; i < numt; i++) {
		// Random starting pos
		sx = random(0, MDMAXX-sw);
		sy = random(0, MDMAXY-sh);

		// Find a location for the mini set piece
		found = 0;
		bailcnt = 0;
		while ((found == 0) && (bailcnt < 200)) {
			found = 1;

			if (((sx >= SP4x1) && (sx <= SP4x2)) && ((sy >= SP4y1) && (sy <= SP4y2))) found = 0;

			if ((cx != -1) && (sx >= (cx - sw)) && (sx <= (cx + 12))) {
				sx = random(0, MDMAXX-sw);
				sy = random(0, MDMAXY-sh);
				found = 0;
			}
			if ((cy != -1) && (sy >= (cy - sh)) && (sy <= (cy + 12))) {
				sx = random(0, MDMAXX-sw);
				sy = random(0, MDMAXY-sh);
				found = 0;
			}
			ii = 2;
			for (yy = 0; ((yy < sh) && (found == 1)); yy++) {
				for (xx = 0; ((xx < sw) && (found == 1)); xx++) {
					if ((miniset[ii] != 0) && (dungeon[sx+xx][sy+yy] != miniset[ii])) found = 0;
					if (dflags[sx+xx][sy+yy] != 0) found = 0;
					ii++;
				}
			}
			if (found == 0) {
				sx++;
				if (sx == (MDMAXX - sw)) {
					sx = 0;
					sy++;
					if (sy == (MDMAXY - sh)) sy = 0;
				}

			}
			bailcnt++;
		}

		if (bailcnt >= 200) return(FALSE);

		// Place mini set piece
		ii = (sh * sw) + 2;
		for (yy = 0; yy < sh; yy++) {
			for (xx = 0; xx < sw; xx++) {
				if (miniset[ii] != 0) {
					dungeon[sx+xx][sy+yy] = miniset[ii];
					dflags[sx+xx][sy+yy] |= BFLAG_SETPC;
				}
				ii++;
			}
		}
	}
	if (currlevel == 15)	// Don't take this out 
		{
			quests[Q_BETRAYER]._qtx = sx+1 ;
			quests[Q_BETRAYER]._qty = sy+1;			
		}
	
	if (setview == 1) {
		ViewX = (sx << 1) + 3 + (DIRTEDGED2) + 2;
		ViewY = (sy << 1) + 4 + (DIRTEDGED2) + 2;
	}

	if (ldir == LVL_DOWN) {
		LvlViewX = (sx << 1) + 3 + (DIRTEDGED2) + 2;
		LvlViewY = (sy << 1) + 4 + (DIRTEDGED2) + 2;
	}

	return(TRUE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L4FTVR(int i, int j, int x, int y, int d)
{
	if ((dTransVal[x][y] == 0) && (dungeon[i][j] == FLOOR_PIC)) {
		dTransVal[x][y] = TransVal;
		dTransVal[x+1][y] = TransVal;
		dTransVal[x][y+1] = TransVal;
		dTransVal[x+1][y+1] = TransVal;
		DRLG_L4FTVR(i+1,j, x+2,y, 1);
		DRLG_L4FTVR(i-1,j, x-2,y, 2);
		DRLG_L4FTVR(i,j+1, x,y+2, 3);
		DRLG_L4FTVR(i,j-1, x,y-2, 4);

		DRLG_L4FTVR(i-1,j-1, x-2,y-2, 5);
		DRLG_L4FTVR(i+1,j-1, x+2,y-2, 6);
		DRLG_L4FTVR(i-1,j+1, x-2,y+2, 7);
		DRLG_L4FTVR(i+1,j+1, x+2,y+2, 8);
	} else {
		if (d == 1) {
			dTransVal[x][y] = TransVal;
			dTransVal[x][y+1] = TransVal;
		}
		if (d == 2) {
			dTransVal[x+1][y] = TransVal;
			dTransVal[x+1][y+1] = TransVal;
		}
		if (d == 3) {
			dTransVal[x][y] = TransVal;
			dTransVal[x+1][y] = TransVal;
		}
		if (d == 4) {
			dTransVal[x][y+1] = TransVal;
			dTransVal[x+1][y+1] = TransVal;
		}
		if (d == 5) dTransVal[x+1][y+1] = TransVal;
		if (d == 6) dTransVal[x][y+1] = TransVal;
		if (d == 7) dTransVal[x+1][y] = TransVal;
		if (d == 8) dTransVal[x][y] = TransVal;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L4FloodTVal()
{
	int i, j;
	int xx,yy;

	yy = DIRTEDGED2;
	for (j = 0; j < MDMAXY; j++) {
		xx = DIRTEDGED2;
		for (i = 0; i < MDMAXX; i++) {
			if ((dungeon[i][j] == FLOOR_PIC) && (dTransVal[xx][yy] == 0)) {
				DRLG_L4FTVR(i,j,xx,yy,0);
				TransVal++;
			}
			xx += 2;
		}
		yy += 2;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
BOOL IsDURWall(char d)
{
	if (d == DURWALL_PIC) return(TRUE);
	if (d == DURWALL2_PIC) return(TRUE);
	if (d == DURWALL3_PIC) return(TRUE);
	return(FALSE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
BOOL IsDLLWall(char dd)
{
	if (dd == DLLWALL_PIC) return(TRUE);
	if (dd == DLLWALL2_PIC) return(TRUE);
	if (dd == DLLWALL3_PIC) return(TRUE);
	return(FALSE);
}
/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L4TransFix()
{
	int i, j;
	int xx,yy;

	yy = DIRTEDGED2;
	for (j = 0; j < MDMAXY; j++) {
		xx = DIRTEDGED2;
		for (i = 0; i < MDMAXX; i++) {
			if (((IsDURWall(dungeon[i][j])) && (dungeon[i][j-1] == DVWALL_PIC))) {
				dTransVal[xx+1][yy] = dTransVal[xx][yy];
				dTransVal[xx+1][yy+1] = dTransVal[xx][yy];
			}
			if (((IsDLLWall(dungeon[i][j])) && (dungeon[i+1][j] == DHWALL_PIC))) { 
				dTransVal[xx][yy+1] = dTransVal[xx][yy];
				dTransVal[xx+1][yy+1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == DVWALL_PIC) {
				dTransVal[xx+1][yy] = dTransVal[xx][yy];
				dTransVal[xx+1][yy+1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == DHWALL_PIC) {
				dTransVal[xx][yy+1] = dTransVal[xx][yy];
				dTransVal[xx+1][yy+1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == DLRWALL_PIC) {
				dTransVal[xx+1][yy] = dTransVal[xx][yy];
				dTransVal[xx][yy+1] = dTransVal[xx][yy];
				dTransVal[xx+1][yy+1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == XARCHWALL_PIC) { 
				dTransVal[xx-1][yy] = dTransVal[xx][yy+1];
				dTransVal[xx][yy] = dTransVal[xx][yy+1];
			}
			if (dungeon[i][j] == YARCHWALL_PIC) { 
				dTransVal[xx][yy-1] = dTransVal[xx+1][yy];
				dTransVal[xx][yy] = dTransVal[xx+1][yy];
			}

			xx += 2;
		}
		yy += 2;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L4Corners()
{
	int i,j;

	for (j = 1; j < MDMAXY-1; j++) {
		for (i = 1; i < MDMAXX-1; i++) {
			if ((dungeon[i][j] >= 18) && (dungeon[i][j] <= 30)) {
				if (dungeon[i+1][j] < 18) {
					dungeon[i][j] += 98;
				} else
				if (dungeon[i][j+1] < 18) dungeon[i][j] += 98;
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void L4FixRim()
{
	for (int i = 0; i < 20; i++) dung[i][0] = 0;
	for (int j = 0; j < 20; j++) dung[0][j] = 0;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void DRLG_L4GeneralFix()
{
	int i,j;

	for (j = 0; j < MDMAXY-1; j++) {
		for (i = 0; i < MDMAXX-1; i++) {
			if ((dungeon[i][j] == 24) || (dungeon[i][j] == 122)) {
				if ((dungeon[i+1][j] == 2) && (dungeon[i][j+1] == 5)) dungeon[i][j] = 17;
			}
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L4(int entry)
{
	long area;
	int t;
	BOOL doneflag;
	int i, j;
	int spi, spj;

	doneflag = FALSE;
	while (!doneflag) {

		DRLG_InitTrans();
		t = 0;

		do {
			InitL4Dungeon();
			L4firstRoom();
			L4FixRim();
			area = GetArea();
			if (area >= L4MIN_AREA) uShape();
			t++;
		}while (area < L4MIN_AREA);
		L4makeDungeon();
		L4makeDmt();
		L4tileFix();

		if (currlevel == 16) L4SaveQuads();
		if (QuestStatus(Q_WARLORD) || ((currlevel == quests[Q_BETRAYER]._qlevel) && (gbMaxPlayers != 1))) {
			for (spi = SP4x1; spi < SP4x2;spi++) {
				for (spj = SP4y1; spj < SP4y2; spj++)					
					dflags[spi][spj] = 1;
			}
		}

		L4AddWall();
		DRLG_L4FloodTVal();
		DRLG_L4TransFix();

		if (setloadflag) {
			DRLG_L4SetSPRoom(SP4x1,SP4y1);
		}

		if (currlevel == 16) {
			DRLG_LoadDiabQuads(TRUE);
		}

		if (QuestStatus(Q_WARLORD)) {
			if (entry == LVL_DOWN) {
				doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 1, LVL_DOWN);
				if ((doneflag) && (currlevel == 13)) doneflag = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, 0, LVL_TWARPDN);
				ViewX++;
			} else {
				if (entry == LVL_UP) {
					doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 0, LVL_DOWN);
					if ((doneflag) && (currlevel == 13)) doneflag = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, 0, LVL_TWARPDN);
					ViewX = (setpc_x << 1) + 6 + DIRTEDGED2;
					ViewY = (setpc_y << 1) + 6 + DIRTEDGED2;
				} else {
					doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 0, LVL_DOWN);
					if ((doneflag) && (currlevel == 13)) doneflag = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, 1, LVL_TWARPDN);
					ViewX++;
				}
			}
		} else 	if (currlevel != 15) {
			if (entry == LVL_DOWN) {
				doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 1, LVL_DOWN);
				if ((doneflag) && (currlevel != 16)) doneflag = DRLG_L4PlaceMiniSet(L4DSTAIRS, 1, 1, -1, -1, 0, LVL_UP);
				if ((doneflag) && (currlevel == 13)) doneflag = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, 0, LVL_TWARPDN);
				ViewX++;
			} else {
				if (entry == LVL_UP) {
					doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 0, LVL_DOWN);
					if ((doneflag) && (currlevel != 16)) doneflag = DRLG_L4PlaceMiniSet(L4DSTAIRS, 1, 1, -1, -1, 1, LVL_UP);
					if ((doneflag) && (currlevel == 13)) doneflag = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, 0, LVL_TWARPDN);
					ViewY++;
				} else {
					doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 0, LVL_DOWN);
					if ((doneflag) && (currlevel != 16)) doneflag = DRLG_L4PlaceMiniSet(L4DSTAIRS, 1, 1, -1, -1, 0, LVL_UP);
					if ((doneflag) && (currlevel == 13)) doneflag = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, 1, LVL_TWARPDN);
					ViewX++;
				}
			}
		} else {
			if (entry == LVL_DOWN) {
				doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 1, LVL_DOWN);
				if (doneflag) {
					if ((gbMaxPlayers != 1) || (quests[Q_DIABLO]._qactive == QUEST_NOTDONE)) 
						doneflag = DRLG_L4PlaceMiniSet(L4PENTA2, 1, 1, -1, -1, 0, LVL_UP);
					else
						doneflag = DRLG_L4PlaceMiniSet(L4PENTA, 1, 1, -1, -1, 0, LVL_UP);
				}
				ViewX++;
			} else {
				doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 0, LVL_DOWN);
				if (doneflag) {
					if ((gbMaxPlayers != 1) || (quests[Q_DIABLO]._qactive == QUEST_NOTDONE))
						doneflag = DRLG_L4PlaceMiniSet(L4PENTA2, 1, 1, -1, -1, 1, LVL_UP);
					else
						doneflag = DRLG_L4PlaceMiniSet(L4PENTA, 1, 1, -1, -1, 1, LVL_UP);
				}
				ViewY++;
			}
		}
	}

	DRLG_L4GeneralFix();

	if (currlevel != 16) DRLG_PlaceThemeRooms(7, 10, FLOOR_PIC, 8, TRUE); 

	// Create shadows
	DRLG_L4Shadows();

	DRLG_L4Corners();

	DRLG_L4Subs();

	extern void DRLG_Init_Globals();
	DRLG_Init_Globals();	

	// for warlord quest
	if (QuestStatus(Q_WARLORD)) {
		for (j = 0; j < MDMAXY; j++) {
			for (i = 0; i < MDMAXX; i++) pdungeon[i][j] = dungeon[i][j];
		}
	}
	DRLG_CheckQuests(SP4x1,SP4y1);

	// make sure nothing is placed on the pentagrams
	if (currlevel == 15) {
		for (j = 0; j < MDMAXY; j++) {
			for (i = 0; i < MDMAXX; i++) {
				if (dungeon[i][j] == 98) Make_SetPC(i-1, j-1, 5, 5);
				if (dungeon[i][j] == 107) Make_SetPC(i-1, j-1, 5, 5);
			}
		}
	}

	// for diablo level, we need to save the current state so we can use the switches
	if (currlevel == 16) {
		// Save the layout for the switches
		for (j = 0; j < MDMAXY; j++) {
			for (i = 0; i < MDMAXX; i++) pdungeon[i][j] = dungeon[i][j];
		}

		// set the current layout
		DRLG_LoadDiabQuads(FALSE);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void DRLG_L4Pass3()
{
	int	i,j,xx,yy;
	long v1,v2,v3,v4,lv;

	// Init dungeon to dirt
	lv = 30 - 1;
	__asm {
		mov		esi,dword ptr [pMegaTiles]
		mov		eax,dword ptr [lv];
		shl		eax,3
		add		esi,eax
		xor		eax,eax
		lodsw
		inc		eax
		mov		dword ptr [v1],eax
		lodsw
		inc		eax
		mov		dword ptr [v2],eax
		lodsw
		inc		eax
		mov		dword ptr [v3],eax
		lodsw
		inc		eax
		mov		dword ptr [v4],eax
	}
	for (yy = 0; yy < DMAXY; yy+=2) {
		for (xx = 0; xx < DMAXX; xx+=2) {
			dPiece[xx][yy] = (int) v1;
			dPiece[xx+1][yy] = (int) v2;
			dPiece[xx][yy+1] = (int) v3;
			dPiece[xx+1][yy+1] = (int) v4;
		}
	}

	// Convert dungeon mega tiles to mini tiles
	yy = DIRTEDGED2;
	for (j = 0; j < MDMAXY; j++) {
		xx = DIRTEDGED2;
		for (i = 0; i < MDMAXX; i++) {
			lv = ((long)dungeon[i][j]) - 1;
			if (lv >= 0) {
				__asm {
					mov		esi,dword ptr [pMegaTiles]
					mov		eax,dword ptr [lv];
					shl		eax,3
					add		esi,eax
					xor		eax,eax
					lodsw
					inc		eax
					mov		dword ptr [v1],eax
					lodsw
					inc		eax
					mov		dword ptr [v2],eax
					lodsw
					inc		eax
					mov		dword ptr [v3],eax
					lodsw
					inc		eax
					mov		dword ptr [v4],eax
				}
			} else {
				v1 = 0;
				v2 = 0;
				v3 = 0;
				v4 = 0;
			}
			dPiece[xx][yy] = (int) v1;
			dPiece[xx+1][yy] = (int) v2;
			dPiece[xx][yy+1] = (int) v3;
			dPiece[xx+1][yy+1] = (int) v4;
			xx += 2;
		}
		yy += 2;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#if IS_VERSION(RETAIL)
void CreateL4Dungeon(unsigned int rseed, int entry)
{
	SetRndSeed(rseed);

	dminx = DIRTEDGED2;
	dminy = DIRTEDGED2;
	dmaxx = DMAXX - (DIRTEDGED2);
	dmaxy = DMAXY - (DIRTEDGED2);

	ViewX = 40;
	ViewY = 40;

	DRLG_InitSetPC();
	DRLG_LoadL4SP();
	DRLG_L4(entry);
	DRLG_L4Pass3();
	DRLG_FreeL4SP();

	// Init the light values for each piece
/*
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			// Tops of doors
			//if (dPiece[i][j] == 13) dSpecial[i][j] = 5;
			//if (dPiece[i][j] == 17) dSpecial[i][j] = 6;
		}
	}
*/
	DRLG_SetPC();
}
#endif


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void L4DrawDung()
{
	byte	*p, *p2;

	p = &dungeon[0][0];
	p2 = &dflags[0][0];
	app_assert(gpBuffer);
	__asm {
		// draw dung
		mov		esi,dword ptr [p]
		mov		edi,dword ptr [gpBuffer]
		add		edi,135248
		mov		edx,40
_YLp:	push	edi
		mov		ecx,40
_XLp:	lodsb
		cmp		al,0
		jz		_Save
		mov		al,229
_Save:	mov		byte ptr [edi],al
		add		edi,768
		loop	_XLp
		pop		edi
		inc		edi
		dec		edx
		jnz		_YLp

		// Draw dflags
		mov		esi,dword ptr [p2]
		mov		edi,dword ptr [gpBuffer]
		add		edi,135248
		mov		edx,40
_YLp2:	push	edi
		mov		ecx,40
_XLp2:	lodsb
		cmp		al,0
		jz		_Skip
		mov		al,146
		mov		byte ptr [edi],al
_Skip:	add		edi,768
		loop	_XLp2
		pop		edi
		inc		edi
		dec		edx
		jnz		_YLp2
	}
}

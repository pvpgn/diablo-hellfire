/*-----------------------------------------------------------------------**
** 								Diablo
**
**							 Objects file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/OBJECTS.CPP 5     2/12/97 10:51a Dbrevik2 $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/
#include <time.h>

#include	"diablo.h"
#pragma hdrstop
#include	"sound.h"
#include	"objects.h"
#include	"objdat.h"
#include	"engine.h"
#include	"gendung.h"
#include	"lighting.h"
#include	"items.h"
#include	"player.h"
#include	"monster.h"
#include	"monstint.h"
#include	"control.h"
#include	"cursor.h"
#include	"spells.h"
#include	"minitext.h"
#include	"textdat.h"
#include	"quests.h"
#include	"drlg_l1.h"
#include	"missiles.h"
#include	"misdat.h"
#include	"effects.h"
#include	"themes.h"
#include	"itemdat.h"
#include	"stores.h"
#include	"monstdat.h"
#include	"error.h"
#include	"msg.h"
#include	"multi.h"
#include	"automap.h"
#include	"drlg_l4.h"
#include	"towners.h"
#include	"inv.h"
#include	"setmaps.h"

/*-----------------------------------------------------------------------*
** Externs
**-----------------------------------------------------------------------*/
int ObjIndex(int x, int y);
static bool OperateFountains(int, int);

/*-----------------------------------------------------------------------*
** Local Defines
**-----------------------------------------------------------------------*/

//#define NO_L5_DOORS						// JKE just until I get doors.

#define DOOR_CLOSED		0
#define DOOR_OPEN		1
#define DOOR_BLOCKED	2

#define TRAP_NODIR		0
#define TRAP_HORIZ		1
#define TRAP_VERT		2

#define TOTAL_FOUNTAINS 4

/*-----------------------------------------------------------------------*
** Global Variables
**-----------------------------------------------------------------------*/

ObjectStruct	object[MAXOBJECTS];
long numobjects;

static char ObjFileList[MAXLVLOBJS];
static int numobjfiles = 0;
static BYTE *pObjCels[MAXLVLOBJS];

int objectactive[MAXOBJECTS];
int objectavail[MAXOBJECTS];

static int trapid;
static int trapdir;

static int leverid;

BOOL InitObjFlag; //True while initing, false otherwise

static int const bxadd[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
static int const byadd[] = { -1, -1, -1, 0, 0, 1, 1, 1 };

int SpellProgress;

#if CHEATS
extern BOOL itemcheat;
extern BOOL uniqcheat;
#endif

/*-----------------------------------------------------------------------*
** Function Prototypes
**-----------------------------------------------------------------------*/
void AddMushPatch();
void AddSlainHero();
void AddSkulkenObject(int, int, int, int);
static void AddNa_Krul_Stuff();
void OpenNaKrul();
static void AddNaKrulLeverObj();
static bool HCheckSpell(int);

/*-----------------------------------------------------------------------*/
// Shrine Data


// Shrine names
static char const * const shrinestrs[] = {
	"Mysterious",		// 0
	"Hidden",			// 1
	"Gloomy",			// 2
	"Weird",			// 3
	"Magical",			// 4
	"Stone",			// 5
	"Religious",		// 6
	"Enchanted",		// 7
	"Thaumaturgic",		// 8
	"Fascinating",		// 9
	"Cryptic",			// 10
	"Magical",			// 11
	"Eldritch",			// 12
	"Eerie",			// 13
	"Divine",			// 14
	"Holy",				// 15
	"Sacred",			// 16
	"Spiritual",		// 17
	"Spooky",			// 18
	"Abandoned",		// 19
	"Creepy",			// 20
	"Quiet",			// 21
	"Secluded",			// 22
	"Ornate",			// 23
	"Glimmering",		// 24
	"Tainted",			// 25
	"Oily",				// 26
	"Glowing",			// 27
	"Mendicant's",		// 28
	"Sparkling",		// 29
	"Town",				// 30
	"Shimmering",			// 31
	"Solar",			// 33
	"Murphy's",			// 34
};
#define NUMSHRINETYPES	(sizeof(shrinestrs)/sizeof(char *))

// Shrine min level it can appear
static char const shrineminlvl[NUMSHRINETYPES] = {
	1,		// 0 Mysterious
	1,		// 1 Hidden
	1,		// 2 Gloomy
	1,		// 3 Weird
	1,		// 4 Magical
	1,		// 5 Stone
	1,		// 6 Religious
	1,		// 7 Enchanted
	1,		// 8 Thaumaturgic
	1,		// 9 Fascinating
	1,		// 10 Cryptic
	1,		// 11 Supernatural
	1,		// 12 Eldritch
	1,		// 13 Eerie
	1,		// 14 Divine
	1,		// 15 Holy
	1,		// 16 Sacred
	1,		// 17 Spiritual
	1,		// 18 Spooky
	1,		// 19 Abandoned
	1,		// 20 Creepy
	1,		// 21 Quiet
	1,		// 22 Secluded
	1,		// 23 Ornate
	1,		// 24 Glimmering
	1,		// 25 Tainted
	1,		// 26 Oily
	1,		// 27 Glowing
	1,		// 28 Mendicants
	1,		// 29 Edisons
	1,		// 30 Town
	1,		// 31 Energy
	1,		// 32 Solar
	1,		// 33 Murphy's
};

// Shrine max level it can appear
static char const shrinemaxlvl[NUMSHRINETYPES] = {
	MAX_LEVELS,		// 0 Mysterious
	MAX_LEVELS,		// 1 Hidden
	MAX_LEVELS,		// 2 Gloomy
	MAX_LEVELS,		// 3 Weird
	MAX_LEVELS,		// 4 Magical
	MAX_LEVELS,		// 5 Stone
	MAX_LEVELS,		// 6 Religious
	8,		// 7 Enchanted
	MAX_LEVELS,		// 8 Thaumaturgic
	MAX_LEVELS,		// 9 Fascinating
	MAX_LEVELS,		// 10 Cryptic
	MAX_LEVELS,		// 11 Supernatural
	MAX_LEVELS,		// 12 Eldritch
	MAX_LEVELS,		// 13 Eerie
	MAX_LEVELS,		// 14 Divine
	MAX_LEVELS,		// 15 Holy
	MAX_LEVELS,		// 16 Sacred
	MAX_LEVELS,		// 17 Spiritual
	MAX_LEVELS,		// 18 Spooky
	MAX_LEVELS,		// 19 Abandoned
	MAX_LEVELS,		// 20 Creepy
	MAX_LEVELS,		// 21 Quiet
	MAX_LEVELS,		// 22 Secluded
	MAX_LEVELS,		// 23 Ornate
	MAX_LEVELS,		// 24 Glimmering
	MAX_LEVELS,		// 25 Tainted
	MAX_LEVELS,		// 26 Oily
	MAX_LEVELS,		// 27 Glowing
	MAX_LEVELS,		// 28 Mendicants
	MAX_LEVELS,		// 29 Edisons
	MAX_LEVELS,		// 30 Town
	MAX_LEVELS,		// 31 Energy
	MAX_LEVELS,		// 32 Solar
	MAX_LEVELS,		// 33 Murphy's
};

#define SHRINE_ALL		0
#define SHRINE_SINGLE	1
#define SHRINE_MULTI	2

// Shrine available for either, single, or multiplayer?
static char const shrineavail[NUMSHRINETYPES] = {
	SHRINE_ALL,		// 0 Mysterious
	SHRINE_ALL,		// 1 Hidden
	SHRINE_SINGLE,	// 2 Gloomy
	SHRINE_SINGLE,	// 3 Weird
	SHRINE_ALL, 	// 4 Magical
	SHRINE_ALL,		// 5 Stone
	SHRINE_ALL,		// 6 Religious
	SHRINE_ALL,		// 7 Enchanted
	SHRINE_SINGLE,	// 8 Thaumaturgic
	SHRINE_ALL, 	// 9 Fascinating
	SHRINE_ALL,		// 10 Cryptic
	SHRINE_ALL,		// 11 Supernatural
	SHRINE_ALL,		// 12 Eldritch
	SHRINE_ALL,		// 13 Eerie
	SHRINE_ALL,		// 14 Divine
	SHRINE_ALL,		// 15 Holy
	SHRINE_ALL,		// 16 Sacred
	SHRINE_ALL,		// 17 Spiritual
	SHRINE_MULTI,	// 18 Spooky
	SHRINE_ALL, 	// 19 Abandoned
	SHRINE_ALL,		// 20 Creepy
	SHRINE_ALL,		// 21 Quiet
	SHRINE_ALL,		// 22 Secluded
	SHRINE_ALL,		// 23 Ornate
	SHRINE_ALL,		// 24 Glimmering
	SHRINE_MULTI,	// 25 Tainted
	SHRINE_ALL,		// 26 Oily
	SHRINE_ALL,		// 27 Glowing
	SHRINE_ALL,		// 28 Mendicants
	SHRINE_ALL,		// 29 Edisons
	SHRINE_ALL,		// 30 Town
	SHRINE_ALL,		// 31 Energy
	SHRINE_SINGLE,	// 32 Solar
	SHRINE_ALL,		// 33 Murphy's
};

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static char const * const StoryBookName[] = {
// The Librium of the Horadrim (White Books)

	"The Great Conflict",
	"The Wages of Sin are War",
	"The Tale of the Horadrim",

// Grimoire of the Burning Hells (Red Books)

	"The Dark Exile",
	"The Sin War",
	"The Binding of the Three",

// The Journals of Lazarus the Betrayer (Normal Book Color)

	"The Realms Beyond",
	"Tale of the Three",
	"The Black King",

// The Journals of Skulken the Necromancer		JKE

	"Journal: The Ensorcellment",
	"Journal: The Meeting",
	"Journal: The Tirade",
	"Journal: His Power Grows",
	"Journal: NA-KRUL",
	"Journal: The End",

	"A Spellbook"
};

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void InitObjectGFX() {
	BYTE fileload[MAXOBJFILES];
	int i,j,t;
	int templevel; //JKE

	ZeroMemory(fileload,sizeof(fileload));

// temp hack to get object art to appear. JKE
	templevel = currlevel;
	if ((currlevel >= CRYPTSTART) && (currlevel <= CRYPTEND))
		templevel = templevel - CRYPTSTART + 1;	// replace templevel with currlevel when done.
	else if ((currlevel >= HIVESTART) && (currlevel <= HIVEEND))
		templevel = templevel - HIVESTART + 9;
	
	for (i = 0; AllObjects[i].oload != -1; ++i) {
		if ((AllObjects[i].oload == OBJMUST)
			&& (templevel >= AllObjects[i].ominlvl)
			&& (templevel <= AllObjects[i].omaxlvl))
			fileload[AllObjects[i].ofindex] = TRUE;

		if (AllObjects[i].otheme != -1) {
			for (t = 0; t < numthemes; ++t) {
				if (theme[t].ttype == AllObjects[i].otheme)
					fileload[AllObjects[i].ofindex] = TRUE;
			}
		}

		if (AllObjects[i].oquest != -1) {
			j = AllObjects[i].oquest;
			if (QuestStatus(j))
				fileload[AllObjects[i].ofindex] = TRUE;
		}
	}

	app_assert(numobjfiles == 0);
	for (i = 0; i < MAXOBJFILES; ++i) {
		if (fileload[i]) {
			char filestr[32];
			ObjFileList[numobjfiles] = i;
			sprintf(filestr, "Objects\\%s.CEL", ObjMasterFList[i]);
			if ((currlevel >= HIVESTART) && (currlevel < CRYPTSTART))	//JKE OBJECTS
				sprintf(filestr, "Objects\\%s.CEL", HObjMasterFList[i]);
			else if (currlevel >= CRYPTSTART)
				sprintf(filestr, "Objects\\%s.CEL", CObjMasterFList[i]);

			app_assert(! pObjCels[numobjfiles]);
			pObjCels[numobjfiles] = LoadFileInMemSig(filestr,NULL,'OGFX');
			++numobjfiles;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void FreeObjectGFX() {
	for (int i = 0; i < numobjfiles; ++i) {
		DiabloFreePtr(pObjCels[i]);
	}
	numobjfiles = 0;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static bool RndLocOk(int xp, int yp) {
	if (dMonster[xp][yp] != 0) return FALSE;
	if (dPlayer[xp][yp] != 0) return FALSE;
	if (dObject[xp][yp] != 0) return FALSE;
	if (dFlags[xp][yp] & BFLAG_SETPC) return FALSE;
	if (nSolidTable[dPiece[xp][yp]]) return FALSE;
	if (leveltype == 1) {
		if ((dPiece[xp][yp] > 126) && (dPiece[xp][yp] < 144))
			return false;
	}

	return true;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static bool TrapLocOk(int xp, int yp) {
	if (dFlags[xp][yp] & BFLAG_SETPC) return false;
	if (nSolidTable[dPiece[xp][yp]]) return false;
	return true;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static bool RoomLocOk(int xp, int yp) {
	if (dPlayer[xp][yp] != 0) return false;
	if (dObject[xp][yp] != 0) return false;
	if (dFlags[xp][yp] & BFLAG_SETPC) return false;
	if (nSolidTable[dPiece[xp][yp]]) return false;
	return true;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void InitRndLocObj(int min, int max, int objtype) {
	int xp, yp;

	int numobjs = random(139,max - min) + min;
	for (int i = 0; i < numobjs; ++i) {
		while (1) {
			xp = random(139,DMAXX - DIRTEDGE) + (DIRTEDGED2);
			yp = random(139,DMAXY - DIRTEDGE) + (DIRTEDGED2);
			if (! RndLocOk(xp-1, yp-1)) continue;
			if (! RndLocOk(xp+0, yp-1)) continue;
			if (! RndLocOk(xp+1, yp-1)) continue;
			if (! RndLocOk(xp-1, yp+0)) continue;
			if (! RndLocOk(xp+0, yp+0)) continue;
			if (! RndLocOk(xp+1, yp+0)) continue;
			if (! RndLocOk(xp-1, yp+1)) continue;
			if (! RndLocOk(xp+0, yp+1)) continue;
			if (! RndLocOk(xp+1, yp+1)) continue;
			break;
		}
		AddObject(objtype, xp, yp);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void InitRndLocBigObj(int min, int max, int objtype) {
	int xp, yp;
	int numobjs = random(140,max - min) + min;
	for (int i = 0; i < numobjs; ++i) {
		while (1) {
			xp = random(140,DMAXX - DIRTEDGE) + (DIRTEDGED2);
			yp = random(140,DMAXY - DIRTEDGE) + (DIRTEDGED2);
			if (! RndLocOk(xp-1, yp-2)) continue;
			if (! RndLocOk(xp+0, yp-2)) continue;
			if (! RndLocOk(xp+1, yp-2)) continue;
			if (! RndLocOk(xp-1, yp-1)) continue;
			if (! RndLocOk(xp+0, yp-1)) continue;
			if (! RndLocOk(xp+1, yp-1)) continue;
			if (! RndLocOk(xp-1, yp+0)) continue;
			if (! RndLocOk(xp+0, yp+0)) continue;
			if (! RndLocOk(xp+1, yp+0)) continue;
			if (! RndLocOk(xp-1, yp+1)) continue;
			if (! RndLocOk(xp+0, yp+1)) continue;
			if (! RndLocOk(xp+1, yp+1)) continue;
			break;
		}

		AddObject(objtype, xp, yp);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void InitRndLocObj5x5(int min, int max, int objtype) {
	int xp, yp, xx, yy, cnt;
	bool done;

	int numobjs = random(139,max - min) + min;
	for (int i = 0; i < numobjs; ++i) {
		cnt = 0;
		done = false;
		while (! done) {
			done = true;
			xp = random(139,DMAXX - DIRTEDGE) + (DIRTEDGED2);
			yp = random(139,DMAXY - DIRTEDGE) + (DIRTEDGED2);
			for (yy = -2; yy <= 2; ++yy) {
				for (xx = -2; xx <= 2; ++xx) if (! RndLocOk(xp+xx,yp+yy)) done = FALSE;
			}
			if (!done) {
				++cnt;
				if (cnt > 20000) return;
			}
		}
		AddObject(objtype, xp, yp);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void ClrAllObjects ()
{
	int		i;

	#if 0
	for (i = 0; i < MAXOBJECTS; ++i) {
		object[i]._ox = 0;
		object[i]._oy = 0;
		object[i]._oAnimData = NULL;
		object[i]._oAnimDelay = 0;
		object[i]._oAnimCnt = 0;
		object[i]._oAnimLen = 0;
		object[i]._oAnimFrame = 0;
		object[i]._oDelFlag = FALSE;
		object[i]._oVar1 = 0;
		object[i]._oVar2 = 0;
		object[i]._oVar3 = 0;
		object[i]._oVar4 = 0;
	}
	#else
	memset(object, 0, sizeof(object));
	#endif
	
	numobjects = 0;
	
	for (i = 0; i < MAXOBJECTS; ++i) {
		objectavail[i] = i;
		//objectactive[i] = 0;
	}
	memset(objectactive, 0, sizeof(objectactive));
	trapid = 1;
	trapdir = TRAP_NODIR;
	leverid = 1;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddTortures()
{
	int yp, xp;

	for (yp = 0; yp < DMAXY; ++yp) {
		for (xp = 0; xp < DMAXX; ++xp) {
			if (dPiece[xp][yp] == 367) {
				AddObject(OBJ_TORTURE1, xp, yp+1);
				AddObject(OBJ_TORTURE3, xp+2, yp-1);
				AddObject(OBJ_TORTURE2, xp, yp+3);
				AddObject(OBJ_TORTURE4, xp+4, yp-1);
				AddObject(OBJ_TORTURE5, xp, yp+5);
				AddObject(OBJ_TNUDEM1, xp+1, yp+3);
				AddObject(OBJ_TNUDEM2, xp+4, yp+5);
				AddObject(OBJ_TNUDEM3, xp+2, yp);
				AddObject(OBJ_TNUDEM4, xp+3, yp+2);
				AddObject(OBJ_TNUDEW1, xp+2, yp+4);
				AddObject(OBJ_TNUDEW2, xp+2, yp+1);
				AddObject(OBJ_TNUDEW3, xp+4, yp+2);
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddCandles()
{
	int const xp = quests[Q_PWATER]._qtx;
	int const yp = quests[Q_PWATER]._qty;

	AddObject(OBJ_STORYCANDLE, xp-2, yp+1);
	AddObject(OBJ_STORYCANDLE, xp+3, yp+1);
	AddObject(OBJ_STORYCANDLE, xp-1, yp+2);
	AddObject(OBJ_STORYCANDLE, xp+2, yp+2);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddTrapLine(int min, int max, int tobjtype, int lobjtype)
{
	int i, j, xp, yp, numobjs;
	int sx, sy, xa, ya, t;
	int lx1, ly1, lx2, ly2;
	bool found;

	numobjs = random(0,max - min) + min;
	for (i = 0; i < numobjs; ++i) {
		found = false;
		while (!found) {
			found = true;
			xp = random(0,DMAXX - DIRTEDGE) + (DIRTEDGED2);
			yp = random(0,DMAXY - DIRTEDGE) + (DIRTEDGED2);
			if (random(0,2)) {
				while (TrapLocOk(xp, yp-1)) --yp;	// Go to bottom
				sx = xp;
				sy = yp;
				xa = 0;
				ya = 1;
				t = 0;
				while (TrapLocOk(xp, yp+1) && found) {
					found = found && RndLocOk(xp-3, yp);
					found = found && RndLocOk(xp-2, yp);
					found = found && RndLocOk(xp-1, yp);
					found = found && RndLocOk(xp, yp);
					found = found && RndLocOk(xp+1, yp);
					found = found && RndLocOk(xp+2, yp);
					found = found && RndLocOk(xp+3, yp);
					++yp;
					++t;
				}
				lx1 = xp-2;
				ly1 = random(0,t-1) + sy + 1;
				lx2 = xp+2;
				ly2 = random(0,t-1) + sy + 1;
				trapdir = TRAP_VERT;
			} else {
				while (TrapLocOk(xp-1, yp)) --xp;	// Go to left
				sx = xp;
				sy = yp;
				xa = 1;
				ya = 0;
				t = 0;
				while (TrapLocOk(xp+1, yp) && found) {
					found = found && RndLocOk(xp, yp-3);
					found = found && RndLocOk(xp, yp-2);
					found = found && RndLocOk(xp, yp-1);
					found = found && RndLocOk(xp, yp);
					found = found && RndLocOk(xp, yp+1);
					found = found && RndLocOk(xp, yp+2);
					found = found && RndLocOk(xp, yp+3);
					++xp;
					++t;
				}
				lx1 = random(0,t-1) + sx + 1;
				ly1 = yp-2;
				lx2 = random(0,t-1) + sx + 1;
				ly2 = yp+2;
				trapdir = TRAP_HORIZ;
			}
			if ((t < 5) || (t > 12)) found = FALSE;
		}

		// Place trap
		xp = sx;
		yp = sy;
		for (j = 0; j <= t; ++j) {
			AddObject(tobjtype, xp, yp);
			xp += xa;
			yp += ya;
		}

		// Place levers
		AddObject(lobjtype, lx1, ly1);
		AddObject(lobjtype, lx2, ly2);

		++trapid;		// Next trap
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddLeverObj(int lx1, int ly1, int lx2, int ly2, int x1, int y1, int x2, int y2)
{
	int xp, yp;
	while (1) {
		xp = random(141,lx2 - lx1 + 1) + lx1;
		yp = random(141,ly2 - ly1 + 1) + ly1;
		if (! RndLocOk(xp-1, yp-1)) continue;
		if (! RndLocOk(xp+0, yp-1)) continue;
		if (! RndLocOk(xp+1, yp-1)) continue;
		if (! RndLocOk(xp-1, yp+0)) continue;
		if (! RndLocOk(xp+0, yp+0)) continue;
		if (! RndLocOk(xp+1, yp+0)) continue;
		if (! RndLocOk(xp-1, yp+1)) continue;
		if (! RndLocOk(xp+0, yp+1)) continue;
		if (! RndLocOk(xp+1, yp+1)) continue;
		break;
	}

	AddObject(OBJ_LEVER, xp, yp);
	int i = dObject[xp][yp] - 1;
	SetObjMapRange(i, x1, y1, x2, y2, leverid);
	++leverid;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddBookLever(int lx1, int ly1, int lx2, int ly2, int x1, int y1, int x2, int y2, int msg)
{
/*	int xp, yp;
	while (1) {
		xp = random(142,lx2 - lx1 + 1) + lx1;
		yp = random(142,ly2 - ly1 + 1) + ly1;
		if (! RndLocOk(xp-1, yp-1)) continue;
		if (! RndLocOk(xp+0, yp-1)) continue;
		if (! RndLocOk(xp+1, yp-1)) continue;
		if (! RndLocOk(xp-1, yp+0)) continue;
		if (! RndLocOk(xp+0, yp+0)) continue;
		if (! RndLocOk(xp+1, yp+0)) continue;
		if (! RndLocOk(xp-1, yp+1)) continue;
		if (! RndLocOk(xp+0, yp+1)) continue;
		if (! RndLocOk(xp+1, yp+1)) continue;
		break;
	}
*/
	int xp, yp, xx, yy, cnt;
	bool done;

		cnt = 0;
		done = false;
		while (! done) {
			done = true;
			xp = random(139,DMAXX - DIRTEDGE) + (DIRTEDGED2);
			yp = random(139,DMAXY - DIRTEDGE) + (DIRTEDGED2);
			for (yy = -2; yy <= 2; ++yy) {
				for (xx = -2; xx <= 2; ++xx) if (! RndLocOk(xp+xx,yp+yy)) done = FALSE;
			}
			if (!done) {
				++cnt;
				if (cnt > 20000) return;
			}
		}
	if (QuestStatus(Q_BLIND)) AddObject(OBJ_BLINDBOOK, xp, yp);
	if (QuestStatus(Q_WARLORD)) AddObject(OBJ_STEELTOME, xp, yp);
	if (QuestStatus(Q_BLOOD)) { 
		xp = (setpc_x << 1) + DIRTEDGED2 + 9;
		yp = (setpc_y << 1) + DIRTEDGED2 + 24;
		AddObject(OBJ_BLOODBOOK, xp, yp);
	}
	app_assert((DWORD)xp < MAXDUNX);
	app_assert((DWORD)yp < MAXDUNY);
	int i = dObject[xp][yp] - 1;
	app_assert((DWORD)i < MAXOBJECTS);
	SetObjMapRange(i, x1, y1, x2, y2, leverid);
	SetBookMsg(i, msg);
	++leverid;
	object[i]._oVar6 = object[i]._oAnimFrame + 1;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void InitRndBarrels()
{
	int xp, yp, o, rv, c, t;

	int numobjs = random(143,5) + 3;
	for (int i = 0; i < numobjs; ++i) {
		while (1) {
			xp = random(143,DMAXX - DIRTEDGE) + (DIRTEDGED2);
			yp = random(143,DMAXY - DIRTEDGE) + (DIRTEDGED2);
			if (! RndLocOk(xp, yp)) continue;
			break;
		}

		// Add normal or exploding barrel
		if (random(143,4)) o = OBJ_BARREL;
		else o = OBJ_BARRELEX;
		AddObject(o, xp, yp);
		c = 1;

		bool found = true;
		while ((random(143,c >> 1) == 0) && (found)) {
			t = 0;
			found = false;
			while ((!found) && (t < 3)) {
				rv = random(143,8);
				xp += bxadd[rv];
				yp += byadd[rv];
				found = RndLocOk(xp, yp);
				++t;
			}
			if (found) {
				if (random(143,5)) o = OBJ_BARREL;
				else o = OBJ_BARRELEX;
				AddObject(o, xp, yp);
				++c;
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void AddL1Objs(int x1, int y1, int x2, int y2)
{
	int i,j,pn;

	for (j = y1; j < y2; ++j) {
		for (i = x1; i < x2; ++i) {
			// Add Light
			pn = dPiece[i][j];
			if (pn == 270) AddObject(OBJ_L1LIGHT, i, j);
			if ((pn == 44) || (pn == 51) || (pn == 214)) 
				AddObject(OBJ_L1DOORL, i, j);
			if ((pn == 46) || (pn == 56)) AddObject(OBJ_L1DOORR, i, j);
		}
	}
}
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void AddL5Objs(int x1, int y1, int x2, int y2)
{
	int i,j,pn;

	for (j = y1; j < y2; ++j) {
		for (i = x1; i < x2; ++i) {
			// Add Light
			pn = dPiece[i][j];
//			if (pn == 270) AddObject(OBJ_L1LIGHT, i, j);
			if (pn == 77)
				AddObject(OBJ_L1DOORL, i, j);
			if (pn == 80) 
				AddObject(OBJ_L1DOORR, i, j);
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void AddL2Objs(int x1, int y1, int x2, int y2)
{
	int i,j,pn;

	for (j = y1; j < y2; ++j) {
		for (i = x1; i < x2; ++i) {
			// Add Doors
			pn = dPiece[i][j];
			if (pn == 13 || pn == 541) AddObject(OBJ_L2DOORL, i, j);
			if (pn == 17 || pn == 542) AddObject(OBJ_L2DOORR, i, j);
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddL3Objs(int x1, int y1, int x2, int y2)
{
	int i,j,pn;

	for (j = y1; j < y2; ++j) {
		for (i = x1; i < x2; ++i) {
			// Add Doors
			pn = dPiece[i][j];
			if (pn == 531) AddObject(OBJ_L3DOORL, i, j);
			if (pn == 534) AddObject(OBJ_L3DOORR, i, j);
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static bool TorchLocOK(int xp, int yp)
{
	if (dFlags[xp][yp] & BFLAG_SETPC) return false;
	return true;
}
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddL2Torches()
{
	int i,j,pn;

	for (j = 0; j < DMAXY; ++j) {
		for (i = 0; i < DMAXX; ++i) {
			// Add Torches
			if(TorchLocOK(i,j)) {
				pn = dPiece[i][j];
				if ((pn == 1) && (random(145,3) == 0)) AddObject(OBJ_TORCHL2, i, j);
				if ((pn == 5) && (random(145,3) == 0)) AddObject(OBJ_TORCHR2, i, j);
				if ((pn == 37) && (random(145,10) == 0) && (dObject[i-1][j] == 0)) AddObject(OBJ_TORCHL, i-1, j);
				if ((pn == 41) && (random(145,10) == 0) && (dObject[i][j-1] == 0)) AddObject(OBJ_TORCHR, i, j-1);
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static bool WallTrapLocOk(int xp, int yp)
{
	if (dFlags[xp][yp] & BFLAG_SETPC) return false;
	if (!nTrapTable[dPiece[xp][yp]]) return false;
	return true;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddObjTraps()
{
	int i,j;
	int x,y;
	int rndv;
	char oi, oi2;

	if (currlevel == 1) rndv = 10;
	if (currlevel >= 2) rndv = 15;
	if (currlevel >= 5) rndv = 20;
	if (currlevel >= 7) rndv = 25;
	for (j = 0; j < DMAXY; ++j) {
		for (i = 0; i < DMAXX; ++i) {
			if ((dObject[i][j] > 0) && (random(144,100) < rndv)) {
				oi = dObject[i][j] - 1;
				if (AllObjects[object[oi]._otype].oTrapFlag) {
					x = i;
					y = j;
					if (random(144,2) == 0) {
						--x;
						while (!nSolidTable[dPiece[x][y]]) --x;
						if (WallTrapLocOk(x, y) && ((i - x) > 1)) {
							AddObject(OBJ_TRAPL, x, y);
							oi2 = dObject[x][y] - 1;
							object[oi2]._oVar1 = i;
							object[oi2]._oVar2 = j;
							object[oi]._oTrapFlag = TRUE;
						}
					} else {
						--y;
						while (!nSolidTable[dPiece[x][y]]) --y;
						if (WallTrapLocOk(x, y) && ((j - y) > 1)) {
							AddObject(OBJ_TRAPR, x, y);
							oi2 = dObject[x][y] - 1;
							object[oi2]._oVar1 = i;
							object[oi2]._oVar2 = j;
							object[oi]._oTrapFlag = TRUE;
						}
					}
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddChestTraps()
{
	int i,j;
	char oi;

	for (j = 0; j < DMAXY; ++j) {
		for (i = 0; i < DMAXX; ++i) {
			if (dObject[i][j] > 0) {
				oi = dObject[i][j] - 1;
				if ((object[oi]._otype >= OBJ_CHEST1) && 
					(object[oi]._otype <= OBJ_CHEST3) &&
					(!object[oi]._oTrapFlag) &&
					(random(0, 100) < 10)) {
					object[oi]._otype = (object[oi]._otype - OBJ_CHEST1) + OBJ_TCHEST1;
					object[oi]._oTrapFlag = TRUE;
					if (leveltype == 2) object[oi]._oVar4 = random(0, 2);	// Type of trap
#if defined(HELLFIRE2)
					else object[oi]._oVar4 = random(0, 7);
#else
					else object[oi]._oVar4 = random(0, 6);
#endif
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------*
** Used by diablo level only!!!!!
**-----------------------------------------------------------------------*/

static void LoadMapObjects(BYTE *pMap, int startx, int starty, int x1, int y1, int w, int h, int leveridx)
{
	int i,j,rw,rh;
	int ox,oy;
	BYTE *lm;
	long mapoff;
	int ot, oi;

	InitObjFlag = TRUE;

	lm = pMap;
	rw = *lm;
	lm += 2;
	rh = *lm;
	// Skip map + height word
	mapoff = ((rw * rh) << 1) + 2;
	// Convert to index mini tile level instead of mega
	rw = rw << 1;
	rh = rh << 1;
	// Skip treasure and monster map
	mapoff += ((rw * rh) << 2);
	lm += mapoff;

	for (j = 0; j < rh; ++j) {
		for (i = 0; i < rw; ++i) {
			if (*lm != 0) {
				ot = *lm;
				ox = i + DIRTEDGED2 + startx;
				oy = j + DIRTEDGED2 + starty;
				AddObject(ObjTypeConv[ot], ox, oy);
				oi = ObjIndex(ox, oy);
				SetObjMapRange(oi, x1, y1, x1+w, y1+h, leveridx);
			}
			lm+=2;
		}
	}

	InitObjFlag = FALSE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void LoadMapObjs(BYTE *pMap, int startx, int starty)
{
	int i,j,rw,rh;
	int ox,oy;
	BYTE *lm;
	long mapoff;
	int ot;

	InitObjFlag = TRUE;

	lm = pMap;
	rw = *lm;
	lm += 2;
	rh = *lm;
	// Skip map + height word
	mapoff = ((rw * rh) << 1) + 2;
	// Convert to index mini tile level instead of mega
	rw = rw << 1;
	rh = rh << 1;
	// Skip treasure and monster map
	mapoff += ((rw * rh) << 2);
	lm += mapoff;

	for (j = 0; j < rh; ++j) {
		for (i = 0; i < rw; ++i) {
			if (*lm != 0) {
				ot = *lm;
				ox = i + DIRTEDGED2 + startx;
				oy = j + DIRTEDGED2 + starty;
				AddObject(ObjTypeConv[ot], ox, oy);
			}
			lm+=2;
		}
	}

	InitObjFlag = FALSE;
}

/*-----------------------------------------------------------------------*
** Levers for diablo level
**-----------------------------------------------------------------------*/

static void AddDiabObjs()
{
	BYTE *pSetPiece;
	int xx, yy;

	pSetPiece = LoadFileInMemSig("Levels\\L4Data\\diab1.DUN",NULL,'STPC');
	xx = (diabquad1x << 1);
	yy = (diabquad1y << 1);
	LoadMapObjects(pSetPiece, xx, yy, diabquad2x, diabquad2y, 11, 12, 1);
	DiabloFreePtr(pSetPiece);

	pSetPiece = LoadFileInMemSig("Levels\\L4Data\\diab2a.DUN",NULL,'STPC');
	xx = (diabquad2x << 1);
	yy = (diabquad2y << 1);
	LoadMapObjects(pSetPiece, xx, yy, diabquad3x, diabquad3y, 11, 11, 2);
	DiabloFreePtr(pSetPiece);

	pSetPiece = LoadFileInMemSig("Levels\\L4Data\\diab3a.DUN",NULL,'STPC');
	xx = (diabquad3x << 1);
	yy = (diabquad3y << 1);
	LoadMapObjects(pSetPiece, xx, yy, diabquad4x, diabquad4y, 9, 9, 3);
	DiabloFreePtr(pSetPiece);
}

static void AddSkulkenBooks(int number)		// Journal entry books JKE
{
	int xp,yp,xx,yy,cnt;
	bool done;

	cnt = 0;
	done = false;
	while (! done) {
		done = true;
		xp = random(139,DMAXX - DIRTEDGE) + (DIRTEDGED2);
		yp = random(139,DMAXY - DIRTEDGE) + (DIRTEDGED2);
		for (yy = -2; yy <= 2; ++yy) {
			for (xx = -3; xx <= 3; ++xx) if (! RndLocOk(xp+xx,yp+yy)) done = FALSE;
		}
		if (!done) {
			++cnt;
			if (cnt > 20000) return;
		}
	}
	AddSkulkenObject(OBJ_STORYBOOK, number, xp, yp);

//	AddObject(OBJ_STORYBOOK, xp, yp);

	AddObject(OBJ_STORYCANDLE, xp-2, yp+1);
	AddObject(OBJ_STORYCANDLE, xp-2, yp);
	AddObject(OBJ_STORYCANDLE, xp-1, yp-1);
	AddObject(OBJ_STORYCANDLE, xp+1, yp-1);
	AddObject(OBJ_STORYCANDLE, xp+2, yp);
	AddObject(OBJ_STORYCANDLE, xp+2, yp+1);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddStoryBooks()
{
	int xp,yp,xx,yy,cnt;
	bool done;

	cnt = 0;
	done = false;
	while (! done) {
		done = true;
		xp = random(139,DMAXX - DIRTEDGE) + (DIRTEDGED2);
		yp = random(139,DMAXY - DIRTEDGE) + (DIRTEDGED2);
		for (yy = -2; yy <= 2; ++yy) {
			for (xx = -3; xx <= 3; ++xx) if (! RndLocOk(xp+xx,yp+yy)) done = FALSE;
		}
		if (!done) {
			++cnt;
			if (cnt > 20000) return;
		}
	}
	AddObject(OBJ_STORYBOOK, xp, yp);
	AddObject(OBJ_STORYCANDLE, xp-2, yp+1);
	AddObject(OBJ_STORYCANDLE, xp-2, yp);
	AddObject(OBJ_STORYCANDLE, xp-1, yp-1);
	AddObject(OBJ_STORYCANDLE, xp+1, yp-1);
	AddObject(OBJ_STORYCANDLE, xp+2, yp);
	AddObject(OBJ_STORYCANDLE, xp+2, yp+1);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddHookedBodies(int freq)
{
	int	i, j, ii, jj;

	//Add tortured bodies on hooks
	for (j = 0; j < MDMAXY; ++j) {
		for (i = 0; i < MDMAXX; ++i) {
			if ((dungeon[i][j] == 1 || dungeon[i][j] == 2) && 
				((!random(0, freq)) && (SkipThemeRoom(i, j)))) {
				ii = (i << 1) + DIRTEDGED2;
				jj = (j << 1) + DIRTEDGED2;
				if ((dungeon[i][j] == 1) && (dungeon[i+1][j] == 6)) {
					switch(random(0, 3)) {
						case 0:
							AddObject(OBJ_TORTURE1, ii+1, jj);
							break;
						case 1:
							AddObject(OBJ_TORTURE2, ii+1, jj);
							break;
						case 2:
							AddObject(OBJ_TORTURE5, ii+1, jj);
							break;
					}
				} else if ((dungeon[i][j] == 2) && (dungeon[i][j+1] == 6)) {
					switch(random(0, 2)) {
						case 0:
							AddObject(OBJ_TORTURE3, ii, jj);
							break;
						case 1:
							AddObject(OBJ_TORTURE4, ii, jj);
							break;
					}
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void AddL4Goodies()
{
	AddHookedBodies(6);
	InitRndLocObj(2, 6, OBJ_TNUDEM1);
	InitRndLocObj(2, 6, OBJ_TNUDEM2);
	InitRndLocObj(2, 6, OBJ_TNUDEM3);
	InitRndLocObj(2, 6, OBJ_TNUDEM4);
	InitRndLocObj(2, 6, OBJ_TNUDEW1);
	InitRndLocObj(2, 6, OBJ_TNUDEW2);
	InitRndLocObj(2, 6, OBJ_TNUDEW3);
	InitRndLocObj(2, 6, OBJ_DECAP);
	InitRndLocObj(1, 3, OBJ_CAULDRON);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void AddLazStand()
{
	int xp,yp,xx,yy,cnt;
	bool done = false;

	cnt = 0;
	while (! done) {
		done = true;
		xp = random(139,DMAXX - DIRTEDGE) + (DIRTEDGED2);
		yp = random(139,DMAXY - DIRTEDGE) + (DIRTEDGED2);
		for (yy = -3; yy <= 3; ++yy) {
			for (xx = -2; xx <= 3; ++xx) if (! RndLocOk(xp+xx,yp+yy)) done = FALSE;
		}
		if (!done) {
			++cnt;
			if (cnt > 10000) {
				// If can't find big area put it anywhere
				InitRndLocObj(1, 1, OBJ_LAZSTAND);
				return;
			}
		}
	}
	AddObject(OBJ_LAZSTAND, xp, yp);
	AddObject(OBJ_TNUDEM2, xp+0, yp+2);
	AddObject(OBJ_STORYCANDLE, xp+1, yp+2);
	AddObject(OBJ_TNUDEM3, xp+2, yp+2);
	AddObject(OBJ_TNUDEW1, xp+0, yp-2);
	AddObject(OBJ_STORYCANDLE, xp+1, yp-2);
	AddObject(OBJ_TNUDEW2, xp+2, yp-2);
	AddObject(OBJ_STORYCANDLE, xp-1, yp-1);
	AddObject(OBJ_TNUDEW3, xp-1, yp+0);
	AddObject(OBJ_STORYCANDLE, xp-1, yp+1);
}

/*-----------------------------------------------------------------------*
** Does not get called for setlevels so no worries
**-----------------------------------------------------------------------*/

void InitObjects ()
{
	int textdef;
	byte *setp;

	ClrAllObjects();
	SpellProgress = 0;

	// No objects on DIABLO_LEVEL. Inited seperatly
	if (currlevel == DIABLO_LEVEL) {
		AddDiabObjs();
		return;
	}

	InitObjFlag = TRUE;
	
	int rs = GetRndSeed();

	if (currlevel == SLAIN_HERO_LEVEL && gbMaxPlayers == 1) {
		AddSlainHero();
	}	

	if (currlevel == quests[Q_BKMUSHRM]._qlevel
		&& quests[Q_BKMUSHRM]._qactive == QUEST_NOTACTIVE) {
		AddMushPatch();
	}

	if (currlevel == STORY_BOOK1_LEVEL) AddStoryBooks();
	else if (currlevel == STORY_BOOK2_LEVEL) AddStoryBooks();
	else if (currlevel == STORY_BOOK3_LEVEL) AddStoryBooks();

	if (currlevel == SKULKEN_BOOK1_LEVEL) 
	{
//		AddSkulkenBooks(0);
		AddSkulkenBooks(1);

//		AddStoryBooks();
//		AddStoryBooks();
	}
	else if (currlevel == SKULKEN_BOOK2_LEVEL) 
	{
		AddSkulkenBooks(2);
		AddSkulkenBooks(3);
	}
	else if (currlevel == SKULKEN_BOOK3_LEVEL) 
	{
		AddSkulkenBooks(4);
		AddSkulkenBooks(5);
	}
	
	if (currlevel == NA_KRUL_LEVEL)
	{
		AddNa_Krul_Stuff();

	}

	if (leveltype == 1) {
		if (QuestStatus(Q_BUTCHER)) AddTortures();
		if (QuestStatus(Q_PWATER)) AddCandles();
		if (QuestStatus(Q_LTBANNER)) AddObject(OBJ_SIGNCHEST,(setpc_x << 1) + DIRTEDGED2 + 10, (setpc_y << 1) + DIRTEDGED2 + 3);

		InitRndLocBigObj(10, 15, OBJ_SARC);
		if (currlevel < CRYPTSTART)
			AddL1Objs(0, 0, DMAXX, DMAXY);
		else
			AddL5Objs(0,0,DMAXX,DMAXY);
		InitRndBarrels();
	}
	if (leveltype == 2) {
		if (QuestStatus(Q_ROCK))
			InitRndLocObj5x5(1, 1, OBJ_STAND);	// Rock stand
		if (QuestStatus(Q_SCHAMB)) 
			InitRndLocObj5x5(1, 1, OBJ_BOOK2R);	// SCamb book

		AddL2Objs(0, 0, DMAXX, DMAXY);
		AddL2Torches();
		if (QuestStatus(Q_BLIND)) {
			if (plr[myplr]._pClass == CLASS_WARRIOR) textdef = TXT_WARBLIND;
			else if (plr[myplr]._pClass == CLASS_ROGUE) textdef = TXT_ROGBLIND;
			else if (plr[myplr]._pClass == CLASS_SORCEROR) textdef = TXT_SORBLIND;
			else if (plr[myplr]._pClass == CLASS_MONK) textdef = TXT_MNKBLIND;
			else if (plr[myplr]._pClass == CLASS_BARD) textdef = TXT_BRDBLIND;
			else if (plr[myplr]._pClass == CLASS_BARBARIAN) textdef = TXT_BARBLIND;
			quests[Q_BLIND]._qmsg = textdef;
			AddBookLever(0, 0, DMAXX, DMAXY, setpc_x, setpc_y, setpc_x+setpc_w+1, setpc_y+setpc_h+1, textdef);
			setp = LoadFileInMemSig("Levels\\L2Data\\Blind2.DUN",NULL,'STPC');	
			LoadMapObjs(setp, (setpc_x << 1), (setpc_y << 1));
			DiabloFreePtr(setp);
		}
		if (QuestStatus(Q_BLOOD)) {
			if (plr[myplr]._pClass == CLASS_WARRIOR) textdef = TXT_WARBLOOD;
			else if (plr[myplr]._pClass == CLASS_ROGUE) textdef = TXT_ROGBLOOD;
			else if (plr[myplr]._pClass == CLASS_SORCEROR) textdef = TXT_SORBLOOD;
			else if (plr[myplr]._pClass == CLASS_MONK) textdef = TXT_MNKBLOOD;
			else if (plr[myplr]._pClass == CLASS_BARD) textdef = TXT_BRDBLOOD;
			else if (plr[myplr]._pClass == CLASS_BARBARIAN) textdef = TXT_BARBLOOD;
			quests[Q_BLOOD]._qmsg = textdef;
			AddBookLever(0, 0, DMAXX, DMAXY, setpc_x, setpc_y + 3, setpc_x + 2, setpc_y + 7, textdef);
			AddObject(OBJ_PEDISTAL, (setpc_x << 1) + DIRTEDGED2 + 9, (setpc_y << 1) +DIRTEDGED2 + 16);
		}
		InitRndBarrels();
	}

	if (leveltype == 3) {
		AddL3Objs(0, 0, DMAXX, DMAXY);
		InitRndBarrels();
	}

	if (leveltype == 4) {
		if (QuestStatus(Q_WARLORD)) {
			if (plr[myplr]._pClass == CLASS_WARRIOR) textdef = TXT_WARLORD;
			else if (plr[myplr]._pClass == CLASS_ROGUE) textdef = TXT_ROGLORD;
			else if (plr[myplr]._pClass == CLASS_SORCEROR) textdef = TXT_SORLORD;
			else if (plr[myplr]._pClass == CLASS_MONK) textdef = TXT_MNKLORD;
			else if (plr[myplr]._pClass == CLASS_BARD) textdef = TXT_BRDLORD;
			else if (plr[myplr]._pClass == CLASS_BARBARIAN) textdef = TXT_BARLORD;
			quests[Q_WARLORD]._qmsg = textdef;
			AddBookLever(0, 0, DMAXX, DMAXY, setpc_x, setpc_y, setpc_x+setpc_w, setpc_y+setpc_h, textdef);
			setp = LoadFileInMemSig("Levels\\L4Data\\Warlord.DUN",NULL,'STPC');	
			LoadMapObjs(setp, (setpc_x << 1), (setpc_y << 1));
			DiabloFreePtr(setp);
		}
		if (QuestStatus(Q_BETRAYER) && (gbMaxPlayers == 1)) {
			AddLazStand();
		}
		InitRndBarrels();
		AddL4Goodies();
	}

	InitRndLocObj(5, 10, OBJ_CHEST1);
	InitRndLocObj(3, 6, OBJ_CHEST2);
	InitRndLocObj(1, 5, OBJ_CHEST3);
	if (leveltype != 4) AddObjTraps();
	if (leveltype > 1) AddChestTraps();

	InitObjFlag = FALSE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SetMapObjects(BYTE *pMap, int startx, int starty)
{
	int i,j,rw,rh;
	int ox,oy;
	BYTE *lm, *h;
	long mapoff;
	int ot;
	bool fileload[MAXOBJFILES];
	char filestr[32];

	ClrAllObjects();
	InitObjFlag = TRUE;

	for (i = 0; i < MAXOBJFILES; ++i) fileload[i] = FALSE;

	// Load all must obj gfx for leveltype
	for (i = 0; AllObjects[i].oload != -1; ++i) {
		if ((AllObjects[i].oload == OBJMUST) && (leveltype == AllObjects[i].olvltype))
			fileload[AllObjects[i].ofindex] = true;
	}

	lm = pMap;
	rw = *lm;
	lm += 2;
	rh = *lm;
	// Skip map + height word
	mapoff = ((rw * rh) << 1) + 2;
	// Convert to index mini tile level instead of mega
	rw = rw << 1;
	rh = rh << 1;
	// Skip treasure and monster map
	mapoff += ((rw * rh) << 2);
	lm += mapoff;

	h = lm;
	for (j = 0; j < rh; ++j) {
		for (i = 0; i < rw; ++i) {
			if (*lm != 0) {
				ot = *lm;
				ot = ObjTypeConv[ot];
				fileload[AllObjects[ot].ofindex] = TRUE;
			}
			lm+=2;
		}
	}

	app_assert(numobjfiles == 0);
	for (i = 0; i < MAXOBJFILES; ++i) {
		if (fileload[i]) {
			ObjFileList[numobjfiles] = i;
			sprintf(filestr, "Objects\\%s.CEL", ObjMasterFList[i]);
			app_assert(! pObjCels[numobjfiles]);
			pObjCels[numobjfiles] = LoadFileInMemSig(filestr,NULL,'OGFX');
			++numobjfiles;
		}
	}

	lm = h;
	for (j = 0; j < rh; ++j) {
		for (i = 0; i < rw; ++i) {
			if (*lm != 0) {
				ot = *lm;
				ox = i + DIRTEDGED2 + startx;
				oy = j + DIRTEDGED2 + starty;
				AddObject(ObjTypeConv[ot], ox, oy);
			}
			lm+=2;
		}
	}

	InitObjFlag = FALSE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void DeleteObject(int oi, int i)
{
	int		ox,oy;

	app_assert((DWORD)oi < MAXOBJECTS);
	ox = object[oi]._ox;
	oy = object[oi]._oy;
	dObject[ox][oy] = 0;

	objectavail[MAXOBJECTS - numobjects] = oi;
	--numobjects;
	if ((numobjects > 0) && (i != numobjects)) {
		objectactive[i] = objectactive[numobjects];
	} 
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void SetupObject(int i, int x, int y, int ot)
{
	int ai, j;

	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._otype = ot;
	object[i]._ox = x;
	object[i]._oy = y;

	ai = AllObjects[ot].ofindex;
	for (j = 0; ObjFileList[j] != ai; ++j);
	object[i]._oAnimData = pObjCels[j];

	object[i]._oAnimFlag = AllObjects[ot].oAnimFlag;
	if (object[i]._oAnimFlag) {
		object[i]._oAnimDelay = AllObjects[ot].oAnimDelay;
		object[i]._oAnimCnt = random(146,AllObjects[ot].oAnimDelay);
		object[i]._oAnimLen = AllObjects[ot].oAnimLen;
		object[i]._oAnimFrame = random(146,AllObjects[ot].oAnimLen-1) + 1;
	} else {
		object[i]._oAnimDelay = 1000;
		object[i]._oAnimCnt = 0;
		object[i]._oAnimLen = AllObjects[ot].oAnimLen;
		object[i]._oAnimFrame = AllObjects[ot].oAnimDelay;
	}

	object[i]._oAnimWidth = AllObjects[ot].oAnimWidth;
	object[i]._oSolidFlag = AllObjects[ot].oSolidFlag;
	object[i]._oMissFlag = AllObjects[ot].oMissFlag;
	object[i]._oLight = AllObjects[ot].oLightFlag;
	object[i]._oDelFlag = FALSE;
	object[i]._oBreak = AllObjects[ot].oBreak;
	object[i]._oSelFlag = AllObjects[ot].oSelFlag;
	object[i]._oPreFlag = FALSE;
	object[i]._oTrapFlag = FALSE;
	object[i]._oDoorFlag = FALSE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SetObjMapRange(int i, int x1, int y1, int x2, int y2, int v)
{
	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oVar1 = x1;
	object[i]._oVar2 = y1;
	object[i]._oVar3 = x2;
	object[i]._oVar4 = y2;
	object[i]._oVar8 = v;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void SetBookMsg(int i, int msg)
{
	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oVar7 = msg;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddL1Door(int i, int x, int y, int ot)
{
	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oDoorFlag = TRUE;
	if (ot == OBJ_L1DOORL) {
		object[i]._oVar1 = dPiece[x][y];
		object[i]._oVar2 = dPiece[x][y-1];
	} else {
		object[i]._oVar1 = dPiece[x][y];
		object[i]._oVar2 = dPiece[x-1][y];
	}
	object[i]._oVar4 = DOOR_CLOSED;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddSCambBook(int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
//	if (quests[Q_SCHAMB]._qactive != QUEST_NOTACTIVE) {
//		object[i]._oSelFlag = OSEL_NONE;
//		object[i]._oAnimFrame = 6;
//	}
	object[i]._oVar1 = setpc_x;
	object[i]._oVar2 = setpc_y;
	object[i]._oVar3 = setpc_x+setpc_w+1;
	object[i]._oVar4 = setpc_y+setpc_h+1;
	object[i]._oVar6 = object[i]._oAnimFrame + 1;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddChest(int i, int t)
{
	app_assert((DWORD)i < MAXOBJECTS);
	if (random(147,2) == 0) object[i]._oAnimFrame += 3;		// Left or right
	object[i]._oRndSeed = GetRndSeed();
	// Set # chest items. If a setlevel then so to max items.
	switch(t) {
		case OBJ_CHEST1:
		case OBJ_TCHEST1:
			if (setlevel) object[i]._oVar1 = 1;
			else object[i]._oVar1 = random(147,2);
			break;
		case OBJ_CHEST2:
		case OBJ_TCHEST2:
			if (setlevel) object[i]._oVar1 = 2;
			else object[i]._oVar1 = random(147,3);
			break;
		case OBJ_CHEST3:
		case OBJ_TCHEST3:
			if (setlevel) object[i]._oVar1 = 3;
			else object[i]._oVar1 = random(147,4);
			break;
	}
	object[i]._oVar2 = random(147,8);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddL2Door(int i, int x, int y, int ot)
{
	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oDoorFlag = TRUE;
	if (ot == OBJ_L2DOORL) ObjSetMicro(x, y, 538);
	else ObjSetMicro(x, y, 540);
	object[i]._oVar4 = DOOR_CLOSED;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddL3Door(int i, int x, int y, int ot)
{
	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oDoorFlag = TRUE;
	if (ot == OBJ_L3DOORL) ObjSetMicro(x, y, 531);
	else ObjSetMicro(x, y, 534);
	object[i]._oVar4 = DOOR_CLOSED;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddSarc(int i)
{
	int x,y;

	app_assert((DWORD)i < MAXOBJECTS);
	x = object[i]._ox;
	y = object[i]._oy - 1;
	dObject[x][y] = -1 - (char)i;
	object[i]._oVar1 = random(153,10);
	object[i]._oRndSeed = GetRndSeed();
	if (object[i]._oVar1 >= 8) object[i]._oVar2 = PreSpawnSkeleton();
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddFlameTrap(int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oVar1 = trapid;		// Set trap id
	object[i]._oVar2 = 0;			// Trap active
	object[i]._oVar3 = trapdir;		// Horizontal, Vertical, or none
	object[i]._oVar4 = 0;			// Init to off
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddFlameLvr(int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oVar1 = trapid;			// Set trap id
	object[i]._oVar2 = OBJ_FLAMEHOLE;	// Trap active
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddTrap(int i, int ot)
{
	int mt, tdiff;

	// Difficulty
	tdiff = (currlevel / 3) + 1;
// Temp hack trap JKE 7/30
	if (currlevel > 16) tdiff = ((currlevel-4)/3) + 1;
	if (currlevel > 20) tdiff = ((currlevel-8)/3) + 1;

	// Missile type
	mt = random(148,tdiff);
	app_assert((DWORD)i < MAXOBJECTS);
	if (mt == 0) object[i]._oVar3 = MIT_ARROW;
	if (mt == 1) object[i]._oVar3 = MIT_FIREBOLT;
	if (mt == 2) object[i]._oVar3 = MIT_LIGHTCTRL;
	object[i]._oVar4 = 0;			// Trap active
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddObjLight(int i, int r)
{
	app_assert((DWORD)i < MAXOBJECTS);
	if (InitObjFlag) {
		DoLighting(object[i]._ox, object[i]._oy, r, -1);
		object[i]._oVar1 = -1;
	} else object[i]._oVar1 = 0;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddBarrel(int i, int ot)
{
	app_assert((DWORD)i < MAXOBJECTS);
	//object[i]._oVar1 = random(149,5) + 5;			// Barrel hit points
	object[i]._oVar1 = 0;
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oVar2 = random(149,10);		// What is inside
	object[i]._oVar3 = random(149,3);		// if item, useful or not
	if (object[i]._oVar2 >= 8)
		object[i]._oVar4 = PreSpawnSkeleton();	// Get monster index if one will pop out
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddShrine(int i)
{
	int st, j;
	bool slist[NUMSHRINETYPES];

	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oPreFlag = TRUE;
	for (j = 0; j < NUMSHRINETYPES; ++j) {
		if ((currlevel >= shrineminlvl[j]) && (currlevel <= shrinemaxlvl[j]))
			slist[j] = true;
		else
			slist[j] = false;
		if ((gbMaxPlayers != 1) && (shrineavail[j] == SHRINE_SINGLE)) slist[j] = false;
		if ((gbMaxPlayers == 1) && (shrineavail[j] == SHRINE_MULTI)) slist[j] = false;
	}

	// Choose shrine type
	do {
		st = random(150,NUMSHRINETYPES); 
	} while (!slist[st]);

	object[i]._oVar1 = st;

	if (random(150,2)) {
		object[i]._oAnimFrame = 12;
		object[i]._oAnimLen = 22;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddBookcase(int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oPreFlag = TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddBookstand(int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oRndSeed = GetRndSeed();
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddBloodFtn(int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oRndSeed = GetRndSeed();
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddPurifyingFountain(int i)
{
	int x, y;

	app_assert((DWORD)i < MAXOBJECTS);
	x = object[i]._ox;
	y = object[i]._oy;
	dObject[x][y-1] = -1 - (char)i;
	dObject[x-1][y] = -1 - (char)i;
	dObject[x-1][y-1] = -1 - (char)i;
	object[i]._oRndSeed = GetRndSeed();
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddArmorStand(int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	if (!armorFlag) {
		//A functional armor stand has already been placed
		//Only place non-functional empty stands
		object[i]._oAnimFlag = 2;
		object[i]._oSelFlag = OSEL_NONE;
	} 
	object[i]._oRndSeed = GetRndSeed();
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddGoatShrine(int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oRndSeed = GetRndSeed();
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddCauldron(int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oRndSeed = GetRndSeed();
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddMurkyFountain(int i)
{
	int x, y;

	app_assert((DWORD)i < MAXOBJECTS);
	x = object[i]._ox;
	y = object[i]._oy;
	dObject[x][y-1] = -1 - (char)i;
	dObject[x-1][y] = -1 - (char)i;
	dObject[x-1][y-1] = -1 - (char)i;
	object[i]._oRndSeed = GetRndSeed();
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddTearFountain(int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oRndSeed = GetRndSeed();
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddDecap(int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oAnimFrame = random(151,8) + 1;
	object[i]._oPreFlag = TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
const void AddVilebook(int i)
{
	if ((setlevel) && (setlvlnum == SL_VILEBETRAYER)) {
		app_assert((DWORD)i < MAXOBJECTS);
		object[i]._oAnimFrame = 4;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
const void AddMagicCircle(int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oPreFlag = TRUE;
	object[i]._oVar6 = OBJNOWARP;
	object[i]._oVar5 = OBJWARP1;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

const void AddBrnCross(int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oRndSeed = GetRndSeed();
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

const void AddPedistal(int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oVar1 = setpc_x;
	object[i]._oVar2 = setpc_y;
	object[i]._oVar3 = setpc_x + setpc_w;
	object[i]._oVar4 = setpc_y + setpc_h;
}
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int const StoryText[3][3] = {
	{ TXT_BOOK11, TXT_BOOK12, TXT_BOOK13 },
	{ TXT_BOOK21, TXT_BOOK22, TXT_BOOK23 },
	{ TXT_BOOK31, TXT_BOOK32, TXT_BOOK33 } };

static void AddStoryBook(int i)
{
	// Use the last level seed to determine book types (which of 3)
	SetRndSeed(glSeedTbl[16]);
	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oVar1 = random(0, 3);
	if (currlevel == STORY_BOOK1_LEVEL) object[i]._oVar2 = StoryText[object[i]._oVar1][0];
	else if (currlevel == STORY_BOOK2_LEVEL)	object[i]._oVar2 = StoryText[object[i]._oVar1][1];
	else if (currlevel == STORY_BOOK3_LEVEL) object[i]._oVar2 = StoryText[object[i]._oVar1][2];
	object[i]._oVar3 = (object[i]._oVar1 * 3) + (currlevel >> 2) - 1;

	object[i]._oAnimFrame = 5 - (object[i]._oVar1 << 1);
	object[i]._oVar4 = object[i]._oAnimFrame + 1;
}

static void addskulkenbook (int i, int number)	// number is the text to associate JKE
{
	app_assert((DWORD)i < MAXOBJECTS);
	if (number > 5)
	{	
		object[i]._oVar8 = number;
		switch (object[i]._oVar8)
		{
			case 6:
//				object[i]._oVar2 = TXT_SPELL1;

				if (plr[myplr]._pClass == CLASS_WARRIOR) object[i]._oVar2 = TXT_SPELL1;
				else if (plr[myplr]._pClass == CLASS_ROGUE) object[i]._oVar2 = TXT_R_SPELL1;
				else if (plr[myplr]._pClass == CLASS_SORCEROR) object[i]._oVar2 = TXT_S_SPELL1;
				else if (plr[myplr]._pClass == CLASS_MONK) object[i]._oVar2 = TXT_M_SPELL1;
				else if (plr[myplr]._pClass == CLASS_BARD) object[i]._oVar2 = TXT_B_SPELL1;
				else if (plr[myplr]._pClass == CLASS_BARBARIAN) object[i]._oVar2 = TXT_C_SPELL1;
				break;

			case 7:
//				object[i]._oVar2 = TXT_SPELL2;
				
				if (plr[myplr]._pClass == CLASS_WARRIOR) object[i]._oVar2 = TXT_SPELL2;
				else if (plr[myplr]._pClass == CLASS_ROGUE) object[i]._oVar2 = TXT_R_SPELL2;
				else if (plr[myplr]._pClass == CLASS_SORCEROR) object[i]._oVar2 = TXT_S_SPELL2;
				else if (plr[myplr]._pClass == CLASS_MONK) object[i]._oVar2 = TXT_M_SPELL2;
				else if (plr[myplr]._pClass == CLASS_BARD) object[i]._oVar2 = TXT_B_SPELL2;
				else if (plr[myplr]._pClass == CLASS_BARBARIAN) object[i]._oVar2 = TXT_C_SPELL2;
				break;

			case 8:
//				object[i]._oVar2 = TXT_SPELL3;

				if (plr[myplr]._pClass == CLASS_WARRIOR) object[i]._oVar2 = TXT_SPELL3;
				else if (plr[myplr]._pClass == CLASS_ROGUE) object[i]._oVar2 = TXT_R_SPELL3;
				else if (plr[myplr]._pClass == CLASS_SORCEROR) object[i]._oVar2 = TXT_S_SPELL3;
				else if (plr[myplr]._pClass == CLASS_MONK) object[i]._oVar2 = TXT_M_SPELL3;
				else if (plr[myplr]._pClass == CLASS_BARD) object[i]._oVar2 = TXT_B_SPELL3;
				else if (plr[myplr]._pClass == CLASS_BARBARIAN) object[i]._oVar2 = TXT_C_SPELL3;
				break;
		}

		object[i]._oVar1 = 1;
		object[i]._oVar3 = 15;

		object[i]._oAnimFrame = 5 - ((object[i]._oVar1) << 1);
		object[i]._oVar4 = object[i]._oAnimFrame + 1;
	}
	else
	{
		object[i]._oVar1 = 1;
		object[i]._oVar2 = TXT_SKULLJRNL1 + number;

		object[i]._oVar3 = 9 + number;

		object[i]._oAnimFrame = 5 - (object[i]._oVar1 << 1);
		object[i]._oVar4 = object[i]._oAnimFrame + 1;
		object[i]._oVar8 = 0;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddWeaponRack(int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	if (!weaponFlag) {
		//A functional weapon rack has already been placed
		//Only place non-functional empty racks
		object[i]._oAnimFlag = 2;
		object[i]._oSelFlag = OSEL_NONE;
	} 
	object[i]._oRndSeed = GetRndSeed();
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddTorturedBody(int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oAnimFrame = random(0, 4) + 1;
	object[i]._oPreFlag = TRUE;
}

/*-----------------------------------------------------------------------*
 * GetRndObjLoc
 *
 *	Gets a x by y empty location for object placement
**-----------------------------------------------------------------------*/

static void GetRndObjLoc(int randarea, int &xx, int &yy)
{
	int	i,j;
	bool failed;

	if (randarea) {
		int tries = 0;
		do {
			if (++tries > 1000 && randarea > 1)
				--randarea;
			xx = random(0, DMAXX);
			yy = random(0, DMAXY);
			failed = false;
			for (i = 0; i < randarea && !failed; ++i)
				for (j = 0; j < randarea && !failed; ++j)
					failed = !RndLocOk(xx + i, yy + j);
		} while (failed);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddMushPatch()
{
	int x,y;
	if (numobjects < MAXOBJECTS) {
		int i = objectavail[0];

		GetRndObjLoc(5, x, y);
		dObject[x+1][y+1] = -1 - (char)i;
		dObject[x+2][y+1] = -1 - (char)i;
		dObject[x+1][y+2] = -1 - (char)i;
		AddObject(OBJ_MUSHPATCH, x+2, y+2);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void AddSlainHero()
{
	int	x, y;

	GetRndObjLoc(5, x, y);
	AddObject(OBJ_SLAINHERO, x+2, y+2);
}


void AddSkulkenObject(int ot, int number, int ox, int oy)
{
	int oi;		// Standard setup  JKE

	if (numobjects < MAXOBJECTS) {
		oi = objectavail[0];
		objectavail[0] = objectavail[MAXOBJECTS - numobjects - 1];
		objectactive[numobjects] = oi;
		dObject[ox][oy] = (char)oi + 1;
		// Standard init
		SetupObject(oi, ox, oy, ot);
		
		addskulkenbook(oi, number);

		app_assert((DWORD)oi < MAXOBJECTS);
		object[oi]._oAnimWidth2 = (object[oi]._oAnimWidth - 64) >> 1;
		++numobjects;
	}
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void AddObject(int ot, int ox, int oy)
{
	int oi;

	if (numobjects < MAXOBJECTS) {
		oi = objectavail[0];
		objectavail[0] = objectavail[MAXOBJECTS - numobjects - 1];
		objectactive[numobjects] = oi;
		dObject[ox][oy] = (char)oi + 1;
		// Standard init
		SetupObject(oi, ox, oy, ot);
		// Bonus inits
		switch (ot) {
			case OBJ_L1LIGHT :
				//AddObjLight(oi, 10);
				AddObjLight(oi, 5);
				break;
			case OBJ_CANDLE1 :
			case OBJ_CANDLE2 :
			case OBJ_SKFIRE :
			case OBJ_BOOKCANDLE:
				AddObjLight(oi, 5);
				break;
			case OBJ_STORYCANDLE:
				AddObjLight(oi, 3);
				break;
			case OBJ_TORCHL:
			case OBJ_TORCHR:
			case OBJ_TORCHL2:
			case OBJ_TORCHR2:
				AddObjLight(oi, 8);
				break;
			case OBJ_L1DOORL :
			case OBJ_L1DOORR :
				AddL1Door(oi, ox, oy, ot);
				break;
			case OBJ_L2DOORL :
			case OBJ_L2DOORR :
				AddL2Door(oi, ox, oy, ot);
				break;
			case OBJ_L3DOORL :
			case OBJ_L3DOORR :
				AddL3Door(oi, ox, oy, ot);
				break;
			case OBJ_BOOK2R:
				AddSCambBook(oi);
				break;
			case OBJ_CHEST1:
			case OBJ_CHEST2:
			case OBJ_CHEST3:
			case OBJ_TCHEST1:
			case OBJ_TCHEST2:
			case OBJ_TCHEST3:
				AddChest(oi, ot);
				break;
			case OBJ_SARC:
				AddSarc(oi);
				break;
			case OBJ_FLAMEHOLE:
				AddFlameTrap(oi);
				break;
			case OBJ_FLAMELVR:
				AddFlameLvr(oi);
				break;
			case OBJ_WATER:
				app_assert((DWORD)oi < MAXOBJECTS);
				object[oi]._oAnimFrame = 1;
				break;
			case OBJ_TRAPL:
			case OBJ_TRAPR:
				AddTrap(oi, ot);
				break;
			case OBJ_BARREL:
			case OBJ_BARRELEX:
				AddBarrel(oi, ot);
				break;
			case OBJ_SHRINEL:
			case OBJ_SHRINER:
				AddShrine(oi);
				break;
			case OBJ_BOOKCASEL:
			case OBJ_BOOKCASER:
				AddBookcase(oi);
				break;
			case OBJ_SKELBOOK:
			case OBJ_BOOKSTAND:
				AddBookstand(oi);
				break;
			case OBJ_BLOODFTN:
				AddBloodFtn(oi);
				break;
			case OBJ_DECAP:
				AddDecap(oi);
				break;
			case OBJ_PURIFYINGFTN:
				AddPurifyingFountain(oi);
				break;
			case OBJ_ARMORSTAND:
			case OBJ_WARARMOR:
				AddArmorStand(oi);
				break;
			case OBJ_GOATSHRINE:
				AddGoatShrine(oi);
				break;
			case OBJ_CAULDRON:
				AddCauldron(oi);
				break;
			case OBJ_MURKYFTN:
				AddMurkyFountain(oi);
				break;
			case OBJ_TEARFTN:
				AddTearFountain(oi);
				break;
			case OBJ_BOOK2L:
				AddVilebook(oi);
				break;
			case OBJ_MCIRCLE1:
			case OBJ_MCIRCLE2:
				AddMagicCircle(oi);
				break;
			case OBJ_STORYBOOK:
				AddStoryBook(oi);
				break;
			case OBJ_TBCROSS:
			case OBJ_BCROSS:
				AddBrnCross(oi);
				AddObjLight(oi, 5);
				break;
			case OBJ_PEDISTAL:
				AddPedistal(oi);
				break;
			case OBJ_WARWEAP:
			case OBJ_WEAPONRACK:
				AddWeaponRack(oi);
				break;
			case OBJ_TNUDEM2:
				AddTorturedBody(oi);
				break;
		}
		app_assert((DWORD)oi < MAXOBJECTS);
		object[oi]._oAnimWidth2 = (object[oi]._oAnimWidth - 64) >> 1;
		++numobjects;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void Obj_Light(int i, int lr)
{
	int		ox, oy;
	int		dx, dy, p, tr;
	bool	turnon = false;

	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oVar1 == -1) return;

	ox = object[i]._ox;
	oy = object[i]._oy;
	tr = lr + PLRLRAD;
	if (lightflag == 0) {
		for (p = 0; (p < MAX_PLRS) && (!turnon); ++p) {
			if (!plr[p].plractive) continue;
			if (currlevel != plr[p].plrlevel) continue;
			dx = abs(plr[p]._px - ox);
			dy = abs(plr[p]._py - oy);
			if ((dx < tr) && (dy < tr)) turnon = true;
		}
	}
	if (turnon) {
		if (object[i]._oVar1 == 0) object[i]._olid = AddLight (ox, oy, lr);
		object[i]._oVar1 = 1;
	} else {
		if (object[i]._oVar1 == 1) AddUnLight(object[i]._olid);
		object[i]._oVar1 = 0;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void Obj_Circle(int i)
{
	int px, py, ox, oy;
	int v1, v2, v3;
	
	app_assert((DWORD)i < MAXOBJECTS);
	ox = object[i]._ox;
	oy = object[i]._oy;
	px = plr[myplr]._px;
	py = plr[myplr]._py;
	if ((px == ox) && (py == oy)) {
		if (object[i]._otype == OBJ_MCIRCLE1) object[i]._oAnimFrame = 2; 
		if (object[i]._otype == OBJ_MCIRCLE2) object[i]._oAnimFrame = 4;
		if ((ox == 45) && (oy == 47)) object[i]._oVar6 = OBJWARP2;
		else if ((ox == 26) && (oy == 46)) object[i]._oVar6 = OBJWARP1;
		else object[i]._oVar6 = OBJNOWARP;
		if ((ox == 35) && (oy == 36) && object[i]._oVar5 == OBJWARP3) {
			object[i]._oVar6 = OBJDONEWARP;
			v1 = 35;
			v2 = 46;
			ObjChangeMapResync(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
			if (quests[Q_BETRAYER]._qactive == QUEST_NOTDONE) 
				quests[Q_BETRAYER]._qvar1 = 4;
			v3 = dPiece[v1][v2];
			//Cast phase
			AddMissile(plr[myplr]._px, plr[myplr]._py, v1, v2, plr[myplr]._pdir, MIT_PHASE, MI_ENEMYMONST, myplr, 0, 0);
			void TrackInit(BOOL bMouseDown);
			TrackInit(FALSE);
// drb.patch1.start.1/24/97
// 			static BYTE sgbMouseDown;
			extern BYTE sgbMouseDown;
// drb.patch1.end.1/24/97
			sgbMouseDown = FALSE;
			ReleaseCapture();
			ClrPlrPath(myplr);
			void StartStand(int, int);
			StartStand(myplr, 0);
		}
	}
	else {
		if (object[i]._otype == OBJ_MCIRCLE1) object[i]._oAnimFrame = 1; 
		if (object[i]._otype == OBJ_MCIRCLE2) object[i]._oAnimFrame = 3;
		object[i]._oVar6 = OBJNOWARP;
	}
}
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void Obj_StopAnim(int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oAnimFrame == object[i]._oAnimLen) {
		object[i]._oAnimCnt = 0;
		object[i]._oAnimDelay = 1000;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void Obj_Door(int i)
{
	int dx, dy;
	bool dok;

	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oVar4 == DOOR_CLOSED) {
		object[i]._oSelFlag = OSEL_ALL;
		object[i]._oMissFlag = FALSE;
	} else {
		dx = object[i]._ox;
		dy = object[i]._oy;
		dok = (dMonster[dx][dy] == 0);
		dok = dok && (dItem[dx][dy] == 0);
		dok = dok && (dDead[dx][dy] == 0);
		dok = dok && (dPlayer[dx][dy] == 0);
		object[i]._oSelFlag = OSEL_TOP;
		if (dok) object[i]._oVar4 = DOOR_OPEN;
		else object[i]._oVar4 = DOOR_BLOCKED;
		object[i]._oMissFlag = TRUE;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void Obj_Sarc(int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oAnimFrame == object[i]._oAnimLen) object[i]._oAnimFlag = FALSE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void ActivateTrapLine(int ttype, int tid)
{
	int i, oi;

	for (i = 0; i < numobjects; ++i) {
		oi = objectactive[i];
		if ((object[oi]._otype == ttype) && (object[oi]._oVar1 == tid)) {
			object[oi]._oVar4 = 1;
			object[oi]._oAnimFlag = TRUE;
			object[oi]._oAnimDelay = 1;
			object[oi]._olid = AddLight (object[oi]._ox, object[oi]._oy, 1);
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void Obj_FlameTrap(int i)
{
	int xp,yp;
	int j;

	// Is trap active?
	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oVar2 != 0) {
		// Not active, try dying down
		if (object[i]._oVar4 != 0) {
			--object[i]._oAnimFrame;
			if (object[i]._oAnimFrame == 1) {
				object[i]._oVar4 = 0;
				AddUnLight(object[i]._olid);
			} else {
				if (object[i]._oAnimFrame <= 4) ChangeLightRadius(object[i]._olid, object[i]._oAnimFrame);
			}
		}
	} else {
		// Active so continue
		// Not on?
		if (object[i]._oVar4 == 0) {
			if (object[i]._oVar3 == TRAP_VERT) {
				xp = object[i]._ox-2;
				yp = object[i]._oy;
				for (j = 0; j < 5; ++j) {
					if ((dPlayer[xp][yp] != 0) || (dMonster[xp][yp] != 0))
						object[i]._oVar4 = 1;
					++xp;
				}
			} else {
				xp = object[i]._ox;
				yp = object[i]._oy-2;
				for (j = 0; j < 5; ++j) {
					if ((dPlayer[xp][yp] != 0) || (dMonster[xp][yp] != 0))
						object[i]._oVar4 = 1;
					++yp;
				}
			}
			if (object[i]._oVar4 != 0) ActivateTrapLine(object[i]._otype, object[i]._oVar1);
		} else {
			if (object[i]._oAnimFrame == object[i]._oAnimLen) object[i]._oAnimFrame = 11;
			if (object[i]._oAnimFrame <= 5) ChangeLightRadius(object[i]._olid, object[i]._oAnimFrame);
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void Obj_Trap(int i)
{
	int oti;
	bool otrig = false;
	int sx, sy, dx, dy;
	int x, y;
	int ax, ay;
	int mdir;

	// Triggered?
	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oVar4 == 0) {
		oti = dObject[object[i]._oVar1][object[i]._oVar2] - 1;
		switch (object[oti]._otype) {
			case OBJ_CHEST1:
			case OBJ_CHEST2:
			case OBJ_CHEST3:
			case OBJ_SWITCHSKL:
			case OBJ_LEVER :
			case OBJ_SARC:
				if (object[oti]._oSelFlag == OSEL_NONE) otrig = true;
				break;
			case OBJ_L1DOORL :
			case OBJ_L1DOORR :
			case OBJ_L2DOORL :
			case OBJ_L2DOORR :
			case OBJ_L3DOORL :
			case OBJ_L3DOORR :
				if (object[oti]._oVar4 != DOOR_CLOSED) otrig = true;
				break;
		}
		if (otrig) {
			object[i]._oVar4 = 1;
			sx = object[i]._ox;
			sy = object[i]._oy;
			dx = object[oti]._ox;
			dy = object[oti]._oy;
			ax = dx;
			ay = dy;
			for (y = ay-1; y <= ay+1; ++y) {
				for (x = ax-1; x <= ax+1; ++x) {
					if (dPlayer[x][y] != 0) {
						dx = x;
						dy = y;
					}
				}
			}
			if (!deltaload) {
				mdir = GetDirection(sx, sy, dx, dy);
				AddMissile(sx, sy, dx, dy, mdir, object[i]._oVar3, MI_ENEMYPLR, -1, 0, 0);
				PlaySfxLoc(IS_TRAP, object[oti]._ox, object[oti]._oy);
			}
			object[oti]._oTrapFlag = FALSE;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void Obj_BCrossDamage(int i) 
{
	int		resist;
	int		damage[4] = { 6, 8, 10, 12 };

	if (plr[myplr]._pmode != PM_DEATH) {
		//Check for fire resistance. If resistance, adjust damage accordingly.
		resist = plr[myplr]._pFireResist;
		if (resist > 0) damage[leveltype-1] -= (damage[leveltype-1] * resist) / 100;
		//Is the player in the fire?
		app_assert((DWORD)i < MAXOBJECTS);
		if ((plr[myplr]._px == object[i]._ox) && (plr[myplr]._py == object[i]._oy-1)) {
			//Subtract damage
			plr[myplr]._pHitPoints -= damage[leveltype-1];
			plr[myplr]._pHPBase -= damage[leveltype-1];
			//Check if player is dead
			if ((plr[myplr]._pHitPoints >> HP_SHIFT) <= 0) {
				// rjs - manashld fix? - SetPlayerHitPoints(myplr, 0);
				StartPlrKill(myplr, FALSE);
			} else {
				//Play pain sfx
				if (plr[myplr]._pClass == CLASS_WARRIOR) PlaySfxLoc(PS_WARR68, plr[myplr]._px, plr[myplr]._py);
				#if !IS_VERSION(SHAREWARE)
				else if (plr[myplr]._pClass == CLASS_ROGUE) PlaySfxLoc(PS_ROGUE68, plr[myplr]._px, plr[myplr]._py);
				else if (plr[myplr]._pClass == CLASS_SORCEROR) PlaySfxLoc(PS_MAGE68, plr[myplr]._px, plr[myplr]._py);
				else if (plr[myplr]._pClass == CLASS_MONK) PlaySfxLoc(PS_MONK68, plr[myplr]._px, plr[myplr]._py);
				else if (plr[myplr]._pClass == CLASS_BARD) PlaySfxLoc(PS_BARD68, plr[myplr]._px, plr[myplr]._py);
				else if (plr[myplr]._pClass == CLASS_BARBARIAN) PlaySfxLoc(PS_BARBARIAN68, plr[myplr]._px, plr[myplr]._py);
				#endif
			}
			drawhpflag = TRUE;
		}
	} 
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void ProcessObjects ()
{
	int		i, oi;

	for (i = 0; i < numobjects; ++i) {
		oi = objectactive[i];
		app_assert((DWORD)oi < MAXOBJECTS);
		switch (object[oi]._otype) {
			case OBJ_L1LIGHT :
				Obj_Light(oi, 10);
				break;
			case OBJ_CANDLE2 :
			case OBJ_SKFIRE :
			case OBJ_BOOKCANDLE:
				Obj_Light(oi, 5);
				break;
			case OBJ_STORYCANDLE:
				Obj_Light(oi, 3);
				break;
			case OBJ_CRUX1 :
			case OBJ_CRUX2 :
			case OBJ_CRUX3 :
			case OBJ_BARREL:
			case OBJ_BARRELEX:
			case OBJ_SHRINEL:
			case OBJ_SHRINER:
				Obj_StopAnim(oi);
				break;
			case OBJ_L1DOORL :
			case OBJ_L1DOORR :
			case OBJ_L2DOORL :
			case OBJ_L2DOORR :
			case OBJ_L3DOORL :
			case OBJ_L3DOORR :
				Obj_Door(oi);
				break;
			case OBJ_TORCHL:
			case OBJ_TORCHR:
			case OBJ_TORCHL2:
			case OBJ_TORCHR2:
				Obj_Light(oi, 8);
				break;
			case OBJ_SARC:
				Obj_Sarc(oi);
				break;
			case OBJ_FLAMEHOLE:
				Obj_FlameTrap(oi);
				break;
			case OBJ_TRAPL:
			case OBJ_TRAPR:
				Obj_Trap(oi);
				break;
			case OBJ_MCIRCLE1:
			case OBJ_MCIRCLE2:
				Obj_Circle(oi);
				break;
			case OBJ_TBCROSS:
			case OBJ_BCROSS:
				Obj_Light(oi, 10);
				Obj_BCrossDamage(oi);
				break;
		}

		// Animate Objects
		if (object[oi]._oAnimFlag) {
			++object[oi]._oAnimCnt;
			if (object[oi]._oAnimCnt >= object[oi]._oAnimDelay) {
				object[oi]._oAnimCnt = 0;
				++object[oi]._oAnimFrame;
				if (object[oi]._oAnimFrame > object[oi]._oAnimLen) object[oi]._oAnimFrame = 1;
			}
		}
	}
	i = 0;
	while (i < numobjects) {
		oi = objectactive[i];
		if (object[oi]._oDelFlag) {
			DeleteObject(oi, i);
			i = 0;
		} else ++i;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void ObjSetMicro(int dx, int dy, int pn)
{
	WORD *mtsource;
	int t;
	WORD *mt;

	dPiece[dx][dy] = pn;
	--pn;
	mt = &(dMT2[CalcRot(dx,dy)].mt[0]);
	if (leveltype != 4) {
		mtsource = (WORD *)(pMiniTiles + 20*pn);
		for(t = 0; t < 10; ++t)
			// MiniTiles array uses opposite y direction
			// hence wierd index on next line
			mt[t] = mtsource[8-(t&0xe)+(t&1)];
	} else {
		mtsource = (WORD *)(pMiniTiles + 32*pn);
		for(t = 0; t < 16; ++t)
			// MiniTiles array uses opposite y direction
			// hence wierd index on next line
			mt[t] = mtsource[14-(t&0xe)+(t&1)];
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void ObjSetMicro12(int dx, int dy)
{
	long	mt1,mt2;
	int		pn;

	pn = dPiece[dx][dy] - 1;
	__asm {
		mov		esi,dword ptr [pMiniTiles]
		xor		eax,eax
		mov		ax,word ptr [pn]
		mov		ebx,20
		mul		ebx
		add		esi,eax
		add		esi,16
		xor		eax,eax
		lodsw
		mov		word ptr [mt1],ax
		lodsw
		mov		word ptr [mt2],ax
	}
	dMT2[CalcRot(dx,dy)].mt[0] = mt1 & 0xffff;
	dMT2[CalcRot(dx,dy)].mt[1] = mt2 & 0xffff;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void ObjSetMini(int x, int y, int v)
{
	long v1,v2,v3,v4;
	int xx, yy;

	__asm {
		mov		esi,dword ptr [pMegaTiles]
		xor		eax,eax
		mov		ax,word ptr [v];
		dec		eax
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
	xx = (x << 1) + DIRTEDGED2;
	yy = (y << 1) + DIRTEDGED2;
	ObjSetMicro(xx, yy, v1);
	ObjSetMicro(xx+1, yy, v2);
	ObjSetMicro(xx, yy+1, v3);
	ObjSetMicro(xx+1, yy+1, v4);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void ObjL1Special(int x1, int y1, int x2, int y2)
{
	int i,j;

	for (j = y1; j <= y2; ++j) {
		for (i = x1; i <= x2; ++i) {
			dSpecial[i][j] = 0;
			// Place tops of arches
			if (dPiece[i][j] == 12) dSpecial[i][j] = 1;
			if (dPiece[i][j] == 11) dSpecial[i][j] = 2;
			if (dPiece[i][j] == 71) dSpecial[i][j] = 1;
			if (dPiece[i][j] == 259) dSpecial[i][j] = 5;
			if (dPiece[i][j] == 249) dSpecial[i][j] = 2;
			if (dPiece[i][j] == 325) dSpecial[i][j] = 2;
			if (dPiece[i][j] == 321) dSpecial[i][j] = 1;
			if (dPiece[i][j] == 255) dSpecial[i][j] = 4;
			if (dPiece[i][j] == 211) dSpecial[i][j] = 1;
			if (dPiece[i][j] == 344) dSpecial[i][j] = 2;
			if (dPiece[i][j] == 341) dSpecial[i][j] = 1;
			if (dPiece[i][j] == 331) dSpecial[i][j] = 2;
			if (dPiece[i][j] == 418) dSpecial[i][j] = 1;
			if (dPiece[i][j] == 421) dSpecial[i][j] = 2;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void ObjL2Special(int x1, int y1, int x2, int y2)
{
	int i,j;

	for (j = y1; j <= y2; ++j) {
		for (i = x1; i <= x2; ++i) {
			dSpecial[i][j] = 0;
			if (dPiece[i][j] == 541) dSpecial[i][j] = 5;
			if (dPiece[i][j] == 178) dSpecial[i][j] = 5;
			if (dPiece[i][j] == 551) dSpecial[i][j] = 5;
			if (dPiece[i][j] == 542) dSpecial[i][j] = 6;
			if (dPiece[i][j] == 553) dSpecial[i][j] = 6;
			if (dPiece[i][j] == 13) dSpecial[i][j] = 5;
			if (dPiece[i][j] == 17) dSpecial[i][j] = 6;
		}
	}

	for (j = y1; j <= y2; ++j) {
		for (i = x1; i <= x2; ++i) {
			if (dPiece[i][j] == 132) {
				dSpecial[i][j+1] = 2;
				dSpecial[i][j+2] = 1;
			}
			if ((dPiece[i][j] == 135) || (dPiece[i][j] == 139)) {
				dSpecial[i+1][j] = 3;
				dSpecial[i+2][j] = 4;
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void DoorSet(int oi, int dx, int dy)
{
	int pn;

	app_assert((DWORD)oi < MAXOBJECTS);
	pn = dPiece[dx][dy];
	if (currlevel < HIVESTART)
	{
		if (pn == 43) ObjSetMicro(dx,dy,392);
		if (pn == 45) ObjSetMicro(dx,dy,394);
		if ((pn == 50) && (object[oi]._otype == OBJ_L1DOORL)) ObjSetMicro(dx,dy,411);
		if ((pn == 50) && (object[oi]._otype == OBJ_L1DOORR)) ObjSetMicro(dx,dy,412);
		if (pn == 54) ObjSetMicro(dx,dy,397);
		if (pn == 55) ObjSetMicro(dx,dy,398);
		if (pn == 61) ObjSetMicro(dx,dy,399);
		if (pn == 67) ObjSetMicro(dx,dy,400);
		if (pn == 68) ObjSetMicro(dx,dy,401);
		if (pn == 69) ObjSetMicro(dx,dy,403);
		if (pn == 70) ObjSetMicro(dx,dy,404);
		if (pn == 72) ObjSetMicro(dx,dy,406);
		if (pn == 212) ObjSetMicro(dx,dy,407);		// Blood
		if (pn == 354) ObjSetMicro(dx,dy,409);		// Plain L
		if (pn == 355) ObjSetMicro(dx,dy,410);		// Plain R
		if (pn == 411) ObjSetMicro(dx,dy,396);		// Double open
		if (pn == 412) ObjSetMicro(dx,dy,396);		// Double open
	}
	else
	{

		if (pn == 75) ObjSetMicro(dx,dy,204);
		if (pn == 79) ObjSetMicro(dx,dy,208);
		if ((pn == 86) && (object[oi]._otype == OBJ_L1DOORL)) ObjSetMicro(dx,dy,232);
		if ((pn == 86) && (object[oi]._otype == OBJ_L1DOORR)) ObjSetMicro(dx,dy,234);
		if (pn == 91) ObjSetMicro(dx,dy,215);
		if (pn == 93) ObjSetMicro(dx,dy,218);
		if (pn == 99) ObjSetMicro(dx,dy,220);
		if (pn == 111) ObjSetMicro(dx,dy,222);
		if (pn == 113) ObjSetMicro(dx,dy,224);
		if (pn == 115) ObjSetMicro(dx,dy,226);
		if (pn == 117) ObjSetMicro(dx,dy,228);
		if (pn == 119) ObjSetMicro(dx,dy,230);
		if (pn == 232) ObjSetMicro(dx,dy,212);		// Double open
		if (pn == 234) ObjSetMicro(dx,dy,212);		// Double open
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void RedoPlayerVision()
{
	int p;

	for (p = 0; p < MAX_PLRS; ++p) {
		if (! plr[p].plractive) continue;
		if (currlevel != plr[p].plrlevel) continue;
		ChangeVisionXY(plr[p]._pvid, plr[p]._px, plr[p]._py);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateL1RDoor(int pnum, int oi, bool sendflag)
{
	int		dx, dy;
	bool	dok;

	app_assert((DWORD)oi < MAXOBJECTS);
	if (object[oi]._oVar4 == DOOR_BLOCKED) {
		if (!deltaload) PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, object[oi]._oy);
		return;
	}
	dx = object[oi]._ox;
	dy = object[oi]._oy;
	if (object[oi]._oVar4 == DOOR_CLOSED) {
		if ((pnum == myplr) && sendflag) NetSendCmdParam1(TRUE,CMD_OPENDOOR,oi);
		if (currlevel < CRYPTSTART)
		{
			if (!deltaload) PlaySfxLoc(IS_DOOROPEN, object[oi]._ox, object[oi]._oy);
		}
		else
		{
			if (!deltaload) PlaySfxLoc(CR_DOOROPEN, object[oi]._ox, object[oi]._oy);
		}
		if (currlevel < CRYPTSTART)
			ObjSetMicro(dx,dy,395);
		else
			ObjSetMicro(dx,dy,209);
		if (currlevel < HIVESTART)
			dSpecial[dx][dy] = 8;
		else
			dSpecial[dx][dy] = 2;
		ObjSetMicro12(dx,dy-1);
		--dx;
		object[oi]._oAnimFrame += 2;
#ifdef NO_L5_DOORS
		if (currlevel > HIVESTART)
			object[oi]._oAnimFrame = -1;
#endif

		object[oi]._oPreFlag = TRUE;
		DoorSet(oi, dx, dy);
		object[oi]._oVar4 = DOOR_OPEN;
		object[oi]._oSelFlag = OSEL_TOP;
		RedoPlayerVision();
	} else {
		if (currlevel < CRYPTSTART)
		{
			if (!deltaload) PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, object[oi]._oy);
		}
		else
		{
			if (!deltaload) PlaySfxLoc(CR_DOORCLOS, object[oi]._ox, object[oi]._oy);
		}
		dok = (dMonster[dx][dy] == 0);
		dok = dok && (dItem[dx][dy] == 0);
		dok = dok && (dDead[dx][dy] == 0);
		if (dok) {
			if ((pnum == myplr) && sendflag) NetSendCmdParam1(TRUE,CMD_CLOSEDOOR,oi);
			object[oi]._oVar4 = DOOR_CLOSED;
			object[oi]._oSelFlag = OSEL_ALL;
			ObjSetMicro(dx,dy,object[oi]._oVar1);
			if (currlevel < HIVESTART)
			{
				if (object[oi]._oVar2 != 50) ObjSetMicro(dx-1,dy,object[oi]._oVar2);
				else {
					// Double door
					if (dPiece[dx-1][dy] == 396) ObjSetMicro(dx-1,dy,411);
					else ObjSetMicro(dx-1,dy,object[oi]._oVar2);
				}
			}
			else
			{
				if (object[oi]._oVar2 != 86) ObjSetMicro(dx-1,dy,object[oi]._oVar2);
				else {
					// Double door
					if (dPiece[dx-1][dy] == 210) ObjSetMicro(dx-1,dy,232);
					else ObjSetMicro(dx-1,dy,object[oi]._oVar2);
				}
			}
			object[oi]._oAnimFrame -= 2;
			object[oi]._oPreFlag = FALSE;
			RedoPlayerVision();
		} else object[oi]._oVar4 = DOOR_BLOCKED;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateL1LDoor(int pnum, int oi, bool sendflag)
{
	int		dx, dy;
	bool	dok;

	app_assert((DWORD)oi < MAXOBJECTS);
	if (object[oi]._oVar4 == DOOR_BLOCKED) {
		if (!deltaload) PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, object[oi]._oy);
		return;
	}
	dx = object[oi]._ox;
	dy = object[oi]._oy;
	if (object[oi]._oVar4 == DOOR_CLOSED) {
		if ((pnum == myplr) && sendflag) NetSendCmdParam1(TRUE,CMD_OPENDOOR,oi);
		if (currlevel < CRYPTSTART)
		{
			if (!deltaload) PlaySfxLoc(IS_DOOROPEN, object[oi]._ox, object[oi]._oy);
		}
		else
		{
			if (!deltaload) PlaySfxLoc(CR_DOOROPEN, object[oi]._ox, object[oi]._oy);
		}
		if (currlevel < CRYPTSTART)
		{
			if (object[oi]._oVar1 == 214) 
				ObjSetMicro(dx,dy,408);	//Blood
			else 
				ObjSetMicro(dx,dy,393);
		}
		else
			ObjSetMicro(dx,dy,206);
		if (currlevel < HIVESTART)
			dSpecial[dx][dy] = 7;
		else
			dSpecial[dx][dy] = 1;
		ObjSetMicro12(dx-1,dy);
		--dy;
		object[oi]._oAnimFrame += 2;
#ifdef NO_L5_DOORS
		if (currlevel > HIVESTART)
			object[oi]._oAnimFrame = -1;
#endif
		object[oi]._oPreFlag = TRUE;
		DoorSet(oi, dx, dy);
		object[oi]._oVar4 = DOOR_OPEN;
		object[oi]._oSelFlag = OSEL_TOP;
		RedoPlayerVision();
	} else {
		if (currlevel < CRYPTSTART)
		{
			if (!deltaload) PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, object[oi]._oy);
		}
		else
		{
			if (!deltaload) PlaySfxLoc(CR_DOORCLOS, object[oi]._ox, object[oi]._oy);
		}
		dok = (dMonster[dx][dy] == 0);
		dok = dok && (dItem[dx][dy] == 0);
		dok = dok && (dDead[dx][dy] == 0);
		if (dok) {
			if ((pnum == myplr) && sendflag) NetSendCmdParam1(TRUE,CMD_CLOSEDOOR,oi);
			object[oi]._oVar4 = DOOR_CLOSED;
			object[oi]._oSelFlag = OSEL_ALL;
			ObjSetMicro(dx,dy,object[oi]._oVar1);
			if (currlevel < HIVESTART)
			{
				if (object[oi]._oVar2 != 50) ObjSetMicro(dx,dy-1,object[oi]._oVar2);
				else {
					// Double door
					if (dPiece[dx][dy-1] == 396) ObjSetMicro(dx,dy-1,412);
					else ObjSetMicro(dx,dy-1,object[oi]._oVar2);
				}
			}
			else
			{
				if (object[oi]._oVar2 != 86) ObjSetMicro(dx,dy-1,object[oi]._oVar2);
				else {
					// Double door
					if (dPiece[dx][dy-1] == 210) ObjSetMicro(dx,dy-1,234);
					else ObjSetMicro(dx,dy-1,object[oi]._oVar2);
				}
			}

			object[oi]._oAnimFrame -= 2;
			object[oi]._oPreFlag = FALSE;
			RedoPlayerVision();
		} else object[oi]._oVar4 = DOOR_BLOCKED;
	}
}

/*-----------------------------------------------------------------------*/
// this is the stupidest fix I've ever seen an' I did it! JKE
/*-----------------------------------------------------------------------*/
void OpenCloseAllDoors()
{
	int i;
	BOOL odeltaload = deltaload;
	
	deltaload = TRUE;
	for (i = 0; i < MAXOBJECTS; ++i)
	{
		if (object[i]._otype == OBJ_L1DOORL)
		{
			OperateL1LDoor(myplr, i, FALSE);
			OperateL1LDoor(myplr, i, FALSE);
		}
		else
			if (object[i]._otype == OBJ_L1DOORR)
			{
				OperateL1RDoor(myplr, i, FALSE);
				OperateL1RDoor(myplr, i, FALSE);
			}
	}
	deltaload = odeltaload;
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateL2RDoor(int pnum, int oi, bool sendflag)
{
	int		dx, dy;
	bool	dok;

	app_assert((DWORD)oi < MAXOBJECTS);
	if (object[oi]._oVar4 == DOOR_BLOCKED) {
		if (!deltaload) PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, object[oi]._oy);
		return;
	}
	dx = object[oi]._ox;
	dy = object[oi]._oy;
	if (object[oi]._oVar4 == DOOR_CLOSED) {
		if ((pnum == myplr) && sendflag) NetSendCmdParam1(TRUE,CMD_OPENDOOR,oi);
		if (!deltaload) PlaySfxLoc(IS_DOOROPEN, object[oi]._ox, object[oi]._oy);
		ObjSetMicro(dx,dy,17);
		object[oi]._oAnimFrame += 2;
		object[oi]._oPreFlag = TRUE;
		object[oi]._oVar4 = DOOR_OPEN;
		object[oi]._oSelFlag = OSEL_TOP;
		RedoPlayerVision();
	} else {
		if (!deltaload) PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, object[oi]._oy);
		dok = (dMonster[dx][dy] == 0);
		dok = dok && (dItem[dx][dy] == 0);
		dok = dok && (dDead[dx][dy] == 0);
		if (dok) {
			if ((pnum == myplr) && sendflag) NetSendCmdParam1(TRUE,CMD_CLOSEDOOR,oi);
			object[oi]._oVar4 = DOOR_CLOSED;
			object[oi]._oSelFlag = OSEL_ALL;
			ObjSetMicro(dx,dy,540);
			object[oi]._oAnimFrame -= 2;
			object[oi]._oPreFlag = FALSE;
			RedoPlayerVision();
		} else object[oi]._oVar4 = DOOR_BLOCKED;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

const void OperateL2LDoor(int pnum, int oi, BOOL sendflag)
{
	int		dx, dy;
	bool	dok;

	app_assert((DWORD)oi < MAXOBJECTS);
	if (object[oi]._oVar4 == DOOR_BLOCKED) {
		if (!deltaload) PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, object[oi]._oy);
		return;
	}
	dx = object[oi]._ox;
	dy = object[oi]._oy;
	if (object[oi]._oVar4 == DOOR_CLOSED) {
		if ((pnum == myplr) && sendflag) NetSendCmdParam1(TRUE,CMD_OPENDOOR,oi);
		if (!deltaload) PlaySfxLoc(IS_DOOROPEN, object[oi]._ox, object[oi]._oy);
		ObjSetMicro(dx,dy,13);
		object[oi]._oAnimFrame += 2;
		object[oi]._oPreFlag = TRUE;
		object[oi]._oVar4 = DOOR_OPEN;
		object[oi]._oSelFlag = OSEL_TOP;
		RedoPlayerVision();
	} else {
		if (!deltaload) PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, object[oi]._oy);
		dok = (dMonster[dx][dy] == 0);
		dok = dok && (dItem[dx][dy] == 0);
		dok = dok && (dDead[dx][dy] == 0);
		if (dok) {
			if ((pnum == myplr) && sendflag) NetSendCmdParam1(TRUE,CMD_CLOSEDOOR,oi);
			object[oi]._oVar4 = DOOR_CLOSED;
			object[oi]._oSelFlag = OSEL_ALL;
			ObjSetMicro(dx,dy,538);
			object[oi]._oAnimFrame -= 2;
			object[oi]._oPreFlag = FALSE;
			RedoPlayerVision();
		} else object[oi]._oVar4 = DOOR_BLOCKED;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateL3RDoor(int pnum, int oi, bool sendflag)
{
	int		dx, dy;
	bool	dok;

	app_assert((DWORD)oi < MAXOBJECTS);
	if (object[oi]._oVar4 == DOOR_BLOCKED) {
		if (!deltaload) PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, object[oi]._oy);
		return;
	}
	dx = object[oi]._ox;
	dy = object[oi]._oy;
	if (object[oi]._oVar4 == DOOR_CLOSED) {
		if ((pnum == myplr) && sendflag) NetSendCmdParam1(TRUE,CMD_OPENDOOR,oi);
		if (!deltaload) PlaySfxLoc(IS_DOOROPEN, object[oi]._ox, object[oi]._oy);
		ObjSetMicro(dx,dy,541);
		object[oi]._oAnimFrame += 2;
		object[oi]._oPreFlag = TRUE;
		object[oi]._oVar4 = DOOR_OPEN;
		object[oi]._oSelFlag = OSEL_TOP;
		RedoPlayerVision();
	} else {
		if (!deltaload) PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, object[oi]._oy);
		dok = (dMonster[dx][dy] == 0);
		dok = dok && (dItem[dx][dy] == 0);
		dok = dok && (dDead[dx][dy] == 0);
		if (dok) {
			if ((pnum == myplr) && sendflag) NetSendCmdParam1(TRUE,CMD_CLOSEDOOR,oi);
			object[oi]._oVar4 = DOOR_CLOSED;
			object[oi]._oSelFlag = OSEL_ALL;
			ObjSetMicro(dx,dy,534);
			object[oi]._oAnimFrame -= 2;
			object[oi]._oPreFlag = FALSE;
			RedoPlayerVision();
		} else object[oi]._oVar4 = DOOR_BLOCKED;
	}
}
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void OperateL3LDoor(int pnum, int oi, bool sendflag)
{
	int		dx, dy;
	bool	dok;

	app_assert((DWORD)oi < MAXOBJECTS);
	if (object[oi]._oVar4 == DOOR_BLOCKED) {
		if (!deltaload) PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, object[oi]._oy);
		return;
	}
	dx = object[oi]._ox;
	dy = object[oi]._oy;
	if (object[oi]._oVar4 == DOOR_CLOSED) {
		if ((pnum == myplr) && sendflag) NetSendCmdParam1(TRUE,CMD_OPENDOOR,oi);
		if (!deltaload) PlaySfxLoc(IS_DOOROPEN, object[oi]._ox, object[oi]._oy);
		ObjSetMicro(dx,dy,538);
		object[oi]._oAnimFrame += 2;
		object[oi]._oPreFlag = TRUE;
		object[oi]._oVar4 = DOOR_OPEN;
		object[oi]._oSelFlag = OSEL_TOP;
		RedoPlayerVision();
	} else {
		if (!deltaload) PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, object[oi]._oy);
		dok = (dMonster[dx][dy] == 0);
		dok = dok && (dItem[dx][dy] == 0);
		dok = dok && (dDead[dx][dy] == 0);
		if (dok) {
			if ((pnum == myplr) && sendflag) NetSendCmdParam1(TRUE,CMD_CLOSEDOOR,oi);
			object[oi]._oVar4 = DOOR_CLOSED;
			object[oi]._oSelFlag = OSEL_ALL;
			ObjSetMicro(dx,dy,531);
			object[oi]._oAnimFrame -= 2;
			object[oi]._oPreFlag = FALSE;
			RedoPlayerVision();
		} else object[oi]._oVar4 = DOOR_BLOCKED;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void MonstCheckDoors(int m)
{
	int		i, oi, dpx, dpy;
	int		mx, my;

	mx = monster[m]._mx;
	my = monster[m]._my;
	if (!(dObject[mx-1][my-1]
		  || dObject[mx][my-1]
		  || dObject[mx+1][my-1]
		  || dObject[mx-1][my]
		  || dObject[mx+1][my]
		  || dObject[mx-1][my+1]
		  || dObject[mx][my+1]
		  || dObject[mx+1][my+1]))
		  return;
	for (i = 0; i < numobjects; ++i) {
		oi = objectactive[i];
		app_assert((DWORD)oi < MAXOBJECTS);
		if ((object[oi]._otype == OBJ_L1DOORL) || (object[oi]._otype == OBJ_L1DOORR)) {
			if (object[oi]._oVar4 == DOOR_CLOSED) {
				dpx = abs(object[oi]._ox - mx);
				dpy = abs(object[oi]._oy - my);
				if ((dpx == 1) && (dpy <= 1) && (object[oi]._otype == OBJ_L1DOORL)) OperateL1LDoor(myplr, oi, true);
				if ((dpx <= 1) && (dpy == 1) && (object[oi]._otype == OBJ_L1DOORR)) OperateL1RDoor(myplr, oi, true);
			}
		}
		if ((object[oi]._otype == OBJ_L2DOORL) || (object[oi]._otype == OBJ_L2DOORR)) {
			if (object[oi]._oVar4 == DOOR_CLOSED) {
				dpx = abs(object[oi]._ox - mx);
				dpy = abs(object[oi]._oy - my);
				if ((dpx == 1) && (dpy <= 1) && (object[oi]._otype == OBJ_L2DOORL)) OperateL2LDoor(myplr, oi, true);
				if ((dpx <= 1) && (dpy == 1) && (object[oi]._otype == OBJ_L2DOORR)) OperateL2RDoor(myplr, oi, true);
			}
		}
		if ((object[oi]._otype == OBJ_L3DOORL) || (object[oi]._otype == OBJ_L3DOORR)) {
			if (object[oi]._oVar4 == DOOR_CLOSED) {
				dpx = abs(object[oi]._ox - mx);
				dpy = abs(object[oi]._oy - my);

				if ((dpx == 1) && (dpy <= 1) && (object[oi]._otype == OBJ_L3DOORR)) OperateL3RDoor(myplr, oi, true);
				if ((dpx <= 1) && (dpy == 1) && (object[oi]._otype == OBJ_L3DOORL)) OperateL3LDoor(myplr, oi, true);
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void ObjChangeMap(int x1, int y1, int x2, int y2)
{
	int i,j;

	for (j = y1; j <= y2; ++j) {
		for (i = x1; i <= x2; ++i) {
			ObjSetMini(i, j, pdungeon[i][j]);
			dungeon[i][j] = pdungeon[i][j];		// For automap
		}
	}
	if ((leveltype == 1)&&(currlevel < HIVESTART)) {		// JKE
		ObjL1Special((x1 << 1) + DIRTEDGED2, (y1 << 1) + DIRTEDGED2, (x2 << 1) + DIRTEDGED2 + 1, (y2 << 1) + DIRTEDGED2 + 1);
		AddL1Objs((x1 << 1) + DIRTEDGED2, (y1 << 1) + DIRTEDGED2, (x2 << 1) + DIRTEDGED2 + 1, (y2 << 1) + DIRTEDGED2 + 1);
	}
	if (leveltype == 2) {
		ObjL2Special((x1 << 1) + DIRTEDGED2, (y1 << 1) + DIRTEDGED2, (x2 << 1) + DIRTEDGED2 + 1, (y2 << 1) + DIRTEDGED2 + 1);
		AddL2Objs((x1 << 1) + DIRTEDGED2, (y1 << 1) + DIRTEDGED2, (x2 << 1) + DIRTEDGED2 + 1, (y2 << 1) + DIRTEDGED2 + 1);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void ObjChangeMapResync(int x1, int y1, int x2, int y2)
{
	int i,j;

	for (j = y1; j <= y2; ++j) {
		for (i = x1; i <= x2; ++i) {
			ObjSetMini(i, j, pdungeon[i][j]);
			dungeon[i][j] = pdungeon[i][j];		// For automap
		}
	}
	
	if ((leveltype == 1)&&(currlevel < HIVESTART))
		ObjL1Special((x1 << 1) + DIRTEDGED2, (y1 << 1) + DIRTEDGED2, (x2 << 1) + DIRTEDGED2 + 1, (y2 << 1) + DIRTEDGED2 + 1);

	if (leveltype == 2)
		ObjL2Special((x1 << 1) + DIRTEDGED2, (y1 << 1) + DIRTEDGED2, (x2 << 1) + DIRTEDGED2 + 1, (y2 << 1) + DIRTEDGED2 + 1);

}
/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateL1Door(int pnum, int i, bool sendflag)
{
	int dpx, dpy;

	app_assert((DWORD)i < MAXOBJECTS);
	dpx = abs(object[i]._ox - plr[pnum]._px);
	dpy = abs(object[i]._oy - plr[pnum]._py);
	if ((dpx == 1) && (dpy <= 1) && (object[i]._otype == OBJ_L1DOORL)) OperateL1LDoor(pnum, i, sendflag);
	if ((dpx <= 1) && (dpy == 1) && (object[i]._otype == OBJ_L1DOORR)) OperateL1RDoor(pnum, i, sendflag);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateLever(int pnum, int i)
{
	bool mapflag;
	int j, oi, ot;

	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oSelFlag == OSEL_NONE) return;
	if (!deltaload) PlaySfxLoc(IS_LEVER, object[i]._ox, object[i]._oy);

	// Toggle to on
	object[i]._oSelFlag = OSEL_NONE;
	++object[i]._oAnimFrame;

	mapflag = true;
	if (currlevel == DIABLO_LEVEL) {
		for (j = 0; j < numobjects; ++j) {
			oi = objectactive[j];
			ot = object[oi]._otype;
			if (ot == OBJ_SWITCHSKL) {
				if ((object[i]._oVar8 == object[oi]._oVar8) && (object[oi]._oSelFlag != OSEL_NONE))
					mapflag = FALSE;
			}
		}
	}
	if (currlevel == NA_KRUL_LEVEL)
	{
//		dPiece[Na_Krul.LeverX][Na_Krul.LeverY] = 316;
		OpenNaKrul();
		Na_Krul.Lever_Thrown = TRUE;
		mapflag = FALSE;
		quests[Q_NA_KRUL]._qactive = QUEST_DONE;
	}
	if (mapflag) ObjChangeMap(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
	if (pnum == myplr) NetSendCmdParam1(FALSE,CMD_OPERATEOBJ,i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateBook(int pnum, int i)
{
	int v1, v2, v3;
	int j, oi, ot, itm;

	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oSelFlag == OSEL_NONE) return;
	if ((setlevel) && (setlvlnum == SL_VILEBETRAYER)) {
		bool found = false;
		bool dowarp = false;
		for (j = 0; j < numobjects; ++j) {
			oi = objectactive[j];
			ot = object[oi]._otype;
			if ((ot == OBJ_MCIRCLE2) && (object[oi]._oVar6 == OBJWARP1)) {
				v1 = 27;
				v2 = 29;
// rmw.patch1.start.1/23/97
// object[oi]._oVar6 == OBJDONEWARP;
				object[oi]._oVar6 = OBJDONEWARP;
// rmw.patch1.end.1/23/97
				dowarp = TRUE;
			}
			if ((ot == OBJ_MCIRCLE2) && (object[oi]._oVar6 == OBJWARP2)) { 
				v1 = 43;
				v2 = 29;
// rmw.patch1.start.1/23/97
// object[oi]._oVar6 == OBJDONEWARP;
				object[oi]._oVar6 = OBJDONEWARP;
// rmw.patch1.end.1/23/97
				dowarp = TRUE;
			}
			if (dowarp) {
				v3 = dPiece[v1][v2];
				++object[dObject[35][36]-1]._oVar5;
				AddMissile(plr[pnum]._px, plr[pnum]._py, v1, v2, plr[pnum]._pdir, MIT_PHASE, MI_ENEMYMONST, pnum, 0, 0);
				found = TRUE;
				dowarp = FALSE;
			}
		}
		if (!found) return;
	}
	// Toggle open
	object[i]._oSelFlag = OSEL_NONE;
	++object[i]._oAnimFrame;
	if ((setlevel) && (setlvlnum == SL_BONECHAMB)) {
		__int64 t = 1;
		plr[myplr]._pMemSpells |= t << (SPL_GUARDIAN-1);
		if (plr[pnum]._pSplLvl[SPL_GUARDIAN] < SPELLCAP) {
			++plr[myplr]._pSplLvl[SPL_GUARDIAN];
		}
		quests[Q_SCHAMB]._qactive = QUEST_DONE;
		if (!deltaload) PlaySfxLoc(IS_QUESTDN, object[i]._ox, object[i]._oy);
		InitDiabloMsg(MSG_INBONE);
		AddMissile(plr[myplr]._px, plr[myplr]._py, object[i]._ox - 2, object[i]._oy - 4, plr[myplr]._pdir, MIT_GUARDIAN, MI_ENEMYMONST, myplr, 0, 0);
	}
	if ((setlevel) && (setlvlnum == SL_VILEBETRAYER)) {
		ObjChangeMapResync(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
		for (itm = 0; itm < numobjects; ++itm) SyncObjectAnim(objectactive[itm]);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateBookLever(int pnum, int i)
{
	int x,y, tren;

	app_assert(gbMaxPlayers == 1);
	x = (setpc_x << 1) + DIRTEDGED2;
	y = (setpc_y << 1) + DIRTEDGED2;
	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oSelFlag == OSEL_NONE) return;
	if (qtextflag) return;
	// Check quests
	if ((object[i]._otype == OBJ_BLINDBOOK) && (quests[Q_BLIND]._qvar1 == 0)) {
		quests[Q_BLIND]._qactive = QUEST_NOTDONE;
		quests[Q_BLIND]._qlog = TRUE;
		quests[Q_BLIND]._qvar1 = 1;
	}
	if ((object[i]._otype == OBJ_BLOODBOOK) && (quests[Q_BLOOD]._qvar1 == 0)) {
		quests[Q_BLOOD]._qactive = QUEST_NOTDONE;
		quests[Q_BLOOD]._qlog = TRUE;
		quests[Q_BLOOD]._qvar1 = 1;
		SpawnQuestItem(IDI_BLDSTONE, (setpc_x << 1) + 3 + DIRTEDGED2, (setpc_y << 1) + 10 + DIRTEDGED2, FALSE, ISEL_FLR);
		SpawnQuestItem(IDI_BLDSTONE, (setpc_x << 1) + 15 + DIRTEDGED2, (setpc_y << 1) + 10 + DIRTEDGED2, FALSE, ISEL_FLR);
		SpawnQuestItem(IDI_BLDSTONE, (setpc_x << 1) + 9 + DIRTEDGED2, (setpc_y << 1) + 17 + DIRTEDGED2, FALSE, ISEL_FLR);
	}
	if ((object[i]._otype == OBJ_STEELTOME) && (quests[Q_WARLORD]._qvar1 == 0)) {
		quests[Q_WARLORD]._qactive = QUEST_NOTDONE;
		quests[Q_WARLORD]._qlog = TRUE;
		quests[Q_WARLORD]._qvar1 = 1;
	}
	// Change map
	if (object[i]._oAnimFrame != object[i]._oVar6) {
		if (object[i]._otype != OBJ_BLOODBOOK) ObjChangeMap(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
		if (object[i]._otype == OBJ_BLINDBOOK) {
			CreateItem(UID_OPTAMULET, x+5, y+5);	
			tren = TransVal;
			TransVal = 9;
			DRLG_MRectTrans(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
			TransVal = tren;
		}
	}
	// Toggle open
	// object[i]._oSelFlag = OSEL_NONE;
	object[i]._oAnimFrame = object[i]._oVar6;
	// Display msg
	InitQTextMsg(object[i]._oVar7);
	if (pnum == myplr) NetSendCmdParam1(FALSE,CMD_OPERATEOBJ,i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateSChambBk(int pnum, int i)
{
	int textdef,j;

	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oSelFlag == OSEL_NONE) return;
	if (qtextflag) return;
	//	object[i]._oSelFlag = OSEL_NONE;
	// Change map
	if (object[i]._oAnimFrame != object[i]._oVar6) {
		ObjChangeMapResync(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
		for (j = 0; j < numobjects; ++j) SyncObjectAnim(objectactive[j]);
	}
	// Toggle open
	object[i]._oAnimFrame = object[i]._oVar6;
	if (quests[Q_SCHAMB]._qactive == QUEST_NOTACTIVE) {
		quests[Q_SCHAMB]._qactive = QUEST_NOTDONE;
		quests[Q_SCHAMB]._qlog = TRUE;
	}

	if (plr[myplr]._pClass == CLASS_WARRIOR) textdef = TXT_WARBONE;
	else if (plr[myplr]._pClass == CLASS_ROGUE) textdef = TXT_ROGBONE;
	else if (plr[myplr]._pClass == CLASS_SORCEROR) textdef = TXT_SORBONE;
	else if (plr[myplr]._pClass == CLASS_MONK) textdef = TXT_MNKBONE;
	else if (plr[myplr]._pClass == CLASS_BARD) textdef = TXT_BRDBONE;
	else if (plr[myplr]._pClass == CLASS_BARBARIAN) textdef = TXT_BARBONE;
	quests[Q_SCHAMB]._qmsg = textdef;
	InitQTextMsg(textdef);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateChest(int pnum, int i, bool sendmsg)
{
	int j, mdir, mtype = MIT_ARROW;

	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oSelFlag == OSEL_NONE) return;
	if (!deltaload) PlaySfxLoc(IS_CHEST, object[i]._ox, object[i]._oy);
	object[i]._oSelFlag = OSEL_NONE;
	object[i]._oAnimFrame += 2;
	if (deltaload) return;
	SetRndSeed(object[i]._oRndSeed);

   	if (setlevel) { //Create only good items for set levels
   		for (j = 0; j < object[i]._oVar1; ++j) {
   			CreateRndItem(object[i]._ox, object[i]._oy, TRUE, sendmsg, FALSE);
   		}
   	} else { 
   		for (j = 0; j < object[i]._oVar1; ++j) {
   			if (object[i]._oVar2) CreateRndItem(object[i]._ox, object[i]._oy, FALSE, sendmsg, FALSE);
   			else CreateRndUseful(pnum, object[i]._ox, object[i]._oy, sendmsg);
   		}
   	}
   
	if ((object[i]._oTrapFlag) &&
		(object[i]._otype >= OBJ_TCHEST1) &&
		(object[i]._otype <= OBJ_TCHEST3)) {
			mdir = GetDirection(object[i]._ox, object[i]._oy, plr[pnum]._px, plr[pnum]._py);
			switch(object[i]._oVar4) {
				case 0:
					mtype = MIT_ARROW;
					break;
				case 1:
					mtype = MIT_FARROW;
					break;
				case 2:
					mtype = MIT_NOVA;
					break;
				case 3:
					mtype = MIT_FLAMEBOX;
//					SpawnSkeleton(object[i]._oVar2, object[i]._ox, object[i]._oy);
					break;
				case 4:
					mtype = MIT_DISENCHANT;
					break;
				case 5:
					mtype = MIT_MANAREMOVE;
					break;
#if defined(HELLFIRE2)				
				// GWP Another good trap might be acid splatted all around.
				case 6:
					mtype = MIT_LIGHTBOX;
//					SpawnSkeleton(object[i]._oVar2, object[i]._ox, object[i]._oy);
					break;
#endif
				default:
					mtype = MIT_ARROW;
					break;	
			}
			AddMissile(object[i]._ox, object[i]._oy, plr[pnum]._px, plr[pnum]._py, mdir, mtype, MI_ENEMYPLR, -1, 0, 0);
			object[i]._oTrapFlag = FALSE;
	}
	if (pnum == myplr) NetSendCmdParam2(FALSE,CMD_PLROPOBJ,pnum,i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateMushPatch(int pnum, int i)
{
	int x, y;

	if (!(quests[Q_BKMUSHRM]._qactive == QUEST_NOTDONE
		  && (quests[Q_BKMUSHRM]._qvar1 >= QS_TOMEGIVEN)))	// FUNGALTM was given to Witch
	{
		if (!deltaload && pnum == myplr) {
			if (plr[myplr]._pClass == CLASS_WARRIOR) PlaySFX(PS_WARR13);
			#if !IS_VERSION(SHAREWARE)
			else if (plr[myplr]._pClass == CLASS_ROGUE) PlaySFX(PS_ROGUE13);
			else if (plr[myplr]._pClass == CLASS_SORCEROR) PlaySFX(PS_MAGE13);
			else if (plr[myplr]._pClass == CLASS_MONK) PlaySFX(PS_MONK13);
			else if (plr[myplr]._pClass == CLASS_BARD) PlaySFX(PS_BARD13);
			else if (plr[myplr]._pClass == CLASS_BARBARIAN) PlaySFX(PS_BARBARIAN13);
			#endif
		}
		return;
	}

	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oSelFlag == OSEL_NONE)
		return;
	if (!deltaload) PlaySfxLoc(IS_CHEST, object[i]._ox, object[i]._oy);
	object[i]._oSelFlag = OSEL_NONE;
	object[i]._oAnimFrame += 1;
	if (deltaload) return;
	GetSuperItemLoc(object[i]._ox, object[i]._oy, x, y);
	SpawnQuestItem(IDI_MUSHROOM, x,y,0, ISEL_NONE);
	quests[Q_BKMUSHRM]._qvar1 = QS_MUSHSPAWNED;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateInnSignChest(int pnum, int i)
{
	int x, y;

	if (!(quests[Q_LTBANNER]._qvar1 == 2))
	{
		if (!deltaload && pnum == myplr) {
			if (plr[myplr]._pClass == CLASS_WARRIOR) PlaySFX(PS_WARR24);
			#if !IS_VERSION(SHAREWARE)
			else if (plr[myplr]._pClass == CLASS_ROGUE) PlaySFX(PS_ROGUE24);
			else if (plr[myplr]._pClass == CLASS_SORCEROR) PlaySFX(PS_MAGE24);
			else if (plr[myplr]._pClass == CLASS_MONK) PlaySFX(PS_MONK24);
			else if (plr[myplr]._pClass == CLASS_BARD) PlaySFX(PS_BARD24);
			else if (plr[myplr]._pClass == CLASS_BARBARIAN) PlaySFX(PS_BARBARIAN24);
			#endif
		}
		return;
	}

	if (object[i]._oSelFlag == OSEL_NONE)
		return;
	if (!deltaload) PlaySfxLoc(IS_CHEST, object[i]._ox, object[i]._oy);
	object[i]._oSelFlag = OSEL_NONE;
	object[i]._oAnimFrame += 2;
	if (deltaload) return;
	GetSuperItemLoc(object[i]._ox, object[i]._oy, x, y);
	SpawnQuestItem(IDI_BANNER, x,y,0, ISEL_NONE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateSlainHero(int pnum, int i, bool sendmsg)
{
	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oSelFlag == OSEL_NONE) return;
	object[i]._oSelFlag = OSEL_NONE;
	if (deltaload) return;

	if (plr[pnum]._pClass == CLASS_WARRIOR) {
		//Create magic breast plate
		CreateMagicArmor(object[i]._ox, object[i]._oy, IT_HARMOR, ITEM_BPLATE, FALSE, TRUE);
		#if !IS_VERSION(SHAREWARE)
		PlaySfxLoc(PS_WARR9, plr[myplr]._px, plr[myplr]._py);
		#endif
	} else if (plr[pnum]._pClass == CLASS_ROGUE) {
		//Create magic long battle bow
		CreateMagicWeapon(object[i]._ox, object[i]._oy, IT_BOW, ITEM_STLLONGBOW, FALSE, TRUE);	
		#if !IS_VERSION(SHAREWARE)
		PlaySfxLoc(PS_ROGUE9, plr[myplr]._px, plr[myplr]._py);
		#endif
	} else if (plr[pnum]._pClass == CLASS_SORCEROR) { 
		//Create book of lightning
		CreateSpellBook(object[i]._ox, object[i]._oy, SPL_LIGHTNING, FALSE, TRUE);
		#if !IS_VERSION(SHAREWARE)
		PlaySfxLoc(PS_MAGE9, plr[myplr]._px, plr[myplr]._py);
		#endif
	} else if (plr[pnum]._pClass == CLASS_MONK) { 
		//Create magic staff 
		CreateMagicWeapon(object[i]._ox, object[i]._oy, IT_STAFF, ITEM_STLSTAFF, FALSE, TRUE);
		#if !IS_VERSION(SHAREWARE)
		PlaySfxLoc(PS_MONK9, plr[myplr]._px, plr[myplr]._py);
		#endif
	} else if (plr[pnum]._pClass == CLASS_BARD) { 
		//Create special sword.
		CreateMagicWeapon(object[i]._ox, object[i]._oy, IT_SWORD, ITEM_BASTSRD, FALSE, TRUE);
		#if !IS_VERSION(SHAREWARE)
		PlaySfxLoc(PS_BARD9, plr[myplr]._px, plr[myplr]._py);
		#endif
	} else if (plr[pnum]._pClass == CLASS_BARBARIAN) { 
		//Create special sword.
		CreateMagicWeapon(object[i]._ox, object[i]._oy, IT_AXE, ITEM_BTLAXE, FALSE, TRUE);
		#if !IS_VERSION(SHAREWARE)
		PlaySfxLoc(PS_BARBARIAN9, plr[myplr]._px, plr[myplr]._py);
		#endif
	}
	if (pnum == myplr) NetSendCmdParam1(FALSE,CMD_OPERATEOBJ,i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateTrapLvr(int i)
{
	int j, oi;

	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oAnimFrame == 1) {
		// Toggle to on
		++object[i]._oAnimFrame;
		for (j = 0; j < numobjects; ++j) {
			oi = objectactive[j];
			if ((object[oi]._otype == object[i]._oVar2) && (object[oi]._oVar1 == object[i]._oVar1)) {
				object[oi]._oVar2 = 1;
				object[oi]._oAnimFlag = FALSE;
			}
		}
	} else {
		// Toggle to off
		--object[i]._oAnimFrame;
		for (j = 0; j < numobjects; ++j) {
			oi = objectactive[j];
			if ((object[oi]._otype == object[i]._oVar2) && (object[oi]._oVar1 == object[i]._oVar1)) {
				object[oi]._oVar2 = 0;
				if (object[oi]._oVar4 != 0) object[oi]._oAnimFlag = TRUE;
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateSarc(int pnum, int i, bool sendmsg)
{
	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oSelFlag == OSEL_NONE) return;
	if (!deltaload) PlaySfxLoc(IS_SARC, object[i]._ox, object[i]._oy);
	object[i]._oSelFlag = OSEL_NONE;
	if (deltaload) {
		object[i]._oAnimFrame = object[i]._oAnimLen;
		return;
	} else {
		object[i]._oAnimFlag = TRUE;
		object[i]._oAnimDelay = 3;
	}
	SetRndSeed(object[i]._oRndSeed);
	if (object[i]._oVar1 <= 2) CreateRndItem(object[i]._ox, object[i]._oy, FALSE, sendmsg, FALSE);
	if (object[i]._oVar1 >= 8) SpawnSkeleton(object[i]._oVar2, object[i]._ox, object[i]._oy);

#if CHEATS	
	if (itemcheat)
		CreateMagicWeapon(object[i]._ox, object[i]._oy, IT_STAFF, ITEM_SHRTSTAFF, FALSE, TRUE);
#endif
		
	if (pnum == myplr) NetSendCmdParam1(FALSE,CMD_OPERATEOBJ,i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateL2Door(int pnum, int i, bool sendflag)
{
	int dpx, dpy;

	app_assert((DWORD)i < MAXOBJECTS);
	dpx = abs(object[i]._ox - plr[pnum]._px);
	dpy = abs(object[i]._oy - plr[pnum]._py);
	if ((dpx == 1) && (dpy <= 1) && (object[i]._otype == OBJ_L2DOORL)) OperateL2LDoor(pnum, i, sendflag);
	if ((dpx <= 1) && (dpy == 1) && (object[i]._otype == OBJ_L2DOORR)) OperateL2RDoor(pnum, i, sendflag);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateL3Door(int pnum, int i, bool sendflag)
{
	int dpx, dpy;

	app_assert((DWORD)i < MAXOBJECTS);
	dpx = abs(object[i]._ox - plr[pnum]._px);
	dpy = abs(object[i]._oy - plr[pnum]._py);
	if ((dpx == 1) && (dpy <= 1) && (object[i]._otype == OBJ_L3DOORR)) OperateL3RDoor(pnum, i, sendflag);
	if ((dpx <= 1) && (dpy == 1) && (object[i]._otype == OBJ_L3DOORL)) OperateL3LDoor(pnum, i, sendflag);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperatePedistal(int pnum, int i)
{
	int jstn;
				
	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oVar6 != 3) {
		if (PlrHasItem(pnum, IDI_BLDSTONE, jstn)) {
			RemoveInvItem(pnum, jstn);
			++object[i]._oAnimFrame;
			++object[i]._oVar6;
		}
		if (object[i]._oVar6 == 1) {
			if (!deltaload) PlaySfxLoc(LS_PUDDLE, object[i]._ox, object[i]._oy);
			ObjChangeMap(setpc_x, setpc_y + 3, setpc_x + 2, setpc_y + 7);
		}
		if (object[i]._oVar6 == 2) {
			if (!deltaload) PlaySfxLoc(LS_PUDDLE, object[i]._ox, object[i]._oy);
			ObjChangeMap(setpc_x + 6, setpc_y + 3, setpc_x + setpc_w, setpc_y + 7);
		}
		if (object[i]._oVar6 == 3) {
			if (!deltaload) PlaySfxLoc(LS_BLODSTAR, object[i]._ox, object[i]._oy);
			ObjChangeMap(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
			byte *setp;
			setp = LoadFileInMemSig("Levels\\L2Data\\Blood2.DUN", NULL, 'STPC');
			LoadMapObjs(setp, (setpc_x << 1), (setpc_y << 1));
			DiabloFreePtr(setp);
			CreateItem(UID_ARMOFVAL, (setpc_x << 1) + 9 + DIRTEDGED2, (setpc_y << 1) + 3 + DIRTEDGED2);
			object[i]._oSelFlag = OSEL_NONE;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void TryDisarm(int pnum, int i)
{
	int j, oi, oti;
	int trapdisper;
	bool checkflag;

	if (pnum == myplr) NewCursor(GLOVE_CURS);

	app_assert((DWORD)i < MAXOBJECTS);
	if (!object[i]._oTrapFlag) return;

	trapdisper = (plr[pnum]._pDexterity << 1) - (currlevel * 5);
	if (random(154,100) > trapdisper) return;

	for (j = 0; j < numobjects; ++j) {
		oi = objectactive[j];
		checkflag = false;
		if (object[oi]._otype == OBJ_TRAPL) checkflag = true;
		if (object[oi]._otype == OBJ_TRAPR) checkflag = true;
		if (checkflag) {
			oti = dObject[object[oi]._oVar1][object[oi]._oVar2] - 1;
			if (oti == i) {
				object[oi]._oVar4 = 1; 
				object[i]._oTrapFlag = FALSE;
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int ItemMiscIdIdx(int imiscid)
{
	int i = 0;
	while ((AllItemsList[i].iRnd == IRND_NO) || (AllItemsList[i].iMiscId != imiscid)) ++i;
	return(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateShrine(int pnum, int i, int sType)
{
	int r, xx, yy, idata, sc;
	int v1, v2, v3, v4;
	__int64 lv, t;
	bool done;

	//Fixes any gold cheats
	if (dropGoldFlag) {
		dropGoldFlag = FALSE;
		dropGoldValue = 0;
	}

	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oSelFlag == OSEL_NONE) return;
	SetRndSeed(object[i]._oRndSeed);
	object[i]._oSelFlag = OSEL_NONE;
	if (! deltaload) {
		PlaySfxLoc(sType, object[i]._ox, object[i]._oy);
		object[i]._oAnimFlag = TRUE;
		object[i]._oAnimDelay = 1;
	} else {
		object[i]._oAnimFrame = object[i]._oAnimLen;
		object[i]._oAnimFlag = FALSE;
	}
	switch (object[i]._oVar1) {
		case 0 :		// Mysterious
			if (deltaload) return;
			if (pnum != myplr) return;
			//Subtract 1 from all attributes
			ModifyPlrStr(pnum, -1);
			ModifyPlrMag(pnum, -1);
			ModifyPlrDex(pnum, -1);
			ModifyPlrVit(pnum, -1);
			//Add 6 to random attribute. We are actually adding 5 since we
			//subtracted 1 above.
			switch(random(0, 4)) {
				case 0:
					ModifyPlrStr(pnum, 6);
					break;
				case 1:
					ModifyPlrMag(pnum, 6);
					break;
				case 2:
					ModifyPlrDex(pnum, 6);
					break;
				case 3:
					ModifyPlrVit(pnum, 6);
					break;
			}
			CheckStats(pnum);
			InitDiabloMsg(SHRINE_1);
			break;
		case 1 :		// Hidden
			v1 = 0;
			if (deltaload) return;
			if (pnum != myplr) return;
			//Find out if player has any items equipped
			for (r = 0; r < NUM_INVLOC; ++r) {
				if (plr[pnum].InvBody[r]._itype != -1) ++v1;
			}
			//Add 10 to max and current durability of all equipped items.
			if (v1 > 0) {
				for (r = 0; r < NUM_INVLOC; ++r) {
					if (plr[pnum].InvBody[r]._itype != -1) {
						//Ignore any items which are indestructible.
						if ((plr[pnum].InvBody[r]._iMaxDur != INFINITE_DUR) &&
							(plr[pnum].InvBody[r]._iMaxDur != 0)) {
							plr[pnum].InvBody[r]._iDurability += 10;
							plr[pnum].InvBody[r]._iMaxDur += 10;
							if (plr[pnum].InvBody[r]._iDurability > plr[pnum].InvBody[r]._iMaxDur)
								plr[pnum].InvBody[r]._iDurability = plr[pnum].InvBody[r]._iMaxDur;
						}
					}
				}
				done = false;
				while (!done) {

					// JMM.PATCH3
					int numitems = 0;
					for( r = 0; r < NUM_INVLOC; ++r ) {
						if ( plr[pnum].InvBody[r]._itype != -1 )
							++numitems;
					}
					if( !numitems ) {
						done = true;
						break;
					}
					// JMM.ENDPATCH3

					r = random(0, NUM_INVLOC);
					if (plr[pnum].InvBody[r]._itype != -1) {
						//Ignore any items which are indestructible.
						if ((plr[pnum].InvBody[r]._iMaxDur != INFINITE_DUR) &&
							(plr[pnum].InvBody[r]._iMaxDur != 0)) {
							//Subtract 20 from random equipped item. We are actually 
							//subtracting 10 since we added 10 above.
							plr[pnum].InvBody[r]._iDurability -= 20;
							plr[pnum].InvBody[r]._iMaxDur -= 20;
							if (plr[pnum].InvBody[r]._iDurability <= 0) plr[pnum].InvBody[r]._iDurability = 1;
							if (plr[pnum].InvBody[r]._iMaxDur <= 0) plr[pnum].InvBody[r]._iMaxDur = 1;
							done = true;
							break;
						}
					}
				}
			}
			InitDiabloMsg(SHRINE_2);
			break;
		case 2:			// Gloomy (Single Player Only)
			if (deltaload) return;
			if (pnum == myplr) {
				if (plr[pnum].HeadItem._itype != -1) plr[pnum].HeadItem._iAC += 2;
				if (plr[pnum].BodyItem._itype != -1) plr[pnum].BodyItem._iAC += 2;
				if (plr[pnum].Hand1Item._itype != -1) {
					if (plr[pnum].Hand1Item._itype == IT_SHIELD)
						plr[pnum].Hand1Item._iAC += 2;
					else {
						--plr[pnum].Hand1Item._iMaxDam;
						if (plr[pnum].Hand1Item._iMaxDam < plr[pnum].Hand1Item._iMinDam)
							plr[pnum].Hand1Item._iMaxDam = plr[pnum].Hand1Item._iMinDam;
					}
				}
				if (plr[pnum].Hand2Item._itype != -1) {
					if (plr[pnum].Hand2Item._itype == IT_SHIELD)
						plr[pnum].Hand2Item._iAC += 2;
					else {
						--plr[pnum].Hand2Item._iMaxDam;
						if (plr[pnum].Hand2Item._iMaxDam < plr[pnum].Hand2Item._iMinDam)
							plr[pnum].Hand2Item._iMaxDam = plr[pnum].Hand2Item._iMinDam;
					}
				}
				for (r = 0; r < plr[pnum]._pNumInv; ++r) {
					switch(plr[pnum].InvList[r]._itype) {
						case IT_SHIELD:
						case IT_ARMOR:
						case IT_MARMOR:
						case IT_HARMOR:
						case IT_HELM:
							plr[pnum].InvList[r]._iAC += 2;
							break;
						case IT_SWORD:
						case IT_AXE:
						case IT_BOW:
						case IT_MACE:
						case IT_STAFF:
							--plr[pnum].InvList[r]._iMaxDam;
							if (plr[pnum].InvList[r]._iMaxDam < plr[pnum].InvList[r]._iMinDam)
								plr[pnum].InvList[r]._iMaxDam = plr[pnum].InvList[r]._iMinDam;
							break;
					}
				}
				InitDiabloMsg(SHRINE_3);
			}
			break;
		case 3:			// Weird (Single Player Only)
			if (deltaload) return;
			if (pnum == myplr) {
				if ((plr[pnum].Hand1Item._itype != -1) && (plr[pnum].Hand1Item._itype != IT_SHIELD)) 
					++plr[pnum].Hand1Item._iMaxDam;
				if ((plr[pnum].Hand2Item._itype != -1) && (plr[pnum].Hand2Item._itype != IT_SHIELD))
					++plr[pnum].Hand2Item._iMaxDam;
				for (r = 0; r < plr[pnum]._pNumInv; ++r) {
					switch(plr[pnum].InvList[r]._itype) {
						case IT_SWORD:
						case IT_AXE:
						case IT_BOW:
						case IT_MACE:
						case IT_STAFF:
							++plr[pnum].InvList[r]._iMaxDam;
							break;
					}
				}
				InitDiabloMsg(SHRINE_4);
			}
			break;
		case 4:			// Magical
		case 11:		// Supernatural
			if (deltaload) return;
			//Cast mana shield
			AddMissile(plr[pnum]._px, plr[pnum]._py, plr[pnum]._px, plr[pnum]._py, 
					   plr[pnum]._pdir, MIT_MANASHIELD, -1, pnum, 0,        
					   (leveltype << 1));
			if (pnum != myplr) return;
			InitDiabloMsg(SHRINE_5);
			break;
		case 5:			// Stone
			if (deltaload) return;
			if (pnum == myplr) {
				for (r = 0; r < NUM_INVLOC; ++r) {
					if (plr[pnum].InvBody[r]._itype == IT_STAFF)
						plr[pnum].InvBody[r]._iCharges = plr[pnum].InvBody[r]._iMaxCharges;
				}
				for (r = 0; r < plr[pnum]._pNumInv; ++r) {
					if (plr[pnum].InvList[r]._itype == IT_STAFF)
						plr[pnum].InvList[r]._iCharges = plr[pnum].InvList[r]._iMaxCharges;
				}
				for (r = 0; r < MAXSPD; ++r) {
					if (plr[pnum].SpdList[r]._itype == IT_STAFF)
						plr[pnum].SpdList[r]._iCharges = plr[pnum].SpdList[r]._iMaxCharges;
				}
				InitDiabloMsg(SHRINE_6);
			}
			break;
		case 6:			// Religious
			if (deltaload) return;
			if (pnum == myplr) {
				for (r = 0; r < NUM_INVLOC; ++r) {
					plr[pnum].InvBody[r]._iDurability = plr[pnum].InvBody[r]._iMaxDur;
				}
				for (r = 0; r < plr[pnum]._pNumInv; ++r) {
					plr[pnum].InvList[r]._iDurability = plr[pnum].InvList[r]._iMaxDur;
				}
				for (r = 0; r < MAXSPD; ++r) {
					plr[pnum].SpdList[r]._iDurability = plr[pnum].SpdList[r]._iMaxDur;
				}
				InitDiabloMsg(SHRINE_7);
			}
			break;
		case 7:			// Enchanted (Levels 1-8 Only)
			if (deltaload) return;
			if (pnum != myplr) return;
			//Get memorized spell count
			sc = 0; 
			lv = 1;
			for (r = 1; r <= MAXSPELLS; ++r) {
				if (plr[pnum]._pMemSpells & lv) ++sc;
				lv = lv << 1;
			}
			//Only add and subtract if player has more than 1 spell memorized
			if (sc > 1) {
				//Add one to all memorized spell levels
				lv = 1;
				for (r = 1; r <= MAXSPELLS; ++r) {
					if (plr[pnum]._pMemSpells & lv) {
						if (plr[pnum]._pSplLvl[r] < SPELLCAP) {
							++plr[pnum]._pSplLvl[r];
							v1 = r;
						}
					}
					lv = lv << 1;
				}
				//Subtract 2 from random memorized spell. We are actually subtracting 
				//1 since we added 1 above.
				done = false;
				while (!done) {
					lv = 1;
					r = random(0, MAXSPELLS);
					lv = lv << r;
					if (plr[pnum]._pMemSpells & lv) {
						if (plr[pnum]._pSplLvl[r+1] >= 2) plr[pnum]._pSplLvl[r+1] -= 2;
						else plr[pnum]._pSplLvl[r+1] = 0;
						done = true;
						break;
					}
				}
			}
			InitDiabloMsg(SHRINE_8);
			break;
		case 8:			// Thaumaturgic (Single Player Only)
			//All chests regenerate
			for (r = 0; r < numobjects; ++r) {
				v1 = objectactive[r];
				app_assert((DWORD)v1 < MAXOBJECTS);
				if (((object[v1]._otype == OBJ_CHEST1) ||
					(object[v1]._otype == OBJ_CHEST2) ||
					(object[v1]._otype == OBJ_CHEST3)) &&
					(object[v1]._oSelFlag == OSEL_NONE)) {
					object[v1]._oRndSeed = GetRndSeed();
					object[v1]._oSelFlag = OSEL_FLR;
					object[v1]._oAnimFrame -= 2;
				}
			}
			if (deltaload) return;
			if (pnum == myplr) InitDiabloMsg(SHRINE_9);
			break;
		case 9:			// Fascinating
			if (deltaload) return;
			if (pnum != myplr) return;
			//Give player fire bolt level +2
			t = 1;
			plr[pnum]._pMemSpells |= t << (SPL_FIREBOLT-1);
			if (plr[pnum]._pSplLvl[SPL_FIREBOLT] < SPELLCAP) ++plr[pnum]._pSplLvl[SPL_FIREBOLT] ;
			if (plr[pnum]._pSplLvl[SPL_FIREBOLT] < SPELLCAP) ++plr[pnum]._pSplLvl[SPL_FIREBOLT] ;
			//Subtract 1/10 of mana
			v1 = plr[pnum]._pMaxManaBase / 10;
			v2 = plr[pnum]._pMana - plr[pnum]._pManaBase;
			v3 = plr[pnum]._pMaxMana - plr[pnum]._pMaxManaBase;
			plr[pnum]._pManaBase -= v1;
			plr[pnum]._pMana -= v1;
			plr[pnum]._pMaxMana -= v1;
			plr[pnum]._pMaxManaBase -= v1;
			if ((plr[pnum]._pMana >> MANA_SHIFT) <= 0) {
				plr[pnum]._pMana = v2; 
				plr[pnum]._pManaBase = 0;
			}
			if ((plr[pnum]._pMaxMana >> MANA_SHIFT) <= 0) {
				plr[pnum]._pMaxMana = v3;
				plr[pnum]._pMaxManaBase = 0;
			}
			InitDiabloMsg(SHRINE_10);
			break;
		case 10:		// Cryptic
			if (deltaload) return;
			//Cast nova
			AddMissile(plr[pnum]._px, plr[pnum]._py, plr[pnum]._px, plr[pnum]._py, 
				       plr[pnum]._pdir, MIT_NOVA, -1, pnum, 0, (leveltype << 1));
			if (pnum != myplr) return;
			//Give full mana
			plr[pnum]._pMana = plr[pnum]._pMaxMana;
			plr[pnum]._pManaBase = plr[pnum]._pMaxManaBase;
			InitDiabloMsg(SHRINE_11);
			break;
		case 12:		// Eldritch
			if (deltaload) return;
			if (pnum == myplr) {
				//All health and mana potions become full rejuv.
				for (r = 0; r < plr[pnum]._pNumInv; ++r) {
					if (plr[pnum].InvList[r]._itype == IT_MISC) {
						if ((plr[pnum].InvList[r]._iMiscId == IMID_PLHEAL) ||
							(plr[pnum].InvList[r]._iMiscId == IMID_PMANA)) {
							idata = ItemMiscIdIdx(IMID_REJUV);
							SetPlrHandItem(&plr[pnum].HoldItem, idata);
							GetPlrHandSeed(&plr[pnum].HoldItem);
							plr[pnum].HoldItem._iStatFlag = TRUE;
							plr[pnum].InvList[r] = plr[pnum].HoldItem;
						}
						if ((plr[pnum].InvList[r]._iMiscId == IMID_PHEAL) ||
							(plr[pnum].InvList[r]._iMiscId == IMID_PFMANA)) {
							idata = ItemMiscIdIdx(IMID_FREJUV);
							SetPlrHandItem(&plr[pnum].HoldItem, idata);
							GetPlrHandSeed(&plr[pnum].HoldItem);
							plr[pnum].HoldItem._iStatFlag = TRUE;
							plr[pnum].InvList[r] = plr[pnum].HoldItem;
						}
					}
				}
				for (r = 0; r < MAXSPD; ++r) {
					if (plr[pnum].SpdList[r]._itype == IT_MISC) {
						if ((plr[pnum].SpdList[r]._iMiscId == IMID_PLHEAL) ||
							(plr[pnum].SpdList[r]._iMiscId == IMID_PMANA)) {
							idata = ItemMiscIdIdx(IMID_REJUV);
							SetPlrHandItem(&plr[pnum].HoldItem, idata);
							GetPlrHandSeed(&plr[pnum].HoldItem);
							plr[pnum].HoldItem._iStatFlag = TRUE;
							plr[pnum].SpdList[r] = plr[pnum].HoldItem;
						}
						if ((plr[pnum].SpdList[r]._iMiscId == IMID_PHEAL) ||
							(plr[pnum].SpdList[r]._iMiscId == IMID_PFMANA)) {
							idata = ItemMiscIdIdx(IMID_FREJUV);
							SetPlrHandItem(&plr[pnum].HoldItem, idata);
							GetPlrHandSeed(&plr[pnum].HoldItem);
							plr[pnum].HoldItem._iStatFlag = TRUE;
							plr[pnum].SpdList[r] = plr[pnum].HoldItem;
						}
					}
				}
				InitDiabloMsg(SHRINE_13);
			}
			break;
		case 13:		// Eerie
			if (deltaload) return;
			if (pnum != myplr) return;
			//Add 2 to magic
			ModifyPlrMag(pnum, 2);
			CheckStats(pnum);
			InitDiabloMsg(SHRINE_14);
			break;
		case 14:		// Divine
			if (deltaload) return;
			if (pnum != myplr) return;
			//Put this in because full rejuvs are only available from level 7 on.
			if ((currlevel << 1) < 7) {
				//Create 1 full heal and 1 full mana
				CreateTypeItem(object[i]._ox, object[i]._oy, FALSE, IT_MISC, IMID_PFMANA, FALSE, TRUE);
				CreateTypeItem(object[i]._ox, object[i]._oy, FALSE, IT_MISC, IMID_PHEAL, FALSE, TRUE);
			} else {					
				//Create 2 full rejuv
				CreateTypeItem(object[i]._ox, object[i]._oy, FALSE, IT_MISC, IMID_FREJUV, FALSE, TRUE);
				CreateTypeItem(object[i]._ox, object[i]._oy, FALSE, IT_MISC, IMID_FREJUV, FALSE, TRUE);
			}
			//Give full mana and life
			plr[pnum]._pMana = plr[pnum]._pMaxMana;
			plr[pnum]._pManaBase = plr[pnum]._pMaxManaBase;
			plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
			plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;
			InitDiabloMsg(SHRINE_15);
			break;
		case 15:		// Holy
			if (deltaload) return;
			v4 = 0;
			do {
				v1 = random(159,DMAXX);
				v2 = random(159,DMAXY);
				v3 = dPiece[v1][v2];
				++v4;
				if (v4 > (DMAXX*DMAXY)) break;
			} while (nSolidTable[v3] != 0 || dObject[v1][v2] != 0 || dMonster[v1][v2] != 0);
			//Cast phase
			AddMissile(plr[pnum]._px, plr[pnum]._py, v1, v2, plr[pnum]._pdir, 
				       MIT_PHASE, -1, pnum, 0, (leveltype << 1));
			if (pnum != myplr) return;
			InitDiabloMsg(SHRINE_16);
			break;
		case 16:		// Sacred
			if (deltaload) return;
			if (pnum != myplr) return;
			//Give player charged bolt level +2
			t = 1;
			plr[pnum]._pMemSpells |= t << (SPL_CBOLT-1);
			if (plr[pnum]._pSplLvl[SPL_CBOLT] < SPELLCAP) ++plr[pnum]._pSplLvl[SPL_CBOLT] ;
			if (plr[pnum]._pSplLvl[SPL_CBOLT] < SPELLCAP) ++plr[pnum]._pSplLvl[SPL_CBOLT] ;
			//Subtract 1/10 of mana
			v1 = plr[pnum]._pMaxManaBase / 10;
			v2 = plr[pnum]._pMana - plr[pnum]._pManaBase;
			v3 = plr[pnum]._pMaxMana - plr[pnum]._pMaxManaBase;
			plr[pnum]._pManaBase -= v1;
			plr[pnum]._pMana -= v1;
			plr[pnum]._pMaxMana -= v1;
			plr[pnum]._pMaxManaBase -= v1;
			if ((plr[pnum]._pMana >> MANA_SHIFT) <= 0) {
				plr[pnum]._pMana = v2; 
				plr[pnum]._pManaBase = 0;
			}
			if ((plr[pnum]._pMaxMana >> MANA_SHIFT) <= 0) {
				plr[pnum]._pMaxMana = v3;
				plr[pnum]._pMaxManaBase = 0;
			}
			InitDiabloMsg(SHRINE_17);
			break;
		case 17:		// Spiritual
			if (deltaload) return;
			if (pnum != myplr) return;
			//Fill inventory with gold
			for (r = 0; r < MAXINV; ++r) {
				if (plr[pnum].InvGrid[r] == 0) {
					v2 = (leveltype * 5) + random(160,leveltype * 10);
					v1 = plr[pnum]._pNumInv;
					plr[pnum].InvList[v1] = golditem;
					plr[pnum].InvList[v1]._iSeed = GetRndSeed();
					++plr[pnum]._pNumInv;
					plr[pnum].InvGrid[r] = plr[pnum]._pNumInv;
					plr[pnum].InvList[v1]._ivalue = v2;
					plr[pnum]._pGold += v2;
					SetGoldCurs(pnum, v1);
				}
			}
			InitDiabloMsg(SHRINE_18);
			break;
		case 18:		// Spooky (Multi Player Only)
			if (deltaload) return;
			if (pnum == myplr) InitDiabloMsg(SHRINE_19);
			else {
				//Full rejuv for other players
				InitDiabloMsg(SHRINE_19B);
				plr[myplr]._pHitPoints = plr[myplr]._pMaxHP;
				plr[myplr]._pHPBase = plr[myplr]._pMaxHPBase;
				plr[myplr]._pMana = plr[myplr]._pMaxMana;
				plr[myplr]._pManaBase = plr[myplr]._pMaxManaBase;
			}
			break;
		case 19:		// Abandoned
			if (deltaload) return;
			if (pnum != myplr) return;
			//Add 2 to dexterity
			ModifyPlrDex(pnum, 2);
			CheckStats(pnum);
			if (pnum == myplr) InitDiabloMsg(SHRINE_20);
			break;
		case 20:		// Creepy
			if (deltaload) return;
			if (pnum != myplr) return;
			//Add 2 to strength
			ModifyPlrStr(pnum, 2);
			CheckStats(pnum);
			if (pnum == myplr) InitDiabloMsg(SHRINE_21);
			break;
		case 21:		// Quiet
			if (deltaload) return;
			if (pnum != myplr) return;
			//Add 2 to vitality
			ModifyPlrVit(pnum, 2);
			CheckStats(pnum);
			if (pnum == myplr) InitDiabloMsg(SHRINE_22);
			break;
		case 22:		// Secluded
			if (deltaload) return;
			if (pnum == myplr) {
				//Complete automap for current level
				for (yy = 0; yy < AUTOMAPY; ++yy) {
					for (xx = 0; xx < AUTOMAPX; ++xx) automapview[xx][yy] = TRUE;
				}
				InitDiabloMsg(SHRINE_23);
			}
			break;
		case 23:		// Ornate
			if (deltaload) return;
			if (pnum != myplr) return;
			//Give player holy bolt level +2
			t = 1;
			plr[pnum]._pMemSpells |= t << (SPL_HBOLT-1);
			if (plr[pnum]._pSplLvl[SPL_HBOLT] < SPELLCAP) ++plr[pnum]._pSplLvl[SPL_HBOLT] ;
			if (plr[pnum]._pSplLvl[SPL_HBOLT] < SPELLCAP) ++plr[pnum]._pSplLvl[SPL_HBOLT] ;
			//Subtract 1/10 of mana
			v1 = plr[pnum]._pMaxManaBase / 10;
			v2 = plr[pnum]._pMana - plr[pnum]._pManaBase;
			v3 = plr[pnum]._pMaxMana - plr[pnum]._pMaxManaBase;
			plr[pnum]._pManaBase -= v1;
			plr[pnum]._pMana -= v1;
			plr[pnum]._pMaxMana -= v1;
			plr[pnum]._pMaxManaBase -= v1;
			if ((plr[pnum]._pMana >> MANA_SHIFT) <= 0) {
				plr[pnum]._pMana = v2; 
				plr[pnum]._pManaBase = 0;
			}
			if ((plr[pnum]._pMaxMana >> MANA_SHIFT) <= 0) {
				plr[pnum]._pMaxMana = v3;
				plr[pnum]._pMaxManaBase = 0;
			}
			InitDiabloMsg(SHRINE_24);
			break;
		case 24:		// Glimmering
			if (deltaload) return;
			if (pnum != myplr) return;
			//All items are identified
			for (r = 0; r < NUM_INVLOC; ++r) {
				if ((plr[pnum].InvBody[r]._iMagical != IMAGIC_NONE) && !plr[pnum].InvBody[r]._iIdentified)
					plr[pnum].InvBody[r]._iIdentified = TRUE;
			}
			for (r = 0; r < plr[pnum]._pNumInv; ++r) {
				if ((plr[pnum].InvList[r]._iMagical != IMAGIC_NONE) && !plr[pnum].InvList[r]._iIdentified)
					plr[pnum].InvList[r]._iIdentified = TRUE;
			}
			for (r = 0; r < MAXSPD; ++r) {
				if ((plr[pnum].SpdList[r]._iMagical != IMAGIC_NONE) && !plr[pnum].SpdList[r]._iIdentified)
					plr[pnum].SpdList[r]._iIdentified = TRUE;
			}
			InitDiabloMsg(SHRINE_25);
			break;
		case 25:		// Tainted (Multi Player Only)
			if (deltaload) return;
			if (pnum == myplr) InitDiabloMsg(SHRINE_26);
			else {
				//All other players get +1 to 3 random stats and -1 to other
				InitDiabloMsg(SHRINE_26B);
				r = random(155,4);
				if (r == 0) v1 = 1;
				else v1 = -1;
				if (r == 1) v2 = 1;
				else v2 = -1;
				if (r == 2) v3 = 1;
				else v3 = -1;
				if (r == 3) v4 = 1;
				else v4 = -1;
				ModifyPlrStr(myplr, v1);
				ModifyPlrMag(myplr, v2);
				ModifyPlrDex(myplr, v3);
				ModifyPlrVit(myplr, v4);
				CheckStats(myplr);
			}
			break;
		case 26:	// Oily
			// Increase good stats by 1.
			if (deltaload) return;
			if (pnum != myplr) return;
			InitDiabloMsg(SHRINE_27);
			switch(plr[myplr]._pClass)
			{
				case CLASS_SORCEROR:
					ModifyPlrMag(myplr,2);
					break;
				case CLASS_ROGUE:
					ModifyPlrDex(myplr,2);
					break;
				case CLASS_WARRIOR:	
					ModifyPlrStr(myplr,2);
					break;
				case CLASS_BARBARIAN:	
					ModifyPlrVit(myplr,2);
					break;
				case CLASS_MONK:
					ModifyPlrStr(myplr,1);
					ModifyPlrDex(myplr,1);
					break;
				case CLASS_BARD:
					ModifyPlrDex(myplr,1);
					ModifyPlrMag(myplr,1);	
					break;
			}
			CheckStats(pnum);
			AddMissile(object[i]._ox, object[i]._oy, plr[myplr]._px, plr[myplr]._py, plr[myplr]._pdir, MIT_FIREWALL, MI_ENEMYPLR, 0, 2 + (2 * currlevel), 0);
			break;
		case 27:	// Glowing
			{
			if (deltaload) return;
			if (pnum != myplr) return;
			InitDiabloMsg(SHRINE_28);
			long CurrentExp = plr[myplr]._pExperience;
			long ModMagic;
			
			if (CurrentExp > 5000)
			{
				ModMagic = 5;
				CurrentExp = (long)(CurrentExp * 0.95);
			}
			else
			{
				ModMagic = CurrentExp/1000;
				CurrentExp = 0;
				
			}
			
			ModifyPlrMag(myplr, ModMagic);
			plr[myplr]._pExperience = CurrentExp;
			CheckStats(pnum);
			
			}
			break;
		case 28:	// Mendicant's
			{
			if (deltaload) return;
			if (pnum != myplr) return;
			InitDiabloMsg(SHRINE_29);
			long const HalfCurrentGold = plr[myplr]._pGold / 2;
			
			AddPlrExperience(myplr, plr[myplr]._pLevel, HalfCurrentGold);
			TakePlrsMoney(HalfCurrentGold);	// uses global var myplr.
			CheckStats(pnum);
			}
			break;
		case 29:	// Sparkling
			if (deltaload) return;
			if (pnum != myplr) return;
			InitDiabloMsg(SHRINE_30);
			AddPlrExperience(myplr, plr[myplr]._pLevel, 1000 * currlevel);
			AddMissile(object[i]._ox, object[i]._oy, plr[myplr]._px, plr[myplr]._py, plr[myplr]._pdir, MIT_FLASH, MI_ENEMYPLR, 0, 2 + (3 * currlevel), 0);
			CheckStats(pnum);
			break;
		case 30:	// Town
			if (deltaload) return;
			if (pnum != myplr) return;
			InitDiabloMsg(SHRINE_31);
			AddMissile(object[i]._ox, object[i]._oy, plr[myplr]._px, plr[myplr]._py, plr[myplr]._pdir, MIT_TOWN, MI_ENEMYPLR, 0, 0, 0);
			break;
		case 31:	// Shimmering
			if (deltaload) return;
			if (pnum != myplr) return;
			InitDiabloMsg(SHRINE_32);
			plr[pnum]._pMana = plr[pnum]._pMaxMana;	// restore to full mana.
			plr[pnum]._pManaBase = plr[pnum]._pMaxManaBase;
			break;
		case 32:	// Solar
			{
			if (deltaload) return;
			if (pnum != myplr) return;
			time_t time_of_day = time(0);
			struct tm * const tmbuf = localtime(&time_of_day);
			
			if (tmbuf->tm_hour > 20 || tmbuf->tm_hour < 4)
			{
				InitDiabloMsg(SHRINE_33D);
				ModifyPlrVit(myplr, 2);
			}
			else if (tmbuf->tm_hour > 18)
			{
				InitDiabloMsg(SHRINE_33C);
				ModifyPlrMag(myplr, 2);
			}
			else if (tmbuf->tm_hour > 12)
			{
				InitDiabloMsg(SHRINE_33B);
				ModifyPlrStr(myplr, 2);
			}
			else if (tmbuf->tm_hour > 4)
			{
				InitDiabloMsg(SHRINE_33A);
				ModifyPlrDex(myplr, 2);
			}
			
			CheckStats(pnum);
			}
			break;
		case 33:	// Murphy's
			{
			if (deltaload) return;
			if (pnum != myplr) return;
			InitDiabloMsg(SHRINE_34);
			long i;
			long broken = 0;
			
			for (i = 0; i < 7; ++i)
			{
				ItemStruct * const itm = &plr[myplr].InvBody[i];
				
				if (itm->_itype == -1) continue;
				
				if (random(0,3) == 0 
					&& itm->_iDurability != INFINITE_DUR
					&& itm->_iDurability != 0)	// There are two values for infinite durablity !!!
				{
					// Randomly break something.
					itm->_iDurability /= 2;
					broken = 1;
					break;
				}
			}
			
			if (!broken)
			{
				long const ThirdCurrentGold = plr[myplr]._pGold / 3;
				TakePlrsMoney(ThirdCurrentGold);	// uses global var myplr.
			}
			}
			break;
	}
	CalcPlrInv(pnum, TRUE);
	force_redraw = FULLDRAW;
	if (pnum == myplr) NetSendCmdParam2(FALSE,CMD_PLROPOBJ,pnum,i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateSkelBook(int pnum, int i, bool sendmsg)
{
	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oSelFlag == OSEL_NONE) return;
	if (!deltaload) PlaySfxLoc(IS_ISCROL, object[i]._ox, object[i]._oy);
	object[i]._oSelFlag = OSEL_NONE;
	object[i]._oAnimFrame += 2;
	if (deltaload) return;
	SetRndSeed(object[i]._oRndSeed);
	if (random(161,5)) CreateTypeItem(object[i]._ox, object[i]._oy, FALSE, IT_MISC, IMID_SCROLL, sendmsg, FALSE);
	else CreateTypeItem(object[i]._ox, object[i]._oy, FALSE, IT_MISC, IMID_BOOK, sendmsg, FALSE);
	if (pnum == myplr) NetSendCmdParam1(FALSE,CMD_OPERATEOBJ,i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateBookCase(int pnum, int i, bool sendmsg)
{
	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oSelFlag == OSEL_NONE) return;
	if (!deltaload) PlaySfxLoc(IS_ISCROL, object[i]._ox, object[i]._oy);
	object[i]._oSelFlag = OSEL_NONE;
	object[i]._oAnimFrame -= 2;
	if (deltaload) return;
	SetRndSeed(object[i]._oRndSeed);
	CreateTypeItem(object[i]._ox, object[i]._oy, FALSE, IT_MISC, IMID_BOOK, sendmsg, FALSE);
	if (QuestStatus(Q_ZHAR)) {
		if (monster[4].mName == UniqMonst[MU_ZHAR].mName) {
			if ((monster[4]._msquelch == 255) && (monster[4]._mhitpoints != 0)) {
				monster[4].mtalkmsg = TXT_ZHAR2;
				M_StartStand(0, monster[4]._mdir);
				monster[4]._mgoal = MG_ATTACK2;
				monster[4]._mmode = MM_TALK;
			}
		}
	}
	if (pnum == myplr) NetSendCmdParam1(FALSE,CMD_OPERATEOBJ,i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateDecap(int pnum, int i, bool sendmsg)
{
	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oSelFlag == OSEL_NONE) return;
	object[i]._oSelFlag = OSEL_NONE;
	if (deltaload) return;
	SetRndSeed(object[i]._oRndSeed);
	CreateRndItem(object[i]._ox, object[i]._oy, FALSE, sendmsg, FALSE);
	if (pnum == myplr) NetSendCmdParam1(FALSE,CMD_OPERATEOBJ,i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateArmorStand(int pnum, int i, bool sendmsg)
{
	int	uniqueRnd;

	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oSelFlag == OSEL_NONE) return;
	object[i]._oSelFlag = OSEL_NONE;
	++object[i]._oAnimFrame;
	if (deltaload) return;
	SetRndSeed(object[i]._oRndSeed);

	//Randomize uniqueness
	uniqueRnd = random(0, 2);
	//Create an appropriate armor for the current level
	if (currlevel <= 5) {
		//Create unique only IT_ARMOR
		CreateTypeItem(object[i]._ox, object[i]._oy, TRUE, IT_ARMOR, IMID_NONE, sendmsg, FALSE);
	} else if (currlevel >= 6 && currlevel <= 9) {
		//Create random unique IT_MARMOR
		CreateTypeItem(object[i]._ox, object[i]._oy, uniqueRnd, IT_MARMOR, IMID_NONE, sendmsg, FALSE);
	} else if (currlevel >= 10 && currlevel <= 12) {
		//Create non-unique IT_HARMOR
		CreateTypeItem(object[i]._ox, object[i]._oy, FALSE, IT_HARMOR, IMID_NONE, sendmsg, FALSE);
	} else if (currlevel >= 13 && currlevel <= 16) {
		//Create unique IT_HARMOR
		CreateTypeItem(object[i]._ox, object[i]._oy, TRUE, IT_HARMOR, IMID_NONE, sendmsg, FALSE);
	} else if (currlevel >= 17) {
		//Use the uniques for now, but we will add our own specifics JKE 7/30
		CreateTypeItem(object[i]._ox, object[i]._oy, TRUE, IT_HARMOR, IMID_NONE, sendmsg, FALSE);
	}

	if (pnum == myplr) NetSendCmdParam1(FALSE,CMD_OPERATEOBJ,i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static int FindValidShrine(int i)
/*-----------------------------------------------------------------------*
** Description: Used by OperateGoatShrine and OperateCauldron to find a
**              usable shrine function.
**		 Input:	i = Object index
**      Return:	Shrine function index.
**-----------------------------------------------------------------------*/
{
	int		rv;
	bool	done = false;

	//Search until we find a valid shrine type. Ignore Thaumaturgic shrine 
	//because it could possibly screw up the animation.
	do {
		rv = random(0, NUMSHRINETYPES);
		if ((currlevel >= shrineminlvl[rv]) && 
			(currlevel <= shrinemaxlvl[rv]) &&
			(rv != 8)) {
			done = true;
		}
		if (done) {
			if ((gbMaxPlayers != 1) && (shrineavail[rv] == SHRINE_SINGLE)) {
				done = FALSE;
			} else if ((gbMaxPlayers == 1) && (shrineavail[rv] == SHRINE_MULTI)) {
				done = FALSE;
			} else done = TRUE; //We found a shrine type we can use.
		}
	} while (!done);
	
	return rv;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateGoatShrine(int pnum, int i, int sType)
{
	app_assert((DWORD)i < MAXOBJECTS);
	SetRndSeed(object[i]._oRndSeed);
	object[i]._oVar1 = FindValidShrine(i); 
	OperateShrine(pnum, i, sType); 
	object[i]._oAnimDelay = 2;
	force_redraw = FULLDRAW;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateCauldron(int pnum, int i, int sType)
{
	app_assert((DWORD)i < MAXOBJECTS);
	SetRndSeed(object[i]._oRndSeed);
	object[i]._oVar1 = FindValidShrine(i); 
	OperateShrine(pnum, i, sType); 
	object[i]._oAnimFrame = 3;
	object[i]._oAnimFlag = FALSE;
	force_redraw = FULLDRAW;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static bool OperateFountains(int pnum, int i)
{
	int ii;
	bool rv = false;

	app_assert((DWORD)i < MAXOBJECTS);
	SetRndSeed(object[i]._oRndSeed);

	switch(object[i]._otype) {
		case OBJ_PURIFYINGFTN:
			if (deltaload) return rv;
			if (pnum != myplr) return rv;
			if (plr[pnum]._pMana < plr[pnum]._pMaxMana) {
				PlaySfxLoc(LS_FOUNTAIN, object[i]._ox, object[i]._oy);
				//Add 1 Mana point
				plr[pnum]._pMana += (1 << MANA_SHIFT);
				plr[pnum]._pManaBase += (1 << MANA_SHIFT);
				if (plr[pnum]._pMana > plr[pnum]._pMaxMana) {
					plr[pnum]._pMana = plr[pnum]._pMaxMana;
					plr[pnum]._pManaBase = plr[pnum]._pMaxManaBase;
				}
				rv = true;
			} else if (!deltaload) PlaySfxLoc(LS_FOUNTAIN, object[i]._ox, object[i]._oy);
			break;
		case OBJ_BLOODFTN:
			if (deltaload) return rv;
			if (pnum != myplr) return rv;
			if (plr[pnum]._pHitPoints < plr[pnum]._pMaxHP) {
				PlaySfxLoc(LS_FOUNTAIN, object[i]._ox, object[i]._oy);
				//Add 1 HP point
				plr[pnum]._pHitPoints += (1 << HP_SHIFT);
				plr[pnum]._pHPBase += (1 << HP_SHIFT);
				if (plr[pnum]._pHitPoints > plr[pnum]._pMaxHP) {
					plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
					plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;
				}
				rv = true;
			} else if (!deltaload) PlaySfxLoc(LS_FOUNTAIN, object[i]._ox, object[i]._oy);
			break;
		case OBJ_MURKYFTN:
			if (object[i]._oSelFlag != OSEL_NONE) {
				if (!deltaload) PlaySfxLoc(LS_FOUNTAIN, object[i]._ox, object[i]._oy);
				object[i]._oSelFlag = OSEL_NONE;
				if (deltaload) return rv;
				//Casts infravision
				AddMissile(plr[pnum]._px, plr[pnum]._py, plr[pnum]._px, plr[pnum]._py, 
					       plr[pnum]._pdir, MIT_INFRA, -1, pnum, 0, 
						   (leveltype << 1));
				rv = true;
				if (pnum == myplr) NetSendCmdParam1(FALSE,CMD_OPERATEOBJ,i);
			}
			break;
		case OBJ_TEARFTN:
			if (object[i]._oSelFlag != OSEL_NONE) { 
				int rndVal;
				int statVal = -1;
				int saveRnd = -1;
				int status = FALSE;
				ii = 0;
				if (!deltaload) PlaySfxLoc(LS_FOUNTAIN, object[i]._ox, object[i]._oy);
				object[i]._oSelFlag = OSEL_NONE;
				if (deltaload) return rv;
				if (pnum != myplr) return rv;
				//-1 to random stat, +1 to another
				do {
					if ((rndVal = random(0, 4)) != saveRnd) {
						switch(rndVal) {
							case 0:
								ModifyPlrStr(pnum, statVal);
								break;
							case 1:
								ModifyPlrMag(pnum, statVal);
								break;
							case 2:
								ModifyPlrDex(pnum, statVal);
								break;
							case 3:
								ModifyPlrVit(pnum, statVal);
								break;
						}
						saveRnd = rndVal;
						statVal = 1;
						++ii;
					}
					if (ii > 1) status = TRUE;
				} while(!status);
				CheckStats(pnum);
				rv = true;
				if (pnum == myplr) NetSendCmdParam1(FALSE,CMD_OPERATEOBJ,i);
			} 
			break;
	}
	force_redraw = FULLDRAW;
	return rv;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateWeaponRack(int pnum, int i, bool sendmsg)
{
	int	weaponType, sfxType;

	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oSelFlag == OSEL_NONE) return;
	SetRndSeed(object[i]._oRndSeed);

	//Choose random weapon/sfx type
	switch((random(0, 4)+1)) {
		case 1:
			weaponType = IT_SWORD;
			sfxType = IS_FSWOR;
			break;
		case 2:
			weaponType = IT_AXE;
			sfxType = IS_FAXE;
			break;
		case 3:
			weaponType = IT_BOW;
			sfxType = IS_FBOW;
			break;
		case 4:
			weaponType = IT_MACE;
			sfxType = IS_FAXE;
			break;
	}			

	object[i]._oSelFlag = OSEL_NONE;
	++object[i]._oAnimFrame; 
	if (deltaload) return;
	
	//Create a random weapon. Create unique weapons from level 4 down.
	if (leveltype > 1) CreateTypeItem(object[i]._ox, object[i]._oy, TRUE, weaponType, IMID_NONE, sendmsg, FALSE);
	else CreateTypeItem(object[i]._ox, object[i]._oy, FALSE, weaponType, IMID_NONE, sendmsg, FALSE);

	if (pnum == myplr) NetSendCmdParam1(FALSE,CMD_OPERATEOBJ,i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateStoryBook(int pnum, int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oSelFlag == OSEL_NONE) return;
	if (deltaload) return;
	if (qtextflag) return;
	if (pnum != myplr) return;
	object[i]._oAnimFrame = object[i]._oVar4;
	PlaySfxLoc(IS_ISCROL, object[i]._ox, object[i]._oy);

	if ((object[i]._oVar8 != 0) && (currlevel == NA_KRUL_LEVEL))
	{
		if ((Na_Krul.Lever_Thrown != TRUE) && (quests[Q_NA_KRUL]._qactive != QUEST_DONE))
			if(HCheckSpell(object[i]._oVar8))
			{
				NetSendCmd(FALSE,CMD_OPEN_NAKRUL);
				return;
			}
	}
	else if (currlevel >= CRYPTSTART)
	{
		quests[Q_NA_KRUL]._qactive = QUEST_NOTDONE;
		quests[Q_NA_KRUL]._qlog = TRUE;
		quests[Q_NA_KRUL]._qmsg = object[i]._oVar2;
	}

	InitQTextMsg(object[i]._oVar2);
	NetSendCmdParam1(FALSE,CMD_OPERATEOBJ,i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void OperateLazStand(int pnum, int i)
{
	int x, y;

	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oSelFlag == OSEL_NONE) return;
	if (deltaload) return;
	if (qtextflag) return;
	if (pnum != myplr) return;
	++object[i]._oAnimFrame;
	object[i]._oSelFlag = OSEL_NONE;
	GetSuperItemLoc(object[i]._ox, object[i]._oy, x, y);
	SpawnQuestItem(IDI_LAZSTAFF, x, y, 0, ISEL_NONE);
	//quests[Q_BKMUSHRM]._qvar1 = QS_MUSHSPAWNED;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void OperateObject(int pnum, int i, BOOL TeleFlag)
{
	bool senditemmsg;

	if (pnum == myplr) senditemmsg = true;
	else senditemmsg = false;

	app_assert((DWORD)i < MAXOBJECTS);
	switch (object[i]._otype) {
		case OBJ_L1DOORL :
		case OBJ_L1DOORR :
			if (TeleFlag) {
				if (object[i]._otype == OBJ_L1DOORL) OperateL1LDoor(pnum, i, true);
				if (object[i]._otype == OBJ_L1DOORR) OperateL1RDoor(pnum, i, true);
			} else {
				if (pnum == myplr) OperateL1Door(pnum, i, TRUE);
			}
			break;
		case OBJ_L2DOORL :
		case OBJ_L2DOORR :
			if (TeleFlag) {
				if (object[i]._otype == OBJ_L2DOORL) OperateL2LDoor(pnum, i, true);
				if (object[i]._otype == OBJ_L2DOORR) OperateL2RDoor(pnum, i, true);
			} else {
				if (pnum == myplr) OperateL2Door(pnum, i, TRUE);
			}
			break;
		case OBJ_L3DOORL :
		case OBJ_L3DOORR :
			if (TeleFlag) {
				if (object[i]._otype == OBJ_L3DOORL) OperateL3LDoor(pnum, i, true);
				if (object[i]._otype == OBJ_L3DOORR) OperateL3RDoor(pnum, i, true);
			} else {
				if (pnum == myplr) OperateL3Door(pnum, i, TRUE);
			}
			break;
		case OBJ_SWITCHSKL:
		case OBJ_LEVER :
			OperateLever(pnum, i);
			break;
		case OBJ_BOOK2L:
			OperateBook(pnum, i);
			break;
		case OBJ_BOOK2R:
			OperateSChambBk(pnum, i);
			break;
		case OBJ_CHEST1:
		case OBJ_CHEST2:
		case OBJ_CHEST3:
		case OBJ_TCHEST1:
		case OBJ_TCHEST2:
		case OBJ_TCHEST3:
			OperateChest(pnum, i, senditemmsg);
			break;
		case OBJ_SARC:
			OperateSarc(pnum, i, senditemmsg);
			break;
		case OBJ_FLAMELVR:
			OperateTrapLvr(i);
			break;
//		case OBJ_BOOKLVR:
		case OBJ_BLINDBOOK:
		case OBJ_BLOODBOOK:
		case OBJ_STEELTOME:
			OperateBookLever(pnum, i);
			break;
		case OBJ_SHRINEL:
		case OBJ_SHRINER:
			OperateShrine(pnum, i, IS_MAGIC);
			break;
		case OBJ_SKELBOOK:
		case OBJ_BOOKSTAND:
			OperateSkelBook(pnum, i, senditemmsg);
			break;
		case OBJ_BOOKCASEL:
		case OBJ_BOOKCASER:
			OperateBookCase(pnum, i, senditemmsg);
			break;
		case OBJ_DECAP:
			OperateDecap(pnum, i, senditemmsg);
			break;
		case OBJ_ARMORSTAND:
		case OBJ_WARARMOR:
			OperateArmorStand(pnum, i, senditemmsg);
			break;
		case OBJ_GOATSHRINE:
			OperateGoatShrine(pnum, i, LS_GSHRINE); 
			break;
		case OBJ_CAULDRON:
			OperateCauldron(pnum, i, LS_CALDRON);
			break;
		case OBJ_MURKYFTN:
		case OBJ_TEARFTN:
		case OBJ_BLOODFTN:
		case OBJ_PURIFYINGFTN:
			OperateFountains(pnum, i);
			break;
		case OBJ_STORYBOOK:
			OperateStoryBook(pnum, i);
			break;
		case OBJ_PEDISTAL:
			OperatePedistal(pnum, i);
			break;
		case OBJ_WARWEAP:
		case OBJ_WEAPONRACK:
			OperateWeaponRack(pnum, i, senditemmsg);
			break;
		case OBJ_MUSHPATCH:
			OperateMushPatch(pnum, i);
			break;
		case OBJ_LAZSTAND:
			OperateLazStand(pnum, i);
			break;
		case OBJ_SLAINHERO:
			OperateSlainHero(pnum, i, senditemmsg);
			break;
		case OBJ_SIGNCHEST:
			OperateInnSignChest(pnum, i);
			break;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void SyncOpL1Door(int pnum, int cmd, int i)
{
	if (pnum == myplr) return;
	bool opok = false;
	app_assert((DWORD)i < MAXOBJECTS);
	if ((cmd == CMD_OPENDOOR) && (object[i]._oVar4 == DOOR_CLOSED)) opok = TRUE;
	if ((cmd == CMD_CLOSEDOOR) && (object[i]._oVar4 == DOOR_OPEN)) opok = TRUE;
	if (opok) {
		if (object[i]._otype == OBJ_L1DOORL) OperateL1LDoor(-1, i, false);
		if (object[i]._otype == OBJ_L1DOORR) OperateL1RDoor(-1, i, false);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void SyncOpL2Door(int pnum, int cmd, int i)
{
	if (pnum == myplr) return;
	bool opok = false;
	app_assert((DWORD)i < MAXOBJECTS);
	if ((cmd == CMD_OPENDOOR) && (object[i]._oVar4 == DOOR_CLOSED)) opok = true;
	if ((cmd == CMD_CLOSEDOOR) && (object[i]._oVar4 == DOOR_OPEN)) opok = true;
	if (opok) {
		if (object[i]._otype == OBJ_L2DOORL) OperateL2LDoor(-1, i, false);
		if (object[i]._otype == OBJ_L2DOORR) OperateL2RDoor(-1, i, false);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void SyncOpL3Door(int pnum, int cmd, int i)
{
	if (pnum == myplr) return;
	bool opok = false;
	app_assert((DWORD)i < MAXOBJECTS);
	if ((cmd == CMD_OPENDOOR) && (object[i]._oVar4 == DOOR_CLOSED)) opok = true;
	if ((cmd == CMD_CLOSEDOOR) && (object[i]._oVar4 == DOOR_OPEN)) opok = true;
	if (opok) {
		if (object[i]._otype == OBJ_L3DOORL) OperateL3LDoor(-1, i, false);
		if (object[i]._otype == OBJ_L3DOORR) OperateL3RDoor(-1, i, false);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SyncOpObject(int pnum, int cmd, int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	switch (object[i]._otype) {
		case OBJ_L1DOORL :
		case OBJ_L1DOORR :
			SyncOpL1Door(pnum, cmd, i);
			break;
		case OBJ_L2DOORL :
		case OBJ_L2DOORR :
			SyncOpL2Door(pnum, cmd, i);
			break;
		case OBJ_L3DOORL :
		case OBJ_L3DOORR :
			SyncOpL3Door(pnum, cmd, i);
			break;
		case OBJ_SWITCHSKL:
		case OBJ_LEVER :
			OperateLever(pnum, i);
			break;
		case OBJ_CHEST1:
		case OBJ_CHEST2:
		case OBJ_CHEST3:
		case OBJ_TCHEST1:
		case OBJ_TCHEST2:
		case OBJ_TCHEST3:
			OperateChest(pnum, i, FALSE);
			break;
		case OBJ_SARC:
			OperateSarc(pnum, i, FALSE);
			break;
//		case OBJ_BOOKLVR:
		case OBJ_BLINDBOOK:
		case OBJ_BLOODBOOK:
		case OBJ_STEELTOME:
			OperateBookLever(pnum, i);
			break;
		case OBJ_SHRINEL:
		case OBJ_SHRINER:
			OperateShrine(pnum, i, IS_MAGIC);
			break;
		case OBJ_SKELBOOK:
		case OBJ_BOOKSTAND:
			OperateSkelBook(pnum, i, FALSE);
			break;
		case OBJ_BOOKCASEL:
		case OBJ_BOOKCASER:
			OperateBookCase(pnum, i, FALSE);
			break;
		case OBJ_DECAP:
			OperateDecap(pnum, i, FALSE);
			break;
		case OBJ_ARMORSTAND:
		case OBJ_WARARMOR:
			OperateArmorStand(pnum, i, FALSE);
			break;
		case OBJ_GOATSHRINE:
			OperateGoatShrine(pnum, i, LS_GSHRINE); 
			break;
		case OBJ_CAULDRON:
			OperateCauldron(pnum, i, LS_CALDRON);
			break;
		case OBJ_MURKYFTN:
		case OBJ_TEARFTN:
			OperateFountains(pnum, i);
			break;
		case OBJ_STORYBOOK:
			OperateStoryBook(pnum, i);
			break;
		case OBJ_PEDISTAL:
			OperatePedistal(pnum, i);
			break;
		case OBJ_WARWEAP:
		case OBJ_WEAPONRACK:
			OperateWeaponRack(pnum, i, FALSE);
			break;
		case OBJ_MUSHPATCH:
			OperateMushPatch(pnum, i);
			break;
		case OBJ_SLAINHERO:
			OperateSlainHero(pnum, i, FALSE);
			break;
		case OBJ_SIGNCHEST:
			OperateInnSignChest(pnum, i);
			break;
	}	
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void BreakCrux(int i)
{
	int j, ot, oi;
	bool mapflag;

	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oAnimFlag = TRUE;
	object[i]._oAnimFrame = 1;
	object[i]._oAnimDelay = 1;
	object[i]._oSolidFlag = TRUE;
	object[i]._oMissFlag = TRUE;
	object[i]._oBreak = OBJ_BROKEN;
	object[i]._oSelFlag = OSEL_NONE;

	mapflag = true;
	for (j = 0; j < numobjects; ++j) {
		oi = objectactive[j];
		ot = object[oi]._otype;
		if ((ot == OBJ_CRUX1) || (ot == OBJ_CRUX2) || (ot == OBJ_CRUX3)) {
			if (object[i]._oVar8 == object[oi]._oVar8) {
				if (object[oi]._oBreak != OBJ_BROKEN) mapflag = false;
			}
		}
	}

	if (mapflag) {
		if (!deltaload) PlaySfxLoc(IS_LEVER, object[i]._ox, object[i]._oy);
		ObjChangeMap(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

// DKT971012: Gary has items.cpp checked out; this should go there

void RndUnique(int x, int y, int level)
{
	int uil[MAXUITEMS];
	int i, numui = 0;
	
	for (i=UID_LGTFORGE+1; i < MAXUITEMS && UniqueItemList[i].UIItemId != -1; ++i)
	{
		if (level*2 >= UniqueItemList[i].UIMinLvl)
			uil[numui++] = i;
	}
	
	if (numui > 0)
	{
		int uitem = uil[random(0, numui)];
		SpawnUnique(uitem, x, y);
	}
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void BreakBarrel(int pnum, int i, int dam, BOOL forcebreak, BOOL sendmsg)
{
	int x, y, oi;

	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oSelFlag == OSEL_NONE) return;		// already broken?
	if (forcebreak) {
		object[i]._oVar1 = 0;		// force it to bust open
	} else {
		object[i]._oVar1 -= dam;				// normal damage
		// if someone else is breaking the object, and it wasn't a force break
		// message, then it hasn't broken yet.
		if ((pnum != myplr) && (object[i]._oVar1 <= 0)) object[i]._oVar1 = 1;
	}
	if (object[i]._oVar1 > 0) {
		if (!deltaload) PlaySfxLoc(IS_IBOW, object[i]._ox, object[i]._oy);
		return;
	}

	object[i]._oVar1 = 0;		// broken
	object[i]._oAnimFlag = TRUE;
	object[i]._oAnimFrame = 1;
	object[i]._oAnimDelay = 1;
	object[i]._oSolidFlag = FALSE;
	object[i]._oMissFlag = TRUE;
	object[i]._oBreak = OBJ_BROKEN;
	object[i]._oSelFlag = OSEL_NONE;
	object[i]._oPreFlag = TRUE;
	if (deltaload) {
		object[i]._oAnimFrame = object[i]._oAnimLen;
		object[i]._oAnimCnt = 0;
		object[i]._oAnimDelay = 1000;
		return;
	}
	if (object[i]._otype == OBJ_BARRELEX) {
		bool foo;
		
		if (currlevel >= CRYPTSTART && currlevel <= CRYPTEND)
			PlaySfxLoc(IS_URNFIRE, object[i]._ox, object[i]._oy);
		else if (currlevel >= HIVESTART && currlevel <= HIVEEND)
			PlaySfxLoc(IS_PODFIRE, object[i]._ox, object[i]._oy);
		else	
			PlaySfxLoc(IS_BARLFIRE, object[i]._ox, object[i]._oy);
		for (y = object[i]._oy-1; y <= object[i]._oy+1; ++y) {
			for (x = object[i]._ox-1; x <= object[i]._ox+1; ++x) {
				if (dMonster[x][y] > 0) MonsterTrapHit(dMonster[x][y]-1, 1, 4, 0, MIMT_FIRE, 0);
				if (dPlayer[x][y] > 0) PlayerMHit(dPlayer[x][y]-1, -1, 0, 8, 16, MIMT_FIRE, 0, FALSE, &foo);
				if (dObject[x][y] > 0) {
					oi = dObject[x][y] - 1;
					if ((object[oi]._otype == OBJ_BARRELEX) && (object[oi]._oBreak != OBJ_BROKEN))
						BreakBarrel(pnum, oi, dam, TRUE, sendmsg);
				}
			}
		}
	} else {
		if (currlevel >= CRYPTSTART && currlevel <= CRYPTEND)
			PlaySfxLoc(IS_URN, object[i]._ox, object[i]._oy);
		else if (currlevel >= HIVESTART && currlevel <= HIVEEND)
			PlaySfxLoc(IS_POD, object[i]._ox, object[i]._oy);
		else	
			PlaySfxLoc(IS_BARREL, object[i]._ox, object[i]._oy);
		SetRndSeed(object[i]._oRndSeed);
#if CHEATS	
		if (uniqcheat)
			RndUnique(object[i]._ox, object[i]._oy, currlevel);
		else
#endif	
		if (object[i]._oVar2 <= 1) {
			if (!object[i]._oVar3) CreateRndUseful(pnum, object[i]._ox, object[i]._oy, sendmsg);
			else CreateRndItem(object[i]._ox, object[i]._oy, FALSE, sendmsg, FALSE);
		}
		if (object[i]._oVar2 >= 8) SpawnSkeleton(object[i]._oVar4, object[i]._ox, object[i]._oy);
	}
	

	if (pnum == myplr) NetSendCmdParam2(FALSE,CMD_BREAKOBJ,pnum,i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void BreakObject(int pnum, int oi)
{
	int objdam, mind, maxd;

	if (pnum != -1) {
		mind = plr[pnum]._pIMinDam;
		maxd = plr[pnum]._pIMaxDam;
		objdam = random(163,maxd - mind + 1) + mind;
		objdam += (objdam * plr[pnum]._pIBonusDam) / 100;
		objdam += plr[pnum]._pIBonusDamMod + plr[pnum]._pDamageMod;
	} else objdam = 10;

	app_assert((DWORD)oi < MAXOBJECTS);
	switch (object[oi]._otype) {
		case OBJ_CRUX1 :
		case OBJ_CRUX2 :
		case OBJ_CRUX3 :
			BreakCrux(oi);
			break;
		case OBJ_BARREL:
		case OBJ_BARRELEX:
			BreakBarrel(pnum, oi, objdam, FALSE, TRUE);
			break;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SyncBreakObj(int pnum, int oi)
{
	app_assert((DWORD)oi < MAXOBJECTS);
	switch (object[oi]._otype) {
		case OBJ_BARREL:
		case OBJ_BARRELEX:
			BreakBarrel(pnum, oi, 0, TRUE, FALSE);
			break;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void SyncL1Doors(int i)
{
	int		dx, dy;

	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oVar4 == DOOR_CLOSED) {
		object[i]._oMissFlag = FALSE;
		return;
	} else object[i]._oMissFlag = TRUE;
	dx = object[i]._ox;
	dy = object[i]._oy;
	object[i]._oSelFlag = OSEL_TOP;
	if (currlevel < HIVESTART)
	{
		if (object[i]._otype == OBJ_L1DOORL) {
			if (object[i]._oVar1 == 214) ObjSetMicro(dx,dy,408);	//Blood
			else ObjSetMicro(dx,dy,393);
			dSpecial[dx][dy] = 7;
			ObjSetMicro12(dx-1,dy);
			--dy;
		} else {
			ObjSetMicro(dx,dy,395);
			if (currlevel < HIVESTART)
				dSpecial[dx][dy] = 8;
			ObjSetMicro12(dx,dy-1);
			--dx;
		}
	}
	else
	{
		if (object[i]._otype == OBJ_L1DOORL) {
			ObjSetMicro(dx,dy,206);
			dSpecial[dx][dy] = 1;
			ObjSetMicro12(dx-1,dy);
			--dy;
		} else {
			ObjSetMicro(dx,dy,209);
			dSpecial[dx][dy] = 2;
			ObjSetMicro12(dx,dy-1);
			--dx;
		}
	}

	DoorSet(i, dx, dy);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void SyncCrux(int i)
{
	int j, ot, oi;
	bool mapflag;

	mapflag = TRUE;
	for (j = 0; j < numobjects; ++j) {
		oi = objectactive[j];
		app_assert((DWORD)oi < MAXOBJECTS);
		ot = object[oi]._otype;
		if ((ot == OBJ_CRUX1) || (ot == OBJ_CRUX2) || (ot == OBJ_CRUX3)) {
			if (object[i]._oVar8 == object[oi]._oVar8) {
				if (object[oi]._oBreak != OBJ_BROKEN) mapflag = FALSE;
			}
		}
	}

	if (mapflag) ObjChangeMap(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void SyncLever(int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oSelFlag == OSEL_NONE) {
		ObjChangeMap(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void SyncQSTLever(int i)
{
	int tren;

	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oAnimFrame == object[i]._oVar6) {
		ObjChangeMapResync(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
		if (object[i]._otype == OBJ_BLINDBOOK) {
			tren = TransVal;
			TransVal = 9;
			DRLG_MRectTrans(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
			TransVal = tren;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void SyncPedistal(int i)
{
	byte *setp;

	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oVar6 == 1) {
		ObjChangeMapResync(setpc_x, setpc_y + 3, setpc_x + 2, setpc_y + 7);
	}
	if (object[i]._oVar6 == 2) {
		ObjChangeMapResync(setpc_x, setpc_y + 3, setpc_x + 2, setpc_y + 7);
		ObjChangeMapResync(setpc_x + 6, setpc_y + 3, setpc_x + setpc_w, setpc_y + 7);
	}
	if (object[i]._oVar6 == 3) {
		ObjChangeMapResync(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
 		setp = LoadFileInMemSig("Levels\\L2Data\\Blood2.DUN", NULL, 'STPC');
		LoadMapObjs(setp, (setpc_x << 1), (setpc_y << 1));
		DiabloFreePtr(setp);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void SyncL2Doors(int i)
{
	int	dx, dy;

	app_assert((DWORD)i < MAXOBJECTS);
	if (object[i]._oVar4 == DOOR_CLOSED) object[i]._oMissFlag = FALSE;
	else object[i]._oMissFlag = TRUE;
	dx = object[i]._ox;
	dy = object[i]._oy;
	object[i]._oSelFlag = OSEL_TOP;
	
	//Fix doors upon returning to level or loading level
	if ((object[i]._otype == OBJ_L2DOORL) && 
		(object[i]._oVar4 == DOOR_CLOSED)) {
		ObjSetMicro(dx,dy,538);
	} else if ((object[i]._otype == OBJ_L2DOORL) && 
		       ((object[i]._oVar4 == DOOR_OPEN) || 
			    (object[i]._oVar4 == DOOR_BLOCKED))) {
		ObjSetMicro(dx,dy,13); 
	} else if ((object[i]._otype == OBJ_L2DOORR) && 
		       (object[i]._oVar4 == DOOR_CLOSED)) {
		ObjSetMicro(dx,dy,540);
	} else if ((object[i]._otype == OBJ_L2DOORR) && 
		       ((object[i]._oVar4 == DOOR_OPEN) ||
			    (object[i]._oVar4 == DOOR_BLOCKED))) {
		ObjSetMicro(dx,dy,17); 
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

static void SyncL3Doors(int i)
{
	int		dx, dy;

	app_assert((DWORD)i < MAXOBJECTS);
	object[i]._oMissFlag = TRUE;
	dx = object[i]._ox;
	dy = object[i]._oy;
	object[i]._oSelFlag = OSEL_TOP;

	//Fix doors upon returning to level or loading level
	if ((object[i]._otype == OBJ_L3DOORL) && 
		(object[i]._oVar4 == DOOR_CLOSED)) {
		ObjSetMicro(dx,dy,531);
	} else if ((object[i]._otype == OBJ_L3DOORL) && 
		       ((object[i]._oVar4 == DOOR_OPEN) || 
			    (object[i]._oVar4 == DOOR_BLOCKED))) {
		ObjSetMicro(dx,dy,538);
	} else if ((object[i]._otype == OBJ_L3DOORR) && 
		       (object[i]._oVar4 == DOOR_CLOSED)) {
		ObjSetMicro(dx,dy,534);
	} else if ((object[i]._otype == OBJ_L3DOORR) && 
		       ((object[i]._oVar4 == DOOR_OPEN) ||
			    (object[i]._oVar4 == DOOR_BLOCKED))) {
		ObjSetMicro(dx,dy,541);
	} 
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SyncObjectAnim(int o)
{
	int ai, ot, j;

	app_assert((DWORD)o < MAXOBJECTS);
	ot = object[o]._otype;
	ai = AllObjects[ot].ofindex;
	for (j = 0; ObjFileList[j] != ai; ++j);
	object[o]._oAnimData = pObjCels[j];

	switch (object[o]._otype) {
		case OBJ_L1DOORL :
		case OBJ_L1DOORR :
			SyncL1Doors(o);
			break;
		case OBJ_L2DOORL :
		case OBJ_L2DOORR :
			SyncL2Doors(o);
			break;
		case OBJ_L3DOORL :
		case OBJ_L3DOORR :
			SyncL3Doors(o);
			break;
		case OBJ_CRUX1 :
		case OBJ_CRUX2 :
		case OBJ_CRUX3 :
			SyncCrux(o);
			break;
		case OBJ_SWITCHSKL:
		case OBJ_LEVER :
		case OBJ_BOOK2L:
			SyncLever(o);
			break;
		case OBJ_BLINDBOOK:
		case OBJ_STEELTOME:		
		case OBJ_BOOK2R:
			SyncQSTLever(o);
			break;
		case OBJ_PEDISTAL:
			SyncPedistal(o);
			break;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void GetObjectStr(int i)
{
	app_assert((DWORD)i < MAXOBJECTS);
	switch (object[i]._otype) {
		case OBJ_CRUX1 :
		case OBJ_CRUX2 :
		case OBJ_CRUX3 :
			strcpy(infostr, "Crucified Skeleton");
			break;
		case OBJ_LEVER :
		case OBJ_FLAMELVR:
			strcpy(infostr, "Lever");
			break;
		case OBJ_L1DOORL :
		case OBJ_L1DOORR :
		case OBJ_L2DOORL :
		case OBJ_L2DOORR :
		case OBJ_L3DOORL :
		case OBJ_L3DOORR :
			if (object[i]._oVar4 == DOOR_OPEN) strcpy(infostr, "Open Door");
			if (object[i]._oVar4 == DOOR_CLOSED) strcpy(infostr, "Closed Door");
			if (object[i]._oVar4 == DOOR_BLOCKED) strcpy(infostr, "Blocked Door");
			break;
		case OBJ_BOOK2L:
			if (setlevel) {
				if (setlvlnum == SL_BONECHAMB)
					strcpy(infostr, "Ancient Tome");
				else if (setlvlnum == SL_VILEBETRAYER)
					strcpy(infostr, "Book of Vileness");
			}
			break;
		case OBJ_SWITCHSKL:
			strcpy(infostr, "Skull Lever");
			break;
//		case OBJ_BOOKLVR:
//			strcpy(infostr, "Tome");
//			break;
		case OBJ_BOOK2R:
			strcpy(infostr, "Mythical Book");
			break;
		case OBJ_CHEST1:
		case OBJ_TCHEST1:
			strcpy(infostr, "Small Chest");
			break;
		case OBJ_CHEST2:
		case OBJ_TCHEST2:
			strcpy(infostr, "Chest");
			break;
		case OBJ_CHEST3:
		case OBJ_TCHEST3:
		case OBJ_SIGNCHEST:
			strcpy(infostr, "Large Chest");
			break;
		case OBJ_SARC:
			strcpy(infostr, "Sarcophagus");
			break;
		case OBJ_BOOKSHELF:
			strcpy(infostr, "Bookshelf");
			break;
		case OBJ_BOOKCASEL:
		case OBJ_BOOKCASER:
			strcpy(infostr, "Bookcase");
			break;
		case OBJ_BARREL:
		case OBJ_BARRELEX:
			if (currlevel >= HIVESTART && currlevel <= HIVEEND)
				strcpy(infostr, "Pod");
			else if (currlevel >= CRYPTSTART && currlevel <= CRYPTEND)
				strcpy(infostr, "Urn");
			else
				strcpy(infostr, "Barrel");
			break;
		case OBJ_SHRINEL:
		case OBJ_SHRINER:
			sprintf(tempstr, "%s Shrine", shrinestrs[object[i]._oVar1]);
			strcpy(infostr, tempstr);
			break;
		case OBJ_SKELBOOK:
			strcpy(infostr, "Skeleton Tome");
			break;
		case OBJ_BOOKSTAND:
			strcpy(infostr, "Library Book");
			break;
		case OBJ_BLOODFTN:
			strcpy(infostr, "Blood Fountain");
			break;
		case OBJ_DECAP:
			strcpy(infostr, "Decapitated Body");
			break;
		case OBJ_BLINDBOOK:
			strcpy(infostr, "Book of the Blind");
			break;
		case OBJ_STEELTOME:
			strcpy(infostr, "Steel Tome");
			break;
		case OBJ_BLOODBOOK:
			strcpy(infostr, "Book of Blood");
			break;
		case OBJ_PURIFYINGFTN:
			strcpy(infostr, "Purifying Spring");
			break;
		case OBJ_ARMORSTAND:
		case OBJ_WARARMOR:
			strcpy(infostr, "Armor");
			break;
		case OBJ_WARWEAP:
			strcpy(infostr, "Weapon Rack");
			break;
		case OBJ_GOATSHRINE:
			strcpy(infostr, "Goat Shrine");
			break;
		case OBJ_CAULDRON:
			strcpy(infostr, "Cauldron");
			break;
		case OBJ_MURKYFTN:
			strcpy(infostr, "Murky Pool");
			break;
		case OBJ_TEARFTN:
			strcpy(infostr, "Fountain of Tears");
			break;
		case OBJ_PEDISTAL:
			strcpy(infostr, "Pedestal of Blood");
			break;
		case OBJ_STORYBOOK:
			strcpy(infostr, StoryBookName[object[i]._oVar3]);
			break;
		case OBJ_WEAPONRACK:
			strcpy(infostr, "Weapon Rack");
			break;
		case OBJ_MUSHPATCH:
			strcpy(infostr, "Mushroom Patch");
			break;
		case OBJ_LAZSTAND:
			strcpy(infostr, "Vile Stand");
			break;
		case OBJ_SLAINHERO:
			strcpy(infostr, "Slain Hero");
			break;
	}
	if ((plr[myplr]._pClass == CLASS_ROGUE) && (object[i]._oTrapFlag)) {
		sprintf(tempstr, "Trapped %s", infostr);
		strcpy(infostr, tempstr);
		infoclr = ICOLOR_RED;
	}
}

static void SpellBook (int number, int locx, int locy)
{
	AddSkulkenObject(OBJ_STORYBOOK, number, locx, locy);

	
}

static void AddNa_Krul_Stuff()
{
	int spell;

	AddNaKrulLeverObj();

	spell = random(0, 6);
	switch (spell)
	{
		case 0:
			SpellBook(6, Na_Krul.x + 3, Na_Krul.y);
			SpellBook(7, Na_Krul.x + 2, Na_Krul.y - 3);
			SpellBook(8, Na_Krul.x + 2, Na_Krul.y + 2);
			break;
		case 1:
			SpellBook(6, Na_Krul.x + 3, Na_Krul.y);
			SpellBook(8, Na_Krul.x + 2, Na_Krul.y - 3);
			SpellBook(7, Na_Krul.x + 2, Na_Krul.y + 2);
			break;
		case 2:
			SpellBook(7, Na_Krul.x + 3, Na_Krul.y);
			SpellBook(6, Na_Krul.x + 2, Na_Krul.y - 3);
			SpellBook(8, Na_Krul.x + 2, Na_Krul.y + 2);
			break;
		case 3:
			SpellBook(7, Na_Krul.x + 3, Na_Krul.y);
			SpellBook(8, Na_Krul.x + 2, Na_Krul.y - 3);
			SpellBook(6, Na_Krul.x + 2, Na_Krul.y + 2);
			break;
		case 4:
			SpellBook(8, Na_Krul.x + 3, Na_Krul.y);
			SpellBook(7, Na_Krul.x + 2, Na_Krul.y - 3);
			SpellBook(6, Na_Krul.x + 2, Na_Krul.y + 2);
			break;
		case 5:
			SpellBook(8, Na_Krul.x + 3, Na_Krul.y);
			SpellBook(6, Na_Krul.x + 2, Na_Krul.y - 3);
			SpellBook(7, Na_Krul.x + 2, Na_Krul.y + 2);
			break;
	}


}

static bool HCheckSpell(int number)
{

	switch (number)
	{
		case 6:
			SpellProgress = 1;
			break;
		case 7:
			if (SpellProgress == 1)
				SpellProgress = 2;
			else
				SpellProgress = 0;
			break;
		case 8:
			if (SpellProgress == 2)
			{
//				qtextflag = FALSE;
//				stream_stop();
				return true;
			}
			else
				SpellProgress = 0;
	}
	return false;
}


void OpenNaKrul()
{
	if (currlevel == NA_KRUL_LEVEL)
	{
		PlaySfxLoc(CR_DOOROPEN, Na_Krul.x, Na_Krul.y);
		dPiece[Na_Krul.x][Na_Krul.y] = 298;
		dPiece[Na_Krul.x][Na_Krul.y - 1] = 301;
		dPiece[Na_Krul.x][Na_Krul.y - 2] = 300;
		dPiece[Na_Krul.x][Na_Krul.y + 1] = 299;

		SetDungeonMicros();
	}
}

void OpenNaKrul2()
{
	dPiece[Na_Krul.x][Na_Krul.y] = 298;
	dPiece[Na_Krul.x][Na_Krul.y - 1] = 301;
	dPiece[Na_Krul.x][Na_Krul.y - 2] = 300;
	dPiece[Na_Krul.x][Na_Krul.y + 1] = 299;

	SetDungeonMicros();
}

static void AddNaKrulLeverObj()
{
	int xp, yp;
	while (1) {
		xp = random(141,80) + DIRTEDGED2;
		yp = random(141,80) + DIRTEDGED2;
		if (! RndLocOk(xp-1, yp-1)) continue;
		if (! RndLocOk(xp+0, yp-1)) continue;
		if (! RndLocOk(xp+1, yp-1)) continue;
		if (! RndLocOk(xp-1, yp+0)) continue;
		if (! RndLocOk(xp+0, yp+0)) continue;
		if (! RndLocOk(xp+1, yp+0)) continue;
		if (! RndLocOk(xp-1, yp+1)) continue;
		if (! RndLocOk(xp+0, yp+1)) continue;
		if (! RndLocOk(xp+1, yp+1)) continue;
		break;
	}

	// temp test
	xp = Na_Krul.x + 3;
	yp = Na_Krul.y - 1;

	Na_Krul.LeverX = xp;
	Na_Krul.LeverY = yp;
	AddObject(OBJ_LEVER, xp, yp);
}


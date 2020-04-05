/*-----------------------------------------------------------------------**
** 								Diablo
**
**							 Set Maps file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/SETMAPS.CPP 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"setmaps.h"
#include	"gendung.h"
#include	"palette.h"
#include	"engine.h"

#include	"objects.h"
#include	"quests.h"

#include	"drlg_l1.h"
#include	"drlg_l2.h"
#include	"drlg_l3.h"
#include	"drlg_l4.h"

#include	"trigs.h"

/*-----------------------------------------------------------------------*
** Global Variables
**-----------------------------------------------------------------------*/

static byte SkelKingTrans1[] = {
		19, 47, 26, 55,
		26, 49, 30, 53 };
		//18, 49, 18, 49 };

static byte SkelKingTrans2[] = {
		33, 19, 47, 29,
		37, 29, 43, 39 };

static byte SkelKingTrans3[] = {
		27, 53, 35, 61,
		27, 35, 34, 42,
		45, 35, 53, 43,
		45, 53, 53, 61,
		31, 39, 49, 57 };

static byte SkelKingTrans4[] = {
		49, 45, 58, 51,
		57, 31, 62, 37,
		63, 31, 69, 40,
		59, 41, 73, 55,
		63, 55, 69, 65,
		73, 45, 78, 51,
		79, 43, 89, 53 };

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static byte SkelChamTrans1[] = {
		43, 19, 50, 26,
		51, 19, 59, 26,
		35, 27, 42, 34,
		43, 27, 49, 34,
		50, 27, 59, 34};

static byte SkelChamTrans2[] = {
		19, 31, 34, 47,
		34, 35, 42, 42};

static byte SkelChamTrans3[] = {
		43, 35, 50, 42,
		51, 35, 62, 42,
		63, 31, 66, 46,
		67, 31, 78, 34,
		67, 35, 78, 42,
		67, 43, 78, 46,
		35, 43, 42, 51,
		43, 43, 49, 51,
		50, 43, 59, 51};

char *SetLevelName[] = {
	"",
	"Skeleton King's Lair",
	"Bone Chamber",
	"Maze",
	"Poisoned Water Supply",
	"Archbishop Lazarus' Lair",
};

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int ObjIndex(int x, int y)
{
	int i, oi;

	for (i = 0; i < numobjects; i++) {
		oi = objectactive[i];
		if ((object[oi]._ox == x) && (object[oi]._oy == y))
			return(oi);
	}
	app_fatal("ObjIndex: Active object not found at (%d,%d)",x,y);
	return -1;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void AddSKingObjs()
{
	// Levers
	SetObjMapRange(ObjIndex(64,34), 20,  7, 23, 10, 1);
	SetObjMapRange(ObjIndex(64,59), 20, 14, 21, 16, 2);

	// Crux
	SetObjMapRange(ObjIndex(27,37), 8, 1, 15, 11, 3);
	SetObjMapRange(ObjIndex(46,35), 8, 1, 15, 11, 3);
	SetObjMapRange(ObjIndex(49,53), 8, 1, 15, 11, 3);
	SetObjMapRange(ObjIndex(27,53), 8, 1, 15, 11, 3);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void AddSChamObjs()
{
	// Levers
	SetObjMapRange(ObjIndex(37,30), 17, 0, 21, 5, 1);
	SetObjMapRange(ObjIndex(37,46), 13, 0, 16, 5, 2);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void AddVileObjs()
{
	// Books
	SetObjMapRange(ObjIndex(26,45),  1, 1, 9, 10, 1);
	SetObjMapRange(ObjIndex(45,46), 11, 1, 20, 10, 2);

	//Magic Circles
	SetObjMapRange(ObjIndex(35,36), 7, 11, 13, 18, 3);
 }

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void DRLG_SetMapTrans(char sFileName[])
{
	int		i,j,rw,rh;
	byte	*pLevelMap,*lm;
	long	mapoff;

	//*** This data file seems to be stored as word values, but
	//*** it is being accessed and stored in dTransVal as bytes.
	//*** Is this correct?  If so, the data is double sized.   -Collin

	// Load map
	pLevelMap = LoadFileInMemSig(sFileName,NULL,'LMPt');
	lm = pLevelMap;

	rw = *lm;
	lm += 2;
	rh = *lm;
	// Skip map + height word
	mapoff = ((rw * rh) << 1) + 2;
	// Convert to index mini tile level instead of mega
	rw = rw << 1;
	rh = rh << 1;
	// Skip treasure, monster, trap maps
	mapoff += (((rw * rh) << 1) * 3);
	lm += mapoff;
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			dTransVal[i+DIRTEDGED2][j+DIRTEDGED2] = *lm;
			lm += 2;
		}
	}

	// Free map
	DiabloFreePtr(pLevelMap);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
void LoadSetMap()
{
	switch (setlvlnum) {
		case SL_SKELKING:
			if (quests[Q_SKELKING]._qactive == QUEST_NOTACTIVE) {
				quests[Q_SKELKING]._qactive = QUEST_NOTDONE;
				quests[Q_SKELKING]._qvar1 = 1;
			}
			LoadPreL1Dungeon("Levels\\L1Data\\SklKng1.DUN", 83, 45);
			LoadL1Dungeon("Levels\\L1Data\\SklKng2.DUN", 83, 45);
			LoadPalette ("Levels\\L1Data\\L1_2.pal");
			DRLG_AreaTrans(2, &SkelKingTrans1[0]);
			DRLG_ListTrans(2, &SkelKingTrans2[0]);
			DRLG_AreaTrans(5, &SkelKingTrans3[0]);
			DRLG_ListTrans(7, &SkelKingTrans4[0]);
			AddL1Objs(0, 0, DMAXX, DMAXY);
			AddSKingObjs();
			InitSKingTriggers();
			break;

		case SL_BONECHAMB:
			LoadPreL2Dungeon("Levels\\L2Data\\Bonecha2.DUN", 69, 39);
			LoadL2Dungeon("Levels\\L2Data\\Bonecha1.DUN", 69, 39);
			LoadPalette ("Levels\\L2Data\\L2_2.pal");
			DRLG_ListTrans(5, &SkelChamTrans1[0]);
			DRLG_AreaTrans(2, &SkelChamTrans2[0]);
			DRLG_ListTrans(9, &SkelChamTrans3[0]);
			AddL2Objs(0, 0, DMAXX, DMAXY);
			AddSChamObjs();
			InitSChambTriggers();
			break;

		case SL_MAZE:
			LoadPreL1Dungeon("Levels\\L1Data\\Lv1MazeA.DUN", 20, 50);
			LoadL1Dungeon("Levels\\L1Data\\Lv1MazeB.DUN", 20, 50);
			LoadPalette ("Levels\\L1Data\\L1_5.pal");
			AddL1Objs(0, 0, DMAXX, DMAXY);
			DRLG_SetMapTrans("Levels\\L1Data\\Lv1MazeA.DUN");
			break;

		case SL_POISONWATER:
			if (quests[Q_PWATER]._qactive == QUEST_NOTACTIVE) 
				quests[Q_PWATER]._qactive = QUEST_NOTDONE;
			LoadPreL3Dungeon("Levels\\L3Data\\Foulwatr.DUN", 19, 50);
			LoadL3Dungeon("Levels\\L3Data\\Foulwatr.DUN", 20, 50);
			LoadPalette ("Levels\\L3Data\\L3pfoul.pal");
			InitPWaterTriggers();
			break;

		case SL_VILEBETRAYER:
			if (quests[Q_BETRAYER]._qactive == QUEST_DONE)
				quests[Q_BETRAYER]._qvar2 = QS_VBRP4;
			else if (quests[Q_BETRAYER]._qactive == QUEST_NOTDONE)
				quests[Q_BETRAYER]._qvar2 = QS_VBRP3;
			LoadPreL1Dungeon("Levels\\L1Data\\Vile1.DUN", 35, 36);
			LoadL1Dungeon("Levels\\L1Data\\Vile2.DUN", 35, 36);
			LoadPalette ("Levels\\L1Data\\L1_2.pal");
			AddL1Objs(0, 0, DMAXX, DMAXY);
			AddVileObjs();
			DRLG_SetMapTrans("Levels\\L1Data\\Vile1.DUN");
			InitNoTriggers();
			break;
	}
}
#endif
